
#include <Windows.h>
#include <queue>
#include "locator.h"
#include "spatial_input.h"
#include "ring_buffer.h"
#include "timestamps.h"
#include "lock.h"
#include "log.h"

#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.Perception.h>
#include <winrt/Windows.Perception.Spatial.h>
#include <winrt/Windows.Perception.People.h>

using namespace winrt::Windows::Foundation::Numerics;
using namespace winrt::Windows::Perception;
using namespace winrt::Windows::Perception::Spatial;
using namespace winrt::Windows::Perception::People;

class si_frame
{
private:
    ULONG m_count;

public:
    int32_t valid;
    SpatialInput_Frame head_pose;
    SpatialInput_Ray eye_ray;
    winrt::Windows::Perception::People::JointPose left_hand[HAND_JOINTS];
    winrt::Windows::Perception::People::JointPose right_hand[HAND_JOINTS];

    si_frame();

    ULONG AddRef();
    ULONG Release();
};

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

static SRWLOCK g_lock;
static ring_buffer<si_frame*> g_buffer;
static HANDLE g_event_enable = NULL; // CloseHandle
static HANDLE g_event_quit = NULL; // CloseHandle
static HANDLE g_thread = NULL; // CloseHandle

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// OK
si_frame::si_frame() : m_count(1)
{
}

// OK
ULONG si_frame::AddRef()
{
    return InterlockedIncrement(&m_count);
}

// OK
ULONG si_frame::Release()
{
    ULONG uCount = InterlockedDecrement(&m_count);
    if (uCount != 0) { return uCount; }
    delete this;
    return uCount;
}

// OK
static void SI_Insert(si_frame* frame, uint64_t timestamp)
{
    si_frame* f;
    SRWLock srw(&g_lock, true);
    f = g_buffer.insert(frame, timestamp);
    if (f) { f->Release(); }
}

// OK
static void SI_Clear()
{
    SRWLock srw(&g_lock, true);
    for (int32_t i = 0; i < g_buffer.size(); ++i) { g_buffer.at(i)->Release(); }
    g_buffer.reset();
}

// OK
static void SI_Acquire()
{
    PerceptionTimestamp ts = nullptr;
    SpatialCoordinateSystem world = nullptr;
    si_frame* frame;
    UINT64 qpc;
    int status1;
    int status2;

    WaitForSingleObject(g_event_enable, INFINITE);

    do
    {
    Sleep(1000 / 30);

    qpc = GetCurrentQPCTimestamp();
    ts = QPCTimestampToPerceptionTimestamp(qpc);
    world = Locator_GetWorldCoordinateSystem(ts);

    frame = new si_frame();

    status1 = SpatialInput_GetHeadPoseAndEyeRay(world, ts, frame->head_pose, frame->eye_ray);
    status2 = SpatialInput_GetHandPose(world, ts, frame->left_hand, frame->right_hand);

    frame->valid = (status1 | (status2 << 2)) & 0x0F;

    SI_Insert(frame, qpc);
    }
    while (WaitForSingleObject(g_event_enable, 0) == WAIT_OBJECT_0);
    
    SI_Clear();
}

// OK
static DWORD WINAPI SI_EntryPoint(void *param)
{
    (void)param;

    ShowMessage("SI: Waiting for consent");

    SpatialInput_WaitForEyeConsent();

    do { SI_Acquire(); } while (WaitForSingleObject(g_event_quit, 0) == WAIT_TIMEOUT);

    return 0;
}

// OK
void SI_SetEnable(bool enable)
{
    if (enable) { SetEvent(g_event_enable); } else { ResetEvent(g_event_enable); }
}

// OK
int SI_Get(int32_t stamp, void*& f, uint64_t& t, int32_t& s)
{
    SRWLock srw(&g_lock, false);
    int v = g_buffer.get(stamp, (si_frame*&)f, t, s);
    if (v == 0) { ((si_frame*&)f)->AddRef(); }
    return v;
}

// OK
int SI_Get(uint64_t timestamp, int time_preference, bool tiebreak_right, void*& f, uint64_t& t, int32_t& s)
{
    SRWLock srw(&g_lock, false);
    int v = g_buffer.get(timestamp, time_preference, tiebreak_right, (si_frame*&)f, t, s);
    if (v == 0) { ((si_frame*&)f)->AddRef(); }
    return v;
}

// OK
void SI_Release(void* frame)
{
    ((si_frame*)frame)->Release();
}

// OK
void SI_Extract(void* frame, int32_t* valid, void const** head_buffer, int32_t* head_length, void const** eye_buffer, int32_t* eye_length, void const** left_buffer, int32_t* left_length, void const** right_buffer, int32_t* right_length)
{
    si_frame* f = (si_frame*)frame;
    *valid = f->valid;
    *head_buffer = &f->head_pose;
    *head_length = sizeof(si_frame::head_pose) / sizeof(float);
    *eye_buffer = &f->eye_ray;
    *eye_length = sizeof(si_frame::eye_ray) / sizeof(float);
    *left_buffer = f->left_hand;
    *left_length = HAND_JOINTS;
    *right_buffer = f->right_hand;
    *right_length = HAND_JOINTS;
}

// OK
void SI_Initialize(int buffer_size)
{
    InitializeSRWLock(&g_lock);
    g_buffer.reset(buffer_size);
    g_event_enable = CreateEvent(NULL, TRUE, FALSE, NULL);
    g_event_quit = CreateEvent(NULL, TRUE, FALSE, NULL);
    g_thread = CreateThread(NULL, 0, SI_EntryPoint, NULL, 0, NULL);
}

// OK
void SI_Quit()
{
    SetEvent(g_event_quit);
}

// OK
void SI_Cleanup()
{
    WaitForSingleObject(g_thread, INFINITE);
    CloseHandle(g_thread);
    CloseHandle(g_event_quit);
    CloseHandle(g_event_enable);
}

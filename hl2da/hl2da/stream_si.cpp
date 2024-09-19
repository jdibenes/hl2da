
#include "spatial_input.h"
#include "locator.h"
#include "extended_execution.h"
#include "frame_buffer.h"
#include "timestamps.h"
#include "log.h"

#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.Perception.h>
#include <winrt/Windows.Perception.Spatial.h>
#include <winrt/Windows.Perception.People.h>

using namespace winrt::Windows::Foundation::Numerics;
using namespace winrt::Windows::Perception;
using namespace winrt::Windows::Perception::Spatial;
using namespace winrt::Windows::Perception::People;

class si_frame : public sensor_frame
{
public:
    int32_t valid;
    SpatialInput_Frame head_pose;
    SpatialInput_Ray eye_ray;
    winrt::Windows::Perception::People::JointPose left_hand[HAND_JOINTS];
    winrt::Windows::Perception::People::JointPose right_hand[HAND_JOINTS];

    si_frame();
    ~si_frame();
};

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

static frame_buffer g_buffer;
static HANDLE g_event_enable = NULL; // CloseHandle
static HANDLE g_event_quit = NULL; // CloseHandle
static HANDLE g_thread = NULL; // CloseHandle

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// OK
si_frame::si_frame() : sensor_frame()
{

}

// OK
si_frame::~si_frame()
{

}

// OK
static void SI_Acquire(int base_priority)
{
    PerceptionTimestamp ts = nullptr;
    SpatialCoordinateSystem world = nullptr;
    si_frame* frame;
    UINT64 qpc;
    int status1;
    int status2;

    WaitForSingleObject(g_event_enable, INFINITE);
    SetThreadPriority(GetCurrentThread(), ExtendedExecution_GetInterfacePriority(INTERFACE_ID::ID_SI));

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

    g_buffer.Insert(frame, qpc);
    }
    while (WaitForSingleObject(g_event_enable, 0) == WAIT_OBJECT_0);
    
    g_buffer.Clear();

    SetThreadPriority(GetCurrentThread(), base_priority);
}

// OK
static DWORD WINAPI SI_EntryPoint(void *param)
{
    (void)param;
    SpatialInput_WaitForEyeConsent();
    int base_priority = GetThreadPriority(GetCurrentThread());
    do { SI_Acquire(base_priority); } while (WaitForSingleObject(g_event_quit, 0) == WAIT_TIMEOUT);
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
    return g_buffer.Get(stamp, (sensor_frame*&)f, t, s);
}

// OK
int SI_Get(uint64_t timestamp, int time_preference, bool tiebreak_right, void*& f, uint64_t& t, int32_t& s)
{
    return g_buffer.Get(timestamp, time_preference, tiebreak_right, (sensor_frame*&)f, t, s);
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
    g_buffer.Reset(buffer_size);
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

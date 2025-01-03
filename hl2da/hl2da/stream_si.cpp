
#include "extended_execution.h"
#include "spatial_input.h"
#include "frame_buffer.h"
#include "server_ports.h"

#include <winrt/Windows.Perception.People.h>

using namespace winrt::Windows::Perception::People;

class si_frame : public sensor_frame
{
public:
    int32_t valid;
    SpatialInput_Frame head_pose;
    SpatialInput_Ray eye_ray;
    JointPose left_hand[HAND_JOINTS];
    JointPose right_hand[HAND_JOINTS];

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

static void SI_Push(uint32_t valid, SpatialInput_Frame* head_pose, SpatialInput_Ray* eye_ray, JointPose* left_hand, JointPose* right_hand, UINT64 timestamp, void* param)
{
    (void)param;

    si_frame* frame = new si_frame();

    frame->valid = valid;
    memcpy(&frame->head_pose,  head_pose,  sizeof(frame->head_pose));
    memcpy(&frame->eye_ray,    eye_ray,    sizeof(frame->eye_ray));
    memcpy( frame->left_hand,  left_hand,  sizeof(frame->left_hand));
    memcpy( frame->right_hand, right_hand, sizeof(frame->right_hand));

    g_buffer.Insert(frame, timestamp);
}

// OK
static void SI_Acquire(int base_priority)
{
    WaitForSingleObject(g_event_enable, INFINITE);

    SetThreadPriority(GetCurrentThread(), ExtendedExecution_GetInterfacePriority(PORT_ID_SI));
    SpatialInput_ExecuteSensorLoop(SI_Push, NULL, g_event_enable);
    SetThreadPriority(GetCurrentThread(), base_priority);
    g_buffer.Clear();
}

// OK
static DWORD WINAPI SI_EntryPoint(void *param)
{
    (void)param;
    if (!SpatialInput_WaitForConsent()) { return 0; }
    int base_priority = GetThreadPriority(GetCurrentThread());
    do { SI_Acquire(base_priority); } while (WaitForSingleObject(g_event_quit, 0) == WAIT_TIMEOUT);
    return 0;
}

// OK
void SI_SetEnable(bool enable)
{
    (void)enable;
    SetEvent(g_event_enable);
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

    *valid        = f->valid;
    *head_buffer  = &f->head_pose;
    *head_length  = sizeof(si_frame::head_pose) / sizeof(float);
    *eye_buffer   = &f->eye_ray;
    *eye_length   = sizeof(si_frame::eye_ray) / sizeof(float);
    *left_buffer  = f->left_hand;
    *left_length  = HAND_JOINTS;
    *right_buffer = f->right_hand;
    *right_length = HAND_JOINTS;
}

// OK
void SI_Initialize(int buffer_size)
{
    g_buffer.Reset(buffer_size);
    g_event_quit = CreateEvent(NULL, TRUE, FALSE, NULL);
    g_event_enable = CreateEvent(NULL, FALSE, FALSE, NULL);
    g_thread = CreateThread(NULL, 0, SI_EntryPoint, NULL, 0, NULL);
}

// OK
void SI_Cleanup()
{
    SetEvent(g_event_quit);
    WaitForSingleObject(g_thread, INFINITE);
    CloseHandle(g_thread);
    CloseHandle(g_event_enable);
    CloseHandle(g_event_quit);
}


#include "extended_execution.h"
#include "extended_eye_tracking.h"
#include "frame_buffer.h"
#include "server_ports.h"

#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Microsoft.MixedReality.EyeTracking.h>

using namespace winrt::Windows::Foundation::Numerics;
using namespace winrt::Microsoft::MixedReality::EyeTracking;

struct eet_data
{
    float3 c_origin;
    float3 c_direction;
    float3 l_origin;
    float3 l_direction;
    float3 r_origin;
    float3 r_direction;
    float  l_openness;
    float  r_openness;
    float  vergence_distance;
};

class ee_frame : public sensor_frame
{
public:
    uint32_t valid;
    eet_data frame;
    float4x4 pose;

    ee_frame();
    ~ee_frame();
};

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

static frame_buffer g_buffer;
static HANDLE g_event_enable = NULL; // CloseHandle
static HANDLE g_event_quit = NULL; // CloseHandle
static HANDLE g_thread = NULL; // CloseHandle
static int32_t g_fps_index;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// OK
ee_frame::ee_frame() : sensor_frame()
{
}

// OK
ee_frame::~ee_frame()
{
}

// OK
static void EE_Push(EyeGazeTrackerReading const& frame, UINT64 host_ticks, void* param)
{
    (void)param;

    ee_frame* f = new ee_frame();

    if (frame)
    {
    bool cg_valid = frame.TryGetCombinedEyeGazeInTrackerSpace(f->frame.c_origin, f->frame.c_direction);
    bool lg_valid = frame.TryGetLeftEyeGazeInTrackerSpace(f->frame.l_origin, f->frame.l_direction);
    bool rg_valid = frame.TryGetRightEyeGazeInTrackerSpace(f->frame.r_origin, f->frame.r_direction);
    bool lo_valid = frame.TryGetLeftEyeOpenness(f->frame.l_openness);
    bool ro_valid = frame.TryGetRightEyeOpenness(f->frame.r_openness);
    bool vd_valid = frame.TryGetVergenceDistance(f->frame.vergence_distance);
    bool ec_valid = frame.IsCalibrationValid();

    f->valid = (vd_valid << 6) | (ro_valid << 5) | (lo_valid << 4) | (rg_valid << 3) | (lg_valid << 2) | (cg_valid << 1) | (ec_valid << 0);
    f->pose  = ExtendedEyeTracking_GetNodeWorldPose(host_ticks);
    }
    else
    {
    memset(f, 0, sizeof(ee_frame));
    }

    g_buffer.Insert(f, host_ticks);
}

// OK
static void EE_Acquire()
{
    int base_priority = GetThreadPriority(GetCurrentThread());
    uint8_t fps;

    WaitForSingleObject(g_event_enable, INFINITE);

    ExtendedEyeTracking_Open(true);

    if (!ExtendedEyeTracking_Status()) { goto _fail; }

    SetThreadPriority(GetCurrentThread(), ExtendedExecution_GetInterfacePriority(PORT_ID_EET));

    switch (g_fps_index & 3)
    {
    case 0:  fps = 30; break;
    case 1:  fps = 60; break;
    case 2:  fps = 90; break;
    default: fps = 30;
    }

    ExtendedEyeTracking_SetTargetFrameRate(fps);

    ExtendedEyeTracking_ExecuteSensorLoop(EE_Push, NULL, g_event_enable);

    ExtendedEyeTracking_Close();

    g_buffer.Clear();

    SetThreadPriority(GetCurrentThread(), base_priority);

    return;

_fail:
    WaitForSingleObject(g_event_enable, INFINITE);
}

// OK
static DWORD WINAPI EE_EntryPoint(void* param)
{
    (void)param;
    if (!ExtendedEyeTracking_WaitForConsent()) { return 0; }
    do { EE_Acquire(); } while (WaitForSingleObject(g_event_quit, 0) == WAIT_TIMEOUT);
    return 0;
}

// OK
void EE_SetFormat(int fps_index)
{
    g_fps_index = fps_index;
}

// OK
void EE_SetEnable(bool enable)
{
    (void)enable;
    ReleaseSemaphore(g_event_enable, 1, NULL);
}

// OK
int EE_Get(int32_t stamp, void*& f, uint64_t& t, int32_t& s)
{
    return g_buffer.Get(stamp, (sensor_frame*&)f, t, s);
}

// OK
int EE_Get(uint64_t timestamp, int time_preference, bool tiebreak_right, void*& f, uint64_t& t, int32_t& s)
{
    return g_buffer.Get(timestamp, time_preference, tiebreak_right, (sensor_frame*&)f, t, s);
}

// OK
void EE_Release(void* frame)
{
    ((ee_frame*)frame)->Release();
}

// OK
void EE_Extract(void* frame, int32_t* valid, void const** buffer, int32_t* length, void const** pose_buffer, int32_t* pose_length)
{
    ee_frame* f = (ee_frame*)frame;

    *valid       = f->valid;
    *buffer      = &f->frame;
    *length      = sizeof(ee_frame::frame) / sizeof(float);
    *pose_buffer = &f->pose;
    *pose_length = sizeof(ee_frame::pose) / sizeof(float);
}

// OK
void EE_Initialize(int buffer_size)
{
    g_buffer.Reset(buffer_size);
    g_event_quit = CreateEvent(NULL, TRUE, FALSE, NULL);
    g_event_enable = CreateSemaphore(NULL, 0, LONG_MAX, NULL);
    g_thread = CreateThread(NULL, 0, EE_EntryPoint, NULL, 0, NULL);
}

// OK
void EE_Cleanup()
{
    SetEvent(g_event_quit);
    WaitForSingleObject(g_thread, INFINITE);
    CloseHandle(g_thread);
    CloseHandle(g_event_enable);
    CloseHandle(g_event_quit);
}

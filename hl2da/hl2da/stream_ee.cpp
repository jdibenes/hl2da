
#include "extended_eye_tracking.h"
#include "locator.h"
#include "frame_buffer.h"
#include "timestamps.h"
#include "log.h"

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.Perception.h>
#include <winrt/Windows.Perception.Spatial.h>
#include <winrt/Microsoft.MixedReality.EyeTracking.h>

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Numerics;
using namespace winrt::Windows::Perception;
using namespace winrt::Windows::Perception::Spatial;
using namespace winrt::Microsoft::MixedReality::EyeTracking;

struct ee_data
{
    winrt::Windows::Foundation::Numerics::float3 c_origin;
    winrt::Windows::Foundation::Numerics::float3 c_direction;
    winrt::Windows::Foundation::Numerics::float3 l_origin;
    winrt::Windows::Foundation::Numerics::float3 l_direction;
    winrt::Windows::Foundation::Numerics::float3 r_origin;
    winrt::Windows::Foundation::Numerics::float3 r_direction;
    float  l_openness;
    float  r_openness;
    float  vergence_distance;
};

struct ee_frame : public sensor_frame
{
public:
    uint32_t valid;
    ee_data frame;
    winrt::Windows::Foundation::Numerics::float4x4 pose;

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
static void EE_Acquire(SpatialLocator const &locator, uint64_t utc_offset)
{
    PerceptionTimestamp ts = nullptr;
    EyeGazeTrackerReading egtr = nullptr;
    int32_t fps;
    DateTime td;
    uint64_t timestamp;
    uint64_t delay;
    int64_t max_delta;
    ee_frame* f;
    bool cg_valid;
    bool lg_valid;
    bool rg_valid;
    bool lo_valid;
    bool ro_valid;
    bool vd_valid;
    bool ec_valid;

    WaitForSingleObject(g_event_enable, INFINITE);

    switch (g_fps_index & 3)
    {
    case 0:  fps = 30; break;
    case 1:  fps = 60; break;
    case 2:  fps = 90; break;
    default: fps = 30;
    }

    max_delta = HNS_BASE / fps;
    delay = max_delta * 1;

    ExtendedEyeTracking_SetTargetFrameRate(g_fps_index);

    do
    {
    timestamp = GetCurrentUTCTimestamp() - delay;

    Sleep(1000 / fps);
    
    egtr = ExtendedEyeTracking_GetReading(DateTime(QPCTimestampToTimeSpan(timestamp)), max_delta);

    f = new ee_frame();

    if (egtr)
    {
    timestamp = egtr.Timestamp().time_since_epoch().count() - utc_offset;

    cg_valid = egtr.TryGetCombinedEyeGazeInTrackerSpace(f->frame.c_origin, f->frame.c_direction);
    lg_valid = egtr.TryGetLeftEyeGazeInTrackerSpace(f->frame.l_origin, f->frame.l_direction);
    rg_valid = egtr.TryGetRightEyeGazeInTrackerSpace(f->frame.r_origin, f->frame.r_direction);
    lo_valid = egtr.TryGetLeftEyeOpenness(f->frame.l_openness);
    ro_valid = egtr.TryGetRightEyeOpenness(f->frame.r_openness);
    vd_valid = egtr.TryGetVergenceDistance(f->frame.vergence_distance);
    ec_valid = egtr.IsCalibrationValid();

    f->valid = (vd_valid << 6) | (ro_valid << 5) | (lo_valid << 4) | (rg_valid << 3) | (lg_valid << 2) | (cg_valid << 1) | (ec_valid << 0);

    ts = QPCTimestampToPerceptionTimestamp(timestamp);
    f->pose = Locator_Locate(ts, locator, Locator_GetWorldCoordinateSystem(ts));
    }
    else
    {
    timestamp -= utc_offset;
    f->valid = 0;
    }

    g_buffer.Insert(f, timestamp);
    }
    while (WaitForSingleObject(g_event_enable, 0) == WAIT_OBJECT_0);

    g_buffer.Clear();
}

// OK
static DWORD WINAPI EE_EntryPoint(void* param)
{
    (void)param;

    SpatialLocator locator = nullptr;
    uint64_t utc_offset;

    ExtendedEyeTracking_Initialize();

    locator = ExtendedEyeTracking_CreateLocator();
    utc_offset = GetQPCToUTCOffset(32);

    do { EE_Acquire(locator, utc_offset); } while (WaitForSingleObject(g_event_quit, 0) == WAIT_TIMEOUT);

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
    if (enable) { SetEvent(g_event_enable); } else { ResetEvent(g_event_enable); }
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
    *valid = f->valid;
    *buffer = &f->frame;
    *length = sizeof(ee_frame::frame) / sizeof(float);
    *pose_buffer = &f->pose;
    *pose_length = sizeof(ee_frame::pose) / sizeof(float);
}

// OK
void EE_Initialize(int buffer_size)
{
    g_buffer.Reset(buffer_size);
    g_event_enable = CreateEvent(NULL, TRUE, FALSE, NULL);
    g_event_quit = CreateEvent(NULL, TRUE, FALSE, NULL);
    g_thread = CreateThread(NULL, 0, EE_EntryPoint, NULL, 0, NULL);
}

// OK
void EE_Quit()
{
    SetEvent(g_event_quit);
}

// OK
void EE_Cleanup()
{
    WaitForSingleObject(g_thread, INFINITE);
    CloseHandle(g_thread);
    CloseHandle(g_event_quit);
    CloseHandle(g_event_enable);
}


#include "research_mode.h"
#include "locator.h"
#include "extended_execution.h"
#include "frame_buffer.h"
#include "timestamps.h"
#include "log.h"

#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.Perception.h>
#include <winrt/Windows.Perception.Spatial.h>
#include <winrt/Windows.Perception.Spatial.h>
#include <winrt/Windows.Perception.Spatial.Preview.h>

using namespace winrt::Windows::Foundation::Numerics;
using namespace winrt::Windows::Perception;
using namespace winrt::Windows::Perception::Spatial;
using namespace winrt::Windows::Perception::Spatial;
using namespace winrt::Windows::Perception::Spatial::Preview;

struct vlc_metadata
{
    uint64_t exposure;
    uint32_t gain;
    uint32_t _reserved;
};

class rm_frame : public sensor_frame
{
public:
    IResearchModeSensorFrame* rmsf;
    vlc_metadata vlc;
    winrt::Windows::Foundation::Numerics::float4x4 pose;

    rm_frame(IResearchModeSensorFrame* f, winrt::Windows::Foundation::Numerics::float4x4 const& p);
    ~rm_frame();
};

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

constexpr int SENSOR_COUNT = 9;

static CRITICAL_SECTION g_depth_lock;
static bool g_bypass_lock;
static int64_t g_vlc_constant_factor = -125000;

static frame_buffer g_buffer[SENSOR_COUNT];
static HANDLE g_event_enable[SENSOR_COUNT]; // CloseHandle
static HANDLE g_event_quit[SENSOR_COUNT]; // CloseHandle
static HANDLE g_thread[SENSOR_COUNT]; // CloseHandle

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// OK
rm_frame::rm_frame(IResearchModeSensorFrame* f, float4x4 const& p) : sensor_frame(), rmsf(f), pose(p)
{
}

// OK
rm_frame::~rm_frame()
{
    rmsf->Release();
}

// OK
static void RM_Acquire(IResearchModeSensor* sensor, int id, SpatialLocator const& locator, int base_priority)
{
    bool is_depth_camera = ((id == DEPTH_AHAT) || (id == DEPTH_LONG_THROW)) && !g_bypass_lock;
    bool is_vlc = (id >= LEFT_FRONT) && (id <= RIGHT_RIGHT);
    PerceptionTimestamp perception_timestamp = nullptr;
    HANDLE event_enable = g_event_enable[id];
    IResearchModeSensorFrame* pSensorFrame; // Release
    uint64_t host_ticks;
    ResearchModeSensorTimestamp timestamp;    
    float4x4 pose;

    WaitForSingleObject(event_enable, INFINITE);
    SetThreadPriority(GetCurrentThread(), ExtendedExecution_GetInterfacePriority(id));

    if (is_depth_camera) { EnterCriticalSection(&g_depth_lock); }

    sensor->OpenStream();

    do
    {
    sensor->GetNextBuffer(&pSensorFrame); // block

    pSensorFrame->GetTimeStamp(&timestamp);
    host_ticks = timestamp.HostTicks + (is_vlc ? g_vlc_constant_factor : 0); // workaround for https://github.com/microsoft/HoloLens2ForCV/issues/134
    perception_timestamp = QPCTimestampToPerceptionTimestamp(host_ticks);
    pose = Locator_Locate(perception_timestamp, locator, Locator_GetWorldCoordinateSystem(perception_timestamp));

    g_buffer[id].Insert(new rm_frame(pSensorFrame, pose), host_ticks);
    }
    while (WaitForSingleObject(event_enable, 0) == WAIT_OBJECT_0);

    g_buffer[id].Clear();

    sensor->CloseStream();

    if (is_depth_camera) { LeaveCriticalSection(&g_depth_lock); }

    SetThreadPriority(GetCurrentThread(), base_priority);
}

// OK
static DWORD WINAPI RM_EntryPoint(void* param)
{
    SpatialLocator locator = nullptr;
    IResearchModeSensor* sensor;
    ResearchModeSensorType type;
    int base_priority;
    bool ok;

    sensor = (IResearchModeSensor*)param;
    type = sensor->GetSensorType();

    ok = ResearchMode_WaitForConsent(sensor);
    if (!ok) { return false; }

    locator = ResearchMode_GetLocator();
    base_priority = GetThreadPriority(GetCurrentThread());

    do { RM_Acquire(sensor, type, locator, base_priority); } while (WaitForSingleObject(g_event_quit[type], 0) == WAIT_TIMEOUT);

    return 0;
}

// OK
void RM_SetEnable(int id, bool enable)
{
    HANDLE event_enable = g_event_enable[id];
    if (enable) { SetEvent(event_enable); } else { ResetEvent(event_enable); }
}

// OK
int RM_Get(int id, int32_t stamp, void*& f, uint64_t& t, int32_t& s)
{
    return g_buffer[id].Get(stamp, (sensor_frame*&)f, t, s);
}

// OK
int RM_Get(int id, uint64_t timestamp, int time_preference, bool tiebreak_right, void*& f, uint64_t& t, int32_t& s)
{
    return g_buffer[id].Get(timestamp, time_preference, tiebreak_right, (sensor_frame*&)f, t, s);
}

// OK
void RM_Release(void* frame)
{
    ((rm_frame*)frame)->Release();
}

// OK
void RM_Extract_VLC(void* frame, void const** buffer, int32_t* length, void const** metadata_buffer, int32_t* metadata_length, void const** pose_buffer, int32_t* pose_length)
{
    rm_frame* f = (rm_frame*)frame;
    IResearchModeSensorVLCFrame* pVLCFrame; // Release
    uint8_t const* b;
    size_t l;

    f->rmsf->QueryInterface(IID_PPV_ARGS(&pVLCFrame));
    pVLCFrame->GetBuffer(&b, &l);
    pVLCFrame->GetExposure(&f->vlc.exposure);
    pVLCFrame->GetGain(&f->vlc.gain);
    pVLCFrame->Release();

    *buffer = b;
    *length = (int32_t)l;
    *pose_buffer = &f->pose;
    *pose_length = sizeof(rm_frame::pose) / sizeof(float);
    *metadata_buffer = &f->vlc;
    *metadata_length = sizeof(rm_frame::vlc) / sizeof(vlc_metadata);
}

// OK
void RM_Extract_Depth_AHAT(void* frame, void const** buffer, int32_t* length, void const** ab_depth_buffer, int32_t* ab_depth_length, void const** pose_buffer, int32_t* pose_length)
{
    rm_frame* f = (rm_frame*)frame;
    IResearchModeSensorDepthFrame* pDepthFrame; // Release
    uint16_t const* b;
    size_t l;
    uint16_t const* ab_depth_b;
    size_t ab_depth_l;

    f->rmsf->QueryInterface(IID_PPV_ARGS(&pDepthFrame));
    pDepthFrame->GetBuffer(&b, &l);
    pDepthFrame->GetAbDepthBuffer(&ab_depth_b, &ab_depth_l);
    pDepthFrame->Release();

    *buffer = b;
    *length = (int32_t)l;
    *ab_depth_buffer = ab_depth_b;
    *ab_depth_length = (int32_t)ab_depth_l;
    *pose_buffer = &f->pose;
    *pose_length = sizeof(rm_frame::pose) / sizeof(float);
}

// OK
void RM_Extract_Depth_Longthrow(void* frame, void const** buffer, int32_t* length, void const** ab_depth_buffer, int32_t* ab_depth_length, void const** sigma_buffer, int32_t* sigma_length, void const** pose_buffer, int32_t* pose_length)
{
    rm_frame* f = (rm_frame*)frame;
    IResearchModeSensorDepthFrame* pDepthFrame; // Release
    uint16_t const* b;
    size_t l;
    uint16_t const* ab_depth_b;
    size_t ab_depth_l;
    uint8_t const* sigma_b;
    size_t sigma_l;

    f->rmsf->QueryInterface(IID_PPV_ARGS(&pDepthFrame));
    pDepthFrame->GetBuffer(&b, &l);
    pDepthFrame->GetAbDepthBuffer(&ab_depth_b, &ab_depth_l);
    pDepthFrame->GetSigmaBuffer(&sigma_b, &sigma_l);
    pDepthFrame->Release();

    *buffer = b;
    *length = (int32_t)l;
    *ab_depth_buffer = ab_depth_b;
    *ab_depth_length = (int32_t)ab_depth_l;
    *sigma_buffer = sigma_b;
    *sigma_length = (int32_t)sigma_l;
    *pose_buffer = &f->pose;
    *pose_length = sizeof(rm_frame::pose) / sizeof(float);
}

// OK
void RM_Extract_IMU_Accelerometer(void* frame, void const** buffer, int32_t* length, void const** pose_buffer, int32_t* pose_length)
{
    rm_frame* f = (rm_frame*)frame;
    IResearchModeAccelFrame* pAccFrame; // Release
    AccelDataStruct const* b;
    size_t l;
    
    f->rmsf->QueryInterface(IID_PPV_ARGS(&pAccFrame));
    pAccFrame->GetCalibratedAccelarationSamples(&b, &l);
    pAccFrame->Release();

    *buffer = b;
    *length = (int32_t)l;
    *pose_buffer = &f->pose;
    *pose_length = sizeof(rm_frame::pose) / sizeof(float);
}

// OK
void RM_Extract_IMU_Gyroscope(void* frame, void const** buffer, int32_t* length, void const** pose_buffer, int32_t* pose_length)
{
    rm_frame* f = (rm_frame*)frame;
    IResearchModeGyroFrame* pGyroFrame; // Release
    GyroDataStruct const* b;
    size_t l;

    f->rmsf->QueryInterface(IID_PPV_ARGS(&pGyroFrame));
    pGyroFrame->GetCalibratedGyroSamples(&b, &l);
    pGyroFrame->Release();

    *buffer = b;
    *length = (int32_t)l;
    *pose_buffer = &f->pose;
    *pose_length = sizeof(rm_frame::pose) / sizeof(float);
}

// OK
void RM_Extract_IMU_Magnetometer(void* frame, void const** buffer, int32_t* length, void const** pose_buffer, int32_t* pose_length)
{
    rm_frame* f = (rm_frame*)frame;
    IResearchModeMagFrame* pMagFrame; // Release
    MagDataStruct const* b;
    size_t l;

    f->rmsf->QueryInterface(IID_PPV_ARGS(&pMagFrame));
    pMagFrame->GetMagnetometerSamples(&b, &l);
    pMagFrame->Release();

    *buffer = b;
    *length = (int32_t)l;
    *pose_buffer = &f->pose;
    *pose_length = sizeof(rm_frame::pose) / sizeof(float);
}

// OK
void RM_InitializeDepthLock()
{
    InitializeCriticalSection(&g_depth_lock);
}

// OK
void RM_CleanupDepthLock()
{
    DeleteCriticalSection(&g_depth_lock);
}

// OK
void RM_BypassDepthLock(bool bypass)
{
    g_bypass_lock = bypass;
}

// OK
void RM_VLC_SetConstantFactor(int64_t factor)
{
    g_vlc_constant_factor = factor;
}

// OK
void RM_Initialize(int id, int32_t buffer_size)
{
    g_buffer[id].Reset(buffer_size);
    g_event_enable[id] = CreateEvent(NULL, TRUE, FALSE, NULL);
    g_event_quit[id] = CreateEvent(NULL, TRUE, FALSE, NULL);
    g_thread[id] = CreateThread(NULL, 0, RM_EntryPoint, ResearchMode_GetSensor((ResearchModeSensorType)id), NULL, NULL);
}

// OK
void RM_Quit(int id)
{
    SetEvent(g_event_quit[id]);
}

// OK
void RM_Cleanup(int id)
{
    WaitForSingleObject(g_thread[id], INFINITE);

    CloseHandle(g_thread[id]);
    CloseHandle(g_event_quit[id]);
    CloseHandle(g_event_enable[id]);
}

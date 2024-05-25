
#include "stream_rm.h"
#include "locator.h"
#include "timestamps.h"
#include "ring_buffer.h"
#include "lock.h"
#include "log.h"
#include "types.h"

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

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

constexpr int SENSOR_COUNT = 9;

static CRITICAL_SECTION g_depth_lock;
static SRWLOCK g_lock[SENSOR_COUNT];
static ring_buffer<rm_frame*> g_buffer[SENSOR_COUNT];
static HANDLE g_event_enable[SENSOR_COUNT]; // CloseHandle
static HANDLE g_event_quit[SENSOR_COUNT]; // CloseHandle
static HANDLE g_thread[SENSOR_COUNT]; // CloseHandle

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// OK
rm_frame::rm_frame(IResearchModeSensorFrame* f, float4x4 p) : rmsf(f), pose(p), m_count(1)
{
}

// OK
ULONG rm_frame::AddRef()
{
    return InterlockedIncrement(&m_count);
}

// OK
ULONG rm_frame::Release()
{
    ULONG uCount = InterlockedDecrement(&m_count);
    if (uCount != 0) { return uCount; }
    if (rmsf) { rmsf->Release(); }
    delete this;    
    return uCount;
}

// OK
static void RM_Insert(int id, IResearchModeSensorFrame* pSensorFrame, float4x4 pose, uint64_t timestamp)
{
    rm_frame* f;
    SRWLock srw(&g_lock[id], true);
    f = g_buffer[id].insert(new rm_frame(pSensorFrame, pose), timestamp);
    if (f) { f->Release(); }
}

// OK
static void RM_Clear(int id)
{
    SRWLock srw(&g_lock[id], true);
    for (int32_t i = 0; i < g_buffer[id].size(); ++i) { g_buffer[id].at(i)->Release(); }
    g_buffer[id].reset();
}

// OK
static void RM_Acquire(IResearchModeSensor* sensor, int id, SpatialLocator const& locator)
{
    bool is_depth_camera = (id == DEPTH_AHAT) || (id == DEPTH_LONG_THROW);
    PerceptionTimestamp perception_timestamp = nullptr;
    HANDLE event_enable = g_event_enable[id];
    IResearchModeSensorFrame* pSensorFrame; // Release
    uint64_t host_ticks;
    ResearchModeSensorTimestamp timestamp;    
    float4x4 pose;

    ShowMessage(L"RM%d: Waiting for enable", id);

    WaitForSingleObject(event_enable, INFINITE);

    ShowMessage(L"RM%d: Enabled", id);

    if (is_depth_camera) { EnterCriticalSection(&g_depth_lock); }

    sensor->OpenStream();

    do
    {
    sensor->GetNextBuffer(&pSensorFrame); // block

    pSensorFrame->GetTimeStamp(&timestamp);

    host_ticks = timestamp.HostTicks;
    perception_timestamp = QPCTimestampToPerceptionTimestamp(host_ticks);

    pose = Locator_Locate(perception_timestamp, locator, Locator_GetWorldCoordinateSystem(perception_timestamp));

    RM_Insert(id, pSensorFrame, pose, host_ticks);
    }
    while (WaitForSingleObject(event_enable, 0) == WAIT_OBJECT_0);

    RM_Clear(id);

    sensor->CloseStream();

    if (is_depth_camera) { LeaveCriticalSection(&g_depth_lock); }

    ShowMessage(L"RM%d: Disabled", id);
}

// OK
static DWORD WINAPI RM_EntryPoint(void* param)
{
    SpatialLocator locator = nullptr;
    IResearchModeSensor* sensor;
    ResearchModeSensorType type;
    bool ok;

    sensor = (IResearchModeSensor*)param;
    type = sensor->GetSensorType();

    ShowMessage(L"RM%d (%s): Waiting for consent", type, sensor->GetFriendlyName());

    ok = ResearchMode_WaitForConsent(sensor);
    if (!ok) { return false; }

    locator = ResearchMode_GetLocator();

    do { RM_Acquire(sensor, type, locator); } while (WaitForSingleObject(g_event_quit, 0) == WAIT_TIMEOUT);

    return 0;
}

// OK
void RM_SetEnable(int id, bool enable)
{
    HANDLE event_enable = g_event_enable[id];
    if (enable) { SetEvent(event_enable); } else { ResetEvent(event_enable); }
}

// OK
int RM_Get(int id, int32_t stamp, rm_frame*& f, uint64_t& t, int32_t& s)
{
    SRWLock srw(&g_lock[id], false);
    int v = g_buffer[id].get(stamp, f, t, s);
    if (v == 0) { f->AddRef(); }
    return v;
}

// OK
int RM_Get(int id, uint64_t timestamp, int time_preference, bool tiebreak_right, rm_frame*& f, uint64_t& t, int32_t& s)
{
    SRWLock srw(&g_lock[id], false);
    int v = g_buffer[id].get(timestamp, time_preference, tiebreak_right, f, t, s);
    if (v == 0) { f->AddRef(); }
    return v;
}

// OK
void RM_Extract(IResearchModeSensorFrame* f, rm_data_vlc& d)
{
    IResearchModeSensorVLCFrame* pVLCFrame; // Release
    
    f->QueryInterface(IID_PPV_ARGS(&pVLCFrame));
    pVLCFrame->GetBuffer(&d.buffer, &d.length);
    pVLCFrame->Release();
}

// OK
void RM_Extract(IResearchModeSensorFrame* f, rm_data_zht& d)
{
    IResearchModeSensorDepthFrame* pDepthFrame; // Release

    f->QueryInterface(IID_PPV_ARGS(&pDepthFrame));
    pDepthFrame->GetBuffer(&d.buffer, &d.length);
    pDepthFrame->GetAbDepthBuffer(&d.ab_depth_buffer, &d.ab_depth_length);
    pDepthFrame->Release();
}

// OK
void RM_Extract(IResearchModeSensorFrame* f, rm_data_zlt& d)
{
    IResearchModeSensorDepthFrame* pDepthFrame; // Release

    f->QueryInterface(IID_PPV_ARGS(&pDepthFrame));
    pDepthFrame->GetBuffer(&d.buffer, &d.length);
    pDepthFrame->GetAbDepthBuffer(&d.ab_depth_buffer, &d.ab_depth_length);
    pDepthFrame->GetSigmaBuffer(&d.sigma_buffer, &d.sigma_length);
    pDepthFrame->Release();
}

// OK
void RM_Extract(IResearchModeSensorFrame* f, rm_data_acc& d)
{
    IResearchModeAccelFrame* pAccFrame; // Release

    f->QueryInterface(IID_PPV_ARGS(&pAccFrame));
    pAccFrame->GetCalibratedAccelarationSamples(&d.buffer, &d.length);
    pAccFrame->Release();
}

// OK
void RM_Extract(IResearchModeSensorFrame* f, rm_data_gyr& d)
{
    IResearchModeGyroFrame* pGyroFrame; // Release

    f->QueryInterface(IID_PPV_ARGS(&pGyroFrame));
    pGyroFrame->GetCalibratedGyroSamples(&d.buffer, &d.length);
    pGyroFrame->Release();
}

// OK
void RM_Extract(IResearchModeSensorFrame* f, rm_data_mag& d)
{
    IResearchModeMagFrame* pMagFrame; // Release

    f->QueryInterface(IID_PPV_ARGS(&pMagFrame));
    pMagFrame->GetMagnetometerSamples(&d.buffer, &d.length);
    pMagFrame->Release();
}

// OK
void RM_GetExtrinsics(int id, float* out)
{
    IResearchModeSensor* sensor = ResearchMode_GetSensor((ResearchModeSensorType)id);
    IResearchModeCameraSensor* pCameraSensor; // Release
    IResearchModeAccelSensor* pAccelSensor; // Release
    IResearchModeGyroSensor* pGyroSensor; // Release
    DirectX::XMFLOAT4X4 extrinsics;

    switch (id)
    {
    case LEFT_FRONT:
    case LEFT_LEFT:
    case RIGHT_FRONT:
    case RIGHT_RIGHT:
    case DEPTH_AHAT:
    case DEPTH_LONG_THROW:
        sensor->QueryInterface(IID_PPV_ARGS(&pCameraSensor));
        pCameraSensor->GetCameraExtrinsicsMatrix(&extrinsics);
        pCameraSensor->Release();
        break;
    case IMU_ACCEL:
        sensor->QueryInterface(IID_PPV_ARGS(&pAccelSensor));
        pAccelSensor->GetExtrinsicsMatrix(&extrinsics);
        pAccelSensor->Release();
        break;
    case IMU_GYRO:
        sensor->QueryInterface(IID_PPV_ARGS(&pGyroSensor));
        pGyroSensor->GetExtrinsicsMatrix(&extrinsics);
        pGyroSensor->Release();
        break;
    case IMU_MAG:
    default:
        memset(extrinsics.m, 0, sizeof(extrinsics.m));
        break;
    }

    memcpy(out, extrinsics.m, sizeof(extrinsics.m));
}

// OK
void RM_MapImagePointToCameraUnitPlane(int id, float const* in, float* out, int point_count)
{
    IResearchModeSensor* sensor = ResearchMode_GetSensor((ResearchModeSensorType)id);
    IResearchModeCameraSensor* pCameraSensor; // Release

    sensor->QueryInterface(IID_PPV_ARGS(&pCameraSensor));

    for (int i = 0; i < point_count; ++i)
    {
    float uv[2] = { in[2 * i], in[(2 * i) + 1] };
    float xy[2];

    pCameraSensor->MapImagePointToCameraUnitPlane(uv, xy);

    out[2 * i] = xy[0];
    out[(2 * i) + 1] = xy[1];
    }

    pCameraSensor->Release();
}

// OK
void RM_MapCameraSpaceToImagePoint(int id, float const* in, float* out, int point_count)
{
    IResearchModeSensor* sensor = ResearchMode_GetSensor((ResearchModeSensorType)id);
    IResearchModeCameraSensor* pCameraSensor; // Release

    sensor->QueryInterface(IID_PPV_ARGS(&pCameraSensor));

    for (int i = 0; i < point_count; ++i)
    {
    float xy[2] = { in[2 * i], in[(2 * i) + 1] };
    float uv[2];

    pCameraSensor->MapCameraSpaceToImagePoint(xy, uv);

    out[2 * i] = uv[0];
    out[(2 * i) + 1] = uv[1];
    }

    pCameraSensor->Release();
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
void RM_Initialize(int id, int32_t buffer_size)
{
    InitializeSRWLock(&g_lock[id]);
    g_buffer[id].reset(buffer_size);
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

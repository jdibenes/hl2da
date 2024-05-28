
#include <Windows.h>
#include "locator.h"
#include "extended_eye_tracking.h"
#include "stream_ee.h"
#include "ring_buffer.h"
#include "timestamps.h"
#include "lock.h"
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

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

static SRWLOCK g_lock;
static ring_buffer<ee_frame*> g_buffer;
static HANDLE g_event_enable = NULL; // CloseHandle
static HANDLE g_event_quit = NULL; // CloseHandle
static HANDLE g_thread = NULL; // CloseHandle

static int32_t g_fps_index;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// OK
ee_frame::ee_frame() : m_count(1)
{
}

// OK
ULONG ee_frame::AddRef()
{
    return InterlockedIncrement(&m_count);
}

// OK
ULONG ee_frame::Release()
{
    ULONG uCount = InterlockedDecrement(&m_count);
    if (uCount != 0) { return uCount; }
    delete this;
    return uCount;
}

// OK
static void EE_Insert(ee_frame* frame, uint64_t timestamp)
{
    ee_frame* f;
    SRWLock srw(&g_lock, true);
    f = g_buffer.insert(frame, timestamp);
    if (f) { f->Release(); }
}

// OK
static void EE_Clear()
{
    SRWLock srw(&g_lock, true);
    for (int32_t i = 0; i < g_buffer.size(); ++i) { g_buffer.at(i)->Release(); }
    g_buffer.reset();
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

    EE_Insert(f, timestamp);
    }
    while (WaitForSingleObject(g_event_enable, 0) == WAIT_OBJECT_0);

    EE_Clear();
}

// OK
static DWORD WINAPI EE_EntryPoint(void* param)
{
    (void)param;

    SpatialLocator locator = nullptr;
    uint64_t utc_offset;

    ShowMessage("EET: Waiting for consent");

    ExtendedEyeTracking_Initialize();
    locator = ExtendedEyeTracking_CreateLocator();
    utc_offset = GetQPCToUTCOffset(32);

    do { EE_Acquire(locator, utc_offset); } while (WaitForSingleObject(g_event_quit, 0) == WAIT_TIMEOUT);

    return 0;
}

// OK
void EE_SetFPS(int fps_index)
{
    g_fps_index = fps_index;
}

// OK
void EE_SetEnable(bool enable)
{
    if (enable) { SetEvent(g_event_enable); } else { ResetEvent(g_event_enable); }
}

// OK
int EE_Get(int32_t stamp, ee_frame*& f, uint64_t& t, int32_t& s)
{
    SRWLock srw(&g_lock, false);
    int v = g_buffer.get(stamp, f, t, s);
    if (v == 0) { f->AddRef(); }
    return v;
}

// OK
int EE_Get(uint64_t timestamp, int time_preference, bool tiebreak_right, ee_frame*& f, uint64_t& t, int32_t& s)
{
    SRWLock srw(&g_lock, false);
    int v = g_buffer.get(timestamp, time_preference, tiebreak_right, f, t, s);
    if (v == 0) { f->AddRef(); }
    return v;
}

// OK
void EE_Initialize(int buffer_size)
{
    InitializeSRWLock(&g_lock);
    g_buffer.reset(buffer_size);
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

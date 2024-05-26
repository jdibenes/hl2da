
#include <MemoryBuffer.h>
#include "stream_pv.h"
#include "personal_video.h"
#include "locator.h"
#include "ring_buffer.h"
#include "lock.h"
#include "log.h"
#include "timestamps.h"
#include "research_mode.h"

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Capture.h>
#include <winrt/Windows.Media.Capture.Frames.h>
#include <winrt/Windows.Media.Devices.Core.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.Perception.Spatial.h>
#include <winrt/Windows.Graphics.Imaging.h>

using namespace winrt::Windows::Media::Capture;
using namespace winrt::Windows::Media::Capture::Frames;
using namespace winrt::Windows::Media::Devices::Core;
using namespace winrt::Windows::Foundation::Numerics;
using namespace winrt::Windows::Perception::Spatial;

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

static SRWLOCK g_lock;
static ring_buffer<pv_frame*> g_buffer;
static HANDLE g_event_client = NULL; // CloseHandle
static HANDLE g_event_enable = NULL; // CloseHandle
static HANDLE g_event_quit = NULL; // CloseHandle
static HANDLE g_thread = NULL; // CloseHandle

static std::atomic<bool> g_reader_status;
pv_videoformat g_format;
MRCVideoOptions g_options;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// OK
pv_frame::pv_frame(MediaFrameReference const& f, float4 const& k, float4x4 const& p) : mfr(f), intrinsics(k), pose(p), m_count(1)
{
}

// OK
ULONG pv_frame::AddRef()
{
    return InterlockedIncrement(&m_count);
}

// OK
ULONG pv_frame::Release()
{
    ULONG uCount = InterlockedDecrement(&m_count);
    if (uCount != 0) { return uCount; }
    mfr = nullptr; // Close?
    delete this;
    return uCount;
}

// OK
static void PV_Insert(MediaFrameReference const& frame, float4 const& k, float4x4 const& pose, uint64_t timestamp)
{
    pv_frame* f;
    SRWLock srw(&g_lock, true);
    f = g_buffer.insert(new pv_frame(frame, k, pose), timestamp);
    if (f) { f->Release(); }
}

// OK
static void PV_Clear()
{
    SRWLock srw(&g_lock, true);
    for (int32_t i = 0; i < g_buffer.size(); ++i) { g_buffer.at(i)->Release(); }
    g_buffer.reset();
}

// OK
static void PV_OnVideoFrameArrived(MediaFrameReader const& sender, MediaFrameArrivedEventArgs const& args)
{
    (void)args;
    
    CameraIntrinsics intrinsics = nullptr;
    MediaFrameReference frame = nullptr;
    int64_t timestamp;
    float2 f;
    float2 c;
    float4 k;
    float4x4 pose;

    if (!g_reader_status) { return; }
    frame = sender.TryAcquireLatestFrame();
    if (!frame) { return; }

    timestamp = frame.SystemRelativeTime().Value().count();

    intrinsics = frame.VideoMediaFrame().CameraIntrinsics();
    f = intrinsics.FocalLength();
    c = intrinsics.PrincipalPoint();
    k = float4(f.x, f.y, c.x, c.y);

    pose = Locator_GetTransformTo(frame.CoordinateSystem(), Locator_GetWorldCoordinateSystem(QPCTimestampToPerceptionTimestamp(timestamp)));

    PV_Insert(frame, k, pose, timestamp);
}

// OK
static void PV_Stream()
{
    MediaFrameReader videoFrameReader = nullptr;
    bool ok;

    WaitForSingleObject(g_event_enable, INFINITE);
    WaitForSingleObject(g_event_client, 0);

    PersonalVideo_Open(g_options);

    ok = PersonalVideo_SetFormat(g_format.width, g_format.height, g_format.framerate);
    if (!ok) { return; }

    videoFrameReader = PersonalVideo_CreateFrameReader();

    videoFrameReader.AcquisitionMode(MediaFrameReaderAcquisitionMode::Buffered);
    videoFrameReader.FrameArrived(PV_OnVideoFrameArrived);

    g_reader_status = true;
    videoFrameReader.StartAsync().get();
    WaitForSingleObject(g_event_client, INFINITE);
    g_reader_status = false;
    videoFrameReader.StopAsync().get();

    videoFrameReader.Close();

    PV_Clear();

    PersonalVideo_Close();
}

// OK
static DWORD WINAPI PV_EntryPoint(void *param)
{
    (void)param;
    PersonalVideo_RegisterEvent(g_event_client);
    do { PV_Stream(); } while (WaitForSingleObject(g_event_quit, 0) == WAIT_TIMEOUT);
    return 0;
}

// OK
void PV_SetFormat(pv_captureformat const& cf)
{
    g_options = cf.mrcvo;
    g_format = cf.vf;
}

// OK
void PV_SetEnable(bool enable)
{
    enable ? SetEvent(g_event_enable) : SetEvent(g_event_client);
}

// OK
int PV_Get(int32_t stamp, pv_frame*& f, uint64_t& t, int32_t& s)
{
    SRWLock srw(&g_lock, false);
    int v = g_buffer.get(stamp, f, t, s);
    if (v == 0) { f->AddRef(); }
    return v;
}

// OK
int PV_Get(uint64_t timestamp, int time_preference, bool tiebreak_right, pv_frame*& f, uint64_t& t, int32_t& s)
{
    SRWLock srw(&g_lock, false);
    int v = g_buffer.get(timestamp, time_preference, tiebreak_right, f, t, s);
    if (v == 0) { f->AddRef(); }
    return v;
}

// OK
void PV_Extract(MediaFrameReference const& ref, pv_data& d)
{
    winrt::Windows::Graphics::Imaging::SoftwareBitmap bmp = ref.VideoMediaFrame().SoftwareBitmap();
    winrt::Windows::Graphics::Imaging::BitmapBuffer buf = bmp.LockBuffer(winrt::Windows::Graphics::Imaging::BitmapBufferAccessMode::Read);
    winrt::Windows::Foundation::IMemoryBufferReference m_ref = buf.CreateReference();
    winrt::impl::com_ref<Windows::Foundation::IMemoryBufferByteAccess> bba = m_ref.as<Windows::Foundation::IMemoryBufferByteAccess>();

    bba->GetBuffer(&d.buffer, &d.length);
}

// OK
void PV_Initialize(int32_t buffer_size)
{
    InitializeSRWLock(&g_lock);
    g_buffer.reset(buffer_size);
    g_event_client = CreateEvent(NULL, FALSE, FALSE, NULL);
    g_event_enable = CreateEvent(NULL, FALSE, FALSE, NULL);
    g_event_quit = CreateEvent(NULL, TRUE, FALSE, NULL);
    g_thread = CreateThread(NULL, 0, PV_EntryPoint, NULL, 0, NULL);
}

// OK
void PV_Quit()
{
    SetEvent(g_event_quit);
}

// OK
void PV_Cleanup()
{
    WaitForSingleObject(g_thread, INFINITE);

    CloseHandle(g_thread);
    CloseHandle(g_event_quit);
    CloseHandle(g_event_enable);
    CloseHandle(g_event_client);
    
    g_thread = NULL;
    g_event_quit = NULL;
    g_event_enable = NULL;
    g_event_client = NULL;
}

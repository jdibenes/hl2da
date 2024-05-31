
#include "stream_pv.h"
#include "personal_video.h"
#include "locator.h"
#include "frame_buffer.h"
#include "timestamps.h"
#include "log.h"

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

class pv_frame : public sensor_frame
{
public:
    winrt::Windows::Graphics::Imaging::SoftwareBitmap bmp = nullptr;
    winrt::Windows::Graphics::Imaging::BitmapBuffer buf = nullptr;
    winrt::Windows::Foundation::IMemoryBufferReference ref;

    winrt::Windows::Media::Capture::Frames::MediaFrameReference mfr;
    winrt::Windows::Foundation::Numerics::float4 intrinsics;
    winrt::Windows::Foundation::Numerics::float4x4 pose;

    pv_frame(winrt::Windows::Media::Capture::Frames::MediaFrameReference const& f, winrt::Windows::Foundation::Numerics::float4 const& k, winrt::Windows::Foundation::Numerics::float4x4 const& p);
    ~pv_frame();
};

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

static frame_buffer g_buffer;
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
pv_frame::pv_frame(MediaFrameReference const& f, float4 const& k, float4x4 const& p) : sensor_frame(), mfr(f), intrinsics(k), pose(p)
{
    bmp = f.VideoMediaFrame().SoftwareBitmap();
    buf = bmp.LockBuffer(winrt::Windows::Graphics::Imaging::BitmapBufferAccessMode::Read);
    ref = buf.CreateReference();
}

// OK
pv_frame::~pv_frame()
{
    ref.Close();
    buf.Close();
    bmp.Close();
    mfr.Close();
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

    g_buffer.Insert(new pv_frame(frame, k, pose), timestamp);
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

    g_buffer.Clear();

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
int PV_Get(int32_t stamp, void*& f, uint64_t& t, int32_t& s)
{
    return g_buffer.Get(stamp, (sensor_frame*&)f, t, s);
}

// OK
int PV_Get(uint64_t timestamp, int time_preference, bool tiebreak_right, void*& f, uint64_t& t, int32_t& s)
{
    return g_buffer.Get(timestamp, time_preference, tiebreak_right, (sensor_frame*&)f, t, s);
}

// OK
void PV_Release(void* frame)
{
    ((pv_frame*)frame)->Release();
}

// OK
void PV_Extract(void* frame, void const** buffer, int32_t* length, void const** intrinsics_buffer, int32_t* intrinsics_length, void const** pose_buffer, int32_t* pose_length)
{
    pv_frame* f = (pv_frame*)frame;

    *buffer = f->ref.data();
    *length = (int32_t)f->ref.Capacity();
    *intrinsics_buffer = &f->intrinsics;
    *intrinsics_length = sizeof(pv_frame::intrinsics) / sizeof(float);
    *pose_buffer = &f->pose;
    *pose_length = sizeof(pv_frame::pose) / sizeof(float);
}

// OK
void PV_Initialize(int32_t buffer_size)
{
    g_buffer.Reset(buffer_size);
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

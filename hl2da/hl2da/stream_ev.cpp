
#include "stream_ev.h"
#include "extended_video.h"
#include "frame_buffer.h"
#include "timestamps.h"
#include "log.h"

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Capture.h>
#include <winrt/Windows.Media.Capture.Frames.h>
#include <winrt/Windows.Media.MediaProperties.h>
#include <winrt/Windows.Media.Devices.Core.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.Graphics.Imaging.h>

using namespace winrt::Windows::Media::Capture;
using namespace winrt::Windows::Media::Capture::Frames;
using namespace winrt::Windows::Media::Devices::Core;
using namespace winrt::Windows::Foundation::Numerics;

class ev_frame : public sensor_frame
{
public:
    winrt::Windows::Graphics::Imaging::SoftwareBitmap bmp = nullptr;
    winrt::Windows::Graphics::Imaging::BitmapBuffer buf = nullptr;
    winrt::Windows::Foundation::IMemoryBufferReference ref;

    winrt::Windows::Media::Capture::Frames::MediaFrameReference mfr;
    ev_videoformat format;

    ev_frame(winrt::Windows::Media::Capture::Frames::MediaFrameReference const& f);
    ~ev_frame();
};

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

static frame_buffer g_buffer;
static HANDLE g_event_client = NULL; // CloseHandle
static HANDLE g_event_enable = NULL; // CloseHandle
static HANDLE g_event_quit = NULL; // CloseHandle
static HANDLE g_thread = NULL; // CloseHandle

static std::atomic<bool> g_reader_status = false;
static ev_videoformat g_format;
static MRCVideoOptions g_options;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// OK
ev_frame::ev_frame(MediaFrameReference const& f) : sensor_frame(), mfr(f)
{
    auto const& frameformat = f.Format();
    auto const& videoformat = frameformat.VideoFormat();
    auto const& framerate   = frameformat.FrameRate();

    format.width = (uint16_t)videoformat.Width();
    format.height = (uint16_t)videoformat.Height();
    format.framerate = (uint8_t)((double)framerate.Numerator() / (double)framerate.Denominator());

    wcscpy_s(format.subtype, sizeof(ev_videoformat::subtype) / sizeof(wchar_t), frameformat.Subtype().c_str());

    bmp = f.VideoMediaFrame().SoftwareBitmap();
    buf = bmp.LockBuffer(winrt::Windows::Graphics::Imaging::BitmapBufferAccessMode::Read);
    ref = buf.CreateReference();
}

// OK
ev_frame::~ev_frame()
{
    ref.Close();
    buf.Close();
    bmp.Close();
    mfr.Close();
}

// OK
static void EV_OnVideoFrameArrived(MediaFrameReader const& sender, MediaFrameArrivedEventArgs const& args)
{
    (void)args;
    
    CameraIntrinsics intrinsics = nullptr;
    MediaFrameReference frame = nullptr;
    int64_t timestamp;

    if (!g_reader_status) { return; }
    frame = sender.TryAcquireLatestFrame();
    if (!frame) { return; }

    timestamp = frame.SystemRelativeTime().Value().count();

    g_buffer.Insert(new ev_frame(frame), timestamp);

    if (WaitForSingleObject(g_event_enable, 0) == WAIT_OBJECT_0) { return; }
    SetEvent(g_event_client);
    g_reader_status = false;
}

// OK
static void EV_Acquire()
{
    MediaFrameReader videoFrameReader = nullptr;
    bool ok;

    WaitForSingleObject(g_event_enable, INFINITE);
    WaitForSingleObject(g_event_client, 0);

    ExtendedVideo_Open(g_options);
    ok = ExtendedVideo_Status();
    if (ok)
    {
    ok = ExtendedVideo_SetFormat(g_format.width, g_format.height, g_format.framerate, g_format.subtype);
    if (ok)
    {
    videoFrameReader = ExtendedVideo_CreateFrameReader();

    videoFrameReader.AcquisitionMode(MediaFrameReaderAcquisitionMode::Buffered);
    videoFrameReader.FrameArrived(EV_OnVideoFrameArrived);

    g_reader_status = true;
    videoFrameReader.StartAsync().get();
    WaitForSingleObject(g_event_client, INFINITE);
    g_reader_status = false;
    videoFrameReader.StopAsync().get();

    videoFrameReader.Close();

    g_buffer.Clear();
    }

    ExtendedVideo_Close();
    }

    ResetEvent(g_event_enable);
}

// OK
static DWORD WINAPI EV_EntryPoint(void *param)
{
    (void)param;
    ExtendedVideo_RegisterEvent(g_event_client);
    do { EV_Acquire(); } while (WaitForSingleObject(g_event_quit, 0) == WAIT_TIMEOUT);
    return 0;
}

// OK
void EV_SetFormat(ev_captureformat const& cf)
{
    g_options = cf.mrcvo;
    g_format = cf.vf;
}

// OK
void EV_SetEnable(bool enable)
{
    enable ? SetEvent(g_event_enable) : ResetEvent(g_event_enable);
}

// OK
int EV_Get(int32_t stamp, void*& f, uint64_t& t, int32_t& s)
{
    return g_buffer.Get(stamp, (sensor_frame*&)f, t, s);
}

// OK
int EV_Get(uint64_t timestamp, int time_preference, bool tiebreak_right, void*& f, uint64_t& t, int32_t& s)
{
    return g_buffer.Get(timestamp, time_preference, tiebreak_right, (sensor_frame*&)f, t, s);
}

// OK
void EV_Release(void* frame)
{
    ((ev_frame*)frame)->Release();
}

// OK
void EV_Extract(void* frame, void const** buffer, int32_t* length, void const** format_buffer, int32_t* format_length)
{
    ev_frame* f = (ev_frame*)frame;

    *buffer = f->ref.data();
    *length = (int32_t)f->ref.Capacity();
    *format_buffer = &f->format;
    *format_length = sizeof(ev_frame::format) / sizeof(ev_videoformat);
}

// OK
void EV_Initialize(int32_t buffer_size)
{
    g_buffer.Reset(buffer_size);
    g_event_client = CreateEvent(NULL, FALSE, FALSE, NULL);
    g_event_enable = CreateEvent(NULL, TRUE, FALSE, NULL);
    g_event_quit = CreateEvent(NULL, TRUE, FALSE, NULL);
    g_thread = CreateThread(NULL, 0, EV_EntryPoint, NULL, 0, NULL);
}

// OK
void EV_Quit()
{
    SetEvent(g_event_quit);
}

// OK
void EV_Cleanup()
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

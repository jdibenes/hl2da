
#include "stream_ev.h"
#include "extended_execution.h"
#include "extended_video.h"
#include "frame_buffer.h"
#include "server_ports.h"

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Media.MediaProperties.h>
#include <winrt/Windows.Media.Capture.Frames.h>
#include <winrt/Windows.Graphics.Imaging.h>

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Media::MediaProperties;
using namespace winrt::Windows::Media::Capture::Frames;
using namespace winrt::Windows::Graphics::Imaging;

class ev_frame : public sensor_frame
{
public:
    SoftwareBitmap bmp = nullptr;
    BitmapBuffer buf = nullptr;
    IMemoryBufferReference ref;

    MediaFrameReference mfr;
    ev_videoformat format;

    ev_frame(MediaFrameReference const& f);
    ~ev_frame();
};

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

static frame_buffer g_buffer;
static HANDLE g_event_enable = NULL; // CloseHandle
static HANDLE g_event_quit = NULL; // CloseHandle
static HANDLE g_thread = NULL; // CloseHandle
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

    format.width     = (uint16_t)videoformat.Width();
    format.height    = (uint16_t)videoformat.Height();
    format.framerate = (uint8_t)((double)framerate.Numerator() / (double)framerate.Denominator());

    wcscpy_s(format.subtype, sizeof(ev_videoformat::subtype) / sizeof(wchar_t), frameformat.Subtype().c_str());

    bmp = f.VideoMediaFrame().SoftwareBitmap();
    buf = bmp.LockBuffer(BitmapBufferAccessMode::Read);
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
static void EV_Push(MediaFrameReference const& frame, void* param)
{
    (void)param;
    uint64_t timestamp = frame.SystemRelativeTime().Value().count();
    g_buffer.Insert(new ev_frame(frame), timestamp);
}

// OK
static void EV_Acquire(int base_priority)
{
    WaitForSingleObject(g_event_enable, INFINITE);   

    uint16_t     width  = g_format.width;
    uint16_t     height = g_format.height;
    uint8_t      fps    = g_format.framerate;
    VideoSubtype subtype;

    if (!ExtendedVideo_ParseSubtype(g_format.subtype, subtype)) { return; }
    ExtendedVideo_Open(g_options);
    if (!ExtendedVideo_Status()) { return; }
    if (ExtendedVideo_SetFormat(width, height, fps, subtype))
    {
    SetThreadPriority(GetCurrentThread(), ExtendedExecution_GetInterfacePriority(PORT_ID_EV));
    ExtendedVideo_ExecuteSensorLoop(MediaFrameReaderAcquisitionMode::Buffered, EV_Push, NULL, g_event_enable);
    SetThreadPriority(GetCurrentThread(), base_priority);
    }
    ExtendedVideo_Close();
    g_buffer.Clear();
}

// OK
static DWORD WINAPI EV_EntryPoint(void* param)
{
    (void)param;
    int base_priority = GetThreadPriority(GetCurrentThread());
    do { EV_Acquire(base_priority); } while (WaitForSingleObject(g_event_quit, 0) == WAIT_TIMEOUT);
    return 0;
}

// OK
void EV_SetFormat(ev_captureformat const& cf)
{
    g_options = cf.mrcvo;
    g_format  = cf.vf;
}

// OK
void EV_SetEnable(bool enable)
{
    (void)enable;
    SetEvent(g_event_enable);
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

    *buffer        = f->ref.data();
    *length        = (int32_t)f->ref.Capacity();
    *format_buffer = &f->format;
    *format_length = sizeof(ev_frame::format) / sizeof(ev_videoformat);
}

// OK
void EV_Initialize(int32_t buffer_size)
{
    g_buffer.Reset(buffer_size);
    g_event_quit = CreateEvent(NULL, TRUE, FALSE, NULL);
    g_event_enable = CreateEvent(NULL, FALSE, FALSE, NULL);
    g_thread = CreateThread(NULL, 0, EV_EntryPoint, NULL, 0, NULL);
}

// OK
void EV_Cleanup()
{
    SetEvent(g_event_quit);
    WaitForSingleObject(g_thread, INFINITE);
    CloseHandle(g_thread);
    CloseHandle(g_event_enable);
    CloseHandle(g_event_quit);
}

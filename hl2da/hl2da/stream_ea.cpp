
#include "extended_audio.h"
#include "frame_buffer.h"
#include "timestamps.h"
#include "log.h"

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.h>
#include <winrt/Windows.Media.MediaProperties.h>
#include <winrt/Windows.Media.Capture.h>
#include <winrt/Windows.Media.Capture.Frames.h>
#include <winrt/Windows.Media.Devices.Core.h>
#include <winrt/Windows.Foundation.Numerics.h>

using namespace winrt::Windows::Media;
using namespace winrt::Windows::Media::Capture;
using namespace winrt::Windows::Media::Capture::Frames;
using namespace winrt::Windows::Media::Devices::Core;
using namespace winrt::Windows::Media::MediaProperties;
using namespace winrt::Windows::Foundation::Numerics;

struct ea_audioformat
{
    uint32_t bitrate;
    uint32_t bits_per_sample;
    uint32_t channel_count;
    uint32_t sample_rate;
    wchar_t subtype[64];
};

class ea_frame : public sensor_frame
{
public:
    uint8_t* buffer;
    int32_t length;
    ea_audioformat format;

    ea_frame(uint8_t* b, int32_t l, AudioEncodingProperties const& aep);
    ~ea_frame();
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
static MRCAudioOptions g_options;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// OK
ea_frame::ea_frame(uint8_t* b, int32_t l, AudioEncodingProperties const& aep) : sensor_frame(), buffer(b), length(l)
{
    format.bitrate         = aep.Bitrate();
    format.bits_per_sample = aep.BitsPerSample();
    format.channel_count   = aep.ChannelCount();
    format.sample_rate     = aep.SampleRate();

    wcscpy_s(format.subtype, sizeof(ea_audioformat::subtype) / sizeof(wchar_t), aep.Subtype().c_str());
}

// OK
ea_frame::~ea_frame()
{
    delete[] buffer;
}

// OK
static void EA_OnAudioFrameArrived(MediaFrameReader const& sender, MediaFrameArrivedEventArgs const& args)
{
    (void)args;

    MediaFrameReference frame = nullptr;

    if (!g_reader_status) { return; }
    frame = sender.TryAcquireLatestFrame();
    if (!frame) { return; }

    int64_t         timestamp = frame.SystemRelativeTime().Value().count();
    AudioMediaFrame amf       = frame.AudioMediaFrame();
    auto            aep       = amf.AudioEncodingProperties();
    AudioFrame      audio     = amf.GetAudioFrame();
    AudioBuffer     buffer    = audio.LockBuffer(AudioBufferAccessMode::Read);
    auto            reference = buffer.CreateReference();
    uint8_t const*  src_addr  = reference.data();
    uint32_t        bytes     = reference.Capacity();
    uint8_t*        dst_addr  = new uint8_t[bytes]; // delete

    memcpy(dst_addr, src_addr, bytes);

    g_buffer.Insert(new ea_frame(dst_addr, bytes, aep), timestamp);

    reference.Close();
    buffer.Close();
    audio.Close();

    if (WaitForSingleObject(g_event_enable, 0) != WAIT_OBJECT_0) { SetEvent(g_event_client); }    
}

// OK
static void EA_Acquire()
{
    MediaFrameReader reader = nullptr;
    bool ok;

    WaitForSingleObject(g_event_enable, INFINITE);
    WaitForSingleObject(g_event_client, 0);

    ok = ExtendedAudio_Open(g_options);
    if (ok) 
    {
    reader = ExtendedAudio_CreateFrameReader(); 

    reader.AcquisitionMode(MediaFrameReaderAcquisitionMode::Buffered);
    reader.FrameArrived(EA_OnAudioFrameArrived);

    g_reader_status = true;
    reader.StartAsync().get();
    WaitForSingleObject(g_event_client, INFINITE);
    g_reader_status = false;
    reader.StopAsync().get();

    reader.Close();

    g_buffer.Clear();

    ExtendedAudio_Close();
    }

    ResetEvent(g_event_enable);
}

// OK
static DWORD WINAPI EA_EntryPoint(void* param)
{
    (void)param;
    ExtendedAudio_RegisterEvent(g_event_client);
    do { EA_Acquire(); } while (WaitForSingleObject(g_event_quit, 0) == WAIT_TIMEOUT);
    return 0;
}

// OK
void EA_SetFormat(MRCAudioOptions const& cf)
{
    g_options = cf;
}

// OK
void EA_SetEnable(bool enable)
{
    if (enable) { SetEvent(g_event_enable); } else { ResetEvent(g_event_enable); }
}

// OK
int EA_Get(int32_t stamp, void*& f, uint64_t& t, int32_t& s)
{
    return g_buffer.Get(stamp, (sensor_frame*&)f, t, s);
}

// OK
int EA_Get(uint64_t timestamp, int time_preference, bool tiebreak_right, void*& f, uint64_t& t, int32_t& s)
{
    return g_buffer.Get(timestamp, time_preference, tiebreak_right, (sensor_frame*&)f, t, s);
}

// OK
void EA_Release(void* frame)
{
    ((ea_frame*)frame)->Release();
}

// OK
void EA_Extract(void* frame, void const** buffer, int32_t* length, void const** format_buffer, int32_t* format_length)
{
    ea_frame* f = (ea_frame*)frame;

    *buffer = f->buffer;
    *length = (int32_t)f->length;
    *format_buffer = &f->format;
    *format_length = sizeof(ea_frame::format) / sizeof(ea_audioformat);
}

// OK
void EA_Initialize(int32_t buffer_size)
{
    g_buffer.Reset(buffer_size);
    g_event_client = CreateEvent(NULL, FALSE, FALSE, NULL);
    g_event_enable = CreateEvent(NULL, TRUE, FALSE, NULL);
    g_event_quit = CreateEvent(NULL, TRUE, FALSE, NULL);
    g_thread = CreateThread(NULL, 0, EA_EntryPoint, NULL, 0, NULL);
}

// OK
void EA_Quit()
{
    SetEvent(g_event_quit);
}

// OK
void EA_Cleanup()
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

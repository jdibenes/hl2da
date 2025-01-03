
#include <mfapi.h>
#include "stream_pv.h"
#include "extended_execution.h"
#include "personal_video.h"
#include "frame_buffer.h"
#include "server_ports.h"

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Capture.Frames.h>
#include <winrt/Windows.Media.Devices.Core.h>
#include <winrt/Windows.Graphics.Imaging.h>

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Numerics;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Media::Capture::Frames;
using namespace winrt::Windows::Media::Devices::Core;
using namespace winrt::Windows::Graphics::Imaging;

struct pv_metadata
{
    float4   intrinsics;
    uint64_t exposure_time;
    uint64x2 exposure_compensation;
    uint32_t lens_position;
    uint32_t focus_state;
    uint32_t iso_speed;
    uint32_t white_balance;
    float2   iso_gains;
    float3   white_balance_gains;
    float7   extrinsics_mf;
};

class pv_frame : public sensor_frame
{
public:
    SoftwareBitmap bmp = nullptr;
    BitmapBuffer buf = nullptr;
    IMemoryBufferReference ref;

    MediaFrameReference mfr;
    pv_metadata pv;
    float4x4 pose;

    pv_frame(MediaFrameReference const& f, pv_metadata const& m, float4x4 const& p);
    ~pv_frame();
};

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

static frame_buffer g_buffer;
static HANDLE g_event_enable = NULL; // CloseHandle
static HANDLE g_event_quit = NULL; // CloseHandle
static HANDLE g_thread = NULL; // CloseHandle
static pv_videoformat g_format;
static MRCVideoOptions g_options;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// OK
pv_frame::pv_frame(MediaFrameReference const& f, pv_metadata const& m, float4x4 const& p) : sensor_frame(), mfr(f), pv(m), pose(p)
{
    bmp = f.VideoMediaFrame().SoftwareBitmap();
    buf = bmp.LockBuffer(BitmapBufferAccessMode::Read);
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
static void PV_Push(MediaFrameReference const& frame, void* param)
{
    (void)param;

    int64_t          timestamp  = frame.SystemRelativeTime().Value().count();
    CameraIntrinsics intrinsics = frame.VideoMediaFrame().CameraIntrinsics();
    auto const&      metadata   = frame.Properties().Lookup(MFSampleExtension_CaptureMetadata).as<IMapView<winrt::guid, IInspectable>>();
    auto const&      extrinsics = frame.Properties().Lookup(MFSampleExtension_CameraExtrinsics).as<IReferenceArray<uint8_t>>().Value();

    float2 f = intrinsics.FocalLength();
    float2 c = intrinsics.PrincipalPoint();

    pv_metadata m;

    m.intrinsics            = float4(f.x, f.y, c.x, c.y);
    m.exposure_time         = metadata.Lookup(MF_CAPTURE_METADATA_EXPOSURE_TIME).as<uint64_t>();
    m.exposure_compensation = *(uint64x2*)metadata.Lookup(MF_CAPTURE_METADATA_EXPOSURE_COMPENSATION).as<IReferenceArray<uint8_t>>().Value().begin();
    m.iso_speed             = metadata.Lookup(MF_CAPTURE_METADATA_ISO_SPEED).as<uint32_t>();
    m.iso_gains             = *(float2*)metadata.Lookup(MF_CAPTURE_METADATA_ISO_GAINS).as<IReferenceArray<uint8_t>>().Value().begin();
    m.lens_position         = metadata.Lookup(MF_CAPTURE_METADATA_LENS_POSITION).as<uint32_t>();
    m.focus_state           = metadata.Lookup(MF_CAPTURE_METADATA_FOCUSSTATE).as<uint32_t>();
    m.white_balance         = metadata.Lookup(MF_CAPTURE_METADATA_WHITEBALANCE).as<uint32_t>();
    m.white_balance_gains   = *(float3*)metadata.Lookup(MF_CAPTURE_METADATA_WHITEBALANCE_GAINS).as<IReferenceArray<uint8_t>>().Value().begin();
    m.extrinsics_mf         = *(float7*)&((float*)extrinsics.begin())[5];

    float4x4 pose = PersonalVideo_GetFrameWorldPose(frame);

    g_buffer.Insert(new pv_frame(frame, m, pose), timestamp);
}

// OK
static void PV_Acquire(int base_priority)
{
    WaitForSingleObject(g_event_enable, INFINITE);

    uint16_t width  = g_format.width;
    uint16_t height = g_format.height;
    uint8_t  fps    = g_format.framerate;

    PersonalVideo_Open(g_options);
    if (!PersonalVideo_Status()) { return; }
    if (PersonalVideo_SetFormat(width, height, fps))
    {
    SetThreadPriority(GetCurrentThread(), ExtendedExecution_GetInterfacePriority(PORT_ID_PV));
    PersonalVideo_ExecuteSensorLoop(MediaFrameReaderAcquisitionMode::Buffered, PV_Push, NULL, g_event_enable);
    SetThreadPriority(GetCurrentThread(), base_priority);
    }
    PersonalVideo_Close();
    g_buffer.Clear();
}

// OK
static DWORD WINAPI PV_EntryPoint(void* param)
{
    (void)param;
    int base_priority = GetThreadPriority(GetCurrentThread());
    do { PV_Acquire(base_priority); } while (WaitForSingleObject(g_event_quit, 0) == WAIT_TIMEOUT);
    return 0;
}

// OK
void PV_SetFormat(pv_captureformat const& cf)
{
    g_options = cf.mrcvo;
    g_format  = cf.vf;
}

// OK
void PV_SetEnable(bool enable)
{
    (void)enable;
    SetEvent(g_event_enable);
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

    *buffer            = f->ref.data();
    *length            = (int32_t)f->ref.Capacity();
    *intrinsics_buffer = &f->pv;
    *intrinsics_length = sizeof(pv_frame::pv) / sizeof(pv_metadata);
    *pose_buffer       = &f->pose;
    *pose_length       = sizeof(pv_frame::pose) / sizeof(float);
}

// OK
void PV_Initialize(int32_t buffer_size)
{
    g_buffer.Reset(buffer_size);
    g_event_quit = CreateEvent(NULL, TRUE, FALSE, NULL);
    g_event_enable = CreateEvent(NULL, FALSE, FALSE, NULL);
    g_thread = CreateThread(NULL, 0, PV_EntryPoint, NULL, 0, NULL);
}

// OK
void PV_Cleanup()
{
    SetEvent(g_event_quit);
    WaitForSingleObject(g_thread, INFINITE);
    CloseHandle(g_thread);
    CloseHandle(g_event_enable);
    CloseHandle(g_event_quit);
}

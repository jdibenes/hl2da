
#include "../hl2da/locator.h"
#include "../hl2da/research_mode.h"
#include "../hl2da/personal_video.h"
#include "../hl2da/spatial_input.h"
#include "../hl2da/stream_rm.h"
#include "../hl2da/stream_pv.h"
#include "../hl2da/stream_mc.h"
#include "../hl2da/stream_si.h"
#include "../hl2da/stream_ee.h"
#include "../hl2da/stream_ea.h"
#include "../hl2da/stream_ev.h"
#include "../hl2da/log.h"

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.ApplicationModel.Core.h>

using namespace winrt::Windows::UI::Core;
using namespace winrt::Windows::ApplicationModel::Core;

#define PLUGIN_EXPORT extern "C" __declspec(dllexport)

// OK
PLUGIN_EXPORT
void Copy(void const* source, void* destination, int bytes)
{
    memcpy(destination, source, bytes);
}

// OK
PLUGIN_EXPORT
void DebugMessage(char const* str)
{
    ShowMessage("%s", str);
}

// OK
PLUGIN_EXPORT
void InitializeComponents()
{
    Locator_Initialize();
    ResearchMode_Initialize();
    PersonalVideo_Initialize();
    SpatialInput_Initialize();

    RM_InitializeDepthLock();
}

// OK
PLUGIN_EXPORT
void InitializeComponentsOnUI()
{
    HANDLE event_done = CreateEvent(NULL, TRUE, FALSE, NULL);
    CoreApplication::Views().GetAt(0).Dispatcher().RunAsync(CoreDispatcherPriority::High, [=]() { InitializeComponents(); SetEvent(event_done); }).get();
    WaitForSingleObject(event_done, INFINITE);
    CloseHandle(event_done);
}

// OK
PLUGIN_EXPORT
int OverrideWorldCoordinateSystem(void* scs_ptr)
{
    winrt::Windows::Perception::Spatial::SpatialCoordinateSystem scs = nullptr;
    if (scs_ptr)
    {
    winrt::copy_from_abi(scs, scs_ptr);
    scs = Locator_SanitizeSpatialCoordinateSystem(scs);
    if (!scs) { return false; }
    }
    Locator_OverrideWorldCoordinateSystem(scs);
    return true;
}

// OK
PLUGIN_EXPORT
void Initialize(int id, int buffer_size)
{
    switch (id)
    {
    case  0:
    case  1:
    case  2:
    case  3:
    case  4:
    case  5:
    case  6:
    case  7:
    case  8: RM_Initialize(id, buffer_size); break;
    case  9: PV_Initialize(    buffer_size); break;
    case 10: MC_Initialize(    buffer_size); break;
    case 11: SI_Initialize(    buffer_size); break;
    case 12: EE_Initialize(    buffer_size); break;
    case 13: EA_Initialize(    buffer_size); break;
    case 14: EV_Initialize(    buffer_size); break;
    }
}

// OK
PLUGIN_EXPORT
void SetEnable(int id, int enable)
{
    switch (id)
    {
    case  0:
    case  1:
    case  2:
    case  3:
    case  4:
    case  5:
    case  6:
    case  7:
    case  8: RM_SetEnable(id, enable != 0); break;
    case  9: PV_SetEnable(    enable != 0); break;
    case 10: MC_SetEnable(    enable != 0); break;
    case 11: SI_SetEnable(    enable != 0); break;
    case 12: EE_SetEnable(    enable != 0); break;
    case 13: EA_SetEnable(    enable != 0); break;
    case 14: EV_SetEnable(    enable != 0); break;
    }
}

// OK
PLUGIN_EXPORT
int GetByFramestamp(int id, int32_t stamp, void** frame, uint64_t* timestamp, int32_t* framestamp)
{
    switch (id)
    {
    case  0:
    case  1:
    case  2:
    case  3:
    case  4:
    case  5:
    case  6:
    case  7:
    case  8: return RM_Get(id, stamp, *frame, *timestamp, *framestamp);
    case  9: return PV_Get(    stamp, *frame, *timestamp, *framestamp);
    case 10: return MC_Get(    stamp, *frame, *timestamp, *framestamp);
    case 11: return SI_Get(    stamp, *frame, *timestamp, *framestamp);
    case 12: return EE_Get(    stamp, *frame, *timestamp, *framestamp);
    case 13: return EA_Get(    stamp, *frame, *timestamp, *framestamp);
    case 14: return EV_Get(    stamp, *frame, *timestamp, *framestamp);
    }

    return -1;
}

// OK
PLUGIN_EXPORT
int GetByTimestamp(int id, uint64_t stamp, int time_preference, int tiebreak_right, void** frame, uint64_t* timestamp, int32_t* framestamp)
{
    switch (id)
    {
    case  0:
    case  1:
    case  2:
    case  3:
    case  4:
    case  5:
    case  6:
    case  7:
    case  8: return RM_Get(id, stamp, time_preference, tiebreak_right != 0, *frame, *timestamp, *framestamp);
    case  9: return PV_Get(    stamp, time_preference, tiebreak_right != 0, *frame, *timestamp, *framestamp);
    case 10: return MC_Get(    stamp, time_preference, tiebreak_right != 0, *frame, *timestamp, *framestamp);
    case 11: return SI_Get(    stamp, time_preference, tiebreak_right != 0, *frame, *timestamp, *framestamp);
    case 12: return EE_Get(    stamp, time_preference, tiebreak_right != 0, *frame, *timestamp, *framestamp);
    case 13: return EA_Get(    stamp, time_preference, tiebreak_right != 0, *frame, *timestamp, *framestamp);
    case 14: return EV_Get(    stamp, time_preference, tiebreak_right != 0, *frame, *timestamp, *framestamp);
    }

    return -1;
}

// OK
PLUGIN_EXPORT
void Release(int id, void* frame)
{
    switch (id)
    {
    case  0:
    case  1:
    case  2:
    case  3:
    case  4:
    case  5:
    case  6:
    case  7:
    case  8: RM_Release(frame); break;
    case  9: PV_Release(frame); break;
    case 10: MC_Release(frame); break;
    case 11: SI_Release(frame); break;
    case 12: EE_Release(frame); break;
    case 13: EA_Release(frame); break;
    case 14: EV_Release(frame); break;
    }
}

// OK
PLUGIN_EXPORT
void Extract(int id, void* frame, int32_t* valid, void const** b, int32_t* l)
{
    switch (id)
    {
    case  0:
    case  1:
    case  2:
    case  3: RM_Extract_VLC(              frame,        b + 0, l + 0,                             b + 3, l + 3); break;
    case  4: RM_Extract_Depth_AHAT(       frame,        b + 0, l + 0, b + 1, l + 1,               b + 3, l + 3); break;
    case  5: RM_Extract_Depth_Longthrow(  frame,        b + 0, l + 0, b + 1, l + 1, b + 2, l + 2, b + 3, l + 3); break;
    case  6: RM_Extract_IMU_Accelerometer(frame,        b + 0, l + 0,                             b + 3, l + 3); break;
    case  7: RM_Extract_IMU_Gyroscope(    frame,        b + 0, l + 0,                             b + 3, l + 3); break;
    case  8: RM_Extract_IMU_Magnetometer( frame,        b + 0, l + 0,                             b + 3, l + 3); break;
    case  9: PV_Extract(                  frame,        b + 0, l + 0,               b + 2, l + 2, b + 3, l + 3); break;
    case 10: MC_Extract(                  frame,        b + 0, l + 0                                          ); break;
    case 11: SI_Extract(                  frame, valid, b + 0, l + 0, b + 1, l + 1, b + 2, l + 2, b + 3, l + 3); break;
    case 12: EE_Extract(                  frame, valid, b + 0, l + 0,                             b + 3, l + 3); break;
    case 13: EA_Extract(                  frame,        b + 0, l + 0,               b + 2, l + 2              ); break;
    case 14: EV_Extract(                  frame,        b + 0, l + 0,               b + 2, l + 2              ); break;
    }
}

// OK
PLUGIN_EXPORT
void GetExtrinsics_RM(int id, float* out)
{
    switch (id)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7: RM_GetExtrinsics(id, out); break;
    }
}

// OK
PLUGIN_EXPORT
void MapImagePointToCameraUnitPlane_RM(int id, float const* in, float* out, int point_count)
{
    switch (id)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5: RM_MapImagePointToCameraUnitPlane(id, in, out, point_count); break;
    }    
}

// OK
PLUGIN_EXPORT
void MapCameraSpaceToImagePoint_RM(int id, float const* in, float* out, int point_count)
{
    switch (id)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5: RM_MapCameraSpaceToImagePoint(id, in, out, point_count); break;
    }    
}

// OK
PLUGIN_EXPORT
void BypassDepthLock_RM(int bypass)
{
    RM_BypassDepthLock(bypass != 0);
}

// OK
PLUGIN_EXPORT
void SetFormat_PV(void const* cf)
{
    PV_SetFormat(*(pv_captureformat*)cf);
}

// OK
PLUGIN_EXPORT
void SetFormat_MC(int raw)
{
    MC_SetFormat(raw != 0);
}

// OK
PLUGIN_EXPORT
void SetFormat_EE(int fps_index)
{
    EE_SetFormat(fps_index);
}

// OK
PLUGIN_EXPORT
void SetFormat_EA(void const* cf)
{
    EA_SetFormat(*(MRCAudioOptions*)cf);
}

// OK
PLUGIN_EXPORT
void SetFormat_EV(uint32_t width, uint32_t height, uint32_t framerate, wchar_t const* subtype, int32_t shared, uint32_t group_index, uint32_t source_index, uint32_t profile_index)
{
    ev_captureformat cf;

    cf.vf.width     = (uint16_t)width;
    cf.vf.height    = (uint16_t)height;
    cf.vf.framerate = (uint8_t)framerate;

    wcscpy_s(cf.vf.subtype, sizeof(ev_videoformat::subtype) / sizeof(wchar_t), subtype);

    cf.mrcvo.shared         = shared != 0;
    cf.mrcvo.global_opacity = (float)group_index;
    cf.mrcvo.output_width   = (float)source_index;
    cf.mrcvo.output_height  = (float)profile_index;

    EV_SetFormat(cf);
}

// OK
PLUGIN_EXPORT
void PV_SetFocus(uint32_t focusmode, uint32_t autofocusrange, uint32_t distance, uint32_t value, uint32_t disabledriverfallback)
{
    PersonalVideo_SetFocus(focusmode, autofocusrange, distance, value, disabledriverfallback);
}

// OK
PLUGIN_EXPORT
void PV_SetVideoTemporalDenoising(uint32_t mode)
{
    PersonalVideo_SetVideoTemporalDenoising(mode);
}

// OK
PLUGIN_EXPORT
void PV_SetWhiteBalance_Preset(uint32_t preset)
{
    PersonalVideo_SetWhiteBalance_Preset(preset);
}

// OK
PLUGIN_EXPORT
void PV_SetWhiteBalance_Value(uint32_t value)
{
    PersonalVideo_SetWhiteBalance_Value(value);
}

// OK
PLUGIN_EXPORT
void PV_SetExposure(uint32_t setauto, uint32_t value)
{
    PersonalVideo_SetExposure(setauto, value);
}

// OK
PLUGIN_EXPORT
void PV_SetExposurePriorityVideo(uint32_t enabled)
{
    PersonalVideo_SetExposurePriorityVideo(enabled);
}

// OK
PLUGIN_EXPORT
void PV_SetSceneMode(uint32_t mode)
{
    PersonalVideo_SetSceneMode(mode);
}

// OK
PLUGIN_EXPORT
void PV_SetIsoSpeed(uint32_t setauto, uint32_t value)
{
    PersonalVideo_SetIsoSpeed(setauto, value);
}

// OK
PLUGIN_EXPORT
void PV_SetBacklightCompensation(uint32_t enable)
{
    PersonalVideo_SetBacklightCompensation(enable != 0);
}

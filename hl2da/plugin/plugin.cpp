
#include "../hl2da/locator.h"
#include "../hl2da/research_mode.h"
#include "../hl2da/personal_video.h"
#include "../hl2da/stream_rm.h"
#include "../hl2da/stream_pv.h"
#include "../hl2da/stream_mc.h"
#include "../hl2da/stream_si.h"
#include "../hl2da/log.h"

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.ApplicationModel.Core.h>

using namespace winrt::Windows::UI::Core;
using namespace winrt::Windows::ApplicationModel::Core;

#define PLUGIN_EXPORT extern "C" __declspec(dllexport)

PLUGIN_EXPORT
void Copy(void const* source, void* destination, int bytes)
{
    memcpy(destination, source, bytes);
}

PLUGIN_EXPORT
void DebugMessage(char const* str)
{
    ShowMessage("%s", str);
}

PLUGIN_EXPORT
void InitializeComponents()
{
    Locator_Initialize();
    ResearchMode_Initialize();
    PersonalVideo_Initialize();
    SpatialInput_Initialize();

    RM_InitializeDepthLock();
}

PLUGIN_EXPORT
void InitializeGlobal()
{
    HANDLE event_done = CreateEvent(NULL, TRUE, FALSE, NULL);
    CoreApplication::Views().GetAt(0).Dispatcher().RunAsync(CoreDispatcherPriority::High, [=]() { InitializeComponents(); SetEvent(event_done); }).get();
    WaitForSingleObject(event_done, INFINITE);
    CloseHandle(event_done);
}

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
    case 12: //
    case 13: //
    case 14: //
        break;
    }
}

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
    case 12: //
    case 13: //
    case 14: //
        break;
    }
}

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
    case  8: return RM_Get(id, stamp, *(rm_frame**)frame, *timestamp, *framestamp);
    case  9: return PV_Get(    stamp, *(pv_frame**)frame, *timestamp, *framestamp);
    case 10: return MC_Get(    stamp, *(mc_frame**)frame, *timestamp, *framestamp);
    case 11: return SI_Get(    stamp, *(si_frame**)frame, *timestamp, *framestamp);
    case 12: //
    case 13: //
    case 14: //
        break;
    }

    return -1;
}

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
    case  8: return RM_Get(id, stamp, time_preference, tiebreak_right != 0, *(rm_frame**)frame, *timestamp, *framestamp);
    case  9: return PV_Get(    stamp, time_preference, tiebreak_right != 0, *(pv_frame**)frame, *timestamp, *framestamp);
    case 10: return MC_Get(    stamp, time_preference, tiebreak_right != 0, *(mc_frame**)frame, *timestamp, *framestamp);
    case 11: return SI_Get(    stamp, time_preference, tiebreak_right != 0, *(si_frame**)frame, *timestamp, *framestamp);
    case 12: //
    case 13: //
    case 14: //
        break;
    }

    return -1;
}

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
    case  8: ((rm_frame*)frame)->Release(); break;
    case  9: ((pv_frame*)frame)->Release(); break;
    case 10: ((mc_frame*)frame)->Release(); break;
    case 11: ((si_frame*)frame)->Release(); break;
    case 12: //
    case 13: //
    case 14: //
        break;
    }    
}

PLUGIN_EXPORT
void Extract_RM_VLC(void* frame, void const** buffer, int32_t* length, void** pose_buffer, int32_t* pose_length)
{
    rm_frame* f = (rm_frame*)frame;
    rm_data_vlc d;
    RM_Extract(f->rmsf, d);
    *buffer = d.buffer;
    *length = (int32_t)d.length;
    *pose_buffer = &(f->pose);
    *pose_length = sizeof(rm_frame::pose) / sizeof(float);
}

PLUGIN_EXPORT
void Extract_RM_Depth_AHAT(void* frame, void const** buffer, int32_t* length, void const** ab_depth_buffer, int32_t* ab_depth_length, void** pose_buffer, int32_t* pose_length)
{
    rm_frame* f = (rm_frame*)frame;
    rm_data_zht d;
    RM_Extract(f->rmsf, d);
    *buffer = d.buffer;
    *length = (int32_t)d.length;
    *ab_depth_buffer = d.ab_depth_buffer;
    *ab_depth_length = (int32_t)d.ab_depth_length;
    *pose_buffer = &f->pose;
    *pose_length = sizeof(rm_frame::pose) / sizeof(float);
}

PLUGIN_EXPORT
void Extract_RM_Depth_Longthrow(void* frame, void const** buffer, int32_t* length, void const** ab_depth_buffer, int32_t* ab_depth_length, void const** sigma_buffer, int32_t* sigma_length, void** pose_buffer, int32_t* pose_length)
{
    rm_frame* f = (rm_frame*)frame;
    rm_data_zlt d;
    RM_Extract(f->rmsf, d);
    *buffer = d.buffer;
    *length = (int32_t)d.length;
    *ab_depth_buffer = d.ab_depth_buffer;
    *ab_depth_length = (int32_t)d.ab_depth_length;
    *sigma_buffer = d.sigma_buffer;
    *sigma_length = (int32_t)d.sigma_length;
    *pose_buffer = &f->pose;
    *pose_length = sizeof(rm_frame::pose) / sizeof(float);
}

PLUGIN_EXPORT
void Extract_RM_IMU_Accelerometer(void* frame, void const** buffer, int32_t* length, void** pose_buffer, int32_t* pose_length)
{
    rm_frame* f = (rm_frame*)frame;
    rm_data_acc d;
    RM_Extract(f->rmsf, d);
    *buffer = d.buffer;
    *length = (int32_t)d.length;
    *pose_buffer = &f->pose;
    *pose_length = sizeof(rm_frame::pose) / sizeof(float);
}

PLUGIN_EXPORT
void Extract_RM_IMU_Gyroscope(void* frame, void const** buffer, int32_t* length, void** pose_buffer, int32_t* pose_length)
{
    rm_frame* f = (rm_frame*)frame;
    rm_data_gyr d;
    RM_Extract(f->rmsf, d);
    *buffer = d.buffer;
    *length = (int32_t)d.length;
    *pose_buffer = &f->pose;
    *pose_length = sizeof(rm_frame::pose) / sizeof(float);
}

PLUGIN_EXPORT
void Extract_RM_IMU_Magnetometer(void* frame, void const** buffer, int32_t* length, void** pose_buffer, int32_t* pose_length)
{
    rm_frame* f = (rm_frame*)frame;
    rm_data_mag d;
    RM_Extract(f->rmsf, d);
    *buffer = d.buffer;
    *length = (int32_t)d.length;
    *pose_buffer = &f->pose;
    *pose_length = sizeof(rm_frame::pose) / sizeof(float);
}

PLUGIN_EXPORT
void Extract_PV(void* frame, void const** buffer, int32_t* length, void const** intrinsics_buffer, int32_t* intrinsics_length, void const** pose_buffer, int32_t* pose_length)
{
    pv_frame* f = (pv_frame*)frame;
    pv_data d;
    PV_Extract(f->mfr, d);
    *buffer = d.buffer;
    *length = (int32_t)d.length;
    *intrinsics_buffer = &f->intrinsics;
    *intrinsics_length = sizeof(pv_frame::intrinsics) / sizeof(float);
    *pose_buffer = &f->pose;
    *pose_length = sizeof(pv_frame::pose) / sizeof(float);
}

PLUGIN_EXPORT
void Extract_MC(void* frame, void const** buffer, int32_t* length)
{
    mc_frame* f = (mc_frame*)frame;
    *buffer = f->buffer;
    *length = (int32_t)f->length;
}

PLUGIN_EXPORT
void Extract_SI(void* frame, int32_t* valid, void const** head_buffer, int32_t* head_length, void const** eye_buffer, int32_t* eye_length, void const** left_buffer, int32_t* left_length, void const** right_buffer, int32_t* right_length)
{
    si_frame* f = (si_frame*)frame;
    *valid = f->valid;
    *head_buffer = &f->head_pose;
    *head_length = sizeof(si_frame::head_pose) / sizeof(float);
    *eye_buffer = &f->eye_ray;
    *eye_length = sizeof(si_frame::eye_ray) / sizeof(float);
    *left_buffer = f->left_hand;
    *left_length = HAND_JOINTS;
    *right_buffer = f->right_hand;
    *right_length = HAND_JOINTS;
}

PLUGIN_EXPORT
void GetExtrinsics_RM(int id, float* out)
{
    RM_GetExtrinsics(id, out);
}

PLUGIN_EXPORT
void MapImagePointToCameraUnitPlane_RM(int id, float const* in, float* out, int point_count)
{
    RM_MapImagePointToCameraUnitPlane(id, in, out, point_count);
}

PLUGIN_EXPORT
void MapCameraSpaceToImagePoint_RM(int id, float const* in, float* out, int point_count)
{
    RM_MapCameraSpaceToImagePoint(id, in, out, point_count);
}

PLUGIN_EXPORT
void SetFormat_PV(pv_captureformat const* cf)
{
    PV_SetFormat(*cf);
}

PLUGIN_EXPORT
void SetFormat_MC(int raw)
{
    MC_SetFormat(raw != 0);
}

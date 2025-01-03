// Fill out your copyright notice in the Description page of Project Settings.


#include "hl2da_api.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdint.h>

typedef void (*pfn_DebugMessage)(char const* str);
typedef void (*pfn_InitializeComponents)();
typedef int (*pfn_OverrideWorldCoordinateSystem)(void* scs_ptr);
typedef uint64_t(*pfn_GetUTCOffset)(int32_t samples);
typedef void (*pfn_Initialize)(int id, int buffer_size);
typedef void (*pfn_SetConstantFactorVLC_RM)(int64_t factor);
typedef void (*pfn_BypassDepthLock_RM)(int bypass);
typedef void (*pfn_SetFormat_PV)(hl2da_api::pv_captureformat const* cf);
typedef void (*pfn_SetFormat_MC)(int raw);
typedef void (*pfn_SetFormat_EE)(int fps_index);
typedef void (*pfn_SetFormat_EA)(hl2da_api::ea_captureformat const* cf);
typedef void (*pfn_SetFormat_EV)(hl2da_api::ev_captureformat const* cf);
typedef void (*pfn_SetEnable)(int id, int enable);
typedef int (*pfn_GetByFramestamp)(int id, int32_t stamp, void** frame, uint64_t* timestamp, int32_t* framestamp);
typedef int (*pfn_GetByTimestamp)(int id, uint64_t stamp, int time_preference, int tiebreak_right, void** frame, uint64_t* timestamp, int32_t* framestamp);
typedef void (*pfn_Extract)(int id, void* frame, int32_t* valid, void const** b, int32_t* l);
typedef void (*pfn_Release)(int id, void* frame);
typedef void (*pfn_RM_SetEyeSelection)(uint32_t enable);
typedef void (*pfn_RM_GetIntrinsics)(int id, float* uv2xy, float* mapxy, float* k);
typedef void (*pfn_RM_GetExtrinsics)(int id, float* out);
typedef void (*pfn_RM_MapImagePointToCameraUnitPlane)(int id, float const* in, int in_pitch, float* out, int out_pitch, int point_count);
typedef void (*pfn_RM_MapCameraSpaceToImagePoint)(int id, float const* in, int in_pitch, float* out, int out_pitch, int point_count);
typedef void (*pfn_PV_SetFocus)(uint32_t focusmode, uint32_t autofocusrange, uint32_t distance, uint32_t value, uint32_t disabledriverfallback);
typedef void (*pfn_PV_SetVideoTemporalDenoising)(uint32_t mode);
typedef void (*pfn_PV_SetWhiteBalance_Preset)(uint32_t preset);
typedef void (*pfn_PV_SetWhiteBalance_Value)(uint32_t value);
typedef void (*pfn_PV_SetExposure)(uint32_t setauto, uint32_t value);
typedef void (*pfn_PV_SetExposurePriorityVideo)(uint32_t enabled);
typedef void (*pfn_PV_SetSceneMode)(uint32_t mode);
typedef void (*pfn_PV_SetIsoSpeed)(uint32_t setauto, uint32_t value);
typedef void (*pfn_PV_SetBacklightCompensation)(uint32_t enable);
typedef void (*pfn_PV_SetDesiredOptimization)(uint32_t mode);
typedef void (*pfn_PV_SetPrimaryUse)(uint32_t mode);
typedef void (*pfn_PV_SetOpticalImageStabilization)(uint32_t mode);
typedef void (*pfn_PV_SetHdrVideo)(uint32_t mode);
typedef void (*pfn_PV_SetRegionsOfInterest)(uint32_t clear, uint32_t set, uint32_t auto_exposure, uint32_t auto_focus, uint32_t bounds_normalized, float x, float y, float w, float h, uint32_t type, uint32_t weight);
typedef void (*pfn_EX_Request)();
typedef uint32_t (*pfn_EX_Status)();
typedef void (*pfn_EX_SetInterfacePriority)(uint32_t id, int32_t priority);
typedef int32_t (*pfn_EX_GetInterfacePriority)(uint32_t id);
typedef void (*pfn_IMT_ZHTInvalidate)(uint16_t const* depth_in, uint16_t* depth_out);
typedef void (*pfn_IMT_ZLTInvalidate)(uint8_t const* sigma_in, uint16_t const* depth_in, uint16_t* depth_out);
typedef void (*pfn_IMT_YUV2RGB)(uint8_t const* image, uint32_t stride, uint32_t height, uint32_t format_in, uint32_t format_out, void** fc);
typedef void (*pfn_IMT_Extract)(void* fc, void const** buffer, int32_t* length);
typedef void (*pfn_IMT_Release)(void* fc);

#if PLATFORM_HOLOLENS
extern "C" { HMODULE LoadLibraryW(LPCWSTR lpLibFileName); }
#endif

void* hl2da_api::hmod_mmret;
void* hl2da_api::hmod_hl2da;

void* hl2da_api::p_DebugMessage;
void* hl2da_api::p_InitializeComponents;
void* hl2da_api::p_OverrideWorldCoordinateSystem;
void* hl2da_api::p_GetUTCOffset;
void* hl2da_api::p_Initialize;
void* hl2da_api::p_SetConstantFactorVLC_RM;
void* hl2da_api::p_BypassDepthLock_RM;
void* hl2da_api::p_SetFormat_PV;
void* hl2da_api::p_SetFormat_MC;
void* hl2da_api::p_SetFormat_EE;
void* hl2da_api::p_SetFormat_EA;
void* hl2da_api::p_SetFormat_EV;
void* hl2da_api::p_SetEnable;
void* hl2da_api::p_GetByFramestamp;
void* hl2da_api::p_GetByTimestamp;
void* hl2da_api::p_Extract;
void* hl2da_api::p_Release;
void* hl2da_api::p_RM_SetEyeSelection;
void* hl2da_api::p_RM_GetIntrinsics;
void* hl2da_api::p_RM_GetExtrinsics;
void* hl2da_api::p_RM_MapImagePointToCameraUnitPlane;
void* hl2da_api::p_RM_MapCameraSpaceToImagePoint;
void* hl2da_api::p_PV_SetFocus;
void* hl2da_api::p_PV_SetVideoTemporalDenoising;
void* hl2da_api::p_PV_SetWhiteBalance_Preset;
void* hl2da_api::p_PV_SetWhiteBalance_Value;
void* hl2da_api::p_PV_SetExposure;
void* hl2da_api::p_PV_SetExposurePriorityVideo;
void* hl2da_api::p_PV_SetSceneMode;
void* hl2da_api::p_PV_SetIsoSpeed;
void* hl2da_api::p_PV_SetBacklightCompensation;
void* hl2da_api::p_PV_SetDesiredOptimization;
void* hl2da_api::p_PV_SetPrimaryUse;
void* hl2da_api::p_PV_SetOpticalImageStabilization;
void* hl2da_api::p_PV_SetHdrVideo;
void* hl2da_api::p_PV_SetRegionsOfInterest;
void* hl2da_api::p_EX_Request;
void* hl2da_api::p_EX_Status;
void* hl2da_api::p_EX_SetInterfacePriority;
void* hl2da_api::p_EX_GetInterfacePriority;
void* hl2da_api::p_IMT_ZHTInvalidate;
void* hl2da_api::p_IMT_ZLTInvalidate;
void* hl2da_api::p_IMT_YUV2RGB;
void* hl2da_api::p_IMT_Extract;
void* hl2da_api::p_IMT_Release;

int hl2da_api::Load()
{
    FString path_base = FPaths::ProjectPluginsDir() + L"hl2da/Binaries/hl2da/";

    path_base.ReplaceCharInline(L'/', L'\\');

    FString path_dll_mmret = path_base + L"Microsoft.MixedReality.EyeTracking.dll";
    FString path_dll_hl2ss = path_base + L"hl2da.dll";

    if (!FPaths::FileExists(path_dll_mmret)) { return -__LINE__; }
    if (!FPaths::FileExists(path_dll_hl2ss)) { return -__LINE__; }

    hmod_mmret = LoadLibraryW(*path_dll_mmret);
    hmod_hl2da = LoadLibraryW(*path_dll_hl2ss);

    if (hmod_mmret == NULL) { return -__LINE__; }
    if (hmod_hl2da == NULL) { return -__LINE__; }

    p_DebugMessage                      = GetProcAddress((HMODULE)hmod_hl2da, "DebugMessage");
    p_InitializeComponents              = GetProcAddress((HMODULE)hmod_hl2da, "InitializeComponents");
    p_OverrideWorldCoordinateSystem     = GetProcAddress((HMODULE)hmod_hl2da, "OverrideWorldCoordinateSystem");
    p_GetUTCOffset                      = GetProcAddress((HMODULE)hmod_hl2da, "GetUTCOffset");
    p_Initialize                        = GetProcAddress((HMODULE)hmod_hl2da, "Initialize");
    p_SetConstantFactorVLC_RM           = GetProcAddress((HMODULE)hmod_hl2da, "SetConstantFactorVLC_RM");
    p_BypassDepthLock_RM                = GetProcAddress((HMODULE)hmod_hl2da, "BypassDepthLock_RM");
    p_SetFormat_PV                      = GetProcAddress((HMODULE)hmod_hl2da, "SetFormat_PV");
    p_SetFormat_MC                      = GetProcAddress((HMODULE)hmod_hl2da, "SetFormat_MC");
    p_SetFormat_EE                      = GetProcAddress((HMODULE)hmod_hl2da, "SetFormat_EE");
    p_SetFormat_EA                      = GetProcAddress((HMODULE)hmod_hl2da, "SetFormat_EA");
    p_SetFormat_EV                      = GetProcAddress((HMODULE)hmod_hl2da, "SetFormat_EV");
    p_SetEnable                         = GetProcAddress((HMODULE)hmod_hl2da, "SetEnable");
    p_GetByFramestamp                   = GetProcAddress((HMODULE)hmod_hl2da, "GetByFramestamp");
    p_GetByTimestamp                    = GetProcAddress((HMODULE)hmod_hl2da, "GetByTimestamp");
    p_Extract                           = GetProcAddress((HMODULE)hmod_hl2da, "Extract");
    p_Release                           = GetProcAddress((HMODULE)hmod_hl2da, "Release");
    p_RM_SetEyeSelection                = GetProcAddress((HMODULE)hmod_hl2da, "RM_SetEyeSelection");
    p_RM_GetIntrinsics                  = GetProcAddress((HMODULE)hmod_hl2da, "RM_GetIntrinsics");
    p_RM_GetExtrinsics                  = GetProcAddress((HMODULE)hmod_hl2da, "RM_GetExtrinsics");
    p_RM_MapImagePointToCameraUnitPlane = GetProcAddress((HMODULE)hmod_hl2da, "RM_MapImagePointToCameraUnitPlane");
    p_RM_MapCameraSpaceToImagePoint     = GetProcAddress((HMODULE)hmod_hl2da, "RM_MapCameraSpaceToImagePoint");
    p_PV_SetFocus                       = GetProcAddress((HMODULE)hmod_hl2da, "PV_SetFocus");
    p_PV_SetVideoTemporalDenoising      = GetProcAddress((HMODULE)hmod_hl2da, "PV_SetVideoTemporalDenoising");
    p_PV_SetWhiteBalance_Preset         = GetProcAddress((HMODULE)hmod_hl2da, "PV_SetWhiteBalance_Preset");
    p_PV_SetWhiteBalance_Value          = GetProcAddress((HMODULE)hmod_hl2da, "PV_SetWhiteBalance_Value");
    p_PV_SetExposure                    = GetProcAddress((HMODULE)hmod_hl2da, "PV_SetExposure");
    p_PV_SetExposurePriorityVideo       = GetProcAddress((HMODULE)hmod_hl2da, "PV_SetExposurePriorityVideo");
    p_PV_SetSceneMode                   = GetProcAddress((HMODULE)hmod_hl2da, "PV_SetSceneMode");
    p_PV_SetIsoSpeed                    = GetProcAddress((HMODULE)hmod_hl2da, "PV_SetIsoSpeed");
    p_PV_SetBacklightCompensation       = GetProcAddress((HMODULE)hmod_hl2da, "PV_SetBacklightCompensation");
    p_PV_SetDesiredOptimization         = GetProcAddress((HMODULE)hmod_hl2da, "PV_SetDesiredOptimization");    
    p_PV_SetPrimaryUse                  = GetProcAddress((HMODULE)hmod_hl2da, "PV_SetPrimaryUse");
    p_PV_SetOpticalImageStabilization   = GetProcAddress((HMODULE)hmod_hl2da, "PV_SetOpticalImageStabilization");
    p_PV_SetHdrVideo                    = GetProcAddress((HMODULE)hmod_hl2da, "PV_SetHdrVideo");
    p_PV_SetRegionsOfInterest           = GetProcAddress((HMODULE)hmod_hl2da, "PV_SetRegionsOfInterest");
    p_EX_Request                        = GetProcAddress((HMODULE)hmod_hl2da, "EX_Request");
    p_EX_Status                         = GetProcAddress((HMODULE)hmod_hl2da, "EX_Status");
    p_EX_SetInterfacePriority           = GetProcAddress((HMODULE)hmod_hl2da, "EX_SetInterfacePriority");
    p_EX_GetInterfacePriority           = GetProcAddress((HMODULE)hmod_hl2da, "EX_GetInterfacePriority");
    p_IMT_ZHTInvalidate                 = GetProcAddress((HMODULE)hmod_hl2da, "IMT_ZHTInvalidate");
    p_IMT_ZLTInvalidate                 = GetProcAddress((HMODULE)hmod_hl2da, "IMT_ZLTInvalidate");
    p_IMT_YUV2RGB                       = GetProcAddress((HMODULE)hmod_hl2da, "IMT_YUV2RGB");
    p_IMT_Extract                       = GetProcAddress((HMODULE)hmod_hl2da, "IMT_Extract");
    p_IMT_Release                       = GetProcAddress((HMODULE)hmod_hl2da, "IMT_Release");

    if (p_DebugMessage                      == NULL) { return -__LINE__; }
    if (p_InitializeComponents              == NULL) { return -__LINE__; }
    if (p_OverrideWorldCoordinateSystem     == NULL) { return -__LINE__; }
    if (p_GetUTCOffset                      == NULL) { return -__LINE__; }
    if (p_Initialize                        == NULL) { return -__LINE__; }
    if (p_SetConstantFactorVLC_RM           == NULL) { return -__LINE__; }
    if (p_BypassDepthLock_RM                == NULL) { return -__LINE__; }
    if (p_SetFormat_PV                      == NULL) { return -__LINE__; }
    if (p_SetFormat_MC                      == NULL) { return -__LINE__; }
    if (p_SetFormat_EE                      == NULL) { return -__LINE__; }
    if (p_SetFormat_EA                      == NULL) { return -__LINE__; }
    if (p_SetFormat_EV                      == NULL) { return -__LINE__; }
    if (p_SetEnable                         == NULL) { return -__LINE__; }
    if (p_GetByFramestamp                   == NULL) { return -__LINE__; }
    if (p_GetByTimestamp                    == NULL) { return -__LINE__; }
    if (p_Extract                           == NULL) { return -__LINE__; }
    if (p_Release                           == NULL) { return -__LINE__; }
    if (p_RM_SetEyeSelection                == NULL) { return -__LINE__; }
    if (p_RM_GetIntrinsics                  == NULL) { return -__LINE__; }
    if (p_RM_GetExtrinsics                  == NULL) { return -__LINE__; }
    if (p_RM_MapImagePointToCameraUnitPlane == NULL) { return -__LINE__; }
    if (p_RM_MapCameraSpaceToImagePoint     == NULL) { return -__LINE__; }
    if (p_PV_SetFocus                       == NULL) { return -__LINE__; }
    if (p_PV_SetVideoTemporalDenoising      == NULL) { return -__LINE__; }
    if (p_PV_SetWhiteBalance_Preset         == NULL) { return -__LINE__; }
    if (p_PV_SetWhiteBalance_Value          == NULL) { return -__LINE__; }
    if (p_PV_SetExposure                    == NULL) { return -__LINE__; }
    if (p_PV_SetExposurePriorityVideo       == NULL) { return -__LINE__; }
    if (p_PV_SetSceneMode                   == NULL) { return -__LINE__; }
    if (p_PV_SetIsoSpeed                    == NULL) { return -__LINE__; }
    if (p_PV_SetBacklightCompensation       == NULL) { return -__LINE__; }
    if (p_PV_SetDesiredOptimization         == NULL) { return -__LINE__; }
    if (p_PV_SetPrimaryUse                  == NULL) { return -__LINE__; }
    if (p_PV_SetOpticalImageStabilization   == NULL) { return -__LINE__; }
    if (p_PV_SetHdrVideo                    == NULL) { return -__LINE__; }
    if (p_PV_SetRegionsOfInterest           == NULL) { return -__LINE__; }
    if (p_EX_Request                        == NULL) { return -__LINE__; }
    if (p_EX_Status                         == NULL) { return -__LINE__; }
    if (p_EX_SetInterfacePriority           == NULL) { return -__LINE__; }
    if (p_EX_GetInterfacePriority           == NULL) { return -__LINE__; }
    if (p_IMT_ZHTInvalidate                 == NULL) { return -__LINE__; }
    if (p_IMT_ZLTInvalidate                 == NULL) { return -__LINE__; }
    if (p_IMT_YUV2RGB                       == NULL) { return -__LINE__; }
    if (p_IMT_Extract                       == NULL) { return -__LINE__; }
    if (p_IMT_Release                       == NULL) { return -__LINE__; }

    return 1;
}

int hl2da_api::InitializeLibrary()
{
    int initialize_result = Load();
    OutputDebugStringW(*(L"hl2da::Load returned " + FString::FromInt(initialize_result) + L"\n"));
    return initialize_result;
}

hl2da_api::pv_captureformat hl2da_api::CreateFormat_PV(uint16_t width, uint16_t height, uint8_t framerate, bool enable_mrc, bool shared)
{
    pv_captureformat cf;

    cf.enable_mrc = enable_mrc;
    cf.hologram_composition = true;
    cf.recording_indicator = false;
    cf.video_stabilization = false;
    cf.blank_protected = false;
    cf.show_mesh = false;
    cf.shared = shared;
    cf.global_opacity = 0.9f;
    cf.output_width = 0.0f;
    cf.output_height = 0.0f;
    cf.video_stabilization_length = 0;
    cf.hologram_perspective = (int)HOLOGRAM_PERSPECTIVE::PV;
    cf.width = width;
    cf.height = height;
    cf.framerate = framerate;

    return cf;
}

hl2da_api::ea_captureformat hl2da_api::CreateFormat_EA(MIXER_MODE mode, uint8_t device_index, uint8_t source_index)
{
    ea_captureformat cf;

    cf.mixer_mode = (((uint32_t)mode) & 0xFF) | (((uint32_t)device_index) << 8) | (((uint32_t)source_index) << 16);
    cf.loopback_gain = 1.0f;
    cf.microphone_gain = 1.0f;

    return cf;
}

hl2da_api::ev_captureformat hl2da_api::CreateFormat_EV(uint16_t width, uint16_t height, uint8_t framerate, wchar_t const* subtype, bool shared, uint32_t group_index, uint32_t source_index, uint32_t profile_index)
{
    ev_captureformat cf;

    cf.enable_mrc = 0;
    cf.hologram_composition = 0;
    cf.recording_indicator = 0;
    cf.video_stabilization = 0;
    cf.blank_protected = 0;
    cf.show_mesh = 0;
    cf.shared = shared;
    cf.global_opacity = (float)group_index;
    cf.output_width = (float)source_index;
    cf.output_height = (float)profile_index;
    cf.video_stabilization_length = 0;
    cf.hologram_perspective = 0;
    cf.width = width;
    cf.height = height;
    cf.framerate = framerate;

    wcscpy_s(cf.subtype, sizeof(ev_captureformat::subtype) / sizeof(wchar_t), subtype);

    return cf;
}

void hl2da_api::DebugMessage(char const* str)
{
    reinterpret_cast<pfn_DebugMessage>(p_DebugMessage)(str);
}

void hl2da_api::InitializeComponents()
{
    reinterpret_cast<pfn_InitializeComponents>(p_InitializeComponents)();
}

int hl2da_api::OverrideWorldCoordinateSystem(void* scs)
{
    return reinterpret_cast<pfn_OverrideWorldCoordinateSystem>(p_OverrideWorldCoordinateSystem)(scs);
}

uint64_t hl2da_api::GetUTCOffset(int32_t samples)
{
    return reinterpret_cast<pfn_GetUTCOffset>(p_GetUTCOffset)(samples);
}

void hl2da_api::Initialize(SENSOR_ID id, int buffer_size)
{
    reinterpret_cast<pfn_Initialize>(p_Initialize)((int)id, buffer_size);
}

void hl2da_api::SetConstantFactorVLC_RM(int64_t factor)
{
    reinterpret_cast<pfn_SetConstantFactorVLC_RM>(p_SetConstantFactorVLC_RM)(factor);
}

void hl2da_api::BypassDepthLock_RM(bool bypass)
{
    reinterpret_cast<pfn_BypassDepthLock_RM>(p_BypassDepthLock_RM)(bypass);
}

void hl2da_api::SetFormat_PV(pv_captureformat const& cf)
{
    reinterpret_cast<pfn_SetFormat_PV>(p_SetFormat_PV)(&cf);
}

void hl2da_api::SetFormat_Microphone(MC_CHANNELS use)
{
    reinterpret_cast<pfn_SetFormat_MC>(p_SetFormat_MC)((int)use);
}

void hl2da_api::SetFormat_ExtendedEyeTracking(EE_FPS_INDEX fps_index)
{
    reinterpret_cast<pfn_SetFormat_EE>(p_SetFormat_EE)((int)fps_index);
}

void hl2da_api::SetFormat_ExtendedAudio(ea_captureformat const& cf)
{
    reinterpret_cast<pfn_SetFormat_EA>(p_SetFormat_EA)(&cf);
}

void hl2da_api::SetFormat_ExtendedVideo(ev_captureformat const& cf)
{
    reinterpret_cast<pfn_SetFormat_EV>(p_SetFormat_EV)(&cf);
}

void hl2da_api::SetEnable(SENSOR_ID id, bool enable)
{
    reinterpret_cast<pfn_SetEnable>(p_SetEnable)((int)id, enable);
}

int hl2da_api::GetByFramestamp(SENSOR_ID id, int stamp, void** frame, uint64_t* timestamp, int32_t* framestamp)
{
    return reinterpret_cast<pfn_GetByFramestamp>(p_GetByFramestamp)((int)id, stamp, frame, timestamp, framestamp);
}

int hl2da_api::GetByTimestamp(SENSOR_ID id, uint64_t stamp, TIME_PREFERENCE time_preference, bool tiebreak_right, void** frame, uint64_t* timestamp, int32_t* framestamp)
{
    return reinterpret_cast<pfn_GetByTimestamp>(p_GetByTimestamp)((int)id, stamp, (int)time_preference, tiebreak_right, frame, timestamp, framestamp);
}

void hl2da_api::Extract(SENSOR_ID id, void* frame, int32_t* valid, void const** b, int32_t* l)
{
    reinterpret_cast<pfn_Extract>(p_Extract)((int)id, frame, valid, b, l);
}

void hl2da_api::Release(SENSOR_ID id, void* frame)
{
    reinterpret_cast<pfn_Release>(p_Release)((int)id, frame);
}

void hl2da_api::RM_SetEyeSelection(bool enable)
{
    reinterpret_cast<pfn_RM_SetEyeSelection>(p_RM_SetEyeSelection)(enable);
}

void hl2da_api::RM_GetIntrinsics(SENSOR_ID id, float* uv2xy, float* mapxy, float* k)
{
    reinterpret_cast<pfn_RM_GetIntrinsics>(p_RM_GetIntrinsics)((int)id, uv2xy, mapxy, k);
}

void hl2da_api::RM_GetExtrinsics(SENSOR_ID id, float* extrinsics)
{
    reinterpret_cast<pfn_RM_GetExtrinsics>(p_RM_GetExtrinsics)((int)id, extrinsics);
}

void hl2da_api::RM_MapImagePointToCameraUnitPlane(SENSOR_ID id, float const* in, int in_pitch, float* out, int out_pitch, int point_count)
{
    reinterpret_cast<pfn_RM_MapImagePointToCameraUnitPlane>(p_RM_MapImagePointToCameraUnitPlane)((int)id, in, in_pitch, out, out_pitch, point_count);
}

void hl2da_api::RM_MapCameraSpaceToImagePoint(SENSOR_ID id, float const* in, int in_pitch, float* out, int out_pitch, int point_count)
{
    reinterpret_cast<pfn_RM_MapCameraSpaceToImagePoint>(p_RM_MapCameraSpaceToImagePoint)((int)id, in, in_pitch, out, out_pitch, point_count);
}

void hl2da_api::PV_SetFocus(PV_FocusMode focusmode, PV_AutoFocusRange autofocusrange, PV_ManualFocusDistance distance, uint32_t value, PV_DriverFallback disabledriverfallback)
{
    reinterpret_cast<pfn_PV_SetFocus>(p_PV_SetFocus)((uint32_t)focusmode, (uint32_t)autofocusrange, (uint32_t)distance, value, (uint32_t)disabledriverfallback);
}

void hl2da_api::PV_SetVideoTemporalDenoising(PV_VideoTemporalDenoisingMode mode)
{
    reinterpret_cast<pfn_PV_SetVideoTemporalDenoising>(p_PV_SetVideoTemporalDenoising)((uint32_t)mode);
}

void hl2da_api::PV_SetWhiteBalance_Preset(PV_ColorTemperaturePreset preset)
{
    reinterpret_cast<pfn_PV_SetWhiteBalance_Preset>(p_PV_SetWhiteBalance_Preset)((uint32_t)preset);
}

void hl2da_api::PV_SetWhiteBalance_Value(uint32_t value)
{
    reinterpret_cast<pfn_PV_SetWhiteBalance_Value>(p_PV_SetWhiteBalance_Value)(value);
}

void hl2da_api::PV_SetExposure(PV_ExposureMode setauto, uint32_t value)
{
    reinterpret_cast<pfn_PV_SetExposure>(p_PV_SetExposure)((uint32_t)setauto, value);
}

void hl2da_api::PV_SetExposurePriorityVideo(PV_ExposurePriorityVideo enabled)
{
    reinterpret_cast<pfn_PV_SetExposurePriorityVideo>(p_PV_SetExposurePriorityVideo)((uint32_t)enabled);
}

void hl2da_api::PV_SetSceneMode(PV_CaptureSceneMode mode)
{
    reinterpret_cast<pfn_PV_SetSceneMode>(p_PV_SetSceneMode)((uint32_t)mode);
}

void hl2da_api::PV_SetIsoSpeed(PV_IsoSpeedMode setauto, uint32_t value)
{
    reinterpret_cast<pfn_PV_SetIsoSpeed>(p_PV_SetIsoSpeed)((uint32_t)setauto, value);
}

void hl2da_api::PV_SetBacklightCompensation(PV_BacklightCompensationState enable)
{
    reinterpret_cast<pfn_PV_SetBacklightCompensation>(p_PV_SetBacklightCompensation)((uint32_t)enable);
}

void hl2da_api::PV_SetDesiredOptimization(PV_MediaCaptureOptimization mode)
{
    reinterpret_cast<pfn_PV_SetDesiredOptimization>(p_PV_SetDesiredOptimization)((uint32_t)mode);
}

void hl2da_api::PV_SetPrimaryUse(PV_CaptureUse mode)
{
    reinterpret_cast<pfn_PV_SetPrimaryUse>(p_PV_SetPrimaryUse)((uint32_t)mode);
}

void hl2da_api::PV_SetOpticalImageStabilization(PV_OpticalImageStabilizationMode mode)
{
    reinterpret_cast<pfn_PV_SetOpticalImageStabilization>(p_PV_SetOpticalImageStabilization)((uint32_t)mode);
}

void hl2da_api::PV_SetHdrVideo(PV_HdrVideoMode mode)
{
    reinterpret_cast<pfn_PV_SetHdrVideo>(p_PV_SetHdrVideo)((uint32_t)mode);
}

void hl2da_api::PV_SetRegionsOfInterest(bool clear, bool set, bool auto_exposure, bool auto_focus, bool bounds_normalized, float x, float y, float w, float h, PV_RegionOfInterestType type, uint32_t weight)
{
    reinterpret_cast<pfn_PV_SetRegionsOfInterest>(p_PV_SetRegionsOfInterest)(clear, set, auto_exposure, auto_focus, bounds_normalized, x, y, w, h, (uint32_t)type, weight);
}

void hl2da_api::EX_Request()
{
    reinterpret_cast<pfn_EX_Request>(p_EX_Request)();
}

uint32_t hl2da_api::EX_Status()
{
    return reinterpret_cast<pfn_EX_Status>(p_EX_Status)();
}

void hl2da_api::EX_SetInterfacePriority(SENSOR_ID id, InterfacePriority priority)
{
    reinterpret_cast<pfn_EX_SetInterfacePriority>(p_EX_SetInterfacePriority)((uint32_t)id, (int32_t)priority);
}

hl2da_api::InterfacePriority hl2da_api::EX_GetInterfacePriority(SENSOR_ID id)
{
    return static_cast<InterfacePriority>(reinterpret_cast<pfn_EX_GetInterfacePriority>(p_EX_GetInterfacePriority)((int)id));
}

void hl2da_api::IMT_ZHTInvalidate(uint16_t const* depth_in, uint16_t* depth_out)
{
    reinterpret_cast<pfn_IMT_ZHTInvalidate>(p_IMT_ZHTInvalidate)(depth_in, depth_out);
}

void hl2da_api::IMT_ZLTInvalidate(uint8_t const* sigma_in, uint16_t const* depth_in, uint16_t* depth_out)
{
    reinterpret_cast<pfn_IMT_ZLTInvalidate>(p_IMT_ZLTInvalidate)(sigma_in, depth_in, depth_out);
}

void hl2da_api::IMT_YUV2RGB(uint8_t const* image, uint32_t stride, uint32_t height, IMT_Format format_in, IMT_Format format_out, void** fc)
{
    reinterpret_cast<pfn_IMT_YUV2RGB>(p_IMT_YUV2RGB)(image, stride, height, (uint32_t)format_in, (uint32_t)format_out, fc);
}

void hl2da_api::IMT_Extract(void* fc, void const** buffer, int32_t* length)
{
    reinterpret_cast<pfn_IMT_Extract>(p_IMT_Extract)(fc, buffer, length);
}

void hl2da_api::IMT_Release(void* fc)
{
    reinterpret_cast<pfn_IMT_Release>(p_IMT_Release)(fc);
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "hl2da_api.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdint.h>

typedef void (*pfn_DebugMessage)(char const* str);
typedef void (*pfn_InitializeComponents)();
typedef int (*pfn_OverrideWorldCoordinateSystem)(void* scs_ptr);
typedef void (*pfn_Initialize)(int id, int buffer_size);
typedef void (*pfn_SetEnable)(int id, int enable);
typedef int (*pfn_GetByFramestamp)(int id, int32_t stamp, void** frame, uint64_t* timestamp, int32_t* framestamp);
typedef int (*pfn_GetByTimestamp)(int id, uint64_t stamp, int time_preference, int tiebreak_right, void** frame, uint64_t* timestamp, int32_t* framestamp);
typedef void (*pfn_Release)(int id, void* frame);
typedef void (*pfn_Extract)(int id, void* frame, int32_t* valid, void const** b, int32_t* l);
typedef void (*pfn_GetExtrinsics_RM)(int id, float* out);
typedef void (*pfn_MapImagePointToCameraUnitPlane_RM)(int id, float const* in, float* out, int point_count);
typedef void (*pfn_MapCameraSpaceToImagePoint_RM)(int id, float const* in, float* out, int point_count);
typedef void (*pfn_SetFormat_PV)(hl2da_api::pv_captureformat const* cf);
typedef void (*pfn_SetFormat_MC)(int raw);
typedef void (*pfn_SetFormat_EE)(int fps_index);

#if PLATFORM_HOLOLENS
extern "C" { HMODULE LoadLibraryW(LPCWSTR lpLibFileName); }
#endif

void* hl2da_api::hmod_mmret;
void* hl2da_api::hmod_hl2ss;

void* hl2da_api::p_DebugMessage;
void* hl2da_api::p_InitializeComponents;
void* hl2da_api::p_OverrideWorldCoordinateSystem;
void* hl2da_api::p_Initialize;
void* hl2da_api::p_SetEnable;
void* hl2da_api::p_GetByFramestamp;
void* hl2da_api::p_GetByTimestamp;
void* hl2da_api::p_Release;
void* hl2da_api::p_Extract;
void* hl2da_api::p_GetExtrinsics_RM;
void* hl2da_api::p_MapImagePointToCameraUnitPlane_RM;
void* hl2da_api::p_MapCameraSpaceToImagePoint_RM;
void* hl2da_api::p_SetFormat_PV;
void* hl2da_api::p_SetFormat_MC;
void* hl2da_api::p_SetFormat_EE;

int hl2da_api::Load()
{
    FString path_base = FPaths::ProjectPluginsDir() + L"hl2da/Binaries/hl2da/";

    path_base.ReplaceCharInline(L'/', L'\\');

    FString path_dll_mmret = path_base + L"Microsoft.MixedReality.EyeTracking.dll";
    FString path_dll_hl2ss = path_base + L"hl2da.dll";

    if (!FPaths::FileExists(path_dll_mmret)) { return -1; }
    if (!FPaths::FileExists(path_dll_hl2ss)) { return -2; }

    hmod_mmret = LoadLibraryW(*path_dll_mmret);
    hmod_hl2ss = LoadLibraryW(*path_dll_hl2ss);

    if (hmod_mmret == NULL) { return -3; }
    if (hmod_hl2ss == NULL) { return -4; }

    p_DebugMessage = GetProcAddress((HMODULE)hmod_hl2ss, "DebugMessage");
    p_InitializeComponents = GetProcAddress((HMODULE)hmod_hl2ss, "InitializeComponents");
    p_OverrideWorldCoordinateSystem = GetProcAddress((HMODULE)hmod_hl2ss, "OverrideWorldCoordinateSystem");
    p_Initialize = GetProcAddress((HMODULE)hmod_hl2ss, "Initialize");
    p_SetEnable = GetProcAddress((HMODULE)hmod_hl2ss, "SetEnable");
    p_GetByFramestamp = GetProcAddress((HMODULE)hmod_hl2ss, "GetByFramestamp");
    p_GetByTimestamp = GetProcAddress((HMODULE)hmod_hl2ss, "GetByTimestamp");
    p_Release = GetProcAddress((HMODULE)hmod_hl2ss, "Release");
    p_Extract = GetProcAddress((HMODULE)hmod_hl2ss, "Extract");
    p_GetExtrinsics_RM = GetProcAddress((HMODULE)hmod_hl2ss, "GetExtrinsics_RM");
    p_MapImagePointToCameraUnitPlane_RM = GetProcAddress((HMODULE)hmod_hl2ss, "MapImagePointToCameraUnitPlane_RM");
    p_MapCameraSpaceToImagePoint_RM = GetProcAddress((HMODULE)hmod_hl2ss, "MapCameraSpaceToImagePoint_RM");
    p_SetFormat_PV = GetProcAddress((HMODULE)hmod_hl2ss, "SetFormat_PV");
    p_SetFormat_MC = GetProcAddress((HMODULE)hmod_hl2ss, "SetFormat_MC");
    p_SetFormat_EE = GetProcAddress((HMODULE)hmod_hl2ss, "SetFormat_EE");

    if (p_DebugMessage == NULL) { return -5; }
    if (p_InitializeComponents == NULL) { return -6; }
    if (p_OverrideWorldCoordinateSystem == NULL) { return -7; }
    if (p_Initialize == NULL) { return -8; }
    if (p_SetEnable == NULL) { return -9; }
    if (p_GetByFramestamp == NULL) { return -10; }
    if (p_GetByTimestamp == NULL) { return -11; }
    if (p_Release == NULL) { return -12; }
    if (p_Extract == NULL) { return -13; }
    if (p_GetExtrinsics_RM == NULL) { return -14; }
    if (p_MapImagePointToCameraUnitPlane_RM == NULL) { return -15; }
    if (p_MapCameraSpaceToImagePoint_RM == NULL) { return -16; }
    if (p_SetFormat_PV == NULL) { return -17; }
    if (p_SetFormat_MC == NULL) { return -18; }
    if (p_SetFormat_EE == NULL) { return -19; }

    return 1;
}

int hl2da_api::InitializeLibrary()
{
    int initialize_result = Load();
    OutputDebugStringW(*(L"hl2da::Load returned " + FString::FromInt(initialize_result) + L"\n"));
    return initialize_result;
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

void hl2da_api::Initialize(SENSOR_ID id, int buffer_size)
{
    reinterpret_cast<pfn_Initialize>(p_Initialize)((int)id, buffer_size);
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

void hl2da_api::Release(SENSOR_ID id, void* frame)
{
    reinterpret_cast<pfn_Release>(p_Release)((int)id, frame);
}

void hl2da_api::Extract(SENSOR_ID id, void* frame, int32_t* valid, void const** b, int32_t* l)
{
    reinterpret_cast<pfn_Extract>(p_Extract)((int)id, frame, valid, b, l);
}

void hl2da_api::GetExtrinsics_RM(SENSOR_ID id, float* extrinsics)
{
    reinterpret_cast<pfn_GetExtrinsics_RM>(p_GetExtrinsics_RM)((int)id, extrinsics);
}

void hl2da_api::MapImagePointToCameraUnitPlane_RM(SENSOR_ID id, float const* image_points, float* camera_points, int point_count)
{
    reinterpret_cast<pfn_MapImagePointToCameraUnitPlane_RM>(p_MapImagePointToCameraUnitPlane_RM)((int)id, image_points, camera_points, point_count);
}

void hl2da_api::MapCameraSpaceToImagePoint_RM(SENSOR_ID id, float const* camera_points, float* image_points, int point_count)
{
    reinterpret_cast<pfn_MapCameraSpaceToImagePoint_RM>(p_MapCameraSpaceToImagePoint_RM)((int)id, camera_points, image_points, point_count);
}

void hl2da_api::SetFormat_PV(pv_captureformat const& cf)
{
    reinterpret_cast<pfn_SetFormat_PV>(p_SetFormat_PV)(&cf);
}

void hl2da_api::SetFormat_MC(MC_CHANNELS use)
{
    reinterpret_cast<pfn_SetFormat_MC>(p_SetFormat_MC)((int)use);
}

void hl2da_api::SetFormat_EE(EE_FPS_INDEX fps_index)
{
    reinterpret_cast<pfn_SetFormat_EE>(p_SetFormat_EE)((int)fps_index);
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
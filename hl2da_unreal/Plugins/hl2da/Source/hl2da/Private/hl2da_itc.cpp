// Fill out your copyright notice in the Description page of Project Settings.


#include "hl2da_itc.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdint.h>

typedef void (*pfn_Crop)(void* image_in, int width_in, int height_in, int bpp_in, void* image_out, int x_out, int y_out, int width_out, int height_out);
typedef void (*pfn_RM_Undistort)(int id, float* mapxy, int interpolation, int border_mode, uint16_t border_value, void* image_in, void* image_out);
typedef void (*pfn_RM_ToBGRX)(int id, void* image_in, bool alpha, void* image_out);
typedef void (*pfn_RM_DepthInitializeRays)(int id, float* uv2xy);
typedef void (*pfn_RM_DepthNormalize)(int id, uint16_t* depth_in, float* depth_out);
typedef void (*pfn_RM_DepthRepeat)(int id, float* depth, float* depth3);
typedef void (*pfn_RM_DepthTo3D)(int id, float* depth3, float* points);
typedef void (*pfn_RM_DepthFill2Build)(int id, float* depth3, float* points_ul, float* points_br);
typedef void (*pfn_RM_DepthFill1)(int id, float* depth, float* image_points, float* local_depths, float* out, int width, int height);
typedef void (*pfn_RM_DepthFill2)(int id, float* depth, float* image_points_ul, float* image_points_br, float* local_depths, float* out, int width, int height);
typedef void (*pfn_TransformPoints3)(float* transform, float* points_in, int count, float* points_out);
typedef void (*pfn_GetPoints3Channel)(float* points_in, int count, int channel, float* points_out);
typedef void (*pfn_ProjectPoints3)(float* intrinsics, float* transform, float* points_in, int count, float* points_out);

#if PLATFORM_HOLOLENS
extern "C" { HMODULE LoadLibraryW(LPCWSTR lpLibFileName); }
#endif

void* hl2da_itc::hmod_hl2da_itc;

void* hl2da_itc::p_Crop;
void* hl2da_itc::p_RM_Undistort;
void* hl2da_itc::p_RM_ToBGRX;
void* hl2da_itc::p_RM_DepthInitializeRays;
void* hl2da_itc::p_RM_DepthNormalize;
void* hl2da_itc::p_RM_DepthRepeat;
void* hl2da_itc::p_RM_DepthTo3D;
void* hl2da_itc::p_RM_DepthFill2Build;
void* hl2da_itc::p_RM_DepthFill1;
void* hl2da_itc::p_RM_DepthFill2;
void* hl2da_itc::p_TransformPoints3;
void* hl2da_itc::p_GetPoints3Channel;
void* hl2da_itc::p_ProjectPoints3;

int hl2da_itc::Load()
{
    FString path_base = FPaths::ProjectPluginsDir() + L"hl2da/Binaries/hl2da/";

    path_base.ReplaceCharInline(L'/', L'\\');

    FString path_dll_hl2ss = path_base + L"hl2da_itc.dll";

    if (!FPaths::FileExists(path_dll_hl2ss)) { return -__LINE__; }

    hmod_hl2da_itc = LoadLibraryW(*path_dll_hl2ss);

    if (hmod_hl2da_itc == NULL) { return -__LINE__; }

    p_Crop                   = GetProcAddress((HMODULE)hmod_hl2da_itc, "Crop");
    p_RM_Undistort           = GetProcAddress((HMODULE)hmod_hl2da_itc, "RM_Undistort");
    p_RM_ToBGRX              = GetProcAddress((HMODULE)hmod_hl2da_itc, "RM_ToBGRX");
    p_RM_DepthInitializeRays = GetProcAddress((HMODULE)hmod_hl2da_itc, "RM_DepthInitializeRays");
    p_RM_DepthNormalize      = GetProcAddress((HMODULE)hmod_hl2da_itc, "RM_DepthNormalize");
    p_RM_DepthRepeat         = GetProcAddress((HMODULE)hmod_hl2da_itc, "RM_DepthRepeat");
    p_RM_DepthTo3D           = GetProcAddress((HMODULE)hmod_hl2da_itc, "RM_DepthTo3D");
    p_RM_DepthFill2Build     = GetProcAddress((HMODULE)hmod_hl2da_itc, "RM_DepthFill2Build");
    p_RM_DepthFill1          = GetProcAddress((HMODULE)hmod_hl2da_itc, "RM_DepthFill1");
    p_RM_DepthFill2          = GetProcAddress((HMODULE)hmod_hl2da_itc, "RM_DepthFill2");
    p_TransformPoints3       = GetProcAddress((HMODULE)hmod_hl2da_itc, "TransformPoints3");
    p_GetPoints3Channel      = GetProcAddress((HMODULE)hmod_hl2da_itc, "GetPoints3Channel");
    p_ProjectPoints3         = GetProcAddress((HMODULE)hmod_hl2da_itc, "ProjectPoints3");

    if (p_Crop                   == NULL) { return -__LINE__; }
    if (p_RM_Undistort           == NULL) { return -__LINE__; }
    if (p_RM_ToBGRX              == NULL) { return -__LINE__; }
    if (p_RM_DepthInitializeRays == NULL) { return -__LINE__; }
    if (p_RM_DepthNormalize      == NULL) { return -__LINE__; }
    if (p_RM_DepthRepeat         == NULL) { return -__LINE__; }
    if (p_RM_DepthTo3D           == NULL) { return -__LINE__; }
    if (p_RM_DepthFill2Build     == NULL) { return -__LINE__; }
    if (p_RM_DepthFill1          == NULL) { return -__LINE__; }
    if (p_RM_DepthFill2          == NULL) { return -__LINE__; }
    if (p_TransformPoints3       == NULL) { return -__LINE__; }
    if (p_GetPoints3Channel      == NULL) { return -__LINE__; }
    if (p_ProjectPoints3         == NULL) { return -__LINE__; }

    return 1;    
}

int hl2da_itc::InitializeLibrary()
{
    int initialize_result = Load();
    OutputDebugStringW(*(L"hl2da_itc::Load returned " + FString::FromInt(initialize_result) + L"\n"));
    return initialize_result;
}

void hl2da_itc::Crop(void* image_in, int width_in, int height_in, int bpp_in, void* image_out, int x_out, int y_out, int width_out, int height_out)
{
    reinterpret_cast<pfn_Crop>(p_Crop)(image_in, width_in, height_in, bpp_in, image_out, x_out, y_out, width_out, height_out);
}

void hl2da_itc::RM_Undistort(int id, float* mapxy, int interpolation, int border_mode, uint16_t border_value, void* image_in, void* image_out)
{
    reinterpret_cast<pfn_RM_Undistort>(p_RM_Undistort)(id, mapxy, interpolation, border_mode, border_value, image_in, image_out);
}

void hl2da_itc::RM_ToBGRX(int id, void* image_in, bool alpha, void* image_out)
{
    reinterpret_cast<pfn_RM_ToBGRX>(p_RM_ToBGRX)(id, image_in, alpha, image_out);
}

void hl2da_itc::RM_DepthInitializeRays(int id, float* uv2xy)
{
    reinterpret_cast<pfn_RM_DepthInitializeRays>(p_RM_DepthInitializeRays)(id, uv2xy);
}

void hl2da_itc::RM_DepthNormalize(int id, uint16_t* depth_in, float* depth_out)
{
    reinterpret_cast<pfn_RM_DepthNormalize>(p_RM_DepthNormalize)(id, depth_in, depth_out);
}

void hl2da_itc::RM_DepthRepeat(int id, float* depth, float* depth3)
{
    reinterpret_cast<pfn_RM_DepthRepeat>(p_RM_DepthRepeat)(id, depth, depth3);
}

void hl2da_itc::RM_DepthTo3D(int id, float* depth3, float* points)
{
    reinterpret_cast<pfn_RM_DepthTo3D>(p_RM_DepthTo3D)(id, depth3, points);
}

void hl2da_itc::RM_DepthFill2Build(int id, float* depth3, float* points_ul, float* points_br)
{
    reinterpret_cast<pfn_RM_DepthFill2Build>(p_RM_DepthFill2Build)(id, depth3, points_ul, points_br);
}

void hl2da_itc::RM_DepthFill1(int id, float* depth, float* image_points, float* local_depths, float* out, int width, int height)
{
    reinterpret_cast<pfn_RM_DepthFill1>(p_RM_DepthFill1)(id, depth, image_points, local_depths, out, width, height);
}

void hl2da_itc::RM_DepthFill2(int id, float* depth, float* image_points_ul, float* image_points_br, float* local_depths, float* out, int width, int height)
{
    reinterpret_cast<pfn_RM_DepthFill2>(p_RM_DepthFill2)(id, depth, image_points_ul, image_points_br, local_depths, out, width, height);
}

void hl2da_itc::TransformPoints3(float* transform, float* points_in, int count, float* points_out)
{
    reinterpret_cast<pfn_TransformPoints3>(p_TransformPoints3)(transform, points_in, count, points_out);
}

void hl2da_itc::GetPoints3Channel(float* points_in, int count, int channel, float* points_out)
{
    reinterpret_cast<pfn_GetPoints3Channel>(p_GetPoints3Channel)(points_in, count, channel, points_out);
}

void hl2da_itc::ProjectPoints3(float* intrinsics, float* transform, float* points_in, int count, float* points_out)
{
    reinterpret_cast<pfn_ProjectPoints3>(p_ProjectPoints3)(intrinsics, transform, points_in, count, points_out);
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class HL2DA_API hl2da_itc
{
private:
    static void* hmod_hl2da_itc;

    static void* p_Crop;
    static void* p_RM_Undistort;
    static void* p_RM_ToBGRX;
    static void* p_RM_DepthInitializeRays;
    static void* p_RM_DepthNormalize;
    static void* p_RM_DepthRepeat;
    static void* p_RM_DepthTo3D;
    static void* p_RM_DepthFill2Build;
    static void* p_RM_DepthFill1;
    static void* p_RM_DepthFill2;
    static void* p_TransformPoints3;
    static void* p_GetPoints3Channel;
    static void* p_ProjectPoints3;

    static int Load();

public:
    static int InitializeLibrary();

    static void Crop(void* image_in, int width_in, int height_in, int bpp_in, void* image_out, int x_out, int y_out, int width_out, int height_out);
    static void RM_Undistort(int id, float* mapxy, int interpolation, int border_mode, uint16_t border_value, void* image_in, void* image_out);
    static void RM_ToBGRX(int id, void* image_in, bool alpha, void* image_out);
    static void RM_DepthInitializeRays(int id, float* uv2xy);
    static void RM_DepthNormalize(int id, uint16_t* depth_in, float* depth_out);
    static void RM_DepthRepeat(int id, float* depth, float* depth3);
    static void RM_DepthTo3D(int id, float* depth3, float* points);
    static void RM_DepthFill2Build(int id, float* depth3, float* points_ul, float* points_br);
    static void RM_DepthFill1(int id, float* depth, float* image_points, float* local_depths, float* out, int width, int height);
    static void RM_DepthFill2(int id, float* depth, float* image_points_ul, float* image_points_br, float* local_depths, float* out, int width, int height);
    static void TransformPoints3(float* transform, float* points_in, int count, float* points_out);
    static void GetPoints3Channel(float* points_in, int count, int channel, float* points_out);
    static void ProjectPoints3(float* intrinsics, float* transform, float* points_in, int count, float* points_out);
};

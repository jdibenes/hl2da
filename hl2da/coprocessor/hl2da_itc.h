
#pragma once

#include <stdint.h>

#define PLUGIN_IMPORT extern "C" __declspec(dllimport)

PLUGIN_IMPORT
void ITC_Crop(void* image_in, int32_t width_in, int32_t height_in, int32_t bpp_in, void* image_out, int32_t x_out, int32_t y_out, int32_t width_out, int32_t height_out);

PLUGIN_IMPORT
void ITC_RMUndistort(int32_t id, float* mapxy, int32_t interpolation, int32_t border_mode, uint16_t border_value, void* image_in, void* image_out);

PLUGIN_IMPORT
void ITC_RMToBGRX(int32_t id, void* image_in, int32_t alpha, void* image_out);

PLUGIN_IMPORT
void ITC_RMDepthInitializeRays(int32_t id, float* uv2xy);

PLUGIN_IMPORT
void ITC_RMDepthNormalize(int32_t id, uint16_t* depth_in, float* depth_out);

PLUGIN_IMPORT
void ITC_RMDepthTo3D(int32_t id, float* depth, float* points);

PLUGIN_IMPORT
void ITC_RMDepthFill1(int32_t id, float* depth, float* image_points, float* local_depths, float* out, int32_t width, int32_t height);

PLUGIN_IMPORT
void ITC_TransformIdentity(float* transform_out);

PLUGIN_IMPORT
void ITC_TranformInvert(float* transform_in, float* transform_out);

PLUGIN_IMPORT
void ITC_TransformMultiply(float* transform_in_A, float* transform_in_B, float* transform_out);

PLUGIN_IMPORT
void ITC_TransformPoints3(float* transform, float* points_in, int32_t count, float* points_out);

PLUGIN_IMPORT
void ITC_GetPoints2Channel(float* points_in, int32_t count, int32_t channel, float* points_out);

PLUGIN_IMPORT
void ITC_GetPoints3Channel(float* points_in, int32_t count, int32_t channel, float* points_out);

PLUGIN_IMPORT
void ITC_ProjectPoints3(float* intrinsics, float* transform, float* points_in, int32_t count, float* points_out);


#include "effect.h"
#include "depth.h"
#include "transform.h"

#define PLUGIN_EXPORT extern "C" __declspec(dllexport)

PLUGIN_EXPORT
void ITC_Crop(void* image_in, int32_t width_in, int32_t height_in, int32_t bpp_in, void* image_out, int32_t x_out, int32_t y_out, int32_t width_out, int32_t height_out)
{
    Crop(image_in, width_in, height_in, bpp_in, image_out, x_out, y_out, width_out, height_out);
}

PLUGIN_EXPORT
void ITC_RMUndistort(int32_t id, float* mapxy, int32_t interpolation, int32_t border_mode, uint16_t border_value, void* image_in, void* image_out)
{
    RM_Undistort(id, mapxy, interpolation, border_mode, border_value, image_in, image_out);
}

PLUGIN_EXPORT
void ITC_RMToBGRX(int32_t id, void* image_in, int32_t alpha, void* image_out)
{
    RM_ToBGRX(id, image_in, alpha != 0, image_out);
}

PLUGIN_EXPORT
void ITC_RMDepthInitializeRays(int32_t id, float* uv2xy)
{
    RM_DepthInitializeRays(id, uv2xy);
}

PLUGIN_EXPORT
void ITC_RMDepthNormalize(int32_t id, uint16_t* depth_in, float* depth_out)
{
    RM_DepthNormalize(id, depth_in, depth_out);
}

PLUGIN_EXPORT
void ITC_RMDepthTo3D(int32_t id, float* depth, float* points)
{
    RM_DepthTo3D(id, depth, points);
}

PLUGIN_EXPORT
void ITC_RMDepthFill1(int32_t id, float* depth, float* image_points, float* local_depths, float* out, int32_t width, int32_t height)
{
    RM_DepthFill1(id, depth, image_points, local_depths, out, width, height);
}

PLUGIN_EXPORT
void ITC_TransformIdentity(float* transform_out)
{
    TransformIdentity(transform_out);
}

PLUGIN_EXPORT
void ITC_TranformInvert(float* transform_in, float* transform_out)
{
    TranformInvert(transform_in, transform_out);
}

PLUGIN_EXPORT
void ITC_TransformMultiply(float* transform_in_A, float* transform_in_B, float* transform_out)
{
    TransformMultiply(transform_in_A, transform_in_B, transform_out);
}

PLUGIN_EXPORT
void ITC_TransformPoints3(float* transform, float* points_in, int32_t count, float* points_out)
{
    TransformPoints3(transform, points_in, count, points_out);
}

PLUGIN_EXPORT
void ITC_GetPoints2Channel(float* points_in, int32_t count, int32_t channel, float* points_out)
{
    GetPoints2Channel(points_in, count, channel, points_out);
}

PLUGIN_EXPORT
void ITC_GetPoints3Channel(float* points_in, int32_t count, int32_t channel, float* points_out)
{
    GetPoints3Channel(points_in, count, channel, points_out);
}

PLUGIN_EXPORT
void ITC_ProjectPoints3(float* intrinsics, float* transform, float* points_in, int32_t count, float* points_out)
{
    ProjectPoints3(intrinsics, transform, points_in, count, points_out);
}

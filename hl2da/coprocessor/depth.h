
#pragma once

#include <stdint.h>

void RM_DepthInitializeRays(int id, float* uv2xy);
void RM_DepthNormalize(int id, uint16_t* depth_in, float* depth_out);
void RM_DepthTo3D(int id, float* depth, float* points);
void RM_DepthFill1(int id, float* depth, float* image_points, float* local_depths, float* out, int width, int height);

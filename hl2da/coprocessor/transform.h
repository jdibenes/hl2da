
#pragma once

void TransformIdentity(float* transform_out);
void TranformInvert(float* transform_in, float* transform_out);
void TransformMultiply(float* transform_in_A, float* transform_in_B, float* transform_out);
void TransformPoints3(float* transform, float* points_in, int count, float* points_out);
void GetPoints2Channel(float* points_in, int count, int channel, float* points_out);
void GetPoints3Channel(float* points_in, int count, int channel, float* points_out);
void ProjectPoints3(float* intrinsics, float* transform, float* points_in, int count, float* points_out);

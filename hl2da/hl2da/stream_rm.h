
#pragma once

#include <stdint.h>

void RM_SetEnable(int id, bool enable);

int RM_Get(int id, int32_t stamp, void*& f, uint64_t& t, int32_t& s);
int RM_Get(int id, uint64_t timestamp, int time_preference, bool tiebreak_right, void*& f, uint64_t& t, int32_t& s);

void RM_Release(void* frame);

void RM_Extract_VLC(void* frame, void const** buffer, int32_t* length, void const** pose_buffer, int32_t* pose_length);
void RM_Extract_Depth_AHAT(void* frame, void const** buffer, int32_t* length, void const** ab_depth_buffer, int32_t* ab_depth_length, void const** pose_buffer, int32_t* pose_length);
void RM_Extract_Depth_Longthrow(void* frame, void const** buffer, int32_t* length, void const** ab_depth_buffer, int32_t* ab_depth_length, void const** sigma_buffer, int32_t* sigma_length, void const** pose_buffer, int32_t* pose_length);
void RM_Extract_IMU_Accelerometer(void* frame, void const** buffer, int32_t* length, void const** pose_buffer, int32_t* pose_length);
void RM_Extract_IMU_Gyroscope(void* frame, void const** buffer, int32_t* length, void const** pose_buffer, int32_t* pose_length);
void RM_Extract_IMU_Magnetometer(void* frame, void const** buffer, int32_t* length, void const** pose_buffer, int32_t* pose_length);

void RM_GetExtrinsics(int id, float* out);
void RM_MapImagePointToCameraUnitPlane(int id, float const* in, float* out, int point_count);
void RM_MapCameraSpaceToImagePoint(int id, float const* in, float* out, int point_count);

void RM_InitializeDepthLock();
void RM_CleanupDepthLock();
void RM_BypassDepthLock(bool bypass);

void RM_Initialize(int id, int32_t buffer_size);
void RM_Quit(int id);
void RM_Cleanup(int id);


#pragma once

#include <stdint.h>
#include "researchmode/ResearchModeApi.h"

#define PLUGIN_IMPORT extern "C" __declspec(dllimport)

enum sensor_id
{
    RM_VLC_LEFTFRONT,
    RM_VLC_LEFTLEFT,
    RM_VLC_RIGHTFRONT,
    RM_VLC_RIGHTRIGHT,
    RM_DEPTH_AHAT,
    RM_DEPTH_LONGTHROW,
    RM_IMU_ACCELEROMETER,
    RM_IMU_GYROSCOPE,
    RM_IMU_MAGNETOMETER
};

PLUGIN_IMPORT
void DebugMessage(char const* str);

PLUGIN_IMPORT
void Unpack_U8(void const* source, uint8_t* destination, int elements);

PLUGIN_IMPORT
void Unpack_U16(void const* source, uint16_t* destination, int elements);

PLUGIN_IMPORT
void Unpack_U32(void const* source, uint32_t* destination, int elements);

PLUGIN_IMPORT
void Unpack_U64(void const* source, uint64_t* destination, int elements);

PLUGIN_IMPORT
void Unpack_RM_IMU_Accelerometer(void const* source, AccelDataStruct* destination, int elements);

PLUGIN_IMPORT
void Unpack_RM_IMU_Gyroscope(void const* source, GyroDataStruct* destination, int elements);

PLUGIN_IMPORT
void Unpack_RM_IMU_Magnetometer(void const* source, MagDataStruct* destination, int elements);

PLUGIN_IMPORT
void InitializeGlobal();

PLUGIN_IMPORT
void Initialize(int id, int buffer_size);

PLUGIN_IMPORT
void SetEnable(int id, int enable);

PLUGIN_IMPORT
int GetByFramestamp(int id, int32_t stamp, void** frame, uint64_t* timestamp, int32_t* framestamp);

PLUGIN_IMPORT
int GetByTimestamp(int id, uint64_t stamp, int time_preference, int tiebreak_right, void** frame, uint64_t* timestamp, int32_t* framestamp);

PLUGIN_IMPORT
void Release_RM(void* frame);

PLUGIN_IMPORT
void Extract_RM_VLC(void* frame, void const** buffer, int32_t* length, void** pose_buffer, int32_t* pose_length);

PLUGIN_IMPORT
void Extract_RM_Depth_AHAT(void* frame, void const** buffer, int32_t* length, void const** ab_depth_buffer, int32_t* ab_depth_length, void** pose_buffer, int32_t* pose_length);

PLUGIN_IMPORT
void Extract_RM_Depth_Longthrow(void* frame, void const** buffer, int32_t* length, void const** ab_depth_buffer, int32_t* ab_depth_length, void const** sigma_buffer, int32_t* sigma_length, void** pose_buffer, int32_t* pose_length);

PLUGIN_IMPORT
void Extract_RM_IMU_Accelerometer(void* frame, void const** buffer, int32_t* length, void** pose_buffer, int32_t* pose_length);

PLUGIN_IMPORT
void Extract_RM_IMU_Gyroscope(void* frame, void const** buffer, int32_t* length, void** pose_buffer, int32_t* pose_length);

PLUGIN_IMPORT
void Extract_RM_IMU_Magnetometer(void* frame, void const** buffer, int32_t* length, void** pose_buffer, int32_t* pose_length);

PLUGIN_IMPORT
void GetExtrinsics_RM(int id, float* out);

PLUGIN_IMPORT
void MapImagePointToCameraUnitPlane_RM(int id, float const* in, float* out, int point_count);

PLUGIN_IMPORT
void MapCameraSpaceToImagePoint_RM(int id, float const* in, float* out, int point_count);

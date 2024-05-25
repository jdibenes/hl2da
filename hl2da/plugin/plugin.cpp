
#include "../hl2da/locator.h"
#include "../hl2da/research_mode.h"
#include "../hl2da/stream_rm.h"
#include "../hl2da/log.h"

#define PLUGIN_EXPORT extern "C" __declspec(dllexport)

template<typename T>
void BlockCopy(void const* source, T* destination, int elements)
{
    memcpy(destination, source, elements * sizeof(T));
}

PLUGIN_EXPORT
void DebugMessage(char const* str)
{
    ShowMessage("%s", str);
}

PLUGIN_EXPORT
void Unpack_U8(void const* source, uint8_t* destination, int elements)
{
    BlockCopy(source, destination, elements);
}

PLUGIN_EXPORT
void Unpack_U16(void const* source, uint16_t* destination, int elements)
{
    BlockCopy(source, destination, elements);
}

PLUGIN_EXPORT
void Unpack_U32(void const* source, uint32_t* destination, int elements)
{
    BlockCopy(source, destination, elements);
}

PLUGIN_EXPORT
void Unpack_U64(void const* source, uint64_t* destination, int elements)
{
    BlockCopy(source, destination, elements);
}

PLUGIN_EXPORT
void Unpack_RM_IMU_Accelerometer(void const* source, AccelDataStruct* destination, int elements)
{
    BlockCopy(source, destination, elements);
}

PLUGIN_EXPORT
void Unpack_RM_IMU_Gyroscope(void const* source, GyroDataStruct* destination, int elements)
{
    BlockCopy(source, destination, elements);
}

PLUGIN_EXPORT
void Unpack_RM_IMU_Magnetometer(void const* source, MagDataStruct* destination, int elements)
{
    BlockCopy(source, destination, elements);
}

PLUGIN_EXPORT
void InitializeGlobal()
{
    Locator_Initialize();
    ResearchMode_Initialize();
    RM_InitializeDepthLock();
}

PLUGIN_EXPORT
void Initialize(int id, int buffer_size)
{
    switch (id)
    {
    case  0:
    case  1:
    case  2:
    case  3:
    case  4:
    case  5:
    case  6:
    case  7:
    case  8: RM_Initialize(id, buffer_size); break;
    case  9: //
    case 10: //
    case 11: //
    case 12: //
    case 13: //
    case 14: //
        break;
    }
}

PLUGIN_EXPORT
void SetEnable(int id, int enable)
{
    switch (id)
    {
    case  0:
    case  1:
    case  2:
    case  3:
    case  4:
    case  5:
    case  6:
    case  7:
    case  8: RM_SetEnable(id, enable != 0); break;
    case  9: //
    case 10: //
    case 11: //
    case 12: //
    case 13: //
    case 14: //
        break;
    }
}

PLUGIN_EXPORT
int GetByFramestamp(int id, int32_t stamp, void** frame, uint64_t* timestamp, int32_t* framestamp)
{
    switch (id)
    {
    case  0:
    case  1:
    case  2:
    case  3:
    case  4:
    case  5:
    case  6:
    case  7:
    case  8: return RM_Get(id, stamp, *(rm_frame**)frame, *timestamp, *framestamp);
    case  9: //
    case 10: //
    case 11: //
    case 12: //
    case 13: //
    case 14: //
        break;
    }

    return -1;
}

PLUGIN_EXPORT
int GetByTimestamp(int id, uint64_t stamp, int time_preference, int tiebreak_right, void** frame, uint64_t* timestamp, int32_t* framestamp)
{
    switch (id)
    {
    case  0:
    case  1:
    case  2:
    case  3:
    case  4:
    case  5:
    case  6:
    case  7:
    case  8: return RM_Get(id, stamp, time_preference, tiebreak_right != 0, *(rm_frame**)frame, *timestamp, *framestamp);
    case  9: //
    case 10: //
    case 11: //
    case 12: //
    case 13: //
    case 14: //
        break;
    }

    return -1;
}

PLUGIN_EXPORT
void Release_RM(void* frame)
{
    ((rm_frame*)frame)->Release();
}

PLUGIN_EXPORT
void Extract_RM_VLC(void* frame, void const** buffer, int32_t* length, void** pose_buffer, int32_t* pose_length)
{
    rm_frame* f = (rm_frame*)frame;
    rm_data_vlc d;
    RM_Extract(f->rmsf, d);
    *buffer = d.buffer;
    *length = (int32_t)d.length;
    *pose_buffer = &(f->pose);
    *pose_length = sizeof(rm_frame::pose);
}

PLUGIN_EXPORT
void Extract_RM_Depth_AHAT(void* frame, void const** buffer, int32_t* length, void const** ab_depth_buffer, int32_t* ab_depth_length, void** pose_buffer, int32_t* pose_length)
{
    rm_frame* f = (rm_frame*)frame;
    rm_data_zht d;
    RM_Extract(f->rmsf, d);
    *buffer = d.buffer;
    *length = (int32_t)d.length;
    *ab_depth_buffer = d.ab_depth_buffer;
    *ab_depth_length = (int32_t)d.ab_depth_length;
    *pose_buffer = &f->pose;
    *pose_length = sizeof(rm_frame::pose);
}

PLUGIN_EXPORT
void Extract_RM_Depth_Longthrow(void* frame, void const** buffer, int32_t* length, void const** ab_depth_buffer, int32_t* ab_depth_length, void const** sigma_buffer, int32_t* sigma_length, void** pose_buffer, int32_t* pose_length)
{
    rm_frame* f = (rm_frame*)frame;
    rm_data_zlt d;
    RM_Extract(f->rmsf, d);
    *buffer = d.buffer;
    *length = (int32_t)d.length;
    *ab_depth_buffer = d.ab_depth_buffer;
    *ab_depth_length = (int32_t)d.ab_depth_length;
    *sigma_buffer = d.sigma_buffer;
    *sigma_length = (int32_t)d.sigma_length;
    *pose_buffer = &f->pose;
    *pose_length = sizeof(rm_frame::pose);
}

PLUGIN_EXPORT
void Extract_RM_IMU_Accelerometer(void* frame, void const** buffer, int32_t* length, void** pose_buffer, int32_t* pose_length)
{
    rm_frame* f = (rm_frame*)frame;
    rm_data_acc d;
    RM_Extract(f->rmsf, d);
    *buffer = d.buffer;
    *length = (int32_t)d.length;
    *pose_buffer = &f->pose;
    *pose_length = sizeof(rm_frame::pose);
}

PLUGIN_EXPORT
void Extract_RM_IMU_Gyroscope(void* frame, void const** buffer, int32_t* length, void** pose_buffer, int32_t* pose_length)
{
    rm_frame* f = (rm_frame*)frame;
    rm_data_gyr d;
    RM_Extract(f->rmsf, d);
    *buffer = d.buffer;
    *length = (int32_t)d.length;
    *pose_buffer = &f->pose;
    *pose_length = sizeof(rm_frame::pose);
}

PLUGIN_EXPORT
void Extract_RM_IMU_Magnetometer(void* frame, void const** buffer, int32_t* length, void** pose_buffer, int32_t* pose_length)
{
    rm_frame* f = (rm_frame*)frame;
    rm_data_mag d;
    RM_Extract(f->rmsf, d);
    *buffer = d.buffer;
    *length = (int32_t)d.length;
    *pose_buffer = &f->pose;
    *pose_length = sizeof(rm_frame::pose);
}

PLUGIN_EXPORT
void GetExtrinsics_RM(int id, float* out)
{
    RM_GetExtrinsics(id, out);
}

PLUGIN_EXPORT
void MapImagePointToCameraUnitPlane_RM(int id, float const* in, float* out, int point_count)
{
    RM_MapImagePointToCameraUnitPlane(id, in, out, point_count);
}

PLUGIN_EXPORT
void MapCameraSpaceToImagePoint_RM(int id, float const* in, float* out, int point_count)
{
    RM_MapCameraSpaceToImagePoint(id, in, out, point_count);
}

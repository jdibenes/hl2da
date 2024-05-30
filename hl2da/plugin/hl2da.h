
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
    RM_IMU_MAGNETOMETER,
    PV,
    MICROPHONE,
    SPATIAL_INPUT,
    EXTENDED_EYE_TRACKING,
};

struct AccelDataStruct
{
    uint64_t VinylHupTicks;
    uint64_t SocTicks;
    float AccelValues[3];
    float temperature;
};

struct GyroDataStruct
{
    uint64_t VinylHupTicks;
    uint64_t SocTicks;
    float GyroValues[3];
    float temperature;
};

struct MagDataStruct
{
    uint64_t VinylHupTicks;
    uint64_t SocTicks;
    float MagValues[3];
};

struct pv_videoformat
{
    uint16_t width;
    uint16_t height;
    uint8_t framerate;
};

struct mrc_video_options
{
    bool enable;
    bool hologram_composition;
    bool recording_indicator;
    bool video_stabilization;
    bool blank_protected;
    bool show_mesh;
    bool shared;
    uint8_t _reserved[1];
    float global_opacity;
    float output_width;
    float output_height;
    uint32_t video_stabilization_length;
    uint32_t hologram_perspective;
};

struct pv_captureformat
{
    mrc_video_options mrcvo;
    pv_videoformat vf;
    uint8_t _reserved[2];
};

struct joint_pose
{
    float rx;
    float ry;
    float rz;
    float rw;
    float tx;
    float ty;
    float tz;
    float radius;
    int32_t accuracy;
};

PLUGIN_IMPORT
void Copy(void const* source, void* destination, int bytes);

PLUGIN_IMPORT
void DebugMessage(char const* str);

PLUGIN_IMPORT
void InitializeComponents();

PLUGIN_IMPORT
void InitializeComponentsOnUI();

PLUGIN_IMPORT
int OverrideWorldCoordinateSystem(void* scs_ptr);

PLUGIN_IMPORT
void Initialize(int id, int buffer_size);

PLUGIN_IMPORT
void SetEnable(int id, int enable);

PLUGIN_IMPORT
int GetByFramestamp(int id, int32_t stamp, void** frame, uint64_t* timestamp, int32_t* framestamp);

PLUGIN_IMPORT
int GetByTimestamp(int id, uint64_t stamp, int time_preference, int tiebreak_right, void** frame, uint64_t* timestamp, int32_t* framestamp);

PLUGIN_IMPORT
void Release(int id, void* frame);

PLUGIN_IMPORT
void Extract(int id, void* frame, int32_t* valid, void const** b, int32_t* l);

PLUGIN_IMPORT
void GetExtrinsics_RM(int id, float* out);

PLUGIN_IMPORT
void MapImagePointToCameraUnitPlane_RM(int id, float const* in, float* out, int point_count);

PLUGIN_IMPORT
void MapCameraSpaceToImagePoint_RM(int id, float const* in, float* out, int point_count);

PLUGIN_IMPORT
void SetFormat_PV(pv_captureformat const* cf);

PLUGIN_IMPORT
void SetFormat_MC(int raw);

PLUGIN_IMPORT
void SetFormat_EE(int fps_index);

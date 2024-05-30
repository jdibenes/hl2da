// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class HL2DA_API hl2da_api
{
public:
    enum class SENSOR_ID : int32_t
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
        float _reserved;
    };

    struct JointPose
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

    enum class STATUS : int32_t
    {
        DISCARDED = -1,
        OK = 0,
        WAIT = 1,
    };

    enum class TIME_PREFERENCE : int32_t
    {
        PAST = -1,
        NEAREST = 0,
        FUTURE = 1,
    };

    enum class MC_CHANNELS : int32_t
    {
        USE_2 = 0,
        USE_5 = 1,
    };

    enum class EE_FPS_INDEX : int32_t
    {
        FPS_30 = 0,
        FPS_60 = 1,
        FPS_90 = 2,
    };

    enum SI_VALID : int32_t
    {
        VALID_HEAD = 1,
        VALID_EYE = 2,
        VALID_LEFT = 4,
        VALID_RIGHT = 8,
    };

    enum EE_VALID : int32_t
    {
        VALID_CALIBRATION = 1,
        VALID_COMBINED_GAZE = 2,
        VALID_LEFT_GAZE = 4,
        VALID_RIGHT_GAZE = 8,
        VALID_LEFT_OPENNESS = 16,
        VALID_RIGHT_OPENNESS = 32,
        VALID_VERGENCE_DISTANCE = 64,
    };

    enum class HOLOGRAM_PERSPECTIVE : int32_t
    {
        DISPLAY = 0,
        PV = 1,
    };

    struct pv_captureformat
    {
        bool enable_mrc;
        bool hologram_composition;
        bool recording_indicator;
        bool video_stabilization;
        bool blank_protected;
        bool show_mesh;
        bool shared;
        uint8_t _reserved_0[1];
        float global_opacity;
        float output_width;
        float output_height;
        uint32_t video_stabilization_length;
        uint32_t hologram_perspective;
        uint16_t width;
        uint16_t height;
        uint8_t framerate;
        uint8_t _reserved_1[3];
    };

private:
    static void* hmod_mmret;
    static void* hmod_hl2ss;

    static void* p_DebugMessage;
    static void* p_InitializeComponents;
    static void* p_OverrideWorldCoordinateSystem;
    static void* p_Initialize;
    static void* p_SetEnable;
    static void* p_GetByFramestamp;
    static void* p_GetByTimestamp;
    static void* p_Release;
    static void* p_Extract;
    static void* p_GetExtrinsics_RM;
    static void* p_MapImagePointToCameraUnitPlane_RM;
    static void* p_MapCameraSpaceToImagePoint_RM;
    static void* p_SetFormat_PV;
    static void* p_SetFormat_MC;
    static void* p_SetFormat_EE;

    static int Load();
    
public:
    static int InitializeLibrary();
    static void DebugMessage(char const* str);
    static void InitializeComponents();
    static int OverrideWorldCoordinateSystem(void* scs);
    static void Initialize(SENSOR_ID id, int buffer_size);
    static void SetEnable(SENSOR_ID id, bool enable);
    static int GetByFramestamp(SENSOR_ID id, int stamp, void** frame, uint64_t* timestamp, int32_t* framestamp);
    static int GetByTimestamp(SENSOR_ID id, uint64_t stamp, TIME_PREFERENCE time_preference, bool tiebreak_right, void** frame, uint64_t* timestamp, int32_t* framestamp);
    static void Release(SENSOR_ID id, void* frame);
    static void Extract(SENSOR_ID id, void* frame, int32_t* valid, void const** b, int32_t* l);
    static void GetExtrinsics_RM(SENSOR_ID id, float* extrinsics);
    static void MapImagePointToCameraUnitPlane_RM(SENSOR_ID id, float const* image_points, float* camera_points, int point_count);
    static void MapCameraSpaceToImagePoint_RM(SENSOR_ID id, float const* camera_points, float* image_points, int point_count);
    static void SetFormat_PV(pv_captureformat const& cf);
    static void SetFormat_MC(MC_CHANNELS use);
    static void SetFormat_EE(EE_FPS_INDEX fps_index);

    static pv_captureformat CreateFormat_PV(uint16_t width, uint16_t height, uint8_t framerate, bool enable_mrc, bool shared);
};

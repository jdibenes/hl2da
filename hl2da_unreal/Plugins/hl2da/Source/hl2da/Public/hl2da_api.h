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
        EXTENDED_AUDIO,
        EXTENDED_VIDEO,
    };

    enum class MICROPHONE_ARRAY_CHANNEL : int32_t
    {
        TOP_LEFT = 0,
        TOP_CENTER = 1,
        TOP_RIGHT = 2,
        BOTTOM_LEFT = 3,
        BOTTOM_RIGHT = 4,
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

    enum class SI_HandJointKind : int32_t
    {
        Palm = 0,
        Wrist = 1,
        ThumbMetacarpal = 2,
        ThumbProximal = 3,
        ThumbDistal = 4,
        ThumbTip = 5,
        IndexMetacarpal = 6,
        IndexProximal = 7,
        IndexIntermediate = 8,
        IndexDistal = 9,
        IndexTip = 10,
        MiddleMetacarpal = 11,
        MiddleProximal = 12,
        MiddleIntermediate = 13,
        MiddleDistal = 14,
        MiddleTip = 15,
        RingMetacarpal = 16,
        RingProximal = 17,
        RingIntermediate = 18,
        RingDistal = 19,
        RingTip = 20,
        LittleMetacarpal = 21,
        LittleProximal = 22,
        LittleIntermediate = 23,
        LittleDistal = 24,
        LittleTip = 25,
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

    enum class SI_VALID : int32_t
    {
        HEAD = 1,
        EYE = 2,
        LEFT = 4,
        RIGHT = 8,
    };

    enum class EE_VALID : int32_t
    {
        CALIBRATION = 1,
        COMBINED_GAZE = 2,
        LEFT_GAZE = 4,
        RIGHT_GAZE = 8,
        LEFT_OPENNESS = 16,
        RIGHT_OPENNESS = 32,
        VERGENCE_DISTANCE = 64,
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

    enum class MIXER_MODE : uint32_t
    {
        MICROPHONE = 0,
        SYSTEM = 1,
        BOTH = 2,
    };

    struct ea_captureformat
    {
        uint32_t mixer_mode;
        float loopback_gain;
        float microphone_gain;
    };

    struct ea_audioformat
    {
        uint32_t bitrate;
        uint32_t bits_per_sample;
        uint32_t channel_count;
        uint32_t sample_rate;
        wchar_t subtype[64];
    };

    struct ev_captureformat
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
        uint8_t _reserved_1;
        wchar_t subtype[64];
        uint8_t _reserved_2[2];
    };

    struct ev_videoformat
    {
        uint16_t width;
        uint16_t height;
        uint8_t framerate;
        uint8_t _reserved;
        wchar_t subtype[64];
    };

    enum class PV_FocusMode : uint32_t
    {
        Auto = 0,
        Single = 1,
        Continuous = 2,
        Manual = 3,
    };

    enum class PV_AutoFocusRange : uint32_t
    {
        FullRange = 0,
        Macro = 1,
        Normal = 2,
    };

    enum class PV_ManualFocusDistance : uint32_t
    {
        Infinity = 0,
        Nearest = 2,
    };

    enum class PV_FocusValue : uint32_t
    {
        Min = 170,
        Max = 10000,
    };

    enum class PV_DriverFallback : uint32_t
    {
        Enable = 0,
        Disable = 1,
    };

    enum class PV_VideoTemporalDenoisingMode : uint32_t
    {
        Off = 0,
        On = 1,
    };

    enum class PV_ColorTemperaturePreset : uint32_t
    {
        Auto = 0,
        Manual = 1,
        Cloudy = 2,
        Daylight = 3,
        Flash = 4,
        Fluorescent = 5,
        Tungsten = 6,
        Candlelight = 7,
    };

    enum class PV_WhiteBalanceValue : uint32_t
    {
        Min = 2300, // 25
        Max = 7500, // 25
    };

    enum class PV_ExposureMode : uint32_t
    {
        Manual = 0,
        Auto = 1,
    };

    enum class PV_ExposureValue : uint32_t
    {
        Min = 1000, // 10
        Max = 660000, // 10
    };

    enum class PV_ExposurePriorityVideo : uint32_t
    {
        Disabled = 0,
        Enabled = 1,
    };

    enum class PV_CaptureSceneMode : uint32_t
    {
        Auto = 0,
        Macro = 2,
        Portrait = 3,
        Sport = 4,
        Snow = 5,
        Night = 6,
        Beach = 7,
        Sunset = 8,
        Candlelight = 9,
        Landscape = 10,
        NightPortrait = 11,
        Backlit = 12,
    };

    enum class PV_IsoSpeedMode : uint32_t
    {
        Manual = 0,
        Auto = 1,
    };

    enum class PV_IsoSpeedValue : uint32_t
    {
        Min = 100,
        Max = 3200,
    };

    enum class PV_BacklightCompensationState : uint32_t
    {
        Disable = 0,
        Enable = 1,
    };

    enum class IMT_Format : uint32_t
    {
        Rgba8 = 30,
        Gray16 = 57,
        Gray8 = 62,
        Bgra8 = 87,
        Nv12 = 103,
        Yuy2 = 107,
    };

private:
    static void* hmod_mmret;
    static void* hmod_hl2da;

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
    static void* p_BypassDepthLock_RM;
    static void* p_SetFormat_PV;
    static void* p_SetFormat_MC;
    static void* p_SetFormat_EE;
    static void* p_SetFormat_EA;
    static void* p_SetFormat_EV;
    static void* p_GetUTCOffset;
    static void* p_PV_SetFocus;
    static void* p_PV_SetVideoTemporalDenoising;
    static void* p_PV_SetWhiteBalance_Preset;
    static void* p_PV_SetWhiteBalance_Value;
    static void* p_PV_SetExposure;
    static void* p_PV_SetExposurePriorityVideo;
    static void* p_PV_SetSceneMode;
    static void* p_PV_SetIsoSpeed;
    static void* p_PV_SetBacklightCompensation;
    static void* p_IMT_ZHTInvalidate;
    static void* p_IMT_ZLTInvalidate;
    static void* p_IMT_YUV2RGB;
    static void* p_IMT_Extract;
    static void* p_IMT_Release;

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
    static void BypassDepthLock_RM(bool bypass);
    static void SetFormat_PV(pv_captureformat const& cf);
    static void SetFormat_Microphone(MC_CHANNELS use);
    static void SetFormat_ExtendedEyeTracking(EE_FPS_INDEX fps_index);
    static void SetFormat_ExtendedAudio(ea_captureformat const& cf);
    static void SetFormat_ExtendedVideo(ev_captureformat const& cf);
    static uint64_t GetUTCOffset(int32_t samples = 32);
    static pv_captureformat CreateFormat_PV(uint16_t width, uint16_t height, uint8_t framerate, bool enable_mrc, bool shared);
    static ea_captureformat CreateFormat_EA(MIXER_MODE mode, uint8_t device_index, uint8_t source_index);
    static ev_captureformat CreateFormat_EV(uint16_t width, uint16_t height, uint8_t framerate, wchar_t const* subtype, bool shared, uint32_t group_index, uint32_t source_index, uint32_t profile_index);
    static void PV_SetFocus(PV_FocusMode focusmode, PV_AutoFocusRange autofocusrange, PV_ManualFocusDistance distance, uint32_t value, PV_DriverFallback disabledriverfallback);
    static void PV_SetVideoTemporalDenoising(PV_VideoTemporalDenoisingMode mode);
    static void PV_SetWhiteBalance_Preset(PV_ColorTemperaturePreset preset);
    static void PV_SetWhiteBalance_Value(uint32_t value);
    static void PV_SetExposure(PV_ExposureMode setauto, uint32_t value);
    static void PV_SetExposurePriorityVideo(PV_ExposurePriorityVideo enabled);
    static void PV_SetSceneMode(PV_CaptureSceneMode mode);
    static void PV_SetIsoSpeed(PV_IsoSpeedMode setauto, uint32_t value);
    static void PV_SetBacklightCompensation(PV_BacklightCompensationState enable);
    static void IMT_ZHTInvalidate(uint16_t const* depth_in, uint16_t* depth_out);
    static void IMT_ZLTInvalidate(uint8_t const* sigma_in, uint16_t const* depth_in, uint16_t* depth_out);
    static void IMT_YUV2RGB(uint8_t const* image, uint32_t stride, uint32_t height, IMT_Format format_in, IMT_Format format_out, void** fc);
    static void IMT_Extract(void* fc, void const** buffer, int32_t* length);
    static void IMT_Release(void* fc);
};

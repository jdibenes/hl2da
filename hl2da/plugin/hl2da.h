
#pragma once

#include <stdint.h>

#define PLUGIN_IMPORT extern "C" __declspec(dllimport)

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
void BypassDepthLock_RM(int bypass);

PLUGIN_IMPORT
void SetFormat_PV(void const* cf);

PLUGIN_IMPORT
void SetFormat_MC(int raw);

PLUGIN_IMPORT
void SetFormat_EE(int fps_index);

PLUGIN_IMPORT
void SetFormat_EA(void const* cf);

PLUGIN_IMPORT
void SetFormat_EV(void const* cf);

PLUGIN_IMPORT
uint64_t GetUTCOffset(int32_t samples);

PLUGIN_IMPORT
void PV_SetFocus(uint32_t focusmode, uint32_t autofocusrange, uint32_t distance, uint32_t value, uint32_t disabledriverfallback);

PLUGIN_IMPORT
void PV_SetVideoTemporalDenoising(uint32_t mode);

PLUGIN_IMPORT
void PV_SetWhiteBalance_Preset(uint32_t preset);

PLUGIN_IMPORT
void PV_SetWhiteBalance_Value(uint32_t value);

PLUGIN_IMPORT
void PV_SetExposure(uint32_t setauto, uint32_t value);

PLUGIN_IMPORT
void PV_SetExposurePriorityVideo(uint32_t enabled);

PLUGIN_IMPORT
void PV_SetSceneMode(uint32_t mode);

PLUGIN_IMPORT
void PV_SetIsoSpeed(uint32_t setauto, uint32_t value);

PLUGIN_IMPORT
void PV_SetBacklightCompensation(uint32_t enable);

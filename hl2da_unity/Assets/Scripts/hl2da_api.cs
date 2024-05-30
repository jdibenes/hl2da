
using System;
using System.Runtime.InteropServices;

public static class hl2da_api
{
    public enum SENSOR_ID
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

    [StructLayout(LayoutKind.Explicit)]
    public struct AccelDataStruct
    {
        [FieldOffset( 0)] public ulong VinylHupTicks;
        [FieldOffset( 8)] public ulong SocTicks;
        [FieldOffset(16)] public float x;
        [FieldOffset(20)] public float y;
        [FieldOffset(24)] public float z;
        [FieldOffset(28)] public float temperature;
        // 32
    };

    [StructLayout(LayoutKind.Explicit)]
    public struct GyroDataStruct
    {
        [FieldOffset( 0)] public ulong VinylHupTicks;
        [FieldOffset( 8)] public ulong SocTicks;
        [FieldOffset(16)] public float x;
        [FieldOffset(20)] public float y;
        [FieldOffset(24)] public float z;
        [FieldOffset(28)] public float temperature;
        // 32
    };

    [StructLayout(LayoutKind.Explicit)]
    public struct MagDataStruct
    {
        [FieldOffset( 0)] public ulong VinylHupTicks;
        [FieldOffset( 8)] public ulong SocTicks;
        [FieldOffset(16)] public float x;
        [FieldOffset(20)] public float y;
        [FieldOffset(24)] public float z;
        [FieldOffset(28)] private float _pad;
        // 32
    };

    [StructLayout(LayoutKind.Explicit)]
    public struct JointPose
    {
        [FieldOffset( 0)] public float rx;
        [FieldOffset( 4)] public float ry;
        [FieldOffset( 8)] public float rz;
        [FieldOffset(12)] public float rw;
        [FieldOffset(16)] public float tx;
        [FieldOffset(20)] public float ty;
        [FieldOffset(24)] public float tz;
        [FieldOffset(28)] public float radius;
        [FieldOffset(32)] public int accuracy;
        // 36
    }

    public enum STATUS
    {
        DISCARDED = -1,
        OK = 0,
        WAIT = 1,
    }

    public enum TIME_PREFERENCE
    {
        PAST = -1,
        NEAREST = 0,
        FUTURE = 1,
    }

    public enum MC_CHANNELS
    {
        USE_2 = 0,
        USE_5 = 1,
    }

    public enum EE_FPS_INDEX
    {
        FPS_30 = 0,
        FPS_60 = 1,
        FPS_90 = 2,
    }

    [Flags]
    public enum SI_VALID
    {
        HEAD = 1,
        EYE = 2,
        LEFT = 4,
        RIGHT = 8,
    }

    [Flags]
    public enum EE_VALID
    {
        CALIBRATION = 1,
        COMBINED_GAZE = 2,
        LEFT_GAZE = 4,
        RIGHT_GAZE = 8,
        LEFT_OPENNESS = 16,
        RIGHT_OPENNESS = 32,
        VERGENCE_DISTANCE = 64,
    }

    public enum HOLOGRAM_PERSPECTIVE
    {
        DISPLAY = 0,
        PV = 1,
    }

    [StructLayout(LayoutKind.Explicit)]
    public struct pv_captureformat
    {
        [FieldOffset( 0)] public byte enable_mrc;
        [FieldOffset( 1)] public byte hologram_composition;
        [FieldOffset( 2)] public byte recording_indicator;
        [FieldOffset( 3)] public byte video_stabilization;
        [FieldOffset( 4)] public byte blank_protected;
        [FieldOffset( 5)] public byte show_mesh;
        [FieldOffset( 6)] public byte shared;
        [FieldOffset( 7)] private byte _reserved_0;
        [FieldOffset( 8)] public float global_opacity;
        [FieldOffset(12)] public float output_width;
        [FieldOffset(16)] public float output_height;
        [FieldOffset(20)] public uint video_stabilization_length;
        [FieldOffset(24)] public uint hologram_perspective;
        [FieldOffset(28)] public ushort width;
        [FieldOffset(30)] public ushort height;
        [FieldOffset(32)] public byte framerate;
        [FieldOffset(33)] private byte _pad_0;
        [FieldOffset(34)] private ushort _reserved_1;
        // 36
    }

    [DllImport("hl2da")]
    public static extern void Copy(IntPtr source, IntPtr destination, int bytes);

    [DllImport("hl2da")]
    public static extern void DebugMessage(string str);

    [DllImport("hl2da")]
    public static extern void InitializeComponentsOnUI();

    [DllImport("hl2da")]
    public static extern int OverrideWorldCoordinateSystem(IntPtr scs);

    [DllImport("hl2da")]
    public static extern void Initialize(int id, int buffer_size);

    [DllImport("hl2da")]
    public static extern void SetEnable(int id, int enable);

    [DllImport("hl2da")]
    public static extern int GetByFramestamp(int id, int stamp, ref IntPtr frame, ref ulong timestamp, ref int framestamp);

    [DllImport("hl2da")]
    public static extern int GetByTimestamp(int id, ulong stamp, int time_preference, int tiebreak_right, ref IntPtr frame, ref ulong timestamp, ref int framestamp);

    [DllImport("hl2da")]
    public static extern void Release(int id, IntPtr frame);

    [DllImport("hl2da")]
    public static extern void Extract(int id, IntPtr frame, ref int valid, IntPtr[] b, int[] l);

    [DllImport("hl2da")]
    public static extern void GetExtrinsics_RM(int id, IntPtr extrinsics);

    [DllImport("hl2da")]
    public static extern void MapImagePointToCameraUnitPlane_RM(int id, IntPtr image_points, IntPtr camera_points, int point_count);

    [DllImport("hl2da")]
    public static extern void MapCameraSpaceToImagePoint_RM(int id, IntPtr camera_points, IntPtr image_points, int point_count);

    [DllImport("hl2da")]
    public static extern void SetFormat_PV(ref pv_captureformat cf);

    [DllImport("hl2da")]
    public static extern void SetFormat_MC(int raw);

    [DllImport("hl2da")]
    public static extern void SetFormat_EE(int fps_index);
}


using System;
using System.Runtime.InteropServices;
using UnityEngine;

public static class hl2da
{
#region PLUGIN_INTERFACE

#if WINDOWS_UWP
    [DllImport("hl2da")]
    private static extern void Copy(IntPtr source, IntPtr destination, int bytes);

    [DllImport("hl2da")]
    private static extern void DebugMessage(string str);

    [DllImport("hl2da")]
    private static extern void InitializeComponentsOnUI();

    [DllImport("hl2da")]
    private static extern int OverrideWorldCoordinateSystem(IntPtr scs);

    [DllImport("hl2da")]
    private static extern void Initialize(int id, int buffer_size);

    [DllImport("hl2da")]
    private static extern void SetEnable(int id, int enable);

    [DllImport("hl2da")]
    private static extern int GetByFramestamp(int id, int stamp, ref IntPtr frame, ref ulong timestamp, ref int framestamp);

    [DllImport("hl2da")]
    private static extern int GetByTimestamp(int id, ulong stamp, int time_preference, int tiebreak_right, ref IntPtr frame, ref ulong timestamp, ref int framestamp);

    [DllImport("hl2da")]
    private static extern void Release(int id, IntPtr frame);

    [DllImport("hl2da")]
    private static extern void Extract(int id, IntPtr frame, ref int valid, IntPtr[] b, int[] l);

    [DllImport("hl2da")]
    private static extern void GetExtrinsics_RM(int id, IntPtr extrinsics);

    [DllImport("hl2da")]
    private static extern void MapImagePointToCameraUnitPlane_RM(int id, IntPtr image_points, IntPtr camera_points, int point_count);

    [DllImport("hl2da")]
    private static extern void MapCameraSpaceToImagePoint_RM(int id, IntPtr camera_points, IntPtr image_points, int point_count);

    [DllImport("hl2da")]
    private static extern void SetFormat_PV(ref pv_captureformat cf);

    [DllImport("hl2da")]
    private static extern void SetFormat_MC(int raw);

    [DllImport("hl2da")]
    private static extern void SetFormat_EE(int fps_index);
#else
    private static void Copy(IntPtr source, IntPtr destination, int bytes)
    {

    }

    private static void DebugMessage(string str)
    {

    }

    private static void InitializeComponentsOnUI()
    {

    }

    private static int OverrideWorldCoordinateSystem(IntPtr scs)
    {
        return 0;
    }

    private static void Initialize(int id, int buffer_size)
    {

    }

    private static void SetEnable(int id, int enable)
    {

    }

    private static int GetByFramestamp(int id, int stamp, ref IntPtr frame, ref ulong timestamp, ref int framestamp)
    {
        return -1;
    }

    private static int GetByTimestamp(int id, ulong stamp, int time_preference, int tiebreak_right, ref IntPtr frame, ref ulong timestamp, ref int framestamp)
    {
        return -1;
    }

    private static void Release(int id, IntPtr frame)
    {

    }

    private static void Extract(int id, IntPtr frame, ref int valid, IntPtr[] b, int[] l)
    {

    }

    private static void GetExtrinsics_RM(int id, IntPtr extrinsics)
    {

    }

    private static void MapImagePointToCameraUnitPlane_RM(int id, IntPtr image_points, IntPtr camera_points, int point_count)
    {

    }

    private static void MapCameraSpaceToImagePoint_RM(int id, IntPtr camera_points, IntPtr image_points, int point_count)
    {

    }

    private static void SetFormat_PV(ref pv_captureformat cf)
    {

    }

    private static void SetFormat_MC(int raw)
    {

    }

    private static void SetFormat_EE(int fps_index)
    {

    }

#endif

#endregion PLUGIN_INTERFACE

#region UNITY_LAYER

    public enum sensor_id
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

    public enum get_status
    {
        DISCARDED = -1,
        OK = 0,
        WAIT = 1,
    }

    public enum search_preference
    {
        PAST = -1,
        NEAREST = 0,
        FUTURE = 1,
    }

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
    public struct pv_captureformat
    {
        [FieldOffset( 0)] public byte enable;
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

    public class frame_buffer
    {
        public IntPtr frame;

        public get_status status;
        public sensor_id id;
        public ulong timestamp;
        public int framestamp;
        public int valid;

        public IntPtr[] buffer;
        public int[] length;

        private void reset()
        {
            frame = IntPtr.Zero;

            status     = (get_status)(-1);
            id         = (sensor_id)(-1);
            timestamp  = 0;
            framestamp = 0;
            valid  = 0;

            buffer = new IntPtr[4];
            length = new int[4];
        }

        public frame_buffer()
        {
            reset();
        }

        public void Destroy()
        {
            if (frame == IntPtr.Zero) { return; }
            Release((int)id, frame);
            reset();
        }

        ~frame_buffer()
        {
            Destroy();
        }
    };

    public static void Copy<T>(IntPtr source, IntPtr destination, int length)
    {
        Copy(source, destination, length * Marshal.SizeOf(typeof(T)));
    }

    public static void PrintDebugMessage(string str)
    {
        DebugMessage(str);
    }

    public static void InitializeComponents()
    {
        InitializeComponentsOnUI();
    }

    public static bool OverrideWorldCoordinateSystem()
    {
        var scs = Microsoft.MixedReality.OpenXR.PerceptionInterop.GetSceneCoordinateSystem(Pose.identity);
        if (scs == null) { return false; }
        var unk = Marshal.GetIUnknownForObject(scs);
        bool ret = OverrideWorldCoordinateSystem(unk) != 0;
        Marshal.Release(unk);
        return ret;
    }

    public static void InitializeStream(sensor_id id, int buffer_size)
    {
        Initialize((int)id, buffer_size);
    }

    public static void SetStreamEnable(sensor_id id, bool enable)
    {
        SetEnable((int)id, enable ? 1 : 0);
    }

    private static void Extract(frame_buffer fb)
    {
        Extract((int)fb.id, fb.frame, ref fb.valid, fb.buffer, fb.length);
    }

    public static frame_buffer GetStreamFrame(sensor_id id, int framestamp)
    {
        frame_buffer fb = new frame_buffer();
        fb.id = id;
        fb.status = (get_status)GetByFramestamp((int)fb.id, framestamp, ref fb.frame, ref fb.timestamp, ref fb.framestamp);
        if (fb.frame != IntPtr.Zero) { Extract(fb); }
        return fb;
    }

    public static frame_buffer GetStreamFrame(sensor_id id, ulong timestamp, search_preference time_preference, bool tiebreak_right)
    {
        frame_buffer fb = new frame_buffer();
        fb.id = id;
        fb.status = (get_status)GetByTimestamp((int)fb.id, timestamp, (int)time_preference, tiebreak_right ? 1 : 0, ref fb.frame, ref fb.timestamp, ref fb.framestamp);
        if (fb.frame != IntPtr.Zero) { Extract(fb); }
        return fb;
    }

    public static void GetSensorExtrinsics(sensor_id id, IntPtr extrinsics)
    {
        GetExtrinsics_RM((int)id, extrinsics);
    }

    public static void MapImagePointToCameraUnitPlane(sensor_id id, IntPtr image_points, IntPtr camera_points, int point_count)
    {
        MapImagePointToCameraUnitPlane_RM((int)id, image_points, camera_points, point_count);
    }

    public static void MapCameraSpaceToImagePoint(sensor_id id, IntPtr camera_points, IntPtr image_points, int point_count)
    {
        MapCameraSpaceToImagePoint_RM((int)id, camera_points, image_points, point_count);
    }

    public static void SetStreamFormat_PV(pv_captureformat cf)
    {
        SetFormat_PV(ref cf);
    }

    public static void SetStreamFormat_Microphone(bool raw)
    {
        SetFormat_MC(raw ? 1 : 0);
    }

    public static void SetStreamFormat_ExtendedEyeTracking(int fps_index)
    {
        SetFormat_EE(fps_index);
    }

#endregion UNITY_LAYER

#region UNITY_LAYER_UTILITIES
    
    public const int RM_VLC_WIDTH  = 640;
    public const int RM_VLC_HEIGHT = 480;
    public const int RM_VLC_FPS    = 30;

    public const int RM_DEPTH_AHAT_WIDTH  = 512;
    public const int RM_DEPTH_AHAT_HEIGHT = 512;
    public const int RM_DEPTH_AHAT_FPS    = 45;

    public const int RM_DEPTH_LONGTHROW_WIDTH  = 320;
    public const int RM_DEPTH_LONGTHROW_HEIGHT = 288;
    public const int RM_DEPTH_LONGTHROW_FPS    = 5;

    public const int SI_HEAD_FLOATS = 9;
    public const int SI_EYE_FLOATS = 6;
    public const int SI_HAND_JOINTS = 26;

    public const int POSE_ROWS = 4;
    public const int POSE_COLS = 4;

    public static void Copy<T>(IntPtr source, object destination, int length)
    {
        GCHandle h = GCHandle.Alloc(destination, GCHandleType.Pinned);
        Copy<T>(source, h.AddrOfPinnedObject(), length);
        h.Free();
    }

    public static T[] Unpack1D<T>(IntPtr source, int length)
    {
        T[] destination = new T[length];
        Copy<T>(source, destination, destination.Length);
        return destination;
    }

    public static T[,] Unpack2D<T>(IntPtr source, int length_0, int length_1)
    {
        T[,] destination = new T[length_0, length_1];
        Copy<T>(source, destination, destination.Length);
        return destination;
    }

    public static float[,] GetSensorExtrinsics(sensor_id id)
    {
        float[,] extrinsics = new float[POSE_ROWS, POSE_COLS];
        GCHandle h = GCHandle.Alloc(extrinsics, GCHandleType.Pinned);
        GetSensorExtrinsics(id, h.AddrOfPinnedObject());
        h.Free();
        return extrinsics;
    }

    public static float[,] MapImagePointToCameraUnitPlane(sensor_id id, float[,] image_points)
    {
        float[,] camera_points = new float[image_points.GetLength(0), image_points.GetLength(1)];

        GCHandle hi = GCHandle.Alloc(image_points,  GCHandleType.Pinned);
        GCHandle ho = GCHandle.Alloc(camera_points, GCHandleType.Pinned);

        MapImagePointToCameraUnitPlane(id, hi.AddrOfPinnedObject(), ho.AddrOfPinnedObject(), image_points.Length / 2);

        hi.Free();
        ho.Free();

        return camera_points;
    }

    public static float[,] MapCameraSpaceToImagePoint(sensor_id id, float[,] camera_points)
    {
        float[,] image_points = new float[camera_points.GetLength(0), camera_points.GetLength(1)];

        GCHandle hi = GCHandle.Alloc(camera_points, GCHandleType.Pinned);
        GCHandle ho = GCHandle.Alloc(image_points,  GCHandleType.Pinned);        

        MapCameraSpaceToImagePoint(id, hi.AddrOfPinnedObject(), ho.AddrOfPinnedObject(), camera_points.Length / 2);

        hi.Free();
        ho.Free();

        return image_points;
    }

    public static pv_captureformat CreateFormat_PV(ushort width, ushort height, byte framerate, bool enable_mrc, bool shared)
    {
        pv_captureformat cf = new pv_captureformat();

        cf.enable = enable_mrc ? (byte)1 : (byte)0;
        cf.hologram_composition = 1;
        cf.recording_indicator = 0;
        cf.video_stabilization = 0;
        cf.blank_protected = 0;
        cf.show_mesh = 0;
        cf.shared = shared ? (byte)1 : (byte)0;
        cf.global_opacity = 0.9f;
        cf.output_width = 0.0f;
        cf.output_height = 0.0f;
        cf.video_stabilization_length = 0;
        cf.hologram_perspective = 1;
        cf.width = width;
        cf.height = height;
        cf.framerate = framerate;

        return cf;
    }

#endregion
}

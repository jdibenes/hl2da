
using System;
using System.Runtime.InteropServices;

public static class hl2da
{
#if WINDOWS_UWP
    [DllImport("hl2da")]
    private static extern void DebugMessage(string str);

    [DllImport("hl2da")]
    private static extern void Unpack_U8(IntPtr source, byte[] destination, int elements);

    [DllImport("hl2da")]
    private static extern void Unpack_U16(IntPtr source, ushort[] destination, int elements);

    [DllImport("hl2da")]
    private static extern void Unpack_U32(IntPtr source, uint[] destination, int elements);

    [DllImport("hl2da")]
    private static extern void Unpack_U64(IntPtr source, ulong[] destination, int elements);

    [DllImport("hl2da")]
    private static extern void Unpack_RM_IMU_Accelerometer(IntPtr source, AccelDataStruct[] destination, int elements);

    [DllImport("hl2da")]
    private static extern void Unpack_RM_IMU_Gyroscope(IntPtr source, GyroDataStruct[] destination, int elements);

    [DllImport("hl2da")]
    private static extern void Unpack_RM_IMU_Magnetometer(IntPtr source, MagDataStruct[] destination, int elements);

    [DllImport("hl2da")]
    private static extern void InitializeGlobal();

    [DllImport("hl2da")]
    private static extern void Initialize(int id, int buffer_size);

    [DllImport("hl2da")]
    private static extern void SetEnable(int id, int enable);

    [DllImport("hl2da")]
    private static extern int GetByFramestamp(int id, int stamp, ref IntPtr frame, ref ulong timestamp, ref int framestamp);

    [DllImport("hl2da")]
    private static extern int GetByTimestamp(int id, ulong stamp, int time_preference, int tiebreak_right, ref IntPtr frame, ref ulong timestamp, ref int framestamp);

    [DllImport("hl2da")]
    private static extern void Release_RM(IntPtr frame);

    [DllImport("hl2da")]
    private static extern void Extract_RM_VLC(IntPtr frame, out IntPtr buffer, out int length, out IntPtr pose_buffer, out int pose_length);

    [DllImport("hl2da")]
    private static extern void Extract_RM_Depth_AHAT(IntPtr frame, out IntPtr buffer, out int length, out IntPtr ab_depth_buffer, out int ab_depth_length, out IntPtr pose_buffer, out int pose_length);

    [DllImport("hl2da")]
    private static extern void Extract_RM_Depth_Longthrow(IntPtr frame, out IntPtr buffer, out int length, out IntPtr ab_depth_buffer, out int ab_depth_length, out IntPtr sigma_buffer, out int sigma_length, out IntPtr pose_buffer, out int pose_length);

    [DllImport("hl2da")]
    private static extern void Extract_RM_IMU_Accelerometer(IntPtr frame, out IntPtr buffer, out int length, out IntPtr pose_buffer, out int pose_length);

    [DllImport("hl2da")]
    private static extern void Extract_RM_IMU_Gyroscope(IntPtr frame, out IntPtr buffer, out int length, out IntPtr pose_buffer, out int pose_length);

    [DllImport("hl2da")]
    private static extern void Extract_RM_IMU_Magnetometer(IntPtr frame, out IntPtr buffer, out int length, out IntPtr pose_buffer, out int pose_length);

    [DllImport("hl2da")]
    private static extern void GetExtrinsics_RM(int id, float[,] extrinsics);

    [DllImport("hl2da")]
    private static extern void MapImagePointToCameraUnitPlane_RM(int id, float[,] image_points, float[,] camera_points, int point_count);

    [DllImport("hl2da")]
    private static extern void MapCameraSpaceToImagePoint_RM(int id, float[,] camera_points, float[,] image_points, int point_count);
#else
    private static void DebugMessage(string str)
    {

    }

    private static void Unpack_U8(IntPtr source, byte[] destination, int elements)
    {

    }

    private static void Unpack_U16(IntPtr source, ushort[] destination, int elements)
    {

    }

    private static void Unpack_U32(IntPtr source, uint[] destination, int elements)
    {

    }

    private static void Unpack_U64(IntPtr source, ulong[] destination, int elements)
    {

    }

    private static void Unpack_RM_IMU_Accelerometer(IntPtr source, AccelDataStruct[] destination, int elements)
    {

    }

    private static void Unpack_RM_IMU_Gyroscope(IntPtr source, GyroDataStruct[] destination, int elements)
    {

    }

    private static void Unpack_RM_IMU_Magnetometer(IntPtr source, MagDataStruct[] destination, int elements)
    {

    }

    private static void InitializeGlobal()
    {

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

    private static void Release_RM(IntPtr frame)
    {

    }

    private static void Extract_RM_VLC(IntPtr frame, out IntPtr buffer, out int length, out IntPtr pose_buffer, out int pose_length)
    {
        buffer = IntPtr.Zero;
        length = 0;
        pose_buffer = IntPtr.Zero;
        pose_length = 0;
    }

    private static void Extract_RM_Depth_AHAT(IntPtr frame, out IntPtr buffer, out int length, out IntPtr ab_depth_buffer, out int ab_depth_length, out IntPtr pose_buffer, out int pose_length)
    {
        buffer = IntPtr.Zero;
        length = 0;
        ab_depth_buffer = IntPtr.Zero;
        ab_depth_length = 0;
        pose_buffer = IntPtr.Zero;
        pose_length = 0;
    }

    private static void Extract_RM_Depth_Longthrow(IntPtr frame, out IntPtr buffer, out int length, out IntPtr ab_depth_buffer, out int ab_depth_length, out IntPtr sigma_buffer, out int sigma_length, out IntPtr pose_buffer, out int pose_length)
    {
        buffer = IntPtr.Zero;
        length = 0;
        ab_depth_buffer = IntPtr.Zero;
        ab_depth_length = 0;
        sigma_buffer = IntPtr.Zero;
        sigma_length = 0;
        pose_buffer = IntPtr.Zero;
        pose_length = 0;
    }

    private static void Extract_RM_IMU_Accelerometer(IntPtr frame, out IntPtr buffer, out int length, out IntPtr pose_buffer, out int pose_length)
    {
        buffer = IntPtr.Zero;
        length = 0;
        pose_buffer = IntPtr.Zero;
        pose_length = 0;
    }

    private static void Extract_RM_IMU_Gyroscope(IntPtr frame, out IntPtr buffer, out int length, out IntPtr pose_buffer, out int pose_length)
    {
        buffer = IntPtr.Zero;
        length = 0;
        pose_buffer = IntPtr.Zero;
        pose_length = 0;
    }

    private static void Extract_RM_IMU_Magnetometer(IntPtr frame, out IntPtr buffer, out int length, out IntPtr pose_buffer, out int pose_length)
    {
        buffer = IntPtr.Zero;
        length = 0;
        pose_buffer = IntPtr.Zero;
        pose_length = 0;
    }

    private static void GetExtrinsics_RM(int id, float[,] extrinsics)
    {

    }

    private static void MapImagePointToCameraUnitPlane_RM(int id, float[,] image_points, float[,] camera_points, int point_count)
    {

    }

    private static void MapCameraSpaceToImagePoint_RM(int id, float[,] camera_points, float[,] image_points, int point_count)
    {

    }
#endif

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
        RM_IMU_MAGNETOMETER
    };

    public enum get_status
    {
        DISCARDED = -1,
        OK = 0,
        NOT_YET_ARRIVED = 1,
    }

    public enum search_preference
    {
        PAST = -1,
        NEAREST = 0,
        FUTURE = 1
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

    public class frame_buffer
    {
        public IntPtr frame;

        public get_status status;
        public sensor_id id;
        public ulong timestamp;
        public int framestamp;
        private int _reserved;

        public IntPtr buffer;
        public IntPtr ab_depth_buffer;
        public IntPtr sigma_buffer;
        public IntPtr pose_buffer;

        public int length;        
        public int ab_depth_length;        
        public int sigma_length;        
        public int pose_length;

        private void reset()
        {
            frame = IntPtr.Zero;

            status     = (get_status)(-1);
            id         = (sensor_id)(-1);
            timestamp  = 0;
            framestamp = 0;
            _reserved  = 0;

            buffer          = IntPtr.Zero;
            ab_depth_buffer = IntPtr.Zero;
            sigma_buffer    = IntPtr.Zero;
            pose_buffer     = IntPtr.Zero;

            length          = 0;
            ab_depth_length = 0;
            sigma_length    = 0;
            pose_length     = 0;
        }

        public frame_buffer()
        {
            reset();
        }

        public void Destroy()
        {
            if (frame == IntPtr.Zero) { return; }
            Release_RM(frame);
            reset();
        }

        ~frame_buffer()
        {
            Destroy();
        }
    };

    public static void Print(string str)
    {
        DebugMessage(str);
    }

    public static void InitializeComponents()
    {
        InitializeGlobal();
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
        switch (fb.id)
        {
        case sensor_id.RM_VLC_LEFTFRONT:     Extract_RM_VLC(fb.frame, out fb.buffer, out fb.length, out fb.pose_buffer, out fb.pose_length); break;
        case sensor_id.RM_VLC_LEFTLEFT:      Extract_RM_VLC(fb.frame, out fb.buffer, out fb.length, out fb.pose_buffer, out fb.pose_length); break;
        case sensor_id.RM_VLC_RIGHTFRONT:    Extract_RM_VLC(fb.frame, out fb.buffer, out fb.length, out fb.pose_buffer, out fb.pose_length); break;
        case sensor_id.RM_VLC_RIGHTRIGHT:    Extract_RM_VLC(fb.frame, out fb.buffer, out fb.length, out fb.pose_buffer, out fb.pose_length); break;
        case sensor_id.RM_DEPTH_AHAT:        Extract_RM_Depth_AHAT(fb.frame, out fb.buffer, out fb.length, out fb.ab_depth_buffer, out fb.ab_depth_length, out fb.pose_buffer, out fb.pose_length); break;
        case sensor_id.RM_DEPTH_LONGTHROW:   Extract_RM_Depth_Longthrow(fb.frame, out fb.buffer, out fb.length, out fb.ab_depth_buffer, out fb.ab_depth_length, out fb.sigma_buffer, out fb.sigma_length, out fb.pose_buffer, out fb.pose_length); break;
        case sensor_id.RM_IMU_ACCELEROMETER: Extract_RM_IMU_Accelerometer(fb.frame, out fb.buffer, out fb.length, out fb.pose_buffer, out fb.pose_length); break;
        case sensor_id.RM_IMU_GYROSCOPE:     Extract_RM_IMU_Gyroscope(fb.frame, out fb.buffer, out fb.length, out fb.pose_buffer, out fb.pose_length); break;
        case sensor_id.RM_IMU_MAGNETOMETER:  Extract_RM_IMU_Magnetometer(fb.frame, out fb.buffer, out fb.length, out fb.pose_buffer, out fb.pose_length); break;
        }
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

    public static void GetSensorExtrinsics(sensor_id id, float[,] extrinsics)
    {
        GetExtrinsics_RM((int)id, extrinsics);
    }

    public static void MapImagePointToCameraUnitPlane(sensor_id id, float[,] image_points, float[,] camera_points)
    {
        MapImagePointToCameraUnitPlane_RM((int)id, image_points, camera_points, image_points.GetLength(0));
    }

    public static void MapCameraSpaceToImagePoint(sensor_id id, float[,] camera_points, float[,] image_points)
    {
        MapCameraSpaceToImagePoint_RM((int)id, camera_points, image_points, image_points.GetLength(0));
    }

    public static void Unpack(IntPtr source, int length, out byte[] destination)
    {
        destination = new byte[length];
        Unpack_U8(source, destination, length);
    }

    public static void Unpack(IntPtr source, int length, out ushort[] destination)
    {
        destination = new ushort[length];
        Unpack_U16(source, destination, length);
    }

    public static void Unpack(IntPtr source, int length, out uint[] destination)
    {
        destination = new uint[length];
        Unpack_U32(source, destination, length);
    }

    public static void Unpack(IntPtr source, int length, out ulong[] destination)
    {
        destination = new ulong[length];
        Unpack_U64(source, destination, length);
    }

    public static void Unpack(IntPtr source, int length, out AccelDataStruct[] destination)
    {
        destination = new AccelDataStruct[length];
        Unpack_RM_IMU_Accelerometer(source, destination, length);
    }

    public static void Unpack(IntPtr source, int length, out GyroDataStruct[] destination)
    {
        destination = new GyroDataStruct[length];
        Unpack_RM_IMU_Gyroscope(source, destination, length);
    }

    public static void Unpack(IntPtr source, int length, out MagDataStruct[] destination)
    {
        destination = new MagDataStruct[length];
        Unpack_RM_IMU_Magnetometer(source, destination, length);
    }
}


using System;
using System.Runtime.InteropServices;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public static class hl2da
{
#if WINDOWS_UWP
    [DllImport("hl2da")]
    private static extern void DebugMessage(string str);

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
#else
    private static void DebugMessage(string str)
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
#endif
    public static void Print(string str)
    {
        DebugMessage(str);
    }

    public static void InitializeComponents()
    {
        InitializeGlobal();
    }

    public static void InitializeStream(int id, int buffer_size)
    {
        Initialize(id, buffer_size);
    }

    public static void SetStreamEnable(int id, bool enable)
    {
        SetEnable(id, enable ? 1 : 0);
    }

    private static void Extract(frame_buffer fb)
    {
        if (fb.frame == IntPtr.Zero) { return; }

        switch (fb.id)
        {
        case 0: Extract_RM_VLC(fb.frame, out fb.buffer, out fb.length, out fb.pose_buffer, out fb.pose_length); break;
        case 1: Extract_RM_VLC(fb.frame, out fb.buffer, out fb.length, out fb.pose_buffer, out fb.pose_length); break;
        case 2: Extract_RM_VLC(fb.frame, out fb.buffer, out fb.length, out fb.pose_buffer, out fb.pose_length); break;
        case 3: Extract_RM_VLC(fb.frame, out fb.buffer, out fb.length, out fb.pose_buffer, out fb.pose_length); break;
        case 4: Extract_RM_Depth_AHAT(fb.frame, out fb.buffer, out fb.length, out fb.ab_depth_buffer, out fb.ab_depth_length, out fb.pose_buffer, out fb.pose_length); break;
        case 5: Extract_RM_Depth_Longthrow(fb.frame, out fb.buffer, out fb.length, out fb.ab_depth_buffer, out fb.ab_depth_length, out fb.sigma_buffer, out fb.sigma_length, out fb.pose_buffer, out fb.pose_length); break;
        case 6: Extract_RM_IMU_Accelerometer(fb.frame, out fb.buffer, out fb.length, out fb.pose_buffer, out fb.pose_length); break;
        case 7: Extract_RM_IMU_Gyroscope(fb.frame, out fb.buffer, out fb.length, out fb.pose_buffer, out fb.pose_length); break;
        case 8: Extract_RM_IMU_Magnetometer(fb.frame, out fb.buffer, out fb.length, out fb.pose_buffer, out fb.pose_length); break;
        }
    }

    public static frame_buffer GetStreamFrame(int id, int stamp)
    {
        frame_buffer fb = new frame_buffer();
        fb.status = GetByFramestamp(id, stamp, ref fb.frame, ref fb.timestamp, ref fb.framestamp);
        fb.id = id;
        Extract(fb);
        return fb;
    }

    public static frame_buffer GetStreamFrame(int id, ulong stamp, int time_preference, int tiebreak_right)
    {
        frame_buffer fb = new frame_buffer();
        fb.status = GetByTimestamp(id, stamp, time_preference, tiebreak_right, ref fb.frame, ref fb.timestamp, ref fb.framestamp);
        fb.id = id;
        Extract(fb);
        return fb;
    }

    



    public class frame_buffer
    {
        public IntPtr frame;

        public int status;
        public int id;
        public ulong timestamp;
        public int framestamp;
        public int _reserved;

        public IntPtr buffer;
        public IntPtr ab_depth_buffer;
        public IntPtr sigma_buffer;
        public IntPtr pose_buffer;

        public int length;        
        public int ab_depth_length;        
        public int sigma_length;        
        public int pose_length;

        public frame_buffer()
        {
            frame = IntPtr.Zero;
            status = -1;
            id = -1;
            timestamp = 0;
            framestamp = 0;
            _reserved = 0;
            buffer = IntPtr.Zero;
            ab_depth_buffer = IntPtr.Zero;
            sigma_buffer = IntPtr.Zero;
            pose_buffer = IntPtr.Zero;
            length = 0;
            ab_depth_length = 0;
            sigma_length = 0;
            pose_length = 0;
        }

        public void Destroy()
        {
            if (frame == IntPtr.Zero) { return; }
            Release_RM(frame);
            frame = IntPtr.Zero;
        }

        ~frame_buffer()
        {
            Destroy();
        }
    };


    


}


using System;
using System.Runtime.InteropServices;
using UnityEngine;

public class hl2da_user
{
    public const int POSE_ROWS = 4;
    public const int POSE_COLS = 4;

    public static void Copy<T>(IntPtr source, IntPtr destination, int length)
    {
        hl2da_api.Copy(source, destination, length * Marshal.SizeOf(typeof(T)));
    }

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

    public static void InitializeComponents()
    {
        hl2da_api.InitializeComponentsOnUI();
    }

    public static bool OverrideWorldCoordinateSystem()
    {
        var scs = Microsoft.MixedReality.OpenXR.PerceptionInterop.GetSceneCoordinateSystem(Pose.identity);
        if (scs == null) { return false; }
        var unk = Marshal.GetIUnknownForObject(scs);
        bool ret = hl2da_api.OverrideWorldCoordinateSystem(unk) != 0;
        Marshal.Release(unk);
        return ret;
    }

    public static void Initialize(hl2da_api.SENSOR_ID id, int buffer_size)
    {
        hl2da_api.Initialize((int)id, buffer_size);
    }

    public static void SetEnable(hl2da_api.SENSOR_ID id, bool enable)
    {
        hl2da_api.SetEnable((int)id, enable ? 1 : 0);
    }

    public static float[,] GetExtrinsics_RM(hl2da_api.SENSOR_ID id)
    {
        float[,] extrinsics = new float[POSE_ROWS, POSE_COLS];
        GCHandle h = GCHandle.Alloc(extrinsics, GCHandleType.Pinned);
        hl2da_api.GetExtrinsics_RM((int)id, h.AddrOfPinnedObject());
        h.Free();
        return extrinsics;
    }

    public static float[,] MapImagePointToCameraUnitPlane_RM(hl2da_api.SENSOR_ID id, float[,] image_points)
    {
        float[,] camera_points = new float[image_points.GetLength(0), image_points.GetLength(1)];

        GCHandle hi = GCHandle.Alloc(image_points, GCHandleType.Pinned);
        GCHandle ho = GCHandle.Alloc(camera_points, GCHandleType.Pinned);

        hl2da_api.MapImagePointToCameraUnitPlane_RM((int)id, hi.AddrOfPinnedObject(), ho.AddrOfPinnedObject(), image_points.Length / 2);

        hi.Free();
        ho.Free();

        return camera_points;
    }

    public static float[,] MapCameraSpaceToImagePoint_RM(hl2da_api.SENSOR_ID id, float[,] camera_points)
    {
        float[,] image_points = new float[camera_points.GetLength(0), camera_points.GetLength(1)];

        GCHandle hi = GCHandle.Alloc(camera_points, GCHandleType.Pinned);
        GCHandle ho = GCHandle.Alloc(image_points, GCHandleType.Pinned);

        hl2da_api.MapCameraSpaceToImagePoint_RM((int)id, hi.AddrOfPinnedObject(), ho.AddrOfPinnedObject(), camera_points.Length / 2);

        hi.Free();
        ho.Free();

        return image_points;
    }

    public static void BypassDepthLock_RM(bool bypass)
    {
        hl2da_api.BypassDepthLock_RM(bypass ? 1 : 0);
    }

    public static void SetFormat_PV(hl2da_api.pv_captureformat cf)
    {
        hl2da_api.SetFormat_PV(ref cf);
    }

    public static void SetFormat_Microphone(hl2da_api.MC_CHANNELS set)
    {
        hl2da_api.SetFormat_MC((int)set);
    }

    public static void SetFormat_ExtendedEyeTracking(hl2da_api.EE_FPS_INDEX fps_index)
    {
        hl2da_api.SetFormat_EE((int)fps_index);
    }

    public static void SetFormat_ExtendedAudio(hl2da_api.ea_captureformat cf)
    {
        hl2da_api.SetFormat_EA(ref cf);
    }

    public static void SetFormat_ExtendedVideo(hl2da_api.ev_captureformat cf)
    {
        hl2da_api.SetFormat_EV(ref cf);
    }

    public static ulong GetUTCOffset(int samples = 32)
    {
        return hl2da_api.GetUTCOffset(samples);
    }

    public static hl2da_api.pv_captureformat CreateFormat_PV(ushort width, ushort height, byte framerate, bool enable_mrc, bool shared)
    {
        hl2da_api.pv_captureformat cf = new hl2da_api.pv_captureformat();

        cf.enable_mrc = enable_mrc ? (byte)1 : (byte)0;
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
        cf.hologram_perspective = (int)hl2da_api.HOLOGRAM_PERSPECTIVE.PV;
        cf.width = width;
        cf.height = height;
        cf.framerate = framerate;

        return cf;
    }

    public static hl2da_api.ea_captureformat CreateFormat_EA(hl2da_api.MIXER_MODE mode, byte device_index, byte source_index)
    {
        hl2da_api.ea_captureformat cf = new hl2da_api.ea_captureformat();

        cf.mixer_mode = (((uint)mode) & 0xFF) | (((uint)device_index) << 8) | (((uint)source_index) << 16);
        cf.loopback_gain = 1.0f;
        cf.microphone_gain = 1.0f;

        return cf;
    }

    public static hl2da_api.ev_captureformat CreateFormat_EV(ushort width, ushort height, byte framerate, string subtype, bool shared, uint group_index, uint source_index, uint profile_index)
    {
        hl2da_api.ev_captureformat cf = new hl2da_api.ev_captureformat();

        cf.enable_mrc = 0;
        cf.hologram_composition = 0;
        cf.recording_indicator = 0;
        cf.video_stabilization = 0;
        cf.blank_protected = 0;
        cf.show_mesh = 0;
        cf.shared = shared ? (byte)1 : (byte)0;
        cf.global_opacity = group_index;
        cf.output_width = source_index;
        cf.output_height = profile_index;
        cf.video_stabilization_length = 0;
        cf.hologram_perspective = 0;
        cf.width = width;
        cf.height = height;
        cf.framerate = framerate;
        cf.subtype = subtype;

        return cf;
    }

    public static hl2da_api.ea_audioformat UnpackFormat_EA(IntPtr buffer)
    {
        return Marshal.PtrToStructure<hl2da_api.ea_audioformat>(buffer);
    }

    public static hl2da_api.ev_videoformat UnpackFormat_EV(IntPtr buffer)
    {
        return Marshal.PtrToStructure<hl2da_api.ev_videoformat>(buffer);
    }

    public static void PV_SetFocus(hl2da_api.PV_FocusMode focusmode, hl2da_api.PV_AutoFocusRange autofocusrange, hl2da_api.PV_ManualFocusDistance distance, uint value, hl2da_api.PV_DriverFallback disabledriverfallback)
    {
        hl2da_api.PV_SetFocus((uint)focusmode, (uint)autofocusrange, (uint)distance, value, (uint)disabledriverfallback);
    }

    public static void PV_SetVideoTemporalDenoising(hl2da_api.PV_VideoTemporalDenoisingMode mode)
    {
        hl2da_api.PV_SetVideoTemporalDenoising((uint)mode);
    }

    public static void PV_SetWhiteBalance_Preset(hl2da_api.PV_ColorTemperaturePreset preset)
    {
        hl2da_api.PV_SetWhiteBalance_Preset((uint)preset);
    }

    public static void PV_SetWhiteBalance_Value(uint value)
    {
        hl2da_api.PV_SetWhiteBalance_Value(value);
    }

    public static void PV_SetExposure(hl2da_api.PV_ExposureMode setauto, uint value)
    {
        hl2da_api.PV_SetExposure((uint)setauto, value);
    }

    public static void PV_SetExposurePriorityVideo(hl2da_api.PV_ExposurePriorityVideo enabled)
    {
        hl2da_api.PV_SetExposurePriorityVideo((uint)enabled);
    }

    public static void PV_SetSceneMode(hl2da_api.PV_CaptureSceneMode mode)
    {
        hl2da_api.PV_SetSceneMode((uint)mode);
    }

    public static void PV_SetIsoSpeed(hl2da_api.PV_IsoSpeedMode setauto, uint value)
    {
        hl2da_api.PV_SetIsoSpeed((uint)setauto, value);
    }

    public static void PV_SetBacklightCompensation(hl2da_api.PV_BacklightCompensationState enable)
    {
        hl2da_api.PV_SetBacklightCompensation((uint)enable);
    }
}

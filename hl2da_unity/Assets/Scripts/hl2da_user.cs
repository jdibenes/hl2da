
using System;
using System.Runtime.InteropServices;
using UnityEngine;

public static partial class hl2da
{
    public class user
    {
        public const int POSE_ROWS = 4;
        public const int POSE_COLS = 4;

        public static void Copy<T>(IntPtr source, IntPtr destination, int length)
        {
            hl2da.Copy(source, destination, length * Marshal.SizeOf(typeof(T)));
        }

        public static void Copy<T>(IntPtr source, object destination, int length)
        {
            using (var p = pointer.get(destination)) { Copy<T>(source, p.value, length); }
        }

        public static T Unpack<T>(IntPtr p)
        {
            return Marshal.PtrToStructure<T>(p);
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
            hl2da.InitializeComponentsOnUI();
        }

        public static bool OverrideWorldCoordinateSystem()
        {
            var scs = Microsoft.MixedReality.OpenXR.PerceptionInterop.GetSceneCoordinateSystem(Pose.identity);
            if (scs == null) { return false; }
            var unk = Marshal.GetIUnknownForObject(scs);
            bool ret = hl2da.OverrideWorldCoordinateSystem(unk) != 0;
            Marshal.Release(unk);
            return ret;
        }

        public static ulong GetUTCOffset(int samples = 32)
        {
            return hl2da.GetUTCOffset(samples);
        }

        public static void Initialize(hl2da.SENSOR_ID id, int buffer_size)
        {
            hl2da.Initialize((int)id, buffer_size);
        }

        public static void SetConstantFactorVLC_RM(long factor = -125000)
        {
            hl2da.SetConstantFactorVLC_RM(factor);
        }

        public static void BypassDepthLock_RM(bool bypass)
        {
            hl2da.BypassDepthLock_RM(bypass ? 1 : 0);
        }

        public static void SetFormat_PV(hl2da.pv_captureformat cf)
        {
            hl2da.SetFormat_PV(ref cf);
        }

        public static void SetFormat_Microphone(hl2da.MC_CHANNELS set)
        {
            hl2da.SetFormat_MC((int)set);
        }

        public static void SetFormat_ExtendedEyeTracking(hl2da.EE_FPS_INDEX fps_index)
        {
            hl2da.SetFormat_EE((int)fps_index);
        }

        public static void SetFormat_ExtendedAudio(hl2da.ea_captureformat cf)
        {
            hl2da.SetFormat_EA(ref cf);
        }

        public static void SetFormat_ExtendedVideo(hl2da.ev_captureformat cf)
        {
            hl2da.SetFormat_EV(ref cf);
        }

        public static void SetEnable(hl2da.SENSOR_ID id, bool enable)
        {
            hl2da.SetEnable((int)id, enable ? 1 : 0);
        }

        public static void RM_SetEyeSelection(bool enable)
        {
            hl2da.RM_SetEyeSelection(enable ? 1 : 0);
        }

        public static void RM_GetIntrinsics(hl2da.SENSOR_ID id, out float[,] uv2xy, out float[,] mapxy, out float[] k)
        {
            RM_GetResolution(id, out int w, out int h);

            uv2xy = new float[2 * h, w];
            mapxy = new float[2 * h, w];
            k = new float[4];

            using (pointer h1 = pointer.get(uv2xy), h2 = pointer.get(mapxy), h3 = pointer.get(k)) { hl2da.RM_GetIntrinsics((int)id, h1.value, h2.value, h3.value); }
        }

        public static float[,] RM_GetExtrinsics(hl2da.SENSOR_ID id)
        {
            float[,] dst = new float[4, 4];
            using (pointer h = pointer.get(dst)) { hl2da.RM_GetExtrinsics((int)id, h.value); }
            return dst;
        }

        public static float[,] RM_MapImagePointToCameraUnitPlane(hl2da.SENSOR_ID id, float[,] image_points, int pitch = 2)
        {
            float[,] camera_points = new float[image_points.GetLength(0), image_points.GetLength(1)];
            using (pointer hi = pointer.get(image_points), ho = pointer.get(camera_points)) { hl2da.RM_MapImagePointToCameraUnitPlane((int)id, hi.value, pitch, ho.value, pitch, image_points.Length / pitch); }
            return camera_points;
        }

        public static float[,] RM_MapCameraSpaceToImagePoint(hl2da.SENSOR_ID id, float[,] camera_points, int pitch = 2)
        {
            float[,] image_points = new float[camera_points.GetLength(0), camera_points.GetLength(1)];
            using (pointer hi = pointer.get(camera_points), ho = pointer.get(image_points)) { hl2da.RM_MapCameraSpaceToImagePoint((int)id, hi.value, pitch, ho.value, pitch, camera_points.Length / pitch); }
            return image_points;
        }

        public static hl2da.pv_captureformat CreateFormat_PV(ushort width, ushort height, byte framerate, bool enable_mrc, bool shared)
        {
            hl2da.pv_captureformat cf = new hl2da.pv_captureformat();

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
            cf.hologram_perspective = (int)hl2da.HOLOGRAM_PERSPECTIVE.PV;
            cf.width = width;
            cf.height = height;
            cf.framerate = framerate;

            return cf;
        }

        public static hl2da.ea_captureformat CreateFormat_EA(hl2da.MIXER_MODE mode, byte device_index, byte source_index)
        {
            hl2da.ea_captureformat cf = new hl2da.ea_captureformat();

            cf.mixer_mode = (((uint)mode) & 0xFF) | (((uint)device_index) << 8) | (((uint)source_index) << 16);
            cf.loopback_gain = 1.0f;
            cf.microphone_gain = 1.0f;

            return cf;
        }

        public static hl2da.ev_captureformat CreateFormat_EV(ushort width, ushort height, byte framerate, string subtype, bool shared, uint group_index, uint source_index, uint profile_index)
        {
            hl2da.ev_captureformat cf = new hl2da.ev_captureformat();

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

        public static void PV_SetFocus(hl2da.PV_FocusMode focusmode, hl2da.PV_AutoFocusRange autofocusrange, hl2da.PV_ManualFocusDistance distance, uint value, hl2da.PV_DriverFallback disabledriverfallback)
        {
            hl2da.PV_SetFocus((uint)focusmode, (uint)autofocusrange, (uint)distance, value, (uint)disabledriverfallback);
        }

        public static void PV_SetVideoTemporalDenoising(hl2da.PV_VideoTemporalDenoisingMode mode)
        {
            hl2da.PV_SetVideoTemporalDenoising((uint)mode);
        }

        public static void PV_SetWhiteBalance_Preset(hl2da.PV_ColorTemperaturePreset preset)
        {
            hl2da.PV_SetWhiteBalance_Preset((uint)preset);
        }

        public static void PV_SetWhiteBalance_Value(uint value)
        {
            hl2da.PV_SetWhiteBalance_Value(value);
        }

        public static void PV_SetExposure(hl2da.PV_ExposureMode setauto, uint value)
        {
            hl2da.PV_SetExposure((uint)setauto, value);
        }

        public static void PV_SetExposurePriorityVideo(hl2da.PV_ExposurePriorityVideo enabled)
        {
            hl2da.PV_SetExposurePriorityVideo((uint)enabled);
        }

        public static void PV_SetSceneMode(hl2da.PV_CaptureSceneMode mode)
        {
            hl2da.PV_SetSceneMode((uint)mode);
        }

        public static void PV_SetIsoSpeed(hl2da.PV_IsoSpeedMode setauto, uint value)
        {
            hl2da.PV_SetIsoSpeed((uint)setauto, value);
        }

        public static void PV_SetBacklightCompensation(hl2da.PV_BacklightCompensationState enable)
        {
            hl2da.PV_SetBacklightCompensation((uint)enable);
        }

        public static void PV_SetDesiredOptimization(hl2da.PV_MediaCaptureOptimization mode)
        {
            hl2da.PV_SetDesiredOptimization((uint)mode);
        }

        public static void PV_SetPrimaryUse(hl2da.PV_CaptureUse mode)
        {
            hl2da.PV_SetPrimaryUse((uint)mode);
        }

        public static void PV_SetOpticalImageStabilization(hl2da.PV_OpticalImageStabilizationMode mode)
        {
            hl2da.PV_SetOpticalImageStabilization((uint)mode);
        }

        public static void PV_SetHdrVideo(hl2da.PV_HdrVideoMode mode)
        {
            hl2da.PV_SetHdrVideo((uint)mode);
        }

        public static void PV_SetRegionsOfInterest(bool clear, bool set, bool auto_exposure, bool auto_focus, bool bounds_normalized, float x, float y, float w, float h, hl2da.PV_RegionOfInterestType type, uint weight)
        {
            hl2da.PV_SetRegionsOfInterest(clear ? 1 : 0, set ? 1 : 0, auto_exposure ? 1 : 0, auto_focus ? 1 : 0, bounds_normalized ? 1 : 0, x, y, w, h, (uint)type, weight);
        }

        public static void EX_SetInterfacePriority(hl2da.SENSOR_ID id, hl2da.InterfacePriority priority)
        {
            hl2da.EX_SetInterfacePriority((uint)id, (int)priority);
        }

        public static int EX_GetInterfacePriority(hl2da.SENSOR_ID id)
        {
            return hl2da.EX_GetInterfacePriority((uint)id);
        }
    }
}

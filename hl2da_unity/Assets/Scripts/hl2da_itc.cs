
using System;
using System.Runtime.InteropServices;

public static class hl2da_itc
{
    [DllImport("hl2da_itc")]
    public static extern void ITC_Crop(IntPtr image_in, int width_in, int height_in, int bpp_in, IntPtr image_out, int x_out, int y_out, int width_out, int height_out);

    [DllImport("hl2da_itc")]
    public static extern void ITC_RMUndistort(int id, IntPtr mapxy, int interpolation, int border_mode, ushort border_value, IntPtr image_in, IntPtr image_out);

    [DllImport("hl2da_itc")]
    public static extern void ITC_RMToBGRX(int id, IntPtr image_in, int alpha, IntPtr image_out);
    
    [DllImport("hl2da_itc")]
    public static extern void ITC_RMDepthInitializeRays(int id, IntPtr uv2xy);
    
    [DllImport("hl2da_itc")]
    public static extern void ITC_RMDepthNormalize(int id, IntPtr depth_in, IntPtr depth_out);
    
    [DllImport("hl2da_itc")]
    public static extern void ITC_RMDepthTo3D(int id, IntPtr depth, IntPtr points);
    
    [DllImport("hl2da_itc")]
    public static extern void ITC_RMDepthFill1(int id, IntPtr depth, IntPtr image_points, IntPtr local_depths, IntPtr depth_out, int width, int height);
    
    [DllImport("hl2da_itc")]
    public static extern void ITC_TransformIdentity(IntPtr transform_out);
    
    [DllImport("hl2da_itc")]
    public static extern void ITC_TranformInvert(IntPtr transform_in, IntPtr transform_out);
    
    [DllImport("hl2da_itc")]
    public static extern void ITC_TransformMultiply(IntPtr transform_in_A, IntPtr transform_in_B, IntPtr transform_out);
    
    [DllImport("hl2da_itc")]
    public static extern void ITC_TransformPoints3(IntPtr transform, IntPtr points_in, int count, IntPtr points_out);
    
    [DllImport("hl2da_itc")]
    public static extern void ITC_GetPoints2Channel(IntPtr points_in, int count, int channel, IntPtr points_out);
    
    [DllImport("hl2da_itc")]
    public static extern void ITC_GetPoints3Channel(IntPtr points_in, int count, int channel, IntPtr points_out);
    
    [DllImport("hl2da_itc")]
    public static extern void ITC_ProjectPoints3(IntPtr intrinsics, IntPtr transform, IntPtr points_in, int count, IntPtr points_out);
}

using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

public static class hl2da_coprocessor
{
    public static byte[,,] Crop(IntPtr image_in, int width_in, int height_in, int bpp_in, int x_out, int y_out, int width_out, int height_out)
    {
        byte[,,] r = new byte[height_out, width_out, bpp_in];
        GCHandle h = GCHandle.Alloc(r, GCHandleType.Pinned);
        hl2da_itc.ITC_Crop(image_in, width_in, height_in, bpp_in, h.AddrOfPinnedObject(), x_out, y_out, width_out, height_out);
        h.Free();
        return r;
    }

    public static T[,] RM_Undistort<T>(hl2da_api.SENSOR_ID id, float[,] mapxy, int interpolation, int border_mode, ushort border_value, IntPtr image_in)
    {
        if (!hl2da_user.RM_GetResolution(id, out int w, out int h)) { return null; }

        T[,] r = new T[h, w];

        GCHandle h1 = GCHandle.Alloc(mapxy, GCHandleType.Pinned);
        GCHandle h2 = GCHandle.Alloc(r,     GCHandleType.Pinned);
        hl2da_itc.ITC_RMUndistort((int)id, h1.AddrOfPinnedObject(), interpolation, border_mode, border_value, image_in, h2.AddrOfPinnedObject());
        h1.Free();
        h2.Free();

        return r;
    }

    public static T[,,] RM_ToBGRX<T>(hl2da_api.SENSOR_ID id, IntPtr image_in, bool alpha)
    {
        if (!hl2da_user.RM_GetResolution(id, out int w, out int h)) { return null; }

        T[,,] r = new T[h, w, alpha ? 4 : 3];

        GCHandle h1 = GCHandle.Alloc(r, GCHandleType.Pinned);
        hl2da_itc.ITC_RMToBGRX((int)id, image_in, alpha ? 1 : 0, h1.AddrOfPinnedObject());
        h1.Free();

        return r;
    }

    // TODO:
}

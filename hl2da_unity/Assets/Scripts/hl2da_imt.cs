
using System;

public class hl2da_imt
{
    private IntPtr container;

    private IntPtr buffer;
    private int length;

    public hl2da_imt()
    {
        Reset();
    }

    ~hl2da_imt()
    {
        Destroy();
    }
    
    public IntPtr Buffer { get { return buffer; } }

    public int Length { get { return length; } }
    
    private void Reset()
    {
        container = IntPtr.Zero;
        buffer = IntPtr.Zero;
        length = 0;
    }

    public void Destroy()
    {
        if (container == IntPtr.Zero) { return; }
        hl2da_api.IMT_Release(container);
        Reset();
    }

    public static hl2da_imt Convert(IntPtr image, uint stride, uint height, hl2da_api.IMT_Format format_in, hl2da_api.IMT_Format format_out)
    {
        hl2da_imt fc = new hl2da_imt();
        hl2da_api.IMT_YUV2RGB(image, stride, height, (uint)format_in, (uint)format_out, ref fc.container);
        if (fc.container != IntPtr.Zero) { hl2da_api.IMT_Extract(fc.container, ref fc.buffer, ref fc.length); }
        return fc;
    }

    public static uint GetStride_PV(uint width)
    {
        return width + ((64 - (width & 63)) & 63);
    }
}


using System;

public static partial class hl2da_api
{
    public class imt
    {
        private IntPtr container;

        private IntPtr buffer;
        private int length;

        public imt()
        {
            Reset();
        }

        ~imt()
        {
            Dispose(false);
        }

        public IntPtr Buffer { get { return buffer; } }

        public int Length { get { return length; } }

        private void Reset()
        {
            container = IntPtr.Zero;
            buffer = IntPtr.Zero;
            length = 0;
        }

        protected virtual void Dispose(bool disposing)
        {
            if (container == IntPtr.Zero) { return; }
            hl2da_api.IMT_Release(container);
            Reset();
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        public static imt Convert(IntPtr image, uint stride, uint height, hl2da_api.IMT_Format format_in, hl2da_api.IMT_Format format_out)
        {
            imt fc = new imt();
            hl2da_api.IMT_YUV2RGB(image, stride, height, (uint)format_in, (uint)format_out, ref fc.container);
            if (fc.container != IntPtr.Zero) { hl2da_api.IMT_Extract(fc.container, ref fc.buffer, ref fc.length); }
            return fc;
        }

        public static uint GetStride_PV(uint width)
        {
            return (width + 63U) & ~63U;
        }
    }
}


using System;

public static partial class hl2da
{
    public class framebuffer : IDisposable
    {
        private const int BUFFER_COUNT = 4;

        private IntPtr frame;

        private hl2da.STATUS status;
        private hl2da.SENSOR_ID id;
        private ulong timestamp;
        private int framestamp;
        private int valid;

        private IntPtr[] buffer;
        private int[] length;

        public framebuffer()
        {
            Reset();
        }

        ~framebuffer()
        {
            Dispose(false);
        }

        public hl2da.STATUS Status { get { return status; } }

        public hl2da.SENSOR_ID Id { get { return id; } }

        public ulong Timestamp { get { return timestamp; } }

        public int Framestamp { get { return framestamp; } }

        public int Valid { get { return valid; } }

        public hl2da.SI_VALID Valid_SI { get { return (hl2da.SI_VALID)valid; } }

        public hl2da.EE_VALID Valid_EE { get { return (hl2da.EE_VALID)valid; } }

        public IntPtr Buffer(uint index)
        {
            if (index >= BUFFER_COUNT) { return IntPtr.Zero; }
            return buffer[index];
        }

        public int Length(uint index)
        {
            if (index >= BUFFER_COUNT) { return 0; }
            return length[index];
        }

        private void Reset()
        {
            frame = IntPtr.Zero;
            status = hl2da.STATUS.DISCARDED;
            id = (hl2da.SENSOR_ID)(-1);
            timestamp = 0;
            framestamp = 0;
            valid = 0;
            buffer = new IntPtr[4];
            length = new int[4];
        }

        protected virtual void Dispose(bool disposing)
        {
            if (frame == IntPtr.Zero) { return; }
            hl2da.Release((int)id, frame);
            Reset();
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        private static void Extract(framebuffer fb)
        {
            hl2da.Extract((int)fb.id, fb.frame, ref fb.valid, fb.buffer, fb.length);
        }

        public static framebuffer GetFrame(hl2da.SENSOR_ID id, int framestamp)
        {
            framebuffer fb = new framebuffer();
            fb.id = id;
            fb.status = (hl2da.STATUS)hl2da.GetByFramestamp((int)id, framestamp, ref fb.frame, ref fb.timestamp, ref fb.framestamp);
            if (fb.frame != IntPtr.Zero) { Extract(fb); }
            return fb;
        }

        public static framebuffer GetFrame(hl2da.SENSOR_ID id, ulong timestamp, hl2da.TIME_PREFERENCE time_preference, bool tiebreak_right)
        {
            framebuffer fb = new framebuffer();
            fb.id = id;
            fb.status = (hl2da.STATUS)hl2da.GetByTimestamp((int)id, timestamp, (int)time_preference, tiebreak_right ? 1 : 0, ref fb.frame, ref fb.timestamp, ref fb.framestamp);
            if (fb.frame != IntPtr.Zero) { Extract(fb); }
            return fb;
        }
    }
}


using System;

public class hl2da_framebuffer
{
    private const int BUFFER_COUNT = 4;

    private IntPtr frame;

    private hl2da_api.STATUS status;
    private hl2da_api.SENSOR_ID id;
    private ulong timestamp;
    private int framestamp;
    private int valid;

    private IntPtr[] buffer;
    private int[] length;

    public hl2da_framebuffer()
    {
        Reset();
    }

    ~hl2da_framebuffer()
    {
        Destroy();
    }

    public hl2da_api.STATUS Status { get { return status; } }

    public hl2da_api.SENSOR_ID Id { get { return id; } }

    public ulong Timestamp { get { return timestamp; } }

    public int Framestamp { get { return framestamp; } }

    public int Valid { get { return valid; } }

    public hl2da_api.SI_VALID Valid_SI { get { return (hl2da_api.SI_VALID)valid; } }

    public hl2da_api.EE_VALID Valid_EE { get { return (hl2da_api.EE_VALID)valid; } }

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
        status = hl2da_api.STATUS.DISCARDED;
        id = (hl2da_api.SENSOR_ID)(-1);
        timestamp = 0;
        framestamp = 0;
        valid = 0;
        buffer = new IntPtr[4];
        length = new int[4];
    }

    public void Destroy()
    {
        if (frame == IntPtr.Zero) { return; }
        hl2da_api.Release((int)id, frame);
        Reset();
    }

    private static void Extract(hl2da_framebuffer fb)
    {
        hl2da_api.Extract((int)fb.id, fb.frame, ref fb.valid, fb.buffer, fb.length);
    }

    public static hl2da_framebuffer GetFrame(hl2da_api.SENSOR_ID id, int framestamp)
    {
        hl2da_framebuffer fb = new hl2da_framebuffer();
        fb.id = id;
        fb.status = (hl2da_api.STATUS)hl2da_api.GetByFramestamp((int)id, framestamp, ref fb.frame, ref fb.timestamp, ref fb.framestamp);
        if (fb.frame != IntPtr.Zero) { Extract(fb); }
        return fb;
    }

    public static hl2da_framebuffer GetFrame(hl2da_api.SENSOR_ID id, ulong timestamp, hl2da_api.TIME_PREFERENCE time_preference, bool tiebreak_right)
    {
        hl2da_framebuffer fb = new hl2da_framebuffer();
        fb.id = id;
        fb.status = (hl2da_api.STATUS)hl2da_api.GetByTimestamp((int)id, timestamp, (int)time_preference, tiebreak_right ? 1 : 0, ref fb.frame, ref fb.timestamp, ref fb.framestamp);
        if (fb.frame != IntPtr.Zero) { Extract(fb); }
        return fb;
    }
}

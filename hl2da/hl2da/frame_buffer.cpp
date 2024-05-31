
#include "frame_buffer.h"
#include "lock.h"

// OK
sensor_frame::sensor_frame() : m_count(1)
{

}

// OK
sensor_frame::~sensor_frame()
{

}

// OK
ULONG sensor_frame::AddRef()
{
    return InterlockedIncrement(&m_count);
}

// OK
ULONG sensor_frame::Release()
{
    ULONG uCount = InterlockedDecrement(&m_count);
    if (uCount != 0) { return uCount; }
    delete this;
    return uCount;
}

// OK
frame_buffer::frame_buffer()
{
    InitializeSRWLock(&m_lock);
}

// OK
frame_buffer::~frame_buffer()
{
    Clear();
}

// OK
void frame_buffer::Reset(int buffer_size)
{
    SRWLock srw(&m_lock, true);
    m_buffer.reset(buffer_size);
}

// OK
void frame_buffer::Insert(sensor_frame* f, uint64_t timestamp)
{
    SRWLock srw(&m_lock, true);
    sensor_frame* old = m_buffer.insert(f, timestamp);
    if (old) { old->Release(); }
}

// OK
void frame_buffer::Clear()
{
    SRWLock srw(&m_lock, true);
    for (int32_t i = 0; i < m_buffer.size(); ++i) { m_buffer.at(i)->Release(); }
    m_buffer.reset();
}

// OK
int frame_buffer::Get(int32_t stamp, sensor_frame*& f, uint64_t& t, int32_t& s)
{
    SRWLock srw(&m_lock, false);
    int v = m_buffer.get(stamp, f, t, s);
    if (v == 0) { f->AddRef(); }
    return v;
}

// OK
int frame_buffer::Get(uint64_t timestamp, int time_preference, bool tiebreak_right, sensor_frame*& f, uint64_t& t, int32_t& s)
{
    SRWLock srw(&m_lock, false);
    int v = m_buffer.get(timestamp, time_preference, tiebreak_right, f, t, s);
    if (v == 0) { f->AddRef(); }
    return v;
}

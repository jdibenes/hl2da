
#pragma once

#include <Windows.h>
#include "ring_buffer.h"

class sensor_frame
{
private:
    ULONG m_count;

public:
    sensor_frame();
    virtual ~sensor_frame() = 0;

    ULONG AddRef();
    ULONG Release();
};

class frame_buffer
{
private:
    ring_buffer<sensor_frame*> m_buffer;
    SRWLOCK m_lock;

public:
    frame_buffer();
    ~frame_buffer();

    void Reset(int buffer_size);
    void Insert(sensor_frame* f, uint64_t timestamp);
    void Clear();

    int Get(int32_t stamp, sensor_frame*& f, uint64_t& t, int32_t& s);
    int Get(uint64_t timestamp, int time_preference, bool tiebreak_right, sensor_frame*& f, uint64_t& t, int32_t& s);
};


#pragma once

#include <Windows.h>

#include <winrt/Windows.Foundation.Numerics.h>

struct ee_data
{
    winrt::Windows::Foundation::Numerics::float3 c_origin;
    winrt::Windows::Foundation::Numerics::float3 c_direction;
    winrt::Windows::Foundation::Numerics::float3 l_origin;
    winrt::Windows::Foundation::Numerics::float3 l_direction;
    winrt::Windows::Foundation::Numerics::float3 r_origin;
    winrt::Windows::Foundation::Numerics::float3 r_direction;
    float  l_openness;
    float  r_openness;
    float  vergence_distance;
};

struct ee_frame
{
private:
    ULONG m_count;

public:
    uint32_t valid;
    ee_data frame;
    winrt::Windows::Foundation::Numerics::float4x4 pose;

    ee_frame();

    ULONG AddRef();
    ULONG Release();
};

void EE_SetFPS(int fps_index);
void EE_SetEnable(bool enable);

int EE_Get(int32_t stamp, ee_frame*& f, uint64_t& t, int32_t& s);
int EE_Get(uint64_t timestamp, int time_preference, bool tiebreak_right, ee_frame*& f, uint64_t& t, int32_t& s);

void EE_Initialize(int buffer_size);
void EE_Quit();
void EE_Cleanup();

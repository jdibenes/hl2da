
#pragma once

#include "research_mode.h"

#include <winrt/Windows.Foundation.Numerics.h>

class rm_frame
{
private:
    ULONG m_count;

public:
    IResearchModeSensorFrame* rmsf;
    winrt::Windows::Foundation::Numerics::float4x4 pose;

    rm_frame(IResearchModeSensorFrame* f, winrt::Windows::Foundation::Numerics::float4x4 p);

    ULONG AddRef();
    ULONG Release();
};

struct rm_data_vlc
{
    uint8_t const* buffer;
    size_t length;
};

struct rm_data_zht
{
    uint16_t const* buffer;
    size_t length;
    uint16_t const* ab_depth_buffer;
    size_t ab_depth_length;
};

struct rm_data_zlt
{
    uint16_t const* buffer;
    size_t length;
    uint16_t const* ab_depth_buffer;
    size_t ab_depth_length;
    uint8_t const* sigma_buffer;
    size_t sigma_length;
};

struct rm_data_acc
{
    AccelDataStruct const* buffer;
    size_t length;
};

struct rm_data_gyr
{
    GyroDataStruct const* buffer;
    size_t length;
};

struct rm_data_mag
{
    MagDataStruct const* buffer;
    size_t length;
};

void RM_SetEnable(int id, bool enable);

int RM_Get(int id, int32_t stamp, rm_frame*& f, uint64_t& t, int32_t& s);
int RM_Get(int id, uint64_t timestamp, int time_preference, bool tiebreak_right, rm_frame*& f, uint64_t& t, int32_t& s);

void RM_Extract(IResearchModeSensorFrame* f, rm_data_vlc& d);
void RM_Extract(IResearchModeSensorFrame* f, rm_data_zht& d);
void RM_Extract(IResearchModeSensorFrame* f, rm_data_zlt& d);
void RM_Extract(IResearchModeSensorFrame* f, rm_data_acc& d);
void RM_Extract(IResearchModeSensorFrame* f, rm_data_gyr& d);
void RM_Extract(IResearchModeSensorFrame* f, rm_data_mag& d);

void RM_GetExtrinsics(int id, float* out);
void RM_MapImagePointToCameraUnitPlane(int id, float const* in, float* out, int point_count);
void RM_MapCameraSpaceToImagePoint(int id, float const* in, float* out, int point_count);

void RM_InitializeDepthLock();
void RM_CleanupDepthLock();

void RM_Initialize(int id, int32_t buffer_size);
void RM_Quit(int id);
void RM_Cleanup(int id);

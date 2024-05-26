
#pragma once

#include <Windows.h>
#include "custom_video_effect.h"

#include <winrt/Windows.Media.Capture.Frames.h>
#include <winrt/Windows.Foundation.Numerics.h>

class pv_frame
{
private:
    ULONG m_count;

public:
    winrt::Windows::Media::Capture::Frames::MediaFrameReference mfr;
    winrt::Windows::Foundation::Numerics::float4 intrinsics;
    winrt::Windows::Foundation::Numerics::float4x4 pose;

    pv_frame(winrt::Windows::Media::Capture::Frames::MediaFrameReference const& f, winrt::Windows::Foundation::Numerics::float4 const& k, winrt::Windows::Foundation::Numerics::float4x4 const& p);

    ULONG AddRef();
    ULONG Release();
};

struct pv_videoformat
{
    uint16_t width;
    uint16_t height;
    uint8_t framerate;
};

struct pv_captureformat
{
    MRCVideoOptions mrcvo; // 28 4 aligned
    pv_videoformat vf;     // 6  2 aligned
    uint8_t _reserved[2];  // 34+2 = 36 4 aligned
};

struct pv_data
{
    uint8_t* buffer;
    uint32_t length;
};

void PV_SetFormat(pv_captureformat const& cf);
void PV_SetEnable(bool enable);

int PV_Get(int32_t stamp, pv_frame*& f, uint64_t& t, int32_t& s);
int PV_Get(uint64_t timestamp, int time_preference, bool tiebreak_right, pv_frame*& f, uint64_t& t, int32_t& s);

void PV_Extract(winrt::Windows::Media::Capture::Frames::MediaFrameReference const& ref, pv_data& d);

void PV_Initialize(int32_t buffer_size);
void PV_Quit();
void PV_Cleanup();

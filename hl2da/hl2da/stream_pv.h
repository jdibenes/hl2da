
#pragma once

#include <stdint.h>
#include "custom_video_effect.h"

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

void PV_SetFormat(pv_captureformat const& cf);
void PV_SetEnable(bool enable);

int PV_Get(int32_t stamp, void*& f, uint64_t& t, int32_t& s);
int PV_Get(uint64_t timestamp, int time_preference, bool tiebreak_right, void*& f, uint64_t& t, int32_t& s);

void PV_Release(void* frame);

void PV_Extract(void* frame, void const** buffer, int32_t* length, void const** intrinsics_buffer, int32_t* intrinsics_length, void const** pose_buffer, int32_t* pose_length);

void PV_Initialize(int32_t buffer_size);
void PV_Quit();
void PV_Cleanup();

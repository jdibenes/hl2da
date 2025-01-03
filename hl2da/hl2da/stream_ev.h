
#pragma once

#include <stdint.h>
#include "custom_video_effect.h"

struct ev_videoformat
{
    uint16_t width;
    uint16_t height;
    uint8_t framerate;
    uint8_t _reserved;
    wchar_t subtype[64];
};

struct ev_captureformat
{
    MRCVideoOptions mrcvo; //  28 4 aligned
    ev_videoformat vf;     // 134 2 aligned
    uint8_t _reserved[2];  // 162+2 = 164 4 aligned
};

void EV_SetFormat(ev_captureformat const& cf);
void EV_SetEnable(bool enable);

int EV_Get(int32_t stamp, void*& f, uint64_t& t, int32_t& s);
int EV_Get(uint64_t timestamp, int time_preference, bool tiebreak_right, void*& f, uint64_t& t, int32_t& s);

void EV_Release(void* frame);

void EV_Extract(void* frame, void const** buffer, int32_t* length, void const** format_buffer, int32_t* format_length);

void EV_Initialize(int32_t buffer_size);
void EV_Cleanup();

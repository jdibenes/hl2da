
#pragma once

#include <stdint.h>
#include "custom_audio_effect.h"

void EA_SetFormat(MRCAudioOptions const& cf);
void EA_SetEnable(bool enable);

int EA_Get(int32_t stamp, void*& f, uint64_t& t, int32_t& s);
int EA_Get(uint64_t timestamp, int time_preference, bool tiebreak_right, void*& f, uint64_t& t, int32_t& s);

void EA_Release(void* frame);

void EA_Extract(void* frame, void const** buffer, int32_t* length, void const** format_buffer, int32_t* format_length);

void EA_Initialize(int32_t buffer_size);
void EA_Cleanup();

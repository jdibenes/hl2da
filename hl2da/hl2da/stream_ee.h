
#pragma once

#include <stdint.h>

void EE_SetFormat(int fps_index);
void EE_SetEnable(bool enable);

int EE_Get(int32_t stamp, void*& f, uint64_t& t, int32_t& s);
int EE_Get(uint64_t timestamp, int time_preference, bool tiebreak_right, void*& f, uint64_t& t, int32_t& s);

void EE_Release(void* frame);

void EE_Extract(void* frame, int32_t* valid, void const** buffer, int32_t* length, void const** pose_buffer, int32_t* pose_length);

void EE_Initialize(int buffer_size);
void EE_Quit();
void EE_Cleanup();

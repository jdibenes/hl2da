
#pragma once

#include <stdint.h>

void MC_SetFormat(bool raw);
void MC_SetEnable(bool enable);

int MC_Get(int32_t stamp, void*& f, uint64_t& t, int32_t& s);
int MC_Get(uint64_t timestamp, int time_preference, bool tiebreak_right, void*& f, uint64_t& t, int32_t& s);

void MC_Release(void* frame);

void MC_Extract(void* frame, void const** buffer, int32_t* length);

void MC_Initialize(int32_t buffer_size);
void MC_Quit();
void MC_Cleanup();

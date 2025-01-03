
#pragma once

#include <stdint.h>

void SI_SetEnable(bool enable);

int SI_Get(int32_t stamp, void*& f, uint64_t& t, int32_t& s);
int SI_Get(uint64_t timestamp, int time_preference, bool tiebreak_right, void*& f, uint64_t& t, int32_t& s);

void SI_Release(void* frame);

void SI_Extract(void* frame, int32_t* valid, void const** head_buffer, int32_t* head_length, void const** eye_buffer, int32_t* eye_length, void const** left_buffer, int32_t* left_length, void const** right_buffer, int32_t* right_length);

void SI_Initialize(int buffer_size);
void SI_Cleanup();

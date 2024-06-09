
#pragma once

#include <stdint.h>

void Converter_Initialize();
void Converter_Cleanup();

void Converter_ZHTInvalidate(uint16_t const* depth_in, uint16_t* depth_out);
void Converter_ZLTInvalidate(uint8_t const* sigma_in, uint16_t const* depth_in, uint16_t* depth_out);
void Converter_YUV2RGB(uint8_t* image, uint32_t stride, uint32_t height, uint32_t format_in, uint32_t format_out, void** fc);
void Converter_Extract(void* fc, void const** buffer, int32_t* length);
void Converter_Release(void* fc);

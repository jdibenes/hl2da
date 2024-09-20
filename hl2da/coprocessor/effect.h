
#pragma once

#include <stdint.h>

void Crop(void* image_in, int width_in, int height_in, int bpp_in, void* image_out, int x_out, int y_out, int width_out, int height_out);
void RM_Undistort(int id, float* mapxy, int interpolation, int border_mode, uint16_t border_value, void* image_in, void* image_out);
void RM_ToBGRX(int id, void* image_in, bool alpha, void* image_out);

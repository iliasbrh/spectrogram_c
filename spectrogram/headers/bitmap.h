#ifndef BITMAP_H
#define BITMAP_H

#include "base.h"
#include "stdio.h"
#include "stdlib.h"

typedef struct {
		u16 file_format;
		// zero padding for 32bit alignment here

		u32 file_size;
		u32 reserved_zeros;

		u32 offset_bytes_to_data;
		u32 bitmapinfo_block_size;

		i32 pixel_width;
		i32 pixel_height;

		u16 planes;
		u16 bits_per_pixel;

		// all following will most of the time be zeros
		u32 compression;  // zero for no compression
		u32 img_size; 	  // can be zero for rgb bitmap
		u32 pxlPerMeterX; // zero for unspecified
		u32 pxlPerMeterY;
		u32 clr_used;
		u32 clr_important;
} bmp_header;


void writeBMPfromF32(f32* data, u16 width, u16 height, char* file_name);

#endif

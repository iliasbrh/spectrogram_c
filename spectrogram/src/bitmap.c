#include "bitmap.h"

void writeBMPfromF32(f32* data, u16 width, u16 height, char* file_name) {
		FILE* output_file = fopen(file_name, "wb");
		u16 padded_width = ((width*3 + 3) / 4) * 4; // multiplied by 3 then padded
		
		bmp_header hdr = {
				.file_format = 0x4D42, // BM

				.file_size = 54 + padded_width * height,
				.reserved_zeros = 0x0000,

				.offset_bytes_to_data = 54,
				.bitmapinfo_block_size = 40,

				.pixel_width = (i32)(u32)width,
				.pixel_height = (i32)(u32)height,

				.planes = 1,
				.bits_per_pixel = 24,
				
				.compression = 0,
				.img_size = 0,
				.pxlPerMeterX = 0,
				.pxlPerMeterY = 0,
				.clr_used = 0,
				.clr_important = 0
		};

		// dumping it in two parts to skip the zero padding of 32bit alignment of the struct
		fwrite(&hdr, 2, 1, output_file);
		fwrite(&hdr.file_size, 52, 1, output_file);

		// determine min and max of the float array
		f32 min = data[0];
		f32 max = data[0];
		for (u32 i=0; i<width*height; i++) {
				min = MIN(min, data[i]);
				max = MAX(max, data[i]);
		}

		// normalize every values to [0, 1]
		for (u32 i=0; i<width*height; i++)
				data[i] = (data[i] - min) / (max - min);

		// multiply by 255.0f then quantize to u8
		u8* out = (u8*)malloc(padded_width*height);
		u32 n_zero_padding = padded_width - width * 3; // end of line padding
		u32 ind = 0;
		for (u32 i=0; i<height; i++) {
				for (u32 j=0; j<width; j++) {
						out[ind++] = (u8)(data[j+i*width] * 255.0f);
						out[ind++] = (u8)(data[j+i*width] * 255.0f);
						out[ind++] = (u8)(data[j+i*width] * 255.0f);
				}
				for (u32 pad=0; pad<n_zero_padding; pad++)
						out[ind++] = 0;
		}

		fwrite(out, padded_width*height, 1, output_file);
		free(out);
}






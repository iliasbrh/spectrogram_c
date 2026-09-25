#include "wav_parser.h"


wav_header read_header(FILE* wav_file) {
		wav_header hdr;

		// reading RIFF, file_size and WAVE
		b32 error_code = fread(&hdr, 12, 1, wav_file);

		// skipping chunks that are not format chunk, i.e. metadata
		hdr.fmt_buffer = 0x00000000;
		while (hdr.fmt_buffer != 0x20746d66) { // "fmt "
				error_code = fread(&(hdr.fmt_buffer), 8, 1, wav_file);
				if (hdr.fmt_buffer != 0x20746d66)
						fseek(wav_file, (i64)((hdr.size_fmt_block_buffer+1)&(~1)), SEEK_CUR);
		}

		error_code = fread(&(hdr.audio_format), 16, 1, wav_file);
		if (hdr.size_fmt_block_buffer > 16)
				fseek(wav_file, (i64)((hdr.size_fmt_block_buffer+1)&(~1)) - 16, SEEK_CUR);

		return hdr;
}

i32 size24to32(u8* buffer) {
		i32 res = buffer[0] | (buffer[1] << 8) | ((buffer[2] & 0x7f) << 16);
				
		if (0x80 & buffer[2])
				res -= (i32)(1 << 23);

		return res;
} 

wav_t read_wav(FILE* wav_file) {
		wav_t wav;
		wav.hdr = read_header(wav_file);
		printf("Successfully read the header\n");
		print_header(wav.hdr);

		// skipping chunks that are not data
		wav.data_buffer = 0x00000000;
		b32 error_code;
		while (wav.data_buffer != 0x61746164) { // "data"
				error_code = fread(&(wav.data_buffer), 8, 1, wav_file);
				if (wav.data_buffer != 0x61746164)
						fseek(wav_file, (i64)((wav.size + 1) & (~1)), SEEK_CUR);
		}

		u32 n_samples = (8 * wav.size) / (wav.hdr.bits_per_sample);
		printf("Number of samples in a given channel : %i\n", (i32)n_samples);
		wav.data = (f32**)malloc(wav.hdr.n_channels*sizeof(f32*));
		for (u32 i=0; i<wav.hdr.n_channels; i++)
				wav.data[i] = (f32*)malloc(n_samples * sizeof(f32) / wav.hdr.n_channels);

		u8* buffer = (u8*)malloc(8); // max size is f64 so 8bytes
		if (wav.hdr.audio_format == 1) {
				f32 half_range = 1u << (wav.hdr.bits_per_sample - 1); // treating the 1 as unsigned to avoid undefined behaviour in the case of 1 << 31 (since 1 is i32 by default)
				for (u32 i=0; i<n_samples / wav.hdr.n_channels; i++) {
						for (u32 channel=0; channel < wav.hdr.n_channels; channel++) {
								error_code = fread(buffer, wav.hdr.bits_per_sample / 8, 1, wav_file);
								switch (wav.hdr.bits_per_sample)
								{
										case 8:
												wav.data[channel][i] = ((f32)(*(i8*)buffer)) / half_range; break;
										case 16:
												wav.data[channel][i] = ((f32)(*(i16*)buffer)) / half_range; break;
										case 24:
												wav.data[channel][i] = size24to32(buffer); break;
										case 32:
												wav.data[channel][i] = ((f32)(*(i32*)buffer)) / half_range; break;
										default:
												break;
								}
						}
				}
		} else if (wav.hdr.audio_format == 3) {
				for (u32 i=0; i<n_samples; i++) {
						for (u32 channel=0; channel < wav.hdr.n_channels; channel++) {
								error_code = fread(buffer, wav.hdr.bits_per_sample / 8, 1, wav_file);
								switch(wav.hdr.bits_per_sample)
								{
										case 32:
												memcpy(&wav.data[channel][i], buffer, sizeof(f32)); break;
										case 64:
												f64 tmp;
												memcpy(&tmp, buffer, sizeof(f64));
												wav.data[channel][i] = (f32)tmp; // loss of precision in downcast but don't care
												break;
								}
						}
				}
		} else { printf("Wrong audio format : %i\n", (i32)wav.hdr.audio_format);}

		return wav;
}


void print_header(wav_header hdr) {
		printf("Riff : %.4s\n", (char*)(&hdr.riff_buffer));
		printf("File size : %i\n", (i32)hdr.file_size);
		printf("Wave : %.4s\n", (char*)(&hdr.wave_buffer));
		printf("Format : %.4s\n", (char*)(&hdr.fmt_buffer));
		printf("Size of format block buffer : %i\n", (i32)hdr.size_fmt_block_buffer);

		printf("Audio format : %i\n", (i32)hdr.audio_format);
		printf("Number of channels : %i\n", (i32)hdr.n_channels);
		printf("Sampling frequency : %i\n", (i32)hdr.sampling_frequency);
		printf("Bits per sample : %i\n", (i32)hdr.bits_per_sample);
}


#ifndef WAV_PARSER_H
#define WAV_PARSER_H

#include "base.h"


typedef struct {
		u32 riff_buffer;
		u32 file_size;

		u32 wave_buffer;
		u32 fmt_buffer;
		u32 size_fmt_block_buffer;

		u16 audio_format; // 1 : PCM int || 3 : PCM or IEEE float
		u16 n_channels;

		u32 sampling_frequency;

		u32 bytes_per_second;
		u16 bytes_per_block;

		u16 bits_per_sample;
} wav_header;

typedef struct {
		wav_header hdr;

		u32 data_buffer;
		u32 size; // size of data in bytes

		f32** data; // pointers to data of each channel 
} wav_t;

wav_header read_header(FILE* wav_file);
wav_t read_wav(FILE* wav_file);

void print_header(wav_header hdr);


#endif

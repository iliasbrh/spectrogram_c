#include "wav_parser.h"


wav_header read_header(FILE* wav_file) {
		wav_header hdr;

		// reading RIFF, file_size and WAVE
		(void)!fread(&hdr, 12, 1, wav_file);

		// skipping chunks that are not format chunk, i.e. metadata
		hdr.fmt_buffer = 0x00000000;
		while (hdr.fmt_buffer != 0x20746d66) { // "fmt "
				(void)!fread(&(hdr.fmt_buffer), 8, 1, wav_file);
				if (hdr.fmt_buffer != 0x20746d66)
						fseek(wav_file, (i64)((hdr.size_fmt_block_buffer+1)&(~1u)), SEEK_CUR);
		}

		(void)!fread(&(hdr.audio_format), 16, 1, wav_file);
		if (hdr.size_fmt_block_buffer > 16)
				fseek(wav_file, (i64)((hdr.size_fmt_block_buffer+1)&(~1u)) - 16, SEEK_CUR);

		return hdr;
}

i32 size24to32(const u8* buffer) { // helper
		i32 res = buffer[0] | (buffer[1] << 8) | ((buffer[2] & 0x7f) << 16);
				
		if (0x80 & buffer[2])
				res -= (i32)(1 << 23);

		return res;
} 

wav_t read_wav(FILE* wav_file) {
		wav_t wav;
		wav.hdr = read_header(wav_file);
		print_header(wav.hdr);

		// skipping chunks that are not data
		wav.data_buffer = 0x00000000;
		while (wav.data_buffer != 0x61746164) { // "data"
				(void)!fread(&(wav.data_buffer), 8, 1, wav_file);
				if (wav.data_buffer != 0x61746164)
						fseek(wav_file, (i64)((wav.size + 1) & (~1u)), SEEK_CUR);
		}

		u32 n_samples = (8 * wav.size) / (wav.hdr.bits_per_sample);
	 	// printf("Total samples (across all channels) : %i\n", (i32)n_samples);
		wav.data = (f32**)malloc(wav.hdr.n_channels*sizeof(f32*));
		for (u32 i=0; i<wav.hdr.n_channels; i++)
				wav.data[i] = (f32*)malloc(n_samples * sizeof(f32) / wav.hdr.n_channels);


		u8* buffer = (u8*)malloc(n_samples * wav.hdr.bits_per_sample / 8);
		(void)!fread(buffer, n_samples * wav.hdr.bits_per_sample / 8, 1, wav_file);
		if (wav.hdr.audio_format == 1) {
				f32 half_range = 1u << (wav.hdr.bits_per_sample - 1);
				u32 idx = 0;
				switch (wav.hdr.bits_per_sample) 
				{
						// duplicated code but it avoids the switch case nested into the for loops
						case 8:
								for (u32 i=0; i<n_samples / wav.hdr.n_channels; i++)
										for (u32 channel=0; channel<wav.hdr.n_channels; channel++) {
												wav.data[channel][i] = ((f32)(*(i8*)(buffer+idx) - half_range)) / half_range;
												idx += 1;
										}
								break;
						case 16:
								for (u32 i=0; i<n_samples / wav.hdr.n_channels; i++)
										for (u32 channel=0; channel<wav.hdr.n_channels; channel++) {
												wav.data[channel][i] = ((f32)(*(i16*)(buffer+idx))) / half_range;
												idx += 2;
										}
								break;
						case 24:
								for (u32 i=0; i<n_samples / wav.hdr.n_channels; i++)
										for (u32 channel=0; channel<wav.hdr.n_channels; channel++) {
												wav.data[channel][i] = (f32)size24to32(buffer+idx);
												idx += 3;
										}
								break;
						case 32:
								for (u32 i=0; i<n_samples / wav.hdr.n_channels; i++)
										for (u32 channel=0; channel<wav.hdr.n_channels; channel++) {
												wav.data[channel][i] = ((f32)(*(i32*)(buffer+idx))) / half_range;
												idx += 4;
										}
								break;
						default:
								break;
				}
		} else if (wav.hdr.audio_format == 3) {
				u32 idx = 0;
				switch(wav.hdr.bits_per_sample) 
				{
						case 32:
								for (u32 i=0; i<n_samples / wav.hdr.n_channels; i++)
										for (u32 channel=0; channel<wav.hdr.n_channels; channel++) {
												wav.data[channel][i] = *(f32*)(buffer+idx);
												idx += 4;
										}
								break;
						case 64:
								for (u32 i=0; i<n_samples / wav.hdr.n_channels; i++)
										for (u32 channel=0; channel<wav.hdr.n_channels; channel++) {
												wav.data[channel][i] = (f32)*(f64*)(buffer+idx);
												idx += 8;
										}
								break;
						default:
								break;
				}
		} else { printf("Wrong audio format : %i\n", (i32)wav.hdr.audio_format);}

		free(buffer);

		return wav;
}


void print_header(wav_header hdr) {
		// printf("Riff : %.4s\n", (char*)(&hdr.riff_buffer));
		printf("File size          : %i Bytes\n", (i32)hdr.file_size);
		// printf("Wave : %.4s\n", (char*)(&hdr.wave_buffer));
		// printf("Format : %.4s\n", (char*)(&hdr.fmt_buffer));
		// printf("Size of format block : %i\n", (i32)hdr.size_fmt_block_buffer);

		// printf("Audio format 	   : %s\n", ((i32)hdr.audio_format == 1) ? "int" : "float");
		printf("Number of channels : %i\n", (i32)hdr.n_channels);
		printf("Sampling frequency : %i Hz\n", (i32)hdr.sampling_frequency);
		printf("Bits per sample    : %i bits\n", (i32)hdr.bits_per_sample);
}


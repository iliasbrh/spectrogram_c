#ifndef ARG_CONTEXT_H
#define ARG_CONTEXT_H

#include "base.h"
#include "wav_parser.h"
#include "argparse.h"


typedef struct {
		u32 channel;

		u16 n_fft;
		u32 hop_length; // 0 if we want n_fft / 2

		b32 make_mel;
		u16 n_mels;
		const char* file_name;
} args_t;

typedef struct {
		u32 n_samples_per_channel;

		u32 channel;

		u16 n_fft;
		u32 hop_length;

		u16 time_bins;
		u16 freq_bins;

		f32 max_freq; // the frequency of the highest bin

		f32* signal;
} spectrogram_context;

typedef struct {
		spectrogram_context spec_ctx;

		u16 n_mels;
} mel_spectrogram_context;

args_t parse_args(i32 argc, const char** argv);
spectrogram_context get_spectrogram_context(wav_t wav, args_t arguments);
mel_spectrogram_context get_mel_spectrogram_context(wav_t wav, args_t arguments);
void print_mel_ctx(mel_spectrogram_context mel_ctx);


#endif

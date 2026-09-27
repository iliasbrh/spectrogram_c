#ifndef ARG_CONTEXT_H
#define ARG_CONTEXT_H

#include "base.h"
#include "wav_parser.h"


typedef struct {
		u32 channel;

		u32 n_fft;
		u32 hop_length; // 0 if we want n_fft / 2

		u32 n_mels;
} args_t;

args_t get_args();

typedef struct {
		u32 n_samples_per_channel;

		u32 channel;

		u32 n_fft;
		u32 hop_length;

		u32 time_bins;
		u32 freq_bins;

		f32 max_freq;

		f32* signal;
} spectrogram_context;

typedef struct {
		spectrogram_context spec_ctx;

		u32 n_mels;
} mel_spectrogram_context;

spectrogram_context get_spectrogram_context(wav_t wav, args_t arguments);
mel_spectrogram_context get_mel_spectrogram_context(wav_t wav, args_t arguments);
void print_mel_ctx(mel_spectrogram_context mel_ctx);


#endif

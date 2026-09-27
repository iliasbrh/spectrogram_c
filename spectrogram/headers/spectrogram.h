#ifndef SPECTROGRAM_H
#define SPECTROGRAM_H

#include "base.h"

#include "wav_parser.h"
#include "maths.h"
#include "arg_context.h"

f32* make_spectrogram(wav_t wav, spectrogram_context spec_ctx, char* output_file_name);
f32* build_mel_filter_bank(u32 n_bins, f32 max_freq, u32 n_mels);
f32* make_mel_spec(wav_t wav, mel_spectrogram_context mel_ctx, char* output_file_name);

#endif

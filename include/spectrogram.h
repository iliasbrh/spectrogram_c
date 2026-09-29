#ifndef SPECTROGRAM_H
#define SPECTROGRAM_H

#include "base.h"

#include "wav_parser.h"
#include "maths.h"
#include "arg_context.h"

f32* make_spectrogram(spectrogram_context spec_ctx);
f32* build_mel_filter_bank(u32 n_bins, f32 max_freq, u32 n_mels);
f32* make_mel_spec(mel_spectrogram_context mel_ctx);

#endif

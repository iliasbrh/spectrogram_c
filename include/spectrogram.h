#ifndef SPECTROGRAM_H
#define SPECTROGRAM_H

#include "base.h"

#include "wav_parser.h"
#include "maths.h"
#include "arg_context.h"
#include "dft.h"

c64* make_complex_spectrogram(spectrogram_context spec_ctx);
f64* make_spectrogram(spectrogram_context spec_ctx);
f64* build_mel_filter_bank(u32 n_bins, f64 max_freq, u32 n_mels);
f64* make_mel_spec(mel_spectrogram_context mel_ctx);


#endif

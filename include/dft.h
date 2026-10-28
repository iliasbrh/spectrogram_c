#ifndef DFT_H
#define DFT_H

#include "maths.h"
#include "arg_context.h"

void dft(const f64* signal, const f64* window, c64* out, u32 n_fft, f64* scaled_by_window);
c64* spec_dft(spectrogram_context spec_ctx);

void radix2(const f64* scaled_signal, c64* out_buffer, u32 N, u32 stride); // recursive function for radix2
c64* spec_radix2(spectrogram_context spec_ctx);

void bluestein(const f64* signal, const f64* window, c64* out, u32 n_fft, f64* scaled_by_window, f64* extended_buffer);
c64* spec_bluestein(spectrogram_context spec_ctx);



#endif

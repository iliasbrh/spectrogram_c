#ifndef DFT_H
#define DFT_H

#include "maths.h"
#include "arg_context.h"

void dft(const f32* signal, const f32* window, float complex* out, u32 n_fft, f32* scaled_by_window);
float complex* spec_dft(spectrogram_context spec_ctx);

void radix2(const f32* scaled_signal, float complex* out_buffer, u32 N, u32 stride); // recursive function for radix2
float complex* spec_radix2(spectrogram_context spec_ctx);

void bluestein(const f32* signal, const f32* window, float complex* out, u32 n_fft, f32* scaled_by_window, f32* extended_buffer);
float complex* spec_bluestein(spectrogram_context spec_ctx);



#endif

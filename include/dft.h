#ifndef DFT_H
#define DFT_H

#include "maths.h"
#include "arg_context.h"

void dft(const f64* signal, c64* out, u32 n_fft);
c64* spec_dft(spectrogram_context spec_ctx);

// recursive function for radix2
void radix2(const f64* scaled_signal, c64* out_buffer, u32 N, u32 stride, i32 forward); // forward = 1 if forward dft, forward = 0 if reverse dft
void radix2_c(const c64* scaled_signal, c64* out_buffer, u32 N, u32 stride, i32 forward);
c64* spec_radix2(spectrogram_context spec_ctx);

void bluestein(const f64* signal, c64* extended_out_buffer, 
		       u32 n_fft, u32 M, 
			   c64* extended_a_buffer, c64* extended_a_fft_buffer, 
			   c64* precomputed_b_star, c64* precomputed_b_fft);
c64* spec_bluestein(spectrogram_context spec_ctx);



#endif

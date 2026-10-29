#include "dft.h"

void dft(const f64* signal, c64* out, u32 n_fft) {
		// signal and window of size n_fft
		// out of size n_fft/2 + 1
		for (u32 i=0; i<n_fft/2 + 1; i++) {
				c64 expo = 1.0;
				c64 factor = cexp(-2. * I * PI * (f64)i / (f64)n_fft);
				for (u32 j=0; j<n_fft; j++) {
						out[i] += expo * signal[j];
						expo *= factor;
				}
		}
}
c64* spec_dft(spectrogram_context spec_ctx) {
		f64* hann_window = build_hann_window(spec_ctx.n_fft);
		f64* scaled_by_window = (f64*)malloc(spec_ctx.n_fft*sizeof(f64)); // allocating once the buffer for all DFTs

		c64* complex_spectrogram = (c64*)malloc(spec_ctx.time_bins * spec_ctx.freq_bins * sizeof(c64));
		memset(complex_spectrogram, 0, spec_ctx.time_bins * spec_ctx.freq_bins * sizeof(c64));

		// short time fourier transforms with sliding window
		for (u32 t=0; t<spec_ctx.time_bins; t++) {
				for (u32 i=0; i<spec_ctx.n_fft; i++)
						scaled_by_window[i] = spec_ctx.signal[spec_ctx.hop_length * t + i] * hann_window[i];
				dft(scaled_by_window, 
					complex_spectrogram + (t * spec_ctx.freq_bins), 
					spec_ctx.n_fft);
		}

		free(hann_window);
		free(scaled_by_window);

		return complex_spectrogram;
}

void radix2(const f64* scaled_signal, c64* out_buffer, u32 N, u32 stride, b32 forward) { // forward = 1 if forward dft, forward = 0 if reverse dft
		if (N == 1)
				out_buffer[0] = scaled_signal[0];
		else
		{
				u32 half = N/2;
				radix2(scaled_signal, out_buffer, half, 2*stride, 1);
				radix2(scaled_signal+stride, out_buffer+half, half, 2*stride, 1);

				for (u32 k=0; k<half; k++) {
						c64 p = out_buffer[k];
						c64 q = cexp((forward * -1.) * 2. * PI * I * (f64)k / (f64)N) * out_buffer[k+half];
						out_buffer[k] = p + q;
						out_buffer[k+half] = p - q;
				}
		}
}
void radix2_c(const c64* scaled_signal, c64* out_buffer, u32 N, u32 stride, b32 forward) { // forward = 1 if forward dft, forward = 0 if reverse dft
		if (N == 1)
				out_buffer[0] = scaled_signal[0];
		else
		{
				u32 half = N/2;
				radix2_c(scaled_signal, out_buffer, half, 2*stride, 1);
				radix2_c(scaled_signal+stride, out_buffer+half, half, 2*stride, 1);

				for (u32 k=0; k<half; k++) {
						c64 p = out_buffer[k];
						c64 q = cexp((forward * -1.) * 2. * PI * I * (f64)k / (f64)N) * out_buffer[k+half];
						out_buffer[k] = p + q;
						out_buffer[k+half] = p - q;
				}
		}
}
c64* spec_radix2(spectrogram_context spec_ctx) {
		f64* hann_window = build_hann_window(spec_ctx.n_fft);
		f64* scaled_by_window = (f64*)malloc(spec_ctx.n_fft*sizeof(f64));
		c64* extended_buffer = (c64*)malloc(spec_ctx.n_fft*sizeof(c64));

		c64* complex_spectrogram = (c64*)malloc(spec_ctx.time_bins * spec_ctx.freq_bins * sizeof(c64));
		memset(complex_spectrogram, 0, spec_ctx.time_bins * spec_ctx.freq_bins * sizeof(c64));

		// fft computation
		for (u32 t=0; t<spec_ctx.time_bins; t++) {
				f64* signal = spec_ctx.signal + spec_ctx.hop_length * t;
				for (u32 i=0; i<spec_ctx.n_fft; i++)
						scaled_by_window[i] = signal[i] * hann_window[i];

				radix2(scaled_by_window, extended_buffer,
				       spec_ctx.n_fft, 1, 1);
				memcpy(complex_spectrogram + (t*spec_ctx.freq_bins), extended_buffer, spec_ctx.freq_bins*sizeof(c64));
		}


		free(hann_window);
		free(scaled_by_window);
		free(extended_buffer);

		return complex_spectrogram;
}

void bluestein(const f64* signal, c64* extended_out_buffer, 
		       u32 n_fft, u32 M, 
			   c64* extended_a_buffer, c64* extended_a_fft_buffer, 
			   c64* precomputed_b_star, c64* precomputed_b_fft) {
		// extended_out_buffer of size M
		// extended_a_buffer of size M, where M is the next power of 2 starting from 2N-1 included

		// zero padding input signal
		memset(extended_a_buffer, 0, M*sizeof(c64));
		for (u32 i=0; i<n_fft; i++)
				extended_a_buffer[i] = (c64)signal[i] * precomputed_b_star[i];

		// fft of zero padded input into fft_buffer
		radix2_c(extended_a_buffer, extended_a_fft_buffer, M, 1, 1);

		// multiply pointwise with precomputed_b_fft (to get the convolution result)
		// and dividing by M which is part of the incoming fft that comes next
		for (u32 i=0; i<M; i++)
				extended_a_fft_buffer[i] *= precomputed_b_fft[i] / (f64)M;
		
		// inverse fft into the out_buffer 
		radix2_c(extended_a_fft_buffer, extended_out_buffer, M, 1, false);

		for (u32 i=0; i<n_fft; i++) // don't care about i when n_fft <= i < M
				extended_out_buffer[i] *= precomputed_b_star[i];
}
c64* spec_bluestein(spectrogram_context spec_ctx) {
		f64* hann_window = build_hann_window(spec_ctx.n_fft);
		f64* scaled_by_window = (f64*)malloc(spec_ctx.n_fft*sizeof(f64));
		c64* complex_spectrogram = (c64*)malloc(spec_ctx.time_bins * spec_ctx.freq_bins * sizeof(c64));
		memset(complex_spectrogram, 0, spec_ctx.time_bins * spec_ctx.freq_bins * sizeof(c64));

		u32 M = next_pow2(2*spec_ctx.n_fft - 1);
		c64* extended_out_buffer = (c64*)malloc(M*sizeof(c64));
		c64* extended_a_buffer = (c64*)malloc(M*sizeof(c64));
		c64* extended_a_fft_buffer = (c64*)malloc(M*sizeof(c64));
		c64* precomputed_b = (c64*)malloc(M*sizeof(c64));
		c64* precomputed_b_fft = (c64*)malloc(M*sizeof(c64));
		c64* precomputed_b_star = (c64*)malloc(spec_ctx.n_fft*sizeof(c64));

		// precomputing b then b_star, then b_fft
		memset(precomputed_b, 0, M*sizeof(c64));
		precomputed_b[0] = (c64)1.;
		for (u32 i=1; i<spec_ctx.n_fft; i++) {
				c64 tmp = cexp(I * PI * (f64)((i*i)%(2*spec_ctx.n_fft)) / (f64)spec_ctx.n_fft);
				precomputed_b[i] = tmp;
				precomputed_b[M - i] = tmp;
		}

		for (u32 i=0; i<spec_ctx.n_fft; i++)
				precomputed_b_star[i] = conj(precomputed_b[i]);

		radix2_c(precomputed_b, precomputed_b_fft, M, 1, 1);

		// fft computation
		for (u32 t=0; t<spec_ctx.time_bins; t++) {
				f64* signal = spec_ctx.signal + spec_ctx.hop_length * t;
				for (u32 i=0; i<spec_ctx.n_fft; i++)
						scaled_by_window[i] = signal[i] * hann_window[i];

				bluestein(scaled_by_window, extended_out_buffer,
						  spec_ctx.n_fft, M,
						  extended_a_buffer, extended_a_fft_buffer,
						  precomputed_b_star, precomputed_b_fft);

				memcpy(complex_spectrogram + (t*spec_ctx.freq_bins), extended_out_buffer, spec_ctx.freq_bins*sizeof(c64));
		}

		free(hann_window);
		free(scaled_by_window);

		free(extended_a_buffer);
		free(extended_out_buffer);
		free(extended_a_fft_buffer);
		free(precomputed_b);
		free(precomputed_b_fft);
		free(precomputed_b_star);

		return complex_spectrogram;
}


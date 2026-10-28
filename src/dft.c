#include "dft.h"

void dft(const f64* signal, const f64* window, c64* out, u32 n_fft, f64* scaled_by_window) {
		// signal and window of size n_fft
		// out of size n_fft/2 + 1
		for (u32 i=0; i<n_fft; i++)
				scaled_by_window[i] = signal[i] * window[i];

		c64 expo, factor;
		for (u32 i=0; i<n_fft/2 + 1; i++) {
				expo = 1.0;
				factor = cexp(-2. * I * PI * (f64)i / (f64)n_fft);
				for (u32 j=0; j<n_fft; j++) {
						out[i] += expo * scaled_by_window[j];
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
		for (u32 t=0; t<spec_ctx.time_bins; t++)
				dft(spec_ctx.signal + spec_ctx.hop_length * t, 
				    hann_window, 
					complex_spectrogram + (t * spec_ctx.freq_bins), 
					spec_ctx.n_fft,
					scaled_by_window);

		free(hann_window);
		free(scaled_by_window);

		return complex_spectrogram;
}

void radix2(const f64* scaled_signal, c64* out_buffer, u32 N, u32 stride) {
		if (N == 1)
				out_buffer[0] = scaled_signal[0];
		else
		{
				u32 half = N/2;
				radix2(scaled_signal, out_buffer, half, 2*stride);
				radix2(scaled_signal+stride, out_buffer+half, half, 2*stride);

				for (u32 k=0; k<half; k++) {
						c64 p = out_buffer[k];
						c64 q = cexp(-2. * PI * I * (f64)k / (f64)N) * out_buffer[k+half];
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
		f64* signal;
		for (u32 t=0; t<spec_ctx.time_bins; t++) {
				signal = spec_ctx.signal + spec_ctx.hop_length * t;
				for (u32 i=0; i<spec_ctx.n_fft; i++)
						scaled_by_window[i] = signal[i] * hann_window[i];

				radix2(scaled_by_window, extended_buffer,
				       spec_ctx.n_fft, 1);
				memcpy(complex_spectrogram + (t*spec_ctx.freq_bins), extended_buffer, spec_ctx.freq_bins*sizeof(c64));
		}


		free(hann_window);
		free(scaled_by_window);
		free(extended_buffer);

		return complex_spectrogram;
}

void bluestein(const f64* signal, const f64* window, c64* out, u32 n_fft, f64* scaled_by_window, f64* extended_buffer) {

}
c64* spec_bluestein(spectrogram_context spec_ctx) {

}


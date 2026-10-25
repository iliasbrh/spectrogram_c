#include "dft.h"

void dft(const f32* signal, const f32* window, float complex* out, u32 n_fft, f32* scaled_by_window) {
		// signal and window of size n_fft
		// out of size n_fft/2 + 1
		for (u32 i=0; i<n_fft; i++)
				scaled_by_window[i] = signal[i] * window[i];

		float complex expo, factor;
		for (u32 i=0; i<n_fft/2 + 1; i++) {
				expo = 1.0f;
				factor = cexpf(-2.0f * I * PI * (f32)i / (f32)n_fft);
				for (u32 j=0; j<n_fft; j++) {
						out[i] += expo * scaled_by_window[j];
						expo *= factor;
				}
		}
}
float complex* spec_dft(spectrogram_context spec_ctx) {
		f32* hann_window = build_hann_window(spec_ctx.n_fft);
		f32* scaled_by_window = (f32*)malloc(spec_ctx.n_fft*sizeof(f32)); // allocating once the buffer for all DFTs

		float complex* complex_spectrogram = (float complex*)malloc(spec_ctx.time_bins * spec_ctx.freq_bins * sizeof(float complex));
		memset(complex_spectrogram, 0, spec_ctx.time_bins * spec_ctx.freq_bins * sizeof(float complex));

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

void radix2(const f32* scaled_signal, float complex* out_buffer, u32 N, u32 stride) {
		if (N == 1)
				out_buffer[0] = scaled_signal[0];
		else
		{
				u32 half = N/2;
				radix2(scaled_signal, out_buffer, half, 2*stride);
				radix2(scaled_signal+stride, out_buffer+half, half, 2*stride);

				float complex p, q;
				for (u32 k=0; k<half; k++) {
						p = out_buffer[k];
						q = cexpf(-2.0f * PI * I * (f32)k / (f32)N) * out_buffer[k+half];
						out_buffer[k] = p + q;
						out_buffer[k+half] = p - q;
				}
		}
}
float complex* spec_radix2(spectrogram_context spec_ctx) {
		f32* hann_window = build_hann_window(spec_ctx.n_fft);
		f32* scaled_by_window = (f32*)malloc(spec_ctx.n_fft*sizeof(f32));
		float complex* extended_buffer = (float complex*)malloc(spec_ctx.n_fft*sizeof(float complex));

		float complex* complex_spectrogram = (float complex*)malloc(spec_ctx.time_bins * spec_ctx.freq_bins * sizeof(float complex));
		memset(complex_spectrogram, 0, spec_ctx.time_bins * spec_ctx.freq_bins * sizeof(float complex));

		// fft computation
		f32* signal;
		for (u32 t=0; t<spec_ctx.time_bins; t++) {
				signal = spec_ctx.signal + spec_ctx.hop_length * t;
				for (u32 i=0; i<spec_ctx.n_fft; i++)
						scaled_by_window[i] = signal[i] * hann_window[i];

				radix2(scaled_by_window, extended_buffer,
				       spec_ctx.n_fft, 1);
				memcpy(complex_spectrogram + (t*spec_ctx.freq_bins), extended_buffer, spec_ctx.freq_bins*sizeof(float complex));
		}


		free(hann_window);
		free(scaled_by_window);
		free(extended_buffer);

		return complex_spectrogram;
}

float complex* spec_bluestein(spectrogram_context spec_ctx) {

}


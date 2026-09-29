#include "maths.h"

void fft(const f32* signal, const f32* window, float complex* out, u32 n_fft) { // lazy fourier transform implementation, fast fourier transform will come later
		// signal and window of size n_fft
		// out of size n_fft/2 + 1
		f32* scaled_by_window = (f32*)malloc(n_fft*sizeof(f32));
		for (u32 i=0; i<n_fft; i++)
				scaled_by_window[i] = signal[i] * window[i];

		memset(out, 0, (n_fft/2 + 1) * sizeof(float complex));
		for (u32 i=0; i<n_fft/2 + 1; i++) {
				float complex expo = 1.0f;
				float complex factor = cexpf(-2.0f * I * PI * (f32)i / (f32)n_fft);
				for (u32 j=0; j<n_fft; j++) {
						out[i] += expo * scaled_by_window[j];
						expo *= factor;
				}
		}

		free(scaled_by_window);
}

f32* complex_to_db(const float complex* input, u32 N) { // takes the complex array, then computes their square magniture (power), then computes 10*log_base10
		f32* out = (f32*)malloc(N*sizeof(f32));
		for (u32 i=0; i<N; i++)
				out[i] = 20.0f * log10f(cabsf(input[i]) + 1e-5f); // epsilon to avoid -inf
		
		return out;
}

f32* build_hann_window(u32 N) {
		f32* out = (f32*)malloc(N * sizeof(f32));
		for (u32 i=0; i<N; i++)
				out[i] = 0.5f - 0.5f * cosf(2.0f * PI * (f32)i / (f32)(N - 1));
		return out;
}

f32* transpose(const f32* data, u32 width, u32 height) {
		f32* out = (f32*)malloc(width*height*sizeof(f32));
		for (u32 row=0; row<height; row++)
				for (u32 col=0; col<width; col++)
						out[row + col*height] = data[col + row*width];

		return out;
}

f32* matmul(const f32* A, const f32* B, u32 N, u32 M, u32 P) {
		f32* out = (f32*)malloc(N*P*sizeof(f32));
		memset(out, 0.0f, N*P*sizeof(f32));
		for (u32 row=0; row<N; row++)
				for (u32 k=0; k<M; k++)
						for (u32 col=0; col<P; col++)
								out[row*P+col] += A[row*M+k] * B[k*P+col];

		return out;
}

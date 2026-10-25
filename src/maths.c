#include "maths.h"


f32* complex_to_db(const float complex* input, u32 N) { // takes the complex array, then computes their square magniture (power), then computes 10*log_base10
		f32* out = (f32*)malloc(N*sizeof(f32));
		for (u32 i=0; i<N; i++)
				out[i] = 20.0f * log10f(cabsf(input[i]) + 1e-5f); // epsilon to avoid -inf
		
		return out;
}

// complex to db has to be splitted in two parts (to get the energy) for mel spectrograms
f32* complex_to_energy(const float complex* input, u32 N) {
		f32* out = (f32*)malloc(N*sizeof(f32));
		f32 tmp;
		for (u32 i=0; i<N; i++) {
				tmp = cabsf(input[i]);
				out[i] = tmp*tmp; 
		}
		
		return out;
}
f32* energy_to_db(const float* input, u32 N) {
		f32* out = (f32*)malloc(N*sizeof(f32));
		for (u32 i=0; i<N; i++)
				out[i] = 10.0f * log10f(input[i] + 1e-5f); // epsilon to avoid -inf

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


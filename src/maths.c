#include "maths.h"


f64* complex_to_db(const c64* input, u32 N) { // takes the complex array, then computes their square magniture (power), then computes 10*log_base10
		f64* out = (f64*)malloc(N*sizeof(f64));
		for (u32 i=0; i<N; i++)
				out[i] = 20. * log10(cabs(input[i]) + 1e-5); // epsilon to avoid -inf
		
		return out;
}

// complex to db has to be splitted in two parts (to get the energy) for mel spectrograms
f64* complex_to_energy(const c64* input, u32 N) {
		f64* out = (f64*)malloc(N*sizeof(f64));
		for (u32 i=0; i<N; i++) {
				f64 tmp = cabs(input[i]);
				out[i] = tmp*tmp; 
		}
		
		return out;
}
f64* energy_to_db(const f64* input, u32 N) {
		f64* out = (f64*)malloc(N*sizeof(f64));
		for (u32 i=0; i<N; i++)
				out[i] = 10. * log10(input[i] + 1e-5); // epsilon to avoid -inf

		return out;
}

f64* build_hann_window(u32 N) {
		f64* out = (f64*)malloc(N * sizeof(f64));
		for (u32 i=0; i<N; i++)
				out[i] = 0.5 - 0.5 * cos(2. * PI * (f64)i / (f64)(N - 1));
		return out;
}

f64* transpose(const f64* data, u32 width, u32 height) {
		f64* out = (f64*)malloc(width*height*sizeof(f64));
		for (u32 row=0; row<height; row++)
				for (u32 col=0; col<width; col++)
						out[row + col*height] = data[col + row*width];

		return out;
}

f64* matmul(const f64* A, const f64* B, u32 N, u32 M, u32 P) {
		f64* out = (f64*)malloc(N*P*sizeof(f64));
		memset(out, 0, N*P*sizeof(f64));
		for (u32 row=0; row<N; row++)
				for (u32 k=0; k<M; k++)
						for (u32 col=0; col<P; col++)
								out[row*P+col] += A[row*M+k] * B[k*P+col];

		return out;
}


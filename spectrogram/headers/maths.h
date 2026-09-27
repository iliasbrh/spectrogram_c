#ifndef MATHS_H
#define MATHS_H

#include "base.h"

#include <complex.h>
#include "math.h"


#define PI 3.14159265358979323846

void fft(const f32* signal, const f32* window, float complex* out, u32 n_fft);
f32* complex_to_db(const float complex* input, u32 N); // takes the complex array, takes their square magniture (power), then computes 10*log_base10
f32* build_hann_window(u32 N);
f32* transpose(const f32* data, u32 width, u32 height);

#endif

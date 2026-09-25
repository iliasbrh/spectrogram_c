#ifndef MATHS_H
#define MATHS_H

#include "base.h"

#include "wav_parser.h"
#include <complex.h>
#include "math.h"
#include "string.h"


#define PI 3.14159265358979323846

void fft(f32* signal, f32* window, float complex* out, u32 n_fft);
void complex_to_db(float complex* input, f32* output, u32 N); // takes the complex array, takes their square magniture (power), then computes 10*log_base10
f32* build_hann_window(u32 N);
f32* transpose(f32* data, u32 width, u32 height);

#endif

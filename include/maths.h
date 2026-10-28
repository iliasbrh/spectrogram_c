#ifndef MATHS_H
#define MATHS_H

#include "base.h"

#include <complex.h>
#include "math.h"


#define PI 3.14159265358979323846

f64* complex_to_energy(const c64* input, u32 N);
f64* energy_to_db(const f64* input, u32 N);
f64* complex_to_db(const c64* input, u32 N); // takes the complex array, takes their square magniture (power), then computes 10*log_base10
f64* build_hann_window(u32 N);
f64* transpose(const f64* data, u32 width, u32 height);
f64* matmul(const f64* A, const f64* B, u32 N, u32 M, u32 P);


#endif

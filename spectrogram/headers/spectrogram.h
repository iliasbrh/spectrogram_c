#ifndef SPECTROGRAM_H
#define SPECTROGRAM_H

#include "base.h"
#include "wav_parser.h"
#include "maths.h"
#include "bitmap.h"

#include "stdio.h"
#include "stdlib.h"

void make_spectrogram(FILE* wav_file, wav_t wav, char* output_file_name);

#endif

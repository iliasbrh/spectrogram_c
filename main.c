#include "wav_parser.h"
#include "maths.h" // custom extended maths for fft, hann window, and complex numbers compatibility
#include "bitmap.h"
#include "spectrogram.h"


int main() {
		FILE* wav_file = fopen("./file3.wav", "rb");
		wav_t wav = read_wav(wav_file);

		char output_file_name[] = "test.bmp";
		make_spectrogram(wav_file, wav, output_file_name);

		return 0;
}

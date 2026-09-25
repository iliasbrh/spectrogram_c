#include "spectrogram.h"

void make_spectrogram(FILE* wav_file, wav_t wav, char* output_file_name) {
		u32 n_samples = (8 * wav.size) / (wav.hdr.bits_per_sample * wav.hdr.n_channels); // n_samples for each channel
		u32 n_fft = 512;
		u32 hop_length = n_fft/2;

		u32 time_dim = (n_samples - hop_length + 1) / hop_length;
		u32 freq_dim = n_fft / 2 + 1; // n_bins

		f32* signal = wav.data[0]; // only computing the spectrogram on one channel
		f32* hann_window = build_hann_window(n_fft);

		float complex* complex_spectrogram = (float complex*)malloc(time_dim * freq_dim * sizeof(float complex));
		if (!complex_spectrogram) printf("Memory allocation for spectrogram failed !");
		for (u32 t=0; t<time_dim; t++)
				fft(signal + hop_length * t, hann_window, complex_spectrogram + (t * freq_dim), n_fft);

		f32* db_spectrogram = (f32*)malloc(time_dim * freq_dim * sizeof(f32));
		complex_to_db(complex_spectrogram, db_spectrogram, time_dim*freq_dim);

		f32* transposed_img = transpose(db_spectrogram, freq_dim, time_dim);
		
		writeBMPfromF32(transposed_img, time_dim, freq_dim, output_file_name);

		for (u32 i=0; i<wav.hdr.n_channels; i++)
				free(wav.data[i]);
		free(wav.data);

		free(hann_window);
		free(complex_spectrogram);
		free(db_spectrogram);
		free(transposed_img);
}

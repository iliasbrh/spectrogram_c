#include "spectrogram.h"

f32* make_spectrogram(wav_t wav, spectrogram_context spec_ctx, char* output_file_name) {
		f32* hann_window = build_hann_window(spec_ctx.n_fft);

		float complex* complex_spectrogram = (float complex*)malloc(spec_ctx.time_bins * spec_ctx.freq_bins * sizeof(float complex));
		for (u32 t=0; t<spec_ctx.time_bins; t++)
				fft(spec_ctx.signal + spec_ctx.hop_length * t, 
				    hann_window, 
					complex_spectrogram + (t * spec_ctx.freq_bins), 
					spec_ctx.n_fft);

		f32* db_spectrogram = complex_to_db(complex_spectrogram, 
										    spec_ctx.time_bins*spec_ctx.freq_bins);
		f32* transposed_img = transpose(db_spectrogram, 
										spec_ctx.freq_bins, spec_ctx.time_bins);
		
		free(hann_window);
		free(complex_spectrogram);
		free(db_spectrogram);

		return transposed_img;
}

f32* build_mel_filter_bank(u32 n_bins, f32 max_freq, u32 n_mels) {
		f32 max_mel = 2595.0f * log10f(1.0f + max_freq / 700.0f);
		f32 min_mel = 2595.0f * log10f(1.0f + 20.0f / 700.0f);
		// the mel_bins are equally spaced from 0 to max_mel, where max_mel is max_freq in mels
		
		// storing the indexes of the frequency bins with back conversion from linear mel scale
		u32* idx_freq_from_mels = (u32*)malloc((n_mels+1) * sizeof(u32));
		f32 tmp;
		for (u32 i=0; i<n_mels+2; i++) {
				tmp = 700.0f * (expf((min_mel + (f32)i / (f32)(n_mels+1) * (max_mel - min_mel)) / 1127.0f) - 1.0f);
				// tmp = max_freq * k/n_bins for a given k that we have to find
				// k = n_bins * tmp / max_freq
				// since it might not be an integer we take its floor
				idx_freq_from_mels[i] = (u32)(n_bins * tmp / max_freq);
		}

		f32* result = (f32*)malloc(n_mels * n_bins * sizeof(f32));
		memset(result, 0.0f, n_mels*n_bins*sizeof(f32));
		for (u32 i=1; i<n_mels+1; i++) {
				u32 start = idx_freq_from_mels[i-1];
				u32 end = idx_freq_from_mels[i+1];
				
				u32 half_step = (end - start) / 2;
				for (u32 j=0; j<half_step; j++)
						result[i*n_bins+start+j] = ((f32)j) / ((f32)(half_step*half_step));
				for (u32 j=0; j<half_step; j++)
						result[i*n_bins+start+half_step+j] = 1.0f/(f32)half_step - ((f32)j) / ((f32)(half_step*half_step));
		}

		return result;
}

f32* matmul(f32* A, f32* B, u32 N, u32 M, u32 P) {
		f32* out = (f32*)malloc(N*P*sizeof(f32));
		memset(out, 0.0f, N*P*sizeof(f32));
		for (u32 row=0; row<N; row++)
				for (u32 k=0; k<M; k++)
						for (u32 col=0; col<P; col++)
								out[row*P+col] += A[row*M+k] * B[k*P+col];

		return out;
}

f32* make_mel_spec(wav_t wav, mel_spectrogram_context mel_ctx, char* output_file_name) {
		f32* vanilla_spec = make_spectrogram(wav, mel_ctx.spec_ctx, output_file_name);
		
		f32* filter_banks = build_mel_filter_bank(mel_ctx.spec_ctx.freq_bins, wav.hdr.sampling_frequency / 2, mel_ctx.n_mels);
		
		return matmul(filter_banks, vanilla_spec, mel_ctx.n_mels, mel_ctx.spec_ctx.freq_bins, mel_ctx.spec_ctx.time_bins);
}

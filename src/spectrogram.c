#include "spectrogram.h"

c64* make_complex_spectrogram(spectrogram_context spec_ctx) {
		c64* complex_spectrogram = spec_bluestein(spec_ctx);

		return complex_spectrogram;
}

f64* make_spectrogram(spectrogram_context spec_ctx) {
		c64* complex_spectrogram = make_complex_spectrogram(spec_ctx);

		f64* db_spectrogram = complex_to_db(complex_spectrogram, 
										    spec_ctx.time_bins*spec_ctx.freq_bins);
		f64* transposed_img = transpose(db_spectrogram, 
										spec_ctx.freq_bins, spec_ctx.time_bins);
		
		free(complex_spectrogram);
		free(db_spectrogram);

		return transposed_img;
}

f64* build_mel_filter_bank(u32 n_bins, f64 max_freq, u32 n_mels) {
		f64 max_mel = 2595. * log10(1. + max_freq / 700.);
		f64 min_mel = 2595. * log10(1. + 20. / 700.);
		// the mel_bins are equally spaced from min_mel to max_mel, where min_mel is for 20Hz and max_mel is max_freq in mels
		
		// storing the indexes of the frequency bins with back conversion from linear mel scale
		u32* idx_freq_from_mels = (u32*)malloc((n_mels+2) * sizeof(u32));
		for (u32 i=0; i<n_mels+2; i++) {
				f64 tmp = 700. * (exp((min_mel + (f64)i / (f64)(n_mels+1) * (max_mel - min_mel)) / 1127.) - 1.);
				idx_freq_from_mels[i] = (u32)((f64)n_bins * tmp / max_freq);
		}

		f64* result = (f64*)malloc(n_mels * n_bins * sizeof(f64));
		memset(result, 0, n_mels*n_bins*sizeof(f64));
		for (u32 i=1; i<n_mels+1; i++) {
				u32 start = idx_freq_from_mels[i-1];
				u32 end = idx_freq_from_mels[i+1];
				
				u32 half_step = (end - start) / 2;
				f64 div = 1./(f64)(half_step*half_step);
				f64 max = 1./(f64)half_step;

				for (u32 j=0; j<half_step; j++)
						result[(i-1)*n_bins+start+j] = (j+1)*div;
				for (u32 j=0; j<half_step; j++)
						result[(i-1)*n_bins+start+half_step+j] = max - (j+1)*div;
		}

		free(idx_freq_from_mels);

		return result;
}

f64* make_mel_spec(mel_spectrogram_context mel_ctx) {
		c64* complex_spectrogram = make_complex_spectrogram(mel_ctx.spec_ctx);
		f64* energy_spec = complex_to_energy(complex_spectrogram, 
										    mel_ctx.spec_ctx.time_bins*mel_ctx.spec_ctx.freq_bins);
		f64* transposed_img = transpose(energy_spec, 
										mel_ctx.spec_ctx.freq_bins, mel_ctx.spec_ctx.time_bins);

		f64* filter_banks = build_mel_filter_bank(mel_ctx.spec_ctx.freq_bins, mel_ctx.spec_ctx.max_freq, mel_ctx.n_mels);

		f64* mel_energies = matmul(filter_banks, transposed_img, 
								   mel_ctx.n_mels, mel_ctx.spec_ctx.freq_bins, mel_ctx.spec_ctx.time_bins);
		f64* out = energy_to_db(mel_energies,
								mel_ctx.n_mels*mel_ctx.spec_ctx.time_bins);

		free(complex_spectrogram);
		free(energy_spec);
		free(transposed_img);
		free(filter_banks);
		free(mel_energies);

		return out;
}

#include "arg_context.h"

args_t get_args() {
		args_t default_args = {
				.channel = 0,

				.n_fft = 2048,
				.hop_length = 0,

				.n_mels = 128 
		};

		return default_args;
}

spectrogram_context get_spectrogram_context(wav_t wav, args_t arguments) {
		spectrogram_context res = {
				.n_samples_per_channel = (8*wav.size) / (wav.hdr.bits_per_sample * wav.hdr.n_channels),
				.channel = arguments.channel,

				.n_fft = arguments.n_fft,
		};
		
		if (arguments.hop_length == 0)
				res.hop_length = res.n_fft / 2;
		else
				res.hop_length = arguments.hop_length;

		u32 silence_padding = 0; // some audio files (like the ones from Windows sound recorder) have silence at the beginning
		while (wav.data[res.channel][silence_padding++] == 0.0f) {}
		res.signal = wav.data[res.channel] + silence_padding - 1;
		res.n_samples_per_channel -= silence_padding - 1;

		res.time_bins = (res.n_samples_per_channel - res.hop_length + 1) / res.hop_length;
		res.freq_bins = res.n_fft / 2 + 1;

		return res;
}

mel_spectrogram_context get_mel_spectrogram_context(wav_t wav, args_t arguments) {
		mel_spectrogram_context res;
		res.spec_ctx = get_spectrogram_context(wav, arguments);
		
		res.n_mels = arguments.n_mels;

		return res;
}

void print_mel_ctx(mel_spectrogram_context mel_ctx) {
		printf("Samples per channel : %u\n", mel_ctx.spec_ctx.n_samples_per_channel);
		printf("Channel : %u\n", mel_ctx.spec_ctx.channel);
		printf("n_fft : %u\n", mel_ctx.spec_ctx.n_fft);
		printf("Hop length : %u\n", mel_ctx.spec_ctx.hop_length);
		printf("Time bins : %u\n", mel_ctx.spec_ctx.time_bins);
		printf("Frequency bins : %u\n", mel_ctx.spec_ctx.freq_bins);
		printf("n_mels : %u\n", mel_ctx.n_mels);
}

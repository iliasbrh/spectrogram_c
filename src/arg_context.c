#include "arg_context.h"

static const char* const usages[] = {
		"spectrogram [options] [[--] args]",
		"spectrogram [options]",
		NULL,
};

args_t parse_args(i32 argc, const char** argv) {
		args_t res = {
				.channel = 0,

				.n_fft = 2048,
				.hop_length = 0,

				.make_mel = true,
				.n_mels = 128,
				.file_name = NULL
		};

		struct argparse_option options[] = {
				OPT_HELP(),
				OPT_GROUP("Spectrogram's hyperparameters"),
				OPT_INTEGER('n', "n_fft", &res.n_fft, "length of sliding Short Time Fourier Transform window, default 2048", NULL, 0, 0),
				OPT_INTEGER('h', "hop_length", &res.hop_length, "length of hops between each Short Time Fourier Transform computations, default n_fft/2", NULL, 0, 0),
				OPT_INTEGER('c', "channel", &res.channel, "audio channel to compute the spectrogram on, default 0", NULL, 0, 0),
				OPT_GROUP("Mel spectrogram's hyperparameters"),
				OPT_BOOLEAN('\0', "mel", &res.make_mel, "output a mel spectrogram instead of a vanilla spectrogram, default true", NULL, 0, 0),
				OPT_INTEGER('m', "n_mels", &res.n_mels, "mel bins for mel spectrograms, default 128", NULL, 0, 0),
				OPT_STRING(0, "name", &res.file_name, "name of the .wav file to compute the spectrogram from", NULL, 0, 0),
				OPT_END(),
		};

		struct argparse argparse;
		argparse_init(&argparse, options, usages, 0);
		argparse_describe(&argparse, "\nThis program computes spectrograms, or mel spectrograms, from .wav audio files to .bmp image files.", "\n");

		argc = argparse_parse(&argparse, argc, argv);

		if (res.file_name == NULL)
				fprintf(stderr, "Error : no file name specified.\n");

		return res;
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

		res.time_bins = (u16)((res.n_samples_per_channel - res.hop_length + 1) / res.hop_length);
		res.freq_bins = res.n_fft / 2 + 1;

		res.max_freq = (f32)wav.hdr.sampling_frequency / 2.0f;

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

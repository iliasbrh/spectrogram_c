#include "spectrogram.h"
#include "bitmap.h"



int main(i32 argc, const char** argv) {
		args_t arguments = parse_args(argc, argv);

		FILE* wav_file = fopen(arguments.file_name, "rb");
		wav_t wav = read_wav(wav_file);
		
		if (arguments.make_mel) {
				char output_file_name[] = "mel_spectrogram.bmp";
				mel_spectrogram_context mel_ctx = get_mel_spectrogram_context(wav, arguments);
				f32* spec = make_mel_spec(mel_ctx);
				writeBMPfromF32(spec, mel_ctx.spec_ctx.time_bins, mel_ctx.n_mels, output_file_name);
				free(spec);
				printf("\nThe spectrogram was successfully generated as mel_spectrogram.bmp.\n");
		}
		else {
				char output_file_name[] = "spectrogram.bmp";
				spectrogram_context spec_ctx = get_spectrogram_context(wav, arguments);
				f32* spec = make_spectrogram(spec_ctx);
				writeBMPfromF32(spec, spec_ctx.time_bins, spec_ctx.freq_bins, output_file_name);
				free(spec);
				printf("\nThe spectrogram was successfully generated as spectrogram.bmp.\n");
		}

		


		for (u32 i=0; i<wav.hdr.n_channels; i++)
				free(wav.data[i]);
		free(wav.data);

		return 0;
}

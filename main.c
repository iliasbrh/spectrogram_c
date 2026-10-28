#include "spectrogram.h"
#include "bitmap.h"

int main(i32 argc, const char** argv) {
		args_t arguments = parse_args(argc, argv);

		FILE* wav_file = fopen(arguments.file_name, "rb");
		wav_t wav = read_wav(wav_file);
		f64* spec;
		
		if (arguments.make_mel) {
				mel_spectrogram_context mel_ctx = get_mel_spectrogram_context(wav, arguments);
				spec = make_mel_spec(mel_ctx);
				writeBMPfromF32(spec, mel_ctx.spec_ctx.time_bins, mel_ctx.n_mels, arguments.output_file_name);
		}
		else {
				spectrogram_context spec_ctx = get_spectrogram_context(wav, arguments);
				spec = make_spectrogram(spec_ctx);
				writeBMPfromF32(spec, spec_ctx.time_bins, spec_ctx.freq_bins, arguments.output_file_name);
		}


		free(spec);
		for (u32 i=0; i<wav.hdr.n_channels; i++)
				free(wav.data[i]);
		free(wav.data);
		fclose(wav_file);

		printf("\nThe spectrogram was successfully generated as %s.\n", arguments.output_file_name);

		return 0;
}

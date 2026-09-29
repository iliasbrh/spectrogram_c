#include "spectrogram.h"
#include "bitmap.h"



int main(i32 argc, const char** argv) {
		args_t arguments = parse_args(argc, argv);

		FILE* wav_file = fopen(arguments.file_name, "rb");
		wav_t wav = read_wav(wav_file);

		mel_spectrogram_context mel_ctx = get_mel_spectrogram_context(wav, arguments);

		// print_mel_ctx(mel_ctx);

		char output_file_name[] = "spectrogram.bmp";
		f32* spec = make_mel_spec(mel_ctx);

		
		writeBMPfromF32(spec, mel_ctx.spec_ctx.time_bins, mel_ctx.n_mels, output_file_name);


		for (u32 i=0; i<wav.hdr.n_channels; i++)
				free(wav.data[i]);
		free(wav.data);
		free(spec);

		return 0;
}

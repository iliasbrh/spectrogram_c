#include "spectrogram.h"
#include "bitmap.h"



int main() {
		FILE* wav_file = fopen("./file3.wav", "rb");
		wav_t wav = read_wav(wav_file);

		args_t arguments = get_args();
		mel_spectrogram_context mel_ctx = get_mel_spectrogram_context(wav, arguments);

		print_mel_ctx(mel_ctx);

		char output_file_name[] = "spectrogram.bmp";
		f32* spec = make_mel_spec(wav, mel_ctx, output_file_name);


		writeBMPfromF32(spec, mel_ctx.spec_ctx.time_bins, mel_ctx.n_mels, output_file_name);


		for (u32 i=0; i<wav.hdr.n_channels; i++)
				free(wav.data[i]);
		free(wav.data);
		free(spec);

		return 0;
}

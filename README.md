# Spectrogram

Computing spectrograms from .wav audio files into .bmp format.  

The project does the following tasks :  
- parses .wav files to extract audio signals as float arrays  
- computes Short Time Fourier Tranforms with a Hann window applied and regular hops  
- writes a Bitmap file (.bmp, uncompressed) for the resulting image  

## To run it on your own

Add a .wav file in the main directory, then run `make build` and `./spectrogram file.wav`. The info header of the wav file will be printed and the .bmp resulting file will appear.

## Flags and recommended parameters  

`./spectrogram --help` provides the following flag details.  
  
<pre>
Spectrogram's hyperparameters  
    -n, --n_fft=int         length of the sliding Short Time Fourier Transform window, default 512  
    -h, --hop_length=int    length of the hops between each Short Time Fourier Transform computations, default n_fft/2  
    -c, --channel=int       index of the audio channel to compute the spectrogram on, default 0  
  
Mel spectrogram's hyperparameters  
    --mel                   output a mel spectrogram instead of a vanilla spectrogram, default false  
    -m, --n_mels=int        mel bins for mel spectrograms, default 32  
</pre>
  
For vanilla spectrograms I recommend `n_fft=512`, while for mel spectrograms more bins are required to have better resolution, especially for lower frequencies, so I recommend `n_fft=2048` and `n_mels=128`, or `n_fft=1024` and `n_mels=32`. You can then adjust depending on your audio's sampling frequency and duration.  

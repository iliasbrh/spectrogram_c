# Spectrogram

Computing spectrograms from .wav audio files into .bmp format.  

The project does the following tasks :  
- parses .wav files to extract audio signals as float arrays  
- computes Short Time Fourier Tranforms with a Hann window applied and regular hops  
- writes a Bitmap file (.bmp, uncompressed) for the resulting image  

## To run it on your own

Add a .wav file in the main directory, then edit the main.c file with the .wav file name and the .bmp resulting file name that you want as output.  

Then run `make build` and `./run.exe`. The info header of the file will be printed and the .bmp resulting file will appear.

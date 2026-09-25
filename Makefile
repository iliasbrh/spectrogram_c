build:
	gcc spectrogram/src/*.c main.c \
		-I spectrogram/headers -lm -O3 \
	   	-o run.exe

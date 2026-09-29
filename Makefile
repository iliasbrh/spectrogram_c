SOURCES = src/*.c \
		  dependencies/argparse/argparse.c \
		  main.c

build: $(SOURCES)
	@gcc $(SOURCES) \
		-I include -I dependencies/argparse \
		-lm -O3 \
	   	-o spectrogram

debug: $(SOURCES)
	gcc $(SOURCES) -g \
		-pedantic -Wall -Wextra -Wshadow -Wconversion -fsanitize=undefined \
		-lm \
		-I include -I dependencies/argparse \
		-o debug.exe

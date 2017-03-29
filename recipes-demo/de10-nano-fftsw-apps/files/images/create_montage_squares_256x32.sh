#!/bin/sh

montage \
	-background '#000000' \
	-font Liberation-Mono-Regular \
	c16_256x32_square.png \
	c32_256x32_square.png \
	fft_256x32_square.png \
	fftdma_256x32_square.png \
	neon16_256x32_square.png \
	neon32_256x32_square.png \
	-tile 1x6 \
	-geometry 640x480+5+5 \
	squares_256x32.png


#!/bin/sh

montage \
	-background '#000000' \
	-font Liberation-Mono-Regular \
	c16_4096_square.png \
	c32_4096_square.png \
	fft_4096_square.png \
	fftdma_4096_square.png \
	neon16_4096_square.png \
	neon32_4096_square.png \
	-tile 1x6 \
	-geometry 640x480+5+5 \
	squares_4096.png


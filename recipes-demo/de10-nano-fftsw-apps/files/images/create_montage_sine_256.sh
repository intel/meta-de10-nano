#!/bin/sh

montage \
	-background '#000000' \
	-font Liberation-Mono-Regular \
	c16_256_sine.png \
	c32_256_sine.png \
	fft_256_sine.png \
	fftdma_256_sine.png \
	neon16_256_sine.png \
	neon32_256_sine.png \
	-tile 1x6 \
	-geometry 640x480+5+5 \
	sines_256.png


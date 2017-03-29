#!/bin/sh

montage \
	-background '#000000' \
	-font Liberation-Mono-Regular \
	c16_4096_sine.png \
	c32_4096_sine.png \
	fft_4096_sine.png \
	fftdma_4096_sine.png \
	neon16_4096_sine.png \
	neon32_4096_sine.png \
	-tile 1x6 \
	-geometry 640x480+5+5 \
	sines_4096.png


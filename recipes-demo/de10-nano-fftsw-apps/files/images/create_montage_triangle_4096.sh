#!/bin/sh

montage \
	-background '#000000' \
	-font Liberation-Mono-Regular \
	c16_4096_triangle.png \
	c32_4096_triangle.png \
	fft_4096_triangle.png \
	fftdma_4096_triangle.png \
	neon16_4096_triangle.png \
	neon32_4096_triangle.png \
	-tile 1x6 \
	-geometry 640x480+5+5 \
	triangles_4096.png


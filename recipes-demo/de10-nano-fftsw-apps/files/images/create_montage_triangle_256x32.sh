#!/bin/sh

montage \
	-background '#000000' \
	-font Liberation-Mono-Regular \
	c16_256x32_triangle.png \
	c32_256x32_triangle.png \
	fft_256x32_triangle.png \
	fftdma_256x32_triangle.png \
	neon16_256x32_triangle.png \
	neon32_256x32_triangle.png \
	-tile 1x6 \
	-geometry 640x480+5+5 \
	triangles_256x32.png


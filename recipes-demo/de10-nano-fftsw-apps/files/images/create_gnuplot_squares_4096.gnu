#!/usr/bin/gnuplot -d

set terminal pngcairo background "#FFFFFF" fontscale 1.0 size 1024, 768

set output "c16_4096_square.png"
set title "c16_4096_square"
plot for [i=1:2] "c16_4096_square.txt" using i with lines title columnhead

set output "c32_4096_square.png"
set title "c32_4096_square"
plot for [i=1:2] "c32_4096_square.txt" using i with lines title columnhead

set output "neon16_4096_square.png"
set title "neon16_4096_square"
plot for [i=1:2] "neon16_4096_square.txt" using i with lines title columnhead

set output "neon32_4096_square.png"
set title "neon32_4096_square"
plot for [i=1:2] "neon32_4096_square.txt" using i with lines title columnhead

set output "fft_4096_square.png"
set title "fft_4096_square"
plot for [i=1:2] "fft_4096_square.txt" using i with lines title columnhead

set output "fftdma_4096_square.png"
set title "fftdma_4096_square"
plot for [i=1:2] "fftdma_4096_square.txt" using i with lines title columnhead


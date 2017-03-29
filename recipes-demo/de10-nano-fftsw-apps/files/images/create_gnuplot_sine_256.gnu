#!/usr/bin/gnuplot -d

set terminal pngcairo background "#FFFFFF" fontscale 1.0 size 1024, 768

set output "c16_256_sine.png"
set title "c16_256_sine"
plot for [i=1:2] "c16_256_sine.txt" using i with lines title columnhead

set output "c32_256_sine.png"
set title "c32_256_sine"
plot for [i=1:2] "c32_256_sine.txt" using i with lines title columnhead

set output "neon16_256_sine.png"
set title "neon16_256_sine"
plot for [i=1:2] "neon16_256_sine.txt" using i with lines title columnhead

set output "neon32_256_sine.png"
set title "neon32_256_sine"
plot for [i=1:2] "neon32_256_sine.txt" using i with lines title columnhead

set output "fft_256_sine.png"
set title "fft_256_sine"
plot for [i=1:2] "fft_256_sine.txt" using i with lines title columnhead

set output "fftdma_256_sine.png"
set title "fftdma_256_sine"
plot for [i=1:2] "fftdma_256_sine.txt" using i with lines title columnhead


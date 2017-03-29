#!/bin/sh

[ -d "output_waveforms" ] || {
	echo ""
	echo "ERROR: directory 'output_waveforms' does not exits, please create it."
	echo ""
	exit 1	
}

./c16_256x32x128 --input=input_waveforms/ne10cpx_short_sine1M.bin --output=output_waveforms/c16_256x32x128_sine.bin > sine256x32x128.log
rm output_waveforms/c16_256x32x128_sine.bin
./c32_256x32x128 --input=input_waveforms/ne10cpx_long_sine1M.bin --output=output_waveforms/c32_256x32x128_sine.bin >> sine256x32x128.log
rm output_waveforms/c32_256x32x128_sine.bin
./neon16_256x32x128 --input=input_waveforms/ne10cpx_short_sine1M.bin --output=output_waveforms/neon16_256x32x128_sine.bin >> sine256x32x128.log
rm output_waveforms/neon16_256x32x128_sine.bin
./neon32_256x32x128 --input=input_waveforms/ne10cpx_long_sine1M.bin --output=output_waveforms/neon32_256x32x128_sine.bin >> sine256x32x128.log
rm output_waveforms/neon32_256x32x128_sine.bin
./fft_256x32x128 --input=input_waveforms/ne10cpx_short_sine1M.bin --output=output_waveforms/fft_256x32x128_sine.bin >> sine256x32x128.log
rm output_waveforms/fft_256x32x128_sine.bin
./fftdma_256x32x128 --input=input_waveforms/ne10cpx_short_sine1M.bin --output=output_waveforms/fftdma_256x32x128_sine.bin >> sine256x32x128.log
rm output_waveforms/fftdma_256x32x128_sine.bin

./c16_256x32x128 --input=input_waveforms/ne10cpx_short_square1M.bin --output=output_waveforms/c16_256x32x128_square.bin > square256x32x128.log
rm output_waveforms/c16_256x32x128_square.bin
./c32_256x32x128 --input=input_waveforms/ne10cpx_long_square1M.bin --output=output_waveforms/c32_256x32x128_square.bin >> square256x32x128.log
rm output_waveforms/c32_256x32x128_square.bin
./neon16_256x32x128 --input=input_waveforms/ne10cpx_short_square1M.bin --output=output_waveforms/neon16_256x32x128_square.bin >> square256x32x128.log
rm output_waveforms/neon16_256x32x128_square.bin
./neon32_256x32x128 --input=input_waveforms/ne10cpx_long_square1M.bin --output=output_waveforms/neon32_256x32x128_square.bin >> square256x32x128.log
rm output_waveforms/neon32_256x32x128_square.bin
./fft_256x32x128 --input=input_waveforms/ne10cpx_short_square1M.bin --output=output_waveforms/fft_256x32x128_square.bin >> square256x32x128.log
rm output_waveforms/fft_256x32x128_square.bin
./fftdma_256x32x128 --input=input_waveforms/ne10cpx_short_square1M.bin --output=output_waveforms/fftdma_256x32x128_square.bin >> square256x32x128.log
rm output_waveforms/fftdma_256x32x128_square.bin

./c16_256x32x128 --input=input_waveforms/ne10cpx_short_triangle1M.bin --output=output_waveforms/c16_256x32x128_triangle.bin > triangle256x32x128.log
rm output_waveforms/c16_256x32x128_triangle.bin
./c32_256x32x128 --input=input_waveforms/ne10cpx_long_triangle1M.bin --output=output_waveforms/c32_256x32x128_triangle.bin >> triangle256x32x128.log
rm output_waveforms/c32_256x32x128_triangle.bin
./neon16_256x32x128 --input=input_waveforms/ne10cpx_short_triangle1M.bin --output=output_waveforms/neon16_256x32x128_triangle.bin >> triangle256x32x128.log
rm output_waveforms/neon16_256x32x128_triangle.bin
./neon32_256x32x128 --input=input_waveforms/ne10cpx_long_triangle1M.bin --output=output_waveforms/neon32_256x32x128_triangle.bin >> triangle256x32x128.log
rm output_waveforms/neon32_256x32x128_triangle.bin
./fft_256x32x128 --input=input_waveforms/ne10cpx_short_triangle1M.bin --output=output_waveforms/fft_256x32x128_triangle.bin >> triangle256x32x128.log
rm output_waveforms/fft_256x32x128_triangle.bin
./fftdma_256x32x128 --input=input_waveforms/ne10cpx_short_triangle1M.bin --output=output_waveforms/fftdma_256x32x128_triangle.bin >> triangle256x32x128.log
rm output_waveforms/fftdma_256x32x128_triangle.bin



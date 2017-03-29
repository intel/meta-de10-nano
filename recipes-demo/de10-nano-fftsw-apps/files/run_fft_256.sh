#!/bin/sh

[ -d "output_waveforms" ] || {
	echo ""
	echo "ERROR: directory 'output_waveforms' does not exits, please create it."
	echo ""
	exit 1	
}

./c16_256 --input=input_waveforms/ne10cpx_short_sine256.bin --output=output_waveforms/c16_256_sine.bin > sine256.log
cat output_waveforms/c16_256_sine.bin | ./ne10cpx_short_to_text > output_waveforms/c16_256_sine.txt
./c32_256 --input=input_waveforms/ne10cpx_long_sine256.bin --output=output_waveforms/c32_256_sine.bin >> sine256.log
cat output_waveforms/c32_256_sine.bin | ./ne10cpx_long_to_text > output_waveforms/c32_256_sine.txt
./neon16_256 --input=input_waveforms/ne10cpx_short_sine256.bin --output=output_waveforms/neon16_256_sine.bin >> sine256.log
cat output_waveforms/neon16_256_sine.bin | ./ne10cpx_short_to_text > output_waveforms/neon16_256_sine.txt
./neon32_256 --input=input_waveforms/ne10cpx_long_sine256.bin --output=output_waveforms/neon32_256_sine.bin >> sine256.log
cat output_waveforms/neon32_256_sine.bin | ./ne10cpx_long_to_text > output_waveforms/neon32_256_sine.txt
./fft_256 --input=input_waveforms/ne10cpx_short_sine256.bin --output=output_waveforms/fft_256_sine.bin >> sine256.log
cat output_waveforms/fft_256_sine.bin | ./ne10cpx_long_to_text > output_waveforms/fft_256_sine.txt
./fftdma_256 --input=input_waveforms/ne10cpx_short_sine256.bin --output=output_waveforms/fftdma_256_sine.bin >> sine256.log
cat output_waveforms/fftdma_256_sine.bin | ./ne10cpx_long_to_text > output_waveforms/fftdma_256_sine.txt

./c16_256 --input=input_waveforms/ne10cpx_short_square256.bin --output=output_waveforms/c16_256_square.bin > square256.log
cat output_waveforms/c16_256_square.bin | ./ne10cpx_short_to_text > output_waveforms/c16_256_square.txt
./c32_256 --input=input_waveforms/ne10cpx_long_square256.bin --output=output_waveforms/c32_256_square.bin >> square256.log
cat output_waveforms/c32_256_square.bin | ./ne10cpx_long_to_text > output_waveforms/c32_256_square.txt
./neon16_256 --input=input_waveforms/ne10cpx_short_square256.bin --output=output_waveforms/neon16_256_square.bin >> square256.log
cat output_waveforms/neon16_256_square.bin | ./ne10cpx_short_to_text > output_waveforms/neon16_256_square.txt
./neon32_256 --input=input_waveforms/ne10cpx_long_square256.bin --output=output_waveforms/neon32_256_square.bin >> square256.log
cat output_waveforms/neon32_256_square.bin | ./ne10cpx_long_to_text > output_waveforms/neon32_256_square.txt
./fft_256 --input=input_waveforms/ne10cpx_short_square256.bin --output=output_waveforms/fft_256_square.bin >> square256.log
cat output_waveforms/fft_256_square.bin | ./ne10cpx_long_to_text > output_waveforms/fft_256_square.txt
./fftdma_256 --input=input_waveforms/ne10cpx_short_square256.bin --output=output_waveforms/fftdma_256_square.bin >> square256.log
cat output_waveforms/fftdma_256_square.bin | ./ne10cpx_long_to_text > output_waveforms/fftdma_256_square.txt

./c16_256 --input=input_waveforms/ne10cpx_short_triangle256.bin --output=output_waveforms/c16_256_triangle.bin > triangle256.log
cat output_waveforms/c16_256_triangle.bin | ./ne10cpx_short_to_text > output_waveforms/c16_256_triangle.txt
./c32_256 --input=input_waveforms/ne10cpx_long_triangle256.bin --output=output_waveforms/c32_256_triangle.bin >> triangle256.log
cat output_waveforms/c32_256_triangle.bin | ./ne10cpx_long_to_text > output_waveforms/c32_256_triangle.txt
./neon16_256 --input=input_waveforms/ne10cpx_short_triangle256.bin --output=output_waveforms/neon16_256_triangle.bin >> triangle256.log
cat output_waveforms/neon16_256_triangle.bin | ./ne10cpx_short_to_text > output_waveforms/neon16_256_triangle.txt
./neon32_256 --input=input_waveforms/ne10cpx_long_triangle256.bin --output=output_waveforms/neon32_256_triangle.bin >> triangle256.log
cat output_waveforms/neon32_256_triangle.bin | ./ne10cpx_long_to_text > output_waveforms/neon32_256_triangle.txt
./fft_256 --input=input_waveforms/ne10cpx_short_triangle256.bin --output=output_waveforms/fft_256_triangle.bin >> triangle256.log
cat output_waveforms/fft_256_triangle.bin | ./ne10cpx_long_to_text > output_waveforms/fft_256_triangle.txt
./fftdma_256 --input=input_waveforms/ne10cpx_short_triangle256.bin --output=output_waveforms/fftdma_256_triangle.bin >> triangle256.log
cat output_waveforms/fftdma_256_triangle.bin | ./ne10cpx_long_to_text > output_waveforms/fftdma_256_triangle.txt


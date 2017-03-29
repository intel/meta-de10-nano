#!/bin/sh

[ -d "output_waveforms" ] || {
	echo ""
	echo "ERROR: directory 'output_waveforms' does not exits, please create it."
	echo ""
	exit 1	
}

./c16_256x32 --input=input_waveforms/ne10cpx_short_sine256x32.bin --output=output_waveforms/c16_256x32_sine.bin > sine256x32.log
cat output_waveforms/c16_256x32_sine.bin | ./ne10cpx_short_to_text > output_waveforms/c16_256x32_sine.txt
./c32_256x32 --input=input_waveforms/ne10cpx_long_sine256x32.bin --output=output_waveforms/c32_256x32_sine.bin >> sine256x32.log
cat output_waveforms/c32_256x32_sine.bin | ./ne10cpx_long_to_text > output_waveforms/c32_256x32_sine.txt
./neon16_256x32 --input=input_waveforms/ne10cpx_short_sine256x32.bin --output=output_waveforms/neon16_256x32_sine.bin >> sine256x32.log
cat output_waveforms/neon16_256x32_sine.bin | ./ne10cpx_short_to_text > output_waveforms/neon16_256x32_sine.txt
./neon32_256x32 --input=input_waveforms/ne10cpx_long_sine256x32.bin --output=output_waveforms/neon32_256x32_sine.bin >> sine256x32.log
cat output_waveforms/neon32_256x32_sine.bin | ./ne10cpx_long_to_text > output_waveforms/neon32_256x32_sine.txt
./fft_256x32 --input=input_waveforms/ne10cpx_short_sine256x32.bin --output=output_waveforms/fft_256x32_sine.bin >> sine256x32.log
cat output_waveforms/fft_256x32_sine.bin | ./ne10cpx_long_to_text > output_waveforms/fft_256x32_sine.txt
./fftdma_256x32 --input=input_waveforms/ne10cpx_short_sine256x32.bin --output=output_waveforms/fftdma_256x32_sine.bin >> sine256x32.log
cat output_waveforms/fftdma_256x32_sine.bin | ./ne10cpx_long_to_text > output_waveforms/fftdma_256x32_sine.txt

./c16_256x32 --input=input_waveforms/ne10cpx_short_square256x32.bin --output=output_waveforms/c16_256x32_square.bin > square256x32.log
cat output_waveforms/c16_256x32_square.bin | ./ne10cpx_short_to_text > output_waveforms/c16_256x32_square.txt
./c32_256x32 --input=input_waveforms/ne10cpx_long_square256x32.bin --output=output_waveforms/c32_256x32_square.bin >> square256x32.log
cat output_waveforms/c32_256x32_square.bin | ./ne10cpx_long_to_text > output_waveforms/c32_256x32_square.txt
./neon16_256x32 --input=input_waveforms/ne10cpx_short_square256x32.bin --output=output_waveforms/neon16_256x32_square.bin >> square256x32.log
cat output_waveforms/neon16_256x32_square.bin | ./ne10cpx_short_to_text > output_waveforms/neon16_256x32_square.txt
./neon32_256x32 --input=input_waveforms/ne10cpx_long_square256x32.bin --output=output_waveforms/neon32_256x32_square.bin >> square256x32.log
cat output_waveforms/neon32_256x32_square.bin | ./ne10cpx_long_to_text > output_waveforms/neon32_256x32_square.txt
./fft_256x32 --input=input_waveforms/ne10cpx_short_square256x32.bin --output=output_waveforms/fft_256x32_square.bin >> square256x32.log
cat output_waveforms/fft_256x32_square.bin | ./ne10cpx_long_to_text > output_waveforms/fft_256x32_square.txt
./fftdma_256x32 --input=input_waveforms/ne10cpx_short_square256x32.bin --output=output_waveforms/fftdma_256x32_square.bin >> square256x32.log
cat output_waveforms/fftdma_256x32_square.bin | ./ne10cpx_long_to_text > output_waveforms/fftdma_256x32_square.txt

./c16_256x32 --input=input_waveforms/ne10cpx_short_triangle256x32.bin --output=output_waveforms/c16_256x32_triangle.bin > triangle256x32.log
cat output_waveforms/c16_256x32_triangle.bin | ./ne10cpx_short_to_text > output_waveforms/c16_256x32_triangle.txt
./c32_256x32 --input=input_waveforms/ne10cpx_long_triangle256x32.bin --output=output_waveforms/c32_256x32_triangle.bin >> triangle256x32.log
cat output_waveforms/c32_256x32_triangle.bin | ./ne10cpx_long_to_text > output_waveforms/c32_256x32_triangle.txt
./neon16_256x32 --input=input_waveforms/ne10cpx_short_triangle256x32.bin --output=output_waveforms/neon16_256x32_triangle.bin >> triangle256x32.log
cat output_waveforms/neon16_256x32_triangle.bin | ./ne10cpx_short_to_text > output_waveforms/neon16_256x32_triangle.txt
./neon32_256x32 --input=input_waveforms/ne10cpx_long_triangle256x32.bin --output=output_waveforms/neon32_256x32_triangle.bin >> triangle256x32.log
cat output_waveforms/neon32_256x32_triangle.bin | ./ne10cpx_long_to_text > output_waveforms/neon32_256x32_triangle.txt
./fft_256x32 --input=input_waveforms/ne10cpx_short_triangle256x32.bin --output=output_waveforms/fft_256x32_triangle.bin >> triangle256x32.log
cat output_waveforms/fft_256x32_triangle.bin | ./ne10cpx_long_to_text > output_waveforms/fft_256x32_triangle.txt
./fftdma_256x32 --input=input_waveforms/ne10cpx_short_triangle256x32.bin --output=output_waveforms/fftdma_256x32_triangle.bin >> triangle256x32.log
cat output_waveforms/fftdma_256x32_triangle.bin | ./ne10cpx_long_to_text > output_waveforms/fftdma_256x32_triangle.txt


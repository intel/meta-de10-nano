#!/bin/sh

[ -d "output_waveforms" ] || {
	echo ""
	echo "ERROR: directory 'output_waveforms' does not exits, please create it."
	echo ""
	exit 1	
}

./c16_4096 --input=input_waveforms/ne10cpx_short_sine4096.bin --output=output_waveforms/c16_4096_sine.bin > sine4096.log
cat output_waveforms/c16_4096_sine.bin | ./ne10cpx_short_to_text > output_waveforms/c16_4096_sine.txt
./c32_4096 --input=input_waveforms/ne10cpx_long_sine4096.bin --output=output_waveforms/c32_4096_sine.bin >> sine4096.log
cat output_waveforms/c32_4096_sine.bin | ./ne10cpx_long_to_text > output_waveforms/c32_4096_sine.txt
./neon16_4096 --input=input_waveforms/ne10cpx_short_sine4096.bin --output=output_waveforms/neon16_4096_sine.bin >> sine4096.log
cat output_waveforms/neon16_4096_sine.bin | ./ne10cpx_short_to_text > output_waveforms/neon16_4096_sine.txt
./neon32_4096 --input=input_waveforms/ne10cpx_long_sine4096.bin --output=output_waveforms/neon32_4096_sine.bin >> sine4096.log
cat output_waveforms/neon32_4096_sine.bin | ./ne10cpx_long_to_text > output_waveforms/neon32_4096_sine.txt
./fft_4096 --input=input_waveforms/ne10cpx_short_sine4096.bin --output=output_waveforms/fft_4096_sine.bin >> sine4096.log
cat output_waveforms/fft_4096_sine.bin | ./ne10cpx_long_to_text > output_waveforms/fft_4096_sine.txt
./fftdma_4096 --input=input_waveforms/ne10cpx_short_sine4096.bin --output=output_waveforms/fftdma_4096_sine.bin >> sine4096.log
cat output_waveforms/fftdma_4096_sine.bin | ./ne10cpx_long_to_text > output_waveforms/fftdma_4096_sine.txt

./c16_4096 --input=input_waveforms/ne10cpx_short_square4096.bin --output=output_waveforms/c16_4096_square.bin > square4096.log
cat output_waveforms/c16_4096_square.bin | ./ne10cpx_short_to_text > output_waveforms/c16_4096_square.txt
./c32_4096 --input=input_waveforms/ne10cpx_long_square4096.bin --output=output_waveforms/c32_4096_square.bin >> square4096.log
cat output_waveforms/c32_4096_square.bin | ./ne10cpx_long_to_text > output_waveforms/c32_4096_square.txt
./neon16_4096 --input=input_waveforms/ne10cpx_short_square4096.bin --output=output_waveforms/neon16_4096_square.bin >> square4096.log
cat output_waveforms/neon16_4096_square.bin | ./ne10cpx_short_to_text > output_waveforms/neon16_4096_square.txt
./neon32_4096 --input=input_waveforms/ne10cpx_long_square4096.bin --output=output_waveforms/neon32_4096_square.bin >> square4096.log
cat output_waveforms/neon32_4096_square.bin | ./ne10cpx_long_to_text > output_waveforms/neon32_4096_square.txt
./fft_4096 --input=input_waveforms/ne10cpx_short_square4096.bin --output=output_waveforms/fft_4096_square.bin >> square4096.log
cat output_waveforms/fft_4096_square.bin | ./ne10cpx_long_to_text > output_waveforms/fft_4096_square.txt
./fftdma_4096 --input=input_waveforms/ne10cpx_short_square4096.bin --output=output_waveforms/fftdma_4096_square.bin >> square4096.log
cat output_waveforms/fftdma_4096_square.bin | ./ne10cpx_long_to_text > output_waveforms/fftdma_4096_square.txt

./c16_4096 --input=input_waveforms/ne10cpx_short_triangle4096.bin --output=output_waveforms/c16_4096_triangle.bin > triangle4096.log
cat output_waveforms/c16_4096_triangle.bin | ./ne10cpx_short_to_text > output_waveforms/c16_4096_triangle.txt
./c32_4096 --input=input_waveforms/ne10cpx_long_triangle4096.bin --output=output_waveforms/c32_4096_triangle.bin >> triangle4096.log
cat output_waveforms/c32_4096_triangle.bin | ./ne10cpx_long_to_text > output_waveforms/c32_4096_triangle.txt
./neon16_4096 --input=input_waveforms/ne10cpx_short_triangle4096.bin --output=output_waveforms/neon16_4096_triangle.bin >> triangle4096.log
cat output_waveforms/neon16_4096_triangle.bin | ./ne10cpx_short_to_text > output_waveforms/neon16_4096_triangle.txt
./neon32_4096 --input=input_waveforms/ne10cpx_long_triangle4096.bin --output=output_waveforms/neon32_4096_triangle.bin >> triangle4096.log
cat output_waveforms/neon32_4096_triangle.bin | ./ne10cpx_long_to_text > output_waveforms/neon32_4096_triangle.txt
./fft_4096 --input=input_waveforms/ne10cpx_short_triangle4096.bin --output=output_waveforms/fft_4096_triangle.bin >> triangle4096.log
cat output_waveforms/fft_4096_triangle.bin | ./ne10cpx_long_to_text > output_waveforms/fft_4096_triangle.txt
./fftdma_4096 --input=input_waveforms/ne10cpx_short_triangle4096.bin --output=output_waveforms/fftdma_4096_triangle.bin >> triangle4096.log
cat output_waveforms/fftdma_4096_triangle.bin | ./ne10cpx_long_to_text > output_waveforms/fftdma_4096_triangle.txt


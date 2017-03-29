#!/bin/sh

[ -d "output_waveforms" ] || {
	echo ""
	echo "ERROR: directory 'output_waveforms' does not exits, please create it."
	echo ""
	exit 1	
}

./stream_fpga_256x32x128 --input=input_waveforms/ne10cpx_short_sine256.bin --output=output_waveforms/stream_fpga_256x32x128_sine.bin > sine_stream_256x32x128.log
rm output_waveforms/stream_fpga_256x32x128_sine.bin
./stream_neon32_256x32x128 --input=input_waveforms/ne10cpx_long_sine256.bin --output=output_waveforms/stream_neon32_256x32x128_sine.bin >> sine_stream_256x32x128.log
rm output_waveforms/stream_neon32_256x32x128_sine.bin
./stream_raw_256x32x128 --input=input_waveforms/ne10cpx_long_sine256.bin --output=output_waveforms/stream_raw_256x32x128_sine.bin >> sine_stream_256x32x128.log
rm output_waveforms/stream_raw_256x32x128_sine.bin

./stream_fpga_256x32x128 --input=input_waveforms/ne10cpx_short_square256.bin --output=output_waveforms/stream_fpga_256x32x128_square.bin > square_stream_256x32x128.log
rm output_waveforms/stream_fpga_256x32x128_square.bin
./stream_neon32_256x32x128 --input=input_waveforms/ne10cpx_long_square256.bin --output=output_waveforms/stream_neon32_256x32x128_square.bin >> square_stream_256x32x128.log
rm output_waveforms/stream_neon32_256x32x128_square.bin
./stream_raw_256x32x128 --input=input_waveforms/ne10cpx_long_square256.bin --output=output_waveforms/stream_raw_256x32x128_square.bin >> square_stream_256x32x128.log
rm output_waveforms/stream_raw_256x32x128_square.bin

./stream_fpga_256x32x128 --input=input_waveforms/ne10cpx_short_triangle256.bin --output=output_waveforms/stream_fpga_256x32x128_triangle.bin > triangle_stream_256x32x128.log
rm output_waveforms/stream_fpga_256x32x128_triangle.bin
./stream_neon32_256x32x128 --input=input_waveforms/ne10cpx_long_triangle256.bin --output=output_waveforms/stream_neon32_256x32x128_triangle.bin >> triangle_stream_256x32x128.log
rm output_waveforms/stream_neon32_256x32x128_triangle.bin
./stream_raw_256x32x128 --input=input_waveforms/ne10cpx_long_triangle256.bin --output=output_waveforms/stream_raw_256x32x128_triangle.bin >> triangle_stream_256x32x128.log
rm output_waveforms/stream_raw_256x32x128_triangle.bin



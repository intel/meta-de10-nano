#!/bin/sh

[ -d "output_waveforms" ] || {
	echo ""
	echo "ERROR: directory 'output_waveforms' does not exits, please create it."
	echo ""
	exit 1	
}

echo "running run_fft_256.sh"
./run_fft_256.sh
echo "running run_fft_256x32.sh"
./run_fft_256x32.sh
echo "running run_fft_256x32x128.sh"
./run_fft_256x32x128.sh
echo "running run_fft_4096.sh"
./run_fft_4096.sh
echo "running run_stream_256x32x128.sh"
./run_stream_256x32x128.sh
echo "running run_stream_256x16x1.sh"
./run_stream_256x16x1.sh
echo "running run_stream_256x1x1.sh"
./run_stream_256x1x1.sh


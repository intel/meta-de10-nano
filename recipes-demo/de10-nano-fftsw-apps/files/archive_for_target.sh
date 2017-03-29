#!/bin/sh

COPY_LIST="
README_TARGET.TXT
create_input_waveforms.sh
duplicate_x128.sh
duplicate_x32.sh
duplicate_x8.sh
run_all.sh
run_fft_256.sh
run_fft_4096.sh
run_fft_256x32.sh
run_fft_256x32x128.sh
run_stream_256x32x128.sh
run_stream_256x16x1.sh
run_stream_256x1x1.sh
create_real_short_sine32
create_real_short_square32
create_real_short_triangle32
real_short_to_ne10cpx_long
real_short_to_ne10cpx_short
ne10cpx_short_to_text
ne10cpx_long_to_text
c16_256
c32_256
fft_256
fftdma_256
neon16_256
neon32_256
c16_4096
c32_4096
fft_4096
fftdma_4096
neon16_4096
neon32_4096
c16_256x32
c32_256x32
fft_256x32
fftdma_256x32
neon16_256x32
neon32_256x32
c16_256x32x128
c32_256x32x128
fft_256x32x128
fftdma_256x32x128
neon16_256x32x128
neon32_256x32x128
stream_fpga_256x32x128
stream_fpga_256x16x1
stream_fpga_256x1x1
stream_neon32_256x32x128
stream_neon32_256x16x1
stream_neon32_256x1x1
stream_raw_256x32x128
stream_raw_256x16x1
stream_raw_256x1x1
"

for NEXT in ${COPY_LIST}
do
	[ -f "${NEXT}" ] || {
		echo "File: '${NEXT}' in copy list was not found."
		exit 1
	}
done

tar czf fft.tgz ${COPY_LIST}


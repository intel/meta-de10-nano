#!/bin/sh

COPY_LIST="
LICENSE
c16_256.c
c16_256x32.c
c16_256x32x128.c
c16_4096.c
c32_256.c
c32_256x32.c
c32_256x32x128.c
c32_4096.c
create_real_short_sine32.c
create_real_short_square32.c
create_real_short_triangle32.c
fft_256.c
fft_256x32.c
fft_256x32x128.c
fft_4096.c
fftdma_256.c
fftdma_256x32.c
fftdma_256x32x128.c
fftdma_4096.c
ne10cpx_long_to_text.c
ne10cpx_short_to_text.c
neon16_256.c
neon16_256x32.c
neon16_256x32x128.c
neon16_4096.c
neon32_256.c
neon32_256x32.c
neon32_256x32x128.c
neon32_4096.c
overhead.c
real_short_to_ne10cpx_long.c
real_short_to_ne10cpx_short.c
stream_fpga_256x32x128.c
stream_fpga_256x16x1.c
stream_fpga_256x1x1.c
stream_neon32_256x32x128.c
stream_neon32_256x16x1.c
stream_neon32_256x1x1.c
stream_raw_256x32x128.c
stream_raw_256x16x1.c
stream_raw_256x1x1.c
archive_for_target.sh
README_TARGET_BUILD.TXT
README_TARGET.TXT
clean_all.sh
create_input_waveforms.sh
duplicate_x128.sh
duplicate_x32.sh
duplicate_x8.sh
run_all.sh
run_fft_256.sh
run_fft_256x32.sh
run_fft_256x32x128.sh
run_fft_4096.sh
run_stream_256x32x128.sh
run_stream_256x16x1.sh
run_stream_256x1x1.sh
setup_target_fft_env.sh
target_build_all.sh
target_build_app.sh
target_build_lib.sh
images/create_gnuplot_sine_256.gnu
images/create_gnuplot_sine_256x32.gnu
images/create_gnuplot_sine_4096.gnu
images/create_gnuplot_squares_256.gnu
images/create_gnuplot_squares_256x32.gnu
images/create_gnuplot_squares_4096.gnu
images/create_gnuplot_triangle_256.gnu
images/create_gnuplot_triangle_256x32.gnu
images/create_gnuplot_triangle_4096.gnu
images/create_montage_sine_256.sh
images/create_montage_sine_256x32.sh
images/create_montage_sine_4096.sh
images/create_montage_squares_256.sh
images/create_montage_squares_256x32.sh
images/create_montage_squares_4096.sh
images/create_montage_triangle_256.sh
images/create_montage_triangle_256x32.sh
images/create_montage_triangle_4096.sh
images/README_IMAGES.TXT
"

for NEXT in ${COPY_LIST}
do
	[ -f "${NEXT}" ] || {
		echo "File: '${NEXT}' in copy list was not found."
		exit 1
	}
done

tar czf fft_src.tgz ${COPY_LIST}


To build the applications in this directory, you should be able to run these
target scripts in this order:

./target_build_all.sh
	this will build all the applications
./archive_for_target.sh
	this will archive all applications and target scripts into fft.tgz for
	installation on the target
	
At this point everything should be built.  If you'd like to install and run
everything you can run this script:

./setup_target_fft_env.sh
	this target script will ensure that the fft_driver is installed, ensure
	that /mnt/ram has a 100MB tmpfs ram disk mounted, and then extract the
	fft.tgz archive into /mnt/ram/fft and run the create_input_waveforms.sh
	script and then runs the run_all.sh script

NOTE: see the README_TARGET.TXT file for additional information

Description of directory contents:

target_build_all.sh
	target script to build all applications and libraries

target_build_app.sh
	target script to build one application

target_build_lib.sh
	target script to build one library

clean_all.sh
	target script to clean all build objects

archive_for_target.sh
	target script to archive all applications and scripts for target
	installation

create_input_waveforms.sh
	target script that executes waveform creation applications and
	duplication scripts to create all input waveforms for the applications
	to consume

duplicate_x128.sh
duplicate_x32.sh
duplicate_x8.sh
	these target scripts duplicate a file x8, x32 or x128 times.  These are
	used to replicate the fundamental waveforms into longer patterns

images/
	this directory contains host scripts to create PNG images of waveform
	data text files using gnu plot and montage from imagemagick

run_all.sh
run_fft_256.sh
run_fft_256x32.sh
run_fft_256x32x128.sh
run_fft_4096.sh
run_stream_256x32x128.sh
run_stream_256x16x1.sh
run_stream_256x1x1.sh
	these target scripts execute all of the various permutations of the demo
	applications mentioned below
	
setup_target_fft_env.sh
	this target script is used to extract the fft.tgz archive onto the ram
	disk mounted at /mnt/ram

overhead.c
	this is the source for the overhead library that all of the applications
	are linked against

c16_256.c
c16_256x32.c
c16_256x32x128.c
c16_4096.c
	these applications use the NE10 C functions to implement the FFT
	algorithm for a variety of input sample sizes with 16-bit resolution
	
c32_256.c
c32_256x32.c
c32_256x32x128.c
c32_4096.c
	these applications use the NE10 C functions to implement the FFT
	algorithm for a variety of input sample sizes with 32-bit resolution

fft_256.c
fft_256x32.c
fft_256x32x128.c
fft_4096.c
	these applications use the FPGA logic to implement the FFT
	algorithm for a variety of input sample sizes.  This variant pushes
	the input samples into the FPGA and pulls the results back out using
	programmed I/O from the HPS Cortex A9 processor

fftdma_256.c
fftdma_256x32.c
fftdma_256x32x128.c
fftdma_4096.c
	these applications use the FPGA logic to implement the FFT
	algorithm for a variety of input sample sizes.  This variant moves
	the input samples into the FPGA and the results back out using DMA
	through the ACP port of the Cortex A9 processor

neon16_256.c
neon16_256x32.c
neon16_256x32x128.c
neon16_4096.c
	these applications use the NE10 NEON functions to implement the FFT
	algorithm for a variety of input sample sizes with 16-bit resolution

neon32_256.c
neon32_256x32.c
neon32_256x32x128.c
neon32_4096.c
	these applications use the NE10 NEON functions to implement the FFT
	algorithm for a variety of input sample sizes with 32-bit resolution

stream_fpga_256x32x128.c
stream_fpga_256x16x1.c
stream_fpga_256x1x1.c
stream_neon32_256x32x128.c
stream_neon32_256x16x1.c
stream_neon32_256x1x1.c
stream_raw_256x32x128.c
stream_raw_256x16x1.c
stream_raw_256x1x1.c
	these applications stream data from the FPGA into ramdisk files, the
	fpga variant DMAs the input stream thru the FPGA FFT logic before it
	is DMAed through the ACP port of the Cortex A9, the neon variant DMAs
	the raw input data stream through the ACP port and then computes the FFT
	algorithm on it before saving to the ram disk, the raw variant DMAs the
	raw input data stream through the ACP port and then directly into the
	ram disk

create_real_short_sine32.c
create_real_short_square32.c
create_real_short_triangle32.c
	these applications create the initial 32 samples of their respective
	waveform, sine, square and triangle
	
real_short_to_ne10cpx_long.c
real_short_to_ne10cpx_short.c
	these applications translate the real short data format into ne10cpx
	long and short format which is consumed by the demo applications
	
ne10cpx_long_to_text.c
ne10cpx_short_to_text.c
	these applications translate the ne10cpx long and short format binary
	data into text format output which may be read by gnu plot for example


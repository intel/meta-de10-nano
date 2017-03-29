On the DE10-Nano target board, you should be able to run the following script to
install and execute the FFT example applications:

./setup_target_fft_env.sh
	this target script will ensure that the fft_driver is installed, ensure
	that /mnt/ram has a 100MB tmpfs ram disk mounted, and then extract the
	fft.tgz archive into /mnt/ram/fft and run the create_input_waveforms.sh
	script and then runs the run_all.sh script

Description of directory contents:

create_input_waveforms.sh
	target script that executes waveform creation applications and
	duplication scripts to create all input waveforms for the applications
	to consume

duplicate_x128.sh
duplicate_x32.sh
duplicate_x8.sh
	these target scripts duplicate a file x8, x32 or x128 times.  These are
	used to replicate the fundamental waveforms into longer patterns

run_all.sh
run_fft_256.sh
run_fft_256x32.sh
run_fft_256x32x128.sh
run_fft_4096.sh
run_stream_256x32x128.sh
	these target scripts execute all of the various permutations of the demo
	applications mentioned below
	
setup_target_fft_env.sh
	this target script is used to extract the fft.tgz archive onto the ram
	disk mounted at /mnt/ram

c16_256
c16_256x32
c16_256x32x128
c16_4096
	these applications use the NE10 C functions to implement the FFT
	algorithm for a variety of input sample sizes with 16-bit resolution
	
c32_256
c32_256x32
c32_256x32x128
c32_4096
	these applications use the NE10 C functions to implement the FFT
	algorithm for a variety of input sample sizes with 32-bit resolution

fft_256
fft_256x32
fft_256x32x128
fft_4096
	these applications use the FPGA logic to implement the FFT
	algorithm for a variety of input sample sizes.  This variant pushes
	the input samples into the FPGA and pulls the results back out using
	programmed I/O from the HPS Cortex A9 processor

fftdma_256
fftdma_256x32
fftdma_256x32x128
fftdma_4096
	these applications use the FPGA logic to implement the FFT
	algorithm for a variety of input sample sizes.  This variant moves
	the input samples into the FPGA and the results back out using DMA
	through the ACP port of the Cortex A9 processor

neon16_256
neon16_256x32
neon16_256x32x128
neon16_4096
	these applications use the NE10 NEON functions to implement the FFT
	algorithm for a variety of input sample sizes with 16-bit resolution

neon32_256
neon32_256x32
neon32_256x32x128
neon32_4096
	these applications use the NE10 NEON functions to implement the FFT
	algorithm for a variety of input sample sizes with 32-bit resolution

stream_fpga_256x32x128
stream_fpga_256x16x1
stream_fpga_256x1x1
stream_neon32_256x32x128
stream_neon32_256x16x1
stream_neon32_256x1x1
stream_raw_256x32x128
stream_raw_256x16x1
stream_raw_256x1x1
	these applications stream data from the FPGA into ramdisk files, the
	fpga variant DMAs the input stream thru the FPGA FFT logic before it
	is DMAed through the ACP port of the Cortex A9, the neon variant DMAs
	the raw input data stream through the ACP port and then computes the FFT
	algorithm on it before saving to the ram disk, the raw variant DMAs the
	raw input data stream through the ACP port and then directly into the
	ram disk

create_real_short_sine32
create_real_short_square32
create_real_short_triangle32
	these applications create the initial 32 samples of their respective
	waveform, sine, square and triangle
	
real_short_to_ne10cpx_long
real_short_to_ne10cpx_short
	these applications translate the real short data format into ne10cpx
	long and short format which is consumed by the demo applications
	
ne10cpx_long_to_text
ne10cpx_short_to_text
	these applications translate the ne10cpx long and short format binary
	data into text format output which may be read by gnu plot for example


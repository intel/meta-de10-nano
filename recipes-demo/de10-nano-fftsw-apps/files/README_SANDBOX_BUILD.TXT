To build the applications in this directory, you should be able to run these
target scripts in this order:

./sandbox_build_all.sh
	this will build all the applications
	
At this point everything should be built.

Description of directory contents:

sandbox_build_all.sh
	target script to build all applications and libraries

target_build_app.sh
	target script to build one application

target_build_lib.sh
	target script to build one library

clean_all.sh
	target script to clean all build objects

create_input_waveforms.sh
	target script that executes waveform creation applications and
	duplication scripts to create all input waveforms for the applications
	to consume

duplicate_x8.sh
	this target script duplicates a file x8 times.  This is	used to
	replicate the fundamental waveforms into longer patterns

overhead.c
	this is the source for the overhead library that all of the applications
	are linked against

fftdma_256.c
	this application uses the FPGA logic to implement the FFT
	algorithm for a variety of input sample sizes.  This variant moves
	the input samples into the FPGA and the results back out using DMA
	through the ACP port of the Cortex A9 processor

neon32_256.c
	this application uses the NE10 NEON functions to implement the FFT
	algorithm for a variety of input sample sizes with 32-bit resolution

create_real_short_sine32.c
create_real_short_square32.c
create_real_short_triangle32.c
	these applications create the initial 32 samples of their respective
	waveform, sine, square and triangle
	
real_short_to_ne10cpx_long.c
	this application translates the real short data format into ne10cpx
	long and short format which is consumed by the demo applications
	
ne10cpx_long_to_text.c
	this application translates the ne10cpx long and short format binary
	data into text format output which may be read by gnu plot for example


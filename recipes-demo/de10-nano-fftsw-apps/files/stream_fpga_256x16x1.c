#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <error.h>
#include <errno.h>

#include "NE10.h"

/* customize these macros and variables for a particular implementation */
#define FFT_IN_ROUNDS	(1)
#define FFT_CALC_POINTS	(256)
#define FFT_CALC_ROUNDS	(16)
#define FFT_POINTS	(FFT_CALC_POINTS * FFT_CALC_ROUNDS)
#define IN_SAMPLE_TYPE	ne10_fft_cpx_int16_t
#define OUT_SAMPLE_TYPE	ne10_fft_cpx_int32_t

const char *g_test = "stream_fpga_256x16x1";
const char *g_description = "256x16x1 point FFT, FPGA, 16-bit input 24/32-bit output";

/* these functions and variables are implemented in overhead.c as a library */
extern void initialize_everything(int argc, char **argv);
extern uint64_t get_gt_value(void);
extern void release_everything(void);
extern void print_results(void);

extern char *g_input_filename;
extern char *g_output_filename;
extern uint64_t g_start_time;
extern uint64_t g_end_time;

int main(int argc, char **argv)
{
	int input_fd;
	int output_fd;
	int fft256stream_dev_fd;
	int result;
	IN_SAMPLE_TYPE *in_buf;
	OUT_SAMPLE_TYPE *out_buf;
	int j;
	
	initialize_everything(argc, argv);
	
	/* allocate storage for input, output and config buffers */
	in_buf = (IN_SAMPLE_TYPE*) NE10_MALLOC (FFT_CALC_POINTS * sizeof(IN_SAMPLE_TYPE));
	if(in_buf == NULL)
		error(1, errno, "in_buf allocation");
		
	out_buf = (OUT_SAMPLE_TYPE*) NE10_MALLOC (FFT_POINTS * sizeof(OUT_SAMPLE_TYPE));
	if(out_buf == NULL)
		error(1, errno, "out_buf allocation");
		
	/* open the input and output files and fft dev */
	input_fd = open(g_input_filename, O_RDONLY);
	if(input_fd < 0)
		error(1, errno, "opening input file '%s'", g_input_filename);
	
	output_fd = open(g_output_filename, O_WRONLY | O_CREAT, ~((mode_t)umask));
	if(output_fd < 0)
		error(1, errno, "opening output file '%s'", g_output_filename);

	fft256stream_dev_fd = open("/dev/fft256stream", O_RDWR);
	if(fft256stream_dev_fd < 0)
		error(1, errno, "opening fft256stream_dev_fd");

	/* read the input data */
	result = read(input_fd, in_buf, FFT_CALC_POINTS * sizeof(IN_SAMPLE_TYPE));
	if(result < 0)
		error(1, errno, "read input file");
	if(result != (FFT_CALC_POINTS * sizeof(IN_SAMPLE_TYPE)))
		error(1, 0, "input data size, expected %d but got %d", FFT_CALC_POINTS * sizeof(IN_SAMPLE_TYPE), result);
	
	/* write the waveform buffer */
	result = write(fft256stream_dev_fd, in_buf, FFT_CALC_POINTS * sizeof(IN_SAMPLE_TYPE));
	if(result < 0)
		error(1, errno, "write waveform buffer");
	if(result != (FFT_CALC_POINTS * sizeof(IN_SAMPLE_TYPE)))
		error(1, 0, "output data size, expected %d but got %d", FFT_CALC_POINTS * sizeof(IN_SAMPLE_TYPE), result);
	
	/* capture the start value of the GT */
	g_start_time = get_gt_value();
	
	for(j = 0 ; j < FFT_IN_ROUNDS ; j++) {
		/* fetch the preprocessed stream from the FPGA, 16 bit input 24/32 bit output */
		result = read(fft256stream_dev_fd, out_buf, FFT_POINTS * sizeof(OUT_SAMPLE_TYPE));
		if(result < 0)
			error(1, errno, "read from fft256stream_dev_fd");
		if (result != (int)(FFT_POINTS * sizeof(OUT_SAMPLE_TYPE)))
			error(1, 0, "fft256stream_dev_fd output data size, expected %d but got %d", FFT_POINTS * sizeof(OUT_SAMPLE_TYPE), result);
	
		/* write the output data */
		result = write(output_fd, out_buf, FFT_POINTS * sizeof(OUT_SAMPLE_TYPE));
		if(result < 0)
			error(1, errno, "write output file");
		if(result != (FFT_POINTS * sizeof(OUT_SAMPLE_TYPE)))
			error(1, 0, "output data size, expected %d but got %d", FFT_POINTS * sizeof(OUT_SAMPLE_TYPE), result);
	}
	
	/* capture the end value of the GT */
	g_end_time = get_gt_value();

	/* close the input and output files and fft dev */
	close(fft256stream_dev_fd);
	close(output_fd);
	close(input_fd);

	/* free storage for input, output and config buffers */
	NE10_FREE (out_buf);
	NE10_FREE (in_buf);
	
	print_results();
	release_everything();
	return 0;
}


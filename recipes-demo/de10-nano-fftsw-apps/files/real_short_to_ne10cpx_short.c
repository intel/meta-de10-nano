#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <error.h>
#include <errno.h>

#include "NE10.h"

#define BUFFER_SIZE	(512)
static short input_buffer[BUFFER_SIZE];
static ne10_fft_cpx_int16_t output_buffer[BUFFER_SIZE];

int main(void) {

	int i;
	int read_result;
	int write_result;
	int read_object_count;

	read_result = read(STDIN_FILENO, input_buffer, sizeof(input_buffer));	
	if (read_result < 0) {
		error(1, errno, "reading from stdin");
	}
	
	read_object_count = read_result / (int)sizeof(short);
	if (read_result != (read_object_count * (int)sizeof(short))) {
		error(1, 0, "did not read a multiple of input buffer type");
	}
	
	while (read_object_count > 0) {
		for (i = 0 ; i < read_object_count ; i++) {
			output_buffer[i].r = input_buffer[i];
			output_buffer[i].i = 0;
		}

		write_result = write(STDOUT_FILENO, output_buffer, (read_object_count * sizeof(ne10_fft_cpx_int16_t)));
		if (write_result < 0) {
			error(1, errno, "writing to stdout");
		}
	
		if (write_result != (read_object_count * (int)sizeof(ne10_fft_cpx_int16_t))) {
			error(1, 0, "failed to write %d bytes to stdout, only wrote %d bytes", (read_object_count * sizeof(ne10_fft_cpx_int16_t)), write_result);
		}

		read_result = read(STDIN_FILENO, input_buffer, sizeof(input_buffer));	
		if (read_result < 0) {
			error(1, errno, "reading from stdin");
		}

		read_object_count = read_result / (int)sizeof(short);
		if (read_result != (read_object_count * (int)sizeof(short))) {
			error(1, 0, "did not read a multiple of input buffer type");
		}
	}
	
	return 0;
}


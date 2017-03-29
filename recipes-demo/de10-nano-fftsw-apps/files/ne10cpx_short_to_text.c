#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <error.h>
#include <errno.h>

#include "NE10.h"

#define BUFFER_SIZE	(512)
static ne10_fft_cpx_int16_t input_buffer[BUFFER_SIZE];

int main(void) {

	int i;
	int read_result;
	int read_object_count;
	
	read_result = read(STDIN_FILENO, input_buffer, BUFFER_SIZE * sizeof(ne10_fft_cpx_int16_t));
	if (read_result < 0) {
		error(1, errno, "reading from stdin");
	}
	
	read_object_count = read_result / (int)sizeof(ne10_fft_cpx_int16_t);
	if (read_result != (read_object_count * (int)sizeof(ne10_fft_cpx_int16_t))) {
		error(1, 0, "did not read a multiple of input buffer type");
	}
	
	printf("%s\t%s\n", "real", "imag");
	
	while (read_object_count > 0) {
		for (i = 0 ; i < read_object_count ; i++) {
			printf("%d\t%d\n", input_buffer[i].r, input_buffer[i].i);
		}

		read_result = read(STDIN_FILENO, input_buffer, BUFFER_SIZE * sizeof(ne10_fft_cpx_int16_t));
		if (read_result < 0) {
			error(1, errno, "reading from stdin");
		}

		read_object_count = read_result / (int)sizeof(ne10_fft_cpx_int16_t);
		if (read_result != (read_object_count * (int)sizeof(ne10_fft_cpx_int16_t))) {
			error(1, 0, "did not read a multiple of input buffer type");
		}
	}
	
	return 0;
}


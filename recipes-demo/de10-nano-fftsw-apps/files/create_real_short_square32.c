#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <error.h>
#include <errno.h>

#define SAMPLES_PER_PERIOD	(32)
static short waveform_array[SAMPLES_PER_PERIOD];

int main(void) {

	int i;
	int result;
	short wave_temp;
	
	wave_temp = SHRT_MAX;
	for(i = 0 ; i < (SAMPLES_PER_PERIOD) ; i++) {
		if (i == (SAMPLES_PER_PERIOD / 2))
			wave_temp = SHRT_MIN;

		waveform_array[i] = wave_temp;
	}

	result = write(STDOUT_FILENO, &waveform_array, sizeof(waveform_array));
	if (result < 0) {
		error(1, errno, "writing to stdout");
	}
	
	if (result != sizeof(waveform_array)) {
		error(1, 0, "failed to write %d bytes to stdout, only wrote %d bytes", sizeof(waveform_array), result);
	}
	
	return 0;
}


#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <error.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>

int main(void) {

	int i;
	const char *leds_array[] = {
		"fpga_led0",
		"fpga_led1",
		"fpga_led2",
		"fpga_led3",
		"fpga_led4",
		"fpga_led5",
		"fpga_led6",
		"fpga_led7"
	};
	int leds_array_count = (sizeof leds_array) / (sizeof *leds_array);
	int led_fd;
	char path[PATH_MAX];
	int path_length;
	int result;
	char brightness;

	// turn off all leds
	for (i = 0 ; i < leds_array_count ; i++ ) {
		// set trigger to none
		path_length = snprintf(path, PATH_MAX,
				"/sys/class/leds/%s/trigger",
				leds_array[i]);
		if(path_length < 0)
			error(1, 0, "path output error");
		if(path_length >= PATH_MAX)
			error(1, 0, "path length overflow");

		led_fd = open(path, O_WRONLY | O_SYNC);
		if(led_fd < 0)
			error(1, errno, "could not open file '%s'", path);

		result = write(led_fd, "none", 4);
		if(result < 0)
			error(1, errno, "writing 'none' to '%s'", path);
		if(result != 4)
			error(1, 0, "did not write 4 bytes to '%s'", path);

		result = close(led_fd);
		if(result < 0)
			error(1, errno, "could not close file '%s'", path);

		// set brightness to 0
		path_length = snprintf(path, PATH_MAX,
				"/sys/class/leds/%s/brightness",
				leds_array[i]);
		if(path_length < 0)
			error(1, 0, "path output error");
		if(path_length >= PATH_MAX)
			error(1, 0, "path length overflow");

		led_fd = open(path, O_WRONLY | O_SYNC);
		if(led_fd < 0)
			error(1, errno, "could not open file '%s'", path);

		result = write(led_fd, "0", 1);
		if(result < 0)
			error(1, errno, "writing 'none' to '%s'", path);
		if(result != 1)
	
		led_fd = open(path, O_WRONLY | O_SYNC);
		if(led_fd < 0)
			error(1, errno, "could not open file '%s'", path);

		result = write(led_fd, "0", 1);
		if(result < 0)
			error(1, errno, "writing '0' to '%s'", path);
		if(result != 1)
			error(1, 0, "did not write 1 byte to '%s'", path);

		result = close(led_fd);
		if(result < 0)
			error(1, errno, "could not close file '%s'", path);
	}

	// toggle the leds individually
	for (i = 0 ; i < (leds_array_count * 2) ; i++ ) {
		// toggle the brightness
		path_length = snprintf(path, PATH_MAX,
				"/sys/class/leds/%s/brightness",
				leds_array[i % leds_array_count]);
		if(path_length < 0)
			error(1, 0, "path output error");
		if(path_length >= PATH_MAX)
			error(1, 0, "path length overflow");

		led_fd = open(path, O_RDWR | O_SYNC);
		if(led_fd < 0)
			error(1, errno, "could not open file '%s'", path);

		result = read(led_fd, &brightness, 1);
		if(result < 0)
			error(1, errno, "reading 1 byte from '%s'", path);
		if(result != 1)
			error(1, 0, "did not read 1 byte from '%s'", path);

		if(brightness == '0')
			brightness = '1';
		else if(brightness == '1')
			brightness = '0';
		else
			error(1, 0, "unexpected value for brightness");

		result = write(led_fd, &brightness, 1);
		if(result < 0)
			error(1, errno, "writing brightness to '%s'", path);
		if(result != 1)
			error(1, 0, "did not write 1 byte to '%s'", path);

		result = close(led_fd);
		if(result < 0)
			error(1, errno, "could not close file '%s'", path);

		result = usleep(125000);
		if(result < 0)
			error(1, errno, "usleep error");
	}

	return 0;
}


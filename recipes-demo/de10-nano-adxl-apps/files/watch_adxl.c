#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <error.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include "linux/input.h"

#define INPUT_DEV_NODE "/dev/input/by-path/platform-ffc04000.i2c-event"
#define SYSFS_DEVICE_DIR "/sys/devices/platform/soc/ffc04000.i2c/i2c-0/0-0053/"

#define EV_CODE_X (0)
#define EV_CODE_Y (1)
#define EV_CODE_Z (2)

#define LOOP_COUNT (100)

void write_sysfs_cntl_file(const char *dir_name, const char *file_name,
		const char *write_str) {

	char path[PATH_MAX];
	int path_length;
	int file_fd;
	int result;

	// create the path to the file we need to open
	path_length = snprintf(path, PATH_MAX, "%s/%s",	dir_name, file_name);
	if(path_length < 0)
		error(1, 0, "path output error");
	if(path_length >= PATH_MAX)
		error(1, 0, "path length overflow");

	// open the file
	file_fd = open(path, O_WRONLY | O_SYNC);
	if(file_fd < 0)
		error(1, errno, "could not open file '%s'", path);
	
	// write the string to the file
	result = write(file_fd, write_str, strlen(write_str));
	if(result < 0)
		error(1, errno, "writing to '%s'", path);
	if((size_t)(result) != strlen(write_str))
		error(1, errno, "buffer underflow writing '%s'", path);

	// close the file
	result = close(file_fd);
	if(result < 0)
		error(1, errno, "could not close file '%s'", path);
}

int main(void) {
	int event_dev_fd;
	const char *input_dev_node = INPUT_DEV_NODE;
	int result;
	int i;
	int loop;
	struct input_event the_event;
	struct input_absinfo the_absinfo;
	int abs_value_array[3] = {0};

	int last_value_x;
	int last_value_y;
	int last_value_z;
	int event_type_array[LOOP_COUNT];
	int event_code_array[LOOP_COUNT];
	int event_value_array[LOOP_COUNT];
	int x_abs_value_array[LOOP_COUNT];
	int y_abs_value_array[LOOP_COUNT];
	int z_abs_value_array[LOOP_COUNT];

	// enable adxl
	write_sysfs_cntl_file(SYSFS_DEVICE_DIR, "disable", "0");

	// set the sample rate to maximum
	write_sysfs_cntl_file(SYSFS_DEVICE_DIR, "rate", "15");

	// do not auto sleep
	write_sysfs_cntl_file(SYSFS_DEVICE_DIR, "autosleep", "0");
	
	// open the event device node
	event_dev_fd = open(input_dev_node, O_RDONLY | O_SYNC);
	if(event_dev_fd < 0)
		error(1, errno, "could not open file '%s'", input_dev_node);
	
	// read the current state of each axis
	printf("\n");
	for(i = 0 ; i < 3 ; i++ ) {
		result = ioctl (event_dev_fd, EVIOCGABS(i), &the_absinfo);
		if(result < 0)
			error(1, errno, "ioctl from '%s'", input_dev_node);
		
		switch(i) {
		case(EV_CODE_X) :
			printf("X-axis\n");
			break;
		case(EV_CODE_Y) :
			printf("Y-axis\n");
			break;
		case(EV_CODE_Z) :
			printf("Z-axis\n");
			break;
		default :
			printf("unknown-axis\n");
			break;
		}

		printf("%14s = %d\n", "value", the_absinfo.value);
		printf("%14s = %d\n", "minimum", the_absinfo.minimum);
		printf("%14s = %d\n", "maximum", the_absinfo.maximum);
		printf("%14s = %d\n", "fuzz", the_absinfo.fuzz);
		printf("%14s = %d\n", "flat", the_absinfo.flat);
		printf("%14s = %d\n", "resolution", the_absinfo.resolution);
		printf("\n");
	}
	
	fflush(stdout);

	// read the next LOOP_COUNT events
	for(loop = 0 ; loop < LOOP_COUNT ; loop++) {
		// read the next event
		result = read(event_dev_fd, &the_event, 
				sizeof(struct input_event));
		if(result < 0)
			error(1, errno, "reading %d from '%s'", 
					sizeof(struct input_event),
					input_dev_node);
		if(result != sizeof(struct input_event))
			error(1, 0, "did not read %d bytes from '%s'", 
					sizeof(struct input_event),
					input_dev_node);
		
		event_type_array[loop] = the_event.type;
		event_code_array[loop] = the_event.code;
		event_value_array[loop] = the_event.value;

		// read the current state of each axis
		for(i = 0 ; i < 3 ; i++ ) {
			result = ioctl (event_dev_fd, EVIOCGABS(i), 
					&the_absinfo);
			if(result < 0)
				error(1, errno, "ioctl from '%s'",
						input_dev_node);
				
			abs_value_array[i] = the_absinfo.value;
		}
		
		x_abs_value_array[loop] = abs_value_array[0];
		y_abs_value_array[loop] = abs_value_array[1];
		z_abs_value_array[loop] = abs_value_array[2];
	}

	// print the accumulated event data
	printf("Event capture results...\n");
	for(loop = 0 ; loop < LOOP_COUNT ; loop++) {

		// if the event is EV_ABS, then process it, otherwise ignore it
		if(event_type_array[loop] == EV_SYN) {

			printf("EV_SYN event\n");
			printf("%d,%d,%d\n",
					x_abs_value_array[loop],
					y_abs_value_array[loop],
					z_abs_value_array[loop]);
			
		} else if(event_type_array[loop] == EV_ABS) {

			// look for the codes that we expect
			switch(event_code_array[loop]) {
			case(EV_CODE_X) :
				printf("X-axis EV_ABS event = ");
				break;
			case(EV_CODE_Y) :
				printf("Y-axis EV_ABS event = ");
				break;
			case(EV_CODE_Z) :
				printf("Z-axis EV_ABS event = ");
				break;
			default :
				printf("unknown");
				break;
			}

			// output the value of the event
			printf("'%d'\n", event_value_array[loop]);

			printf("%d,%d,%d\n",
					x_abs_value_array[loop],
					y_abs_value_array[loop],
					z_abs_value_array[loop]);
		} else {
			printf("Unexpected event type '%d'\n", 
					event_type_array[loop]);
		}
	}

	fflush(stdout);
	
	// read current value for each axis so we can monitor changes by polling

	// x-axis
	result = ioctl (event_dev_fd, EVIOCGABS(0), 
			&the_absinfo);
	if(result < 0)
		error(1, errno, "ioctl from '%s'",
				input_dev_node);
	last_value_x = the_absinfo.value;

	// y-axis
	result = ioctl (event_dev_fd, EVIOCGABS(1), 
			&the_absinfo);
	if(result < 0)
		error(1, errno, "ioctl from '%s'",
				input_dev_node);
	last_value_y = the_absinfo.value;

	// z-axis
	result = ioctl (event_dev_fd, EVIOCGABS(2), 
			&the_absinfo);
	if(result < 0)
		error(1, errno, "ioctl from '%s'",
				input_dev_node);
	last_value_z = the_absinfo.value;

	// capture the next LOOP_COUNT transitions that occur on any axis
	for(loop = 0 ; loop < LOOP_COUNT ;) {

		// read the current state of each axis
		for(i = 0 ; i < 3 ; i++ ) {
			result = ioctl (event_dev_fd, EVIOCGABS(i), 
					&the_absinfo);
			if(result < 0)
				error(1, errno, "ioctl from '%s'",
						input_dev_node);
				
			abs_value_array[i] = the_absinfo.value;
		}
		
		// if any axis has changed, log the new values, otherwise skip
		if(		(abs_value_array[0] != last_value_x) ||
				(abs_value_array[1] != last_value_y) ||
				(abs_value_array[2] != last_value_z) ) {
			
			x_abs_value_array[loop] = abs_value_array[0];
			y_abs_value_array[loop] = abs_value_array[1];
			z_abs_value_array[loop] = abs_value_array[2];

			last_value_x = abs_value_array[0];
			last_value_y = abs_value_array[1];
			last_value_z = abs_value_array[2];

			loop++;
		}
	}

	// print the polling results
	printf("\n");
	printf("Polling results\n");
	for(loop = 0 ; loop < LOOP_COUNT ; loop++) {

		printf("%d,%d,%d\n",
				x_abs_value_array[loop],
				y_abs_value_array[loop],
				z_abs_value_array[loop]);
	}
	
	// close the device node
	result = close(event_dev_fd);
	if(result < 0)
		error(1, errno, "could not close file '%s'", input_dev_node);

	// disable adxl
	write_sysfs_cntl_file(SYSFS_DEVICE_DIR, "disable", "1");
}


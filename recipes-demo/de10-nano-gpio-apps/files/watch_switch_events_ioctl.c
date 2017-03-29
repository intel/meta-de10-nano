#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <error.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include "linux/input.h"

int main(void) {
	int event_dev_fd;
	const char *path = "/dev/input/by-path/platform-soc:base-fpga-region:keys-event";
	struct input_event the_event;
	int result;
	unsigned char keys[9];
	
	// open the event device node
	event_dev_fd = open(path, O_RDONLY | O_SYNC);
	if(event_dev_fd < 0)
		error(1, errno, "could not open file '%s'", path);
	
	while(1) {
		// read the next event
		result = read(event_dev_fd, &the_event, 
				sizeof(struct input_event));
		if(result < 0)
			error(1, errno, "reading %d from '%s'", 
					sizeof(struct input_event), path);
		if(result != sizeof(struct input_event))
			error(1, 0, "did not read %d bytes from '%s'", 
					sizeof(struct input_event), path);
		
		// if the event is EV_KEY, then process it, otherwise ignore it
		if(the_event.type == EV_KEY) {

			// look for the codes that we expect
			switch(the_event.code) {
			case(KEY_F6) :
				printf("SW0");
				break;
			case(KEY_F7) :
				printf("SW1");
				break;
			case(KEY_F8) :
				printf("SW2");
				break;
			case(KEY_F9) :
				printf("SW3");
				break;
			default :
				printf("unknown");
				break;
			}
			
			// output the value of the event
			printf(" moved to value '%d'\n", the_event.value);

			// read the current state of the keys
			result = ioctl (event_dev_fd, EVIOCGKEY(sizeof keys),
					&keys);
			if(result < 0)
				error(1, errno, "ioctl from '%s'", path);
			
			printf("SW0(%d) SW1(%d) SW2(%d) SW3(%d)\n",
					(keys[8] & 0x01) ? (1) : (0),
					(keys[8] & 0x02) ? (1) : (0),
					(keys[8] & 0x04) ? (1) : (0),
					(keys[8] & 0x08) ? (1) : (0)
				);
		}
	}
}


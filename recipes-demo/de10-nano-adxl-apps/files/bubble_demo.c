#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <error.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "linux/input.h"
#include <dirent.h>
#include <poll.h>
#include <wait.h>

#define INPUT_DEV_NODE "/dev/input/by-path/platform-ffc04000.i2c-event"
#define SYSFS_DEVICE_DIR "/sys/devices/platform/soc/ffc04000.i2c/i2c-0/0-0053/"

#define EV_CODE_X (0)
#define EV_CODE_Y (1)
#define EV_CODE_Z (2)

int g_stop_running = 0;
int g_termination_count = 0;

void write_sysfs_cntl_file(const char *dir_name, const char *file_name,
		const char *write_str);
		
void poll_for_KEY0_press(void);

void my_sig_child_handler(int signal_number) {
	(void)signal_number;
	g_stop_running = 1;
}

void my_termination_handler(int signal_number) {

	int result;
	pid_t child_pid;
	int status;
	
	if(g_termination_count > 0)
		return;

	g_termination_count++;
			
	// enable heartbeat LED
	write_sysfs_cntl_file("/sys/class/leds/hps_led0", "trigger", 
			"heartbeat");
	
	// disable adxl
	write_sysfs_cntl_file(SYSFS_DEVICE_DIR, "disable", "1");

	result = kill(-(getpgrp()), SIGTERM);
	if(result < 0) {
		error(0, errno, "SIGTERM pgrp");
		result = kill(-(getpgrp()), SIGKILL);
		if(result < 0)
			error(0, errno, "SIGKILL pgrp");
	}

	do {
		child_pid = wait(&status);
		if((child_pid == -1) && (errno != ECHILD))
			error(1, errno, "wait");
	} while(child_pid != -1);
	
	// print termination signal that we received
	if(signal_number == SIGTERM)
		printf("Bubble level demo exiting due to SIGTERM signal...\n");
	else if(signal_number == SIGINT)
		printf("Bubble level demo exiting due to SIGINT signal...\n");
	else if(signal_number == SIGQUIT)
		printf("Bubble level demo exiting due to SIGQUIT signal...\n");
	else 
		printf("Bubble level demo exiting due to unknown signal...\n");

	exit(EXIT_SUCCESS);
}

int main(void) {
	int i;
	int result;
	int event_dev_fd;
	const char *input_dev_node = INPUT_DEV_NODE;
	struct input_absinfo the_absinfo;
	int led_fd[8];
	int led_col;
	int avg_array[10] = { 0 };
	int avg_array_index = 0;
	int avg_value;
	pid_t key0_child_pid;
	pid_t main_child_pid;
	struct sigaction new_action, old_action;

	const char *led_trigger[8] = {
		"/sys/class/leds/fpga_led0/trigger",
		"/sys/class/leds/fpga_led1/trigger",
		"/sys/class/leds/fpga_led2/trigger",
		"/sys/class/leds/fpga_led3/trigger",
		"/sys/class/leds/fpga_led4/trigger",
		"/sys/class/leds/fpga_led5/trigger",
		"/sys/class/leds/fpga_led6/trigger",
		"/sys/class/leds/fpga_led7/trigger" };
	
	const char *led_trigger_str = "none";
	int led_trigger_str_len = 4;

	const char *led_brightness[8] = {
		"/sys/class/leds/fpga_led0/brightness",
		"/sys/class/leds/fpga_led1/brightness",
		"/sys/class/leds/fpga_led2/brightness",
		"/sys/class/leds/fpga_led3/brightness",
		"/sys/class/leds/fpga_led4/brightness",
		"/sys/class/leds/fpga_led5/brightness",
		"/sys/class/leds/fpga_led6/brightness",
		"/sys/class/leds/fpga_led7/brightness" };
	
	const char *led_on_str = "1";
	int led_on_str_len = 1;
	const char *led_off_str = "0";
	int led_off_str_len = 1;
	
	// fork main to deal with heartbeat LED
	main_child_pid = fork();
	if(main_child_pid != 0) {
		int int_result;

		// disable heartbeat LED
		write_sysfs_cntl_file("/sys/class/leds/hps_led0", "trigger", 
				"none");
		
		// register a SIGTERM handler
		new_action.sa_handler = my_termination_handler;
		sigemptyset(&new_action.sa_mask);
		new_action.sa_flags = 0;
	
		result = sigaction(SIGTERM, NULL, &old_action);
		if(result < 0)
			error(1, errno, "sigaction");
		
		if(old_action.sa_handler != SIG_IGN) {
			result = sigaction(SIGTERM, &new_action, NULL);
			if(result < 0)
				error(1, errno, "sigaction");
		}
		else
			error(1, 0, "SIGTERM handler already installed");

		// register a SIGINT handler
		result = sigaction(SIGINT, NULL, &old_action);
		if(result < 0)
			error(1, errno, "sigaction");
		
		if(old_action.sa_handler != SIG_IGN) {
			result = sigaction(SIGINT, &new_action, NULL);
			if(result < 0)
				error(1, errno, "sigaction");
		}
		else
			error(1, 0, "SIGINT handler already installed");

		// register a SIGQUIT handler
		result = sigaction(SIGQUIT, NULL, &old_action);
		if(result < 0)
			error(1, errno, "sigaction");
		
		if(old_action.sa_handler != SIG_IGN) {
			result = sigaction(SIGQUIT, &new_action, NULL);
			if(result < 0)
				error(1, errno, "sigaction");
		}
		else
			error(1, 0, "SIGINT handler already installed");

		// wait for the main child to terminate
		waitpid(main_child_pid, &int_result, 0);

		// enable heartbeat LED
		write_sysfs_cntl_file("/sys/class/leds/hps_led0", "trigger", 
				"heartbeat");

		// disable adxl
		write_sysfs_cntl_file(SYSFS_DEVICE_DIR, "disable", "1");
		
		// return the status from the main child
		if(WIFEXITED(int_result)) {
			exit(WEXITSTATUS(int_result));
		} else {
			error(1, 0, "main child exited abnormally");
		}
	}
	

	// initialize led triggers to "none"
	for(i = 0 ; i < 8 ; i++) {
		led_fd[i] = open(led_trigger[i], O_WRONLY | O_SYNC);
		if(led_fd[i] < 0)
			error(1, errno, "File: '%s'", led_trigger[i]);

		result = write(led_fd[i], led_trigger_str, led_trigger_str_len);
		if(result < 0)
			error(1, errno, "File: '%s'", led_trigger[i]);
		if(result != led_trigger_str_len)
			error(1, 0, "File: '%s'", led_trigger[i]);

		result = close(led_fd[i]);
		if(result < 0)
			error(1, errno, "could not close file '%s'",
					led_trigger[i]);
	}

	// initialize led brightness to "off"
	for(i = 0 ; i < 8 ; i++) {
		led_fd[i] = open(led_brightness[i], O_WRONLY | O_SYNC);
		if(led_fd[i] < 0)
			error(1, errno, "File: '%s'", led_brightness[i]);

		result = write(led_fd[i], led_off_str, led_off_str_len);
		if(result < 0)
			error(1, errno, "File: '%s'", led_brightness[i]);
		if(result != led_off_str_len)
			error(1, 0, "File: '%s'", led_brightness[i]);
	}

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
	
	printf("\
\n\
1 - Notice that the heartbeat LED has been stopped while this program executes.\
\n\
2 - Notice the 8 LEDs beside the Ethernet connector on the board, they should\n\
    behave like a bubble level.  As you tip your board from side to side on\n\
    the short axis of the board, the leds will indicate the tilt as a bubble\n\
    level would.\n\
7 - Press the KEY0 push button located near the edge of the board in order to\n\
    stop the bubble level demo program.\n\
8 - When you successfully stop the program, the heartbeat LED will resume.\
\n\
\n\
");
	
	// register our SIGCHLD handler
	new_action.sa_handler = my_sig_child_handler;
	sigemptyset(&new_action.sa_mask);
	new_action.sa_flags = 0;
	
	result = sigaction(SIGCHLD, NULL, &old_action);
	if(result < 0)
		error(1, errno, "sigaction");
		
	if(old_action.sa_handler != SIG_IGN) {
		result = sigaction(SIGCHLD, &new_action, NULL);
		if(result < 0)
			error(1, errno, "sigaction");
	}
	else
		error(1, 0, "SIGCHLD handler already installed");

	// fork the KEY0 push button monitor process
	key0_child_pid = fork();
	if(key0_child_pid == 0) {
		poll_for_KEY0_press();
		exit(EXIT_SUCCESS);
	}
	
	// main bubble level loop
	while (1) {

		if(g_stop_running != 0)
		break;

		result = ioctl (event_dev_fd, EVIOCGABS(EV_CODE_X),
				&the_absinfo);
		if(result < 0)
			error(1, errno, "ioctl from '%s'",
				input_dev_node);
		
		avg_array[avg_array_index++] = the_absinfo.value;
		if(avg_array_index >= 10)
			avg_array_index = 0;

		avg_value = 0;
		for(i = 0 ; i < 10 ; i++)
			avg_value += avg_array[i];
		avg_value /= 10;

		if(avg_value < -40)
			led_col = 0x01;
		else if(avg_value < -30)
			led_col = 0x02;
		else if(avg_value < -20)
			led_col = 0x04;
		else if(avg_value < 0)
			led_col = 0x08;
		else if(avg_value > 40)
			led_col = 0x80;
		else if(avg_value > 30)
			led_col = 0x40;
		else if(avg_value > 20)
			led_col = 0x20;
		else if(avg_value >= 0)
			led_col = 0x10;

		// output each LED sequentially down the column
		for(i = 0 ; i < 8 ; i++) {
			int row_mask = 1 << i;
			if((row_mask & led_col) == 0) {
				result = write(led_fd[i], led_off_str,
						led_off_str_len);
				if(result < 0)
					error(1, errno, "File: '%s'",
						led_brightness[i]);
				if(result != led_off_str_len)
					error(1, 0, "File: '%s'",
						led_brightness[i]);
			} else {
				result = write(led_fd[i], led_on_str,
						led_on_str_len);
				if(result < 0)
					error(1, errno, "File: '%s'",
						led_brightness[i]);
				if(result != led_on_str_len)
					error(1, 0, "File: '%s'",
						led_brightness[i]);
			}
		}

		usleep(10 * 1000);
	}

	// close the led brightness files after setting them to "off"
	for(i = 0 ; i < 8 ; i++) {
		result = write(led_fd[i], led_off_str, led_off_str_len);
		if(result < 0)
			error(1, errno, "File: '%s'", led_brightness[i]);
		if(result != led_off_str_len)
			error(1, 0, "File: '%s'", led_brightness[i]);

		result = close(led_fd[i]);
		if(result < 0)
			error(1, errno, "could not close file '%s'",
					led_brightness[i]);
	}

	// close the device node
	result = close(event_dev_fd);
	if(result < 0)
		error(1, errno, "could not close file '%s'", input_dev_node);
		
	printf("Bubble level demo exiting due to KEY0 push button press...\n");
}

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

void poll_for_KEY0_press(void) {
	DIR *gpio_dir;
	const char *gpio_dir_path = "/sys/class/gpio";
	const char *gpiochip_str = "gpiochip";
	const char *gpio_label = "gpio@0x100005000";
	size_t gpiochip_str_len = strlen(gpiochip_str);
	int result;
	struct dirent *dir_entry;
	char path[PATH_MAX+1];
	int path_length;
	int file_fd;
	char buffer[PATH_MAX+1];
	char gpio_number_buffer[PATH_MAX+1];
	char *str_result = NULL;
	char *newline_ptr;
	struct pollfd pollfd_struct;
           
	// open the sysfs gpio directory
	gpio_dir = opendir(gpio_dir_path);
	if(gpio_dir == NULL)
		error(1, errno, "could not open directory '%s'", gpio_dir_path);
	
	// find the gpio controller for the KEY0 push button 'gpio@0x100005000'
	while(1) {
		// read the next directory entry
		errno = 0;
		dir_entry = readdir(gpio_dir);
		if(dir_entry == NULL) {
			if(errno != 0) {
				result = closedir(gpio_dir);
				if(result < 0)
				   error(1, errno, "could not closedir");

				error(1, errno, "reading directory '%s'",
						gpio_dir_path);
			}
			else
				break;
		}
		
		// check if this is a gpio controller entry
		result = strncmp(dir_entry->d_name, gpiochip_str, 
				gpiochip_str_len);
		if(result != 0)
			continue;
			
		// open the gpio controller label file and read label value
		path_length = snprintf(path, PATH_MAX, "%s/%s/label",
				gpio_dir_path, dir_entry->d_name);
		if(path_length < 0)
			error(1, 0, "path output error");
		if(path_length >= PATH_MAX)
			error(1, 0, "path length overflow");

		file_fd = open(path, O_RDONLY | O_SYNC);
		if(file_fd < 0)
			error(1, errno, "could not open file '%s'", path);
		
		result = read(file_fd, buffer, PATH_MAX);
		if(result < 0)
			error(1, errno, "reading from '%s'", path);
		if(result == PATH_MAX)
			error(1, errno, "buffer overflow reading '%s'", path);

		buffer[PATH_MAX] = 0;

		result = close(file_fd);
		if(result < 0)
			error(1, errno, "could not close file '%s'", path);

		// test the gpio controller label value for our gpio controller
		str_result = strstr(buffer, gpio_label);
		if(str_result != NULL)
			break;
	}

	closedir(gpio_dir);

	if(str_result == NULL)
		error(1, 0, "unable to locate gpio controller");

	// open the gpio controller base file and read base value
	path_length = snprintf(path, PATH_MAX, "%s/%s/base",
			gpio_dir_path, dir_entry->d_name);
	if(path_length < 0)
		error(1, 0, "path output error");
	if(path_length >= PATH_MAX)
		error(1, 0, "path length overflow");

	file_fd = open(path, O_RDONLY | O_SYNC);
	if(file_fd < 0)
		error(1, errno, "could not open file '%s'", path);
	
	result = read(file_fd, gpio_number_buffer, PATH_MAX);
	if(result < 0)
		error(1, errno, "reading from '%s'", path);
	if(result == PATH_MAX)
		error(1, errno, "buffer overflow reading '%s'", path);

	gpio_number_buffer[PATH_MAX] = 0;	

	result = close(file_fd);
	if(result < 0)
		error(1, errno, "could not close file '%s'", path);

	// remove the newline at the end of the string
	newline_ptr = strchr(gpio_number_buffer,'\n');
	if(newline_ptr != NULL)
		memset(newline_ptr, '\0', 1);
	
	// open the gpio export file and write our gpio number
	path_length = snprintf(path, PATH_MAX, "%s/export",
			gpio_dir_path);
	if(path_length < 0)
		error(1, 0, "path output error");
	if(path_length >= PATH_MAX)
		error(1, 0, "path length overflow");

	file_fd = open(path, O_WRONLY | O_SYNC);
	if(file_fd < 0)
		error(1, errno, "could not open file '%s'", path);
	
	result = write(file_fd, gpio_number_buffer, strlen(gpio_number_buffer));
	// NOTE: we don't bother checking for errors here because if this gpio
	// has already been exported this write will receive a device busy error
	// which is perfectly normal.
	
	result = close(file_fd);
	if(result < 0)
		error(1, errno, "could not close file '%s'", path);

	// open the gpio edge file and write 'falling' to it
	path_length = snprintf(path, PATH_MAX, "%s/gpio%s/edge",
			gpio_dir_path, gpio_number_buffer);
	if(path_length < 0)
		error(1, 0, "path output error");
	if(path_length >= PATH_MAX)
		error(1, 0, "path length overflow");

	file_fd = open(path, O_WRONLY | O_SYNC);
	if(file_fd < 0)
		error(1, errno, "could not open file '%s'", path);
	
	result = write(file_fd, "falling", 7);
	if(result < 0)
		error(1, errno, "writing to '%s'", path);
	if(result != 7)
		error(1, errno, "buffer underflow writing '%s'", path);

	result = close(file_fd);
	if(result < 0)
		error(1, errno, "could not close file '%s'", path);
			
	// open the gpio value file and poll the file
	path_length = snprintf(path, PATH_MAX, "%s/gpio%s/value",
			gpio_dir_path, gpio_number_buffer);
	if(path_length < 0)
		error(1, 0, "path output error");
	if(path_length >= PATH_MAX)
		error(1, 0, "path length overflow");

	file_fd = open(path, O_RDONLY | O_SYNC);
	if(file_fd < 0)
		error(1, errno, "could not open file '%s'", path);
	
	// we first need to read the file before we can poll it, otherwise poll
	// will not block
	result = read(file_fd, buffer, PATH_MAX);
	if(result < 0)
		error(1, errno, "reading from '%s'", path);
	if(result == PATH_MAX)
		error(1, errno, "buffer overflow reading '%s'", path);

	buffer[PATH_MAX] = 0;

	pollfd_struct.fd = file_fd;
	pollfd_struct.events = POLLPRI | POLLERR;
	pollfd_struct.revents = 0;
	
	result = poll(&pollfd_struct, 1, -1);
	if(result < 0)
		error(1, errno, "poll returned error");
	
	result = close(file_fd);
	if(result < 0)
		error(1, errno, "could not close file '%s'", path);
			
	// open the gpio unexport file and write our gpio number
	path_length = snprintf(path, PATH_MAX, "%s/unexport",
			gpio_dir_path);
	if(path_length < 0)
		error(1, 0, "path output error");
	if(path_length >= PATH_MAX)
		error(1, 0, "path length overflow");

	file_fd = open(path, O_WRONLY | O_SYNC);
	if(file_fd < 0)
		error(1, errno, "could not open file '%s'", path);
	
	result = write(file_fd, gpio_number_buffer, strlen(gpio_number_buffer));
	if(result < 0)
		error(1, errno, "writing to '%s'", path);
	if((size_t)(result) != strlen(gpio_number_buffer))
		error(1, errno, "buffer underflow writing '%s'", path);

	result = close(file_fd);
	if(result < 0)
		error(1, errno, "could not close file '%s'", path);
}


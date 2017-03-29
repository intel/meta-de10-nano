#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <error.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <poll.h>

int main(void) {
	DIR *gpio_dir;
	const char *gpio_dir_path = "/sys/class/gpio";
	const char *gpiochip_str = "gpiochip";
	const char *gpio_label = "gpio@0x100005000";
	size_t gpiochip_str_len = strlen(gpiochip_str);
	int result;
	struct dirent *dir_entry;
	char path[PATH_MAX];
	int path_length;
	int file_fd;
	char buffer[PATH_MAX+1] = {0};
	char gpio_number_buffer[PATH_MAX+1] = {0};
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
			if(errno != 0)
				error(1, errno, "reading directory '%s'",
						gpio_dir_path);
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

		result = close(file_fd);
		if(result < 0)
			error(1, errno, "could not close file '%s'", path);

		buffer[PATH_MAX] = 0;
		// test the gpio controller label value for our gpio controller
		str_result = strstr(buffer, gpio_label);
		if(str_result != NULL)
			break;
	}

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

	result = close(file_fd);
	if(result < 0)
		error(1, errno, "could not close file '%s'", path);

	gpio_number_buffer[PATH_MAX] = 0;
	
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

	printf("Please press the KEY0 push button.\n");

	buffer[PATH_MAX] = 0;

	pollfd_struct.fd = file_fd;
	pollfd_struct.events = POLLPRI | POLLERR;
	pollfd_struct.revents = 0;
	
	result = poll(&pollfd_struct, 1, -1);
	if(result < 0)
		error(1, errno, "poll returned error");
	
	if(result == 1)
		printf("KEY0 push button pressed.\n");
	else
		printf("Poll returned value other than 1.\n");

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


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

/*
 * PERIPHBASE and Global Timer offset
 */
#define PERIPHBASE		(0xFFFEC000)
#define GLOBAL_TIMER_OFFSET	(0x200)

extern const char *g_test;
extern const char *g_description;

int g_mpuclk_rate;
int g_mpu_periph_clk_rate;
int g_fft_sub_clk_0_clk_rate;
char *g_input_filename = NULL;
char *g_output_filename = NULL;
int g_devmem_fd;
void *g_periphbase_map;
uint32_t g_gt_samples_ave;
volatile uint32_t *g_global_timer_ptr;
uint64_t g_start_time;
uint64_t g_end_time;

void print_usage(char **argv)
{
		error(0, 0, " ");
		error(0, 0, "USAGE: %s --input=<input file> --output=<output file>", argv[0]);
		error(1, 0, " ");
}

uint64_t get_gt_value(void)
{
	uint32_t gt_lo;
	uint32_t gt_hi_0;
	uint32_t gt_hi_1;
	uint64_t gt_value;

	do {
		gt_hi_0 = g_global_timer_ptr[1];
		gt_lo = g_global_timer_ptr[0];
		gt_hi_1 = g_global_timer_ptr[1];

	} while(gt_hi_0 != gt_hi_1);
	
	gt_value = ((uint64_t)gt_hi_0 << 32) | ((uint64_t)gt_lo & (uint64_t)0xFFFFFFFF);
	
	return gt_value;
}

void initialize_everything(int argc, char **argv)
{
	int i;
	int fd;
	int result;
	long periph_size;
	char in_str[16];
	const char *input_option_str = "--input=";
	const char *output_option_str = "--output=";
	int input_option_str_len = strlen(input_option_str);
	int output_option_str_len = strlen(output_option_str);
	uint64_t gt_samples[10];
	uint64_t gt_samples_sum;
	ne10_result_t status;

	/* validate input arguments */
	if (argc != 3)
		print_usage(argv);
		
	if(strncmp(input_option_str, argv[1], input_option_str_len) == 0) {
		g_input_filename = argv[1] + input_option_str_len;

		if(strncmp(output_option_str, argv[2], output_option_str_len) == 0) {
			g_output_filename = argv[2] + output_option_str_len;
		} else {
			error(0, 0, " ");
			error(0, 0, "ERROR: no --output option found on command line");
			print_usage(argv);
		}
	} else {
		if(strncmp(input_option_str, argv[2], input_option_str_len) == 0) {
			g_input_filename = argv[2] + input_option_str_len;

			if(strncmp(output_option_str, argv[1], output_option_str_len) == 0) {
				g_output_filename = argv[1] + output_option_str_len;
			} else {
				error(0, 0, " ");
				error(0, 0, "ERROR: no --output option found on command line");
				print_usage(argv);
			}
		} else {
			error(0, 0, " ");
			error(0, 0, "ERROR: no --input option found on command line");
			print_usage(argv);
		}
	}
	
	/* initialize the NE10 NEON library */
	status = ne10_init();
	if (status != NE10_OK)
		error(1, 0, "NE10 init failed.");
		
	status = ne10_HasNEON();
	if (status != NE10_OK)
		error(1, 0, "NEON is not available.");

	/* gather system info */
	fd = open("/sys/kernel/debug/clk/mpuclk/clk_rate", O_RDONLY);
	if(fd < 0)
		error(1, errno, "opening sysfs mpuclk/clk_rate");
		
	result = read(fd, in_str, 16);
	if (result < 0)
		error(1, errno, "reading sysfs mpuclk/clk_rate");
		
	close(fd);
	g_mpuclk_rate = atoi(in_str);
	
	fd = open("/sys/kernel/debug/clk/mpu_periph_clk/clk_rate", O_RDONLY);
	if(fd < 0)
		error(1, errno, "opening sysfs mpu_periph_clk/clk_rate");
		
	result = read(fd, in_str, 16);
	if (result < 0)
		error(1, errno, "reading sysfs mpu_periph_clk/clk_rate");
		
	close(fd);
	g_mpu_periph_clk_rate = atoi(in_str);
	
	fd = open("/sys/firmware/devicetree/base/soc/base-fpga-region/fft_sub_clk_0/clock-frequency", O_RDONLY);
	if(fd < 0)
		error(1, errno, "opening sysfs fft_sub_clk_0/clock-frequency");
		
	result = read(fd, in_str, 16);
	if (result < 0)
		error(1, errno, "reading sysfs fft_sub_clk_0/clock-frequency");
		
	close(fd);
	g_fft_sub_clk_0_clk_rate = atoi(in_str);
	
	/* open the /dev/mem device */
	g_devmem_fd = open("/dev/mem", O_RDWR | O_SYNC);
	if(g_devmem_fd < 0)
		error(1, errno, "open /dev/mem");

	/* mmap periphbase */
	periph_size = sysconf(_SC_PAGE_SIZE);
	if(periph_size >= 0) {
		g_periphbase_map = mmap(NULL, periph_size, PROT_READ|PROT_WRITE, MAP_SHARED, g_devmem_fd, PERIPHBASE);
		if(g_periphbase_map == MAP_FAILED)
			error(1, errno, "mmap /dev/mem");
	}
	else 
		error(1, errno, "mmap /dev/mem");

	/* calculate global timer pointer */
	g_global_timer_ptr = (uint32_t*)((uint32_t)g_periphbase_map + GLOBAL_TIMER_OFFSET);

	/* calibrate get_gt_value */
	gt_samples[0] = get_gt_value();
	gt_samples[1] = get_gt_value();
	gt_samples[2] = get_gt_value();
	gt_samples[3] = get_gt_value();
	gt_samples[4] = get_gt_value();
	gt_samples[5] = get_gt_value();
	gt_samples[6] = get_gt_value();
	gt_samples[7] = get_gt_value();
	gt_samples[8] = get_gt_value();
	gt_samples[9] = get_gt_value();
	
	gt_samples_sum = 0;
	for(i = 0 ; i < 9 ; i++) {
		gt_samples_sum += gt_samples[i + 1] - gt_samples[i];
	}
	g_gt_samples_ave = gt_samples_sum / 9;
}

void release_everything(void)
{
	int result = 0;
	long periph_size;

	periph_size = sysconf(_SC_PAGE_SIZE);
	
	/* munmap and close /dev/mem */
	if(periph_size >= 0)
		result = munmap(g_periphbase_map, periph_size);
	if((result < 0) | (periph_size < 0))
		error(1, errno, "munmap /dev/mem");

	close(g_devmem_fd);
}

void print_results(void)
{
	uint32_t tick_count = (g_end_time - g_start_time) - g_gt_samples_ave;
	float sec_time = ((float)1 / (float)g_mpu_periph_clk_rate) * (float)tick_count;
	float us_time = sec_time * 1000000;
	
	printf("                  Test: %s\n", g_test);
	printf("           Description: %s\n", g_description);
	printf("            Input file: '%s'\n", g_input_filename);
	printf("           Output file: '%s'\n", g_output_filename);
	printf("         CPU Frequency: %d hertz\n", g_mpuclk_rate);
	printf("          GT Frequency: %d hertz\n", g_mpu_periph_clk_rate);
	printf("    FPGA FFT Frequency: %d hertz\n", g_fft_sub_clk_0_clk_rate);
	printf("Computation (GT ticks): %d\n", tick_count);
	printf("      Computation (us): %f\n", us_time);
}


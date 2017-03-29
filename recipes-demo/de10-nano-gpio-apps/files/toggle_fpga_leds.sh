#!/bin/sh

LEDS="
fpga_led0
fpga_led1
fpga_led2
fpga_led3
fpga_led4
fpga_led5
fpga_led6
fpga_led7
"

# turn off all leds
for NEXT_LED in ${LEDS}
do
	echo "none" > /sys/class/leds/${NEXT_LED}/trigger
	echo "0" > /sys/class/leds/${NEXT_LED}/brightness
done

# toggle the leds individually
for NEXT_LED in ${LEDS} ${LEDS}
do
	cat /sys/class/leds/${NEXT_LED}/brightness | \
		tr '01' '10' > /sys/class/leds/${NEXT_LED}/brightness
	usleep 125000
done


#!/bin/sh

# locate the gpio controller for the KEY0 push button 'gpio@0x100005000'
GPIO_DIRNAME="unknown"
for NEXT in $(find /sys/class/gpio -name "gpiochip*" | sort)
do
	cat "${NEXT}/label" | grep "gpio@0x100005000" > /dev/null && {
		GPIO_DIRNAME="${NEXT}"
		break
	}
done

[ "unknown" == "${GPIO_DIRNAME}" ] && {
	echo "unable to locate gpio controller"
	exit 1
} || {
	# the KEY0 gpio is the first gpio in this controller
	GPIO_NUMBER="$(cat "${GPIO_DIRNAME}/base")"
	echo "${GPIO_NUMBER}" > /sys/class/gpio/export
	GPIO_VALUE="$(cat /sys/class/gpio/gpio${GPIO_NUMBER}/value)"
	echo "KEY0 push button is at state '${GPIO_VALUE}'"
	echo "${GPIO_NUMBER}" > /sys/class/gpio/unexport
}


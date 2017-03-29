#!/bin/sh

gcc \
	-march=armv7-a \
	-mfloat-abi=hard \
	-mfpu=vfp3 \
	-mthumb-interwork \
	-mthumb \
	-O2 \
	-g \
	-feliminate-unused-debug-types \
	-std=gnu99 \
	-W \
	-Wall \
	-Werror \
	-Wc++-compat \
	-Wwrite-strings \
	-Wstrict-prototypes \
	-pedantic \
	-o watch_switch_events_ioctl \
	watch_switch_events_ioctl.c


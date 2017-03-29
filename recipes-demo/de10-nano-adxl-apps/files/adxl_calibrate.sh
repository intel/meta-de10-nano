#!/bin/sh

SYSFS_DEVICE_DIR="/sys/devices/platform/soc/ffc04000.i2c/i2c-0/0-0053/"
DEVFS_INPUT_EVENT_FILE="/dev/input/by-path/platform-ffc04000.i2c-event"

# enabled adxl
echo 0 > ${SYSFS_DEVICE_DIR}/disable

# set the sample rate to maximum
echo 15 > ${SYSFS_DEVICE_DIR}/rate

# do not auto sleep
echo 0 > ${SYSFS_DEVICE_DIR}/autosleep

# read some samples
hexdump -n 160 ${DEVFS_INPUT_EVENT_FILE} > /dev/null

# store calibration
echo 1 > ${SYSFS_DEVICE_DIR}/calibrate


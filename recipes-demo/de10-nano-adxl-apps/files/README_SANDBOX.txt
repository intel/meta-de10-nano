This readme describes how the Analog Devices ADXL345 Input 3-Axis Digital
Accelerometer Linux Driver is deployed on the DE10 Nano target environment. You
may find the following references useful for more information on this topic as
well.

<linux-source-tree>/drivers/input/misc/adxl34x.c
<linux-source-tree>/drivers/input/misc/adxl34x.h
<linux-source-tree>/drivers/input/misc/adxl34x-i2c.c
<linux-source-tree>/drivers/input/misc/adxl34x-spi.c
<linux-source-tree>/include/linux/input/adxl34x.h

http://wiki.analog.com/resources/tools-software/linux-drivers/input-misc/adxl345

If you cut and paste the following function into a console running on the DE10
Nano target you can extract the useful information contained in the run time
device tree maintained by the kernel in the procfs.
################################################################################
# find adxl34x in device tree
################################################################################
function find_adxl_dt () {
for NEXT in $(find -L /proc/device-tree -name "compatible" | sort)
do
cat ${NEXT} | grep "adi,adxl345" > /dev/null && {
ADXL_DIRNAME="$(dirname ${NEXT})"
echo ${ADXL_DIRNAME}
echo -e "\tcompatible = '$(cat ${ADXL_DIRNAME}/compatible)'"
echo -e "\t      name = '$(cat ${ADXL_DIRNAME}/name)'"
REG_HEX="$(hexdump -v -e '"0x"' -e '4/1 "%02x"' "${ADXL_DIRNAME}/reg")"
echo -e "\t       reg = '${REG_HEX}'"
}
done
}
################################################################################

When we run the function above on the DE10 Nano target it searches for nodes
containing the 'compatible' string containing 'adi,adxl345' which is the
identifier of the ADI accelerometer in our system.  The function then prints the
path to the node that it found and extracts the compatible string and 'name' and
'reg' properties and prints those statistics out as well.

root@de10-nano:~# find_adxl_dt
/proc/device-tree/soc/i2c@ffc04000/adxl345@0
        compatible = 'adi,adxl345'
              name = 'adxl345'
               reg = '0x00000053'
root@de10-nano:~#

We can tell from the above output that this accelerometer is attached to the
I2C controller at 0xFFC04000, and the I2C address of the accelerometer is 0x53.

There are a number of ways that we can now find the sysfs resources that refer
to this hardware:

root@de10-nano:~# ls /sys/bus/i2c
devices            drivers_autoprobe  uevent
drivers            drivers_probe
root@de10-nano:~# ls /sys/bus/i2c/devices/
0-0053  i2c-0
root@de10-nano:~# ls /sys/bus/i2c/drivers
adxl34x     dummy       ltc2978     pmbus
at24        lcd-comm    max1619     rtc-ds1307
root@de10-nano:~# ls /sys/bus/i2c/drivers/adxl34x/
0-0053  bind    uevent  unbind
root@de10-nano:~#

In the '/sys/bus/i2c/' directory there is a 'devices' and 'drivers' directory.
The 'devices' directory contains a link called '0-0053' which represents I2C bus
0 device address 0x53.  The 'drivers' directory contains a 'adxl34x' directory
which in turn contains a link called '0-0053' as well.  Both of these links
point into the actual device entry:

root@de10-nano:~# ls -l /sys/bus/i2c/devices/0-0053
lrwxrwxrwx    1 root     root             0 Jan  1  1970 /sys/bus/i2c/devices/0-0053 -> ../../../devices/platform/soc/ffc04000.i2c/i2c-0/0-0053
root@de10-nano:~# ls -l /sys/bus/i2c/drivers/adxl34x/0-0053
lrwxrwxrwx    1 root     root             0 Jul 11 15:21 /sys/bus/i2c/drivers/adxl34x/0-0053 -> ../../../../devices/platform/soc/ffc04000.i2c/i2c-0/0-0053
root@de10-nano:~#

So if we look at the contents of the actual device directory we see the expected
sysfs files provided by the adxl34x driver:

root@de10-nano:~# ls /sys/devices/platform/soc/ffc04000.i2c/i2c-0/0-0053
autosleep  disable    input      name       power      subsystem
calibrate  driver     modalias   position   rate       uevent
root@de10-nano:~#

Some of these sysfs files are provided by the adxl34x driver to provide access
from userspace into the configuration and settings of the driver.  These files
are 'autosleep', 'disable', 'calibrate', 'position', and 'rate'.  Please see
the adxl34x driver sources and documentation on how these files are used.

The 'input' sysfs file provided in the device directory will tell us which input
event device the adxl345 has been register as.  If we follow this path we see:

root@de10-nano:~# ls /sys/devices/platform/soc/ffc04000.i2c/i2c-0/0-0053/input/
input0

And following into 'input0' we see:

root@de10-nano:~# ls /sys/devices/platform/soc/ffc04000.i2c/i2c-0/0-0053/input/input0/
capabilities  id            name          properties    uniq
device        modalias      phys          subsystem
event0        mouse0        power         uevent
root@de10-nano:~# cat /sys/devices/platform/soc/ffc04000.i2c/i2c-0/0-0053/input/input0/name
ADXL34x accelerometer

Now if we go into the 'event0' we can see the 'dev' entry that contains the
device node details:

root@de10-nano:~# ls /sys/devices/platform/soc/ffc04000.i2c/i2c-0/0-0053/input/input0/event0/
dev        device     power      subsystem  uevent
root@de10-nano:~# hexdump -Cv /sys/devices/platform/soc/ffc04000.i2c/i2c-0/0-0053/input/input0/event0/dev
00000000  31 33 3a 36 34 0a                                 |13:64.|
00000006

And if we then list '/dev/input/' looking for device major 13 and  minor 64, we
can see that indeed '/dev/input/event0' is our accelerometer input device.

root@de10-nano:~# ls /dev/input/ -l
total 0
drwxr-xr-x    2 root     root            80 Jul  9 16:25 by-path
crw-rw----    1 root     input      13,  64 Jan  1  1970 event0
crw-rw----    1 root     input      13,  65 Jul  9 16:25 event1
crw-rw----    1 root     input      13,  63 Jul  9 16:25 mice
crw-rw----    1 root     input      13,  32 Jan  1  1970 mouse0
root@de10-nano:~#

There is actually a very convenient way to discover the input device by using
the 'by-path' directory like this:

root@de10-nano:~# ls /dev/input/by-path/
platform-ffc04000.i2c-event  platform-soc:keys-event
root@de10-nano:~# ls -l /dev/input/by-path/platform-ffc04000.i2c-event
lrwxrwxrwx    1 root     root             9 Jan  1  1970 /dev/input/by-path/platform-ffc04000.i2c-event -> ../event0
root@de10-nano:~#

--------------------------------------------------------------------------------
Example programs and scripts
--------------------------------------------------------------------------------
This directory contains a few examples to demonstrate how to interact with the
accelerometer on the DE10 Nano board.  There is a shell script called
'adxl_calibrate.sh' which shows how to calibrate the accelerometer.  There is a
C program called 'watch_adxl.c' that shows how to interact with the sysfs files
and the input event node provided by the adxl34x driver.  And there is a C
program called 'tap_detect.c' which demonstrates how to detect the tap events
that the accelerometer produces.

To build the 'watch_adxl.c' application simply run the 'build_watch_adxl.sh'
shell script.  That will compile the 'watch_adxl.c' source file and produce the
executable 'watch_adxl' application.  Refer to the 'build_watch_adxl.sh' script
to see how the application is actually compiled and refer to the C program
source file for more details on how it actually works.

To build the 'tap_detect.c' application simply run the 'build_tap_detect.sh'
shell script.  That will compile the 'tap_detect.c' source file and produce the
executable 'tap_detect' application.  Refer to the 'build_tap_detect.sh' script
to see how the application is actually compiled and refer to the C program
source file for more details on how it actually works.

Refer to the 'adxl_calibrate.sh' source file for more details on how it
actually works.

Once you've built the applications, you can run both the script and the
applications like this:

./adxl_calibrate.sh		<<< to run the script
./watch_adxl			<<< to run the program
./tap_detect			<<< to run the program


This readme describes the linux kernel gpio-leds framework as it deploys on the
DE10-Nano target environment.  You may find the following references useful for
more information on this topic as well.

<linux-source-tree>/Documentation/leds/leds-class.txt
<linux-source-tree>/Documentation/devicetree/bindings/leds/common.txt
<linux-source-tree>/Documentation/devicetree/bindings/leds/leds-gpio.txt

If you cut and paste the following function into a console running on the
DE10-Nano target you can extract the useful information contained in the run
time devicetree maintained by the kernel in the procfs.

################################################################################
# find gpio-leds in device tree
################################################################################
function find_gpio_leds_dt ()
{
    for NEXT in $(find -L /proc/device-tree -name "compatible" | sort);
    do
        cat ${NEXT} | grep -xz "gpio-leds" > /dev/null && {
            LEDS_DIRNAME="$(dirname ${NEXT})";
            LEDS_COMPATIBLE="$(cat ${LEDS_DIRNAME}/compatible)";
            echo "${LEDS_DIRNAME}";
            echo -e "\tcompatible = '${LEDS_COMPATIBLE}'";
            for NEXT_LED in $(find -L "${LEDS_DIRNAME}" -name "gpios" | sort);
            do
                NEXT_LED_DIR="$(dirname ${NEXT_LED})";
                echo "${NEXT_LED_DIR}";
                LEDS_GPIOS="$(hexdump -v -e '"0x" 4/1 "%02x" " "' "${NEXT_LED}")";
                CONTROLLER_PHANDLE_HEX=$(echo ${LEDS_GPIOS} | cut -d ' ' -f 1);
                GPIO_BIT_HEX=$(echo ${LEDS_GPIOS} | cut -d ' ' -f 2);
                INVERTED_FLAG_HEX=$(echo ${LEDS_GPIOS} | cut -d ' ' -f 3);
                printf "             gpios = ('%d', '%d', '%d') : ('%s', '%s', '%s')\n" "${CONTROLLER_PHANDLE_HEX}" "${GPIO_BIT_HEX}" "${INVERTED_FLAG_HEX}" "controller" "bit" "flag";
                GPIO_CONTROLLER="unknown";
                CONTROLLER_PHANDLE_DEC="$(printf "%d" "${CONTROLLER_PHANDLE_HEX}")";
                for NEXT in $(find -L /proc/device-tree -name "phandle" | sort);
                do
                    PHANDLE_HEX="$(hexdump -v -e '"0x" 4/1 "%02x"' "${NEXT}")";
                    PHANDLE_DEC="$(printf "%d" "${PHANDLE_HEX}")";
                    [ "${PHANDLE_DEC}" -eq "${CONTROLLER_PHANDLE_DEC}" ] && {
                        GPIO_CONTROLLER="$(dirname ${NEXT})"
                    };
                done;
                printf "        controller = '%s'\n" "${GPIO_CONTROLLER}";
            done
        };
    done
}
################################################################################

The function above is provided in the file 'find_gpio_leds_dt.src', which you
can source into your environment by running 'source find_gpio_leds_dt.src'.

When we run the function above on the DE10-Nano target it searches for nodes
containing the 'compatible' string 'gpio-leds', there should be only one node
located.  The function then prints the path to the node that it found and 
extracts the 'gpios' binding for each led node and prints these statistics.

root@DE10-Nano:~# find_gpio_leds_dt
/proc/device-tree/leds
        compatible = 'gpio-leds'
/proc/device-tree/leds/hps0
             gpios = ('51', '24', '0') : ('controller', 'bit', 'flag')
        controller = '/proc/device-tree/soc/gpio@ff709000/gpio-controller@0'
/proc/device-tree/soc/leds
        compatible = 'gpio-leds'
/proc/device-tree/soc/leds/fpga0
             gpios = ('49', '0', '0') : ('controller', 'bit', 'flag')
        controller = '/proc/device-tree/soc/bridge@0xc0000000/gpio@0x100003000'
/proc/device-tree/soc/leds/fpga1
             gpios = ('49', '1', '0') : ('controller', 'bit', 'flag')
        controller = '/proc/device-tree/soc/bridge@0xc0000000/gpio@0x100003000'
/proc/device-tree/soc/leds/fpga2
             gpios = ('49', '2', '0') : ('controller', 'bit', 'flag')
        controller = '/proc/device-tree/soc/bridge@0xc0000000/gpio@0x100003000'
/proc/device-tree/soc/leds/fpga3
             gpios = ('49', '3', '0') : ('controller', 'bit', 'flag')
        controller = '/proc/device-tree/soc/bridge@0xc0000000/gpio@0x100003000'
/proc/device-tree/soc/leds/fpga4
             gpios = ('49', '4', '0') : ('controller', 'bit', 'flag')
        controller = '/proc/device-tree/soc/bridge@0xc0000000/gpio@0x100003000'
/proc/device-tree/soc/leds/fpga5
             gpios = ('49', '5', '0') : ('controller', 'bit', 'flag')
        controller = '/proc/device-tree/soc/bridge@0xc0000000/gpio@0x100003000'
/proc/device-tree/soc/leds/fpga6
             gpios = ('49', '6', '0') : ('controller', 'bit', 'flag')
        controller = '/proc/device-tree/soc/bridge@0xc0000000/gpio@0x100003000'
/proc/device-tree/soc/leds/fpga7
             gpios = ('49', '7', '0') : ('controller', 'bit', 'flag')
        controller = '/proc/device-tree/soc/bridge@0xc0000000/gpio@0x100003000'

For more information on the gpio controllers framework, please read the
README_gpio.txt document.  The 'gpio@0x100003000' controller identified above
maps to the 'led_pio' controller that provides an 8-bit output, fpga based gpio,
registered as 'gpio-leds' in the device tree to be used in the gpio-leds
framework to drive LED0-LED7 on the DE10-Nano board.  The 'gpio@ff709000'
controller identified above maps to the 'gpio1' controller that provides a 
29-bit, hps based gpio controller, the HPS GPIO53 port served by this controller
is registered as 'gpio-leds' in the device tree to be used in the gpio-leds
framework to drive the USERLED on the Altas board.

The gpio-led framework will register sysfs entries for each led port, and
provide files that we can use to control and query the state of the leds.  If we
look at the sysfs led class directory like this:

root@DE10-Nano:~# ls /sys/class/leds/
fpga_led0  fpga_led2  fpga_led4  fpga_led6  hps_led0
fpga_led1  fpga_led3  fpga_led5  fpga_led7

We see all the led entries that the gpio-leds framework has registered for us.
Each of these directories contain the following format:

root@DE10-Nano:~# ls /sys/class/leds/fpga_led0
brightness      max_brightness  subsystem       uevent
device          power           trigger
root@DE10-Nano:~# ls /sys/class/leds/hps_led0
brightness      max_brightness  subsystem       uevent
device          power           trigger

There are two files in these entries that are particularly useful to us, the
'trigger' file and the 'brightness' file.  The 'trigger' file identifies what
automatic triggers get applied to the led port, by default the 'hps_led0' port
is assigned to be triggered as 'heartbeat' and if we examine the 'trigger' file
we should see that:

root@DE10-Nano:~# cat /sys/class/leds/hps_led0/trigger
none nand-disk mmc0 timer oneshot [heartbeat] backlight gpio cpu0 cpu1 default-on

If we look at any of the 'fpga_led*' entiries we should see no trigger applied
to them;

root@DE10-Nano:~# cat /sys/class/leds/fpga_led0/trigger
[none] nand-disk mmc0 timer oneshot heartbeat backlight gpio cpu0 cpu1 default-on

The 'brightness' file allows us to turn the led on or off and query the state of
the led.  Like this

root@DE10-Nano:~# cat /sys/class/leds/fpga_led0/brightness
0

The 'fpga_led0' port is currently off, so if we set this port to anything but
zero, we will turn that led on, like this:

root@DE10-Nano:~# echo 1 > /sys/class/leds/fpga_led0/brightness
root@DE10-Nano:~# cat /sys/class/leds/fpga_led0/brightness
1

Now the 'fpga_led0' port is on, if we sest this port back to zero the we will
turn that led off, like this:

root@DE10-Nano:~# echo 0 > /sys/class/leds/fpga_led0/brightness
root@DE10-Nano:~# cat /sys/class/leds/fpga_led0/brightness
0

--------------------------------------------------------------------------------
Example programs and scripts
--------------------------------------------------------------------------------
This directory contains a few examples to demonstrate how to control the LEDs on
the DE10-Nano board.  There is a shell script called 'toggle_fpga_leds.sh' and a C
program called 'toggle_fpga_leds.c'.  Each of these examples toggle the FPGA
LEDs in exactly the same way.

To build the 'toggle_fpga_leds' application simply run the
'build_toggle_fpga_leds.sh' shell script.  That will compile the
'toggle_fpga_leds.c' source file and produce the executable 'toggle_fpga_leds'
application.  Refer to the 'build_toggle_fpga_leds.sh' script to see how the
application is actually compiled and refer to the C program source file and the
shell script source file for more details on how they actually work.

Once you've built the application, you can run both the script and the
application like this:

./toggle_fpga_leds.sh	<<< to run the script
./toggle_fpga_leds	<<< to run the program

The program and script will exit automatically after they sequentially turn each
led on and then off.

Both the program and the script manipulate the LEDs by interacting with the
sysfs file entries provided by the linux gpio-led framework which leverages the
linux gpio controller framework.


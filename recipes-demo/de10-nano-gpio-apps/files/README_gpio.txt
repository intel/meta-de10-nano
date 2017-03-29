This readme describes the linux kernel gpio framework as it deploys on the
DE10-Nano target environment.  You may find the following references useful for
more information on this topic as well.

<linux-source-tree>/Documentation/gpio/gpio.txt
<linux-source-tree>/Documentation/gpio/sysfs.txt
<linux-source-tree>/Documentation/devicetree/bindings/gpio/gpio.txt
<linux-source-tree>/Documentation/devicetree/bindings/gpio/snps-dwapb-gpio.txt
<linux-source-tree>/Documentation/devicetree/bindings/gpio/gpio-altera.txt

If you cut and paste the following function into a console running on the
DE10-Nano target you can extract the useful information contained in the run
time devicetree maintained by the kernel in the procfs.

################################################################################
# find gpio controllers in device tree
################################################################################
function find_gpio_controllers_dt ()
{
    for NEXT in $(find -L /proc/device-tree -name "compatible" | sort);
    do
        cat ${NEXT} | grep -xz "snps,dw-apb-gpio" > /dev/null && {
            GPIO_DIRNAME="$(dirname ${NEXT})";
            echo ${GPIO_DIRNAME};
            GPIO_COMPATIBLE="$(cat ${GPIO_DIRNAME}/compatible)";
            echo -e "\tcompatible = '${GPIO_COMPATIBLE}'";
            WF="${GPIO_DIRNAME}/gpio-controller@0/snps,nr-gpios";
            GPIO_HEX_WIDTH="$(hexdump -v -e '"0x"' -e '4/1 "%02x"' "${WF}")";
            GPIO_WIDTH=$(printf "%d" ${GPIO_HEX_WIDTH});
            echo -e "\t     width = '${GPIO_WIDTH}'"
        };
        cat ${NEXT} | grep -e "altr,pio" > /dev/null && {
            GPIO_DIRNAME="$(dirname ${NEXT})";
            echo ${GPIO_DIRNAME};
            GPIO_COMPATIBLE="$(cat ${GPIO_DIRNAME}/compatible)";
            echo -e "\tcompatible = '${GPIO_COMPATIBLE}'";
            WF="${GPIO_DIRNAME}/altr,gpio-bank-width";
            GPIO_HEX_WIDTH="$(hexdump -v -e '"0x"' -e '4/1 "%02x"' "${WF}")";
            GPIO_WIDTH=$(printf "%d" ${GPIO_HEX_WIDTH});
            echo -e "\t     width = '${GPIO_WIDTH}'"
        };
    done
}
################################################################################

The function above is provided in the file 'find_gpio_controllers_dt.src', which
you can source into your environment by running
'source find_gpio_controllers_dt.src'.

When we run the function above on the DE10-Nano target it searches for nodes
containing the 'compatible' string fragments of 'snps,dw-apb-gpio' or 'altr,pio'
which are the identifiers for the hard GPIO controllers and the soft GPIO
controllers, respectively, in our system.  The function then prints the path to
the node that it found and extracts the compatible string and width of the gpio
controller and prints those statistics out as well.  It does this for all gpio
controller nodes that it locates in the device tree.

root@DE10-Nano:~# find_gpio_controllers_dt
/proc/device-tree/soc/bridge@0xc0000000/gpio@0x100003000
        compatible = 'altr,pio-15.0altr,pio-1.0'
             width = '8'
/proc/device-tree/soc/bridge@0xc0000000/gpio@0x100004000
        compatible = 'altr,pio-15.0altr,pio-1.0'
             width = '4'
/proc/device-tree/soc/bridge@0xc0000000/gpio@0x100005000
        compatible = 'altr,pio-15.0altr,pio-1.0'
             width = '2'
/proc/device-tree/soc/gpio@ff708000
        compatible = 'snps,dw-apb-gpio'
             width = '29'
/proc/device-tree/soc/gpio@ff709000
        compatible = 'snps,dw-apb-gpio'
             width = '29'
/proc/device-tree/soc/gpio@ff70a000
        compatible = 'snps,dw-apb-gpio'
             width = '27'

Now the run time binary device tree blob does not contain some of the useful
labels that were used in the original DTS source files that can help us
understand what gpio controllers belong to what functions on the board.  These
fragments out of the original DTS source file contain the labels which show
the functions of the various gpio controllers a little better:

led_pio: gpio@0x100003000 {
};
dipsw_pio: gpio@0x100004000 {
}; 
button_pio: gpio@0x100005000 {
};
gpio@ff708000 {
	gpio0: gpio-controller@0 {
	};
};
gpio@ff709000 {
	gpio1: gpio-controller@0 {
	};
};
gpio@ff70a000 {
	gpio2: gpio-controller@0 {
	};
};

This is how these gpio controllers are implemented on the DE10-Nano target:

led_pio - 8-bit output, fpga based gpio, registered as 'gpio-leds' in the device
          tree to be used in the gpio-leds framework to drive LED0-LED7 on the
          DE10-Nano board.
          
dipsw_pio - 4-bit input, fpga based gpio, registered as 'gpio-keys' in the
            device tree to be used in the gpio-keys framework to receive input
            events from switches SW0-SW3 on the DE10-Nano board.

button_pio - 2-bit input, fpga based gpio, receives input from push buttons
             KEY0 and KEY1 on the DE10-Nano board.

gpio0 - 29-bit, hps based gpio controller, no interfaces connected to these pins
        on the Altas board.

gpio1 - 29-bit, hps based gpio controller, the HPS GPIO53 port served by this
        controller is registered as 'gpio-leds' in the device tree to be used in
        the gpio-leds framework to drive the USERLED on the Altas board.  The
        HPS GPIO54 port served by this controller receives input from push
        button KEY2 on the DE10-Nano board.

gpio2 - 27-bit, hps based gpio controller, no interfaces connected to these pins
        on the DE10-Nano board.

The kernel gpio framework creates some sysfs entries for all the gpio
controllers that we can interact with to gain access to and manipulate the gpio
ports that we are interested in. These sysfs entries are located in
'/sys/class/gpio', like this:

root@DE10-Nano:~# ls /sys/class/gpio
export       gpiochip415  gpiochip427  gpiochip483
gpiochip413  gpiochip419  gpiochip454  unexport

Each of the gpio controllers is represented by a gpiochip* entry in the display
above.  To determine which gpiochip* entry corresponds to which gpio controller
we can run a loop like this to discover them:

for NEXT in $(find /sys/class/gpio/ -name "gpiochip*" | sort)
do
echo ${NEXT} = $(cat ${NEXT}/label)
done

/sys/class/gpio/gpiochip413 = /soc/bridge@0xc0000000/gpio@0x100005000
/sys/class/gpio/gpiochip415 = /soc/bridge@0xc0000000/gpio@0x100004000
/sys/class/gpio/gpiochip419 = /soc/bridge@0xc0000000/gpio@0x100003000
/sys/class/gpio/gpiochip427 = ff70a000.gpio
/sys/class/gpio/gpiochip454 = ff709000.gpio
/sys/class/gpio/gpiochip483 = ff708000.gpio

You should be able to correlate the string from the 'label' file with the output
that we saw from the device tree entries.  We can also extract the width of each
gpio controller from the sysfs entries like this:

for NEXT in $(find /sys/class/gpio/ -name "gpiochip*" | sort)
do
echo ${NEXT} = $(cat ${NEXT}/ngpio)
done

/sys/class/gpio/gpiochip413 = 2
/sys/class/gpio/gpiochip415 = 4
/sys/class/gpio/gpiochip419 = 8
/sys/class/gpio/gpiochip427 = 27
/sys/class/gpio/gpiochip454 = 29
/sys/class/gpio/gpiochip483 = 29

These widths should correlate to what was reported out of the device tree.

Now if we want to work with a raw gpio we can use the gpio framework to do so.
There are a few raw gpios on the DE10-Nano system that we can demonstrate.  We
cannot demonstrate any interaction with the gpios that were registered with the
gpio-leds or gpio-keys framework since those frameworks are in control of those
gpio ports, but those are demonstrated in their own specific readme files.

Let's try to read the state of the 'button_pio' ports that are controlled by the
'gpio@0x100005000' controller which maps into 'gpiochip157' for the 2-bits that
are connected to the KEY0 and KEY1 push buttons.  To being we 'export' the two
gpio ports that we're interested in like this:

root@DE10-Nano:~# echo 413 > /sys/class/gpio/export
root@DE10-Nano:~# echo 414 > /sys/class/gpio/export

And now you can see that we have two new entires in the sysfs, each representing
the individual gpio port that we exported.

root@DE10-Nano:~# ls /sys/class/gpio
export       gpio414      gpiochip415  gpiochip427  gpiochip483
gpio413      gpiochip413  gpiochip419  gpiochip454  unexport

Each of these individual gpio directories contain the following files:

root@DE10-Nano:~# ls /sys/class/gpio/gpio413
active_low  direction   edge        power       subsystem   uevent      value
root@DE10-Nano:~# cat /sys/class/gpio/gpio413/active_low
0
root@DE10-Nano:~# cat /sys/class/gpio/gpio413/direction
in
root@DE10-Nano:~# cat /sys/class/gpio/gpio413/edge
none
root@DE10-Nano:~# cat /sys/class/gpio/gpio413/value
1

We can see from above that this port is defined as an active high input with no
edge detection interrupt currently enabled and the value of the port is
currently 1.  Reading the 'value' sysfs file will always return the live state
of this gpio port.  If we read the 'value' file a few times as we press and
release the KEY0 push button we can see the live state change:

root@DE10-Nano:~# cat /sys/class/gpio/gpio413/value
1
root@DE10-Nano:~# cat /sys/class/gpio/gpio413/value
0
root@DE10-Nano:~# cat /sys/class/gpio/gpio413/value
1
root@DE10-Nano:~# cat /sys/class/gpio/gpio413/value
0
root@DE10-Nano:~# cat /sys/class/gpio/gpio413/value
1

Now if we write the phrase 'falling' into the 'edge' file then the edge capture
interrupt functionality will be enabled for this gpio:

root@DE10-Nano:~# echo falling > /sys/class/gpio/gpio413/edge
root@DE10-Nano:~# cat /sys/class/gpio/gpio413/edge
falling

At this point two things occur, if we could call poll() or select() against the
file descriptor for the 'value' file, it would block until the interrupt fired,
if we simply read() the 'value' file then we will continue to see the live state
of the port.  The second thing that we can observe is that the
'/proc/interrupts' file shows our newly active IRQ for this PIO that has been
registered:

root@DE10-Nano:~# cat /proc/interrupts | grep 144
144:          1          0  altera-gpio   0  gpiolib

If we press and release the KEY0 push button a few times we should see the 
interrupt count increase.

root@DE10-Nano:~# cat /proc/interrupts | grep 144
144:          5          0  altera-gpio   0  gpiolib

Now the gpio414 that we exported for the KEY1 push button will operate the same
way.  If we press and release KEY1 while we monitor the 'value' file for gpio414
we should see the same behavior appear:

root@DE10-Nano:~# cat /sys/class/gpio/gpio414/value
1
root@DE10-Nano:~# cat /sys/class/gpio/gpio414/value
0
root@DE10-Nano:~# cat /sys/class/gpio/gpio414/value
1
root@DE10-Nano:~# cat /sys/class/gpio/gpio414/value
0

There is one other raw gpio on the DE10-Nano board that we could interact with,
it's the KEY2 push button connected to the HPS GPIO54 input which comes in
through the gpio1 controller that is assigned to gpiochip198.  To export this
gpio port we calculate it's position in gpio1 by using this equation:

root@DE10-Nano:~# expr 454 + 54 - 29
479
                       ^     ^    ^
                       |     |    |
                       |     |    +--- the number of HPS GPIO ports in gpio0
                       |     +-------- the HPS GPIO we want
                       +-------------- the base gpio in gpiochip454

So we want to export gpio 479 to gain access to this port, like this:

root@DE10-Nano:~# echo 479 > /sys/class/gpio/export
root@DE10-Nano:~# ls /sys/class/gpio/
export       gpio414      gpiochip413  gpiochip419  gpiochip454  unexport
gpio413      gpio479      gpiochip415  gpiochip427  gpiochip483

And we see the gpio479 file in the sysfs now.  We can interact with this gpio
the same way that we did the others:

root@DE10-Nano:~# cat /sys/class/gpio/gpio479/value
1
root@DE10-Nano:~# cat /sys/class/gpio/gpio479/value
0
root@DE10-Nano:~# cat /sys/class/gpio/gpio479/value
1

And if we enable interrupts for this port like we did on the other port:

root@DE10-Nano:~# echo falling > /sys/class/gpio/gpio479/edge
root@DE10-Nano:~# cat /sys/class/gpio/gpio479/edge
falling

Then we see this gpio controller appear in the '/proc/interrupts' listing:

root@DE10-Nano:~# cat /proc/interrupts | grep 116
116:          0          0  gpio-dwapb   3  0-0053

When we are done using these gpio ports that we have exported into user space we
can 'unexport' them by passing the same values into the 'unexport' sysfs file
that we used to export the ports originally:

root@DE10-Nano:~# echo 413 > /sys/class/gpio/unexport
root@DE10-Nano:~# echo 414 > /sys/class/gpio/unexport
root@DE10-Nano:~# echo 479 > /sys/class/gpio/unexport
root@DE10-Nano:~# ls /sys/class/gpio
export       gpiochip415  gpiochip427  gpiochip483
gpiochip413  gpiochip419  gpiochip454  unexport

And we can see that the exported entries have been removed from the sysfs.

--------------------------------------------------------------------------------
Example programs and scripts
--------------------------------------------------------------------------------
This directory contains a few examples to demonstrate how to interact with the
push buttons on the DE10-Nano board that have been registered in the gpio
framework.  There is a shell script called 'show_KEY0_pb_state.sh' and a C
program called 'show_KEY0_pb_state.c'.  Each of these examples query the gpio
state in exactly the same way.  Then there is a C program called
'poll_KEY0_pb_state.c' which simply adds a poll() call to the
'show_KEY0_pb_state.sh' program to demonstrate the interrupt functionality
provided by the gpio framework to detect the push button press via a hardware
interrupt, which can only be accomplished with the poll() or select() type of
system call.

To build the 'show_KEY0_pb_state.c' application simply run the
'build_show_KEY0_pb_state.sh' shell script.  That will compile the
'show_KEY0_pb_state.c' source file and produce the executable
'show_KEY0_pb_state' application.  Refer to the 'build_show_KEY0_pb_state.sh'
script to see how the application is actually compiled and refer to the C
program source file for more details on how it actually works.

To build the 'poll_KEY0_pb_state.c' application simply run the
'build_poll_KEY0_pb_state.sh' shell script.  That will compile the
'poll_KEY0_pb_state.c' source file and produce the executable
'poll_KEY0_pb_state' application.  Refer to the 'build_poll_KEY0_pb_state.sh'
script to see how the application is actually compiled and refer to the C
program source file for more details on how it actually works.

Refer to the 'show_KEY0_pb_state.sh' source file for more details on how it
actually works.

Once you've built the applications, you can run both the script and the
applications like this:

./show_KEY0_pb_state.sh		<<< to run the script
./show_KEY0_pb_state		<<< to run the program
./poll_KEY0_pb_state		<<< to run the program with poll()

The 'show' program and script will read the current state of the KEY0 push
button input and print it out, and then exit.  The 'poll' program will enable
the KEY0 push button to generate a falling edge interrupt when it is pressed and
then ask you to press the KEY0 push button.  When the 'poll' program detects the
interrupt event it prints that status out and exits.

Both the programs and the script monitor the push buttons by interacting with
the linux gpio controller framework.


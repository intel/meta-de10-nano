SUMMARY = "Example applications for DE-Nano FPGA GPIO manipulation"
SECTION = "DE10-Nano"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=d252b8b27e3e6ea89c9c6912b3c4373d"

PACKAGE_ARCH = "${MACHINE_ARCH}"

PR = "r0"
PV = "1.0"
PV_append = "+${PR}"

SRC_URI = "\
	file://build_poll_KEY0_pb_state.sh \
	file://build_show_KEY0_pb_state.sh \
	file://build_toggle_fpga_leds.sh \
	file://build_watch_switch_events_ioctl.sh \
	file://build_watch_switch_events.sh \
	file://LICENSE \
	file://poll_KEY0_pb_state.c \
	file://README_first.txt \
	file://README_gpio-keys.txt \
	file://README_gpio-leds.txt \
	file://README_gpio.txt \
	file://show_KEY0_pb_state.c \
	file://show_KEY0_pb_state.sh \
	file://toggle_fpga_leds.c \
	file://toggle_fpga_leds.sh \
	file://watch_switch_events.c \
	file://watch_switch_events_ioctl.c \
	file://watch_switch_events.sh \
	file://find_gpio_controllers_dt.src \
	file://find_gpio_leds_dt.src \
	file://find_gpio_keys_dt.src \
"
TOP_FILES  = "LICENSE"
TOP_FILES += "README_first.txt"

RAW_GPIO_FILES  = "build_poll_KEY0_pb_state.sh"
RAW_GPIO_FILES += "build_show_KEY0_pb_state.sh"
RAW_GPIO_FILES += "poll_KEY0_pb_state.c"
RAW_GPIO_FILES += "README_gpio.txt"
RAW_GPIO_FILES += "show_KEY0_pb_state.c"
RAW_GPIO_FILES += "show_KEY0_pb_state.sh"
RAW_GPIO_FILES += "find_gpio_controllers_dt.src"

GPIO_LEDS_FILES  = "build_toggle_fpga_leds.sh"
GPIO_LEDS_FILES += "README_gpio-leds.txt"
GPIO_LEDS_FILES += "toggle_fpga_leds.c"
GPIO_LEDS_FILES += "toggle_fpga_leds.sh"
GPIO_LEDS_FILES += "find_gpio_leds_dt.src"

GPIO_KEYS_FILES  = "build_watch_switch_events_ioctl.sh"
GPIO_KEYS_FILES += "build_watch_switch_events.sh"
GPIO_KEYS_FILES += "README_gpio-keys.txt"
GPIO_KEYS_FILES += "watch_switch_events.c"
GPIO_KEYS_FILES += "watch_switch_events_ioctl.c"
GPIO_KEYS_FILES += "watch_switch_events.sh"
GPIO_KEYS_FILES += "find_gpio_keys_dt.src"

S = "${WORKDIR}"

do_install () {
	
	cd ${S}

	install -d ${D}/examples/gpio/sandbox
	cp -a ${TOP_FILES} ${D}/examples/gpio/sandbox

	install -d ${D}/examples/gpio/sandbox/raw-gpio
	cp -a ${RAW_GPIO_FILES} ${D}/examples/gpio/sandbox/raw-gpio

	install -d ${D}/examples/gpio/sandbox/gpio-leds
	cp -a ${GPIO_LEDS_FILES} ${D}/examples/gpio/sandbox/gpio-leds

	install -d ${D}/examples/gpio/sandbox/gpio-keys
	cp -a ${GPIO_KEYS_FILES} ${D}/examples/gpio/sandbox/gpio-keys

	rm -f gpio_sandbox.tgz
	tar czf gpio_sandbox.tgz -C ${D}/examples/gpio sandbox
	cp -a gpio_sandbox.tgz ${D}/examples/gpio
}

FILES_${PN} += "examples/gpio/"


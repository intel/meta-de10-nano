SUMMARY = "Example applications for DE10-Nano accelerometer"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=d252b8b27e3e6ea89c9c6912b3c4373d"

PACKAGE_ARCH = "${MACHINE_ARCH}" 

PR = "r0"

SRC_URI = "\
	file://adxl_calibrate.sh \
	file://build_bubble_demo.sh \
	file://build_tap_detect.sh \
	file://build_watch_adxl.sh \
	file://LICENSE \
	file://bubble_demo.c \
	file://README_SANDBOX.txt \
	file://README_SRC.txt \
	file://tap_detect.c \
	file://watch_adxl.c \
"

ADXL_BIN_FILES  = "bubble_demo"

ADXL_SRC_FILES  = "build_bubble_demo.sh"
ADXL_SRC_FILES += "LICENSE"
ADXL_SRC_FILES += "bubble_demo.c"
ADXL_SRC_FILES += "README_SRC.txt"

ADXL_SANDBOX_FILES  = "adxl_calibrate.sh"
ADXL_SANDBOX_FILES += "build_tap_detect.sh"
ADXL_SANDBOX_FILES += "build_watch_adxl.sh"
ADXL_SANDBOX_FILES += "LICENSE"
ADXL_SANDBOX_FILES += "README_SANDBOX.txt"
ADXL_SANDBOX_FILES += "tap_detect.c"
ADXL_SANDBOX_FILES += "watch_adxl.c"

S = "${WORKDIR}"

do_configure () {

	rm -f bubble_demo
}
do_compile () {

	${CC} \
		-march=armv7-a \
		-mfloat-abi=hard \
		-mfpu=vfp3 \
		-mthumb-interwork \
		-mthumb \
		-O2 -D_FORTIFY_SOURCE=2 \
		-g \
		-feliminate-unused-debug-types  \
		-std=gnu99 \
		-W \
		-Wall \
		-Werror \
		-Wc++-compat \
		-Wwrite-strings \
		-Wstrict-prototypes \
		-Wformat -Wformat-security \
		-pedantic \
		-fstack-protector-strong \
		-z noexecstack \
		-z relro -z now \
		-o "bubble_demo" \
		"bubble_demo.c"
}

do_install () {
	
	cd ${S}

	install -d ${D}/examples/adxl/bin
	cp -a ${ADXL_BIN_FILES} ${D}/examples/adxl/bin

	install -d ${D}/examples/adxl/src
	cp -a ${ADXL_SRC_FILES} ${D}/examples/adxl/src

	install -d ${D}/examples/adxl/sandbox
	cp -a ${ADXL_SANDBOX_FILES} ${D}/examples/adxl/sandbox

	rm -f adxl_src.tgz
	tar czf adxl_src.tgz -C ${D}/examples/adxl/src ${ADXL_SRC_FILES}
	cp -a adxl_src.tgz ${D}/examples/adxl

	rm -f adxl_sandbox.tgz
	tar czf adxl_sandbox.tgz -C ${D}/examples/adxl/sandbox ${ADXL_SANDBOX_FILES}
	cp -a adxl_sandbox.tgz ${D}/examples/adxl
}

INHIBIT_PACKAGE_DEBUG_SPLIT = "1"
PACKAGES = "${PN}"
FILES_${PN} += "examples/adxl/"


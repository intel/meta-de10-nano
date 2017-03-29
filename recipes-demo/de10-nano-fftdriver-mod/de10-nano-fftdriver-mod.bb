SUMMARY = "Example Driver for DE10-Nano FPGA based FFT"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=12f884d2ae1ff87c09e5b7ccc2c4ca7e"
SECTION = "DE10-Nano"

inherit module

PACKAGE_ARCH = "${MACHINE_ARCH}"

PR = "r0"
PV = "1.0"
PV_append = "+${PR}"

SRC_URI = "file://Makefile \
           file://COPYING \
           file://fft_driver.c \
           file://fft_driver.h \
          "

S = "${WORKDIR}"

# The inherit of module.bbclass will automatically name module packages with
# "kernel-module-" prefix as required by the oe-core build environment.

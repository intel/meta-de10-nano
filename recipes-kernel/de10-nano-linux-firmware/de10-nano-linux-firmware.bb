SUMMARY = "Firmware for the DE0-Nano and DE10-Nano FPGAs"

LICENSE = "Firmware-de10-nano & Firmware-de0-nano & MIT"

LIC_FILES_CHKSUM = "\
	file://${WORKDIR}/hardware/de10-nano/output_files/LICENSE.de10-nano.rbf;md5=b023c4e641168784dd29eca3d87d3dba \
	file://${WORKDIR}/hardware/de0-nano/output_files/LICENSE.de0-nano.rbf;md5=4f612b7b520cc528f216ce2b7a6fd620 \
	file://${WORKDIR}/hardware/LICENSE;md5=d252b8b27e3e6ea89c9c6912b3c4373d \
"

NO_GENERIC_LICENSE[Firmware-de10-nano] = "${WORKDIR}/hardware/de10-nano/output_files/LICENSE.de10-nano.rbf"
NO_GENERIC_LICENSE[Firmware-de0-nano] = "${WORKDIR}/hardware/de0-nano/output_files/LICENSE.de0-nano.rbf"

inherit deploy

PACKAGE_ARCH = "${MACHINE_ARCH}"

SRCREV_FORMAT = "hardware"

# Build:03-22-17_01.32.46 QUARTUS:Version=16.1.2.203
SRCREV_hardware = "d03450606b22a5f4f0d39da79fe169745ceffbec"

PV="${PN}+git${SRCPV}"

SRC_URI += " \
	git://github.com/01org/de10-nano-hardware.git;destsuffix=hardware;name=hardware;branch=RELEASE_BUILDS;protocol=https \
"

do_install () {
        cd ${WORKDIR}/hardware
	install -d ${D}${base_libdir}/firmware
	install -m 0644 LICENSE ${D}${base_libdir}/firmware/LICENSE.${MACHINE}.dtbo
        install -m 0644 devicetrees/${MACHINE}.dtbo ${D}${base_libdir}/firmware

}

do_deploy () {
        cd ${WORKDIR}/hardware
	install -d ${DEPLOYDIR}
	install -m 0644 ${MACHINE}/output_files/LICENSE.${MACHINE}.rbf ${DEPLOYDIR}
	install -m 0644 ${MACHINE}/output_files/${MACHINE}.rbf ${DEPLOYDIR}
}

addtask deploy after do_install

FILES_${PN} = "${base_libdir}/firmware"
LICENSE_${PN} = "MIT"


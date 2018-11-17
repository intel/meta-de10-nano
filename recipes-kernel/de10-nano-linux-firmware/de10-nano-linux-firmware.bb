SUMMARY = "Firmware for the DE0-Nano and DE10-Nano FPGAs"

LICENSE = "Firmware-de10-nano & Firmware-de0-nano & MIT"

LIC_FILES_CHKSUM = "\
	file://${WORKDIR}/de10-nano-fft/output_files/LICENSE.de10-nano-fft.rbf;md5=61ed20c72cdbf29b0335d0658105cab8 \
	file://${WORKDIR}/LICENSE;md5=d252b8b27e3e6ea89c9c6912b3c4373d \
"

NO_GENERIC_LICENSE[Firmware-de10-nano] = "${WORKDIR}/de10-nano-fft/output_files/LICENSE.de10-nano-fft.rbf"

inherit deploy

PACKAGE_ARCH = "${MACHINE_ARCH}"

SRCREV_FORMAT = "hardware"

# Build:03-22-17_01.32.46 QUARTUS:Version=16.1.2.203
# SRCREV_hardware = "d03450606b22a5f4f0d39da79fe169745ceffbec"

SRC_URI_append_de10-nano = "\
    https://github.com/intel/de10-nano-hardware/releases/download/RELEASE-20180612_19.29.23/de10-nano-build_20180612.tgz \
"
SRC_URI[md5sum] = "cb8b127e813a22d73f94bc543ddcbb76"



#PV="${PN}+git${SRCPV}"
#SRC_URI += " \
#	git://github.com/01org/de10-nano-hardware.git;destsuffix=hardware;name=hardware;branch=RELEASE_BUILDS;protocol=https \
#"

do_install () {
        cd ${WORKDIR}
	install -d ${D}${base_libdir}/firmware
	install -m 0644 LICENSE ${D}${base_libdir}/firmware/LICENSE.${MACHINE}-fft.dtbo
        install -m 0644 devicetrees/${MACHINE}-fft.dtbo ${D}${base_libdir}/firmware

}

do_deploy () {
        cd ${WORKDIR}
	install -d ${DEPLOYDIR}
	install -m 0644 ${MACHINE}-fft/output_files/LICENSE.${MACHINE}-fft.rbf ${DEPLOYDIR}
	install -m 0644 ${MACHINE}-fft/output_files/${MACHINE}-fft.rbf ${DEPLOYDIR}
}

addtask deploy after do_install

FILES_${PN} = "${base_libdir}/firmware"
LICENSE_${PN} = "MIT"


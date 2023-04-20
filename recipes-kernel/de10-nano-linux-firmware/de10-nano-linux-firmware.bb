SUMMARY = "Firmware for the DE10-Nano FPGAs"

LICENSE = "Firmware-de10-nano & MIT"

inherit deploy

PACKAGE_ARCH = "${MACHINE_ARCH}"

SRCREV_FORMAT = "hardware"

SRC_URI_append_de10-nano = "\
     ${DE10_NANO_HW_SRC} \
"
SRC_URI[md5sum] = "${DE10_NANO_HW_MD5}"

NO_GENERIC_LICENSE[Firmware-de10-nano] = "${DE10_NANO_HW_RBF_LIC_PATH}"

LIC_FILES_CHKSUM = "${DE10_NANO_HW_LIC_FILES_CHKSUM}"

do_install () {
	cd ${WORKDIR}
	install -d ${D}${base_libdir}/firmware
	install -m 0644 ${DE10_NANO_HW_DTBO_LIC_PATH} ${D}${base_libdir}/firmware/LICENSE.${MACHINE}.dtbo
	install -m 0644 ${DE10_NANO_HW_DTBO_PATH}     ${D}${base_libdir}/firmware/${MACHINE}.dtbo

}

do_deploy () {
	cd ${WORKDIR}
	install -d ${DEPLOYDIR}
	install -m 0644 ${DE10_NANO_HW_RBF_LIC_PATH} ${DEPLOYDIR}/LICENSE.${MACHINE}.rbf
	install -m 0644 ${DE10_NANO_HW_RBF_PATH}     ${DEPLOYDIR}/${MACHINE}.rbf
}

addtask deploy after do_install

FILES_${PN} = "${base_libdir}/firmware"
LICENSE_${PN} = "MIT"


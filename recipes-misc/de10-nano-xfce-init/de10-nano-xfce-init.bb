SUMMARY = "DE10-Nano XFCE service"
DESCRIPTION = "Start xfce after FPGA load."
AUTHOR = "Dalon Westergreen <dwesterg@gmail.com>"
SECTION = "DE10-Nano"

PR = "r0"

inherit systemd

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/LICENSE;md5=4d92cd373abda3937c2bc47fbc49d690"

PACKAGE_ARCH = "${MACHINE_ARCH}"

PV = "1.0${PR}"
FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
SRC_URI = "file://de10-nano-xfce-init.service \
          "

do_install() {
	install -d ${D}${base_libdir}/systemd/system
	install -m 0644 ${WORKDIR}/*.service ${D}${base_libdir}/systemd/system
}

ALLOW_EMPTY_${PN} = "1"

FILES_${PN} = "${base_libdir}/systemd/system/de10-nano-xfce-init.service \
		      "

NATIVE_SYSTEMD_SUPPORT = "1"
SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE_${PN} = "de10-nano-xfce-init.service"

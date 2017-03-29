SUMMARY = "DE-Nano initialize synergy client"
DESCRIPTION = "Scripts to start synergy client"
AUTHOR = "Dalon Westergreen <dwesterg@gmail.com>"
SECTION = "DE10-Nano"

PR = "r0"

inherit systemd

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/LICENSE;md5=4d92cd373abda3937c2bc47fbc49d690"

PACKAGE_ARCH = "${MACHINE_ARCH}"
PV = "1.0${PR}"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
SRC_URI = "file://de10-nano-synergy-init.service \
          "

do_install() {
	install -d ${D}${base_libdir}/systemd/system
	install -m 0644 ${WORKDIR}/*.service ${D}${base_libdir}/systemd/system
}

FILES_${PN} = "${base_libdir}/systemd/system/de10-nano-synergy-init.service \
               ${sysconfdir}/systemd/ \
              "

NATIVE_SYSTEMD_SUPPORT = "1"
SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE_${PN} = "de10-nano-synergy-init.service"

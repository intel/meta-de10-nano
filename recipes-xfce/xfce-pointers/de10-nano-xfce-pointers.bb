SUMMARY = "DE10-Nano XFCE Pointers config"
DESCRIPTION = "Disable ADXL accelerometer as mouse input"
AUTHOR = "Dalon Westergreen <dwesterg@gmail.com>"
SECTION = "DE10-Nano"

PR = "r0"

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/LICENSE;md5=4d92cd373abda3937c2bc47fbc49d690"

PACKAGE_ARCH = "${MACHINE_ARCH}"

PV = "1.0${PR}"
FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
SRC_URI = "file://pointers.xml \
          "

do_install() {
	install -d ${D}${sysconfdir}/skel/
	install -d ${D}${sysconfdir}/skel/.config/xfce4
	install -d ${D}${sysconfdir}/skel/.config/xfce4/xfconf
	install -d ${D}${sysconfdir}/skel/.config/xfce4/xfconf/xfce-perchannel-xml
	install -m 0644 ${WORKDIR}/pointers.xml ${D}${sysconfdir}/skel/.config/xfce4/xfconf/xfce-perchannel-xml/pointers.xml

	install -d ${D}/home/root/.config/xfce4/xfconf/xfce-perchannel-xml
	install -m 0644 ${WORKDIR}/pointers.xml ${D}/home/root/.config/xfce4/xfconf/xfce-perchannel-xml/pointers.xml
}

FILES_${PN} = "\
		${sysconfdir}/skel/.config/xfce4/xfconf/xfce-perchannel-xml/pointers.xml \
		/home/root/.config/xfce4/xfconf/xfce-perchannel-xml/pointers.xml \
		"

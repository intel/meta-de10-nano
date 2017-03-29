DESCRIPTION = "QT Application to control DE10-Nano USB Gadget"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=d252b8b27e3e6ea89c9c6912b3c4373d"

PACKAGE_ARCH = "${MACHINE_ARCH}" 

DEPENDS = "qtbase"
inherit qmake5

S = "${WORKDIR}"

inherit pkgconfig

PR = "r0"

SRC_URI = " \
	file://dialog.cpp \
	file://mainwindow.cpp \
	file://dialog.h \
	file://mainwindow.h \
	file://dialog.ui \
	file://mainwindow.ui \
	file://LICENSE \
	file://USB_Gadget.pro \
	file://main.cpp \
	file://USB_Gadget.pro.user \
	"

FILES_${PN} = " \
	/home/root/Desktop/USB_Gadget \
	"

do_install() {
	install -d ${D}/home/root/Desktop
	install -m 755 ${WORKDIR}/build/USB_Gadget ${D}/home/root/Desktop
}


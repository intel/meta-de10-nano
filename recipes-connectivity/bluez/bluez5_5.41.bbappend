#for SDP example we need bluez4 compatiblity
FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
SRC_URI_append = "\
	file://bluetooth.service \
	"

do_install_append() {
	install -m 644 ${WORKDIR}/bluetooth.service ${D}${base_libdir}/systemd/system
}


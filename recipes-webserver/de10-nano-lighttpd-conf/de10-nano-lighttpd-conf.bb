SUMMARY = "DE10-Nano customization"
DESCRIPTION = "DE-Nano lighttpd configuration customization to enable cgi, set 404 page, and change serverroot"
AUTHOR = "Dalon Westergreen <dwesterg@gmail.com>"
SECTION = "DE10-Nano"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM="file://${COMMON_LICENSE_DIR}/BSD-3-Clause;md5=550794465ba0ec5312d6919e203a55f9"

inherit allarch
ALLOW_EMPTY_${PN} = "1"

PR = "r0"
PV = "1.0${PR}"

# Post process the lighttpd.conf file to enable cgi module, sets home page and update 404 page
pkg_postinst_${PN}() {
#!/bin/sh
sed -ir 's:#\(.*mod_cgi\"\,\):\1:' $D/etc/lighttpd.conf
sed -i '/server.document-root/c server.document-root        = "/usr/share/webcontent"' $D/etc/lighttpd.conf
sed -ir 's:\(index-file.names *\=\).*\(,\):\1 ("index.html"\2:' $D/etc/lighttpd.conf
echo 'server.error-handler-404   = "/not_found.html"' >> $D/etc/lighttpd.conf
echo 'cgi.assign = ( ".sh"  => "/bin/sh" )' >> $D/etc/lighttpd.conf
}

RDEPENDS_{PN} = "lighttpd-module-cgi"

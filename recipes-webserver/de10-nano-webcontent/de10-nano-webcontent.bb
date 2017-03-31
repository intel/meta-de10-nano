DESCRIPTION = "DE10-Nano web content"
AUTHOR = "Dalon Westergreen <dwesterg@gmail.com>"
SECTION = "DE10-Nano"

LICENSE = "MIT & GPLv2"
LIC_FILES_CHKSUM = "file://LICENSE;md5=d252b8b27e3e6ea89c9c6912b3c4373d"

PACKAGE_ARCH = "${MACHINE_ARCH}"

PR = "r0"

SRCREV = "d698e162fe303b6fd382f04f1aa343db1f11bd25"
PV = "1.0+git${SRCPV}"


SRC_URI += " \
	git://github.com/01org/de10-nano-webcontent.git;protocol=https \
	"

S = "${WORKDIR}/git"

inherit allarch

IMAGEDATESTAMP = "${@time.strftime('%Y.%m.%d',time.gmtime())}"

do_install() {
	install -d ${D}${datadir}/webcontent
	cp -a ${S}/* ${D}${datadir}/webcontent
	echo "DE10-Nano Image" > ${D}${datadir}/webcontent/assets/image-version-info.txt 
	echo "BUILD VERSION: ${IMAGEDATESTAMP}" >> ${D}${datadir}/webcontent/assets/image-version-info.txt
	if [ -n "${DISTRO_NAME}" ]; then
	  echo -n "DISTRO: ${DISTRO_NAME} " >> ${D}${datadir}/webcontent/assets/image-version-info.txt
	  if [ -n "${DISTRO_VERSION}" ]; then
	    echo "${DISTRO_VERSION}" >> ${D}${datadir}/webcontent/assets/image-version-info.txt
	  fi
	fi
}

FILES_${PN} += "${datadir}/webcontent"

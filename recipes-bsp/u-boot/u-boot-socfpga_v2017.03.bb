require recipes-bsp/u-boot/u-boot-socfpga-common.inc                                               
require recipes-bsp/u-boot/u-boot-socfpga-env.inc                                                  
require ${COREBASE}/meta/recipes-bsp/u-boot/u-boot.inc 

PR = "r0"

FILESEXTRAPATHS_prepend := "${THISDIR}/files/v2017.03:"

DEPENDS += "u-boot-mkimage-native"

SRCREV = "b24cf8540a85a9bf97975aadd6a7542f166c78a3"

SRCREV_FORMAT = "hardware"

# Build:03-22-17_01.32.46 QUARTUS:Version=16.1.2.203
SRCREV_hardware = "d03450606b22a5f4f0d39da79fe169745ceffbec" 

SRC_URI_append = "\
	file://0001-arm-socfpga-Move-CONFIG_EXTRA_ENV_SETTINGS-to-common.patch \
	file://0002-arm-socfpga-Update-DE0-Nano-SoC-to-support-distro-bo.patch \
	file://0003-arm-socfpga-add-support-for-Terasic-DE10-Nano-board.patch \
	file://0004-Add-HDMI-init-to-de10-env.patch \
	file://0005-Add-DE10-Nano-HDMI-configuration-and-debug-apps.patch \
	file://0006-arm-socfpga-fix-issue-with-warm-reset-when-CSEL-is-0.patch \
	file://STARTUP.BMP \
	file://STARTUP.BMP.LICENSE \
	"

SRC_URI_append_de0-nano = "\
	git://github.com/01org/de10-nano-hardware.git;destsuffix=hardware;name=hardware;branch=RELEASE_BUILDS;protocol=https \
"

SRC_URI_append_de10-nano = "\
	git://github.com/01org/de10-nano-hardware.git;destsuffix=hardware;name=hardware;branch=RELEASE_BUILDS;protocol=https \
"

# Update project specific files in uboot 
do_configure_append_de0-nano() {
	${WORKDIR}/git/arch/arm/mach-socfpga/qts-filter.sh \
	    cyclone5 \
	    ${WORKDIR}/hardware/de0-nano/ \
	    ${WORKDIR}/hardware/de0-nano/preloader/ \
	    ${WORKDIR}/git//board/terasic/de0-nano-soc/qts/
}

do_configure_append_de10-nano() {
	${WORKDIR}/git/arch/arm/mach-socfpga/qts-filter.sh \
	    cyclone5 \
	    ${WORKDIR}/hardware/de10-nano/ \
	    ${WORKDIR}/hardware/de10-nano/preloader/ \
	    ${WORKDIR}/git/board/terasic/de10-nano/qts/
}

do_deploy_append() {
	install -m 644 ${B}/socfpga_de10_nano_defconfig/examples/standalone/de10_nano_hdmi_config.bin ${DEPLOYDIR}
	install -m 644 ${B}/socfpga_de10_nano_defconfig/examples/standalone/de10_nano_hdmi_config.srec ${DEPLOYDIR}

	install -m 644 ${B}/socfpga_de10_nano_defconfig/examples/standalone/dump_adv7513_edid.bin ${DEPLOYDIR}
	install -m 644 ${B}/socfpga_de10_nano_defconfig/examples/standalone/dump_adv7513_edid.srec ${DEPLOYDIR}

	install -m 644 ${B}/socfpga_de10_nano_defconfig/examples/standalone/dump_adv7513_regs.bin ${DEPLOYDIR}
	install -m 644 ${B}/socfpga_de10_nano_defconfig/examples/standalone/dump_adv7513_regs.srec ${DEPLOYDIR}

	install -m 644 ${WORKDIR}/STARTUP.BMP ${DEPLOYDIR}
	install -m 644 ${WORKDIR}/STARTUP.BMP.LICENSE ${DEPLOYDIR}
}

DEPENDS += "dtc-native"                                                         
DEPENDS += "u-boot-mkimage-native"

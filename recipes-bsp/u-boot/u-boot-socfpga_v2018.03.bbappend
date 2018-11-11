
FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRCREV_FORMAT = "hardware"

SRCREV_hardware = "d03450606b22a5f4f0d39da79fe169745ceffbec" 

SRC_URI_append = "\
  file://v2018.03/0001-Add-DE10-Nano-HDMI-configuration-and-debug-apps.patch \
  file://v2018.03/0002-Fixes-setenv-env_set-call.patch \
  file://STARTUP.BMP \
  file://STARTUP.BMP.LICENSE \
"


SRC_URI_append_de10-nano = "\
	git://github.com/intel/de10-nano-hardware.git;destsuffix=hardware;name=hardware;branch=RELEASE_BUILDS;protocol=https \
"

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




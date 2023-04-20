
FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRCREV_FORMAT = "hardware"

SRC_URI_append = "\
  file://v2018.03/0001-Add-DE10-Nano-HDMI-configuration-and-debug-apps.patch \
  file://v2018.03/0002-Fixes-setenv-env_set-call.patch \
  file://STARTUP.BMP \
  file://STARTUP.BMP.LICENSE \
"

RRECOMMENDS_${PN} += "de10-u-boot-scr" 

SRC_URI_append_de10-nano = "\
    https://github.com/intel/de10-nano-hardware/releases/download/RELEASE-20180612_19.29.23/de10-nano-build_20180612.tgz \
"
SRC_URI[md5sum] = "cb8b127e813a22d73f94bc543ddcbb76"

do_configure_append_de10-nano() {
	${WORKDIR}/git/arch/arm/mach-socfpga/qts-filter.sh \
	    cyclone5 \
	    ${WORKDIR}/de10-nano-fft/ \
	    ${WORKDIR}/de10-nano-fft/preloader/ \
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




PREFERRED_PROVIDER_virtual/kernel ?= "linux-altera-ltsi"
PREFERRED_VERSION_linux-altera-ltsi ?= "4.9%"

PREFERRED_PROVIDER_virtual/bootloader ?= "u-boot-socfpga"
PREFERRED_VERSION_u-boot-socfpga = "v2018.03%"

UBOOT_CONFIG ??= "de10-nano"
UBOOT_CONFIG[de10-nano] = "socfpga_de10_nano_defconfig"

SPL_BINARY = "u-boot-with-spl.sfp"

IMAGE_FSTYPES = "cpio ext3 tar.gz wic"

IMAGE_OVERHEAD_FACTOR ?= "1.3"

require conf/machine/include/socfpga.inc

IMAGE_BOOT_FILES_append = "de10-nano.rbf LICENSE.de10-nano.rbf de10_nano_hdmi_config.bin dump_adv7513_edid.bin dump_adv7513_regs.bin STARTUP.BMP STARTUP.BMP.LICENSE boot.scr"

INHERIT += " rootfs_ipk "

# This is to remove unmaintained packages which pose security/support issues
BAD_RECOMMENDATIONS = " libnss-mdns cpufrequtils cpufreq-tweaks fbset consolekit wireless-tools "
RDEPENDS_pulseaudio-module-console-kit_remove = "consolekit"

DISTRO_FEATURES_remove = "opengl"                                               
DISTRO_FEATURES_remove = "opencl"                                               
DISTRO_FEATURES_remove = "wayland"                                              
DISTRO_FEATURES_remove = "zeroconf"

# remove wireless-tools as it is unmaintained
VIRTUAL-RUNTIME_wireless-tools = "iw"

# set empty root password
INHERIT += " extrausers "
EXTRA_USERS_PARAMS = "usermod -p U6aMy0wojraho root;"

require conf/distro/include/security_flags.inc 
# with security flags, some packages fail
SECURITY_CFLAGS_pn-libvncserver = ""


DE10_NANO_HW_SRC           ??= "https://github.com/intel/de10-nano-hardware/releases/download/RELEASE-20180612_19.29.23/de10-nano-build_20180612.tgz"
DE10_NANO_HW_MD5           ??= "cb8b127e813a22d73f94bc543ddcbb76"

DE10_NANO_HW_DTBO_LIC_PATH ??= "${WORKDIR}/LICENSE"
DE10_NANO_HW_DTBO_PATH     ??= "${WORKDIR}/devicetrees/de10-nano-fft.dtbo"
DE10_NANO_HW_RBF_LIC_PATH  ??= "${WORKDIR}/de10-nano-fft/output_files/LICENSE.de10-nano-fft.rbf"
DE10_NANO_HW_RBF_PATH      ??= "${WORKDIR}/de10-nano-fft/output_files/de10-nano-fft.rbf"

DE10_NANO_HW_LIC_FILES_CHKSUM ??= "\
	file://${WORKDIR}/de10-nano-fft/output_files/LICENSE.de10-nano-fft.rbf;md5=61ed20c72cdbf29b0335d0658105cab8 \
	file://${WORKDIR}/LICENSE;md5=d252b8b27e3e6ea89c9c6912b3c4373d \
"


PREFERRED_PROVIDER_virtual/kernel ?= "linux-altera-ltsi"
PREFERRED_VERSION_linux-altera-ltsi ?= "4.1%"

PREFERRED_PROVIDER_virtual/bootloader ?= "u-boot-socfpga"
PREFERRED_VERSION_u-boot-socfpga = "v2017.03%"

UBOOT_CONFIG ??= "de10-nano"
UBOOT_CONFIG[de0-nano-soc] = "socfpga_de0_nano_soc_defconfig"
UBOOT_CONFIG[de10-nano] = "socfpga_de10_nano_defconfig"

SPL_BINARY = "u-boot-with-spl.sfp"

IMAGE_CLASSES += "sdcard_image-socfpga"

IMAGE_FSTYPES = "cpio ext3 tar.gz socfpga-sdimg"

IMAGE_OVERHEAD_FACTOR = "3"

require conf/machine/include/socfpga.inc

SOCFPGA_SDIMG_EXTRA_FILES = "de10-nano.rbf de0-nano.rbf LICENSE.de10-nano.rbf LICENSE.de0-nano.rbf de10_nano_hdmi_config.bin dump_adv7513_edid.bin dump_adv7513_regs.bin STARTUP.BMP STARTUP.BMP.LICENSE"

INHERIT += " rootfs_ipk "

# This is to remove unmaintained packages which pose security/support issues
BAD_RECOMMENDATIONS = " libavahi-core avahi-utils avahi-daemon avahi-autoipd libnss-mdns cpufrequtils fbset consolekit wireless-tools "
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

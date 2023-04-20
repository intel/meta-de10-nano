PR = "r0"
FILESEXTRAPATHS_prepend := "${THISDIR}/config:"

DE10_NANO_EXTRA_KERNEL_CONFIG ??= ""

SRC_URI_append_de10-nano += " \
			file://socfpga-4.9-ltsi/cfg/block.cfg \
			file://socfpga-4.9-ltsi/cfg/configfs.cfg \
			file://socfpga-4.9-ltsi/cfg/gpio-keys.cfg \
			file://socfpga-4.9-ltsi/cfg/input_misc.cfg \
			file://socfpga-4.9-ltsi/cfg/led-triggers.cfg \
			file://socfpga-4.9-ltsi/cfg/usb_gadget.cfg \
			file://socfpga-4.9-ltsi/cfg/framebuffer.cfg \
			file://socfpga-4.9-ltsi/cfg/framebuffer-console.cfg \
			file://socfpga-4.9-ltsi/cfg/usb_vid.cfg \
			file://socfpga-4.9-ltsi/cfg/bluetooth.cfg \
			file://socfpga-4.9-ltsi/cfg/wireless.cfg \
			${DE10_NANO_EXTRA_KERNEL_CONFIG} \
			file://socfpga-4.9-ltsi/patches/0001-Add-DE10-Nano-devicetree.patch \
			file://socfpga-4.9-ltsi/patches/0002-Enable-mmc-in-de10_nano-dts.patch \
			"

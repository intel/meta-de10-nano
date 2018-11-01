PR = "r0"
FILESEXTRAPATHS_prepend := "${THISDIR}/config:"

SRC_URI_append_de0-nano += " \
			file://socfpga-4.9-ltsi/cfg/configfs.cfg \
			file://socfpga-4.9-ltsi/cfg/gpio-keys.cfg \
			file://socfpga-4.9-ltsi/cfg/input_misc.cfg \
			file://socfpga-4.9-ltsi/cfg/led-triggers.cfg \
			file://socfpga-4.9-ltsi/cfg/usb_gadget.cfg \
			file://socfpga-4.9-ltsi/cfg/usb_vid.cfg \
			file://socfpga-4.9-ltsi/cfg/bluetooth.cfg \
			file://socfpga-4.9-ltsi/cfg/wireless.cfg \
			file://socfpga-4.9-ltsi/patches/0001-Add-DE10-Nano-devicetree.patch \
			"
SRC_URI_append_de10-nano += " \
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
			file://socfpga-4.9-ltsi/patches/0001-Add-DE10-Nano-devicetree.patch \
			"

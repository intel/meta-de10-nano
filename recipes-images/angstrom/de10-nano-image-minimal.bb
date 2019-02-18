require recipes-images/angstrom/console-base-image.bb

PV = '1.0'
PR = 'r0'

DEPENDS += "bash perl gcc i2c-tools"


IMAGE_INSTALL += " \
	kernel-image \
	kernel-modules \
	linux-firmware \
	vim vim-vimrc \
	gdbserver \
	bash i2c-tools ethtool \
	vim \
	curl \
	python-multiprocessing \
	python-compiler \
	python-misc \
	de10-nano-linux-firmware \
	de10-nano-fpga-init \
	"

export IMAGE_BASENAME = "de10-nano-image-minimal"


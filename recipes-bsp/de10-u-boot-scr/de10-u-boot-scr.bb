SUMMARY = "U-boot boot scripts for DE10-nano"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"
COMPATIBLE_MACHINE = "^de10-nano$"

DEPENDS = "u-boot-mkimage-native"

INHIBIT_DEFAULT_DEPS = "1"

SRC_URI = "file://boot.cmd.in"

DE10_KERNEL_BOOTCMD ?= "console=ttyS0,115200 earlyprintk rootfstype=ext4 root=${mmcroot} ro rootwait mem=0x3F000000 memmap=16M$0x3F000000 "
DE10_BOOT_SCRIPT  ?= "${WORKDIR}/boot.cmd.in"

do_compile() {
    sed -e 's/@@KERNEL_IMAGETYPE@@/${KERNEL_IMAGETYPE}/' \
        -e 's/@@KERNEL_BOOTCMD@@/${DE10_KERNEL_BOOTCMD}/' \
        -e 's/@@MACHINE@@/${MACHINE}/' \
        ${DE10_BOOT_SCRIPT} > "${WORKDIR}/boot.cmd"
    mkimage -A arm -T script -C none -n "Boot script" -d "${WORKDIR}/boot.cmd" boot.scr
}

inherit deploy nopackages

do_deploy() {
    install -d ${DEPLOYDIR}
    install -m 0644 boot.scr ${DEPLOYDIR}
}

addtask do_deploy after do_compile before do_build

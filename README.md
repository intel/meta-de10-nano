# Layer to Support the Terasic DE10-Nano\* Board

## Overview
Instructions to build the image for the Terasic DE10-Nano\* development board and then write that image to a microSD card.

This layer provides support for building a demonstration and development image of Linux\* for the [Terasic DE10-Nano](https://www.terasic.com.tw/cgi-bin/page/archive.pl?Language=English&CategoryNo=205&No=1046&PartNo=8) kit's development board.

## Build Instructions
Please refer to [README.md](https://github.com/Angstrom-distribution/angstrom-manifest/blob/master/README.md) for any prerequisites. These instructions assume prerequisites have been met.  

**Note**: See the instructions for configuring proxies.

Steps to build the image:

1. Clone the manifest.
2. Add the meta-de10-nano layer.
3. Fetch repositories.
4. Build the image.


### Step 1: Clone the Manifest
Clone the manifest to get all required recipes for building the image.
```
mkdir de10-nano-build
cd de10-nano-build
repo init -u git://github.com/Angstrom-distribution/angstrom-manifest -b angstrom-v2016.12-yocto2.2 
```
### Step 2: Add the meta-de10-nano Layer
The default manifests do not include the meta-de10-layer. Therefore, we add the layer and resolve any issues encountered with errant layers.

```
mkdir .repo/local_manifests
```
Now we create the manifest to add the meta-de10-layer. The following will create .repo/local_manifests/de10-nano.xml. This specifies a specific revision for meta-altera and meta-de10-nano.

```
cat << EOF > .repo/local_manifests/de10-nano.xml
<?xml version="1.0" encoding="UTF-8"?>                                          
<manifest>                                                                      
        <remove-project name="kraj/meta-altera" />                              
        <remove-project name="koenkooi/meta-photography" />                     
        <remove-project name="openembedded/meta-linaro" />                      
        <project name="openembedded/meta-linaro" path="layers/meta-linaro" remote="linaro" revision="992eaa0a1969c2056a5321c122eaa8cd808c1c82" upstream="master"/>
        <project remote="github"  name="kraj/meta-altera" path="layers/meta-altera" revision="cf7fc462cc6a5e82f2de76bb21e09675be7ae316"/>
        <project name="01org/meta-de10-nano" path="layers/meta-de10-nano" remote="github" revision="refs/tags/VERSION-2017.03.31"/>
</manifest> 
EOF
```
The above also disables meta-photography, so we have to edit conf/bblayers.conf to remove the reference to it.
```
sed -i '/meta-photography/d' .repo/manifests/conf/bblayers.conf
```
We also need to add in the new meta-de10-nano layer to the bblayers.conf

```
sed -i '/meta-altera/a \ \ \$\{TOPDIR\}\/layers\/meta-de10-nano \\' .repo/manifests/conf/bblayers.conf
```
### Step 3: Fetch the Repositories from the Manifest
```
repo sync
```

### Step 4: Build the Image
Estimated to complete: 2–3 hours.
```
MACHINE=de10-nano . ./setup-environment
bitbake de10-nano-image
```

## After Building the Image
The result of this lengthy build is an image that can be written to an SD card which will enable the Terasic DE10-Nano board to boot Linux\*. The image provides access via serial port, a graphical interface, USB, and Ethernet. As part of the build, the recipes populate an FPGA image as well as the associated device trees.  

The build output is located in deploy/glibc/images/de10-nano/

```
[de10-nano]$ ls
Angstrom-de10-nano-image-glibc-ipk-v2016.12-de10-nano.rootfs.cpio           de10-nano.rbf                                                           u-boot-de10-nano.img
Angstrom-de10-nano-image-glibc-ipk-v2016.12-de10-nano.rootfs.ext3           dump_adv7513_edid.bin                                                   u-boot-de10-nano.img-de10-nano
Angstrom-de10-nano-image-glibc-ipk-v2016.12-de10-nano.rootfs.manifest       dump_adv7513_edid.srec                                                  u-boot-de10-nano-v2017.03+gitAUTOINC+d03450606b-r0.img
Angstrom-de10-nano-image-glibc-ipk-v2016.12-de10-nano.rootfs.socfpga-sdimg  dump_adv7513_regs.bin                                                   u-boot.img
Angstrom-de10-nano-image-glibc-ipk-v2016.12-de10-nano.rootfs.tar.gz         dump_adv7513_regs.srec                                                  u-boot.img-de10-nano
Angstrom-de10-nano-image-glibc-ipk-v2016.12-de10-nano.rootfs.tar.xz         extlinux.conf                                                           u-boot-with-spl.sfp
de10_nano_hdmi_config.bin                                                   extlinux.conf-de10-nano                                                 u-boot-with-spl.sfp-de10-nano
de10_nano_hdmi_config.srec                                                  extlinux.conf-de10-nano-r0                                              u-boot-with-spl.sfp-de10-nano-de10-nano
de10-nano-image-Angstrom-v2016.12.socfpga-sdimg                             LICENSE.de10-nano.rbf                                                   u-boot-with-spl.sfp-de10-nano-v2017.03+gitAUTOINC+d03450606b-r0-de10-nano-v2017.03+gitAUTOINC+d03450606b-r0
de10-nano-image-de10-nano.cpio                                              Log.txt                                                                 zImage
de10-nano-image-de10-nano.ext3                                              modules--4.1.33-ltsi+git0+b84195c056-r0.1-de10-nano-20170330172917.tgz  zImage--4.1.33-ltsi+git0+b84195c056-r0.1-de10-nano-20170330172917.bin
de10-nano-image-de10-nano.manifest                                          modules-de10-nano.tgz                                                   zImage--4.1.33-ltsi+git0+b84195c056-r0.1-socfpga_cyclone5_de10_nano-20170330172917.dtb
de10-nano-image-de10-nano.socfpga-sdimg                                     README_-_DO_NOT_DELETE_FILES_IN_THIS_DIRECTORY.txt                      zImage-de10-nano.bin
de10-nano-image-de10-nano.tar.gz                                            STARTUP.BMP                                                             zImage-socfpga_cyclone5_de10_nano.dtb
de10-nano-image-de10-nano.tar.xz                                            STARTUP.BMP.LICENSE

```
The SD card image name in the above list is "Angstrom-de10-nano-image-glibc-ipk-v2016.12-de10-nano.rootfs.socfpga-sdimg".  

**Note**: prebuilt images can be found [here](https://software.intel.com/en-us/iot/hardware/fpga/de10-nano).

### Write the Image to a MicroSD Card
These instructions only cover Linux\*, for alternate instructions please go [here](https://software.intel.com/en-us/write-image-to-micro-sd-card).

**Caution**: These instructions use the dd command which should be used with EXTREME CAUTION. It is very easy to accidentally overwrite the wrong device which can lead to data loss as well as hours spent rebuilding your machine. 

The first step is to insert the SD Card using either a dedicated SD Card interface or a USB adapter into your machine. Discover which device this shows up. Usually the device shows up as /dev/sdX or /dev/mmcblkX where X is the device number. As an example, our dedicated SD Card interface shows up as /dev/mmcblk0.

**Note**: for safety reasons these instructions will use /dev/mmcblkX as this should never be a real device.

It will take a few minutes to write the image (~2GB).
```
cd deploy/glibc/images/de10-nano/
sudo dd if=Angstrom-de10-nano-image-glibc-ipk-v2016.12-de10-nano.rootfs.socfpga-sdimg of=/dev/mmxblkX bs=1M && sync && sync
```

After this completes, insert the microSD card into the DE10-Nano board and then power it on.

 ## Additional Resources
* [Discover the Terasic DE10-Nano Kit](https://signin.intel.com/logout?target=https://software.intel.com/en-us/iot/hardware/fpga/de10-nano)
* [Terasic DE10-Nano Get Started Guide](https://software.intel.com/en-us/terasic-de10-nano-get-started-guide)
* [Project: My First FPGA](https://software.intel.com/en-us/articles/my-first-fpga)
* [Learn more about Intel® FPGAs](https://software.intel.com/en-us/iot/hardware/fpga/)
* [DE10-Nano FPGA Hardware Project](https://github.com/01org/de10-nano-hardware)

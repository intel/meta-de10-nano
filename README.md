# Layer to support the DE10-Nano board

## Objective
This layer provides support for building a demonstration and development image of linux for the [Terasic DE10-Nano kit](https://www.terasic.com.tw/cgi-bin/page/archive.pl?Language=English&CategoryNo=205&No=1046&PartNo=8) development board.

## Build Intructions
Please refer to [README.md](https://github.com/Angstrom-distribution/angstrom-manifest/blob/master/README.md) for any prerequisits, these instuctions assume prerequisits have been met.  Please also note the instructions for configuring proxies.

### Step 1: Cloning the manifest
We need to clone the manifest to get all of the recipes required for building
```
mkdir de10-nano-build
cd de10-nano-build
repo init -u git://github.com/Angstrom-distribution/angstrom-manifest -b angstrom-v2016.12-yocto2.2 
```
### Step 2: Add the meta-de10-nano layer
The default manifests do not include the meta-de10-layer.  We will add the layer, and tidy up some issue encountered with errant layers.

```
mkdir .repo/local_manifests
```
Now using your favorite editor, create a file .repo/local_manifests/de10-nano.xml and put the following in it:

```
<?xml version="1.0" encoding="UTF-8"?>                                          
<manifest>                                                                      
        <remove-project name="kraj/meta-altera" />                              
        <remove-project name="koenkooi/meta-photography" />                     
        <remove-project name="openembedded/meta-linaro" />                      
                                                                                        <project name="openembedded/meta-linaro" path="layers/meta-linaro" remote="linaro" revision="992eaa0a1969c2056a5321c122eaa8cd808c1c82" upstream="master"/>
        <project remote="github"  name="kraj/meta-altera" path="layers/meta-altera" revision="cf7fc462cc6a5e82f2de76bb21e09675be7ae316"/>
        <project name="01org/meta-de10-nano" path="layers/meta-de10-nano" remote="github" revision="refs/tags/VERSION-2017.03.30"/>
</manifest> 
```

### Step 3: Fetch the repositories provided in the manifest
```
repo sync
```

### Step 4: Build
This will take a few hours. 
```
MACHINE=de10-nano . ./setup-environment
bitbake de10-nano-image
```

## What next?
The result of this lengthy build is an SDCard image that can be burned to allow the Terasic DE10-Nano Kit to boot Linux\*.  The image provides access via serial port, a graphical interface, USB, and Ethernet.  As part of the build, the recipes populate an FPGA image as well as the associated devicetrees.  

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
The SDCard image name in the above list is "Angstrom-de10-nano-image-glibc-ipk-v2016.12-de10-nano.rootfs.socfpga-sdimg".  Please remember that prebuilt images can be found [here](https://signin.intel.com/logout?target=https://software.intel.com/en-us/iot/hardware/fpga/de10-nano).

### Programming the SDCard image
These instructions only over Linux\*, for alternate instructions please go [here](https://software.intel.com/en-us/write-image-to-micro-sd-card).

These instructions use the dd command, I suggest using EXTREME CAUTION as it is trivial to overwrite the wrong device which can lead to data loss as well as hours spent rebuilding your machine.

The first step is to insert the SDCard using either a dedicated SDCard interface or a USB adapter into your machine.  I will leave it to you to discover which device this shows up as but it is ussually /dev/sdX or /dev/mmcblkX where X is the device number.  I use a dedicated SDCard interface which shows up as /dev/mmcblk0, for safety reasons these instructions will use /dev/mmcblkX as this should never be a real device.

It will take a few minutes to write the ~2GB image.
```
cd deploy/glibc/images/de10-nano/
sudo dd if=Angstrom-de10-nano-image-glibc-ipk-v2016.12-de10-nano.rootfs.socfpga-sdimg of=/dev/mmxblkX bs=1M && sync && sync
```

After this is complete, plug the card into the kit and power on the board.

 ## Additional Resources
* [Discover the Terasic DE10-Nano Kit](https://signin.intel.com/logout?target=https://software.intel.com/en-us/iot/hardware/fpga/de10-nano)
* [Terasic DE10-Nano Get Started Guide](https://software.intel.com/en-us/terasic-de10-nano-get-started-guide)
* [Project: My First FPGA](https://software.intel.com/en-us/articles/my-first-fpga)
* [Learn more about Intel® FPGAs](https://software.intel.com/en-us/iot/hardware/fpga/)

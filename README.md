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
repo init -u git://github.com/Angstrom-distribution/angstrom-manifest -b angstrom-v2018.06-sumo
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
  <project remote="github" name="kraj/meta-altera" path="layers/meta-altera" revision="786bee6f01287fb3427aa57996cfdf07d356dfc4" branch="master"/>
  <remove-project name="koenkooi/meta-photography" />                     
  <project name="feddischson/meta-de10-nano" path="layers/meta-de10-nano" remote="github" branch="sumo"/>
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

The build output is located in deploy/glibc/images/de10-nano/.

## What next?
The result of this lengthy build is an SDCard image that can be burned to allow the Terasic DE10-Nano Kit to boot Linux\*.  The image provides access via serial port, a graphical interface, USB, and Ethernet.  As part of the build, the recipes populate an FPGA image as well as the associated devicetrees.  

The build output is located in deploy/glibc/images/de10-nano/.

The SDCard image name is "Angstrom-de10-nano-image-glibc-ipk-v2018.06-de10-nano.rootfs.wic".  

**Caution**: These instructions use the dd command which should be used with EXTREME CAUTION. It is very easy to accidentally overwrite the wrong device which can lead to data loss as well as hours spent rebuilding your machine. 

The first step is to insert the SD Card using either a dedicated SD Card interface or a USB adapter into your machine. Discover which device this shows up. Usually the device shows up as /dev/sdX or /dev/mmcblkX where X is the device number. As an example, our dedicated SD Card interface shows up as /dev/mmcblk0.

**Note**: for safety reasons these instructions will use /dev/mmcblkX as this should never be a real device.

It will take a few minutes to write the image (~2GB).
```
cd deploy/glibc/images/de10-nano/
sudo dd if=Angstrom-de10-nano-image-glibc-ipk-v2018.06-de10-nano.rootfs.wic of=/dev/mmxblkX bs=1M && sync && sync
```

After this completes, insert the microSD card into the DE10-Nano board and then power it on.


## Minimal-Image
In addition to the de10-nano-image, there exists a minimal image which is created via
```
bitbake de10-nano-image-minimal
```
This image holds just a basic system without apps and webserver.
To further reduce the size, the image overhead factor could be reduced e.g. to 1.1:
```
echo 'IMAGE_OVERHEAD_FACTOR = "1.1"' >> conf/local.conf
```

## Extension and custom FPGA design

The following example shows shows how to use a custom FPGA design for the DE10-board:
```
# File: conf/local.conf



# The *.tgz file must hold the *.rbf, a device-tree-blob and licenses:
DE10_NANO_HW_SRC           = "file:///path/to/my_custom_DE10_system.tgz"
# MD5 of the *.tgz file:
DE10_NANO_HW_MD5           = "25f46aa9ceb2bb4f43bf5240e05132e7"

# Assuming, that all files are in the folder my_cystom_DE10_system, the following 
# is used to define the paths within the tgz
DE10_NANO_HW_DTBO_LIC_PATH = "my_custom_DE10_system/license_of_dtb.txt"
DE10_NANO_HW_DTBO_PATH     = "my_custom_DE10_system/de10_sys.dtb"
DE10_NANO_HW_RBF_LIC_PATH  = "my_custom_DE10_system/license_of_rbf.txt"
DE10_NANO_HW_RBF_PATH      = "my_custom_DE10_system/de10_sys.rbf"

# Checksum of the *.rbf file:
DE10_NANO_HW_LIC_FILES_CHKSUM = "\
	file://${WORKDIR}/my_custom_DE10_system/de10_sys.rbf;md5=f9f7b28f5ebafbdf17106fed80f43da2\
"

# In addition, a custom u-boot script can be used in order to adapt the initialization of the custom FPGA design.
DE10_BOOT_SCRIPT = "/path/to/my_boot_script.in"

# If further kernel modules are required, add them to a file and pass it via
DE10_NANO_EXTRA_KERNEL_CONFIG = "file://${TOPDIR}/conf/kernel_extra.cfg"

# In some cases, it is usedful to initialize something via shell script during bootup, 
# for example GPIOs via sys-interface.
# For this, a copy of recipes-misc/de10-nano-fpga-init/files/de10-nano-fpga-init.sh can be placed in config.
# The following entry considers it:
DE10_FPGA_INIT_SH     = "file://${TOPDIR}/conf/de10-nano-fpga-init.sh"


```



 ## Additional Resources
* [Discover the Terasic DE10-Nano Kit](https://signin.intel.com/logout?target=https://software.intel.com/en-us/iot/hardware/fpga/de10-nano)
* [Terasic DE10-Nano Get Started Guide](https://software.intel.com/en-us/terasic-de10-nano-get-started-guide)
* [Project: My First FPGA](https://software.intel.com/en-us/articles/my-first-fpga)
* [Learn more about Intel® FPGAs](https://software.intel.com/en-us/iot/hardware/fpga/)
* [DE10-Nano FPGA Hardware Project](https://github.com/01org/de10-nano-hardware)

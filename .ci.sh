#!/bin/bash

mkdir de10-nano-build
cd de10-nano-build
repo init -u git://github.com/Angstrom-distribution/angstrom-manifest -b angstrom-v2018.06-sumo
mkdir .repo/local_manifests
cat << EOF > .repo/local_manifests/de10-nano.xml
<?xml version="1.0" encoding="UTF-8"?>                                          
<manifest>
  <remove-project name="kraj/meta-altera" />                              
  <project remote="github" name="kraj/meta-altera" path="layers/meta-altera" revision="786bee6f01287fb3427aa57996cfdf07d356dfc4" branch="master"/>
  <remove-project name="koenkooi/meta-photography" />                     
  <project name="feddischson/meta-de10-nano" path="layers/meta-de10-nano" remote="github" branch="sumo"/>
</manifest>
EOF

sed -i '/meta-photography/d' .repo/manifests/conf/bblayers.conf
sed -i '/meta-altera/a \ \ \$\{TOPDIR\}\/layers\/meta-de10-nano \\' .repo/manifests/conf/bblayers.conf
repo sync
MACHINE=de10-nano . ./setup-environment
bitbake de10-nano-image
bitbake meta-toolchain


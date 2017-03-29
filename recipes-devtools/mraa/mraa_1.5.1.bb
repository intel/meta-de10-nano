require mraa.inc

SRCREV = "6f9b470d8d25e2c8ba1586cd9d707b870ab30010"
SRCREV_de10-nano = "81457f02c3b1bbccfc0a88be1af23335c9457da4"
SRCREV_de0-nano = "81457f02c3b1bbccfc0a88be1af23335c9457da4"

SRC_URI = "git://github.com/intel-iot-devkit/mraa.git;protocol=https"
SRC_URI_de10-nano = "git://github.com/Propanu/mraa.git;protocol=https;branch=de10-nano"
SRC_URI_de0-nano = "git://github.com/Propanu/mraa.git;protocol=https;branch=de10-nano"

PACKAGECONFIG_de10-nano ?= "python nodejs"
PACKAGECONFIG_de0-nano ?= "python nodejs"

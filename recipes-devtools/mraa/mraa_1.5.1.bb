require mraa.inc

SRCREV = "6f9b470d8d25e2c8ba1586cd9d707b870ab30010"
SRCREV_de10-nano = "5e34a5cd3be7514cca3e75971a1f271712e01d1c"
SRCREV_de0-nano = "5e34a5cd3be7514cca3e75971a1f271712e01d1c"

SRC_URI = "git://github.com/intel-iot-devkit/mraa.git;protocol=https"

PACKAGECONFIG_de10-nano ?= "python nodejs"
PACKAGECONFIG_de0-nano ?= "python nodejs"

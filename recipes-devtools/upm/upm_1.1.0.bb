SUMMARY = "Sensor/Actuator repository for Mraa"
SECTION = "libs"
AUTHOR = "Brendan Le Foll, Tom Ingleby, Yevgeniy Kiveisha"

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=66493d54e65bfc12c7983ff2e884f37f"

DEPENDS = "nodejs mraa"

SRCREV="b9988469ba5d353456d5a162ae807caa0475dbab"

SRC_URI = "git://github.com/intel-iot-devkit/upm.git \
          "

S = "${WORKDIR}/git"

inherit distutils-base pkgconfig python-dir cmake

PACKAGES =+ "python-${PN} node-${PN}"

# python-upm package containing Python bindings
FILES_python-${PN} = "${PYTHON_SITEPACKAGES_DIR}/ \
                      ${datadir}/${BPN}/examples/python/ \
                      ${prefix}/src/debug/${BPN}/${PV}-${PR}/build/src/*/pyupm_* \
                     "
RDEPENDS_python-${PN} += "python mraa"
INSANE_SKIP_python-${PN} = "debug-files"


# node-upm package containing Nodejs bindings
FILES_node-${PN} = "${libdir}/node_modules/ \
                    ${datadir}/${BPN}/examples/javascript/ \
                   "
RDEPENDS_node-${PN} += "nodejs mraa"
INSANE_SKIP_node-${PN} = "debug-files"

FILES_${PN}-doc += " ${datadir}/upm/examples/"
RDEPENDS_${PN} += " mraa"

PACKAGECONFIG ??= "python nodejs"
PACKAGECONFIG[python] = "-DBUILDSWIGPYTHON=ON, -DBUILDSWIGPYTHON=OFF, swig-native python python3,"
PACKAGECONFIG[nodejs] = "-DBUILDSWIGNODE=ON, -DBUILDSWIGNODE=OFF, swig-native nodejs,"
PACKAGECONFIG[java] = "-DBUILDSWIGJAVA=ON, -DBUILDSWIGJAVA=OFF, swig-native icedtea7-native,"


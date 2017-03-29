#!/bin/sh

CC_TEST="${CC:?}"
type -t ${CC%${CC#*gcc}} > /dev/null 2>&1 || {
	echo ""
	echo "ERROR: cross compiler tools are not visible in the environment."
	echo ""
	exit 1
}

THE_SYSROOT="$(${CC:?} -print-sysroot)"

[ -f "${THE_SYSROOT:?}${includedir:?}/NE10.h" ] || {
	echo ""
	echo "ERROR: cannot locate include file 'NE10.h'."
	echo "ERROR: '${THE_SYSROOT:?}${includedir:?}/NE10.h'"
	echo ""
	exit 1
}

[ -f "${THE_SYSROOT:?}${libdir:?}/libNE10.so" ] || {
	echo ""
	echo "ERROR: cannot locate library archive 'libNE10.so'."
	echo "ERROR: '${THE_SYSROOT:?}${libdir:?}/libNE10.so'"
	echo ""
	exit 1
}

[ "$#" -eq "1" ] || {
	echo ""
	echo "USAGE: ${0} <application name>"
	echo ""
	exit 1
}

[ -e "${1}" ] && {
	echo ""
	echo "${1} already exists"
	echo "please remove it and try again"
	echo "rm ${1}"
	echo ""
	exit 1
}

[ -e "${1}.c" ] || {
	echo ""
	echo "${1}.c does not exists"
	echo "cannot compile it"
	echo ""
	exit 1
}

${CC:?} \
	-march=armv7-a \
	-mfloat-abi=hard \
	-mfpu=neon \
	-mthumb-interwork \
	-mthumb \
	-O2 -D_FORTIFY_SOURCE=2 \
	-g \
	-feliminate-unused-debug-types  \
	-std=gnu99 \
	-W \
	-Wall \
	-Werror \
	-Wc++-compat \
	-Wwrite-strings \
	-Wstrict-prototypes \
	-Wformat -Wformat-security \
	-pedantic \
	-fstack-protector-strong \
	-z noexecstack \
	-z relro -z now \
	-o "${1}.o" \
	-c \
	"${1}.c"

[ "${1}.c" -nt "${1}.o" ] && {
	echo ""
	echo "${1}.c is newer than ${1}.o"
	echo "cannot link it"
	echo ""
	exit 1
}

${CXX:?} \
	-march=armv7-a \
	-mfloat-abi=hard \
	-mfpu=neon \
	-mthumb-interwork \
	-mthumb \
	-O2 -D_FORTIFY_SOURCE=2 \
	-Wformat -Wformat-security \
	-fstack-protector-strong \
	-z noexecstack \
	-z relro -z now \
	-g \
	"${1}.o" \
	-o "${1}" \
	-L. \
	-loverhead \
	-lNE10 \
	-lm


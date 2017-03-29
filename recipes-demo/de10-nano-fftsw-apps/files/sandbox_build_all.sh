#!/bin/sh

APP_LIST="
create_real_short_sine32
create_real_short_square32
create_real_short_triangle32
real_short_to_ne10cpx_long
real_short_to_ne10cpx_short
ne10cpx_long_to_text
fftdma_256
neon32_256
"

LIB_LIST="
overhead
"

type -t gcc > /dev/null 2>&1 || {
	echo ""
	echo "ERROR: cross compiler tools are not visible in the environment."
	echo ""
	exit 1
}

THE_SYSROOT="$(gcc -print-sysroot)"

[ -f "${THE_SYSROOT:?}/usr/include/NE10.h" ] || {
        echo ""
        echo "ERROR: cannot locate include file 'NE10.h'."
        echo "ERROR: '${THE_SYSROOT:?}/usr/include/NE10.h'"
        echo ""
        exit 1
}

[ -f "${THE_SYSROOT:?}/usr/lib/libNE10.so" ] || {
        echo ""
        echo "ERROR: cannot locate library archive 'libNE10.so'."
        echo "ERROR: '${THE_SYSROOT:?}/usr/lib/libNE10.so'"
        echo ""
        exit 1
}

for NEXT in ${LIB_LIST}
do
	./target_build_lib.sh "${NEXT}"
done

for NEXT in ${APP_LIST}
do
	./target_build_app.sh "${NEXT}"
done


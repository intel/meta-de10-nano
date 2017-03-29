#!/bin/sh

COPY_LIST="
LICENSE
create_real_short_sine32.c
create_real_short_square32.c
create_real_short_triangle32.c
fftdma_256.c
ne10cpx_long_to_text.c
neon32_256.c
overhead.c
real_short_to_ne10cpx_long.c
real_short_to_ne10cpx_short.c
README_SANDBOX_BUILD.TXT
README_SANDBOX.TXT
clean_all.sh
sandbox_create_input_waveforms.sh
duplicate_x8.sh
sandbox_build_all.sh
target_build_app.sh
target_build_lib.sh
"

for NEXT in ${COPY_LIST}
do
	[ -f "${NEXT}" ] || {
		echo "File: '${NEXT}' in copy list was not found."
		exit 1
	}
done

tar czf fft_sandbox.tgz ${COPY_LIST}


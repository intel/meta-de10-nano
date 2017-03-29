#!/bin/sh

COPY_LIST="
README_TARGET.TXT
fft.tgz
setup_target_fft_env.sh
"

for NEXT in ${COPY_LIST}
do
	[ -f "${NEXT}" ] || {
		echo "File: '${NEXT}' in copy list does not exist."
		exit 1
	}

	scp -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null ${NEXT} root@192.168.7.1:/home/root/fft 2> /dev/null || {
		echo "ERROR: while copying file '${NEXT}' to target."
		exit 1
	}
done

exit 0


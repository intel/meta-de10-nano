#!/bin/sh

[ $# -eq "1" ] || {
	echo "" 1>&2
	echo "USAGE: ${0} <input file>" 1>&2
	echo "" 1>&2
	exit 1
}

[ -f "${1}" ] || {
	echo "" 1>&2
	echo "ERROR: file does not exist: '${1}'" 1>&2
	echo "" 1>&2
	exit 1
}

for I in {1..128}
do
	cat "${1}"
done

exit 0


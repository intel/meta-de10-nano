#!/bin/sh

WAVEFORM_LIST="
sine
square
triangle
"
cd $(dirname ${0})

[ -d input_waveforms ] && {
	echo ""
	echo "ERROR: directory '$(pwd)/input_waveforms' already exists, please delete before running"
	echo ""
	exit 1
}

mkdir input_waveforms || {
	echo ""
	echo "ERROR: could not create directory '$(pwd)/input_waveforms'"
	echo ""
	exit 1
}

for WAVEFORM in ${WAVEFORM_LIST}
do
	echo "Creating base waveforms for ${WAVEFORM} wave..."
	./create_real_short_${WAVEFORM}32 > input_waveforms/real_short_${WAVEFORM}32.bin
	./duplicate_x8.sh   input_waveforms/real_short_${WAVEFORM}32.bin  > input_waveforms/real_short_${WAVEFORM}256.bin
	echo "Creating ne10cpx_short waveforms for ${WAVEFORM} wave..."
	cat input_waveforms/real_short_${WAVEFORM}256.bin    | ./real_short_to_ne10cpx_short > input_waveforms/ne10cpx_short_${WAVEFORM}256.bin
	echo "Creating ne10cpx_long waveforms for ${WAVEFORM} wave..."
	cat input_waveforms/real_short_${WAVEFORM}256.bin    | ./real_short_to_ne10cpx_long > input_waveforms/ne10cpx_long_${WAVEFORM}256.bin
	rm input_waveforms/real_short_${WAVEFORM}32.bin
	rm input_waveforms/real_short_${WAVEFORM}256.bin
done

[ -d output_waveforms ] && {
	exit 0
}

mkdir output_waveforms || {
	echo ""
	echo "ERROR: could not create directory '$(pwd)/output_waveforms'"
	echo ""
	exit 1
}

exit 0


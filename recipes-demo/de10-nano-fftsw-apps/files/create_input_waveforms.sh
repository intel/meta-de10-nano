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
	./duplicate_x32.sh  input_waveforms/real_short_${WAVEFORM}256.bin > input_waveforms/real_short_${WAVEFORM}256x32.bin
	./duplicate_x128.sh input_waveforms/real_short_${WAVEFORM}32.bin  > input_waveforms/real_short_${WAVEFORM}4096.bin
	./duplicate_x32.sh  input_waveforms/real_short_${WAVEFORM}32.bin  > input_waveforms/real_short_${WAVEFORM}1K.bin
	./duplicate_x32.sh  input_waveforms/real_short_${WAVEFORM}1K.bin  > input_waveforms/real_short_${WAVEFORM}32K.bin
	./duplicate_x32.sh  input_waveforms/real_short_${WAVEFORM}32K.bin > input_waveforms/real_short_${WAVEFORM}1M.bin
	rm input_waveforms/real_short_${WAVEFORM}1K.bin
	rm input_waveforms/real_short_${WAVEFORM}32K.bin
	echo "Creating ne10cpx_short waveforms for ${WAVEFORM} wave..."
	cat input_waveforms/real_short_${WAVEFORM}256.bin    | ./real_short_to_ne10cpx_short > input_waveforms/ne10cpx_short_${WAVEFORM}256.bin
	cat input_waveforms/real_short_${WAVEFORM}256x32.bin | ./real_short_to_ne10cpx_short > input_waveforms/ne10cpx_short_${WAVEFORM}256x32.bin
	cat input_waveforms/real_short_${WAVEFORM}4096.bin     | ./real_short_to_ne10cpx_short > input_waveforms/ne10cpx_short_${WAVEFORM}4096.bin
	cat input_waveforms/real_short_${WAVEFORM}1M.bin     | ./real_short_to_ne10cpx_short > input_waveforms/ne10cpx_short_${WAVEFORM}1M.bin
	echo "Creating ne10cpx_long waveforms for ${WAVEFORM} wave..."
	cat input_waveforms/real_short_${WAVEFORM}256.bin    | ./real_short_to_ne10cpx_long > input_waveforms/ne10cpx_long_${WAVEFORM}256.bin
	cat input_waveforms/real_short_${WAVEFORM}256x32.bin | ./real_short_to_ne10cpx_long > input_waveforms/ne10cpx_long_${WAVEFORM}256x32.bin
	cat input_waveforms/real_short_${WAVEFORM}4096.bin     | ./real_short_to_ne10cpx_long > input_waveforms/ne10cpx_long_${WAVEFORM}4096.bin
	cat input_waveforms/real_short_${WAVEFORM}1M.bin     | ./real_short_to_ne10cpx_long > input_waveforms/ne10cpx_long_${WAVEFORM}1M.bin
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


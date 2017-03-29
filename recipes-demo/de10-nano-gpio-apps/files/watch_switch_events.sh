#$/bin/sh

#open the input event device for our gpio-keys
exec 7<>/dev/input/by-path/platform-soc:base-fpga-region:keys-event

# start a subshell with the output from the device file as its input
(
	# loop forever
	while [ true ]
	do

	# get the next 16 byte event data, then extract the three hex strings
	# for type, code and value in little endian form and convert each value
	# to a decimal representation
	SAMPLE="$(dd bs=16 count=1 2> /dev/null | hexdump -v -e '16/1 "%02x "' -e '"\n"' | sed -e "s/.*\s.*\s.*\s.*\s.*\s.*\s.*\s.*\s\(.*\)\s\(.*\)\s\(.*\)\s\(.*\)\s\(.*\)\s\(.*\)\s\(.*\)\s\(.*\)/0x\2\1 0x\4\3 0x\8\7\6\5/")"
	SAMPLE_TYPE="$(printf "%d" "$(echo ${SAMPLE} | cut -d ' ' -f 1)")"
	SAMPLE_CODE="$(printf "%d" "$(echo ${SAMPLE} | cut -d ' ' -f 2)")"
	SAMPLE_VALUE="$(printf "%d" "$(echo ${SAMPLE} | cut -d ' ' -f 3)")"

	# if the sample type is EV_KEY, then process it, otherwise ignore it
	[ "${SAMPLE_TYPE}" -eq "1" ] && {

		# look for the codes that we expect
		case "${SAMPLE_CODE}" in
			64)
				echo -n "SW0"
				;;
			65)
				echo -n "SW1"
				;;
			66)
				echo -n "SW2"
				;;
			67)
				echo -n "SW3"
				;;
			*)
				echo -n "unknown"
				;;
		esac

		# output the value of the event
		echo " moved to value '${SAMPLE_VALUE}'"
	}
	done
) <&7


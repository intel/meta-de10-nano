#!/bin/sh

cd $(dirname ${0})

echo "Executing from directory '$(pwd)'."

[ -d '/sys/class/misc/fft' ] || {
	echo "Attempting to install 'fft_driver'..."
	modprobe fft_driver > /dev/null 2>&1 || {
		echo ""
		echo "ERROR: while attempting 'modprobe fft_driver'"
		echo "Exiting script due to error..."
		echo ""
		exit 1
	}

	sleep 1

	[ -d "/sys/class/misc/fft" ] || {
		echo ""
		echo "ERROR: fft_driver presence is not detected after modprobe..."
		echo "Exiting script due to error..."
		echo ""
		exit 1
	}
}

echo "FFT driver appears to be installed."

[ -d '/mnt' ] || {
	echo "Attempting to create directory '/mnt'..."
	mkdir /mnt > /dev/null 2>&1 || {
		echo ""
		echo "ERROR: while attempting 'mkdir /mnt'"
		echo "Exiting script due to error..."
		echo ""
		exit 1
	}
}

echo "Directory '/mnt' exists."

[ -d '/mnt/ram' ] || {
	echo "Attempting to create directory '/mnt/ram'..."
	mkdir /mnt/ram > /dev/null 2>&1 || {
		echo ""
		echo "ERROR: while attempting 'mkdir /mnt/ram'"
		echo "Exiting script due to error..."
		echo ""
		exit 1
	}
}

echo "Directory '/mnt/ram' exists."

MNT_RAM_TYPE="$(stat -f -c '%T' /mnt/ram)"
[ ${MNT_RAM_TYPE} == "tmpfs" ] || {
	echo "Attempting to mount 'tmpfs' on '/mnt/ram'..."
	mount -t tmpfs -o size=100m tmpfs /mnt/ram > /dev/null 2>&1 || {
		echo ""
		echo "ERROR: while attempting 'mount -t tmpfs -o size=1m tmpfs /mnt/ram'"
		echo "Exiting script due to error..."
		echo ""
		exit 1
	}

	MNT_RAM_TYPE="$(stat -f -c '%T' /mnt/ram)"
	[ ${MNT_RAM_TYPE} == "tmpfs" ] || {
		echo ""
		echo "ERROR: tmpfs file system not detected after mount at '/mnt/ram'"
		echo "Exiting script due to error..."
		echo ""
		exit 1
	}
}

echo "tmpfs appears to be mounted at '/mnt/ram'."

[ -d '/mnt/ram/fft' ] && {
	echo ""
	echo "ERROR: directory already exists '/mnt/ram/fft', please delete before proceeding."
	echo "You could use this 'rm -rf /mnt/ram/fft' to delete the directory."
	echo "Exiting script due to error..."
	echo ""
	exit 1
}

echo "Directory '/mnt/ram/fft' does not exist."

mkdir /mnt/ram/fft > /dev/null 2>&1 || {
	echo ""
	echo "ERROR: while attempting 'mkdir /mnt/ram/fft'"
	echo "Exiting script due to error..."
	echo ""
	exit 1
}

echo "Created directory '/mnt/ram/fft'."

[ -f './fft.tgz' ] || {
	echo ""
	echo "ERROR: unable to locate file '$(pwd)/fft.tgz'"
	echo "Exiting script due to error..."
	echo ""
	exit 1
}

echo "File './fft.tgz' exists."

cp './fft.tgz' /mnt/ram/fft > /dev/null 2>&1 || {
	echo ""
	echo "ERROR: while attempting 'cp './fft.tgz' /mnt/ram/fft'"
	echo "Exiting script due to error..."
	echo ""
	exit 1
}

echo "Copied './fft.tgz' to '/mnt/ram/fft'."

cd '/mnt/ram/fft' > /dev/null 2>&1 || {
	echo ""
	echo "ERROR: while attempting 'cd '/mnt/ram/fft''"
	echo "Exiting script due to error..."
	echo ""
	exit 1
}

echo "Changed into directory '/mnt/ram/fft'."

tar xf 'fft.tgz' > /dev/null 2>&1 || {
	echo ""
	echo "ERROR: while attempting 'tar xf 'fft.tgz''"
	echo "Exiting script due to error..."
	echo ""
	exit 1
}

echo "Extracted archive 'fft.tgz'."

echo "Attempting to execute './create_input_waveforms.sh'."

./create_input_waveforms.sh > /dev/null 2>&1 || {
	echo ""
	echo "ERROR: while attempting './create_input_waveforms.sh'"
	echo "Exiting script due to error..."
	echo ""
	exit 1
}

echo "Executed './create_input_waveforms.sh'."

echo "Attempting to execute './run_all.sh'."

./run_all.sh > /dev/null 2>&1 || {
	echo ""
	echo "ERROR: while attempting './run_all.sh'"
	echo "Exiting script due to error..."
	echo ""
	exit 1
}

echo "Executed './run_all.sh'."

echo "Target setup complete."

exit 0


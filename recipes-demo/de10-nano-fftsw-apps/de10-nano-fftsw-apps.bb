SUMMARY = "Example application for DE-Nano FPGA FFT Benchmarking"
SECTION = "DE10-Nano"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=d252b8b27e3e6ea89c9c6912b3c4373d"
DEPENDS = "ne10"
RDEPENDS_${PN} = "ne10"

PACKAGE_ARCH = "${MACHINE_ARCH}"

PR = "r0"
PV = "1.0"

inherit systemd

SRC_URI = " \
	file://archive_for_target.sh \
	file://archive_src_for_sandbox.sh \
	file://archive_src_for_target.sh \
	file://build_all.sh \
	file://build_app.sh \
	file://build_lib.sh \
	file://c16_256.c \
	file://c16_256x32.c \
	file://c16_256x32x128.c \
	file://c16_4096.c \
	file://c32_256.c \
	file://c32_256x32.c \
	file://c32_256x32x128.c \
	file://c32_4096.c \
	file://clean_all.sh \
	file://LICENSE \
	file://copy_to_target.sh \
	file://create_input_waveforms.sh \
	file://create_real_short_sine32.c \
	file://create_real_short_square32.c \
	file://create_real_short_triangle32.c \
	file://de10-nano-fftsw-init.service \
	file://duplicate_x128.sh \
	file://duplicate_x32.sh \
	file://duplicate_x8.sh \
	file://fft_256.c \
	file://fft_256x32.c \
	file://fft_256x32x128.c \
	file://fft_4096.c \
	file://fftdma_256.c \
	file://fftdma_256x32.c \
	file://fftdma_256x32x128.c \
	file://fftdma_4096.c \
	file://images/create_gnuplot_sine_256.gnu \
	file://images/create_gnuplot_sine_256x32.gnu \
	file://images/create_gnuplot_sine_4096.gnu \
	file://images/create_gnuplot_squares_256.gnu \
	file://images/create_gnuplot_squares_256x32.gnu \
	file://images/create_gnuplot_squares_4096.gnu \
	file://images/create_gnuplot_triangle_256.gnu \
	file://images/create_gnuplot_triangle_256x32.gnu \
	file://images/create_gnuplot_triangle_4096.gnu \
	file://images/create_montage_sine_256.sh \
	file://images/create_montage_sine_256x32.sh \
	file://images/create_montage_sine_4096.sh \
	file://images/create_montage_squares_256.sh \
	file://images/create_montage_squares_256x32.sh \
	file://images/create_montage_squares_4096.sh \
	file://images/create_montage_triangle_256.sh \
	file://images/create_montage_triangle_256x32.sh \
	file://images/create_montage_triangle_4096.sh \
	file://images/README_IMAGES.TXT \
	file://ne10cpx_long_to_text.c \
	file://ne10cpx_short_to_text.c \
	file://neon16_256.c \
	file://neon16_256x32.c \
	file://neon16_256x32x128.c \
	file://neon16_4096.c \
	file://neon32_256.c \
	file://neon32_256x32.c \
	file://neon32_256x32x128.c \
	file://neon32_4096.c \
	file://overhead.c \
	file://README_HOST.TXT \
	file://README_SANDBOX_BUILD.TXT \
	file://README_SANDBOX.TXT \
	file://README_TARGET_BUILD.TXT \
	file://README_TARGET.TXT \
	file://real_short_to_ne10cpx_long.c \
	file://real_short_to_ne10cpx_short.c \
	file://run_all.sh \
	file://run_fft_256.sh \
	file://run_fft_256x32.sh \
	file://run_fft_256x32x128.sh \
	file://run_fft_4096.sh \
	file://run_stream_256x16x1.sh \
	file://run_stream_256x1x1.sh \
	file://run_stream_256x32x128.sh \
	file://sandbox_build_all.sh \
	file://sandbox_create_input_waveforms.sh \
	file://setup_env.src \
	file://setup_target_fft_env.sh \
	file://stream_fpga_256x16x1.c \
	file://stream_fpga_256x1x1.c \
	file://stream_fpga_256x32x128.c \
	file://stream_neon32_256x16x1.c \
	file://stream_neon32_256x1x1.c \
	file://stream_neon32_256x32x128.c \
	file://stream_raw_256x16x1.c \
	file://stream_raw_256x1x1.c \
	file://stream_raw_256x32x128.c \
	file://target_build_all.sh \
	file://target_build_app.sh \
	file://target_build_lib.sh \
"

S = "${WORKDIR}"

do_configure () {
	cd ${S}
	bash clean_all.sh
}

do_compile () {
	cd ${S}
	bash build_all.sh
}

do_install () {

	install -d ${D}${base_libdir}/systemd/system
	install -m 0644 ${WORKDIR}/*.service ${D}${base_libdir}/systemd/system
	
	cd ${S}

	install -d ${D}/examples/fft/bin
	bash archive_for_target.sh
	cp -a --no-preserve=ownership fft.tgz ${D}/examples/fft/bin/
	cp -a --no-preserve=ownership README_TARGET.TXT ${D}/examples/fft/bin/
	cp -a --no-preserve=ownership setup_target_fft_env.sh ${D}/examples/fft/bin/

	install -d ${D}/examples/fft/src
	bash archive_src_for_target.sh
	cp -a --no-preserve=ownership fft_src.tgz ${D}/examples/fft
	tar  --no-same-owner -xf fft_src.tgz -C ${D}/examples/fft/src

	install -d ${D}/examples/fft/sandbox
	bash archive_src_for_sandbox.sh
	cp -a --no-preserve=ownership fft_sandbox.tgz ${D}/examples/fft
	tar  --no-same-owner -xf fft_sandbox.tgz -C ${D}/examples/fft/sandbox
}

FILES_${PN} += "examples/fft/bin/"

RDEPENDS_${PN} += "gnuplot"
FILES_${PN} += "examples/fft/fft_src.tgz"
FILES_${PN} += "examples/fft/fft_sandbox.tgz"
FILES_${PN} += "examples/fft/src/"
FILES_${PN} += "examples/fft/sandbox/"

NATIVE_SYSTEMD_SUPPORT = "1"
SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE_${PN} = "de10-nano-fftsw-init.service"


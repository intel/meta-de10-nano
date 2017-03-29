After you have built the FFT example programs in the src directory and run the
'setup_target_fft_env.sh' script to install them into the ramdisk you can change
into the 'output_waveforms' directory:

root@DE10-Nano:/examples/fft/src# cd /mnt/ram/fft/output_waveforms/

There should be no PNG image files in the directory at this point.

output_waveforms# ls *.png
ls: *.png: No such file or directory

To create the PNG images for the output data from each FFT example program you
can run each of the 'create_gnuplot_*.gnu' scripts like this:

output_waveforms# find /examples/fft/src/images/ -name "create_gnuplot_*.gnu" \
			-exec \{\} \;

Now there should be an image file for each output data file:

output_waveforms# ls *.png
c16_256_sine.png            fftdma_256_sine.png
c16_256_square.png          fftdma_256_square.png
c16_256_triangle.png        fftdma_256_triangle.png
c16_256x32_sine.png         fftdma_256x32_sine.png
c16_256x32_square.png       fftdma_256x32_square.png
c16_256x32_triangle.png     fftdma_256x32_triangle.png
c16_4096_sine.png           fftdma_4096_sine.png
c16_4096_square.png         fftdma_4096_square.png
c16_4096_triangle.png       fftdma_4096_triangle.png
c32_256_sine.png            neon16_256_sine.png
c32_256_square.png          neon16_256_square.png
c32_256_triangle.png        neon16_256_triangle.png
c32_256x32_sine.png         neon16_256x32_sine.png
c32_256x32_square.png       neon16_256x32_square.png
c32_256x32_triangle.png     neon16_256x32_triangle.png
c32_4096_sine.png           neon16_4096_sine.png
c32_4096_square.png         neon16_4096_square.png
c32_4096_triangle.png       neon16_4096_triangle.png
fft_256_sine.png            neon32_256_sine.png
fft_256_square.png          neon32_256_square.png
fft_256_triangle.png        neon32_256_triangle.png
fft_256x32_sine.png         neon32_256x32_sine.png
fft_256x32_square.png       neon32_256x32_square.png
fft_256x32_triangle.png     neon32_256x32_triangle.png
fft_4096_sine.png           neon32_4096_sine.png
fft_4096_square.png         neon32_4096_square.png
fft_4096_triangle.png       neon32_4096_triangle.png

Now to package these image files up into a montage of each group of output we
can run the 'create_montage_*.sh' scripts like this:

output_waveforms# find /examples/fft/src/images/ -name "create_montage_*.sh" \
			-exec \{\} \;

Now we should see the montage images in the directory as well.

output_waveforms# ls *.png
c16_256_sine.png            fftdma_256x32_triangle.png
c16_256_square.png          fftdma_4096_sine.png
c16_256_triangle.png        fftdma_4096_square.png
c16_256x32_sine.png         fftdma_4096_triangle.png
c16_256x32_square.png       neon16_256_sine.png
c16_256x32_triangle.png     neon16_256_square.png
c16_4096_sine.png           neon16_256_triangle.png
c16_4096_square.png         neon16_256x32_sine.png
c16_4096_triangle.png       neon16_256x32_square.png
c32_256_sine.png            neon16_256x32_triangle.png
c32_256_square.png          neon16_4096_sine.png
c32_256_triangle.png        neon16_4096_square.png
c32_256x32_sine.png         neon16_4096_triangle.png
c32_256x32_square.png       neon32_256_sine.png
c32_256x32_triangle.png     neon32_256_square.png
c32_4096_sine.png           neon32_256_triangle.png
c32_4096_square.png         neon32_256x32_sine.png
c32_4096_triangle.png       neon32_256x32_square.png
fft_256_sine.png            neon32_256x32_triangle.png
fft_256_square.png          neon32_4096_sine.png
fft_256_triangle.png        neon32_4096_square.png
fft_256x32_sine.png         neon32_4096_triangle.png
fft_256x32_square.png       sines_256.png
fft_256x32_triangle.png     sines_256x32.png
fft_4096_sine.png           sines_4096.png
fft_4096_square.png         squares_256.png
fft_4096_triangle.png       squares_256x32.png
fftdma_256_sine.png         squares_4096.png
fftdma_256_square.png       triangles_256.png
fftdma_256_triangle.png     triangles_256x32.png
fftdma_256x32_sine.png      triangles_4096.png
fftdma_256x32_square.png

Now to view these images you can do one of two things.

If you are running a VNC session into the DE10-Nano target, then from the command
prompt that you just ran the above commands from you can type 'display <image>'
to run the Imagemagick file viewer.  Otherwise, if you'd like to view these
image files from a web browser on your host development, then copy the images
into the '/usr/share/webcontent/images/' directory on the DE10-Nano target.
Then from a web browser on your development host you can point your web browser
to '<DE10-Nano-ip-address>/images/<image>'.


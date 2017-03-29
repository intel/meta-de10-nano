To build the applications in this directory, please follow the guidance provided
at the top of the README_SANDBOX_BUILD.TXT file.

Once you have built the applications in this directory you can follow these
directions to make use of them.

We want to run the 'fftdma_256' program and the 'neon32_256' program that we
just built.  These programs require some input waveform data to feed into the
FFT algorithm and they output the real and imaginary FFT results.  To create the
input waveform data we can run this script:

sandbox# ./sandbox_create_input_waveforms.sh

The 'sandbox_create_input_waveforms.sh' script runs a few of the utiltiy
programs that we just build along with some of the utility scripts to create
some input waveform patterns that we can feed into the 'fftdma_256' and
'neon32_256' programs.  After we run this script we should see the following
output that it created:

sandbox# ls input_waveforms/ output_waveforms/
input_waveforms/:
ne10cpx_long_sine256.bin       ne10cpx_short_sine256.bin
ne10cpx_long_square256.bin     ne10cpx_short_square256.bin
ne10cpx_long_triangle256.bin   ne10cpx_short_triangle256.bin

output_waveforms/:

The 'fftdma_256' program consumes 16-bit sample inputs which is what the
'ne10cpx_short_*.bin' files contain, and the 'neon32_256' program consumes
32-bit sample inputs which is what the 'ne10cpx_long_*.bin' files contain.

Now we're ready to run the programs.  Execute each program like this:

sandbox# ./fftdma_256 \
	--input=input_waveforms/ne10cpx_short_sine256.bin \
	--output=output_waveforms/fftdma_256_sine.bin

sandbox# ./neon32_256 \
	--input=input_waveforms/ne10cpx_long_sine256.bin \
	--output=output_waveforms/neon32_256_sine.bin

And now we should see the raw binary output from the two program executions,
like this:

sandbox# ls output_waveforms/
fftdma_256_sine.bin  neon32_256_sine.bin

You can see the raw data by dumping it with 'hexdump -Cv', but if we really want
to "see" the data, we could also convert it into a PNG image file that we can
view with a web browser.

To create a PNG image out of the output results, first we need to convert the
binary data into a text file that 'gnuplot' can digest.  We have previously
compiled a utility program that can help us do that, the 'ne10cpx_long_to_text'
utility.  We can convert the two output data files like this:

sandbox# cat output_waveforms/fftdma_256_sine.bin | \
	./ne10cpx_long_to_text > output_waveforms/fftdma_256_sine.txt

sandbox# cat output_waveforms/neon32_256_sine.bin | \
	./ne10cpx_long_to_text > output_waveforms/neon32_256_sine.txt

And now we should see the binary output files along with the converted text
files in the output directory:
	
sandbox# ls output_waveforms/
fftdma_256_sine.bin  neon32_256_sine.bin
fftdma_256_sine.txt  neon32_256_sine.txt

Next we can have gnuplot plot the data from the text files like this:

sandbox# gnuplot -d -e '
set terminal pngcairo background "#FFFFFF" fontscale 1.0 size 1024, 768;
set output "output_waveforms/fftdma_256_sine.png";
set title "fftdma_256_sine";
plot for [i=1:2] "output_waveforms/fftdma_256_sine.txt"
using i with line title columnhead;'

sandbox# gnuplot -d -e '
set terminal pngcairo background "#FFFFFF" fontscale 1.0 size 1024, 768;
set output "output_waveforms/neon32_256_sine.png";
set title "neon32_256_sine";
plot for [i=1:2] "output_waveforms/neon32_256_sine.txt"
using i with line title columnhead;'

And now we should see the PNG files in the output directory along with the
binary and text files:

sandbox# ls output_waveforms/
fftdma_256_sine.bin  fftdma_256_sine.txt  neon32_256_sine.png
fftdma_256_sine.png  neon32_256_sine.bin  neon32_256_sine.txt

Let's convert the input waveform into a PNG so that we can "see" that as well.
We follow the same steps as above.

sandbox# cat input_waveforms/ne10cpx_long_sine256.bin | \
	./ne10cpx_long_to_text > input_waveforms/ne10cpx_long_sine256.txt

sandbox# gnuplot -d -e '
set terminal pngcairo background "#FFFFFF" fontscale 1.0 size 1024, 768;
set output "input_waveforms/ne10cpx_long_sine256.png" ;
set title "input_256_sine" ;
plot for [i=1:2] "input_waveforms/ne10cpx_long_sine256.txt"
using i with line title columnhead ;'

Now lets put all the images together into one large montage.

sandbox# montage \
	-background '#000000' \
	-font Liberation-Mono-Regular \
	"input_waveforms/ne10cpx_long_sine256.png" \
	"output_waveforms/neon32_256_sine.png" \
	"output_waveforms/fftdma_256_sine.png" \
	-tile 1x3 \
	-geometry +5+5 \
	sines_256.png

VIEW MONTAGE IMAGE IN WEB BROWSER
To view this montage in a web browser, we can copy it over to the images
directory that the web server is pulling from on the DE10-Nano target system.
That directory is '/usr/share/webcontent/images', so we can copy the montage
file like this:

sandbox# cp \
	sines_256.png \
	/usr/share/webcontent/images/

To view the montage, we need to start a web browser on our host machine and
surf to the image on the Altas target, the montage will be visible at the
following URL:

<DE10-Nano-ip-address>/images/sines_256.png

VIEW MONTAGE IMAGE IN VNC SESSION
To view this montage in a VNC session, simply run the 'display' image viewer
application:

sandbox# display sines_256.png


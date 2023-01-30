# NTSC-CRT
NTSC video signal encoding / decoding emulation by EMMIR 2018-2023
================================================================

### Click the image to see a YouTube video of it running in real time:
[![alt text](/scube.png?raw=true)](https://www.youtube.com/watch?v=ucfPRtV6--c)
### Click the image to see a YouTube video of it running in real time:
[![alt text](/kc.png?raw=true)](https://www.youtube.com/watch?v=ucfPRtV6--c)
### Example of artifact colors being used purposely by specially designed art (not my own)
![alt text](/artifactcolor.png?raw=true)
### Example of artifact colors creating a rainbow by specially designed art (not my own)
![alt text](/rainbow.png?raw=true)

### Example of dot crawl in a standard interlaced NTSC video
https://user-images.githubusercontent.com/109979235/212872851-38d0558e-b4bb-4011-8090-046e585093c9.mov


### NES mode  
YouTube video of the filter running in an NES emulator:
https://www.youtube.com/watch?v=giML77yy7To

### Description
The result of going down a very deep rabbit hole.
I learned a lot about analog signal processing, television, and the NTSC standard in the process.
Written to be compatible with C89.

Just like King's Crook (my from-scratch 3D game), this code follows the same restrictions:

1. Everything must be done in software, no explicit usage of hardware acceleration.
2. No floating point types or literals, everything must be integer only.
3. No 3rd party libraries, only C standard library and OS libraries for window, input, etc.
4. No languages used besides C.
5. No compiler specific features and no SIMD.
6. Single threaded.

This program performs relatively well and can be easily used in real-time applications
to emulate NTSC output. It is by no means fully optimized (mainly for readability), so  
there is a lot of room for performance gains.

------
### Feature List:

- Relatively accurate composite NTSC image output  
  -- with bandlimited luma/chroma  
  -- color artifacts (extends to being able to show specially patterned b/w images as color)  
  -- accurate dot crawl  
- VSYNC and HSYNC
- Signal noise (optional)
- Interlaced and progressive scan
- Monochrome and full color
- Vertically aligned chroma OR checkerboard chroma OR sawtoothed chroma (specified in #define in crt_core.h)
- NES decoding support

## Important
The command line program provided does not let you mess with all the settings
like black/white point, hue, brightness, saturation, and contrast.

In the crt_main.c file, there are two main()'s.
One is for a command line program and the other uses my FW library (found here https://github.com/LMP88959/PL3D-KC)
to provide real-time NTSC emulation with adjustable parameters.

The famous waterfall 'rainbow' effect created as a result of dithering will show if it is compiled with `CRT_CHROMA_PATTERN` set to 0.
Specially patterned black and white images can be encoded/decoded with color just like a real composite NTSC display.

## Regarding NES Mode

NES Mode is a separate version of NTSC-CRT that has adjusted timings to match the non-standard NES specifications.  
https://www.nesdev.org/wiki/NTSC_video  
These timings and extra NES specific features were incorporated into the NES version by Persune.  
Extra features include the NES-specific NTSC frame pulses, dot skipping every odd frame, and border colors.

#### `Massive thank you to Persune for helping improve the NES version!`

## Compiling

```sh
cd NTSC-CRT

cc -O3 -o ntsc *.c
```

or using CMake on Linux, macOS, or Windows:

```sh
cmake -B build
cmake --build build
build/ntsc
```

The default command line takes a single PPM or BMP image file and outputs a processed PPM or BMP file:

```
usage: ./ntsc -m|o|f|p|r|h|a outwidth outheight noise artifact_hue infile outfile
sample usage: ./ntsc -op 640 480 24 0 in.ppm out.ppm
sample usage: ./ntsc - 832 624 0 90 in.ppm out.ppm
-- NOTE: the - after the program name is required
	artifact_hue is [0, 359]
------------------------------------------------------------
	m : monochrome
	o : do not prompt when overwriting files
	f : odd field (only meaningful in progressive mode)
	p : progressive scan (rather than interlaced)
	r : raw image (needed for images that use artifact colors)
	a : save analog signal as image instead of decoded image
	h : print help

by default, the image will be full color, interlaced, and scaled to the output dimensions
```

There is also the option of "live" rendering to a video window from an input PPM/BMP image file:

```sh
cmake -B build -Dlive=on
cmake --build build
build/ntsc my.ppm
```

### Adding NTSC-CRT to your C/C++ project:

Global variables:
```c
#include "crt_core.h"

static struct CRT crt;
static struct NTSC_SETTINGS ntsc;
static int color = 1;
static int noise = 12;
static int field = 0;
static int raw = 0;
static int hue = 0;
```

In your initialization function:
```c
/* pass it the buffer to be drawn on screen */
crt_init(&crt, screen_width, screen_height, screen_buffer);
/* specify some settings */
crt.blend = 1;
crt.scanlines = 1;

```

In your drawing loop:
```c
ntsc.rgb = video_buffer; /* buffer from your rendering */
ntsc.w = video_width;
ntsc.h = video_height;
ntsc.as_color = color;
ntsc.field = field & 1;
ntsc.raw = raw;
ntsc.hue = hue;
if (ntsc.field == 0) {
  ntsc.frame ^= 1;
}
crt_modulate(&crt, &ntsc);
crt_demodulate(&crt, noise);
field ^= 1;
```

### Other Information
Web version by @binji (might not be up to date):  
https://binji.github.io/NTSC-CRT/  
To use the web version, drag a PPM image into the web browser.

If you have any questions feel free to leave a comment on YouTube OR
join the King's Crook Discord server :)

YouTube: https://www.youtube.com/@EMMIR_KC/videos

Discord: https://discord.gg/hdYctSmyQJ

itch.io: https://kingscrook.itch.io/kings-crook

## License
Feel free to use the code in any way you would like, however, if you release anything with it,
a comment in your code/README saying where you got this code would be a nice gesture but it’s not mandatory.

The software is provided "as is", without warranty of any kind, express or implied,
including but not limited to the warranties of merchantability,
fitness for a particular purpose and noninfringement.
In no event shall the authors or copyright holders be liable for any claim,
damages or other liability, whether in an action of contract, tort or otherwise,
arising from, out of or in connection with the software or the use or other dealings in the software.
------
Thank you for your interest!

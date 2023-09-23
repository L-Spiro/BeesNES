# Palette Generator

yet another NES palette generator, in Python
<img src="docs/palette preview.png">
<img src="docs/waveform phase.gif">
<img src="docs/QAM phase.gif">
<img src="docs/palette preview emphasis.gif">

something to note: there _is_ no one true NES palette, but this generator can pretty much approach colors that looks good enough. feel free to adjust to taste!

## Requirements

This script requires `numpy` for arrays and matrix math.

This script requires the `colour-science` library for linear light functions, color adaptation, and CIE 1931 colorimetry diagrams.

This script requires `matplotlib` for graphs.

## Usage
```
usage: palgen-persune.py [-h] [--html-hex] [--wiki-table] [--c-table] [-d] [-r RENDER_IMG] [-w] [-p] [--skip-plot]
                         [-o OUTPUT] [--float-pal FLOAT_PAL] [-e] [-t TEST_IMAGE] [-n NORMALIZE] [-c CLIP] [-pal]
                         [-cbr COLORBURST_REFERENCE] [-bri BRIGHTNESS] [-con CONTRAST] [-hue HUE] [-sat SATURATION]
                         [-blp BLACK_POINT] [-whp WHITE_POINT] [-phs PHASE_SKEW] [-aps ANTIEMPHASIS_PHASE_SKEW]
                         [-ela EMPHASIS_LUMA_ATTENUATION] [-rfc REFERENCE_COLORSPACE] [-dsc DISPLAY_COLORSPACE]
                         [-cat CHROMATIC_ADAPTATION_TRANSFORM] [-ict]
                         [-rpr REFERENCE_PRIMARIES_R REFERENCE_PRIMARIES_R]
                         [-rpg REFERENCE_PRIMARIES_G REFERENCE_PRIMARIES_G]
                         [-rpb REFERENCE_PRIMARIES_B REFERENCE_PRIMARIES_B]
                         [-rpw REFERENCE_PRIMARIES_W REFERENCE_PRIMARIES_W]
                         [-dpr DISPLAY_PRIMARIES_R DISPLAY_PRIMARIES_R] [-dpg DISPLAY_PRIMARIES_G DISPLAY_PRIMARIES_G]
                         [-dpb DISPLAY_PRIMARIES_B DISPLAY_PRIMARIES_B] [-dpw DISPLAY_PRIMARIES_W DISPLAY_PRIMARIES_W]

yet another NES palette generator

options:
  -h, --help            show this help message and exit
  --html-hex            print HTML hex triplet values for each palette color
  --wiki-table          print MediaWiki formatted color table
  --c-table             print an array of hex formatted c-style unsigned integers
  -d, --debug           debug messages
  -r RENDER_IMG, --render-img RENDER_IMG
                        render views and diagrams as images in docs folder with the provided file extension.
  -w, --waveforms       view composite waveforms
  -p, --phase-QAM       view QAM demodulation
  --skip-plot           skips showing the palette plot
  -o OUTPUT, --output OUTPUT
                        .pal file output
  --float-pal FLOAT_PAL
                        .pal file but with 32-bit single precision floating point numbers
  -e, --emphasis        include emphasis entries
  -t TEST_IMAGE, --test-image TEST_IMAGE
                        use 256x240 uint16 raw binary PPU frame buffer for palette proofreading
  -n NORMALIZE, --normalize NORMALIZE
                        0 = normalize all colors within gamut (ignores black and white points, contrast, and
                        brightness), 1 = same as 1, but clip negative values, 2 = color is desaturated until RGB
                        channels are within range, 3 = same as 2 but clipped negative values
  -c CLIP, --clip CLIP  clips out-of-gamut RGB colors. 0 = any of the RGB channels are clipped to max (default), 1 =
                        color is darkened until RGB channels are in range, 2 = color is desaturated until RGB channels
                        are in range
  -pal                  designates the colorburst reference to -U ± 45 degrees
  -cbr COLORBURST_REFERENCE, --colorburst-reference COLORBURST_REFERENCE
                        phase of colorburst reference. default is 8
  -bri BRIGHTNESS, --brightness BRIGHTNESS
                        brightness delta, -1.0 to 1.0, default = 0.0
  -con CONTRAST, --contrast CONTRAST
                        contrast delta, 0.0 to 1.0, default = 0.0
  -hue HUE, --hue HUE   hue angle delta, in degrees, default = 0.0
  -sat SATURATION, --saturation SATURATION
                        saturation delta, -1.0 to 1.0, default = 0.0
  -blp BLACK_POINT, --black-point BLACK_POINT
                        black point, in voltage units relative to blanking, default = (luma level $0F/$1F)
  -whp WHITE_POINT, --white-point WHITE_POINT
                        white point, in voltage units relative to blanking, default = (luma level $20)
  -phs PHASE_SKEW, --phase-skew PHASE_SKEW
                        differential phase distortion, in degrees, default = 0.0
  -aps ANTIEMPHASIS_PHASE_SKEW, --antiemphasis-phase-skew ANTIEMPHASIS_PHASE_SKEW
                        additonal phase distortion on colors $x2/$x6/$xA, in degrees, default = 0.0
  -ela EMPHASIS_LUMA_ATTENUATION, --emphasis-luma-attenuation EMPHASIS_LUMA_ATTENUATION
                        additonal luma brightness on colors $x4/$x8/$xC, in voltage units, default = 0.0
  -rfc REFERENCE_COLORSPACE, --reference-colorspace REFERENCE_COLORSPACE
                        use colour.RGB_COLOURSPACES reference colorspace, default = "ITU-R BT.709"
  -dsc DISPLAY_COLORSPACE, --display-colorspace DISPLAY_COLORSPACE
                        Use colour.RGB_COLOURSPACES display colorspace, default = "ITU-R BT.709"
  -cat CHROMATIC_ADAPTATION_TRANSFORM, --chromatic-adaptation-transform CHROMATIC_ADAPTATION_TRANSFORM
                        chromatic adaptation transform method, default = None
  -ict, --inverse-chromatic-transform
                        invert direction of chromatic adaptation transform method (from display to reference)
  -rpr REFERENCE_PRIMARIES_R REFERENCE_PRIMARIES_R, --reference-primaries-r REFERENCE_PRIMARIES_R REFERENCE_PRIMARIES_R
                        set custom reference color primary R, in CIE xy chromaticity coordinates
  -rpg REFERENCE_PRIMARIES_G REFERENCE_PRIMARIES_G, --reference-primaries-g REFERENCE_PRIMARIES_G REFERENCE_PRIMARIES_G
                        set custom reference color primary G, in CIE xy chromaticity coordinates
  -rpb REFERENCE_PRIMARIES_B REFERENCE_PRIMARIES_B, --reference-primaries-b REFERENCE_PRIMARIES_B REFERENCE_PRIMARIES_B
                        set custom reference color primary B, in CIE xy chromaticity coordinates
  -rpw REFERENCE_PRIMARIES_W REFERENCE_PRIMARIES_W, --reference-primaries-w REFERENCE_PRIMARIES_W REFERENCE_PRIMARIES_W
                        set custom reference whitepoint, in CIE xy chromaticity coordinates
  -dpr DISPLAY_PRIMARIES_R DISPLAY_PRIMARIES_R, --display-primaries-r DISPLAY_PRIMARIES_R DISPLAY_PRIMARIES_R
                        set custom display color primary R, in CIE xy chromaticity coordinates
  -dpg DISPLAY_PRIMARIES_G DISPLAY_PRIMARIES_G, --display-primaries-g DISPLAY_PRIMARIES_G DISPLAY_PRIMARIES_G
                        set custom display color primary G, in CIE xy chromaticity coordinates
  -dpb DISPLAY_PRIMARIES_B DISPLAY_PRIMARIES_B, --display-primaries-b DISPLAY_PRIMARIES_B DISPLAY_PRIMARIES_B
                        set custom display color primary B, in CIE xy chromaticity coordinates
  -dpw DISPLAY_PRIMARIES_W DISPLAY_PRIMARIES_W, --display-primaries-w DISPLAY_PRIMARIES_W DISPLAY_PRIMARIES_W
                        set custom display whitepoint, in CIE xy chromaticity coordinates

version 0.7.3
```

## License

This work is licensed under the MIT-0 license.

Copyright (C) Persune 2023.

## Credits

Special thanks to:
- NewRisingSun
- lidnariq
- _aitchFactor
- jekuthiel

This would have not been possible without their help!

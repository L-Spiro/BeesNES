# palgen NES
# Copyright (C) 2023 Persune
# inspired by PalGen, Copyright (C) 2018 DragWx <https://github.com/DragWx>
# testing out the concepts from https://www.nesdev.org/wiki/NTSC_video#Composite_decoding
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of this
# software and associated documentation files (the "Software"), to deal in the Software
# without restriction, including without limitation the rights to use, copy, modify,
# merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
# PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

import argparse
import os
import sys
import numpy as np

parser=argparse.ArgumentParser(
    description="yet another NES palette generator",
    epilog="version 0.7.3")
# print output options
parser.add_argument(
    "--html-hex",
    action="store_true",
    help="print HTML hex triplet values for each palette color")
parser.add_argument(
    "--wiki-table",
    action="store_true",
    help="print MediaWiki formatted color table")
parser.add_argument(
    "--c-table",
    action="store_true",
    help="print an array of hex formatted c-style unsigned integers")
parser.add_argument(
    "-d",
    "--debug",
    action="store_true",
    help="debug messages")

# visualization options
parser.add_argument(
    "-r",
    "--render-img",
    type=str,
    help="render views and diagrams as images in docs folder with the provided file extension.")
parser.add_argument(
    "-w",
    "--waveforms",
    action="store_true",
    help="view composite waveforms")
parser.add_argument(
    "-p",
    "--phase-QAM",
    action="store_true",
    help="view QAM demodulation")

# file interaction options
parser.add_argument(
    "--skip-plot",
    action="store_true",
    help="skips showing the palette plot")
parser.add_argument(
    "-o",
    "--output",
    type=str,
    help=".pal file output")
parser.add_argument(
    "--float-pal",
    type=str,
    help=".pal file but with 32-bit single precision floating point numbers")
parser.add_argument(
    "-e",
    "--emphasis",
    action="store_true",
    help="include emphasis entries")
parser.add_argument(
    "-t",
    "--test-image",
    type=str,
    help="use 256x240 uint16 raw binary PPU frame buffer for palette proofreading")

# generation options
parser.add_argument(
    "-n",
    "--normalize",
    type=int,
    help="0 = normalize all colors within gamut (ignores black and white points, contrast, and brightness), 1 = same as 1, but clip negative values, 2 = color is desaturated until RGB channels are within range, 3 = same as 2 but clipped negative values",
    default=(-1))
parser.add_argument(
    "-c",
    "--clip",
    type=int,
    help="clips out-of-gamut RGB colors. 0 = any of the RGB channels are clipped to max (default), 1 = color is darkened until RGB channels are in range, 2 = color is desaturated until RGB channels are in range",
    default=0)
parser.add_argument(
    "-pal",
    action="store_true",
    help = "designates the colorburst reference to -U ± 45 degrees")
parser.add_argument(
    "-cbr",
    "--colorburst-reference",
    type = np.float64,
    help = "phase of colorburst reference. default is 8",
    default = 8)

# color adjustment options
parser.add_argument(
    "-bri",
    "--brightness",
    type = np.float64,
    help = "brightness delta, -1.0 to 1.0, default = 0.0",
    default = 0.0)
parser.add_argument(
    "-con",
    "--contrast",
    type = np.float64,
    help = "contrast delta, 0.0 to 1.0, default = 0.0",
    default = 0.0)
parser.add_argument(
    "-hue",
    "--hue",
    type = np.float64,
    help = "hue angle delta, in degrees, default = 0.0",
    default = 0)
parser.add_argument(
    "-sat",
    "--saturation",
    type = np.float64,
    help ="saturation delta, -1.0 to 1.0, default = 0.0",
    default = 0)
parser.add_argument(
    "-blp",
    "--black-point",
    type = np.float64,
    help = "black point, in voltage units relative to blanking, default = (luma level $0F/$1F)")
parser.add_argument(
    "-whp",
    "--white-point",
    type = np.float64,
    help = "white point, in voltage units relative to blanking, default = (luma level $20)")

# analog distortion effects options
parser.add_argument(
    "-phs",
    "--phase-skew",
    type = np.float64,
    help = "differential phase distortion, in degrees, default = 0.0",
    default = 0)
parser.add_argument(
    "-aps",
    "--antiemphasis-phase-skew",
    type = np.float64,
    help = "additonal phase distortion on colors $x2/$x6/$xA, in degrees, default = 0.0",
    default = 0)
parser.add_argument(
    "-ela",
    "--emphasis-luma-attenuation",
    type = np.float64,
    help = "additonal luma brightness on colors $x4/$x8/$xC, in voltage units, default = 0.0",
    default = 0)

# colorimetry options
parser.add_argument(
    "-rfc",
    "--reference-colorspace",
    type = str,
    help = "use colour.RGB_COLOURSPACES reference colorspace, default = \"ITU-R BT.709\"",
    default = 'ITU-R BT.709')
parser.add_argument(
    "-dsc",
    "--display-colorspace",
    type = str,
    help = "Use colour.RGB_COLOURSPACES display colorspace, default = \"ITU-R BT.709\"",
    default = 'ITU-R BT.709')
parser.add_argument(
    "-cat",
    "--chromatic-adaptation-transform",
    type = str,
    help = "chromatic adaptation transform method, default = None")
parser.add_argument(
    "-ict",
    "--inverse-chromatic-transform",
    action="store_true",
    help = "invert direction of chromatic adaptation transform method (from display to reference)")

parser.add_argument(
    "-rpr",
    "--reference-primaries-r",
    type = np.float64,
    nargs=2,
    help = "set custom reference color primary R, in CIE xy chromaticity coordinates")
parser.add_argument(
    "-rpg",
    "--reference-primaries-g",
    type = np.float64,
    nargs=2,
    help = "set custom reference color primary G, in CIE xy chromaticity coordinates")
parser.add_argument(
    "-rpb",
    "--reference-primaries-b",
    type = np.float64,
    nargs=2,
    help = "set custom reference color primary B, in CIE xy chromaticity coordinates")
parser.add_argument(
    "-rpw",
    "--reference-primaries-w",
    type = np.float64,
    nargs=2,
    help = "set custom reference whitepoint, in CIE xy chromaticity coordinates")

parser.add_argument(
    "-dpr",
    "--display-primaries-r",
    type = np.float64,
    nargs=2,
    help = "set custom display color primary R, in CIE xy chromaticity coordinates")
parser.add_argument(
    "-dpg",
    "--display-primaries-g",
    type = np.float64,
    nargs=2,
    help = "set custom display color primary G, in CIE xy chromaticity coordinates")
parser.add_argument(
    "-dpb",
    "--display-primaries-b",
    type = np.float64,
    nargs=2,
    help = "set custom display color primary B, in CIE xy chromaticity coordinates")
parser.add_argument(
    "-dpw",
    "--display-primaries-w",
    type = np.float64,
    nargs=2,
    help = "set custom display whitepoint, in CIE xy chromaticity coordinates")

args = parser.parse_args()

if (args.skip_plot) and (args.output is None) and (args.float_pal is None) and not (args.render_img is not None):
    sys.exit("warning! palette is generated but not plotted or outputted")

if not (args.skip_plot) or (args.render_img is not None): 
    import matplotlib.pyplot as plt
    import matplotlib.gridspec as gridspec
    import colour.plotting.diagrams

import colour.models

# voltage highs and lows
# from https://forums.nesdev.org/viewtopic.php?p=159266#p159266
# signal[4][2][2] $0x-$3x, $x0/$xD, no emphasis/emphasis
signal_table = np.array([
    [
        [ 0.616, 0.500 ],
        [ 0.228, 0.192 ]
    ],
    [
        [ 0.840, 0.676 ],
        [ 0.312, 0.256 ]
    ],
    [
        [ 1.100, 0.896 ],
        [ 0.552, 0.448 ]
    ],
    [
        [ 1.100, 0.896 ],
        [ 0.880, 0.712 ]
    ]
], np.float64)

# B-Y and R-Y reduction factors
BY_rf = 0.492111
RY_rf = 0.877283

# derived from the NTSC base matrix of luminance and color-difference
RGB_to_YUV = np.array([
    [ 0.299,        0.587,        0.114],
    [-0.299*BY_rf, -0.587*BY_rf,  0.886*BY_rf],
    [ 0.701*RY_rf, -0.587*RY_rf, -0.114*RY_rf]
], np.float64)

# special thanks to NewRisingSun for teaching me how chromatic adaptations work!
# special thanks to _aitchFactor for pointing out that colour-science has
# chromatic adaptation functions!

# reference color profile colorspace
s_colorspace = colour.RGB_Colourspace(
    colour.RGB_COLOURSPACES[args.reference_colorspace].name,
    colour.RGB_COLOURSPACES[args.reference_colorspace].primaries,
    colour.RGB_COLOURSPACES[args.reference_colorspace].whitepoint)

if (args.reference_primaries_r is not None
    and args.reference_primaries_g is not None
    and args.reference_primaries_b is not None):
    s_colorspace.name = "custom primaries"
    s_colorspace.primaries = np.array([
        args.reference_primaries_r,
        args.reference_primaries_g,
        args.reference_primaries_b
    ])
else:
    s_colorspace.name = colour.RGB_COLOURSPACES[args.reference_colorspace].name
    s_colorspace.primaries = colour.RGB_COLOURSPACES[args.reference_colorspace].primaries

if (args.reference_primaries_w is not None):
    s_colorspace.whitepoint = args.reference_primaries_w
    s_colorspace.whitepoint_name = "custom whitepoint"
else:
    s_colorspace.whitepoint_name = colour.RGB_COLOURSPACES[args.reference_colorspace].whitepoint_name
    s_colorspace.whitepoint = colour.RGB_COLOURSPACES[args.reference_colorspace].whitepoint

# display color profile colorspace
t_colorspace = colour.RGB_Colourspace(
    colour.RGB_COLOURSPACES[args.display_colorspace].name,
    colour.RGB_COLOURSPACES[args.display_colorspace].primaries,
    colour.RGB_COLOURSPACES[args.display_colorspace].whitepoint)

if (args.display_primaries_r is not None
    and args.display_primaries_g is not None
    and args.display_primaries_b is not None): 
    t_colorspace.name = "custom primaries"
    t_colorspace.primaries = np.array([
        args.display_primaries_r,
        args.display_primaries_g,
        args.display_primaries_b
    ])
else:
    t_colorspace.name = colour.RGB_COLOURSPACES[args.display_colorspace].name
    t_colorspace.primaries = colour.RGB_COLOURSPACES[args.display_colorspace].primaries

if (args.display_primaries_w is not None):
    t_colorspace.whitepoint = args.display_primaries_w
    t_colorspace.whitepoint_name = "custom whitepoint"
else:
    t_colorspace.whitepoint_name = colour.RGB_COLOURSPACES[args.display_colorspace].whitepoint_name
    t_colorspace.whitepoint = colour.RGB_COLOURSPACES[args.display_colorspace].whitepoint

s_colorspace.name = "Reference colorspace: {}".format(s_colorspace.name)
t_colorspace.name = "Display colorspace: {}".format(t_colorspace.name)

# decoded RGB buffer
# has to be zero'd out for the normalize function to work
RGB_buffer = np.zeros([8,4,16,3], np.float64)

# fix issue with colors
offset = 1
emphasis_offset = 1

# due to the way the waveform is encoded, the hue is off by 15 degrees,
# or 1/2 of a sample
colorburst_offset = args.colorburst_reference - 6 - 0.5

if (args.pal): colorburst_offset += 1.5

# signal buffer normalization
if (args.black_point is not None):
    signal_black_point = signal_table[1, 1, 0] + args.black_point
else:
    signal_black_point = signal_table[1, 1, 0]

if (args.white_point is not None):
    signal_white_point = signal_table[1, 1, 0] + args.white_point
else:
    signal_white_point = signal_table[3, 0, 0]

# used for image sequence plotting
sequence_counter = 0

def color_clip_darken(RGB_buffer):
    for emph in range(RGB_buffer.shape[0]):
        for luma in range(RGB_buffer.shape[1]):
            for chroma in range(RGB_buffer.shape[2]):
                # if any of the RGB channels are greater than 1
                if np.any(np.greater(RGB_buffer[emph, luma, chroma], [1.0, 1.0, 1.0])):
                    # subtract all channels by delta of greatest channel
                    # algorithm by DragWx
                    darken_factor = np.max(RGB_buffer[emph, luma, chroma])
                    RGB_buffer[emph, luma, chroma] /= darken_factor
    return RGB_buffer

def color_clip_desaturate(RGB_buffer):
    for emph in range(RGB_buffer.shape[0]):
        for luma in range(RGB_buffer.shape[1]):
            for chroma in range(RGB_buffer.shape[2]):
                # if any of the RGB channels are greater than 1
                if np.any(np.greater(RGB_buffer[emph, luma, chroma], [1.0, 1.0, 1.0])):
                    # desaturate that specific color until channels are within range
                    # algorithm by DragWx
                    darken_factor = np.max(RGB_buffer[emph, luma, chroma])
                    YUV_calc = np.matmul(RGB_to_YUV, RGB_buffer[emph, luma, chroma])
                    RGB_buffer[emph, luma, chroma] -= YUV_calc[0]
                    RGB_buffer[emph, luma, chroma] /= darken_factor
                    RGB_buffer[emph, luma, chroma] += YUV_calc[0]
    return RGB_buffer

# figure plotting for palette preview
# TODO: interactivity
def NES_palette_plot(RGB_buffer, RGB_uncorrected, emphasis, all_emphasis = False, export_diagrams = False, export_img = False):
    if (args.skip_plot and not (export_diagrams or export_img)):
        return
    if all_emphasis or not args.emphasis:
        RGB_sub = RGB_buffer
        RGB_sub_raw = RGB_uncorrected
    else:
        RGB_sub = np.split(RGB_buffer, 8, 0)[emphasis]
        RGB_sub_raw = np.split(RGB_uncorrected, 8, 0)[emphasis]

    fig = plt.figure(tight_layout=True, dpi=96)
    gs = gridspec.GridSpec(2, 2)
    
    ax0 = fig.add_subplot(gs[1, 1])
    ax1 = fig.add_subplot(gs[0, 1], projection='polar')
    # preview on indexed image (if provided)
    if (args.test_image is not None and (all_emphasis or not args.emphasis)):
        ax2 = fig.add_subplot(gs[0, 0])
        ax3 = fig.add_subplot(gs[1, 0])
        with open(args.test_image, mode="rb") as index_file:
            index_image = np.reshape(np.frombuffer(index_file.read(), dtype=np.uint16), (240, 256))
            preview_image = np.empty([240,256,3], np.float64)
            for y in range(index_image.shape[0]):
                for x in range(index_image.shape[1]):
                    preview_image[y,x] = RGB_sub[(index_image[y,x] >> 4), (index_image[y,x] & 0x0F)]
            ax3.set_title("Palette preview")
            ax3.imshow(preview_image)
            if (export_img):
                from PIL import Image
                image_filename = os.path.splitext(args.test_image)[0]
                imageout = Image.fromarray(np.ubyte(np.around(preview_image * 255)))
                imageout.save("{0}.png".format(image_filename))
                imageout.close()
    else:
        ax2 = fig.add_subplot(gs[:, 0])
    
    if (export_diagrams):
        fig.suptitle('NES palette (emphasis = {0:03b})'.format(emphasis))
    else:
        fig.suptitle('NES palette')

    # colors
    ax0.set_title("Color swatches")
    ax0.imshow(RGB_sub)

    # polar plot
    YUV_calc = np.einsum('ij,klj->kli', RGB_to_YUV, RGB_sub, dtype=np.float64)
    color_theta = np.arctan2(YUV_calc[:, :, 2], YUV_calc[:, :, 1])
    color_r = YUV_calc[:, :, 0]

    ax1.set_title("RGB color phase")
    ax1.set_yticklabels([])
    ax1.axis([0, 2*np.pi, 0, 1])
    ax1.scatter(color_theta, color_r, c=np.reshape(RGB_sub,(RGB_sub.shape[0]*RGB_sub.shape[1], 3)), marker=None, s=color_r*500, zorder=3)

    # CIE graph
    colour.plotting.plot_RGB_chromaticities_in_chromaticity_diagram_CIE1931(
        RGB_sub_raw,
        colourspace=s_colorspace,
        show_whitepoints=False,
        scatter_kwargs=dict(c=np.reshape(RGB_sub_raw,(RGB_sub_raw.shape[0]*16, 3)),alpha=0.1),
        plot_kwargs=dict(color="gray"),
        figure=fig,
        axes=ax2,
        standalone=False,
        show_diagram_colours=False,
        show_spectral_locus=True,
        spectral_locus_colours='RGB',
        transparent_background=False)
    colour.plotting.plot_RGB_chromaticities_in_chromaticity_diagram_CIE1931(
        RGB_sub,
        colourspace=t_colorspace,
        show_whitepoints=False,
        scatter_kwargs=dict(c=np.reshape(RGB_sub,(RGB_sub.shape[0]*16, 3))),
        plot_kwargs=dict(color="red"),
        figure=fig,
        axes=ax2,
        standalone=False,
        show_diagram_colours=False,
        show_spectral_locus=True,
        spectral_locus_colours='RGB',
        transparent_background=False)
    ax2.set_title("CIE 1931 chromaticity diagram")
    ax2.grid(which='both', color='grey', linewidth=0.5, linestyle='-', alpha=0.2)
    ax2.axis([0, 1, 0, 1])
    ax2.set_aspect('equal', 'box')

    fig.set_size_inches(20, 11.25)
    if (args.test_image is not None and (all_emphasis or not args.emphasis)):
        # only tighten the layout if the palette preview is enabled
        fig.tight_layout()
    if (export_diagrams):
        plt.savefig("docs/palette preview emphasis {0:03}.{1}".format(emphasis, args.render_img))
    elif (export_img):
        plt.savefig("docs/palette preview.{0}".format(args.render_img))
        if not (args.skip_plot):
            plt.show()
    else:
        plt.show()
    plt.close()

def NES_QAM_plot(voltage_buffer, U_buffer, V_buffer, emphasis, luma, hue, sequence_counter):
    fig = plt.figure(tight_layout=True)
    gs = gridspec.GridSpec(3, 2)

    axY = fig.add_subplot(gs[0,0])
    axU = fig.add_subplot(gs[1,0])
    axV = fig.add_subplot(gs[2,0])
    ax1 = fig.add_subplot(gs[:,1], projection='polar')
    fig.suptitle("QAM demodulating ${0:02X} emphasis {1:03b}".format((luma<<4 | hue), emphasis))
    w = voltage_buffer
    x = np.arange(0,12)
    Y_avg = np.average(voltage_buffer)
    U_avg = np.average(U_buffer)
    V_avg = np.average(V_buffer)
    
    range_axis = (signal_white_point / (signal_white_point - signal_black_point)) - signal_black_point
    axY.set_title("Y decoding")
    axY.set_ylabel("value")
    axY.axis([0, 12, 0, range_axis])
    axY.plot(x, voltage_buffer, 'o-', linewidth=0.7, label='composite signal')
    axY.plot(x, np.full((12), Y_avg), 'o-', linewidth=0.7, label='Y value = {:< z.3f}'.format(Y_avg))
    axY.legend(loc='lower right')
    
    axU.set_title("U decoding")
    axU.set_ylabel("value")
    axU.axis([0, 12, -2*range_axis, 2*range_axis])
    axU.plot(x, U_buffer, 'o-', linewidth=0.7, label='demodulated U signal')
    axU.plot(x, np.full((12), U_avg), 'o-', linewidth=0.7, label='U value = {:< z.3f}'.format(U_avg))
    axU.legend(loc='lower right')
    
    axV.set_title("V decoding")
    axV.set_ylabel("value")
    axV.axis([0, 12, -2*range_axis, 2*range_axis])
    axV.plot(x, V_buffer, 'o-', linewidth=0.7, label='demodulated V signal')
    axV.plot(x, np.full((12), V_avg), 'o-', linewidth=0.7, label='V value = {:< z.3f}'.format(V_avg))
    axV.legend(loc='lower right')
    
    color_theta = np.arctan2(V_avg, U_avg)
    color_r =  np.sqrt(U_avg**2 + V_avg**2)
    ax1.axis([0, 2*np.pi, 0, 0.6])
    ax1.set_title("Phasor plot")
    ax1.scatter(color_theta, color_r)
    ax1.vlines(color_theta, 0, color_r)
    
    fig.set_size_inches(16, 9)
    if (args.render_img is not None):
        plt.savefig("docs/QAM phase {0:03}.{1}".format(sequence_counter, args.render_img), dpi=96)
    else:
        plt.show()
    plt.close()

def NES_waveform_plot(voltage_buffer, emphasis, luma, hue, sequence_counter):
    fig = plt.figure(tight_layout=True)
    fig.suptitle("${0:02X} emphasis {1:03b}".format((luma<<4 | hue), emphasis))
    ax = fig.subplots()
    x = np.arange(0,12)
    y = voltage_buffer
    ax.axis([0, 12, 0, 1.5])
    ax.set_xlabel("Sample count")
    ax.set_ylabel("Voltage")
    ax.plot(x, y, 'o-', linewidth=0.7)
    
    fig.set_size_inches(16, 9)
    if (args.render_img is not None):
        plt.savefig("docs/waveform phase {0:03}.{1}".format(sequence_counter, args.render_img), dpi=120)
    else:
        plt.show()
    plt.close()


for emphasis in range(8):
    # emphasis bitmask, travelling from lsb to msb
    emphasis_wave = 0
    if bool(emphasis & 0b001):		# tint R; aligned to color phase C
        emphasis_wave |= 0b000001111110;
    if bool(emphasis & 0b010):		# tint G; aligned to color phase 4
        emphasis_wave |= 0b111000000111;
    if bool(emphasis & 0b100):		# tint B; aligned to color phase 8
        emphasis_wave |= 0b011111100000;

    # 111111------
    # 22222------2
    # 3333------33
    # 444------444
    # 55------5555
    # 6------66666
    # ------777777
    # -----888888-
    # ----999999--
    # ---AAAAAA---
    # --BBBBBB----
    # -CCCCCC-----
    # signal buffer for decoding
    voltage_buffer = np.empty([12], np.float64)
    U_buffer = np.empty([12], np.float64)
    V_buffer = np.empty([12], np.float64)

    for luma in range(4):
        for hue in range(16):
            # encode voltages into composite waveform
            for wave_phase in range(12):
                # 0 = waveform high; 1 = waveform low
                n_wave_level = 0
                # 1 = emphasis activate
                emphasis_level = int(bool(emphasis_wave & (1 << ((wave_phase - hue + emphasis_offset) % 12))))

                if (wave_phase >= 6): n_wave_level = 1

                # rows $x0 amd $xD
                if (hue == 0x00): n_wave_level = 0
                if (hue == 0x0D): n_wave_level = 1

                #rows $xE-$xF
                if (hue >= 0x0E):
                    voltage_buffer[wave_phase] = signal_table[1, 1, 0]
                else:
                    voltage_buffer[(wave_phase - hue + offset) % 12] = signal_table[luma, n_wave_level, emphasis_level]

            # TODO: better filter voltage buffer

            # convolution approach
            # kernel = np.array([0.25, 0.75, 1, -0.2, -0.05, 0], np.float64)
            # voltage_buffer = np.delete(np.convolve(voltage_buffer, kernel), [0, 13, 14, 15, 16])

            # fft approach
            # voltage_buffer = np.fft.fft(voltage_buffer)
            # kernel = np.array([1, 1, 0.5, 0.5, 0.5, 0.3, 0, 0, 0, 0, 0, 0], np.float64)
            # voltage_buffer *= kernel
            # voltage_buffer = np.fft.ifft(voltage_buffer)

            antiemphasis_row_chroma = args.antiemphasis_phase_skew if (hue == 0x2 or hue == 0x6 or hue == 0xA) else 0
            emphasis_row_luma = args.emphasis_luma_attenuation if (hue == 0x4 or hue == 0x8 or hue == 0xC) else 0
            # normalize voltage
            # decode voltage buffer to YUV
            # we use RGB_buffer[] as a temporary buffer for YUV
            
            # decode Y
            RGB_buffer[emphasis, luma, hue, 0] = np.average(voltage_buffer) + emphasis_row_luma
            # decode U
            for t in range(12):
                U_buffer[t] = voltage_buffer[t] * np.sin(
                    2 * np.pi / 12 * (t + colorburst_offset) +
                    np.radians(args.hue + antiemphasis_row_chroma) -
                    np.radians(args.phase_skew * luma)) * 2
            RGB_buffer[emphasis, luma, hue, 1] = np.average(U_buffer) * (args.saturation + 1)

            # decode V
            for t in range(12):
                V_buffer[t] = voltage_buffer[t] * np.cos(
                    2 * np.pi / 12 * (t + colorburst_offset) +
                    np.radians(args.hue + antiemphasis_row_chroma) -
                    np.radians(args.phase_skew * luma)) * 2
            RGB_buffer[emphasis, luma, hue, 2] = np.average(V_buffer) * (args.saturation + 1)

            # decode YUV to RGB
            RGB_buffer[emphasis, luma, hue] = np.matmul(np.linalg.inv(RGB_to_YUV), RGB_buffer[emphasis, luma, hue])

            # visualize chroma decoding
            if (args.debug):
                print("${0:02X} emphasis {1:03b}".format((luma<<4 | hue), emphasis) + "\n" + str(voltage_buffer))
            if (args.waveforms):
                NES_waveform_plot(voltage_buffer, emphasis, luma, hue, sequence_counter)
            if (args.phase_QAM):
                NES_QAM_plot(voltage_buffer, U_buffer, V_buffer, emphasis, luma, hue, sequence_counter)

            sequence_counter += 1
    if not (args.emphasis):
        break

# apply black and white points, brightness, and contrast
RGB_buffer -= signal_black_point
RGB_buffer /= (signal_white_point - signal_black_point)
RGB_buffer += args.brightness
RGB_buffer *= (args.contrast + 1)

# preserve uncorrected RGB for color plotting
RGB_uncorrected = RGB_buffer
# convert RGB to display output
# convert signal to linear light
RGB_buffer = colour.models.oetf_inverse_BT709(RGB_buffer)

# transform linear light
if (args.inverse_chromatic_transform):
    RGB_buffer = colour.RGB_to_RGB(
        RGB_buffer,
        t_colorspace,
        s_colorspace,
        chromatic_adaptation_transform=args.chromatic_adaptation_transform)
else:
    RGB_buffer = colour.RGB_to_RGB(
        RGB_buffer,
        s_colorspace,
        t_colorspace,
        chromatic_adaptation_transform=args.chromatic_adaptation_transform)

# convert linear light to signal
RGB_buffer = colour.models.oetf_BT709(RGB_buffer)

# fit RGB within range of 0.0-1.0
# clip takes priority over normalize
if (args.normalize != -1):
    match args.normalize:
        case 0:
            RGB_buffer -= np.amin(RGB_buffer)
            RGB_buffer /= np.amax(RGB_buffer)
            RGB_uncorrected -= np.amin(RGB_uncorrected)
            RGB_uncorrected /= np.amax(RGB_uncorrected)
        case 1:
            RGB_buffer /= np.amax(RGB_buffer)
            RGB_uncorrected /= np.amax(RGB_uncorrected)
        case _:
            print("normalize option not recognized, using default")
            RGB_buffer -= np.amin(RGB_buffer)
            RGB_buffer /= (np.amax(RGB_buffer) - np.amin(RGB_buffer))
            RGB_uncorrected -= np.amin(RGB_uncorrected)
            RGB_uncorrected /= (np.amax(RGB_uncorrected) - np.amin(RGB_uncorrected))
else:
    match args.clip:
        case 1:
            RGB_buffer = color_clip_darken(RGB_buffer)
            RGB_uncorrected = color_clip_darken(RGB_uncorrected)
        case 2:
            RGB_buffer = color_clip_desaturate(RGB_buffer)
            RGB_uncorrected = color_clip_desaturate(RGB_uncorrected)

# clip to 0.0-1.0 to ensure everything is within range
np.clip(RGB_buffer, 0, 1, out=RGB_buffer)
np.clip(RGB_uncorrected, 0, 1, out=RGB_uncorrected)

# display data about the palette, and optionally write a .pal file
if (args.emphasis):
    RGB_buffer = np.reshape(RGB_buffer,(32, 16, 3))
    RGB_uncorrected = np.reshape(RGB_uncorrected,(32, 16, 3))
else:
    # crop non-emphasis colors if not enabled
    RGB_buffer = RGB_buffer[0]
    RGB_uncorrected = RGB_uncorrected[0]

if (args.output is not None):
    with open(args.output, mode="wb") as Palette_file:
        Palette_file.write(np.uint8(np.around(RGB_buffer * 0xFF)))

if (args.float_pal is not None):
    with open(args.float_pal, mode="wb") as Palette_file:
        Palette_file.write(RGB_buffer)

if (args.html_hex):
    for luma in range(RGB_buffer.shape[0]):
        for hue in range(16):
            print(
                "#{0:02X}{1:02X}{2:02X}".format(
                    np.uint8(np.around(RGB_buffer[luma, hue, 0] * 0xFF)),
                    np.uint8(np.around(RGB_buffer[luma, hue, 1] * 0xFF)),
                    np.uint8(np.around(RGB_buffer[luma, hue, 2] * 0xFF))))
        print("")

if (args.wiki_table):
    print("{|class=\"wikitable\"")
    for luma in range(4):
        print("|-")
        for hue in range(16):
            color_r = int(np.around(RGB_buffer[luma, hue, 0] * 0xFF))
            color_g = int(np.around(RGB_buffer[luma, hue, 1] * 0xFF))
            color_b = int(np.around(RGB_buffer[luma, hue, 2] * 0xFF))
            contrast = 0xFFF if ((color_r*299 + color_g*587 + color_b*114) <= 127500) else 0x000
            print("|style=\"border:0px;background-color:#{0:02X}{1:02X}{2:02X};width:32px;height:32px;color:#{3:03x};text-align:center\"|${4:02X}".format(
                color_r,
                color_g,
                color_b,
                contrast,
                ((luma << 4) + hue)))
    print("|}")
if (args.c_table):
    for luma in range(RGB_buffer.shape[0]):
        for hue in range(16):
            print(
                "0x{0:02x}, 0x{1:02x}, 0x{2:02x},".format(
                    np.uint8(np.around(RGB_buffer[luma, hue, 0] * 0xFF)),
                    np.uint8(np.around(RGB_buffer[luma, hue, 1] * 0xFF)),
                    np.uint8(np.around(RGB_buffer[luma, hue, 2] * 0xFF))))
        print("")

if (args.render_img is not None):
    for emphasis in range(8):
        NES_palette_plot(RGB_buffer, RGB_uncorrected, emphasis, False, (args.render_img is not None))
        if not (args.emphasis):
            break

NES_palette_plot(RGB_buffer, RGB_uncorrected, 0, True, False, (args.render_img is not None))

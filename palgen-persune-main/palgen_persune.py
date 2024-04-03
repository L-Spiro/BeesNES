# palgen NES
# Copyright (C) 2024 Persune
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
import colour.models
import colour.plotting.diagrams

VERSION = "0.12.2"

def parse_argv(argv):
    parser=argparse.ArgumentParser(
        description="yet another NES palette generator",
        epilog="version " + VERSION)
    # output options
    parser.add_argument(
        "-d",
        "--debug",
        action="store_true",
        help="debug messages")
    parser.add_argument(
        "--skip-plot",
        action="store_true",
        help="skips showing the palette plot")
    parser.add_argument(
        "-o",
        "--output",
        type=str,
        help="file output path")
    parser.add_argument(
        "-f",
        "--file-format",
        choices=[
            ".pal uint8",
            ".pal double",
            ".pal Jasc",
            ".gpl",
            ".png",
            ".txt HTML hex",
            ".txt MediaWiki",
            ".h uint8_t",
        ],
        default=".pal uint8",
        help="file output format. default = \".pal uint8\"")
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

    # generation options
    parser.add_argument(
        "-n",
        "--normalize",
        type=str,
        help="normalize all colors within gamut by scaling values",
        choices=[
            "scale",
            "scale clip negative"
        ])
    parser.add_argument(
        "-ppu",
        type=str,
        help="PPU chip used for generating colors. default = 2C02",
        choices=[
            "2C02",
            "2C03",
            "2C04-0000",
            "2C04-0001",
            "2C04-0002",
            "2C04-0003",
            "2C04-0004",
            "2C05-99",
            "2C07",
        ],
        default = "2C02")
    parser.add_argument(
        "-c",
        "--clip",
        type=str,
        help="clips out-of-gamut RGB colors",
        choices=[
            "darken",
            "desaturate"
        ])

    # color decoding options
    parser.add_argument(
        "-bri",
        "--brightness",
        type = np.float64,
        help = "luma brightness delta in IRE units, default = 0.0",
        default = 0.0)
    parser.add_argument(
        "-con",
        "--contrast",
        type = np.float64,
        help = "luma contrast factor, default = 1.0",
        default = 1.0)
    parser.add_argument(
        "-hue",
        "--hue",
        type = np.float64,
        help = "chroma hue angle delta, in degrees, default = 0.0",
        default = 0)
    parser.add_argument(
        "-sat",
        "--saturation",
        type = np.float64,
        help ="chroma saturation factor, default = 1.0",
        default = 1)
    parser.add_argument(
        "-blp",
        "--black-point",
        type = np.float64,
        help = "black point, in IRE units, default = 0 IRE")
    parser.add_argument(
        "-whp",
        "--white-point",
        type = np.float64,
        help = "white point, in IRE units, default = level $20")
    parser.add_argument(
        "-gai",
        "--gain",
        type = np.float64,
        help = "gain adjustment to signal before decoding, in IRE units, default = 0.0",
        default = 0.0)

    # analog effects options
    parser.add_argument(
        "-phs",
        "--phase-skew",
        type = np.float64,
        help = "differential phase distortion for composite PPUs, in degrees, default = 0.0",
        default = 0)
    parser.add_argument(
        "-aps",
        "--antiemphasis-phase-skew",
        type = np.float64,
        help = "additonal phase distortion on colors $x2/$x6/$xA for composite PPUs, in degrees, default = 0.0",
        default = 0)
    parser.add_argument(
        "-ela",
        "--emphasis-luma-attenuation",
        type = np.float64,
        help = "additonal luma brightness on colors $x4/$x8/$xC for composite PPUs, in voltage units, default = 0.0",
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
        help = "chromatic adaptation transform method, default = None",
        default = None)
    parser.add_argument(
        "-ict",
        "--inverse-chromatic-transform",
        action="store_true",
        help = "invert direction of chromatic adaptation transform method (from display to reference)")
    parser.add_argument(
        "-oetf",
        "--opto-electronic",
        type=str,
        help="applies \"colour.models\" color component transform function to use as opto-electronic transform function, default = \"ITU-R BT.709\"",
        default = "ITU-R BT.709")
    parser.add_argument(
        "-eotf",
        "--electro-optic",
        type=str,
        help="applies \"colour.models\" color component transform function to use as electro-optic transform function, default = \"ITU-R BT.709\"",
        default = "ITU-R BT.709")
    parser.add_argument(
        "--opto-electronic-disable",
        action = "store_true",
        help = "disable converting linear light to linear signal")
    parser.add_argument(
        "--electro-optic-disable",
        action = "store_true",
        help = "disable converting linear signal to linear light")

    # colorimetry reference RGB and whitepoint primaries
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

    # colorimetry display RGB and whitepoint primaries
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
    
    return parser.parse_args(argv[1:])

# figure plotting for palette preview
# TODO: interactivity
def palette_plot(RGB_buffer,
    RGB_uncorrected,
    emphasis,
    all_emphasis = False,
    export_diagrams = False,
    export_img = False,
    args=None,
    s_colorspace = None,
    t_colorspace = None):
    if (args.skip_plot and not (export_diagrams or export_img)):
        return

    import matplotlib.pyplot as plt
    import matplotlib.gridspec as gridspec
    import colour.plotting.diagrams

    RGB_sub = RGB_buffer
    RGB_sub_raw = RGB_uncorrected

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

def composite_QAM_plot(voltage_buffer,
    U_buffer,
    V_buffer,
    buffer_size,
    emphasis,
    luma,
    hue,
    sequence_counter,
    args=None,
    signal_black_point=None,
    signal_white_point=None):

    import matplotlib.pyplot as plt
    import matplotlib.gridspec as gridspec

    fig = plt.figure(tight_layout=True)
    gs = gridspec.GridSpec(3, 2)

    axY = fig.add_subplot(gs[0,0])
    axU = fig.add_subplot(gs[1,0])
    axV = fig.add_subplot(gs[2,0])
    ax1 = fig.add_subplot(gs[:,1], projection='polar')
    fig.suptitle("QAM demodulating ${0:02X} emphasis {1:03b}".format((luma<<4 | hue), emphasis))
    x = np.arange(0,buffer_size)
    Y_avg = np.average(voltage_buffer)
    U_avg = np.average(U_buffer)
    V_avg = np.average(V_buffer)
    
    range_axis = ((signal_white_point / (signal_white_point - signal_black_point)) - signal_black_point) * 140
    axY.set_title("Y decoding")
    axY.set_ylabel("IRE")
    axY.axis([0, buffer_size, -50, range_axis])
    axY.plot(x, voltage_buffer, 'o-', linewidth=0.7, label='composite signal')
    axY.plot(x, np.full((buffer_size), Y_avg), 'o-', linewidth=0.7, label='Y value = {:< z.3f}'.format(Y_avg))
    axY.legend(loc='lower right')
    
    axU.set_title("U decoding")
    axU.set_ylabel("IRE")
    axU.axis([0, buffer_size, -range_axis, range_axis])
    axU.plot(x, U_buffer, 'o-', linewidth=0.7, label='demodulated U signal')
    axU.plot(x, np.full((buffer_size), U_avg), 'o-', linewidth=0.7, label='U value = {:< z.3f}'.format(U_avg))
    axU.legend(loc='lower right')
    
    axV.set_title("V decoding")
    axV.set_ylabel("IRE")
    axV.axis([0, buffer_size, -range_axis, range_axis])
    axV.plot(x, V_buffer, 'o-', linewidth=0.7, label='demodulated V signal')
    axV.plot(x, np.full((buffer_size), V_avg), 'o-', linewidth=0.7, label='V value = {:< z.3f}'.format(V_avg))
    axV.legend(loc='lower right')
    
    color_theta = np.arctan2(V_avg, U_avg)
    color_r =  np.sqrt(U_avg**2 + V_avg**2)
    ax1.axis([0, 2*np.pi, 0, 60])
    ax1.set_title("UV Phasor plot")
    ax1.scatter(color_theta, color_r)
    ax1.vlines(color_theta, 0, color_r)
    
    fig.set_size_inches(16, 9)
    if (args.render_img is not None):
        plt.savefig("docs/QAM phase {0:03}.{1}".format(sequence_counter, args.render_img), dpi=96)
    else:
        plt.show()
    plt.close()

def composite_waveform_plot(voltage_buffer, emphasis, luma, hue, sequence_counter, args=None):
    import matplotlib.pyplot as plt

    fig = plt.figure(tight_layout=True)
    fig.suptitle("${0:02X} emphasis {1:03b}".format((luma<<4 | hue), emphasis))
    ax = fig.subplots()
    x = np.arange(0,12)
    y = voltage_buffer
    ax.axis([0, 12, -50, 140])
    ax.set_xlabel("Sample count")
    ax.set_ylabel("IRE")
    ax.plot(x, y, 'o-', linewidth=0.7)
    
    fig.set_size_inches(16, 9)
    if (args.render_img is not None):
        plt.savefig("docs/waveform phase {0:03}.{1}".format(sequence_counter, args.render_img), dpi=120)
    else:
        plt.show()
    plt.close()

def normalize_RGB(RGB_buffer, args=None):
    # clip takes priority over normalize
    if (args.clip is not None):
        match args.clip:
            case "darken":
                color_clip_darken(RGB_buffer)
            case "desaturate":
                color_clip_desaturate(RGB_buffer)
    elif (args.normalize is not None):
        if (args.normalize != "scale clip negative"):
            RGB_buffer -= np.amin(RGB_buffer)
        RGB_buffer /= np.amax(RGB_buffer)

    # clip to 0.0-1.0 to ensure everything is within range
    np.clip(RGB_buffer, 0, 1, out=RGB_buffer)

def color_clip_darken(RGB_buffer):
    for luma in range(RGB_buffer.shape[0]):
        for chroma in range(RGB_buffer.shape[1]):
            # if any of the RGB channels are greater than 1
            if np.any(np.greater(RGB_buffer[luma, chroma], [1.0, 1.0, 1.0])):
                # subtract all channels by delta of greatest channel
                # algorithm by DragWx
                darken_factor = np.max(RGB_buffer[luma, chroma])
                RGB_buffer[luma, chroma] /= darken_factor

def color_clip_desaturate(RGB_buffer):
    for luma in range(RGB_buffer.shape[0]):
        for chroma in range(RGB_buffer.shape[1]):
            # if any of the RGB channels are greater than 1
            if np.any(np.greater(RGB_buffer[luma, chroma], [1.0, 1.0, 1.0])):
                # desaturate that specific color until channels are within range
                # algorithm by DragWx
                darken_factor = np.max(RGB_buffer[luma, chroma])
                YUV_calc = np.matmul(RGB_to_YUV, RGB_buffer[luma, chroma])
                RGB_buffer[luma, chroma] -= YUV_calc[0]
                RGB_buffer[luma, chroma] /= darken_factor
                RGB_buffer[luma, chroma] += YUV_calc[0]

# B-Y and R-Y reduction factors
# S170m-2004.pdf: Composite Analog Video Signal NTSC for Studio Applications. Page 16.
BY_rf = 0.492111
RY_rf = 0.877283

# derived from the NTSC base matrix of luminance and color-difference
# S170m-2004.pdf: Composite Analog Video Signal NTSC for Studio Applications. Page 4.
RGB_to_YUV = np.array([
    [ 0.299,        0.587,        0.114],
    [-0.299*BY_rf, -0.587*BY_rf,  0.886*BY_rf],
    [ 0.701*RY_rf, -0.587*RY_rf, -0.114*RY_rf]
], np.float64)

# signal LUTs
# voltage highs and lows
# from https://forums.nesdev.org/viewtopic.php?p=159266#p159266
# signal[4][2][2] $0x-$3x, $x0/$xD, no emphasis/emphasis
signal_table_composite = np.array([
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

def pixel_codec_composite(YUV_buffer, args=None, signal_black_point=None, signal_white_point=None):
    # used for image sequence plotting
    sequence_counter = 0

    if (args.ppu == "2C07"):
        colorburst_phase = 8.5
    else:
        colorburst_phase = 8
    # due to the way the waveform is encoded, the hue is off by 1/2 of a sample
    colorburst_offset = colorburst_phase - 6 - 0.5

    colorburst_factor = 6
    color_gen_clock_factor = 2
    buffer_size = int(colorburst_factor * color_gen_clock_factor)

    # signal buffers for decoding
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

    voltage_buffer = np.empty([buffer_size], np.float64)
    U_buffer = np.empty([buffer_size], np.float64)
    V_buffer = np.empty([buffer_size], np.float64)

    for emphasis in range(8):
        # emphasis bitmask, travelling from lsb to msb
        emphasis_wave = 0
        if bool(emphasis & 0b001):		# tint R; aligned to color phase C
            emphasis_wave |= 0b000001111110;
        if bool(emphasis & 0b010):		# tint G; aligned to color phase 4
            emphasis_wave |= 0b111000000111;
        if bool(emphasis & 0b100):		# tint B; aligned to color phase 8
            emphasis_wave |= 0b011111100000;

        for luma in range(4):
            for hue in range(16):
                # encode voltages into composite waveform
                for wave_phase in range(12):
                    # 0 = waveform high; 1 = waveform low
                    n_wave_level = 0
                    # 1 = emphasis activate
                    emphasis_level = int(bool(emphasis_wave & (1 << ((wave_phase - hue + 1) % 12))))

                    if (wave_phase >= 6): n_wave_level = 1

                    # rows $x0 amd $xD
                    if (hue == 0x00): n_wave_level = 0
                    if (hue == 0x0D): n_wave_level = 1

                    #rows $xE-$xF
                    if (hue >= 0x0E):
                        voltage_buffer[wave_phase] = signal_table_composite[1, 1, 0]
                    else:
                        voltage_buffer[(wave_phase - hue + 1) % 12] = signal_table_composite[luma, n_wave_level, emphasis_level]

                # apply analog effects
                antiemphasis_column_chroma = (
                    args.antiemphasis_phase_skew if (
                        hue == 0x2 or
                        hue == 0x6 or
                        hue == 0xA)
                    else 0)
                emphasis_row_luma = (
                    args.emphasis_luma_attenuation if (
                        hue == 0x4 or
                        hue == 0x8 or
                        hue == 0xC)
                    else 0)

                # decode voltage buffer to YUV
                # based on SMPTE 170M-2004, page 17, section A.5, equation 10
                # N = 0.925(Y) + 7.5 + 0.925*(U)*sin(2*π*f_sc*t) + 0.925*(V)*cos(2*π*f_sc*t) 
                # scaling factor 0.925 is already accounted for during normalization
                # luma pedestal 7.5 is already accounted for during normalization
                
                # shift blanking to 0
                voltage_buffer -= signal_table_composite[1, 1, 0]
                
                # convert to IRE
                voltage_buffer *= 140

                # apply gain
                voltage_buffer += args.gain

                # decode Y

                # apply brightness and contrast
                YUV_buffer[emphasis, luma, hue, 0] = (np.average(voltage_buffer) + emphasis_row_luma + args.brightness) * args.contrast

                # decode U
                # also apply hue and saturation
                for t in range(12):
                    U_buffer[t] = (voltage_buffer[t] - np.average(voltage_buffer) ) * 2 * np.sin(
                        2 * np.pi / 12 * (t + colorburst_offset) +
                        np.radians(
                            args.hue + antiemphasis_column_chroma -
                            (args.phase_skew * luma))
                    ) * args.saturation
                YUV_buffer[emphasis, luma, hue, 1] = np.average(U_buffer)

                # decode V
                # also apply hue and saturation
                for t in range(12):
                    V_buffer[t] = (voltage_buffer[t] - np.average(voltage_buffer)) * 2 * np.cos(
                        2 * np.pi / 12 * (t + colorburst_offset) +
                        np.radians(
                            args.hue + antiemphasis_column_chroma -
                            (args.phase_skew * luma))
                    ) * args.saturation
                YUV_buffer[emphasis, luma, hue, 2] = np.average(V_buffer)

                # visualize chroma decoding
                if (args.debug):
                    print("${0:02X} emphasis {1:03b}".format((luma<<4 | hue), emphasis) + "\n" + str(voltage_buffer))
                if (args.waveforms):
                    composite_waveform_plot(voltage_buffer, emphasis, luma, hue, sequence_counter, args)
                if (args.phase_QAM):
                    composite_QAM_plot(voltage_buffer, U_buffer, V_buffer, buffer_size, emphasis, luma, hue, sequence_counter, args, signal_black_point, signal_white_point)

                sequence_counter += 1

        if not (args.emphasis):
            # clip unused emphasis space
            YUV_buffer = np.split(YUV_buffer, 8, 0)[0]
            break

    return YUV_buffer

def rgb_oct_triplet_to_float_array(signal_triplet, emphasis):
    red = ((signal_triplet & 0xF00) >> 8) if not (emphasis & 0b001) else 7
    green = ((signal_triplet & 0x0F0) >> 4) if not (emphasis & 0b010) else 7
    blue = ((signal_triplet & 0x00F)) if not (emphasis & 0b100) else 7
    return np.array([red, green, blue], np.float64)

def pixel_codec_rgb(YUV_buffer, args=None, signal_black_point=None, signal_white_point=None):
    signal_table_rgb = np.empty([0x40], np.uint16)
    match args.ppu:
        case "2C04-0000"|"2C04-0001"|"2C04-0002"|"2C04-0003"|"2C04-0004":
            # 2C04 "sorted" PPU palette LUT
            # from https://www.nesdev.org/wiki/PPU_palettes#2C04
            signal_table_rgb = np.array([
                0x333,0x014,0x006,0x326,0x403,0x503,0x510,0x420,0x320,0x120,0x031,0x040,0x022,0x111,0x003,0x020,
                0x555,0x036,0x027,0x407,0x507,0x704,0x700,0x630,0x430,0x140,0x040,0x053,0x044,0x222,0x200,0x310,
                0x777,0x357,0x447,0x637,0x707,0x737,0x740,0x750,0x660,0x360,0x070,0x276,0x077,0x444,0x000,0x000,
                0x777,0x567,0x657,0x757,0x747,0x755,0x764,0x770,0x773,0x572,0x473,0x276,0x467,0x666,0x653,0x760 
            ], np.uint16)
        case _:
            # RGB PPU palettes LUT
            # from https://forums.nesdev.org/viewtopic.php?p=98955#p98955
            signal_table_rgb = np.array([
                0x333,0x014,0x006,0x326,0x403,0x503,0x510,0x420,0x320,0x120,0x031,0x040,0x022,0x000,0x000,0x000,
                0x555,0x036,0x027,0x407,0x507,0x704,0x700,0x630,0x430,0x140,0x040,0x053,0x044,0x000,0x000,0x000,
                0x777,0x357,0x447,0x637,0x707,0x737,0x740,0x750,0x660,0x360,0x070,0x276,0x077,0x000,0x000,0x000,
                0x777,0x567,0x657,0x757,0x747,0x755,0x764,0x772,0x773,0x572,0x473,0x276,0x467,0x000,0x000,0x000
            ], np.uint16)

    IQ_tilt = np.radians(33)
    RGB_to_YIQ = np.array([
        RGB_to_YUV[0,:],
        ((RGB_to_YUV[1,:] * np.cos(IQ_tilt)) - (RGB_to_YUV[2,:]*np.sin(IQ_tilt))),
        ((RGB_to_YUV[1,:] * np.sin(IQ_tilt)) + (RGB_to_YUV[2,:]*np.cos(IQ_tilt)))
    ], np.float64)
    YUV_to_YIQ = np.array([
        [1, 0, 0],
        [0, np.cos(IQ_tilt), np.sin(IQ_tilt)],
        [0, -np.sin(IQ_tilt), np.cos(IQ_tilt)]
    ], np.float64)

    # 2C04 LUTs
    scramble = np.empty([0x40], np.uint8)
    match args.ppu:
        case "2C04-0001":
            scramble = np.array([
                0x35,0x23,0x16,0x22,0x1C,0x09,0x1D,0x15,0x20,0x00,0x27,0x05,0x04,0x28,0x08,0x20,
                0x21,0x3E,0x1F,0x29,0x3C,0x32,0x36,0x12,0x3F,0x2B,0x2E,0x1E,0x3D,0x2D,0x24,0x01,
                0x0E,0x31,0x33,0x2A,0x2C,0x0C,0x1B,0x14,0x2E,0x07,0x34,0x06,0x13,0x02,0x26,0x2E,
                0x2E,0x19,0x10,0x0A,0x39,0x03,0x37,0x17,0x0F,0x11,0x0B,0x0D,0x38,0x25,0x18,0x3A
            ], np.uint8)
        case "2C04-0002":
            scramble = np.array([
                0x2E,0x27,0x18,0x39,0x3A,0x25,0x1C,0x31,0x16,0x13,0x38,0x34,0x20,0x23,0x3C,0x0B,
                0x0F,0x21,0x06,0x3D,0x1B,0x29,0x1E,0x22,0x1D,0x24,0x0E,0x2B,0x32,0x08,0x2E,0x03,
                0x04,0x36,0x26,0x33,0x11,0x1F,0x10,0x02,0x14,0x3F,0x00,0x09,0x12,0x2E,0x28,0x20,
                0x3E,0x0D,0x2A,0x17,0x0C,0x01,0x15,0x19,0x2E,0x2C,0x07,0x37,0x35,0x05,0x0A,0x2D
            ], np.uint8)
        case "2C04-0003":
            scramble = np.array([
                0x14,0x25,0x3A,0x10,0x0B,0x20,0x31,0x09,0x01,0x2E,0x36,0x08,0x15,0x3D,0x3E,0x3C,
                0x22,0x1C,0x05,0x12,0x19,0x18,0x17,0x1B,0x00,0x03,0x2E,0x02,0x16,0x06,0x34,0x35,
                0x23,0x0F,0x0E,0x37,0x0D,0x27,0x26,0x20,0x29,0x04,0x21,0x24,0x11,0x2D,0x2E,0x1F,
                0x2C,0x1E,0x39,0x33,0x07,0x2A,0x28,0x1D,0x0A,0x2E,0x32,0x38,0x13,0x2B,0x3F,0x0C
            ], np.uint8)
        case "2C04-0004":
            scramble = np.array([
                0x18,0x03,0x1C,0x28,0x2E,0x35,0x01,0x17,0x10,0x1F,0x2A,0x0E,0x36,0x37,0x0B,0x39,
                0x25,0x1E,0x12,0x34,0x2E,0x1D,0x06,0x26,0x3E,0x1B,0x22,0x19,0x04,0x2E,0x3A,0x21,
                0x05,0x0A,0x07,0x02,0x13,0x14,0x00,0x15,0x0C,0x3D,0x11,0x0F,0x0D,0x38,0x2D,0x24,
                0x33,0x20,0x08,0x16,0x3F,0x2B,0x20,0x3C,0x2E,0x27,0x23,0x31,0x29,0x32,0x2C,0x09
            ], np.uint8)
        case _:
            scramble = np.arange(0x00, 0x40)

    for emphasis in range(8):
        for luma in range(4):
            for hue in range(16):
                color_byte = (luma << 4) | hue

                # decode palette LUT to RGB
                YUV_buffer[emphasis, luma, hue] = rgb_oct_triplet_to_float_array(signal_table_rgb[scramble[color_byte]], emphasis) / 7

                # convert to IRE scale so that i don't have to complicate black/white points
                YUV_buffer[emphasis, luma, hue] *= 100

                # apply gain
                YUV_buffer[emphasis, luma, hue] += args.gain

                # encode RGB to YIQ for hue and saturation adjustment
                YUV_buffer[emphasis, luma, hue] = np.matmul(RGB_to_YIQ, YUV_buffer[emphasis, luma, hue])

                # Titler functionality
                if (args.ppu == "2C05-99"):
                    # reduce Q component by half
                    YUV_buffer[emphasis, luma, hue, 2] *= 0.5

                # apply brightness and contrast
                YUV_buffer[emphasis, luma, hue, 0] += args.brightness
                YUV_buffer[emphasis, luma, hue, 0] *= args.contrast
                
                # apply hue
                YUV_buffer[emphasis, luma, hue, 1] = (
                    (YUV_buffer[emphasis, luma, hue, 1] * np.cos(np.radians(args.hue))) -
                    (YUV_buffer[emphasis, luma, hue, 2] * np.sin(np.radians(args.hue)))
                )
                YUV_buffer[emphasis, luma, hue, 2] = (
                    (YUV_buffer[emphasis, luma, hue, 1] * np.sin(np.radians(args.hue))) +
                    (YUV_buffer[emphasis, luma, hue, 2] * np.cos(np.radians(args.hue)))
                )

                # apply saturation
                YUV_buffer[emphasis, luma, hue, 1] *= args.saturation
                YUV_buffer[emphasis, luma, hue, 2] *= args.saturation
                
                # convert to YUV for later decoding
                YUV_buffer[emphasis, luma, hue] = np.matmul(YUV_to_YIQ, YUV_buffer[emphasis, luma, hue])

        if not (args.emphasis):
            # clip unused emphasis space
            YUV_buffer = np.split(YUV_buffer, 8, 0)[0]
            break

    return YUV_buffer

def output_binary_uint8(RGB_buffer, args=None):
    with open((os.path.splitext(args.output)[0] + ".pal"), mode="wb") as Palette_file:
        Palette_file.write(np.uint8(np.around(RGB_buffer * 0xFF)))

def output_binary_double(RGB_buffer, args=None):
    with open((os.path.splitext(args.output)[0] + ".pal"), mode="wb") as Palette_file:
        Palette_file.write(RGB_buffer)

def output_gimp_pal(RGB_buffer, args=None):
    with open((os.path.splitext(args.output)[0] + ".gpl"), mode="wt", newline='\n') as Palette_file:
        Palette_file.write("GIMP Palette\n")
        Palette_file.write("Name: generated NES/FC palette\n")
        Palette_file.write(f"Columns: {RGB_buffer.shape[1]}\n")
        Palette_file.write("# https://github.com/Gumball2415/palgen-persune")
        for luma in range(RGB_buffer.shape[0]):
            for hue in range(RGB_buffer.shape[1]):
                color_byte = ((luma & 0x3) << 4) | hue
                emphasis_byte = (luma >> 2) & 0x7
                Palette_file.write(
                    "\n{0:4d}{1:4d}{2:4d} ${3:02X} emphasis {4:03b}".format(
                        np.uint8(np.around(RGB_buffer[luma, hue, 0] * 0xFF)),
                        np.uint8(np.around(RGB_buffer[luma, hue, 1] * 0xFF)),
                        np.uint8(np.around(RGB_buffer[luma, hue, 2] * 0xFF)),
                        color_byte,
                        emphasis_byte))

def output_png(RGB_buffer, args=None):
    if (args.emphasis):
        sys.exit("error: this format does not support emphasis")
    from PIL import Image, ImagePalette
    imgindex = np.arange(0, int(RGB_buffer.size/3), dtype=np.uint8)
    
    img = Image.frombytes('P', (RGB_buffer.shape[1],RGB_buffer.shape[0]), imgindex)
    # convert to list of uint8
    RGB_buffer_uint8 = list(
        np.uint8(
            np.around(
                RGB_buffer * 0xFF)).ravel())

    img.putpalette(list(RGB_buffer_uint8), rawmode="RGB")
    img.save((os.path.splitext(args.output)[0] + ".png"), optimize=True)

def output_jasc_pal(RGB_buffer, args=None):
    with open((os.path.splitext(args.output)[0] + ".pal"), mode="wt", newline='\n') as Palette_file:
        Palette_file.write("JASC-PAL\n0100")
        Palette_file.write(f"\n{int(RGB_buffer.size/3)}")

        # JASC-PAL stops loading when it encounters #000000??
        black_entry_exists = False

        for luma in range(RGB_buffer.shape[0]):
            for hue in range(RGB_buffer.shape[1]):
                if (RGB_buffer[luma, hue, :].all() != 0):
                    Palette_file.write(
                        "\n{0:0d} {1:0d} {2:0d}".format(
                            np.uint8(np.around(RGB_buffer[luma, hue, 0] * 0xFF)),
                            np.uint8(np.around(RGB_buffer[luma, hue, 1] * 0xFF)),
                            np.uint8(np.around(RGB_buffer[luma, hue, 2] * 0xFF))))
                else:
                    black_entry_exists = True
        if black_entry_exists:
            Palette_file.write("\n0 0 0")

def output_html_hex(RGB_buffer, args=None):
    with open((os.path.splitext(args.output)[0] + ".txt"), mode="wt", newline='\n') as Palette_file:
        for luma in range(RGB_buffer.shape[0]):
            for hue in range(RGB_buffer.shape[1]):
                Palette_file.write(
                    "#{0:02X}{1:02X}{2:02X}\n".format(
                        np.uint8(np.around(RGB_buffer[luma, hue, 0] * 0xFF)),
                        np.uint8(np.around(RGB_buffer[luma, hue, 1] * 0xFF)),
                        np.uint8(np.around(RGB_buffer[luma, hue, 2] * 0xFF))))

def output_mediawiki_table(RGB_buffer, args=None):
    with open((os.path.splitext(args.output)[0] + ".txt"), mode="wt", newline='\n') as Palette_file:
        Palette_file.write("{|class=\"wikitable\"\n")
        for luma in range(RGB_buffer.shape[0]):
            Palette_file.write("|-\n")
            for hue in range(RGB_buffer.shape[1]):
                color_r = int(np.around(RGB_buffer[luma, hue, 0] * 0xFF))
                color_g = int(np.around(RGB_buffer[luma, hue, 1] * 0xFF))
                color_b = int(np.around(RGB_buffer[luma, hue, 2] * 0xFF))
                contrast = 0xFFF if ((color_r*299 + color_g*587 + color_b*114) <= 127500) else 0x000
                color_byte = ((luma & 0x3) << 4) | hue
                Palette_file.write("|style=\"border:0px;background-color:#{0:02X}{1:02X}{2:02X};width:32px;height:32px;color:#{3:03x};text-align:center\"|${4:02X}\n".format(
                    color_r,
                    color_g,
                    color_b,
                    contrast,
                    color_byte))
        Palette_file.write("|}\n")

def output_cstyle_table(RGB_buffer, args=None):
    with open((os.path.splitext(args.output)[0] + ".h"), mode="wt", newline='\n') as Palette_file:
        for luma in range(RGB_buffer.shape[0]):
            for hue in range(RGB_buffer.shape[1]):
                Palette_file.write(
                    "0x{0:02x}, 0x{1:02x}, 0x{2:02x},\n".format(
                        np.uint8(np.around(RGB_buffer[luma, hue, 0] * 0xFF)),
                        np.uint8(np.around(RGB_buffer[luma, hue, 1] * 0xFF)),
                        np.uint8(np.around(RGB_buffer[luma, hue, 2] * 0xFF))))
            Palette_file.write("\n")

# debugging, don't mind this
def NES_SMPTE_plot(RGB_uncorrected, emphasis, args=None):
    import matplotlib.pyplot as plt
    if (args.skip_plot):
        return
    if not args.emphasis:
        RGB_sub_raw = RGB_uncorrected
    else:
        RGB_sub_raw = np.split(RGB_uncorrected, 8, 0)[emphasis]

    print(RGB_sub_raw.shape)
    RGB_sub_raw_SMPTE = np.zeros([7,3], np.float64)

    RGB_SMPTE = np.array([
        [0.75, 0.75, 0.75],
        [0.75, 0.75, 0],
        [0, 0.75, 0.75],
        [0, 0.75, 0],
        [0.75, 0, 0.75],
        [0.75, 0, 0],
        [0, 0, 0.75],
    ], np.float64)

    YUV_SMPTE = np.zeros(RGB_SMPTE.shape, np.float64)
    YUV_SMPTE[0,:] = np.matmul(RGB_to_YUV, RGB_SMPTE[0, :])
    YUV_SMPTE[1,:] = np.matmul(RGB_to_YUV, RGB_SMPTE[1, :])
    YUV_SMPTE[2,:] = np.matmul(RGB_to_YUV, RGB_SMPTE[2, :])
    YUV_SMPTE[3,:] = np.matmul(RGB_to_YUV, RGB_SMPTE[3, :])
    YUV_SMPTE[4,:] = np.matmul(RGB_to_YUV, RGB_SMPTE[4, :])
    YUV_SMPTE[5,:] = np.matmul(RGB_to_YUV, RGB_SMPTE[5, :])
    YUV_SMPTE[6,:] = np.matmul(RGB_to_YUV, RGB_SMPTE[6, :])

    RGB_sub_raw_SMPTE[0,:] = RGB_sub_raw[emphasis, 0x1, 0x0, :]
    RGB_sub_raw_SMPTE[1,:] = RGB_sub_raw[emphasis, 0x2, 0x8, :]
    RGB_sub_raw_SMPTE[2,:] = RGB_sub_raw[emphasis, 0x2, 0xC, :]
    RGB_sub_raw_SMPTE[3,:] = RGB_sub_raw[emphasis, 0x1, 0xA, :]
    RGB_sub_raw_SMPTE[4,:] = RGB_sub_raw[emphasis, 0x1, 0x4, :]
    RGB_sub_raw_SMPTE[5,:] = RGB_sub_raw[emphasis, 0x1, 0x6, :]
    RGB_sub_raw_SMPTE[6,:] = RGB_sub_raw[emphasis, 0x0, 0x2, :]

    color_theta_SMPTE = np.arctan2(YUV_SMPTE[:, 2], YUV_SMPTE[:, 1])
    color_r_SMPTE = np.sqrt(YUV_SMPTE[:, 2]**2 + YUV_SMPTE[:, 1]**2)

    YUV_calc = np.zeros(RGB_sub_raw_SMPTE.shape, np.float64)
    YUV_calc[0,:] = np.matmul(RGB_to_YUV, RGB_sub_raw_SMPTE[0, :])
    YUV_calc[1,:] = np.matmul(RGB_to_YUV, RGB_sub_raw_SMPTE[1, :])
    YUV_calc[2,:] = np.matmul(RGB_to_YUV, RGB_sub_raw_SMPTE[2, :])
    YUV_calc[3,:] = np.matmul(RGB_to_YUV, RGB_sub_raw_SMPTE[3, :])
    YUV_calc[4,:] = np.matmul(RGB_to_YUV, RGB_sub_raw_SMPTE[4, :])
    YUV_calc[5,:] = np.matmul(RGB_to_YUV, RGB_sub_raw_SMPTE[5, :])
    YUV_calc[6,:] = np.matmul(RGB_to_YUV, RGB_sub_raw_SMPTE[6, :])

    color_theta = np.arctan2(YUV_calc[:, 2], YUV_calc[:, 1])
    color_r = np.sqrt(YUV_calc[:, 2]**2 + YUV_calc[:, 1]**2)

    print(YUV_calc)

    fig = plt.figure(tight_layout=True, dpi=96)
    gs = gridspec.GridSpec(2, 2)
    ax1 = fig.add_subplot(gs[:, :], projection='polar')
    fig.suptitle('NES palette')

    ax1.set_title("Vectorscope Plot")
    ax1.set_yticklabels([])
    ax1.axis([0, 2*np.pi, 0, 0.5])
    ax1.scatter(color_theta_SMPTE, color_r_SMPTE, c=RGB_SMPTE, marker=None, s=color_r*500, zorder=3)
    ax1.plot(color_theta, color_r, marker=None, zorder=3)

    fig.set_size_inches(20, 11.25)
    fig.tight_layout()
    plt.show()
    plt.close()

def main(argv=None):
    args = parse_argv(argv or sys.argv)

    if (args.skip_plot) and (args.output is None) and not (args.render_img is not None):
        sys.exit("warning! palette is generated but not plotted or outputted")

    # special thanks to NewRisingSun for teaching me how chromatic adaptations work!
    # special thanks to _aitchFactor for pointing out that colour-science has
    # chromatic adaptation functions!

    # suppress warnings for colour-science
    with colour.utilities.suppress_warnings(*[True] * 4):
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

        # decoded RGB buffer
        # has to be zero'd out for the normalize function to work
        RGB_buffer = np.zeros([8,4,16,3], np.float64)
        signal_black_point = 0
        signal_white_point = 100

        # generate color!
        match args.ppu:
            case "2C03"|"2C04-0000"|"2C04-0001"|"2C04-0002"|"2C04-0003"|"2C04-0004"|"2C05-99":
                # signal buffer normalization
                if (args.black_point is not None):
                    signal_black_point = args.black_point

                if (args.white_point is not None):
                    signal_white_point = args.white_point
                    
                # we use RGB_buffer[] as a temporary buffer for YUV
                RGB_buffer = pixel_codec_rgb(RGB_buffer, args, signal_black_point, signal_white_point)
            case "2C02"|"2C07":
                # signal buffer normalization
                if (args.black_point is not None):
                    signal_black_point = args.black_point

                if (args.white_point is not None):
                    signal_white_point = args.white_point
                else:
                    signal_white_point = 140 * (signal_table_composite[3, 0, 0] - signal_table_composite[1, 1, 0])

                # we use RGB_buffer[] as a temporary buffer for YUV
                RGB_buffer = pixel_codec_composite(RGB_buffer, args, signal_black_point, signal_white_point)

        # reshape buffer after encoding
        if (args.emphasis):
            RGB_buffer = np.reshape(RGB_buffer,(32, 16, 3))
        else:
            RGB_buffer = np.reshape(RGB_buffer,(4, 16, 3))

        # convert back to RGB
        RGB_buffer = np.einsum('ij,klj->kli', np.linalg.inv(RGB_to_YUV), RGB_buffer, dtype=np.float64)

        # apply black and white points
        # this also scales the values back roughly within range of 0 to 1
        RGB_buffer -= signal_black_point
        RGB_buffer /= (signal_white_point - signal_black_point)

        # debug: a rough vectorscope plot
        # NES_SMPTE_plot(RGB_buffer, 0, args, plt)

        # fit RGB within range of 0.0-1.0
        normalize_RGB(RGB_buffer, args)

        # preserve uncorrected RGB for color plotting
        RGB_uncorrected = RGB_buffer

        # convert RGB to display output
        if ((args.electro_optic != args.opto_electronic) or (t_colorspace != s_colorspace)
            or (args.electro_optic_disable) or (args.opto_electronic_disable)):
            # convert linear signal to linear light, if permitted
            if (not args.electro_optic_disable):
                RGB_buffer = colour.cctf_decoding(RGB_buffer, function=args.electro_optic)
                RGB_uncorrected = colour.cctf_decoding(RGB_uncorrected, function=args.electro_optic)

            # transform color primaries
            if (t_colorspace != s_colorspace):
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

            # convert linear light to linear signal, if permitted
            if (not args.opto_electronic_disable):
                RGB_buffer = colour.cctf_encoding(RGB_buffer, function=args.opto_electronic)
                RGB_uncorrected = colour.cctf_encoding(RGB_uncorrected, function=args.opto_electronic)

        # clip again, the transform may produce values beyond 0-1
        normalize_RGB(RGB_buffer, args)
        normalize_RGB(RGB_uncorrected, args)
        
        output_format = {
            ".pal uint8": output_binary_uint8,
            ".pal double": output_binary_double,
            ".pal Jasc": output_jasc_pal,
            ".txt HTML hex": output_html_hex,
            ".txt MediaWiki": output_mediawiki_table,
            ".h uint8_t": output_cstyle_table,
            ".gpl": output_gimp_pal,
            ".png": output_png
            
        }
        if (args.output is not None):
            output_format[args.file_format](RGB_buffer, args)

        s_colorspace.name = "Reference colorspace: {}".format(s_colorspace.name)
        t_colorspace.name = "Display colorspace: {}".format(t_colorspace.name)

        if (args.render_img is not None):
            for emphasis in range(8):
                palette_plot(RGB_buffer, RGB_uncorrected, emphasis, False, False, (args.render_img is not None), args, s_colorspace, t_colorspace)
                if not (args.emphasis):
                    break

        palette_plot(RGB_buffer, RGB_uncorrected, 0, True, False, (args.render_img is not None), args, s_colorspace, t_colorspace)

if __name__=='__main__':
    main(sys.argv)

import numpy as np
rgb_palette = True

palette = np.array([
 "333","014","006","326","403","503","510","420","320","120","031","040","022","000","000","000",
 "555","036","027","407","507","704","700","630","430","140","040","053","044","000","000","000",
 "777","357","447","637","707","737","740","750","660","360","070","276","077","000","000","000",
 "777","567","657","757","747","755","764","772","773","572","473","276","467","000","000","000"
])

if (rgb_palette):
    print("{|class=\"wikitable\"\n|-\n! style=\"text-align:center\"|\n! style=\"text-align:center\"|$x0\n! style=\"text-align:center\"|$x1\n! style=\"text-align:center\"|$x2\n! style=\"text-align:center\"|$x3\n! style=\"text-align:center\"|$x4\n! style=\"text-align:center\"|$x5\n! style=\"text-align:center\"|$x6\n! style=\"text-align:center\"|$x7\n! style=\"text-align:center\"|$x8\n! style=\"text-align:center\"|$x9\n! style=\"text-align:center\"|$xA\n! style=\"text-align:center\"|$xB\n! style=\"text-align:center\"|$xC\n! style=\"text-align:center\"|$xD\n! style=\"text-align:center\"|$xE\n! style=\"text-align:center\"|$xF")
    for luma in range(4):
        print("|-")
        print("! style=\"text-align:center\"|${0:0X}x".format(luma))
        for hue in range(16):
            color = int(str(palette[(luma * 16) + hue]), 16)
            color_r = int(255 * ((color >> 8) & 0x7) / 7)
            color_g = int(255 * ((color >> 4) & 0x7) / 7)
            color_b = int(255 * (color & 0x7) / 7)
            contrast = 0xFFF if ((color_r*299 + color_g*587 + color_b*114) <= 127500) else 0x000
            print("|style=\"border:0px;background-color:#{0:02X}{1:02X}{2:02X};width:32px;height:32px;color:#{3:03x};text-align:center\"|{4:03X}".format(
                color_r,
                color_g,
                color_b,
                contrast,
                color))
    print("|}")
else:
    print("{|class=\"wikitable\"")
    for luma in range(4):
        print("|-")
        for hue in range(16):
            color = int(str(palette[(luma * 16) + hue]), 16)
            color_r = int(255 * ((color >> 8) & 0x7) / 7)
            color_g = int(255 * ((color >> 4) & 0x7) / 7)
            color_b = int(255 * (color & 0x7) / 7)
            contrast = 0xFFF if ((color_r*299 + color_g*587 + color_b*114) <= 127500) else 0x000
            print("|style=\"border:0px;background-color:#{0:02X}{1:02X}{2:02X};width:32px;height:32px;color:#{3:03x};text-align:center\"|${4:02X}".format(
                color_r,
                color_g,
                color_b,
                contrast,
                ((luma << 4) + hue)))
    print("|}")

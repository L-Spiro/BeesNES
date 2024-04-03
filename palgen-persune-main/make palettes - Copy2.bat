py palgen_persune.py --skip-plot -o docs/example_palettes/2C02G_phs_aps_ela_NTSC-1953.pal -aps 5 -ela 0.01429 -phs -5.0 -blp 0.053571429 -rfc "NTSC (1953)" -e
py palgen_persune.py --skip-plot -o docs/example_palettes/2C02G_phs_aps_ela_NTSC-J.pal  -aps 5 -ela 0.01429 -phs -5.0 -rpr 0.618 0.350 -rpg 0.280 0.605 -rpb 0.152 0.063 -rpw 0.28314501 0.29711289 -e
py palgen_persune.py --skip-plot -o docs/example_palettes/2C02G_phs_aps_ela_NTSC.pal -aps 5 -ela 0.01429 -phs -5.0 -blp 0.053571429 -e
py palgen_persune.py --skip-plot -o docs/example_palettes/2C02G_aps_ela_NTSC_persune_tink.pal -aps 5 -ela 0.01429 -e -hue -2.5 -sat -0.1
py palgen_persune.py --skip-plot -o docs/example_palettes/2C02G_aps_ela_NTSC_persune_GVUSB2_NTSC_MJ.pal -aps 5 -ela 0.01429 -e -hue 5 -sat -0.4 -bri 0.02 -con 0.075
py palgen_persune.py --skip-plot -o docs/example_palettes/2C02G_aps_ela_NTSC_persune_GVUSB2_NTSC_M.pal -aps 5 -ela 0.01429 -e -hue 0 -sat -0.4 -blp 0.053571429
py palgen_persune.py --skip-plot -o docs/example_palettes/2C02-2C07_aps_ela_persune_neutral.pal -aps 5 -ela 0.01429 -e -hue 7.5 -sat -0.4 -blp 0.053571429
py palgen_persune.py --skip-plot -o docs/example_palettes/2C07_aps_ela_PAL.pal -ppu 2C07 -cbr 7 -aps 5 -ela 0.01429 -blp 0.053571429 -e

py palgen_persune.py --skip-plot -o docs/example_palettes/2C02-2C07_aps_ela_persune_neutral.fpal -aps 5 -ela 0.01429 -e -hue 7.5 -sat -0.4 -blp 0.053571429 -bri -0.02
py palgen_persune.py --skip-plot -o docs/example_palettes/2C07_aps_ela_PAL.fpal -ppu 2C07 -cbr 7 -aps 5 -ela 0.01429 -blp 0.053571429 -e -hue 1.25 -sat -0.33333333 -bri -0.015 -whp 0.8928571428571429


py palgen_persune.py --skip-plot -aps 5 -ela 0.01429 -e -o docs/example_palettes/2C02_aps_ela_default
py palgen_persune.py --skip-plot -aps 5 -ela 0.01429 -e -hue 3.75 -sat 0.8 -o docs/example_palettes/2C02-2C07_aps_ela_persune_neutral
py palgen_persune.py --skip-plot -aps 5 -ela 0.01429 -phs -5.0 -blp 7.5 -e -o docs/example_palettes/2C02G_phs_aps_ela_NTSC
py palgen_persune.py --skip-plot -aps 5 -ela 0.01429 -phs -5.0 -blp 7.5 -rfc "NTSC (1953)" -e -o docs/example_palettes/2C02G_phs_aps_ela_NTSC-1953
py palgen_persune.py --skip-plot -aps 5 -ela 0.01429 -phs -5.0 -rpr 0.618 0.350 -rpg 0.280 0.605 -rpb 0.152 0.063 -rpw 0.28314501 0.29711289 -e -o docs/example_palettes/2C02G_phs_aps_ela_NTSC-J
py palgen_persune.py --skip-plot -aps 5 -ela 0.01429 -e -hue -2.5 -sat 0.9 -o docs/example_palettes/2C02G_aps_ela_NTSC_persune_tink
py palgen_persune.py --skip-plot -aps 5 -ela 0.01429 -e -o docs/example_palettes/2C02G_aps_ela_NTSC_persune_GVUSB2_NTSC_M_J -phs -5 -hue -12 -sat 0.8
py palgen_persune.py --skip-plot -aps 5 -ela 0.01429 -e -o docs/example_palettes/2C02G_aps_ela_NTSC_persune_GVUSB2_NTSC_M -phs -5 -hue -12 -sat 0.8 -gai -6.5 -blp 6
py palgen_persune.py --skip-plot -ela 0.01429 -ppu "2C07" -blp 7.5 -e -o docs/example_palettes/2C07_ela_PAL
py palgen_persune.py --skip-plot -ppu "2C03" -rpr 0.622 0.338 -rpg 0.343 0.590 -rpb 0.153 0.059 -rpw 0.28314501 0.29711289 -e -o docs/example_palettes/2C03_DeMarsh_1980s_RGB
py palgen_persune.py --skip-plot -ppu "2C05-99" -sat 0.8 -e -o docs/example_palettes/2C05-99_composite_forple

py palgen_persune.py --skip-plot -o docs/example_palettes/2C02-2C07_aps_ela_persune_neutral_noEO.pal -aps 5 -ela 0.01429 -e -blp 7.5 -whp 100 -hue 8.0 -sat 0.5 -bri -0.33622191294189625174291745443606 --opto-electronic-disable --electro-optic-disable -f ".pal double"
py palgen_persune.py --skip-plot -o docs/example_palettes/2C07_aps_ela_noOE_PAL.pal -ppu 2C07 -aps 5 -ela 0.01429 -e -hue 1.25 -sat -0.33333333 -bri -0.015 -whp 0.8928571428571429 --opto-electronic-disable --electro-optic-disable -f ".pal double"

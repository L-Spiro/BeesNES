py palgen-persune.py --skip-plot -o docs/example_palettes/2C02G_phs_aps_ela_NTSC-1953.pal -aps 5 -ela 0.01429 -phs -5.0 -blp 0.053571429 -rfc "NTSC (1953)" -e
py palgen-persune.py --skip-plot -o docs/example_palettes/2C02G_phs_aps_ela_NTSC-J.pal -aps 5 -ela 0.01429 -phs -5.0 -rpr 0.618 0.350 -rpg 0.280 0.605 -rpb 0.152 0.063 -rpw 0.28314501 0.29711289 -e
py palgen-persune.py --skip-plot -o docs/example_palettes/2C02G_phs_aps_ela_NTSC.pal -aps 5 -ela 0.01429 -phs -5.0 -blp 0.053571429 -e
py palgen-persune.py --skip-plot -o docs/example_palettes/2C02G_aps_ela_NTSC_persune_tink.pal -aps 5 -ela 0.01429 -e -hue -2.5 -sat -0.1
py palgen-persune.py --skip-plot -o docs/example_palettes/2C02G_aps_ela_NTSC_persune_GVUSB2_NTSC_MJ.pal -aps 5 -ela 0.01429 -e -hue 5 -sat -0.4 -bri 0.02 -con 0.075
py palgen-persune.py --skip-plot -o docs/example_palettes/2C02G_aps_ela_NTSC_persune_GVUSB2_NTSC_M.pal -aps 5 -ela 0.01429 -e -hue 0 -sat -0.4 -blp 0.053571429
py palgen-persune.py --skip-plot -o docs/example_palettes/2C02-2C07_aps_ela_persune_neutral.pal -aps 5 -ela 0.01429 -e -hue 7.5 -sat -0.4 -blp 0.053571429
py palgen-persune.py --skip-plot -o docs/example_palettes/2C07_aps_ela_PAL.pal -pal -cbr 7 -aps 5 -ela 0.01429 -blp 0.053571429 -e

py palgen-persune.py --skip-plot --float-pal docs/example_palettes/2C02G_phs_aps_ela_float_NTSC-1953.fpal -aps 5 -ela 0.01429 -phs -5.0 -blp 0.053571429 -rfc "NTSC (1953)" -e
py palgen-persune.py --skip-plot --float-pal docs/example_palettes/2C02G_phs_aps_ela_float_NTSC-J.fpal -aps 5 -ela 0.01429 -phs -5.0 -rpr 0.618 0.350 -rpg 0.280 0.605 -rpb 0.152 0.063 -rpw 0.28314501 0.29711289 -e
py palgen-persune.py --skip-plot --float-pal docs/example_palettes/2C02G_phs_aps_ela_float_NTSC.fpal -aps 5 -ela 0.01429 -phs -5.0 -blp 0.053571429 -e
py palgen-persune.py --skip-plot --float-pal docs/example_palettes/2C02G_aps_ela_NTSC_float_persune_tink.fpal -aps 5 -ela 0.01429 -e -hue -2.5 -sat -0.1
py palgen-persune.py --skip-plot --float-pal docs/example_palettes/2C02G_aps_ela_NTSC_float_persune_GVUSB2_NTSC_MJ.fpal -aps 5 -ela 0.01429 -e -hue 5 -sat -0.4 -bri 0.02 -con 0.075
py palgen-persune.py --skip-plot --float-pal docs/example_palettes/2C02G_aps_ela_NTSC_float_persune_GVUSB2_NTSC_M.fpal -aps 5 -ela 0.01429 -e -hue 0 -sat -0.4 -blp 0.053571429
py palgen-persune.py --skip-plot --float-pal docs/example_palettes/2C02-2C07_aps_ela_float_persune_neutral.fpal -aps 5 -ela 0.01429 -e -hue 7.5 -sat -0.4 -blp 0.053571429
py palgen-persune.py --skip-plot --float-pal docs/example_palettes/2C07_aps_ela_float_applysrgb_PAL.fpal -pal -cbr 7 -aps 5 -ela 0.01429 -blp 0.053571429 -e -hue 1.25 -sat -0.33333333 -bri -0.015 -whp 0.8928571428571429 --linear-light

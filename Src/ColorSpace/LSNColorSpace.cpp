/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Colorspace functionality.
 */


#include "LSNColorSpace.h"
#include "../Localization/LSNLocalization.h"
#include "../Math/LSNMatrix4x4.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <format>
#include <iterator>
#include <numbers>

namespace lsn {

	// == Members.
	float CColorSpace::m_fSmpte[8] = {																/**< Chromaticities and white point for SMPTE 170M, SMPTE 240M, BT.601 525. */
		0.630f, 0.340f,
		0.310f, 0.595f,
		0.155f, 0.070f,
		0.3127f, 0.3290f
	};

	float CColorSpace::m_f1953[8] = {																/**< Chromaticities and white point for NTSC 1953. */
		0.670f, 0.330f,
		0.210f, 0.710f,
		0.140f, 0.080f,
		0.3100f, 0.3160f
	};

	float CColorSpace::m_fBt2020[8] = {																/**< Chromaticities and white point for BT.2020. */
		0.708f, 0.292f,
		0.170f, 0.797f,
		0.131f, 0.046f,
		0.3127f, 0.3290f
	};

	float CColorSpace::m_fBt709[8] = {																/**< Chromaticities and white point for BT.709 (and sRGB). */
		0.640f, 0.330f,
		0.300f, 0.600f,
		0.150f, 0.060f,
		0.3127f, 0.3290f
	};

	float CColorSpace::m_fBt601_625[8] = {															/**< Chromaticities and white point for BT.601 625. */
		0.640f, 0.330f,
		0.290f, 0.600f,
		0.150f, 0.060f,
		0.3127f, 0.3290f
	};

	float CColorSpace::m_fBt470_NTSC[8] = {															/**< Chromaticities and white point for BT.470 NTSC. */
		0.670f, 0.330f,
		0.210f, 0.710f,
		0.140f, 0.080f,
		0.3100f, 0.3160f
	};
	float CColorSpace::m_fBt470_PAL[8] = {															/**< Chromaticities and white point for BT.470 PAL. */
		0.640f, 0.330f,
		0.290f, 0.600f,
		0.150f, 0.060f,
		0.3127f, 0.3290f
	};

	float CColorSpace::m_fsRgb[8] = {																/**< Chromaticities and white point for sRGB. */
		0.640f, 0.330f,
		0.300f, 0.600f,
		0.150f, 0.060f,
		0.3127f, 0.3290f
	};

	float CColorSpace::m_fNtscJ[8] = {																/**< Chromaticities and white point for NTSC-J. */
		0.618f, 0.350f,
		0.280f, 0.605f,
		0.152f, 0.063f,
		0.28315f, 0.29711f
	};

	float CColorSpace::m_fToshibaCF2005[8] = {														/**< Chromaticities and white point for a Toshiba CF2005 as measured by patchy68k: https://forums.nesdev.org/viewtopic.php?p=302438. */
		0.642388f, 0.339424f,
		0.301f, 0.601f,
		0.143599f, 0.070975f,
		0.281f, 0.311f
	};

	float CColorSpace::m_fNtscJ_Sony_PVM_20M2U[8] = {												/**< Chromaticities and white point for NTSC-J ala Sony PVM 20M2U. */
		0.630f, 0.345f,
		0.285f, 0.605f,
		0.150f, 0.065f,
		0.2838f, 0.2984f
	};

	float CColorSpace::m_fNtscJ_Sony_PVM_20L2MDU[8] = {												/**< Chromaticities and white point for NTSC-J ala Sony PVM 20L2MDU. */
		0.625f, 0.345f,
		0.280f, 0.605f,
		0.150f, 0.065f,
		0.2838f, 0.2984f
	};

	float CColorSpace::m_fNtscJ_Sony_P22_Trinitron[8] = {											/**< Chromaticities and white point for NTSC-J ala Sony Trinitron P22. */
		0.621f, 0.340f,
		0.281f, 0.606f,
		0.152f, 0.067f,
		0.2838f, 0.2984f
	};

	// The following come from gamutthingy (https://github.com/ChthonVII/gamutthingy/blob/master/src/constants.h).
	float CColorSpace::m_fToshiba1992[8] = {														/**< Chromaticities and white point for a typical Toshiba television (1992 Toshiba patent). */
		// "The phosphor in a cathode ray tube (CRT) used in the typical television receiver":  https://patents.google.com/patent/US5301017A
		0.657f, 0.338f,
		0.297f, 0.609f,
		0.148f, 0.054f,
		// 9300K + 27 MPCD.
		0.281f, 0.311f
	};

	float CColorSpace::m_fMatsushita1979[8] = {														/**< Chromaticities and white point for a typical Matsushita television (1979 Matsushita patent). */
		// "Typical chromaticities of phosphors... in a recent television receiver":  https://patents.google.com/patent/US4167750A
		0.631f, 0.347f,
		0.268f, 0.585f,
		0.150f, 0.071f,
		// 9300K + 27 MPCD (paired with these phosphors in the patent).
		0.281f, 0.311f
	};

	float CColorSpace::m_fRcaColorTrak[8] = {														/**< Chromaticities and white point for an RCA ColorTrak Remote E13169GM as measured by patchy68k. */
		// 1989, US model.  https://github.com/ChthonVII/gamutthingy/issues/1#issuecomment-2672961597
		0.623492333f, 0.343975233f,
		0.312266967f, 0.590305267f,
		0.153586233f, 0.064531667f,
		// Measured (near D75).
		0.301f, 0.308f
	};

	float CColorSpace::m_fSonyGdm17Se1[8] = {														/**< Chromaticities and white point for a Sony GDM-17SE1 Trinitron monitor as measured by Bohnsack et al. */
		// 1994 model, measured in 1997.  https://www.scholars.northwestern.edu/en/publications/characteristics-of-the-sony-multiscan-17se-trinitron-color-graphi
		0.6233f, 0.3390f,
		0.2838f, 0.5895f,
		0.1519f, 0.0669f,
		// Measured (probably meant to be 9300K + 8 MPCD).
		0.2836f, 0.2963f
	};

	float CColorSpace::m_fAppleMultipleScan1705[8] = {												/**< Chromaticities and white point for an Apple Multiple Scan 1705 monitor. */
		// 1995-1997, manufacturer data (+/- 0.015).  https://til-2001.mirror.kb1max.com/techinfo.nsf/artnum/n24445/index.html
		0.625f, 0.340f,
		0.285f, 0.605f,
		0.150f, 0.065f,
		// 9300K + 8 MPCD (Judd MPCD, as measured by Yagishita et al.).
		0.2838f, 0.2984f
	};

	float CColorSpace::m_fNecMultiSyncC400[8] = {													/**< Chromaticities and white point for an NEC MultiSync C400 monitor. */
		// https://www.cs.ucf.edu/courses/cap5725/spring2003/Chromaticity.htm
		0.610f, 0.350f,
		0.307f, 0.595f,
		0.150f, 0.065f,
		// Measured (probably meant to be 9300K + 27 MPCD).
		0.280f, 0.315f
	};

	float CColorSpace::m_fKdsVs19[8] = {															/**< Chromaticities and white point for a KDS VS19 monitor. */
		// https://www.cs.ucf.edu/courses/cap5725/spring2003/Chromaticity.htm
		0.625f, 0.340f,
		0.285f, 0.605f,
		0.150f, 0.065f,
		// Measured (9300K + 27 MPCD).
		0.281f, 0.311f
	};

	// The following were harvested from CustomPalette.java and from measurements found online.
	float CColorSpace::m_fToshibaCF2005Gm[8] = {													/**< Chromaticities and white point for a Toshiba Blackstripe CF2005 (US, 1985):  the geometric mean of 10 samples by PlainOldPants. */
		// https://forums.libretro.com/t/plainoldpantss-shader-presets/45498?page=5
		0.638994661420802f, 0.34054912704359f,
		0.302680061977106f, 0.611123488970143f,
		0.143735472664549f, 0.0703919024997988f,
		// Not measured (9300K + 27 MPCD, as the other CF2005 preset).
		0.281f, 0.311f
	};

	float CColorSpace::m_fRcaColorTrak8200K[8] = {													/**< Chromaticities and white point for an RCA ColorTrak Remote E13169GM (US, 1989) with an 8200K white point. */
		// Phosphors measured by patchy68k:  https://github.com/ChthonVII/gamutthingy/issues/1#issuecomment-2672961597
		0.623492333f, 0.343975233f,
		0.312266967f, 0.590305267f,
		0.153586233f, 0.064531667f,
		// 8200K (D-white), as paired with this television in CustomPalette.java:  https://ieeexplore.ieee.org/document/234645
		0.294f, 0.309f
	};

	float CColorSpace::m_fSonyKv27S22[8] = {														/**< Chromaticities and white point for a Sony Trinitron KV-27S22 (US, 1997). */
		// Trinitron phosphors (manufacturer reference data, +/- 0.03):  https://www.color.org/wpaper1.xalter
		0.621f, 0.340f,
		0.281f, 0.606f,
		0.152f, 0.067f,
		// 9300K + 27 MPCD (guessed and checked by PlainOldPants):  https://forums.libretro.com/t/correct-color-some-evidence-related-to-the-nes/48914/99
		0.281f, 0.311f
	};

	float CColorSpace::m_fSonyTrinitron9300K27[8] = {												/**< Chromaticities and white point for a Sony Trinitron (Sony Multiscan 17se phosphors) with a 9300K + 27 MPCD white point. */
		// Bohnsack et al., "Characteristics of the Sony Multiscan 17se Trinitron color graphic display," Spatial Vision 10(4), 1997, as paired in CustomPalette.java.
		0.6233f, 0.3390f,
		0.2838f, 0.5895f,
		0.1519f, 0.0669f,
		// 9300K + 27 MPCD:  https://ieeexplore.ieee.org/document/4319950
		0.281f, 0.311f
	};

	float CColorSpace::m_fSonyTrinitronD65[8] = {													/**< Chromaticities and white point for a Sony Trinitron (Sony Multiscan 17se phosphors) with a D65 white point. */
		// Bohnsack et al. (1997), as paired in CustomPalette.java.
		0.6233f, 0.3390f,
		0.2838f, 0.5895f,
		0.1519f, 0.0669f,
		// D65.
		0.3127f, 0.3290f
	};

	float CColorSpace::m_fSonyTrinitronD93[8] = {													/**< Chromaticities and white point for a Sony Trinitron (Sony Multiscan 17se phosphors) with a 9300K + 8 MPCD white point. */
		// Bohnsack et al. (1997), as paired in CustomPalette.java.
		0.6233f, 0.3390f,
		0.2838f, 0.5895f,
		0.1519f, 0.0669f,
		// 9300K + 8 MPCD (Nagaoka, 1979):  https://www.jstage.jst.go.jp/article/itej1978/33/12/33_12_1013/_pdf/-char/ja
		0.2831f, 0.2971f
	};

	float CColorSpace::m_fSonyBvm20F1U[8] = {														/**< Chromaticities and white point for a Sony BVM-20F1U broadcast monitor (US, SMPTE C phosphors), measured. */
		// Measured with a Konica Minolta CS-2000:  https://reposit.haw-hamburg.de/bitstream/20.500.12738/19790/1/BA_Redefinition_of_current_display_colour.pdf
		// Only the green was measured on this unit; the red and blue are those of the BVM-20F1E measured alongside it.
		0.6381f, 0.3317f,
		0.3087f, 0.5985f,
		0.1502f, 0.05802f,
		// Calibrated to D65.
		0.3127f, 0.3290f
	};

	float CColorSpace::m_fSonyBvm20F1E[8] = {														/**< Chromaticities and white point for a Sony BVM-20F1E broadcast monitor (PAL, EBU phosphors), measured. */
		// Measured with a Konica Minolta CS-2000:  https://reposit.haw-hamburg.de/bitstream/20.500.12738/19790/1/BA_Redefinition_of_current_display_colour.pdf
		0.6381f, 0.3317f,
		0.2918f, 0.6021f,
		0.1502f, 0.05802f,
		// Calibrated to D65.
		0.3127f, 0.3290f
	};

	float CColorSpace::m_fSonyTrinitronPal6900K[8] = {												/**< Chromaticities and white point for a PAL Sony Trinitron television as measured by DariusG. */
		// The middles of the measured ranges:  https://forums.libretro.com/t/dogways-grading-shader-slang/27148/524
		0.635f, 0.325f,
		0.290f, 0.585f,
		0.1525f, 0.0575f,
		// Measured 6900K (the coordinates of 6900K on the daylight locus, from gamutthingy).
		0.306769f, 0.322990f
	};

	float CColorSpace::m_fPanasonicCt36D30BD65[8] = {												/**< Chromaticities and white point for a Panasonic CT-36D30B (US, 2000) with a D65 white point. */
		// Phosphors measured by patchy68k (from CustomPalette.java):  https://github.com/ChthonVII/gamutthingy/issues/4
		0.651212890899489f, 0.327170850978215f,
		0.311678021451133f, 0.607717178761663f,
		0.15033614521611f, 0.0665766255634717f,
		// D65 (as paired in CustomPalette.java).
		0.3127f, 0.3290f
	};

	float CColorSpace::m_fPanasonicCt36D30BWarm[8] = {												/**< Chromaticities and white point for a Panasonic CT-36D30B (US, 2000) on its Warm setting. */
		// Phosphors measured by patchy68k (from CustomPalette.java):  https://github.com/ChthonVII/gamutthingy/issues/4
		0.651212890899489f, 0.327170850978215f,
		0.311678021451133f, 0.607717178761663f,
		0.15033614521611f, 0.0665766255634717f,
		// Measured Warm (about 7100K):  https://forums.libretro.com/t/plainoldpantss-shader-presets/45498?page=5
		0.3057f, 0.3145f
	};

	float CColorSpace::m_fPanasonicCt36D30BNormal[8] = {											/**< Chromaticities and white point for a Panasonic CT-36D30B (US, 2000) on its Normal setting. */
		// Phosphors measured by patchy68k (from CustomPalette.java):  https://github.com/ChthonVII/gamutthingy/issues/4
		0.651212890899489f, 0.327170850978215f,
		0.311678021451133f, 0.607717178761663f,
		0.15033614521611f, 0.0665766255634717f,
		// Measured Normal (about 9800K):  https://forums.libretro.com/t/plainoldpantss-shader-presets/45498?page=5
		0.2801f, 0.2922f
	};

	const CColorSpace::LSN_COLOR_SPACE_DATA CColorSpace::m_csdData[LSN_CSP_TOTAL] = {				/**< The information about each color space, in LSN_COLOR_SPACE order (pointer, name, ID, TV-preset saturation, is a TV preset, TV-preset decoder chip). */
		{ m_fsRgb,								LSN_LSTR( LSN_STR_CSP_SRGB ),							"srgb",								1.0f,		false,		LSN_DM_NONE },
		{ m_fBt709,								LSN_LSTR( LSN_STR_CSP_BT709 ),							"bt709",							1.0f,		false,		LSN_DM_NONE },
		{ m_fBt2020,							LSN_LSTR( LSN_STR_CSP_BT2020 ),							"bt2020",							1.0f,		false,		LSN_DM_NONE },
		{ m_fSmpte,								LSN_LSTR( LSN_STR_CSP_SMPTE_C ),						"smpte_c",							1.0f,		false,		LSN_DM_NONE },
		{ m_f1953,								LSN_LSTR( LSN_STR_CSP_NTSC_1953 ),						"ntsc_1953",						1.0f,		false,		LSN_DM_NONE },
		{ m_fBt470_NTSC,						LSN_LSTR( LSN_STR_CSP_BT470_NTSC ),						"bt470_ntsc",						1.0f,		false,		LSN_DM_NONE },
		{ m_fNtscJ,								LSN_LSTR( LSN_STR_CSP_NTSC_J ),							"ntsc_j",							1.0f,		false,		LSN_DM_NONE },
		{ m_fBt470_PAL,							LSN_LSTR( LSN_STR_CSP_BT470_PAL ),						"bt470_pal",						1.0f,		false,		LSN_DM_NONE },
		{ m_fBt601_625,							LSN_LSTR( LSN_STR_CSP_BT601_625 ),						"bt601_625",						1.0f,		false,		LSN_DM_NONE },
		{ m_fToshibaCF2005,						LSN_LSTR( LSN_STR_CSP_TOSHIBA_CF2005 ),					"toshiba_cf2005",					1.1155f,	true,		LSN_DM_TA7644BP },
		{ m_fToshibaCF2005Gm,					LSN_LSTR( LSN_STR_CSP_TOSHIBA_CF2005_GM ),				"toshiba_cf2005_gm",				1.0f,		true,		LSN_DM_TA7644BP_MEASURED },
		{ m_fRcaColorTrak,						LSN_LSTR( LSN_STR_CSP_RCA_COLORTRAK ),					"rca_colortrak_e13169gm",			1.0f,		true,		LSN_DM_RCA_COLORTRAK },
		{ m_fRcaColorTrak8200K,					LSN_LSTR( LSN_STR_CSP_RCA_COLORTRAK_8200K ),			"rca_colortrak_e13169gm_8200k",		1.2f,		true,		LSN_DM_RCA_COLORTRAK },
		{ m_fSonyKv27S22,						LSN_LSTR( LSN_STR_CSP_SONY_KV_27S22 ),					"sony_kv_27s22",					1.0f,		true,		LSN_DM_CXA2025AS_US },
		{ m_fSonyTrinitron9300K27,				LSN_LSTR( LSN_STR_CSP_SONY_TRINITRON_CXA1870S_US ),		"sony_trinitron_cxa1870s_us",		1.2f,		true,		LSN_DM_CXA1870S_US },
		{ m_fSonyTrinitronD65,					LSN_LSTR( LSN_STR_CSP_SONY_PVM_CXA2060BS_US ),			"sony_pvm_cxa2060bs_us",			1.0f,		true,		LSN_DM_CXA2060BS_US },
		{ m_fNtscJ_Sony_PVM_20M2U,				LSN_LSTR( LSN_STR_CSP_SONY_PVM_20M2U ),					"sony_pvm_20m2u",					1.0f,		true,		LSN_DM_CXA2060BS_US },
		{ m_fNtscJ_Sony_PVM_20L2MDU,			LSN_LSTR( LSN_STR_CSP_SONY_PVM_20L2MDU ),				"sony_pvm_20l2mdu",					1.0f,		true,		LSN_DM_CXA2060BS_US },
		{ m_fSonyBvm20F1U,						LSN_LSTR( LSN_STR_CSP_SONY_BVM_20F1U ),					"sony_bvm_20f1u",					1.0f,		true,		LSN_DM_NONE },
		{ m_fPanasonicCt36D30BD65,				LSN_LSTR( LSN_STR_CSP_PANASONIC_CT_36D30B_D65 ),		"panasonic_ct_36d30b_d65",			1.0f,		true,		LSN_DM_PANASONIC_CT_36D30B },
		{ m_fPanasonicCt36D30BWarm,				LSN_LSTR( LSN_STR_CSP_PANASONIC_CT_36D30B_WARM ),		"panasonic_ct_36d30b_warm",			1.0f,		true,		LSN_DM_PANASONIC_CT_36D30B },
		{ m_fPanasonicCt36D30BNormal,			LSN_LSTR( LSN_STR_CSP_PANASONIC_CT_36D30B_NORMAL ),		"panasonic_ct_36d30b_normal",		1.0f,		true,		LSN_DM_PANASONIC_CT_36D30B },
		{ m_fNtscJ_Sony_P22_Trinitron,			LSN_LSTR( LSN_STR_CSP_SONY_P22_TRINITRON ),				"sony_trinitron_p22",				1.0f,		true,		LSN_DM_CXA1464AS_JP },
		{ m_fSonyTrinitron9300K27,				LSN_LSTR( LSN_STR_CSP_SONY_TRINITRON_CXA1870S_JP ),		"sony_trinitron_cxa1870s_jp",		1.2f,		true,		LSN_DM_CXA1870S_JP },
		{ m_fSonyTrinitronD93,					LSN_LSTR( LSN_STR_CSP_SONY_PVM_CXA2060BS_JP ),			"sony_pvm_cxa2060bs_jp",			1.0f,		true,		LSN_DM_CXA2060BS_JP },
		{ m_fToshiba1992,						LSN_LSTR( LSN_STR_CSP_TOSHIBA_1992 ),					"toshiba_1992",						1.0f,		true,		LSN_DM_TA8801AN },
		{ m_fMatsushita1979,					LSN_LSTR( LSN_STR_CSP_MATSUSHITA_1979 ),				"matsushita_1979",					1.0f,		true,		LSN_DM_MATSUSHITA_1979 },
		{ m_fSonyTrinitronD65,					LSN_LSTR( LSN_STR_CSP_SONY_PVM_CXA2060BS_PAL ),			"sony_pvm_cxa2060bs_pal",			1.0f,		true,		LSN_DM_CXA2060BS_PAL },
		{ m_fSonyBvm20F1E,						LSN_LSTR( LSN_STR_CSP_SONY_BVM_20F1E ),					"sony_bvm_20f1e",					1.0f,		true,		LSN_DM_NONE },
		{ m_fSonyTrinitronPal6900K,				LSN_LSTR( LSN_STR_CSP_SONY_TRINITRON_PAL_6900K ),		"sony_trinitron_pal_6900k",			1.0f,		true,		LSN_DM_NONE },
		{ m_fSonyGdm17Se1,						LSN_LSTR( LSN_STR_CSP_SONY_GDM_17SE1 ),					"sony_gdm_17se1",					1.0f,		false,		LSN_DM_NONE },
		{ m_fAppleMultipleScan1705,				LSN_LSTR( LSN_STR_CSP_APPLE_MULTIPLE_SCAN_1705 ),		"apple_multiple_scan_1705",			1.0f,		false,		LSN_DM_NONE },
		{ m_fNecMultiSyncC400,					LSN_LSTR( LSN_STR_CSP_NEC_MULTISYNC_C400 ),				"nec_multisync_c400",				1.0f,		false,		LSN_DM_NONE },
		{ m_fKdsVs19,							LSN_LSTR( LSN_STR_CSP_KDS_VS19 ),						"kds_vs19",							1.0f,		false,		LSN_DM_NONE },
	};

	/** The information about each decoder chip, in LSN_DEMODULATOR order.  From gamutthingy (https://github.com/ChthonVII/gamutthingy/blob/master/src/constants.h):
	 *	datasheets, and measurements by patchy68k.  LSN_DM_NONE lists the standard NTSC axes (DemodulatorMatrix() uses the exact standard coefficients for it). */
	const CColorSpace::LSN_DEMODULATOR_DATA CColorSpace::m_ddDemodulators[LSN_DM_TOTAL] = {
		// Name											ID							R-Y, G-Y, and B-Y angles				R-Y, G-Y, and B-Y gains
		{ LSN_LSTR( LSN_STR_DM_NONE ),					"none",						{ 90.0f, 236.0f, 0.0f },				{ 0.56f, 0.34f, 1.0f } },
		{ LSN_LSTR( LSN_STR_DM_CXA1464AS_JP ),			"cxa1464as_jp",				{ 98.0f, 243.0f, 0.0f },				{ 0.78f, 0.31f, 1.0f } },								// Trinitron ~1993-1995.
		{ LSN_LSTR( LSN_STR_DM_CXA1465AS_US ),			"cxa1465as_us",				{ 114.0f, 255.0f, 0.0f },				{ 0.78f, 0.31f, 1.0f } },								// Trinitron ~1993-1995.
		{ LSN_LSTR( LSN_STR_DM_CXA1870S_JP ),			"cxa1870s_jp",				{ 96.0f, 240.0f, 0.0f },				{ 0.80f, 0.30f, 1.0f } },								// Trinitron ~1996.
		{ LSN_LSTR( LSN_STR_DM_CXA1870S_US ),			"cxa1870s_us",				{ 105.0f, 252.0f, 0.0f },				{ 0.80f, 0.30f, 1.0f } },								// Trinitron ~1996.
		{ LSN_LSTR( LSN_STR_DM_CXA2025AS_JP ),			"cxa2025as_jp",				{ 95.0f, 240.0f, 0.0f },				{ 0.78f, 0.30f, 1.0f } },								// Trinitron ~1997.
		{ LSN_LSTR( LSN_STR_DM_CXA2025AS_US ),			"cxa2025as_us",				{ 112.0f, 252.0f, 0.0f },				{ 0.83f, 0.30f, 1.0f } },								// Trinitron ~1997.
		{ LSN_LSTR( LSN_STR_DM_CXA2060BS_JP ),			"cxa2060bs_jp",				{ 95.0f, 236.0f, 0.0f },				{ 0.78f, 0.33f, 1.0f } },								// Trinitron PVM ~1998.
		{ LSN_LSTR( LSN_STR_DM_CXA2060BS_US ),			"cxa2060bs_us",				{ 102.0f, 236.0f, 0.0f },				{ 0.78f, 0.30f, 1.0f } },								// Trinitron PVM ~1998.
		{ LSN_LSTR( LSN_STR_DM_CXA2060BS_PAL ),			"cxa2060bs_pal",			{ 90.0f, 227.0f, 0.0f },				{ float( 0.492111 / 0.877283 ), 0.34f, 1.0f } },		// Trinitron PVM ~1998.
		{ LSN_LSTR( LSN_STR_DM_CXA1213AS ),				"cxa1213as",				{ 99.0f, 240.0f, 11.0f },				{ 0.77f, 0.30f, 1.0f } },								// Trinitron(?) ~1992.
		{ LSN_LSTR( LSN_STR_DM_TA7644BP ),				"ta7644bp",					{ 107.0f, 240.0f, 0.0f },				{ 0.95f, 0.31f, 1.0f } },								// Blackstripe CF2005 (1985), datasheet.
		{ LSN_LSTR( LSN_STR_DM_TA7644BP_MEASURED ),		"ta7644bp_measured",		{ 108.9f, 243.3f, 0.0f },				{ 0.996f, 0.351f, 1.0f } },								// Blackstripe CF2005 (1985), measured by patchy68k (deteriorated unit).
		{ LSN_LSTR( LSN_STR_DM_TA7698AP ),				"ta7698ap",					{ 105.0f, 235.0f, 0.0f },				{ 1.0f, 0.38f, 1.0f } },								// ~1988 (JVC TM-9U).
		{ LSN_LSTR( LSN_STR_DM_TA8801AN ),				"ta8801an",					{ 95.0f, 240.0f, 0.0f },				{ 0.84f, 0.31f, 1.0f } },								// CF-3272B (1993).
		{ LSN_LSTR( LSN_STR_DM_TA8867AN ),				"ta8867an",					{ 112.0f, 237.0f, 0.0f },				{ 0.84f, 0.33f, 1.0f } },								// CE-20D10 (1994).
		{ LSN_LSTR( LSN_STR_DM_TA8867BN ),				"ta8867bn",					{ 104.0f, 240.0f, 0.0f },				{ 0.91f, 0.31f, 1.0f } },
		{ LSN_LSTR( LSN_STR_DM_TDA8362 ),				"tda8362",					{ 100.0f, 235.0f, -10.0f },				{ 1.14f, 0.30f, 1.14f } },								// Hitachi CMT2187/2196/2198/2199.
		{ LSN_LSTR( LSN_STR_DM_LA7655N ),				"la7655n",					{ 105.0f, 240.0f, 0.0f },				{ 0.90f, 0.30f, 1.0f } },								// ~1989 (Mitsubishi?).
		{ LSN_LSTR( LSN_STR_DM_RCA_COLORTRAK ),			"rca_colortrak_e13169gm",	{ 94.5488524399f, 255.376054365f, 0.0f },	{ 0.806809988011f, 0.295471057738f, 1.0f } },		// Unknown chip, 1989, measured by patchy68k.
		{ LSN_LSTR( LSN_STR_DM_PANASONIC_CT_36D30B ),	"panasonic_ct_36d30b",		{ 101.580982936f, 232.937328893f, 0.0f },	{ 0.91062624791989f, 0.388131199173125f, 1.0f } },	// Unknown chip, 2000, measured by patchy68k.
		{ LSN_LSTR( LSN_STR_DM_MATSUSHITA_1979 ),		"matsushita_1979",			{ 100.0f, 237.0f, 0.0f },				{ float( 2.5 / 2.65 ), float( 0.75 / 2.65 ), 1.0f } },	// 1979 Matsushita patent (US4167750A).
		{ LSN_LSTR( LSN_STR_DM_TDA884X_JP ),			"tda884x_jp",				{ 95.0f, 240.0f, 0.0f },				{ float( 1.59 / 2.03 ), float( 0.61 / 2.03 ), 1.0f } },	// TDA8840/41/42/43/44/46/47 datasheet (MAT = 0).
		{ LSN_LSTR( LSN_STR_DM_TDA884X_US ),			"tda884x_us",				{ 99.0f, 223.0f, -1.0f },				{ float( 1.53 / 2.20 ), float( 0.70 / 2.20 ), 1.0f } },	// TDA8846/47/57 datasheet (MAT = 1).
	};

	/**
	 * Multiplies 2 3-by-3 matrices (_dRet = _dA * _dB).
	 *
	 * \param _dA The left matrix.
	 * \param _dB The right matrix.
	 * \param _dRet Holds the returned product.  Must not be either input.
	 */
	static void Multiply3x3( const double _dA[3][3], const double _dB[3][3], double _dRet[3][3] ) {
		for ( size_t I = 0; I < 3; ++I ) {
			for ( size_t J = 0; J < 3; ++J ) {
				_dRet[I][J] = _dA[I][0] * _dB[0][J] + _dA[I][1] * _dB[1][J] + _dA[I][2] * _dB[2][J];
			}
		}
	}

	/**
	 * Inverts a 3-by-3 matrix.
	 *
	 * \param _dM The matrix to invert.
	 * \param _dRet Holds the returned inverse (the identity matrix if _dM is singular).
	 */
	static void Invert3x3( const double _dM[3][3], double _dRet[3][3] ) {
		const double dDet = _dM[0][0] * (_dM[1][1] * _dM[2][2] - _dM[1][2] * _dM[2][1]) -
			_dM[0][1] * (_dM[1][0] * _dM[2][2] - _dM[1][2] * _dM[2][0]) +
			_dM[0][2] * (_dM[1][0] * _dM[2][1] - _dM[1][1] * _dM[2][0]);
		if ( dDet == 0.0 ) {
			for ( size_t I = 0; I < 3; ++I ) {
				for ( size_t J = 0; J < 3; ++J ) { _dRet[I][J] = (I == J) ? 1.0 : 0.0; }
			}
			return;
		}
		const double dInvDet = 1.0 / dDet;
		_dRet[0][0] = (_dM[1][1] * _dM[2][2] - _dM[1][2] * _dM[2][1]) * dInvDet;
		_dRet[0][1] = (_dM[0][2] * _dM[2][1] - _dM[0][1] * _dM[2][2]) * dInvDet;
		_dRet[0][2] = (_dM[0][1] * _dM[1][2] - _dM[0][2] * _dM[1][1]) * dInvDet;
		_dRet[1][0] = (_dM[1][2] * _dM[2][0] - _dM[1][0] * _dM[2][2]) * dInvDet;
		_dRet[1][1] = (_dM[0][0] * _dM[2][2] - _dM[0][2] * _dM[2][0]) * dInvDet;
		_dRet[1][2] = (_dM[0][2] * _dM[1][0] - _dM[0][0] * _dM[1][2]) * dInvDet;
		_dRet[2][0] = (_dM[1][0] * _dM[2][1] - _dM[1][1] * _dM[2][0]) * dInvDet;
		_dRet[2][1] = (_dM[0][1] * _dM[2][0] - _dM[0][0] * _dM[2][1]) * dInvDet;
		_dRet[2][2] = (_dM[0][0] * _dM[1][1] - _dM[0][1] * _dM[1][0]) * dInvDet;
	}

	// == Functions.
	/**
	 * Creates a conversion matrix given a color-space's chromaticities and white point.
	 *
	 * \param _pfChromas the chroma and white-point values.  Source from the members of this class.
	 * \return The RGB to XYZ transformation matrix.
	 */
	CMatrix4x4<LSN_ST_AVX512> CColorSpace::ComputeRgbToXyzMatrix( const float * _pfChromas ) {
		return ComputeRgbToXyzMatrix( _pfChromas[0], _pfChromas[1], _pfChromas[2], _pfChromas[3], _pfChromas[4], _pfChromas[5], _pfChromas[6], _pfChromas[7] );
	}

	/**
	 * Creates a conversion matrix given a color-space's chromaticities and white point.
	 *
	 * \param _pfChromas the chroma and white-point values.  Source from the members of this class.
	 * \return The XYZ to RGB transformation matrix.
	 */
	CMatrix4x4<LSN_ST_AVX512> CColorSpace::ComputeXyzToRgbMatrix( const float * _pfChromas ) {
		return ComputeXyzToRgbMatrix( _pfChromas[0], _pfChromas[1], _pfChromas[2], _pfChromas[3], _pfChromas[4], _pfChromas[5], _pfChromas[6], _pfChromas[7] );
	}

	/**
	 * Gets information about a color space.
	 *
	 * \param _cspColorSpace The color space.
	 * \return Returns the information about the color space, or the information about sRGB if _cspColorSpace is invalid.
	 */
	const CColorSpace::LSN_COLOR_SPACE_DATA & CColorSpace::Data( LSN_COLOR_SPACE _cspColorSpace ) {
		return m_csdData[(_cspColorSpace < LSN_CSP_TOTAL) ? _cspColorSpace : LSN_CSP_SRGB];
	}

	/**
	 * Gets information about a television decoder chip.
	 *
	 * \param _dmDemodulator The decoder chip.
	 * \return Returns the information about the decoder chip, or the information about LSN_DM_NONE if _dmDemodulator is invalid.
	 */
	const CColorSpace::LSN_DEMODULATOR_DATA & CColorSpace::Demodulator( LSN_DEMODULATOR _dmDemodulator ) {
		return m_ddDemodulators[(_dmDemodulator < LSN_DM_TOTAL) ? _dmDemodulator : LSN_DM_NONE];
	}

	/**
	 * Creates the coefficients that convert Y'UV to R'G'B' the way a television's decoder chip does (with its color correction):
	 *	R' = Y' + _pfMatrix[0] * U + _pfMatrix[1] * V, G' = Y' + _pfMatrix[2] * U + _pfMatrix[3] * V, and
	 *	B' = Y' + _pfMatrix[4] * U + _pfMatrix[5] * V.  The gains are denormalized as gamutthingy does by default:  the B-Y gain becomes
	 *	1 / 0.492111 (renormalized to the U/V ellipse when the B-Y axis is off 0 degrees and its gain is not 1).
	 *
	 * \param _dmDemodulator The decoder chip.  LSN_DM_NONE gives the standard Y'UV-to-R'G'B' coefficients.
	 * \param _pfMatrix Holds the 6 returned coefficients.
	 */
	void CColorSpace::DemodulatorMatrix( LSN_DEMODULATOR _dmDemodulator, float * _pfMatrix ) {
		if ( _dmDemodulator >= LSN_DM_TOTAL || _dmDemodulator == LSN_DM_NONE ) {
			// The standard coefficients (exactly what the filters used before decoder chips could be chosen).
			_pfMatrix[0] = 0.0f;
			_pfMatrix[1] = 1.139883025203f;
			_pfMatrix[2] = -0.394642233974f;
			_pfMatrix[3] = -0.580621847591f;
			_pfMatrix[4] = 2.032061872219f;
			_pfMatrix[5] = 0.0f;
			return;
		}
		const LSN_DEMODULATOR_DATA & ddData = m_ddDemodulators[_dmDemodulator];
		constexpr double dDegToRad = std::numbers::pi / 180.0;
		constexpr double dUScale = 1.0 / 0.492111;					// B-Y = U / 0.492111.
		constexpr double dVScale = 1.0 / 0.877283;					// R-Y = V / 0.877283.
		const double dBlueAngle = ddData.fAngles[2] * dDegToRad;
		const double dBlueGain = ddData.fGains[2];
		double dNorm = dUScale;
		if ( dBlueGain != 1.0 && dBlueAngle != 0.0 ) {
			// The upscale factors form an ellipse with dUScale on the U axis and dVScale on the V axis; its radius at the B-Y angle becomes the B-Y gain.
			const double dSin = std::sin( dBlueAngle ), dCos = std::cos( dBlueAngle );
			const double dRadius = (dUScale * dVScale) / std::sqrt( (dUScale * dUScale * dSin * dSin) + (dVScale * dVScale * dCos * dCos) );
			dNorm = dRadius / dBlueGain;
		}
		for ( size_t I = 0; I < 3; ++I ) {
			const double dAngle = ddData.fAngles[I] * dDegToRad;
			const double dGain = ddData.fGains[I] * dNorm;
			_pfMatrix[I*2+0] = float( dGain * std::cos( dAngle ) );
			_pfMatrix[I*2+1] = float( dGain * std::sin( dAngle ) );
		}
	}

	/**
	 * Finds a color space by the ID returned in LSN_COLOR_SPACE_DATA::pcId.
	 *
	 * \param _pcId The ID of the color space.
	 * \return Returns the color space or LSN_CSP_TOTAL if there is no color space with that ID.
	 */
	CColorSpace::LSN_COLOR_SPACE CColorSpace::FromId( const char * _pcId ) {
		if ( !_pcId ) { return LSN_CSP_TOTAL; }
		for ( uint32_t I = 0; I < LSN_CSP_TOTAL; ++I ) {
			if ( std::strcmp( m_csdData[I].pcId, _pcId ) == 0 ) { return LSN_COLOR_SPACE( I ); }
		}
		return LSN_CSP_TOTAL;
	}

	/**
	 * Finds a color space by its index in the list as it was before settings saved color spaces by ID (settings versions below
	 *	LSN_BEESNES_SETTINGS_VERSION_TV_CONTROLS).
	 *
	 * \param _ui32Index The old index.
	 * \return Returns the color space or LSN_CSP_TOTAL if the index is invalid.
	 */
	CColorSpace::LSN_COLOR_SPACE CColorSpace::FromLegacyIndex( uint32_t _ui32Index ) {
		// The IDs of the color spaces in their old order.
		static const char * const s_pcIds[] = {
			"srgb",
			"bt709",
			"smpte_c",
			"ntsc_1953",
			"bt470_ntsc",
			"bt470_pal",
			"bt601_625",
			"bt2020",
			"ntsc_j",
			"toshiba_cf2005",
			"sony_pvm_20m2u",
			"sony_pvm_20l2mdu",
			"sony_trinitron_p22",
			"toshiba_1992",
			"matsushita_1979",
			"rca_colortrak_e13169gm",
			"sony_gdm_17se1",
			"apple_multiple_scan_1705",
			"nec_multisync_c400",
			"kds_vs19",
		};
		if ( _ui32Index >= std::size( s_pcIds ) ) { return LSN_CSP_TOTAL; }
		return FromId( s_pcIds[_ui32Index] );
	}

	/**
	 * Creates the matrix that converts linear RGB in one color space to linear RGB in another.  RGB is converted to CIE XYZ through the
	 *	source primaries and back through the destination primaries.  White points are not adapted, so a source white that is bluer than
	 *	the destination white stays bluer (as it was on the original screens).  If source white would then exceed 1.0 in any channel,
	 *	the matrix is scaled so that source white just reaches 1.0 in its brightest channel instead of clipping.
	 *
	 * \param _cspSrc The color space of the input.
	 * \param _cspDst The color space of the output (normally the display, sRGB).
	 * \param _waAdaptation How the source white point is adapted to the destination white point.  With LSN_WA_NONE, white points are
	 *	not adapted and the scaling described above applies; otherwise source white becomes destination white exactly.
	 * \return Returns the conversion matrix.  It is exactly the identity matrix if the color spaces are the same.
	 */
	CColorSpace::LSN_COLOR_MATRIX CColorSpace::ConversionMatrix( LSN_COLOR_SPACE _cspSrc, LSN_COLOR_SPACE _cspDst, LSN_WHITE_ADAPTATION _waAdaptation ) {
		LSN_COLOR_MATRIX cmRet;
		const float * pfSrc = Data( _cspSrc ).pfChromas;
		const float * pfDst = Data( _cspDst ).pfChromas;
		if ( pfSrc == pfDst || std::memcmp( pfSrc, pfDst, sizeof( float ) * 8 ) == 0 ) { return cmRet; }

		CMatrix4x4<LSN_ST_AVX512> mSrcToXyz = ComputeRgbToXyzMatrix( pfSrc );
		CMatrix4x4<LSN_ST_AVX512> mXyzToDst = ComputeXyzToRgbMatrix( pfDst );
		double dSrcToXyz[3][3], dXyzToDst[3][3];
		for ( size_t I = 0; I < 3; ++I ) {
			for ( size_t J = 0; J < 3; ++J ) {
				dSrcToXyz[I][J] = mSrcToXyz[I][J];
				dXyzToDst[I][J] = mXyzToDst[I][J];
			}
		}

		// The chromatic-adaptation transform (identity for none).
		double dCat[3][3] = {
			{ 1.0, 0.0, 0.0 },
			{ 0.0, 1.0, 0.0 },
			{ 0.0, 0.0, 1.0 },
		};
		if ( _waAdaptation != LSN_WA_NONE && _waAdaptation < LSN_WA_TOTAL ) {
			// The cone-response matrices.
			static const double dBradford[3][3] = {
				{ 0.8951, 0.2664, -0.1614 },
				{ -0.7502, 1.7135, 0.0367 },
				{ 0.0389, -0.0685, 1.0296 },
			};
			static const double dVonKries[3][3] = {
				{ 0.40024, 0.70760, -0.08081 },
				{ -0.22630, 1.16532, 0.04570 },
				{ 0.0, 0.0, 0.91822 },
			};
			static const double dXyzScaling[3][3] = {
				{ 1.0, 0.0, 0.0 },
				{ 0.0, 1.0, 0.0 },
				{ 0.0, 0.0, 1.0 },
			};
			static const double dCat02[3][3] = {
				{ 0.7328, 0.4296, -0.1624 },
				{ -0.7036, 1.6975, 0.0061 },
				{ 0.0030, 0.0136, 0.9834 },
			};
			const double (* pdCone)[3] = dBradford;
			switch ( _waAdaptation ) {
				case LSN_WA_VON_KRIES : { pdCone = dVonKries; break; }
				case LSN_WA_XYZ_SCALING : { pdCone = dXyzScaling; break; }
				case LSN_WA_CAT02 : { pdCone = dCat02; break; }
				default : {}
			}
			double dConeInv[3][3];
			Invert3x3( pdCone, dConeInv );

			// The white points in XYZ (Y = 1).
			const double dSrcW[3] = { double( pfSrc[6] ) / pfSrc[7], 1.0, (1.0 - pfSrc[6] - pfSrc[7]) / pfSrc[7] };
			const double dDstW[3] = { double( pfDst[6] ) / pfDst[7], 1.0, (1.0 - pfDst[6] - pfDst[7]) / pfDst[7] };
			double dSrcCone[3], dDstCone[3];
			for ( size_t I = 0; I < 3; ++I ) {
				dSrcCone[I] = pdCone[I][0] * dSrcW[0] + pdCone[I][1] * dSrcW[1] + pdCone[I][2] * dSrcW[2];
				dDstCone[I] = pdCone[I][0] * dDstW[0] + pdCone[I][1] * dDstW[1] + pdCone[I][2] * dDstW[2];
			}
			// CAT = inverse(Cone) * diag(Dst / Src) * Cone.
			double dScaled[3][3];
			for ( size_t I = 0; I < 3; ++I ) {
				const double dFactor = dSrcCone[I] != 0.0 ? dDstCone[I] / dSrcCone[I] : 1.0;
				for ( size_t J = 0; J < 3; ++J ) {
					dScaled[I][J] = pdCone[I][J] * dFactor;
				}
			}
			Multiply3x3( dConeInv, dScaled, dCat );
		}

		double dTmp[3][3], dConv[3][3];
		Multiply3x3( dCat, dSrcToXyz, dTmp );
		Multiply3x3( dXyzToDst, dTmp, dConv );

		// Without adaptation, source white is (1, 1, 1) and may exceed 1.0 in a channel; scale so that it does not.
		double dScale = 1.0;
		if ( _waAdaptation == LSN_WA_NONE || _waAdaptation >= LSN_WA_TOTAL ) {
			double dMaxWhite = 0.0;
			for ( size_t I = 0; I < 3; ++I ) {
				dMaxWhite = std::max( dMaxWhite, dConv[I][0] + dConv[I][1] + dConv[I][2] );
			}
			dScale = (dMaxWhite > 1.0) ? (1.0 / dMaxWhite) : 1.0;
		}
		for ( size_t I = 0; I < 3; ++I ) {
			for ( size_t J = 0; J < 3; ++J ) {
				cmRet.fM[I][J] = float( dConv[I][J] * dScale );
			}
		}
		return cmRet;
	}

	/**
	 * Converts a linear color from one color space to another and brings it into the gamut of the destination, exactly as the
	 *	shader code from ShaderStatement() does.  Used to bake the color-space conversion into palettes.
	 *
	 * \param _cmMatrix The color-space conversion (see ConversionMatrix()).  The identity matrix changes nothing.
	 * \param _pfLuma The luminance coefficients of the destination color space (see LuminanceCoefficients()), or nullptr for sRGB.
	 * \param _gmGamutMapping How out-of-gamut colors are handled.
	 * \param _pdRgb The linear red, green, and blue values to convert in place.
	 */
	void CColorSpace::ConvertLinear( const LSN_COLOR_MATRIX &_cmMatrix, const float * _pfLuma, LSN_GAMUT_MAPPING _gmGamutMapping, double * _pdRgb ) {
		if ( !_pdRgb || _cmMatrix.IsIdentity() ) { return; }
		const double dIn[3] = { _pdRgb[0], _pdRgb[1], _pdRgb[2] };
		for ( size_t I = 0; I < 3; ++I ) {
			_pdRgb[I] = double( _cmMatrix.fM[I][0] ) * dIn[0] + double( _cmMatrix.fM[I][1] ) * dIn[1] + double( _cmMatrix.fM[I][2] ) * dIn[2];
		}
		if ( _gmGamutMapping == LSN_GM_CLIP ) {
			for ( size_t I = 0; I < 3; ++I ) { _pdRgb[I] = std::clamp( _pdRgb[I], 0.0, 1.0 ); }
			return;
		}
		static const float fSrgbLuma[3] = { 2.12639006e-01f, 7.15168679e-01f, 7.21923187e-02f };
		const float * pfLuma = _pfLuma ? _pfLuma : fSrgbLuma;
		// Gamut mapping:  move toward the luminance just far enough that every channel fits (see ShaderStatement()).
		const double dY = std::clamp( double( pfLuma[0] ) * _pdRgb[0] + double( pfLuma[1] ) * _pdRgb[1] + double( pfLuma[2] ) * _pdRgb[2], 0.0, 1.0 );
		double dT = 1.0;
		for ( size_t I = 0; I < 3; ++I ) {
			const double dD = _pdRgb[I] - dY;
			const double dThisT = (dD >= 0.0) ? ((1.0 - dY) / std::max( dD, 1.0e-6 )) : (dY / std::max( -dD, 1.0e-6 ));
			dT = std::min( dT, dThisT );
		}
		dT = std::clamp( dT, 0.0, 1.0 );
		for ( size_t I = 0; I < 3; ++I ) {
			_pdRgb[I] = std::clamp( dY + (_pdRgb[I] - dY) * dT, 0.0, 1.0 );
		}
	}

	/**
	 * Gets the luminance coefficients of a color space (the Y row of its RGB-to-XYZ matrix).
	 *
	 * \param _cspColorSpace The color space.
	 * \param _pfLuma Holds the returned 3 coefficients.
	 */
	void CColorSpace::LuminanceCoefficients( LSN_COLOR_SPACE _cspColorSpace, float * _pfLuma ) {
		CMatrix4x4<LSN_ST_AVX512> mToXyz = ComputeRgbToXyzMatrix( Data( _cspColorSpace ).pfChromas );
		for ( size_t I = 0; I < 3; ++I ) {
			_pfLuma[I] = mToXyz[1][I];
		}
	}

	/**
	 * Creates the shader code of the gamma pass:  the CRT gamma curve (sign-preserving if there is a color-space conversion), then
	 *	the conversion and gamut mapping.  The code reads and writes c.rgb.  HLSL code calls the functions in LSNGamma.hlsl; GLSL code
	 *	calls the functions of the same names that the Vulkan gamma pass embeds.
	 *
	 * \param _gpPass The parameters of the pass.
	 * \param _bHlsl If true, HLSL is generated, otherwise GLSL.
	 * \return Returns the code.
	 */
	std::string CColorSpace::GammaPassStatements( const LSN_GAMMA_PASS &_gpPass, bool _bHlsl ) {
		const std::string sVec3 = _bHlsl ? "float3" : "vec3";
		auto aNum = []( double _dVal ) { return std::format( "{:.8e}", _dVal ); };
		auto aPow = [&]( const char * _pcExp ) { return "pow( c.rgb, " + sVec3 + "( " + _pcExp + ", " + _pcExp + ", " + _pcExp + " ) )"; };
		std::string sGammaCall = "c.rgb";
		switch ( _gpPass.gGamma ) {
			case CNesPalette::LSN_G_CRT1 :			{ sGammaCall = "CrtProperToLinear3( c.rgb, " + aNum( _gpPass.fCrtWhite ) + ", " + aNum( _gpPass.fCrtBrightness ) + " )"; break; }
			case CNesPalette::LSN_G_CRT2 :			{ sGammaCall = "CrtProper2ToLinear3( c.rgb )"; break; }
			case CNesPalette::LSN_G_sRGB :			{ sGammaCall = "sRGBtoLinear3_Precise( c.rgb )"; break; }
			case CNesPalette::LSN_G_SMPTE170M :		{ sGammaCall = "SMPTE170MtoLinear3_Precise( c.rgb )"; break; }
			case CNesPalette::LSN_G_DCIP3 :			{ sGammaCall = "DCIP3toLinear3( c.rgb )"; break; }
			case CNesPalette::LSN_G_ADOBERGB :		{ sGammaCall = "AdobeRGBtoLinear3( c.rgb )"; break; }
			case CNesPalette::LSN_G_SMPTE240M :		{ sGammaCall = "SMPTE240MtoLinear3_Precise( c.rgb )"; break; }
			case CNesPalette::LSN_G_POW_1_96 :		{ sGammaCall = aPow( "1.96" ); break; }
			case CNesPalette::LSN_G_POW_2_0 :		{ sGammaCall = aPow( "2.0" ); break; }
			case CNesPalette::LSN_G_POW_2_2 :		{ sGammaCall = aPow( "2.2222222222222223" ); break; }
			case CNesPalette::LSN_G_POW_2_35 :		{ sGammaCall = aPow( "2.35" ); break; }
			case CNesPalette::LSN_G_POW_2_4 :		{ sGammaCall = aPow( "2.4" ); break; }
			case CNesPalette::LSN_G_POW_2_5 :		{ sGammaCall = aPow( "2.5" ); break; }
			case CNesPalette::LSN_G_POW_2_7 :		{ sGammaCall = aPow( "2.7" ); break; }
			case CNesPalette::LSN_G_POW_2_8 :		{ sGammaCall = aPow( "2.8" ); break; }
			default : { break; }
		}
		// With a color-space conversion, out-of-gamut (negative) inputs must survive the gamma curve (it would make them NaN).
		if ( !_gpPass.cmMatrix.IsIdentity() ) { sGammaCall = SignedGammaCall( sGammaCall ); }
		return "    c.rgb = " + sGammaCall + ";\n    " + ShaderStatement( _gpPass.cmMatrix, _bHlsl, _gpPass.fLuma, _gpPass.gmGamutMapping );
	}

	/**
	 * Creates shader code that converts the linear color in c.rgb to the values sent to the monitor (see LSN_OUTPUT_TRANSFORM).
	 *	The code needs no functions, so it can be placed in any shader.
	 *
	 * \param _otTransform The output transform.
	 * \param _bHlsl If true, HLSL is generated, otherwise GLSL.
	 * \return Returns the code, or an empty string if the sRGB hardware does the whole conversion on its own.
	 */
	std::string CColorSpace::OutputTransformStatement( const LSN_OUTPUT_TRANSFORM &_otTransform, bool _bHlsl ) {
		if ( _otTransform.IsHardwareSrgb() ) { return std::string(); }
		const std::string sVec3 = _bHlsl ? "float3" : "vec3";
		auto aNum = []( double _dVal ) { return std::format( "{:.8e}", _dVal ); };
		auto aSplat = [&]( double _dVal ) { std::string sNum = aNum( _dVal ); return sVec3 + "( " + sNum + ", " + sNum + ", " + sNum + " )"; };
		// A piecewise curve:  _dSlope * x below _dThresh, otherwise _dA * pow( x, _dExp ) - (_dA - 1).
		auto aPiecewise = [&]( double _dThresh, double _dSlope, double _dA, double _dExp ) {
			return "        vOtS = step( " + aSplat( _dThresh ) + ", c.rgb );\n"
				"        c.rgb = (" + sVec3 + "( 1.0, 1.0, 1.0 ) - vOtS) * (c.rgb * " + aNum( _dSlope ) + ") + vOtS * (pow( c.rgb, " + aSplat( _dExp ) + " ) * " + aNum( _dA ) + " - " + aSplat( _dA - 1.0 ) + ");\n";
		};
		auto aPow = [&]( double _dExp ) { return "        c.rgb = pow( c.rgb, " + aSplat( 1.0 / _dExp ) + " );\n"; };

		std::string sRet = "{\n"
			"        " + sVec3 + " vOtS;\n"
			"        c.rgb = clamp( c.rgb, " + sVec3 + "( 0.0, 0.0, 0.0 ), " + sVec3 + "( 1.0, 1.0, 1.0 ) );\n";
		// Encode with the monitor's curve.
		switch ( _otTransform.gGamma ) {
			case CNesPalette::LSN_G_NONE : { break; }
			case CNesPalette::LSN_G_SMPTE170M : {
				sRet += aPiecewise( 0.0180539685108078128139563744980, 4.5, 1.09929682680944297568093048767, 0.45 );
				break;
			}
			case CNesPalette::LSN_G_SMPTE240M : {
				sRet += aPiecewise( 0.0228215855294450278845097557223, 4.0, 1.1115721959217312597711924126997, 0.45 );
				break;
			}
			case CNesPalette::LSN_G_DCIP3 :			{ sRet += aPow( 2.6 ); break; }
			case CNesPalette::LSN_G_ADOBERGB :		{ sRet += aPow( 2.19921875 ); break; }
			case CNesPalette::LSN_G_POW_1_96 :		{ sRet += aPow( 1.96 ); break; }
			case CNesPalette::LSN_G_POW_2_0 :		{ sRet += aPow( 2.0 ); break; }
			case CNesPalette::LSN_G_POW_2_2 :		{ sRet += aPow( 2.22222222222222232090871330001391470432281494140625 ); break; }
			case CNesPalette::LSN_G_POW_2_35 :		{ sRet += aPow( 2.35 ); break; }
			case CNesPalette::LSN_G_POW_2_4 :		{ sRet += aPow( 2.4 ); break; }
			case CNesPalette::LSN_G_POW_2_5 :		{ sRet += aPow( 2.5 ); break; }
			case CNesPalette::LSN_G_POW_2_7 :		{ sRet += aPow( 2.7 ); break; }
			case CNesPalette::LSN_G_POW_2_8 :		{ sRet += aPow( 2.8 ); break; }
			default : {
				// sRGB (and automatic).  The CRT curves are not monitor curves (and LSN_G_CRT2 cannot be inverted in real time), so they
				//	also land here.
				sRet += aPiecewise( 0.0030399346397784318338231024370, 12.9232101807878549948327417951, 1.055, 1.0 / 2.4 );
			}
		}
		// The output range.
		if ( _otTransform.orOutputRange == LSN_OR_LIMITED ) {
			sRet += "        c.rgb = c.rgb * " + aNum( 219.0 / 255.0 ) + " + " + aSplat( 16.0 / 255.0 ) + ";\n";
		}
		// Undo the sRGB curve the hardware is about to apply (IEC 61966-2-1, as the hardware implements it).
		sRet += "        c.rgb = clamp( c.rgb, " + sVec3 + "( 0.0, 0.0, 0.0 ), " + sVec3 + "( 1.0, 1.0, 1.0 ) );\n"
			"        vOtS = step( " + aSplat( 0.04045 ) + ", c.rgb );\n"
			"        c.rgb = (" + sVec3 + "( 1.0, 1.0, 1.0 ) - vOtS) * (c.rgb * " + aNum( 1.0 / 12.92 ) + ") + vOtS * pow( (c.rgb + " + aSplat( 0.055 ) + ") * " + aNum( 1.0 / 1.055 ) + ", " + aSplat( 2.4 ) + " );\n"
			"    }\n";
		return sRet;
	}

	/**
	 * Creates shader code that converts the linear color in c.rgb by a matrix and then maps the result into [0, 1] without changing
	 *	its hue:  rather than clamping each channel (which shifts the hue of out-of-gamut colors), the color is desaturated toward
	 *	its own luminance just far enough for every channel to fit.  If the luminance itself is outside [0, 1], it is clamped first.
	 *	The code is a single block, so it can be placed anywhere a statement can.
	 *
	 * \param _cmMatrix The conversion matrix.
	 * \param _bHlsl If true, HLSL is generated (float3), otherwise GLSL (vec3).
	 * \param _pfLuma The 3 luminance coefficients of the destination color space, or nullptr for those of sRGB.
	 * \param _gmGamutMapping How out-of-gamut colors are handled.
	 * \return Returns the code, or an empty string if the matrix is the identity matrix.
	 */
	std::string CColorSpace::ShaderStatement( const LSN_COLOR_MATRIX &_cmMatrix, bool _bHlsl, const float * _pfLuma, LSN_GAMUT_MAPPING _gmGamutMapping ) {
		if ( _cmMatrix.IsIdentity() ) { return std::string(); }
		const std::string sVec3 = _bHlsl ? "float3" : "vec3";
		// 9 significant digits round-trip a float exactly; the exponent form is valid in both languages.
		auto aNum = []( float _fVal ) { return std::format( "{:.8e}", _fVal ); };
		std::string sRet = "{\n";
		// The conversion (the output is linear sRGB).
		sRet += "        c.rgb = " + sVec3 + "(\n";
		for ( size_t I = 0; I < 3; ++I ) {
			sRet += "            dot( " + sVec3 + "( " + aNum( _cmMatrix.fM[I][0] ) + ", " + aNum( _cmMatrix.fM[I][1] ) + ", " + aNum( _cmMatrix.fM[I][2] ) + " ), c.rgb )";
			sRet += (I < 2) ? ",\n" : " );\n";
		}
		if ( _gmGamutMapping == LSN_GM_CLIP ) {
			sRet += "        c.rgb = clamp( c.rgb, " + sVec3 + "( 0.0, 0.0, 0.0 ), " + sVec3 + "( 1.0, 1.0, 1.0 ) );\n"
				"    }\n";
			return sRet;
		}
		static const float fSrgbLuma[3] = { 2.12639006e-01f, 7.15168679e-01f, 7.21923187e-02f };
		const float * pfLuma = _pfLuma ? _pfLuma : fSrgbLuma;
		// Gamut mapping.  For each channel, find how much of its distance from the luminance (vD) can be kept:  (1 - Y) / D above the
		//	luminance and Y / -D below it.  step() picks between them in both languages (GLSL mix() and HLSL lerp() differ).
		sRet +=
			"        float fCsY = clamp( dot( c.rgb, " + sVec3 + "( " + aNum( pfLuma[0] ) + ", " + aNum( pfLuma[1] ) + ", " + aNum( pfLuma[2] ) + " ) ), 0.0, 1.0 );\n"
			"        " + sVec3 + " vCsD = c.rgb - " + sVec3 + "( fCsY, fCsY, fCsY );\n"
			"        " + sVec3 + " vCsUp = step( " + sVec3 + "( 0.0, 0.0, 0.0 ), vCsD );\n"
			"        " + sVec3 + " vCsT = vCsUp * ((1.0 - fCsY) / max( vCsD, " + sVec3 + "( 1.0e-6, 1.0e-6, 1.0e-6 ) )) +\n"
			"            (" + sVec3 + "( 1.0, 1.0, 1.0 ) - vCsUp) * (fCsY / max( -vCsD, " + sVec3 + "( 1.0e-6, 1.0e-6, 1.0e-6 ) ));\n"
			"        float fCsT = clamp( min( vCsT.x, min( vCsT.y, vCsT.z ) ), 0.0, 1.0 );\n"
			"        c.rgb = clamp( " + sVec3 + "( fCsY, fCsY, fCsY ) + vCsD * fCsT, " + sVec3 + "( 0.0, 0.0, 0.0 ), " + sVec3 + "( 1.0, 1.0, 1.0 ) );\n"
			"    }\n";
		return sRet;
	}

	/**
	 * Wraps a gamma-curve call so that it works on values outside [0, 1].  The composite decoders (my filters in
	 *	particular) produce negative values for colors outside the gamut of the source, and the gamma curves return NaN for them
	 *	(pow() of a negative number).  NaN in one channel then spreads to the whole pixel through the color-space matrix.  The
	 *	wrapped call applies the curve to the magnitude and restores the sign, so the color reaches the matrix (and the gamut
	 *	mapping after it) intact.
	 *
	 * \param _sGammaCall The gamma-curve call, which reads its input from c.rgb.
	 * \return Returns the sign-preserving form of the call (or _sGammaCall if it does not change c.rgb).
	 */
	std::string CColorSpace::SignedGammaCall( const std::string &_sGammaCall ) {
		if ( _sGammaCall == "c.rgb" ) { return _sGammaCall; }
		std::string sRet = _sGammaCall;
		const std::string sFind = "c.rgb", sReplace = "abs( c.rgb )";
		for ( size_t sPos = sRet.find( sFind ); sPos != std::string::npos; sPos = sRet.find( sFind, sPos + sReplace.size() ) ) {
			sRet.replace( sPos, sFind.size(), sReplace );
		}
		return "sign( c.rgb ) * " + sRet;
	}

	/**
	 * Creates a conversion matrix given a color-space's chromaticities and white point.
	 *
	 * \param _fRedX X coordinate of red primary.
	 * \param _fRedY Y coordinate of red primary.
	 * \param _fGreenX X coordinate of green primary.
	 * \param _fGreenY Y coordinate of green primary.
	 * \param _fBlueX X coordinate of blue primary.
	 * \param _fBlueY Y coordinate of blue primary.
	 * \param _fWhiteX X coordinate of white point.
	 * \param _fWhiteY Y coordinate of white point.
	 * \return The RGB to XYZ transformation matrix.
	 */
	CMatrix4x4<LSN_ST_AVX512> CColorSpace::ComputeRgbToXyzMatrix( float _fRedX, float _fRedY,
		float _fGreenX, float _fGreenY,
		float _fBlueX, float _fBlueY,
		float _fWhiteX, float _fWhiteY ) {
		// Compute Z coordinates.
		float fRedZ = 1.0f - _fRedX - _fRedY;
		float fGreenZ = 1.0f - _fGreenX - _fGreenY;
		float fBlueZ = 1.0f - _fBlueX - _fBlueY;
		float fWhiteZ = 1.0f - _fWhiteX - _fWhiteY;

		// Create the chromaticity matrix.
		CMatrix4x4<LSN_ST_AVX512> mChromaMtx(
			{ _fRedX,	_fGreenX,	_fBlueX,	0.0f },
			{ _fRedY,	_fGreenY,	_fBlueY,	0.0f },
			{ fRedZ,	fGreenZ,	fBlueZ,		0.0f } );

		// Compute the white point in XYZ.
		CVector4<LSN_ST_AVX512> vWhitePoint = {
			_fWhiteX / _fWhiteY,
			1.0f,
			fWhiteZ / _fWhiteY,
			0.0f
		};

		// Invert the chromaticity matrix.
		CMatrix4x4<LSN_ST_AVX512> mInvChromaMtx = mChromaMtx.Inverse();

		// Compute the scaling factors S = inv(M) * W.
		CVector4<LSN_ST_AVX512> S = mInvChromaMtx * vWhitePoint;

		// Scale the columns of the chromaticity matrix.
		CMatrix4x4<LSN_ST_AVX512> mRgbToXyzMat;
		mRgbToXyzMat.Identity();
		for ( int I = 0; I < 3; ++I ) {
			mRgbToXyzMat[0][I] = mChromaMtx[0][I] * S[I];
			mRgbToXyzMat[1][I] = mChromaMtx[1][I] * S[I];
			mRgbToXyzMat[2][I] = mChromaMtx[2][I] * S[I];
		}

		return mRgbToXyzMat;
	}

	/**
	 * Creates a conversion matrix given a color-space's chromaticities and white point.
	 *
	 * \param _fRedX X coordinate of red primary.
	 * \param _fRedY Y coordinate of red primary.
	 * \param _fGreenX X coordinate of green primary.
	 * \param _fGreenY Y coordinate of green primary.
	 * \param _fBlueX X coordinate of blue primary.
	 * \param _fBlueY Y coordinate of blue primary.
	 * \param _fWhiteX X coordinate of white point.
	 * \param _fWhiteY Y coordinate of white point.
	 * \return The XYZ to RGB transformation matrix.
	 */
	CMatrix4x4<LSN_ST_AVX512> CColorSpace::ComputeXyzToRgbMatrix( float _fRedX, float _fRedY,
		float _fGreenX, float _fGreenY,
		float _fBlueX, float _fBlueY,
		float _fWhiteX, float _fWhiteY ) {
		return ComputeRgbToXyzMatrix( _fRedX, _fRedY,
			_fGreenX, _fGreenY,
			_fBlueX, _fBlueY,
			_fWhiteX, _fWhiteY ).Inverse();
	}

}	// namespace lsn

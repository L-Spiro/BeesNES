#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The fields (settings) shown by the Video Options dialog:  what each one is, how it is shown, and how it is read from and
 *	written to the options.
 */

#include "LSNVideoOptionsFields.h"
#include "../../BeesNES/LSNBeesNes.h"
#include "../../Localization/LSNLocalization.h"
#include "../WinUtilities/LSNWinUtilities.h"
#include "../../Options/LSNLSpiroPresets.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <format>
#include <numbers>

#if defined( LSN_DX9 ) || defined( LSN_DX12 ) || defined( LSN_VULKAN1 )
/** Defined if any GPU API is available (the dialog then shows the GPU versions of settings that exist in both versions). */
#define LSN_VO_HAVE_GPU
#endif	// #if defined( LSN_DX9 ) || defined( LSN_DX12 ) || defined( LSN_VULKAN1 )

namespace lsn {

	// == Members.
	/** If true, the palette display shows the color indices (not saved; the display does not exist yet). */
	static bool												s_bShowColorIndices = false;

	/** The fields. */
	static const CVideoOptionsFields::LSN_FIELD_INFO		s_fiFields[CVideoOptionsFields::LSN_VF_TOTAL] = {
		// Host machine.
		{ CVideoOptionsFields::LSN_VFT_COMBO,		LSN_LSTR( LSN_STR_VO_MONITOR_COLOR_SPACE ),		reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_MONITOR_COLOR_SPACE )) },
		{ CVideoOptionsFields::LSN_VFT_COMBO,		LSN_LSTR( LSN_STR_VO_MONITOR_GAMMA ),			reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_MONITOR_GAMMA )) },
		{ CVideoOptionsFields::LSN_VFT_COMBO,		LSN_LSTR( LSN_STR_VO_WHITE_ADAPTATION ),		reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_WHITE_ADAPTATION )) },
		{ CVideoOptionsFields::LSN_VFT_COMBO,		LSN_LSTR( LSN_STR_VO_GAMUT ),					reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_GAMUT )) },
		{ CVideoOptionsFields::LSN_VFT_COMBO,		LSN_LSTR( LSN_STR_VO_OUTPUT_RANGE ),			reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_OUTPUT_RANGE )) },

		// Common:  color correction.
		{ CVideoOptionsFields::LSN_VFT_COMBO,		LSN_LSTR( LSN_STR_VO_TV_PRESET ),				reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_TV_PRESET )) },
		{ CVideoOptionsFields::LSN_VFT_COMBO,		LSN_LSTR( LSN_STR_VO_COLOR_SPACE ),				reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_COLOR_SPACE )) },
		{ CVideoOptionsFields::LSN_VFT_COMBO,		LSN_LSTR( LSN_STR_VO_DEMODULATOR ),				reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_DEMODULATOR )) },
		{ CVideoOptionsFields::LSN_VFT_COMBO,		LSN_LSTR( LSN_STR_VO_GAMMA ),					reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_GAMMA )) },
		{ CVideoOptionsFields::LSN_VFT_FLOAT,		LSN_LSTR( LSN_STR_VO_CRT_WHITE ),				reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_CRT_WHITE )) },
		{ CVideoOptionsFields::LSN_VFT_FLOAT,		LSN_LSTR( LSN_STR_VO_CRT_BRIGHTNESS ),			reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_CRT_BRIGHTNESS )) },
		// Common:  phosphor decay.
		{ CVideoOptionsFields::LSN_VFT_CHECK,		LSN_LSTR( LSN_STR_VO_ENABLED ),					reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_PHOSPHOR_ENABLED )) },
		{ CVideoOptionsFields::LSN_VFT_FLOAT,		LSN_LSTR( LSN_STR_VO_BASE_LEVEL ),				reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_BASE_LEVEL )) },
		{ CVideoOptionsFields::LSN_VFT_FLOAT,		LSN_LSTR( LSN_STR_VO_TIME ),					reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_TIME )) },
		{ CVideoOptionsFields::LSN_VFT_FLOAT,		LSN_LSTR( LSN_STR_VO_RED_MULTIPLIER ),			reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_RED_MULTIPLIER )) },
		{ CVideoOptionsFields::LSN_VFT_FLOAT,		LSN_LSTR( LSN_STR_VO_BLUE_MULTIPLIER ),			reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_BLUE_MULTIPLIER )) },
		// Common:  internal scaling.
		{ CVideoOptionsFields::LSN_VFT_INT,			LSN_LSTR( LSN_STR_VO_SCALE_X ),					reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_SCALE_X )) },
		{ CVideoOptionsFields::LSN_VFT_INT,			LSN_LSTR( LSN_STR_VO_SCALE_Y ),					reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_SCALE_Y )) },
		{ CVideoOptionsFields::LSN_VFT_CHECK,		LSN_LSTR( LSN_STR_VO_DYNAMIC ),					reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_DYNAMIC )) },
		// Common:  convolution.
		{ CVideoOptionsFields::LSN_VFT_CHECK,		LSN_LSTR( LSN_STR_VO_ENABLED ),					reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_CONVOLUTION )) },
		{ CVideoOptionsFields::LSN_VFT_COMBO,		LSN_LSTR( LSN_STR_VO_LOW_FILTER ),				reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_LOW_FILTER )) },
		{ CVideoOptionsFields::LSN_VFT_COMBO,		LSN_LSTR( LSN_STR_VO_MEDIUM_FILTER ),			reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_MEDIUM_FILTER )) },
		{ CVideoOptionsFields::LSN_VFT_COMBO,		LSN_LSTR( LSN_STR_VO_HIGH_FILTER ),				reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_HIGH_FILTER )) },
		{ CVideoOptionsFields::LSN_VFT_COMBO,		LSN_LSTR( LSN_STR_VO_MAX_FILTER ),				reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_MAX_FILTER )) },

		// Palette.
		{ CVideoOptionsFields::LSN_VFT_COMBO,		LSN_LSTR( LSN_STR_VO_LOAD ),					reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_LOAD )) },
		{ CVideoOptionsFields::LSN_VFT_DISPLAY,		nullptr,										reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_PALETTE_DISPLAY )) },
		{ CVideoOptionsFields::LSN_VFT_CHECK,		LSN_LSTR( LSN_STR_VO_SHOW_COLOR_INDICES ),		reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_SHOW_COLOR_INDICES )) },

		// Shared by the composite filters.
		{ CVideoOptionsFields::LSN_VFT_FLOAT,		LSN_LSTR( LSN_STR_VO_HUE ),						reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_HUE )) },
		{ CVideoOptionsFields::LSN_VFT_FLOAT,		LSN_LSTR( LSN_STR_VO_BRIGHTNESS ),				reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_BRIGHTNESS )) },
		{ CVideoOptionsFields::LSN_VFT_FLOAT,		LSN_LSTR( LSN_STR_VO_CONTRAST ),				reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_CONTRAST )) },
		{ CVideoOptionsFields::LSN_VFT_FLOAT,		LSN_LSTR( LSN_STR_VO_SATURATION ),				reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_SATURATION )) },
		{ CVideoOptionsFields::LSN_VFT_FLOAT,		LSN_LSTR( LSN_STR_VO_WHITE_LEVEL ),				reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_WHITE_LEVEL )) },
		{ CVideoOptionsFields::LSN_VFT_FLOAT,		LSN_LSTR( LSN_STR_VO_BLACK_LEVEL ),				reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_BLACK_LEVEL )) },

		// Blargg.
		{ CVideoOptionsFields::LSN_VFT_FLOAT,		LSN_LSTR( LSN_STR_VO_BLARGG_GAMMA ),			reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_BLARGG_GAMMA )) },
		{ CVideoOptionsFields::LSN_VFT_FLOAT,		LSN_LSTR( LSN_STR_VO_SHARPNESS ),				reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_SHARPNESS )) },
		{ CVideoOptionsFields::LSN_VFT_FLOAT,		LSN_LSTR( LSN_STR_VO_FRINGING ),				reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_FRINGING )) },
		{ CVideoOptionsFields::LSN_VFT_FLOAT,		LSN_LSTR( LSN_STR_VO_BLEED ),					reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_BLARGG_BLEED )) },
		{ CVideoOptionsFields::LSN_VFT_FLOAT,		LSN_LSTR( LSN_STR_VO_ARTIFACTS ),				reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_ARTIFACTS )) },
		{ CVideoOptionsFields::LSN_VFT_CHECK,		LSN_LSTR( LSN_STR_VO_MERGE_FIELDS ),			reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_MERGE_FIELDS )) },

		// L. Spiro.
		{ CVideoOptionsFields::LSN_VFT_FLOAT,		LSN_LSTR( LSN_STR_VO_BLEED ),					reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_LSPIRO_BLEED )) },
		{ CVideoOptionsFields::LSN_VFT_COMBO,		LSN_LSTR( LSN_STR_VO_DEEP_PRESET ),				reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_DEEP_PRESET )) },
		{ CVideoOptionsFields::LSN_VFT_COMBO,		LSN_LSTR( LSN_STR_VO_CHROMA_FILTER ),			reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_CHROMA_FILTER )) },
		{ CVideoOptionsFields::LSN_VFT_FLOAT,		LSN_LSTR( LSN_STR_VO_CHROMA_PARM0 ),			reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_FILTER_PARMS )) },
		{ CVideoOptionsFields::LSN_VFT_FLOAT,		LSN_LSTR( LSN_STR_VO_CHROMA_PARM1 ),			reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_FILTER_PARMS )) },
		{ CVideoOptionsFields::LSN_VFT_INT,			LSN_LSTR( LSN_STR_VO_CHROMA_KERNEL ),			reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_CHROMA_KERNEL )) },
		{ CVideoOptionsFields::LSN_VFT_COMBO,		LSN_LSTR( LSN_STR_VO_LUMA_FILTER ),				reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_LUMA_FILTER )) },
		{ CVideoOptionsFields::LSN_VFT_FLOAT,		LSN_LSTR( LSN_STR_VO_LUMA_PARM0 ),				reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_FILTER_PARMS )) },
		{ CVideoOptionsFields::LSN_VFT_FLOAT,		LSN_LSTR( LSN_STR_VO_LUMA_PARM1 ),				reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_FILTER_PARMS )) },
		{ CVideoOptionsFields::LSN_VFT_INT,			LSN_LSTR( LSN_STR_VO_LUMA_KERNEL ),				reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_LUMA_KERNEL )) },
		{ CVideoOptionsFields::LSN_VFT_COMBO,		LSN_LSTR( LSN_STR_VO_VERTICAL_COMB ),			reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_VERTICAL_COMB )) },
		{ CVideoOptionsFields::LSN_VFT_COMBO,		LSN_LSTR( LSN_STR_VO_VERTICAL_COMB ),			reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_NTSC_COMB )) },
		{ CVideoOptionsFields::LSN_VFT_COMBO,		LSN_LSTR( LSN_STR_VO_NOISE_TYPE ),				reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_NOISE_TYPE )) },
		{ CVideoOptionsFields::LSN_VFT_FLOAT,		LSN_LSTR( LSN_STR_VO_NOISE_AMOUNT ),			reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_NOISE_AMOUNT )) },
		{ CVideoOptionsFields::LSN_VFT_FLOAT,		LSN_LSTR( LSN_STR_VO_BASE_LEVEL ),				reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_SW_BASE_LEVEL )) },
		{ CVideoOptionsFields::LSN_VFT_FLOAT,		LSN_LSTR( LSN_STR_VO_TIME ),					reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_SW_TIME )) },

		// LMP88959.
		{ CVideoOptionsFields::LSN_VFT_INT,			LSN_LSTR( LSN_STR_VO_NOISE ),					reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_LMP_NOISE )) },
		{ CVideoOptionsFields::LSN_VFT_CHECK,		LSN_LSTR( LSN_STR_VO_BLEND ),					reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_BLEND )) },
		{ CVideoOptionsFields::LSN_VFT_CHECK,		LSN_LSTR( LSN_STR_VO_VSYNC ),					reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_VSYNC )) },
		{ CVideoOptionsFields::LSN_VFT_CHECK,		LSN_LSTR( LSN_STR_VO_HSYNC ),					reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_HSYNC )) },
		{ CVideoOptionsFields::LSN_VFT_CHECK,		LSN_LSTR( LSN_STR_VO_CHROMA_CORRECTION ),		reinterpret_cast<const char *>(LSN_U8STR( LSN_STR_VO_TT_CHROMA_CORRECTION )) },
	};

	// == Groups.
	/** The Host Machine display fields. */
	static const CVideoOptionsFields::LSN_VIDEO_FIELD		s_vfHostDisplay[] = {
		CVideoOptionsFields::LSN_VF_MONITOR_COLOR_SPACE, CVideoOptionsFields::LSN_VF_MONITOR_GAMMA, CVideoOptionsFields::LSN_VF_WHITE_ADAPTATION,
		CVideoOptionsFields::LSN_VF_GAMUT, CVideoOptionsFields::LSN_VF_OUTPUT_RANGE,
	};
	/** The color-correction fields. */
	static const CVideoOptionsFields::LSN_VIDEO_FIELD		s_vfColorCorrection[] = {
		CVideoOptionsFields::LSN_VF_TV_PRESET, CVideoOptionsFields::LSN_VF_COLOR_SPACE, CVideoOptionsFields::LSN_VF_GAMMA,
		CVideoOptionsFields::LSN_VF_CRT_WHITE, CVideoOptionsFields::LSN_VF_CRT_BRIGHTNESS,
	};
	/** The color-correction fields of the L. Spiro filters (which also emulate the decoder chip). */
	static const CVideoOptionsFields::LSN_VIDEO_FIELD		s_vfColorCorrectionLSpiro[] = {
		CVideoOptionsFields::LSN_VF_TV_PRESET, CVideoOptionsFields::LSN_VF_COLOR_SPACE, CVideoOptionsFields::LSN_VF_DEMODULATOR,
		CVideoOptionsFields::LSN_VF_GAMMA, CVideoOptionsFields::LSN_VF_CRT_WHITE, CVideoOptionsFields::LSN_VF_CRT_BRIGHTNESS,
	};
	/** The phosphor-decay fields. */
	static const CVideoOptionsFields::LSN_VIDEO_FIELD		s_vfPhosphor[] = {
		CVideoOptionsFields::LSN_VF_PHOSPHOR_ENABLED, CVideoOptionsFields::LSN_VF_PHOSPHOR_LEVEL, CVideoOptionsFields::LSN_VF_PHOSPHOR_TIME,
		CVideoOptionsFields::LSN_VF_PHOSPHOR_RED, CVideoOptionsFields::LSN_VF_PHOSPHOR_BLUE,
	};
	/** The internal-scaling fields. */
	static const CVideoOptionsFields::LSN_VIDEO_FIELD		s_vfScaling[] = {
		CVideoOptionsFields::LSN_VF_SCALE_X, CVideoOptionsFields::LSN_VF_SCALE_Y, CVideoOptionsFields::LSN_VF_SCALE_DYNAMIC,
	};
	/** The convolution fields. */
	static const CVideoOptionsFields::LSN_VIDEO_FIELD		s_vfConvolution[] = {
		CVideoOptionsFields::LSN_VF_CONV_ENABLED, CVideoOptionsFields::LSN_VF_CONV_LOW, CVideoOptionsFields::LSN_VF_CONV_MEDIUM,
		CVideoOptionsFields::LSN_VF_CONV_HIGH, CVideoOptionsFields::LSN_VF_CONV_MAX,
	};
	/** The palette fields. */
	static const CVideoOptionsFields::LSN_VIDEO_FIELD		s_vfPalette[] = {
		CVideoOptionsFields::LSN_VF_PALETTE_LOAD, CVideoOptionsFields::LSN_VF_PALETTE_DISPLAY, CVideoOptionsFields::LSN_VF_PALETTE_SHOW_INDICES,
	};
	/** The Blargg fields. */
	static const CVideoOptionsFields::LSN_VIDEO_FIELD		s_vfBlargg[] = {
		CVideoOptionsFields::LSN_VF_HUE, CVideoOptionsFields::LSN_VF_BRIGHTNESS,
		CVideoOptionsFields::LSN_VF_CONTRAST, CVideoOptionsFields::LSN_VF_SATURATION, CVideoOptionsFields::LSN_VF_BLARGG_GAMMA,
		CVideoOptionsFields::LSN_VF_BLARGG_SHARPNESS, CVideoOptionsFields::LSN_VF_BLARGG_FRINGING, CVideoOptionsFields::LSN_VF_BLARGG_BLEED,
		CVideoOptionsFields::LSN_VF_BLARGG_ARTIFACTS, CVideoOptionsFields::LSN_VF_BLARGG_MERGE_FIELDS,
	};
	/** The L. Spiro standard fields. */
	static const CVideoOptionsFields::LSN_VIDEO_FIELD		s_vfLSpiroStandard[] = {
		CVideoOptionsFields::LSN_VF_HUE, CVideoOptionsFields::LSN_VF_BRIGHTNESS,
		CVideoOptionsFields::LSN_VF_CONTRAST, CVideoOptionsFields::LSN_VF_SATURATION, CVideoOptionsFields::LSN_VF_WHITE_LEVEL,
		CVideoOptionsFields::LSN_VF_BLACK_LEVEL, CVideoOptionsFields::LSN_VF_LSPIRO_BLEED,
	};
	/** The L. Spiro NTSC deep-internals fields. */
	static const CVideoOptionsFields::LSN_VIDEO_FIELD		s_vfLSpiroNtscDeep[] = {
		CVideoOptionsFields::LSN_VF_DEEP_PRESET,
		CVideoOptionsFields::LSN_VF_CHROMA_FILTER, CVideoOptionsFields::LSN_VF_CHROMA_PARM0, CVideoOptionsFields::LSN_VF_CHROMA_PARM1,
		CVideoOptionsFields::LSN_VF_CHROMA_KERNEL, CVideoOptionsFields::LSN_VF_LUMA_FILTER, CVideoOptionsFields::LSN_VF_LUMA_PARM0,
		CVideoOptionsFields::LSN_VF_LUMA_PARM1, CVideoOptionsFields::LSN_VF_LUMA_KERNEL, CVideoOptionsFields::LSN_VF_NTSC_COMB,
	};
	/** The L. Spiro PAL deep-internals fields. */
	static const CVideoOptionsFields::LSN_VIDEO_FIELD		s_vfLSpiroPalDeep[] = {
		CVideoOptionsFields::LSN_VF_DEEP_PRESET,
		CVideoOptionsFields::LSN_VF_CHROMA_FILTER, CVideoOptionsFields::LSN_VF_CHROMA_PARM0, CVideoOptionsFields::LSN_VF_CHROMA_PARM1,
		CVideoOptionsFields::LSN_VF_CHROMA_KERNEL, CVideoOptionsFields::LSN_VF_LUMA_FILTER, CVideoOptionsFields::LSN_VF_LUMA_PARM0,
		CVideoOptionsFields::LSN_VF_LUMA_PARM1, CVideoOptionsFields::LSN_VF_LUMA_KERNEL, CVideoOptionsFields::LSN_VF_VERTICAL_COMB,
	};
	/** The L. Spiro noise fields. */
	static const CVideoOptionsFields::LSN_VIDEO_FIELD		s_vfLSpiroNoise[] = {
		CVideoOptionsFields::LSN_VF_NOISE_TYPE, CVideoOptionsFields::LSN_VF_NOISE_AMOUNT,
	};
	/** The L. Spiro software phosphor-decay fields. */
	static const CVideoOptionsFields::LSN_VIDEO_FIELD		s_vfLSpiroSwPhosphor[] = {
		CVideoOptionsFields::LSN_VF_SW_PHOSPHOR_LEVEL, CVideoOptionsFields::LSN_VF_SW_PHOSPHOR_TIME,
	};
	/** The LMP88959 NTSC fields. */
	static const CVideoOptionsFields::LSN_VIDEO_FIELD		s_vfLmpNtsc[] = {
		CVideoOptionsFields::LSN_VF_HUE, CVideoOptionsFields::LSN_VF_BRIGHTNESS,
		CVideoOptionsFields::LSN_VF_CONTRAST, CVideoOptionsFields::LSN_VF_SATURATION, CVideoOptionsFields::LSN_VF_WHITE_LEVEL,
		CVideoOptionsFields::LSN_VF_BLACK_LEVEL, CVideoOptionsFields::LSN_VF_LMP_NOISE, CVideoOptionsFields::LSN_VF_LMP_BLEND,
		CVideoOptionsFields::LSN_VF_LMP_VSYNC, CVideoOptionsFields::LSN_VF_LMP_HSYNC,
	};
	/** The LMP88959 PAL fields. */
	static const CVideoOptionsFields::LSN_VIDEO_FIELD		s_vfLmpPal[] = {
		CVideoOptionsFields::LSN_VF_BRIGHTNESS, CVideoOptionsFields::LSN_VF_CONTRAST, CVideoOptionsFields::LSN_VF_SATURATION,
		CVideoOptionsFields::LSN_VF_WHITE_LEVEL, CVideoOptionsFields::LSN_VF_BLACK_LEVEL, CVideoOptionsFields::LSN_VF_LMP_NOISE,
		CVideoOptionsFields::LSN_VF_LMP_BLEND, CVideoOptionsFields::LSN_VF_LMP_CHROMA_CORRECTION,
	};

	/** The left column of the Host Machine page. */
	static const CVideoOptionsFields::LSN_FIELD_GROUP		s_fgHostLeft[] = {
		{ LSN_LSTR( LSN_STR_VO_GROUP_DISPLAY ),				s_vfHostDisplay,			std::size( s_vfHostDisplay ) },
	};
	/** The left column of the filter pages (the common settings). */
	static const CVideoOptionsFields::LSN_FIELD_GROUP		s_fgCommon[] = {
		{ LSN_LSTR( LSN_STR_VO_GROUP_COLOR_CORRECTION ),	s_vfColorCorrection,		std::size( s_vfColorCorrection ) },
		{ LSN_LSTR( LSN_STR_VO_GROUP_PHOSPHOR_DECAY ),		s_vfPhosphor,				std::size( s_vfPhosphor ) },
		{ LSN_LSTR( LSN_STR_VO_GROUP_INTERNAL_SCALING ),	s_vfScaling,				std::size( s_vfScaling ) },
		{ LSN_LSTR( LSN_STR_VO_GROUP_CONVOLUTION ),			s_vfConvolution,			std::size( s_vfConvolution ),		CWinUtilities::LSN_VOWI_CONV_DEFAULTS },
	};
	/** The left column of the L. Spiro pages (the common settings with the decoder chip). */
	static const CVideoOptionsFields::LSN_FIELD_GROUP		s_fgCommonLSpiro[] = {
		{ LSN_LSTR( LSN_STR_VO_GROUP_COLOR_CORRECTION ),	s_vfColorCorrectionLSpiro,	std::size( s_vfColorCorrectionLSpiro ) },
		{ LSN_LSTR( LSN_STR_VO_GROUP_PHOSPHOR_DECAY ),		s_vfPhosphor,				std::size( s_vfPhosphor ) },
		{ LSN_LSTR( LSN_STR_VO_GROUP_INTERNAL_SCALING ),	s_vfScaling,				std::size( s_vfScaling ) },
		{ LSN_LSTR( LSN_STR_VO_GROUP_CONVOLUTION ),			s_vfConvolution,			std::size( s_vfConvolution ),		CWinUtilities::LSN_VOWI_CONV_DEFAULTS },
	};
	/** The right column of the Palette page. */
	static const CVideoOptionsFields::LSN_FIELD_GROUP		s_fgPalette[] = {
		{ LSN_LSTR( LSN_STR_VO_GROUP_PALETTE ),				s_vfPalette,				std::size( s_vfPalette ) },
	};
	/** The right column of the Blargg page. */
	static const CVideoOptionsFields::LSN_FIELD_GROUP		s_fgBlargg[] = {
		{ LSN_LSTR( LSN_STR_VO_GROUP_BLARGG ),				s_vfBlargg,					std::size( s_vfBlargg ) },
	};
	/** The right column of the L. Spiro NTSC page. */
	static const CVideoOptionsFields::LSN_FIELD_GROUP		s_fgLSpiroNtsc[] = {
		{ LSN_LSTR( LSN_STR_VO_GROUP_STANDARD ),			s_vfLSpiroStandard,			std::size( s_vfLSpiroStandard ) },
		{ LSN_LSTR( LSN_STR_VO_GROUP_DEEP ),				s_vfLSpiroNtscDeep,			std::size( s_vfLSpiroNtscDeep ) },
		{ LSN_LSTR( LSN_STR_VO_GROUP_NOISE ),				s_vfLSpiroNoise,			std::size( s_vfLSpiroNoise ) },
		{ LSN_LSTR( LSN_STR_VO_GROUP_SW_PHOSPHOR ),			s_vfLSpiroSwPhosphor,		std::size( s_vfLSpiroSwPhosphor ) },
	};
	/** The right column of the L. Spiro PAL page. */
	static const CVideoOptionsFields::LSN_FIELD_GROUP		s_fgLSpiroPal[] = {
		{ LSN_LSTR( LSN_STR_VO_GROUP_STANDARD ),			s_vfLSpiroStandard,			std::size( s_vfLSpiroStandard ) },
		{ LSN_LSTR( LSN_STR_VO_GROUP_DEEP ),				s_vfLSpiroPalDeep,			std::size( s_vfLSpiroPalDeep ) },
		{ LSN_LSTR( LSN_STR_VO_GROUP_NOISE ),				s_vfLSpiroNoise,			std::size( s_vfLSpiroNoise ) },
		{ LSN_LSTR( LSN_STR_VO_GROUP_SW_PHOSPHOR ),			s_vfLSpiroSwPhosphor,		std::size( s_vfLSpiroSwPhosphor ) },
	};
	/** The right column of the LMP88959 NTSC page. */
	static const CVideoOptionsFields::LSN_FIELD_GROUP		s_fgLmpNtsc[] = {
		{ LSN_LSTR( LSN_STR_VO_GROUP_LMP ),					s_vfLmpNtsc,				std::size( s_vfLmpNtsc ) },
	};
	/** The right column of the LMP88959 PAL page. */
	static const CVideoOptionsFields::LSN_FIELD_GROUP		s_fgLmpPal[] = {
		{ LSN_LSTR( LSN_STR_VO_GROUP_LMP ),					s_vfLmpPal,					std::size( s_vfLmpPal ) },
	};

	// == Presets.
	/** Every region. */
	static const LSN_PPU_METRICS							s_pmAll[] = { LSN_PM_NTSC_M, LSN_PM_NTSC_J, LSN_PM_PAL, LSN_PM_DENDY, LSN_PM_PALM, LSN_PM_PALN };
	/** The NTSC regions. */
	static const LSN_PPU_METRICS							s_pmNtsc[] = { LSN_PM_NTSC_M, LSN_PM_NTSC_J };
	/** The PAL regions. */
	static const LSN_PPU_METRICS							s_pmPal[] = { LSN_PM_PAL, LSN_PM_DENDY, LSN_PM_PALM, LSN_PM_PALN };
	/** The PAL regions that the LMP88959 PAL filter handles. */
	static const LSN_PPU_METRICS							s_pmLmpPal[] = { LSN_PM_PAL, LSN_PM_DENDY };

	/** The palette presets. */
	static const CVideoOptionsFields::LSN_PALETTE_PRESET	s_ppPalettes[] = {
		{ LSN_LSTR( LSN_STR_VO_PAL_STANDARD_NTSC ),			L"2C02G_wiki.fpal",								CNesPalette::LSN_G_CRT2,	1.0f,	0.0181f * 8.0f,
			CColorSpace::LSN_CSP_SRGB,		CColorSpace::LSN_CSP_SONY_P22_TRINITRON },
		{ LSN_LSTR( LSN_STR_VO_PAL_STANDARD_PAL ),			L"2C07_wiki.fpal",								CNesPalette::LSN_G_CRT1,	1.0f,	0.0181f * 8.0f,
			CColorSpace::LSN_CSP_SRGB,		CColorSpace::LSN_CSP_SRGB },
		{ LSN_LSTR( LSN_STR_VO_PAL_TOSHIBA ),				L"toshiba_cf2005_1985_beta_2025_05_26.pal",		CNesPalette::LSN_G_sRGB,	1.0f,	0.0181f * 8.0f,
			CColorSpace::LSN_CSP_SRGB,		CColorSpace::LSN_CSP_SRGB },
		{ LSN_LSTR( LSN_STR_VO_PAL_PANASONIC ),				L"2000_US_panasonic_ct36d30b_beta.pal",			CNesPalette::LSN_G_sRGB,	1.0f,	0.0181f * 8.0f,
			CColorSpace::LSN_CSP_SRGB,		CColorSpace::LSN_CSP_SRGB },
	};

	/** The item data of the From File... entry of the palette combo box. */
	static constexpr LPARAM									LSN_PALETTE_FROM_FILE = -2;

	/** The item data of the entry of the palette combo box that shows a palette loaded from a file. */
	static constexpr LPARAM									LSN_PALETTE_CUSTOM = -1;

	// == Helpers.
	/**
	 * Gets the L. Spiro settings shown by the dialog (the GPU version if a GPU API is available).
	 *
	 * \param _froOptions The settings of a region.
	 * \return Returns the L. Spiro settings shown by the dialog.
	 */
	static inline LSN_LSPIRO_OPTIONS &						ShownLSpiro( LSN_FILTER_REGION_OPTIONS &_froOptions ) {
#ifdef LSN_VO_HAVE_GPU
		return _froOptions.lsoLSpiroGpu;
#else
		return _froOptions.lsoLSpiroCpu;
#endif	// #ifdef LSN_VO_HAVE_GPU
	}

	/**
	 * Gets the LMP88959 settings shown by the dialog (the GPU version if a GPU API is available).
	 *
	 * \param _froOptions The settings of a region.
	 * \param _bPal If true, the PAL settings are returned, otherwise the NTSC settings.
	 * \return Returns the LMP88959 settings shown by the dialog.
	 */
	static inline LSN_LMP88959_SETTINGS &					ShownLmp( LSN_FILTER_REGION_OPTIONS &_froOptions, bool _bPal ) {
#ifdef LSN_VO_HAVE_GPU
		return _bPal ? _froOptions.lsLmpPalGpu : _froOptions.lsLmpNtscGpu;
#else
		return _bPal ? _froOptions.lsLmpPalCpu : _froOptions.lsLmpNtscCpu;
#endif	// #ifdef LSN_VO_HAVE_GPU
	}

	/**
	 * Determines whether a page is one of the L. Spiro pages.
	 *
	 * \param _vpkKind The kind of page.
	 * \return Returns true if the page is one of the L. Spiro pages.
	 */
	static inline bool										IsLSpiro( CVideoOptionsFields::LSN_VIDEO_PAGE_KIND _vpkKind ) {
		return _vpkKind == CVideoOptionsFields::LSN_VPK_LSPIRO_NTSC || _vpkKind == CVideoOptionsFields::LSN_VPK_LSPIRO_PAL;
	}

	/**
	 * Determines whether a page is one of the LMP88959 pages.
	 *
	 * \param _vpkKind The kind of page.
	 * \return Returns true if the page is one of the LMP88959 pages.
	 */
	static inline bool										IsLmp( CVideoOptionsFields::LSN_VIDEO_PAGE_KIND _vpkKind ) {
		return _vpkKind == CVideoOptionsFields::LSN_VPK_LMP_NTSC || _vpkKind == CVideoOptionsFields::LSN_VPK_LMP_PAL;
	}

	/**
	 * Gets the name of a CRT gamma curve.
	 *
	 * \param _gGamma The gamma curve.
	 * \return Returns the name of the gamma curve.
	 */
	static const wchar_t *									GammaName( CNesPalette::LSN_GAMMA _gGamma ) {
		switch ( _gGamma ) {
			case CNesPalette::LSN_G_NONE : { return LSN_LSTR( LSN_STR_VO_NONE ); }
			case CNesPalette::LSN_G_AUTO : { return LSN_LSTR( LSN_STR_VO_GAMMA_AUTO ); }
			case CNesPalette::LSN_G_CRT1 : { return LSN_LSTR( LSN_STR_VO_GAMMA_CRT1 ); }
			case CNesPalette::LSN_G_CRT2 : { return LSN_LSTR( LSN_STR_VO_GAMMA_CRT2 ); }
			case CNesPalette::LSN_G_sRGB : { return LSN_LSTR( LSN_STR_VO_GAMMA_SRGB ); }
			case CNesPalette::LSN_G_SMPTE170M : { return LSN_LSTR( LSN_STR_VO_GAMMA_SMPTE170M ); }
			case CNesPalette::LSN_G_DCIP3 : { return LSN_LSTR( LSN_STR_VO_GAMMA_DCIP3 ); }
			case CNesPalette::LSN_G_ADOBERGB : { return LSN_LSTR( LSN_STR_VO_GAMMA_ADOBERGB ); }
			case CNesPalette::LSN_G_SMPTE240M : { return LSN_LSTR( LSN_STR_VO_GAMMA_SMPTE240M ); }
			case CNesPalette::LSN_G_POW_1_96 : { return LSN_LSTR( LSN_STR_VO_GAMMA_POW_1_96 ); }
			case CNesPalette::LSN_G_POW_2_0 : { return LSN_LSTR( LSN_STR_VO_GAMMA_POW_2_0 ); }
			case CNesPalette::LSN_G_POW_2_2 : { return LSN_LSTR( LSN_STR_VO_GAMMA_POW_2_2 ); }
			case CNesPalette::LSN_G_POW_2_35 : { return LSN_LSTR( LSN_STR_VO_GAMMA_POW_2_35 ); }
			case CNesPalette::LSN_G_POW_2_4 : { return LSN_LSTR( LSN_STR_VO_GAMMA_POW_2_4 ); }
			case CNesPalette::LSN_G_POW_2_5 : { return LSN_LSTR( LSN_STR_VO_GAMMA_POW_2_5 ); }
			case CNesPalette::LSN_G_POW_2_7 : { return LSN_LSTR( LSN_STR_VO_GAMMA_POW_2_7 ); }
			case CNesPalette::LSN_G_POW_2_8 : { return LSN_LSTR( LSN_STR_VO_GAMMA_POW_2_8 ); }
			default : { return L""; }
		}
	}

	/**
	 * Gets the name of a convolution (resampling) filter.
	 *
	 * \param _ffFilter The filter (LSN_FF_TOTAL for none).
	 * \return Returns the name of the filter.
	 */
	static const wchar_t *									ResamplerName( CResamplerBase::LSN_FILTER_FUNCS _ffFilter ) {
		switch ( _ffFilter ) {
			case CResamplerBase::LSN_FF_POINT : { return LSN_LSTR( LSN_STR_VO_RF_POINT ); }
			case CResamplerBase::LSN_FF_LINEAR : { return LSN_LSTR( LSN_STR_VO_RF_LINEAR ); }
			case CResamplerBase::LSN_FF_QUADRATICSHARP : { return LSN_LSTR( LSN_STR_VO_RF_QUADRATICSHARP ); }
			case CResamplerBase::LSN_FF_QUADRATIC : { return LSN_LSTR( LSN_STR_VO_RF_QUADRATIC ); }
			case CResamplerBase::LSN_FF_QUADRATICAPPROX : { return LSN_LSTR( LSN_STR_VO_RF_QUADRATICAPPROX ); }
			case CResamplerBase::LSN_FF_QUADRATICMIX : { return LSN_LSTR( LSN_STR_VO_RF_QUADRATICMIX ); }
			case CResamplerBase::LSN_FF_KAISER : { return LSN_LSTR( LSN_STR_VO_RF_KAISER ); }
			case CResamplerBase::LSN_FF_LANCZOS2 : { return LSN_LSTR( LSN_STR_VO_RF_LANCZOS2 ); }
			case CResamplerBase::LSN_FF_LANCZOS3 : { return LSN_LSTR( LSN_STR_VO_RF_LANCZOS3 ); }
			case CResamplerBase::LSN_FF_LANCZOS4 : { return LSN_LSTR( LSN_STR_VO_RF_LANCZOS4 ); }
			case CResamplerBase::LSN_FF_LANCZOS6 : { return LSN_LSTR( LSN_STR_VO_RF_LANCZOS6 ); }
			case CResamplerBase::LSN_FF_LANCZOS8 : { return LSN_LSTR( LSN_STR_VO_RF_LANCZOS8 ); }
			case CResamplerBase::LSN_FF_LANCZOS12 : { return LSN_LSTR( LSN_STR_VO_RF_LANCZOS12 ); }
			case CResamplerBase::LSN_FF_LANCZOS64 : { return LSN_LSTR( LSN_STR_VO_RF_LANCZOS64 ); }
			case CResamplerBase::LSN_FF_MITCHELL : { return LSN_LSTR( LSN_STR_VO_RF_MITCHELL ); }
			case CResamplerBase::LSN_FF_ROBIDOUX : { return LSN_LSTR( LSN_STR_VO_RF_ROBIDOUX ); }
			case CResamplerBase::LSN_FF_ROBIDOUXSHARP : { return LSN_LSTR( LSN_STR_VO_RF_ROBIDOUXSHARP ); }
			case CResamplerBase::LSN_FF_ROBIDOUXSOFT : { return LSN_LSTR( LSN_STR_VO_RF_ROBIDOUXSOFT ); }
			case CResamplerBase::LSN_FF_CATMULLROM : { return LSN_LSTR( LSN_STR_VO_RF_CATMULLROM ); }
			case CResamplerBase::LSN_FF_BSPLINE : { return LSN_LSTR( LSN_STR_VO_RF_BSPLINE ); }
			case CResamplerBase::LSN_FF_ADOBEBICUBIC : { return LSN_LSTR( LSN_STR_VO_RF_ADOBEBICUBIC ); }
			case CResamplerBase::LSN_FF_CARDINALSPLINEUNIFORM : { return LSN_LSTR( LSN_STR_VO_RF_CARDINALSPLINEUNIFORM ); }
			case CResamplerBase::LSN_FF_HERMITE : { return LSN_LSTR( LSN_STR_VO_RF_HERMITE ); }
			case CResamplerBase::LSN_FF_HAMMING : { return LSN_LSTR( LSN_STR_VO_RF_HAMMING ); }
			case CResamplerBase::LSN_FF_HANNING : { return LSN_LSTR( LSN_STR_VO_RF_HANNING ); }
			case CResamplerBase::LSN_FF_BLACKMAN : { return LSN_LSTR( LSN_STR_VO_RF_BLACKMAN ); }
			case CResamplerBase::LSN_FF_JINC : { return LSN_LSTR( LSN_STR_VO_RF_JINC ); }
			case CResamplerBase::LSN_FF_GAUSSIANSHARP : { return LSN_LSTR( LSN_STR_VO_RF_GAUSSIANSHARP ); }
			case CResamplerBase::LSN_FF_GAUSSIAN : { return LSN_LSTR( LSN_STR_VO_RF_GAUSSIAN ); }
			case CResamplerBase::LSN_FF_BELL : { return LSN_LSTR( LSN_STR_VO_RF_BELL ); }
			default : { return LSN_LSTR( LSN_STR_VO_NONE ); }
		}
	}

	/**
	 * Gets the name of an L. Spiro kernel function.
	 *
	 * \param _ffFunc The kernel function.
	 * \return Returns the name of the kernel function.
	 */
	static const wchar_t *									KernelName( LSN_FILTER_FUNC _ffFunc ) {
		switch ( _ffFunc ) {
			case LSN_FF_BOX : { return LSN_LSTR( LSN_STR_VO_FF_BOX ); }
			case LSN_FF_LANCZOSX : { return LSN_LSTR( LSN_STR_VO_FF_LANCZOS ); }
			case LSN_FF_KAISER : { return LSN_LSTR( LSN_STR_VO_FF_KAISER ); }
			case LSN_FF_BLACKMAN : { return LSN_LSTR( LSN_STR_VO_FF_BLACKMAN ); }
			case LSN_FF_GAUSSIANX : { return LSN_LSTR( LSN_STR_VO_FF_GAUSSIAN ); }
			case LSN_FF_CARDINAL_SPLINE_UNIFORM : { return LSN_LSTR( LSN_STR_VO_FF_CARDINAL_SPLINE_UNIFORM ); }
			case LSN_FF_BARTLETT : { return LSN_LSTR( LSN_STR_VO_FF_BARTLETT ); }
			case LSN_FF_CRT_HUMP : { return LSN_LSTR( LSN_STR_VO_FF_CRT_HUMP ); }
			default : { return L""; }
		}
	}

	/**
	 * Determines whether the settings being edited are those of the L. Spiro PAL filter.
	 *
	 * \param _ftTarget The settings being edited.
	 * \return Returns true if the page is the L. Spiro PAL page.
	 */
	static inline bool										IsLSpiroPal( const CVideoOptionsFields::LSN_FIELD_TARGET &_ftTarget ) {
		return _ftTarget.vpkKind == CVideoOptionsFields::LSN_VPK_LSPIRO_PAL;
	}

	/**
	 * Adds an entry to a combo box.
	 *
	 * \param _pcbCombo The combo box.
	 * \param _pwcName The text of the entry.
	 * \param _lpData The item data of the entry.
	 */
	static void												AddEntry( CComboBox * _pcbCombo, const wchar_t * _pwcName, LPARAM _lpData ) {
		INT iIdx = _pcbCombo->AddString( _pwcName );
		if ( iIdx >= 0 ) { _pcbCombo->SetItemData( iIdx, _lpData ); }
	}

	/**
	 * Gets the index of the palette preset that a palette path loads, if any.
	 *
	 * \param _ftTarget The settings being edited.
	 * \return Returns the index of the palette preset, or LSN_PALETTE_CUSTOM.
	 */
	static LPARAM											PalettePresetOf( const CVideoOptionsFields::LSN_FIELD_TARGET &_ftTarget ) {
		if ( !_ftTarget.ppoPalette ) { return LSN_PALETTE_CUSTOM; }
		std::wstring wsPath = _ftTarget.ppoPalette->wsPath;
		if ( wsPath.empty() ) {
			// The default palettes (see CBeesNes::ApplyPaletteOptions()).
			return (_ftTarget.pmRegion == LSN_PM_NTSC_M || _ftTarget.pmRegion == LSN_PM_NTSC_J) ? 0 : 1;
		}
		try {
			std::filesystem::path pFile = std::filesystem::path( wsPath ).filename();
			for ( size_t I = 0; I < std::size( s_ppPalettes ); ++I ) {
				if ( ::_wcsicmp( pFile.c_str(), s_ppPalettes[I].pwcFile ) == 0 ) { return LPARAM( I ); }
			}
		}
		catch ( ... ) {}
		return LSN_PALETTE_CUSTOM;
	}

	// == Functions.
	/**
	 * Gets information about a field.
	 *
	 * \param _vfField The field.
	 * \return Returns information about the field.
	 */
	const CVideoOptionsFields::LSN_FIELD_INFO & CVideoOptionsFields::Info( LSN_VIDEO_FIELD _vfField ) {
		return s_fiFields[(_vfField < LSN_VF_TOTAL) ? _vfField : 0];
	}

	/**
	 * Gets the label of a field for the current settings.  The labels of the kernel parameters name what the selected filter function
	 *	does with them (or say that they are unused); the other labels are fixed (Info()).
	 *
	 * \param _vfField The field.
	 * \param _ftTarget The settings being edited.
	 * \return Returns the label of the field.
	 */
	const wchar_t * CVideoOptionsFields::Label( LSN_VIDEO_FIELD _vfField, const LSN_FIELD_TARGET &_ftTarget ) {
		bool bLuma;
		size_t sParm;
		switch ( _vfField ) {
			case LSN_VF_CHROMA_PARM0 : { bLuma = false; sParm = 0; break; }
			case LSN_VF_CHROMA_PARM1 : { bLuma = false; sParm = 1; break; }
			case LSN_VF_LUMA_PARM0 : { bLuma = true; sParm = 0; break; }
			case LSN_VF_LUMA_PARM1 : { bLuma = true; sParm = 1; break; }
			default : { return Info( _vfField ).pwcLabel; }
		}
		double dFunc = 0.0;
		if ( !GetValue( bLuma ? LSN_VF_LUMA_FILTER : LSN_VF_CHROMA_FILTER, _ftTarget, dFunc ) ) { return Info( _vfField ).pwcLabel; }
		switch ( LSN_FILTER_FUNC( uint32_t( dFunc ) ) ) {
			case LSN_FF_LANCZOSX : {
				if ( sParm == 0 ) { return bLuma ? LSN_LSTR( LSN_STR_VO_LUMA_LOBES ) : LSN_LSTR( LSN_STR_VO_CHROMA_LOBES ); }
				break;
			}
			case LSN_FF_KAISER : {
				if ( sParm == 0 ) { return bLuma ? LSN_LSTR( LSN_STR_VO_LUMA_BETA ) : LSN_LSTR( LSN_STR_VO_CHROMA_BETA ); }
				break;
			}
			case LSN_FF_BLACKMAN : {
				if ( sParm == 0 ) { return bLuma ? LSN_LSTR( LSN_STR_VO_LUMA_CUTOFF ) : LSN_LSTR( LSN_STR_VO_CHROMA_CUTOFF ); }
				return bLuma ? LSN_LSTR( LSN_STR_VO_LUMA_SIGNAL_HZ ) : LSN_LSTR( LSN_STR_VO_CHROMA_SIGNAL_HZ );
			}
			case LSN_FF_CRT_HUMP : {
				if ( sParm == 0 ) { return bLuma ? LSN_LSTR( LSN_STR_VO_LUMA_FLAT_WIDTH ) : LSN_LSTR( LSN_STR_VO_CHROMA_FLAT_WIDTH ); }
				return bLuma ? LSN_LSTR( LSN_STR_VO_LUMA_POWER ) : LSN_LSTR( LSN_STR_VO_CHROMA_POWER );
			}
			default : {}
		}
		return bLuma ? LSN_LSTR( LSN_STR_VO_LUMA_UNUSED ) : LSN_LSTR( LSN_STR_VO_CHROMA_UNUSED );
	}

	/**
	 * Gets the groups shown in the left column of a page (the common settings, or the display settings of the Host Machine page).
	 *
	 * \param _vpkKind The kind of page.
	 * \param _sTotal Holds the returned number of groups.
	 * \return Returns the groups.
	 */
	const CVideoOptionsFields::LSN_FIELD_GROUP * CVideoOptionsFields::LeftGroups( LSN_VIDEO_PAGE_KIND _vpkKind, size_t &_sTotal ) {
		if ( _vpkKind == LSN_VPK_HOST ) {
			_sTotal = std::size( s_fgHostLeft );
			return s_fgHostLeft;
		}
		if ( _vpkKind == LSN_VPK_LSPIRO_NTSC || _vpkKind == LSN_VPK_LSPIRO_PAL ) {
			_sTotal = std::size( s_fgCommonLSpiro );
			return s_fgCommonLSpiro;
		}
		_sTotal = std::size( s_fgCommon );
		return s_fgCommon;
	}

	/**
	 * Gets the groups shown in the right column of a page (the filter-specific settings).
	 *
	 * \param _vpkKind The kind of page.
	 * \param _sTotal Holds the returned number of groups.
	 * \return Returns the groups.
	 */
	const CVideoOptionsFields::LSN_FIELD_GROUP * CVideoOptionsFields::RightGroups( LSN_VIDEO_PAGE_KIND _vpkKind, size_t &_sTotal ) {
		switch ( _vpkKind ) {
			case LSN_VPK_PALETTE : { _sTotal = std::size( s_fgPalette ); return s_fgPalette; }
			case LSN_VPK_BLARGG : { _sTotal = std::size( s_fgBlargg ); return s_fgBlargg; }
			case LSN_VPK_LSPIRO_NTSC : { _sTotal = std::size( s_fgLSpiroNtsc ); return s_fgLSpiroNtsc; }
			case LSN_VPK_LSPIRO_PAL : { _sTotal = std::size( s_fgLSpiroPal ); return s_fgLSpiroPal; }
			case LSN_VPK_LMP_NTSC : { _sTotal = std::size( s_fgLmpNtsc ); return s_fgLmpNtsc; }
			case LSN_VPK_LMP_PAL : { _sTotal = std::size( s_fgLmpPal ); return s_fgLmpPal; }
			default : { _sTotal = 0; return nullptr; }
		}
	}

	/**
	 * Gets the regional presets of a page.
	 *
	 * \param _vpkKind The kind of page.
	 * \param _sTotal Holds the returned number of presets.
	 * \return Returns the regions that have presets for the given kind of page (empty for the Host Machine page).
	 */
	const LSN_PPU_METRICS * CVideoOptionsFields::Presets( LSN_VIDEO_PAGE_KIND _vpkKind, size_t &_sTotal ) {
		switch ( _vpkKind ) {
			case LSN_VPK_PALETTE : { _sTotal = std::size( s_pmAll ); return s_pmAll; }
			case LSN_VPK_BLARGG : {}				LSN_FALLTHROUGH
			case LSN_VPK_LSPIRO_NTSC : {}			LSN_FALLTHROUGH
			case LSN_VPK_LMP_NTSC : { _sTotal = std::size( s_pmNtsc ); return s_pmNtsc; }
			case LSN_VPK_LSPIRO_PAL : { _sTotal = std::size( s_pmPal ); return s_pmPal; }
			case LSN_VPK_LMP_PAL : { _sTotal = std::size( s_pmLmpPal ); return s_pmLmpPal; }
			default : { _sTotal = 0; return nullptr; }
		}
	}

	/**
	 * Gets the name of a region as shown in the regional-preset combo box.
	 *
	 * \param _pmRegion The region.
	 * \return Returns the name of the region.
	 */
	const wchar_t * CVideoOptionsFields::RegionName( LSN_PPU_METRICS _pmRegion ) {
		switch ( _pmRegion ) {
			case LSN_PM_NTSC_M : { return LSN_LSTR( LSN_STR_VO_REGION_NTSC_M ); }
			case LSN_PM_NTSC_J : { return LSN_LSTR( LSN_STR_VO_REGION_NTSC_J ); }
			case LSN_PM_PAL : { return LSN_LSTR( LSN_STR_VO_REGION_PAL_I ); }
			case LSN_PM_DENDY : { return LSN_LSTR( LSN_STR_VO_REGION_PAL_DK ); }
			case LSN_PM_PALM : { return LSN_LSTR( LSN_STR_VO_REGION_PAL_M ); }
			case LSN_PM_PALN : { return LSN_LSTR( LSN_STR_VO_REGION_PAL_N ); }
			default : { return L""; }
		}
	}

	/**
	 * Gets the name of a filter tab.
	 *
	 * \param _vpkKind The kind of page.
	 * \return Returns the name of the tab.
	 */
	const wchar_t * CVideoOptionsFields::TabName( LSN_VIDEO_PAGE_KIND _vpkKind ) {
		switch ( _vpkKind ) {
			case LSN_VPK_HOST : { return LSN_LSTR( LSN_STR_VO_HOST_MACHINE ); }
			case LSN_VPK_PALETTE : { return LSN_LSTR( LSN_STR_VO_TAB_PALETTE ); }
			case LSN_VPK_BLARGG : { return LSN_LSTR( LSN_STR_VO_TAB_BLARGG ); }
			case LSN_VPK_LSPIRO_NTSC : { return LSN_LSTR( LSN_STR_VO_TAB_LSPIRO_NTSC ); }
			case LSN_VPK_LSPIRO_PAL : { return LSN_LSTR( LSN_STR_VO_TAB_LSPIRO_PAL ); }
			case LSN_VPK_LMP_NTSC : { return LSN_LSTR( LSN_STR_VO_TAB_LMP_NTSC ); }
			case LSN_VPK_LMP_PAL : { return LSN_LSTR( LSN_STR_VO_TAB_LMP_PAL ); }
			default : { return L""; }
		}
	}

	/**
	 * Gets the palette presets.
	 *
	 * \param _sTotal Holds the returned number of presets.
	 * \return Returns the palette presets.
	 */
	const CVideoOptionsFields::LSN_PALETTE_PRESET * CVideoOptionsFields::PalettePresets( size_t &_sTotal ) {
		_sTotal = std::size( s_ppPalettes );
		return s_ppPalettes;
	}

	/**
	 * Fills the combo box of a field with its entries.
	 *
	 * \param _vfField The field.
	 * \param _ftTarget The settings being edited (some lists depend on them).
	 * \param _pcbCombo The combo box to fill.
	 */
	void CVideoOptionsFields::FillCombo( LSN_VIDEO_FIELD _vfField, const LSN_FIELD_TARGET &_ftTarget, CComboBox * _pcbCombo ) {
		if ( !_pcbCombo ) { return; }
		_pcbCombo->ResetContent();
		switch ( _vfField ) {
			case LSN_VF_MONITOR_COLOR_SPACE : {}	LSN_FALLTHROUGH
			case LSN_VF_COLOR_SPACE : {
				for ( uint32_t I = 0; I < CColorSpace::LSN_CSP_TOTAL; ++I ) {
					AddEntry( _pcbCombo, CColorSpace::Data( CColorSpace::LSN_COLOR_SPACE( I ) ).pwcName, LPARAM( I ) );
				}
				break;
			}
			case LSN_VF_MONITOR_GAMMA : {
				// The CRT curves (and Automatic) are not monitor curves.
				for ( uint32_t I = 0; I < CNesPalette::LSN_G_TOTAL; ++I ) {
					if ( CColorSpace::IsMonitorGamma( CNesPalette::LSN_GAMMA( I ) ) ) {
						AddEntry( _pcbCombo, GammaName( CNesPalette::LSN_GAMMA( I ) ), LPARAM( I ) );
					}
				}
				break;
			}
			case LSN_VF_GAMMA : {
				for ( uint32_t I = 0; I < CNesPalette::LSN_G_TOTAL; ++I ) {
					AddEntry( _pcbCombo, GammaName( CNesPalette::LSN_GAMMA( I ) ), LPARAM( I ) );
				}
				break;
			}
			case LSN_VF_WHITE_ADAPTATION : {
				AddEntry( _pcbCombo, LSN_LSTR( LSN_STR_VO_NONE ), CColorSpace::LSN_WA_NONE );
				AddEntry( _pcbCombo, LSN_LSTR( LSN_STR_VO_WA_BRADFORD ), CColorSpace::LSN_WA_BRADFORD );
				AddEntry( _pcbCombo, LSN_LSTR( LSN_STR_VO_WA_VON_KRIES ), CColorSpace::LSN_WA_VON_KRIES );
				AddEntry( _pcbCombo, LSN_LSTR( LSN_STR_VO_WA_XYZ_SCALING ), CColorSpace::LSN_WA_XYZ_SCALING );
				AddEntry( _pcbCombo, LSN_LSTR( LSN_STR_VO_WA_CAT02 ), CColorSpace::LSN_WA_CAT02 );
				break;
			}
			case LSN_VF_GAMUT : {
				AddEntry( _pcbCombo, LSN_LSTR( LSN_STR_VO_GM_PRESERVE_HUE ), CColorSpace::LSN_GM_PRESERVE_HUE );
				AddEntry( _pcbCombo, LSN_LSTR( LSN_STR_VO_GM_CLIP ), CColorSpace::LSN_GM_CLIP );
				break;
			}
			case LSN_VF_OUTPUT_RANGE : {
				AddEntry( _pcbCombo, LSN_LSTR( LSN_STR_VO_OR_FULL ), CColorSpace::LSN_OR_FULL );
				AddEntry( _pcbCombo, LSN_LSTR( LSN_STR_VO_OR_LIMITED ), CColorSpace::LSN_OR_LIMITED );
				break;
			}
			case LSN_VF_DEMODULATOR : {
				for ( uint32_t I = 0; I < CColorSpace::LSN_DM_TOTAL; ++I ) {
					AddEntry( _pcbCombo, CColorSpace::Demodulator( CColorSpace::LSN_DEMODULATOR( I ) ).pwcName, LPARAM( I ) );
				}
				break;
			}
			case LSN_VF_TV_PRESET : {
				AddEntry( _pcbCombo, LSN_LSTR( LSN_STR_VO_CUSTOM ), -1 );
				for ( uint32_t I = 0; I < CColorSpace::LSN_CSP_TOTAL; ++I ) {
					const CColorSpace::LSN_COLOR_SPACE_DATA & csdData = CColorSpace::Data( CColorSpace::LSN_COLOR_SPACE( I ) );
					if ( csdData.bTvPreset ) { AddEntry( _pcbCombo, csdData.pwcName, LPARAM( I ) ); }
				}
				break;
			}
			case LSN_VF_CONV_LOW : {}				LSN_FALLTHROUGH
			case LSN_VF_CONV_MEDIUM : {}			LSN_FALLTHROUGH
			case LSN_VF_CONV_HIGH : {}				LSN_FALLTHROUGH
			case LSN_VF_CONV_MAX : {
				AddEntry( _pcbCombo, ResamplerName( CResamplerBase::LSN_FF_TOTAL ), LPARAM( CResamplerBase::LSN_FF_TOTAL ) );
				for ( size_t I = 0; I < CResamplerBase::LSN_FF_TOTAL; ++I ) {
					AddEntry( _pcbCombo, ResamplerName( CResamplerBase::LSN_FILTER_FUNCS( I ) ), LPARAM( I ) );
				}
				break;
			}
			case LSN_VF_PALETTE_LOAD : {
				if ( PalettePresetOf( _ftTarget ) == LSN_PALETTE_CUSTOM && _ftTarget.ppoPalette ) {
					try {
						std::wstring wsName = std::filesystem::path( _ftTarget.ppoPalette->wsPath ).filename().wstring();
						AddEntry( _pcbCombo, wsName.c_str(), LSN_PALETTE_CUSTOM );
					}
					catch ( ... ) {}
				}
				for ( size_t I = 0; I < std::size( s_ppPalettes ); ++I ) {
					AddEntry( _pcbCombo, s_ppPalettes[I].pwcName, LPARAM( I ) );
				}
				AddEntry( _pcbCombo, LSN_LSTR( LSN_STR_VO_PAL_FROM_FILE ), LSN_PALETTE_FROM_FILE );
				break;
			}
			case LSN_VF_CHROMA_FILTER : {}			LSN_FALLTHROUGH
			case LSN_VF_LUMA_FILTER : {
				for ( uint32_t I = 0; I < LSN_FF_FUNC_TOTAL; ++I ) {
					AddEntry( _pcbCombo, KernelName( LSN_FILTER_FUNC( I ) ), LPARAM( I ) );
				}
				break;
			}
			case LSN_VF_VERTICAL_COMB : {
				AddEntry( _pcbCombo, LSN_LSTR( LSN_STR_VO_NONE ), CLSpiroPalFilterBase::LSN_CF_NONE );
				AddEntry( _pcbCombo, LSN_LSTR( LSN_STR_VO_CF_DELAY_LINE ), CLSpiroPalFilterBase::LSN_CF_DELAY_LINE );
				AddEntry( _pcbCombo, LSN_LSTR( LSN_STR_VO_CF_3_LINE ), CLSpiroPalFilterBase::LSN_CF_3_LINE );
				break;
			}
			case LSN_VF_NTSC_COMB : {
				AddEntry( _pcbCombo, LSN_LSTR( LSN_STR_VO_NONE ), CLSpiroNtscFilterBase::LSN_CF_NONE );
				AddEntry( _pcbCombo, LSN_LSTR( LSN_STR_VO_NCF_2_LINE ), CLSpiroNtscFilterBase::LSN_CF_2_LINE );
				AddEntry( _pcbCombo, LSN_LSTR( LSN_STR_VO_NCF_3_LINE ), CLSpiroNtscFilterBase::LSN_CF_3_LINE );
				AddEntry( _pcbCombo, LSN_LSTR( LSN_STR_VO_NCF_ADAPTIVE ), CLSpiroNtscFilterBase::LSN_CF_ADAPTIVE_3_LINE );
				break;
			}
			case LSN_VF_DEEP_PRESET : {
				AddEntry( _pcbCombo, LSN_LSTR( LSN_STR_VO_CUSTOM ), -1 );
				size_t sTotal = 0;
				const LSN_LSPIRO_DEEP_PRESET * pldpPresets = CLSpiroPresets::DeepPresets( sTotal );
				const bool bPal = IsLSpiroPal( _ftTarget );
				for ( size_t I = 0; I < sTotal; ++I ) {
					if ( pldpPresets[I].bPal == bPal ) { AddEntry( _pcbCombo, pldpPresets[I].pwcName, LPARAM( I ) ); }
				}
				break;
			}
			case LSN_VF_NOISE_TYPE : {
				AddEntry( _pcbCombo, LSN_LSTR( LSN_STR_VO_NONE ), LSN_NT_NONE );
				AddEntry( _pcbCombo, LSN_LSTR( LSN_STR_VO_NT_CUSTOM ), LSN_NT_CUSTOM );
				AddEntry( _pcbCombo, LSN_LSTR( LSN_STR_VO_NT_PHYSICAL ), LSN_NT_PHYSICAL );
				break;
			}
			default : {}
		}
		_pcbCombo->AutoSetMinListWidth();
	}

	/**
	 * Gets the value of a field.  Combo boxes use the item data of their entries, check boxes 0 or 1, and the rest the value shown.
	 *
	 * \param _vfField The field.
	 * \param _ftTarget The settings being edited.
	 * \param _dValue Holds the returned value.
	 * \return Returns true if the field has a value on the given kind of page.
	 */
	bool CVideoOptionsFields::GetValue( LSN_VIDEO_FIELD _vfField, const LSN_FIELD_TARGET &_ftTarget, double &_dValue ) {
		constexpr double dRadToDeg = 180.0 / std::numbers::pi;
		const LSN_VIDEO_PAGE_KIND vpkKind = _ftTarget.vpkKind;
		// Host machine.
		if ( _ftTarget.phdoHost ) {
			const CColorSpace::LSN_DISPLAY_SETTINGS & dsDisplay = _ftTarget.phdoHost->dsDisplay;
			switch ( _vfField ) {
				case LSN_VF_MONITOR_COLOR_SPACE : { _dValue = dsDisplay.cspColorSpace; return true; }
				case LSN_VF_MONITOR_GAMMA : { _dValue = dsDisplay.gGamma; return true; }
				case LSN_VF_WHITE_ADAPTATION : { _dValue = dsDisplay.waWhiteAdaptation; return true; }
				case LSN_VF_GAMUT : { _dValue = dsDisplay.gmGamutMapping; return true; }
				case LSN_VF_OUTPUT_RANGE : { _dValue = dsDisplay.orOutputRange; return true; }
				default : {}
			}
		}
		if ( !_ftTarget.pfroFilters ) { return false; }
		LSN_FILTER_REGION_OPTIONS & froOptions = (*_ftTarget.pfroFilters);
		const LSN_GPU_FILTER_OPTIONS * pgfoGpu = GpuOptions( _ftTarget );

		// Common.
		if ( pgfoGpu ) {
			switch ( _vfField ) {
				case LSN_VF_TV_PRESET : {
					_dValue = CColorSpace::Data( pgfoGpu->cspColorSpace ).bTvPreset ? double( pgfoGpu->cspColorSpace ) : -1.0;
					return true;
				}
				case LSN_VF_COLOR_SPACE : { _dValue = pgfoGpu->cspColorSpace; return true; }
				case LSN_VF_GAMMA : {
					// The palette applies its CRT curve on the CPU (see LSN_PALETTE_OPTIONS::gCrtGamma).
					if ( vpkKind == LSN_VPK_PALETTE ) {
						if ( !_ftTarget.ppoPalette ) { return false; }
						_dValue = _ftTarget.ppoPalette->gCrtGamma[_ftTarget.pmRegion];
						return true;
					}
					_dValue = pgfoGpu->gGamma;
					return true;
				}
				case LSN_VF_CRT_WHITE : { _dValue = pgfoGpu->fCrt1White; return true; }
				case LSN_VF_CRT_BRIGHTNESS : { _dValue = pgfoGpu->fCrt1Brightness; return true; }
				case LSN_VF_PHOSPHOR_ENABLED : { _dValue = pgfoGpu->bPhosphorDecay ? 1.0 : 0.0; return true; }
				case LSN_VF_PHOSPHOR_LEVEL : { _dValue = pgfoGpu->fPhosphorDecayLevel; return true; }
				case LSN_VF_PHOSPHOR_TIME : { _dValue = pgfoGpu->fPhosphorDecayPeriod; return true; }
				case LSN_VF_PHOSPHOR_RED : { _dValue = pgfoGpu->fPhosphorRedMultiplier * 100.0; return true; }
				case LSN_VF_PHOSPHOR_BLUE : { _dValue = pgfoGpu->fPhosphorBlueMultiplier * 100.0; return true; }
				case LSN_VF_SCALE_X : { _dValue = pgfoGpu->ui32HorSharpness; return true; }
				case LSN_VF_SCALE_Y : { _dValue = pgfoGpu->ui32VertSharpness; return true; }
				case LSN_VF_SCALE_DYNAMIC : { _dValue = pgfoGpu->bDynamicScaling ? 1.0 : 0.0; return true; }
				case LSN_VF_CONV_ENABLED : { _dValue = pgfoGpu->bHighQualityResampler ? 1.0 : 0.0; return true; }
				case LSN_VF_CONV_LOW : { _dValue = double( pgfoGpu->ffConvolution[CGpuFilterBase::LSN_CB_LOW] ); return true; }
				case LSN_VF_CONV_MEDIUM : { _dValue = double( pgfoGpu->ffConvolution[CGpuFilterBase::LSN_CB_MEDIUM] ); return true; }
				case LSN_VF_CONV_HIGH : { _dValue = double( pgfoGpu->ffConvolution[CGpuFilterBase::LSN_CB_HIGH] ); return true; }
				case LSN_VF_CONV_MAX : { _dValue = double( pgfoGpu->ffConvolution[CGpuFilterBase::LSN_CB_MAX] ); return true; }
				default : {}
			}
		}

		switch ( vpkKind ) {
			case LSN_VPK_PALETTE : {
				switch ( _vfField ) {
					case LSN_VF_PALETTE_LOAD : { _dValue = double( PalettePresetOf( _ftTarget ) ); return true; }
					case LSN_VF_PALETTE_SHOW_INDICES : { _dValue = s_bShowColorIndices ? 1.0 : 0.0; return true; }
					default : {}
				}
				break;
			}
			case LSN_VPK_BLARGG : {
				const LSN_BLARGG_OPTIONS & boBlargg = froOptions.boBlarggNtsc;
				switch ( _vfField ) {
					case LSN_VF_HUE : { _dValue = boBlargg.dHue * 180.0; return true; }
					case LSN_VF_BRIGHTNESS : { _dValue = boBlargg.dBrightness; return true; }
					case LSN_VF_CONTRAST : { _dValue = boBlargg.dContrast; return true; }
					case LSN_VF_SATURATION : { _dValue = boBlargg.dSaturation; return true; }
					case LSN_VF_BLARGG_GAMMA : { _dValue = boBlargg.dGamma; return true; }
					case LSN_VF_BLARGG_SHARPNESS : { _dValue = boBlargg.dSharpness; return true; }
					case LSN_VF_BLARGG_FRINGING : { _dValue = boBlargg.dFringing; return true; }
					case LSN_VF_BLARGG_BLEED : { _dValue = boBlargg.dBleed; return true; }
					case LSN_VF_BLARGG_ARTIFACTS : { _dValue = boBlargg.dArtifacts; return true; }
					case LSN_VF_BLARGG_MERGE_FIELDS : { _dValue = boBlargg.bMergeFields ? 1.0 : 0.0; return true; }
					default : {}
				}
				break;
			}
			case LSN_VPK_LSPIRO_NTSC : {}			LSN_FALLTHROUGH
			case LSN_VPK_LSPIRO_PAL : {
				const LSN_LSPIRO_OPTIONS & lsoLSpiro = ShownLSpiro( froOptions );
				switch ( _vfField ) {
					case LSN_VF_HUE : { _dValue = lsoLSpiro.fHue * dRadToDeg; return true; }
					case LSN_VF_BRIGHTNESS : { _dValue = lsoLSpiro.fBrightness; return true; }
					case LSN_VF_CONTRAST : { _dValue = lsoLSpiro.fContrast; return true; }
					case LSN_VF_SATURATION : { _dValue = lsoLSpiro.fSaturation; return true; }
					case LSN_VF_WHITE_LEVEL : { _dValue = lsoLSpiro.fWhiteLevel; return true; }
					case LSN_VF_BLACK_LEVEL : { _dValue = lsoLSpiro.fBlackLevel; return true; }
					case LSN_VF_LSPIRO_BLEED : { _dValue = lsoLSpiro.fBleed; return true; }
					case LSN_VF_CHROMA_FILTER : { _dValue = lsoLSpiro.ffFilterChroma; return true; }
					case LSN_VF_CHROMA_PARM0 : { _dValue = lsoLSpiro.fFilterChromaParms[0]; return true; }
					case LSN_VF_CHROMA_PARM1 : { _dValue = lsoLSpiro.fFilterChromaParms[1]; return true; }
					case LSN_VF_CHROMA_KERNEL : { _dValue = lsoLSpiro.ui32KernelSizeChroma; return true; }
					case LSN_VF_LUMA_FILTER : { _dValue = lsoLSpiro.ffFilterY; return true; }
					case LSN_VF_LUMA_PARM0 : { _dValue = lsoLSpiro.fFilterYParms[0]; return true; }
					case LSN_VF_LUMA_PARM1 : { _dValue = lsoLSpiro.fFilterYParms[1]; return true; }
					case LSN_VF_LUMA_KERNEL : { _dValue = lsoLSpiro.ui32KernelSizeY; return true; }
					case LSN_VF_VERTICAL_COMB : { _dValue = lsoLSpiro.cfCombFilter; return true; }
					case LSN_VF_NTSC_COMB : { _dValue = lsoLSpiro.cfNtscCombFilter; return true; }
					case LSN_VF_DEEP_PRESET : {
						// The first preset that matches, or Custom.
						_dValue = -1.0;
						size_t sTotal = 0;
						const LSN_LSPIRO_DEEP_PRESET * pldpPresets = CLSpiroPresets::DeepPresets( sTotal );
						const bool bPal = vpkKind == LSN_VPK_LSPIRO_PAL;
						for ( size_t I = 0; I < sTotal; ++I ) {
							if ( pldpPresets[I].bPal == bPal && CLSpiroPresets::Matches( pldpPresets[I], lsoLSpiro ) ) {
								_dValue = double( I );
								break;
							}
						}
						return true;
					}
					case LSN_VF_NOISE_TYPE : { _dValue = lsoLSpiro.ntNoiseType; return true; }
					case LSN_VF_NOISE_AMOUNT : { _dValue = lsoLSpiro.fNoiseAmount; return true; }
					case LSN_VF_DEMODULATOR : { _dValue = lsoLSpiro.dmDemodulator; return true; }
					// The software phosphor decay exists only in the software settings.
					case LSN_VF_SW_PHOSPHOR_LEVEL : { _dValue = froOptions.lsoLSpiroCpu.fPhosphorDecayLevel; return true; }
					case LSN_VF_SW_PHOSPHOR_TIME : { _dValue = froOptions.lsoLSpiroCpu.fPhosphorDecayPeriod; return true; }
					default : {}
				}
				break;
			}
			case LSN_VPK_LMP_NTSC : {}				LSN_FALLTHROUGH
			case LSN_VPK_LMP_PAL : {
				const LSN_LMP88959_SETTINGS & lsLmp = ShownLmp( froOptions, vpkKind == LSN_VPK_LMP_PAL );
				switch ( _vfField ) {
					case LSN_VF_HUE : { _dValue = lsLmp.i32Hue; return true; }
					case LSN_VF_BRIGHTNESS : { _dValue = lsLmp.i32Brightness; return true; }
					case LSN_VF_CONTRAST : { _dValue = lsLmp.i32Contrast; return true; }
					case LSN_VF_SATURATION : { _dValue = lsLmp.i32Saturation; return true; }
					case LSN_VF_WHITE_LEVEL : { _dValue = lsLmp.i32WhitePoint; return true; }
					case LSN_VF_BLACK_LEVEL : { _dValue = lsLmp.i32BlackPoint; return true; }
					case LSN_VF_LMP_NOISE : { _dValue = lsLmp.i32Noise; return true; }
					case LSN_VF_LMP_BLEND : { _dValue = lsLmp.bBlend ? 1.0 : 0.0; return true; }
					case LSN_VF_LMP_VSYNC : { _dValue = lsLmp.bVSync ? 1.0 : 0.0; return true; }
					case LSN_VF_LMP_HSYNC : { _dValue = lsLmp.bHSync ? 1.0 : 0.0; return true; }
					case LSN_VF_LMP_CHROMA_CORRECTION : { _dValue = lsLmp.bChromaCorrection ? 1.0 : 0.0; return true; }
					default : {}
				}
				break;
			}
			default : {}
		}
		return false;
	}

	/**
	 * Sets the value of a field.  Settings that exist in both a software and a GPU version are both set.
	 *
	 * \param _vfField The field.
	 * \param _ftTarget The settings being edited.
	 * \param _dValue The value to set.
	 * \return Returns true if the value was set.
	 */
	bool CVideoOptionsFields::SetValue( LSN_VIDEO_FIELD _vfField, LSN_FIELD_TARGET &_ftTarget, double _dValue ) {
		constexpr double dDegToRad = std::numbers::pi / 180.0;
		if ( !std::isfinite( _dValue ) ) { return false; }
		{
			double dMin, dMax;
			if ( Range( _vfField, dMin, dMax ) ) { _dValue = std::clamp( _dValue, dMin, dMax ); }
		}
		const LSN_VIDEO_PAGE_KIND vpkKind = _ftTarget.vpkKind;
		const float fValue = float( _dValue );
		const int32_t i32Value = int32_t( std::clamp( std::round( _dValue ), -2147483648.0, 2147483647.0 ) );
		const bool bValue = _dValue != 0.0;
		const uint32_t ui32Enum = uint32_t( std::max( 0.0, _dValue ) );

		// Host machine.
		if ( _ftTarget.phdoHost ) {
			CColorSpace::LSN_DISPLAY_SETTINGS & dsDisplay = _ftTarget.phdoHost->dsDisplay;
			switch ( _vfField ) {
				case LSN_VF_MONITOR_COLOR_SPACE : {
					if ( ui32Enum >= CColorSpace::LSN_CSP_TOTAL ) { return false; }
					dsDisplay.cspColorSpace = CColorSpace::LSN_COLOR_SPACE( ui32Enum );
					return true;
				}
				case LSN_VF_MONITOR_GAMMA : {
					if ( !CColorSpace::IsMonitorGamma( CNesPalette::LSN_GAMMA( ui32Enum ) ) ) { return false; }
					dsDisplay.gGamma = CNesPalette::LSN_GAMMA( ui32Enum );
					return true;
				}
				case LSN_VF_WHITE_ADAPTATION : {
					if ( ui32Enum >= CColorSpace::LSN_WA_TOTAL ) { return false; }
					dsDisplay.waWhiteAdaptation = CColorSpace::LSN_WHITE_ADAPTATION( ui32Enum );
					return true;
				}
				case LSN_VF_GAMUT : {
					if ( ui32Enum >= CColorSpace::LSN_GM_TOTAL ) { return false; }
					dsDisplay.gmGamutMapping = CColorSpace::LSN_GAMUT_MAPPING( ui32Enum );
					return true;
				}
				case LSN_VF_OUTPUT_RANGE : {
					if ( ui32Enum >= CColorSpace::LSN_OR_TOTAL ) { return false; }
					dsDisplay.orOutputRange = CColorSpace::LSN_OUTPUT_RANGE( ui32Enum );
					return true;
				}
				default : {}
			}
		}
		if ( !_ftTarget.pfroFilters ) { return false; }
		LSN_FILTER_REGION_OPTIONS & froOptions = (*_ftTarget.pfroFilters);
		LSN_GPU_FILTER_OPTIONS * pgfoGpu = GpuOptions( _ftTarget );

		// Common.
		if ( pgfoGpu ) {
			switch ( _vfField ) {
				case LSN_VF_TV_PRESET : {
					if ( _dValue < 0.0 || ui32Enum >= CColorSpace::LSN_CSP_TOTAL ) { return false; }
					ApplyTvPreset( CColorSpace::LSN_COLOR_SPACE( ui32Enum ), _ftTarget );
					return true;
				}
				case LSN_VF_COLOR_SPACE : {
					if ( ui32Enum >= CColorSpace::LSN_CSP_TOTAL ) { return false; }
					pgfoGpu->cspColorSpace = CColorSpace::LSN_COLOR_SPACE( ui32Enum );
					return true;
				}
				case LSN_VF_GAMMA : {
					if ( ui32Enum >= CNesPalette::LSN_G_TOTAL ) { return false; }
					if ( vpkKind == LSN_VPK_PALETTE ) {
						if ( !_ftTarget.ppoPalette ) { return false; }
						_ftTarget.ppoPalette->gCrtGamma[_ftTarget.pmRegion] = CNesPalette::LSN_GAMMA( ui32Enum );
						return true;
					}
					pgfoGpu->gGamma = CNesPalette::LSN_GAMMA( ui32Enum );
					return true;
				}
				case LSN_VF_CRT_WHITE : { pgfoGpu->fCrt1White = fValue; return true; }
				case LSN_VF_CRT_BRIGHTNESS : { pgfoGpu->fCrt1Brightness = fValue; return true; }
				case LSN_VF_PHOSPHOR_ENABLED : { pgfoGpu->bPhosphorDecay = bValue; return true; }
				case LSN_VF_PHOSPHOR_LEVEL : { pgfoGpu->fPhosphorDecayLevel = fValue; return true; }
				case LSN_VF_PHOSPHOR_TIME : { pgfoGpu->fPhosphorDecayPeriod = fValue; return true; }
				case LSN_VF_PHOSPHOR_RED : { pgfoGpu->fPhosphorRedMultiplier = fValue / 100.0f; return true; }
				case LSN_VF_PHOSPHOR_BLUE : { pgfoGpu->fPhosphorBlueMultiplier = fValue / 100.0f; return true; }
				case LSN_VF_SCALE_X : { pgfoGpu->ui32HorSharpness = uint32_t( std::clamp( i32Value, 1, 32 ) ); return true; }
				case LSN_VF_SCALE_Y : { pgfoGpu->ui32VertSharpness = uint32_t( std::clamp( i32Value, 1, 32 ) ); return true; }
				case LSN_VF_SCALE_DYNAMIC : { pgfoGpu->bDynamicScaling = bValue; return true; }
				case LSN_VF_CONV_ENABLED : { pgfoGpu->bHighQualityResampler = bValue; return true; }
				case LSN_VF_CONV_LOW : {}			LSN_FALLTHROUGH
				case LSN_VF_CONV_MEDIUM : {}		LSN_FALLTHROUGH
				case LSN_VF_CONV_HIGH : {}			LSN_FALLTHROUGH
				case LSN_VF_CONV_MAX : {
					if ( ui32Enum > CResamplerBase::LSN_FF_TOTAL ) { return false; }
					pgfoGpu->ffConvolution[CGpuFilterBase::LSN_CB_LOW + (_vfField - LSN_VF_CONV_LOW)] = CResamplerBase::LSN_FILTER_FUNCS( ui32Enum );
					return true;
				}
				default : {}
			}
		}

		switch ( vpkKind ) {
			case LSN_VPK_PALETTE : {
				switch ( _vfField ) {
					case LSN_VF_PALETTE_LOAD : {
						if ( _dValue < 0.0 ) { return false; }
						ApplyPalettePreset( size_t( ui32Enum ), _ftTarget );
						return true;
					}
					case LSN_VF_PALETTE_SHOW_INDICES : { s_bShowColorIndices = bValue; return true; }
					default : {}
				}
				break;
			}
			case LSN_VPK_BLARGG : {
				LSN_BLARGG_OPTIONS & boBlargg = froOptions.boBlarggNtsc;
				switch ( _vfField ) {
					case LSN_VF_HUE : { boBlargg.dHue = _dValue / 180.0; return true; }
					case LSN_VF_BRIGHTNESS : { boBlargg.dBrightness = _dValue; return true; }
					case LSN_VF_CONTRAST : { boBlargg.dContrast = _dValue; return true; }
					case LSN_VF_SATURATION : { boBlargg.dSaturation = _dValue; return true; }
					case LSN_VF_BLARGG_GAMMA : { boBlargg.dGamma = _dValue; return true; }
					case LSN_VF_BLARGG_SHARPNESS : { boBlargg.dSharpness = _dValue; return true; }
					case LSN_VF_BLARGG_FRINGING : { boBlargg.dFringing = _dValue; return true; }
					case LSN_VF_BLARGG_BLEED : { boBlargg.dBleed = _dValue; return true; }
					case LSN_VF_BLARGG_ARTIFACTS : { boBlargg.dArtifacts = _dValue; return true; }
					case LSN_VF_BLARGG_MERGE_FIELDS : { boBlargg.bMergeFields = bValue; return true; }
					default : {}
				}
				break;
			}
			case LSN_VPK_LSPIRO_NTSC : {}			LSN_FALLTHROUGH
			case LSN_VPK_LSPIRO_PAL : {
				const bool bPal = vpkKind == LSN_VPK_LSPIRO_PAL;
				// Both the software and the GPU settings are set.
				for ( LSN_LSPIRO_OPTIONS * plsoThis : { &froOptions.lsoLSpiroCpu, &froOptions.lsoLSpiroGpu } ) {
					LSN_LSPIRO_OPTIONS & lsoLSpiro = (*plsoThis);
					const double dSignalHz = CLSpiroPresets::SignalHz( bPal, lsoLSpiro.ui16PixelToSignal );
					switch ( _vfField ) {
						case LSN_VF_HUE : { lsoLSpiro.fHue = float( _dValue * dDegToRad ); break; }
						case LSN_VF_BRIGHTNESS : { lsoLSpiro.fBrightness = fValue; break; }
						case LSN_VF_CONTRAST : { lsoLSpiro.fContrast = fValue; break; }
						case LSN_VF_SATURATION : { lsoLSpiro.fSaturation = fValue; break; }
						case LSN_VF_WHITE_LEVEL : { lsoLSpiro.fWhiteLevel = fValue; break; }
						case LSN_VF_BLACK_LEVEL : { lsoLSpiro.fBlackLevel = fValue; break; }
						case LSN_VF_LSPIRO_BLEED : { lsoLSpiro.fBleed = fValue; break; }
						case LSN_VF_CHROMA_FILTER : {
							if ( ui32Enum >= LSN_FF_FUNC_TOTAL ) { return false; }
							if ( lsoLSpiro.ffFilterChroma != LSN_FILTER_FUNC( ui32Enum ) ) {
								// The parameters mean different things to each function.
								lsoLSpiro.ffFilterChroma = LSN_FILTER_FUNC( ui32Enum );
								CLSpiroPresets::DefaultKernelParms( lsoLSpiro.ffFilterChroma, false, dSignalHz, lsoLSpiro.fFilterChromaParms );
							}
							break;
						}
						case LSN_VF_CHROMA_PARM0 : { lsoLSpiro.fFilterChromaParms[0] = fValue; break; }
						case LSN_VF_CHROMA_PARM1 : { lsoLSpiro.fFilterChromaParms[1] = fValue; break; }
						case LSN_VF_CHROMA_KERNEL : { lsoLSpiro.ui32KernelSizeChroma = uint32_t( std::clamp( i32Value, 1, 128 ) ); break; }
						case LSN_VF_LUMA_FILTER : {
							if ( ui32Enum >= LSN_FF_FUNC_TOTAL ) { return false; }
							if ( lsoLSpiro.ffFilterY != LSN_FILTER_FUNC( ui32Enum ) ) {
								// The parameters mean different things to each function.
								lsoLSpiro.ffFilterY = LSN_FILTER_FUNC( ui32Enum );
								CLSpiroPresets::DefaultKernelParms( lsoLSpiro.ffFilterY, true, dSignalHz, lsoLSpiro.fFilterYParms );
							}
							break;
						}
						case LSN_VF_LUMA_PARM0 : { lsoLSpiro.fFilterYParms[0] = fValue; break; }
						case LSN_VF_LUMA_PARM1 : { lsoLSpiro.fFilterYParms[1] = fValue; break; }
						case LSN_VF_LUMA_KERNEL : { lsoLSpiro.ui32KernelSizeY = uint32_t( std::clamp( i32Value, 1, 128 ) ); break; }
						case LSN_VF_VERTICAL_COMB : {
							if ( ui32Enum >= CLSpiroPalFilterBase::LSN_CF_TOTAL ) { return false; }
							lsoLSpiro.cfCombFilter = CLSpiroPalFilterBase::LSN_COMB_FILTER( ui32Enum );
							break;
						}
						case LSN_VF_NTSC_COMB : {
							if ( ui32Enum >= CLSpiroNtscFilterBase::LSN_CF_TOTAL ) { return false; }
							lsoLSpiro.cfNtscCombFilter = CLSpiroNtscFilterBase::LSN_COMB_FILTER( ui32Enum );
							break;
						}
						case LSN_VF_DEEP_PRESET : {
							// Custom changes nothing.
							size_t sTotal = 0;
							const LSN_LSPIRO_DEEP_PRESET * pldpPresets = CLSpiroPresets::DeepPresets( sTotal );
							if ( _dValue < 0.0 || ui32Enum >= sTotal || pldpPresets[ui32Enum].bPal != bPal ) { return true; }
							CLSpiroPresets::Apply( pldpPresets[ui32Enum], lsoLSpiro );
							break;
						}
						case LSN_VF_NOISE_TYPE : {
							if ( ui32Enum >= LSN_NT_TOTAL ) { return false; }
							lsoLSpiro.ntNoiseType = LSN_NOISE_TYPE( ui32Enum );
							break;
						}
						case LSN_VF_NOISE_AMOUNT : { lsoLSpiro.fNoiseAmount = std::clamp( fValue, 0.0f, LSN_VIDEO_LIMITS::fMaxNoiseAmount ); break; }
						case LSN_VF_DEMODULATOR : {
							if ( ui32Enum >= CColorSpace::LSN_DM_TOTAL ) { return false; }
							lsoLSpiro.dmDemodulator = CColorSpace::LSN_DEMODULATOR( ui32Enum );
							break;
						}
						case LSN_VF_SW_PHOSPHOR_LEVEL : { lsoLSpiro.fPhosphorDecayLevel = fValue; break; }
						case LSN_VF_SW_PHOSPHOR_TIME : { lsoLSpiro.fPhosphorDecayPeriod = fValue; break; }
						default : { return false; }
					}
				}
				return true;
			}
			case LSN_VPK_LMP_NTSC : {}				LSN_FALLTHROUGH
			case LSN_VPK_LMP_PAL : {
				const bool bPal = vpkKind == LSN_VPK_LMP_PAL;
				// Both the software and the GPU settings are set.
				for ( LSN_LMP88959_SETTINGS * plsThis : { bPal ? &froOptions.lsLmpPalCpu : &froOptions.lsLmpNtscCpu, bPal ? &froOptions.lsLmpPalGpu : &froOptions.lsLmpNtscGpu } ) {
					LSN_LMP88959_SETTINGS & lsLmp = (*plsThis);
					switch ( _vfField ) {
						case LSN_VF_HUE : { lsLmp.i32Hue = i32Value; break; }
						case LSN_VF_BRIGHTNESS : { lsLmp.i32Brightness = i32Value; break; }
						case LSN_VF_CONTRAST : { lsLmp.i32Contrast = i32Value; break; }
						case LSN_VF_SATURATION : { lsLmp.i32Saturation = i32Value; break; }
						case LSN_VF_WHITE_LEVEL : { lsLmp.i32WhitePoint = i32Value; break; }
						case LSN_VF_BLACK_LEVEL : { lsLmp.i32BlackPoint = i32Value; break; }
						case LSN_VF_LMP_NOISE : { lsLmp.i32Noise = std::max( i32Value, 0 ); break; }
						case LSN_VF_LMP_BLEND : { lsLmp.bBlend = bValue; break; }
						case LSN_VF_LMP_VSYNC : { lsLmp.bVSync = bValue; break; }
						case LSN_VF_LMP_HSYNC : { lsLmp.bHSync = bValue; break; }
						case LSN_VF_LMP_CHROMA_CORRECTION : { lsLmp.bChromaCorrection = bValue; break; }
						default : { return false; }
					}
				}
				return true;
			}
			default : {}
		}
		return false;
	}

	/**
	 * Determines whether a field can currently be edited (for example, the CRT1 parameters only when the CRT1 curve is selected).
	 *
	 * \param _vfField The field.
	 * \param _ftTarget The settings being edited.
	 * \return Returns true if the field can currently be edited.
	 */
	bool CVideoOptionsFields::IsEditable( LSN_VIDEO_FIELD _vfField, const LSN_FIELD_TARGET &_ftTarget ) {
		double dValue = 0.0;
		switch ( _vfField ) {
			case LSN_VF_CRT_WHITE : {}				LSN_FALLTHROUGH
			case LSN_VF_CRT_BRIGHTNESS : {
				// Automatic can resolve to the CRT1 curve (except for palettes, where it means the curve the palette file asks for).
				if ( !GetValue( LSN_VF_GAMMA, _ftTarget, dValue ) ) { return false; }
				if ( CNesPalette::LSN_GAMMA( dValue ) == CNesPalette::LSN_G_CRT1 ) { return true; }
				return CNesPalette::LSN_GAMMA( dValue ) == CNesPalette::LSN_G_AUTO && _ftTarget.vpkKind != LSN_VPK_PALETTE;
			}
			case LSN_VF_PHOSPHOR_LEVEL : {}			LSN_FALLTHROUGH
			case LSN_VF_PHOSPHOR_TIME : {}			LSN_FALLTHROUGH
			case LSN_VF_PHOSPHOR_RED : {}			LSN_FALLTHROUGH
			case LSN_VF_PHOSPHOR_BLUE : {
				return GetValue( LSN_VF_PHOSPHOR_ENABLED, _ftTarget, dValue ) && dValue != 0.0;
			}
			case LSN_VF_CONV_LOW : {}				LSN_FALLTHROUGH
			case LSN_VF_CONV_MEDIUM : {}			LSN_FALLTHROUGH
			case LSN_VF_CONV_HIGH : {}				LSN_FALLTHROUGH
			case LSN_VF_CONV_MAX : {
				return GetValue( LSN_VF_CONV_ENABLED, _ftTarget, dValue ) && dValue != 0.0;
			}
			case LSN_VF_CHROMA_PARM0 : {
				return GetValue( LSN_VF_CHROMA_FILTER, _ftTarget, dValue ) && CLSpiroPresets::KernelParmCount( LSN_FILTER_FUNC( dValue ) ) >= 1;
			}
			case LSN_VF_CHROMA_PARM1 : {
				return GetValue( LSN_VF_CHROMA_FILTER, _ftTarget, dValue ) && CLSpiroPresets::KernelParmCount( LSN_FILTER_FUNC( dValue ) ) >= 2;
			}
			case LSN_VF_LUMA_PARM0 : {
				return GetValue( LSN_VF_LUMA_FILTER, _ftTarget, dValue ) && CLSpiroPresets::KernelParmCount( LSN_FILTER_FUNC( dValue ) ) >= 1;
			}
			case LSN_VF_LUMA_PARM1 : {
				return GetValue( LSN_VF_LUMA_FILTER, _ftTarget, dValue ) && CLSpiroPresets::KernelParmCount( LSN_FILTER_FUNC( dValue ) ) >= 2;
			}
			case LSN_VF_NOISE_AMOUNT : {
				return GetValue( LSN_VF_NOISE_TYPE, _ftTarget, dValue ) && LSN_NOISE_TYPE( dValue ) == LSN_NT_CUSTOM;
			}
			case LSN_VF_PALETTE_SHOW_INDICES : {
				// The palette display does not exist yet.
				return false;
			}
			default : { return true; }
		}
	}

	/**
	 * Applies a TV preset:  sets the color space, the saturation (relative to the default saturation of the filter), and (L. Spiro
	 *	filters) the decoder chip.
	 *
	 * \param _cspPreset The color space that models the television.
	 * \param _ftTarget The settings being edited.
	 */
	void CVideoOptionsFields::ApplyTvPreset( CColorSpace::LSN_COLOR_SPACE _cspPreset, LSN_FIELD_TARGET &_ftTarget ) {
		LSN_GPU_FILTER_OPTIONS * pgfoGpu = GpuOptions( _ftTarget );
		if ( !pgfoGpu || !_ftTarget.pfroFilters || _cspPreset >= CColorSpace::LSN_CSP_TOTAL ) { return; }
		const CColorSpace::LSN_COLOR_SPACE_DATA & csdData = CColorSpace::Data( _cspPreset );
		pgfoGpu->cspColorSpace = _cspPreset;

		const LSN_FILTER_REGION_OPTIONS froDefaults = LSN_FILTER_REGION_OPTIONS::Defaults( _ftTarget.pmRegion );
		LSN_FILTER_REGION_OPTIONS & froOptions = (*_ftTarget.pfroFilters);
		const double dSat = csdData.fSaturation;
		switch ( _ftTarget.vpkKind ) {
			case LSN_VPK_BLARGG : {
				// Blargg's saturation is an offset from 1.0 (0 = normal).
				froOptions.boBlarggNtsc.dSaturation = (1.0 + froDefaults.boBlarggNtsc.dSaturation) * dSat - 1.0;
				break;
			}
			case LSN_VPK_LSPIRO_NTSC : {}			LSN_FALLTHROUGH
			case LSN_VPK_LSPIRO_PAL : {
				froOptions.lsoLSpiroCpu.fSaturation = float( froDefaults.lsoLSpiroCpu.fSaturation * dSat );
				froOptions.lsoLSpiroGpu.fSaturation = float( froDefaults.lsoLSpiroGpu.fSaturation * dSat );
				// The decoder chip of the television.
				froOptions.lsoLSpiroCpu.dmDemodulator = csdData.dmDemodulator;
				froOptions.lsoLSpiroGpu.dmDemodulator = csdData.dmDemodulator;
				break;
			}
			case LSN_VPK_LMP_NTSC : {
				froOptions.lsLmpNtscCpu.i32Saturation = int32_t( std::round( froDefaults.lsLmpNtscCpu.i32Saturation * dSat ) );
				froOptions.lsLmpNtscGpu.i32Saturation = int32_t( std::round( froDefaults.lsLmpNtscGpu.i32Saturation * dSat ) );
				break;
			}
			case LSN_VPK_LMP_PAL : {
				froOptions.lsLmpPalCpu.i32Saturation = int32_t( std::round( froDefaults.lsLmpPalCpu.i32Saturation * dSat ) );
				froOptions.lsLmpPalGpu.i32Saturation = int32_t( std::round( froDefaults.lsLmpPalGpu.i32Saturation * dSat ) );
				break;
			}
			default : {}
		}
	}

	/**
	 * Applies a palette preset:  sets the palette path, the CRT gamma curve, the CRT1 parameters, and the color space.
	 *
	 * \param _sPreset The index of the palette preset.
	 * \param _ftTarget The settings being edited.
	 */
	void CVideoOptionsFields::ApplyPalettePreset( size_t _sPreset, LSN_FIELD_TARGET &_ftTarget ) {
		if ( _sPreset >= std::size( s_ppPalettes ) || !_ftTarget.ppoPalette || !_ftTarget.pfroFilters ) { return; }
		const LSN_PALETTE_PRESET & ppPreset = s_ppPalettes[_sPreset];
		_ftTarget.ppoPalette->wsPath = CBeesNes::DefaultPaletteFolder() + ppPreset.pwcFile;
		_ftTarget.ppoPalette->gCrtGamma[_ftTarget.pmRegion] = ppPreset.gCrtGamma;
		_ftTarget.pfroFilters->gfoPalette.fCrt1White = ppPreset.fCrt1White;
		_ftTarget.pfroFilters->gfoPalette.fCrt1Brightness = ppPreset.fCrt1Brightness;
		_ftTarget.pfroFilters->gfoPalette.cspColorSpace = (_ftTarget.pmRegion == LSN_PM_NTSC_J) ? ppPreset.cspNtscJColorSpace : ppPreset.cspColorSpace;
	}

	/**
	 * Applies the settings that go with a palette loaded from a file:  the sRGB gamma curve and the sRGB color space.
	 *
	 * \param _ftTarget The settings being edited.
	 */
	void CVideoOptionsFields::ApplyCustomPalette( LSN_FIELD_TARGET &_ftTarget ) {
		if ( !_ftTarget.ppoPalette || !_ftTarget.pfroFilters ) { return; }
		_ftTarget.ppoPalette->gCrtGamma[_ftTarget.pmRegion] = CNesPalette::LSN_G_sRGB;
		_ftTarget.pfroFilters->gfoPalette.cspColorSpace = CColorSpace::LSN_CSP_SRGB;
	}

	/**
	 * Restores the defaults of the settings shown on a page.
	 *
	 * \param _ftTarget The settings being edited.
	 */
	void CVideoOptionsFields::RestoreDefaults( LSN_FIELD_TARGET &_ftTarget ) {
		if ( _ftTarget.vpkKind == LSN_VPK_HOST ) {
			if ( _ftTarget.phdoHost ) { (*_ftTarget.phdoHost) = LSN_HOST_DISPLAY_OPTIONS(); }
			return;
		}
		if ( !_ftTarget.pfroFilters ) { return; }
		const LSN_FILTER_REGION_OPTIONS froDefaults = LSN_FILTER_REGION_OPTIONS::Defaults( _ftTarget.pmRegion );
		LSN_FILTER_REGION_OPTIONS & froOptions = (*_ftTarget.pfroFilters);
		switch ( _ftTarget.vpkKind ) {
			case LSN_VPK_PALETTE : {
				froOptions.gfoPalette = froDefaults.gfoPalette;
				if ( _ftTarget.ppoPalette ) {
					const bool bUseGlobal = _ftTarget.ppoPalette->bUseGlobal;
					(*_ftTarget.ppoPalette) = LSN_PALETTE_OPTIONS();
					_ftTarget.ppoPalette->bUseGlobal = bUseGlobal;
				}
				break;
			}
			case LSN_VPK_BLARGG : {
				froOptions.boBlarggNtsc = froDefaults.boBlarggNtsc;
				froOptions.gfoBlargg = froDefaults.gfoBlargg;
				break;
			}
			case LSN_VPK_LSPIRO_NTSC : {}			LSN_FALLTHROUGH
			case LSN_VPK_LSPIRO_PAL : {
				froOptions.lsoLSpiroCpu = froDefaults.lsoLSpiroCpu;
				froOptions.lsoLSpiroGpu = froDefaults.lsoLSpiroGpu;
				froOptions.gfoLSpiro = froDefaults.gfoLSpiro;
				break;
			}
			case LSN_VPK_LMP_NTSC : {
				froOptions.lsLmpNtscCpu = froDefaults.lsLmpNtscCpu;
				froOptions.lsLmpNtscGpu = froDefaults.lsLmpNtscGpu;
				froOptions.gfoLmpNtsc = froDefaults.gfoLmpNtsc;
				break;
			}
			case LSN_VPK_LMP_PAL : {
				froOptions.lsLmpPalCpu = froDefaults.lsLmpPalCpu;
				froOptions.lsLmpPalGpu = froDefaults.lsLmpPalGpu;
				froOptions.gfoLmpPal = froDefaults.gfoLmpPal;
				break;
			}
			default : {}
		}
	}

	/**
	 * Restores the defaults of the convolution settings shown on a page.
	 *
	 * \param _ftTarget The settings being edited.
	 */
	void CVideoOptionsFields::RestoreConvolutionDefaults( LSN_FIELD_TARGET &_ftTarget ) {
		LSN_GPU_FILTER_OPTIONS * pgfoGpu = GpuOptions( _ftTarget );
		if ( !pgfoGpu ) { return; }
		LSN_FILTER_REGION_OPTIONS froDefaults = LSN_FILTER_REGION_OPTIONS::Defaults( _ftTarget.pmRegion );
		LSN_FIELD_TARGET ftDefaults = _ftTarget;
		ftDefaults.pfroFilters = &froDefaults;
		const LSN_GPU_FILTER_OPTIONS * pgfoDefaults = GpuOptions( ftDefaults );
		if ( !pgfoDefaults ) { return; }
		pgfoGpu->bHighQualityResampler = pgfoDefaults->bHighQualityResampler;
		for ( size_t I = 0; I < std::size( pgfoGpu->ffConvolution ); ++I ) {
			pgfoGpu->ffConvolution[I] = pgfoDefaults->ffConvolution[I];
		}
	}

	/**
	 * Gets the range of the values of a field that would not break its filter.  Values outside of the range are clamped.
	 *
	 * \param _vfField The field.
	 * \param _dMin Holds the returned lowest value (in the units shown by the dialog).
	 * \param _dMax Holds the returned highest value (in the units shown by the dialog).
	 * \return Returns true if the field has a limited range.
	 */
	bool CVideoOptionsFields::Range( LSN_VIDEO_FIELD _vfField, double &_dMin, double &_dMax ) {
		switch ( _vfField ) {
			case LSN_VF_CRT_WHITE : {
				_dMin = LSN_VIDEO_LIMITS::fMinCrt1White;
				_dMax = LSN_VIDEO_LIMITS::fMaxCrt1White;
				return true;
			}
			case LSN_VF_CRT_BRIGHTNESS : {
				_dMin = LSN_VIDEO_LIMITS::fMinCrt1Brightness;
				_dMax = LSN_VIDEO_LIMITS::fMaxCrt1Brightness;
				return true;
			}
			case LSN_VF_PHOSPHOR_LEVEL : {}				LSN_FALLTHROUGH
			case LSN_VF_SW_PHOSPHOR_LEVEL : {
				_dMin = LSN_VIDEO_LIMITS::fMinPhosphorLevel;
				_dMax = LSN_VIDEO_LIMITS::fMaxPhosphorLevel;
				return true;
			}
			case LSN_VF_PHOSPHOR_TIME : {}				LSN_FALLTHROUGH
			case LSN_VF_SW_PHOSPHOR_TIME : {
				_dMin = LSN_VIDEO_LIMITS::fMinPhosphorTime;
				_dMax = LSN_VIDEO_LIMITS::fMaxPhosphorTime;
				return true;
			}
			case LSN_VF_PHOSPHOR_RED : {}				LSN_FALLTHROUGH
			case LSN_VF_PHOSPHOR_BLUE : {
				// Shown in percent.
				_dMin = LSN_VIDEO_LIMITS::fMinPhosphorMultiplier * 100.0;
				_dMax = LSN_VIDEO_LIMITS::fMaxPhosphorMultiplier * 100.0;
				return true;
			}
			case LSN_VF_LSPIRO_BLEED : {
				_dMin = 0.0;
				_dMax = LSN_VIDEO_LIMITS::fMaxBleed;
				return true;
			}
			case LSN_VF_NOISE_AMOUNT : {
				_dMin = 0.0;
				_dMax = LSN_VIDEO_LIMITS::fMaxNoiseAmount;
				return true;
			}
			default : { return false; }
		}
	}

	/**
	 * Gets the GPU settings shown on a page.
	 *
	 * \param _ftTarget The settings being edited.
	 * \return Returns the GPU settings that the common settings of the page edit, or nullptr for the Host Machine page.
	 */
	LSN_GPU_FILTER_OPTIONS * CVideoOptionsFields::GpuOptions( const LSN_FIELD_TARGET &_ftTarget ) {
		if ( !_ftTarget.pfroFilters ) { return nullptr; }
		LSN_FILTER_REGION_OPTIONS & froOptions = (*_ftTarget.pfroFilters);
		switch ( _ftTarget.vpkKind ) {
			case LSN_VPK_PALETTE : { return &froOptions.gfoPalette; }
			case LSN_VPK_BLARGG : { return &froOptions.gfoBlargg; }
			case LSN_VPK_LSPIRO_NTSC : {}			LSN_FALLTHROUGH
			case LSN_VPK_LSPIRO_PAL : { return &froOptions.gfoLSpiro; }
			case LSN_VPK_LMP_NTSC : { return &froOptions.gfoLmpNtsc; }
			case LSN_VPK_LMP_PAL : { return &froOptions.gfoLmpPal; }
			default : { return nullptr; }
		}
	}

	/**
	 * Formats a value for an edit control.
	 *
	 * \param _vfField The field.
	 * \param _dValue The value.
	 * \return Returns the text to show.
	 */
	std::wstring CVideoOptionsFields::FormatValue( LSN_VIDEO_FIELD _vfField, double _dValue ) {
		if ( Info( _vfField ).vftType == LSN_VFT_INT ) {
			return std::format( L"{}", int64_t( std::round( _dValue ) ) );
		}
		// The shortest text that reads back as the same single-precision value (most settings are stored as float).
		return std::format( L"{}", float( _dValue ) );
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The user-adjustable settings of the video filters (palette, Blargg, LMP88959, and L. Spiro), for each region.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../ColorSpace/LSNColorSpace.h"
#include "../Filters/LSNGpuFilterBase.h"
#include "../Filters/LSNLmp88959Settings.h"
#include "../Filters/LSNLSpiroNtscFilterBase.h"
#include "../Filters/LSNLSpiroPalFilterBase.h"
#include "../Filters/LSNNesPalette.h"
#include "../Filters/nes_ntsc/nes_ntsc.h"
#include "../Utilities/LSNUtilities.h"

#include <algorithm>
#include <numbers>


namespace lsn {

	/**
	 * Struct LSN_VIDEO_LIMITS
	 * \brief The limits of settings that break a filter outside of them.
	 *
	 * Description: The limits of settings that break a filter outside of them.  The phosphors decay from their starting level to 0.001
	 *	over the decay time, so a starting level at or below 0.001 never decays (or grows without bound) and a time of 0 divides by 0.
	 */
	struct LSN_VIDEO_LIMITS {
		static constexpr float								fMinPhosphorLevel = 0.002f;									/**< The lowest starting phosphor level. */
		static constexpr float								fMaxPhosphorLevel = 1.0f;									/**< The highest starting phosphor level. */
		static constexpr float								fMinPhosphorTime = 0.001f;									/**< The shortest phosphor decay time, in seconds. */
		static constexpr float								fMaxPhosphorTime = 60.0f;									/**< The longest phosphor decay time, in seconds. */
		static constexpr float								fMinPhosphorMultiplier = 0.01f;								/**< The lowest red/blue phosphor decay-time multiplier. */
		static constexpr float								fMaxPhosphorMultiplier = 10.0f;								/**< The highest red/blue phosphor decay-time multiplier. */
		static constexpr float								fMinCrt1White = 0.01f;										/**< The lowest white level of the CRT1 curve. */
		static constexpr float								fMaxCrt1White = 10.0f;										/**< The highest white level of the CRT1 curve. */
		static constexpr float								fMinCrt1Brightness = -0.3f;									/**< The lowest brightness of the CRT1 curve (it breaks at -0.35). */
		static constexpr float								fMaxCrt1Brightness = 1.0f;									/**< The highest brightness of the CRT1 curve. */
		static constexpr float								fMaxBleed = 0.99f;											/**< The highest L. Spiro bleed (it must stay below 1). */
		static constexpr float								fMaxNoiseAmount = 20.0f;									/**< The highest L. Spiro noise (standard deviation, in units of the black-to-white range; far past the point of pure snow). */


		// == Functions.
		/**
		 * Brings the phosphor settings into their valid ranges.
		 *
		 * \param _fLevel The starting level to clamp.
		 * \param _fTime The decay time to clamp.
		 */
		static inline void									ClampPhosphor( float &_fLevel, float &_fTime ) {
			_fLevel = std::clamp( _fLevel, fMinPhosphorLevel, fMaxPhosphorLevel );
			_fTime = std::clamp( _fTime, fMinPhosphorTime, fMaxPhosphorTime );
		}
	};

	/**
	 * Struct LSN_GPU_FILTER_OPTIONS
	 * \brief The settings shared by every GPU filter.
	 *
	 * Description: The settings shared by every GPU filter (see CGpuFilterBase).
	 */
	struct LSN_GPU_FILTER_OPTIONS {
		CColorSpace::LSN_COLOR_SPACE						cspColorSpace = CColorSpace::LSN_CSP_SRGB;					/**< The color space of the emulated CRT. */
		CNesPalette::LSN_GAMMA								gGamma = CNesPalette::LSN_G_AUTO;							/**< The gamma curve applied by the gamma pass. */
		float												fCrt1White = 1.0f;											/**< The white level (Lw) of the LSN_G_CRT1 curve. */
		float												fCrt1Brightness = 0.0181f * 8.0f;							/**< The brightness (black lift, B) of the LSN_G_CRT1 curve. */
		bool												bPhosphorDecay = true;										/**< If true, phosphor decay is emulated. */
		float												fPhosphorDecayLevel = CGpuFilterBase::DefaultPhosphorDecayIntensity();	/**< The initial phosphor decay level. */
		float												fPhosphorDecayPeriod = CGpuFilterBase::DefaultPhosphorDecayDuration();	/**< The time it takes the phosphors to decay to 0.001. */
		float												fPhosphorRedMultiplier = 0.45f;								/**< The red phosphor's decay time relative to the green phosphor's. */
		float												fPhosphorBlueMultiplier = 0.25f;							/**< The blue phosphor's decay time relative to the green phosphor's. */
		uint32_t											ui32VertSharpness = 3;										/**< How much the image is scaled vertically before the final resampling (internal scaling Y). */
		uint32_t											ui32HorSharpness = 1;										/**< How much the image is scaled horizontally before the final resampling (internal scaling X). */
		bool												bDynamicScaling = true;										/**< If true, the internal scaling follows the output size (capped at the factors above). */
		bool												bHighQualityResampler = true;								/**< If true, the final composite render uses convolution (the high-quality 2-pass resampler). */
		CResamplerBase::LSN_FILTER_FUNCS					ffConvolution[CGpuFilterBase::LSN_CB_TOTAL] = {				/**< The convolution filter of each band of output scales (LSN_FF_TOTAL = none). */
			CResamplerBase::LSN_FF_CARDINALSPLINEUNIFORM,
			CResamplerBase::LSN_FF_ROBIDOUXSHARP,
			CResamplerBase::LSN_FF_LINEAR,
			CResamplerBase::LSN_FF_TOTAL,
		};


		// == Functions.
		/**
		 * Sets the convolution filters of the 4 bands of output scales.
		 *
		 * \param _ffLow The filter below 2.5 times the native size.
		 * \param _ffMedium The filter below 3.5 times the native size.
		 * \param _ffHigh The filter below 4.5 times the native size.
		 * \param _ffMax The filter at 4.5 times the native size or above (LSN_FF_TOTAL = none).
		 */
		inline void											SetConvolution( CResamplerBase::LSN_FILTER_FUNCS _ffLow, CResamplerBase::LSN_FILTER_FUNCS _ffMedium,
			CResamplerBase::LSN_FILTER_FUNCS _ffHigh, CResamplerBase::LSN_FILTER_FUNCS _ffMax ) {
			ffConvolution[CGpuFilterBase::LSN_CB_LOW] = _ffLow;
			ffConvolution[CGpuFilterBase::LSN_CB_MEDIUM] = _ffMedium;
			ffConvolution[CGpuFilterBase::LSN_CB_HIGH] = _ffHigh;
			ffConvolution[CGpuFilterBase::LSN_CB_MAX] = _ffMax;
		}
	};


	/**
	 * Struct LSN_HOST_DISPLAY_OPTIONS
	 * \brief The settings of the host machine's display.
	 *
	 * Description: The settings of the host machine's display (the monitor's color space, gamma curve, white-point adaptation, gamut
	 *	mapping, and output range).  These are global only (not per-game).  The gamma curve also applies to the software palette; the rest
	 *	apply only to the GPU filters.
	 */
	struct LSN_HOST_DISPLAY_OPTIONS {
		CColorSpace::LSN_DISPLAY_SETTINGS					dsDisplay;													/**< The display settings. */
	};


	/**
	 * Struct LSN_BLARGG_OPTIONS
	 * \brief The settings of Blargg's NTSC filter.
	 *
	 * Description: The settings of Blargg's NTSC filter (see nes_ntsc_setup_t, whose units these use).
	 */
	struct LSN_BLARGG_OPTIONS {
		double												dHue = 7.89 / 180.0;										/**< -1 = -180 degrees, 1 = 180 degrees. */
		double												dSaturation = -0.250;										/**< -1 = grayscale (0.0), 1 = oversaturated colors (2.0). */
		double												dContrast = 0.0;											/**< -1 = dark (0.5), 1 = light (1.5). */
		double												dBrightness = -0.084;										/**< -1 = dark (0.5), 1 = light (1.5). */
		double												dSharpness = 0.78;											/**< Edge contrast enhancement/blurring. */
		double												dGamma = 0.0;												/**< -1 = dark (1.5), 1 = light (0.5). */
		double												dResolution = 0.0;											/**< Image resolution. */
		double												dArtifacts = 0.62;											/**< Artifacts caused by color changes. */
		double												dFringing = 0.05;											/**< Color artifacts caused by brightness changes. */
		double												dBleed = 0.0;												/**< Color bleed (color resolution reduction). */
		bool												bMergeFields = false;										/**< If true, even and odd fields are merged to reduce flicker. */


		// == Functions.
		/**
		 * Gets the defaults of the NTSC filter.
		 *
		 * \return Returns the defaults of the NTSC filter.
		 */
		static inline LSN_BLARGG_OPTIONS					NtscDefaults() { return LSN_BLARGG_OPTIONS(); }

		/**
		 * Converts the options to the structure used by Blargg's filter.
		 *
		 * \return Returns the options as a nes_ntsc_setup_t (with no decoder matrix or palettes).
		 */
		inline nes_ntsc_setup_t								ToSetup() const {
			nes_ntsc_setup_t nsRet = nes_ntsc_composite;
			nsRet.hue = dHue;
			nsRet.saturation = dSaturation;
			nsRet.contrast = dContrast;
			nsRet.brightness = dBrightness;
			nsRet.sharpness = dSharpness;
			nsRet.gamma = dGamma;
			nsRet.resolution = dResolution;
			nsRet.artifacts = dArtifacts;
			nsRet.fringing = dFringing;
			nsRet.bleed = dBleed;
			nsRet.merge_fields = bMergeFields ? 1 : 0;
			return nsRet;
		}
	};


	/** The kinds of signal noise of the L. Spiro filters. */
	enum LSN_NOISE_TYPE : uint32_t {
		LSN_NT_NONE,																										/**< No noise. */
		LSN_NT_CUSTOM,																										/**< Gaussian noise of a given amount (LSN_LSPIRO_OPTIONS::fNoiseAmount). */
		LSN_NT_PHYSICAL,																									/**< Thermal (Johnson-Nyquist) noise at the host CPU's temperature. */
		LSN_NT_TOTAL,																										/**< The number of noise types. */
	};


	/**
	 * Struct LSN_LSPIRO_OPTIONS
	 * \brief The settings of the L. Spiro NTSC and PAL filters.
	 *
	 * Description: The settings of the L. Spiro NTSC and PAL filters (see CLSpiroNtscFilterBase and CLSpiroPalFilterBase).  The PAL-only
	 *	settings are ignored by the NTSC filters.  The phosphor-decay settings are used only by the software filters (the GPU filters use
	 *	LSN_GPU_FILTER_OPTIONS).
	 */
	struct LSN_LSPIRO_OPTIONS {
		uint16_t											ui16WidthScale = 8;											/**< How many output samples each input pixel becomes horizontally. */
		uint32_t											ui32KernelSizeY = 12;										/**< The size of the Y filter kernel. */
		uint32_t											ui32KernelSizeChroma = 12;									/**< The size of the chroma filter kernel. */
		LSN_FILTER_FUNC										ffFilterY = LSN_FF_BOX;										/**< The Y filter function. */
		float												fFilterYParms[2] = { 0.0f, 0.0f };							/**< The parameters of the Y filter function. */
		LSN_FILTER_FUNC										ffFilterChroma = LSN_FF_BOX;								/**< The chroma filter function. */
		float												fFilterChromaParms[2] = { 0.0f, 0.0f };						/**< The parameters of the chroma filter function. */
		float												fHue = float( 8.0 * std::numbers::pi / 180.0 );				/**< The hue, in radians. */
		float												fGamma = 1.0f / 0.45f;										/**< The CRT gamma. */
		float												fBrightness = 0.0f;											/**< The brightness (a black-level offset, as on a television). */
		float												fSaturation = -0.40f + 1.0f;								/**< The saturation. */
		float												fContrast = 1.0f - 0.070710678118654752440084436210485f;	/**< The contrast (a gain from black applied to luma and chroma, as on a television). */
		float												fBlackLevel = 0.312f;										/**< The black level. */
		float												fWhiteLevel = 1.100f;										/**< The white level. */
		float												fBleed = 0.0f;												/**< How much of each filter kernel bleeds into its neighbors. */
		float												fPhosphorDecayPeriod = 1.79113161563873291015625f;			/**< The time it takes the phosphors to decay to 0.001 (software only). */
		float												fPhosphorDecayLevel = 0.25f;								/**< The initial phosphor decay level (software only). */
		CLSpiroPalFilterBase::LSN_COMB_FILTER				cfCombFilter = CLSpiroPalFilterBase::LSN_CF_NONE;			/**< The vertical comb filter (PAL only). */
		CLSpiroNtscFilterBase::LSN_COMB_FILTER				cfNtscCombFilter = CLSpiroNtscFilterBase::LSN_CF_NONE;		/**< The vertical comb filter (NTSC only). */
		float												fCombWeight = 0.5f;											/**< The share of each line's chroma taken from its neighbors (PAL only). */
		uint16_t											ui16PixelToSignal = 10;										/**< How many signals each pixel generates (PAL only). */
		LSN_NOISE_TYPE										ntNoiseType = LSN_NT_PHYSICAL;								/**< The kind of signal noise. */
		float												fNoiseAmount = 0.0225f;										/**< The standard deviation of the noise (LSN_NT_CUSTOM only). */
		CColorSpace::LSN_DEMODULATOR						dmDemodulator = CColorSpace::LSN_DM_NONE;					/**< The decoder chip of the emulated television (its color correction). */
	};


	/**
	 * Struct LSN_FILTER_REGION_OPTIONS
	 * \brief The settings of every filter for a single region.
	 *
	 * Description: The settings of every filter for a single region.  Each filter has presets only for the regions it can handle:
	 *	palette (every region), Blargg (NTSC-M and NTSC-J), L. Spiro NTSC (NTSC-M and NTSC-J), L. Spiro PAL (PAL-I, PAL-D/K, PAL-M, and
	 *	PAL-N), LMP88959 NTSC (NTSC-M and NTSC-J), and LMP88959 PAL (PAL-I and PAL-D/K).  A filter object made for one region (the
	 *	L. Spiro filters, the NTSC-J GPU filters, and the PAL-I and PAL-D/K LMP88959 filters) uses the settings of that region.  A filter
	 *	object shared by several regions uses the preset nearest the active region:  the palette uses the active region; the software
	 *	Blargg, LMP88959 NTSC, and L. Spiro NTSC filters use NTSC-J when it is active, otherwise NTSC-M.  The fields of filters that have no
	 *	preset for a region are unused in that region.
	 */
	struct LSN_FILTER_REGION_OPTIONS {
		bool												bUseGlobal = true;											/**< Use the global settings rather than these (per-game settings only). */

		LSN_GPU_FILTER_OPTIONS								gfoPalette;													/**< The GPU palette filters. */

		LSN_BLARGG_OPTIONS									boBlarggNtsc;												/**< Blargg's NTSC filter (software and GPU). */
		LSN_GPU_FILTER_OPTIONS								gfoBlargg;													/**< The GPU Blargg filters. */

		LSN_LMP88959_SETTINGS								lsLmpNtscCpu = LSN_LMP88959_SETTINGS::NtscCpuDefaults();	/**< The LMP88959 NTSC software filter. */
		LSN_LMP88959_SETTINGS								lsLmpNtscGpu = LSN_LMP88959_SETTINGS::NtscGpuDefaults();	/**< The LMP88959 NTSC GPU filters. */
		LSN_GPU_FILTER_OPTIONS								gfoLmpNtsc;													/**< The LMP88959 NTSC GPU filters. */
		LSN_LMP88959_SETTINGS								lsLmpPalCpu = LSN_LMP88959_SETTINGS::PalCpuDefaults();		/**< The LMP88959 PAL software filter. */
		LSN_LMP88959_SETTINGS								lsLmpPalGpu = LSN_LMP88959_SETTINGS::PalGpuDefaults();		/**< The LMP88959 PAL GPU filters. */
		LSN_GPU_FILTER_OPTIONS								gfoLmpPal;													/**< The LMP88959 PAL GPU filters. */

		LSN_LSPIRO_OPTIONS									lsoLSpiroCpu;												/**< This region's L. Spiro software filter. */
		LSN_LSPIRO_OPTIONS									lsoLSpiroGpu;												/**< This region's L. Spiro GPU filters. */
		LSN_GPU_FILTER_OPTIONS								gfoLSpiro;													/**< This region's L. Spiro GPU filters. */


		// == Functions.
		/**
		 * Gets the default settings of a region.  These match how each filter was configured before its settings became adjustable.  The
		 *	software L. Spiro defaults depend on the SIMD features of the CPU (the filter is made cheaper on CPUs without AVX).
		 *
		 * \param _pmRegion The region.
		 * \return Returns the default settings of the region.
		 */
		static LSN_FILTER_REGION_OPTIONS					Defaults( LSN_PPU_METRICS _pmRegion ) {
			LSN_FILTER_REGION_OPTIONS froRet;
			froRet.gfoPalette.gGamma = CNesPalette::LSN_G_NONE;
			froRet.gfoPalette.ui32VertSharpness = 5;
			froRet.gfoPalette.ui32HorSharpness = 5;
			froRet.gfoLmpPal.gGamma = CNesPalette::LSN_G_CRT1;

			// The convolution filters each filter looks best with.
			froRet.gfoBlargg.SetConvolution( CResamplerBase::LSN_FF_ROBIDOUX, CResamplerBase::LSN_FF_LINEAR, CResamplerBase::LSN_FF_LINEAR, CResamplerBase::LSN_FF_TOTAL );
			froRet.gfoLmpNtsc.SetConvolution( CResamplerBase::LSN_FF_ROBIDOUXSHARP, CResamplerBase::LSN_FF_ROBIDOUXSOFT, CResamplerBase::LSN_FF_LINEAR, CResamplerBase::LSN_FF_TOTAL );
			froRet.gfoLmpPal.SetConvolution( CResamplerBase::LSN_FF_ROBIDOUX, CResamplerBase::LSN_FF_ROBIDOUXSOFT, CResamplerBase::LSN_FF_LINEAR, CResamplerBase::LSN_FF_TOTAL );

			// The NTSC-J (Famicom) GPU filters convert from the color space of a Japanese television.
			if ( _pmRegion == LSN_PM_NTSC_J ) {
				for ( LSN_GPU_FILTER_OPTIONS * pgfoThis : { &froRet.gfoBlargg, &froRet.gfoLmpNtsc, &froRet.gfoLSpiro } ) {
					pgfoThis->cspColorSpace = CColorSpace::LSN_CSP_TOSHIBA_CF2005;
				}
			}

			const bool bAvx = CUtilities::IsAvx512FSupported() || CUtilities::IsAvxSupported();
			const bool bSse4 = CUtilities::IsSse4Supported();
			LSN_LSPIRO_OPTIONS & lsoCpu = froRet.lsoLSpiroCpu;
			LSN_LSPIRO_OPTIONS & lsoGpu = froRet.lsoLSpiroGpu;
			switch ( _pmRegion ) {
				case LSN_PM_PAL : {}				LSN_FALLTHROUGH
				case LSN_PM_DENDY : {}				LSN_FALLTHROUGH
				case LSN_PM_PALM : {}				LSN_FALLTHROUGH
				case LSN_PM_PALN : {
					// The PAL filter's own defaults.
					for ( LSN_LSPIRO_OPTIONS * plsoThis : { &lsoCpu, &lsoGpu } ) {
						plsoThis->ui16WidthScale = 10;
						plsoThis->fGamma = 2.8f;
						plsoThis->fBlackLevel = 0.0f;
					}
					froRet.gfoLSpiro.gGamma = CNesPalette::LSN_G_CRT1;
					break;
				}
				default : {}
			}
			switch ( _pmRegion ) {
				case LSN_PM_PAL : {
					lsoCpu.cfCombFilter = lsoGpu.cfCombFilter = CLSpiroPalFilterBase::LSN_CF_DELAY_LINE;
					lsoCpu.ui16WidthScale = bAvx ? 10 : (bSse4 ? 5 : 2);
					if ( bAvx ) {
						lsoCpu.ui32KernelSizeY = lsoCpu.ui32KernelSizeChroma = 48;
						lsoCpu.ffFilterY = LSN_FF_LANCZOSX; lsoCpu.fFilterYParms[0] = 2.5f; lsoCpu.fFilterYParms[1] = 0.0f;
						lsoCpu.ffFilterChroma = LSN_FF_GAUSSIANX;
					}
					lsoGpu.ui16WidthScale = 10;
					lsoGpu.ui32KernelSizeY = lsoGpu.ui32KernelSizeChroma = 48;
					lsoGpu.ffFilterY = LSN_FF_LANCZOSX; lsoGpu.fFilterYParms[0] = 2.5f; lsoGpu.fFilterYParms[1] = 0.0f;
					lsoGpu.ffFilterChroma = LSN_FF_GAUSSIANX;
					break;
				}
				case LSN_PM_DENDY : {
					lsoCpu.cfCombFilter = lsoGpu.cfCombFilter = CLSpiroPalFilterBase::LSN_CF_3_LINE;
					lsoCpu.fGamma = 2.35f;
					lsoCpu.fPhosphorDecayPeriod = 2.0f;
					lsoCpu.ui16WidthScale = bAvx ? 10 : (bSse4 ? 5 : 2);
					if ( bAvx ) {
						lsoCpu.ui32KernelSizeY = lsoCpu.ui32KernelSizeChroma = 44;
						lsoCpu.ffFilterY = LSN_FF_CARDINAL_SPLINE_UNIFORM;
						lsoCpu.ffFilterChroma = LSN_FF_GAUSSIANX;
					}
					lsoGpu.ui16WidthScale = 10;
					lsoGpu.ui32KernelSizeY = lsoGpu.ui32KernelSizeChroma = 44;
					lsoGpu.ffFilterY = LSN_FF_CARDINAL_SPLINE_UNIFORM;
					lsoGpu.ffFilterChroma = LSN_FF_GAUSSIANX;
					break;
				}
				case LSN_PM_PALM : {
					lsoCpu.cfCombFilter = lsoGpu.cfCombFilter = CLSpiroPalFilterBase::LSN_CF_DELAY_LINE;
					lsoCpu.ui16PixelToSignal = 8;
					lsoCpu.fGamma = 1.0f / 0.45f;
					lsoCpu.ui16WidthScale = bAvx ? 8 : (bSse4 ? 4 : 2);
					if ( bAvx ) {
						lsoCpu.ui32KernelSizeY = lsoCpu.ui32KernelSizeChroma = 46;
						lsoCpu.ffFilterY = LSN_FF_CARDINAL_SPLINE_UNIFORM;
						lsoCpu.ffFilterChroma = LSN_FF_GAUSSIANX;
					}
					lsoGpu.ui16WidthScale = 8;
					lsoGpu.ui32KernelSizeY = lsoGpu.ui32KernelSizeChroma = 46;
					lsoGpu.ffFilterY = LSN_FF_CARDINAL_SPLINE_UNIFORM;
					lsoGpu.ffFilterChroma = LSN_FF_GAUSSIANX;
					break;
				}
				case LSN_PM_PALN : {
					lsoCpu.cfCombFilter = lsoGpu.cfCombFilter = CLSpiroPalFilterBase::LSN_CF_3_LINE;
					lsoCpu.ui16PixelToSignal = 8;
					lsoCpu.fGamma = 2.5f;
					lsoCpu.fPhosphorDecayPeriod = 1.9f;
					lsoCpu.ui16WidthScale = bAvx ? 8 : (bSse4 ? 4 : 2);
					if ( bAvx ) {
						lsoCpu.ui32KernelSizeY = lsoCpu.ui32KernelSizeChroma = 48;
						lsoCpu.ffFilterY = LSN_FF_GAUSSIANX;
						lsoCpu.ffFilterChroma = LSN_FF_GAUSSIANX;
					}
					lsoGpu.ui16WidthScale = 8;
					lsoGpu.ui32KernelSizeY = lsoGpu.ui32KernelSizeChroma = 48;
					lsoGpu.ffFilterY = LSN_FF_GAUSSIANX;
					lsoGpu.ffFilterChroma = LSN_FF_GAUSSIANX;
					break;
				}
				default : {
					// NTSC-M and NTSC-J.
					lsoCpu.ui16WidthScale = bAvx ? 8 : (bSse4 ? 4 : 2);
					if ( bAvx ) {
						lsoCpu.ui32KernelSizeY = lsoCpu.ui32KernelSizeChroma = 48;
						lsoCpu.ffFilterY = LSN_FF_LANCZOSX; lsoCpu.fFilterYParms[0] = 99.0f / 40.0f; lsoCpu.fFilterYParms[1] = 0.0f;
						lsoCpu.ffFilterChroma = LSN_FF_GAUSSIANX;
					}
					lsoGpu.ui16WidthScale = 8;
					lsoGpu.ui32KernelSizeY = lsoGpu.ui32KernelSizeChroma = 48;
					lsoGpu.ffFilterY = LSN_FF_LANCZOSX; lsoGpu.fFilterYParms[0] = 99.0f / 40.0f; lsoGpu.fFilterYParms[1] = 0.0f;
					lsoGpu.ffFilterChroma = LSN_FF_GAUSSIANX;
				}
			}
			return froRet;
		}
	};


	/**
	 * Struct LSN_VIDEO_FILTER_OPTIONS
	 * \brief The settings of every filter for every region.
	 *
	 * Description: The settings of every filter for every region.  The options hold one of these as the global settings and one as the
	 *	per-game settings.
	 */
	struct LSN_VIDEO_FILTER_OPTIONS {
		LSN_VIDEO_FILTER_OPTIONS() {
			for ( size_t I = 0; I < LSN_PM_CONSOLE_TOTAL; ++I ) {
				froRegions[I] = LSN_FILTER_REGION_OPTIONS::Defaults( LSN_PPU_METRICS( I ) );
			}
		}


		// == Members.
		LSN_FILTER_REGION_OPTIONS							froRegions[LSN_PM_CONSOLE_TOTAL];							/**< The settings of each region (indexed by LSN_PPU_METRICS). */
	};

}	// namespace lsn

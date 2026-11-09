/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The presets of the Deep Internals of the L. Spiro NTSC and PAL filters (their filter kernels and comb filters).
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Localization/LSNLocalization.h"
#include "LSNVideoFilterOptions.h"

#include <cstddef>
#include <iterator>


namespace lsn {

	/**
	 * Struct LSN_LSPIRO_DEEP_PRESET
	 * \brief A preset of the Deep Internals of an L. Spiro filter.
	 *
	 * Description: A preset of the Deep Internals of an L. Spiro filter:  the chroma and luma kernels and the vertical comb filter.
	 */
	struct LSN_LSPIRO_DEEP_PRESET {
		const wchar_t *										pwcName;										/**< The name shown in the Preset combo box. */
		bool												bPal;											/**< True for the L. Spiro PAL filter, false for the L. Spiro NTSC filter. */
		LSN_FILTER_FUNC										ffChroma;										/**< The chroma filter function. */
		float												fChromaParms[2];								/**< The parameters of the chroma filter function. */
		uint32_t											ui32ChromaKernel;								/**< The size of the chroma kernel. */
		LSN_FILTER_FUNC										ffLuma;											/**< The luma filter function. */
		float												fLumaParms[2];									/**< The parameters of the luma filter function. */
		uint32_t											ui32LumaKernel;									/**< The size of the luma kernel. */
		uint8_t												ui8Comb;										/**< The comb filter:  a CLSpiroNtscFilterBase::LSN_COMB_FILTER (NTSC) or CLSpiroPalFilterBase::LSN_COMB_FILTER (PAL). */
	};


	/**
	 * Class CLSpiroPresets
	 * \brief The presets of the Deep Internals of the L. Spiro filters.
	 *
	 * Description: The presets of the Deep Internals of the L. Spiro filters.  Add entries to the table in DeepPresets() to add presets;
	 *	the Video Options dialog lists the entries for each filter in table order.
	 */
	class CLSpiroPresets {
	public :
		// == Functions.
		/**
		 * Gets the table of presets.
		 *
		 * \param _sTotal Holds the returned number of presets.
		 * \return Returns the table of presets.
		 */
		static inline const LSN_LSPIRO_DEEP_PRESET *		DeepPresets( size_t &_sTotal ) {
			static const LSN_LSPIRO_DEEP_PRESET s_ldpPresets[] = {
				// Name											PAL		Chroma											Kernel	Luma													Kernel	Comb
				// == NTSC.
				{ LSN_LSTR( LSN_STR_VO_DP_LSPIRO_NTSC ),		false,	LSN_FF_GAUSSIANX,	{ 0.0f, 0.0f },				48,		LSN_FF_LANCZOSX,					{ 99.0f, 40.0f },	48,		CLSpiroNtscFilterBase::LSN_CF_NONE },
				{ LSN_LSTR( LSN_STR_VO_DP_LSPIRO_NTSC_2 ),		false,	LSN_FF_GAUSSIANX,	{ 0.0f, 0.0f },				48,		LSN_FF_LANCZOSX,					{ 50.0f, 19.0f },	48,		CLSpiroNtscFilterBase::LSN_CF_NONE },
				{ LSN_LSTR( LSN_STR_VO_DP_LSPIRO_NTSC_SOFT ),	false,	LSN_FF_GAUSSIANX,	{ 0.0f, 0.0f },				48,		LSN_FF_LANCZOSX,					{ 100.0f, 65.0f },	35,		CLSpiroNtscFilterBase::LSN_CF_NONE },
				{ LSN_LSTR( LSN_STR_VO_DP_FAST ),				false,	LSN_FF_BOX,			{ 0.0f, 0.0f },				12,		LSN_FF_BOX,							{ 0.0f, 0.0f },		12,		CLSpiroNtscFilterBase::LSN_CF_NONE },

				// == PAL.
				{ LSN_LSTR( LSN_STR_VO_DP_PAL_I ),				true,	LSN_FF_GAUSSIANX,	{ 0.0f, 0.0f },				48,		LSN_FF_LANCZOSX,					{ 5.0f, 2.0f },		48,		CLSpiroPalFilterBase::LSN_CF_DELAY_LINE },
				{ LSN_LSTR( LSN_STR_VO_DP_PAL_DK ),				true,	LSN_FF_GAUSSIANX,	{ 0.0f, 0.0f },				44,		LSN_FF_CARDINAL_SPLINE_UNIFORM,		{ 0.0f, 0.0f },		44,		CLSpiroPalFilterBase::LSN_CF_3_LINE },
				{ LSN_LSTR( LSN_STR_VO_DP_PAL_M ),				true,	LSN_FF_GAUSSIANX,	{ 0.0f, 0.0f },				46,		LSN_FF_CARDINAL_SPLINE_UNIFORM,		{ 0.0f, 0.0f },		46,		CLSpiroPalFilterBase::LSN_CF_DELAY_LINE },
				{ LSN_LSTR( LSN_STR_VO_DP_PAL_N ),				true,	LSN_FF_GAUSSIANX,	{ 0.0f, 0.0f },				48,		LSN_FF_GAUSSIANX,					{ 0.0f, 0.0f },		48,		CLSpiroPalFilterBase::LSN_CF_3_LINE },
				{ LSN_LSTR( LSN_STR_VO_DP_FAST ),				true,	LSN_FF_BOX,			{ 0.0f, 0.0f },				12,		LSN_FF_BOX,							{ 0.0f, 0.0f },		12,		CLSpiroPalFilterBase::LSN_CF_DELAY_LINE },
			};
			_sTotal = std::size( s_ldpPresets );
			return s_ldpPresets;
		}

		/**
		 * Applies a preset to L. Spiro settings.
		 *
		 * \param _ldpPreset The preset to apply.
		 * \param _lsoOptions The settings to which to apply the preset.
		 */
		static inline void									Apply( const LSN_LSPIRO_DEEP_PRESET &_ldpPreset, LSN_LSPIRO_OPTIONS &_lsoOptions ) {
			_lsoOptions.ffFilterChroma = _ldpPreset.ffChroma;
			_lsoOptions.fFilterChromaParms[0] = _ldpPreset.fChromaParms[0];
			_lsoOptions.fFilterChromaParms[1] = _ldpPreset.fChromaParms[1];
			_lsoOptions.ui32KernelSizeChroma = _ldpPreset.ui32ChromaKernel;
			_lsoOptions.ffFilterY = _ldpPreset.ffLuma;
			_lsoOptions.fFilterYParms[0] = _ldpPreset.fLumaParms[0];
			_lsoOptions.fFilterYParms[1] = _ldpPreset.fLumaParms[1];
			_lsoOptions.ui32KernelSizeY = _ldpPreset.ui32LumaKernel;
			if ( _ldpPreset.bPal ) { _lsoOptions.cfCombFilter = CLSpiroPalFilterBase::LSN_COMB_FILTER( _ldpPreset.ui8Comb ); }
			else { _lsoOptions.cfNtscCombFilter = CLSpiroNtscFilterBase::LSN_COMB_FILTER( _ldpPreset.ui8Comb ); }
		}

		/**
		 * Determines whether L. Spiro settings match a preset.  The parameters of filter functions that do not use them are ignored.
		 *
		 * \param _ldpPreset The preset to compare.
		 * \param _lsoOptions The settings to compare.
		 * \return Returns true if the settings match the preset.
		 */
		static inline bool									Matches( const LSN_LSPIRO_DEEP_PRESET &_ldpPreset, const LSN_LSPIRO_OPTIONS &_lsoOptions ) {
			if ( _lsoOptions.ffFilterChroma != _ldpPreset.ffChroma || _lsoOptions.ui32KernelSizeChroma != _ldpPreset.ui32ChromaKernel ) { return false; }
			if ( _lsoOptions.ffFilterY != _ldpPreset.ffLuma || _lsoOptions.ui32KernelSizeY != _ldpPreset.ui32LumaKernel ) { return false; }
			const size_t sChromaParms = KernelParmCount( _ldpPreset.ffChroma );
			for ( size_t I = 0; I < sChromaParms; ++I ) {
				if ( _lsoOptions.fFilterChromaParms[I] != _ldpPreset.fChromaParms[I] ) { return false; }
			}
			const size_t sLumaParms = KernelParmCount( _ldpPreset.ffLuma );
			for ( size_t I = 0; I < sLumaParms; ++I ) {
				if ( _lsoOptions.fFilterYParms[I] != _ldpPreset.fLumaParms[I] ) { return false; }
			}
			return _ldpPreset.bPal ? (uint8_t( _lsoOptions.cfCombFilter ) == _ldpPreset.ui8Comb) : (uint8_t( _lsoOptions.cfNtscCombFilter ) == _ldpPreset.ui8Comb);
		}

		/**
		 * Gets the number of parameters a filter function uses.
		 *
		 * \param _ffFunc The filter function.
		 * \return Returns the number of parameters (0, 1, or 2) the filter function uses.
		 */
		static inline size_t								KernelParmCount( LSN_FILTER_FUNC _ffFunc ) {
			switch ( _ffFunc ) {
				case LSN_FF_KAISER : { return 1; }
				case LSN_FF_LANCZOSX : {}				LSN_FALLTHROUGH
				case LSN_FF_BLACKMAN : {}				LSN_FALLTHROUGH
				case LSN_FF_CRT_HUMP : { return 2; }
				default : { return 0; }
			}
		}

		/**
		 * Gets the rate of the signals of an L. Spiro filter.
		 *
		 * \param _bPal True for the L. Spiro PAL filter.
		 * \param _ui16PixelToSignal The number of signals per pixel (PAL only).
		 * \return Returns the rate of the signals, in Hz.
		 */
		static inline double								SignalHz( bool _bPal, uint16_t _ui16PixelToSignal ) {
			// PAL:  each pixel (26.6017125 MHz / 5) becomes _ui16PixelToSignal signals.
			return _bPal ? (26601712.5 / 5.0 * _ui16PixelToSignal) : (315000000.0 / 88.0 * 12.0);
		}

		/**
		 * Gets the default parameters of a filter function, set when a filter function is selected.  Functions without parameters leave
		 *	the parameters unchanged.
		 *
		 * \param _ffFunc The filter function.
		 * \param _bLuma True for the luma filter, false for the chroma filter.
		 * \param _dSignalHz The rate of the signals, in Hz (see SignalHz()).
		 * \param _pfParms The 2 parameters to set.
		 */
		static inline void									DefaultKernelParms( LSN_FILTER_FUNC _ffFunc, bool _bLuma, double _dSignalHz, float * _pfParms ) {
			switch ( _ffFunc ) {
				case LSN_FF_LANCZOSX : {
					// 3 lobes.
					_pfParms[0] = 3.0f;
					_pfParms[1] = 1.0f;
					break;
				}
				case LSN_FF_KAISER : {
					// Beta = 4.1226, the window this filter used before beta could be set.
					_pfParms[0] = 412.26f;
					_pfParms[1] = 0.0f;
					break;
				}
				case LSN_FF_BLACKMAN : {
					// Luma rolls off by 3 MHz (below the subcarrier); the demodulated chroma is limited to about 1.3 MHz (the I bandwidth).
					_pfParms[0] = _bLuma ? 3000000.0f : 1300000.0f;
					_pfParms[1] = float( _dSignalHz );
					break;
				}
				case LSN_FF_CRT_HUMP : {
					// A flat middle of 4 signals with linear fall-offs.
					_pfParms[0] = 800.0f;
					_pfParms[1] = 100.0f;
					break;
				}
				default : {}
			}
		}
	};

}	// namespace lsn

/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Bisqwit's NTSC filter with my own adjustments.
 */

#include "LSNNtscBisqwitLSpiroFilter.h"


namespace lsn {

	// == Members.
	const float CNtscBisqwitLSpiroFilter::m_fLevels[16] = {							/**< Output levels. */
		0.228f, 0.312f, 0.552f, 0.880f, // Signal low.
		0.616f, 0.840f, 1.100f, 1.100f, // Signal high.
		0.192f, 0.256f, 0.448f, 0.712f, // Signal low, attenuated.
		0.500f, 0.676f, 0.896f, 0.896f  // Signal high, attenuated.
	};

	CNtscBisqwitLSpiroFilter::CNtscBisqwitLSpiroFilter() {
	}
	CNtscBisqwitLSpiroFilter::~CNtscBisqwitLSpiroFilter() {
	}

	// == Functions.
	/**
	 * Converts a 9-bit PPU output index to an NTSC signal.
	 * 
	 * \param _ui16Pixel The PPU output index to convert.
	 * \param _ui16Phase The phase counter.
	 * \return Returns the signal produced by the PPU output index.
	 **/
	float CNtscBisqwitLSpiroFilter::IndexToNtscSignal( uint16_t _ui16Pixel, uint16_t _ui16Phase ) {
		// Decode the NES color.
		uint16_t ui16Color = (_ui16Pixel & 0x0F);								// 0..15 "cccc".
		uint16_t ui16Level = (ui16Color >= 0xE) ? 1 : (_ui16Pixel >> 4) & 3;	// 0..3  "ll".  For colors 14..15, level 1 is forced.
		uint16_t ui16Emphasis = (_ui16Pixel >> 6);								// 0..7  "eee".

#define LSN_INCOLORPHASE( COLOR )					(((COLOR) + _ui16Phase) % 12 < 6)
		// When de-emphasis bits are set, some parts of the signal are attenuated:
		// Colors [14..15] are not affected by de-emphasis.
		uint16_t ui16Atten = ((ui16Color < 0xE) &&
			((ui16Emphasis & 1) && LSN_INCOLORPHASE( 0xC )) ||
			((ui16Emphasis & 2) && LSN_INCOLORPHASE( 0x4 )) ||
			((ui16Emphasis & 4) && LSN_INCOLORPHASE( 0x8 ))) ? 8 : 0;

		// The square wave for this color alternates between these two voltages:
		float fLow  = m_fLevels[ui16Level+ui16Atten];
		float fHigh = (&m_fLevels[4])[ui16Level+ui16Atten];
		if ( ui16Color == 0 ) { return fHigh; }			// For color 0, only high level is emitted.
		if ( ui16Color > 12 ) { return fLow; }			// For colors 13..15, only low level is emitted.

		return LSN_INCOLORPHASE( ui16Color ) ? fHigh : fLow;
#undef LSN_INCOLORPHASE
	}

}	// namespace lsn

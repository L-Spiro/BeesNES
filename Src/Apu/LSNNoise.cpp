/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: An APU noise unit.  Generates noise.
 */

#include "LSNNoise.h"


namespace lsn {

	CNoise::CNoise() {
	}
	CNoise::~CNoise() {
	}

	// == Functions.
	/**
	 * Resets the unit to a known state.
	 **/
	void CNoise::ResetToKnown() {
		CLengthCounter::ResetToKnown();
		CEnvelope::ResetToKnown();
		CSequencer::ResetToKnown();
		m_bMode = false;
		m_ui32Sequence = 1;
	}

	/**
	 * Handles the tick work.
	 * 
	 * \return Returns a value such that if the lowest bit (0) is set, noise is expected.
	 **/
	uint8_t CNoise::WeDoBeTicknTho() {
		m_ui32Sequence = ((
			(m_ui32Sequence & 0b000000000000001) ^
			((m_bMode ? (m_ui32Sequence >> 6) : (m_ui32Sequence >> 1)) & 0b000000000000001)
			) << 14) |
			((m_ui32Sequence & 0x7FFF) >> 1);
		return (m_ui32Sequence & 0b000000000000001) == 0;
	}

}	// namespace lsn

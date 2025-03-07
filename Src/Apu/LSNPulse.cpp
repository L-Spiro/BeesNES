/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: An APU pulse.  Generates square waves.
 */

#include "LSNPulse.h"


namespace lsn {

	CPulse::CPulse() :
		CSweeper( m_ui16Reload ) {
	}
	CPulse::~CPulse() {
	}

	// == Functions.
	/**
	 * Resets the unit to a known state.
	 **/
	void CPulse::ResetToKnown() {
		CLengthCounter::ResetToKnown();
		CEnvelope::ResetToKnown();
		CSequencer::ResetToKnown();
		CSweeper::ResetToKnown();
	}

	/**
	 * Handles the tick work.
	 * 
	 * \return Returns an output value.
	 **/
	uint8_t CPulse::WeDoBeTicknTho() {
		uint8_t ui8Seq = uint8_t( m_ui32Sequence >> m_ui8SeqOff-- );
		if LSN_UNLIKELY( m_ui8SeqOff == 0xFF ) { m_ui8SeqOff = 0x7; }
		return ui8Seq & 1;
	}

}	// namespace lsn

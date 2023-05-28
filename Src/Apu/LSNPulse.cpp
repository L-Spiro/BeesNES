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

}	// namespace lsn

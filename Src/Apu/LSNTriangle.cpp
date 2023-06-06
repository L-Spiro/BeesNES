/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: An APU triangle unit.  Generates a triangle often used for bass.
 */

#include "LSNTriangle.h"


namespace lsn {

	CTriangle::CTriangle() {
	}
	CTriangle::~CTriangle() {
	}

	// == Functions.
	/**
	 * Resets the unit to a known state.
	 **/
	void CTriangle::ResetToKnown() {
		CLengthCounter::ResetToKnown();
		CSequencer::ResetToKnown();
	}

	/**
	 * Handles the tick work.
	 * 
	 * \return Returns an output value.
	 **/
	uint8_t CTriangle::WeDoBeTicknTho() {
		return (m_ui32Sequence & 0b000000000000001) == 0;
	}

}	// namespace lsn

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: An APU triangle unit.  Generates a triangle often used for bass.
 */

#include "LSNTriangle.h"


namespace lsn {

	// == Members.
	/** The triangle shape. */
	uint8_t CTriangle::m_ui8Triangle[32] = {
		15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0,
		 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15
	};

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
		CLinearCounter::ResetToKnown();
		CSequencer::ResetToKnown();
		m_ui8Out = m_ui8Triangle[m_ui8SeqOff];
	}

	/**
	 * Handles the tick work.
	 * 
	 * \return Returns an output value.
	 **/
	uint8_t CTriangle::WeDoBeTicknTho() {
		uint8_t ui8Idx = m_ui8Triangle[m_ui8SeqOff++];
		m_ui8SeqOff %= LSN_ELEMENTS( m_ui8Triangle );
		return ui8Idx;
	}

	/**
	 * Returns the condition for ticking the sequencer.
	 * 
	 * \param _bEnabled Whether the unit is enabled for not.
	 * \return Return true to perform a sequencer tick, or false to skip sequencer work.
	 **/
	bool CTriangle::ShouldBeTicknTho( bool /*_bEnabled*/ ) {
		return m_ui8Counter != 0 && m_ui8LinearCounter != 0;
	}

}	// namespace lsn

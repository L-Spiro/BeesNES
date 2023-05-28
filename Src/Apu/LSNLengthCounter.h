/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: An APU length counter.  Counts a timer down to determine the timing of an event.
 */


#pragma once

#include "../LSNLSpiroNes.h"

namespace lsn {

	/**
	 * Class CLengthCounter
	 * \brief An APU length counter.
	 *
	 * Description: An APU length counter.  Counts a timer down to determine the timing of an event.
	 */
	class CLengthCounter {
	public :
		inline CLengthCounter();
		inline ~CLengthCounter();


		// == Functions.
		/**
		 * Updates the counter.
		 * 
		 * \param _bEnabled Determines if work should actually be done.
		 * \param _bHalt The halt flag.
		 * \return Returns the counter after any updating that needs to be done.
		 **/
		inline uint8_t								TickLengthCounter( bool _bEnabled, bool _bHalt );

		/**
		 * Sets the length counter value.
		 * 
		 * \param _ui8Val The 8-bit length counter value to set.
		 **/
		inline void									SetLengthCounter( uint8_t _ui8Val );

		/**
		 * Gets the length counter.
		 * 
		 * \return Returns the length counter.
		 **/
		inline uint8_t								GetLengthCounter() const;

		/**
		 * Resets the unit to a known state.
		 **/
		inline void									ResetToKnown();


	protected :
		// == Members.
		/** The counter. */
		uint8_t										m_ui8Counter;

	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	inline CLengthCounter::CLengthCounter() :
		m_ui8Counter( 0 ) {
	}
	inline CLengthCounter::~CLengthCounter() {
	}

	// == Functions.
	/**
	 * Updates the counter.
	 * 
	 * \param _bEnabled Determines if work should actually be done.
	 * \param _bHalt The halt flag.
	 * \return Returns the counter after any updating that needs to be done.
	 **/
	inline uint8_t CLengthCounter::TickLengthCounter( bool _bEnabled, bool _bHalt ) {
		if ( !_bEnabled ) {
			m_ui8Counter = 0;
		}
		else if ( m_ui8Counter && !_bHalt ) {
			--m_ui8Counter;
		}
		return m_ui8Counter;
	}

	/**
	 * Sets the length counter value.
	 * 
	 * \param _ui8Val The 8-bit length counter value to set.
	 **/
	inline void CLengthCounter::SetLengthCounter( uint8_t _ui8Val ) {
		m_ui8Counter = _ui8Val;
	}

	/**
	 * Gets the length counter.
	 * 
	 * \return Returns the length counter.
	 **/
	inline uint8_t CLengthCounter::GetLengthCounter() const {
		return m_ui8Counter;
	}

	/**
	 * Resets the unit to a known state.
	 **/
	inline void CLengthCounter::ResetToKnown() {
		m_ui8Counter = 0;
	}

}	// namespace lsn

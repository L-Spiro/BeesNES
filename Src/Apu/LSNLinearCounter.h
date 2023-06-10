/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: An APU linear counter.  Counts a timer down to determine the timing of an event.
 */


#pragma once

#include "../LSNLSpiroNes.h"

namespace lsn {

	/**
	 * Class CLinearCounter
	 * \brief An APU linear counter.
	 *
	 * Description: An APU linear counter.  Counts a timer down to determine the timing of an event.
	 */
	class CLinearCounter {
	public :
		inline CLinearCounter();
		inline ~CLinearCounter();


		// == Functions.
		/**
		 * Updates the counter.
		 * 
		 * \param _bHalt The halt flag.
		 * \return Returns the counter after any updating that needs to be done.
		 **/
		inline uint8_t								TickLinearCounter( bool _bHalt );

		/**
		 * Sets the linear counter value.
		 * 
		 * \param _ui8Val The 8-bit linear counter value to set.
		 **/
		inline void									SetLinearCounter( uint8_t _ui8Val );

		/**
		 * Gets the linear counter.
		 * 
		 * \return Returns the linear counter.
		 **/
		inline uint8_t								GetLinearCounter() const;

		/**
		 * Sets the reload flag.
		 **/
		inline void									SetLinearReload();

		/**
		 * Resets the unit to a known state.
		 **/
		inline void									ResetToKnown();


	protected :
		// == Members.
		/** The counter. */
		uint8_t										m_ui8LinearCounter			= 0;
		/** The reload value. */
		uint8_t										m_ui8LinearReload		= 0;
		/** The reload flag. */
		bool										m_bReloadLinear			= true;
	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	inline CLinearCounter::CLinearCounter() {
	}
	inline CLinearCounter::~CLinearCounter() {
	}

	// == Functions.
	/**
	 * Updates the counter.
	 * 
	 * \param _bHalt The halt flag.
	 * \return Returns the counter after any updating that needs to be done.
	 **/
	inline uint8_t CLinearCounter::TickLinearCounter( bool _bHalt ) {
		if ( m_bReloadLinear ) {
			m_bReloadLinear = false;
			m_ui8LinearCounter = m_ui8LinearReload;
		}
		else if ( m_ui8LinearCounter ) {
			--m_ui8LinearCounter;
		}
		if ( _bHalt ) {
			m_ui8LinearReload = 0;
		}
		return m_ui8LinearCounter;
	}

	/**
	 * Sets the linear counter value.
	 * 
	 * \param _ui8Val The 8-bit linear counter value to set.
	 **/
	inline void CLinearCounter::SetLinearCounter( uint8_t _ui8Val ) { m_ui8LinearReload = _ui8Val; }

	/**
	 * Gets the linear counter.
	 * 
	 * \return Returns the linear counter.
	 **/
	inline uint8_t CLinearCounter::GetLinearCounter() const { return m_ui8LinearCounter; }

	/**
	 * Sets the reload flag.
	 **/
	inline void CLinearCounter::SetLinearReload() {
		m_bReloadLinear = true;
	}

	/**
	 * Resets the unit to a known state.
	 **/
	inline void CLinearCounter::ResetToKnown() {
		m_ui8LinearCounter = m_ui8LinearReload = 0;
		m_bReloadLinear = true;
	}

}	// namespace lsn

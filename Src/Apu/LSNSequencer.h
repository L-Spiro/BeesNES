/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: An APU sequencer.  Counts a timer down to determine the timing of an event.
 */


#pragma once

#include "../LSNLSpiroNes.h"

namespace lsn {

	/**
	 * Class CSequencer
	 * \brief An APU sequencer.
	 *
	 * Description: An APU sequencer.  Counts a timer down to determine the timing of an event
	 */
	class CSequencer {
	public :
		CSequencer();
		~CSequencer();


		// == Functions.
		/**
		 * Ticks the sequencer.
		 * 
		 * \param _bEnabled Determines if work should actually be done.
		 * \return Returns m_ui8Out.
		 **/
		inline uint8_t							TickSequencer( bool _bEnabled );

		/**
		 * Sets the sequence value.
		 * 
		 * \param _ui8Val The 8-bit sequence value to set.
		 **/
		inline void								SetSeq( uint8_t _ui8Val );

		/**
		 * Sets the timer low value.
		 * 
		 * \param _ui8Val The timer low value.
		 * \return Returns the timer value.
		 **/
		inline uint16_t							SetTimerLow( uint8_t _ui8Val );

		/**
		 * Sets the timer high value.  Only the bottom 3 bits are used.
		 * 
		 * \param _ui8Val The timer high value.
		 * \param _bReset If true, the duty cycle is reset.
		 * \return Returns the timer value.
		 **/
		inline uint16_t							SetTimerHigh( uint8_t _ui8Val, bool _bReset = true );

		/**
		 * Sets the timer (reload) value directly.  Use for noise.
		 * 
		 * \param _ui16Val The value to set.
		 * \return Returns the timer value.
		 **/
		inline uint16_t							SetTimer( uint16_t _ui16Val );

		/**
		 * Gets the timer value.
		 * 
		 * \return Returns the timer value.
		 **/
		inline uint16_t							GetTimer() const;

		/**
		 * Gets the timer reload value.
		 * 
		 * \return Returns the timer reload value.
		 **/
		inline uint16_t							GetTimerReload() const;

		/**
		 * Gets the output value (1 or 0).
		 * 
		 * \return Returns the current output value.
		 **/
		inline uint8_t							Output() const;

		/**
		 * Resets the unit to a known state.
		 **/
		inline void								ResetToKnown();


	protected :
		// == Members.
		/** The sequence of bits that determines when to trigger actions. */
		uint32_t								m_ui32Sequence		= 0;
		/** The current timer value. */
		uint16_t								m_ui16Timer			= 0;
		/** The timer-reload value. */
		uint16_t								m_ui16Reload		= 0;
		/** The output value. */
		uint8_t									m_ui8Out			= 0;
		/** The sequence offset. */
		uint8_t									m_ui8SeqOff			= 0;


		// == Functions.
		/**
		 * Handles the tick work.
		 * 
		 * \return Returns an output value.
		 **/
		virtual uint8_t							WeDoBeTicknTho() { return 0; }

		/**
		 * Returns the condition for ticking the sequencer.
		 * 
		 * \param _bEnabled Whether the unit is enabled for not.
		 * \return Return true to perform a sequencer tick, or false to skip sequencer work.
		 **/
		virtual bool							ShouldBeTicknTho( bool _bEnabled ) { return _bEnabled; }
	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Ticks the sequencer.
	 * 
	 * \param _bEnabled Determines if work should actually be done.
	 * \return Returns m_ui8Out.
	 **/
	inline uint8_t CSequencer::TickSequencer( bool _bEnabled ) {
		if ( ShouldBeTicknTho( _bEnabled ) ) {
			if ( --m_ui16Timer == 0xFFFF ) {
				m_ui16Timer = m_ui16Reload;

				m_ui8Out = WeDoBeTicknTho();
			}
		}
		return m_ui8Out;
	}

	/**
	 * Sets the sequence value.
	 * 
	 * \param _ui8Val The 8-bit sequence value to set.
	 **/
	inline void CSequencer::SetSeq( uint8_t _ui8Val ) {
		m_ui32Sequence = _ui8Val;
	}

	/**
	 * Sets the timer low value.
	 * 
	 * \param _ui8Val The timer low value.
	 * \return Returns the timer value.
	 **/
	inline uint16_t CSequencer::SetTimerLow( uint8_t _ui8Val ) {
		m_ui16Reload = (m_ui16Reload & 0xFF00) | _ui8Val;
		return m_ui16Reload;
	}

	/**
	 * Sets the timer high value.  Only the bottom 3 bits are used.
	 * 
	 * \param _ui8Val The timer high value.
	 * \return Returns the timer value.
	 **/
	inline uint16_t CSequencer::SetTimerHigh( uint8_t _ui8Val, bool _bReset ) {
		m_ui16Reload = (m_ui16Reload & 0x00FF) | (uint16_t( _ui8Val & 0b111 ) << 8);
		if ( _bReset ) {
			m_ui16Timer = m_ui16Reload;
			m_ui8SeqOff = 0;
		}
		return m_ui16Reload;
	}

	/**
	 * Sets the timer (reload) value directly.  Use for noise.
	 * 
	 * \param _ui16Val The value to set.
	 * \return Returns the timer value.
	 **/
	inline uint16_t CSequencer::SetTimer( uint16_t _ui16Val ) {
		m_ui16Reload = _ui16Val;
		return m_ui16Reload;
	}

	/**
	 * Gets the timer value.
	 * 
	 * \return Returns the timer value.
	 **/
	inline uint16_t CSequencer::GetTimer() const {
		return m_ui16Timer;
	}

	/**
	 * Gets the timer reload value.
	 * 
	 * \return Returns the timer reload value.
	 **/
	inline uint16_t CSequencer::GetTimerReload() const {
		return m_ui16Reload;
	}

	/**
	 * Gets the output value (1 or 0).
	 * 
	 * \return Returns the current output value.
	 **/
	inline uint8_t CSequencer::Output() const {
		return m_ui8Out;
	}

	/**
	 * Resets the unit to a known state.
	 **/
	inline void CSequencer::ResetToKnown() {
		m_ui32Sequence		= 0;
		m_ui16Timer			= 0;
		m_ui16Reload		= 0;
		m_ui8Out			= 0;
		m_ui8SeqOff			= 0;
	}

}	// namespace lsn

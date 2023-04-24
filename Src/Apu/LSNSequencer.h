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
		virtual ~CSequencer();


		// == Functions.
		/**
		 * Ticks the sequencer.
		 * 
		 * \param _bEnabled Determines if work should actually be done.
		 * \return Returns m_ui8Out.
		 **/
		inline uint8_t							Tick( bool _bEnabled );

		/**
		 * Sets the sequence value.
		 * 
		 * \param _ui8Val The 8-bit sequence value to set.
		 **/
		inline void								SetSeq( uint8_t _ui8Val );


	protected :
		// == Members.
		/** The sequence of bits that determines when to trigger actions. */
		uint32_t								m_ui32Sequence = 0;
		/** The current timer value. */
		uint16_t								m_ui16Timer = 0;
		/** The timer-reload value. */
		uint16_t								m_ui16Reload = 0;
		/** The output value. */
		uint8_t									m_ui8Out = 0;


		// == Functions.
		/**
		 * Handles the tick work.
		 * 
		 * \param _ui32S The current sequence bits.
		 **/
		virtual void							WeDoBeTicknTho( uint32_t &/*_ui32S*/ ) {}
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
	inline uint8_t CSequencer::Tick( bool _bEnabled ) {
		if ( _bEnabled ) {
			if ( --m_ui16Timer == 0xFFFF ) {
				m_ui16Timer = m_ui16Reload + 1;
				WeDoBeTicknTho( m_ui32Sequence );
				m_ui8Out = uint8_t( m_ui32Sequence & 1 );
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

}	// namespace lsn

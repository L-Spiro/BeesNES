/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: An APU pulse.  Generates square waves.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNApuUnit.h"
#include "LSNEnvelope.h"
#include "LSNLengthCounter.h"
#include "LSNSequencer.h"


namespace lsn {

	/**
	 * Class CPulse
	 * \brief An APU pulse.  Generates square waves.
	 *
	 * Description: An APU pulse.  Generates square waves.
	 */
	class CPulse : public CApuUnit, public CLengthCounter, public CEnvelope, public CSequencer {
	public :
		CPulse();
		virtual ~CPulse();


	protected :
		// == Members.
		


		// == Functions.
		/**
		 * Handles the tick work.
		 * 
		 * \param _ui32S The current sequence bits.
		 **/
		virtual uint8_t							WeDoBeTicknTho( uint32_t &/*_ui32S*/ ) {
			/*if ( ::cos( m_ui64SeqOff / 1.0 ) >= 1.0 ) { return 0; }
			if ( ::cos( m_ui64SeqOff / 1.0 ) >= 1.0 ) { return 0; }
			if ( ::cos( m_ui64SeqOff / 1.0 ) >= 1.0 ) { return 0; }
			if ( ::cos( m_ui64SeqOff / 1.0 ) >= 1.0 ) { return 0; }
			if ( ::cos( m_ui64SeqOff / 1.0 ) >= 1.0 ) { return 0; }*/
			return 0;//::_rotr8( (*reinterpret_cast<uint8_t *>(&_ui32S)), uint8_t( m_ui64SeqOff++ & 7 ) );
		}

	};

}	// namespace lsn

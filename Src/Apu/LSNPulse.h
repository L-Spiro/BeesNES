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
#include "LSNSweeper.h"


namespace lsn {

	/**
	 * Class CPulse
	 * \brief An APU pulse.  Generates square waves.
	 *
	 * Description: An APU pulse.  Generates square waves.
	 */
	class CPulse : public CApuUnit, public CLengthCounter, public CEnvelope, public CSequencer, public CSweeper {
	public :
		CPulse();
		virtual ~CPulse();


		// == Functions.
		/**
		 * Resets the unit to a known state.
		 **/
		void									ResetToKnown();

		/**
		 * Determines if the pulse channel should produce sound.
		 * 
		 * \param _bEnabled The status of the pulse channel.
		 * \return Returns true if the channel should produce audio.
		 **/
		inline bool								ProducingSound( bool _bEnabled ) const;


	protected :
		// == Members.
		


		// == Functions.
		/**
		 * Handles the tick work.
		 * 
		 * \return Returns an output value.
		 **/
		virtual uint8_t							WeDoBeTicknTho();

	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	/**
	 * Determines if the pulse channel should produce sound.
	 * 
	 * \param _bEnabled The status of the pulse channel.
	 * \return Returns true if the channel should produce audio.
	 **/
	inline bool CPulse::ProducingSound( bool _bEnabled ) const {
		return _bEnabled &&
			GetLengthCounter() > 0 &&
			GetTimerReload() >= 8 &&
			Output() &&
			!SweeperMuted();
	}

}	// namespace lsn

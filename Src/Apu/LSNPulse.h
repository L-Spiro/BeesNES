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

		/**
		 * Determines if the pulse channel is basically on or not.  IE, evn if it is spitting out a 0 value, it may still technically be "on".
		 * 
		 * \param _bEnabled The status of the pulse channel.
		 * \return Returns true if the channel is basically on.
		 **/
		inline bool								BasicallyOn( bool _bEnabled ) const;


	protected :
		// == Members.
		


		// == Functions.
		/**
		 * Handles the tick work.
		 * 
		 * \return Returns an output value.
		 **/
		virtual uint8_t							WeDoBeTicknTho();

		/**
		 * Returns the condition for ticking the sequencer.
		 * 
		 * \param _bEnabled Whether the unit is enabled for not.
		 * \return Return true to perform a sequencer tick, or false to skip sequencer work.
		 **/
		virtual bool							ShouldBeTicknTho( bool /*_bEnabled*/ ) { return true; }

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

	/**
	 * Determines if the pulse channel is basically on or not.  IE, evn if it is spitting out a 0 value, it may still technically be "on".
	 * 
	 * \param _bEnabled The status of the pulse channel.
	 * \return Returns true if the channel is basically on.
	 **/
	inline bool CPulse::BasicallyOn( bool _bEnabled ) const {
		bool bRet = _bEnabled &&
			GetLengthCounter() > 0 &&
			GetTimerReload() >= 8 &&
			//Output() &&
			!SweeperMuted();
		/*if ( bRet ) {
			volatile int gjhggh = 0;
		}
		else {
			volatile int gjhggh = 0;
		}*/
		return bRet;
	}

}	// namespace lsn

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

}	// namespace lsn

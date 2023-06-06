/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: An APU triangle unit.  Generates a triangle often used for bass.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNApuUnit.h"
#include "LSNLengthCounter.h"
#include "LSNSequencer.h"


namespace lsn {

	/**
	 * Class CTriangle
	 * \brief An APU triangle unit.  Generates a triangle often used for bass.
	 *
	 * Description: An APU triangle unit.  Generates a triangle often used for bass.
	 */
	class CTriangle : public CApuUnit, public CLengthCounter, public CSequencer {
	public :
		CTriangle();
		virtual ~CTriangle();


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
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.

}	// namespace lsn

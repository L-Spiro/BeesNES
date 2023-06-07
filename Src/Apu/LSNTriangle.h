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
		/** The triangle shape. */
		static uint8_t							m_ui8Triangle[32];
		

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
		virtual bool							ShouldBeTicknTho( bool _bEnabled );
	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.

}	// namespace lsn

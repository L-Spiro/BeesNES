/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: An APU noise unit.  Generates noise.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNApuUnit.h"
#include "LSNEnvelope.h"
#include "LSNLengthCounter.h"
#include "LSNSequencer.h"


namespace lsn {

	/**
	 * Class CNoise
	 * \brief An APU noise unit.  Generates noise.
	 *
	 * Description: An APU noise unit.  Generates noise.
	 */
	class CNoise : public CApuUnit, public CLengthCounter, public CEnvelope, public CSequencer {
	public :
		CNoise();
		virtual ~CNoise();


		// == Functions.
		/**
		 * Resets the unit to a known state.
		 **/
		void									ResetToKnown();

		/**
		 * Sets the mode flag.
		 * 
		 * \param _bFlag Whether the mode flag is set or not.
		 **/
		inline void								SetModeFlag( bool _bFlag );

		/**
		 * Determines if the noise channel should produce sound.
		 * 
		 * \param _bEnabled The status of the noise channel.
		 * \return Returns true if the channel should produce audio.
		 **/
		inline bool								ProducingSound( bool _bEnabled ) const;

	protected :
		// == Members.
		/** The mode flag. */
		bool									m_bMode				= false;
		


		// == Functions.
		/**
		 * Handles the tick work.
		 * 
		 * \return Returns an output value.
		 **/
		virtual uint8_t							WeDoBeTicknTho();

		/**
		 * Returns the condition for ticking the sequencer.  The timer clocks the shift register even while the channel is disabled
		 *	or its length counter is 0; those only silence the output.
		 * 
		 * \param _bEnabled Whether the unit is enabled for not.
		 * \return Return true to perform a sequencer tick, or false to skip sequencer work.
		 **/
		virtual bool							ShouldBeTicknTho( bool /*_bEnabled*/ ) { return true; }

	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Sets the mode flag.
	 * 
	 * \param _bFlag Whether the mode flag is set or not.
	 **/
	inline void CNoise::SetModeFlag( bool _bFlag ) {
		m_bMode = _bFlag != false;
	}

	/**
	 * Determines if the noise channel should produce sound.
	 * 
	 * \param _bEnabled The status of the noise channel.
	 * \return Returns true if the channel should produce audio.
	 **/
	inline bool CNoise::ProducingSound( bool _bEnabled ) const {
		return _bEnabled &&
			GetLengthCounter() > 0 &&
			Output();
	}

}	// namespace lsn

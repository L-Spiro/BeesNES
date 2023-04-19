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
#include "LSNSequencer.h"


namespace lsn {

	/**
	 * Class CPulse
	 * \brief An APU pulse.  Generates square waves.
	 *
	 * Description: An APU pulse.  Generates square waves.
	 */
	class CPulse : public CSequencer, public CApuUnit {
	public :
		CPulse();
		virtual ~CPulse();


	protected :
		// == Functions.
		/**
		 * Handles the tick work.
		 * 
		 * \param _ui32S The current sequence bits.
		 **/
		virtual void							WeDoBeTicknTho( uint32_t &_ui32S ) {
			(*reinterpret_cast<uint8_t *>(&_ui32S)) = ::_rotr8( (*reinterpret_cast<uint8_t *>(&_ui32S)), 1 );
		}

	};

}	// namespace lsn

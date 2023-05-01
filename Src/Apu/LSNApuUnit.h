/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The primary base class for each APU unit (pulse, noise, etc.)
 */


#pragma once

#include "../LSNLSpiroNes.h"


namespace lsn {

	/**
	 * Class CApuUnit
	 * \brief The primary base class for each APU unit (pulse, noise, etc.)
	 *
	 * Description: The primary base class for each APU unit (pulse, noise, etc.)
	 */
	class CApuUnit {
	public :
		CApuUnit() {
		}
		virtual ~CApuUnit() {
		}


		// == Functions.
		/**
		 * Gets a value in the length table.
		 * 
		 * \param _ui8Idx The index of the value from the length table to get.
		 * \return Returns the value at the given index in the length table.
		 **/
		static inline uint8_t				LenTable( uint8_t _ui8Idx );


	protected :
		// == Members.
		/** The length table. */
		static uint8_t						m_ui8LenTable[32];
	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets a value in the length table.
	 * 
	 * \param _ui8Idx The index of the value from the length table to get.
	 * \return Returns the value at the given index in the length table.
	 **/
	inline uint8_t CApuUnit::LenTable( uint8_t _ui8Idx ) { return m_ui8LenTable[_ui8Idx]; }

}	// namespace lsn

/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for all PPU's.
 */


#pragma once


#include "../LSNLSpiroNes.h"
#include "../Bus/LSNBus.h"

namespace lsn {

	/**
	 * Class CPpuBase
	 * \brief The base class for all CPU's.
	 *
	 * Description: The base class for all CPU's.
	 */
	class CPpuBase {
	public :
		CPpuBase( CCpuBus * _pbBus ) :
			m_ui64Cycle( 0 ),
			m_pbBus( _pbBus ) {
		}


		// == Functions.
		

		/**
		 * Gets the cycle count.
		 *
		 * \return Returns the cycle count.
		 */
		inline uint64_t									GetCycleCount() const { return m_ui64Cycle; }


	protected :
		// == Members.
		uint64_t										m_ui64Cycle;									/**< The total PPU cycles that have ticked. */
		CCpuBus *										m_pbBus;										/**< Pointer to the bus. */
	};

}	// namespace lsn

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

	// == Enumerations.
	/** APU hardware types. */
	enum LSN_APU_TYPE {
		LSN_AT_NTSC										= 0,								/**< NTSC. */
		LSN_AT_PAL										= 1,								/**< PAL. */
		LSN_AT_TOTAL,
	};

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

		/**
		 * Gets a value from the noise period table.
		 * 
		 * \param _tTable The system type.  One of the LSN_APU_TYPE values.
		 * \param _ui8Idx The index in the table for which to get the period.
		 * \return Returns the associated noise period.
		 **/
		template <unsigned _tTable>
		static inline uint16_t				NoiseTable( uint8_t _ui8Idx ) {
			return m_ui16NoisePeriods[_tTable][_ui8Idx];
		}


	protected :
		// == Members.
		/** The length table. */
		static uint8_t						m_ui8LenTable[32];

		/** The noise periods for NTSC and PAL. */
		static uint16_t						m_ui16NoisePeriods[LSN_AT_TOTAL][0x10];
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

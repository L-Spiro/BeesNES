/**
 * Copyright L. Spiro 2021
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The glue between each component of the system. The bus allows components to talk to
 *	each other and keeps track of floating values for the emulation of an open bus.
 * All memory accesses that would go across a real NES bus go across this. This means components
 *	with internal RAM can still manage their own RAM internally however they please.
 *
 * Ideally we would organize things so that components could "connect" to it and reserve address
 *	ranges for themselves etc., really keeping things nice and modular, but emulating real cycles
 *	will be taxing enough that anything else that can be optimized must be optimized, so this
 *	class hardcodes what addresses connect to what etc. and the logic behind the floating value.
 *
 * An outward-facing design decision is to have the entire block of system RAM contiguous in memory
 *	here to make it easier to parse by external readers (IE an external debugger).
 *
 * Since the memory is contiguous and directly part of this class, allocating this on the stack
 *	may cause a stack overflow.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Cpu/LSNCpu6502.h"

namespace lsn {

	/**
	 * Class CBus
	 * \brief The glue between each component of the system.
	 *
	 * Description: The glue between each component of the system. The bus allows components to talk to
	 *	each other and keeps track of floating values for the emulation of an "open" bus.
	 * All memory accesses that would go across a real NES bus go across this. This means components
	 *	with internal RAM can still manage their own RAM internally however they please.
	 *
	 * Ideally we would organize things so that components could "connect" to it and reserve address
	 *	ranges for themselves etc., really keeping things nice and modular, but emulating real cycles
	 *	will be taxing enough that anything else that can be optimized must be optimized, so this
	 *	class hardcodes what addresses connect to what etc. and the logic behind the floating value.
	 *
	 * An outward-facing design decision is to have the entire block of system RAM contiguous in memory
	 *	here to make it easier to parse by external readers (IE an external debugger).
	 *
	 * Since the memory is contiguous and directly part of this class, allocating this on the stack
	 *	may cause a stack overflow.
	 */
	class CBus {
	public :
		// == Various constructors.
		CBus();
		~CBus();


		// == Functions.
		/**
		 * Resets the bus to a known state.
		 */
		void								ResetToKnown();

		/**
		 * Performs an "analog" reset, allowing previous data to remain.
		 */
		void								ResetAnalog();

		/**
		 * Performs a CPU read of a given address.
		 *
		 * \param _ui16Addr The address to read.
		 * \return Returns the requested value.
		 */
		inline uint8_t						CpuRead( uint16_t _ui16Addr );

		/**
		 * Performs a CPU write of a given address.
		 *
		 * \param _ui16Addr The address to write.
		 * \param _ui8Val The value to write.
		 */
		inline void							CpuWrite( uint16_t _ui16Addr, uint8_t _ui8Val );

		/**
		 * Special-case function to set the floating value on the bus.  Only used by JAM instructions or in very rare specialized cases.
		 *
		 * \param _ui8Val The value to write.
		 */
		inline uint8_t						SetFloat( uint8_t _ui8Val );


	protected :
		// == Members.
		uint8_t								m_ui8Ram[1024*64];				/**< System memory of 64 kilobytes. */
		uint8_t								m_ui8LastRead;					/**< The floating value. */
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Performs a CPU read of a given address.
	 *
	 * \param _ui16Addr The address to read.
	 * \return Returns the requested value.
	 */
	inline uint8_t CBus::CpuRead( uint16_t _ui16Addr ) {
		if ( _ui16Addr >= LSN_CPU_START && _ui16Addr < (LSN_CPU_START + LSN_CPU_FULL_SIZE) ) {
			//m_ui8LastRead = m_ui8Ram[((_ui16Addr-LSN_CPU_START)%LSN_INTERNAL_RAM)+LSN_CPU_START];
			m_ui8LastRead = m_ui8Ram[_ui16Addr&(LSN_INTERNAL_RAM-1)];
			return m_ui8LastRead;
		}
		return m_ui8LastRead;
	}

	/**
	 * Performs a CPU write of a given address.
	 *
	 * \param _ui16Addr The address to write.
	 * \param _ui8Val The value to write.
	 */
	inline void CBus::CpuWrite( uint16_t _ui16Addr, uint8_t _ui8Val ) {
		if ( _ui16Addr >= LSN_CPU_START && _ui16Addr < (LSN_CPU_START + LSN_CPU_FULL_SIZE) ) {
			//m_ui8Ram[((_ui16Addr-LSN_CPU_START)%LSN_INTERNAL_RAM)+LSN_CPU_START] = _ui8Val;
			m_ui8Ram[_ui16Addr&(LSN_INTERNAL_RAM-1)] = _ui8Val;
			return;
		}
	}

	/**
	 * Special-case function to set the floating value on the bus.  Only used by JAM instructions or in very rare specialized cases.
	 *
	 * \param _ui8Val The value to write.
	 */
	inline uint8_t CBus::SetFloat( uint8_t _ui8Val ) {
		m_ui8LastRead = _ui8Val;
		return m_ui8LastRead;
	}

}	// namespace lsn

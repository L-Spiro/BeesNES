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
 * We have to make this as fast as possible but there are a lot of quirks to accessing any given
 *	address, plus we want it to be extensible, able to handle the CPU bus and the PPU bus all in
 *	one class.  We can't afford a bunch of virtual function calls that could allow plug-&-play
 *	behavior where each component gets to customize its RAM access, and we also want to avoid too
 *	many branches, modulo operations (mirroring), and other heavy logic required to hardcode the
 *	whole system into a tangled mess.
 * We achieve per-address programmability with minimal branching and outer logic by storing an
 *	array of pointers to functions that each handle the access logic for that address.  One pointer
 *	per address.  The outer logic is branchless, simply calling the function associated with that
 *	address.  The functions themselves already know how to access the address to which they are
 *	assigned and can perform the minimal amount of processing necessary to access any specific
 *	address.  This means there will not be several if/else statements to check address ranges each
 *	time an example address is read or written plus modulo operations to handle mirroring, there
 *	will only be the minimal access code related to any specific address, whether that be mirroring
 *	or any other special-case operation for a given address.  This solves the performance issue
 *	along with the hard-to-follow logic issue.  During setup, the hardware components get to assign
 *	these accessors to addresses as they please, solving the extensibility/flexibility issues that
 *	would have been served by virtual functions.  Ultiately, memory access can be made into an
 *	entirely branchless system.
 *
 * An outward-facing design decision is to have the entire block of system RAM contiguous in memory
 *	here to make it easier to parse by external readers (IE an external debugger).
 *
 * Since the memory is contiguous and directly part of this class, allocating this on the stack
 *	may cause a stack overflow.
 */


#pragma once

#include "../LSNLSpiroNes.h"

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
	 * We have to make this as fast as possible but there are a lot of quirks to accessing any given
	 *	address, plus we want it to be extensible, able to handle the CPU bus and the PPU bus all in
	 *	one class.  We can't afford a bunch of virtual function calls that could allow plug-&-play
	 *	behavior where each component gets to customize its RAM access, and we also want to avoid too
	 *	many branches, modulo operations (mirroring), and other heavy logic required to hardcode the
	 *	whole system into a tangled mess.
	 * We achieve per-address programmability with minimal branching and outer logic by storing an
	 *	array of pointers to functions that each handle the access logic for that address.  One pointer
	 *	per address.  The outer logic is branchless, simply calling the function associated with that
	 *	address.  The functions themselves already know how to access the address to which they are
	 *	assigned and can perform the minimal amount of processing necessary to access any specific
	 *	address.  This means there will not be several if/else statements to check address ranges each
	 *	time an example address is read or written plus modulo operations to handle mirroring, there
	 *	will only be the minimal access code related to any specific address, whether that be mirroring
	 *	or any other special-case operation for a given address.  This solves the performance issue
	 *	along with the hard-to-follow logic issue.  During setup, the hardware components get to assign
	 *	these accessors to addresses as they please, solving the extensibility/flexibility issues that
	 *	would have been served by virtual functions.  Ultiately, memory access can be made into an
	 *	entirely branchless system.
	 *
	 * An outward-facing design decision is to have the entire block of system RAM contiguous in memory
	 *	here to make it easier to parse by external readers (IE an external debugger).
	 *
	 * Since the memory is contiguous and directly part of this class, allocating this on the stack
	 *	may cause a stack overflow.
	 */
	template <unsigned _uSize>
	class CBus {
	public :
		// == Various constructors.
		CBus() :
			m_ui8LastRead( 0 ) {
		}
		~CBus() {
			ResetToKnown();
		}


		// == Functions.
		/**
		 * Resets the bus to a known state.
		 */
		void								ResetToKnown() {
			std::memset( m_ui8Ram, 0, sizeof( m_ui8Ram ) );
			m_ui8LastRead = 0;
		}

		/**
		 * Performs an "analog" reset, allowing previous data to remain.
		 */
		void								ResetAnalog() {
		}

		/**
		 * Performs a CPU read of a given address.
		 *
		 * \param _ui16Addr The address to read.
		 * \return Returns the requested value.
		 */
		inline uint8_t						CpuRead( uint16_t _ui16Addr ) {
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
		inline void							CpuWrite( uint16_t _ui16Addr, uint8_t _ui8Val ) {
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
		inline uint8_t						SetFloat( uint8_t _ui8Val ) {
			m_ui8LastRead = _ui8Val;
			return m_ui8LastRead;
		}


	protected :
		// == Members.
		uint8_t								m_ui8Ram[_uSize];				/**< Memory of _uSize bytes. */
		uint8_t								m_ui8LastRead;					/**< The floating value. */
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Types.
	/** The system-wide CPU bus. */
	typedef CBus<LSN_MEM_FULL_SIZE>			CCpuBus;

	/** The PPU bus. */
	typedef CBus<LSN_PPU_MEM_FULL_SIZE>		CPpuBus;

}	// namespace lsn

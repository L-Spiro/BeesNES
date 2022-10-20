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
			//std::memset( m_aaAccessors, 0, sizeof( m_aaAccessors ) );
			for ( auto I = Size(); I--; ) {
				SetReadFunc( uint16_t( I ), StdRead, nullptr, uint16_t( I ) );
				SetWriteFunc( uint16_t( I ), StdWrite, nullptr, uint16_t( I ) );
			}
		}
		~CBus() {
			ResetToKnown();
		}


		// == Types.
		/** An address-reading function. */
		typedef void (LSN_FASTCALL *		PfReadFunc)( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t &_ui8Ret );

		/** An address-reading function. */
		typedef void (LSN_FASTCALL *		PfWriteFunc)( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t _ui8Val );


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
		inline uint8_t						Read( uint16_t _ui16Addr ) {
			m_aaAccessors[_ui16Addr].pfReader( m_aaAccessors[_ui16Addr].pvReaderParm0,
				m_aaAccessors[_ui16Addr].ui16ReaderParm1,
				m_ui8Ram, m_ui8LastRead );
			return m_ui8LastRead;
			/*if ( _ui16Addr >= LSN_CPU_START && _ui16Addr < (LSN_CPU_START + LSN_CPU_FULL_SIZE) ) {
				//m_ui8LastRead = m_ui8Ram[((_ui16Addr-LSN_CPU_START)%LSN_INTERNAL_RAM)+LSN_CPU_START];
				m_ui8LastRead = m_ui8Ram[_ui16Addr&(LSN_INTERNAL_RAM-1)];
				return m_ui8LastRead;
			}
			return m_ui8LastRead;*/
		}

		/**
		 * Performs a CPU write of a given address.
		 *
		 * \param _ui16Addr The address to write.
		 * \param _ui8Val The value to write.
		 */
		inline void							Write( uint16_t _ui16Addr, uint8_t _ui8Val ) {
			m_aaAccessors[_ui16Addr].pfWriter( m_aaAccessors[_ui16Addr].pvWriterParm0,
				m_aaAccessors[_ui16Addr].ui16WriterParm1,
				m_ui8Ram, _ui8Val );
			/*if ( _ui16Addr >= LSN_CPU_START && _ui16Addr < (LSN_CPU_START + LSN_CPU_FULL_SIZE) ) {
				//m_ui8Ram[((_ui16Addr-LSN_CPU_START)%LSN_INTERNAL_RAM)+LSN_CPU_START] = _ui8Val;
				m_ui8Ram[_ui16Addr&(LSN_INTERNAL_RAM-1)] = _ui8Val;
				return;
			}*/
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

		/**
		 * Gets the size of the bus in bytes.
		 *
		 * \return Returns the size of the bus in bytes.
		 */
		inline constexpr uint32_t			Size() const { return _uSize; }

		/**
		 * Sets the read function for a given address.
		 *
		 * \param _ui16Address The address to assign the read function.
		 * \param _pfReadFunc The function to assign to the address.
		 * \param _pvParm0 A data value assigned to the address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to the address.
		 */
		void								SetReadFunc( uint16_t _ui16Address, PfReadFunc _pfReadFunc, void * _pvParm0, uint16_t _ui16Parm1 ) {
			if ( _ui16Parm1 < Size() ) {
				m_aaAccessors[_ui16Address].pfReader = _pfReadFunc;
				m_aaAccessors[_ui16Address].pvReaderParm0 = _pvParm0;
				m_aaAccessors[_ui16Address].ui16ReaderParm1 = _ui16Parm1;
			}
		}

		/**
		 * Sets the write function for a given address.
		 *
		 * \param _ui16Address The address to assign the write function.
		 * \param _pfWriteFunc The function to assign to the address.
		 * \param _pvParm0 A data value assigned to the address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to the address.
		 */
		void								SetWriteFunc( uint16_t _ui16Address, PfWriteFunc _pfWriteFunc, void * _pvParm0, uint16_t _ui16Parm1 ) {
			if ( _ui16Parm1 < Size() ) {
				m_aaAccessors[_ui16Address].pfWriter = _pfWriteFunc;
				m_aaAccessors[_ui16Address].pvWriterParm0 = _pvParm0;
				m_aaAccessors[_ui16Address].ui16WriterParm1 = _ui16Parm1;
			}
		}

		/**
		 * Copy data to the bus.
		 *
		 * \param _pui8Data The data to copy.
		 * \param _ui32Size The size of the data to which _pui8Data points.
		 * \param _ui16Address The address to which to copy the data.
		 */
		inline void						CopyToMemory( const uint8_t * _pui8Data, uint32_t _ui32Size, uint16_t _ui16Address ) {
			if ( _ui16Address < Size() ) {
				size_t stEnd = _ui32Size + size_t( _ui16Address );
				if ( stEnd > Size() ) { stEnd = Size(); }
				std::memcpy( &m_ui8Ram[_ui16Address], _pui8Data, stEnd - _ui16Address );
			}
		}

		/**
		 * A standard read function.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 * \return Returns true if the operation succeeds.  Return false to have the bus return the floating value.
		 */
		static void LSN_FASTCALL			StdRead( void * /*_pvParm0*/, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t &_ui8Ret ) {
			_ui8Ret = _pui8Data[_ui16Parm1];
		}

		/**
		 * A standard write function.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL			StdWrite( void * /*_pvParm0*/, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t _ui8Val ) {
			_pui8Data[_ui16Parm1] = _ui8Val;
		}

		/**
		 * A function usable for addresses that can't be read.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 * \return Returns true if the operation succeeds.  Return false to have the bus return the floating value.
		 */
		static void							NoRead( void * /*_pvParm0*/, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t &/*_ui8Ret*/ ) {
		}

		/**
		 * A function usable for addresses that can't be written.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The value to write.
		 */
		static void							NoWrite( void * /*_pvParm0*/, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t /*_ui8Val*/ ) {
		}



	protected :
		// == Types.
		/** An address accessor. */
		struct LSN_ADDR_ACCESSOR {
			PfReadFunc						pfReader;						/**< The function for reading the assigned address. */
			void *							pvReaderParm0;					/**< The reader's first parameter. */
			PfWriteFunc						pfWriter;						/**< The function for writing the assigned address. */
			void *							pvWriterParm0;					/**< The writer's first parameter. */
			uint16_t						ui16ReaderParm1;				/**< The reader's second parameter. */
			uint16_t						ui16WriterParm1;				/**< The writer's second parameter. */
		};


		// == Members.
		LSN_ADDR_ACCESSOR					m_aaAccessors[_uSize];			/**< Access functions. */
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

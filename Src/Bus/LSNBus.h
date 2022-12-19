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


		// == Types.
		/** An address-reading function. */
		typedef void (LSN_FASTCALL *		PfReadFunc)( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t &_ui8Ret );

		/** An address-reading function. */
		typedef void (LSN_FASTCALL *		PfWriteFunc)( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t _ui8Val );

		/** An address accessor. */
		struct LSN_ADDR_ACCESSOR {
			PfReadFunc						pfReader;						/**< The function for reading the assigned address. */
			void *							pvReaderParm0;					/**< The reader's first parameter. */
			PfWriteFunc						pfWriter;						/**< The function for writing the assigned address. */
			void *							pvWriterParm0;					/**< The writer's first parameter. */
			uint16_t						ui16ReaderParm1;				/**< The reader's second parameter. */
			uint16_t						ui16WriterParm1;				/**< The writer's second parameter. */
		};

		/** A trampoline is a read/write function that has been inserted to perform its own operation at a given address and then optionally call the original read/write function for that address. */
		struct LSN_TRAMPOLINE {
			void *							pvReaderParm0;					/**< The trampoline reader's first parameter. */
			void *							pvWriterParm0;					/**< The trampoline writer's first parameter. */
			LSN_ADDR_ACCESSOR				aaOriginalFuncs;				/**< The original read/write functions and parameters so that the trampoline can call them if needed. */
		};


		// == Functions.
		/**
		 * Resets the bus to a known state.
		 */
		void								ResetToKnown() {
			ResetAnalog();
			std::memset( m_ui8Ram, 0, sizeof( m_ui8Ram ) );
			m_ui8LastRead = 0;
		}

		/**
		 * Performs an "analog" reset, allowing previous data to remain.
		 */
		void								ResetAnalog() {
			for ( auto I = Size(); I--; ) {
				SetReadFunc( uint16_t( I ), StdRead, nullptr, uint16_t( I ) );
				SetWriteFunc( uint16_t( I ), StdWrite, nullptr, uint16_t( I ) );
			}
		}

		/**
		 * Performs a CPU read of a given address.
		 *
		 * \param _ui16Addr The address to read.
		 * \return Returns the requested value.
		 */
		inline uint8_t						Read( uint16_t _ui16Addr ) {
			LSN_ADDR_ACCESSOR & aaAcc = m_aaAccessors[_ui16Addr];
			aaAcc.pfReader( aaAcc.pvReaderParm0,
				aaAcc.ui16ReaderParm1,
				m_ui8Ram, m_ui8LastRead );
			return m_ui8LastRead;
		}

		/**
		 * Performs a CPU write of a given address.
		 *
		 * \param _ui16Addr The address to write.
		 * \param _ui8Val The value to write.
		 */
		inline void							Write( uint16_t _ui16Addr, uint8_t _ui8Val ) {
			LSN_ADDR_ACCESSOR & aaAcc = m_aaAccessors[_ui16Addr];
			aaAcc.pfWriter( aaAcc.pvWriterParm0,
				aaAcc.ui16WriterParm1,
				m_ui8Ram, _ui8Val );
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
		 * Gets the floating value on the bus.
		 *
		 * \return Returns the floating value on the bus.
		 */
		inline uint8_t						GetFloat() const { return m_ui8LastRead; }

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
		 * Inserts a read trampoline function for a given address.  The read function's _pvParm0 value will be a pointer to the trampoline.  reinterpret_cast<LSN_TRAMPOLINE *>(_pvParm0)->pvReaderParm0 holds the "_pvParm0" value passed here.
		 *
		 * \param _ui16Address The address to assign the read function.
		 * \param _pfReadFunc The function to assign to the address.
		 * \param _pvParm0 A data value assigned to the address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to the address.
		 * \param _ptTrampoline A pointer to receive the trampoline's "original" data.
		 */
		void								SetTrampolineReadFunc( uint16_t _ui16Address, PfReadFunc _pfReadFunc, void * _pvParm0, uint16_t _ui16Parm1, LSN_TRAMPOLINE * _ptTrampoline ) {
			if ( _ui16Parm1 < Size() && _ptTrampoline ) {
				_ptTrampoline->pvReaderParm0 = _pvParm0;
				_ptTrampoline->aaOriginalFuncs.pfReader = m_aaAccessors[_ui16Address].pfReader;
				_ptTrampoline->aaOriginalFuncs.pvReaderParm0 = m_aaAccessors[_ui16Address].pvReaderParm0;
				_ptTrampoline->aaOriginalFuncs.ui16ReaderParm1 = m_aaAccessors[_ui16Address].ui16ReaderParm1;
				m_aaAccessors[_ui16Address].pfReader = _pfReadFunc;
				m_aaAccessors[_ui16Address].pvReaderParm0 = _ptTrampoline;
				m_aaAccessors[_ui16Address].ui16ReaderParm1 = _ui16Parm1;
			}
		}

		/**
		 * Inserts a write trampoline function for a given address.  The write function's _pvParm0 value will be a pointer to the trampoline.  reinterpret_cast<LSN_TRAMPOLINE *>(_pvParm0)->pvWriterParm0 holds the "_pvParm0" value passed here.
		 *
		 * \param _ui16Address The address to assign the write function.
		 * \param _pfWriteFunc The function to assign to the address.
		 * \param _pvParm0 A data value assigned to the address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to the address.
		 * \param _ptTrampoline A pointer to receive the trampoline's "original" data.
		 */
		void								SetTrampolineWriteFunc( uint16_t _ui16Address, PfWriteFunc _pfWriteFunc, void * _pvParm0, uint16_t _ui16Parm1, LSN_TRAMPOLINE * _ptTrampoline ) {
			if ( _ui16Parm1 < Size() && _ptTrampoline ) {
				_ptTrampoline->pvWriterParm0 = _pvParm0;
				_ptTrampoline->aaOriginalFuncs.pfWriter = m_aaAccessors[_ui16Address].pfWriter;
				_ptTrampoline->aaOriginalFuncs.pvWriterParm0 = m_aaAccessors[_ui16Address].pvWriterParm0;
				_ptTrampoline->aaOriginalFuncs.ui16WriterParm1 = m_aaAccessors[_ui16Address].ui16WriterParm1;
				m_aaAccessors[_ui16Address].pfWriter = _pfWriteFunc;
				m_aaAccessors[_ui16Address].pvWriterParm0 = _ptTrampoline;
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
		inline void							CopyToMemory( const uint8_t * _pui8Data, uint32_t _ui32Size, uint16_t _ui16Address ) {
			if ( _ui16Address < Size() ) {
				size_t stEnd = _ui32Size + size_t( _ui16Address );
				if ( stEnd > Size() ) { stEnd = Size(); }
				std::memcpy( &m_ui8Ram[_ui16Address], _pui8Data, stEnd - _ui16Address );
			}
		}

		/**
		 * Inspect a RAM location for debug purposes.
		 *
		 * \param _ui16Address The address to read.
		 * \return Returns the value at the given address, bypassing normal bus access routines.
		 */
		inline uint8_t						DBG_Inspect( uint16_t _ui16Address ) {
			return m_ui8Ram[_ui16Address];
		}

		/**
		 * A standard read function.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL			StdRead( void * /*_pvParm0*/, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t &_ui8Ret ) {
			_ui8Ret = _pui8Data[_ui16Parm1];
		}

		/**
		 * A standard write function.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
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
		 */
		static void LSN_FASTCALL			NoRead( void * /*_pvParm0*/, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t &/*_ui8Ret*/ ) {
		}

		/**
		 * A function usable for addresses that can't be written.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL			NoWrite( void * /*_pvParm0*/, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t /*_ui8Val*/ ) {
		}



	protected :
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

/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 009 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper009
	 * \brief Mapper 009 implementation.
	 *
	 * Description: Mapper 009 implementation.
	 */
	class CMapper009 : public CMapperBase {
	public :
		CMapper009();
		virtual ~CMapper009();


		// == Functions.
		/**
		 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
		 *
		 * \param _rRom The ROM data.
		 */
		virtual void									InitWithRom( LSN_ROM &_rRom );

		/**
		 * Applies mapping to the CPU and PPU busses.
		 *
		 * \param _pbCpuBus A pointer to the CPU bus.
		 * \param _pbPpuBus A pointer to the PPU bus.
		 */
		virtual void									ApplyMap( CCpuBus * _pbCpuBus, CPpuBus * _pbPpuBus );


	protected :
		// == Members.
		/** Latch-0 trampolines at 0x0FD8 and 0x0FE8. */
		CPpuBus::LSN_TRAMPOLINE							m_tLatch0Trampoline[2];
		/** Latch-1 trampoline from 0x1FD8 to 0x1FDF. */
		CPpuBus::LSN_TRAMPOLINE							m_tLatch1Trampoline_1FD8_1FDF[0x1FDF-0x1FD8+1];
		/** Latch-1 trampoline from 0x1FE8 to 0x1FEF. */
		CPpuBus::LSN_TRAMPOLINE							m_tLatch1Trampoline_1FE8_1FEF[0x1FEF-0x1FE8+1];
		/** RAM. */
		uint8_t											m_ui8PgmRam[8*1024];
		/** The 0000 CHR ROM bank used when m_ui8Latch0 == 0xFD. */
		uint8_t											m_ui8ChrBankLatch0_FD;
		/** The 0000 CHR ROM bank used when m_ui8Latch0 == 0xFE. */
		uint8_t											m_ui8ChrBankLatch0_FE;
		/** The 1000 CHR ROM bank used when m_ui8Latch1 == 0xFD. */
		uint8_t											m_ui8ChrBankLatch1_FD;
		/** The 1000 CHR ROM bank used when m_ui8Latch1 == 0xFE. */
		uint8_t											m_ui8ChrBankLatch1_FE;
		/** Latch 0. */
		uint8_t											m_ui8Latch0;
		/** Latch 1. */
		uint8_t											m_ui8Latch1;


		// == Functions.
		/**
		 * Reads from the PGM RAM.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Mapper009PgmRamRead( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret );

		/**
		 * Writes to the PGM RAM.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						Mapper009PgmRamWrite( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val );

		/**
		 * PPU $0000-$0FFF: Two 4 KB switchable CHR ROM banks.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Mapper009ChrBankRead_0000_0FFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret );

		/**
		 * PPU $1000-$1FFF: Two 4 KB switchable CHR ROM banks.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Mapper009ChrBankRead_1000_1FFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret );

		/**
		 * PRG ROM bank select ($A000-$AFFF).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						SelectBankA000_AFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val );

		/**
		 * CHR ROM $FD/0000 bank select ($B000-$BFFF).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						SelectBankB000_BFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val );

		/**
		 * CHR ROM $FE/0000 bank select ($C000-$CFFF).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						SelectBankC000_CFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val );

		/**
		 * CHR ROM $FD/1000 bank select ($D000-$DFFF).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						SelectBankD000_DFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val );

		/**
		 * CHR ROM $FE/1000 bank select ($E000-$EFFF).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						SelectBankE000_EFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val );

		/**
		 * Mirroring ($F000-$FFFF).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						SetMirroringF000_FFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val );

		/**
		 * PPU reads $0FD8: latch 0 is set to $FD for subsequent reads.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						SetLatch_0FD8_Trampoline( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t &_ui8Ret );

		/**
		 * PPU reads $0FE8: latch 0 is set to $FE for subsequent reads.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						SetLatch_0FE8_Trampoline( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t &_ui8Ret );

		/**
		 * PPU reads $1FD8 through $1FDF: latch 1 is set to $FD for subsequent reads.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						SetLatch_1FD8_1FDF_Trampoline( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t &_ui8Ret );

		/**
		 * PPU reads $1FE8 through $1FEF: latch 1 is set to $FE for subsequent reads.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						SetLatch_1FE8_1FEF_Trampoline( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t &_ui8Ret );

	};

}	// namespace lsn

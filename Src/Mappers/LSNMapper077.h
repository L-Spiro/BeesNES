/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 077 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper077
	 * \brief Mapper 077 implementation.
	 *
	 * Description: Mapper 077 implementation.
	 */
	class CMapper077 : public CMapperBase {
	public :
		CMapper077() {
		}
		virtual ~CMapper077() {
		}


		// == Functions.
		/**
		 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
		 *
		 * \param _rRom The ROM data.
		 */
		virtual void									InitWithRom( LSN_ROM &_rRom ) {
			CMapperBase::InitWithRom( _rRom );
			SanitizeRegs<32 * 1024, 2 * 1024>();
		}

		/**
		 * Applies mapping to the CPU and PPU busses.
		 *
		 * \param _pbCpuBus A pointer to the CPU bus.
		 * \param _pbPpuBus A pointer to the PPU bus.
		 */
		virtual void									ApplyMap( CCpuBus * _pbCpuBus, CPpuBus * _pbPpuBus ) {
			// ================
			// SWAPPABLE BANKS
			// ================
			// CPU.
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead_8000, this, uint16_t( I - 0x8000 ) );
			}
			// PPU.
			for ( uint32_t I = 0x0000; I < 0x0800; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead_0800, this, uint16_t( I - 0x0000 ) );
			}
			// RAM.
			for ( uint32_t I = 0x0800; I < LSN_PPU_PALETTE_MEMORY; ++I ) {
				uint16_t ui16Final = uint16_t( I );
				if ( ui16Final >= 0x3000 ) {
					ui16Final = ((ui16Final - 0x3000) & 0x7FF) + 0x3000;
				}
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper077::ChrRamRead, this, uint16_t( ui16Final - 0x0800 ) );
				_pbPpuBus->SetWriteFunc( uint16_t( I ), &CMapper077::ChrRamWrite, this, uint16_t( ui16Final - 0x0800 ) );
			}


			// ================
			// BANK-SELECT
			// ================
			// PGM bank-select.
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper077::SelectBank8000_FFFF, this, 0 );	// Treated as ROM.
			}
		}


	protected :
		// == Members.
		/** The RAM. Mapper has 8 kilobytes, but we consume some of the PPU’s VRAM for implementation convenience. */
		uint8_t											m_ui8ChrRam[10*1024];


		// == Functions.
		/**
		 * Selects a bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						SelectBank8000_FFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper077 * pmThis = reinterpret_cast<CMapper077 *>(_pvParm0);
			pmThis->SetPgmBank<0, 32 * 1024>( _ui8Val & 0b1111 );
			pmThis->SetChrBank<0, 2 * 1024>( _ui8Val >> 4 );
		}

		/**
		 * Reads from the CHR RAM.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						ChrRamRead( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper077 * pmThis = reinterpret_cast<CMapper077 *>(_pvParm0);
			_ui8Ret = pmThis->m_ui8ChrRam[_ui16Parm1];
		}

		/**
		 * Writes to the CHR RAM.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						ChrRamWrite( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper077 * pmThis = reinterpret_cast<CMapper077 *>(_pvParm0);
			pmThis->m_ui8ChrRam[_ui16Parm1] = _ui8Val;
		}
	};

}	// namespace lsn

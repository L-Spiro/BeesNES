/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 080 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper080
	 * \brief Mapper 080 implementation.
	 *
	 * Description: Mapper 080 implementation.
	 */
	class CMapper080 : public CMapperBase {
	public :
		CMapper080() {
		}
		virtual ~CMapper080() {
		}


		// == Functions.
		/**
		 * Gets the PGM bank size.
		 *
		 * \return Returns the size of the PGM banks.
		 */
		static constexpr uint16_t						PgmBankSize() { return 8 * 1024; }

		/**
		 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
		 *
		 * \param _rRom The ROM data.
		 * \param _pcbCpuBase A pointer to the CPU.
		 */
		virtual void									InitWithRom( LSN_ROM &_rRom, CCpuBase * _pcbCpuBase, CPpuBase * _ppbPpuBase, CInterruptable * _piInter, CBussable * _pbPpuBus ) {
			CMapperBase::InitWithRom( _rRom, _pcbCpuBase, _ppbPpuBase, _piInter, _pbPpuBus );
			SanitizeRegs<8 * 1024, 1 * 1024>();
			m_bRamAllowed = false;
		}

		/**
		 * Applies mapping to the CPU and PPU busses.
		 *
		 * \param _pbCpuBus A pointer to the CPU bus.
		 * \param _pbPpuBus A pointer to the PPU bus.
		 */
		virtual void									ApplyMap( CCpuBus * _pbCpuBus, CPpuBus * _pbPpuBus ) {
			CMapperBase::ApplyMap( _pbCpuBus, _pbPpuBus );

			// ================
			// FIXED BANKS
			// ================
			// Set the reads of the fixed bank at the end.
			m_stFixedOffset = std::max<size_t>( m_prRom->vPrgRom.size(), PgmBankSize() ) - PgmBankSize();
			for ( uint32_t I = 0xE000; I < 0x10000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead_Fixed, this, uint16_t( (I - 0xE000) % (m_prRom->vPrgRom.size() - m_stFixedOffset) ) );
			}
			

			// ================
			// SWAPPABLE BANKS
			// ================
			// CPU.
			for ( uint32_t I = 0x8000; I < 0xA000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<0, 8 * 1024>, this, uint16_t( I - 0x8000 ) );
			}
			for ( uint32_t I = 0xA000; I < 0xC000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<1, 8 * 1024>, this, uint16_t( I - 0xA000 ) );
			}
			for ( uint32_t I = 0xC000; I < 0xE000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<2, 8 * 1024>, this, uint16_t( I - 0xC000 ) );
			}
			// PPU.
			for ( uint32_t I = 0x0000; I < 0x0400; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<0, 1 * 1024>, this, uint16_t( I - 0x0000 ) );
			}
			for ( uint32_t I = 0x0400; I < 0x0800; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<1, 1 * 1024>, this, uint16_t( I - 0x0400 ) );
			}
			for ( uint32_t I = 0x0800; I < 0x0C00; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<2, 1 * 1024>, this, uint16_t( I - 0x0800 ) );
			}
			for ( uint32_t I = 0x0C00; I < 0x1000; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<3, 1 * 1024>, this, uint16_t( I - 0x0C00 ) );
			}
			for ( uint32_t I = 0x1000; I < 0x1400; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<4, 1 * 1024>, this, uint16_t( I - 0x1000 ) );
			}
			for ( uint32_t I = 0x1400; I < 0x1800; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<5, 1 * 1024>, this, uint16_t( I - 0x1400 ) );
			}
			for ( uint32_t I = 0x1800; I < 0x1C00; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<6, 1 * 1024>, this, uint16_t( I - 0x1800 ) );
			}
			for ( uint32_t I = 0x1C00; I < 0x2000; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<7, 1 * 1024>, this, uint16_t( I - 0x1C00 ) );
			}
			// RAM.
			for ( uint32_t I = 0x7F00; I < 0x8000; ++I ) {
				uint16_t ui16Final = uint16_t( I - 0x7F00 );
				ui16Final %= 128;
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper080::ChrRamRead, this, ui16Final );
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper080::ChrRamWrite, this, ui16Final );
			}


			// ================
			// BANK-SELECT
			// ================
			// PGM bank-select.
			{
				_pbCpuBus->SetWriteFunc( 0x7EFA, &CMapper080::SelectBank7EFA_7EFB, this, 0 );	// Treated as ROM.
				_pbCpuBus->SetWriteFunc( 0x7EFB, &CMapper080::SelectBank7EFA_7EFB, this, 0 );	// Treated as ROM.
				_pbCpuBus->SetWriteFunc( 0x7EFC, &CMapper080::SelectBank7EFC_7EFD, this, 0 );	// Treated as ROM.
				_pbCpuBus->SetWriteFunc( 0x7EFD, &CMapper080::SelectBank7EFC_7EFD, this, 0 );	// Treated as ROM.
				_pbCpuBus->SetWriteFunc( 0x7EFE, &CMapper080::SelectBank7EFE_7EFF, this, 0 );	// Treated as ROM.
				_pbCpuBus->SetWriteFunc( 0x7EFF, &CMapper080::SelectBank7EFE_7EFF, this, 0 );	// Treated as ROM.
			}
			// CHR bank-select.
			{
				_pbCpuBus->SetWriteFunc( 0x7EF0, &CMapper080::SelectBank7EF0_7EF0, this, 0 );	// Treated as ROM.
				_pbCpuBus->SetWriteFunc( 0x7EF1, &CMapper080::SelectBank7EF1_7EF1, this, 0 );	// Treated as ROM.
				_pbCpuBus->SetWriteFunc( 0x7EF2, &CMapper080::SelectBank7EF2_7EF2, this, 0 );	// Treated as ROM.
				_pbCpuBus->SetWriteFunc( 0x7EF3, &CMapper080::SelectBank7EF3_7EF3, this, 0 );	// Treated as ROM.
				_pbCpuBus->SetWriteFunc( 0x7EF4, &CMapper080::SelectBank7EF4_7EF4, this, 0 );	// Treated as ROM.
				_pbCpuBus->SetWriteFunc( 0x7EF5, &CMapper080::SelectBank7EF5_7EF5, this, 0 );	// Treated as ROM.
			}
			// Mirroring select.
			{
				_pbCpuBus->SetWriteFunc( 0x7EF6, &CMapper080::SelectBank7EF6_7EF7, this, 0 );	// Treated as ROM.
				_pbCpuBus->SetWriteFunc( 0x7EF7, &CMapper080::SelectBank7EF6_7EF7, this, 0 );	// Treated as ROM.
			}
			// RAM access.
			{
				_pbCpuBus->SetWriteFunc( 0x7EF8, &CMapper080::SelectBank7EF8_7EF9, this, 0 );	// Treated as ROM.
				_pbCpuBus->SetWriteFunc( 0x7EF9, &CMapper080::SelectBank7EF8_7EF9, this, 0 );	// Treated as ROM.
			}


			// ================
			// MIRRORING
			// ================
			ApplyControllableMirrorMap( _pbPpuBus );
		}


	protected :
		// == Members.
		/** The RAM. */
		uint8_t											m_ui8ChrRam[128];
		/** Access to RAM. */
		bool											m_bRamAllowed;


		// == Functions.
		/**
		 * Selects CHR banks 0 and 1.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank7EF0_7EF0( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper080 * pmThis = reinterpret_cast<CMapper080 *>(_pvParm0);
			pmThis->SetChrBank<0, 1 * 1024>( _ui8Val );
			pmThis->SetChrBank<1, 1 * 1024>( _ui8Val + 1 );
		}

		/**
		 * Selects CHR banks 2 and 3.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank7EF1_7EF1( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper080 * pmThis = reinterpret_cast<CMapper080 *>(_pvParm0);
			pmThis->SetChrBank<2, 1 * 1024>( _ui8Val );
			pmThis->SetChrBank<3, 1 * 1024>( _ui8Val + 1 );
		}

		/**
		 * Selects CHR bank 4.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank7EF2_7EF2( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper080 * pmThis = reinterpret_cast<CMapper080 *>(_pvParm0);
			pmThis->SetChrBank<4, 1 * 1024>( _ui8Val );
		}

		/**
		 * Selects CHR bank 5.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank7EF3_7EF3( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper080 * pmThis = reinterpret_cast<CMapper080 *>(_pvParm0);
			pmThis->SetChrBank<5, 1 * 1024>( _ui8Val );
		}

		/**
		 * Selects CHR bank 6.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank7EF4_7EF4( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper080 * pmThis = reinterpret_cast<CMapper080 *>(_pvParm0);
			pmThis->SetChrBank<6, 1 * 1024>( _ui8Val );
		}

		/**
		 * Selects CHR bank 7.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank7EF5_7EF5( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper080 * pmThis = reinterpret_cast<CMapper080 *>(_pvParm0);
			pmThis->SetChrBank<7, 1 * 1024>( _ui8Val );
		}

		/**
		 * Selects mirroring.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank7EF6_7EF7( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper080 * pmThis = reinterpret_cast<CMapper080 *>(_pvParm0);
			switch ( _ui8Val & 1 ) {
				case 0 : {
					pmThis->m_mmMirror = LSN_MM_HORIZONTAL;
					break;
				}
				case 1 : {
					pmThis->m_mmMirror = LSN_MM_VERTICAL;
					break;
				}
			}
		}

		/**
		 * Selects RAM access.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank7EF8_7EF9( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper080 * pmThis = reinterpret_cast<CMapper080 *>(_pvParm0);
			pmThis->m_bRamAllowed = _ui8Val == 0xA3;
		}

		/**
		 * Selects PGM bank 0.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank7EFA_7EFB( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper080 * pmThis = reinterpret_cast<CMapper080 *>(_pvParm0);
			pmThis->SetPgmBank<0, 8 * 1024>( _ui8Val );
		}

		/**
		 * Selects PGM bank 1.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank7EFC_7EFD( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper080 * pmThis = reinterpret_cast<CMapper080 *>(_pvParm0);
			pmThis->SetPgmBank<1, 8 * 1024>( _ui8Val );
		}

		/**
		 * Selects PGM bank 2.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank7EFE_7EFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper080 * pmThis = reinterpret_cast<CMapper080 *>(_pvParm0);
			pmThis->SetPgmBank<2, 8 * 1024>( _ui8Val );
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
			CMapper080 * pmThis = reinterpret_cast<CMapper080 *>(_pvParm0);
			if ( pmThis->m_bRamAllowed ) {
				_ui8Ret = pmThis->m_ui8ChrRam[_ui16Parm1];
			}
		}

		/**
		 * Writes to the CHR RAM.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						ChrRamWrite( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper080 * pmThis = reinterpret_cast<CMapper080 *>(_pvParm0);
			if ( pmThis->m_bRamAllowed ) {
				pmThis->m_ui8ChrRam[_ui16Parm1] = _ui8Val;
			}
		}
	};

}	// namespace lsn

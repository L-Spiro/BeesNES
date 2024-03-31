/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 032 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper032
	 * \brief Mapper 032 implementation.
	 *
	 * Description: Mapper 032 implementation.
	 */
	class CMapper032 : public CMapperBase {
	public :
		CMapper032() {
		}
		virtual ~CMapper032() {
		}


		// == Functions.
		/**
		 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
		 *
		 * \param _rRom The ROM data.
		 * \param _pcbCpuBase A pointer to the CPU.
		 */
		virtual void									InitWithRom( LSN_ROM &_rRom, CCpuBase * _pcbCpuBase ) {
			CMapperBase::InitWithRom( _rRom, _pcbCpuBase );
			m_ui8PgmBank = 0;
			SanitizeRegs<0x2000, 0x0400>();
			SetPgmBank<1, 0x2000>( -2 );
			m_ui8Mode = 0;

			size_t stBanks = m_prRom->vPrgRom.size() / 0x2000;
			m_ui8Neg1Bank = uint8_t( stBanks - (-(-1) % stBanks) );
			m_ui8Neg2Bank = uint8_t( stBanks - (-(-2) % stBanks) );
		}

		/**
		 * Applies mapping to the CPU and PPU busses.
		 *
		 * \param _pbCpuBus A pointer to the CPU bus.
		 * \param _pbPpuBus A pointer to the PPU bus.
		 */
		virtual void									ApplyMap( CCpuBus * _pbCpuBus, CPpuBus * _pbPpuBus ) {
			CMapperBase::ApplyMap( _pbCpuBus, _pbPpuBus );

#define LSN_MAJOR_BALL_CRC								0xC0FED437
			// ================
			// FIXED BANKS
			// ================
			m_stFixedOffset = std::max<size_t>( m_prRom->vPrgRom.size(), 0x2000 ) - 0x2000;
			for ( uint32_t I = 0xE000; I < 0x10000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead_Fixed, this, uint16_t( I - 0xE000 ) );
			}

			 
			// ================
			// SWAPPABLE BANKS
			// ================
			// CPU.
			for ( uint32_t I = 0x8000; I < 0xA000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper032::PgmBankRead_8000_A000, this, uint16_t( I - 0x8000 ) );
			}
			for ( uint32_t I = 0xA000; I < 0xC000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper032::PgmBankRead_A000_C000, this, uint16_t( I - 0xA000 ) );
			}
			for ( uint32_t I = 0xC000; I < 0xE000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper032::PgmBankRead_C000_E000, this, uint16_t( I - 0xC000 ) );
			}
			// PPU.
			for ( uint32_t I = 0x0000; I < 0x0400; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<0, 0x0400>, this, uint16_t( I - 0x0000 ) );
			}
			for ( uint32_t I = 0x0400; I < 0x0800; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<1, 0x0400>, this, uint16_t( I - 0x0400 ) );
			}
			for ( uint32_t I = 0x0800; I < 0x0C00; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<2, 0x0400>, this, uint16_t( I - 0x0800 ) );
			}
			for ( uint32_t I = 0x0C00; I < 0x1000; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<3, 0x0400>, this, uint16_t( I - 0x0C00 ) );
			}
			for ( uint32_t I = 0x1000; I < 0x1400; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<4, 0x0400>, this, uint16_t( I - 0x1000 ) );
			}
			for ( uint32_t I = 0x1400; I < 0x1800; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<5, 0x0400>, this, uint16_t( I - 0x1400 ) );
			}
			for ( uint32_t I = 0x1800; I < 0x1C00; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<6, 0x0400>, this, uint16_t( I - 0x1800 ) );
			}
			for ( uint32_t I = 0x1C00; I < 0x2000; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<7, 0x0400>, this, uint16_t( I - 0x1C00 ) );
			}


			// ================
			// BANK-SELECT
			// ================
			// PGM bank-select 0.
			for ( uint32_t I = 0x8000; I <= 0x8007; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper032::SelectBank8000_8007, this, uint16_t( I - 0x8000 ) );	// Treated as ROM.
			}
			// PGM bank-select 1.
			for ( uint32_t I = 0xA000; I <= 0xA007; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper032::SelectBankA000_A007, this, uint16_t( I - 0xA000 ) );	// Treated as ROM.
			}
			// CHR bank-select.
			for ( uint32_t I = 0xB000; I <= 0xB007; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper032::SelectBankB000_B007, this, uint16_t( I - 0xB000 ) );
			}
			// Mode/mirroring.
			if ( m_prRom->riInfo.ui16SubMapper != 1 && m_prRom->riInfo.ui32Crc != LSN_MAJOR_BALL_CRC ) {
				for ( uint32_t I = 0x9000; I <= 0x9007; ++I ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper032::SelectModeEtc9000_9007, this, uint16_t( I - 0x9000 ) );
				}
			}


			// ================
			// MIRRORING
			// ================
			if ( m_prRom->riInfo.ui16SubMapper == 1 || m_prRom->riInfo.ui32Crc == LSN_MAJOR_BALL_CRC ) {
				ApplyMirroring( LSN_MM_1_SCREEN_A, _pbPpuBus, nullptr );
			}
			else {
				ApplyControllableMirrorMap( _pbPpuBus );
			}
#undef LSN_MAJOR_BALL_CRC
		}


	protected :
		// == Members.
		/** The PGM mode. */
		uint8_t											m_ui8Mode;
		/** The index of the -1 bank. */
		uint8_t											m_ui8Neg1Bank;
		/** The index of the -2 bank. */
		uint8_t											m_ui8Neg2Bank;

		// == Functions.
		/**
		 * PRG mode and mirroring.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectModeEtc9000_9007( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper032 * pmThis = reinterpret_cast<CMapper032 *>(_pvParm0);
			pmThis->m_ui8Mode = _ui8Val & 0b10;

			switch ( _ui8Val & 0b01 ) {
				case 0 : {
					pmThis->m_mmMirror = LSN_MM_VERTICAL;
					break;
				}
				case 1 : {
					pmThis->m_mmMirror = LSN_MM_HORIZONTAL;
					break;
				}
			}
		}

		/**
		 * PRG register 0.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank8000_8007( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper032 * pmThis = reinterpret_cast<CMapper032 *>(_pvParm0);
			pmThis->SetPgmBank<0, 0x2000>( _ui8Val & 0b11111 );
		}

		/**
		 * PRG register 1.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBankA000_A007( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper032 * pmThis = reinterpret_cast<CMapper032 *>(_pvParm0);
			pmThis->SetPgmBank<1, 0x2000>( _ui8Val & 0b11111 );
		}

		/**
		 * CHR register 0.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBankB000_B007( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper032 * pmThis = reinterpret_cast<CMapper032 *>(_pvParm0);
			pmThis->SetChrBank<0x0400>( _ui16Parm1 & 0x7, _ui8Val );
		}

		/**
		 * Reads from the first bank depending on the mode.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						PgmBankRead_8000_A000( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper032 * pmThis = reinterpret_cast<CMapper032 *>(_pvParm0);
			if ( !pmThis->m_ui8Mode ) {
				_ui8Ret = pmThis->m_prRom->vPrgRom.data()[size_t(_ui16Parm1)+(size_t(pmThis->m_ui8PgmBanks[0])*0x2000)];
			}
			else {
				_ui8Ret = pmThis->m_prRom->vPrgRom.data()[size_t(_ui16Parm1)+(size_t(pmThis->m_ui8Neg2Bank)*0x2000)];
			}
		}

		/**
		 * Reads from the second bank depending on the mode.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						PgmBankRead_A000_C000( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper032 * pmThis = reinterpret_cast<CMapper032 *>(_pvParm0);
			if ( !pmThis->m_ui8Mode ) {
				_ui8Ret = pmThis->m_prRom->vPrgRom.data()[size_t(_ui16Parm1)+(size_t(pmThis->m_ui8PgmBanks[1])*0x2000)];
			}
			else {
				_ui8Ret = pmThis->m_prRom->vPrgRom.data()[size_t(_ui16Parm1)+(size_t(pmThis->m_ui8PgmBanks[0])*0x2000)];
			}
		}

		/**
		 * Reads from the third bank depending on the mode.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						PgmBankRead_C000_E000( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper032 * pmThis = reinterpret_cast<CMapper032 *>(_pvParm0);
			if ( !pmThis->m_ui8Mode ) {
				_ui8Ret = pmThis->m_prRom->vPrgRom.data()[size_t(_ui16Parm1)+(size_t(pmThis->m_ui8Neg2Bank)*0x2000)];
			}
			else {
				_ui8Ret = pmThis->m_prRom->vPrgRom.data()[size_t(_ui16Parm1)+(size_t(pmThis->m_ui8PgmBanks[1])*0x2000)];
			}
		}
	};

}	// namespace lsn

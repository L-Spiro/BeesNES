/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 076 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper076
	 * \brief Mapper 076 implementation.
	 *
	 * Description: Mapper 076 implementation.
	 */
	class CMapper076 : public CMapperBase {
	public :
		CMapper076() {
		}
		virtual ~CMapper076() {
		}


		// == Functions.
		/**
		 * Gets the PGM bank size.
		 *
		 * \return Returns the size of the PGM banks.
		 */
		static constexpr uint16_t						PgmBankSize() { return 8 * 1024; }

		/**
		 * Gets the CHR bank size.
		 *
		 * \return Returns the size of the CHR banks.
		 */
		static constexpr uint16_t						ChrBankSize() { return 2 * 1024; }

		/**
		 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
		 *
		 * \param _rRom The ROM data.
		 * \param _pcbCpuBase A pointer to the CPU.
		 */
		virtual void									InitWithRom( LSN_ROM &_rRom, CCpuBase * _pcbCpuBase, CPpuBase * _ppbPpuBase, CInterruptable * _piInter, CBussable * _pbPpuBus ) {
			CMapperBase::InitWithRom( _rRom, _pcbCpuBase, _ppbPpuBase, _piInter, _pbPpuBus );
			SanitizeRegs<PgmBankSize(), ChrBankSize()>();

			m_ui8Reg0 = 0;
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
			// Submapper 1 does not allow switching of PGM banks.
			if ( m_prRom->riInfo.ui16SubMapper == 1 ) {
				m_stFixedOffset = 0;
				for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead_Fixed, this, uint16_t( (I - 0x8000) % (m_prRom->vPrgRom.size() - m_stFixedOffset) ) );
				}
			}
			else {
				// Set the reads of the fixed bank at the end.		
				m_stFixedOffset = std::max<size_t>( m_prRom->vPrgRom.size(), PgmBankSize() * 2 ) - PgmBankSize() * 2;
				for ( uint32_t I = 0xC000; I < 0x10000; ++I ) {
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead_Fixed, this, uint16_t( (I - 0xC000) % (m_prRom->vPrgRom.size() - m_stFixedOffset) ) );
				}
			}
			

			// ================
			// SWAPPABLE BANKS
			// ================
			// CPU.
			// Submapper 1 does not allow switching of PGM banks.
			if ( m_prRom->riInfo.ui16SubMapper != 1 ) {
				for ( uint32_t I = 0x8000; I < 0xA000; ++I ) {
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<0, PgmBankSize()>, this, uint16_t( I - 0x8000 ) );
				}
				for ( uint32_t I = 0xA000; I < 0xC000; ++I ) {
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<1, PgmBankSize()>, this, uint16_t( I - 0xA000 ) );
				}
			}
			// PPU.
			if ( m_prRom->vChrRom.size() ) {
				for ( uint32_t I = 0x0000; I < 0x0800; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<0, ChrBankSize()>, this, uint16_t( I - 0x0000 ) );
				}
				for ( uint32_t I = 0x0800; I < 0x1000; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<1, ChrBankSize()>, this, uint16_t( I - 0x0800 ) );
				}
				for ( uint32_t I = 0x1000; I < 0x1800; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<2, ChrBankSize()>, this, uint16_t( I - 0x1000 ) );
				}
				for ( uint32_t I = 0x1800; I < 0x2000; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<3, ChrBankSize()>, this, uint16_t( I - 0x1800 ) );
				}
			}


			// ================
			// BANK-SELECT
			// ================
			// PGM bank-select.
			for ( uint32_t I = 0x8000; I < 0xA000; ++I ) {
				if ( (I & 0xE001) == 0x8000 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper076::SelectBank8000_9FFE, this, 0 );
				}
				else if ( (I & 0xE001) == 0x8001 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper076::SelectBank8001_9FFF, this, 0 );
				}
			}
		}


	protected :
		// == Members.
		/** Register 1. */
		uint8_t											m_ui8Reg0;


		// == Functions.
		/**
		 * Bank select ($8000-$9FFE, even).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank8000_9FFE( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper076 * pmThis = reinterpret_cast<CMapper076 *>(_pvParm0);
			pmThis->m_ui8Reg0 = _ui8Val;
		}

		/**
		 * Bank data ($8001-$9FFF, odd).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank8001_9FFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper076 * pmThis = reinterpret_cast<CMapper076 *>(_pvParm0);
			/*
			 *	   $8001:  [..DD DDDD]    Data port:
			 *	          R:2 ->  CHR reg 0  (2k @ $0000)
			 *	          R:3 ->  CHR reg 1  (2k @ $0800)
			 *	          R:4 ->  CHR reg 2  (2k @ $1000)
			 *	          R:5 ->  CHR reg 3  (2k @ $1800)
			 *	          R:6 ->  PRG reg 0  (8k @ $8000)
			 *	          R:7 ->  PRG reg 1  (8k @ $a000)
			 */
			switch ( pmThis->m_ui8Reg0 & 0b111 ) {
				case 0b010 : {
					// R:2 ->  CHR reg 0  (2k @ $0000)
					pmThis->SetChrBank<0, ChrBankSize()>( _ui8Val );
					break;
				}
				case 0b011 : {
					// R:3 ->  CHR reg 1  (2k @ $0800)
					pmThis->SetChrBank<1, ChrBankSize()>( _ui8Val );
					break;
				}
				case 0b100 : {
					// 4 ->  CHR reg 2  (2k @ $1000)
					pmThis->SetChrBank<2, ChrBankSize()>( _ui8Val );
					break;
				}
				case 0b101 : {
					// 5 ->  CHR reg 3  (2k @ $1800)
					pmThis->SetChrBank<3, ChrBankSize()>( _ui8Val );
					break;
				}
				case 0b110 : {
					// 6 ->  PRG reg 0  (8k @ $8000)
					pmThis->SetPgmBank<0, PgmBankSize()>( _ui8Val & 0b00001111 );
					break;
				}
				case 0b111 : {
					// 7 ->  PRG reg 1  (8k @ $a000)
					pmThis->SetPgmBank<1, PgmBankSize()>( _ui8Val & 0b00001111 );
					break;
				}
			}
		}
	};

}	// namespace lsn

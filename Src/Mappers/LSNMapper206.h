/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 206 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper206
	 * \brief Mapper 206 implementation.
	 *
	 * Description: Mapper 206 implementation.
	 */
	class CMapper206 : public CMapperBase {
	public :
		CMapper206() {
		}
		virtual ~CMapper206() {
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
		static constexpr uint16_t						ChrBankSize() { return 1 * 1024; }

		/**
		 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
		 *
		 * \param _rRom The ROM data.
		 * \param _pcbCpuBase A pointer to the CPU.
		 */
		virtual void									InitWithRom( LSN_ROM &_rRom, CCpuBase * _pcbCpuBase, CPpuBase * _ppbPpuBase, CInterruptable * _piInter, CBussable * _pbPpuBus ) {
			CMapperBase::InitWithRom( _rRom, _pcbCpuBase, _ppbPpuBase, _piInter, _pbPpuBus );
			SanitizeRegs<PgmBankSize(), ChrBankSize()>();
			// Used when $8000.D6 == 1.
			SetPgmBank<2, PgmBankSize()>( -2 );

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
			// Submapper 1 does not allow swicthing of PGM banks.
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
			// Submapper 1 does not allow swicthing of PGM banks.
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
				for ( uint32_t I = 0x0000; I < 0x0400; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<0, ChrBankSize()>, this, uint16_t( I - 0x0000 ) );
				}
				for ( uint32_t I = 0x0400; I < 0x0800; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<1, ChrBankSize()>, this, uint16_t( I - 0x0400 ) );
				}
				for ( uint32_t I = 0x0800; I < 0x0C00; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<2, ChrBankSize()>, this, uint16_t( I - 0x0800 ) );
				}
				for ( uint32_t I = 0x0C00; I < 0x1000; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<3, ChrBankSize()>, this, uint16_t( I - 0x0C00 ) );
				}
				for ( uint32_t I = 0x1000; I < 0x1400; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<4, ChrBankSize()>, this, uint16_t( I - 0x1000 ) );
				}
				for ( uint32_t I = 0x1400; I < 0x1800; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<5, ChrBankSize()>, this, uint16_t( I - 0x1400 ) );
				}
				for ( uint32_t I = 0x1800; I < 0x1C00; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<6, ChrBankSize()>, this, uint16_t( I - 0x1800 ) );
				}
				for ( uint32_t I = 0x1C00; I < 0x2000; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<7, ChrBankSize()>, this, uint16_t( I - 0x1C00 ) );
				}
			}


			// ================
			// BANK-SELECT
			// ================
			// PGM bank-select.
			for ( uint32_t I = 0x8000; I < 0xA000; ++I ) {
				if ( (I & 0xE001) == I ) {
					if ( (I & 1) == 0 ) {
						_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper206::SelectBank8000_9FFE, this, 0 );
					}
					else {
						_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper206::SelectBank8001_9FFF, this, 0 );
					}
				}
			}



			// ================
			// MIRRORING
			// ================
			//if ( m_prRom->riInfo.mmMirroring == LSN_MM_4_SCREENS ) {
			//	//ApplyMirroring( LSN_MM_4_SCREENS, _pbPpuBus, this );
			//}
			//else {
			//	ApplyControllableMirrorMap( _pbPpuBus );
			//}
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
			CMapper206 * pmThis = reinterpret_cast<CMapper206 *>(_pvParm0);
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
			CMapper206 * pmThis = reinterpret_cast<CMapper206 *>(_pvParm0);
			/*
			 *	7  bit  0
			 *	---- ----
			 *	xxxx xRRR
			 *	      |||
			 *	      +++- Specify which bank register to update on next write to Bank Data register
			 *	           0: Select 2 KB CHR bank at PPU $0000-$07FF
			 *	           1: Select 2 KB CHR bank at PPU $0800-$0FFF
			 *	           2: Select 1 KB CHR bank at PPU $1000-$13FF
			 *	           3: Select 1 KB CHR bank at PPU $1400-$17FF
			 *	           4: Select 1 KB CHR bank at PPU $1800-$1BFF
			 *	           5: Select 1 KB CHR bank at PPU $1C00-$1FFF
			 *	           6: Select 8 KB PRG ROM bank at $8000-$9FFF
			 *	           7: Select 8 KB PRG ROM bank at $A000-$BFFF
			 */
			switch ( pmThis->m_ui8Reg0 & 0b111 ) {
				case 0b000 : {
					// 000: R0: Select 2 KB CHR bank at PPU $0000-$07FF (or $1000-$17FF)
					pmThis->SetChrBank2x<0, ChrBankSize()>( _ui8Val & 0b11111110 );
					break;
				}
				case 0b001 : {
					// 001: R1: Select 2 KB CHR bank at PPU $0800-$0FFF (or $1800-$1FFF)
					pmThis->SetChrBank2x<2, ChrBankSize()>( _ui8Val & 0b11111110 );
					break;
				}
				case 0b010 : {
					// 010: R2: Select 1 KB CHR bank at PPU $1000-$13FF (or $0000-$03FF)
					pmThis->SetChrBank<4, ChrBankSize()>( _ui8Val );
					break;
				}
				case 0b011 : {
					// 011: R3: Select 1 KB CHR bank at PPU $1400-$17FF (or $0400-$07FF)
					pmThis->SetChrBank<5, ChrBankSize()>( _ui8Val );
					break;
				}
				case 0b100 : {
					// 100: R4: Select 1 KB CHR bank at PPU $1800-$1BFF (or $0800-$0BFF)
					pmThis->SetChrBank<6, ChrBankSize()>( _ui8Val );
					break;
				}
				case 0b101 : {
					// 101: R5: Select 1 KB CHR bank at PPU $1C00-$1FFF (or $0C00-$0FFF)
					pmThis->SetChrBank<7, ChrBankSize()>( _ui8Val );
					break;
				}
				case 0b110 : {
					// 110: R6: Select 8 KB PRG ROM bank at $8000-$9FFF (or $C000-$DFFF)
					pmThis->SetPgmBank<0, PgmBankSize()>( _ui8Val & 0b00001111 );
					break;
				}
				case 0b111 : {
					// 111: R7: Select 8 KB PRG ROM bank at $A000-$BFFF)
					pmThis->SetPgmBank<1, PgmBankSize()>( _ui8Val & 0b00001111 );
					break;
				}
			}
		}
	};

}	// namespace lsn

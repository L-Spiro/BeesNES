/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 034 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper034
	 * \brief Mapper 034 implementation.
	 *
	 * Description: Mapper 034 implementation.
	 */
	class CMapper034 : public CMapperBase {
	public :
		CMapper034() {
		}
		virtual ~CMapper034() {
		}


		// == Functions.
		/**
		 * Gets the PGM bank size.
		 *
		 * \return Returns the size of the PGM banks.
		 */
		static constexpr uint16_t						PgmBankSize() { return 32 * 1024; }

		/**
		 * Gets the CHR bank size.
		 *
		 * \return Returns the size of the CHR banks.
		 */
		static constexpr uint16_t						ChrBankSize() { return 4 * 1024; }

		/**
		 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
		 *
		 * \param _rRom The ROM data.
		 * \param _pcbCpuBase A pointer to the CPU.
		 */
		virtual void									InitWithRom( LSN_ROM &_rRom, CCpuBase * _pcbCpuBase, CInterruptable * _piInter, CBussable * _pbPpuBus ) {
			CMapperBase::InitWithRom( _rRom, _pcbCpuBase, _piInter, _pbPpuBus );
			SanitizeRegs<PgmBankSize(), ChrBankSize()>();
			// For compatibility between both BNROM and NINA-001 boards.
			m_ui8PgmBanks[0] = 0;
			m_ui8ChrBanks[0] = 0;
			m_ui8ChrBanks[1] = 1;
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
			// SWAPPABLE BANKS
			// ================
			// CPU.
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<0, PgmBankSize()>, this, uint16_t( I - 0x8000 ) );
			}
			if ( m_prRom->vChrRom.size() ) {
				// PPU.
				for ( uint32_t I = 0x0000; I < 0x1000; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<0, ChrBankSize()>, this, uint16_t( I - 0x0000 ) );
				}
				for ( uint32_t I = 0x1000; I < 0x2000; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<1, ChrBankSize()>, this, uint16_t( I - 0x1000 ) );
				}
			}


			// ================
			// BANK-SELECT
			// ================
			// PGM bank-select.
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper034::SelectBank8000_FFFF, this, 0 );	// Treated as ROM.
			}
			
			_pbCpuBus->SetWriteFunc( 0x7FFD, &CMapper034::SelectBank7FFD, this, 0 );	// Treated as ROM.
			if ( m_prRom->vChrRom.size() ) {
				_pbCpuBus->SetWriteFunc( 0x7FFE, &CMapper034::SelectBank7FFE, this, 0 );	// Treated as ROM.
				_pbCpuBus->SetWriteFunc( 0x7FFF, &CMapper034::SelectBank7FFF, this, 0 );	// Treated as ROM.
			}
		}


	protected :
		// == Members.


		// == Functions.
		/**
		 * Selects a bank (BNROM).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank8000_FFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper034 * pmThis = reinterpret_cast<CMapper034 *>(_pvParm0);
			pmThis->SetPgmBank<0, PgmBankSize()>( _ui8Val & 0b11 );
		}

		/**
		 * Selects a bank (NINA-001).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank7FFD( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper034 * pmThis = reinterpret_cast<CMapper034 *>(_pvParm0);
			pmThis->SetPgmBank<0, PgmBankSize()>( _ui8Val & 0b1 );
		}

		/**
		 * Selects a bank (NINA-001).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank7FFE( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper034 * pmThis = reinterpret_cast<CMapper034 *>(_pvParm0);
			pmThis->SetChrBank<0, ChrBankSize()>( _ui8Val & 0b1111 );
		}

		/**
		 * Selects a bank (NINA-001).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank7FFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper034 * pmThis = reinterpret_cast<CMapper034 *>(_pvParm0);
			pmThis->SetChrBank<1, ChrBankSize()>( _ui8Val & 0b1111 );
		}
	};

}	// namespace lsn

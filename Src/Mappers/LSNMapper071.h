/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 071 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper071
	 * \brief Mapper 071 implementation.
	 *
	 * Description: Mapper 071 implementation.
	 */
	class CMapper071 : public CMapperBase {
	public :
		CMapper071() {
		}
		virtual ~CMapper071() {
		}


		// == Functions.
		/**
		 * Gets the PGM bank size.
		 *
		 * \return Returns the size of the PGM banks.
		 */
		static constexpr uint16_t						PgmBankSize() { return 16 * 1024; }

		/**
		 * Gets the CHR bank size.
		 *
		 * \return Returns the size of the CHR banks.
		 */
		static constexpr uint16_t						ChrBankSize() { return 8 * 1024; }

		/**
		 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
		 *
		 * \param _rRom The ROM data.
		 * \param _pcbCpuBase A pointer to the CPU.
		 */
		virtual void									InitWithRom( LSN_ROM &_rRom, CCpuBase * _pcbCpuBase, CBussable * _pbPpuBus ) {
			CMapperBase::InitWithRom( _rRom, _pcbCpuBase, _pbPpuBus );
			SanitizeRegs<PgmBankSize(), ChrBankSize()>();
			m_ui8Bank = 0;
			SetPgmBank<0, PgmBankSize()>( (m_ui8Bank & 0b1100) | 0b11 );
			SetPgmBank<1, PgmBankSize()>( m_ui8Bank );
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
			// ================
			// FIXED BANKS
			// ================
			// Set the reads of the fixed bank at the end.		
			m_stFixedOffset = std::max<size_t>( m_prRom->vPrgRom.size(), PgmBankSize() ) - PgmBankSize();
			for ( uint32_t I = 0xC000; I < 0x10000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead_Fixed, this, uint16_t( (I - 0xC000) % m_prRom->vPrgRom.size() ) );
			}
			// Set the reads of the fixed bank at the end.
			//for ( uint32_t I = 0xC000; I < 0x10000; ++I ) {
			//	// $C000-$FFFF: Fixed to last bank.
			//	_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<0, PgmBankSize()>, this, uint16_t( I - 0xC000 ) );
			//}
			

			// ================
			// SWAPPABLE BANKS
			// ================
			// CPU.
			for ( uint32_t I = 0x8000; I < 0xC000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<1, PgmBankSize()>, this, uint16_t( I - 0x8000 ) );
			}


			// ================
			// BANK-SELECT
			// ================
			// PGM bank-select.
			if ( m_prRom->riInfo.ui16Chip == CDatabase::LSN_C_BF9096 ) {
				for ( uint32_t I = 0x8000; I < 0xC000; ++I ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper071::SelectBank8000_BFFF, this, 0 );
				}
			}
			for ( uint32_t I = 0xC000; I < 0x10000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper071::SelectBankC000_FFFF, this, 0 );
			}


			// ================
			// MIRRORING
			// ================
			if ( m_prRom->riInfo.ui16Chip == CDatabase::LSN_C_BF9097 ) {
				ApplyControllableMirrorMap( _pbPpuBus );
			}
		}


	protected :
		// == Members.
		/** The block and page. */
		uint8_t											m_ui8Bank;


		// == Functions.
		/**
		 * PRG block select.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank8000_BFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper071 * pmThis = reinterpret_cast<CMapper071 *>(_pvParm0);
			if ( pmThis->m_prRom->riInfo.ui16SubMapper == 1 ) {	// Aladdin Deck Enhancer.
				pmThis->m_ui8Bank = (pmThis->m_ui8Bank & 0b00011) |
					((((_ui8Val >> 1) & 0b01000) >> 1) |
					 (((_ui8Val << 1) & 0b10000) >> 1));
			}
			else {
				pmThis->m_ui8Bank = (pmThis->m_ui8Bank & 0b00011) | ((_ui8Val & 0b11000) >> 1);
			}

			pmThis->SetPgmBank<0, PgmBankSize()>( (pmThis->m_ui8Bank & 0b1100) | 0b11 );
			pmThis->SetPgmBank<1, PgmBankSize()>( pmThis->m_ui8Bank );
		}

		/**
		 * Selects a bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBankC000_FFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper071 * pmThis = reinterpret_cast<CMapper071 *>(_pvParm0);
			if ( pmThis->m_prRom->riInfo.ui16Chip == CDatabase::LSN_C_BF9096 ) {
				pmThis->m_ui8Bank = (pmThis->m_ui8Bank & 0b1100) | (_ui8Val & 0b0011);
			}
			else if ( pmThis->m_prRom->riInfo.ui16Chip == CDatabase::LSN_C_BF9097 ) {
				pmThis->m_ui8Bank = (_ui8Val & 0b0111);
			}
			else {
				pmThis->m_ui8Bank = (_ui8Val & 0b1111);
			}
			
			pmThis->SetPgmBank<0, PgmBankSize()>( (pmThis->m_ui8Bank & 0b1100) | 0b11 );
			pmThis->SetPgmBank<1, PgmBankSize()>( pmThis->m_ui8Bank );
		}
	};

}	// namespace lsn

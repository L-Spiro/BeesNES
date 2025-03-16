/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 232 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper234
	 * \brief Mapper 232 implementation.
	 *
	 * Description: Mapper 232 implementation.
	 */
	class CMapper234 : public CMapperBase {
	public :
		CMapper234() {
		}
		virtual ~CMapper234() {
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
			m_ui8Reg80 = m_ui8RegE8 = 0;
			/*SetPgmBank<0, PgmBankSize()>( (m_ui8Bank & 0b1100) | 0x3 );
			SetPgmBank<1, PgmBankSize()>( m_ui8Bank );*/
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
			for ( uint32_t I = 0xC000; I < 0x10000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<0, PgmBankSize()>, this, uint16_t( I - 0xC000 ) );
			}
			

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
			// Outer: $FF80-$FF9F
			for ( uint32_t I = 0xFF80; I <= 0xFF9F; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper234::ReadFF80_FF9F, this, uint16_t( I ) );
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CCpuBus::NoWrite, this, 0 );
			}
			// Inner: $FFE8-$FFF7
			for ( uint32_t I = 0xFFE8; I <= 0xFFF7; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper234::ReadFFE8_FFF7, this, uint16_t( I ) );
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CCpuBus::NoWrite, this, 0 );
			}
		}


	protected :
		// == Members.
		/** The $FF80-$FF9F register. */
		uint8_t											m_ui8Reg80;
		/** The $FFE8-$FFF7 register. */
		uint8_t											m_ui8RegE8;


		// == Functions.
		/**
		 * Reads the 80-9F registers.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						ReadFF80_FF9F( void * /*_pvParm0*/, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t &/*_ui8Ret*/ ) {
			//CMapper234 * pmThis = reinterpret_cast<CMapper234 *>(_pvParm0);
		}

		/**
		 * Outer bank select.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBankFF80_FF9F( void * /*_pvParm0*/, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t /*_ui8Val*/ ) {
			//CMapper234 * pmThis = reinterpret_cast<CMapper234 *>(_pvParm0);
			//if ( pmThis->m_prRom->riInfo.ui16SubMapper == 1 ) {	// Aladdin Deck Enhancer.
			//	pmThis->m_ui8Bank = (pmThis->m_ui8Bank & 0b00011) |
			//		((((_ui8Val >> 1) & 0b01000) >> 1) |
			//		 (((_ui8Val << 1) & 0b10000) >> 1));
			//}
			//else {
			//	pmThis->m_ui8Bank = (pmThis->m_ui8Bank & 0b00011) | ((_ui8Val & 0b11000) >> 1);
			//}

			//pmThis->SetPgmBank<0, PgmBankSize()>( (pmThis->m_ui8Bank & 0b1100) | 0x3 );
			//pmThis->SetPgmBank<1, PgmBankSize()>( pmThis->m_ui8Bank );
		}

		/**
		 * Reads the E8-F7 registers.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						ReadFFE8_FFF7( void * /*_pvParm0*/, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t &/*_ui8Ret*/ ) {
			//CMapper234 * pmThis = reinterpret_cast<CMapper234 *>(_pvParm0);
		}

		/**
		 * Iinner bank select.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBankFFE8_FFF7( void * /*_pvParm0*/, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t /*_ui8Val*/ ) {
			//CMapper234 * pmThis = reinterpret_cast<CMapper234 *>(_pvParm0);
			/*pmThis->m_ui8Bank = (pmThis->m_ui8Bank & 0b1100) | (_ui8Val & 0b0011);
			
			pmThis->SetPgmBank<0, PgmBankSize()>( (pmThis->m_ui8Bank & 0b1100) | 0x3 );
			pmThis->SetPgmBank<1, PgmBankSize()>( pmThis->m_ui8Bank );*/
		}
	};

}	// namespace lsn

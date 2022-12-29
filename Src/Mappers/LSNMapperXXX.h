/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper XXX implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapperXXX
	 * \brief Mapper XXX implementation.
	 *
	 * Description: Mapper XXX implementation.
	 */
	class CMapperXXX : public CMapperBase {
	public :
		CMapperXXX() {
		}
		virtual ~CMapperXXX() {
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
		static constexpr uint16_t						ChrBankSize() { return 2 * 1024; }

		/**
		 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
		 *
		 * \param _rRom The ROM data.
		 * \param _pcbCpuBase A pointer to the CPU.
		 */
		virtual void									InitWithRom( LSN_ROM &_rRom, CCpuBase * _pcbCpuBase ) {
			CMapperBase::InitWithRom( _rRom, _pcbCpuBase );
			SanitizeRegs<PgmBankSize(), ChrBankSize()>();
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
			for ( uint32_t I = 0xC000; I < 0x10000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead_Fixed, this, uint16_t( I - 0xC000 ) );
			}
			

			// ================
			// SWAPPABLE BANKS
			// ================
			// CPU.
			for ( uint32_t I = 0x8000; I < 0xC000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<0, PgmBankSize()>, this, uint16_t( I - 0x8000 ) );
			}
			// PPU.
			for ( uint32_t I = 0x0000; I < 0x2000; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<0, ChrBankSize()>, this, uint16_t( I - 0x0000 ) );
			}


			// ================
			// BANK-SELECT
			// ================
			// PGM bank-select.
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapperXXX::SelectBankY000_YFFF, this, 0 );	// Treated as ROM.
			}


			// ================
			// MIRRORING
			// ================
			ApplyControllableMirrorMap( _pbPpuBus );
		}


	protected :
		// == Members.


		// == Functions.
		/**
		 * Selects a bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						SelectBankY000_YFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapperXXX * pmThis = reinterpret_cast<CMapperXXX *>(_pvParm0);
			pmThis->SetPgmBank<0, PgmBankSize()>( _ui8Val & 0b1111 );
		}
	};

}	// namespace lsn

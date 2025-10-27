/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 184 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper184
	 * \brief Mapper 184 implementation.
	 *
	 * Description: Mapper 184 implementation.
	 */
	class CMapper184 : public CMapperBase {
	public :
		CMapper184() {
		}
		virtual ~CMapper184() {
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
		virtual void									InitWithRom( LSN_ROM &_rRom, CCpuBase * _pcbCpuBase, CPpuBase * _ppbPpuBase, CInterruptable * _piInter, CBussable * _pbPpuBus ) {
			CMapperBase::InitWithRom( _rRom, _pcbCpuBase, _ppbPpuBase, _piInter, _pbPpuBus );
			SanitizeRegs<1, ChrBankSize()>();
			
			SetChrBank<0, ChrBankSize()>( m_ui8ChrBanks[0] & 0b00000111 );
			SetChrBank<1, ChrBankSize()>( (m_ui8ChrBanks[1] & 0b00000111) | 0b100 );
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
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead_Fixed, this, uint16_t( (I - 0x8000) % (m_prRom->vPrgRom.size() - m_stFixedOffset) ) );
			}


			// ================
			// SWAPPABLE BANKS
			// ================
			// PPU.
			if ( m_prRom->vChrRom.size() ) {
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
			// CHR bank-select.
			// Writes to the whole area are used to select a bank.
			for ( uint32_t I = 0x6000; I < 0x8000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper184::SelectBank6000_7FFF, this, 0 );
			}
		}


	protected :
		// == Functions.
		/**
		 * Selects a bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank6000_7FFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper184 * pmThis = reinterpret_cast<CMapper184 *>(_pvParm0);
			pmThis->SetChrBank<0, ChrBankSize()>( _ui8Val & 0b00000111 );
			pmThis->SetChrBank<1, ChrBankSize()>( ((_ui8Val & 0b01110000) >> 4) | 0b100 );
			// The most significant bit of H is always set in hardware. (i.e. its range is 4 to 7)
		}
	};

}	// namespace lsn

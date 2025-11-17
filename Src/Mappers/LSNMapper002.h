/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 002 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper002
	 * \brief Mapper 002 implementation.
	 *
	 * Description: Mapper 002 implementation.
	 */
	class CMapper002 : public CMapperBase {
	public :
		CMapper002() {
		}
		virtual ~CMapper002() {
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
		virtual void									InitWithRom( LSN_ROM &_rRom, CCpuBase * _pcbCpuBase, CPpuBase * _ppbPpuBase, CInterruptable * _piInter, CBussable * _pbPpuBus ) {
			CMapperBase::InitWithRom( _rRom, _pcbCpuBase, _ppbPpuBase, _piInter, _pbPpuBus );
			m_ui8PgmBank = 0;
			m_bBusConflicts = _rRom.riInfo.ui16SubMapper != 1;
			{
				uint32_t ui32Banks = uint32_t( std::max<size_t>( 1, _rRom.vPrgRom.size() / PgmBankSize() ) );
				uint32_t ui32Bits  = 0;
				while ( (1U << ui32Bits) < ui32Banks ) { ++ui32Bits; }
				m_ui8Mask = uint8_t( (ui32Bits ? ((1U << ui32Bits) - 1U) : 0U) );
			}
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
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead_Fixed, this, uint16_t( (I - 0xC000) % (m_prRom->vPrgRom.size() - m_stFixedOffset) ) );
			}


			// ================
			// SWAPPABLE BANKS
			// ================
			// CPU.
			// Set the reads of the selectable bank.
			for ( uint32_t I = 0x8000; I < 0xC000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<0, PgmBankSize()>, this, uint16_t( I - 0x8000 ) );
			}


			// ================
			// BANK-SELECT
			// ================
			// PGM bank-select.
			// Writes to the whole area are used to select a bank.
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper002::SelectBank, this, uint16_t( I ) );
			}
		}


	protected :
		// == Members.
		/** The bank-select mask. */
		uint8_t											m_ui8Mask;
		/** Bus conflicts? */
		bool											m_bBusConflicts = true;


		// == Functions.
		/**
		 * Selects a bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper002 * pmThis = reinterpret_cast<CMapper002 *>(_pvParm0);
			if LSN_LIKELY( pmThis->m_bBusConflicts ) {
				size_t sIdx;
				if ( _ui16Parm1 < 0xC000 ) {
					sIdx = size_t( _ui16Parm1 - 0x8000 ) + size_t( pmThis->m_ui8PgmBanks[0] ) * PgmBankSize();
				}
				else {
					sIdx = size_t( _ui16Parm1 - 0xC000 ) + pmThis->m_stFixedOffset;
				}
				sIdx %= pmThis->m_prRom->vPrgRom.size();
				_ui8Val &= pmThis->m_prRom->vPrgRom[sIdx];		// Conflict = AND.
			}
			pmThis->SetPgmBank<0, PgmBankSize()>( (_ui8Val & pmThis->m_ui8Mask) );
		}
	};

}	// namespace lsn

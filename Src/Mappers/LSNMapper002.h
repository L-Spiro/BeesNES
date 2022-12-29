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
		 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
		 *
		 * \param _rRom The ROM data.
		 * \param _pcbCpuBase A pointer to the CPU.
		 */
		virtual void									InitWithRom( LSN_ROM &_rRom, CCpuBase * _pcbCpuBase ) {
			CMapperBase::InitWithRom( _rRom, _pcbCpuBase );
			m_ui8PgmBank = 0;
			m_ui8Mask = 0b0111;
		}

		/**
		 * Applies mapping to the CPU and PPU busses.
		 *
		 * \param _pbCpuBus A pointer to the CPU bus.
		 * \param _pbPpuBus A pointer to the PPU bus.
		 */
		virtual void									ApplyMap( CCpuBus * _pbCpuBus, CPpuBus * _pbPpuBus ) {
			CMapperBase::ApplyMap( _pbCpuBus, _pbPpuBus );

			// Set the reads of the fixed bank at the end.		
			m_stFixedOffset = std::max<size_t>( m_prRom->vPrgRom.size(), 0x4000 ) - 0x4000;
			for ( uint32_t I = 0xC000; I < 0x10000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead_Fixed, this, uint16_t( I - 0xC000 ) );
			}
			// Set the reads of the selectable bank.
			for ( uint32_t I = 0x8000; I < 0xC000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead_4000, this, uint16_t( I - 0x8000 ) );
			}

			// Writes to the whole area are used to select a bank.
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper002::SelectBank, this, 0 );	// Treated as ROM.
			}
		}


	protected :
		// == Members.
		/** The bank-select mask. */
		uint8_t											m_ui8Mask;


		// == Functions.
		/**
		 * Selects a bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						SelectBank( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper002 * pmThis = reinterpret_cast<CMapper002 *>(_pvParm0);
			pmThis->m_ui8PgmBank = (_ui8Val & pmThis->m_ui8Mask) % (pmThis->m_prRom->vPrgRom.size() / 0x4000);

		}
	};

}	// namespace lsn

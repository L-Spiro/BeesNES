/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper XXX implementation.
 */

#include "LSNMapperXXX.h"


namespace lsn {

	CMapperXXX::CMapperXXX() {
	}
	CMapperXXX::~CMapperXXX() {
	}

	// == Functions.
	/**
	 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
	 *
	 * \param _rRom The ROM data.
	 */
	void CMapperXXX::InitWithRom( LSN_ROM &_rRom ) {
		CMapperBase::InitWithRom( _rRom );
		m_ui8PgmBank = 0;
	}

	/**
	 * Applies mapping to the CPU and PPU busses.
	 *
	 * \param _pbCpuBus A pointer to the CPU bus.
	 * \param _pbPpuBus A pointer to the PPU bus.
	 */
	void CMapperXXX::ApplyMap( CCpuBus * _pbCpuBus, CPpuBus * /*_pbPpuBus*/ ) {
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
			_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapperXXX::SelectBankY000_YFFF, this, 0 );	// Treated as ROM.
		}
	}

	/**
	 * Selects a bank.
	 *
	 * \param _pvParm0 A data value assigned to this address.
	 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
	 * \param _pui8Data The buffer to which to write.
	 * \param _ui8Ret The value to write.
	 */
	void LSN_FASTCALL CMapperXXX::SelectBankY000_YFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
		CMapperXXX * pmThis = reinterpret_cast<CMapperXXX *>(_pvParm0);
		pmThis->m_ui8PgmBank = (_ui8Val & 0b1111) % (pmThis->m_prRom->vPrgRom.size() / 0x4000);

	}

}	// namespace lsn

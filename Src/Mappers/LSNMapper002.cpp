/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 002 implementation.
 */

#include "LSNMapper002.h"


namespace lsn {

	CMapper002::CMapper002() {
	}
	CMapper002::~CMapper002() {
	}

	// == Functions.
	/**
	 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
	 *
	 * \param _rRom The ROM data.
	 */
	void CMapper002::InitWithRom( LSN_ROM &_rRom ) {
		CMapperBase::InitWithRom( _rRom );
		m_ui8Bank = 0;
	}

	/**
	 * Applies mapping to the CPU and PPU busses.
	 *
	 * \param _pbCpuBus A pointer to the CPU bus.
	 * \param _pbPpuBus A pointer to the PPU bus.
	 */
	void CMapper002::ApplyMap( CCpuBus * _pbCpuBus, CPpuBus * /*_pbPpuBus*/ ) {
		// Set the reads of the fixed bank at the end.		
		m_stFixedOffset = std::max<size_t>( m_prRom->vPrgRom.size(), 0x4000 ) - 0x4000;
		for ( uint32_t I = 0xC000; I < 0x10000; ++I ) {
			_pbCpuBus->SetReadFunc(
				uint16_t( I ),								// The address to reroute to the mapper ([0xC000..0xFFFF]).
				&CMapper002::CpuFixedBankRead, this,		// The function pointer and parameter.
				uint16_t( I - 0xC000 )						// The offset to use inside CMapper002::CpuFixedBankRead() (XXX + pmThis->m_stFixedOffset).  ([0x0000..0x3FFF])
			);
															// m_stFixedOffset is 0x1C000, so if I is, for example, 0xC090:
															//	(uint16_t( I - 0xC000 ) + pmThis->m_stFixedOffset)
															//	(uint16_t( I - 0xC000 ) + 0x1C000)
															//	(uint16_t( 0xC090 - 0xC000 ) + 0x1C000)
															//	0x90 + 0x1C000
															//	0x1C090
															// m_prRom->vPrgRom[0x1C090] would be read.
		}
		// Set the reads of the selectable bank.
		for ( uint32_t I = 0x8000; I < 0xC000; ++I ) {
			_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper002::CpuBankRead, this, uint16_t( I - 0x8000 ) );
		}

		// Writes to the whole area are used to select a bank.
		for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
			_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper002::SelectBank, this, 0 );	// Treated as ROM.
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
	void LSN_FASTCALL CMapper002::SelectBank( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
		CMapper002 * pmThis = reinterpret_cast<CMapper002 *>(_pvParm0);
		pmThis->m_ui8Bank = _ui8Val & 0b0111;	// Trying to handle UNROM only for now.
												// (UNROM uses bits 2-0; UOROM uses bits 3-0)
	}

	/**
	 * Reads from PGM ROM using m_ui8Bank to select a bank.
	 *
	 * \param _pvParm0 A data value assigned to this address.
	 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
	 * \param _pui8Data The buffer from which to read.
	 * \param _ui8Ret The read value.
	 */
	void LSN_FASTCALL CMapper002::CpuBankRead( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
		CMapper002 * pmThis = reinterpret_cast<CMapper002 *>(_pvParm0);
		_ui8Ret = pmThis->m_prRom->vPrgRom[_ui16Parm1+(size_t( pmThis->m_ui8Bank )*0x4000)];
	}

	/**
	 * Reads the fixed bank.
	 *
	 * \param _pvParm0 A data value assigned to this address.
	 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
	 * \param _pui8Data The buffer from which to read.
	 * \param _ui8Ret The read value.
	 */
	void LSN_FASTCALL CMapper002::CpuFixedBankRead( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
		CMapper002 * pmThis = reinterpret_cast<CMapper002 *>(_pvParm0);
		_ui8Ret = pmThis->m_prRom->vPrgRom[_ui16Parm1+pmThis->m_stFixedOffset];
	}

}	// namespace lsn

/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 000 implementation.
 */

#include "LSNMapper003.h"


namespace lsn {

	CMapper003::CMapper003() {
	}
	CMapper003::~CMapper003() {
	}

	// == Functions.
	/**
	 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
	 *
	 * \param _rRom The ROM data.
	 */
	void CMapper003::InitWithRom( LSN_ROM &_rRom ) {
		CMapperBase::InitWithRom( _rRom );
		m_ui8ChrBank = 0;
	}

	/**
	 * Applies mapping to the CPU and PPU busses.
	 *
	 * \param _pbCpuBus A pointer to the CPU bus.
	 * \param _pbPpuBus A pointer to the PPU bus.
	 */
	void CMapper003::ApplyMap( CCpuBus * _pbCpuBus, CPpuBus * _pbPpuBus ) {
		for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
			_pbCpuBus->SetTrampolineReadFunc( uint16_t( I ), &CMapperBase::StdMapperCpuRead_Trampoline, this, uint16_t( (I - 0x8000) % m_prRom->vPrgRom.size() ), &m_tCpuTrampolines[I-0x8000] );
			_pbCpuBus->SetWriteFunc( uint16_t( I ), &Mapper003CpuWrite, this, 0 );	// Treated as ROM.
		}

		for ( uint32_t I = 0x0000; I < 0x2000; ++I ) {
			_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead_2000, this, uint16_t( I ) );
		}
	}

	/**
	 * CPU ROM-area writes are used to set the CHR bank.
	 *
	 * \param _pvParm0 A data value assigned to this address.
	 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
	 * \param _pui8Data The buffer to which to write.
	 * \param _ui8Ret The value to write.
	 */
	void LSN_FASTCALL CMapper003::Mapper003CpuWrite( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
		CMapper003 * pm3This = reinterpret_cast<CMapper003 *>(_pvParm0);
		pm3This->m_ui8ChrBank = _ui8Val & 0x3;
		// This area is ROM so deny any further writing operations.
	}

}	// namespace lsn

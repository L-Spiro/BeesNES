/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 000 implementation.
 */

#include "LSNMapper000.h"


namespace lsn {

	CMapper000::CMapper000() {
	}
	CMapper000::~CMapper000() {
	}

	// == Functions.
	/**
	 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
	 *
	 * \param _rRom The ROM data.
	 */
	void CMapper000::InitWithRom( LSN_ROM &_rRom ) {
		CMapperBase::InitWithRom( _rRom );
	}

	/**
	 * Applies mapping to the CPU and PPU busses.
	 *
	 * \param _pbCpuBus A pointer to the CPU bus.
	 * \param _pbPpuBus A pointer to the PPU bus.
	 */
	void CMapper000::ApplyMap( CCpuBus * _pbCpuBus, CPpuBus * /*_pbPpuBus*/ ) {
		for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
			_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::StdMapperCpuRead, this, uint16_t( (I - 0x8000) % m_prRom->vPrgRom.size() ) );
			_pbCpuBus->SetWriteFunc( uint16_t( I ), &CCpuBus::NoWrite, nullptr, uint16_t( I ) );	// Treated as ROM.
		}
	}

}	// namespace lsn

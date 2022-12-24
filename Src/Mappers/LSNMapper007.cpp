/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 007 implementation.
 */

#include "LSNMapper007.h"


namespace lsn {

	CMapper007::CMapper007() {
	}
	CMapper007::~CMapper007() {
	}

	// == Functions.
	/**
	 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
	 *
	 * \param _rRom The ROM data.
	 */
	void CMapper007::InitWithRom( LSN_ROM &_rRom ) {
		CMapperBase::InitWithRom( _rRom );
		m_ui8PgmBank = 0;
	}

	/**
	 * Applies mapping to the CPU and PPU busses.
	 *
	 * \param _pbCpuBus A pointer to the CPU bus.
	 * \param _pbPpuBus A pointer to the PPU bus.
	 */
	void CMapper007::ApplyMap( CCpuBus * _pbCpuBus, CPpuBus * _pbPpuBus ) {
		for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
			_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead_8000, this, uint16_t( I - 0x8000 ) );
		}

		// Writes to the whole area are used to select a bank.
		for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
			_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper007::SelectBank8000_FFFF, this, 0 );	// Treated as ROM.
		}

		ApplyControllableMirrorMap( _pbPpuBus );
	}

	/**
	 * Bank select ($8000-$FFFF).
	 *
	 * \param _pvParm0 A data value assigned to this address.
	 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
	 * \param _pui8Data The buffer to which to write.
	 * \param _ui8Ret The value to write.
	 */
	void LSN_FASTCALL CMapper007::SelectBank8000_FFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
		CMapper007 * pmThis = reinterpret_cast<CMapper007 *>(_pvParm0);
		/*
		 *	7  bit  0
		 *	---- ----
		 *	xxxM xPPP
		 *	   |  |||
		 *	   |  +++- Select 32 KB PRG ROM bank for CPU $8000-$FFFF
		 *	   +------ Select 1 KB VRAM page for all 4 nametables
		 */
		pmThis->m_ui8PgmBank = (_ui8Val & 0b0111) % (pmThis->m_prRom->vPrgRom.size() / 0x8000);
		switch ( _ui8Val & 0b10000 ) {
			case 0b10000 : {
				pmThis->m_mmMirror = LSN_MM_1_SCREEN_B;
				break;
			}
			case 0b00000 : {
				pmThis->m_mmMirror = LSN_MM_1_SCREEN_A;
				break;
			}
		}
	}

}	// namespace lsn

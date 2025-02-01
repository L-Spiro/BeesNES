/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 113 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper113
	 * \brief Mapper 113 implementation.
	 *
	 * Description: Mapper 113 implementation.
	 */
	class CMapper113 : public CMapperBase {
	public :
		CMapper113() {
		}
		virtual ~CMapper113() {
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
			// SWAPPABLE BANKS
			// ================
			// CPU.
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<0, PgmBankSize()>, this, uint16_t( (I - 0x8000) % m_prRom->vPrgRom.size() ) );
			}
			// PPU.
			for ( uint32_t I = 0x0000; I < 0x2000; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<0, ChrBankSize()>, this, uint16_t( I - 0x0000 ) );
			}


			// ================
			// BANK-SELECT
			// ================
			// PGM bank-select.
			for ( uint32_t I = 0x4100; I < 0x6000; ++I ) {
				if ( (I & 0b1110000100000000) == 0b0100000100000000 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper113::SelectBank4100_5FFF, this, 0 );	// Treated as ROM.
				}
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
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank4100_5FFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper113 * pmThis = reinterpret_cast<CMapper113 *>(_pvParm0);
			/*
			 *	7  bit  0
			 *	---- ----
			 *	MCPP PCCC
			 *	|||| ||||
			 *	|+||-|+++-- Select 8 KB CHR ROM bank for PPU $0000-$1FFF
			 *	| ++-+----- Select 32 KB PRG ROM bank for CPU $8000-$FFFF
			 *	+---------- Mirroring control (0 = Horizontal, 1 = Vertical)
			 */
			switch ( _ui8Val >> 7 ) {
				case 0 : {
					pmThis->m_mmMirror = LSN_MM_HORIZONTAL;
					break;
				}
				case 1 : {
					pmThis->m_mmMirror = LSN_MM_VERTICAL;
					break;
				}
			}
			pmThis->SetPgmBank<0, PgmBankSize()>( (_ui8Val >> 3) & 0b111 );
			pmThis->SetChrBank<0, ChrBankSize()>( ((_ui8Val & 0b01000000) >> 3) | (_ui8Val & 0b111) );
		}
	};

}	// namespace lsn

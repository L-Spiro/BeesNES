/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 041 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper041
	 * \brief Mapper 041 implementation.
	 *
	 * Description: Mapper 041 implementation.
	 */
	class CMapper041 : public CMapperBase {
	public :
		CMapper041() {
		}
		virtual ~CMapper041() {
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
		virtual void									InitWithRom( LSN_ROM &_rRom, CCpuBase * _pcbCpuBase, CInterruptable * _piInter, CBussable * _pbPpuBus ) {
			CMapperBase::InitWithRom( _rRom, _pcbCpuBase, _piInter, _pbPpuBus );
			SanitizeRegs<PgmBankSize(), ChrBankSize()>();

			// Both registers are cleared on powerup, or while the reset button is pressed.
			m_ui16Outer = 0;
			m_ui8Inner = 0;

			SetPgmBank<0, PgmBankSize()>( m_ui16Outer & 0b111 );
			SetChrBank<0, ChrBankSize()>(
				((m_ui8Inner & 0b11) * ((m_ui16Outer >> 2) & 1)) |			// Low bank enabled/disabled by E.
				((m_ui16Outer >> 1) & 0b1100)								// High bank.
				);
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
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<0, PgmBankSize()>, this, uint16_t( I - 0x8000 ) );
			}
			// PPU.
			for ( uint32_t I = 0x0000; I < 0x2000; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<0, ChrBankSize()>, this, uint16_t( I - 0x0000 ) );
			}


			// ================
			// BANK-SELECT
			// ================
			// PGM/CHR bank-select.
			for ( uint32_t I = 0x6000; I < 0x6800; ++I ) {
				if ( (I & 0b1111100000000000) == 0b0110000000000000 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper041::SelectBank6000_67FF, this, uint16_t( I ) );	// Treated as ROM.
				}
			}
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper041::SelectBank8000_FFFF, this, uint16_t( I ) );		// Treated as ROM.
			}


			// ================
			// MIRRORING
			// ================
			ApplyControllableMirrorMap( _pbPpuBus );
		}


	protected :
		// == Members.
		/** The outer bank select. */
		uint16_t										m_ui16Outer;
		/** The inner bank select. */
		uint8_t											m_ui8Inner;


		// == Functions.
		/**
		 * Outer Bank Select ($6000-$67FF).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank6000_67FF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t /*_ui8Val*/ ) {
			CMapper041 * pmThis = reinterpret_cast<CMapper041 *>(_pvParm0);
			pmThis->m_ui16Outer = _ui16Parm1;
			/**
			 *	15   11   7  bit  0 (address lines)
			 *	---- ---- ---- ----
			 *	0110 0xxx xxMC CEPP
			 *				││ ││││
			 *				││ │├┴┴── Select 32KiB PRG ROM bank for CPU $8000─$FFFF
			 *				││ ││
			 *				││ │└──── If high, enable Inner Bank Select
			 *				│└─┴───── Select 32KiB outer CHR ROM bank
			 *				└──────── Mirroring (0=Vertical, 1=Horizontal)
			 */
			pmThis->SetPgmBank<0, PgmBankSize()>( pmThis->m_ui16Outer & 0b111 );

			pmThis->SetChrBank<0, ChrBankSize()>(
				(pmThis->m_ui8Inner & 0b11) |				// Low bank.
				((pmThis->m_ui16Outer >> 1) & 0b1100)		// High bank.
				);

			switch ( (pmThis->m_ui16Outer >> 5) & 1 ) {
				case 0 : {
					pmThis->m_mmMirror = LSN_MM_VERTICAL;
					break;
				}
				case 1 : {
					pmThis->m_mmMirror = LSN_MM_HORIZONTAL;
					break;
				}
			}
		}

		/**
		 * Inner CHR Bank Select ($8000-$FFFF), bus conflicts.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank8000_FFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper041 * pmThis = reinterpret_cast<CMapper041 *>(_pvParm0);
			pmThis->m_ui8Inner = _ui8Val;
			/**
			 *	7  bit  0
			 *	---- ----
			 *	xxZZ xxcc
			 *	  ││   ││
			 *	  ││   └┴── Select 8KiB inner bank of outer 32KiB CHR bank chosen above
			 *	  └┴─────── Extant but unused
			 */
			// Note that the Inner CHR Bank Select only can be written while the PRG ROM bank is 4, 5, 6, or 7. This means that execution must pass through one of the upper four PRG banks to choose the inner CHR bank before playing any games in the lower four PRG banks.


			if ( ((pmThis->m_ui16Outer >> 2) & 1) ) {		// Changing of low bank enabled/disabled by E.
				pmThis->SetChrBank<0, ChrBankSize()>(
					(pmThis->m_ui8Inner & 0b11) |			// Low bank.			
					((pmThis->m_ui16Outer >> 1) & 0b1100)	// High bank.
					);
			}
		}
	};

}	// namespace lsn

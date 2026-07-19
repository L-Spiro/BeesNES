/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 228 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper228
	 * \brief Mapper 228 implementation.
	 *
	 * Description: NES Mapper 228 (Action 52 / Cheetahmen II) implementation.
	 */
	class CMapper228 : public CMapperBase {
	public :
		CMapper228() {
		}
		virtual ~CMapper228() {
		}


		// == Functions.
		/**
		 * Gets the PGM bank size.
		 *
		 * \return Returns the size of the PGM banks (16 KiB).
		 */
		static constexpr uint16_t						PgmBankSize() { return 16 * 1024; }

		/**
		 * Gets the CHR bank size.
		 *
		 * \return Returns the size of the CHR banks (8 KiB).
		 */
		static constexpr uint16_t						ChrBankSize() { return 8 * 1024; }

		/**
		 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
		 *
		 * \param _rRom The ROM data.
		 * \param _pcbCpuBase A pointer to the CPU.
		 * \param _ppbPpuBase A pointer to the PPU.
		 * \param _piInter A pointer to the interruptable system.
		 * \param _pbPpuBus A pointer to the PPU bus.
		 */
		virtual void									InitWithRom( LSN_ROM &_rRom, CCpuBase * _pcbCpuBase, CPpuBase * _ppbPpuBase, CInterruptable * _piInter, CBussable * _pbPpuBus ) {
			CMapperBase::InitWithRom( _rRom, _pcbCpuBase, _ppbPpuBase, _piInter, _pbPpuBus );
			SanitizeRegs<PgmBankSize(), ChrBankSize()>();

			/*m_ui8P = 0;
			m_ui8H = 0;
			m_ui8M = 0;
			m_ui8O = 0;
			m_ui8ChrHigh = 0;
			m_ui8ChrLow = 0;*/

			UpdateBanks();
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
			for ( uint32_t I = 0x8000; I < 0xC000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<0, PgmBankSize()>, this, uint16_t( I - 0x8000 ) );
			}
			for ( uint32_t I = 0xC000; I < 0x10000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<1, PgmBankSize()>, this, uint16_t( I - 0xC000 ) );
			}

			// PPU.
			// Only map the read function. The base class handles ignoring writes to CHR ROM.
			for ( uint32_t I = 0x0000; I < 0x2000; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<0, ChrBankSize()>, this, uint16_t( I ) );
			}

			// ================
			// BANK-SELECT
			// ================
			// Register value.
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper228::SelectBank8000_FFFF, this, uint16_t( I ) );
			}

			// ================
			// MIRRORING
			// ================
			ApplyControllableMirrorMap( _pbPpuBus );
		}


	protected :
		// == Members.
		uint8_t											m_ui8P;										/**< PRG Page Select (5 bits). */
		uint8_t											m_ui8H;										/**< PRG Chip Select (2 bits). */
		uint8_t											m_ui8M;										/**< Mirroring (0 = Vert, 1 = Horz). */
		uint8_t											m_ui8O;										/**< PRG Mode (0 = 32K, 1 = 16K mirrored). */
		uint8_t											m_ui8ChrHigh;								/**< High 4 bits of CHR (from address bus). */
		uint8_t											m_ui8ChrLow;								/**< Low 2 bits of CHR (from data bus). */


		// == Functions.
		/**
		 * Updates the bank selection.
		 **/
		void inline										UpdateBanks() {
			// PRG bank index is built from the chip select (H) shifted above the page select (P).
			uint8_t ui8Prg = (m_ui8H << 5) | m_ui8P;

			if ( m_ui8O == 0 ) {
				// 32 KiB mode: Contiguous 16 KiB banks.
				SetPgmBank<0, PgmBankSize()>( ui8Prg & ~1 );
				SetPgmBank<1, PgmBankSize()>( (ui8Prg & ~1) | 1 );
			}
			else {
				// 16 KiB mode: Same bank mapped to both $8000 and $C000.
				SetPgmBank<0, PgmBankSize()>( ui8Prg );
				SetPgmBank<1, PgmBankSize()>( ui8Prg );
			}
			
			// CHR bank index combines the high 4 bits (from address) and low 2 bits (from data).
			uint8_t ui8Chr = (m_ui8ChrHigh << 2) | m_ui8ChrLow;
			SetChrBank<0, ChrBankSize()>( ui8Chr );

			m_mmMirror = (m_ui8M == 0) ? LSN_MM_VERTICAL : LSN_MM_HORIZONTAL;
		}

		/**
		 * Selects a bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address. Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank8000_FFFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper228 * pmThis = reinterpret_cast<CMapper228 *>(_pvParm0);
			
			/*
			 * Address:
			 * A~FEDC BA98 7654 3210
			 *   ..MH HPPP PPO. CCCC
			 *   M = Mirroring (0 = Vert, 1 = Horz)
			 *   H = PRG Chip Select (2 bits)
			 *   P = PRG Page Select (5 bits)
			 *   O = PRG Mode (0 = 32K, 1 = 16K mirrored)
			 *   C = High 4 bits of CHR
			 *
			 * Data:
			 * D~7654 3210
			 *   .... ..CC
			 *   C = Low 2 bits of CHR
			 */
			pmThis->m_ui8M = (_ui16Parm1 >> 13) & 0x01;
			pmThis->m_ui8H = (_ui16Parm1 >> 11) & 0x03;
			pmThis->m_ui8P = (_ui16Parm1 >> 6) & 0x1F;
			pmThis->m_ui8O = (_ui16Parm1 >> 5) & 0x01;
			pmThis->m_ui8ChrHigh = _ui16Parm1 & 0x0F;
			
			pmThis->m_ui8ChrLow = _ui8Val & 0x03;
			
			pmThis->UpdateBanks();
		}
	};

}	// namespace lsn

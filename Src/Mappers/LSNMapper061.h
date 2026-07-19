/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 061 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper061
	 * \brief Mapper 061 implementation.
	 *
	 * Description: NES Mapper 061 (20-in-1 / 15-in-1 / 32-in-1 multicarts) implementation.
	 */
	class CMapper061 : public CMapperBase {
	public :
		CMapper061() {
		}
		virtual ~CMapper061() {
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

			m_ui8C = 0;
			m_ui8M = 0;
			m_ui8c = 0;
			m_ui8p = 0;
			m_ui8N = 0;
			m_ui8P = 0;
			
			// Submapper 1 is required for boards with 256 KiB CHR ROM to wire A13.
			m_bIsSubmapper1 = (_rRom.vChrRom.size() > 128 * 1024);

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
			if ( m_prRom->vChrRom.size() > 0 ) {
				for ( uint32_t I = 0x0000; I < 0x2000; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<0, ChrBankSize()>, this, uint16_t( I ) );
				}
			}


			// ================
			// BANK-SELECT
			// ================
			// Address value.
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper061::SelectBank8000_FFFF, this, uint16_t( I ) );
			}


			// ================
			// MIRRORING
			// ================
			ApplyControllableMirrorMap( _pbPpuBus );
		}


	protected :
		// == Members.
		uint8_t											m_ui8C;										/**< CHR Bank A16..A13 (Submapper 0) or A17..A14 (Submapper 1). */
		uint8_t											m_ui8M;										/**< Mirroring (0 = Vert, 1 = Horz). */
		uint8_t											m_ui8c;										/**< CHR A13 (Submapper 1 only). */
		uint8_t											m_ui8p;										/**< PRG A14 (When N = 1). */
		uint8_t											m_ui8N;										/**< PRG Mode (0 = 32K, 1 = 16K mirrored). */
		uint8_t											m_ui8P;										/**< PRG A18..A15. */
		bool											m_bIsSubmapper1;							/**< Tracks if this board is wired as Submapper 1. */


		// == Functions.
		/**
		 * Updates the bank selection.
		 **/
		void inline										UpdateBanks() {
			if ( m_ui8N == 0 ) {
				// 32 KiB mode: Contiguous 16 KiB banks mapped at $8000 and $C000.
				SetPgmBank<0, PgmBankSize()>( m_ui8P << 1 );
				SetPgmBank<1, PgmBankSize()>( (m_ui8P << 1) | 1 );
			}
			else {
				// 16 KiB mode: Same bank mapped to both $8000 and $C000.
				SetPgmBank<0, PgmBankSize()>( (m_ui8P << 1) | m_ui8p );
				SetPgmBank<1, PgmBankSize()>( (m_ui8P << 1) | m_ui8p );
			}
			
			// CHR bank.
			uint8_t ui8Chr = m_bIsSubmapper1 ? ((m_ui8C << 1) | m_ui8c) : m_ui8C;
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
		static void LSN_FASTCALL						SelectBank8000_FFFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t /*_ui8Val*/ ) {
			CMapper061 * pmThis = reinterpret_cast<CMapper061 *>(_pvParm0);
			
			/*
			 * Address:
			 * A~FEDC BA98 7654 3210
			 *   1... CCCC McpN PPPP
			 *   C = CHR bank (A16..A13 for submapper 0, A17..A14 for submapper 1)
			 *   M = Mirroring (0 = Vert, 1 = Horz)
			 *   c = CHR A13 (submapper 1 only)
			 *   p = PRG A14 if N=1
			 *   N = PRG Mode (0 = 32K, 1 = 16K mirrored)
			 *   P = PRG A18..A15
			 */
			pmThis->m_ui8C = (_ui16Parm1 >> 8) & 0x0F;
			pmThis->m_ui8M = (_ui16Parm1 >> 7) & 0x01;
			pmThis->m_ui8c = (_ui16Parm1 >> 6) & 0x01;
			pmThis->m_ui8p = (_ui16Parm1 >> 5) & 0x01;
			pmThis->m_ui8N = (_ui16Parm1 >> 4) & 0x01;
			pmThis->m_ui8P = _ui16Parm1 & 0x0F;
			
			pmThis->UpdateBanks();
		}
	};

}	// namespace lsn

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 226 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper226
	 * \brief Mapper 226 implementation.
	 *
	 * Description: NES Mapper 226 (76-in-1 multicart) implementation.
	 */
	class CMapper226 : public CMapperBase {
	public :
		CMapper226() {
		}
		virtual ~CMapper226() {
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

			m_ui8P = 0;
			m_ui8M = 0;
			m_ui8O = 0;
			m_ui8H = 0;

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


			// ================
			// BANK-SELECT
			// ================
			// Register value.
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper226::SelectBank8000_FFFF, this, uint16_t( I ) );
			}


			// ================
			// MIRRORING
			// ================
			ApplyControllableMirrorMap( _pbPpuBus );
		}


	protected :
		// == Members.
		uint8_t											m_ui8P;										/**< Low 6 bits of PRG Reg. */
		uint8_t											m_ui8M;										/**< Mirroring (0 = Horz, 1 = Vert). */
		uint8_t											m_ui8O;										/**< PRG Mode (0 = 32K, 1 = 16K). */
		uint8_t											m_ui8H;										/**< High bit of PRG. */


		// == Functions.
		/**
		 * Updates the bank selection.
		 **/
		void inline										UpdateBanks() {
			uint8_t ui8Bank = (m_ui8H << 6) | m_ui8P;

			if ( m_ui8O == 0 ) {
				// 32 KiB mode: Contiguous 16 KiB banks. 
				// We clear the lowest bit of the bank index to align to 32 KiB chunks.
				SetPgmBank<0, PgmBankSize()>( ui8Bank & ~1 );
				SetPgmBank<1, PgmBankSize()>( (ui8Bank & ~1) | 1 );
			}
			else {
				// 16 KiB mode: Same bank mapped to both $8000 and $C000.
				SetPgmBank<0, PgmBankSize()>( ui8Bank );
				SetPgmBank<1, PgmBankSize()>( ui8Bank );
			}

			m_mmMirror = (m_ui8M == 0) ? LSN_MM_HORIZONTAL : LSN_MM_VERTICAL;
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
			CMapper226 * pmThis = reinterpret_cast<CMapper226 *>(_pvParm0);
			
			if ( (_ui16Parm1 & 0x01) == 0 ) {
				/*
				 * $8000 (Even addresses):
				 * D~7654 3210
				 *   PMOP PPPP
				 *   P = Low 6 bits of PRG Reg
				 *   M = Mirroring (0=Horz, 1=Vert)
				 *   O = PRG Mode (0=32K, 1=16K mirrored)
				 */
				pmThis->m_ui8P = (_ui8Val & 0x1F) | ((_ui8Val >> 2) & 0x20);
				pmThis->m_ui8M = (_ui8Val >> 6) & 0x01;
				pmThis->m_ui8O = (_ui8Val >> 5) & 0x01;
			}
			else {
				/*
				 * $8001 (Odd addresses):
				 * D~7654 3210
				 *   .... ...H
				 *   H = High bit of PRG
				 */
				pmThis->m_ui8H = _ui8Val & 0x01;
			}
			
			pmThis->UpdateBanks();
		}
	};

}	// namespace lsn

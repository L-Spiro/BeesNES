/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 225 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper225
	 * \brief Mapper 225 implementation.
	 *
	 * Description: NES Mapper 225 (52/58/64/72-in-1 multicart) implementation.
	 */
	class CMapper225 : public CMapperBase {
	public :
		CMapper225() {
		}
		virtual ~CMapper225() {
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

			m_ui8H = 0;
			m_ui8M = 0;
			m_ui8O = 0;
			m_ui8P = 0;
			m_ui8C = 0;
			
			for ( size_t I = 0; I < 4; ++I ) {
				m_ui8Ram[I] = 0;
			}

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
			for ( uint32_t I = 0x0000; I < 0x2000; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<0, ChrBankSize()>, this, uint16_t( I ) );
			}


			// ================
			// BANK-SELECT
			// ================
			// RAM registers (Mirrored across $5800-$5FFF).
			for ( uint32_t I = 0x5800; I < 0x6000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper225::Read5800_5FFF, this, uint16_t( I & 0x03 ) );
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper225::Write5800_5FFF, this, uint16_t( I & 0x03 ) );
			}

			// Bank select.
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper225::SelectBank8000_FFFF, this, uint16_t( I ) );
			}


			// ================
			// MIRRORING
			// ================
			ApplyControllableMirrorMap( _pbPpuBus );
		}


	protected :
		// == Members.
		uint8_t											m_ui8Ram[4];								/**< 16-bit RAM (4 bits per byte) mapped at $5800-$5803. */
		uint8_t											m_ui8H;										/**< High bit (bit 7 for PRG and CHR). */
		uint8_t											m_ui8M;										/**< Mirroring. */
		uint8_t											m_ui8O;										/**< PRG Mode. */
		uint8_t											m_ui8P;										/**< PRG Reg. */
		uint8_t											m_ui8C;										/**< CHR Reg. */


		// == Functions.
		/**
		 * Updates the bank selection.
		 **/
		void inline										UpdateBanks() {
			uint8_t ui8Prg = (m_ui8H << 6) | m_ui8P;
			
			if ( m_ui8O == 1 ) {
				// 16 KiB mode: Same bank mapped to both $8000 and $C000.
				SetPgmBank<0, PgmBankSize()>( ui8Prg );
				SetPgmBank<1, PgmBankSize()>( ui8Prg );
			}
			else {
				// 32 KiB mode: Contiguous 16 KiB banks.
				SetPgmBank<0, PgmBankSize()>( ui8Prg & ~1 );
				SetPgmBank<1, PgmBankSize()>( (ui8Prg & ~1) | 1 );
			}
			
			SetChrBank<0, ChrBankSize()>( (m_ui8H << 6) | m_ui8C );

			m_mmMirror = (m_ui8M == 1) ? LSN_MM_HORIZONTAL : LSN_MM_VERTICAL;
		}

		/**
		 * Reads from the RAM at $5800-$5FFF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Read5800_5FFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper225 * pmThis = reinterpret_cast<CMapper225 *>(_pvParm0);
			// Returns only the 4-bit data.
			_ui8Ret = pmThis->m_ui8Ram[_ui16Parm1];
		}

		/**
		 * Writes to the RAM at $5800-$5FFF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write5800_5FFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper225 * pmThis = reinterpret_cast<CMapper225 *>(_pvParm0);
			// Stores only the bottom 4 bits.
			pmThis->m_ui8Ram[_ui16Parm1] = _ui8Val & 0x0F;
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
			CMapper225 * pmThis = reinterpret_cast<CMapper225 *>(_pvParm0);
			
			/*
			 * Address:
			 * A~FEDC BA98 7654 3210
			 *   .HMO PPPP PPCC CCCC
			 *   H = High bit (acts as bit 7 for PRG and CHR regs)
			 *   M = Mirroring (0=Vert, 1=Horz)
			 *   O = PRG Mode (0 = 32K PRG, 1 = 16K PRG)
			 *   P = PRG Reg
			 *   C = CHR Reg
			 */
			pmThis->m_ui8H = (_ui16Parm1 >> 14) & 0x01;
			pmThis->m_ui8M = (_ui16Parm1 >> 13) & 0x01;
			pmThis->m_ui8O = (_ui16Parm1 >> 12) & 0x01;
			pmThis->m_ui8P = (_ui16Parm1 >> 6) & 0x3F;
			pmThis->m_ui8C = _ui16Parm1 & 0x3F;
			
			pmThis->UpdateBanks();
		}
	};

}	// namespace lsn

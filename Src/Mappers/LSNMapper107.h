/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 107 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper107
	 * \brief Mapper 107 implementation.
	 *
	 * Description: NES Mapper 107 (Magic Dragon) implementation.
	 */
	class CMapper107 : public CMapperBase {
	public :
		CMapper107() {
		}
		virtual ~CMapper107() {
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

			m_ui8Bank = 0;

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
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper107::SelectBank8000_FFFF, this, 0 );
			}
		}


	protected :
		// == Members.
		uint8_t											m_ui8Bank;									/**< The current PRG and CHR bank (8 bits). */


		// == Functions.
		/**
		 * Updates the bank selection.
		 **/
		void inline										UpdateBanks() {
			// Mapper 107 maps in 32 KiB PRG chunks based on the top 7 bits.
			// The lowest bit of the register selects the 8 KiB CHR bank.
			// We clear the lowest bit of the bank index to map the 16 KiB chunks at N and N+1.
			SetPgmBank<0, PgmBankSize()>( m_ui8Bank & ~1 );
			SetPgmBank<1, PgmBankSize()>( (m_ui8Bank & ~1) | 1 );
			
			// CHR bank uses the exact register value (8 KiB chunks).
			SetChrBank<0, ChrBankSize()>( m_ui8Bank );
		}

		/**
		 * Selects a bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address. Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank8000_FFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper107 * pmThis = reinterpret_cast<CMapper107 *>(_pvParm0);
			
			/*
			 * Data:
			 * D~7654 3210
			 *   BBBB BBBb
			 *   |||| ||||
			 *   ++++ ++++- Select 8 KB CHR ROM bank for PPU $0000-$1FFF
			 *   ++++ +++-- Select 32 KB PRG ROM bank for CPU $8000-$FFFF
			 */
			pmThis->m_ui8Bank = _ui8Val;
			
			pmThis->UpdateBanks();
		}
	};

}	// namespace lsn

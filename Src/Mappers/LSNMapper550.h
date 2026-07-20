/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 550 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper550
	 * \brief Mapper 550 implementation.
	 *
	 * Description: NES 2.0 Mapper 550 (1993 Chess Series 7-in-1) implementation.
	 */
	class CMapper550 : public CMapperBase {
	public :
		CMapper550() {
		}
		virtual ~CMapper550() {
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
		 * \return Returns the size of the CHR banks (4 KiB).
		 */
		static constexpr uint16_t						ChrBankSize() { return 4 * 1024; }

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

			m_ui8OuterRegister = 0;
			m_ui8MhromLatch = 0;
			
			// Standard MMC1 default state (16K PRG, fixed $C000).
			m_ui8Control = 0x0C;
			m_ui8Chr0 = 0;
			m_ui8Chr1 = 0;
			m_ui8Prg = 0;
			
			m_ui8ShiftReg = 0;
			m_ui8ShiftCount = 0;

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
			if ( m_prRom->vPrgRom.size() ) {
				for ( uint32_t I = 0x8000; I < 0xC000; ++I ) {
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<0, PgmBankSize()>, this, uint16_t( I - 0x8000 ) );
				}
				for ( uint32_t I = 0xC000; I < 0x10000; ++I ) {
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<1, PgmBankSize()>, this, uint16_t( I - 0xC000 ) );
				}
			}
			
			if ( m_prRom->vChrRom.size() ) {
				// PPU.
				// Broken into two 4 KiB loops to accommodate the MMC1 4K CHR mode.
				for ( uint32_t I = 0x0000; I < 0x1000; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<0, ChrBankSize()>, this, uint16_t( I - 0x0000 ) );
				}
				for ( uint32_t I = 0x1000; I < 0x2000; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<1, ChrBankSize()>, this, uint16_t( I - 0x1000 ) );
				}
			}


			// ================
			// BANK-SELECT
			// ================
			// Outer Bank Register ($7000-$7FFF).
			for ( uint32_t I = 0x7000; I < 0x8000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper550::WriteOuterBank, this, 0 );
			}
			
			// MHROM Latch / MMC1 Register ($8000-$FFFF).
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper550::Write8000_FFFF, this, uint16_t( I ) );
			}


			// ================
			// MIRRORING
			// ================
			ApplyControllableMirrorMap( _pbPpuBus );
		}


	protected :
		// == Members.
		/** Internal Outer Bank register ($7000-$7FFF). */
		uint8_t											m_ui8OuterRegister;
		/** Internal MHROM Latch register ($8000-$FFFF). */
		uint8_t											m_ui8MhromLatch;
		
		/** MMC1 Control register. */
		uint8_t											m_ui8Control;
		/** MMC1 CHR 0 register. */
		uint8_t											m_ui8Chr0;
		/** MMC1 CHR 1 register. */
		uint8_t											m_ui8Chr1;
		/** MMC1 PRG register. */
		uint8_t											m_ui8Prg;
		
		/** MMC1 Serial Shift register. */
		uint8_t											m_ui8ShiftReg;
		/** MMC1 Serial Shift count. */
		uint8_t											m_ui8ShiftCount;


		// == Functions.
		/**
		 * Updates the bank selection.
		 **/
		void inline										UpdateBanks() {
			bool bIsMmc1 = (m_ui8OuterRegister & 0x07) == 3;
			
			uint32_t ui32OuterPrgBase = 0;
			uint32_t ui32OuterChrBase = 0;
			
			if ( bIsMmc1 ) {
				// MMC1 Mode: Outer gives PRG A18..17.
				// A17 is 128 KiB (or 8 * 16 KiB chunks). 
				ui32OuterPrgBase = ((m_ui8OuterRegister >> 1) & 0x03) << 3;
			}
			else {
				// MHROM Mode: Outer gives PRG A18..16.
				// A16 is 64 KiB (or 4 * 16 KiB chunks).
				ui32OuterPrgBase = (m_ui8OuterRegister & 0x07) << 2;
			}
			
			// Outer provides CHR A16..15 regardless of mode.
			// A15 is 32 KiB (or 8 * 4 KiB chunks).
			ui32OuterChrBase = ((m_ui8OuterRegister >> 1) & 0x03) << 3;
			
			
			// ---------------------------------------------------------
			// BANK ASSIGNMENT
			// ---------------------------------------------------------
			if ( !bIsMmc1 ) {
				// --- MHROM Mode ---
				// Inner PRG uses bit 4 for PRG A15 (1 * 32 KiB -> 2 * 16 KiB chunks).
				uint32_t ui32InnerPrg = ((m_ui8MhromLatch >> 4) & 0x01) << 1;
				SetPgmBank2x<0, PgmBankSize()>( uint16_t( ui32OuterPrgBase + ui32InnerPrg ) );
				
				// Inner CHR uses bits 0..1 for CHR A14..A13 (1 * 8 KiB -> 2 * 4 KiB chunks).
				uint32_t ui32InnerChr = (m_ui8MhromLatch & 0x03) << 1;
				SetChrBank2x<0, ChrBankSize()>( uint16_t( ui32OuterChrBase + ui32InnerChr ) );
			}
			else {
				// --- MMC1 Mode ---
				uint8_t ui8PrgMode = (m_ui8Control >> 2) & 0x03;
				
				// Inner MMC1 provides A14..A16. Outer handles the rest.
				uint32_t ui32InnerPrg = m_ui8Prg & 0x07;
				
				if ( ui8PrgMode <= 1 ) {
					// 32 KiB PRG Mode
					uint32_t ui32Prg = ui32OuterPrgBase + (ui32InnerPrg & ~1);
					SetPgmBank2x<0, PgmBankSize()>( uint16_t( ui32Prg ) );
				}
				else if ( ui8PrgMode == 2 ) {
					// 16 KiB PRG Mode: Fix $8000, swap $C000
					SetPgmBank<0, PgmBankSize()>( uint16_t( ui32OuterPrgBase ) );
					SetPgmBank<1, PgmBankSize()>( uint16_t( ui32OuterPrgBase + ui32InnerPrg ) );
				}
				else {
					// 16 KiB PRG Mode: Swap $8000, fix $C000 to last bank of inner 128K region (Bank 7)
					SetPgmBank<0, PgmBankSize()>( uint16_t( ui32OuterPrgBase + ui32InnerPrg ) );
					SetPgmBank<1, PgmBankSize()>( uint16_t( ui32OuterPrgBase + 7 ) );
				}
				
				// Inner MMC1 provides CHR A12..A14. Outer handles the rest.
				uint8_t ui8ChrMode = (m_ui8Control >> 4) & 0x01;
				uint32_t ui32InnerChr0 = m_ui8Chr0 & 0x07;
				uint32_t ui32InnerChr1 = m_ui8Chr1 & 0x07;
				
				if ( ui8ChrMode == 0 ) {
					// 8 KiB CHR Mode
					uint32_t ui32Chr = ui32OuterChrBase + (ui32InnerChr0 & ~1);
					SetChrBank2x<0, ChrBankSize()>( uint16_t( ui32Chr ) );
				}
				else {
					// 4 KiB CHR Mode
					SetChrBank<0, ChrBankSize()>( uint16_t( ui32OuterChrBase + ui32InnerChr0 ) );
					SetChrBank<1, ChrBankSize()>( uint16_t( ui32OuterChrBase + ui32InnerChr1 ) );
				}
			}
			
			// Nametable mirroring is decided by the MMC1 Control register regardless of mode.
			uint8_t ui8Mirror = m_ui8Control & 0x03;
			switch ( ui8Mirror ) {
				case 0 : { m_mmMirror = LSN_MM_1_SCREEN_A; break; }
				case 1 : { m_mmMirror = LSN_MM_1_SCREEN_B; break; }
				case 2 : { m_mmMirror = LSN_MM_VERTICAL; break; }
				case 3 : { m_mmMirror = LSN_MM_HORIZONTAL; break; }
			}
		}

		/**
		 * Updates the Outer Bank Register.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						WriteOuterBank( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper550 * pmThis = reinterpret_cast<CMapper550 *>(_pvParm0);
			
			// Bit 3 acts as a permanent lock.
			if ( !(pmThis->m_ui8OuterRegister & 0x08) ) {
				pmThis->m_ui8OuterRegister = _ui8Val;
				pmThis->UpdateBanks();
			}
		}

		/**
		 * Updates the MHROM Latch and processes the MMC1 serial shift stream.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write8000_FFFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper550 * pmThis = reinterpret_cast<CMapper550 *>(_pvParm0);
			
			// Both logic systems execute unconditionally on writes to $8000-$FFFF.
			pmThis->m_ui8MhromLatch = _ui8Val;
			bool bIsMmc1 = (pmThis->m_ui8OuterRegister & 0x07) == 3;
			
			if ( _ui8Val & 0x80 ) {
				pmThis->m_ui8ShiftReg = 0;
				pmThis->m_ui8ShiftCount = 0;
				pmThis->m_ui8Control |= 0x0C;
				pmThis->UpdateBanks();
			}
			else {
				pmThis->m_ui8ShiftReg = uint8_t( (pmThis->m_ui8ShiftReg >> 1) | ((_ui8Val & 0x01) << 4) );
				pmThis->m_ui8ShiftCount++;
				
				bool bUpdatedMmc1Inner = false;
				
				if ( pmThis->m_ui8ShiftCount == 5 ) {
					uint8_t ui8Target = (_ui16Parm1 >> 13) & 0x03;
					switch ( ui8Target ) {
						case 0 : { pmThis->m_ui8Control = pmThis->m_ui8ShiftReg; break; }
						case 1 : { pmThis->m_ui8Chr0 = pmThis->m_ui8ShiftReg; break; }
						case 2 : { pmThis->m_ui8Chr1 = pmThis->m_ui8ShiftReg; break; }
						case 3 : { pmThis->m_ui8Prg = pmThis->m_ui8ShiftReg; break; }
					}
					
					pmThis->m_ui8ShiftReg = 0;
					pmThis->m_ui8ShiftCount = 0;
					bUpdatedMmc1Inner = true;
				}
				
				// We must trigger an update if the MMC1 registers completed a shift, 
				// OR if we are in MHROM mode (because the MHROM Latch changes on every write).
				if ( bUpdatedMmc1Inner || !bIsMmc1 ) {
					pmThis->UpdateBanks();
				}
			}
		}
	};

}	// namespace lsn

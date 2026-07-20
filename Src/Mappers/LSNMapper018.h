/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 18 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

#include <algorithm>


namespace lsn {

	/**
	 * Class CMapper018
	 * \brief Mapper 18 implementation.
	 *
	 * Description: NES Mapper 18 (Jaleco SS88006) implementation.
	 */
	class CMapper018 : public CMapperBase {
	public :
		CMapper018() {
		}
		virtual ~CMapper018() {
		}


		// == Functions.
		/**
		 * Gets the PGM bank size.
		 *
		 * \return Returns the size of the PGM banks (8 KiB).
		 */
		static constexpr uint16_t						PgmBankSize() { return 8 * 1024; }

		/**
		 * Gets the CHR bank size.
		 *
		 * \return Returns the size of the CHR banks (1 KiB).
		 */
		static constexpr uint16_t						ChrBankSize() { return 1 * 1024; }

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

			m_ui8Prg[0] = 0;
			m_ui8Prg[1] = 1;
			m_ui8Prg[2] = 2;
			
			for ( uint8_t I = 0; I < 8; ++I ) {
				m_ui8Chr[I] = I;
			}
			SanitizeRegs<PgmBankSize(), ChrBankSize()>();
			
			m_ui8PrgRamProtect = 0;
			
			m_ui16IrqReload = 0;
			m_ui16IrqCounter = 0;
			m_ui8IrqControl = 0;
			m_bIrqStatus = false;
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
			// FIXED BANKS
			// ================
			if ( m_prRom->vPrgRom.size() ) {
				m_stFixedOffset = std::max<size_t>( m_prRom->vPrgRom.size(), PgmBankSize() ) - PgmBankSize();
				for ( uint32_t I = 0xE000; I < 0x10000; ++I ) {
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead_Fixed, this, uint16_t( (I - 0xE000) % (m_prRom->vPrgRom.size() - m_stFixedOffset) ) );
				}
			}


			// ================
			// SWAPPABLE BANKS
			// ================
			// CPU.
			if ( m_prRom->vPrgRom.size() ) {
				for ( uint32_t I = 0x8000; I < 0xA000; ++I ) {
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<0, PgmBankSize()>, this, uint16_t( I - 0x8000 ) );
				}
				for ( uint32_t I = 0xA000; I < 0xC000; ++I ) {
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<1, PgmBankSize()>, this, uint16_t( I - 0xA000 ) );
				}
				for ( uint32_t I = 0xC000; I < 0xE000; ++I ) {
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<2, PgmBankSize()>, this, uint16_t( I - 0xC000 ) );
				}
			}
			
			// PPU.
			if ( m_prRom->vChrRom.size() ) {
				for ( uint32_t I = 0x0000; I < 0x0400; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<0, ChrBankSize()>, this, uint16_t( I - 0x0000 ) );
				}
				for ( uint32_t I = 0x0400; I < 0x0800; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<1, ChrBankSize()>, this, uint16_t( I - 0x0400 ) );
				}
				for ( uint32_t I = 0x0800; I < 0x0C00; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<2, ChrBankSize()>, this, uint16_t( I - 0x0800 ) );
				}
				for ( uint32_t I = 0x0C00; I < 0x1000; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<3, ChrBankSize()>, this, uint16_t( I - 0x0C00 ) );
				}
				for ( uint32_t I = 0x1000; I < 0x1400; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<4, ChrBankSize()>, this, uint16_t( I - 0x1000 ) );
				}
				for ( uint32_t I = 0x1400; I < 0x1800; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<5, ChrBankSize()>, this, uint16_t( I - 0x1400 ) );
				}
				for ( uint32_t I = 0x1800; I < 0x1C00; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<6, ChrBankSize()>, this, uint16_t( I - 0x1800 ) );
				}
				for ( uint32_t I = 0x1C00; I < 0x2000; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<7, ChrBankSize()>, this, uint16_t( I - 0x1C00 ) );
				}
			}


			// ================
			// BANK-SELECT
			// ================
			// CPU.
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper018::Write8000_FFFF, this, uint16_t( I ) );
			}


			// ================
			// MIRRORING
			// ================
			ApplyControllableMirrorMap( _pbPpuBus );
		}
		
		/**
		 * Performs a single CPU-cycle update for the mapper's IRQ counter.
		 */
		virtual void									Tick() {
			if ( m_ui8IrqControl & 0x01 ) {
				uint16_t ui16Mask = 0;
				if ( m_ui8IrqControl & 0x08 ) { ui16Mask = 0x0FFF; }
				else if ( m_ui8IrqControl & 0x04 ) { ui16Mask = 0x00FF; }
				else if ( m_ui8IrqControl & 0x02 ) { ui16Mask = 0x000F; }
				else { ui16Mask = 0xFFFF; }

				uint16_t ui16Count = m_ui16IrqCounter & ui16Mask;
				
				if ( ui16Count == 0 ) {
					m_bIrqStatus = true;
					m_ui16IrqCounter = (m_ui16IrqCounter & ~ui16Mask) | ui16Mask;
					m_pInterruptable->Irq( LSN_IS_MAPPER );
				}
				else {
					m_ui16IrqCounter = (m_ui16IrqCounter & ~ui16Mask) | ((ui16Count - 1) & ui16Mask);
				}
			}
		}


	protected :
		// == Members.
		/** The 3 switchable PRG banks. */
		uint8_t											m_ui8Prg[3];
		/** The 8 switchable CHR banks. */
		uint8_t											m_ui8Chr[8];
		/** PRG RAM protection register. */
		uint8_t											m_ui8PrgRamProtect;
		
		/** 16-bit IRQ reload value. */
		uint16_t										m_ui16IrqReload;
		/** 16-bit IRQ counter. */
		uint16_t										m_ui16IrqCounter;
		/** IRQ control register. */
		uint8_t											m_ui8IrqControl;
		/** IRQ asserted status. */
		bool											m_bIrqStatus;


		// == Functions.
		/**
		 * Handles register writes from $8000 to $FFFF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write8000_FFFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper018 * pmThis = reinterpret_cast<CMapper018 *>(_pvParm0);
			
			// The ASIC uses A15..A12 to select the register, and A1..A0 to select the nibble.
			uint16_t ui16Mask = _ui16Parm1 & 0xF003;
			
			switch ( ui16Mask ) {
				// PRG Select 0 ($8000-$8003).
				case 0x8000 : {
					pmThis->m_ui8Prg[0] = (pmThis->m_ui8Prg[0] & 0xF0) | (_ui8Val & 0x0F);
					pmThis->SetPgmBank<0, PgmBankSize()>( pmThis->m_ui8Prg[0] );
					break;
				}
				case 0x8001 : {
					pmThis->m_ui8Prg[0] = (pmThis->m_ui8Prg[0] & 0x0F) | ((_ui8Val & 0x03) << 4);
					pmThis->SetPgmBank<0, PgmBankSize()>( pmThis->m_ui8Prg[0] );
					break;
				}
				// PRG Select 1 ($8002-$8003).
				case 0x8002 : {
					pmThis->m_ui8Prg[1] = (pmThis->m_ui8Prg[1] & 0xF0) | (_ui8Val & 0x0F);
					pmThis->SetPgmBank<1, PgmBankSize()>( pmThis->m_ui8Prg[1] );
					break;
				}
				case 0x8003 : {
					pmThis->m_ui8Prg[1] = (pmThis->m_ui8Prg[1] & 0x0F) | ((_ui8Val & 0x03) << 4);
					pmThis->SetPgmBank<1, PgmBankSize()>( pmThis->m_ui8Prg[1] );
					break;
				}
				// PRG Select 2 ($9000-$9001).
				case 0x9000 : {
					pmThis->m_ui8Prg[2] = (pmThis->m_ui8Prg[2] & 0xF0) | (_ui8Val & 0x0F);
					pmThis->SetPgmBank<2, PgmBankSize()>( pmThis->m_ui8Prg[2] );
					break;
				}
				case 0x9001 : {
					pmThis->m_ui8Prg[2] = (pmThis->m_ui8Prg[2] & 0x0F) | ((_ui8Val & 0x03) << 4);
					pmThis->SetPgmBank<2, PgmBankSize()>( pmThis->m_ui8Prg[2] );
					break;
				}
				// PRG RAM Protect ($9002).
				case 0x9002 : {
					// Bit 0: PRG RAM chip enable (0: disable; 1: enable).
					// Bit 1: Write protection (0: deny writes; 1: allow writes).
					pmThis->m_ui8PrgRamProtect = _ui8Val & 0x03;
					break;
				}
				
				// CHR Selects 0...7 ($A000-$DFFF).
				case 0xA000 : {
					pmThis->m_ui8Chr[0] = (pmThis->m_ui8Chr[0] & 0xF0) | (_ui8Val & 0x0F);
					pmThis->SetChrBank<0, ChrBankSize()>( pmThis->m_ui8Chr[0] );
					break;
				}
				case 0xA001 : {
					pmThis->m_ui8Chr[0] = (pmThis->m_ui8Chr[0] & 0x0F) | ((_ui8Val & 0x0F) << 4);
					pmThis->SetChrBank<0, ChrBankSize()>( pmThis->m_ui8Chr[0] );
					break;
				}
				case 0xA002 : {
					pmThis->m_ui8Chr[1] = (pmThis->m_ui8Chr[1] & 0xF0) | (_ui8Val & 0x0F);
					pmThis->SetChrBank<1, ChrBankSize()>( pmThis->m_ui8Chr[1] );
					break;
				}
				case 0xA003 : {
					pmThis->m_ui8Chr[1] = (pmThis->m_ui8Chr[1] & 0x0F) | ((_ui8Val & 0x0F) << 4);
					pmThis->SetChrBank<1, ChrBankSize()>( pmThis->m_ui8Chr[1] );
					break;
				}
				
				case 0xB000 : {
					pmThis->m_ui8Chr[2] = (pmThis->m_ui8Chr[2] & 0xF0) | (_ui8Val & 0x0F);
					pmThis->SetChrBank<2, ChrBankSize()>( pmThis->m_ui8Chr[2] );
					break;
				}
				case 0xB001 : {
					pmThis->m_ui8Chr[2] = (pmThis->m_ui8Chr[2] & 0x0F) | ((_ui8Val & 0x0F) << 4);
					pmThis->SetChrBank<2, ChrBankSize()>( pmThis->m_ui8Chr[2] );
					break;
				}
				case 0xB002 : {
					pmThis->m_ui8Chr[3] = (pmThis->m_ui8Chr[3] & 0xF0) | (_ui8Val & 0x0F);
					pmThis->SetChrBank<3, ChrBankSize()>( pmThis->m_ui8Chr[3] );
					break;
				}
				case 0xB003 : {
					pmThis->m_ui8Chr[3] = (pmThis->m_ui8Chr[3] & 0x0F) | ((_ui8Val & 0x0F) << 4);
					pmThis->SetChrBank<3, ChrBankSize()>( pmThis->m_ui8Chr[3] );
					break;
				}
				
				case 0xC000 : {
					pmThis->m_ui8Chr[4] = (pmThis->m_ui8Chr[4] & 0xF0) | (_ui8Val & 0x0F);
					pmThis->SetChrBank<4, ChrBankSize()>( pmThis->m_ui8Chr[4] );
					break;
				}
				case 0xC001 : {
					pmThis->m_ui8Chr[4] = (pmThis->m_ui8Chr[4] & 0x0F) | ((_ui8Val & 0x0F) << 4);
					pmThis->SetChrBank<4, ChrBankSize()>( pmThis->m_ui8Chr[4] );
					break;
				}
				case 0xC002 : {
					pmThis->m_ui8Chr[5] = (pmThis->m_ui8Chr[5] & 0xF0) | (_ui8Val & 0x0F);
					pmThis->SetChrBank<5, ChrBankSize()>( pmThis->m_ui8Chr[5] );
					break;
				}
				case 0xC003 : {
					pmThis->m_ui8Chr[5] = (pmThis->m_ui8Chr[5] & 0x0F) | ((_ui8Val & 0x0F) << 4);
					pmThis->SetChrBank<5, ChrBankSize()>( pmThis->m_ui8Chr[5] );
					break;
				}
				
				case 0xD000 : {
					pmThis->m_ui8Chr[6] = (pmThis->m_ui8Chr[6] & 0xF0) | (_ui8Val & 0x0F);
					pmThis->SetChrBank<6, ChrBankSize()>( pmThis->m_ui8Chr[6] );
					break;
				}
				case 0xD001 : {
					pmThis->m_ui8Chr[6] = (pmThis->m_ui8Chr[6] & 0x0F) | ((_ui8Val & 0x0F) << 4);
					pmThis->SetChrBank<6, ChrBankSize()>( pmThis->m_ui8Chr[6] );
					break;
				}
				case 0xD002 : {
					pmThis->m_ui8Chr[7] = (pmThis->m_ui8Chr[7] & 0xF0) | (_ui8Val & 0x0F);
					pmThis->SetChrBank<7, ChrBankSize()>( pmThis->m_ui8Chr[7] );
					break;
				}
				case 0xD003 : {
					pmThis->m_ui8Chr[7] = (pmThis->m_ui8Chr[7] & 0x0F) | ((_ui8Val & 0x0F) << 4);
					pmThis->SetChrBank<7, ChrBankSize()>( pmThis->m_ui8Chr[7] );
					break;
				}
				
				// IRQ Reload Value ($E000-$EFFF).
				case 0xE000 : {
					pmThis->m_ui16IrqReload = (pmThis->m_ui16IrqReload & 0xFFF0) | (_ui8Val & 0x0F);
					break;
				}
				case 0xE001 : {
					pmThis->m_ui16IrqReload = (pmThis->m_ui16IrqReload & 0xFF0F) | ((uint16_t(_ui8Val & 0x0F)) << 4);
					break;
				}
				case 0xE002 : {
					pmThis->m_ui16IrqReload = (pmThis->m_ui16IrqReload & 0xF0FF) | ((uint16_t(_ui8Val & 0x0F)) << 8);
					break;
				}
				case 0xE003 : {
					pmThis->m_ui16IrqReload = (pmThis->m_ui16IrqReload & 0x0FFF) | ((uint16_t(_ui8Val & 0x0F)) << 12);
					break;
				}
				
				// IRQ Reload / Acknowledge ($F000).
				case 0xF000 : {
					pmThis->m_ui16IrqCounter = pmThis->m_ui16IrqReload;
					pmThis->m_bIrqStatus = false;
					pmThis->m_pInterruptable->ClearIrq( LSN_IS_MAPPER );
					break;
				}
				// IRQ Counter Size ($F001).
				case 0xF001 : {
					pmThis->m_ui8IrqControl = _ui8Val & 0x0F;
					pmThis->m_bIrqStatus = false;
					pmThis->m_pInterruptable->ClearIrq( LSN_IS_MAPPER );
					break;
				}
				// Mirroring Control ($F002).
				case 0xF002 : {
					uint8_t ui8Mirror = _ui8Val & 0x03;
					switch ( ui8Mirror ) {
						case 0 : { pmThis->m_mmMirror = LSN_MM_HORIZONTAL; break; }
						case 1 : { pmThis->m_mmMirror = LSN_MM_VERTICAL; break; }
						case 2 : { pmThis->m_mmMirror = LSN_MM_1_SCREEN_A; break; }
						case 3 : { pmThis->m_mmMirror = LSN_MM_1_SCREEN_B; break; }
					}
					break;
				}
				// Expansion Sound ($F003).
				case 0xF003 : {
					break;
				}
			}
		}
	};

}	// namespace lsn

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 004 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper004
	 * \brief Mapper 004 implementation.
	 *
	 * Description: Mapper 004 implementation.
	 */
	class CMapper004 : public CMapperBase {
	public :
		CMapper004() {
		}
		virtual ~CMapper004() {
		}


		// == Functions.
		/**
		 * Gets the PGM bank size.
		 *
		 * \return Returns the size of the PGM banks.
		 */
		static constexpr uint16_t						PgmBankSize() { return 8 * 1024; }

		/**
		 * Gets the CHR bank size.
		 *
		 * \return Returns the size of the CHR banks.
		 */
		static constexpr uint16_t						ChrBankSize() { return 1 * 1024; }

		/**
		 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
		 *
		 * \param _rRom The ROM data.
		 * \param _pcbCpuBase A pointer to the CPU.
		 */
		virtual void									InitWithRom( LSN_ROM &_rRom, CCpuBase * _pcbCpuBase, CPpuBase * _ppbPpuBase, CInterruptable * _piInter, CBussable * _pbPpuBus ) {
			CMapperBase::InitWithRom( _rRom, _pcbCpuBase, _ppbPpuBase, _piInter, _pbPpuBus );
			SanitizeRegs<PgmBankSize(), ChrBankSize()>();
			// Used when $8000.D6 == 1.
			SetPgmBank<2, PgmBankSize()>( -2 );

			m_ui8Reg0 = m_ui8Reg1 = 0;
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
			// Set the reads of the fixed bank at the end.		
			m_stFixedOffset = std::max<size_t>( m_prRom->vPrgRom.size(), PgmBankSize() ) - PgmBankSize();
			for ( uint32_t I = 0xE000; I < 0x10000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead_Fixed, this, uint16_t( (I - 0xE000) % (m_prRom->vPrgRom.size() - m_stFixedOffset) ) );
			}
			

			// ================
			// SWAPPABLE BANKS
			// ================
			// CPU.
			for ( uint32_t I = 0x8000; I < 0xA000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &Read_PGM_8000_9FFF, this, uint16_t( I - 0x8000 ) );
			}
			for ( uint32_t I = 0xA000; I < 0xC000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<1, PgmBankSize()>, this, uint16_t( I - 0xA000 ) );
			}
			for ( uint32_t I = 0xC000; I < 0xE000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &Read_PGM_C000_DFFF, this, uint16_t( I - 0xC000 ) );
			}
			/*for ( uint32_t I = 0xE000; I < 0x10000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<3, PgmBankSize()>, this, uint16_t( I - 0xE000 ) );
			}*/
			// PPU.
			if ( m_prRom->vChrRom.size() ) {
				for ( uint32_t I = 0x0000; I < 0x0400; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &Read_CHR_0000_03FF, this, uint16_t( I - 0x0000 ) );
				}
				for ( uint32_t I = 0x0400; I < 0x0800; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &Read_CHR_0400_07FF, this, uint16_t( I - 0x0400 ) );
				}
				for ( uint32_t I = 0x0800; I < 0x0C00; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &Read_CHR_0800_0BFF, this, uint16_t( I - 0x0800 ) );
				}
				for ( uint32_t I = 0x0C00; I < 0x1000; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &Read_CHR_0C00_0FFF, this, uint16_t( I - 0x0C00 ) );
				}
				for ( uint32_t I = 0x1000; I < 0x1400; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &Read_CHR_1000_13FF, this, uint16_t( I - 0x1000 ) );
				}
				for ( uint32_t I = 0x1400; I < 0x1800; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &Read_CHR_1400_17FF, this, uint16_t( I - 0x1400 ) );
				}
				for ( uint32_t I = 0x1800; I < 0x1C00; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &Read_CHR_1800_1BFF, this, uint16_t( I - 0x1800 ) );
				}
				for ( uint32_t I = 0x1C00; I < 0x2000; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &Read_CHR_1C00_1FFF, this, uint16_t( I - 0x1C00 ) );
				}
			}
			// RAM.
			for ( uint32_t I = 0x6000; I < 0x8000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &Mapper004PgmRamRead, this, uint16_t( I - 0x6000 ) );
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &Mapper004PgmRamWrite, this, uint16_t( I - 0x6000 ) );
			}


			// ================
			// BANK-SELECT
			// ================
			// PGM bank-select.
			for ( uint32_t I = 0x8000; I < 0xA000; ++I ) {
				if ( (I & 1) == 0 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper004::SelectBank8000_9FFE, this, 0 );
				}
				else {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper004::SelectBank8001_9FFF, this, 0 );
				}
			}
			for ( uint32_t I = 0xA000; I < 0xC000; ++I ) {
				if ( (I & 1) == 0 ) {
					if ( m_prRom->riInfo.mmMirroring != LSN_MM_4_SCREENS ) {
						_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper004::SelectBankA000_BFFE, this, 0 );
					}
				}
				else {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper004::SelectBankA001_BFFF, this, 0 );
				}
			}
			// IRQ regs: $C000-$DFFF (even/odd), $E000-$FFFF (even/odd).
			for ( uint32_t I = 0xC000; I < 0xE000; ++I ) {
				if ( (I & 1) == 0 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper004::Write_C000_DFFE, this, 0 );	// Latch.
				}
				else {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper004::Write_C001_DFFF, this, 0 );	// Reload.
				}
			}
			for ( uint32_t I = 0xE000; I < 0x10000; ++I ) {
				if ( (I & 1) == 0 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper004::Write_E000_FFFE, this, 0 );	// Disable/ack.
				}
				else {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper004::Write_E001_FFFF, this, 0 );	// Enable.
				}
			}



			// ================
			// MIRRORING
			// ================
			if ( m_prRom->riInfo.mmMirroring == LSN_MM_4_SCREENS ) {
				//ApplyMirroring( LSN_MM_4_SCREENS, _pbPpuBus, this );
			}
			else {
				ApplyControllableMirrorMap( _pbPpuBus );
			}
		}


	protected :
		// == Members.
		/** PRG RAM, up to 8 kilobytes. Optional. */
		uint8_t											m_ui8PrgRam[8*1024];
		/** Register 1. */
		uint8_t											m_ui8Reg0;
		/** Register 2. */
		uint8_t											m_ui8Reg1;
		/** Register 3. */
		uint8_t											m_ui8Reg2;
		/** Register 4. */
		uint8_t											m_ui8Reg3;
		/** Bank mode set on $8000-$9FFE, even. */
		uint8_t											m_ui8BankMode;
		/** CHR mode set on $8000-$9FFE, even. */
		uint8_t											m_ui8ChrMode;

		/** IRQ latch ($C000). */
		uint8_t											m_ui8IrqLatch = 0;
		/** IRQ counter (internal). */
		uint8_t											m_ui8IrqCounter = 0;
		/** IRQ enable flag ($E001). */
		bool											m_bIrqEnabled = false;
		/** Reload pending flag (set by $C001, consumed on next qualified rise). */
		bool											m_bIrqReloadPending = false;

		/** Selects galternate/oldh behavior (NEC) vs gnewh (Sharp). */
		bool											m_bAltIrqBehavior = false;   // false = Sharp/new; true = NEC/old.

		/**
		 * A12 must be low for at least this many **PPU dots** before a rising edge
		 * is considered "filtered" (equivalent to 3 M2 falling edges).
		 */
		static constexpr uint32_t						LSN_MMC3_A12_LOW_CPU = 3;

		/** Last CPU cycle at which A12 was observed low (resets the low window). */
		uint64_t										m_ui64LastA12LowCpu = 0;

		/** Last RAW A12 level we observed (false = low, true = high). */
		bool											m_bA12PrevRaw = false;

		/** Whether the **filtered** A12 is currently considered high. */
		bool											m_bA12FilteredHigh = false;

		/** Accumulated CPU cycles that A12 has been low since the last high->low transition. */
		uint64_t                                        m_ui64A12LowAccumCpu = 0;

		/** Last CPU cycle stamp seen by the A12 sampler (to accumulate deltas). */
		uint64_t                                        m_ui64A12LastCpuStamp = 0;




		// == Functions.
		/**
		 * Bank select ($8000-$9FFE, even).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank8000_9FFE( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper004 * pmThis = reinterpret_cast<CMapper004 *>(_pvParm0);
			pmThis->m_ui8Reg0 = _ui8Val;
		}

		/**
		 * Bank data ($8001-$9FFF, odd).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank8001_9FFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper004 * pmThis = reinterpret_cast<CMapper004 *>(_pvParm0);
			/*
			 *	7  bit  0
			 *	---- ----
			 *	CPMx xRRR
			 *	|||   |||
			 *	|||   +++- Specify which bank register to update on next write to Bank Data register
			 *	|||          000: R0: Select 2 KB CHR bank at PPU $0000-$07FF (or $1000-$17FF)
			 *	|||          001: R1: Select 2 KB CHR bank at PPU $0800-$0FFF (or $1800-$1FFF)
			 *	|||          010: R2: Select 1 KB CHR bank at PPU $1000-$13FF (or $0000-$03FF)
			 *	|||          011: R3: Select 1 KB CHR bank at PPU $1400-$17FF (or $0400-$07FF)
			 *	|||          100: R4: Select 1 KB CHR bank at PPU $1800-$1BFF (or $0800-$0BFF)
			 *	|||          101: R5: Select 1 KB CHR bank at PPU $1C00-$1FFF (or $0C00-$0FFF)
			 *	|||          110: R6: Select 8 KB PRG ROM bank at $8000-$9FFF (or $C000-$DFFF)
			 *	|||          111: R7: Select 8 KB PRG ROM bank at $A000-$BFFF
			 *	||+------- Nothing on the MMC3, see MMC6
			 *	|+-------- PRG ROM bank mode (0: $8000-$9FFF swappable,
			 *	|                                $C000-$DFFF fixed to second-last bank;
			 *	|                             1: $C000-$DFFF swappable,
			 *	|                                $8000-$9FFF fixed to second-last bank)
			 *	+--------- CHR A12 inversion (0: two 2 KB banks at $0000-$0FFF,
			 *									 four 1 KB banks at $1000-$1FFF;
			 *								  1: two 2 KB banks at $1000-$1FFF,
			 *									 four 1 KB banks at $0000-$0FFF)
			 */
			switch ( pmThis->m_ui8Reg0 & 0b111 ) {
				case 0b000 : {
					// 000: R0: Select 2 KB CHR bank at PPU $0000-$07FF (or $1000-$17FF)
					pmThis->SetChrBank2x<0, ChrBankSize()>( _ui8Val & 0b11111110 );
					break;
				}
				case 0b001 : {
					// 001: R1: Select 2 KB CHR bank at PPU $0800-$0FFF (or $1800-$1FFF)
					pmThis->SetChrBank2x<2, ChrBankSize()>( _ui8Val & 0b11111110 );
					break;
				}
				case 0b010 : {
					// 010: R2: Select 1 KB CHR bank at PPU $1000-$13FF (or $0000-$03FF)
					pmThis->SetChrBank<4, ChrBankSize()>( _ui8Val );
					break;
				}
				case 0b011 : {
					// 011: R3: Select 1 KB CHR bank at PPU $1400-$17FF (or $0400-$07FF)
					pmThis->SetChrBank<5, ChrBankSize()>( _ui8Val );
					break;
				}
				case 0b100 : {
					// 100: R4: Select 1 KB CHR bank at PPU $1800-$1BFF (or $0800-$0BFF)
					pmThis->SetChrBank<6, ChrBankSize()>( _ui8Val );
					break;
				}
				case 0b101 : {
					// 101: R5: Select 1 KB CHR bank at PPU $1C00-$1FFF (or $0C00-$0FFF)
					pmThis->SetChrBank<7, ChrBankSize()>( _ui8Val );
					break;
				}
				case 0b110 : {
					// 110: R6: Select 8 KB PRG ROM bank at $8000-$9FFF (or $C000-$DFFF)
					pmThis->SetPgmBank<0, PgmBankSize()>( _ui8Val & 0b00111111 );
					break;
				}
				case 0b111 : {
					// 111: R7: Select 8 KB PRG ROM bank at $A000-$BFFF)
					pmThis->SetPgmBank<1, PgmBankSize()>( _ui8Val & 0b00111111 );
					break;
				}
			}
		}

		/**
		 * Mirroring ($A000-$BFFE, even).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBankA000_BFFE( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper004 * pmThis = reinterpret_cast<CMapper004 *>(_pvParm0);
			/*
			 *	7  bit  0
			 *	---- ----
			 *	xxxx xxxM
			 *			|
			 *			+- Nametable mirroring (0: vertical; 1: horizontal)
			 */
			switch ( _ui8Val & 1 ) {
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
		 * PRG RAM protect ($A001-$BFFF, odd).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBankA001_BFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper004 * pmThis = reinterpret_cast<CMapper004 *>(_pvParm0);
			/*
			 *	7  bit  0
			 *	---- ----
			 *	RWXX xxxx
			 *	||||
			 *	||++------ Nothing on the MMC3, see MMC6
			 *	|+-------- Write protection (0: allow writes; 1: deny writes)
			 *	+--------- PRG RAM chip enable (0: disable; 1: enable)
			 */
			pmThis->m_ui8Reg1 = _ui8Val;
		}

		/**
		 * IRQ latch ($C000-$DFFE, even).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write_C000_DFFE( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper004 * pmThis = reinterpret_cast<CMapper004 *>(_pvParm0);
			/**
			 * 7  bit  0
			 * ---- ----
			 * DDDD DDDD
			 * |||| ||||
			 * ++++-++++- IRQ latch value
			 * 
			 * This register specifies the IRQ counter reload value. When the IRQ counter is zero (or a reload is requested through $C001), this value will be copied to the IRQ counter at the NEXT rising edge of the PPU address, presumably at PPU cycle 260 of the current scanline.
			 */
			pmThis->m_ui8IrqLatch = _ui8Val;
		}

		/**
		 * IRQ reload ($C001-$DFFF, odd).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write_C001_DFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t /*_ui8Val*/ ) {
			CMapper004 * pmThis = reinterpret_cast<CMapper004 *>(_pvParm0);
			/**
			 * 7  bit  0
			 * ---- ----
			 * xxxx xxxx
			 * 
			 * Writing any value to this register clears the MMC3 IRQ counter immediately, and then reloads it at the NEXT rising edge of the PPU address, presumably at PPU cycle 260 of the current scanline.
			 */
			pmThis->m_ui8IrqCounter = 0;
			pmThis->m_bIrqReloadPending = true;
			pmThis->m_bA12FilteredHigh = false;
		}

		/**
		 * IRQ disable ($E000-$FFFE, even).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write_E000_FFFE( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t /*_ui8Val*/ ) {
			CMapper004 * pmThis = reinterpret_cast<CMapper004 *>(_pvParm0);
			/**
			 * 7  bit  0
			 * ---- ----
			 * xxxx xxxx
			 * 
			 * Writing any value to this register will disable MMC3 interrupts AND acknowledge any pending interrupts.
			 */
			pmThis->m_bIrqEnabled = false;
			pmThis->m_pInterruptable->ClearIrq( LSN_IS_MAPPER );
		}

		/**
		 * IRQ enable ($E001-$FFFF, odd).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write_E001_FFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t /*_ui8Val*/ ) {
			CMapper004 * pmThis = reinterpret_cast<CMapper004 *>(_pvParm0);
			/**
			 * 7  bit  0
			 * ---- ----
			 * xxxx xxxx
			 * 
			 * Writing any value to this register will enable MMC3 interrupts.
			 */
			pmThis->m_bIrqEnabled = true;
		}


		/**
		 * Handles reads fromm 0x8000-0x9FFF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Read_PGM_8000_9FFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper004 * pmThis = reinterpret_cast<CMapper004 *>(_pvParm0);
			if ( pmThis->m_ui8Reg0 & 0b01000000 ) {
				// $8000.D6 = 1
				_ui8Ret = pmThis->m_prRom->vPrgRom.data()[pmThis->m_ui8PgmBanks[2]*PgmBankSize()+_ui16Parm1];
			}
			else {
				// $8000.D6 = 0
				_ui8Ret = pmThis->m_prRom->vPrgRom.data()[pmThis->m_ui8PgmBanks[0]*PgmBankSize()+_ui16Parm1];
			}
		}

		/**
		 * Handles reads fromm 0xC000-0xDFFF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Read_PGM_C000_DFFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper004 * pmThis = reinterpret_cast<CMapper004 *>(_pvParm0);
			if ( pmThis->m_ui8Reg0 & 0b01000000 ) {
				// $8000.D6 = 1
				_ui8Ret = pmThis->m_prRom->vPrgRom.data()[pmThis->m_ui8PgmBanks[0]*PgmBankSize()+_ui16Parm1];
			}
			else {
				// $8000.D6 = 0
				_ui8Ret = pmThis->m_prRom->vPrgRom.data()[pmThis->m_ui8PgmBanks[2]*PgmBankSize()+_ui16Parm1];
			}
		}

		/**
		 * Handles reads fromm $0000-$03FF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Read_CHR_0000_03FF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper004 * pmThis = reinterpret_cast<CMapper004 *>(_pvParm0);
			if ( pmThis->m_ui8Reg0 & 0b10000000 ) {
				// $8000.D7 = 1
				_ui8Ret = pmThis->m_prRom->vChrRom.data()[pmThis->m_ui8ChrBanks[4]*ChrBankSize()+_ui16Parm1];
			}
			else {
				// $8000.D7 = 0
				_ui8Ret = pmThis->m_prRom->vChrRom.data()[pmThis->m_ui8ChrBanks[0]*ChrBankSize()+_ui16Parm1];
			}
			pmThis->Mmc3_OnPpuA12Sample( uint16_t( 0x0000 + _ui16Parm1 ) );
		}

		/**
		 * Handles reads fromm $0400-$07FF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Read_CHR_0400_07FF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper004 * pmThis = reinterpret_cast<CMapper004 *>(_pvParm0);
			if ( pmThis->m_ui8Reg0 & 0b10000000 ) {
				// $8000.D7 = 1
				_ui8Ret = pmThis->m_prRom->vChrRom.data()[pmThis->m_ui8ChrBanks[5]*ChrBankSize()+_ui16Parm1];
			}
			else {
				// $8000.D7 = 0
				_ui8Ret = pmThis->m_prRom->vChrRom.data()[pmThis->m_ui8ChrBanks[1]*ChrBankSize()+_ui16Parm1];
			}
			pmThis->Mmc3_OnPpuA12Sample( uint16_t( 0x0400 + _ui16Parm1 ) );
		}

		/**
		 * Handles reads fromm $0800-$0BFF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Read_CHR_0800_0BFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper004 * pmThis = reinterpret_cast<CMapper004 *>(_pvParm0);
			if ( pmThis->m_ui8Reg0 & 0b10000000 ) {
				// $8000.D7 = 1
				_ui8Ret = pmThis->m_prRom->vChrRom.data()[pmThis->m_ui8ChrBanks[6]*ChrBankSize()+_ui16Parm1];
			}
			else {
				// $8000.D7 = 0
				_ui8Ret = pmThis->m_prRom->vChrRom.data()[pmThis->m_ui8ChrBanks[2]*ChrBankSize()+_ui16Parm1];
			}
			pmThis->Mmc3_OnPpuA12Sample( uint16_t( 0x0800 + _ui16Parm1 ) );
		}

		/**
		 * Handles reads fromm $0C00-$0FFF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Read_CHR_0C00_0FFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper004 * pmThis = reinterpret_cast<CMapper004 *>(_pvParm0);
			if ( pmThis->m_ui8Reg0 & 0b10000000 ) {
				// $8000.D7 = 1
				_ui8Ret = pmThis->m_prRom->vChrRom.data()[pmThis->m_ui8ChrBanks[7]*ChrBankSize()+_ui16Parm1];
			}
			else {
				// $8000.D7 = 0
				_ui8Ret = pmThis->m_prRom->vChrRom.data()[pmThis->m_ui8ChrBanks[3]*ChrBankSize()+_ui16Parm1];
			}
			pmThis->Mmc3_OnPpuA12Sample( uint16_t( 0x0C00 + _ui16Parm1 ) );
		}

		/**
		 * Handles reads fromm $1000-$13FF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Read_CHR_1000_13FF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper004 * pmThis = reinterpret_cast<CMapper004 *>(_pvParm0);
			if ( pmThis->m_ui8Reg0 & 0b10000000 ) {
				// $8000.D7 = 1
				_ui8Ret = pmThis->m_prRom->vChrRom.data()[pmThis->m_ui8ChrBanks[0]*ChrBankSize()+_ui16Parm1];
			}
			else {
				// $8000.D7 = 0
				_ui8Ret = pmThis->m_prRom->vChrRom.data()[pmThis->m_ui8ChrBanks[4]*ChrBankSize()+_ui16Parm1];
			}
			pmThis->Mmc3_OnPpuA12Sample( uint16_t( 0x1000 + _ui16Parm1 ) );
		}

		/**
		 * Handles reads fromm $1400-$17FF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Read_CHR_1400_17FF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper004 * pmThis = reinterpret_cast<CMapper004 *>(_pvParm0);
			if ( pmThis->m_ui8Reg0 & 0b10000000 ) {
				// $8000.D7 = 1
				_ui8Ret = pmThis->m_prRom->vChrRom.data()[pmThis->m_ui8ChrBanks[1]*ChrBankSize()+_ui16Parm1];
			}
			else {
				// $8000.D7 = 0
				_ui8Ret = pmThis->m_prRom->vChrRom.data()[pmThis->m_ui8ChrBanks[5]*ChrBankSize()+_ui16Parm1];
			}
			pmThis->Mmc3_OnPpuA12Sample( uint16_t( 0x1400 + _ui16Parm1 ) );
		}

		/**
		 * Handles reads fromm $1800-$1BFF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Read_CHR_1800_1BFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper004 * pmThis = reinterpret_cast<CMapper004 *>(_pvParm0);
			if ( pmThis->m_ui8Reg0 & 0b10000000 ) {
				// $8000.D7 = 1
				_ui8Ret = pmThis->m_prRom->vChrRom.data()[pmThis->m_ui8ChrBanks[2]*ChrBankSize()+_ui16Parm1];
			}
			else {
				// $8000.D7 = 0
				_ui8Ret = pmThis->m_prRom->vChrRom.data()[pmThis->m_ui8ChrBanks[6]*ChrBankSize()+_ui16Parm1];
			}
			pmThis->Mmc3_OnPpuA12Sample( uint16_t( 0x1800 + _ui16Parm1 ) );
		}

		/**
		 * Handles reads fromm $1C00-$1FFF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Read_CHR_1C00_1FFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper004 * pmThis = reinterpret_cast<CMapper004 *>(_pvParm0);
			if ( pmThis->m_ui8Reg0 & 0b10000000 ) {
				// $8000.D7 = 1
				_ui8Ret = pmThis->m_prRom->vChrRom.data()[pmThis->m_ui8ChrBanks[3]*ChrBankSize()+_ui16Parm1];
			}
			else {
				// $8000.D7 = 0
				_ui8Ret = pmThis->m_prRom->vChrRom.data()[pmThis->m_ui8ChrBanks[7]*ChrBankSize()+_ui16Parm1];
			}
			pmThis->Mmc3_OnPpuA12Sample( uint16_t( 0x1C00 + _ui16Parm1 ) );
		}

		/**
		 * Reads from the PGM RAM.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Mapper004PgmRamRead( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper004 * pmThis = reinterpret_cast<CMapper004 *>(_pvParm0);
			/*
			 *	7  bit  0
			 *	---- ----
			 *	RWXX xxxx
			 *	||||
			 *	||++------ Nothing on the MMC3, see MMC6
			 *	|+-------- Write protection (0: allow writes; 1: deny writes)
			 *	+--------- PRG RAM chip enable (0: disable; 1: enable)
			 */
			if ( pmThis->m_ui8Reg1 & 0b10000000 ) {
				_ui8Ret = pmThis->m_ui8PrgRam[_ui16Parm1];
			}
		}

		/**
		 * Writes to the PGM RAM.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Mapper004PgmRamWrite( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper004 * pmThis = reinterpret_cast<CMapper004 *>(_pvParm0);
			/*
			 *	7  bit  0
			 *	---- ----
			 *	RWXX xxxx
			 *	||||
			 *	||++------ Nothing on the MMC3, see MMC6
			 *	|+-------- Write protection (0: allow writes; 1: deny writes)
			 *	+--------- PRG RAM chip enable (0: disable; 1: enable)
			 */
			if ( (pmThis->m_ui8Reg1 & 0b11000000) == 0b10000000 ) {
				pmThis->m_ui8PrgRam[_ui16Parm1] = _ui8Val;
			}
			
		}

		/**
		 * Tracks PPU A12 usage (PPU domain) and clocks the MMC3 counter on qualified filtered 0->1 edges.
		 * Debounce is measured in **CPU cycles** so PAL/NTSC differences are handled exactly as on hardware.
		 *
		 * Arming/Disarming Rules:
		 *  - We "arm" the edge detector when A12 has been LOW for >= 3 CPU cycles. This arming happens either
		 *    (a) on a true HIGH->LOW transition, or (b) if we observe LOW long enough even without seeing an
		 *    immediate transition (covers entering a scanline already LOW).
		 *  - When armed, the first qualified LOW->HIGH clocks the counter and disarms.
		 *  - Writing $C001 (reload request) also forces arming so the very next qualified edge will take effect.
		 *
		 * Call from **every** PPU access to $0000-$1FFF (your CHR read handlers already do this).
		 *
		 * @param _ui16PpuAddr The absolute PPU address being accessed (0x0000-0x1FFF).
		 */
		inline void										Mmc3_OnPpuA12Sample( uint16_t _ui16PpuAddr ) {
			const bool bA12Raw = ((_ui16PpuAddr & 0x1000) != 0);
			const uint64_t ui64CpuNow = m_pcbCpu->GetCycleCount();

			// Track first observation to avoid bogus early clocks.
			static bool s_bInit = false;
			if ( !s_bInit ) {
				s_bInit = true;
				m_bA12PrevRaw = bA12Raw;
				m_ui64LastA12LowCpu = ui64CpuNow - LSN_MMC3_A12_LOW_CPU;	// so a currently-LOW bus can arm immediately after 3 cycles
				m_bA12FilteredHigh = !bA12Raw;								// if we start LOW, consider "disarmed" until we arm below
			}

			if ( !bA12Raw ) {
				// RAW LOW
				if ( m_bA12PrevRaw ) {
					// True HIGH->LOW transition: start the low window now and disarm.
					m_ui64LastA12LowCpu = ui64CpuNow;
					m_bA12FilteredHigh = false;
				}
				else {
					// Staying LOW: if we've been LOW long enough, arm even without a detected transition.
					const uint64_t ui64LowCycles = (ui64CpuNow - m_ui64LastA12LowCpu);
					if ( ui64LowCycles >= static_cast<uint64_t>( LSN_MMC3_A12_LOW_CPU ) ) {
						m_bA12FilteredHigh = false;		// "armed" (ready to accept next qualified rise)
					}
				}
				m_bA12PrevRaw = false;
				return;
			}

			// RAW HIGH
			if ( !m_bA12PrevRaw ) {
				// Candidate LOW->HIGH transition. Check that the low period met the >= 3 CPU-cycle requirement.
				const uint64_t ui64LowCycles = (ui64CpuNow - m_ui64LastA12LowCpu);
				if ( !m_bA12FilteredHigh && ui64LowCycles >= static_cast<uint64_t>( LSN_MMC3_A12_LOW_CPU ) ) {
					// Qualified filtered 0->1 edge ¨ clock exactly once, then disarm until we go LOW again.
					Mmc3_ClockIrqCounter();
					m_bA12FilteredHigh = true;
				}
			}
			m_bA12PrevRaw = true;
		}

		/**
		 * Clocks the MMC3 scanline counter on a *filtered* A12 rising edge.
		 * 
		 * Rules:
		 *  - If reload-pending is set or the counter is 0, load counter := latch; else decrement.
		 *  - Sharp/new asserts when (counter == 0).
		 *  - NEC/alternate asserts on (1 -> 0) transition (by decrement or reload).
		 *  - $C000 == 0 Ë Sharp fires every scanline; NEC fires once until $C001 forces a reload.
		 *  - $E000 disables/acks output only; $E001 enables; the counter always runs.
		 */
		inline void										Mmc3_ClockIrqCounter() {
			bool bAssert = false;

			if ( m_bAltIrqBehavior ) {
				const bool bReload = m_bIrqReloadPending || (m_ui8IrqCounter == 0);
				if ( bReload ) {
					const uint8_t ui8Prev = m_ui8IrqCounter;
					m_ui8IrqCounter = m_ui8IrqLatch;
					m_bIrqReloadPending = false;
					if ( m_bIrqEnabled && ui8Prev == 1 && m_ui8IrqCounter == 0 ) { bAssert = true; }
				}
				else {
					const uint8_t ui8Prev = m_ui8IrqCounter;
					--m_ui8IrqCounter;
					if ( m_bIrqEnabled && ui8Prev == 1 ) { bAssert = true; }
				}
			}
			else {
				if ( m_bIrqReloadPending || (m_ui8IrqCounter == 0) ) {
					m_ui8IrqCounter = m_ui8IrqLatch;
					m_bIrqReloadPending = false;
				}
				else {
					--m_ui8IrqCounter;
				}
				if ( m_bIrqEnabled && m_ui8IrqCounter == 0 ) { bAssert = true; }
			}

			if ( bAssert ) {
				m_pInterruptable->Irq( LSN_IS_MAPPER );
			}
		}

		///**
		// * Must be called on every PPU access to $0000-$1FFF (CHR space): pattern fetches during rendering
		// * and any CPU-driven CHR access via $2006/$2007 that targets $1000-$1FFF.  This function applies
		// * the A12 filter ("A12 remained low for >= 3 M2 falling edges") and, upon a qualified 0->1 filtered
		// * transition, clocks the MMC3 scanline counter.
		// * 
		// * @param _ui16PpuAddr The absolute PPU address being accessed (0x0000-0x1FFF).
		// */
		//inline void										Mmc3_OnPpuChrAccess( uint16_t _ui16PpuAddr ) {
		//	const bool bA12Raw = ((_ui16PpuAddr & 0x1000) != 0);

		//	if ( !bA12Raw ) {
		//		// Raw-low: filtered is forced low; low-time accumulation happens in Mmc3_OnM2Falling().
		//		m_bA12PrevFiltered = false;
		//		return;
		//	}

		//	// Raw-high: check for *filtered* 0->1 rise (requires filtered was low AND low-time >= 3).
		//	if ( !m_bA12PrevFiltered && (m_ui8M2LowCnt >= 3) ) {
		//		// Qualified filtered rising edge: clock the counter.
		//		Mmc3_ClockIrqCounter();
		//		m_bA12PrevFiltered = true;
		//		// Do not clear m_ui8M2LowCnt here; it is rebuilt while A12 is low.
		//	}
		//	// Else: remain filtered-low (insufficient low-time), or stay filtered-high (already high).
		//}

	};

}	// namespace lsn

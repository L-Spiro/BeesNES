/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 119 implementation.
 */

#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapper004.h"

namespace lsn {

	/**
	 * Class CMapper119
	 * \brief Mapper 119 implementation.
	 *
	 * Description: Mapper 119 implementation (TQROM).
	 */
	class CMapper119 : public CMapper004 {
	public :
		/**
		 * Constructor.
		 */
		CMapper119() {
		}

		/**
		 * Destructor.
		 */
		virtual ~CMapper119() {
		}


		// == Functions.
		/**
		 * Initializes the mapper with the ROM data.
		 *
		 * \param _rRom The ROM data.
		 * \param _pcbCpuBase A pointer to the CPU.
		 * \param _ppbPpuBase A pointer to the PPU.
		 * \param _piInter A pointer to the interruptable device.
		 * \param _pbPpuBus A pointer to the PPU bus.
		 */
		virtual void									InitWithRom( LSN_ROM &_rRom, CCpuBase * _pcbCpuBase, CPpuBase * _ppbPpuBase, CInterruptable * _piInter, CBussable * _pbPpuBus ) override {
			CMapper004::InitWithRom( _rRom, _pcbCpuBase, _ppbPpuBase, _piInter, _pbPpuBus );
			std::memset( m_ui8RawChr, 0, sizeof( m_ui8RawChr ) );
			std::memset( m_ui8ChrRam, 0, sizeof( m_ui8ChrRam ) );
		}

		/**
		 * Applies mapping to the CPU and PPU busses.
		 *
		 * \param _pbCpuBus A pointer to the CPU bus.
		 * \param _pbPpuBus A pointer to the PPU bus.
		 */
		virtual void									ApplyMap( CCpuBus * _pbCpuBus, CPpuBus * _pbPpuBus ) override {
			CMapper004::ApplyMap( _pbCpuBus, _pbPpuBus );

			// ================
			// OVERRIDE BANK-SELECT
			// ================
			for ( uint32_t I = 0x8000; I < 0xA000; ++I ) {
				if ( (I & 1) != 0 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper119::SelectBank8001_9FFF_119, this, 0 );
				}
			}

			// ================
			// OVERRIDE PPU SWAPPABLE BANKS
			// ================
			for ( uint32_t I = 0x0000; I < 0x0400; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper119::Read_CHR_0000_03FF, this, uint16_t( I - 0x0000 ) );
				_pbPpuBus->SetWriteFunc( uint16_t( I ), &CMapper119::Write_CHR_0000_03FF, this, uint16_t( I - 0x0000 ) );
			}
			for ( uint32_t I = 0x0400; I < 0x0800; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper119::Read_CHR_0400_07FF, this, uint16_t( I - 0x0400 ) );
				_pbPpuBus->SetWriteFunc( uint16_t( I ), &CMapper119::Write_CHR_0400_07FF, this, uint16_t( I - 0x0400 ) );
			}
			for ( uint32_t I = 0x0800; I < 0x0C00; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper119::Read_CHR_0800_0BFF, this, uint16_t( I - 0x0800 ) );
				_pbPpuBus->SetWriteFunc( uint16_t( I ), &CMapper119::Write_CHR_0800_0BFF, this, uint16_t( I - 0x0800 ) );
			}
			for ( uint32_t I = 0x0C00; I < 0x1000; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper119::Read_CHR_0C00_0FFF, this, uint16_t( I - 0x0C00 ) );
				_pbPpuBus->SetWriteFunc( uint16_t( I ), &CMapper119::Write_CHR_0C00_0FFF, this, uint16_t( I - 0x0C00 ) );
			}
			for ( uint32_t I = 0x1000; I < 0x1400; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper119::Read_CHR_1000_13FF, this, uint16_t( I - 0x1000 ) );
				_pbPpuBus->SetWriteFunc( uint16_t( I ), &CMapper119::Write_CHR_1000_13FF, this, uint16_t( I - 0x1000 ) );
			}
			for ( uint32_t I = 0x1400; I < 0x1800; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper119::Read_CHR_1400_17FF, this, uint16_t( I - 0x1400 ) );
				_pbPpuBus->SetWriteFunc( uint16_t( I ), &CMapper119::Write_CHR_1400_17FF, this, uint16_t( I - 0x1400 ) );
			}
			for ( uint32_t I = 0x1800; I < 0x1C00; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper119::Read_CHR_1800_1BFF, this, uint16_t( I - 0x1800 ) );
				_pbPpuBus->SetWriteFunc( uint16_t( I ), &CMapper119::Write_CHR_1800_1BFF, this, uint16_t( I - 0x1800 ) );
			}
			for ( uint32_t I = 0x1C00; I < 0x2000; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper119::Read_CHR_1C00_1FFF, this, uint16_t( I - 0x1C00 ) );
				_pbPpuBus->SetWriteFunc( uint16_t( I ), &CMapper119::Write_CHR_1C00_1FFF, this, uint16_t( I - 0x1C00 ) );
			}
		}

		/**
		 * Called to inform the mapper of a reset.
		 */
		virtual void									Reset() override {
			CMapper004::Reset();
			std::memset( m_ui8RawChr, 0, sizeof( m_ui8RawChr ) );
		}


	protected :
		// == Members.
		/** Raw CHR bank registers (8 1KB windows). */
		uint8_t											m_ui8RawChr[8];
		/** CHR RAM (8KB). */
		uint8_t											m_ui8ChrRam[8 * 1024];


		// == Functions.
		/**
		 * Translates an MMC3 CHR bank into a TQROM physical CHR ROM bank.
		 *
		 * \param _ui8RawMmc3Bank The raw inner MMC3 bank index.
		 * \return Returns the translated physical CHR ROM bank index.
		 */
		inline uint8_t									GetChrRomBank( uint8_t _ui8RawMmc3Bank ) const {
			size_t stBanks = m_prRom->vChrRom.size() / ChrBankSize();
			if LSN_LIKELY( stBanks ) {
				return uint8_t( (_ui8RawMmc3Bank & 0b00111111) % stBanks );
			}
			return 0;
		}

		/**
		 * Translates an MMC3 CHR bank into a TQROM physical CHR RAM bank.
		 *
		 * \param _ui8RawMmc3Bank The raw inner MMC3 bank index.
		 * \return Returns the translated physical CHR RAM bank index.
		 */
		inline uint8_t									GetChrRamBank( uint8_t _ui8RawMmc3Bank ) const {
			return (_ui8RawMmc3Bank & 0b00000111) % 8;
		}

		/**
		 * Checks if the MMC3 CHR bank points to CHR RAM.
		 *
		 * \param _ui8RawMmc3Bank The raw inner MMC3 bank index.
		 * \return Returns true if the bank selects CHR RAM.
		 */
		inline bool										IsChrRam( uint8_t _ui8RawMmc3Bank ) const {
			return (_ui8RawMmc3Bank & 0b01000000) != 0;
		}

		/**
		 * Bank data ($8001-$9FFF, odd).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank8001_9FFF_119( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t _ui8Val ) {
			CMapper119 * pmThis = reinterpret_cast<CMapper119 *>(_pvParm0);
			
			uint8_t ui8Reg = pmThis->m_ui8Reg0 & 0b111;
			switch ( ui8Reg ) {
				case 0b000 : {
					pmThis->m_ui8RawChr[0] = _ui8Val & 0b11111110;
					pmThis->m_ui8RawChr[1] = (_ui8Val & 0b11111110) | 1;
					break;
				}
				case 0b001 : {
					pmThis->m_ui8RawChr[2] = _ui8Val & 0b11111110;
					pmThis->m_ui8RawChr[3] = (_ui8Val & 0b11111110) | 1;
					break;
				}
				case 0b010 : {
					pmThis->m_ui8RawChr[4] = _ui8Val;
					break;
				}
				case 0b011 : {
					pmThis->m_ui8RawChr[5] = _ui8Val;
					break;
				}
				case 0b100 : {
					pmThis->m_ui8RawChr[6] = _ui8Val;
					break;
				}
				case 0b101 : {
					pmThis->m_ui8RawChr[7] = _ui8Val;
					break;
				}
			}

			CMapper004::SelectBank8001_9FFF( _pvParm0, _ui16Parm1, _pui8Data, _ui8Val );
		}


		// ================
		// PPU READ HOOKS
		// ================
		/**
		 * Handles reads from $0000-$03FF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Read_CHR_0000_03FF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper119 * pmThis = reinterpret_cast<CMapper119 *>(_pvParm0);
			uint8_t ui8RawMmc3Bank = (pmThis->m_ui8Reg0 & 0b10000000) ? pmThis->m_ui8RawChr[4] : pmThis->m_ui8RawChr[0];
			if ( pmThis->IsChrRam( ui8RawMmc3Bank ) ) {
				_ui8Ret = pmThis->m_ui8ChrRam[pmThis->GetChrRamBank( ui8RawMmc3Bank ) * ChrBankSize() + _ui16Parm1];
			}
			else {
				_ui8Ret = pmThis->m_prRom->vChrRom.data()[pmThis->GetChrRomBank( ui8RawMmc3Bank ) * ChrBankSize() + _ui16Parm1];
			}
			pmThis->Mmc3_OnPpuA12Sample( uint16_t( 0x0000 + _ui16Parm1 ) );
		}

		/**
		 * Handles reads from $0400-$07FF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Read_CHR_0400_07FF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper119 * pmThis = reinterpret_cast<CMapper119 *>(_pvParm0);
			uint8_t ui8RawMmc3Bank = (pmThis->m_ui8Reg0 & 0b10000000) ? pmThis->m_ui8RawChr[5] : pmThis->m_ui8RawChr[1];
			if ( pmThis->IsChrRam( ui8RawMmc3Bank ) ) {
				_ui8Ret = pmThis->m_ui8ChrRam[pmThis->GetChrRamBank( ui8RawMmc3Bank ) * ChrBankSize() + _ui16Parm1];
			}
			else {
				_ui8Ret = pmThis->m_prRom->vChrRom.data()[pmThis->GetChrRomBank( ui8RawMmc3Bank ) * ChrBankSize() + _ui16Parm1];
			}
			pmThis->Mmc3_OnPpuA12Sample( uint16_t( 0x0400 + _ui16Parm1 ) );
		}

		/**
		 * Handles reads from $0800-$0BFF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Read_CHR_0800_0BFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper119 * pmThis = reinterpret_cast<CMapper119 *>(_pvParm0);
			uint8_t ui8RawMmc3Bank = (pmThis->m_ui8Reg0 & 0b10000000) ? pmThis->m_ui8RawChr[6] : pmThis->m_ui8RawChr[2];
			if ( pmThis->IsChrRam( ui8RawMmc3Bank ) ) {
				_ui8Ret = pmThis->m_ui8ChrRam[pmThis->GetChrRamBank( ui8RawMmc3Bank ) * ChrBankSize() + _ui16Parm1];
			}
			else {
				_ui8Ret = pmThis->m_prRom->vChrRom.data()[pmThis->GetChrRomBank( ui8RawMmc3Bank ) * ChrBankSize() + _ui16Parm1];
			}
			pmThis->Mmc3_OnPpuA12Sample( uint16_t( 0x0800 + _ui16Parm1 ) );
		}

		/**
		 * Handles reads from $0C00-$0FFF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Read_CHR_0C00_0FFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper119 * pmThis = reinterpret_cast<CMapper119 *>(_pvParm0);
			uint8_t ui8RawMmc3Bank = (pmThis->m_ui8Reg0 & 0b10000000) ? pmThis->m_ui8RawChr[7] : pmThis->m_ui8RawChr[3];
			if ( pmThis->IsChrRam( ui8RawMmc3Bank ) ) {
				_ui8Ret = pmThis->m_ui8ChrRam[pmThis->GetChrRamBank( ui8RawMmc3Bank ) * ChrBankSize() + _ui16Parm1];
			}
			else {
				_ui8Ret = pmThis->m_prRom->vChrRom.data()[pmThis->GetChrRomBank( ui8RawMmc3Bank ) * ChrBankSize() + _ui16Parm1];
			}
			pmThis->Mmc3_OnPpuA12Sample( uint16_t( 0x0C00 + _ui16Parm1 ) );
		}

		/**
		 * Handles reads from $1000-$13FF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Read_CHR_1000_13FF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper119 * pmThis = reinterpret_cast<CMapper119 *>(_pvParm0);
			uint8_t ui8RawMmc3Bank = (pmThis->m_ui8Reg0 & 0b10000000) ? pmThis->m_ui8RawChr[0] : pmThis->m_ui8RawChr[4];
			if ( pmThis->IsChrRam( ui8RawMmc3Bank ) ) {
				_ui8Ret = pmThis->m_ui8ChrRam[pmThis->GetChrRamBank( ui8RawMmc3Bank ) * ChrBankSize() + _ui16Parm1];
			}
			else {
				_ui8Ret = pmThis->m_prRom->vChrRom.data()[pmThis->GetChrRomBank( ui8RawMmc3Bank ) * ChrBankSize() + _ui16Parm1];
			}
			pmThis->Mmc3_OnPpuA12Sample( uint16_t( 0x1000 + _ui16Parm1 ) );
		}

		/**
		 * Handles reads from $1400-$17FF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Read_CHR_1400_17FF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper119 * pmThis = reinterpret_cast<CMapper119 *>(_pvParm0);
			uint8_t ui8RawMmc3Bank = (pmThis->m_ui8Reg0 & 0b10000000) ? pmThis->m_ui8RawChr[1] : pmThis->m_ui8RawChr[5];
			if ( pmThis->IsChrRam( ui8RawMmc3Bank ) ) {
				_ui8Ret = pmThis->m_ui8ChrRam[pmThis->GetChrRamBank( ui8RawMmc3Bank ) * ChrBankSize() + _ui16Parm1];
			}
			else {
				_ui8Ret = pmThis->m_prRom->vChrRom.data()[pmThis->GetChrRomBank( ui8RawMmc3Bank ) * ChrBankSize() + _ui16Parm1];
			}
			pmThis->Mmc3_OnPpuA12Sample( uint16_t( 0x1400 + _ui16Parm1 ) );
		}

		/**
		 * Handles reads from $1800-$1BFF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Read_CHR_1800_1BFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper119 * pmThis = reinterpret_cast<CMapper119 *>(_pvParm0);
			uint8_t ui8RawMmc3Bank = (pmThis->m_ui8Reg0 & 0b10000000) ? pmThis->m_ui8RawChr[2] : pmThis->m_ui8RawChr[6];
			if ( pmThis->IsChrRam( ui8RawMmc3Bank ) ) {
				_ui8Ret = pmThis->m_ui8ChrRam[pmThis->GetChrRamBank( ui8RawMmc3Bank ) * ChrBankSize() + _ui16Parm1];
			}
			else {
				_ui8Ret = pmThis->m_prRom->vChrRom.data()[pmThis->GetChrRomBank( ui8RawMmc3Bank ) * ChrBankSize() + _ui16Parm1];
			}
			pmThis->Mmc3_OnPpuA12Sample( uint16_t( 0x1800 + _ui16Parm1 ) );
		}

		/**
		 * Handles reads from $1C00-$1FFF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Read_CHR_1C00_1FFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper119 * pmThis = reinterpret_cast<CMapper119 *>(_pvParm0);
			uint8_t ui8RawMmc3Bank = (pmThis->m_ui8Reg0 & 0b10000000) ? pmThis->m_ui8RawChr[3] : pmThis->m_ui8RawChr[7];
			if ( pmThis->IsChrRam( ui8RawMmc3Bank ) ) {
				_ui8Ret = pmThis->m_ui8ChrRam[pmThis->GetChrRamBank( ui8RawMmc3Bank ) * ChrBankSize() + _ui16Parm1];
			}
			else {
				_ui8Ret = pmThis->m_prRom->vChrRom.data()[pmThis->GetChrRomBank( ui8RawMmc3Bank ) * ChrBankSize() + _ui16Parm1];
			}
			pmThis->Mmc3_OnPpuA12Sample( uint16_t( 0x1C00 + _ui16Parm1 ) );
		}


		// ================
		// PPU WRITE HOOKS
		// ================
		/**
		 * Handles writes to $0000-$03FF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write_CHR_0000_03FF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper119 * pmThis = reinterpret_cast<CMapper119 *>(_pvParm0);
			uint8_t ui8RawMmc3Bank = (pmThis->m_ui8Reg0 & 0b10000000) ? pmThis->m_ui8RawChr[4] : pmThis->m_ui8RawChr[0];
			if ( pmThis->IsChrRam( ui8RawMmc3Bank ) ) {
				pmThis->m_ui8ChrRam[pmThis->GetChrRamBank( ui8RawMmc3Bank ) * ChrBankSize() + _ui16Parm1] = _ui8Val;
			}
			pmThis->Mmc3_OnPpuA12Sample( uint16_t( 0x0000 + _ui16Parm1 ) );
		}

		/**
		 * Handles writes to $0400-$07FF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write_CHR_0400_07FF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper119 * pmThis = reinterpret_cast<CMapper119 *>(_pvParm0);
			uint8_t ui8RawMmc3Bank = (pmThis->m_ui8Reg0 & 0b10000000) ? pmThis->m_ui8RawChr[5] : pmThis->m_ui8RawChr[1];
			if ( pmThis->IsChrRam( ui8RawMmc3Bank ) ) {
				pmThis->m_ui8ChrRam[pmThis->GetChrRamBank( ui8RawMmc3Bank ) * ChrBankSize() + _ui16Parm1] = _ui8Val;
			}
			pmThis->Mmc3_OnPpuA12Sample( uint16_t( 0x0400 + _ui16Parm1 ) );
		}

		/**
		 * Handles writes to $0800-$0BFF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write_CHR_0800_0BFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper119 * pmThis = reinterpret_cast<CMapper119 *>(_pvParm0);
			uint8_t ui8RawMmc3Bank = (pmThis->m_ui8Reg0 & 0b10000000) ? pmThis->m_ui8RawChr[6] : pmThis->m_ui8RawChr[2];
			if ( pmThis->IsChrRam( ui8RawMmc3Bank ) ) {
				pmThis->m_ui8ChrRam[pmThis->GetChrRamBank( ui8RawMmc3Bank ) * ChrBankSize() + _ui16Parm1] = _ui8Val;
			}
			pmThis->Mmc3_OnPpuA12Sample( uint16_t( 0x0800 + _ui16Parm1 ) );
		}

		/**
		 * Handles writes to $0C00-$0FFF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write_CHR_0C00_0FFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper119 * pmThis = reinterpret_cast<CMapper119 *>(_pvParm0);
			uint8_t ui8RawMmc3Bank = (pmThis->m_ui8Reg0 & 0b10000000) ? pmThis->m_ui8RawChr[7] : pmThis->m_ui8RawChr[3];
			if ( pmThis->IsChrRam( ui8RawMmc3Bank ) ) {
				pmThis->m_ui8ChrRam[pmThis->GetChrRamBank( ui8RawMmc3Bank ) * ChrBankSize() + _ui16Parm1] = _ui8Val;
			}
			pmThis->Mmc3_OnPpuA12Sample( uint16_t( 0x0C00 + _ui16Parm1 ) );
		}

		/**
		 * Handles writes to $1000-$13FF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write_CHR_1000_13FF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper119 * pmThis = reinterpret_cast<CMapper119 *>(_pvParm0);
			uint8_t ui8RawMmc3Bank = (pmThis->m_ui8Reg0 & 0b10000000) ? pmThis->m_ui8RawChr[0] : pmThis->m_ui8RawChr[4];
			if ( pmThis->IsChrRam( ui8RawMmc3Bank ) ) {
				pmThis->m_ui8ChrRam[pmThis->GetChrRamBank( ui8RawMmc3Bank ) * ChrBankSize() + _ui16Parm1] = _ui8Val;
			}
			pmThis->Mmc3_OnPpuA12Sample( uint16_t( 0x1000 + _ui16Parm1 ) );
		}

		/**
		 * Handles writes to $1400-$17FF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write_CHR_1400_17FF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper119 * pmThis = reinterpret_cast<CMapper119 *>(_pvParm0);
			uint8_t ui8RawMmc3Bank = (pmThis->m_ui8Reg0 & 0b10000000) ? pmThis->m_ui8RawChr[1] : pmThis->m_ui8RawChr[5];
			if ( pmThis->IsChrRam( ui8RawMmc3Bank ) ) {
				pmThis->m_ui8ChrRam[pmThis->GetChrRamBank( ui8RawMmc3Bank ) * ChrBankSize() + _ui16Parm1] = _ui8Val;
			}
			pmThis->Mmc3_OnPpuA12Sample( uint16_t( 0x1400 + _ui16Parm1 ) );
		}

		/**
		 * Handles writes to $1800-$1BFF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write_CHR_1800_1BFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper119 * pmThis = reinterpret_cast<CMapper119 *>(_pvParm0);
			uint8_t ui8RawMmc3Bank = (pmThis->m_ui8Reg0 & 0b10000000) ? pmThis->m_ui8RawChr[2] : pmThis->m_ui8RawChr[6];
			if ( pmThis->IsChrRam( ui8RawMmc3Bank ) ) {
				pmThis->m_ui8ChrRam[pmThis->GetChrRamBank( ui8RawMmc3Bank ) * ChrBankSize() + _ui16Parm1] = _ui8Val;
			}
			pmThis->Mmc3_OnPpuA12Sample( uint16_t( 0x1800 + _ui16Parm1 ) );
		}

		/**
		 * Handles writes to $1C00-$1FFF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write_CHR_1C00_1FFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper119 * pmThis = reinterpret_cast<CMapper119 *>(_pvParm0);
			uint8_t ui8RawMmc3Bank = (pmThis->m_ui8Reg0 & 0b10000000) ? pmThis->m_ui8RawChr[3] : pmThis->m_ui8RawChr[7];
			if ( pmThis->IsChrRam( ui8RawMmc3Bank ) ) {
				pmThis->m_ui8ChrRam[pmThis->GetChrRamBank( ui8RawMmc3Bank ) * ChrBankSize() + _ui16Parm1] = _ui8Val;
			}
			pmThis->Mmc3_OnPpuA12Sample( uint16_t( 0x1C00 + _ui16Parm1 ) );
		}
	};

}	// namespace lsn

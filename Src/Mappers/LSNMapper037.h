/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 037 implementation.
 */

#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapper004.h"

namespace lsn {

	/**
	 * Class CMapper037
	 * \brief Mapper 037 implementation.
	 *
	 * Description: Mapper 037 implementation (Super Mario Bros + Tetris + Nintendo World Cup).
	 */
	class CMapper037 : public CMapper004 {
	public :
		CMapper037() {}
		virtual ~CMapper037() {}


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
			m_ui8Q = 0;
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
			// OVERRIDE PRG RAM (Outer Bank Register)
			// ================
			for ( uint32_t I = 0x6000; I < 0x8000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CCpuBus::NoRead, this, 0 );
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper037::SelectBank6000_7FFF, this, 0 );
			}

			// ================
			// OVERRIDE CPU SWAPPABLE BANKS
			// ================
			for ( uint32_t I = 0x8000; I < 0xA000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper037::Read_PGM_8000_9FFF, this, uint16_t( I - 0x8000 ) );
			}
			for ( uint32_t I = 0xA000; I < 0xC000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper037::Read_PGM_A000_BFFF, this, uint16_t( I - 0xA000 ) );
			}
			for ( uint32_t I = 0xC000; I < 0xE000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper037::Read_PGM_C000_DFFF, this, uint16_t( I - 0xC000 ) );
			}
			for ( uint32_t I = 0xE000; I < 0x10000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper037::Read_PGM_E000_FFFF, this, uint16_t( I - 0xE000 ) );
			}

			// ================
			// OVERRIDE PPU SWAPPABLE BANKS
			// ================
			if ( m_prRom->vChrRom.size() ) {
				for ( uint32_t I = 0x0000; I < 0x0400; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper037::Read_CHR_0000_03FF, this, uint16_t( I - 0x0000 ) );
				}
				for ( uint32_t I = 0x0400; I < 0x0800; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper037::Read_CHR_0400_07FF, this, uint16_t( I - 0x0400 ) );
				}
				for ( uint32_t I = 0x0800; I < 0x0C00; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper037::Read_CHR_0800_0BFF, this, uint16_t( I - 0x0800 ) );
				}
				for ( uint32_t I = 0x0C00; I < 0x1000; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper037::Read_CHR_0C00_0FFF, this, uint16_t( I - 0x0C00 ) );
				}
				for ( uint32_t I = 0x1000; I < 0x1400; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper037::Read_CHR_1000_13FF, this, uint16_t( I - 0x1000 ) );
				}
				for ( uint32_t I = 0x1400; I < 0x1800; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper037::Read_CHR_1400_17FF, this, uint16_t( I - 0x1400 ) );
				}
				for ( uint32_t I = 0x1800; I < 0x1C00; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper037::Read_CHR_1800_1BFF, this, uint16_t( I - 0x1800 ) );
				}
				for ( uint32_t I = 0x1C00; I < 0x2000; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper037::Read_CHR_1C00_1FFF, this, uint16_t( I - 0x1C00 ) );
				}
			}
		}

		/**
		 * Called to inform the mapper of a reset.
		 */
		virtual void									Reset() override {
			CMapper004::Reset();
			m_ui8Q = 0;
		}


	protected :
		// == Members.
		/** Outer bank selection latch. */
		uint8_t											m_ui8Q = 0;


		// == Functions.
		/**
		 * Applies the Mapper 37 hardware logic to MMC3's PRG output.
		 * 
		 * \param _ui8Mmc3Bank The raw inner MMC3 bank index.
		 * \return Returns the translated physical bank index.
		 */
		inline uint8_t									GetEffectivePgmBank( uint8_t _ui8Mmc3Bank ) const {
			uint8_t ui8Q0 = m_ui8Q & 1;
			uint8_t ui8Q1 = (m_ui8Q >> 1) & 1;
			uint8_t ui8Q2 = (m_ui8Q >> 2) & 1;
			
			uint8_t ui8M16 = (_ui8Mmc3Bank >> 3) & 1;
			
			uint8_t ui8A16 = (ui8Q0 & ui8Q1) | (ui8Q2 & ui8M16);
			
			uint8_t ui8A17 = ui8Q2;
			
			return (_ui8Mmc3Bank & 0b111) | (ui8A16 << 3) | (ui8A17 << 4);
		}
		
		/**
		 * Applies the Mapper 37 hardware logic to MMC3's CHR output.
		 * 
		 * \param _ui8Mmc3Bank The raw inner MMC3 bank index.
		 * \return Returns the translated physical bank index.
		 */
		inline uint8_t									GetEffectiveChrBank( uint8_t _ui8Mmc3Bank ) const {
			uint8_t ui8Q2 = (m_ui8Q >> 2) & 1;
			return (_ui8Mmc3Bank & 0b01111111) | (ui8Q2 << 7);
		}

		/**
		 * Outer Bank Select ($6000-$7FFF).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank6000_7FFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper037 * pmThis = reinterpret_cast<CMapper037 *>(_pvParm0);
			if ( (pmThis->m_ui8Reg1 & 0b11000000) == 0b10000000 ) {
				pmThis->m_ui8Q = _ui8Val & 0x07;
			}
		}


		// ================
		// CPU READ HOOKS
		// ================
		/**
		 * Handles reads from 0x8000-0x9FFF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Read_PGM_8000_9FFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper037 * pmThis = reinterpret_cast<CMapper037 *>(_pvParm0);
			uint8_t ui8Mmc3Bank = (pmThis->m_ui8Reg0 & 0b01000000) ? pmThis->m_ui8PgmBanks[2] : pmThis->m_ui8PgmBanks[0];
			_ui8Ret = pmThis->m_prRom->vPrgRom.data()[pmThis->GetEffectivePgmBank( ui8Mmc3Bank )*PgmBankSize()+_ui16Parm1];
		}

		/**
		 * Handles reads from 0xA000-0xBFFF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Read_PGM_A000_BFFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper037 * pmThis = reinterpret_cast<CMapper037 *>(_pvParm0);
			uint8_t ui8Mmc3Bank = pmThis->m_ui8PgmBanks[1];
			_ui8Ret = pmThis->m_prRom->vPrgRom.data()[pmThis->GetEffectivePgmBank( ui8Mmc3Bank )*PgmBankSize()+_ui16Parm1];
		}

		/**
		 * Handles reads from 0xC000-0xDFFF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Read_PGM_C000_DFFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper037 * pmThis = reinterpret_cast<CMapper037 *>(_pvParm0);
			uint8_t ui8Mmc3Bank = (pmThis->m_ui8Reg0 & 0b01000000) ? pmThis->m_ui8PgmBanks[0] : pmThis->m_ui8PgmBanks[2];
			_ui8Ret = pmThis->m_prRom->vPrgRom.data()[pmThis->GetEffectivePgmBank( ui8Mmc3Bank )*PgmBankSize()+_ui16Parm1];
		}

		/**
		 * Handles reads from 0xE000-0xFFFF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Read_PGM_E000_FFFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper037 * pmThis = reinterpret_cast<CMapper037 *>(_pvParm0);
			uint8_t ui8Mmc3Bank = pmThis->GetPgmBank<PgmBankSize()>( -1 );
			_ui8Ret = pmThis->m_prRom->vPrgRom.data()[pmThis->GetEffectivePgmBank( ui8Mmc3Bank )*PgmBankSize()+_ui16Parm1];
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
			CMapper037 * pmThis = reinterpret_cast<CMapper037 *>(_pvParm0);
			uint8_t ui8Mmc3Bank = (pmThis->m_ui8Reg0 & 0b10000000) ? pmThis->m_ui8ChrBanks[4] : pmThis->m_ui8ChrBanks[0];
			_ui8Ret = pmThis->m_prRom->vChrRom.data()[pmThis->GetEffectiveChrBank( ui8Mmc3Bank )*ChrBankSize()+_ui16Parm1];
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
			CMapper037 * pmThis = reinterpret_cast<CMapper037 *>(_pvParm0);
			uint8_t ui8Mmc3Bank = (pmThis->m_ui8Reg0 & 0b10000000) ? pmThis->m_ui8ChrBanks[5] : pmThis->m_ui8ChrBanks[1];
			_ui8Ret = pmThis->m_prRom->vChrRom.data()[pmThis->GetEffectiveChrBank( ui8Mmc3Bank )*ChrBankSize()+_ui16Parm1];
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
			CMapper037 * pmThis = reinterpret_cast<CMapper037 *>(_pvParm0);
			uint8_t ui8Mmc3Bank = (pmThis->m_ui8Reg0 & 0b10000000) ? pmThis->m_ui8ChrBanks[6] : pmThis->m_ui8ChrBanks[2];
			_ui8Ret = pmThis->m_prRom->vChrRom.data()[pmThis->GetEffectiveChrBank( ui8Mmc3Bank )*ChrBankSize()+_ui16Parm1];
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
			CMapper037 * pmThis = reinterpret_cast<CMapper037 *>(_pvParm0);
			uint8_t ui8Mmc3Bank = (pmThis->m_ui8Reg0 & 0b10000000) ? pmThis->m_ui8ChrBanks[7] : pmThis->m_ui8ChrBanks[3];
			_ui8Ret = pmThis->m_prRom->vChrRom.data()[pmThis->GetEffectiveChrBank( ui8Mmc3Bank )*ChrBankSize()+_ui16Parm1];
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
			CMapper037 * pmThis = reinterpret_cast<CMapper037 *>(_pvParm0);
			uint8_t ui8Mmc3Bank = (pmThis->m_ui8Reg0 & 0b10000000) ? pmThis->m_ui8ChrBanks[0] : pmThis->m_ui8ChrBanks[4];
			_ui8Ret = pmThis->m_prRom->vChrRom.data()[pmThis->GetEffectiveChrBank( ui8Mmc3Bank )*ChrBankSize()+_ui16Parm1];
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
			CMapper037 * pmThis = reinterpret_cast<CMapper037 *>(_pvParm0);
			uint8_t ui8Mmc3Bank = (pmThis->m_ui8Reg0 & 0b10000000) ? pmThis->m_ui8ChrBanks[1] : pmThis->m_ui8ChrBanks[5];
			_ui8Ret = pmThis->m_prRom->vChrRom.data()[pmThis->GetEffectiveChrBank( ui8Mmc3Bank )*ChrBankSize()+_ui16Parm1];
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
			CMapper037 * pmThis = reinterpret_cast<CMapper037 *>(_pvParm0);
			uint8_t ui8Mmc3Bank = (pmThis->m_ui8Reg0 & 0b10000000) ? pmThis->m_ui8ChrBanks[2] : pmThis->m_ui8ChrBanks[6];
			_ui8Ret = pmThis->m_prRom->vChrRom.data()[pmThis->GetEffectiveChrBank( ui8Mmc3Bank )*ChrBankSize()+_ui16Parm1];
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
			CMapper037 * pmThis = reinterpret_cast<CMapper037 *>(_pvParm0);
			uint8_t ui8Mmc3Bank = (pmThis->m_ui8Reg0 & 0b10000000) ? pmThis->m_ui8ChrBanks[3] : pmThis->m_ui8ChrBanks[7];
			_ui8Ret = pmThis->m_prRom->vChrRom.data()[pmThis->GetEffectiveChrBank( ui8Mmc3Bank )*ChrBankSize()+_ui16Parm1];
			pmThis->Mmc3_OnPpuA12Sample( uint16_t( 0x1C00 + _ui16Parm1 ) );
		}
	};

}	// namespace lsn

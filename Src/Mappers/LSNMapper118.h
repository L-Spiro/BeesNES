/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 118 implementation.
 */

#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapper004.h"

namespace lsn {

	/**
	 * Class CMapper118
	 * \brief Mapper 118 implementation.
	 *
	 * Description: Mapper 118 implementation (TxSROM).
	 */
	class CMapper118 : public CMapper004 {
	public :
		CMapper118() {}
		virtual ~CMapper118() {}


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
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper118::SelectBank8001_9FFF_118, this, 0 );
				}
			}
			for ( uint32_t I = 0xA000; I < 0xC000; ++I ) {
				if ( (I & 1) == 0 ) {
					if ( m_prRom->riInfo.mmMirroring != LSN_MM_4_SCREENS ) {
						_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper118::SelectBankA000_BFFE_118, this, 0 );
					}
				}
			}

			// ================
			// OVERRIDE PPU SWAPPABLE BANKS
			// ================
			for ( uint32_t I = 0x2000; I < 0x3F00; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper118::Read_Nametable, this, uint16_t( I ) );
				_pbPpuBus->SetWriteFunc( uint16_t( I ), &CMapper118::Write_Nametable, this, uint16_t( I ) );
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
		/** Raw CHR bank registers (R0-R5). */
		uint8_t											m_ui8RawChr[6];


		// == Functions.
		/**
		 * Calculates the physical CIRAM address for a given PPU address.
		 * 
		 * \param _ui16Addr The PPU address to translate.
		 * \return Returns the translated physical CIRAM address.
		 */
		inline uint16_t									GetCiramAddress( uint16_t _ui16Addr ) const {
			uint16_t ui16Window = _ui16Addr & 0x0C00;
			bool bA12 = (_ui16Addr & 0x1000) != 0;
			bool bChrA12Inversion = (m_ui8Reg0 & 0b10000000) != 0;
			
			uint8_t ui8RawBank = 0;
			
			if ( bA12 != bChrA12Inversion ) {
				switch ( ui16Window ) {
					case 0x0000 : { ui8RawBank = m_ui8RawChr[2]; break; }
					case 0x0400 : { ui8RawBank = m_ui8RawChr[3]; break; }
					case 0x0800 : { ui8RawBank = m_ui8RawChr[4]; break; }
					case 0x0C00 : { ui8RawBank = m_ui8RawChr[5]; break; }
				}
			}
			else {
				switch ( ui16Window ) {
					case 0x0000 : { ui8RawBank = m_ui8RawChr[0]; break; }
					case 0x0400 : { ui8RawBank = m_ui8RawChr[0]; break; }
					case 0x0800 : { ui8RawBank = m_ui8RawChr[1]; break; }
					case 0x0C00 : { ui8RawBank = m_ui8RawChr[1]; break; }
				}
			}
			
			uint16_t ui16A10 = (ui8RawBank & 0x80) ? 0x0400 : 0x0000;
			return LSN_PPU_NAMETABLES | ui16A10 | (_ui16Addr & 0x03FF);
		}

		/**
		 * Handles reads from $2000-$3EFF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Read_Nametable( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t &_ui8Ret ) {
			CMapper118 * pmThis = reinterpret_cast<CMapper118 *>(_pvParm0);
			_ui8Ret = _pui8Data[pmThis->GetCiramAddress( _ui16Parm1 )];
		}

		/**
		 * Handles writes to $2000-$3EFF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write_Nametable( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t _ui8Val ) {
			CMapper118 * pmThis = reinterpret_cast<CMapper118 *>(_pvParm0);
			_pui8Data[pmThis->GetCiramAddress( _ui16Parm1 )] = _ui8Val;
		}

		/**
		 * Bank data ($8001-$9FFF, odd).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank8001_9FFF_118( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t _ui8Val ) {
			CMapper118 * pmThis = reinterpret_cast<CMapper118 *>(_pvParm0);
			
			uint8_t ui8Reg = pmThis->m_ui8Reg0 & 0b111;
			if ( ui8Reg < 6 ) {
				pmThis->m_ui8RawChr[ui8Reg] = _ui8Val;
			}

			CMapper004::SelectBank8001_9FFF( _pvParm0, _ui16Parm1, _pui8Data, _ui8Val );
		}

		/**
		 * Mirroring ($A000-$BFFE, even).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBankA000_BFFE_118( void * /*_pvParm0*/, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t /*_ui8Val*/ ) {
		}
	};

}	// namespace lsn

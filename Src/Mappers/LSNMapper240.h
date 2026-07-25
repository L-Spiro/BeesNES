/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 240 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

#include <algorithm>
#include <vector>

namespace lsn {

	/**
	 * Class CMapper240
	 * \brief Mapper 240 implementation.
	 *
	 * Description: NES Mapper 240.
	 */
	class CMapper240 : public CMapperBase {
	public :
		CMapper240() {
		}
		virtual ~CMapper240() {
			SaveBatteryRam( m_vWram.data(), m_vWram.size() );
		}


		// == Functions.
		/**
		 * Gets the PGM bank size.
		 *
		 * \return Returns the size of the PGM banks (32 KiB).
		 */
		static constexpr uint16_t						PgmBankSize() { return 32 * 1024; }

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

			m_vWram.resize( 8 * 1024 );
			LoadBatteryRam( m_vWram.data(), m_vWram.size() );

			/*SetPgmBank<0, PgmBankSize()>( 0 );
			SetChrBank<0, ChrBankSize()>( 0 );*/
		}

		/**
		 * Called to inform the mapper of a reset.
		 **/
		virtual void									Reset() override {
			SaveBatteryRam( m_vWram.data(), m_vWram.size() );
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
				for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<0, PgmBankSize()>, this, uint16_t( I - 0x8000 ) );
				}
			}
			// PPU.
			if ( m_prRom->vChrRom.size() ) {
				for ( uint32_t I = 0x0000; I < 0x2000; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<0, ChrBankSize()>, this, uint16_t( I - 0x0000 ) );
				}
			}


			// ================
			// BANK-SELECT & RAM
			// ================
			// PRG/CHR Bank Select ($4020-$5FFF).
			for ( uint32_t I = 0x4020; I < 0x6000; ++I ) {
				// Decodes two slightly different PCB behaviors in one sweep.
				// (Jing Ke Xin Zhuan uses $4800 masks; Sheng Huo Lie Zhuan uses $4100 masks)
				if ( ((I & 0xE800) == 0x4800) || ((I & 0xE100) == 0x4100) ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper240::Write4020_5FFF, this, 0 );
				}
			}
			// WRAM ($6000-$7FFF).
			for ( uint32_t I = 0x6000; I < 0x8000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper240::WramRead, this, uint16_t( I - 0x6000 ) );
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper240::WramWrite, this, uint16_t( I - 0x6000 ) );
			}
		}


	protected :
		// == Members.
		/** The 8 KiB WRAM. */
		std::vector<uint8_t>							m_vWram;


		// == Functions.
		/**
		 * Handles latch writes from $4020 to $5FFF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write4020_5FFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper240 * pmThis = reinterpret_cast<CMapper240 *>(_pvParm0);
			
			// $4020-5FFF:  [PPPP CCCC]
			pmThis->SetPgmBank<0, PgmBankSize()>( _ui8Val >> 4 );
			pmThis->SetChrBank<0, ChrBankSize()>( _ui8Val & 0x0F );
		}

		/**
		 * Reads from external WRAM.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						WramRead( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper240 * pmThis = reinterpret_cast<CMapper240 *>(_pvParm0);
			_ui8Ret = pmThis->m_vWram[_ui16Parm1];
		}

		/**
		 * Writes to external WRAM.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						WramWrite( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper240 * pmThis = reinterpret_cast<CMapper240 *>(_pvParm0);
			pmThis->m_vWram[_ui16Parm1] = _ui8Val;
		}
	};

}	// namespace lsn

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 173 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper173
	 * \brief Mapper 173 implementation.
	 *
	 * Description: Mapper 173 implementation.
	 */
	class CMapper173 : public CMapperBase {
	public :
		CMapper173() {
		}
		virtual ~CMapper173() {
		}


		// == Functions.
		/**
		 * Gets the PGM bank size.
		 *
		 * \return Returns the size of the PGM banks.
		 */
		static constexpr uint16_t						PgmBankSize() { return 16 * 1024; }

		/**
		 * Gets the CHR bank size.
		 *
		 * \return Returns the size of the CHR banks.
		 */
		static constexpr uint16_t						ChrBankSize() { return 8 * 1024; }

		/**
		 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
		 *
		 * \param _rRom The ROM data.
		 * \param _pcbCpuBase A pointer to the CPU.
		 */
		virtual void									InitWithRom( LSN_ROM &_rRom, CCpuBase * _pcbCpuBase, CPpuBase * _ppbPpuBase, CInterruptable * _piInter, CBussable * _pbPpuBus ) {
			CMapperBase::InitWithRom( _rRom, _pcbCpuBase, _ppbPpuBase, _piInter, _pbPpuBus );
			SanitizeRegs<PgmBankSize(), ChrBankSize()>();
			
			m_ui8S = m_ui8Rrr = m_ui8Ppp = m_ui8RrrLatch = 0;
			m_bIncrMode = m_bInvMode = m_bInvModeLatch = false;
			
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
				for ( uint32_t I = 0x0000; I < 0x2000; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<0, ChrBankSize()>, this, uint16_t( I - 0x0000 ) );
				}
			}


			// ================
			// BANK-SELECT
			// ================
			// PGM bank-select.
			for ( uint32_t I = 0x4000; I < 0x6000; ++I ) {
				if ( (I & 0xE100) == 0x4100 ) {
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper173::CopyRr4100, this, 0 );
				}
				if ( (I & 0xE103) == 0x4100 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper173::SelectBank4100, this, 0 );
				}
				if ( (I & 0xE103) == 0x4101 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper173::SelectBank4101, this, 0 );
				}
				if ( (I & 0xE103) == 0x4102 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper173::SelectBank4102, this, 0 );
				}
				if ( (I & 0xE103) == 0x4103 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper173::SelectBank4103, this, 0 );
				}
			}
			// Register value.
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper173::SelectBank8000, this, 0 );
			}
		}


	protected :
		// == Members.
		/** Internal RR register. */
		uint8_t											m_ui8Rrr;
		/** Internal PP register. */
		uint8_t											m_ui8Ppp;
		/** Internal S register. */
		uint8_t											m_ui8S;
		/** Latched RR register (updates on $8000). */
		uint8_t											m_ui8RrrLatch;
		/** Increment Mode. */
		bool											m_bIncrMode;
		/** Invert Mode. */
		bool											m_bInvMode;
		/** Latched Invert Mode (updates on $8000). */
		bool											m_bInvModeLatch;


		// == Functions.
		/**
		 * Updates the bank selection.
		 **/
		void inline										UpdateBanks() {
			// Bits 0-1 of the latched RRR register drive PRG A15 and A16.
			uint8_t ui8Prg = m_ui8RrrLatch & 0x03;
			SetPgmBank<0, PgmBankSize()>( ui8Prg << 1 );
			SetPgmBank<1, PgmBankSize()>( (ui8Prg << 1) | 1 );
			
			// Bit 2 of the latched RRR register drives CHR A13.
			// Pin 22 (driving CHR A14) receives the INVERTED state of V (m_bInvMode) when $8000 is written.
			uint8_t ui8Chr = ((m_ui8RrrLatch >> 2) & 0x01) | (m_bInvModeLatch ? 0x00 : 0x02);
			SetChrBank<0, ChrBankSize()>( ui8Chr );
		}

		/**
		 * Reads from the custom latch.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						CopyRr4100( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper173 * pmThis = reinterpret_cast<CMapper173 *>(_pvParm0);
			/**
			 *	Mask: $E100
			 *	 read $4100: [xxxx SRRR]
			 *				  |||| ||||
			 *				  |||| ++++- Copy internal registers 'RRR' and 'S' XOR 'V' to data bus.
			 *				  ++++------ open bus
			 */
			_ui8Ret = (_ui8Ret & ~0b00001111) |
				(pmThis->m_ui8Rrr & 0b00000111) |
				(((pmThis->m_ui8S ^ uint8_t( pmThis->m_bInvMode) ) & 0b1) << 3);
		}

		/**
		 * Update RR.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank4100( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t /*_ui8Val*/ ) {
			CMapper173 * pmThis = reinterpret_cast<CMapper173 *>(_pvParm0);
			/*
			 *	 write $4100: If Increment is set, internal register 'RRR' <- 'RRR'+1
			 *				  Otherwise, if Invert is clear, copy internal register 'PPP' to 'RRR'
			 *							 if Invert is set, copy '~PPP' to 'RRR'
			 *				  'S' is not changed at all.
			 */
			if ( pmThis->m_bIncrMode ) {
				pmThis->m_ui8Rrr = (pmThis->m_ui8Rrr + 1) & 0b00000111;
			}
			else if ( pmThis->m_bInvMode ) {
				pmThis->m_ui8Rrr = (~pmThis->m_ui8Ppp) & 0b00000111;
			}
			else {
				pmThis->m_ui8Rrr = pmThis->m_ui8Ppp & 0b00000111;
			}
		}

		/**
		 * Update Invert Mode.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank4101( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper173 * pmThis = reinterpret_cast<CMapper173 *>(_pvParm0);
			/** write $4101: [.... ...V] - Invert Mode */
			pmThis->m_bInvMode = _ui8Val & 1;
		}

		/**
		 * Copy S and PPP.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank4102( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper173 * pmThis = reinterpret_cast<CMapper173 *>(_pvParm0);
			/** write $4102: [.... SPPP] - Copy data bus to internal registers 'S' and 'PPP'.
			 *                             'S' can be read back immediately; 'PPP' must be copied using $4100 first.
			 */
			pmThis->m_ui8Ppp = _ui8Val & 0b111;
			pmThis->m_ui8S = (_ui8Val >> 3) & 1;
		}

		/**
		 * Update Increment Mode.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank4103( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper173 * pmThis = reinterpret_cast<CMapper173 *>(_pvParm0);
			/** write $4103: [.... ...C] - Increment Mode */
			pmThis->m_bIncrMode = _ui8Val & 1;
		}

		/**
		 * Copy RR to PRG and CHR bank pins.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank8000( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t /*_ui8Val*/ ) {
			CMapper173 * pmThis = reinterpret_cast<CMapper173 *>(_pvParm0);
			/** write $8000: copy internal register 'RRR' and 'V' to output latches. */
			
			pmThis->m_ui8RrrLatch = pmThis->m_ui8Rrr;
			pmThis->m_bInvModeLatch = pmThis->m_bInvMode;
			pmThis->UpdateBanks();
		}
	};

}	// namespace lsn

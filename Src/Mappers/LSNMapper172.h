/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 172 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper172
	 * \brief Mapper 172 implementation.
	 *
	 * Description: NES Mapper 172 (Super Mega P-4070 board) implementation.
	 */
	class CMapper172 : public CMapperBase {
	public :
		CMapper172() {
		}
		virtual ~CMapper172() {
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

			m_ui8Input = 0;
			m_ui8Output = 0;
			m_ui8Register = 0;
			m_bMode = false;
			m_bInvert = false;

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
			if ( m_prRom->vChrRom.size() > 0 ) {
				for ( uint32_t I = 0x0000; I < 0x2000; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<0, ChrBankSize()>, this, uint16_t( I ) );
				}
			}


			// ================
			// BANK-SELECT
			// ================
			// Registers $4100-$4103.
			for ( uint32_t I = 0x4100; I < 0x6000; ++I ) {
				if ( (I & 0xE100) == 0x4100 ) {
					// Read $4100-$4103.
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper172::Read4100_4103, this, 0 );
				}
				
				uint16_t ui16Mask = I & 0xE103;
				if ( ui16Mask == 0x4100 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper172::Write4100, this, 0 );
				}
				else if ( ui16Mask == 0x4101 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper172::Write4101, this, 0 );
				}
				else if ( ui16Mask == 0x4102 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper172::Write4102, this, 0 );
				}
				else if ( ui16Mask == 0x4103 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper172::Write4103, this, 0 );
				}
			}
			
			// Output register.
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper172::SelectBank8000_FFFF, this, 0 );
			}


			// ================
			// MIRRORING
			// ================
			ApplyControllableMirrorMap( _pbPpuBus );
		}


	protected :
		// == Members.
		uint8_t											m_ui8Input;									/**< Internal Input register (6 bits). */
		uint8_t											m_ui8Output;								/**< Internal Output register (6 bits). */
		uint8_t											m_ui8Register;								/**< Internal Register (6 bits). */
		bool											m_bMode;									/**< Mode flag (1 bit). */
		bool											m_bInvert;									/**< Invert flag (1 bit). */


		// == Functions.
		/**
		 * Updates the bank selection.
		 **/
		void inline										UpdateBanks() {
			SetPgmBank<0, PgmBankSize()>( 0 );
			SetPgmBank<1, PgmBankSize()>( 1 );
			
			SetChrBank<0, ChrBankSize()>( m_ui8Output & 0x03 );

			// Nametable mirroring follows the Invert flag upon latching at $8000-$FFFF.
			m_mmMirror = m_bInvert ? LSN_MM_VERTICAL : LSN_MM_HORIZONTAL;
		}
		
		/**
		 * Reverses the lowest 6 bits of a value, simulating the inverted trace lines on the P-4070 board.
		 * 
		 * \param _ui8Val The value to reverse.
		 * \return Returns the reversed 6-bit value.
		 */
		static inline uint8_t							Reverse6( uint8_t _ui8Val ) {
			return uint8_t( ((_ui8Val & 0x01) << 5) |
							((_ui8Val & 0x02) << 3) |
							((_ui8Val & 0x04) << 1) |
							((_ui8Val & 0x08) >> 1) |
							((_ui8Val & 0x10) >> 3) |
							((_ui8Val & 0x20) >> 5) );
		}

		/**
		 * Reads from the custom latch at $4100-$4103.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Read4100_4103( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper172 * pmThis = reinterpret_cast<CMapper172 *>(_pvParm0);
			uint8_t ui8Val = pmThis->m_ui8Register;
			
			// Bits 4-5 are inverted if Invert is set.
			if ( pmThis->m_bInvert ) {
				ui8Val ^= 0x30;
			}
			
			// Bits 6-7 are open bus. Bit order D0-D5 is sent reversed.
			_ui8Ret = (_ui8Ret & 0xC0) | Reverse6( ui8Val );
		}

		/**
		 * Writes to $4100 (Register Update).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write4100( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t /*_ui8Val*/ ) {
			CMapper172 * pmThis = reinterpret_cast<CMapper172 *>(_pvParm0);
			if ( !pmThis->m_bMode ) {
				// Bits 0-5 of Register := Input.
				pmThis->m_ui8Register = pmThis->m_ui8Input;
				
				// Bits 0-3 are inverted if Invert is set.
				if ( pmThis->m_bInvert ) {
					pmThis->m_ui8Register ^= 0x0F;
				}
			}
			else {
				// Bits 0-3 incremented by one. Bits 4-5 are unaffected.
				pmThis->m_ui8Register = (pmThis->m_ui8Register & 0x30) | ((pmThis->m_ui8Register + 1) & 0x0F);
			}
		}

		/**
		 * Writes to $4101 (Invert Mode Update).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write4101( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper172 * pmThis = reinterpret_cast<CMapper172 *>(_pvParm0);
			pmThis->m_bInvert = ((_ui8Val >> 5) & 0x01) != 0;
		}

		/**
		 * Writes to $4102 (Input Update).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write4102( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper172 * pmThis = reinterpret_cast<CMapper172 *>(_pvParm0);
			
			// Mask to 6 bits and reverse to hardware order.
			pmThis->m_ui8Input = Reverse6( _ui8Val & 0x3F );
		}

		/**
		 * Writes to $4103 (Mode Update).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write4103( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper172 * pmThis = reinterpret_cast<CMapper172 *>(_pvParm0);
			pmThis->m_bMode = ((_ui8Val >> 5) & 0x01) != 0;
		}

		/**
		 * Writes to $8000-$FFFF (Output Update & Mirroring).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank8000_FFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t /*_ui8Val*/ ) {
			CMapper172 * pmThis = reinterpret_cast<CMapper172 *>(_pvParm0);
			
			pmThis->m_ui8Output = pmThis->m_ui8Register;
			pmThis->UpdateBanks();
		}
	};

}	// namespace lsn

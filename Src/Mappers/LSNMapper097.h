/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 097 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper097
	 * \brief Mapper 097 implementation.
	 *
	 * Description: Mapper 097 implementation.
	 */
	class CMapper097 : public CMapperBase {
	public :
		CMapper097() {
		}
		virtual ~CMapper097() {
		}


		// == Functions.
		/**
		 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
		 *
		 * \param _rRom The ROM data.
		 * \param _pcbCpuBase A pointer to the CPU.
		 */
		virtual void									InitWithRom( LSN_ROM &_rRom, CCpuBase * _pcbCpuBase ) {
			CMapperBase::InitWithRom( _rRom, _pcbCpuBase );
			SanitizeRegs<0x4000, 0x2000>();
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
			SetPgmBank<0, 0x4000>( -1 );
			

			// ================
			// SWAPPABLE BANKS
			// ================
			// CPU.
			for ( uint32_t I = 0x8000; I < 0xC000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<0, 0x4000>, this, uint16_t( I - 0x8000 ) );
			}
			for ( uint32_t I = 0xC000; I < 0x10000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<1, 0x4000>, this, uint16_t( I - 0xC000 ) );
			}


			// ================
			// BANK-SELECT
			// ================
			// PGM bank-select.
			for ( uint32_t I = 0x8000; I < 0xC000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper097::SelectBank8000_BFFF, this, 0 );	// Treated as ROM.
			}


			// ================
			// MIRRORING
			// ================
			ApplyControllableMirrorMap( _pbPpuBus );
		}


	protected :
		// == Members.


		// == Functions.
		/**
		 * Selects a bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						SelectBank8000_BFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper097 * pmThis = reinterpret_cast<CMapper097 *>(_pvParm0);
			pmThis->SetPgmBank<1, 0x4000>( _ui8Val & 0b11111 );

			switch ( _ui8Val >> 7 ) {
				case 0 : {
					pmThis->m_mmMirror = LSN_MM_HORIZONTAL;
					break;
				}
				case 1 : {
					pmThis->m_mmMirror = LSN_MM_VERTICAL;
					break;
				}
			}
		}
	};

}	// namespace lsn

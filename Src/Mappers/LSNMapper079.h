/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 079 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper079
	 * \brief Mapper 079 implementation.
	 *
	 * Description: Mapper 079 implementation.
	 */
	class CMapper079 : public CMapperBase {
	public :
		CMapper079() {
		}
		virtual ~CMapper079() {
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
			SanitizeRegs<32 * 1024, 8 * 1024>();
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
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<0, 32 * 1024>, this, uint16_t( (I - 0x8000) % m_prRom->vPrgRom.size() ) );
			}
			// PPU.
			for ( uint32_t I = 0x0000; I < 0x2000; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<0, 8 * 1024>, this, uint16_t( I - 0x0000 ) );
			}


			// ================
			// BANK-SELECT
			// ================
			// PGM bank-select.
			for ( uint32_t I = 0x4100; I < 0x6000; ++I ) {
				if ( (I & 0b1110000100000000) == 0b0100000100000000 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper079::SelectBank4100_5FFF, this, 0 );	// Treated as ROM.
				}
			}
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
		static void LSN_FASTCALL						SelectBank4100_5FFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper079 * pmThis = reinterpret_cast<CMapper079 *>(_pvParm0);
			pmThis->SetPgmBank<0, 32 * 1024>( (_ui8Val >> 3) & 1 );
			pmThis->SetChrBank<0, 8 * 1024>( _ui8Val & 0b111 );
		}
	};

}	// namespace lsn

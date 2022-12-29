/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 003 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper003
	 * \brief Mapper 003 implementation.
	 *
	 * Description: Mapper 003 implementation.
	 */
	class CMapper003 : public CMapperBase {
	public :
		CMapper003() {
		}
		virtual ~CMapper003() {
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

			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::StdMapperCpuRead, this, uint16_t( (I - 0x8000) % m_prRom->vPrgRom.size() ) );
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &Mapper003CpuWrite, this, 0 );	// Treated as ROM.
			}

			for ( uint32_t I = 0x0000; I < 0x2000; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead_2000, this, uint16_t( I ) );
			}
		}


	protected :
		// == Members.


		// == Functions.
		/**
		 * CPU ROM-area writes are used to set the CHR bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						Mapper003CpuWrite( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper003 * pmThis = reinterpret_cast<CMapper003 *>(_pvParm0);
			pmThis->SetChrBank<0, 8 * 1024>( _ui8Val & 0x3 );
		}
		
	};

}	// namespace lsn

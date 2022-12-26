/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 087 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper087
	 * \brief Mapper 087 implementation.
	 *
	 * Description: Mapper 087 implementation.
	 */
	class CMapper087 : public CMapperBase {
	public :
		CMapper087() {
		}
		virtual ~CMapper087() {
		}


		// == Functions.
		/**
		 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
		 *
		 * \param _rRom The ROM data.
		 */
		virtual void									InitWithRom( LSN_ROM &_rRom ) {
			CMapperBase::InitWithRom( _rRom );
			m_ui8PgmBank = 0;
			m_ui8ChrBank = uint8_t( m_prRom->vChrRom.size() / (8 * 1024) - 1 );
		}

		/**
		 * Applies mapping to the CPU and PPU busses.
		 *
		 * \param _pbCpuBus A pointer to the CPU bus.
		 * \param _pbPpuBus A pointer to the PPU bus.
		 */
		virtual void									ApplyMap( CCpuBus * _pbCpuBus, CPpuBus * _pbPpuBus ) {
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::StdMapperCpuRead, this, uint16_t( (I - 0x8000) % m_prRom->vPrgRom.size() ) );
			}
			// CHR ROM.
			for ( uint32_t I = 0x0000; I < 0x2000; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead_2000, this, uint16_t( I - 0x0000 ) );
			}

			// Bank select.
			for ( uint32_t I = 0x6000; I < 0x8000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper087::SelectBank6000_7FFF, this, 0 );	// Treated as ROM.
			}
		}


	protected :
		// == Members.


		// == Functions.
		/**
		 * Selects the CHR bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						SelectBank6000_7FFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper087 * pmThis = reinterpret_cast<CMapper087 *>(_pvParm0);
			pmThis->m_ui8ChrBank = (((_ui8Val & 0b01) << 1) & ((_ui8Val & 0b10) >> 1))
				% (pmThis->m_prRom->vChrRom.size() / (8 * 1024));
		}
	};

}	// namespace lsn

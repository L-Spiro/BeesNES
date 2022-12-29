/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 184 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper184
	 * \brief Mapper 184 implementation.
	 *
	 * Description: Mapper 184 implementation.
	 */
	class CMapper184 : public CMapperBase {
	public :
		CMapper184() {
		}
		virtual ~CMapper184() {
		}


		// == Functions.
		/**
		 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
		 *
		 * \param _rRom The ROM data.
		 */
		virtual void									InitWithRom( LSN_ROM &_rRom ) {
			CMapperBase::InitWithRom( _rRom );
			//m_ui8PgmBank = 0;
			m_ui8ChrBank1 = m_ui8ChrBank = 0;
		}

		/**
		 * Applies mapping to the CPU and PPU busses.
		 *
		 * \param _pbCpuBus A pointer to the CPU bus.
		 * \param _pbPpuBus A pointer to the PPU bus.
		 */
		virtual void									ApplyMap( CCpuBus * _pbCpuBus, CPpuBus * _pbPpuBus ) {
			CMapperBase::ApplyMap( _pbCpuBus, _pbPpuBus );

			// Set the reads of the selectable bank.
			for ( uint32_t I = 0x0000; I < 0x1000; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead_1000, this, uint16_t( I - 0x0000 ) );
			}
			for ( uint32_t I = 0x1000; I < 0x2000; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper184::ChrBank1Read_1000, this, uint16_t( I - 0x1000 ) );
			}

			// Writes to the whole area are used to select a bank.
			for ( uint32_t I = 0x6000; I < 0x8000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper184::SelectBank6000_7FFF, this, 0 );	// Treated as ROM.
			}
		}


	protected :
		// == Members.
		/** The 1000-2000 CHR bank. */
		uint8_t											m_ui8ChrBank1;


		// == Functions.
		/**
		 * Reading from CHR ROM via m_ui8ChrBank1.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						ChrBank1Read_1000( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper184 * pmThis = reinterpret_cast<CMapper184 *>(_pvParm0);
			_ui8Ret = pmThis->m_prRom->vChrRom.data()[pmThis->m_ui8ChrBank1*0x1000+_ui16Parm1];
		}

		/**
		 * Selects a bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						SelectBank6000_7FFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper184 * pmThis = reinterpret_cast<CMapper184 *>(_pvParm0);
			pmThis->m_ui8ChrBank1 = ((_ui8Val & 0b01110000) >> 4) % (pmThis->m_prRom->vChrRom.size() / (4 * 1024));
			pmThis->m_ui8ChrBank = (_ui8Val & 0b00000111) % (pmThis->m_prRom->vChrRom.size() / (4 * 1024));
			// The most significant bit of H is always set in hardware. (i.e. its range is 4 to 7)
			pmThis->m_ui8ChrBank1 |= 0x100;
		}
	};

}	// namespace lsn

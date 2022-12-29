/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 078 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper078
	 * \brief Mapper 078 implementation.
	 *
	 * Description: Mapper 078 implementation.
	 */
	class CMapper078 : public CMapperBase {
	public :
		CMapper078() {
		}
		virtual ~CMapper078() {
		}


		// == Functions.
		/**
		 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
		 *
		 * \param _rRom The ROM data.
		 */
		virtual void									InitWithRom( LSN_ROM &_rRom ) {
			CMapperBase::InitWithRom( _rRom );
			SanitizeRegs<16 * 1024, 8 * 1024>();
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
			// Set the reads of the fixed bank at the end.		
			m_stFixedOffset = std::max<size_t>( m_prRom->vPrgRom.size(), (16 * 1024) ) - (16 * 1024);
			for ( uint32_t I = 0xC000; I < 0x10000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead_Fixed, this, uint16_t( I - 0xC000 ) );
			}
			

			// ================
			// SWAPPABLE BANKS
			// ================
			// CPU.
			for ( uint32_t I = 0x8000; I < 0xC000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead_4000, this, uint16_t( I - 0x8000 ) );
			}
			// PPU.
			for ( uint32_t I = 0x0000; I < 0x2000; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead_2000, this, uint16_t( I - 0x0000 ) );
			}


			// ================
			// BANK-SELECT
			// ================
			// PGM bank-select.
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper078::SelectBank8000_FFFF, this, 0 );	// Treated as ROM.
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
		static void LSN_FASTCALL						SelectBank8000_FFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper078 * pmThis = reinterpret_cast<CMapper078 *>(_pvParm0);
			pmThis->SetPgmBank<0, 16 * 1024>( _ui8Val & 0b0111 );
			pmThis->SetChrBank<0, 8 * 1024>( _ui8Val >> 4 );

			switch ( (_ui8Val >> 3) & 1 ) {
				case 0 : {
					if ( pmThis->m_prRom->riInfo.ui32Crc == 0xBC1197A4 ) {
						pmThis->m_mmMirror = LSN_MM_HORIZONTAL;
					}
					else {
						pmThis->m_mmMirror = LSN_MM_1_SCREEN_A;
					}
					break;
				}
				case 1 : {
					if ( pmThis->m_prRom->riInfo.ui32Crc == 0xBC1197A4 ) {
						pmThis->m_mmMirror = LSN_MM_VERTICAL;
					}
					else {
						pmThis->m_mmMirror = LSN_MM_1_SCREEN_B;
					}
					break;
				}
			}
		}
	};

}	// namespace lsn

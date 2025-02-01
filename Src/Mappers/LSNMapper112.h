/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 112 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper112
	 * \brief Mapper 112 implementation.
	 *
	 * Description: Mapper 112 implementation.
	 */
	class CMapper112 : public CMapperBase {
	public :
		CMapper112() {
		}
		virtual ~CMapper112() {
		}


		// == Functions.
		/**
		 * Gets the PGM bank size.
		 *
		 * \return Returns the size of the PGM banks.
		 */
		static constexpr uint16_t						PgmBankSize() { return 0x2000; }

		/**
		 * Gets the CHR bank size.
		 *
		 * \return Returns the size of the CHR banks.
		 */
		static constexpr uint16_t						ChrBankSize() { return 0x0400; }

		/**
		 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
		 *
		 * \param _rRom The ROM data.
		 * \param _pcbCpuBase A pointer to the CPU.
		 */
		virtual void									InitWithRom( LSN_ROM &_rRom, CCpuBase * _pcbCpuBase, CBussable * _pbPpuBus ) {
			CMapperBase::InitWithRom( _rRom, _pcbCpuBase, _pbPpuBus );
			SanitizeRegs<PgmBankSize(), ChrBankSize()>();
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
			m_stFixedOffset = std::max<size_t>( m_prRom->vPrgRom.size(), PgmBankSize() * 2 ) - PgmBankSize() * 2;
			for ( uint32_t I = 0xC000; I < 0x10000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead_Fixed, this, uint16_t( (I - 0xC000) % m_prRom->vPrgRom.size() ) );
			}
			

			// ================
			// SWAPPABLE BANKS
			// ================
			// CPU.
			for ( uint32_t I = 0x8000; I < 0xA000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<0, PgmBankSize()>, this, uint16_t( I - 0x8000 ) );
			}
			for ( uint32_t I = 0xA000; I < 0xC000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<1, PgmBankSize()>, this, uint16_t( I - 0xA000 ) );
			}
			// PPU.
#define LSN_CHR_BANK( X )																														\
	for ( uint32_t I = (X) * ChrBankSize(); I < ((X) + 1U) * ChrBankSize(); ++I ) {																\
		_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<X, ChrBankSize()>, this, uint16_t( I - ((X) * ChrBankSize()) ) );		\
	}
			LSN_CHR_BANK( 0 );
			LSN_CHR_BANK( 1 );
			LSN_CHR_BANK( 2 );
			LSN_CHR_BANK( 3 );
			LSN_CHR_BANK( 4 );
			LSN_CHR_BANK( 5 );
			LSN_CHR_BANK( 6 );
			LSN_CHR_BANK( 7 );
#undef LSN_CHR_BANK


			// ================
			// BANK-SELECT
			// ================
			// PGM bank-select.
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper112::SelectBank8000_FFFF, this, uint16_t( I ) );	// Treated as ROM.
			}


			// ================
			// MIRRORING
			// ================
			ApplyControllableMirrorMap( _pbPpuBus );
		}


	protected :
		// == Members.
		/** Register index to set. */
		uint8_t											m_ui8Reg;


		// == Functions.
		/**
		 * Selects a bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank8000_FFFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper112 * pmThis = reinterpret_cast<CMapper112 *>(_pvParm0);
			switch ( _ui16Parm1 & 0xE001 ) {
				case 0x8000 : {
					pmThis->m_ui8Reg = _ui8Val & 0b111;
					break;
				}
				case 0xA000 : {
					if ( pmThis->m_ui8Reg < 2 ) {
						pmThis->SetPgmBank<PgmBankSize()>( pmThis->m_ui8Reg, _ui8Val );
					}
					else if ( pmThis->m_ui8Reg == 2 ) {
						pmThis->SetChrBank<0, ChrBankSize()>( _ui8Val );
						pmThis->SetChrBank<1, ChrBankSize()>( _ui8Val + 1 );
					}
					else if ( pmThis->m_ui8Reg == 3 ) {
						pmThis->SetChrBank<2, ChrBankSize()>( _ui8Val );
						pmThis->SetChrBank<3, ChrBankSize()>( _ui8Val + 1 );
					}
					else {
						pmThis->SetChrBank<ChrBankSize()>( pmThis->m_ui8Reg, _ui8Val );
					}
					break;
				}
				case 0xE000 : {
					switch ( _ui8Val & 1 ) {
						case 0 : {
							pmThis->m_mmMirror = LSN_MM_VERTICAL;
							break;
						}
						case 1 : {
							pmThis->m_mmMirror = LSN_MM_HORIZONTAL;
							break;
						}
					}
				}
			}
		}
	};

}	// namespace lsn

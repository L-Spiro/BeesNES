/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 095 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper095
	 * \brief Mapper 095 implementation.
	 *
	 * Description: Mapper 095 implementation.
	 */
	class CMapper095 : public CMapperBase {
	public :
		CMapper095() {
		}
		virtual ~CMapper095() {
		}


		// == Functions.
		/**
		 * Gets the PGM bank size.
		 *
		 * \return Returns the size of the PGM banks.
		 */
		static constexpr uint16_t						PgmBankSize() { return 8 * 1024; }

		/**
		 * Gets the CHR bank size.
		 *
		 * \return Returns the size of the CHR banks.
		 */
		static constexpr uint16_t						ChrBankSize() { return 1 * 1024; }

		/**
		 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
		 *
		 * \param _rRom The ROM data.
		 * \param _pcbCpuBase A pointer to the CPU.
		 */
		virtual void									InitWithRom( LSN_ROM &_rRom, CCpuBase * _pcbCpuBase ) {
			CMapperBase::InitWithRom( _rRom, _pcbCpuBase );
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
#define LSN_CHR_BANK( X )																												\
	for ( uint32_t I = (X) * 0x0400; I < ((X) + 1) * 0x0400; ++I ) {																	\
		_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<X, ChrBankSize()>, this, uint16_t( I - ((X) * 0x0400) ) );		\
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
			for ( uint32_t I = 0x8000; I < 0xA000; ++I ) {
				if ( (I & 1) == 0 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper095::SelectBank8000_9FFE_Even, this, 0 );	// Treated as ROM.
				}
				else {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper095::SelectBank8000_9FFE_Odd, this, 0 );	// Treated as ROM.
				}
			}


			// ================
			// MIRRORING
			// ================
			ApplyControllableMirrorMap( _pbPpuBus );
		}


	protected :
		// == Members.
		/** Which bank was last selected. */
		uint8_t											m_ui8BankSelect;

		// == Functions.
		/**
		 * Selects a bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank8000_9FFE_Even( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper095 * pmThis = reinterpret_cast<CMapper095 *>(_pvParm0);
			pmThis->m_ui8BankSelect = _ui8Val & 0b111;
		}

		/**
		 * Selects bank data.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank8000_9FFE_Odd( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper095 * pmThis = reinterpret_cast<CMapper095 *>(_pvParm0);
			/*
			 *	7  bit  0
			 *	---- ----
			 *	..ND DDDD
			 *	  || ||||
			 *	  |+-++++- New bank value, based on last value written to Bank select register
			 *	  |          All registers as standard Namco 108
			 *	  |
			 *	  +------- Nametable select, based on last value written to Bank select register
			 *				 0: Select Nametable A
			 *				 1: Select Nametable B
			 *
			 */
			uint8_t ui8NewVal = _ui8Val & 0b11111;
			uint8_t ui8Mirror = (_ui8Val & 0b100000) >> 5;


			/*
			 *	7  bit  0
			 *	---- ----
			 *	xxxx xRRR
			 *		  |||
			 *		  +++- Specify which bank register to update on next write to Bank Data register
			 *			   0: Select 2 KB CHR bank at PPU $0000-$07FF and nametable at PPU $2000-$27FF
			 *			   1: Select 2 KB CHR bank at PPU $0800-$0FFF and nametable at PPU $2800-$2FFF
			 *			   2: Select 1 KB CHR bank at PPU $1000-$13FF
			 *			   3: Select 1 KB CHR bank at PPU $1400-$17FF
			 *			   4: Select 1 KB CHR bank at PPU $1800-$1BFF
			 *			   5: Select 1 KB CHR bank at PPU $1C00-$1FFF
			 *			   6: Select 8 KB PRG ROM bank at $8000-$9FFF
			 *			   7: Select 8 KB PRG ROM bank at $A000-$BFFF
			 */
			switch ( pmThis->m_ui8BankSelect ) {
				case 0 : {	// 0: Select 2 KB CHR bank at PPU $0000-$07FF and nametable at PPU $2000-$27FF
					pmThis->SetChrBank<0, ChrBankSize()>( ui8NewVal );
					pmThis->SetChrBank<1, ChrBankSize()>( ui8NewVal + 1 );
					switch ( ui8Mirror ) {
						case 0 : {
							pmThis->m_mmMirror = LSN_MM_1_SCREEN_A;
							break;
						}
						case 1 : {
							pmThis->m_mmMirror = LSN_MM_1_SCREEN_B;
							break;
						}
					}
					break;
				}
				case 1 : {	// 1: Select 2 KB CHR bank at PPU $0800-$0FFF and nametable at PPU $2800-$2FFF
					pmThis->SetChrBank<2, ChrBankSize()>( ui8NewVal );
					pmThis->SetChrBank<3, ChrBankSize()>( ui8NewVal + 1 );
					switch ( ui8Mirror ) {
						case 0 : {
							pmThis->m_mmMirror = LSN_MM_1_SCREEN_A;
							break;
						}
						case 1 : {
							pmThis->m_mmMirror = LSN_MM_1_SCREEN_B;
							break;
						}
					}
					break;
				}
				case 2 : {	// 2: Select 1 KB CHR bank at PPU $1000-$13FF
					pmThis->SetChrBank<4, ChrBankSize()>( ui8NewVal );
					break;
				}
				case 3 : {	// 3: Select 1 KB CHR bank at PPU $1400-$17FF
					pmThis->SetChrBank<5, ChrBankSize()>( ui8NewVal );
					break;
				}
				case 4 : {	// 4: Select 1 KB CHR bank at PPU $1800-$1BFF
					pmThis->SetChrBank<6, ChrBankSize()>( ui8NewVal );
					break;
				}
				case 5 : {	// 5: Select 1 KB CHR bank at PPU $1C00-$1FFF
					pmThis->SetChrBank<7, ChrBankSize()>( ui8NewVal );
					break;
				}
				case 6 : {	// 6: Select 8 KB PRG ROM bank at $8000-$9FFF
					pmThis->SetPgmBank<0, PgmBankSize()>( ui8NewVal );
					break;
				}
				case 7 : {	// 6: Select 8 KB PRG ROM bank at $A000-$BFFF
					pmThis->SetPgmBank<1, PgmBankSize()>( ui8NewVal );
					break;
				}
			}
		}
	};

}	// namespace lsn

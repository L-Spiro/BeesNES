/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 033 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper033
	 * \brief Mapper 033 implementation.
	 *
	 * Description: Mapper 033 implementation.
	 */
	class CMapper033 : public CMapperBase {
	public :
		CMapper033() {
		}
		virtual ~CMapper033() {
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
			SanitizeRegs<PgmBankSize(), ChrBankSize() * 2>();
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
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead_Fixed, this, uint16_t( I - 0xC000 ) );
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
			for ( uint32_t I = 0x0000; I < 0x0800; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<0, ChrBankSize() * 2>, this, uint16_t( I - 0x0000 ) );
			}
			for ( uint32_t I = 0x0800; I < 0x1000; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<1, ChrBankSize() * 2>, this, uint16_t( I - 0x0800 ) );
			}
			for ( uint32_t I = 0x1000; I < 0x1400; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<2, ChrBankSize()>, this, uint16_t( I - 0x1000 ) );
			}
			for ( uint32_t I = 0x1400; I < 0x1800; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<3, ChrBankSize()>, this, uint16_t( I - 0x1400 ) );
			}
			for ( uint32_t I = 0x1800; I < 0x1C00; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<4, ChrBankSize()>, this, uint16_t( I - 0x1800 ) );
			}
			for ( uint32_t I = 0x1C00; I < 0x2000; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<5, ChrBankSize()>, this, uint16_t( I - 0x1C00 ) );
			}


			// ================
			// BANK-SELECT
			// ================
			// PGM bank-select.
			for ( uint32_t I = 0x8000; I < 0xC000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper033::SelectBank8000_BFFF, this, uint16_t( I ) );	// Treated as ROM.
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
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank8000_BFFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper033 * pmThis = reinterpret_cast<CMapper033 *>(_pvParm0);
			switch ( _ui16Parm1 & 0xA003 ) {
				case 0x8000 : {
					/*
					 *	$8000 [.MPP PPPP]
					 *	  M = Mirroring (0=Vert, 1=Horz)
					 *	  P = PRG Reg 0 (8k @ $8000)
					 */
					switch ( (_ui8Val >> 6) & 1 ) {
						case 0 : {
							pmThis->m_mmMirror = LSN_MM_VERTICAL;
							break;
						}
						case 1 : {
							pmThis->m_mmMirror = LSN_MM_HORIZONTAL;
							break;
						}
					}
					pmThis->SetPgmBank<0, PgmBankSize()>( _ui8Val & 0b111111 );
					break;
				}
				case 0x8001 : {
					pmThis->SetPgmBank<1, PgmBankSize()>( _ui8Val & 0b111111 );
					break;
				}
				case 0x8002 : {
					// Note that unlike the MMC3, the value written for the two 2 KiB CHR banks do not drop the LSB; the number written specifies the offset into CHR as a multiple of 2 KiB.
					pmThis->SetChrBank<0, ChrBankSize() * 2>( _ui8Val );
					break;
				}
				case 0x8003 : {
					// Note that unlike the MMC3, the value written for the two 2 KiB CHR banks do not drop the LSB; the number written specifies the offset into CHR as a multiple of 2 KiB.
					pmThis->SetChrBank<1, ChrBankSize() * 2>( _ui8Val );
					break;
				}
				case 0xA000 : {
					pmThis->SetChrBank<2, ChrBankSize()>( _ui8Val );
					break;
				}
				case 0xA001 : {
					pmThis->SetChrBank<3, ChrBankSize()>( _ui8Val );
					break;
				}
				case 0xA002 : {
					pmThis->SetChrBank<4, ChrBankSize()>( _ui8Val );
					break;
				}
				case 0xA003 : {
					pmThis->SetChrBank<5, ChrBankSize()>( _ui8Val );
					break;
				}
			}
		}
	};

}	// namespace lsn

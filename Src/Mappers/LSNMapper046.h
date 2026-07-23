/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 046 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper046
	 * \brief Mapper 046 implementation.
	 *
	 * Description: NES Mapper 046 (Color Dreams / Rumpletiltskin) implementation.
	 */
	class CMapper046 : public CMapperBase {
	public :
		CMapper046() {
		}
		virtual ~CMapper046() {
		}


		// == Functions.
		/**
		 * Gets the PGM bank size.
		 *
		 * \return Returns the size of the PGM banks (32 KiB).
		 */
		static constexpr uint16_t						PgmBankSize() { return 32 * 1024; }

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

			m_ui8Reg6000 = 0;
			m_ui8Reg8000 = 0;

			SetPgmBank<0, PgmBankSize()>( 0 );
			SetChrBank<0, ChrBankSize()>( 0 );
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
			if ( m_prRom->vPrgRom.size() ) {
				for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<0, PgmBankSize()>, this, uint16_t( I - 0x8000 ) );
				}
			}
			// PPU.
			if ( m_prRom->vChrRom.size() ) {
				for ( uint32_t I = 0x0000; I < 0x2000; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<0, ChrBankSize()>, this, uint16_t( I - 0x0000 ) );
				}
			}


			// ================
			// BANK-SELECT
			// ================
			for ( uint32_t I = 0x6000; I < 0x8000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper046::Write6000_7FFF, this, uint16_t( I - 0x6000 ) );
			}
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper046::Write8000_FFFF, this, uint16_t( I - 0x8000 ) );
			}


			// ================
			// MIRRORING
			// ================
			//ApplyControllableMirrorMap( _pbPpuBus );
		}

		/**
		 * Called to inform the mapper of a reset.
		 **/
		virtual void									Reset() {
			m_ui8Reg6000 = 0;
			m_ui8Reg8000 = 0;

			SetPgmBank<0, PgmBankSize()>( 0 );
			SetChrBank<0, ChrBankSize()>( 0 );
		}


	protected :
		// == Members.
		/** Internal latch for low bank bits ($6000-$7FFF). */
		uint8_t											m_ui8Reg6000;
		/** Internal latch for high bank bits ($8000-$FFFF). */
		uint8_t											m_ui8Reg8000;


		// == Functions.
		/**
		 * Handles latch writes from $6000 to $7FFF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write6000_7FFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper046 * pmThis = reinterpret_cast<CMapper046 *>(_pvParm0);
			/*uint8_t ui8Rom = pmThis->m_prRom->vPrgRom.data()[size_t(pmThis->m_ui8PgmBanks[0])*PgmBankSize()+size_t(_ui16Parm1)];
			_ui8Val &= ui8Rom;*/

			pmThis->m_ui8Reg6000 = _ui8Val;
			
			// $6000-7FFF:  [CCCC PPPP]   High CHR, PRG bits
			// $8000-FFFF:  [.CCC ...P]   Low CHR, PRG bits
			uint8_t ui8Prg = ((pmThis->m_ui8Reg6000 & 0x0F) << 1) | (pmThis->m_ui8Reg8000 & 0x01);
			pmThis->SetPgmBank<0, PgmBankSize()>( ui8Prg );

			uint8_t ui8Chr = ((pmThis->m_ui8Reg8000 >> 4) & 0b0111) | ((pmThis->m_ui8Reg6000 >> 1) & 0b01111000);
			pmThis->SetChrBank<0, ChrBankSize()>( ui8Chr );
		}

		/**
		 * Handles latch writes from $8000 to $FFFF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write8000_FFFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper046 * pmThis = reinterpret_cast<CMapper046 *>(_pvParm0);
			/*uint8_t ui8Rom = pmThis->m_prRom->vPrgRom.data()[size_t(pmThis->m_ui8PgmBanks[0])*PgmBankSize()+size_t(_ui16Parm1)];
			_ui8Val &= ui8Rom;*/

			pmThis->m_ui8Reg8000 = _ui8Val;
			
			// $6000-7FFF:  [CCCC PPPP]   High CHR, PRG bits
			// $8000-FFFF:  [.CCC ...P]   Low CHR, PRG bits
			uint8_t ui8Prg = ((pmThis->m_ui8Reg6000 & 0x0F) << 1) | (pmThis->m_ui8Reg8000 & 0x01);
			pmThis->SetPgmBank<0, PgmBankSize()>( ui8Prg );

			uint8_t ui8Chr = ((pmThis->m_ui8Reg8000 >> 4) & 0b0111) | ((pmThis->m_ui8Reg6000 >> 1) & 0b01111000);
			pmThis->SetChrBank<0, ChrBankSize()>( ui8Chr );
		}
	};

}	// namespace lsn

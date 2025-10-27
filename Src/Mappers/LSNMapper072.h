/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 072 implementation (relative of Mapper 092).
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper072
	 * \brief Mapper 072 implementation (relative of Mapper 092).
	 *
	 * Description: Mapper 072 implementation (relative of Mapper 092).
	 */
	class CMapper072 : public CMapperBase {
	public :
		CMapper072() {
		}
		virtual ~CMapper072() {
		}


		// == Functions.
		/**
		 * Gets the PGM bank size.
		 *
		 * \return Returns the size of the PGM banks.
		 */
		static constexpr uint16_t						PgmBankSize() { return 16 * 1024; }

		/**
		 * Gets the CHR bank size.
		 *
		 * \return Returns the size of the CHR banks.
		 */
		static constexpr uint16_t						ChrBankSize() { return 8 * 1024; }

		/**
		 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
		 *
		 * \param _rRom The ROM data.
		 * \param _pcbCpuBase A pointer to the CPU.
		 */
		virtual void									InitWithRom( LSN_ROM &_rRom, CCpuBase * _pcbCpuBase, CPpuBase * _ppbPpuBase, CInterruptable * _piInter, CBussable * _pbPpuBus ) {
			CMapperBase::InitWithRom( _rRom, _pcbCpuBase, _ppbPpuBase, _piInter, _pbPpuBus );
			SanitizeRegs<PgmBankSize(), ChrBankSize()>();
			m_ui8Last = 0;
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
			// Set the reads of the fixed bank at the start.		
			m_stFixedOffset = std::max<size_t>( m_prRom->vPrgRom.size(), PgmBankSize() ) - PgmBankSize();
			for ( uint32_t I = 0xC000; I < 0x10000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead_Fixed, this, uint16_t( (I - 0xC000) % (m_prRom->vPrgRom.size() - m_stFixedOffset) ) );
			}
			

			// ================
			// SWAPPABLE BANKS
			// ================
			// CPU.
			for ( uint32_t I = 0x8000; I < 0xC000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<0, PgmBankSize()>, this, uint16_t( I - 0x8000 ) );
			}
			// PPU.
			for ( uint32_t I = 0x0000; I < 0x2000; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<0, ChrBankSize()>, this, uint16_t( I - 0x0000 ) );
			}


			// ================
			// BANK-SELECT
			// ================
			// PGM bank-select.
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper072::SelectBank8000_FFFF, this, 0 );	// Treated as ROM.
			}
		}


	protected :
		// == Members.
		/** Last bank switch. */
		uint8_t											m_ui8Last;


		// == Functions.
		/**
		 * Selects a bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank8000_FFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper072 * pmThis = reinterpret_cast<CMapper072 *>(_pvParm0);
			if ( !(pmThis->m_ui8Last & 0b10000000) && (_ui8Val & 0b10000000) ) {
				pmThis->SetPgmBank<0, PgmBankSize()>( _ui8Val & 0b1111 );
			}
			if ( !(pmThis->m_ui8Last & 0b01000000) && (_ui8Val & 0b01000000) ) {
				pmThis->SetChrBank<0, ChrBankSize()>( _ui8Val & 0b1111 );
			}
			// TODO:
			/*
			 *	 15   11 address   0  7 data  0
			 *	 ---- ---- ---- ----  ---- ----
			 *	 1... .... ...A AAAA  xxRS ....
			 *				  | ||||    ||
			 *				  | ||||    |+------ While 0 and no previous sound is playing, start playing
			 *				  | ||||    |        sound specified by A.
			 *				  | ||||    +------- While 0, the µPD7756C sound IC is held in reset and unable
			 *				  | ||||             to make sound.  Hold this low for at least 34 cycles.
			 *				  +-++++------------ The sound to play when triggered by S bit
			 */
			if ( !(pmThis->m_ui8Last & 0b00010000) && (_ui8Val & 0b00010000) ) {
				// Stop sound A.
			}
			else if ( !(pmThis->m_ui8Last & 0b00010000) && !(_ui8Val & 0b00010000) ) {
				// Play A if no previous sound is playing.
			}

			pmThis->m_ui8Last = _ui8Val;
		}
	};

}	// namespace lsn

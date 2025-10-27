/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 013 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper013
	 * \brief Mapper 013 implementation.
	 *
	 * Description: Mapper 013 implementation.
	 */
	class CMapper013 : public CMapperBase {
	public :
		CMapper013() {
		}
		virtual ~CMapper013() {
		}


		// == Functions.
		/**
		 * Gets the PGM bank size.
		 *
		 * \return Returns the size of the PGM banks.
		 */
		static constexpr uint16_t						PgmBankSize() { return 32 * 1024; }

		/**
		 * Gets the CHR bank size.
		 *
		 * \return Returns the size of the CHR banks.
		 */
		static constexpr uint16_t						ChrBankSize() { return 4 * 1024; }

		/**
		 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
		 *
		 * \param _rRom The ROM data.
		 * \param _pcbCpuBase A pointer to the CPU.
		 */
		virtual void									InitWithRom( LSN_ROM &_rRom, CCpuBase * _pcbCpuBase, CPpuBase * _ppbPpuBase, CInterruptable * _piInter, CBussable * _pbPpuBus ) {
			CMapperBase::InitWithRom( _rRom, _pcbCpuBase, _ppbPpuBase, _piInter, _pbPpuBus );
			SanitizeRegs<PgmBankSize(), ChrBankSize()>();

			// PPU $0000-$0FFF: 4 KiB fixed CHR RAM bank (first page).
			SetChrBank<0, ChrBankSize()>( 0 );

			size_t sBank = m_ui8ChrBanks[1] & 0b0011;
			if ( sBank < 2 ) {
				m_ui8ChrRamBasePtr = m_ui8DefaultChrRam + (sBank * ChrBankSize());
			}
			else {
				m_ui8ChrRamBasePtr = m_ui82ndChrRam + ((sBank & 1) * ChrBankSize());
			}
			
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
			m_stFixedOffset = std::max<size_t>( m_prRom->vPrgRom.size(), PgmBankSize() ) - PgmBankSize();
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead_Fixed, this, uint16_t( (I - 0x8000) % (m_prRom->vPrgRom.size() - m_stFixedOffset) ) );
			}
			

			// ================
			// SWAPPABLE BANKS
			// ================
			// PPU.
			for ( uint32_t I = 0x1000; I < 0x2000; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper013::ChrBankRead_1000_1FFF, this, uint16_t( I - 0x1000 ) );
				_pbPpuBus->SetWriteFunc( uint16_t( I ), &CMapper013::ChrBankWrite_1000_1FFF, this, uint16_t( I - 0x1000 ) );
			}


			// ================
			// BANK-SELECT
			// ================
			// CHR bank-select.
			// Writes to the whole area are used to select a bank.
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper013::SelectBank8000_FFFF, this, 0 );
			}
		}


	protected :
		// == Members.
		/** The CHR RAM start pointer. */
		uint8_t *										m_ui8ChrRamBasePtr;
		/** Second 8 kilobytes of CHR RAM. */
		uint8_t											m_ui82ndChrRam[8*1024];


		// == Functions.
		/**
		 * Reads from CHR RAM using m_ui8ChrRamBasePtr[_ui16Parm1] to select a bank among _uSize-sized banks.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 * \param _uReg The register index.
		 * \param _uSize The bank size.
		 */
		static void LSN_FASTCALL						ChrBankRead_1000_1FFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper013 * pmThis = reinterpret_cast<CMapper013 *>(_pvParm0);
			_ui8Ret = pmThis->m_ui8ChrRamBasePtr[_ui16Parm1];
		}

		/**
		 * Writes to the CHR RAM using m_ui8ChrRamBasePtr[_ui16Parm1] to select a bank among _uSize-sized banks.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						ChrBankWrite_1000_1FFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper013 * pmThis = reinterpret_cast<CMapper013 *>(_pvParm0);
			pmThis->m_ui8ChrRamBasePtr[_ui16Parm1] = _ui8Val;
		}

		/**
		 * Selects a bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank8000_FFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper013 * pmThis = reinterpret_cast<CMapper013 *>(_pvParm0);
			size_t sBank = _ui8Val & 0b0011;
			if ( sBank < 2 ) {
				pmThis->m_ui8ChrRamBasePtr = pmThis->m_ui8DefaultChrRam + (sBank * ChrBankSize());
			}
			else {
				pmThis->m_ui8ChrRamBasePtr = pmThis->m_ui82ndChrRam + ((sBank & 1) * ChrBankSize());
			}
		}
	};

}	// namespace lsn

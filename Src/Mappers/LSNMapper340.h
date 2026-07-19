/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 340 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper340
	 * \brief Mapper 340 implementation.
	 *
	 * Description: NES 2.0 Mapper 340 (BMC-K-3036 35-in-1 multicart) implementation.
	 */
	class CMapper340 : public CMapperBase {
	public :
		CMapper340() {
		}
		virtual ~CMapper340() {
		}


		// == Functions.
		/**
		 * Gets the PGM bank size.
		 *
		 * \return Returns the size of the PGM banks (16 KiB).
		 */
		static constexpr uint16_t						PgmBankSize() { return 16 * 1024; }

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

			m_ui16PrgMask16 = uint16_t( (_rRom.vPrgRom.size() / PgmBankSize()) - 1 );

			m_ui8Outer = 0;
			m_ui8Inner = 0;
			m_ui8Mode = 0;

			UpdateBanks();
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
			for ( uint32_t I = 0x8000; I < 0xC000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<0, PgmBankSize()>, this, uint16_t( I - 0x8000 ) );
			}
			for ( uint32_t I = 0xC000; I < 0x10000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<1, PgmBankSize()>, this, uint16_t( I - 0xC000 ) );
			}


			// ================
			// BANK-SELECT
			// ================
			// Register value.
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				// We pass `I` to _ui16Parm1 so we can extract address bits M, O, and I during the write.
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper340::SelectBank8000_FFFF, this, uint16_t( I ) );
			}
		}


	protected :
		// == Members.
		uint16_t										m_ui16PrgMask16;							/**< Mask for 16-kilobyte PGM banking. */
		uint8_t											m_ui8Mode;									/**< The mode value (M bit). */
		uint8_t											m_ui8Outer;									/**< The outer bank (O bits). */
		uint8_t											m_ui8Inner;									/**< The inner bank (I bits). */


		// == Functions.
		/**
		 * Updates the bank selection.
		 **/
		void inline										UpdateBanks() {
			// The outer bank selects a 128 KiB block (which is 8 * 16 KiB banks).
			uint16_t ui16BaseBank = uint16_t( m_ui8Outer << 3 );

			uint16_t ui16PrgLo = ui16BaseBank | m_ui8Inner;
			uint16_t ui16PrgHi = ui16BaseBank | (m_ui8Mode == 0 ? 7 : m_ui8Inner);

			ui16PrgLo &= m_ui16PrgMask16;
			ui16PrgHi &= m_ui16PrgMask16;

			SetPgmBank<0, PgmBankSize()>( ui16PrgLo );
			SetPgmBank<1, PgmBankSize()>( ui16PrgHi );
		}

		/**
		 * Selects a bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address. Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBank8000_FFFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper340 * pmThis = reinterpret_cast<CMapper340 *>(_pvParm0);
			
			/*
			 * Mask: $8000
			 * A~FEDC BA98 7654 3210  D~7654 3210
			 * .... .... .. MO OIII   .... . III
			 */
			pmThis->m_ui8Mode  = (_ui16Parm1 >> 5) & 0x01;
			pmThis->m_ui8Outer = (_ui16Parm1 >> 3) & 0x03;
			pmThis->m_ui8Inner = (_ui16Parm1 & 0x07) | (_ui8Val & 0x07);
			
			pmThis->UpdateBanks();
		}
	};

}	// namespace lsn

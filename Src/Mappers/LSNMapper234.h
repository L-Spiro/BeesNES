/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 232 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper234
	 * \brief Mapper 232 implementation.
	 *
	 * Description: Mapper 232 implementation.
	 */
	class CMapper234 : public CMapperBase {
	public :
		CMapper234() {
		}
		virtual ~CMapper234() {
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
		static constexpr uint16_t						ChrBankSize() { return 8 * 1024; }

		/**
		 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
		 *
		 * \param _rRom The ROM data.
		 * \param _pcbCpuBase A pointer to the CPU.
		 */
		virtual void									InitWithRom( LSN_ROM &_rRom, CCpuBase * _pcbCpuBase, CBussable * _pbPpuBus ) {
			CMapperBase::InitWithRom( _rRom, _pcbCpuBase, _pbPpuBus );
			SanitizeRegs<PgmBankSize(), ChrBankSize()>();
			m_ui8Reg80 = m_ui8RegE8 = 0;
			m_sPgmMask = m_prRom->vPrgRom.size() - 1;
			m_sChrMask = m_prRom->vChrRom.size() - 1;
			/*SetPgmBank<0, PgmBankSize()>( (m_ui8Bank & 0b1100) | 0x3 );
			SetPgmBank<1, PgmBankSize()>( m_ui8Bank );*/
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
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper234::Read8000_FFFF, this, uint16_t( I - 0x8000 ) );
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CCpuBus::NoWrite, this, 0 );
			}
			// PPU.
			for ( uint32_t I = 0x0000; I < 0x2000; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper234::Read0000_1FFF, this, uint16_t( I - 0x0000 ) );
			}


			// ================
			// BANK-SELECT
			// ================
			// PGM bank-select.
			// Outer: $FF80-$FF9F
			for ( uint32_t I = 0xFF80; I <= 0xFF9F; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper234::ReadFF80_FF9F, this, uint16_t( I - 0x8000 ) );
			}
			// Inner: $FFE8-$FFF7
			for ( uint32_t I = 0xFFE8; I <= 0xFFF7; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper234::ReadFFE8_FFF7, this, uint16_t( I - 0x8000 ) );
			}


			// ================
			// MIRRORING
			// ================
			ApplyControllableMirrorMap( _pbPpuBus );
		}

		/**
		 * Called to inform the mapper of a reset.
		 **/
		virtual void									Reset() {
			m_ui8Reg80 = m_ui8RegE8 = 0;
		}


	protected :
		// == Members.
		/** The PGM mask. */
		size_t											m_sPgmMask = 0;
		/** The CHR mask. */
		size_t											m_sChrMask = 0;
		/** The $FF80-$FF9F register. */
		uint8_t											m_ui8Reg80 = 0;
		/** The $FFE8-$FFF7 register. */
		uint8_t											m_ui8RegE8 = 0;


		// == Functions.
		/**
		 * Reads PGM ROM.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Read8000_FFFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper234 * pmThis = reinterpret_cast<CMapper234 *>(_pvParm0);
			size_t sOff;
			/*	m_ui8Reg80
			 * 7  bit  0
			 * MOQq BBBb  
			 * |||| ||||
			 * |||| ++++-- Block selection
			 * |||+------- ROMs 3+4 Disable (0=normal, 1=disable ROMs 3+4 even if selected by Q)
			 * ||+-------- ROM switch (0=select ROMs 1+2, 1=select ROMs 3+4)
			 * |+--------- Mode (0=CNROM, 1=NINA-03)
			 * +---------- Mirroring (0=Vertical, 1=Horizontal)
			 */

			/*	m_ui8RegE8
			 * 7  bit  0
			 * .cCC ...P
			 *  |||    |
			 *  |||    +-- PRG page
			 *  +++------- CHR page
			 */
			if ( pmThis->m_ui8Reg80 & 0b01000000 ) {
				// 1=NINA-03
				// In NINA-03 mode, the 32kB PRG ROM bank is BBBP
				sOff = (pmThis->m_ui8Reg80 & 0b1110) | (pmThis->m_ui8RegE8 & 1);
			}
			else {
				// 0=CNROM
				// In CNROM mode, the 32kB PRG ROM bank is BBBb
				sOff = pmThis->m_ui8Reg80 & 0b1111;
			}
			sOff *= PgmBankSize();
			_ui8Ret = pmThis->m_prRom->vPrgRom.data()[(sOff+_ui16Parm1)&pmThis->m_sPgmMask];
		}

		/**
		 * Reads CHR ROM.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Read0000_1FFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper234 * pmThis = reinterpret_cast<CMapper234 *>(_pvParm0);
			size_t sOff;
			/*	m_ui8Reg80
			 * 7  bit  0
			 * MOQq BBBb  
			 * |||| ||||
			 * |||| ++++-- Block selection
			 * |||+------- ROMs 3+4 Disable (0=normal, 1=disable ROMs 3+4 even if selected by Q)
			 * ||+-------- ROM switch (0=select ROMs 1+2, 1=select ROMs 3+4)
			 * |+--------- Mode (0=CNROM, 1=NINA-03)
			 * +---------- Mirroring (0=Vertical, 1=Horizontal)
			 */

			/*	m_ui8RegE8
			 * 7  bit  0
			 * .cCC ...P
			 *  |||    |
			 *  |||    +-- PRG page
			 *  +++------- CHR page
			 */
			if ( pmThis->m_ui8Reg80 & 0b01000000 ) {
				// 1=NINA-03
				// In NINA-03 mode, the 8kB CHR ROM bank is BBBcCC
				sOff = ((pmThis->m_ui8Reg80 & 0b1110) << 2) | ((pmThis->m_ui8RegE8 >> 4) & 0b111); 
			}
			else {
				// 0=CNROM
				// In CNROM mode, the 8kB CHR ROM bank is BBBbCC
				sOff = ((pmThis->m_ui8Reg80 & 0b1111) << 2) | ((pmThis->m_ui8RegE8 >> 4) & 0b11); 
			}
			sOff *= ChrBankSize();
			_ui8Ret = pmThis->m_prRom->vChrRom.data()[(sOff+_ui16Parm1)&pmThis->m_sChrMask];
		}

		/**
		 * Reads the 80-9F registers.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						ReadFF80_FF9F( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t &_ui8Ret ) {
			CMapper234 * pmThis = reinterpret_cast<CMapper234 *>(_pvParm0);
			Read8000_FFFF( _pvParm0, _ui16Parm1, _pui8Data, _ui8Ret );

			/*	m_ui8Reg80
			 * 7  bit  0
			 * MOQq BBBb  
			 * |||| ||||
			 * |||| ++++-- Block selection
			 * |||+------- ROMs 3+4 Disable (0=normal, 1=disable ROMs 3+4 even if selected by Q)
			 * ||+-------- ROM switch (0=select ROMs 1+2, 1=select ROMs 3+4)
			 * |+--------- Mode (0=CNROM, 1=NINA-03)
			 * +---------- Mirroring (0=Vertical, 1=Horizontal)
			 */
			// Once any of the Q, q, BBB, or b bits are set, neither this register nor the Lockout defeat register can be updated.
			if LSN_UNLIKELY( !(pmThis->m_ui8Reg80 & 0b00111111) ) {
				pmThis->m_ui8Reg80 = _ui8Ret;
				if ( _ui8Ret & 0x80 ) {
					pmThis->m_mmMirror = LSN_MM_HORIZONTAL;
				}
				else {
					pmThis->m_mmMirror = LSN_MM_VERTICAL;
				}
			}
		}

		/**
		 * Reads the E8-F7 registers.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						ReadFFE8_FFF7( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t &_ui8Ret ) {
			CMapper234 * pmThis = reinterpret_cast<CMapper234 *>(_pvParm0);
			Read8000_FFFF( _pvParm0, _ui16Parm1, _pui8Data, _ui8Ret );

			pmThis->m_ui8RegE8 = _ui8Ret;
		}
	};

}	// namespace lsn

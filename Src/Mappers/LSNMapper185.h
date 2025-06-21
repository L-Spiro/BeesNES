/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 185 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper185
	 * \brief Mapper 185 implementation.
	 *
	 * Description: Mapper 185 implementation.
	 */
	class CMapper185 : public CMapperBase {
	public :
		CMapper185() {
		}
		virtual ~CMapper185() {
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
		virtual void									InitWithRom( LSN_ROM &_rRom, CCpuBase * _pcbCpuBase, CInterruptable * _piInter, CBussable * _pbPpuBus ) {
			CMapperBase::InitWithRom( _rRom, _pcbCpuBase, _piInter, _pbPpuBus );
			SanitizeRegs<PgmBankSize(), ChrBankSize()>();

			switch ( _rRom.riInfo.ui16SubMapper ) {
				case 4 : {
					m_ui8ChrMagic = 0;
					break;
				}
				case 5 : {
					m_ui8ChrMagic = 1;
					break;
				}
				case 6 : {
					m_ui8ChrMagic = 2;
					break;
				}
				case 7 : {
					m_ui8ChrMagic = 3;
					break;
				}
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
			// CPU.
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::StdMapperCpuRead, this, uint16_t( (I - 0x8000) % m_prRom->vPrgRom.size() ) );
			}

			if ( m_prRom->i32WorkRamSize ) {
				m_vPrgRam.resize( m_prRom->i32WorkRamSize );
				for ( uint32_t I = 0x6000; I < 0x8000; ++I ) {
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper185::ReadWRam, this, uint16_t( (I - 0x6000) % m_prRom->i32WorkRamSize ) );
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper185::WriteWRam, this, uint16_t( (I - 0x6000) % m_prRom->i32WorkRamSize ) );
				}
			}


			// ================
			// SWAPPABLE BANKS
			// ================
			// PPU.
			for ( uint32_t I = 0x0000; I < 0x2000; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper185::ReadChrRom, this, uint16_t( I % m_prRom->vChrRom.size() ) );
			}


			// ================
			// BANK-SELECT
			// ================
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper185::Mapper185CpuWrite, this, uint16_t( (I - 0x8000) % m_prRom->vPrgRom.size() ) );
			}
		}


	protected :
		// == Members.
		std::vector<uint8_t>							m_vPrgRam;										/**< PRG RAM. */
		uint8_t											m_ui8Chip = 0;									/**< Chip-select. */
		uint8_t											m_ui8ChrMagic = 0;								/**< The chip select needed for CHR ROM to work. */


		// == Functions.
		/**
		 * CPU ROM-area writes are used to set the CHR bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Mapper185CpuWrite( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper185 * pmThis = reinterpret_cast<CMapper185 *>(_pvParm0);
			_ui8Val &= pmThis->m_prRom->vPrgRom.data()[_ui16Parm1];
			pmThis->m_ui8Chip = _ui8Val;
		}

		/**
		 * Reads bank 0 $6000-$7FFF.
		 *
		 * \tparam _uM Matched against the M bit to decide on what to read.
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						ReadWRam( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper185 * pmThis = reinterpret_cast<CMapper185 *>(_pvParm0);
			_ui8Ret = pmThis->m_vPrgRam[_ui16Parm1];
		}

		/**
		 * Writes to bank 0 $6000-$7FFF.
		 * 
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 **/
		static void LSN_FASTCALL						WriteWRam( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper185 * pmThis = reinterpret_cast<CMapper185 *>(_pvParm0);
			pmThis->m_vPrgRam[_ui16Parm1] = _ui8Val;
		}

		/**
		 * Reads CHR ROM (PPU $0000-$1FFF).
		 *
		 * \tparam _uM Matched against the M bit to decide on what to read.
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						ReadChrRom( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper185 * pmThis = reinterpret_cast<CMapper185 *>(_pvParm0);
			if ( (pmThis->m_ui8Chip & 0b11) == pmThis->m_ui8ChrMagic ) {
				_ui8Ret = pmThis->m_prRom->vChrRom[_ui16Parm1];
			}
			else {
				if ( !_ui16Parm1 ) {
					_ui8Ret = 1;
				}
			}
		}
		
	};

}	// namespace lsn

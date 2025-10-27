/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 003 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper003
	 * \brief Mapper 003 implementation.
	 *
	 * Description: Mapper 003 implementation.
	 */
	class CMapper003 : public CMapperBase {
	public :
		CMapper003() {
		}
		virtual ~CMapper003() {
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
		virtual void									InitWithRom( LSN_ROM &_rRom, CCpuBase * _pcbCpuBase, CPpuBase * _ppbPpuBase, CInterruptable * _piInter, CBussable * _pbPpuBus ) {
			CMapperBase::InitWithRom( _rRom, _pcbCpuBase, _ppbPpuBase, _piInter, _pbPpuBus );
			SanitizeRegs<PgmBankSize(), ChrBankSize()>();

			/*if ( _rRom.i32ChrRamSize == 128 * 1024 || _rRom.vChrRom.size() == 128 * 1024 ) {
				m_ui8ChrBankMask = 0b00001111;
			}*/

			const uint32_t ui32Banks = uint32_t( std::max<size_t>( 1, _rRom.vChrRom.size() / ChrBankSize() ) );
			if ( (ui32Banks & (ui32Banks - 1)) == 0 ) {
				m_ui8ChrBankMask = uint8_t( ui32Banks - 1 );
			}
			else {
				uint32_t ui32Np2 = 1U;
				while ( ui32Np2 < ui32Banks ) { ui32Np2 <<= 1; }
				m_ui8ChrBankMask = uint8_t( ui32Np2 - 1 );
			}


			m_bAndConflicts = _rRom.riInfo.ui16SubMapper == 2;
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
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper003::ReadWRam, this, uint16_t( (I - 0x6000) % m_prRom->i32WorkRamSize ) );
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper003::WriteWRam, this, uint16_t( (I - 0x6000) % m_prRom->i32WorkRamSize ) );
				}
			}


			// ================
			// SWAPPABLE BANKS
			// ================
			// PPU.
			if ( !m_prRom->vChrRom.empty() ) {
				for ( uint32_t I = 0x0000; I < 0x2000; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<0, ChrBankSize()>, this, uint16_t( I ) );
				}
			}


			// ================
			// BANK-SELECT
			// ================
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &Mapper003CpuWrite, this, uint16_t( (I - 0x8000) % m_prRom->vPrgRom.size() ) );
			}
		}


	protected :
		// == Members.
		std::vector<uint8_t>							m_vPrgRam;										/**< PRG RAM. */
		uint8_t											m_ui8ChrBankMask = 0b00000011;					/**< CHR banking mask. */
		bool											m_bAndConflicts = false;						/**< AND-type bus conflicts. */


		// == Functions.
		/**
		 * CPU ROM-area writes are used to set the CHR bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Mapper003CpuWrite( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper003 * pmThis = reinterpret_cast<CMapper003 *>(_pvParm0);
			if ( pmThis->m_prRom->vChrRom.empty() ) { return; }
			if ( pmThis->m_bAndConflicts ) {
				_ui8Val &= pmThis->m_prRom->vPrgRom.data()[_ui16Parm1];
			}
			pmThis->SetChrBank<0, ChrBankSize()>( _ui8Val & pmThis->m_ui8ChrBankMask );
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
			CMapper003 * pmThis = reinterpret_cast<CMapper003 *>(_pvParm0);
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
			CMapper003 * pmThis = reinterpret_cast<CMapper003 *>(_pvParm0);
			pmThis->m_vPrgRam[_ui16Parm1] = _ui8Val;
		}
		
	};

}	// namespace lsn

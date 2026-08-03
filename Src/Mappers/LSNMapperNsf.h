/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: NSF mapper implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"
#include <algorithm>
#include <vector>

namespace lsn {

	/**
	 * Class CMapperNsf
	 * \brief NSF mapper implementation.
	 *
	 * Description: Nintendo Sound Format (NSF) mapper implementation.
	 */
	class CMapperNsf : public CMapperBase {
	public :
		CMapperNsf() {
		}
		virtual ~CMapperNsf() {
		}


		// == Functions.
		/**
		 * Gets the PGM bank size.
		 *
		 * \return Returns the size of the PGM banks (4 KiB).
		 */
		static constexpr uint16_t						PgmBankSize() { return 4 * 1024; }

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

			m_vWram.resize( 8 * 1024 );
			std::fill( m_vWram.begin(), m_vWram.end(), uint8_t( 0 ) );

			SetPgmBank<0, PgmBankSize()>( _rRom.riInfo.ui8BankValues[0] );
			SetPgmBank<1, PgmBankSize()>( _rRom.riInfo.ui8BankValues[1] );
			SetPgmBank<2, PgmBankSize()>( _rRom.riInfo.ui8BankValues[2] );
			SetPgmBank<3, PgmBankSize()>( _rRom.riInfo.ui8BankValues[3] );
			SetPgmBank<4, PgmBankSize()>( _rRom.riInfo.ui8BankValues[4] );
			SetPgmBank<5, PgmBankSize()>( _rRom.riInfo.ui8BankValues[5] );
			SetPgmBank<6, PgmBankSize()>( _rRom.riInfo.ui8BankValues[6] );
			SetPgmBank<7, PgmBankSize()>( _rRom.riInfo.ui8BankValues[7] );
			
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
				for ( uint32_t I = 0x8000; I < 0x9000; ++I ) {
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<0, PgmBankSize()>, this, uint16_t( I - 0x8000 ) );
				}
				for ( uint32_t I = 0x9000; I < 0xA000; ++I ) {
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<1, PgmBankSize()>, this, uint16_t( I - 0x9000 ) );
				}
				for ( uint32_t I = 0xA000; I < 0xB000; ++I ) {
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<2, PgmBankSize()>, this, uint16_t( I - 0xA000 ) );
				}
				for ( uint32_t I = 0xB000; I < 0xC000; ++I ) {
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<3, PgmBankSize()>, this, uint16_t( I - 0xB000 ) );
				}
				for ( uint32_t I = 0xC000; I < 0xD000; ++I ) {
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<4, PgmBankSize()>, this, uint16_t( I - 0xC000 ) );
				}
				for ( uint32_t I = 0xD000; I < 0xE000; ++I ) {
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<5, PgmBankSize()>, this, uint16_t( I - 0xD000 ) );
				}
				for ( uint32_t I = 0xE000; I < 0xF000; ++I ) {
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<6, PgmBankSize()>, this, uint16_t( I - 0xE000 ) );
				}
				for ( uint32_t I = 0xF000; I < 0x10000; ++I ) {
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<7, PgmBankSize()>, this, uint16_t( I - 0xF000 ) );
				}
			}
			// PPU.
			if ( m_prRom->vChrRom.size() ) {
				for ( uint32_t I = 0x0000; I < 0x2000; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<0, ChrBankSize()>, this, uint16_t( I - 0x0000 ) );
				}
			}


			// ================
			// BANK-SELECT & RAM
			// ================
			// NSF Bank Select ($5FF8-$5FFF).
			_pbCpuBus->SetWriteFunc( 0x5FF8, &CMapperNsf::SetPrg<0>, this, 0 );
			_pbCpuBus->SetWriteFunc( 0x5FF9, &CMapperNsf::SetPrg<1>, this, 0 );
			_pbCpuBus->SetWriteFunc( 0x5FFA, &CMapperNsf::SetPrg<2>, this, 0 );
			_pbCpuBus->SetWriteFunc( 0x5FFB, &CMapperNsf::SetPrg<3>, this, 0 );
			_pbCpuBus->SetWriteFunc( 0x5FFC, &CMapperNsf::SetPrg<4>, this, 0 );
			_pbCpuBus->SetWriteFunc( 0x5FFD, &CMapperNsf::SetPrg<5>, this, 0 );
			_pbCpuBus->SetWriteFunc( 0x5FFE, &CMapperNsf::SetPrg<6>, this, 0 );
			_pbCpuBus->SetWriteFunc( 0x5FFF, &CMapperNsf::SetPrg<7>, this, 0 );

			// WRAM ($6000-$7FFF).
			for ( uint32_t I = 0x6000; I < 0x8000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperNsf::WramRead, this, uint16_t( I - 0x6000 ) );
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapperNsf::WramWrite, this, uint16_t( I - 0x6000 ) );
			}
		}


	protected :
		// == Members.
		/** The 8 KiB WRAM. */
		std::vector<uint8_t>							m_vWram;


		// == Functions.
		/**
		 * Templated hook for handling PRG bank switching ($5FF8-$5FFF).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		template <unsigned _uBank>
		static void LSN_FASTCALL						SetPrg( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapperNsf * pmThis = reinterpret_cast<CMapperNsf *>(_pvParm0);
			pmThis->SetPgmBank<_uBank, PgmBankSize()>( _ui8Val );
		}

		/**
		 * Reads from external WRAM.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						WramRead( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapperNsf * pmThis = reinterpret_cast<CMapperNsf *>(_pvParm0);
			_ui8Ret = pmThis->m_vWram[_ui16Parm1];
		}

		/**
		 * Writes to external WRAM.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						WramWrite( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapperNsf * pmThis = reinterpret_cast<CMapperNsf *>(_pvParm0);
			pmThis->m_vWram[_ui16Parm1] = _ui8Val;
		}
	};

}	// namespace lsn

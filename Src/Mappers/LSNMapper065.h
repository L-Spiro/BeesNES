/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 065 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper065
	 * \brief Mapper 065 implementation.
	 *
	 * Description: Mapper 065 implementation.
	 */
	class CMapper065 : public CMapperBase {
	public :
		CMapper065() {
		}
		virtual ~CMapper065() {
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
		virtual void									InitWithRom( LSN_ROM &_rRom, CCpuBase * _pcbCpuBase, CInterruptable * _piInter, CBussable * _pbPpuBus ) {
			CMapperBase::InitWithRom( _rRom, _pcbCpuBase, _piInter, _pbPpuBus );
			m_ui8PgmBanks[0] = 0;
			m_ui8PgmBanks[1] = 1;
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
			m_stFixedOffset = std::max<size_t>( m_prRom->vPrgRom.size(), PgmBankSize() ) - PgmBankSize();
			m_s2ndToLast = std::max<size_t>( m_prRom->vPrgRom.size(), PgmBankSize() * 2 ) - PgmBankSize() * 2;
			for ( uint32_t I = 0xE000; I < 0x10000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead_Fixed, this, uint16_t( (I - 0xE000) % (m_prRom->vPrgRom.size() - m_stFixedOffset) ) );
			}
			

			// ================
			// SWAPPABLE BANKS
			// ================
			// CPU.
			for ( uint32_t I = 0x8000; I <= 0x9FFF; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper065::Read8000_9FFF_or_C000_DFFF<0b00000000>, this, uint16_t( I - 0x8000 ) );
			}
			for ( uint32_t I = 0xA000; I <= 0xBFFF; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<1, PgmBankSize()>, this, uint16_t( I - 0xA000 ) );
			}
			for ( uint32_t I = 0xC000; I <= 0xDFFF; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper065::Read8000_9FFF_or_C000_DFFF<0b10000000>, this, uint16_t( I - 0xC000 ) );
			}
			// PPU.
			for ( uint32_t I = 0x0000; I < 0x0400; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<0, ChrBankSize()>, this, uint16_t( I - 0x0000 ) );
			}
			for ( uint32_t I = 0x0400; I < 0x0800; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<1, ChrBankSize()>, this, uint16_t( I - 0x0400 ) );
			}
			for ( uint32_t I = 0x0800; I < 0x0C00; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<2, ChrBankSize()>, this, uint16_t( I - 0x0800 ) );
			}
			for ( uint32_t I = 0x0C00; I < 0x1000; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<3, ChrBankSize()>, this, uint16_t( I - 0x0C00 ) );
			}
			for ( uint32_t I = 0x1000; I < 0x1400; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<4, ChrBankSize()>, this, uint16_t( I - 0x1000 ) );
			}
			for ( uint32_t I = 0x1400; I < 0x1800; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<5, ChrBankSize()>, this, uint16_t( I - 0x1400 ) );
			}
			for ( uint32_t I = 0x1800; I < 0x1C00; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<6, ChrBankSize()>, this, uint16_t( I - 0x1800 ) );
			}
			for ( uint32_t I = 0x1C00; I < 0x2000; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<7, ChrBankSize()>, this, uint16_t( I - 0x1C00 ) );
			}


			// ================
			// BANK-SELECT
			// ================
			// PGM bank-select.
			// $8000-$9FFF bank-select.
			for ( uint32_t I = 0x8000; I <= 0x8FFF; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper065::SelectBank8000_8FFF, this, 0 );
			}
			// PGM layout.
			_pbCpuBus->SetWriteFunc( uint16_t( 0x9003 ), &CMapper065::SetPgmLayout9000, this, 0 );
			// Mirroring.
			_pbCpuBus->SetWriteFunc( uint16_t( 0x9001 ), &CMapper065::SelectMirror9001, this, 0 );
			// IRQ.
			_pbCpuBus->SetWriteFunc( uint16_t( 0x9003 ), &CMapper065::SetIrqControl9003, this, 0 );
			_pbCpuBus->SetWriteFunc( uint16_t( 0x9004 ), &CMapper065::IrqReload9004, this, 0 );
			_pbCpuBus->SetWriteFunc( uint16_t( 0x9005 ), &CMapper065::SetIrqReloadHigh9005, this, 0 );
			_pbCpuBus->SetWriteFunc( uint16_t( 0x9006 ), &CMapper065::SetIrqReloadLow9006, this, 0 );
			// $A000-$BFFF bank-select.
			for ( uint32_t I = 0xA000; I <= 0xAFFF; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper065::SelectBankA000_AFFF, this, 0 );
			}
			// CHR bank-select.
			_pbCpuBus->SetWriteFunc( uint16_t( 0xB000 ), &CMapper065::SelectChrBankB00X<0>, this, 0 );
			_pbCpuBus->SetWriteFunc( uint16_t( 0xB001 ), &CMapper065::SelectChrBankB00X<1>, this, 0 );
			_pbCpuBus->SetWriteFunc( uint16_t( 0xB002 ), &CMapper065::SelectChrBankB00X<2>, this, 0 );
			_pbCpuBus->SetWriteFunc( uint16_t( 0xB003 ), &CMapper065::SelectChrBankB00X<3>, this, 0 );
			_pbCpuBus->SetWriteFunc( uint16_t( 0xB004 ), &CMapper065::SelectChrBankB00X<4>, this, 0 );
			_pbCpuBus->SetWriteFunc( uint16_t( 0xB005 ), &CMapper065::SelectChrBankB00X<5>, this, 0 );
			_pbCpuBus->SetWriteFunc( uint16_t( 0xB006 ), &CMapper065::SelectChrBankB00X<6>, this, 0 );
			_pbCpuBus->SetWriteFunc( uint16_t( 0xB007 ), &CMapper065::SelectChrBankB00X<7>, this, 0 );


			// ================
			// MIRRORING
			// ================
			ApplyControllableMirrorMap( _pbPpuBus );
		}

		/**
		 * Ticks with the CPU.
		 */
		virtual void									Tick() {
			if ( m_ui8Control & 0b10000000 ) {
				if ( m_ui16Counter != 0 ) {
					if ( --m_ui16Counter == 0 ) {
						m_pInterruptable->Irq( LSN_IS_MAPPER );
					}
				}
			}
		}


	protected :
		// == Members.
		/** The 2nd-to-last bank (VRC4). */
		size_t											m_s2ndToLast = 0;
		/** The reload value. */
		uint16_t										m_ui16Reload = 0;
		/** IRQ The counter. */
		uint16_t										m_ui16Counter = 0;
		/** The IRQ control. */
		uint8_t											m_ui8Control = 0;
		/** PGM layout. */
		uint8_t											m_ui8Layout = 0;


		// == Functions.
		/**
		 * Selects a CHR bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		template <unsigned _uBank>
		static void LSN_FASTCALL						SelectChrBankB00X( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper065 * pmThis = reinterpret_cast<CMapper065 *>(_pvParm0);
			pmThis->SetChrBank<_uBank, ChrBankSize()>( _ui8Val );
		}

		/**
		 * Reads from the swappable $8000-$9FFF/$C000-$DFFF bank (VRC4).  If the M bit matches the _uM value, the $8000 bank is read, otherwise the second-to-last fixed bank is used.
		 *
		 * \tparam _uM Matched against the M bit to decide on what to read.
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		template <unsigned _uM>
		static void LSN_FASTCALL						Read8000_9FFF_or_C000_DFFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t &_ui8Ret ) {
			CMapper065 * pmThis = reinterpret_cast<CMapper065 *>(_pvParm0);
			if ( (pmThis->m_ui8Layout & 0b10000000) == _uM ) {
				CMapperBase::PgmBankRead<0, PgmBankSize()>( _pvParm0, _ui16Parm1, _pui8Data, _ui8Ret );
			}
			else {
				_ui8Ret = pmThis->m_prRom->vPrgRom.data()[_ui16Parm1+pmThis->m_s2ndToLast];
			}
		}

		/**
		 * Selects a bank for $8000-$9FFF.
		 * 
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 **/
		static void LSN_FASTCALL						SelectBank8000_8FFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper065 * pmThis = reinterpret_cast<CMapper065 *>(_pvParm0);
			pmThis->SetPgmBank<0, PgmBankSize()>( _ui8Val );
		}

		/**
		 * Selects a bank for $A000-$BFFF.
		 * 
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 **/
		static void LSN_FASTCALL						SelectBankA000_AFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper065 * pmThis = reinterpret_cast<CMapper065 *>(_pvParm0);
			pmThis->SetPgmBank<1, PgmBankSize()>( _ui8Val );
		}

		/**
		 * Sets the layout.
		 * 
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 **/
		static void LSN_FASTCALL						SetPgmLayout9000( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper065 * pmThis = reinterpret_cast<CMapper065 *>(_pvParm0);
			pmThis->m_ui8Layout = _ui8Val;
		}

		/**
		 * Selects a mirroring mode.
		 * 
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 **/
		static void LSN_FASTCALL						SelectMirror9001( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper065 * pmThis = reinterpret_cast<CMapper065 *>(_pvParm0);
			switch ( _ui8Val >> 6 ) {
				case 0b00 : {
					pmThis->m_mmMirror = LSN_MM_VERTICAL;
					break;
				}
				case 0b10 : {
					pmThis->m_mmMirror = LSN_MM_HORIZONTAL;
					break;
				}
				default : {
					pmThis->m_mmMirror = LSN_MM_1_SCREEN_A;
					break;
				}
			}
		}

		/**
		 * Sets the IRQ control.
		 * 
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 **/
		static void LSN_FASTCALL						SetIrqControl9003( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper065 * pmThis = reinterpret_cast<CMapper065 *>(_pvParm0);
			pmThis->m_ui8Control = _ui8Val;
			pmThis->m_pInterruptable->ClearIrq( LSN_IS_MAPPER );
		}

		/**
		 * Reloads the IRQ counter.
		 * 
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 **/
		static void LSN_FASTCALL						IrqReload9004( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t /*_ui8Val*/ ) {
			CMapper065 * pmThis = reinterpret_cast<CMapper065 *>(_pvParm0);
			pmThis->m_ui16Counter = pmThis->m_ui16Reload;
			pmThis->m_pInterruptable->ClearIrq( LSN_IS_MAPPER );
		}

		/**
		 * Sets the high 8 bits of the IRQ reload value.
		 * 
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 **/
		static void LSN_FASTCALL						SetIrqReloadHigh9005( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper065 * pmThis = reinterpret_cast<CMapper065 *>(_pvParm0);
			pmThis->m_ui16Reload = (pmThis->m_ui16Reload & 0x00FF) | (uint16_t( _ui8Val ) << 8);
		}

		/**
		 * Sets the low 8 bits of the IRQ reload value.
		 * 
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 **/
		static void LSN_FASTCALL						SetIrqReloadLow9006( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper065 * pmThis = reinterpret_cast<CMapper065 *>(_pvParm0);
			pmThis->m_ui16Reload = (pmThis->m_ui16Reload & 0xFF00) | _ui8Val;
		}
	};

}	// namespace lsn

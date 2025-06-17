/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 157 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper157
	 * \brief Mapper 157 implementation.
	 *
	 * Description: Mapper 157 implementation.
	 */
	class CMapper157 : public CMapperBase {
	public :
		CMapper157() {
		}
		virtual ~CMapper157() {
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
		virtual void									InitWithRom( LSN_ROM &_rRom, CCpuBase * _pcbCpuBase, CInterruptable * _piInter, CBussable * _pbPpuBus ) {
			CMapperBase::InitWithRom( _rRom, _pcbCpuBase, _piInter, _pbPpuBus );
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


			// ================
			// BANK-SELECT
			// ================
			// PGM bank-select.
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				if ( (I & 0x800F) == 0x8008 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper157::SelectBank8008, this, 0 );
				}
				else if ( (I & 0x800F) == 0x8009 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper157::SelectMirror8009, this, 0 );
				}
				else if ( (I & 0x800F) == 0x800A ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper157::WriteIrqControl800A, this, 0 );
				}
				else if ( (I & 0x800F) == 0x800B ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper157::WriteIrqReloadHi800B, this, 0 );
				}
				else if ( (I & 0x800F) == 0x800C ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper157::WriteIrqReloadHi800C, this, 0 );
				}
				
			}


			// ================
			// MIRRORING
			// ================
			ApplyControllableMirrorMap( _pbPpuBus );
		}

		/**
		 * Ticks with the CPU.
		 */
		virtual void									Tick() {
			if ( m_ui8Control & 0b00000001 ) {
				if ( m_ui16Counter ) {
					--m_ui16Counter;
				}
				if ( m_ui16Counter == 0 ) {
					m_pInterruptable->Irq( LSN_IS_VRC4_5_6 );
				}
			}
		}


	protected :
		// == Members.
		/** IRQ counter. */
		uint16_t										m_ui16Counter = 0;
		/** IRQ reload value. */
		uint16_t										m_ui16Reload = 0;
		/** IRQ control. */
		uint8_t											m_ui8Control = 0;


		// == Functions.
		/**
		 * Selects a bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		template <unsigned _uBank = 0>
		static void LSN_FASTCALL						SelectBank8008( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper157 * pmThis = reinterpret_cast<CMapper157 *>(_pvParm0);
			pmThis->SetPgmBank<_uBank, PgmBankSize()>( _ui8Val & 0b1111 );
		}

		/**
		 * Selects a CHR bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		template <unsigned _uBank = 0>
		static void LSN_FASTCALL						SelectChrBankX000_XFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper157 * pmThis = reinterpret_cast<CMapper157 *>(_pvParm0);
			pmThis->SetChrBank<_uBank, ChrBankSize()>( _ui8Val );
		}

		/**
		 * Selects a mirroring mode.
		 * 
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 **/
		static void LSN_FASTCALL						SelectMirror8009( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper157 * pmThis = reinterpret_cast<CMapper157 *>(_pvParm0);

			switch ( _ui8Val & 0b11 ) {
				case 0b00 : {
					pmThis->m_mmMirror = LSN_MM_VERTICAL;
					break;
				}
				case 0b01 : {
					pmThis->m_mmMirror = LSN_MM_HORIZONTAL;
					break;
				}
				case 0b10 : {
					pmThis->m_mmMirror = LSN_MM_1_SCREEN_A;
					break;
				}
				case 0b11 : {
					pmThis->m_mmMirror = LSN_MM_1_SCREEN_B;
					break;
				}
			}
		}

		/**
		 * Writes to the IRQ control.
		 * 
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 **/
		static void LSN_FASTCALL						WriteIrqControl800A( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper157 * pmThis = reinterpret_cast<CMapper157 *>(_pvParm0);

			pmThis->m_ui8Control = _ui8Val;
			pmThis->m_pInterruptable->ClearIrq( LSN_IS_VRC4_5_6 );
			if ( pmThis->m_ui8Control & 0b00000001 ) {
				if ( pmThis->m_ui16Counter == 0 ) {
					pmThis->m_pInterruptable->Irq( LSN_IS_VRC4_5_6 );
				}
			}
			pmThis->m_ui16Counter = pmThis->m_ui16Reload;
		}

		/**
		 * Writes to the IRQ reload value low byte.
		 * 
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 **/
		static void LSN_FASTCALL						WriteIrqReloadHi800B( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper157 * pmThis = reinterpret_cast<CMapper157 *>(_pvParm0);

			pmThis->m_ui16Reload = (pmThis->m_ui16Reload & 0xFF00) | _ui8Val;
		}

		/**
		 * Writes to the IRQ reload value high byte.
		 * 
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 **/
		static void LSN_FASTCALL						WriteIrqReloadHi800C( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper157 * pmThis = reinterpret_cast<CMapper157 *>(_pvParm0);

			pmThis->m_ui16Reload = (pmThis->m_ui16Reload & 0x00FF) | (uint16_t( _ui8Val ) << 8);
		}
	};

}	// namespace lsn

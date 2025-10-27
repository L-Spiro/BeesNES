/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 067 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper067
	 * \brief Mapper 067 implementation.
	 *
	 * Description: Mapper 067 implementation.
	 */
	class CMapper067 : public CMapperBase {
	public :
		CMapper067() {
		}
		virtual ~CMapper067() {
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
		static constexpr uint16_t						ChrBankSize() { return 2 * 1024; }

		/**
		 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
		 *
		 * \param _rRom The ROM data.
		 * \param _pcbCpuBase A pointer to the CPU.
		 */
		virtual void									InitWithRom( LSN_ROM &_rRom, CCpuBase * _pcbCpuBase, CPpuBase * _ppbPpuBase, CInterruptable * _piInter, CBussable * _pbPpuBus ) {
			CMapperBase::InitWithRom( _rRom, _pcbCpuBase, _ppbPpuBase, _piInter, _pbPpuBus );
			SanitizeRegs<PgmBankSize(), ChrBankSize()>();
			m_bIrqLatch = false;
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
			// PPU.
			for ( uint32_t I = 0x0000; I < 0x0800; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<0, ChrBankSize()>, this, uint16_t( I - 0x0000 ) );
			}
			for ( uint32_t I = 0x0800; I < 0x1000; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<1, ChrBankSize()>, this, uint16_t( I - 0x0800 ) );
			}
			for ( uint32_t I = 0x1000; I < 0x1800; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<2, ChrBankSize()>, this, uint16_t( I - 0x1000 ) );
			}
			for ( uint32_t I = 0x1800; I < 0x2000; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<3, ChrBankSize()>, this, uint16_t( I - 0x1800 ) );
			}


			// ================
			// BANK-SELECT
			// ================
			// PGM bank-select.
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				if ( (I & 0x8800) == 0x8000 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper067::AcknowledgeIrq8000, this, 0 );
				}
				else if ( (I & 0xF800) == 0x8800 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper067::SelectChrBank8800_BFFF<0>, this, 0 );
				}
				else if ( (I & 0xF800) == 0x9800 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper067::SelectChrBank8800_BFFF<1>, this, 0 );
				}
				else if ( (I & 0xF800) == 0xA800 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper067::SelectChrBank8800_BFFF<2>, this, 0 );
				}
				else if ( (I & 0xF800) == 0xB800 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper067::SelectChrBank8800_BFFF<3>, this, 0 );
				}
				else if ( (I & 0xF800) == 0xC800 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper067::WriteIrqCounterC800, this, 0 );
				}
				else if ( (I & 0xF800) == 0xD800 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper067::WriteIrqControlD800, this, 0 );
				}
				else if ( (I & 0xF800) == 0xE800 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper067::SelectMirrorE800, this, 0 );
				}
				else if ( (I & 0xF800) == 0xF800 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper067::SelectBankF800, this, 0 );
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
			if ( m_ui8Control & 0b00010000 ) {
				if ( --m_ui16Counter == 0xFFFF ) {
					m_pInterruptable->Irq( LSN_IS_MAPPER );
					m_ui8Control &= ~0b00010000;
				}
			}
		}


	protected :
		// == Members.
		/** IRQ counter. */
		uint16_t										m_ui16Counter = 0;
		/** IRQ control. */
		uint8_t											m_ui8Control = 0;
		/** IRQ reload latch. */
		bool											m_bIrqLatch = false;


		// == Functions.
		/**
		 * Selects a bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBankF800( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper067 * pmThis = reinterpret_cast<CMapper067 *>(_pvParm0);
			pmThis->SetPgmBank<0, PgmBankSize()>( _ui8Val & 0b1111 );
		}

		/**
		 * Selects a CHR bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		template <unsigned _uBank>
		static void LSN_FASTCALL						SelectChrBank8800_BFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper067 * pmThis = reinterpret_cast<CMapper067 *>(_pvParm0);
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
		static void LSN_FASTCALL						SelectMirrorE800( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper067 * pmThis = reinterpret_cast<CMapper067 *>(_pvParm0);

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
		 * Acknowledges IRQ.
		 * 
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 **/
		static void LSN_FASTCALL						AcknowledgeIrq8000( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t /*_ui8Val*/ ) {
			CMapper067 * pmThis = reinterpret_cast<CMapper067 *>(_pvParm0);

			pmThis->m_pInterruptable->ClearIrq( LSN_IS_MAPPER );
		}

		/**
		 * Writes to the IRQ counter.
		 * 
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 **/
		static void LSN_FASTCALL						WriteIrqCounterC800( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper067 * pmThis = reinterpret_cast<CMapper067 *>(_pvParm0);

			if ( !pmThis->m_bIrqLatch ) {
				pmThis->m_ui16Counter = (pmThis->m_ui16Counter & 0x00FF) | (uint16_t( _ui8Val ) << 8);
			}
			else {
				pmThis->m_ui16Counter = (pmThis->m_ui16Counter & 0xFF00) | _ui8Val;
			}
			pmThis->m_bIrqLatch = !pmThis->m_bIrqLatch;
		}

		/**
		 * Writes to the IRQ control.
		 * 
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 **/
		static void LSN_FASTCALL						WriteIrqControlD800( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper067 * pmThis = reinterpret_cast<CMapper067 *>(_pvParm0);

			pmThis->m_ui8Control = _ui8Val;
			pmThis->m_bIrqLatch = false;
		}
	};

}	// namespace lsn

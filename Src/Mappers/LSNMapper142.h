/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 142 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper142
	 * \brief Mapper 142 implementation.
	 *
	 * Description: Mapper 142 implementation.
	 */
	class CMapper142 : public CMapperBase {
	public :
		CMapper142() {
		}
		virtual ~CMapper142() {
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
			for ( uint32_t I = 0xE000; I < 0x10000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead_Fixed, this, uint16_t( (I - 0xE000) % (m_prRom->vPrgRom.size() - m_stFixedOffset) ) );
			}
			

			// ================
			// SWAPPABLE BANKS
			// ================
			// CPU.
			for ( uint32_t I = 0x6000; I < 0x8000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<0, PgmBankSize()>, this, uint16_t( I - 0x6000 ) );
			}
			for ( uint32_t I = 0x8000; I < 0xA000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<1, PgmBankSize()>, this, uint16_t( I - 0x8000 ) );
			}
			for ( uint32_t I = 0xA000; I < 0xC000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<2, PgmBankSize()>, this, uint16_t( I - 0xA000 ) );
			}
			for ( uint32_t I = 0xC000; I < 0xE000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<3, PgmBankSize()>, this, uint16_t( I - 0xC000 ) );
			}


			// ================
			// BANK-SELECT
			// ================
			 // IRQ.
			for ( uint32_t I = 0x8000; I < 0x9000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper142::WriteIrqLatch0_8000_8FFF, this, 0 );
			}
			for ( uint32_t I = 0x9000; I < 0xA000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper142::WriteIrqLatch1_9000_9FFF, this, 0 );
			}
			for ( uint32_t I = 0xA000; I < 0xB000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper142::WriteIrqLatch2_A000_AFFF, this, 0 );
			}
			for ( uint32_t I = 0xB000; I < 0xC000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper142::WriteIrqLatch3_B000_BFFF, this, 0 );
			}
			for ( uint32_t I = 0xC000; I < 0xD000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper142::WriteIrqControl_C000_CFFF, this, 0 );
			}
			for ( uint32_t I = 0xD000; I < 0xE000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper142::WriteIrqAcknowledge_D000_DFFF, this, 0 );
			}
			// PGM bank-select.
			for ( uint32_t I = 0xE000; I < 0xF000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper142::SelectBankE000_EFFF, this, 0 );
			}
			for ( uint32_t I = 0xF000; I < 0x10000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper142::SelectBankF000_FFFF, this, 0 );
			}
		}

		/**
		 * Ticks with the CPU.
		 */
		virtual void									Tick() {
			m_viIrq.Tick( m_pInterruptable );
		}


	protected :
		// == Members.
		CVrcIrq3<false>									m_viIrq;					/**< VRC3 IRQ. */
		uint8_t											m_ui8BankSelect;			/**< Bank selector. */


		// == Functions.
		/**
		 * Selects a bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBankE000_EFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper142 * pmThis = reinterpret_cast<CMapper142 *>(_pvParm0);
			pmThis->m_ui8BankSelect = _ui8Val & 0b0111;
		}

		/**
		 * Selects a bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBankF000_FFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper142 * pmThis = reinterpret_cast<CMapper142 *>(_pvParm0);
			switch ( pmThis->m_ui8BankSelect ) {
				case 0 : {
					// Unknown.
					break;
				}
				case 1 : {
					pmThis->SetPgmBank<1, PgmBankSize()>( _ui8Val & 0b1111 );
					break;
				}
				case 2 : {
					pmThis->SetPgmBank<2, PgmBankSize()>( _ui8Val & 0b1111 );
					break;
				}
				case 3 : {
					pmThis->SetPgmBank<3, PgmBankSize()>( _ui8Val & 0b1111 );
					break;
				}
				case 4 : {
					pmThis->SetPgmBank<0, PgmBankSize()>( _ui8Val & 0b1111 );
					break;
				}
				case 5 : {
					// Unknown.
					break;
				}
				case 6 : {
					// No effect.
					break;
				}
				case 7 : {
					// Unknown.
					break;
				}
			}
			
		}

		/**
		 * Writes to the IRQ latch register.
		 * 
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 **/
		static void LSN_FASTCALL						WriteIrqLatch0_8000_8FFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper142 * pmThis = reinterpret_cast<CMapper142 *>(_pvParm0);
			pmThis->m_viIrq.SetLatch_0_3( _ui8Val );
		}

		/**
		 * Writes to the IRQ latch register.
		 * 
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 **/
		static void LSN_FASTCALL						WriteIrqLatch1_9000_9FFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper142 * pmThis = reinterpret_cast<CMapper142 *>(_pvParm0);
			pmThis->m_viIrq.SetLatch_4_7( _ui8Val );
		}

		/**
		 * Writes to the IRQ latch register.
		 * 
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 **/
		static void LSN_FASTCALL						WriteIrqLatch2_A000_AFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper142 * pmThis = reinterpret_cast<CMapper142 *>(_pvParm0);
			pmThis->m_viIrq.SetLatch_8_11( _ui8Val );
		}

		/**
		 * Writes to the IRQ latch register.
		 * 
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 **/
		static void LSN_FASTCALL						WriteIrqLatch3_B000_BFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper142 * pmThis = reinterpret_cast<CMapper142 *>(_pvParm0);
			pmThis->m_viIrq.SetLatch_12_15( _ui8Val );
		}

		/**
		 * Writes to the IRQ control register.
		 * 
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 **/
		static void LSN_FASTCALL						WriteIrqControl_C000_CFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper142 * pmThis = reinterpret_cast<CMapper142 *>(_pvParm0);
			pmThis->m_viIrq.SetControl( _ui8Val, pmThis->m_pInterruptable );
		}

		/**
		 * Acknowledges IRQ.
		 * 
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 **/
		static void LSN_FASTCALL						WriteIrqAcknowledge_D000_DFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t /*_ui8Val*/ ) {
			CMapper142 * pmThis = reinterpret_cast<CMapper142 *>(_pvParm0);
			pmThis->m_viIrq.AcknowledgeIrq( pmThis->m_pInterruptable );
		}

	};

}	// namespace lsn

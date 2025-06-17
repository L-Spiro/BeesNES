/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 073 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper073
	 * \brief Mapper 073 implementation.
	 *
	 * Description: Mapper 073 implementation.
	 */
	class CMapper073 : public CMapperBase {
	public :
		CMapper073() {
		}
		virtual ~CMapper073() {
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
			//if ( m_prRom->i32SaveRamSize ) {
				m_vPrgRam.resize( 0x8000 - 0x6000 );
				for ( uint32_t I = 0x6000; I < 0x8000; ++I ) {
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper073::ReadWram, this, uint16_t( I - 0x6000 ) );
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper073::WriteWram, this, uint16_t( I - 0x6000 ) );
				}
			//}
			// PPU.
			/*for ( uint32_t I = 0x0000; I < 0x2000; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::ChrBankRead<0, ChrBankSize()>, this, uint16_t( I - 0x0000 ) );
			}*/


			// ================
			// BANK-SELECT
			// ================
			// IRQ.
			for ( uint32_t I = 0x8000; I < 0x9000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper073::WriteIrqLatch0_8000_8FFF, this, 0 );
			}
			for ( uint32_t I = 0x9000; I < 0xA000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper073::WriteIrqLatch1_9000_9FFF, this, 0 );
			}
			for ( uint32_t I = 0xA000; I < 0xB000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper073::WriteIrqLatch2_A000_AFFF, this, 0 );
			}
			for ( uint32_t I = 0xB000; I < 0xC000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper073::WriteIrqLatch3_B000_BFFF, this, 0 );
			}
			for ( uint32_t I = 0xC000; I < 0xD000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper073::WriteIrqControl_C000_CFFF, this, 0 );
			}
			for ( uint32_t I = 0xD000; I < 0xE000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper073::WriteIrqAcknowledge_D000_DFFF, this, 0 );
			}
			// PGM bank-select.
			for ( uint32_t I = 0xF000; I < 0x10000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper073::SelectBankF000_FFFF, this, 0 );
			}


			// ================
			// MIRRORING
			// ================
			//ApplyControllableMirrorMap( _pbPpuBus );
		}

		/**
		 * Ticks with the CPU.
		 */
		virtual void									Tick() {
			m_viIrq.Tick( m_pInterruptable );
		}


	protected :
		// == Members.
		/** WRAM. */
		std::vector<uint8_t>							m_vPrgRam;
		/** VRC3 IRQ. */
		CVrcIrq3										m_viIrq;


		// == Functions.
		/**
		 * Selects a bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBankF000_FFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper073 * pmThis = reinterpret_cast<CMapper073 *>(_pvParm0);
			pmThis->SetPgmBank<0, PgmBankSize()>( _ui8Val & 0b0111 );
		}

		/**
		 * Reads WRAM $6000-$7FFF/$C000-$DFFF bank (VRC4).  If the M bit matches the _uM value, the $8000 bank is read, otherwise the second-to-last fixed bank is used.
		 *
		 * \tparam _uM Matched against the M bit to decide on what to read.
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						ReadWram( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper073 * pmThis = reinterpret_cast<CMapper073 *>(_pvParm0);
			_ui8Ret = pmThis->m_vPrgRam[_ui16Parm1];
		}

		/**
		 * Writes to WRAM (VRC4).
		 * 
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 **/
		static void LSN_FASTCALL						WriteWram( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper073 * pmThis = reinterpret_cast<CMapper073 *>(_pvParm0);
			pmThis->m_vPrgRam[_ui16Parm1] = _ui8Val;
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
			CMapper073 * pmThis = reinterpret_cast<CMapper073 *>(_pvParm0);
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
			CMapper073 * pmThis = reinterpret_cast<CMapper073 *>(_pvParm0);
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
			CMapper073 * pmThis = reinterpret_cast<CMapper073 *>(_pvParm0);
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
			CMapper073 * pmThis = reinterpret_cast<CMapper073 *>(_pvParm0);
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
			CMapper073 * pmThis = reinterpret_cast<CMapper073 *>(_pvParm0);
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
			CMapper073 * pmThis = reinterpret_cast<CMapper073 *>(_pvParm0);
			pmThis->m_viIrq.AcknowledgeIrq( pmThis->m_pInterruptable );
		}
	};

}	// namespace lsn

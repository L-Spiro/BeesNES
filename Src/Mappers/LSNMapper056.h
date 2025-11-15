/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 056 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper056
	 * \brief Mapper 056 implementation.
	 *
	 * Description: Mapper 056 implementation.
	 */
	class CMapper056 : public CMapperBase {
	public :
		CMapper056() {
		}
		virtual ~CMapper056() {
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
			for ( uint16_t I = 0; I < std::size( m_ui8PgmBanksF0xx ); ++I ) {
				m_ui8PgmBanksF0xx[I] = (m_ui8PgmBanksF0xx[I] & 0b00001111) | 0b00010000;

				SetPgmBank<PgmBankSize()>( I, m_ui8PgmBanksF0xx[I] );
			}
			SetPgmBank<0, PgmBankSize()>( -1 );
			m_ui8PgmBanksF0xx[0] = m_ui8PgmBanks[0];

			m_vPrgRam.resize( PgmBankSize() );
			for ( uint32_t I = 0x6000; I < 0x8000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper056::ReadWRam, this, uint16_t( (I - 0x6000) % PgmBankSize() ) );
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper056::WriteWRam, this, uint16_t( (I - 0x6000) % PgmBankSize() ) );
			}
			

			// ================
			// SWAPPABLE BANKS
			// ================
			// CPU.
			for ( uint32_t I = 0x8000; I < 0xA000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<1, PgmBankSize()>, this, uint16_t( I - 0x8000 ) );
			}
			for ( uint32_t I = 0xA000; I < 0xC000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<2, PgmBankSize()>, this, uint16_t( I - 0xA000 ) );
			}
			for ( uint32_t I = 0xC000; I < 0xE000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<3, PgmBankSize()>, this, uint16_t( I - 0xC000 ) );
			}
			for ( uint32_t I = 0xE000; I < 0x10000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<0, PgmBankSize()>, this, uint16_t( I - 0xE000 ) );
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
			 // IRQ.
			for ( uint32_t I = 0x8000; I < 0x9000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper056::WriteIrqLatch0_8000_8FFF, this, 0 );
			}
			for ( uint32_t I = 0x9000; I < 0xA000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper056::WriteIrqLatch1_9000_9FFF, this, 0 );
			}
			for ( uint32_t I = 0xA000; I < 0xB000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper056::WriteIrqLatch2_A000_AFFF, this, 0 );
			}
			for ( uint32_t I = 0xB000; I < 0xC000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper056::WriteIrqLatch3_B000_BFFF, this, 0 );
			}
			for ( uint32_t I = 0xC000; I < 0xD000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper056::WriteIrqControl_C000_CFFF, this, 0 );
			}
			for ( uint32_t I = 0xD000; I < 0xE000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper056::WriteIrqAcknowledge_D000_DFFF, this, 0 );
			}
			// PGM bank-select.
			for ( uint32_t I = 0xE000; I < 0xF000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper056::SelectBankE000_EFFF, this, 0 );
			}
			for ( uint32_t I = 0xF000; I < 0x10000; ++I ) {
				// PGM.
				if ( (I & 0xFC03) == 0xF000 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper056::SelectBankF00X<1>, this, 0 );
				}
				else if ( (I & 0xFC03) == 0xF001 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper056::SelectBankF00X<2>, this, 0 );
				}
				else if ( (I & 0xFC03) == 0xF002 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper056::SelectBankF00X<3>, this, 0 );
				}
				else if ( (I & 0xFC03) == 0xF003 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper056::SelectBankF00X<0>, this, 0 );
				}

				// CHR.
				else if ( (I & 0xFC07) == 0xFC00 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper056::SelectBankFC0X<0>, this, 0 );
				}
				else if ( (I & 0xFC07) == 0xFC01 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper056::SelectBankFC0X<1>, this, 0 );
				}
				else if ( (I & 0xFC07) == 0xFC02 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper056::SelectBankFC0X<2>, this, 0 );
				}
				else if ( (I & 0xFC07) == 0xFC03 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper056::SelectBankFC0X<3>, this, 0 );
				}
				else if ( (I & 0xFC07) == 0xFC04 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper056::SelectBankFC0X<4>, this, 0 );
				}
				else if ( (I & 0xFC07) == 0xFC05 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper056::SelectBankFC0X<5>, this, 0 );
				}
				else if ( (I & 0xFC07) == 0xFC06 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper056::SelectBankFC0X<6>, this, 0 );
				}
				else if ( (I & 0xFC07) == 0xFC07 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper056::SelectBankFC0X<7>, this, 0 );
				}

				// Mirroring.
				else if ( (I & 0xFC00) == 0xF800 ) {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper056::SelectMirrorF800, this, 0 );
				}

				else {
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper056::SelectBankF000_FFFF, this, 0 );
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
			m_viIrq.Tick( m_pInterruptable );
		}


	protected :
		// == Members.
		std::vector<uint8_t>							m_vPrgRam;										/**< PRG RAM. */
		CVrcIrq3<false>									m_viIrq;										/**< VRC3 IRQ. */
		uint8_t											m_ui8BankSelect;								/**< Bank selector. */
		uint8_t											m_ui8PgmBanksF0xx[4];							/**< The 4 PGM banks. */


		// == Functions.
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
			CMapper056 * pmThis = reinterpret_cast<CMapper056 *>(_pvParm0);
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
			CMapper056 * pmThis = reinterpret_cast<CMapper056 *>(_pvParm0);
			pmThis->m_vPrgRam[_ui16Parm1] = _ui8Val;
		}

		/**
		 * Selects a bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBankE000_EFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper056 * pmThis = reinterpret_cast<CMapper056 *>(_pvParm0);
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
			CMapper056 * pmThis = reinterpret_cast<CMapper056 *>(_pvParm0);
			switch ( pmThis->m_ui8BankSelect ) {
				case 0 : {
					// Unknown.
					break;
				}
				case 1 : {
					pmThis->m_ui8PgmBanksF0xx[1] = (pmThis->m_ui8PgmBanksF0xx[1] & 0b00010000) | (_ui8Val & 0b00001111);
					pmThis->SetPgmBank<1, PgmBankSize()>( pmThis->m_ui8PgmBanksF0xx[1] );
					break;
				}
				case 2 : {
					pmThis->m_ui8PgmBanksF0xx[2] = (pmThis->m_ui8PgmBanksF0xx[2] & 0b00010000) | (_ui8Val & 0b00001111);
					pmThis->SetPgmBank<2, PgmBankSize()>( pmThis->m_ui8PgmBanksF0xx[2] );
					break;
				}
				case 3 : {
					pmThis->m_ui8PgmBanksF0xx[3] = (pmThis->m_ui8PgmBanksF0xx[3] & 0b00010000) | (_ui8Val & 0b00001111);
					pmThis->SetPgmBank<3, PgmBankSize()>( pmThis->m_ui8PgmBanksF0xx[3] );
					break;
				}
				case 4 : {
					// No effect.
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
		 * PRG bank upper bits ($F000-$F3FF).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		template <size_t _sRegIdx>
		static void LSN_FASTCALL						SelectBankF00X( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t _ui8Val ) {
			CMapper056 * pmThis = reinterpret_cast<CMapper056 *>(_pvParm0);
			pmThis->m_ui8PgmBanksF0xx[_sRegIdx] = (pmThis->m_ui8PgmBanksF0xx[_sRegIdx] & 0b00001111) | (_ui8Val & 0b00010000);

			pmThis->SetPgmBank<_sRegIdx, PgmBankSize()>( pmThis->m_ui8PgmBanksF0xx[_sRegIdx] );

			SelectBankF000_FFFF( _pvParm0, _ui16Parm1, _pui8Data, _ui8Val );
		}

		/**
		 * CHR banking control ($FC00-$FFFF).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		template <size_t _sRegIdx>
		static void LSN_FASTCALL						SelectBankFC0X( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t _ui8Val ) {
			CMapper056 * pmThis = reinterpret_cast<CMapper056 *>(_pvParm0);
			pmThis->SetChrBank<_sRegIdx, ChrBankSize()>( _ui8Val & 0b01111111 );

			SelectBankF000_FFFF( _pvParm0, _ui16Parm1, _pui8Data, _ui8Val );
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
			CMapper056 * pmThis = reinterpret_cast<CMapper056 *>(_pvParm0);
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
			CMapper056 * pmThis = reinterpret_cast<CMapper056 *>(_pvParm0);
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
			CMapper056 * pmThis = reinterpret_cast<CMapper056 *>(_pvParm0);
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
			CMapper056 * pmThis = reinterpret_cast<CMapper056 *>(_pvParm0);
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
			CMapper056 * pmThis = reinterpret_cast<CMapper056 *>(_pvParm0);
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
			CMapper056 * pmThis = reinterpret_cast<CMapper056 *>(_pvParm0);
			pmThis->m_viIrq.AcknowledgeIrq( pmThis->m_pInterruptable );
		}

		/**
		 * Selects a mirroring mode.
		 * 
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 **/
		static void LSN_FASTCALL						SelectMirrorF800( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t _ui8Val ) {
			CMapper056 * pmThis = reinterpret_cast<CMapper056 *>(_pvParm0);

			switch ( _ui8Val & 0b01 ) {
				case 0b00 : {
					pmThis->m_mmMirror = LSN_MM_HORIZONTAL;
					break;
				}
				case 0b01 : {
					pmThis->m_mmMirror = LSN_MM_VERTICAL;
					break;
				}
			}

			SelectBankF000_FFFF( _pvParm0, _ui16Parm1, _pui8Data, _ui8Val );
		}

	};

}	// namespace lsn

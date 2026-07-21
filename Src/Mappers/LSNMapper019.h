/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 019 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNAudioNamco163.h"
#include "LSNMapperBase.h"

#include <algorithm>
#include <vector>

namespace lsn {

	/**
	 * Class CMapper019
	 * \brief Mapper 019 implementation.
	 *
	 * Description: NES Mapper 019 (Namco 163) implementation.
	 */
	class CMapper019 : public CMapperBase {
	public :
		CMapper019() {
		}
		virtual ~CMapper019() {
			SaveBatteryRam( m_vWram.data(), m_vWram.size() );
		}


		// == Functions.
		/**
		 * Gets the PGM bank size.
		 *
		 * \return Returns the size of the PGM banks (8 KiB).
		 */
		static constexpr uint16_t						PgmBankSize() { return 8 * 1024; }

		/**
		 * Gets the CHR bank size.
		 *
		 * \return Returns the size of the CHR banks (1 KiB).
		 */
		static constexpr uint16_t						ChrBankSize() { return 1 * 1024; }

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

			m_pInterruptable = _piInter;

			SetPgmBank<0, PgmBankSize()>( 0 );
			SetPgmBank<1, PgmBankSize()>( 1 );
			SetPgmBank<2, PgmBankSize()>( 2 );
			
			SetChrBank<0, ChrBankSize()>( 0 );
			SetChrBank<1, ChrBankSize()>( 1 );
			SetChrBank<2, ChrBankSize()>( 2 );
			SetChrBank<3, ChrBankSize()>( 3 );
			SetChrBank<4, ChrBankSize()>( 4 );
			SetChrBank<5, ChrBankSize()>( 5 );
			SetChrBank<6, ChrBankSize()>( 6 );
			SetChrBank<7, ChrBankSize()>( 7 );
			
			m_ui8E800 = 0;
			for ( size_t I = 0; I < 8; ++I ) {
				m_ui8ChrRegisters[I] = 0;
			}
			
			m_ui8ChrRegisters[8] = 0xE0;  // $2000.
			m_ui8ChrRegisters[9] = 0xE1;  // $2400.
			m_ui8ChrRegisters[10] = 0xE0; // $2800.
			m_ui8ChrRegisters[11] = 0xE1; // $2C00.
			
			m_vWram.resize( (8 * 1024) + 128 );
			std::fill( m_vWram.begin(), m_vWram.end(), uint8_t( 0 ) );

			LoadBatteryRam( m_vWram.data(), m_vWram.size() );

			m_vCiram.resize( 2 * 1024 );
			std::fill( m_vCiram.begin(), m_vCiram.end(), uint8_t( 0 ) );

			m_ui8InternalRamAddr = 0;
			m_bInternalRamAutoInc = false;
			
			m_ui16IrqCounter = 0;
			m_bIrqEnable = false;
		}

		/**
		 * Called to inform the mapper of a reset.
		 **/
		virtual void									Reset() override {
			SaveBatteryRam( m_vWram.data(), m_vWram.size() );
			m_anAudio.ResetFull( true );
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
			if ( m_prRom->vPrgRom.size() ) {
				m_stFixedOffset = std::max<size_t>( m_prRom->vPrgRom.size(), PgmBankSize() ) - PgmBankSize();
				for ( uint32_t I = 0xE000; I < 0x10000; ++I ) {
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead_Fixed, this, uint16_t( (I - 0xE000) % (m_prRom->vPrgRom.size() - m_stFixedOffset) ) );
				}
			}


			// ================
			// SWAPPABLE BANKS
			// ================
			// CPU.
			if ( m_prRom->vPrgRom.size() ) {
				for ( uint32_t I = 0x8000; I < 0xA000; ++I ) {
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<0, PgmBankSize()>, this, uint16_t( I - 0x8000 ) );
				}
				for ( uint32_t I = 0xA000; I < 0xC000; ++I ) {
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<1, PgmBankSize()>, this, uint16_t( I - 0xA000 ) );
				}
				for ( uint32_t I = 0xC000; I < 0xE000; ++I ) {
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead<2, PgmBankSize()>, this, uint16_t( I - 0xC000 ) );
				}
			}
			// PPU.
			if ( m_prRom->vChrRom.size() ) {
				// CHR-ROM / CIRAM mapping ($0000-$1FFF).
				for ( uint32_t I = 0x0000; I < 0x0400; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper019::PpuRead<0>, this, uint16_t( I - 0x0000 ) );
					_pbPpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::PpuWrite<0>, this, uint16_t( I - 0x0000 ) );
				}
				for ( uint32_t I = 0x0400; I < 0x0800; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper019::PpuRead<1>, this, uint16_t( I - 0x0400 ) );
					_pbPpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::PpuWrite<1>, this, uint16_t( I - 0x0400 ) );
				}
				for ( uint32_t I = 0x0800; I < 0x0C00; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper019::PpuRead<2>, this, uint16_t( I - 0x0800 ) );
					_pbPpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::PpuWrite<2>, this, uint16_t( I - 0x0800 ) );
				}
				for ( uint32_t I = 0x0C00; I < 0x1000; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper019::PpuRead<3>, this, uint16_t( I - 0x0C00 ) );
					_pbPpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::PpuWrite<3>, this, uint16_t( I - 0x0C00 ) );
				}
				for ( uint32_t I = 0x1000; I < 0x1400; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper019::PpuRead<4>, this, uint16_t( I - 0x1000 ) );
					_pbPpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::PpuWrite<4>, this, uint16_t( I - 0x1000 ) );
				}
				for ( uint32_t I = 0x1400; I < 0x1800; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper019::PpuRead<5>, this, uint16_t( I - 0x1400 ) );
					_pbPpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::PpuWrite<5>, this, uint16_t( I - 0x1400 ) );
				}
				for ( uint32_t I = 0x1800; I < 0x1C00; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper019::PpuRead<6>, this, uint16_t( I - 0x1800 ) );
					_pbPpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::PpuWrite<6>, this, uint16_t( I - 0x1800 ) );
				}
				for ( uint32_t I = 0x1C00; I < 0x2000; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper019::PpuRead<7>, this, uint16_t( I - 0x1C00 ) );
					_pbPpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::PpuWrite<7>, this, uint16_t( I - 0x1C00 ) );
				}

				// Nametables ($2000-$2FFF). Replaces ApplyControllableMirrorMap().
				for ( uint32_t I = 0x2000; I < 0x2400; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper019::PpuRead<8>, this, uint16_t( I - 0x2000 ) );
					_pbPpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::PpuWrite<8>, this, uint16_t( I - 0x2000 ) );
				}
				for ( uint32_t I = 0x2400; I < 0x2800; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper019::PpuRead<9>, this, uint16_t( I - 0x2400 ) );
					_pbPpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::PpuWrite<9>, this, uint16_t( I - 0x2400 ) );
				}
				for ( uint32_t I = 0x2800; I < 0x2C00; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper019::PpuRead<10>, this, uint16_t( I - 0x2800 ) );
					_pbPpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::PpuWrite<10>, this, uint16_t( I - 0x2800 ) );
				}
				for ( uint32_t I = 0x2C00; I < 0x3000; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper019::PpuRead<11>, this, uint16_t( I - 0x2C00 ) );
					_pbPpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::PpuWrite<11>, this, uint16_t( I - 0x2C00 ) );
				}
				
				// Nametable Mirrors ($3000-$3EFF).
				for ( uint32_t I = 0x3000; I < 0x3400; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper019::PpuRead<8>, this, uint16_t( I - 0x3000 ) );
					_pbPpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::PpuWrite<8>, this, uint16_t( I - 0x3000 ) );
				}
				for ( uint32_t I = 0x3400; I < 0x3800; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper019::PpuRead<9>, this, uint16_t( I - 0x3400 ) );
					_pbPpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::PpuWrite<9>, this, uint16_t( I - 0x3400 ) );
				}
				for ( uint32_t I = 0x3800; I < 0x3C00; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper019::PpuRead<10>, this, uint16_t( I - 0x3800 ) );
					_pbPpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::PpuWrite<10>, this, uint16_t( I - 0x3800 ) );
				}
				for ( uint32_t I = 0x3C00; I < 0x3F00; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper019::PpuRead<11>, this, uint16_t( I - 0x3C00 ) );
					_pbPpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::PpuWrite<11>, this, uint16_t( I - 0x3C00 ) );
				}
			}


			// ================
			// BANK-SELECT & RAM
			// ================
			// Internal Namco 163 Data Port ($4800-$4FFF).
			if ( m_prRom->riInfo.ui16SubMapper != 2 ) {
				m_anAudio.SetRam( &m_vWram[8*1024] );
				for ( uint32_t I = 0x4800; I < 0x5000; ++I ) {
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper019::InternalRamRead, this, 0 );
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::InternalRamWrite, this, 0 );
				}
			}
			
			// IRQ Low ($5000-$57FF).
			for ( uint32_t I = 0x5000; I < 0x5800; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper019::Read5000, this, 0 );
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::Write5000, this, 0 );
			}
			
			// IRQ High / Enable ($5800-$5FFF).
			for ( uint32_t I = 0x5800; I < 0x6000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper019::Read5800, this, 0 );
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::Write5800, this, 0 );
			}

			// WRAM ($6000-$7FFF).
			for ( uint32_t I = 0x6000; I < 0x8000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper019::WramRead, this, uint16_t( I - 0x6000 ) );
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::WramWrite, this, uint16_t( I - 0x6000 ) );
			}

			// CHR / NT Select 0..11 ($8000-$DFFF).
			for ( uint32_t I = 0x8000; I < 0x8800; ++I ) { _pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::SetPpuBank<0>, this, 0 ); }
			for ( uint32_t I = 0x8800; I < 0x9000; ++I ) { _pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::SetPpuBank<1>, this, 0 ); }
			for ( uint32_t I = 0x9000; I < 0x9800; ++I ) { _pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::SetPpuBank<2>, this, 0 ); }
			for ( uint32_t I = 0x9800; I < 0xA000; ++I ) { _pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::SetPpuBank<3>, this, 0 ); }
			for ( uint32_t I = 0xA000; I < 0xA800; ++I ) { _pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::SetPpuBank<4>, this, 0 ); }
			for ( uint32_t I = 0xA800; I < 0xB000; ++I ) { _pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::SetPpuBank<5>, this, 0 ); }
			for ( uint32_t I = 0xB000; I < 0xB800; ++I ) { _pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::SetPpuBank<6>, this, 0 ); }
			for ( uint32_t I = 0xB800; I < 0xC000; ++I ) { _pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::SetPpuBank<7>, this, 0 ); }
			for ( uint32_t I = 0xC000; I < 0xC800; ++I ) { _pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::SetPpuBank<8>, this, 0 ); }
			for ( uint32_t I = 0xC800; I < 0xD000; ++I ) { _pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::SetPpuBank<9>, this, 0 ); }
			for ( uint32_t I = 0xD000; I < 0xD800; ++I ) { _pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::SetPpuBank<10>, this, 0 ); }
			for ( uint32_t I = 0xD800; I < 0xE000; ++I ) { _pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::SetPpuBank<11>, this, 0 ); }
			
			// PRG Select 0..2 ($E000-$F7FF).
			for ( uint32_t I = 0xE000; I < 0xE800; ++I ) { _pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::SetPrg<0>, this, 0 ); }
			for ( uint32_t I = 0xE800; I < 0xF000; ++I ) { _pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::SetPrg<1>, this, 0 ); }
			for ( uint32_t I = 0xF000; I < 0xF800; ++I ) { _pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::SetPrg<2>, this, 0 ); }
			
			// Namco 163 Internal Address port ($F800-$FFFF).
			for ( uint32_t I = 0xF800; I < 0x10000; ++I ) { _pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::WriteF800, this, 0 ); }
		}
		
		/**
		 * Performs a single CPU-cycle update for the mapper's IRQ counter.
		 */
		virtual void									Tick() {
			if ( m_bIrqEnable ) {
				// The counter continuously stops incrementing and continuously asserts once it hits 0x7FFF.
				if ( m_ui16IrqCounter < 0x7FFF ) {
					m_ui16IrqCounter++;
				}
				if ( m_ui16IrqCounter == 0x7FFF ) {
					if ( m_pInterruptable ) {
						m_pInterruptable->Irq( LSN_IS_MAPPER );
					}
				}
			}
		}


	protected :
		// == Members.
		/** Expansion audio. */
		CAudioNamco163									m_anAudio;

		/** The 12 PPU bank selectors ($8000-$DFFF). */
		uint8_t											m_ui8ChrRegisters[12];

		/** Stores the exact value written to PRG bank 1 ($E800) to preserve bits 6 and 7. */
		uint8_t											m_ui8E800;
		
		/** The 8 KiB WRAM + 128 bytes internal RAM. */
		std::vector<uint8_t>							m_vWram;
		/** The 2 KiB CIRAM (Console Internal RAM) used for nametables. */
		std::vector<uint8_t>							m_vCiram;
		
		/** Internal memory auto-increment flag. */
		bool											m_bInternalRamAutoInc;
		/** Internal memory address pointer. */
		uint8_t											m_ui8InternalRamAddr;
		
		/** 15-bit IRQ counter. */
		uint16_t										m_ui16IrqCounter;
		/** IRQ enable status. */
		bool											m_bIrqEnable;


		// == Functions.
		/**
		 * Templated hook for handling PRG bank switching ($E000-$F7FF).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		template <unsigned _uBank>
		static void LSN_FASTCALL						SetPrg( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper019 * pmThis = reinterpret_cast<CMapper019 *>(_pvParm0);
			pmThis->SetPgmBank<_uBank, PgmBankSize()>( _ui8Val & 0x3F );
			if constexpr ( _uBank == 1 ) {
				pmThis->m_ui8E800 = _ui8Val;
			}
		}

		/**
		 * Templated hook for handling PPU bank switching ($8000-$DFFF).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		template <unsigned _uBank>
		static void LSN_FASTCALL						SetPpuBank( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper019 * pmThis = reinterpret_cast<CMapper019 *>(_pvParm0);
			pmThis->m_ui8ChrRegisters[_uBank] = _ui8Val;
		}

		/**
		 * Reads from internal RAM.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						InternalRamRead( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper019 * pmThis = reinterpret_cast<CMapper019 *>(_pvParm0);
			_ui8Ret = pmThis->m_vWram[(8*1024)+pmThis->m_ui8InternalRamAddr];
			if ( pmThis->m_bInternalRamAutoInc ) {
				pmThis->m_ui8InternalRamAddr = (pmThis->m_ui8InternalRamAddr + 1) & 0x7F;
			}
		}

		/**
		 * Writes to internal RAM.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						InternalRamWrite( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper019 * pmThis = reinterpret_cast<CMapper019 *>(_pvParm0);
			pmThis->m_vWram[(8*1024)+pmThis->m_ui8InternalRamAddr] = _ui8Val;
			if ( pmThis->m_bInternalRamAutoInc ) {
				pmThis->m_ui8InternalRamAddr = (pmThis->m_ui8InternalRamAddr + 1) & 0x7F;
			}
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
			CMapper019 * pmThis = reinterpret_cast<CMapper019 *>(_pvParm0);
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
			CMapper019 * pmThis = reinterpret_cast<CMapper019 *>(_pvParm0);
			pmThis->m_vWram[_ui16Parm1] = _ui8Val;
		}

		/**
		 * Handles IRQ Counter Low register writes ($5000).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write5000( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper019 * pmThis = reinterpret_cast<CMapper019 *>(_pvParm0);
			pmThis->m_ui16IrqCounter = (pmThis->m_ui16IrqCounter & 0x7F00) | _ui8Val;
			if ( pmThis->m_pInterruptable ) {
				pmThis->m_pInterruptable->ClearIrq( LSN_IS_MAPPER );
			}
		}

		/**
		 * Reads from the IRQ counter low.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Read5000( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper019 * pmThis = reinterpret_cast<CMapper019 *>(_pvParm0);
			_ui8Ret = uint8_t( pmThis->m_ui16IrqCounter );
		}

		/**
		 * Handles IRQ Counter High and Enable register writes ($5800).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write5800( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper019 * pmThis = reinterpret_cast<CMapper019 *>(_pvParm0);
			pmThis->m_ui16IrqCounter = (pmThis->m_ui16IrqCounter & 0x00FF) | ((uint16_t( _ui8Val & 0x7F )) << 8);
			pmThis->m_bIrqEnable = (_ui8Val & 0x80) != 0;
			if ( pmThis->m_pInterruptable ) {
				pmThis->m_pInterruptable->ClearIrq( LSN_IS_MAPPER );
			}
		}

		/**
		 * Reads from the IRQ counter high.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Read5800( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper019 * pmThis = reinterpret_cast<CMapper019 *>(_pvParm0);
			_ui8Ret = uint8_t( pmThis->m_ui16IrqCounter >> 8 );
		}

		/**
		 * Handles Namco 163 Internal Address port writes ($F800).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						WriteF800( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper019 * pmThis = reinterpret_cast<CMapper019 *>(_pvParm0);
			pmThis->m_ui8InternalRamAddr = _ui8Val & 0x7F;
			pmThis->m_bInternalRamAutoInc = (_ui8Val & 0x80) != 0;
		}

		/**
		 * Reads from the PPU bus ($0000-$2FFF), mapping to either CHR-ROM or CIRAM.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address (offset within the 1KB bank).
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		template <unsigned _uBank>
		static void LSN_FASTCALL						PpuRead( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t &_ui8Ret ) {
			CMapper019 * pmThis = reinterpret_cast<CMapper019 *>(_pvParm0);
			uint8_t ui8Reg = pmThis->m_ui8ChrRegisters[_uBank];
			bool bRamEnabled = true;

			if ( ui8Reg >= 0xE0 ) {
				if constexpr ( _uBank < 4 ) {
					bRamEnabled = (pmThis->m_ui8E800 & 0x40) == 0;
				}
				else if constexpr ( _uBank < 8 ) {
					bRamEnabled = (pmThis->m_ui8E800 & 0x80) == 0;
				}
			}
			else {
				bRamEnabled = false;
			}

			if ( bRamEnabled ) {
				// An even value (e.g. 0xE0, 0xE2) maps to CIRAM Page A (offset 0x2000).
				// An odd value (e.g. 0xE1, 0xE3) maps to CIRAM Page B (offset 0x2400).
				uint16_t ui16CiramBase = LSN_PPU_NAMETABLES + ((ui8Reg & 1) * LSN_PPU_NAMETABLES_SCREEN);
				_ui8Ret = _pui8Data[ui16CiramBase+_ui16Parm1];
			}
			else {
				size_t stNumBanks = pmThis->m_prRom->vChrRom.size() / ChrBankSize();
				if ( stNumBanks > 0 ) {
					size_t stBank = size_t( ui8Reg ) % stNumBanks;
					size_t stIndex = (stBank * ChrBankSize()) + _ui16Parm1;
					_ui8Ret = pmThis->m_prRom->vChrRom[stIndex];
				}
			}
		}

		/**
		 * Writes to the PPU bus ($0000-$2FFF), writing to CIRAM if enabled.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address (offset within the 1KB bank).
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		template <unsigned _uBank>
		static void LSN_FASTCALL						PpuWrite( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t _ui8Val ) {
			CMapper019 * pmThis = reinterpret_cast<CMapper019 *>(_pvParm0);
			uint8_t ui8Reg = pmThis->m_ui8ChrRegisters[_uBank];
			bool bRamEnabled = true;

			if ( ui8Reg >= 0xE0 ) {
				if constexpr ( _uBank < 4 ) {
					bRamEnabled = (pmThis->m_ui8E800 & 0x40) == 0;
				}
				else if constexpr ( _uBank < 8 ) {
					bRamEnabled = (pmThis->m_ui8E800 & 0x80) == 0;
				}
			}
			else {
				bRamEnabled = false;
			}

			if ( bRamEnabled ) {
				uint16_t ui16CiramBase = LSN_PPU_NAMETABLES + ((ui8Reg & 1) * LSN_PPU_NAMETABLES_SCREEN);
				_pui8Data[ui16CiramBase+_ui16Parm1] = _ui8Val;
			}
		}
	};

}	// namespace lsn

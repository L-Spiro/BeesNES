/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 019 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
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
			
			m_ui8Nt[0] = 0;
			m_ui8Nt[1] = 0;
			m_ui8Nt[2] = 0;
			m_ui8Nt[3] = 0;
			
			// 8 KiB WRAM + 128 bytes internal Namco 163 RAM appended for unified battery saving.
			m_vWram.resize( (8 * 1024) + 128 );
			std::fill( m_vWram.begin(), m_vWram.end(), uint8_t( 0 ) );

			LoadBatteryRam( m_vWram.data(), m_vWram.size() );

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
			}


			// ================
			// BANK-SELECT & RAM
			// ================
			// Internal Namco 163 Data Port ($4800-$4FFF).
			if ( m_prRom->riInfo.ui16SubMapper != 2 ) {
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

			// CHR Select 0..7 ($8000-$BFFF).
			for ( uint32_t I = 0x8000; I < 0x8800; ++I ) { _pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::SetChr<0>, this, 0 ); }
			for ( uint32_t I = 0x8800; I < 0x9000; ++I ) { _pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::SetChr<1>, this, 0 ); }
			for ( uint32_t I = 0x9000; I < 0x9800; ++I ) { _pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::SetChr<2>, this, 0 ); }
			for ( uint32_t I = 0x9800; I < 0xA000; ++I ) { _pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::SetChr<3>, this, 0 ); }
			for ( uint32_t I = 0xA000; I < 0xA800; ++I ) { _pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::SetChr<4>, this, 0 ); }
			for ( uint32_t I = 0xA800; I < 0xB000; ++I ) { _pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::SetChr<5>, this, 0 ); }
			for ( uint32_t I = 0xB000; I < 0xB800; ++I ) { _pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::SetChr<6>, this, 0 ); }
			for ( uint32_t I = 0xB800; I < 0xC000; ++I ) { _pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::SetChr<7>, this, 0 ); }
			
			// Nametable Select 0..3 ($C000-$DFFF).
			for ( uint32_t I = 0xC000; I < 0xC800; ++I ) { _pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::SetNt<0>, this, 0 ); }
			for ( uint32_t I = 0xC800; I < 0xD000; ++I ) { _pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::SetNt<1>, this, 0 ); }
			for ( uint32_t I = 0xD000; I < 0xD800; ++I ) { _pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::SetNt<2>, this, 0 ); }
			for ( uint32_t I = 0xD800; I < 0xE000; ++I ) { _pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::SetNt<3>, this, 0 ); }
			
			// PRG Select 0..2 ($E000-$F7FF).
			for ( uint32_t I = 0xE000; I < 0xE800; ++I ) { _pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::SetPrg<0>, this, 0 ); }
			for ( uint32_t I = 0xE800; I < 0xF000; ++I ) { _pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::SetPrg<1>, this, 0 ); }
			for ( uint32_t I = 0xF000; I < 0xF800; ++I ) { _pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::SetPrg<2>, this, 0 ); }
			
			// Namco 163 Internal Address port ($F800-$FFFF).
			for ( uint32_t I = 0xF800; I < 0x10000; ++I ) { _pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper019::WriteF800, this, 0 ); }


			// ================
			// MIRRORING
			// ================
			ApplyControllableMirrorMap( _pbPpuBus );
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
		/** Pointer to the interruptable system. */
		CInterruptable *								m_pInterruptable;

		/** The 4 NT banks stored purely for calculating mirroring permutations. */
		uint8_t											m_ui8Nt[4];
		
		/** The 8 KiB WRAM + 128 bytes internal RAM. */
		std::vector<uint8_t>							m_vWram;
		
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
		 * Evaluates the Nametable registers ($C000-$D800) and automatically adjusts standard mirroring.
		 **/
		void inline										UpdateMirroring() {
			uint8_t ui8Nt0 = (m_ui8Nt[0] < 0xE0) ? 0xFF : (m_ui8Nt[0] & 1);
			uint8_t ui8Nt1 = (m_ui8Nt[1] < 0xE0) ? 0xFF : (m_ui8Nt[1] & 1);
			uint8_t ui8Nt2 = (m_ui8Nt[2] < 0xE0) ? 0xFF : (m_ui8Nt[2] & 1);
			uint8_t ui8Nt3 = (m_ui8Nt[3] < 0xE0) ? 0xFF : (m_ui8Nt[3] & 1);

			if ( ui8Nt0 == 0 && ui8Nt1 == 1 && ui8Nt2 == 0 && ui8Nt3 == 1 ) { m_mmMirror = LSN_MM_VERTICAL; }
			else if ( ui8Nt0 == 0 && ui8Nt1 == 0 && ui8Nt2 == 1 && ui8Nt3 == 1 ) { m_mmMirror = LSN_MM_HORIZONTAL; }
			else if ( ui8Nt0 == 0 && ui8Nt1 == 0 && ui8Nt2 == 0 && ui8Nt3 == 0 ) { m_mmMirror = LSN_MM_1_SCREEN_A; }
			else if ( ui8Nt0 == 1 && ui8Nt1 == 1 && ui8Nt2 == 1 && ui8Nt3 == 1 ) { m_mmMirror = LSN_MM_1_SCREEN_B; }
		}

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
		}

		/**
		 * Templated hook for handling CHR bank switching ($8000-$B800).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		template <unsigned _uBank>
		static void LSN_FASTCALL						SetChr( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper019 * pmThis = reinterpret_cast<CMapper019 *>(_pvParm0);
			if ( _ui8Val < 0xE0 ) {
				pmThis->SetChrBank<_uBank, ChrBankSize()>( _ui8Val );
			}
			else {
				pmThis->m_ui8Nt[_uBank] = _ui8Val;
				pmThis->UpdateMirroring();
			}
		}

		/**
		 * Templated hook for handling Nametable/Mirroring bank switching ($C000-$D800).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		template <unsigned _uBank>
		static void LSN_FASTCALL						SetNt( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper019 * pmThis = reinterpret_cast<CMapper019 *>(_pvParm0);
			pmThis->m_ui8Nt[_uBank] = _ui8Val;
			pmThis->UpdateMirroring();
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
			_ui8Ret = pmThis->m_vWram[8192 + pmThis->m_ui8InternalRamAddr];
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
			pmThis->m_vWram[8192 + pmThis->m_ui8InternalRamAddr] = _ui8Val;
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
	};

}	// namespace lsn

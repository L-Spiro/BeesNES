/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 069 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNAudio5b.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper069
	 * \brief Mapper 069 implementation.
	 *
	 * Description: Mapper 069 implementation.
	 */
	class CMapper069 : public CMapperBase {
	public :
		CMapper069() {
		}
		virtual ~CMapper069() {
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
			m_vPrgRam.resize( 512 * 1024 );
			for ( uint32_t I = 0x6000; I < 0x8000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper069::ReadBank0, this, uint16_t( I - 0x6000 ) );
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper069::WriteBank0, this, uint16_t( I - 0x6000 ) );
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
			// Control.
			for ( uint32_t I = 0x8000; I < 0xA000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper069::SetControl, this, 0 );
			}
			// Parameter.
			for ( uint32_t I = 0xA000; I < 0xC000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper069::SetParameter, this, 0 );
			}
			if ( m_prRom->riInfo.ui16Chip == CDatabase::LSN_C_SUNSOFT_5B ) {
				m_Audio5b.ApplyMap( _pbCpuBus );
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
			if ( m_prRom->riInfo.ui16Chip == CDatabase::LSN_C_SUNSOFT_5B ) {
				m_Audio5b.Tick();
			}

			if ( m_ui8Control & 0b00000001 ) {
				if ( m_ui8Control & 0b10000000 ) {
					--m_ui16Counter;
				}
				if ( m_ui16Counter == 0xFFFF ) {
					m_pInterruptable->Irq( LSN_IS_MAPPER );
				}
			}
		}

		/**
		 * Called to inform the mapper of a reset.
		 **/
		virtual void									Reset() {
			m_Audio5b.ResetSoft();
		}

		/**
		 * Gets the extended audio sample.
		 * 
		 * \param _fApuSample The APU sample to mix with the extended-audio sample.
		 **/
		virtual float									GetExtAudio( float _fApuSample ) {
			if ( m_prRom->riInfo.ui16Chip == CDatabase::LSN_C_SUNSOFT_5B ) {
				return m_Audio5b.Sample() + _fApuSample;
			}
			return _fApuSample;
		}

		/**
		 * Applies any post affects to the audio sample.  The sample has been filtered and down-sampled to the output Hz by this point in the pipeline.
		 * 
		 * \param _fSample The sample to potentially modify.
		 * \param _fHz The output Hz.
		 * \return Returns the potentially modified sample.
		 **/
		virtual float									PostProcessAudioSample( float _fSample, float /*_fHz*/ ) {
			return m_Audio5b.PostProcessSample( _fSample );
		}


	protected :
		// == Members.
		/** PRG RAM. */
		std::vector<uint8_t>							m_vPrgRam;
		/** The 5B audio chip. */
		CAudio5b										m_Audio5b;
		/** Command Register. */
		uint8_t											m_ui8CmdReg = 0;
		/** PGM bank 0 parameters. */
		uint8_t											m_ui8PgmReg = 0;
		// IRQ.
		/** IRQ counter. */
		uint16_t										m_ui16Counter = 0;
		/** IRQ control. */
		uint8_t											m_ui8Control = 0;


		// == Functions.
		/**
		 * Sets the control register.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SetControl( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper069 * pmThis = reinterpret_cast<CMapper069 *>(_pvParm0);
			pmThis->m_ui8CmdReg = _ui8Val & 0b00001111;
		}

		/**
		 * Sets the control register.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SetParameter( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper069 * pmThis = reinterpret_cast<CMapper069 *>(_pvParm0);
			if ( pmThis->m_ui8CmdReg >= 0 && pmThis->m_ui8CmdReg < 8 ) {
				pmThis->SetChrBank<ChrBankSize()>( pmThis->m_ui8CmdReg, _ui8Val );
				return;
			}

			if ( pmThis->m_ui8CmdReg == 8 ) {
				pmThis->m_ui8PgmReg = _ui8Val;
				pmThis->SetPgmBank<0, PgmBankSize()>( _ui8Val & 0b00111111 );
				return;
			}
			if ( pmThis->m_ui8CmdReg > 8 && pmThis->m_ui8CmdReg <= 0xB ) {
				pmThis->SetPgmBank<PgmBankSize()>( pmThis->m_ui8CmdReg - 8, _ui8Val & 0b00111111 );
				return;
			}
			if ( pmThis->m_ui8CmdReg == 0xC ) {
				switch ( _ui8Val & 0b11 ) {
					case 0b00 : {
						pmThis->m_mmMirror = LSN_MM_VERTICAL;
						return;
					}
					case 0b01 : {
						pmThis->m_mmMirror = LSN_MM_HORIZONTAL;
						return;
					}
					case 0b10 : {
						pmThis->m_mmMirror = LSN_MM_1_SCREEN_A;
						return;
					}
					case 0b11 : {
						pmThis->m_mmMirror = LSN_MM_1_SCREEN_B;
						return;
					}
				}
				return;
			}
			if ( pmThis->m_ui8CmdReg == 0xD ) {
				pmThis->m_ui8Control = _ui8Val;
				pmThis->m_pInterruptable->ClearIrq( LSN_IS_MAPPER );
				return;
			}
			if ( pmThis->m_ui8CmdReg == 0xE ) {
				pmThis->m_ui16Counter = (pmThis->m_ui16Counter & 0xFF00) | _ui8Val;
				return;
			}
			if ( pmThis->m_ui8CmdReg == 0xF ) {
				pmThis->m_ui16Counter = (pmThis->m_ui16Counter & 0x00FF) | (uint16_t( _ui8Val ) << 8);
				return;
			}
		}

		/**
		 * Selects a bank.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectBankY000_YFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper069 * pmThis = reinterpret_cast<CMapper069 *>(_pvParm0);
			pmThis->SetPgmBank<0, PgmBankSize()>( _ui8Val & 0b1111 );
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
		static void LSN_FASTCALL						ReadBank0( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper069 * pmThis = reinterpret_cast<CMapper069 *>(_pvParm0);
			if ( pmThis->m_ui8PgmReg & 0b01000000 ) {
				// RAM.
				if ( pmThis->m_ui8PgmReg & 0b10000000 ) {
					// Enabled.
					_ui8Ret = pmThis->m_vPrgRam[pmThis->m_ui8PgmBank*PgmBankSize()+_ui16Parm1];
				}
			}
			else {
				_ui8Ret = pmThis->m_prRom->vPrgRom.data()[size_t(_ui16Parm1)+(size_t(pmThis->m_ui8PgmBanks[0])*PgmBankSize())];
			}
		}

		/**
		 * Writes to bank 0 $6000-$7FFF.
		 * 
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 **/
		static void LSN_FASTCALL						WriteBank0( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper069 * pmThis = reinterpret_cast<CMapper069 *>(_pvParm0);
			if ( (pmThis->m_ui8PgmReg & 0b11000000) == 0b11000000 ) {
				pmThis->m_vPrgRam[size_t(pmThis->m_ui8PgmBank)*size_t(PgmBankSize())+size_t(_ui16Parm1)] = _ui8Val;
			}
		}
	};

}	// namespace lsn

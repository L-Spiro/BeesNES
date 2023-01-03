/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 001 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper001
	 * \brief Mapper 001 implementation.
	 *
	 * Description: Mapper 001 implementation.
	 */
	class CMapper001 : public CMapperBase {
	public :
		CMapper001() {
		}
		virtual ~CMapper001() {
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
		static constexpr uint16_t						ChrBankSize() { return 4 * 1024; }

		/**
		 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
		 *
		 * \param _rRom The ROM data.
		 * \param _pcbCpuBase A pointer to the CPU.
		 */
		virtual void									InitWithRom( LSN_ROM &_rRom, CCpuBase * _pcbCpuBase ) {
			CMapperBase::InitWithRom( _rRom, _pcbCpuBase );
			SanitizeRegs<PgmBankSize() * 2, ChrBankSize() * 2>();
			SetPgmBank2x<2, PgmBankSize()>( 3 );
			SetChrBank2x<0, ChrBankSize()>( 0 );
			m_ui8Control = 0x1C;
			m_ui8Load = 0;
			m_ui8LoadCnt = 0;
			SetPgmBank<2, PgmBankSize()>( 0 );
			SetPgmBank<3, PgmBankSize()>( -1 );
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
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper001::Read_PGM_8000_FFFF, this, uint16_t( (I - 0x8000) % m_prRom->vPrgRom.size() ) );
			}
			// PPU.
			if ( m_prRom->vChrRom.size() ) {
				for ( uint32_t I = 0x0000; I < 0x2000; ++I ) {
					_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper001::Read_CHR_0000_1FFF, this, uint16_t( I - 0x0000 ) );
				}
			}
			// RAM.
			for ( uint32_t I = 0x6000; I < 0x8000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper001::Mapper001PgmRamRead, this, uint16_t( I - 0x6000 ) );
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper001::Mapper001PgmRamWrite, this, uint16_t( I - 0x6000 ) );
			}


			// ================
			// BANK-SELECT
			// ================
			// PGM bank-select.
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper001::Write_PGM_8000_FFFF, this, uint16_t( I ) );
			}


			// ================
			// MIRRORING
			// ================
			ApplyControllableMirrorMap( _pbPpuBus );			
		}


	protected :
		// == Members.
		/** The RAM. */
		uint8_t											m_ui8PgmRam[8*1024];
		/** The control register. */
		uint8_t											m_ui8Control;
		/** The load register. */
		uint8_t											m_ui8Load;
		/** The load count. */
		uint8_t											m_ui8LoadCnt;


		// == Functions.
		/**
		 * Reads from the PGM RAM.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Mapper001PgmRamRead( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper001 * pmThis = reinterpret_cast<CMapper001 *>(_pvParm0);
			_ui8Ret = pmThis->m_ui8PgmRam[_ui16Parm1];
		}

		/**
		 * Writes to the PGM RAM.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						Mapper001PgmRamWrite( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper001 * pmThis = reinterpret_cast<CMapper001 *>(_pvParm0);
			pmThis->m_ui8PgmRam[_ui16Parm1] = _ui8Val;
		}

		/**
		 * Handles reads fromm 0x8000-0xFFFF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Read_PGM_8000_FFFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper001 * pmThis = reinterpret_cast<CMapper001 *>(_pvParm0);
			if ( pmThis->m_ui8Control & 0b01000 ) {
				// 8-kilobyte chunks.
				if ( _ui16Parm1 >= 0xC000 - 0x8000 ) {
					// Hi chunk.
					_ui8Ret = pmThis->m_prRom->vPrgRom.data()[pmThis->m_ui8PgmBanks[3]*PgmBankSize()+(_ui16Parm1&0x3FFF)];
				}
				else {
					// Lo chunk.
					_ui8Ret = pmThis->m_prRom->vPrgRom.data()[pmThis->m_ui8PgmBanks[2]*PgmBankSize()+(_ui16Parm1&0x3FFF)];
				}
			}
			else {
				// 32 kikobytes.
				_ui8Ret = pmThis->m_prRom->vPrgRom.data()[pmThis->m_ui8PgmBank*(PgmBankSize()*2)+(_ui16Parm1&0x7FFF)];
			}
		}

		/**
		 * Handles writes to 0x8000-0xFFFF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						Write_PGM_8000_FFFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper001 * pmThis = reinterpret_cast<CMapper001 *>(_pvParm0);
			/*
			 *	7  bit  0
			 *	---- ----
			 *	Rxxx xxxD
			 *	|       |
			 *	|       +- Data bit to be shifted into shift register, LSB first
			 *	+--------- A write with bit set will reset shift register
			 *				and write Control with (Control OR $0C), 
			 *				locking PRG ROM at $C000-$FFFF to the last bank.
			 */
			if ( _ui8Val & 0x80 ) {
				pmThis->m_ui8Load = 0x00;
				pmThis->m_ui8LoadCnt = 0;
				pmThis->m_ui8Control = pmThis->m_ui8Control | 0x0C;
			}
			else {
				++pmThis->m_ui8LoadCnt;
				pmThis->m_ui8Load = (pmThis->m_ui8Load >> 1) | ((_ui8Val & 1) << 4);
				if ( pmThis->m_ui8LoadCnt == 5 ) {

					// Determine the target of the write.
					if ( _ui16Parm1 >= 0x8000 && _ui16Parm1 < 0xA000 ) {
						/*
						 *	Control (internal, $8000-$9FFF)
						 *	4bit0
						 *	-----
						 *	CPPMM
						 *	|||||
						 *	|||++- Mirroring (0: one-screen, lower bank; 1: one-screen, upper bank;
						 *	|||               2: vertical; 3: horizontal)
						 *	|++--- PRG ROM bank mode (0, 1: switch 32 KB at $8000, ignoring low bit of bank number;
						 *	|                         2: fix first bank at $8000 and switch 16 KB bank at $C000;
						 *	|                         3: fix last bank at $C000 and switch 16 KB bank at $8000)
						 *	+----- CHR ROM bank mode (0: switch 8 KB at a time; 1: switch two separate 4 KB banks)
						 */
						pmThis->m_ui8Control = pmThis->m_ui8Load & 0b11111;
						switch ( pmThis->m_ui8Control & 0b11 ) {
							case 0 : {
								pmThis->m_mmMirror = LSN_MM_1_SCREEN_A;
								//::OutputDebugStringA( "**** LSN_MM_1_SCREEN_A\r\n" );
								break;
							}
							case 1 : {
								pmThis->m_mmMirror = LSN_MM_1_SCREEN_B;
								//::OutputDebugStringA( "**** LSN_MM_1_SCREEN_B\r\n" );
								break;
							}
							case 2 : {
								pmThis->m_mmMirror = LSN_MM_VERTICAL;
								//::OutputDebugStringA( "**** LSN_MM_VERTICAL\r\n" );
								break;
							}
							case 3 : {
								pmThis->m_mmMirror = LSN_MM_HORIZONTAL;
								//::OutputDebugStringA( "**** LSN_MM_HORIZONTAL\r\n" );
								break;
							}
						}
					}
					else if ( _ui16Parm1 >= 0xA000 && _ui16Parm1 < 0xC000 ) {
						/*
						 *	CHR bank 0 (internal, $A000-$BFFF)
						 *	4bit0
						 *	-----
						 *	CCCCC
						 *	|||||
						 *	+++++- Select 4 KB or 8 KB CHR bank at PPU $0000 (low bit ignored in 8 KB mode)
						 */
						// CHR ROM bank mode (0: switch 8 KB at a time; 1: switch two separate 4 KB banks)
						if ( (pmThis->m_ui8Control & 0b10000) == 0 ) {
							pmThis->SetChrBank<0, ChrBankSize() * 2>( (pmThis->m_ui8Load & 0b11110) >> 1 );
						}
						else {
							pmThis->SetChrBank<1, ChrBankSize()>( pmThis->m_ui8Load & 0b11111 );
						}
					}
					else if ( _ui16Parm1 >= 0xC000 && _ui16Parm1 < 0xE000 ) {
						/*
						 *	CHR bank 1 (internal, $C000-$DFFF)
						 *	4bit0
						 *	-----
						 *	CCCCC
						 *	|||||
						 *	+++++- Select 4 KB CHR bank at PPU $1000 (ignored in 8 KB mode)
						 */
						if ( pmThis->m_ui8Control & 0b10000 ) {
							pmThis->SetChrBank<2, ChrBankSize()>( pmThis->m_ui8Load & 0b11111 );
						}
					}
					else if ( _ui16Parm1 >= 0xE000 && _ui16Parm1 < 0x10000 ) {
						/*
						 *	PRG bank (internal, $E000-$FFFF)
						 *	4bit0
						 *	-----
						 *	RPPPP
						 *	|||||
						 *	|++++- Select 16 KB PRG ROM bank (low bit ignored in 32 KB mode)
						 *	+----- MMC1B and later: PRG RAM chip enable (0: enabled; 1: disabled; ignored on MMC1A)
						 *		   MMC1A: Bit 3 bypasses fixed bank logic in 16K mode (0: affected; 1: bypassed)
						 */
						uint8_t ui8PgmMode = (pmThis->m_ui8Control >> 2) & 0b11;
						switch ( ui8PgmMode ) {
							// 0, 1: switch 32 KB at $8000, ignoring low bit of bank number;
							case 1 : {}
							case 0 : {
								pmThis->SetPgmBank2x<0, 32 * 1024>( (pmThis->m_ui8Load & 0b1110) >> 1 );
								break;
							}
							// 2: fix first bank at $8000 and switch 16 KB bank at $C000;
							case 2 : {
								pmThis->SetPgmBank<2, PgmBankSize()>( 0 );
								pmThis->SetPgmBank<3, PgmBankSize()>( pmThis->m_ui8Load & 0b1111 );
								break;
							}
							// 3: fix last bank at $C000 and switch 16 KB bank at $8000)
							case 3 : {
								pmThis->SetPgmBank<2, PgmBankSize()>( pmThis->m_ui8Load & 0b1111 );
								pmThis->SetPgmBank<3, PgmBankSize()>( -1 );
								break;
							}
						}
					}

					pmThis->m_ui8Load = pmThis->m_ui8LoadCnt = 0;
				}
			}
		}

		/**
		 * Handles CHR reads from 0x0000-0x1FFF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Read_CHR_0000_1FFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper001 * pmThis = reinterpret_cast<CMapper001 *>(_pvParm0);
			if ( !pmThis->m_prRom->vChrRom.size() ) {
				_ui8Ret = pmThis->m_ui8DefaultChrRam[_ui16Parm1];
			}
			else {
				if ( pmThis->m_ui8Control & 0b10000 ) {
					// 4-kilobyte chunks.
					if ( _ui16Parm1 >= ChrBankSize() ) {
						// Hi chunk.
						_ui8Ret = pmThis->m_prRom->vChrRom.data()[pmThis->m_ui8ChrBanks[2]*ChrBankSize()+(_ui16Parm1&0x0FFF)];
					}
					else {
						// Lo chunk.
						_ui8Ret = pmThis->m_prRom->vChrRom.data()[pmThis->m_ui8ChrBanks[1]*ChrBankSize()+(_ui16Parm1&0x0FFF)];
					}
				}
				else {
					// 8 kilobytes.
					_ui8Ret = pmThis->m_prRom->vChrRom.data()[pmThis->m_ui8ChrBank*(ChrBankSize()*2)+(_ui16Parm1&0x1FFF)];
				}
			}
		}
	};

}	// namespace lsn

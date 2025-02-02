/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 001 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Database/LSNDatabase.h"
#include "LSNMapperBase.h"

#define LSN_CHR_BNK_SMALL						2
#define LSN_PGM_BNK_SMALL						4

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
		virtual void									InitWithRom( LSN_ROM &_rRom, CCpuBase * _pcbCpuBase, CBussable * _pbPpuBus ) {
			CMapperBase::InitWithRom( _rRom, _pcbCpuBase, _pbPpuBus );
			SanitizeRegs<PgmBankSize() * 2, ChrBankSize() * 2>();
			SetChrBank2x<0, ChrBankSize()*2>( 0 );
			SetChrBank2x<LSN_CHR_BNK_SMALL, ChrBankSize()>( 0 );
			m_ui8Control = 0x1C;
			m_ui8Load = 0;
			m_ui8LoadCnt = 0;
			SetPgmBank<0, PgmBankSize()*2>( 0 );
			SetPgmBank<1, PgmBankSize()*2>( -1 );
			SetPgmBank<LSN_PGM_BNK_SMALL+0, PgmBankSize()>( 0 );
			SetPgmBank<LSN_PGM_BNK_SMALL+1, PgmBankSize()>( -1 );

			m_bRamEnabled = _rRom.riInfo.ui16Chip != CDatabase::LSN_C_MMC1C;
			m_ui8PgmRamChr0 = m_ui8PgmRamChr1 = m_ui8PgmRamPgm0 = 0;
			m_ui64LastWriteCycle = ~uint64_t( 0 );


			m_vChrRam.resize( std::max<size_t>( m_prRom->vChrRom.size(), 8 * 1024 ) );
			std::memcpy( m_vChrRam.data(), m_prRom->vChrRom.data(), m_prRom->vChrRom.size() );
			m_pui8PgmRam = m_ui8PgmRam;
		}

		/**
		 * Applies mapping to the CPU and PPU busses.
		 *
		 * \param _pbCpuBus A pointer to the CPU bus.
		 * \param _pbPpuBus A pointer to the PPU bus.
		 */
		virtual void									ApplyMap( CCpuBus * _pbCpuBus, CPpuBus * _pbPpuBus ) {
			CMapperBase::ApplyMap( _pbCpuBus, _pbPpuBus );

			bool ibIsSxROM = (m_prRom->riInfo.ui16PcbClass == CDatabase::LSN_PC_SEROM ||
				m_prRom->riInfo.ui16PcbClass == CDatabase::LSN_PC_SHROM ||
				m_prRom->riInfo.ui16PcbClass == CDatabase::LSN_PC_SH1ROM);

			// ================
			// SWAPPABLE BANKS
			// ================
			// CPU.
			for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
				_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper001::Read_PGM_8000_FFFF, this, uint16_t( (I - 0x8000) % m_prRom->vPrgRom.size() ) );
			}
			// PPU.
			for ( uint32_t I = 0x0000; I < 0x2000; ++I ) {
				_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper001::Read_CHR_0000_1FFF, this, uint16_t( I - 0x0000 ) );
				_pbPpuBus->SetWriteFunc( uint16_t( I ), &CMapper001::Write_CHR_0000_1FFF, this, uint16_t( I - 0x0000 ) );
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
			uint32_t ui32Reg = ibIsSxROM ? 0xE000 : 0x10000;
			for ( uint32_t I = 0x8000; I < ui32Reg; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper001::Write_PGM_8000_FFFF, this, uint16_t( I ) );
			}


			// ================
			// MIRRORING
			// ================
			ApplyControllableMirrorMap( _pbPpuBus );			
		}


	protected :
		// == Members.
		/** PGM RAM pointer. */
		uint8_t *										m_pui8PgmRam = nullptr;
		/** The last write cycle. */
		uint64_t										m_ui64LastWriteCycle = ~uint64_t( 0 );
		/** The PGM RAM size. */
		size_t											m_sPgmRamSize = 8 * 1024;
		/** The PGM RAM. */
		uint8_t											m_ui8PgmRam[32*1024];
		/** The CHR RAM size. */
		size_t											m_sChrRamSize = 8 * 1024;
		/** the CHR RAM. */
		std::vector<uint8_t>							m_vChrRam;
		/** The control register. */
		uint8_t											m_ui8Control;
		/** The load register. */
		uint8_t											m_ui8Load;
		/** The load count. */
		uint8_t											m_ui8LoadCnt;
		/** CHR-0 PRG-RAM register. */
		uint8_t											m_ui8PgmRamChr0;
		/** CHR-1 PRG-RAM register. */
		uint8_t											m_ui8PgmRamChr1;
		/** PGM-0 PRG-RAM register. */
		uint8_t											m_ui8PgmRamPgm0;
		/** PGM RAM enabled. */
		bool											m_bRamEnabled;


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
			if ( pmThis->m_bRamEnabled ) {
				_ui8Ret = pmThis->m_pui8PgmRam[_ui16Parm1];
			}
		}

		/**
		 * Writes to the PGM RAM.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Mapper001PgmRamWrite( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper001 * pmThis = reinterpret_cast<CMapper001 *>(_pvParm0);
			if ( pmThis->m_bRamEnabled ) {
				pmThis->m_pui8PgmRam[_ui16Parm1] = _ui8Val;
			}
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
				//if ( pmThis->m_pbPpuBus->LastBusAddress() & (1 << 12) ) {
					// Hi chunk.
					_ui8Ret = pmThis->m_prRom->vPrgRom.data()[pmThis->m_ui8PgmBanks[LSN_PGM_BNK_SMALL+1]*PgmBankSize()+(_ui16Parm1&0x3FFF)];
				}
				else {
					// Lo chunk.
					_ui8Ret = pmThis->m_prRom->vPrgRom.data()[pmThis->m_ui8PgmBanks[LSN_PGM_BNK_SMALL+0]*PgmBankSize()+(_ui16Parm1/*&0x3FFF*/)];
				}
			}
			else {
				// 32 kikobytes.
				_ui8Ret = pmThis->m_prRom->vPrgRom.data()[pmThis->m_ui8PgmBank*(PgmBankSize()*2)+(_ui16Parm1/*&0x7FFF*/)];
			}
		}

		/**
		 * Handles writes to 0x8000-0xFFFF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write_PGM_8000_FFFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper001 * pmThis = reinterpret_cast<CMapper001 *>(_pvParm0);
			uint64_t ui64Cycle = pmThis->m_pcbCpu->GetCycleCount();
			if ( pmThis->m_ui64LastWriteCycle == ui64Cycle + 1 ) {
				::OutputDebugStringA( "   Mapper 1 QUIRK: IT HAPPENED.\r\n" );				
				if ( !(_ui8Val & 0x80) ) { pmThis->m_ui64LastWriteCycle = ui64Cycle; return; }
			}
			pmThis->m_ui64LastWriteCycle = ui64Cycle;
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
						if ( pmThis->m_prRom->riInfo.ui16PcbClass == CDatabase::LSN_PC_SNROM ) {
							pmThis->m_bRamEnabled = (pmThis->m_ui8PgmRamPgm0 == 0) &&
								(pmThis->m_ui8PgmRamChr1 == 0 || ((pmThis->m_ui8Control & 0b10000) != 0)) &&
								(pmThis->m_ui8PgmRamChr0 == 0);
						}
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
						// SxROM:
						switch ( pmThis->m_prRom->riInfo.ui16PcbClass ) {
							case CDatabase::LSN_PC_SNROM : {
								/*
								 *	4bit0
								 *	-----
								 *	ExxxC
								 *	|   |
								 *	|   +- Select 4 KB CHR-RAM bank at PPU $0000 (ignored in 8 KB mode)
								 *	+----- PRG-RAM disable (0: enable, 1: open bus) 
								 */
								pmThis->SetChrBank<LSN_CHR_BNK_SMALL+0, ChrBankSize()>( pmThis->m_ui8Load & 0b00001 );
								pmThis->m_ui8PgmRamChr0 = (pmThis->m_ui8Load & 0b10000);
								pmThis->m_bRamEnabled = (pmThis->m_ui8PgmRamPgm0 == 0) &&
									(pmThis->m_ui8PgmRamChr1 == 0 || ((pmThis->m_ui8Control & 0b10000) != 0)) &&
									(pmThis->m_ui8PgmRamChr0 == 0);
								break;
							}
							case CDatabase::LSN_PC_SXROM : {
								/*
								 *	4bit0
								 *	-----
								 *	PSSxC
								 *	||| |
								 *	||| +- Select 4 KB CHR-RAM bank at PPU $0000 (ignored in 8 KB mode)
								 *	|++--- Select 8 KB PRG-RAM bank
								 *	+----- Select 256 KB PRG-ROM bank
								 */
								pmThis->SetChrBank<LSN_CHR_BNK_SMALL+0, ChrBankSize()>( pmThis->m_ui8Load & 0b00001 );
								break;
							}
							default : {
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
									pmThis->SetChrBank2x<0, ChrBankSize()*2>( (pmThis->m_ui8Load & 0b11110) >> 1 );

									pmThis->SetChrBank<LSN_CHR_BNK_SMALL+0, ChrBankSize()>( pmThis->m_ui8Load & 0b11111 );
									pmThis->SetChrBank<LSN_CHR_BNK_SMALL+1, ChrBankSize()>( (pmThis->m_ui8Load & 0b11111) + 1 );
								}
								else {
									pmThis->SetChrBank<0, ChrBankSize()*2>( (pmThis->m_ui8Load & 0b11110) >> 1 );
							
									pmThis->SetChrBank<LSN_CHR_BNK_SMALL+0, ChrBankSize()>( pmThis->m_ui8Load & 0b11111 );
								}
							}
						}
						
					}
					else if ( _ui16Parm1 >= 0xC000 && _ui16Parm1 < 0xE000 ) {
						// SxROM:
						switch ( pmThis->m_prRom->riInfo.ui16PcbClass ) {
							case CDatabase::LSN_PC_SNROM : {
								/*
								 *	4bit0
								 *	-----
								 *	ExxxC
								 *	|   |
								 *	|   +- Select 4 KB CHR-RAM bank at PPU $0000 (ignored in 8 KB mode)
								 *	+----- PRG-RAM disable (0: enable, 1: open bus) 
								 */
								pmThis->SetChrBank<LSN_CHR_BNK_SMALL+0, ChrBankSize()>( pmThis->m_ui8Load & 0b00001 );
								pmThis->m_ui8PgmRamChr1 = (pmThis->m_ui8Load & 0b10000);
								pmThis->m_bRamEnabled = (pmThis->m_ui8PgmRamPgm0 == 0) &&
									(pmThis->m_ui8PgmRamChr1 == 0 || ((pmThis->m_ui8Control & 0b10000) != 0)) &&
									(pmThis->m_ui8PgmRamChr0 == 0);
								break;
							}
							default : {
								/*
								 *	CHR bank 1 (internal, $C000-$DFFF)
								 *	4bit0
								 *	-----
								 *	CCCCC
								 *	|||||
								 *	+++++- Select 4 KB CHR bank at PPU $1000 (ignored in 8 KB mode)
								 */
								pmThis->SetChrBank<LSN_CHR_BNK_SMALL+1, ChrBankSize()>( pmThis->m_ui8Load & 0b11111 );
							}
						}
						
					}
					else if ( _ui16Parm1 >= 0xE000 /*&& _ui16Parm1 < 0x10000*/ ) {
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
						pmThis->m_ui8PgmRamPgm0 = (pmThis->m_ui8Load & 0b10000);
						if ( pmThis->m_prRom->riInfo.ui16Chip >= CDatabase::LSN_C_MMC1B1 && pmThis->m_prRom->riInfo.ui16Chip < CDatabase::LSN_C_MMC1_END ) {
							pmThis->m_bRamEnabled = pmThis->m_ui8PgmRamPgm0 == 0;
						}
						else if ( pmThis->m_prRom->riInfo.ui16PcbClass == CDatabase::LSN_PC_SNROM ) {
							pmThis->m_bRamEnabled = (pmThis->m_ui8PgmRamPgm0 == 0) &&
								(pmThis->m_ui8PgmRamChr1 == 0 || ((pmThis->m_ui8Control & 0b10000) != 0)) &&
								(pmThis->m_ui8PgmRamChr0 == 0);
						}

						uint8_t ui8PgmMode = (pmThis->m_ui8Control >> 2) & 0b11;
						switch ( ui8PgmMode ) {
							// 0, 1: switch 32 KB at $8000, ignoring low bit of bank number;
							case 1 : {}
							case 0 : {
								pmThis->SetPgmBank<0, PgmBankSize()*2>( (pmThis->m_ui8Load & 0b1110) >> 1 );

								pmThis->SetPgmBank<LSN_PGM_BNK_SMALL+0, PgmBankSize()>( pmThis->m_ui8Load & 0b1111 );
								pmThis->SetPgmBank<LSN_PGM_BNK_SMALL+1, PgmBankSize()>( (pmThis->m_ui8Load & 0b1111) + 1 );
								break;
							}
							// 2: fix first bank at $8000 and switch 16 KB bank at $C000;
							case 2 : {
								pmThis->SetPgmBank<0, PgmBankSize()*2>( 0 );
								pmThis->SetPgmBank<1, PgmBankSize()*2>( (pmThis->m_ui8Load & 0b1110) >> 1 );

								pmThis->SetPgmBank<LSN_PGM_BNK_SMALL+0, PgmBankSize()>( 0 );
								pmThis->SetPgmBank<LSN_PGM_BNK_SMALL+1, PgmBankSize()>( pmThis->m_ui8Load & 0b1111 );
								break;
							}
							// 3: fix last bank at $C000 and switch 16 KB bank at $8000)
							case 3 : {
								pmThis->SetPgmBank<0, PgmBankSize()*2>( (pmThis->m_ui8Load & 0b1110) >> 1 );
								pmThis->SetPgmBank<1, PgmBankSize()*2>( -1 );

								pmThis->SetPgmBank<LSN_PGM_BNK_SMALL+0, PgmBankSize()>( pmThis->m_ui8Load & 0b1111 );
								pmThis->SetPgmBank<LSN_PGM_BNK_SMALL+1, PgmBankSize()>( -1 );
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
			if ( pmThis->m_ui8Control & 0b10000 ) {
				// 4-kilobyte chunks.
				if ( _ui16Parm1 >= ChrBankSize() ) {
				//if ( pmThis->m_pbPpuBus->LastBusAddress() & (1 << 12) ) {
					// Hi chunk.
					_ui8Ret = pmThis->m_vChrRam.data()[pmThis->m_ui8ChrBanks[LSN_CHR_BNK_SMALL+1]*ChrBankSize()+(_ui16Parm1&0x0FFF)];
				}
				else {
					// Lo chunk.
					_ui8Ret = pmThis->m_vChrRam.data()[pmThis->m_ui8ChrBanks[LSN_CHR_BNK_SMALL+0]*ChrBankSize()+(_ui16Parm1/*&0x0FFF*/)];
				}
			}
			else {
				// 8 kilobytes.
				_ui8Ret = pmThis->m_vChrRam.data()[pmThis->m_ui8ChrBank*(ChrBankSize()*2)+(_ui16Parm1/*&0x1FFF*/)];
			}
		}

		/**
		 * Handles CHR writes to 0x0000-0x1FFF.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Write_CHR_0000_1FFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper001 * pmThis = reinterpret_cast<CMapper001 *>(_pvParm0);
			if ( pmThis->m_ui8Control & 0b10000 ) {
				// 4-kilobyte chunks.
				if ( _ui16Parm1 >= ChrBankSize() ) {
					// Hi chunk.
					pmThis->m_vChrRam.data()[pmThis->m_ui8ChrBanks[LSN_CHR_BNK_SMALL+1]*ChrBankSize()+(_ui16Parm1&0x0FFF)] = _ui8Val;
				}
				else {
					// Lo chunk.
					pmThis->m_vChrRam.data()[pmThis->m_ui8ChrBanks[LSN_CHR_BNK_SMALL+0]*ChrBankSize()+(_ui16Parm1/*&0x0FFF*/)] = _ui8Val;
				}
			}
			else {
				// 8 kilobytes.
				pmThis->m_vChrRam.data()[pmThis->m_ui8ChrBank*(ChrBankSize()*2)+(_ui16Parm1/*&0x1FFF*/)] = _ui8Val;
			}
		}
	};

}	// namespace lsn

#undef LSN_PGM_BNK_SMALL
#undef LSN_CHR_BNK_SMALL

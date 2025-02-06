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
			SetPgmBank<0, PgmBankSize()>( 0 );
			SetPgmBank<1, PgmBankSize()>( -1 );

			m_prPgmBank.ui8PgmBank[0] = m_ui8PgmBanks[0];
			m_prPgmBank.ui8PgmBank[1] = m_ui8PgmBanks[1];
			m_crChrBanks[0].ui8ChrBank = m_ui8ChrBanks[0];
			m_crChrBanks[1].ui8ChrBank = m_ui8ChrBanks[1];

			m_ui8Control = 0x1C;
			m_ui8Load = 0;
			m_ui8LoadCnt = 0;

			m_ui64LastWriteCycle = ~uint64_t( 0 );

			// RAM size.
			if ( m_prRom->riInfo.ui16PcbClass == CDatabase::LSN_PC_SOROM || m_prRom->riInfo.ui16PcbClass == CDatabase::LSN_PC_SZROM ) {
				m_sPgmRamSize = 16 * 1024;
			}
			else if ( m_prRom->riInfo.ui16PcbClass == CDatabase::LSN_PC_SXROM ) {
				m_sPgmRamSize = 32 * 1024;
			}


			// CHR RAM.
			if ( m_prRom->riInfo.ui16PcbClass == CDatabase::LSN_PC_SZROM ) {
				m_sChrRamSize = 64 * 1024;
			}
			m_vChrRam.resize( std::max<size_t>( m_prRom->vChrRom.size(), 8 * 1024 ) );
			std::memcpy( m_vChrRam.data(), m_prRom->vChrRom.data(), m_prRom->vChrRom.size() );


			for ( size_t I = 0; I < LSN_ELEMENTS( m_crChrBanks ); ++I ) {
				m_crChrBanks[I].ui8PgmRamBank = m_crChrBanks[I].ui8PgmRamBank % (m_sPgmRamSize / (8 * 1024));
			}

			// For types that have outer banks.
			if ( m_prRom->riInfo.ui16PcbClass == CDatabase::LSN_PC_SOROM ||
				m_prRom->riInfo.ui16PcbClass == CDatabase::LSN_PC_SUROM ||
				m_prRom->riInfo.ui16PcbClass == CDatabase::LSN_PC_SXROM ||
				m_prRom->riInfo.ui16PcbClass == CDatabase::LSN_PC_SZROM ) {
				// Outer ROM banks.
				for ( size_t I = 0; I < LSN_ELEMENTS( m_puiPgmRomOuterBanks ); ++I ) {
					m_puiPgmRomOuterBanks[I] = &m_prRom->vPrgRom.data()[(I*PgmBankSize())%(m_prRom->vPrgRom.size())];
				}
				// Outer RAM banks.
				for ( size_t I = 0; I < LSN_ELEMENTS( m_puiPgmRamOuterBanks ); ++I ) {
					m_puiPgmRamOuterBanks[I] = &m_ui8PgmRam[(I*(8*1024))%m_sPgmRamSize];
				}
			}
			else {
				// No outer ROM banks.
				for ( size_t I = 0; I < LSN_ELEMENTS( m_puiPgmRomOuterBanks ); ++I ) {
					m_puiPgmRomOuterBanks[I] = m_prRom->vPrgRom.data();
				}
				// No outer RAM banks.
				for ( size_t I = 0; I < LSN_ELEMENTS( m_puiPgmRamOuterBanks ); ++I ) {
					m_puiPgmRamOuterBanks[I] = m_ui8PgmRam;
				}
			}

			m_bRamEnabled = GetRamEnabled();
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
				if ( m_prRom->riInfo.ui16PcbClass == CDatabase::LSN_PC_SNROM ) {
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper001::Mapper001PgmRamRead_OB, this, uint16_t( I - 0x6000 ) );
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper001::Mapper001PgmRamWrite_OB, this, uint16_t( I - 0x6000 ) );
				}
				else {
					_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper001::Mapper001PgmRamRead, this, uint16_t( I - 0x6000 ) );
					_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper001::Mapper001PgmRamWrite, this, uint16_t( I - 0x6000 ) );
				}
			}


			// ================
			// BANK-SELECT
			// ================
			// Control, bank swapping.
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
		// == Types.
		/** CHR registers. */
		struct LSN_CHR_REG {
			uint8_t										ui8ChrBank;
			uint8_t										ui8PgmRamBank;
			bool										bPgmRamEnable;
		};

		/** PGM registers. */
		struct LSN_PGM_REG {
			uint8_t										ui8PgmBank[2];
			bool										bPgmRamEnable;
		};


		// == Members.
		/** The last write cycle. */
		uint64_t										m_ui64LastWriteCycle = ~uint64_t( 0 );

		/** The PGM RAM size. */
		size_t											m_sPgmRamSize = 8 * 1024;
		/** The PGM RAM. */
		uint8_t											m_ui8PgmRam[32*1024];
		/** PGM-ROM outer banks. */
		uint8_t *										m_puiPgmRomOuterBanks[16];
		/** PGM-RAM outer banks. */
		uint8_t *										m_puiPgmRamOuterBanks[16];
		/** The CHR RAM size. */
		size_t											m_sChrRamSize = 8 * 1024;
		/** The CHR RAM. */
		std::vector<uint8_t>							m_vChrRam;

		/** The CHR banks. */
		LSN_CHR_REG										m_crChrBanks[2];
		/** The PGM bank. */
		LSN_PGM_REG										m_prPgmBank;
		/** The control register. */
		uint8_t											m_ui8Control;
		/** The load register. */
		uint8_t											m_ui8Load;
		/** The load count. */
		uint8_t											m_ui8LoadCnt;
		/** PGM RAM enabled. */
		bool											m_bRamEnabled;
		

		// == Functions.
		// ================
		// UTILITIES
		// ================
		/**
		 * Returns true if we are in 8-kilobyte mode (CHR).
		 * 
		 * \return Returns true if we are in 8-kilobyte mode.
		 **/
		inline bool										In8kMode_CHR() const {
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
			return (m_ui8Control & 0b10000) == 0;
		}

		/**
		 * Returns true if we are in 32-kilobyte mode (PGM).
		 * 
		 * \return Returns true if we are in 32-kilobyte mode.
		 **/
		inline bool										In32kMode_PGM() const {
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
			return (m_ui8Control & 0b01000) == 0;
		}

		/**
		 * Gets the status of RAM-enable.
		 *
		 * \return Returns the status of RAM-enable.
		 **/
		bool											GetRamEnabled() {
			if ( m_prRom->riInfo.ui16Chip >= CDatabase::LSN_C_MMC1B1 && m_prRom->riInfo.ui16Chip < CDatabase::LSN_C_MMC1_END ) {
				/*
				 *  In order to enable PRG RAM on SNROM...
				 *	The bit in $E000 must be 0 (which the chip itself transforms into 5V)
				 *	The bit in $A000 must be 0 (which the chip keeps as 0)
				 *	The bit in $C000 must either be 0, or the bit in $8000 must specify 8k CHR banking
				 */
				return (m_prPgmBank.bPgmRamEnable) &&
					(m_crChrBanks[1].bPgmRamEnable || In8kMode_CHR()) &&
					(m_crChrBanks[0].bPgmRamEnable);
			}
			else { return m_prRom->riInfo.ui16Chip == CDatabase::LSN_C_MMC1A; }
		}

		// ================
		// RAM
		// ================
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
				_ui8Ret = pmThis->m_ui8PgmRam[_ui16Parm1];
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
				pmThis->m_ui8PgmRam[_ui16Parm1] = _ui8Val;
			}
		}

		/**
		 * Reads from the PGM RAM with bank-switching.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						Mapper001PgmRamRead_OB( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
			CMapper001 * pmThis = reinterpret_cast<CMapper001 *>(_pvParm0);
			if ( pmThis->m_bRamEnabled ) {
				if ( pmThis->In8kMode_CHR() ) {
					size_t sIdx = pmThis->m_crChrBanks[0].ui8PgmRamBank;
					_ui8Ret = pmThis->m_puiPgmRamOuterBanks[sIdx][_ui16Parm1];
				}
				else {
					auto sIdx = (pmThis->m_pbPpuBus->LastBusAddress() >> 12) & 1;
					_ui8Ret = pmThis->m_puiPgmRamOuterBanks[pmThis->m_crChrBanks[sIdx].ui8PgmRamBank][_ui16Parm1];
				}
			}
		}

		/**
		 * Writes to the PGM RAM with bank-switching.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						Mapper001PgmRamWrite_OB( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CMapper001 * pmThis = reinterpret_cast<CMapper001 *>(_pvParm0);
			if ( pmThis->m_bRamEnabled ) {
				if ( pmThis->In8kMode_CHR() ) {
					size_t sIdx = pmThis->m_crChrBanks[0].ui8PgmRamBank;
					pmThis->m_puiPgmRamOuterBanks[sIdx][_ui16Parm1] = _ui8Val;
				}
				else {
					auto sIdx = (pmThis->m_pbPpuBus->LastBusAddress() >> 12) & 1;
					pmThis->m_puiPgmRamOuterBanks[pmThis->m_crChrBanks[sIdx].ui8PgmRamBank][_ui16Parm1] = _ui8Val;
				}
			}
		}

		// ================
		// CPU
		// ================
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
			if ( !pmThis->In32kMode_PGM() ) {
				// 16-kilobyte chunks.
				if ( _ui16Parm1 >= (0xC000 - 0x8000) ) {
					// Hi chunk.
					size_t sBnk = pmThis->m_prPgmBank.ui8PgmBank[1];
					_ui8Ret = pmThis->m_prRom->vPrgRom.data()[sBnk*PgmBankSize()+(_ui16Parm1&0x3FFF)];
				}
				else {
					// Lo chunk.
					size_t sBnk = pmThis->m_prPgmBank.ui8PgmBank[0];
					_ui8Ret = pmThis->m_prRom->vPrgRom.data()[sBnk*PgmBankSize()+(_ui16Parm1/*&0x3FFF*/)];
				}
			}
			else {
				// 32 kikobytes.
				size_t sBnk = pmThis->m_prPgmBank.ui8PgmBank[0] >> 1;
				_ui8Ret = pmThis->m_prRom->vPrgRom.data()[sBnk*(PgmBankSize()*2)+(_ui16Parm1/*&0x7FFF*/)];
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
				return;
			}
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
						pmThis->m_bRamEnabled = pmThis->GetRamEnabled();
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
							pmThis->m_crChrBanks[0].ui8ChrBank = pmThis->m_ui8Load & 0b00001;
							pmThis->m_crChrBanks[0].bPgmRamEnable = !(pmThis->m_ui8Load >> 4);
							pmThis->m_bRamEnabled = pmThis->GetRamEnabled();
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
							pmThis->m_crChrBanks[0].ui8ChrBank = pmThis->m_ui8Load & 0b11111;
							// CHR ROM bank mode (0: switch 8 KB at a time; 1: switch two separate 4 KB banks)
							//if ( (pmThis->m_ui8Control & 0b10000) == 0 ) {
							//	/*pmThis->SetChrBank<0, ChrBankSize()*2>( (pmThis->m_ui8Load & 0b11110) >> 1 );

							//	pmThis->SetChrBank<LSN_CHR_BNK_SMALL+0, ChrBankSize()>( pmThis->m_ui8Load & 0b11111 );
							//	pmThis->SetChrBank<LSN_CHR_BNK_SMALL+1, ChrBankSize()>( (pmThis->m_ui8Load & 0b11111) + 1 );*/
							//}
							//else {
							//	/*pmThis->SetChrBank<0, ChrBankSize()*2>( (pmThis->m_ui8Load & 0b11110) >> 1 );
							//
							//	pmThis->SetChrBank<LSN_CHR_BNK_SMALL+0, ChrBankSize()>( pmThis->m_ui8Load & 0b11111 );*/
							//}
						}
					}
				}
				else if ( _ui16Parm1 >= 0xC000 && _ui16Parm1 < 0xE000 ) {
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
							pmThis->m_crChrBanks[1].ui8ChrBank = pmThis->m_ui8Load & 0b00001;
							pmThis->m_crChrBanks[1].bPgmRamEnable = !(pmThis->m_ui8Load >> 4);
							pmThis->m_bRamEnabled = pmThis->GetRamEnabled();
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
							pmThis->m_crChrBanks[1].ui8ChrBank = pmThis->m_ui8Load & 0b11111;
							//pmThis->SetChrBank<LSN_CHR_BNK_SMALL+1, ChrBankSize()>( pmThis->m_ui8Load & 0b11111 );
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
					pmThis->m_prPgmBank.bPgmRamEnable = !(pmThis->m_ui8Load & 0b10000);
					if ( pmThis->m_prRom->riInfo.ui16Chip >= CDatabase::LSN_C_MMC1B1 && pmThis->m_prRom->riInfo.ui16Chip < CDatabase::LSN_C_MMC1_END ) {
						pmThis->m_bRamEnabled = pmThis->m_prPgmBank.bPgmRamEnable;
					}
					else if ( pmThis->m_prRom->riInfo.ui16PcbClass == CDatabase::LSN_PC_SNROM ) {
						pmThis->m_bRamEnabled = pmThis->GetRamEnabled();
					}

					uint8_t ui8PgmMode = (pmThis->m_ui8Control >> 2) & 0b11;
					switch ( ui8PgmMode ) {
						// 0, 1: switch 32 KB at $8000, ignoring low bit of bank number;
						case 1 : {}
						case 0 : {
							pmThis->m_prPgmBank.ui8PgmBank[0] = pmThis->m_ui8Load & 0b01111;
							/*pmThis->SetPgmBank<0, PgmBankSize()*2>( (pmThis->m_ui8Load & 0b1110) >> 1 );

							pmThis->SetPgmBank<LSN_PGM_BNK_SMALL+0, PgmBankSize()>( pmThis->m_ui8Load & 0b1111 );
							pmThis->SetPgmBank<LSN_PGM_BNK_SMALL+1, PgmBankSize()>( (pmThis->m_ui8Load & 0b1111) + 1 );*/
							break;
						}
						// 2: fix first bank at $8000 and switch 16 KB bank at $C000;
						case 2 : {
							pmThis->m_prPgmBank.ui8PgmBank[0] = 0;
							pmThis->m_prPgmBank.ui8PgmBank[1] = pmThis->m_ui8Load & 0b01111;
							/*pmThis->SetPgmBank<0, PgmBankSize()*2>( 0 );
							pmThis->SetPgmBank<1, PgmBankSize()*2>( (pmThis->m_ui8Load & 0b1110) >> 1 );

							pmThis->SetPgmBank<LSN_PGM_BNK_SMALL+0, PgmBankSize()>( 0 );
							pmThis->SetPgmBank<LSN_PGM_BNK_SMALL+1, PgmBankSize()>( pmThis->m_ui8Load & 0b1111 );*/
							break;
						}
						// 3: fix last bank at $C000 and switch 16 KB bank at $8000)
						case 3 : {
							pmThis->m_prPgmBank.ui8PgmBank[0] = pmThis->m_ui8Load & 0b01111;
							pmThis->m_prPgmBank.ui8PgmBank[1] = pmThis->GetPgmBank<PgmBankSize()>( -1 );
							/*pmThis->SetPgmBank<0, PgmBankSize()*2>( (pmThis->m_ui8Load & 0b1110) >> 1 );
							pmThis->SetPgmBank<1, PgmBankSize()*2>( -1 );

							pmThis->SetPgmBank<LSN_PGM_BNK_SMALL+0, PgmBankSize()>( pmThis->m_ui8Load & 0b1111 );
							pmThis->SetPgmBank<LSN_PGM_BNK_SMALL+1, PgmBankSize()>( -1 );*/
							break;
						}
					}
				}

				pmThis->m_ui8Load = pmThis->m_ui8LoadCnt = 0;
			}
		}

		// ================
		// PPU
		// ================
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
			if ( !pmThis->In8kMode_CHR() ) {
				// 4-kilobyte chunks.
				if ( _ui16Parm1 & (1 << 12) ) {									// Same result, slightly faster.
				//if ( pmThis->m_pbPpuBus->LastBusAddress() & (1 << 12) ) {		// Strictly accurate.
					// Hi chunk.
					size_t sBnk = pmThis->m_crChrBanks[1].ui8ChrBank;
					_ui8Ret = pmThis->m_vChrRam.data()[sBnk*ChrBankSize()+(_ui16Parm1&0x0FFF)];
				}
				else {
					// Lo chunk.
					size_t sBnk = pmThis->m_crChrBanks[0].ui8ChrBank;
					_ui8Ret = pmThis->m_vChrRam.data()[sBnk*ChrBankSize()+(_ui16Parm1/*&0x0FFF*/)];
				}
			}
			else {
				// 8 kilobytes.
				size_t sBnk = pmThis->m_crChrBanks[0].ui8ChrBank >> 1;
				_ui8Ret = pmThis->m_vChrRam.data()[sBnk*(ChrBankSize()*2)+(_ui16Parm1/*&0x1FFF*/)];
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
					size_t sBnk = pmThis->m_crChrBanks[1].ui8ChrBank;
					pmThis->m_vChrRam.data()[sBnk*ChrBankSize()+(_ui16Parm1&0x0FFF)] = _ui8Val;
				}
				else {
					// Lo chunk.
					size_t sBnk = pmThis->m_crChrBanks[0].ui8ChrBank;
					pmThis->m_vChrRam.data()[sBnk*ChrBankSize()+(_ui16Parm1/*&0x0FFF*/)] = _ui8Val;
				}
			}
			else {
				// 8 kilobytes.
				size_t sBnk = pmThis->m_crChrBanks[0].ui8ChrBank >> 1;
				pmThis->m_vChrRam.data()[sBnk*(ChrBankSize()*2)+(_ui16Parm1/*&0x1FFF*/)] = _ui8Val;
			}
		}
	};

}	// namespace lsn


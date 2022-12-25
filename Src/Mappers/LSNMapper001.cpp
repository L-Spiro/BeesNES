/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 001 implementation.
 */

#include "LSNMapper001.h"


namespace lsn {

	CMapper001::CMapper001() {
	}
	CMapper001::~CMapper001() {
	}

	// == Functions.
	/**
	 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
	 *
	 * \param _rRom The ROM data.
	 */
	void CMapper001::InitWithRom( LSN_ROM &_rRom ) {
		CMapperBase::InitWithRom( _rRom );
		m_ui8PgmBank = 3;
		m_ui8Control = 0x1C;
		m_ui8Load = 0;
		m_ui8LoadCnt = 0;

		m_ui8ChrBank4Lo = m_ui8ChrBank4Hi = 0;

		m_ui8PgmBank16Lo = 0;
		m_ui8PgmBank16Hi = uint8_t( m_prRom->vPrgRom.size() / (16 * 1024) - 1 );
	}

	/**
	 * Applies mapping to the CPU and PPU busses.
	 *
	 * \param _pbCpuBus A pointer to the CPU bus.
	 * \param _pbPpuBus A pointer to the PPU bus.
	 */
	void CMapper001::ApplyMap( CCpuBus * _pbCpuBus, CPpuBus * _pbPpuBus ) {
		// Set the reads and writes of the RAM.
		for ( uint32_t I = 0x6000; I < 0x8000; ++I ) {
			_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper001::Mapper001PgmRamRead, this, uint16_t( I - 0x6000 ) );
			_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper001::Mapper001PgmRamWrite, this, uint16_t( I - 0x6000 ) );
		}

		// Writes to the whole area are used to select a bank.
		for ( uint32_t I = 0x8000; I < 0x10000; ++I ) {
			_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper001::Read_PGM_8000_FFFF, this, uint16_t( I - 0x8000 ) );
			_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper001::Write_PGM_8000_FFFF, this, uint16_t( I ) );
		}

		// CHR ROM.
		for ( uint32_t I = 0x0000; I < 0x2000; ++I ) {
			_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper001::Read_CHR_0000_1FFF, this, uint16_t( I - 0x0000 ) );
		}

		// Make the pattern memory into RAM.
		/*for ( uint32_t I = LSN_PPU_PATTERN_TABLES; I < LSN_PPU_NAMETABLES; ++I ) {
			_pbPpuBus->SetWriteFunc( uint16_t( I ), CPpuBus::StdWrite, this, uint16_t( ((I - LSN_PPU_PATTERN_TABLES) % LSN_PPU_PATTERN_TABLE_SIZE) + LSN_PPU_PATTERN_TABLES ) );
		}*/


		ApplyControllableMirrorMap( _pbPpuBus );
	}

	/**
	 * Reads from the PGM RAM.
	 *
	 * \param _pvParm0 A data value assigned to this address.
	 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
	 * \param _pui8Data The buffer from which to read.
	 * \param _ui8Ret The read value.
	 */
	void LSN_FASTCALL CMapper001::Mapper001PgmRamRead( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
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
	void LSN_FASTCALL CMapper001::Mapper001PgmRamWrite( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
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
	void LSN_FASTCALL CMapper001::Read_PGM_8000_FFFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
		CMapper001 * pmThis = reinterpret_cast<CMapper001 *>(_pvParm0);
		if ( pmThis->m_ui8Control & 0b01000 ) {
			// 8-kilobyte chunks.
			if ( _ui16Parm1 >= 0xC000 - 0x8000 ) {
				// Hi chunk.
				_ui8Ret = pmThis->m_prRom->vPrgRom[pmThis->m_ui8PgmBank16Hi*0x4000+(_ui16Parm1&0x3FFF)];
			}
			else {
				// Lo chunk.
				_ui8Ret = pmThis->m_prRom->vPrgRom[pmThis->m_ui8PgmBank16Lo*0x4000+(_ui16Parm1&0x3FFF)];
			}
		}
		else {
			// 32 kikobytes.
			_ui8Ret = pmThis->m_prRom->vPrgRom[pmThis->m_ui8PgmBank*0x8000+(_ui16Parm1&0x7FFF)];
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
	void LSN_FASTCALL CMapper001::Write_PGM_8000_FFFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
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
						pmThis->m_ui8ChrBank = (pmThis->m_ui8Load & 0b11110) >> 1;
						if ( pmThis->m_prRom->vChrRom.size() ) {
							pmThis->m_ui8ChrBank %= uint8_t( pmThis->m_prRom->vChrRom.size() / (0x2000) );
						}
					}
					else {
						pmThis->m_ui8ChrBank4Lo = pmThis->m_ui8Load & 0b11111;
						if ( pmThis->m_prRom->vChrRom.size() ) {
							pmThis->m_ui8ChrBank4Lo %= uint8_t( pmThis->m_prRom->vChrRom.size() / (0x1000) );
						}
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
						pmThis->m_ui8ChrBank4Hi = pmThis->m_ui8Load & 0b11111;
						if ( pmThis->m_prRom->vChrRom.size() ) {
							pmThis->m_ui8ChrBank4Hi %= uint8_t( pmThis->m_prRom->vChrRom.size() / (0x1000) );
						}
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
							pmThis->m_ui8PgmBank = (pmThis->m_ui8Load & 0b1110) >> 1;
							break;
						}
						// 2: fix first bank at $8000 and switch 16 KB bank at $C000;
						case 2 : {
							pmThis->m_ui8PgmBank16Lo = 0;
							pmThis->m_ui8PgmBank16Hi = (pmThis->m_ui8Load & 0b1111);
							break;
						}
						// 3: fix last bank at $C000 and switch 16 KB bank at $8000)
						case 3 : {
							pmThis->m_ui8PgmBank16Lo = (pmThis->m_ui8Load & 0b1111);
							pmThis->m_ui8PgmBank16Hi = uint8_t( pmThis->m_prRom->vPrgRom.size() / (16 * 1024) - 1 );
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
	void LSN_FASTCALL CMapper001::Read_CHR_0000_1FFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t &_ui8Ret ) {
		CMapper001 * pmThis = reinterpret_cast<CMapper001 *>(_pvParm0);
		if ( !pmThis->m_prRom->vChrRom.size() ) {
			_ui8Ret = _pui8Data[_ui16Parm1];
		}
		else {
			if ( pmThis->m_ui8Control & 0b10000 ) {
				// 4-kilobyte chunks.
				if ( _ui16Parm1 >= 0x1000 ) {
					// Hi chunk.
					_ui8Ret = pmThis->m_prRom->vChrRom[pmThis->m_ui8ChrBank4Hi*0x1000+(_ui16Parm1&0x0FFF)];
				}
				else {
					// Lo chunk.
					_ui8Ret = pmThis->m_prRom->vChrRom[pmThis->m_ui8ChrBank4Lo*0x1000+(_ui16Parm1&0x0FFF)];
				}
			}
			else {
				// 8 kilobytes.
				_ui8Ret = pmThis->m_prRom->vChrRom[pmThis->m_ui8ChrBank*0x2000+(_ui16Parm1&0x1FFF)];
			}
		}
	}

}	// namespace lsn

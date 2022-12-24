/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 010 implementation.
 */

#include "LSNMapper010.h"


namespace lsn {

	CMapper010::CMapper010() {
	}
	CMapper010::~CMapper010() {
	}

	// == Functions.
	/**
	 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
	 *
	 * \param _rRom The ROM data.
	 */
	void CMapper010::InitWithRom( LSN_ROM &_rRom ) {
		CMapperBase::InitWithRom( _rRom );
		m_ui8PgmBank = 0;
		m_ui8Latch0 = 0xFD;
		m_ui8Latch1 = 0xFD;

		m_ui8ChrBankLatch0_FD = m_ui8ChrBankLatch0_FE = m_ui8ChrBankLatch1_FD = m_ui8ChrBankLatch1_FE = 0;
	}

	/**
	 * Applies mapping to the CPU and PPU busses.
	 *
	 * \param _pbCpuBus A pointer to the CPU bus.
	 * \param _pbPpuBus A pointer to the PPU bus.
	 */
	void CMapper010::ApplyMap( CCpuBus * _pbCpuBus, CPpuBus * _pbPpuBus ) {
		// ================
		// FIXED BANKS
		// ================
		// 16-kilobyte fixed bank.
		m_stFixedOffset = std::max<size_t>( m_prRom->vPrgRom.size(), (16 * 1024) ) - (16 * 1024);
		for ( uint32_t I = 0xC000; I < 0x10000; ++I ) {
			_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead_Fixed, this, uint16_t( I - 0xC000 ) );
		}

		// ================
		// SWAPPABLE BANKS
		// ================
		// CPU.
		for ( uint32_t I = 0x8000; I < 0xC000; ++I ) {
			_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapperBase::PgmBankRead_4000, this, uint16_t( I - 0x8000 ) );
		}
		// PPU.
		for ( uint32_t I = 0x0000; I < 0x1000; ++I ) {
			_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper010::Mapper010ChrBankRead_0000_0FFF, this, uint16_t( I - 0x0000 ) );
		}
		for ( uint32_t I = 0x1000; I < 0x2000; ++I ) {
			_pbPpuBus->SetReadFunc( uint16_t( I ), &CMapper010::Mapper010ChrBankRead_1000_1FFF, this, uint16_t( I - 0x1000 ) );
		}

		// ================
		// RAM
		// ================
		// Set the reads and writes of the RAM.
		for ( uint32_t I = 0x6000; I < 0x8000; ++I ) {
			_pbCpuBus->SetReadFunc( uint16_t( I ), &CMapper010::Mapper010PgmRamRead, this, uint16_t( I - 0x6000 ) );
			_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper010::Mapper010PgmRamWrite, this, uint16_t( I - 0x6000 ) );
		}

		// ================
		// BANK-SELECT
		// ================
		// Select banks/mirroring.
		for ( uint32_t I = 0xA000; I < 0xB000; ++I ) {
			_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper010::SelectBankA000_AFFF, this, 0 );	// Treated as ROM.
		}
		for ( uint32_t I = 0xB000; I < 0xC000; ++I ) {
			_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper010::SelectBankB000_BFFF, this, 0 );	// Treated as ROM.
		}
		for ( uint32_t I = 0xC000; I < 0xD000; ++I ) {
			_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper010::SelectBankC000_CFFF, this, 0 );	// Treated as ROM.
		}
		for ( uint32_t I = 0xD000; I < 0xE000; ++I ) {
			_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper010::SelectBankD000_DFFF, this, 0 );	// Treated as ROM.
		}
		for ( uint32_t I = 0xE000; I < 0xF000; ++I ) {
			_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper010::SelectBankE000_EFFF, this, 0 );	// Treated as ROM.
		}
		for ( uint32_t I = 0xF000; I < 0x10000; ++I ) {
			_pbCpuBus->SetWriteFunc( uint16_t( I ), &CMapper010::SetMirroringF000_FFFF, this, 0 );	// Treated as ROM.
		}

		// ================
		// PPU READS
		// ================
		for ( uint32_t I = 0x0FD8; I <= 0x0FDF; ++I ) {
			_pbPpuBus->SetTrampolineReadFunc( uint16_t( I ), &CMapper010::SetLatch_0FD8_0FDF_Trampoline, this, uint16_t( I ), &m_tLatch0Trampoline_0FD8_0FDF[uint16_t( I - 0x0FD8 )] );
		}
		for ( uint32_t I = 0x0FE8; I <= 0x0FEF; ++I ) {
			_pbPpuBus->SetTrampolineReadFunc( uint16_t( I ), &CMapper010::SetLatch_0FE8_0FEF_Trampoline, this, uint16_t( I ), &m_tLatch0Trampoline_0FE8_0FEF[uint16_t( I - 0x0FE8 )] );
		}
		for ( uint32_t I = 0x1FD8; I <= 0x1FDF; ++I ) {
			_pbPpuBus->SetTrampolineReadFunc( uint16_t( I ), &CMapper010::SetLatch_1FD8_1FDF_Trampoline, this, uint16_t( I ), &m_tLatch1Trampoline_1FD8_1FDF[uint16_t( I - 0x1FD8 )] );
		}
		for ( uint32_t I = 0x1FE8; I <= 0x1FEF; ++I ) {
			_pbPpuBus->SetTrampolineReadFunc( uint16_t( I ), &CMapper010::SetLatch_1FE8_1FEF_Trampoline, this, uint16_t( I ), &m_tLatch1Trampoline_1FE8_1FEF[uint16_t( I - 0x1FE8 )] );
		}

		// ================
		// MIRRORING
		// ================
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
	void LSN_FASTCALL CMapper010::Mapper010PgmRamRead( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
		CMapper010 * pmThis = reinterpret_cast<CMapper010 *>(_pvParm0);
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
	void LSN_FASTCALL CMapper010::Mapper010PgmRamWrite( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
		CMapper010 * pmThis = reinterpret_cast<CMapper010 *>(_pvParm0);
		pmThis->m_ui8PgmRam[_ui16Parm1] = _ui8Val;
	}

	/**
	 * PPU $0000-$0FFF: Two 4 KB switchable CHR ROM banks.
	 *
	 * \param _pvParm0 A data value assigned to this address.
	 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
	 * \param _pui8Data The buffer from which to read.
	 * \param _ui8Ret The read value.
	 */
	void LSN_FASTCALL CMapper010::Mapper010ChrBankRead_0000_0FFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
		CMapper010 * pmThis = reinterpret_cast<CMapper010 *>(_pvParm0);
		switch ( pmThis->m_ui8Latch0 ) {
			case 0xFD : {
				_ui8Ret = pmThis->m_prRom->vChrRom[pmThis->m_ui8ChrBankLatch0_FD*(4*1024)+_ui16Parm1];
				break;
			}
			case 0xFE : {
				_ui8Ret = pmThis->m_prRom->vChrRom[pmThis->m_ui8ChrBankLatch0_FE*(4*1024)+_ui16Parm1];
				break;
			}
			// I guess if the latch is invalid then return the open bus by doing nothing?  Read the value in _pui8Data? 
		}
	}

	/**
	 * PPU $1000-$1FFF: Two 4 KB switchable CHR ROM banks.
	 *
	 * \param _pvParm0 A data value assigned to this address.
	 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
	 * \param _pui8Data The buffer from which to read.
	 * \param _ui8Ret The read value.
	 */
	void LSN_FASTCALL CMapper010::Mapper010ChrBankRead_1000_1FFF( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
		CMapper010 * pmThis = reinterpret_cast<CMapper010 *>(_pvParm0);
		switch ( pmThis->m_ui8Latch1 ) {
			case 0xFD : {
				_ui8Ret = pmThis->m_prRom->vChrRom[pmThis->m_ui8ChrBankLatch1_FD*(4*1024)+_ui16Parm1];
				break;
			}
			case 0xFE : {
				_ui8Ret = pmThis->m_prRom->vChrRom[pmThis->m_ui8ChrBankLatch1_FE*(4*1024)+_ui16Parm1];
				break;
			}
			// I guess if the latch is invalid then return the open bus by doing nothing?  Read the value in _pui8Data? 
		}
	}

	/**
	 * PRG ROM bank select ($A000-$AFFF).
	 *
	 * \param _pvParm0 A data value assigned to this address.
	 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
	 * \param _pui8Data The buffer to which to write.
	 * \param _ui8Ret The value to write.
	 */
	void LSN_FASTCALL CMapper010::SelectBankA000_AFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
		CMapper010 * pmThis = reinterpret_cast<CMapper010 *>(_pvParm0);
		/*
		 *	7  bit  0
		 *	---- ----
		 *	xxxx PPPP
		 *		 ||||
		 *		 ++++- Select 16 KB PRG ROM bank for CPU $8000-$BFFF
		 */
		pmThis->m_ui8PgmBank = (_ui8Val & 0b1111) % (pmThis->m_prRom->vPrgRom.size() / (16 * 1024));
	}

	/**
	 * CHR ROM $FD/0000 bank select ($B000-$BFFF).
	 *
	 * \param _pvParm0 A data value assigned to this address.
	 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
	 * \param _pui8Data The buffer to which to write.
	 * \param _ui8Ret The value to write.
	 */
	void LSN_FASTCALL CMapper010::SelectBankB000_BFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
		CMapper010 * pmThis = reinterpret_cast<CMapper010 *>(_pvParm0);
		/*
		 *	7  bit  0
		 *	---- ----
		 *	xxxC CCCC
		 *	   | ||||
		 *	   +-++++- Select 4 KB CHR ROM bank for PPU $0000-$0FFF
		 *			   used when latch 0 = $FD
		 */
		pmThis->m_ui8ChrBankLatch0_FD = (_ui8Val & 0b11111) % (pmThis->m_prRom->vChrRom.size() / (4 * 1024));
	}

	/**
	 * CHR ROM $FE/0000 bank select ($C000-$CFFF).
	 *
	 * \param _pvParm0 A data value assigned to this address.
	 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
	 * \param _pui8Data The buffer to which to write.
	 * \param _ui8Ret The value to write.
	 */
	void LSN_FASTCALL CMapper010::SelectBankC000_CFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
		CMapper010 * pmThis = reinterpret_cast<CMapper010 *>(_pvParm0);
		/*
		 *	7  bit  0
		 *	---- ----
		 *	xxxC CCCC
		 *	   | ||||
		 *	   +-++++- Select 4 KB CHR ROM bank for PPU $0000-$0FFF
		 *			   used when latch 0 = $FE
		 */
		pmThis->m_ui8ChrBankLatch0_FE = (_ui8Val & 0b11111) % (pmThis->m_prRom->vChrRom.size() / (4 * 1024));
	}

	/**
	 * CHR ROM $FD/1000 bank select ($D000-$DFFF).
	 *
	 * \param _pvParm0 A data value assigned to this address.
	 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
	 * \param _pui8Data The buffer to which to write.
	 * \param _ui8Ret The value to write.
	 */
	void LSN_FASTCALL CMapper010::SelectBankD000_DFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
		CMapper010 * pmThis = reinterpret_cast<CMapper010 *>(_pvParm0);
		/*
		 *	7  bit  0
		 *	---- ----
		 *	xxxC CCCC
		 *	   | ||||
		 *	   +-++++- Select 4 KB CHR ROM bank for PPU $1000-$1FFF
		 *			   used when latch 1 = $FD
		 */
		pmThis->m_ui8ChrBankLatch1_FD = (_ui8Val & 0b11111) % (pmThis->m_prRom->vChrRom.size() / (4 * 1024));
	}

	/**
	 * CHR ROM $FE/1000 bank select ($E000-$EFFF).
	 *
	 * \param _pvParm0 A data value assigned to this address.
	 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
	 * \param _pui8Data The buffer to which to write.
	 * \param _ui8Ret The value to write.
	 */
	void LSN_FASTCALL CMapper010::SelectBankE000_EFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
		CMapper010 * pmThis = reinterpret_cast<CMapper010 *>(_pvParm0);
		/*
		 *	7  bit  0
		 *	---- ----
		 *	xxxC CCCC
		 *	   | ||||
		 *	   +-++++- Select 4 KB CHR ROM bank for PPU $1000-$1FFF
		 *			   used when latch 1 = $FE
		 */
		pmThis->m_ui8ChrBankLatch1_FE = (_ui8Val & 0b11111) % (pmThis->m_prRom->vChrRom.size() / (4 * 1024));
	}

	/**
	 * Mirroring ($F000-$FFFF).
	 *
	 * \param _pvParm0 A data value assigned to this address.
	 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
	 * \param _pui8Data The buffer to which to write.
	 * \param _ui8Ret The value to write.
	 */
	void LSN_FASTCALL CMapper010::SetMirroringF000_FFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
		CMapper010 * pmThis = reinterpret_cast<CMapper010 *>(_pvParm0);
		/*
		 *	7  bit  0
		 *	---- ----
		 *	xxxx xxxM
		 *			|
		 *			+- Select nametable mirroring (0: vertical; 1: horizontal)
		 */
		switch ( _ui8Val & 0b0001 ) {
			case 0 : {
				pmThis->m_mmMirror = LSN_MM_VERTICAL;
				break;
			}
			case 1 : {
				pmThis->m_mmMirror = LSN_MM_HORIZONTAL;
				break;
			}
		}
	}

	/**
	 * PPU reads $0FD8 through $0FDF: latch 0 is set to $FD.
	 *
	 * \param _pvParm0 A data value assigned to this address.
	 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
	 * \param _pui8Data The buffer from which to read.
	 * \param _ui8Ret The read value.
	 */
	void LSN_FASTCALL CMapper010::SetLatch_0FD8_0FDF_Trampoline( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * _pui8Data, uint8_t &_ui8Ret ) {
		CCpuBus::LSN_TRAMPOLINE * ptTramp = reinterpret_cast<CCpuBus::LSN_TRAMPOLINE *>(_pvParm0);
		CMapper010 * pmBase = reinterpret_cast<CMapper010 *>(ptTramp->pvReaderParm0);
		pmBase->m_ui8Latch0 = 0xFD;
		ptTramp->aaOriginalFuncs.pfReader( ptTramp->aaOriginalFuncs.pvReaderParm0,
			ptTramp->aaOriginalFuncs.ui16ReaderParm1,
			_pui8Data, _ui8Ret );
		
	}

	/**
	 * PPU reads $0FE8 through $0FEF: latch 0 is set to $FE.
	 *
	 * \param _pvParm0 A data value assigned to this address.
	 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
	 * \param _pui8Data The buffer from which to read.
	 * \param _ui8Ret The read value.
	 */
	void LSN_FASTCALL CMapper010::SetLatch_0FE8_0FEF_Trampoline( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * _pui8Data, uint8_t &_ui8Ret ) {
		CCpuBus::LSN_TRAMPOLINE * ptTramp = reinterpret_cast<CCpuBus::LSN_TRAMPOLINE *>(_pvParm0);
		CMapper010 * pmBase = reinterpret_cast<CMapper010 *>(ptTramp->pvReaderParm0);
		pmBase->m_ui8Latch0 = 0xFE;
		ptTramp->aaOriginalFuncs.pfReader( ptTramp->aaOriginalFuncs.pvReaderParm0,
			ptTramp->aaOriginalFuncs.ui16ReaderParm1,
			_pui8Data, _ui8Ret );
		
	}

	/**
	 * PPU reads $1FD8 through $1FDF: latch 1 is set to $FD.
	 *
	 * \param _pvParm0 A data value assigned to this address.
	 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
	 * \param _pui8Data The buffer from which to read.
	 * \param _ui8Ret The read value.
	 */
	void LSN_FASTCALL CMapper010::SetLatch_1FD8_1FDF_Trampoline( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * _pui8Data, uint8_t &_ui8Ret ) {
		CCpuBus::LSN_TRAMPOLINE * ptTramp = reinterpret_cast<CCpuBus::LSN_TRAMPOLINE *>(_pvParm0);
		CMapper010 * pmBase = reinterpret_cast<CMapper010 *>(ptTramp->pvReaderParm0);
		pmBase->m_ui8Latch1 = 0xFD;
		ptTramp->aaOriginalFuncs.pfReader( ptTramp->aaOriginalFuncs.pvReaderParm0,
			ptTramp->aaOriginalFuncs.ui16ReaderParm1,
			_pui8Data, _ui8Ret );
		
	}

	/**
	 * PPU reads $1FE8 through $1FEF: latch 1 is set to $FE.
	 *
	 * \param _pvParm0 A data value assigned to this address.
	 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
	 * \param _pui8Data The buffer from which to read.
	 * \param _ui8Ret The read value.
	 */
	void LSN_FASTCALL CMapper010::SetLatch_1FE8_1FEF_Trampoline( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * _pui8Data, uint8_t &_ui8Ret ) {
		CCpuBus::LSN_TRAMPOLINE * ptTramp = reinterpret_cast<CCpuBus::LSN_TRAMPOLINE *>(_pvParm0);
		CMapper010 * pmBase = reinterpret_cast<CMapper010 *>(ptTramp->pvReaderParm0);
		pmBase->m_ui8Latch1 = 0xFE;
		ptTramp->aaOriginalFuncs.pfReader( ptTramp->aaOriginalFuncs.pvReaderParm0,
			ptTramp->aaOriginalFuncs.ui16ReaderParm1,
			_pui8Data, _ui8Ret );
		
	}

}	// namespace lsn

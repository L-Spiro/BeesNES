/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A base claass for all mappers.
 */

#include "LSNMapperBase.h"


namespace lsn {

	CMapperBase::CMapperBase() :
		m_prRom( nullptr ),
		m_stFixedOffset( 0 ),
		m_mmMirror( LSN_MM_HORIZONTAL ),
		m_ui8PgmBank( 0 ),
		m_ui8ChrBank( 0 ) {
	}
	CMapperBase::~CMapperBase() {
	}

	// == Functions.

	/**
	 * Applies a controllable mirroring map.
	 *
	 * \param _pbPpuBus A pointer to the PPU bus.
	 */
	void CMapperBase::ApplyControllableMirrorMap( CPpuBus * _pbPpuBus ) {
		for ( uint32_t I = LSN_PPU_NAMETABLES; I < LSN_PPU_PALETTE_MEMORY; ++I ) {
			uint16_t ui16Root = ((I - LSN_PPU_NAMETABLES) % LSN_PPU_NAMETABLES_SIZE);	// Mirror The $3000-$3EFF range down to $2000-$2FFF.
			//ui16Root += LSN_PPU_NAMETABLES;
			_pbPpuBus->SetReadFunc( uint16_t( I ), CMapperBase::Read_ControllableMirror, this, ui16Root );
			_pbPpuBus->SetWriteFunc( uint16_t( I ), CMapperBase::Write_ControllableMirror, this, ui16Root );
		}
	}

	/**
	 * A standard mapper PGM trampoline read function.  Maps an address to a given byte in the ROM's PGM space.
	 *
	 * \param _pvParm0 A data value assigned to this address.
	 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
	 * \param _pui8Data The buffer from which to read.
	 * \param _ui8Ret The read value.
	 */
	void LSN_FASTCALL CMapperBase::StdMapperCpuRead_Trampoline( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
		CCpuBus::LSN_TRAMPOLINE * ptTramp = reinterpret_cast<CCpuBus::LSN_TRAMPOLINE *>(_pvParm0);
		CMapperBase * pmBase = reinterpret_cast<CMapperBase *>(ptTramp->pvReaderParm0);
		_ui8Ret = pmBase->m_prRom->vPrgRom[_ui16Parm1];
	}

	/**
	 * A standard mapper PGM read function.  Maps an address to a given byte in the ROM's PGM space.
	 *
	 * \param _pvParm0 A data value assigned to this address.
	 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
	 * \param _pui8Data The buffer from which to read.
	 * \param _ui8Ret The read value.
	 */
	void LSN_FASTCALL CMapperBase::StdMapperCpuRead( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
		CMapperBase * pmBase = reinterpret_cast<CMapperBase *>(_pvParm0);
		_ui8Ret = pmBase->m_prRom->vPrgRom[_ui16Parm1];
	}

	/**
	 * Reads from PGM ROM using m_ui8PgmBank to select a bank.
	 *
	 * \param _pvParm0 A data value assigned to this address.
	 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
	 * \param _pui8Data The buffer from which to read.
	 * \param _ui8Ret The read value.
	 */
	void LSN_FASTCALL CMapperBase::PgmBankRead_8000( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
		CMapperBase * pmThis = reinterpret_cast<CMapperBase *>(_pvParm0);
		_ui8Ret = pmThis->m_prRom->vPrgRom[_ui16Parm1+(pmThis->m_ui8PgmBank*0x8000)];
	}

	/**
	 * Reads from PGM ROM using m_ui8PgmBank to select a bank.
	 *
	 * \param _pvParm0 A data value assigned to this address.
	 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
	 * \param _pui8Data The buffer from which to read.
	 * \param _ui8Ret The read value.
	 */
	void LSN_FASTCALL CMapperBase::PgmBankRead_4000( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
		CMapperBase * pmThis = reinterpret_cast<CMapperBase *>(_pvParm0);
		_ui8Ret = pmThis->m_prRom->vPrgRom[_ui16Parm1+(pmThis->m_ui8PgmBank*0x4000)];
	}

	/**
	 * Reads from PGM ROM using m_ui8PgmBank to select a bank.
	 *
	 * \param _pvParm0 A data value assigned to this address.
	 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
	 * \param _pui8Data The buffer from which to read.
	 * \param _ui8Ret The read value.
	 */
	void LSN_FASTCALL CMapperBase::PgmBankRead_2000( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
		CMapperBase * pmThis = reinterpret_cast<CMapperBase *>(_pvParm0);
		_ui8Ret = pmThis->m_prRom->vPrgRom[_ui16Parm1+(pmThis->m_ui8PgmBank*0x2000)];
	}

	/**
	 * Reads the fixed bank.
	 *
	 * \param _pvParm0 A data value assigned to this address.
	 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
	 * \param _pui8Data The buffer from which to read.
	 * \param _ui8Ret The read value.
	 */
	void LSN_FASTCALL CMapperBase::PgmBankRead_Fixed( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
		CMapperBase * pmThis = reinterpret_cast<CMapperBase *>(_pvParm0);
		_ui8Ret = pmThis->m_prRom->vPrgRom[_ui16Parm1+pmThis->m_stFixedOffset];
	}

	/**
	 * Reading from the PPU range 0x0000-0x2000 returns a value read from the current bank.
	 *
	 * \param _pvParm0 A data value assigned to this address.
	 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
	 * \param _pui8Data The buffer from which to read.
	 * \param _ui8Ret The read value.
	 */
	void LSN_FASTCALL CMapperBase::ChrBankRead_2000( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
		CMapperBase * pmThis = reinterpret_cast<CMapperBase *>(_pvParm0);
		_ui8Ret = pmThis->m_prRom->vChrRom[pmThis->m_ui8ChrBank*0x2000+_ui16Parm1];
	}

	/**
	 * Reading from the PPU range 0x0000-0x2000 returns a value read from the current 4-kilobyte bank.
	 *
	 * \param _pvParm0 A data value assigned to this address.
	 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
	 * \param _pui8Data The buffer from which to read.
	 * \param _ui8Ret The read value.
	 */
	void LSN_FASTCALL CMapperBase::ChrBankRead_1000( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * /*_pui8Data*/, uint8_t &_ui8Ret ) {
		CMapperBase * pmThis = reinterpret_cast<CMapperBase *>(_pvParm0);
		_ui8Ret = pmThis->m_prRom->vChrRom[pmThis->m_ui8ChrBank*0x1000+_ui16Parm1];
	}

	/**
	 * Handles reads from controllable mirror.
	 *
	 * \param _pvParm0 A data value assigned to this address.
	 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
	 * \param _pui8Data The buffer from which to read.
	 * \param _ui8Ret The read value.
	 */
	void LSN_FASTCALL CMapperBase::Read_ControllableMirror( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t &_ui8Ret ) {
		CMapperBase * pmThis = reinterpret_cast<CMapperBase *>(_pvParm0);
		_ui8Ret = _pui8Data[MirrorAddress( _ui16Parm1, pmThis->m_mmMirror )];
	}

	/**
	 * Handles writes to controllable mirror.
	 *
	 * \param _pvParm0 A data value assigned to this address.
	 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
	 * \param _pui8Data The buffer to which to write.
	 * \param _ui8Ret The value to write.
	 */
	void LSN_FASTCALL CMapperBase::Write_ControllableMirror( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t _ui8Val ) {
		CMapperBase * pmThis = reinterpret_cast<CMapperBase *>(_pvParm0);
		_pui8Data[MirrorAddress( _ui16Parm1, pmThis->m_mmMirror )] = _ui8Val;
	}

}	// namespace lsn

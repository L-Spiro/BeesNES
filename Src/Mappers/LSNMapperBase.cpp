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
		m_prRom( nullptr ) {
	}
	CMapperBase::~CMapperBase() {
	}

	// == Functions.
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
	void LSN_FASTCALL CMapperBase::StdMapperCpuRead( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t &_ui8Ret ) {
		CMapperBase * pmBase = reinterpret_cast<CMapperBase *>(_pvParm0);
		_ui8Ret = pmBase->m_prRom->vPrgRom[_ui16Parm1];
	}

}	// namespace lsn

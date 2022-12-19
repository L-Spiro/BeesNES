/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A base claass for all mappers.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Bus/LSNBus.h"
#include "../Roms/LSNRom.h"

namespace lsn {

	/**
	 * Class CMapperBase
	 * \brief A base claass for all mappers.
	 *
	 * Description: A base claass for all mappers.
	 */
	class CMapperBase {
	public :
		CMapperBase();
		virtual ~CMapperBase();


		// == Functions.
		/**
		 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
		 *
		 * \param _rRom The ROM data.
		 */
		virtual void									InitWithRom( LSN_ROM &_rRom ) { m_prRom = &_rRom; }

		/**
		 * Applies mapping to the CPU and PPU busses.
		 *
		 * \param _pbCpuBus A pointer to the CPU bus.
		 * \param _pbPpuBus A pointer to the PPU bus.
		 */
		virtual void									ApplyMap( CCpuBus * /*_pbCpuBus*/, CPpuBus * /*_pbPpuBus*/ ) {}


	protected :
		// == Members.
		/** The ROM used to initialize this mapper. */
		LSN_ROM *										m_prRom;


		// == Functions.
		/**
		 * A standard mapper PGM read function.  Maps an address to a given byte in the ROM's PGM space.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to read from _pui8Data.  It is not constant because sometimes reads do modify status registers etc.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 */
		static void LSN_FASTCALL						StdMapperCpuRead_Trampoline( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t &_ui8Ret );

	};

}	// namespace lsn

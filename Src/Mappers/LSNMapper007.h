/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Mapper 007 implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNMapperBase.h"

namespace lsn {

	/**
	 * Class CMapper007
	 * \brief Mapper 007 implementation.
	 *
	 * Description: Mapper 007 implementation.
	 */
	class CMapper007 : public CMapperBase {
	public :
		CMapper007();
		virtual ~CMapper007();


		// == Functions.
		/**
		 * Initializes the mapper with the ROM data.  This is usually to allow the mapper to extract information such as the number of banks it has, as well as make copies of any data it needs to run.
		 *
		 * \param _rRom The ROM data.
		 */
		virtual void									InitWithRom( LSN_ROM &_rRom );

		/**
		 * Applies mapping to the CPU and PPU busses.
		 *
		 * \param _pbCpuBus A pointer to the CPU bus.
		 * \param _pbPpuBus A pointer to the PPU bus.
		 */
		virtual void									ApplyMap( CCpuBus * _pbCpuBus, CPpuBus * _pbPpuBus );


	protected :
		// == Members.


		// == Functions.
		/**
		 * Bank select ($8000-$FFFF).
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Ret The value to write.
		 */
		static void LSN_FASTCALL						SelectBank8000_FFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val );
	};

}	// namespace lsn

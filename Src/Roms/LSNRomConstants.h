/**
 * Copyright L. Spiro 2021
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: ROM constants.
 */


#pragma once

#include "../LSNLSpiroNes.h"

namespace lsn {

	/** ROM header versions. */
	enum class LSN_ROM_HEADER_VERSION {
		LSN_RHV_ARCHAIC_INES,								/**< Archaic iNES. */
		LSN_RHV_INES,										/**< iNES. */
		LSN_RHV_INES_2,										/**< iNES 2.0. */
	};

}	// namespace lsn

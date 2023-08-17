/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Emulator options.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNInputOptions.h"


namespace lsn {

	/**
	 * Class LSN_OPTIONS
	 * \brief Emulator options.
	 *
	 * Description: Emulator options.
	 */
	struct LSN_OPTIONS {
		/** Global input options. */
		LSN_INPUT_OPTIONS					ioGlobalInputOptions;
		/** This game's input options. */
		LSN_INPUT_OPTIONS					ioThisGameInputOptions;
	};

}	// namespace lsn

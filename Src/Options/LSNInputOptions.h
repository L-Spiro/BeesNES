/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Input options.
 */


#pragma once

#include "../LSNLSpiroNes.h"


namespace lsn {

	/**
	 * Class LSN_INPUT_OPTIONS
	 * \brief Input options.
	 *
	 * Description: Input options.
	 */
	struct LSN_INPUT_OPTIONS {
		uint8_t									ui8ConsoleType = uint8_t( LSN_CT_NES );																				/**< The console type (LSN_CT_NES or LSN_CT_FAMICOM). */
		uint8_t									ui8Expansion = uint8_t( LSN_CT_NONE );																				/**< From the LSN_CONTROLLER_TYPES enumeration. */
		uint8_t									ui8Player[4] = { uint8_t( LSN_CT_NONE ), uint8_t( LSN_CT_NONE ), uint8_t( LSN_CT_NONE ), uint8_t( LSN_CT_NONE ) };	/**< The 4 controllers, from the LSN_CONTROLLER_TYPES enumeration. */
		bool									bUseGlobal = true;																									/**< Game settings use global settings. */
		bool									bUseFourScore = false;																								/**< Use Four Score. */
	};

}	// namespace lsn

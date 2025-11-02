/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Palette options.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Filters/LSNNesPalette.h"

#include <string>


namespace lsn {

	/**
	 * Class LSN_PALETTE_OPTIONS
	 * \brief Palette options.
	 *
	 * Description: Palette options.
	 */
	struct LSN_PALETTE_OPTIONS {
		// == Members.
		std::wstring							wsPath;																												/**< Path to the palette. */
		CNesPalette::LSN_GAMMA					gCrtGamma = CNesPalette::LSN_G_CRT1;																				/**< CRT gamma #1. */
		CNesPalette::LSN_GAMMA					gMonitorGamma = CNesPalette::LSN_G_sRGB;																			/**< sRGB gamma. */
		bool									bUseGlobal = true;																									/**< Use global settings rather than per-game settings. */
	};

}	// namespace lsn

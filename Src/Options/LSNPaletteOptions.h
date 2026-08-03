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
	 * Struct LSN_PALETTE_OPTIONS
	 * \brief Palette options.
	 *
	 * Description: Palette options.
	 */
	struct LSN_PALETTE_OPTIONS {
		// == Members.
		std::wstring							wsPath;																												/**< Path to the palette. */
		CNesPalette::LSN_GAMMA					gCrtGamma[LSN_PM_CONSOLE_TOTAL] = {
			CNesPalette::LSN_G_CRT2,			// NTSC.
			CNesPalette::LSN_G_CRT1,			// PAL.
			CNesPalette::LSN_G_CRT1,			// Dendy.
			CNesPalette::LSN_G_CRT2,			// Brazil.
			CNesPalette::LSN_G_CRT1,			// Argentina.
		};																				/**< CRT gamma #2. */
		CNesPalette::LSN_GAMMA					gMonitorGamma = CNesPalette::LSN_G_sRGB;																			/**< sRGB gamma. */
		bool									bUseGlobal = true;																									/**< Use global settings rather than per-game settings. */
	};

}	// namespace lsn

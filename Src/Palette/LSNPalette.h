/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A simple static NES palette.  Very basic.
 */


#pragma once

#include "../LSNLSpiroNes.h"

namespace lsn {

	struct LSN_PALETTE {
		union {
			uint8_t			ui8Rgb[3];
			struct {
				uint8_t		ui8R;
				uint8_t		ui8G;
				uint8_t		ui8B;
			}				sRgb;
		}					uVals[1<<(6+1+1+1)];	// 6 bits for the standard set of colors, then 1 more bit for each emphasis set.
	};

}	// namespace lsn

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
		}					uVals[0x40];
	};

}	// namespace lsn

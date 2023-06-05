/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The primary base class for each APU unit (pulse, noise, etc.)
 */

#include "LSNPulse.h"


namespace lsn {

	// == Members.
	/** The length table. */
	uint8_t CApuUnit::m_ui8LenTable[32] = {
		10,254, 20,  2, 40,  4, 80,  6, 160,  8, 60, 10, 14, 12, 26, 14,
		12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30
	};

	/** The noise periods for NTSC and PAL. */
	uint16_t CApuUnit::m_ui16NoisePeriods[LSN_AT_TOTAL][0x10] = {
		{ 4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068 },
		{ 4, 8, 14, 30, 60, 88, 118, 148, 188, 236, 354, 472, 708,  944, 1890, 3778 },
	};

}	// namespace lsn

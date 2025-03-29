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
		{ 4 >> 1, 8 >> 1, 16 >> 1, 32 >> 1, 64 >> 1, 96 >> 1, 128 >> 1, 160 >> 1, 202 >> 1, 254 >> 1, 380 >> 1, 508 >> 1, 762 >> 1, 1016 >> 1, 2034 >> 1, 4068 >> 1 },
		{ 4 >> 1, 8 >> 1, 14 >> 1, 30 >> 1, 60 >> 1, 88 >> 1, 118 >> 1, 148 >> 1, 188 >> 1, 236 >> 1, 354 >> 1, 472 >> 1, 708 >> 1,  944 >> 1, 1890 >> 1, 3778 >> 1 },
	};

	/** The old noise periods for NTSC and PAL. */
	uint16_t CApuUnit::m_ui16NoisePeriodsOld[LSN_AT_TOTAL][0x10] = {
		{ 4 >> 1, 8 >> 1, 16 >> 1, 32 >> 1, 64 >> 1, 96 >> 1, 128 >> 1, 160 >> 1, 202 >> 1, 254 >> 1, 380 >> 1, 508 >> 1, 762 >> 1, 1016 >> 1, 2034 >> 1, 2046 >> 1 },
		{ 4 >> 1, 8 >> 1, 16 >> 1, 32 >> 1, 64 >> 1, 96 >> 1, 128 >> 1, 160 >> 1, 202 >> 1, 254 >> 1, 380 >> 1, 508 >> 1, 762 >> 1, 1016 >> 1, 2034 >> 1, 2046 >> 1 },
	};

}	// namespace lsn

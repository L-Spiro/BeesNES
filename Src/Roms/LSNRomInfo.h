/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: ROM information.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include <string>

namespace lsn {

	/**
	 * Class LSN_ROM_INFO
	 * \brief ROM information.
	 *
	 * Description: ROM information.
	 */
	struct LSN_ROM_INFO {
		std::u16string						s16File;										/**< The full file path in UTF-16. */
		std::u16string						s16RomName;										/**< The ROM file name in UTF-16. */
		uint32_t							ui32Crc;										/**< The ROM CRC. */
		LSN_MIRROR_MODE						mmMirroring;									/**< The ROM mirroring mode. */
		LSN_PPU_METRICS						pmConsoleRegion;								/**< NTSC, PAL, or Dendy. */
		uint16_t							ui16Mapper;										/**< The mapper. */
		uint16_t							ui16SubMapper;									/**< The sub-mapper. */
		uint16_t							ui16Chip;										/**< The mapper chip. */
		bool								bMapperSupported;								/**< If true, the necessary mapper is supported. */
	};

}	// namespace lsn

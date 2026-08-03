/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: ROM information.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Utilities/LSNMd5.h"

#include <string>

namespace lsn {

	/**
	 * Struct LSN_ROM_INFO
	 * \brief ROM information.
	 *
	 * Description: ROM information.
	 */
	struct LSN_ROM_INFO {
		std::u16string						s16File;										/**< The full file path in UTF-16. */
		std::u16string						s16RomName;										/**< The ROM file name in UTF-16. */
		uint32_t							ui32Crc;										/**< The ROM CRC. */
		uint32_t							ui32HeaderlessCrc;								/**< The ROM headerless CRC. */
		CMd5::LSN_MD5_HASH					mhMd5;											/**< The ROM MD5. */
		CMd5::LSN_MD5_HASH					mhHeaderlessMd5;								/**< The ROM headerless MD5. */
		LSN_MIRROR_MODE						mmMirroring;									/**< The ROM mirroring mode. */
		LSN_PPU_METRICS						pmConsoleRegion;								/**< NTSC, PAL, or Dendy. */
		uint16_t							ui16Mapper;										/**< The mapper. */
		uint16_t							ui16SubMapper;									/**< The sub-mapper. */
		uint16_t							ui16Chip;										/**< The mapper chip. */
		uint16_t							ui16PcbClass;									/**< The PCB class. */
		bool								bBusConficts = true;							/**< Support bus conflicts (when available)? */
		bool								bMapperSupported;								/**< If true, the necessary mapper is supported. */

		// == NSF
		char								cTrackName[32];									/**< Track name, null-terminated. */
		char								cArtistName[32];								/**< Artist name, null-terminated. */
		char								cCopyrightName[32];								/**< Copyright holder, null-terminated. */
		uint8_t								ui8BankValues[8];								/**< Bankswitch init values for $8000-$FFFF. */
		uint16_t							ui16LoadAddress = 0;							/**< Initial address of PC. */
		uint16_t							ui16InitAddress = 0;							/**< Load address. */
		uint16_t							ui16PlayAddress = 0;							/**< Play address. */
		uint16_t							ui16NtscSpeed = 0;								/**< NTSC play speed. */
		uint16_t							ui16PalSpeed = 0;								/**< PAL play speed. */
		uint8_t								ui8TotalTracks = 0;								/**< Total tracks (1-256). */
		uint8_t								ui8StartingTrack = 0;							/**< Starting track (1-256). */
	};

}	// namespace lsn

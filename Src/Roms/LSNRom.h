/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: ROM data.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNRomInfo.h"
#include <vector>

namespace lsn {

	/**
	 * Class LSN_ROM
	 * \brief ROM data.
	 *
	 * Description: ROM data.
	 */
	struct LSN_ROM {
		LSN_ROM_INFO								riInfo;													/**< The ROM information. */

		std::vector<uint8_t>						vPrgRom;												/**< The program ROM data. */
		std::vector<uint8_t>						vChrRom;												/**< The character ROM data. */
		std::vector<uint8_t>						vTrainerData;											/**< Trainer data. */
		std::vector<std::vector<uint8_t>>			vFdsDiskData;											/**< FDS Disk data. */
		std::vector<std::vector<uint8_t>>			vFdsDiskHeaders;										/**< FDS disk headers. */

		std::vector<uint8_t>						vRawData;												/**< The raw ROM image. */

		int32_t										i32ChrRamSize = -1;										/**< Character RAM size. */
		int32_t										i32SaveChrRamSize = -1;									/**< Save character RAM size. */
		int32_t										i32SaveRamSize = -1;									/**< Save RM size. */
		int32_t										i32WorkRamSize = -1;									/**< Work RAM size. */

		std::wstring								wsSaveFolder;											/**< Folder where ROM-specific data will be stored. */
		std::u16string								u16SaveFilePrefix;										/**< The prefix for all of the files specific to this ROM. */
	};

}	// namespace lsn

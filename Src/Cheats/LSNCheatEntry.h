/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A cheat entry.
 */


#pragma once


#include "../LSNLSpiroNes.h"

#include <string>
#include <vector>

namespace lsn {

	/**
	 * struct LSN_CHEAT_ENTRY
	 * \brief A cheat entry.
	 *
	 * Description: A cheat entry.
	 */
	struct LSN_CHEAT_ENTRY {
		std::wstring									wsDescription;			/**< The cheat description. */

		struct LSN_ADDRESS {
			uint16_t									ui16Address = 0;		/**< The cheat address. */
			uint8_t										ui8Value = 0;			/**< The value to return. */
			uint8_t										ui8CompareValue = 0;	/**< The optional compare value. */
			bool										bUseCompare = false;	/**< If true, ui8Value is only returned on reads to ui16Address if the real value at ui16Address is ui8CompareValue. */
		};
		std::vector<LSN_ADDRESS>						vAddresses;
		bool											bEnabled = false;		/**< Whether the cheat is enabled. */
	};

}	// namespace lsn

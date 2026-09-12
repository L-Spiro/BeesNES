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
		// == Enumerations.
		enum LSN_CHEAT_TYPE : uint16_t {
			LSN_CT_GAME_GENIE,													/**< Game Geni codes. */
			LSN_CT_CUSTOM,														/**< Custom (AAAA:VV:CC). */
		};

		// == Members.
		std::wstring									wsDescription;			/**< The cheat description. */
		std::wstring									wsNotes;				/**< Cheat notes. */
		std::u16string									u16sFile;				/**< File name for the cheat. */

		struct LSN_ADDRESS {
			std::string									sCode;					/**< The code in text form. */
			uint16_t									ui16Address = 0;		/**< The cheat address. */
			uint8_t										ui8Value = 0;			/**< The value to return. */
			uint8_t										ui8CompareValue = 0;	/**< The optional compare value. */
			bool										bUseCompare = false;	/**< If true, ui8Value is only returned on reads to ui16Address if the real value at ui16Address is ui8CompareValue. */
		};
		std::vector<LSN_ADDRESS>						vAddresses;
		LSN_CHEAT_TYPE									ctType = LSN_CT_CUSTOM;	/**< The cheat type. */
		bool											bEnabled = false;		/**< Whether the cheat is enabled. */
	};

}	// namespace lsn

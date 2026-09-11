/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Converts cheat files of various types to an internal representation.
 */


#pragma once


#include "../LSNLSpiroNes.h"
#include "LSNCheatEntry.h"

#include <LSONJson.h>

namespace lsn {

	/**
	 * Class CCheatConverter
	 * \brief Converts cheat files of various types to an internal representation.
	 *
	 * Description: Converts cheat files of various types to an internal representation.
	 */
	class CCheatConverter {
	public :
		/*CCheatConverter() {
		}
		~CCheatConverter() {
		}*/


		// == Functions.
		/**
		 * Reads a Mesen .JSON file and returns all contained cheats inside it.
		 * 
		 * \param _vJson The JSON file loaded into memory.
		 * \param _vCheats The returned cheat list for the given file.
		 * \param _wsError If false is returned, this contains the error to print for the user.
		 * \return Returns true if all cheats were loaded.  If false, _wsError is filled with an error string to present to the user.
		 **/
		static bool										LoadMesenJson( const std::vector<uint8_t> &_vJson, std::vector<LSN_CHEAT_ENTRY> &_vCheats,
			std::wstring &_wsError );
	};

}	// namespace lsn

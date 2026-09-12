/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The global manager of cheats.
 */


#pragma once


#include "../LSNLSpiroNes.h"
#include "LSNCheatConverter.h"
#include "LSNCheatEntry.h"

#include <filesystem>
#include <vector>

namespace lsn {

	/**
	 * Class CCheatManager
	 * \brief The global manager of cheats.
	 *
	 * Description: The global manager of cheats.
	 */
	class CCheatManager {
	public :
		// == Functions.
		/**
		 * Loads all of the built-in cheats given a path to the .ZIP file.
		 * 
		 * \param _pPath Path to the .ZIP file full of .JSON files full of cheats to load.
		 * \return Returns an error string to present to the user or an empty string on success.
		 **/
		static std::wstring								InitializeCheatLibrary( const std::filesystem::path &_pPath );

		/**
		 * Shuts down the cheat manager at the end of the application life cycle.
		 **/
		static void										DestroyCheatLibrary();


	protected :
		// == Members.
		static std::vector<LSN_CHEAT_ENTRY>				s_vBuiltInCheats;				/**< Built-in cheats. */
	};

}	// namespace lsn

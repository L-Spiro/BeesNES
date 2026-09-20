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
#include <set>
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
		CCheatManager() {
		}
		~CCheatManager() {
			DestroyCheatLibrary();
		}


		// == Functions.
		/**
		 * Loads all of the built-in cheats given a path to the .ZIP file.
		 * 
		 * \param _pPath Path to the .ZIP file full of .JSON files full of cheats to load.
		 * \return Returns an error string to present to the user or an empty string on success.
		 **/
		std::wstring								InitializeCheatLibrary( const std::filesystem::path &_pPath );

		/**
		 * Shuts down the cheat manager at the end of the application life cycle.
		 **/
		void										DestroyCheatLibrary();

		/**
		 * Gets the cheat filter.
		 * 
		 * \return Returns a constant reference to the current cheat filter.
		 **/
		const std::wstring &						GetCheatFilter() const { return m_wsFilter; }

		/**
		 * Sets the cheat filter.
		 * 
		 * \param _wsFilter The cheat filter to set.
		 * \return Returns the previous cheat filter.
		 **/
		std::wstring								SetCheatFilter( const std::wstring &_wsFilter ) {
			std::wstring wsTmp = m_wsFilter;
			m_wsFilter = _wsFilter;
			return wsTmp;
		}

		/**
		 * Determines how many games remain through the current filter.  If the filter is empty, all games pass.
		 * 
		 * \return Returns the number of games that are unfiltered via SetCheatFilter().
		 **/
		size_t										CountUnfiltered() const;

		/**
		 * Determines if there is more than 1 game in the unfiltered set.
		 * 
		 * \return Returns true if there is more than 1 game in the unfiltered set.
		 **/
		bool										MoreThanOneUnfiltered() const;

		/**
		 * Gathers all of the names of the games that match the filter string.
		 * 
		 * \return Returns all names of games that pass the filter.
		 **/
		std::set<std::u16string>					GatherGamesWithFilter() const;

		/**
		 * Gathers all of the cheats for a given game.
		 * 
		 * \param _u16sGame The name of the game whose cheats are to be gathered.
		 * \return Returns a vector of indices into the built-in and custom cheats.  If the high bit is set, the cheat is part of the custom set.
		 **/
		std::vector<uint32_t>						GatherCheatsForGame( const std::u16string &_u16sGame ) const;

		/**
		 * Gets a constant reference cheat entry by index.  If the high bit is set, the cheat is in the custom table.
		 * 
		 * \param _ui32Idx The extended index of the cheat to get.
		 * \return Returns a reference to the given cheat.
		 **/
		const LSN_CHEAT_ENTRY &						CheatByIdx( uint32_t _ui32Idx ) const {
			if ( _ui32Idx & 0x80000000U ) {
				// Temp.
				//return m_vBuiltInCheats[0];
			}
			return m_vBuiltInCheats[_ui32Idx];
		}


	protected :
		// == Members.
		std::vector<LSN_CHEAT_ENTRY>				m_vBuiltInCheats;				/**< Built-in cheats. */
		std::wstring								m_wsFilter;						/**< The cheat filter. */
	};

}	// namespace lsn

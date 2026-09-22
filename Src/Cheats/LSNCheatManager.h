/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The global manager of cheats.
 */


#pragma once


#include "../LSNLSpiroNes.h"
#include "../Utilities/LSNHotkeyManager.h"
#include "LSNCheatConverter.h"
#include "LSNCheatEntry.h"

#include <Helpers/LSWHelpers.h>

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


		// == Types.
		/** An active cheat. */
		struct LSN_ACTIVE_CHEAT {
			uint32_t								ui32Idx;					/**< The "extended" or "virtual" index of the item (high bit set = custom cheat. */
#ifdef LSN_USE_WINDOWS
			lsw::LSW_KEY							kToggleOrEnable;			/**< If kDisable is 0, this is a toggle key, otherwise it is the Enable companion to the Disable key. */
			lsw::LSW_KEY							kDisable;					/**< If 0, the mode is Toggle, otherwise it is Enable/Disable. */
#endif	// #ifdef LSN_USE_WINDOWS

			
			/**
			 * Less-than operator.
			 * 
			 * \param _acOther the operand.
			 * \return Returns true if this->ui32Idx < _acOther.ui32Idx.
			 **/
			inline bool								operator < ( const LSN_ACTIVE_CHEAT &_acOther ) const {
				return this->ui32Idx < _acOther.ui32Idx;
			}

			/**
			 * Equality operator.
			 * 
			 * \param _acOther the operand.
			 * \return Returns true if this->ui32Idx == _acOther.ui32Idx.
			 **/
			inline bool								operator == ( const LSN_ACTIVE_CHEAT &_acOther ) const {
				return this->ui32Idx == _acOther.ui32Idx;
			}
		};


		// == Functions.
		/**
		 * Loads all of the built-in cheats given a path to the .ZIP file.
		 * 
		 * \param _pPath Path to the .ZIP file full of .JSON files full of cheats to load.
		 * \param _phmHotkeyManager A pointer to a hotkey manager to be used by this object.
		 * \return Returns an error string to present to the user or an empty string on success.
		 **/
		std::wstring								InitializeCheatLibrary( const std::filesystem::path &_pPath, CHotkeyManager * _phmHotkeyManager );

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
		 * Auto-sets the cheat filter given a game name.
		 * 
		 * \param _u16sName The name of the game for which to auto-set the filter.
		 * \return Returns the previous cheat filter.
		 **/
		std::wstring								AutoSetCheatFilter( const std::u16string &_u16sName );

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

		/**
		 * Gets a pointer to the hotkey manager used by this object.
		 * 
		 * \return Returns a pointer to our hotkey manager.
		 **/
		inline CHotkeyManager *						HotkeyManager() { return m_phmHotkeyManager; }


	protected :
		// == Members.
		std::vector<LSN_CHEAT_ENTRY>				m_vBuiltInCheats;				/**< Built-in cheats. */
		std::wstring								m_wsFilter;						/**< The cheat filter. */
		std::set<LSN_ACTIVE_CHEAT>					m_sActiveCheats;				/**< The list of active cheats. */
		CHotkeyManager *							m_phmHotkeyManager = nullptr;	/**< A pointer to the hotkey manager. */
	};

}	// namespace lsn

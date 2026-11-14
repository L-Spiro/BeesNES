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
#include <shared_mutex>
#include <unordered_map>
#include <vector>

#define LSN_CHEAT_MAN_CUSTOM_FLAG					0x80000U
#define LSN_CHEAT_MAN_INVALID_IDX					0xFFFFFFFFU

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
		/**
		 * The run-time state of a cheat:  whether it is enabled and which hotkeys control it.  Only cheats that are enabled or that have a
		 *	hotkey have one of these (see CCheatManager::SetActiveCheat()), so cheats that are never touched cost nothing extra.
		 **/
		struct LSN_ACTIVE_CHEAT {
			uint32_t								ui32Idx = 0;				/**< The "extended" or "virtual" index of the item (LSN_CHEAT_MAN_CUSTOM_FLAG set = custom cheat). */
			bool									bEnabled = false;			/**< Whether the cheat is enabled. */
			bool									bToggle = true;				/**< If true, kToggleOrEnable toggles the cheat (kDisable is unused), otherwise kToggleOrEnable enables it and kDisable disables it. */
#ifdef LSN_USE_WINDOWS
			lsw::LSW_KEY							kToggleOrEnable = {};		/**< The Toggle key, or the Enable key in Enable/Disable mode.  A bKeyCode of 0 means no key. */
			lsw::LSW_KEY							kDisable = {};				/**< The Disable key in Enable/Disable mode.  A bKeyCode of 0 means no key. */
#endif	// #ifdef LSN_USE_WINDOWS


			// == Functions.
			/**
			 * Determines whether the hotkeys are in Toggle mode (one key toggles the cheat) or Enable/Disable mode (one key enables it and
			 *	another disables it).
			 * 
			 * \return Returns true if the hotkeys are in Toggle mode.
			 **/
			inline bool								IsToggle() const {
				return bToggle;
			}

			/**
			 * Determines whether any hotkey is assigned.
			 * 
			 * \return Returns true if at least one hotkey is assigned.
			 **/
			inline bool								HasHotkey() const {
#ifdef LSN_USE_WINDOWS
				return kToggleOrEnable.bKeyCode != 0 || kDisable.bKeyCode != 0;
#else
				return false;
#endif	// #ifdef LSN_USE_WINDOWS
			}

			/**
			 * Determines whether this record needs to be kept (the cheat is enabled or has a hotkey).
			 * 
			 * \return Returns true if the cheat is enabled or has a hotkey.
			 **/
			inline bool								IsNeeded() const {
				return bEnabled || HasHotkey();
			}

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
		 * Loads all of the built-in cheats given a path to the .ZIP file.  The parsed cheats are cached in a binary file next to the .ZIP
		 *	file (the same name with the extension .lsch), which is loaded instead of the .ZIP file as long as it matches the size and time
		 *	of the .ZIP file.
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
		inline const LSN_CHEAT_ENTRY &				CheatByIdx( uint32_t _ui32Idx ) const {
			if ( IsCustom( _ui32Idx ) ) {
				return m_vCustomCheats[CustomSlot( _ui32Idx )].ceEntry;
			}
			return m_vBuiltInCheats[_ui32Idx];
		}

		/**
		 * Determines whether an extended index refers to an existing cheat.
		 * 
		 * \param _ui32Idx The extended index to check.
		 * \return Returns true if the index refers to a built-in cheat or a custom cheat that has not been deleted.
		 **/
		bool										IsValidIdx( uint32_t _ui32Idx ) const;

		/**
		 * Adds a custom (user-defined) cheat.
		 * 
		 * \param _ceEntry The cheat to add.
		 * \param _pacActive Optional run-time state (enabled state and hotkeys) for the cheat.  Its ui32Idx is ignored.
		 * \return Returns the extended index of the new cheat or LSN_CHEAT_MAN_INVALID_IDX on failure.
		 **/
		uint32_t									AddCustomCheat( const LSN_CHEAT_ENTRY &_ceEntry, const LSN_ACTIVE_CHEAT * _pacActive = nullptr );

		/**
		 * Replaces a custom cheat.  Built-in cheats cannot be changed.
		 * 
		 * \param _ui32Idx The extended index of the custom cheat to replace.
		 * \param _ceEntry The new cheat data.
		 * \param _pacActive Optional new run-time state (enabled state and hotkeys) for the cheat.  Its ui32Idx is ignored.
		 * \return Returns true if _ui32Idx refers to an existing custom cheat and it was replaced.
		 **/
		bool										UpdateCustomCheat( uint32_t _ui32Idx, const LSN_CHEAT_ENTRY &_ceEntry, const LSN_ACTIVE_CHEAT * _pacActive = nullptr );

		/**
		 * Deletes a custom cheat and its run-time state.  Built-in cheats cannot be deleted.  The extended indices of the other cheats do not
		 *	change; the slot is reused by a later AddCustomCheat().
		 * 
		 * \param _ui32Idx The extended index of the custom cheat to delete.
		 * \return Returns true if _ui32Idx referred to an existing custom cheat.
		 **/
		bool										DeleteCustomCheat( uint32_t _ui32Idx );

		/**
		 * Gets the run-time state of a cheat.
		 * 
		 * \param _ui32Idx The extended index of the cheat.
		 * \param _acRet Holds the returned state.  If the cheat has none, it is filled with the defaults (disabled, no hotkeys).
		 * \return Returns true if the cheat has run-time state (it is enabled or has a hotkey).
		 **/
		bool										GetActiveCheat( uint32_t _ui32Idx, LSN_ACTIVE_CHEAT &_acRet ) const;

		/**
		 * Sets the run-time state of a cheat.  If the cheat is disabled and has no hotkeys, its record is removed.
		 * 
		 * \param _acState The state to set.  ui32Idx selects the cheat.
		 * \return Returns false if the index is invalid or there was a memory failure.
		 **/
		bool										SetActiveCheat( const LSN_ACTIVE_CHEAT &_acState );

		/**
		 * Determines whether a cheat is enabled.
		 * 
		 * \param _ui32Idx The extended index of the cheat.
		 * \return Returns true if the cheat is enabled.
		 **/
		bool										IsEnabled( uint32_t _ui32Idx ) const;

		/**
		 * Gets a copy of the run-time state of every cheat that has one.  Safe to call from any thread.
		 * 
		 * \return Returns the run-time states of all enabled cheats and cheats with hotkeys.
		 **/
		std::vector<LSN_ACTIVE_CHEAT>				GatherActiveCheats() const;

		/**
		 * Returns true if the given index goes to a custom cheat.
		 * 
		 * \param _ui32Idx The extended index.
		 * \return Returns true if the top bit is set.
		 **/
		static inline bool							IsCustom( uint32_t _ui32Idx ) {
			return (_ui32Idx & LSN_CHEAT_MAN_CUSTOM_FLAG) != 0;
		}

		/**
		 * Gets the slot in the custom-cheat table of a custom cheat.
		 * 
		 * \param _ui32Idx The extended index of a custom cheat.
		 * \return Returns the index into the custom-cheat table.
		 **/
		static inline uint32_t						CustomSlot( uint32_t _ui32Idx ) {
			return _ui32Idx & (LSN_CHEAT_MAN_CUSTOM_FLAG - 1);
		}

		/**
		 * Gets a pointer to the hotkey manager used by this object.
		 * 
		 * \return Returns a pointer to our hotkey manager.
		 **/
		inline CHotkeyManager *						HotkeyManager() { return m_phmHotkeyManager; }


	protected :
		// == Types.
		/** A slot in the custom-cheat table. */
		struct LSN_CUSTOM_CHEAT {
			LSN_CHEAT_ENTRY							ceEntry;						/**< The cheat. */
			bool									bInUse = false;					/**< False if the cheat was deleted and the slot can be reused. */
		};


		// == Members.
		std::vector<LSN_CHEAT_ENTRY>				m_vBuiltInCheats;				/**< Built-in cheats.  Read-only after InitializeCheatLibrary(). */
		std::vector<LSN_CUSTOM_CHEAT>				m_vCustomCheats;				/**< Custom (user-defined) cheats, indexed by CustomSlot(). */
		std::wstring								m_wsFilter;						/**< The cheat filter. */
		/** The run-time state (enabled state and hotkeys) of cheats, keyed by extended index.  Only cheats that are enabled or have a hotkey
		 *	are in it, and it doubles as the lookup that tells the Cheats dialog which of the listed cheats are enabled. */
		std::unordered_map<uint32_t, LSN_ACTIVE_CHEAT>
													m_mActiveCheats;
		/** Guards m_vCustomCheats and m_mActiveCheats.  The UI thread is the only writer; other threads (such as the emulation thread applying
		 *	cheats) read through the locking functions (GatherActiveCheats() etc.). */
		mutable std::shared_mutex					m_smtxCheats;
		CHotkeyManager *							m_phmHotkeyManager = nullptr;	/**< A pointer to the hotkey manager. */


		// == Functions.
		/**
		 * Loads the built-in cheats from a binary cache.
		 * 
		 * \param _pCachePath The path to the cache.
		 * \param _ui64ZipSize The size of the .ZIP file the cache must match, or 0 to accept the cache whatever it was made from.
		 * \param _i64ZipTime The last-write time of the .ZIP file the cache must match (ignored if _ui64ZipSize is 0).
		 * \return Returns true if the cache was valid and loaded into m_vBuiltInCheats.
		 **/
		bool										LoadCheatCache( const std::filesystem::path &_pCachePath, uint64_t _ui64ZipSize, int64_t _i64ZipTime );

		/**
		 * Saves the built-in cheats to a binary cache.
		 * 
		 * \param _pCachePath The path to the cache.
		 * \param _ui64ZipSize The size of the .ZIP file the cheats came from.
		 * \param _i64ZipTime The last-write time of the .ZIP file the cheats came from.
		 * \return Returns true if the cache was written.
		 **/
		bool										SaveCheatCache( const std::filesystem::path &_pCachePath, uint64_t _ui64ZipSize, int64_t _i64ZipTime ) const;
	};

}	// namespace lsn

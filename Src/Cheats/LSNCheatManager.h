/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The global manager of cheats.
 */


#pragma once


#include "../LSNLSpiroNes.h"
#include "../Bus/LSNBus.h"
#include "../Utilities/LSNHotkeyManager.h"
#include "LSNCheatConverter.h"
#include "LSNCheatEntry.h"

#include <Helpers/LSWHelpers.h>

#include <atomic>
#include <filesystem>
#include <memory>
#include <mutex>
#include <set>
#include <shared_mutex>
#include <type_traits>
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
			bool									bEnabled = false;			/**< Whether the cheat is enabled (its trampolines are on the bus while its ROM runs).  Hotkeys do not change this. */
			bool									bHotkeyOn = true;			/**< Whether an enabled cheat is currently in effect.  Only hotkeys change this; it is reset to true when the cheat is enabled. */
			uint32_t								ui32RomCrc = 0;				/**< The CRC of the ROM for which the cheat was enabled (0 = not yet bound to a ROM).  An enabled cheat is only applied while that ROM runs. */
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
		 * Loads all of the built-in cheats given a path to the .ZIP file, then the custom (user) cheats from the file next to it (see
		 *	CustomCheatsPath()).  The parsed built-in cheats are cached in a binary file next to the .ZIP file (the same name with the
		 *	extension .lsch), which is loaded instead of the .ZIP file as long as it matches the size and time of the .ZIP file.
		 * 
		 * \param _pPath Path to the .ZIP file full of .JSON files full of cheats to load.
		 * \param _phmHotkeyManager A pointer to a hotkey manager to be used by this object.
		 * \return Returns an error string to present to the user or an empty string on success.
		 **/
		std::wstring								InitializeCheatLibrary( const std::filesystem::path &_pPath, CHotkeyManager * _phmHotkeyManager );

		/**
		 * Gets the path of the file that holds the custom (user) cheats:  CustomCheats.lscc next to the built-in cheats.
		 * 
		 * \return Returns the path of the custom-cheats file, or an empty path before InitializeCheatLibrary() is called.
		 **/
		inline const std::filesystem::path &		CustomCheatsPath() const { return m_pCustomPath; }

		/**
		 * Saves the custom cheats (with their hotkeys) to CustomCheatsPath() if they changed since they were loaded or last saved.
		 *	CheatsChanged() calls this.
		 * 
		 * \return Returns true if the cheats were saved or did not need to be saved.
		 **/
		bool										SaveCustomCheats();

		/**
		 * Gets the search string of the Cheats dialog (kept here so that it survives closing the dialog).
		 * 
		 * \return Returns a constant reference to the search string as the user typed it.
		 **/
		const std::wstring &						GetCheatSearch() const { return m_wsSearch; }

		/**
		 * Sets the search string of the Cheats dialog.
		 * 
		 * \param _wsSearch The search string as the user typed it.
		 **/
		void										SetCheatSearch( const std::wstring &_wsSearch ) { m_wsSearch = _wsSearch; }

		/**
		 * Gets the folder from which cheats were last imported.
		 *
		 * \return Returns the folder from which cheats were last imported.
		 **/
		const std::wstring &						LastImportFolder() const { return m_wsImportFolder; }

		/**
		 * Sets the folder from which cheats were last imported.
		 *
		 * \param _wsFolder The folder.
		 **/
		void										SetLastImportFolder( const std::wstring &_wsFolder ) { m_wsImportFolder = _wsFolder; }

		/**
		 * Gets the folder to which cheats were last exported.
		 *
		 * \return Returns the folder to which cheats were last exported.
		 **/
		const std::wstring &						LastExportFolder() const { return m_wsExportFolder; }

		/**
		 * Sets the folder to which cheats were last exported.
		 *
		 * \param _wsFolder The folder.
		 **/
		void										SetLastExportFolder( const std::wstring &_wsFolder ) { m_wsExportFolder = _wsFolder; }

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
		 * Sets the run-time state of a cheat.  If the cheat is disabled and has no hotkeys, its record is removed.  Enabling a cheat binds
		 *	it to the running ROM and puts it in effect.  ui32RomCrc and bHotkeyOn are managed here and ignored in _acState.  A cheat whose
		 *	addresses are used by another enabled cheat is stored disabled (see FindConflict()).
		 * 
		 * \param _acState The state to set.  ui32Idx selects the cheat.
		 * \return Returns false if the index is invalid, there was a memory failure, or the cheat could not be enabled because of a conflict.
		 **/
		bool										SetActiveCheat( const LSN_ACTIVE_CHEAT &_acState );

		/**
		 * Removes the hotkeys of cheats, leaving their enabled states alone.  A cheat that a hotkey had turned off is put back in effect,
		 *	since it no longer has a key to turn it on.  Call CheatsChanged() afterwards.
		 * 
		 * \param _vIdx The extended indices of the cheats.
		 * \return Returns the number of cheats that had hotkeys.
		 **/
		size_t										ClearHotkeys( const std::vector<uint32_t> &_vIdx );

		/**
		 * Sets whether cheats are enabled, leaving their hotkeys alone.  A cheat whose addresses are used by another enabled cheat is not
		 *	enabled.
		 * 
		 * \param _vIdx The extended indices of the cheats.
		 * \param _bEnabled Whether to enable or disable them.
		 * \param _vConflicts Holds the indices of the cheats that could not be enabled because of a conflict.
		 **/
		void										SetEnabled( const std::vector<uint32_t> &_vIdx, bool _bEnabled, std::vector<uint32_t> &_vConflicts );

		/**
		 * Disables every enabled cheat (for any ROM), leaving their hotkeys alone.  Call CheatsChanged() afterwards.
		 * 
		 * \return Returns the number of cheats that were disabled.
		 **/
		size_t										DisableAll();

		/**
		 * Determines whether any cheat is enabled (for any ROM).
		 * 
		 * \return Returns true if at least one cheat is enabled.
		 **/
		bool										AnyEnabled() const;

		/**
		 * Counts the cheats that are enabled for a ROM other than the running one.  These are the cheats that would come back when that ROM
		 *	is loaded again (see ResolveOtherRomCheats()).
		 * 
		 * \return Returns the number of cheats enabled for another ROM, or 0 if no ROM is running.
		 **/
		size_t										CountEnabledForOtherRoms() const;

		/**
		 * Decides what happens to the cheats that are enabled for a ROM other than the running one (after switching ROMs).  They are either
		 *	moved to the running ROM (and put in effect) or disabled.  A moved cheat that uses an address of a cheat already enabled for the
		 *	running ROM (or of a cheat moved before it) is disabled instead.  Call CheatsChanged() afterwards.
		 * 
		 * \param _bKeep If true, the cheats are enabled for the running ROM, otherwise they are disabled.
		 * \param _vConflicts Holds the indices of the cheats that could not be kept because of a conflict.
		 **/
		void										ResolveOtherRomCheats( bool _bKeep, std::vector<uint32_t> &_vConflicts );

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

		/**
		 * Sets the ROM that is about to run (call before the ROM is loaded into the system) or clears it (_ui32Crc = 0).  Enabled cheats that
		 *	are not yet bound to a ROM are bound to this one.  Only cheats enabled for the running ROM are applied.
		 * 
		 * \param _ui32Crc The CRC of the ROM, or 0 if no ROM is running.
		 * \param _u16sName The name of the ROM (its file name; the extension is removed).  New cheats are added to this game.
		 **/
		void										SetRunningRom( uint32_t _ui32Crc, const std::u16string &_u16sName );

		/**
		 * Gets the name of the game that is running (see SetRunningRom()).
		 * 
		 * \return Returns the name of the running game or an empty string if no ROM is running.
		 **/
		inline const std::u16string &				RunningGame() const { return m_u16sRunningGame; }

		/**
		 * Finds an enabled cheat (applied to the running ROM) that uses one of the addresses of a given cheat.  No two applied cheats may
		 *	use the same address.
		 * 
		 * \param _ui32Idx The extended index of the cheat to check (it is skipped when found among the enabled cheats).
		 * \param _ceEntry The codes to check.
		 * \param _ui32OtherIdx Holds the extended index of the conflicting cheat.
		 * \param _ui16Address Holds the shared address.
		 * \return Returns true if a conflicting cheat was found.
		 **/
		bool										FindConflict( uint32_t _ui32Idx, const LSN_CHEAT_ENTRY &_ceEntry, uint32_t &_ui32OtherIdx, uint16_t &_ui16Address ) const;

		/**
		 * Informs the manager that cheats were enabled, disabled, edited, added, or deleted.  Schedules the trampolines to be updated on the
		 *	emulation thread (see ProcessPendingSync()), updates the registered hotkeys, and saves the custom cheats if they changed.  Call from
		 *	the UI thread.
		 **/
		void										CheatsChanged();

		/**
		 * Determines whether the trampolines on the bus need to be updated.  Called by the emulation thread between ticks.
		 * 
		 * \return Returns true if ProcessPendingSync() needs to be called.
		 **/
		inline bool									SyncPending() const { return m_abSyncRequested.load( std::memory_order_relaxed ); }

		/**
		 * Updates the trampolines on the bus to match the enabled cheats.  Call only from the emulation thread (between ticks) or while the
		 *	emulation thread is stopped.
		 **/
		void										ProcessPendingSync();

		/**
		 * Removes every cheat trampoline from the bus.  Call when the ROM is closed (before the mapper is destroyed), from the UI thread
		 *	with the emulation thread stopped.
		 **/
		void										RemoveFromBus();

		/**
		 * The CSystemBase bus-mapped callback (see CSystemBase::SetBusMappedCallback()).  Remapping the bus removed every trampoline, so
		 *	they are inserted again.
		 * 
		 * \param _pvParm Pointer to the CCheatManager object.
		 * \param _pbBus The CPU bus.
		 **/
		static void									BusMapped( void * _pvParm, CCpuBus * _pbBus );

		/**
		 * Registers the hotkeys of the cheats applied to the running ROM, replacing those registered before.  Call from the UI thread (hotkeys
		 *	belong to the thread that owns the window).
		 **/
		void										UpdateHotkeys();


	protected :
		// == Types.
		/**
		 * One cheat code inserted into the bus.  The trampoline function receives a pointer to tTrampoline (the first member, so it is also a
		 *	pointer to this object).  tTrampoline.pvReaderParm0 points to the CCheatManager and the 16-bit parameter of the trampoline function is
		 *	the index of this object in m_vTrampolines, but everything the read needs is copied here so that it costs no extra indirection.
		 *	These are allocated individually and never move while they are on the bus.
		 **/
		struct LSN_CHEAT_TRAMPOLINE {
			CCpuBus::LSN_TRAMPOLINE					tTrampoline;					/**< The bus trampoline.  Must be first. */
			std::atomic<bool>						abActive = true;				/**< Whether the code is in effect (hotkeys change this without removing the trampoline). */
			bool									bUseCompare = false;			/**< Whether the value is only replaced when it matches ui8Compare. */
			uint8_t									ui8Value = 0;					/**< The value returned instead of the real value. */
			uint8_t									ui8Compare = 0;					/**< The value the real value must match for ui8Value to be returned. */
			uint16_t								ui16Address = 0;				/**< The address of the code. */
			uint32_t								ui32CheatIdx = 0;				/**< The extended index of the cheat this code belongs to. */
		};
		static_assert( std::is_standard_layout_v<LSN_CHEAT_TRAMPOLINE>, "LSN_CHEAT_TRAMPOLINE must be standard-layout so that a pointer to tTrampoline is a pointer to it." );

		/** A code to apply, gathered from the enabled cheats. */
		struct LSN_PENDING_CODE {
			uint32_t								ui32CheatIdx;					/**< The extended index of the cheat. */
			uint16_t								ui16Address;					/**< The address. */
			uint8_t									ui8Value;						/**< The value. */
			uint8_t									ui8Compare;						/**< The compare value. */
			bool									bUseCompare;					/**< Whether the compare value is used. */
			bool									bActive;						/**< The hotkey state of the cheat. */
		};


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
		/** The cheat codes on the bus.  Each is allocated separately so that it stays at the same address while it is on the bus. */
		std::vector<std::unique_ptr<LSN_CHEAT_TRAMPOLINE>>
													m_vTrampolines;
		std::mutex									m_mtxTrampolines;				/**< Guards m_vTrampolines (the hotkey handler on the UI thread walks it while the emulation thread may rebuild it). */
		CCpuBus *									m_pbBus = nullptr;				/**< The bus on which the trampolines are inserted. */
		std::atomic<bool>							m_abSyncRequested = false;		/**< Set when the trampolines need to be rebuilt (see ProcessPendingSync()). */
		std::atomic<uint32_t>						m_ui32RomCrc = 0;				/**< The CRC of the running ROM, or 0. */
		std::u16string								m_u16sRunningGame;				/**< The name of the running game (without extension). */
		std::vector<int32_t>						m_vHotkeyIds;					/**< The IDs of the hotkeys registered for cheats. */
		std::wstring								m_wsSearch;						/**< The search string of the Cheats dialog. */
		std::wstring								m_wsImportFolder;				/**< The folder from which cheats were last imported. */
		std::wstring								m_wsExportFolder;				/**< The folder to which cheats were last exported. */
		std::filesystem::path						m_pCustomPath;					/**< The path of the custom-cheats file. */
		std::vector<uint8_t>						m_vLastSavedCustom;				/**< The contents of the custom-cheats file as last loaded or saved (to skip writing it when nothing changed). */


		// == Functions.
		/**
		 * The trampoline read function for cheat codes.  Reads through to the original read function (reads can have side effects and the
		 *	real value is needed for compare codes), then substitutes the cheat value if the code is in effect.
		 * 
		 * \param _pvParm0 Pointer to the LSN_CHEAT_TRAMPOLINE (via its CCpuBus::LSN_TRAMPOLINE).
		 * \param _ui16Parm1 The index of the LSN_CHEAT_TRAMPOLINE in m_vTrampolines.
		 * \param _pui8Data The buffer from which to read.
		 * \param _ui8Ret The read value.
		 **/
		static void LSN_FASTCALL					CheatRead_Trampoline( void * _pvParm0, uint16_t _ui16Parm1, uint8_t * _pui8Data, uint8_t &_ui8Ret );

		/**
		 * Determines whether an active-cheat record is applied to the running ROM.
		 * 
		 * \param _acCheat The record to check.
		 * \return Returns true if the cheat is enabled for the running ROM.
		 **/
		inline bool									AppliesNow( const LSN_ACTIVE_CHEAT &_acCheat ) const {
			uint32_t ui32Crc = m_ui32RomCrc.load( std::memory_order_relaxed );
			return _acCheat.bEnabled && ui32Crc != 0 && _acCheat.ui32RomCrc == ui32Crc;
		}

		/**
		 * Determines whether a record shows as enabled:  it applies to the running ROM, or no ROM is running and it is enabled.
		 * 
		 * \param _acCheat The record to check.
		 * \return Returns true if the cheat should be shown as enabled.
		 **/
		inline bool									ShowsEnabled( const LSN_ACTIVE_CHEAT &_acCheat ) const {
			uint32_t ui32Crc = m_ui32RomCrc.load( std::memory_order_relaxed );
			return _acCheat.bEnabled && (ui32Crc == 0 || _acCheat.ui32RomCrc == 0 || _acCheat.ui32RomCrc == ui32Crc);
		}

		/**
		 * Gets a cheat by extended index with no locking, or nullptr if the index is invalid.  The caller must hold m_smtxCheats.
		 * 
		 * \param _ui32Idx The extended index.
		 * \return Returns a pointer to the cheat or nullptr.
		 **/
		const LSN_CHEAT_ENTRY *						CheatByIdx_NoLock( uint32_t _ui32Idx ) const;

		/**
		 * Rebuilds the trampolines on the bus to match the cheats applied to the running ROM.
		 * 
		 * \param _bFreshMap If true, the bus was just remapped, so the old trampolines are no longer on it and are not removed from it.
		 **/
		void										SyncToBus( bool _bFreshMap );

		/**
		 * The hotkey callback.
		 * 
		 * \param _pvParm0 Pointer to the CCheatManager object.
		 * \param _ui64Parm1 The packed key (see PackKey()).
		 * \param _ui32Parm2 Unused.
		 * \return Returns true to keep the hotkey.
		 **/
		static bool									HotkeyFunc( void * _pvParm0, uint64_t _ui64Parm1, uint32_t _ui32Parm2 );

		/**
		 * Handles a cheat hotkey:  updates the in-effect state of every applied cheat that uses the key.  Several cheats can share a key.
		 * 
		 * \param _ui64Key The packed key (see PackKey()).
		 **/
		void										HandleHotkey( uint64_t _ui64Key );

#ifdef LSN_USE_WINDOWS
		/**
		 * Packs a key into 64 bits for comparison and for passing to the hotkey callback.
		 * 
		 * \param _kKey The key to pack.
		 * \return Returns the packed key, or 0 if no key is assigned.
		 **/
		static inline uint64_t						PackKey( const lsw::LSW_KEY &_kKey ) {
			if ( !_kKey.bKeyCode ) { return 0; }
			return (uint64_t( _kKey.dwScanCode ) << 16) | (uint64_t( _kKey.bKeyModifier ) << 8) | _kKey.bKeyCode;
		}
#endif	// #ifdef LSN_USE_WINDOWS


		// == Functions.
		/**
		 * Loads all of the built-in cheats given a path to the .ZIP file (or the cache next to it).
		 * 
		 * \param _pPath Path to the .ZIP file full of .JSON files full of cheats to load.
		 * \return Returns an error string to present to the user or an empty string on success.
		 **/
		std::wstring								LoadBuiltInCheats( const std::filesystem::path &_pPath );

		/**
		 * Loads the custom cheats (and their hotkeys) from CustomCheatsPath(), replacing any custom cheats.  A damaged file is renamed (with
		 *	".bad" added) so that saving does not overwrite it.
		 * 
		 * \return Returns true if the file was loaded or does not exist.
		 **/
		bool										LoadCustomCheats();

		/**
		 * Creates the contents of the custom-cheats file.
		 * 
		 * \param _vBuffer Holds the returned file contents.
		 * \return Returns true if there was enough memory.
		 **/
		bool										SerializeCustomCheats( std::vector<uint8_t> &_vBuffer ) const;

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

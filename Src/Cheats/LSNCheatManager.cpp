/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The global manager of cheats.
 */

#include "LSNCheatManager.h"
#include "../File/LSNStdFile.h"
#include "../File/LSNZipFile.h"
#include "../Localization/LSNLocalization.h"

#include <cstring>
#include <filesystem>
#include <mutex>

namespace lsn {

	// == Functions.
	/**
	 * Loads all of the built-in cheats given a path to the .ZIP file.
	 * 
	 * \param _pPath Path to the .ZIP file full of .JSON files full of cheats to load.
	 * \param _phmHotkeyManager A pointer to a hotkey manager to be used by this object.
	 * \return Returns an error string to present to the user or an empty string on success.
	 **/
	std::wstring CCheatManager::InitializeCheatLibrary( const std::filesystem::path &_pPath, CHotkeyManager * _phmHotkeyManager ) {
		DestroyCheatLibrary();
		m_phmHotkeyManager = _phmHotkeyManager;

		// The binary cache is much faster to load than the .JSON files.
		std::filesystem::path pCache = _pPath;
		pCache.replace_extension( L".lsch" );
		uint64_t ui64ZipSize = 0;
		int64_t i64ZipTime = 0;
		{
			std::error_code ecErr;
			ui64ZipSize = uint64_t( std::filesystem::file_size( _pPath, ecErr ) );
			if ( ecErr ) { ui64ZipSize = 0; }
			else {
				auto ftTime = std::filesystem::last_write_time( _pPath, ecErr );
				i64ZipTime = ecErr ? 0 : int64_t( ftTime.time_since_epoch().count() );
			}
		}
		// Without the .ZIP file, any cache is better than nothing.
		if ( LoadCheatCache( pCache, ui64ZipSize, i64ZipTime ) ) { return std::wstring(); }
		DestroyCheatLibrary();

		CZipFile zfFile;
		if ( !zfFile.Open( _pPath.generic_u16string().c_str() ) ) { return LSN_LSTR( LSN_FAILED_TO_OPEN_FILE ); }
		std::vector<std::u16string> vFiles;
		std::wstring wsError;
		try {
			if ( zfFile.GatherArchiveFiles( vFiles ) ) {
				std::vector<std::u16string> vFinalFiles;
				for ( size_t I = 0; I < vFiles.size(); ++I ) {
					std::vector<uint8_t> vExtracted;

					if ( !zfFile.ExtractToMemory( vFiles[I], vExtracted ) ) {
						if ( wsError.size() ) { wsError += L"\r\n"; }
						wsError += LSN_LSTR( LSN_FAILED_TO_EXTRACT_FILE );
						wsError += L": ";
						wsError += CUtilities::XStringToWString( vFiles[I].c_str(), vFiles[I].size() );
					}
					else {
						vExtracted.push_back( 0 );
						vExtracted.push_back( 0 );
						std::wstring wsTmp;
						if ( !CCheatConverter::LoadMesenJson( vExtracted, m_vBuiltInCheats, vFiles[I], wsTmp ) ) {
							if ( wsError.size() ) { wsError += L"\r\n"; }
							wsError += CUtilities::XStringToWString( vFiles[I].c_str(), vFiles[I].size() );
							wsError += L" ";
							wsError += LSN_LSTR( LSN_ERROR );
							wsError += L": ";
							wsError += wsTmp;
						}
					}
				}
			}
		}
		catch ( ... ) {
			return LSN_LSTR( LSN_OUT_OF_MEMORY );
		}
		if ( m_vBuiltInCheats.size() ) {
			// Failure only means the next start-up parses the .ZIP file again (the folder may be read-only).
			SaveCheatCache( pCache, ui64ZipSize, i64ZipTime );
		}
		return wsError;
	}

	/** The magic number of the cheat cache ("LSCH"). */
	static constexpr uint32_t						LSN_CHEAT_CACHE_MAGIC = 0x4843534CU;
	/** The version of the cheat cache.  Change it whenever LSN_CHEAT_ENTRY or the file layout changes. */
	static constexpr uint32_t						LSN_CHEAT_CACHE_VERSION = 1;

	/**
	 * Class CCheatCacheReader
	 * \brief Reads the cheat cache from memory.
	 *
	 * Description: A minimal bounds-checked reader of the cheat cache.
	 */
	class CCheatCacheReader {
	public :
		CCheatCacheReader( const std::vector<uint8_t> &_vData ) :
			m_vData( _vData ) {
		}


		// == Functions.
		/**
		 * Reads a plain value.
		 * 
		 * \param _tVal Holds the read value.
		 * \return Returns true if there was enough data.
		 **/
		template <typename _tType>
		bool										Read( _tType &_tVal ) {
			if ( m_vData.size() - m_stPos < sizeof( _tType ) ) { return false; }
			std::memcpy( &_tVal, m_vData.data() + m_stPos, sizeof( _tType ) );
			m_stPos += sizeof( _tType );
			return true;
		}

		/**
		 * Reads a string (a 32-bit length then its characters).
		 * 
		 * \param _tStr Holds the read string.
		 * \return Returns true if there was enough data.
		 **/
		template <typename _tType>
		bool										ReadStr( _tType &_tStr ) {
			uint32_t ui32Len;
			if ( !Read( ui32Len ) ) { return false; }
			const size_t stBytes = size_t( ui32Len ) * sizeof( typename _tType::value_type );
			if ( m_vData.size() - m_stPos < stBytes ) { return false; }
			_tStr.resize( ui32Len );
			if ( stBytes ) { std::memcpy( _tStr.data(), m_vData.data() + m_stPos, stBytes ); }
			m_stPos += stBytes;
			return true;
		}


	protected :
		// == Members.
		const std::vector<uint8_t> &				m_vData;									/**< The data. */
		size_t										m_stPos = 0;								/**< The read position. */
	};

	/**
	 * Appends a plain value to a buffer.
	 * 
	 * \param _vBuffer The buffer.
	 * \param _tVal The value to append.
	 **/
	template <typename _tType>
	static void										CheatCacheWrite( std::vector<uint8_t> &_vBuffer, const _tType &_tVal ) {
		const uint8_t * pui8Src = reinterpret_cast<const uint8_t *>(&_tVal);
		_vBuffer.insert( _vBuffer.end(), pui8Src, pui8Src + sizeof( _tType ) );
	}

	/**
	 * Appends a string (a 32-bit length then its characters) to a buffer.
	 * 
	 * \param _vBuffer The buffer.
	 * \param _tStr The string to append.
	 **/
	template <typename _tType>
	static void										CheatCacheWriteStr( std::vector<uint8_t> &_vBuffer, const _tType &_tStr ) {
		CheatCacheWrite( _vBuffer, uint32_t( _tStr.size() ) );
		const uint8_t * pui8Src = reinterpret_cast<const uint8_t *>(_tStr.data());
		_vBuffer.insert( _vBuffer.end(), pui8Src, pui8Src + _tStr.size() * sizeof( typename _tType::value_type ) );
	}

	/**
	 * Loads the built-in cheats from a binary cache.
	 * 
	 * \param _pCachePath The path to the cache.
	 * \param _ui64ZipSize The size of the .ZIP file the cache must match, or 0 to accept the cache whatever it was made from.
	 * \param _i64ZipTime The last-write time of the .ZIP file the cache must match (ignored if _ui64ZipSize is 0).
	 * \return Returns true if the cache was valid and loaded into m_vBuiltInCheats.
	 **/
	bool CCheatManager::LoadCheatCache( const std::filesystem::path &_pCachePath, uint64_t _ui64ZipSize, int64_t _i64ZipTime ) {
		std::vector<uint8_t> vFile;
		if ( !CStdFile::LoadToMemory( _pCachePath.native().c_str(), vFile ) ) { return false; }
		try {
			CCheatCacheReader ccrReader( vFile );
			uint32_t ui32Magic, ui32Version, ui32CharSize, ui32Total;
			uint64_t ui64Size;
			int64_t i64Time;
			if ( !ccrReader.Read( ui32Magic ) || ui32Magic != LSN_CHEAT_CACHE_MAGIC ) { return false; }
			if ( !ccrReader.Read( ui32Version ) || ui32Version != LSN_CHEAT_CACHE_VERSION ) { return false; }
			if ( !ccrReader.Read( ui32CharSize ) || ui32CharSize != sizeof( wchar_t ) ) { return false; }
			if ( !ccrReader.Read( ui64Size ) || !ccrReader.Read( i64Time ) ) { return false; }
			if ( _ui64ZipSize && (ui64Size != _ui64ZipSize || i64Time != _i64ZipTime) ) { return false; }
			if ( !ccrReader.Read( ui32Total ) ) { return false; }
			// Every entry takes at least 19 bytes; a larger count means the file is damaged.
			if ( size_t( ui32Total ) > vFile.size() / 19 ) { return false; }

			std::vector<LSN_CHEAT_ENTRY> vCheats;
			vCheats.resize( ui32Total );
			for ( auto & ceThis : vCheats ) {
				uint16_t ui16Type;
				uint8_t ui8Enabled;
				uint32_t ui32Addresses;
				if ( !ccrReader.ReadStr( ceThis.wsDescription ) || !ccrReader.ReadStr( ceThis.wsNotes ) || !ccrReader.ReadStr( ceThis.u16sFile ) ) { return false; }
				if ( !ccrReader.Read( ui16Type ) || !ccrReader.Read( ui8Enabled ) || !ccrReader.Read( ui32Addresses ) ) { return false; }
				if ( ui16Type > LSN_CHEAT_ENTRY::LSN_CT_CUSTOM || ui32Addresses > 0x10000 ) { return false; }
				ceThis.ctType = LSN_CHEAT_ENTRY::LSN_CHEAT_TYPE( ui16Type );
				ceThis.bEnabled = ui8Enabled != 0;
				ceThis.vAddresses.resize( ui32Addresses );
				for ( auto & aThis : ceThis.vAddresses ) {
					uint8_t ui8UseCompare;
					if ( !ccrReader.ReadStr( aThis.sCode ) || !ccrReader.Read( aThis.ui16Address ) || !ccrReader.Read( aThis.ui8Value ) ||
						!ccrReader.Read( aThis.ui8CompareValue ) || !ccrReader.Read( ui8UseCompare ) ) { return false; }
					aThis.bUseCompare = ui8UseCompare != 0;
				}
			}
			m_vBuiltInCheats = std::move( vCheats );
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Saves the built-in cheats to a binary cache.
	 * 
	 * \param _pCachePath The path to the cache.
	 * \param _ui64ZipSize The size of the .ZIP file the cheats came from.
	 * \param _i64ZipTime The last-write time of the .ZIP file the cheats came from.
	 * \return Returns true if the cache was written.
	 **/
	bool CCheatManager::SaveCheatCache( const std::filesystem::path &_pCachePath, uint64_t _ui64ZipSize, int64_t _i64ZipTime ) const {
		try {
			std::vector<uint8_t> vBuffer;
			vBuffer.reserve( m_vBuiltInCheats.size() * 160 );
			CheatCacheWrite( vBuffer, LSN_CHEAT_CACHE_MAGIC );
			CheatCacheWrite( vBuffer, LSN_CHEAT_CACHE_VERSION );
			CheatCacheWrite( vBuffer, uint32_t( sizeof( wchar_t ) ) );
			CheatCacheWrite( vBuffer, _ui64ZipSize );
			CheatCacheWrite( vBuffer, _i64ZipTime );
			CheatCacheWrite( vBuffer, uint32_t( m_vBuiltInCheats.size() ) );
			for ( const auto & ceThis : m_vBuiltInCheats ) {
				CheatCacheWriteStr( vBuffer, ceThis.wsDescription );
				CheatCacheWriteStr( vBuffer, ceThis.wsNotes );
				CheatCacheWriteStr( vBuffer, ceThis.u16sFile );
				CheatCacheWrite( vBuffer, uint16_t( ceThis.ctType ) );
				CheatCacheWrite( vBuffer, uint8_t( ceThis.bEnabled ? 1 : 0 ) );
				CheatCacheWrite( vBuffer, uint32_t( ceThis.vAddresses.size() ) );
				for ( const auto & aThis : ceThis.vAddresses ) {
					CheatCacheWriteStr( vBuffer, aThis.sCode );
					CheatCacheWrite( vBuffer, aThis.ui16Address );
					CheatCacheWrite( vBuffer, aThis.ui8Value );
					CheatCacheWrite( vBuffer, aThis.ui8CompareValue );
					CheatCacheWrite( vBuffer, uint8_t( aThis.bUseCompare ? 1 : 0 ) );
				}
			}
			// Write to a temporary file and rename it so a partial cache is never left behind.
			std::filesystem::path pTmp = _pCachePath;
			pTmp += L".tmp";
			if ( !CStdFile::WriteToFile( pTmp.native().c_str(), vBuffer ) ) { return false; }
			std::error_code ecErr;
			std::filesystem::rename( pTmp, _pCachePath, ecErr );
			if ( ecErr ) {
				std::filesystem::remove( pTmp, ecErr );
				return false;
			}
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Shuts down the cheat manager at the end of the application life cycle.
	 **/
	void CCheatManager::DestroyCheatLibrary() {
		std::unique_lock<std::shared_mutex> ulLock( m_smtxCheats );
		m_vBuiltInCheats = std::vector<LSN_CHEAT_ENTRY>();
		// The indices of built-in cheats are about to mean something else.
		for ( auto I = m_mActiveCheats.begin(); I != m_mActiveCheats.end(); ) {
			if ( !IsCustom( I->first ) ) { I = m_mActiveCheats.erase( I ); }
			else { ++I; }
		}
	}

	/**
	 * Determines whether an extended index refers to an existing cheat.
	 * 
	 * \param _ui32Idx The extended index to check.
	 * \return Returns true if the index refers to a built-in cheat or a custom cheat that has not been deleted.
	 **/
	bool CCheatManager::IsValidIdx( uint32_t _ui32Idx ) const {
		if ( IsCustom( _ui32Idx ) ) {
			std::shared_lock<std::shared_mutex> slLock( m_smtxCheats );
			uint32_t ui32Slot = CustomSlot( _ui32Idx );
			return ui32Slot < m_vCustomCheats.size() && m_vCustomCheats[ui32Slot].bInUse;
		}
		return _ui32Idx < m_vBuiltInCheats.size();
	}

	/**
	 * Adds a custom (user-defined) cheat.
	 * 
	 * \param _ceEntry The cheat to add.
	 * \param _pacActive Optional run-time state (enabled state and hotkeys) for the cheat.  Its ui32Idx is ignored.
	 * \return Returns the extended index of the new cheat or LSN_CHEAT_MAN_INVALID_IDX on failure.
	 **/
	uint32_t CCheatManager::AddCustomCheat( const LSN_CHEAT_ENTRY &_ceEntry, const LSN_ACTIVE_CHEAT * _pacActive ) {
		uint32_t ui32Idx = LSN_CHEAT_MAN_INVALID_IDX;
		try {
			std::unique_lock<std::shared_mutex> ulLock( m_smtxCheats );
			// Reuse a deleted slot if there is one.
			size_t stSlot = m_vCustomCheats.size();
			for ( size_t I = 0; I < m_vCustomCheats.size(); ++I ) {
				if ( !m_vCustomCheats[I].bInUse ) {
					stSlot = I;
					break;
				}
			}
			if ( stSlot >= LSN_CHEAT_MAN_CUSTOM_FLAG ) { return LSN_CHEAT_MAN_INVALID_IDX; }
			if ( stSlot == m_vCustomCheats.size() ) { m_vCustomCheats.emplace_back(); }
			m_vCustomCheats[stSlot].ceEntry = _ceEntry;
			m_vCustomCheats[stSlot].bInUse = true;
			ui32Idx = uint32_t( stSlot ) | LSN_CHEAT_MAN_CUSTOM_FLAG;
			m_mActiveCheats.erase( ui32Idx );
		}
		catch ( ... ) { return LSN_CHEAT_MAN_INVALID_IDX; }
		if ( _pacActive ) {
			LSN_ACTIVE_CHEAT acTmp = (*_pacActive);
			acTmp.ui32Idx = ui32Idx;
			SetActiveCheat( acTmp );
		}
		return ui32Idx;
	}

	/**
	 * Replaces a custom cheat.  Built-in cheats cannot be changed.
	 * 
	 * \param _ui32Idx The extended index of the custom cheat to replace.
	 * \param _ceEntry The new cheat data.
	 * \param _pacActive Optional new run-time state (enabled state and hotkeys) for the cheat.  Its ui32Idx is ignored.
	 * \return Returns true if _ui32Idx refers to an existing custom cheat and it was replaced.
	 **/
	bool CCheatManager::UpdateCustomCheat( uint32_t _ui32Idx, const LSN_CHEAT_ENTRY &_ceEntry, const LSN_ACTIVE_CHEAT * _pacActive ) {
		if ( !IsCustom( _ui32Idx ) ) { return false; }
		try {
			std::unique_lock<std::shared_mutex> ulLock( m_smtxCheats );
			uint32_t ui32Slot = CustomSlot( _ui32Idx );
			if ( ui32Slot >= m_vCustomCheats.size() || !m_vCustomCheats[ui32Slot].bInUse ) { return false; }
			m_vCustomCheats[ui32Slot].ceEntry = _ceEntry;
		}
		catch ( ... ) { return false; }
		// Even with the same state, the new codes may now conflict with another enabled cheat.  A conflict leaves the cheat stored but
		//	disabled; callers can tell by checking IsEnabled().
		LSN_ACTIVE_CHEAT acTmp;
		if ( _pacActive ) { acTmp = (*_pacActive); }
		else { GetActiveCheat( _ui32Idx, acTmp ); }
		acTmp.ui32Idx = _ui32Idx;
		SetActiveCheat( acTmp );
		return true;
	}

	/**
	 * Deletes a custom cheat and its run-time state.  Built-in cheats cannot be deleted.  The extended indices of the other cheats do not
	 *	change; the slot is reused by a later AddCustomCheat().
	 * 
	 * \param _ui32Idx The extended index of the custom cheat to delete.
	 * \return Returns true if _ui32Idx referred to an existing custom cheat.
	 **/
	bool CCheatManager::DeleteCustomCheat( uint32_t _ui32Idx ) {
		if ( !IsCustom( _ui32Idx ) ) { return false; }
		std::unique_lock<std::shared_mutex> ulLock( m_smtxCheats );
		uint32_t ui32Slot = CustomSlot( _ui32Idx );
		if ( ui32Slot >= m_vCustomCheats.size() || !m_vCustomCheats[ui32Slot].bInUse ) { return false; }
		m_vCustomCheats[ui32Slot].bInUse = false;
		m_vCustomCheats[ui32Slot].ceEntry = LSN_CHEAT_ENTRY();
		m_mActiveCheats.erase( _ui32Idx );
		// Trailing free slots can go.
		while ( m_vCustomCheats.size() && !m_vCustomCheats.back().bInUse ) {
			m_vCustomCheats.pop_back();
		}
		return true;
	}

	/**
	 * Gets the run-time state of a cheat.
	 * 
	 * \param _ui32Idx The extended index of the cheat.
	 * \param _acRet Holds the returned state.  If the cheat has none, it is filled with the defaults (disabled, no hotkeys).
	 * \return Returns true if the cheat has run-time state (it is enabled or has a hotkey).
	 **/
	bool CCheatManager::GetActiveCheat( uint32_t _ui32Idx, LSN_ACTIVE_CHEAT &_acRet ) const {
		std::shared_lock<std::shared_mutex> slLock( m_smtxCheats );
		auto aFound = m_mActiveCheats.find( _ui32Idx );
		if ( aFound == m_mActiveCheats.end() ) {
			_acRet = LSN_ACTIVE_CHEAT();
			_acRet.ui32Idx = _ui32Idx;
			return false;
		}
		_acRet = aFound->second;
		// A cheat enabled for another ROM is not enabled here.
		_acRet.bEnabled = ShowsEnabled( aFound->second );
		return true;
	}

	/**
	 * Sets the run-time state of a cheat.  If the cheat is disabled and has no hotkeys, its record is removed.  Enabling a cheat binds
	 *	it to the running ROM and puts it in effect.  ui32RomCrc and bHotkeyOn are managed here and ignored in _acState.  A cheat whose
	 *	addresses are used by another enabled cheat is stored disabled (see FindConflict()).
	 * 
	 * \param _acState The state to set.  ui32Idx selects the cheat.
	 * \return Returns false if the index is invalid, there was a memory failure, or the cheat could not be enabled because of a conflict.
	 **/
	bool CCheatManager::SetActiveCheat( const LSN_ACTIVE_CHEAT &_acState ) {
		if ( !IsValidIdx( _acState.ui32Idx ) ) { return false; }
		try {
			std::unique_lock<std::shared_mutex> ulLock( m_smtxCheats );
			LSN_ACTIVE_CHEAT acNew = _acState;
			auto aFound = m_mActiveCheats.find( _acState.ui32Idx );
			bool bWasShown = aFound != m_mActiveCheats.end() && ShowsEnabled( aFound->second );
			if ( acNew.bEnabled && bWasShown ) {
				// Still enabled:  keep its ROM and whether a hotkey has turned it off.
				acNew.ui32RomCrc = aFound->second.ui32RomCrc ? aFound->second.ui32RomCrc : m_ui32RomCrc.load();
				acNew.bHotkeyOn = aFound->second.bHotkeyOn;
			}
			else {
				acNew.ui32RomCrc = acNew.bEnabled ? m_ui32RomCrc.load() : (aFound != m_mActiveCheats.end() ? aFound->second.ui32RomCrc : 0);
				acNew.bHotkeyOn = true;
			}

			bool bRet = true;
			if ( acNew.bEnabled ) {
				// No two applied cheats may use the same address.
				const LSN_CHEAT_ENTRY * pceEntry = CheatByIdx_NoLock( acNew.ui32Idx );
				if ( pceEntry ) {
					for ( const auto & aOther : m_mActiveCheats ) {
						if ( aOther.first == acNew.ui32Idx || !aOther.second.bEnabled || aOther.second.ui32RomCrc != acNew.ui32RomCrc ) { continue; }
						const LSN_CHEAT_ENTRY * pceOther = CheatByIdx_NoLock( aOther.first );
						if ( !pceOther ) { continue; }
						for ( const auto & aMine : pceEntry->vAddresses ) {
							for ( const auto & aTheirs : pceOther->vAddresses ) {
								if ( aMine.ui16Address == aTheirs.ui16Address ) {
									acNew.bEnabled = false;
									bRet = false;
								}
							}
						}
					}
				}
			}

			if ( !acNew.IsNeeded() ) {
				m_mActiveCheats.erase( acNew.ui32Idx );
			}
			else {
				m_mActiveCheats[acNew.ui32Idx] = acNew;
			}
			return bRet;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Sets whether cheats are enabled, leaving their hotkeys alone.  A cheat whose addresses are used by another enabled cheat is not
	 *	enabled.
	 * 
	 * \param _vIdx The extended indices of the cheats.
	 * \param _bEnabled Whether to enable or disable them.
	 * \param _vConflicts Holds the indices of the cheats that could not be enabled because of a conflict.
	 **/
	void CCheatManager::SetEnabled( const std::vector<uint32_t> &_vIdx, bool _bEnabled, std::vector<uint32_t> &_vConflicts ) {
		_vConflicts.clear();
		for ( auto ui32Idx : _vIdx ) {
			LSN_ACTIVE_CHEAT acState;
			GetActiveCheat( ui32Idx, acState );
			if ( acState.bEnabled == _bEnabled ) { continue; }
			acState.bEnabled = _bEnabled;
			if ( !SetActiveCheat( acState ) && _bEnabled ) {
				try { _vConflicts.push_back( ui32Idx ); }
				catch ( ... ) {}
			}
		}
	}

	/**
	 * Sets the ROM that is about to run (call before the ROM is loaded into the system) or clears it (_ui32Crc = 0).  Enabled cheats that
	 *	are not yet bound to a ROM are bound to this one.  Only cheats enabled for the running ROM are applied.
	 * 
	 * \param _ui32Crc The CRC of the ROM, or 0 if no ROM is running.
	 * \param _u16sName The name of the ROM (its file name; the extension is removed).  New cheats are added to this game.
	 **/
	void CCheatManager::SetRunningRom( uint32_t _ui32Crc, const std::u16string &_u16sName ) {
		std::unique_lock<std::shared_mutex> ulLock( m_smtxCheats );
		m_ui32RomCrc = _ui32Crc;
		try {
			m_u16sRunningGame = _ui32Crc ? std::filesystem::path( _u16sName ).replace_extension( u"" ).generic_u16string() : std::u16string();
		}
		catch ( ... ) { m_u16sRunningGame.clear(); }
		if ( _ui32Crc ) {
			for ( auto & aThis : m_mActiveCheats ) {
				if ( aThis.second.bEnabled && aThis.second.ui32RomCrc == 0 ) {
					aThis.second.ui32RomCrc = _ui32Crc;
					aThis.second.bHotkeyOn = true;
				}
			}
		}
	}

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
	bool CCheatManager::FindConflict( uint32_t _ui32Idx, const LSN_CHEAT_ENTRY &_ceEntry, uint32_t &_ui32OtherIdx, uint16_t &_ui16Address ) const {
		std::shared_lock<std::shared_mutex> slLock( m_smtxCheats );
		uint32_t ui32Crc = m_ui32RomCrc.load();
		for ( const auto & aOther : m_mActiveCheats ) {
			if ( aOther.first == _ui32Idx || !aOther.second.bEnabled ) { continue; }
			// Compare against the cheats that are (or will be) applied together with this one.
			uint32_t ui32OtherCrc = aOther.second.ui32RomCrc ? aOther.second.ui32RomCrc : ui32Crc;
			if ( ui32OtherCrc != ui32Crc ) { continue; }
			const LSN_CHEAT_ENTRY * pceOther = CheatByIdx_NoLock( aOther.first );
			if ( !pceOther ) { continue; }
			for ( const auto & aMine : _ceEntry.vAddresses ) {
				for ( const auto & aTheirs : pceOther->vAddresses ) {
					if ( aMine.ui16Address == aTheirs.ui16Address ) {
						_ui32OtherIdx = aOther.first;
						_ui16Address = aMine.ui16Address;
						return true;
					}
				}
			}
		}
		return false;
	}

	/**
	 * Gets a cheat by extended index with no locking, or nullptr if the index is invalid.  The caller must hold m_smtxCheats.
	 * 
	 * \param _ui32Idx The extended index.
	 * \return Returns a pointer to the cheat or nullptr.
	 **/
	const LSN_CHEAT_ENTRY * CCheatManager::CheatByIdx_NoLock( uint32_t _ui32Idx ) const {
		if ( IsCustom( _ui32Idx ) ) {
			uint32_t ui32Slot = CustomSlot( _ui32Idx );
			if ( ui32Slot < m_vCustomCheats.size() && m_vCustomCheats[ui32Slot].bInUse ) { return &m_vCustomCheats[ui32Slot].ceEntry; }
			return nullptr;
		}
		return _ui32Idx < m_vBuiltInCheats.size() ? &m_vBuiltInCheats[_ui32Idx] : nullptr;
	}

	/**
	 * Informs the manager that cheats were enabled, disabled, edited, added, or deleted.  Schedules the trampolines to be updated on the
	 *	emulation thread (see ProcessPendingSync()) and updates the registered hotkeys.  Call from the UI thread.
	 **/
	void CCheatManager::CheatsChanged() {
		m_abSyncRequested.store( true );
		UpdateHotkeys();
	}

	/**
	 * Updates the trampolines on the bus to match the enabled cheats.  Call only from the emulation thread (between ticks) or while the
	 *	emulation thread is stopped.
	 **/
	void CCheatManager::ProcessPendingSync() {
		SyncToBus( false );
	}

	/**
	 * Removes every cheat trampoline from the bus.  Call when the ROM is closed (before the mapper is destroyed), from the UI thread
	 *	with the emulation thread stopped.
	 **/
	void CCheatManager::RemoveFromBus() {
		std::lock_guard<std::mutex> lgLock( m_mtxTrampolines );
		if ( m_pbBus ) {
			// Reverse order:  only the outermost trampoline at an address can be removed.
			for ( auto I = m_vTrampolines.size(); I--; ) {
				m_pbBus->RemoveTrampolineReadFunc( m_vTrampolines[I]->ui16Address, &m_vTrampolines[I]->tTrampoline );
			}
		}
		m_vTrampolines.clear();
	}

	/**
	 * The CSystemBase bus-mapped callback (see CSystemBase::SetBusMappedCallback()).  Remapping the bus removed every trampoline, so
	 *	they are inserted again.
	 * 
	 * \param _pvParm Pointer to the CCheatManager object.
	 * \param _pbBus The CPU bus.
	 **/
	void CCheatManager::BusMapped( void * _pvParm, CCpuBus * _pbBus ) {
		CCheatManager * pcmThis = reinterpret_cast<CCheatManager *>(_pvParm);
		pcmThis->m_pbBus = _pbBus;
		pcmThis->SyncToBus( true );
	}

	/**
	 * Rebuilds the trampolines on the bus to match the cheats applied to the running ROM.
	 * 
	 * \param _bFreshMap If true, the bus was just remapped, so the old trampolines are no longer on it and are not removed from it.
	 **/
	void CCheatManager::SyncToBus( bool _bFreshMap ) {
		m_abSyncRequested.store( false );

		// Gather the codes first so that m_smtxCheats and m_mtxTrampolines are never held together.
		std::vector<LSN_PENDING_CODE> vCodes;
		try {
			std::shared_lock<std::shared_mutex> slLock( m_smtxCheats );
			for ( const auto & aThis : m_mActiveCheats ) {
				if ( !AppliesNow( aThis.second ) ) { continue; }
				const LSN_CHEAT_ENTRY * pceEntry = CheatByIdx_NoLock( aThis.first );
				if ( !pceEntry ) { continue; }
				for ( const auto & aAddr : pceEntry->vAddresses ) {
					LSN_PENDING_CODE pcCode = {
						.ui32CheatIdx = aThis.first,
						.ui16Address = aAddr.ui16Address,
						.ui8Value = aAddr.ui8Value,
						.ui8Compare = aAddr.ui8CompareValue,
						.bUseCompare = aAddr.bUseCompare,
						.bActive = aThis.second.bHotkeyOn,
					};
					vCodes.push_back( pcCode );
				}
			}
		}
		catch ( ... ) { vCodes.clear(); }

		std::lock_guard<std::mutex> lgLock( m_mtxTrampolines );
		if ( !_bFreshMap && m_pbBus ) {
			// Reverse order:  only the outermost trampoline at an address can be removed.
			for ( auto I = m_vTrampolines.size(); I--; ) {
				m_pbBus->RemoveTrampolineReadFunc( m_vTrampolines[I]->ui16Address, &m_vTrampolines[I]->tTrampoline );
			}
		}
		m_vTrampolines.clear();
		if ( !m_pbBus ) { return; }

		try {
			// Never 2 codes on 1 address (enabling a cheat already refuses conflicts; this also covers repeats within one cheat).
			std::vector<bool> vUsed( 0x10000 );
			for ( const auto & pcCode : vCodes ) {
				if ( vUsed[pcCode.ui16Address] ) { continue; }
				if ( m_vTrampolines.size() >= 0xFFFF ) { break; }
				vUsed[pcCode.ui16Address] = true;

				auto pctTramp = std::make_unique<LSN_CHEAT_TRAMPOLINE>();
				pctTramp->abActive.store( pcCode.bActive );
				pctTramp->bUseCompare = pcCode.bUseCompare;
				pctTramp->ui8Value = pcCode.ui8Value;
				pctTramp->ui8Compare = pcCode.ui8Compare;
				pctTramp->ui16Address = pcCode.ui16Address;
				pctTramp->ui32CheatIdx = pcCode.ui32CheatIdx;
				// pvReaderParm0 = this manager; the 16-bit parameter = which code (index into m_vTrampolines).
				m_pbBus->SetTrampolineReadFunc( pcCode.ui16Address, &CCheatManager::CheatRead_Trampoline, this, uint16_t( m_vTrampolines.size() ), &pctTramp->tTrampoline );
				m_vTrampolines.push_back( std::move( pctTramp ) );
			}
		}
		catch ( ... ) {}
	}

	/**
	 * The trampoline read function for cheat codes.  Reads through to the original read function (reads can have side effects and the
	 *	real value is needed for compare codes), then substitutes the cheat value if the code is in effect.
	 * 
	 * \param _pvParm0 Pointer to the LSN_CHEAT_TRAMPOLINE (via its CCpuBus::LSN_TRAMPOLINE).
	 * \param _ui16Parm1 The index of the LSN_CHEAT_TRAMPOLINE in m_vTrampolines.
	 * \param _pui8Data The buffer from which to read.
	 * \param _ui8Ret The read value.
	 **/
	void LSN_FASTCALL CCheatManager::CheatRead_Trampoline( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * _pui8Data, uint8_t &_ui8Ret ) {
		const CCpuBus::LSN_TRAMPOLINE * ptTramp = reinterpret_cast<const CCpuBus::LSN_TRAMPOLINE *>(_pvParm0);
		// Always read:  the read may have side effects, and compare codes need the real value.
		ptTramp->aaOriginalFuncs.pfReader( ptTramp->aaOriginalFuncs.pvReaderParm0, ptTramp->aaOriginalFuncs.ui16ReaderParm1, _pui8Data, _ui8Ret );

		// tTrampoline is the first member, so this is also the LSN_CHEAT_TRAMPOLINE.
		const LSN_CHEAT_TRAMPOLINE * pctCheat = reinterpret_cast<const LSN_CHEAT_TRAMPOLINE *>(ptTramp);
		if LSN_LIKELY( pctCheat->abActive.load( std::memory_order_relaxed ) ) {
			if LSN_LIKELY( !pctCheat->bUseCompare ) {
				_ui8Ret = pctCheat->ui8Value;
			}
			else if ( _ui8Ret == pctCheat->ui8Compare ) {
				_ui8Ret = pctCheat->ui8Value;
			}
		}
	}

	/**
	 * Registers the hotkeys of the cheats applied to the running ROM, replacing those registered before.  Call from the UI thread (hotkeys
	 *	belong to the thread that owns the window).
	 **/
	void CCheatManager::UpdateHotkeys() {
#ifdef LSN_USE_WINDOWS
		if ( !m_phmHotkeyManager ) { return; }
		for ( auto i32Id : m_vHotkeyIds ) {
			m_phmHotkeyManager->DeleteHotkey( i32Id );
		}
		m_vHotkeyIds.clear();

		try {
			// Each distinct key is registered once; its handler updates every cheat that uses it.
			std::vector<lsw::LSW_KEY> vKeys;
			{
				std::shared_lock<std::shared_mutex> slLock( m_smtxCheats );
				for ( const auto & aThis : m_mActiveCheats ) {
					if ( !AppliesNow( aThis.second ) ) { continue; }
					const lsw::LSW_KEY * pkKeys[2] = { &aThis.second.kToggleOrEnable, aThis.second.bToggle ? nullptr : &aThis.second.kDisable };
					for ( auto pkKey : pkKeys ) {
						if ( !pkKey || !PackKey( (*pkKey) ) ) { continue; }
						bool bHave = false;
						for ( const auto & kHave : vKeys ) {
							if ( PackKey( kHave ) == PackKey( (*pkKey) ) ) {
								bHave = true;
								break;
							}
						}
						if ( !bHave ) { vKeys.push_back( (*pkKey) ); }
					}
				}
			}
			for ( const auto & kThis : vKeys ) {
				int32_t i32Id = m_phmHotkeyManager->AddHotkey( kThis.bKeyCode, kThis.bKeyModifier, kThis.dwScanCode, &CCheatManager::HotkeyFunc, this, PackKey( kThis ), 0 );
				if ( i32Id ) { m_vHotkeyIds.push_back( i32Id ); }
			}
		}
		catch ( ... ) {}
#endif	// #ifdef LSN_USE_WINDOWS
	}

	/**
	 * The hotkey callback.
	 * 
	 * \param _pvParm0 Pointer to the CCheatManager object.
	 * \param _ui64Parm1 The packed key (see PackKey()).
	 * \param _ui32Parm2 Unused.
	 * \return Returns true to keep the hotkey.
	 **/
	bool CCheatManager::HotkeyFunc( void * _pvParm0, uint64_t _ui64Parm1, uint32_t /*_ui32Parm2*/ ) {
		reinterpret_cast<CCheatManager *>(_pvParm0)->HandleHotkey( _ui64Parm1 );
		return true;
	}

	/**
	 * Handles a cheat hotkey:  updates the in-effect state of every applied cheat that uses the key.  Several cheats can share a key.
	 * 
	 * \param _ui64Key The packed key (see PackKey()).
	 **/
	void CCheatManager::HandleHotkey( uint64_t _ui64Key ) {
#ifdef LSN_USE_WINDOWS
		std::vector<std::pair<uint32_t, bool>> vChanged;
		try {
			std::unique_lock<std::shared_mutex> ulLock( m_smtxCheats );
			for ( auto & aThis : m_mActiveCheats ) {
				auto & acCheat = aThis.second;
				if ( !AppliesNow( acCheat ) ) { continue; }
				bool bOn = acCheat.bHotkeyOn;
				if ( acCheat.bToggle ) {
					if ( PackKey( acCheat.kToggleOrEnable ) == _ui64Key ) { bOn = !bOn; }
				}
				else {
					if ( PackKey( acCheat.kToggleOrEnable ) == _ui64Key ) { bOn = true; }
					else if ( PackKey( acCheat.kDisable ) == _ui64Key ) { bOn = false; }
				}
				if ( bOn != acCheat.bHotkeyOn ) {
					acCheat.bHotkeyOn = bOn;
					vChanged.push_back( { aThis.first, bOn } );
				}
			}
		}
		catch ( ... ) {}
		if ( vChanged.empty() ) { return; }

		// Only the flags change; the trampolines stay on the bus.
		std::lock_guard<std::mutex> lgLock( m_mtxTrampolines );
		for ( auto & pctThis : m_vTrampolines ) {
			for ( const auto & aChange : vChanged ) {
				if ( pctThis->ui32CheatIdx == aChange.first ) {
					pctThis->abActive.store( aChange.second, std::memory_order_relaxed );
				}
			}
		}
#else
		(void)_ui64Key;
#endif	// #ifdef LSN_USE_WINDOWS
	}

	/**
	 * Determines whether a cheat is enabled.
	 * 
	 * \param _ui32Idx The extended index of the cheat.
	 * \return Returns true if the cheat is enabled.
	 **/
	bool CCheatManager::IsEnabled( uint32_t _ui32Idx ) const {
		std::shared_lock<std::shared_mutex> slLock( m_smtxCheats );
		auto aFound = m_mActiveCheats.find( _ui32Idx );
		return aFound != m_mActiveCheats.end() && ShowsEnabled( aFound->second );
	}

	/**
	 * Gets a copy of the run-time state of every cheat that has one.  Safe to call from any thread.
	 * 
	 * \return Returns the run-time states of all enabled cheats and cheats with hotkeys.
	 **/
	std::vector<CCheatManager::LSN_ACTIVE_CHEAT> CCheatManager::GatherActiveCheats() const {
		std::vector<LSN_ACTIVE_CHEAT> vRet;
		try {
			std::shared_lock<std::shared_mutex> slLock( m_smtxCheats );
			vRet.reserve( m_mActiveCheats.size() );
			for ( const auto & aThis : m_mActiveCheats ) {
				vRet.push_back( aThis.second );
			}
		}
		catch ( ... ) {}
		return vRet;
	}

	/**
	 * Auto-sets the cheat filter given a game name.
	 * 
	 * \param _u16sName The name of the game for which to auto-set the filter.
	 * \return Returns the previous cheat filter.
	 **/
	std::wstring CCheatManager::AutoSetCheatFilter( const std::u16string &_u16sName ) {
		auto wsTmp = std::filesystem::path( _u16sName ).replace_extension( L"" ).generic_wstring();
		auto wsOriginalFilter = SetCheatFilter( wsTmp );
		if ( CountUnfiltered() == 1 ) { return wsOriginalFilter; }
		
		auto wsTmpCopy = wsTmp;

		// If we go this far, we can get fatal mismatches (IE "8 Eyes (Japan)" could be loaded but this would match with "8 Eyes (USA)" and the cheats will crash the game).
		// So after this point we can only accept 2 or more matches.  This allows the game names to be shown in the Cheats window.
		auto iFound = std::find( wsTmp.begin(), wsTmp.end(), L'(' );
		if ( iFound != wsTmp.end() ) {
			wsTmp = wsTmp.substr( 0, iFound - wsTmp.begin() );
		}
		while ( wsTmp.size() && std::iswspace( wsTmp[wsTmp.size()-1] ) ) {
			wsTmp.pop_back();
		}
		SetCheatFilter( wsTmp );
		while ( !MoreThanOneUnfiltered() && wsTmp.size() ) {
			wsTmp.pop_back();
			SetCheatFilter( wsTmp );
		}
		if ( !MoreThanOneUnfiltered() ) {
			SetCheatFilter( wsTmpCopy );
		}
		return wsOriginalFilter;
	}

	/**
	 * Determines how many games remain through the current filter.  If the filter is empty, all games pass.
	 * 
	 * \return Returns the number of games that are unfiltered via SetCheatFilter().
	 **/
	size_t CCheatManager::CountUnfiltered() const {
		return GatherGamesWithFilter().size();
	}

	/**
	 * Determines if there is more than 1 game in the unfiltered set.
	 * 
	 * \return Returns true if there is more than 1 game in the unfiltered set.
	 **/
	bool CCheatManager::MoreThanOneUnfiltered() const {
		std::set<std::u16string> sCnt;
		try {
			std::wstring wsCompareMe = CUtilities::ToLower( m_wsFilter );
			for ( size_t I = 0; I < m_vBuiltInCheats.size(); ++I ) {
				if ( wsCompareMe.size() == 0 || CUtilities::StrICmp_RightIsLowered( CUtilities::XStringToWString( m_vBuiltInCheats[I].u16sFile.c_str(), m_vBuiltInCheats[I].u16sFile.size() ), wsCompareMe, wsCompareMe.size() ) ) {
					sCnt.insert( m_vBuiltInCheats[I].u16sFile );
					if ( sCnt.size() > 1 ) { return true; }
				}
			}
			std::shared_lock<std::shared_mutex> slLock( m_smtxCheats );
			for ( size_t I = 0; I < m_vCustomCheats.size(); ++I ) {
				if ( !m_vCustomCheats[I].bInUse ) { continue; }
				const auto & u16sFile = m_vCustomCheats[I].ceEntry.u16sFile;
				if ( wsCompareMe.size() == 0 || CUtilities::StrICmp_RightIsLowered( CUtilities::XStringToWString( u16sFile.c_str(), u16sFile.size() ), wsCompareMe, wsCompareMe.size() ) ) {
					sCnt.insert( u16sFile );
					if ( sCnt.size() > 1 ) { return true; }
				}
			}
		}
		catch ( ... ) {}
		return false;
	}

	/**
	 * Gathers all of the names of the games that match the filter string.
	 * 
	 * \return Returns all names of games that pass the filter.
	 **/
	std::set<std::u16string> CCheatManager::GatherGamesWithFilter() const {
		std::set<std::u16string> sCnt;
		try {
			std::wstring wsCompareMe = CUtilities::ToLower( m_wsFilter );
			for ( size_t I = 0; I < m_vBuiltInCheats.size(); ++I ) {
				std::wstring wsTmp = CUtilities::XStringToWString( m_vBuiltInCheats[I].u16sFile.c_str(), m_vBuiltInCheats[I].u16sFile.size() );
				if ( wsCompareMe.size() == 0 || CUtilities::StrICmp_RightIsLowered( wsTmp, wsCompareMe, wsCompareMe.size() ) ) {
					sCnt.insert( m_vBuiltInCheats[I].u16sFile );
				}
			}
			std::shared_lock<std::shared_mutex> slLock( m_smtxCheats );
			for ( size_t I = 0; I < m_vCustomCheats.size(); ++I ) {
				if ( !m_vCustomCheats[I].bInUse ) { continue; }
				const auto & u16sFile = m_vCustomCheats[I].ceEntry.u16sFile;
				std::wstring wsTmp = CUtilities::XStringToWString( u16sFile.c_str(), u16sFile.size() );
				if ( wsCompareMe.size() == 0 || CUtilities::StrICmp_RightIsLowered( wsTmp, wsCompareMe, wsCompareMe.size() ) ) {
					sCnt.insert( u16sFile );
				}
			}
		}
		catch ( ... ) {}
		return sCnt;
	}

	/**
	 * Gathers all of the cheats for a given game.
	 * 
	 * \param _u16sGame The name of the game whose cheats are to be gathered.
	 * \return Returns a vector of indices into the built-in and custom cheats.  If the high bit is set, the cheat is part of the custom set.
	 **/
	std::vector<uint32_t> CCheatManager::GatherCheatsForGame( const std::u16string &_u16sGame ) const {
		std::vector<uint32_t> vTmp;
		try {
			// Built-ins.
			for ( size_t I = 0; I < m_vBuiltInCheats.size(); ++I ) {
				if ( m_vBuiltInCheats[I].u16sFile == _u16sGame ) {
					vTmp.push_back( uint32_t( I ) );
				}
			}
			// Customs.
			std::shared_lock<std::shared_mutex> slLock( m_smtxCheats );
			for ( size_t I = 0; I < m_vCustomCheats.size(); ++I ) {
				if ( m_vCustomCheats[I].bInUse && m_vCustomCheats[I].ceEntry.u16sFile == _u16sGame ) {
					vTmp.push_back( uint32_t( I ) | LSN_CHEAT_MAN_CUSTOM_FLAG );
				}
			}
		}
		catch ( ... ) {}
		return vTmp;
	}

}	// namespace lsn

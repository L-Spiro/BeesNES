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
		m_vBuiltInCheats = std::vector<LSN_CHEAT_ENTRY>();
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
		std::set<std::u16string> sCnt;
		try {
			std::wstring wsCompareMe = CUtilities::ToLower( m_wsFilter );
			for ( size_t I = 0; I < m_vBuiltInCheats.size(); ++I ) {
				if ( wsCompareMe.size() == 0 || CUtilities::StrICmp_RightIsLowered( CUtilities::XStringToWString( m_vBuiltInCheats[I].u16sFile.c_str(), m_vBuiltInCheats[I].u16sFile.size() ), wsCompareMe, wsCompareMe.size() ) ) {
					sCnt.insert( m_vBuiltInCheats[I].u16sFile );
				}
			}
		}
		catch ( ... ) {}
		return sCnt.size();
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
			/*
			for ( size_t I = 0; I < m_vBuiltInCheats.size(); ++I ) {
				if ( m_vBuiltInCheats[I].u16sFile == _u16sGame ) {
					vTmp.push_back( uint32_t( I ) );
				}
			}*/
		}
		catch ( ... ) {}
		return vTmp;
	}

}	// namespace lsn

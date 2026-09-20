/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The global manager of cheats.
 */

#include "LSNCheatManager.h"
#include "../File/LSNZipFile.h"
#include "../Localization/LSNLocalization.h"

namespace lsn {

	// == Functions.
	/**
	 * Loads all of the built-in cheats given a path to the .ZIP file.
	 * 
	 * \param _pPath Path to the .ZIP file full of .JSON files full of cheats to load.
	 * \return Returns an error string to present to the user or an empty string on success.
	 **/
	std::wstring CCheatManager::InitializeCheatLibrary( const std::filesystem::path &_pPath ) {
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
		return wsError;
	}

	/**
	 * Shuts down the cheat manager at the end of the application life cycle.
	 **/
	void CCheatManager::DestroyCheatLibrary() {
		m_vBuiltInCheats = std::vector<LSN_CHEAT_ENTRY>();
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

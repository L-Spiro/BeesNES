/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for working with files.
 */


#include "LSNFileBase.h"
#include "../OS/LSNOs.h"

#ifndef LSN_WINDOWS
#include <filesystem>
#endif	// #ifndef LSN_WINDOWS

namespace lsn {

	CFileBase::~CFileBase() {}

	// == Functions.
	/**
	 * Closes the opened file.
	 */
	void CFileBase::Close() {}

	/**
	 * If true, the file is an archive containing more files.
	 *
	 * \return Returns true if the file is an archive, false otherwise.
	 */
	bool CFileBase::IsArchive() const { return false; }

	/**
	 * Loads the opened file to memory, storing the result in _vResult.
	 *
	 * \param _vResult The location where to store the file in memory.
	 * \return Returns true if the file was successfully loaded into memory.
	 */
	bool CFileBase::LoadToMemory( std::vector<uint8_t> &/*_vResult*/ ) const { return false; }

	/**
	 * Gathers the file names in the archive into an array.
	 *
	 * \param _vResult The location where to store the file names.
	 * \return Returns true if the file names were successfully added to the given array.  A return of false will typically indicate that the file is not opened or that it is not a valid .ZIP file.
	 */
	bool CFileBase::GatherArchiveFiles( std::vector<std::u16string> &/*_vResult*/ ) const { return false; }

	/**
	 * Gathers the file names in the archive into an array.
	 *
	 * \param _s16File The name of the file to extract.
	 * \param _vResult The location where to store the file in memory.
	 * \return Returns true if the file was extracted successfully.
	 */
	bool CFileBase::ExtractToMemory( const std::u16string &/*_s16File*/, std::vector<uint8_t> &/*_vResult*/ ) const { return false; }

	/**
	 * Finds files/folders in a given directory.
	 * 
	 * \param _pcFolderPath The path to the directory to search.
	 * \param _pcSearchString A wildcard search string to find only certain files/folders.
	 * \param _bIncludeFolders If true, folders are included in the return.
	 * \param _vResult The return array.  Found files and folders are appended to the array.
	 * \return Returns _vResult.
	 **/
	std::vector<std::u16string> & CFileBase::FindFiles( const char16_t * _pcFolderPath, const char16_t * _pcSearchString, bool _bIncludeFolders, std::vector<std::u16string> &_vResult ) {
#ifdef LSN_WINDOWS
		std::u16string sPath = CUtilities::Replace( std::u16string( _pcFolderPath ), u'/', u'\\' );
		while ( sPath.size() && sPath[sPath.size()-1] == u'\\' ) { sPath.pop_back(); }
		sPath.push_back( u'\\' );
		std::u16string sSearch;
		if ( _pcSearchString ) {
			sSearch = CUtilities::Replace( std::u16string( _pcSearchString ), u'/', u'\\' );
			while ( sSearch[0] == u'\\' ) {
				sSearch.erase( sSearch.begin() );
			}
		}
		else {
			sSearch = u"*";
		}


		std::u16string sSearchPath = sPath + sSearch;
		/*if ( sSearchPath.FindString( 0, L"\\\\?\\" ).ui32Start != 0 ) {
			if ( !sSearchPath.Insert( 0, L"\\\\?\\" ) ) { return false; }
		}*/
		WIN32_FIND_DATAW wfdData;
		HANDLE hDir = ::FindFirstFileW( reinterpret_cast<LPCWSTR>(sSearchPath.c_str()), &wfdData );
		if ( INVALID_HANDLE_VALUE == hDir ) { return _vResult; }
		
		do {
			if ( wfdData.cFileName[0] == u'.' ) { continue; }
			bool bIsFolder = ((wfdData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
			if ( !_bIncludeFolders && bIsFolder ) { continue; }
			try {
				_vResult.push_back( sPath + reinterpret_cast<const char16_t *>(wfdData.cFileName) );
			}
			catch ( ... ) {
				::FindClose( hDir );
				return _vResult;
			}
		} while ( ::FindNextFileW( hDir, &wfdData ) );

		::FindClose( hDir );
		return _vResult;
#else
		// Convert char16_t * to std::u16string.
		std::u16string sPath = _pcFolderPath;
		while ( sPath.size() && sPath.back() == u'\\' ) {
			sPath.pop_back();
		}
		sPath.push_back( u'/' );  // Use forward slash for UNIX-like path.
		
		std::u16string sSearch = _pcSearchString ? _pcSearchString : u"*";
		
		for ( const auto & entry : std::filesystem::directory_iterator( std::filesystem::path( sPath.begin(), sPath.end() ) ) ) {
			const auto & path = entry.path();
			bool isDirectory = entry.is_directory();

			if ( !_bIncludeFolders && isDirectory ) {
				continue;
			}

			std::u16string sFilename = path.filename().u16string();
			if ( sFilename[0] == u'.' ) {
				continue;  // Skip hidden files and directories
			}

			_vResult.push_back(std::u16string( sPath.begin(), sPath.end()) + sFilename );
		}

		return _vResult;

#endif	// #ifdef LSN_WINDOWS
	}

}	// namespace lsn

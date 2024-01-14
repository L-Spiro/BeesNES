/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for working with files.
 */


#include "LSNFileBase.h"
#include "../OS/LSNOs.h"
#include "../Utilities/LSNUtilities.h"

namespace lsn {

	CFileBase::~CFileBase() {}

	// == Functions.
	/**
	 * Opens a file.  The path is given in UTF-8.
	 *
	 * \param _pcPath Path to the file to open.
	 * \return Returns true if the file was opened, false otherwise.
	 */
	bool CFileBase::Open( const char8_t * _pcFile ) {
		bool bErrored;
		std::u16string swTmp = CUtilities::Utf8ToUtf16( _pcFile, &bErrored );
		if ( bErrored ) { return false; }
		return Open( swTmp.c_str() );
	}

	/**
	 * Opens a file.  The path is given in UTF-16.
	 *
	 * \param _pcPath Path to the file to open.
	 * \return Returns true if the file was opened, false otherwise.
	 */
	bool CFileBase::Open( const char16_t * _pcFile ) {
		bool bErrored;
		std::string sTmp = CUtilities::Utf16ToUtf8( _pcFile, &bErrored );
		if ( bErrored ) { return false; }
		return Open( reinterpret_cast<const char8_t *>(sTmp.c_str()) );
	}

	/**
	 * Creates a file.  The path is given in UTF-8.
	 *
	 * \param _pcPath Path to the file to create.
	 * \return Returns true if the file was created, false otherwise.
	 */
	bool CFileBase::Create( const char8_t * _pcFile ) {
		bool bErrored;
		std::u16string swTmp = CUtilities::Utf8ToUtf16( _pcFile, &bErrored );
		if ( bErrored ) { return false; }
		return Create( swTmp.c_str() );
	}

	/**
	 * Creates a file.  The path is given in UTF-16.
	 *
	 * \param _pcPath Path to the file to create.
	 * \return Returns true if the file was created, false otherwise.
	 */
	bool CFileBase::Create( const char16_t * _pcFile ) {
		bool bErrored;
		std::string sTmp = CUtilities::Utf16ToUtf8( _pcFile, &bErrored );
		if ( bErrored ) { return false; }
		return Create( reinterpret_cast<const char8_t *>(sTmp.c_str()) );
	}

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
		std::u16string sPath = CUtilities::Replace( _pcFolderPath, u'/', u'\\' );
		while ( sPath.size() && sPath[sPath.size()] == u'/' ) { sPath.pop_back(); }
		sPath.push_back( u'/' );
		std::u16string sSearch;
		if ( _pcSearchString ) {
			sSearch = CUtilities::Replace( _pcSearchString, u'/', u'\\' );
			while ( sSearch[0] == u'/' ) {
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
			if ( !_bIncludeFolders && bIsFolder ) {
				continue;
			}
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
#endif	// #ifdef LSN_WINDOWS
	}

}	// namespace lsn

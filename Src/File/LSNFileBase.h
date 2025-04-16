/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for working with files.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Utilities/LSNUtilities.h"

#include <string>
#include <vector>

namespace lsn {

	/**
	 * Class CFileBase
	 * \brief The base class for working with files.
	 *
	 * Description: The base class for working with files.
	 */
	class CFileBase {
	public :
		virtual ~CFileBase();


		// == Functions.
		/**
		 * Opens a file.  The path is given in UTF-8.
		 *
		 * \param _pcPath Path to the file to open.
		 * \return Returns true if the file was opened, false otherwise.
		 */
		virtual bool										Open( const char8_t * _pcFile );

		/**
		 * Opens a file.  The path is given in UTF-16.
		 *
		 * \param _pcPath Path to the file to open.
		 * \return Returns true if the file was opened, false otherwise.
		 */
		virtual bool										Open( const char16_t * _pcFile );

		/**
		 * Creates a file.  The path is given in UTF-8.
		 *
		 * \param _pcPath Path to the file to create.
		 * \return Returns true if the file was created, false otherwise.
		 */
		virtual bool										Create( const char8_t * _pcFile );

		/**
		 * Creates a file.  The path is given in UTF-16.
		 *
		 * \param _pcPath Path to the file to create.
		 * \return Returns true if the file was created, false otherwise.
		 */
		virtual bool										Create( const char16_t * _pcFile );

		/**
		 * Closes the opened file.
		 */
		virtual void										Close();

		/**
		 * If true, the file is an archive containing more files.
		 *
		 * \return Returns true if the file is an archive, false otherwise.
		 */
		virtual bool										IsArchive() const;

		/**
		 * Loads the opened file to memory, storing the result in _vResult.
		 *
		 * \param _vResult The location where to store the file in memory.
		 * \return Returns true if the file was successfully loaded into memory.
		 */
		virtual bool										LoadToMemory( std::vector<uint8_t> &_vResult ) const;

		/**
		 * Reads from the file.
		 * 
		 * \param _pvDst The destination for the read.  Must be sized appropriately to contain _sSize bytes.
		 * \param _sSize The number of bytes to read.
		 * \return Returns true if the read succeeded.  The file must be opened for read and the read operation must not extend beyond the end of the file.
		 **/
		virtual bool										Read( void * /*_pvDst*/, size_t /*_sSize*/ ) { return false; }

		/**
		 * Writes to the file.
		 * 
		 * \param _pvSrc The source for the write.
		 * \param _sSize The number of bytes to write.
		 * \return Returns true if the write succeeded.  The file must be opened for write and there must be enough room to extend the file size.
		 **/
		virtual bool										Write( const void * /*_pvSrc*/, size_t /*_sSize*/ ) { return false; }

		/**
		 * Gathers the file names in the archive into an array.
		 *
		 * \param _vResult The location where to store the file names.
		 * \return Returns true if the file names were successfully added to the given array.  A return of false will typically indicate that the file is not opened or that it is not a valid .ZIP file.
		 */
		virtual bool										GatherArchiveFiles( std::vector<std::u16string> &_vResult ) const;

		/**
		 * Gathers the file names in the archive into an array.
		 *
		 * \param _s16File The name of the file to extract.
		 * \param _vResult The location where to store the file in memory.
		 * \return Returns true if the file was extracted successfully.
		 */
		virtual bool										ExtractToMemory( const std::u16string &_s16File, std::vector<uint8_t> &_vResult ) const;

		/**
		 * Gets the size of the file.
		 * 
		 * \return Returns the size of the file.
		 **/
		virtual uint64_t									Size() const { return 0; }

		/**
		 * Gets the current position inside the file.
		 * 
		 * \return Returns the current position inside the file.
		 **/
		virtual uint64_t									GetPos() const { return 0; }

		/**
		 * Moves the file pointer from the current position and returns the new position.
		 * 
		 * \param _i64Offset Amount by which to move the file pointer.
		 * \return Returns the new line position.
		 **/
		virtual uint64_t									MovePointerBy( int64_t /*_i64Offset*/ ) const { return 0; }

		/**
		 * Moves the file pointer to the given file position.
		 * 
		 * \param _ui64Pos The new file position to set.
		 * \param _bFromEnd Whether _ui64Pos is from the end of the file or not. 
		 * \return Returns the new file position.
		 **/
		virtual uint64_t									MovePointerTo( uint64_t /*_ui64Pos*/, bool /*_bFromEnd*/ = false ) const { return 0; }

		/**
		 * Finds files/folders in a given directory.
		 * 
		 * \param _pcFolderPath The path to the directory to search.
		 * \param _pcSearchString A wildcard search string to find only certain files/folders.
		 * \param _bIncludeFolders If true, folders are included in the return.
		 * \param _vResult The return array.  Found files and folders are appended to the array.
		 * \return Returns _vResult.
		 **/
		static std::vector<std::u16string> &				FindFiles( const char16_t * _pcFolderPath, const char16_t * _pcSearchString, bool _bIncludeFolders, std::vector<std::u16string> &_vResult );

		/**
		 * Gets the extension from a file path.
		 *
		 * \param _s16Path The file path whose extension is to be obtained.
		 * \return Returns a string containing the file extension.
		 */
		template <typename _tType>
		static _tType										GetFileExtension( const _tType &_s16Path );

		/**
		 * Compares the extention from a given file path to a given extension string.
		 * 
		 * \param _s16Path The file path whose extension is to be checked.
		 * \param _pcExt The extension to check for being in _s16Path.
		 * \return Returns true if the given file path's extension matches _pcExt.
		 **/
		static inline bool									CmpFileExtension( const std::u16string &_s16Path, const char16_t * _pcExt );

		/**
		 * Removes the extension from a file path.
		 *
		 * \param _s16Path The file path whose extension is to be removed.
		 * \return Returns a string containing the file mname without the extension.
		 */
		template <typename _tType>
		static _tType										NoExtension( const _tType &_s16Path );

		/**
		 * Gets the file name from a file path.
		 *
		 * \param _s16Path The file path whose name is to be obtained.
		 * \return Returns a string containing the file name.
		 */
		template <typename _tType>
		static _tType										GetFileName( const _tType &_s16Path );

		/**
		 * Gets the file path without the file name
		 *
		 * \param _s16Path The file path whose path is to be obtained.
		 * \return Returns a string containing the file path.
		 */
		template <typename _tType>
		static _tType										GetFilePath( const _tType &_s16Path );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the extension from a file path.
	 *
	 * \param _s16Path The file path whose extension is to be obtained.
	 * \return Returns a string containing the file extension.
	 */
	template <typename _tType>
	inline _tType CFileBase::GetFileExtension( const _tType &_s16Path ) {
		_tType s16File = GetFileName( _s16Path );
		typename _tType::size_type stFound = s16File.rfind( typename _tType::value_type( '.' ) );
		if ( stFound == _tType::npos ) { return _tType(); }
		return s16File.substr( stFound + 1 );
	}

	/**
	 * Compares the extention from a given file path to a given extension string.
	 * 
	 * \param _s16Path The file path whose extension is to be checked.
	 * \param _pcExt The extension to check for being in _s16Path.
	 * \return Returns true if the given file path's extension matches _pcExt.
	 **/
	inline bool CFileBase::CmpFileExtension( const std::u16string &_s16Path, const char16_t * _pcExt ) {
		return ::_wcsicmp( reinterpret_cast<const wchar_t *>(lsn::CFileBase::GetFileExtension( _s16Path ).c_str()),
			reinterpret_cast<const wchar_t *>(_pcExt) ) == 0;
	}

	/**
	 * Removes the extension from a file path.
	 *
	 * \param _s16Path The file path whose extension is to be removed.
	 * \return Returns a string containing the file mname without the extension.
	 */
	template <typename _tType>
	inline _tType CFileBase::NoExtension( const _tType &_s16Path ) {
		_tType s16File = GetFileName( _s16Path );
		typename _tType::size_type stFound = s16File.rfind( typename _tType::value_type( '.' ) );
		if ( stFound == _tType::npos ) { return _tType(); }
		return s16File.substr( 0, stFound );
	}

	/**
	 * Gets the file name from a file path.
	 *
	 * \param _s16Path The file path whose name is to be obtained.
	 * \return Returns a string containing the file name.
	 */
	template <typename _tType>
	inline _tType CFileBase::GetFileName( const _tType &_s16Path ) {
		// If the last character is } then it is a file inside a ZIP.
		if ( _s16Path.size() && _s16Path[_s16Path.size()-1] == typename _tType::value_type( '}' ) ) {
			typename _tType::size_type stFound = _s16Path.rfind( typename _tType::value_type( '{' ) );
			_tType s16File = _s16Path.substr( stFound + 1 );
			s16File.pop_back();
			return s16File;
		}
		_tType s16Normalized = CUtilities::Replace( _s16Path, typename _tType::value_type( '/' ), typename _tType::value_type( '\\' ) );
		typename _tType::size_type stFound = s16Normalized.rfind( typename _tType::value_type( '\\' ) );
		_tType s16File = s16Normalized.substr( stFound + 1 );

		return s16File;
	}

	/**
	 * Gets the file path without the file name
	 *
	 * \param _s16Path The file path whose path is to be obtained.
	 * \return Returns a string containing the file path.
	 */
	template <typename _tType>
	inline _tType CFileBase::GetFilePath( const _tType &_s16Path ) {
		if ( _s16Path.size() ) {
			_tType s16Normalized = CUtilities::Replace( _s16Path, typename _tType::value_type( '/' ), typename _tType::value_type( '\\' ) );
			typename _tType::size_type stFound = s16Normalized.rfind( typename _tType::value_type( '\\' ) );
			if ( stFound >= s16Normalized.size() ) { return _tType(); }
			_tType s16File = s16Normalized.substr( 0, stFound + 1 );
			return s16File;
		}
		return _tType();
	}

}	// namespace lsn

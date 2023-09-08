/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Useful utilitie functions.
 */


#include "LSNUtilities.h"
#include "../OS/LSNOs.h"

#ifndef LSN_WINDOWS
#include <codecvt>
#endif

namespace lsn {

	// == Functions.
	/**
	 * Converts a UTF-8 string to a UTF-16 string.  The resulting string may have allocated more characters than necessary but will be terminated with a NULL.
	 *
	 * \param _pcString String to convert.
	 * \param _pbErrored If not nullptr, holds a returned boolean indicating success or failure of the conversion.
	 * \return Returns the converted UTF-16 string.
	 */
	std::u16string CUtilities::Utf8ToUtf16( const char8_t * _pcString, bool * _pbErrored ) {
#ifdef LSN_WINDOWS
		if ( _pbErrored != nullptr ) { (*_pbErrored) = true; }
		std::u16string wsOutput;
		int iLen = ::MultiByteToWideChar( CP_UTF8, MB_ERR_INVALID_CHARS, reinterpret_cast<LPCCH>(_pcString), -1, reinterpret_cast<LPWSTR>(wsOutput.data()), static_cast<int>(wsOutput.size()) );
		if ( iLen <= 0 ) { return wsOutput; }
		wsOutput.resize( iLen + 10 );	// Pretty sure it doesn't have to be + 10; + 1 would probably be fine but an example on MSDN uses + 10 so meh, let's just go with it.
		iLen = ::MultiByteToWideChar( CP_UTF8, MB_ERR_INVALID_CHARS, reinterpret_cast<LPCCH>(_pcString), -1, reinterpret_cast<LPWSTR>(wsOutput.data()), static_cast<int>(wsOutput.size()) );
		if ( iLen <= 0 ) {
			wsOutput.clear();
			return wsOutput;
		}
		if ( _pbErrored != nullptr ) { (*_pbErrored) = false; }
		return wsOutput;
#else
		// Visual Studio reports these as deprecated since C++17.
		if ( _pbErrored != nullptr ) { (*_pbErrored) = false; }
		try {
			return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.from_bytes( reinterpret_cast<const char *>(_pcString) );
		}
		catch ( ... ) { 
			if ( _pbErrored != nullptr ) { (*_pbErrored) = true; }
			return std::u16string();
		}
#endif	// #ifdef LSN_WINDOWS
	}

	/**
	 * Converts a UTF-16 string to a UTF-8 string.  The resulting string may have allocated more characters than necessary but will be terminated with a NULL.
	 *
	 * \param _pcPath String to convert.
	 * \param _pbErrored If not nullptr, holds a returned boolean indicating success or failure of the conversion.
	 * \return Returns the converted UTF-8 string.
	 */
	std::string CUtilities::Utf16ToUtf8( const char16_t * _pcString, bool * _pbErrored ) {
#ifdef LSN_WINDOWS
		if ( _pbErrored != nullptr ) { (*_pbErrored) = true; }
		std::string sOutput;
		int iLen = ::WideCharToMultiByte( CP_UTF8, WC_ERR_INVALID_CHARS, reinterpret_cast<LPCWCH>(_pcString), -1, sOutput.data(), static_cast<int>(sOutput.size()), NULL, NULL );
		if ( iLen <= 0 ) { return sOutput; }
		sOutput.resize( iLen + 10 );
		iLen = ::WideCharToMultiByte( CP_UTF8, WC_ERR_INVALID_CHARS, reinterpret_cast<LPCWCH>(_pcString), -1, sOutput.data(), static_cast<int>(sOutput.size()), NULL, NULL );
		if ( iLen <= 0 ) {
			sOutput.clear();
			return sOutput;
		}
		if ( _pbErrored != nullptr ) { (*_pbErrored) = false; }
		return sOutput;
#else
		// Visual Studio reports these as deprecated since C++17.
		if ( _pbErrored != nullptr ) { (*_pbErrored) = false; }
		try {
			return std::wstring_convert<std::codecvt_utf8<wchar_t>>{}.to_bytes( reinterpret_cast<const wchar_t *>(_pcString) );
		}
		catch ( ... ) { 
			if ( _pbErrored != nullptr ) { (*_pbErrored) = true; }
			return std::string();
		}
#endif	// #ifdef LSN_WINDOWS
	}

	/**
	 * Creates a string with _cReplaceMe replaced with _cWithMe inside _s16String.
	 *
	 * \param _s16String The string in which replacements are to be made.
	 * \param _cReplaceMe The character to replace.
	 * \param _cWithMe The character with which to replace _cReplaceMe.
	 * \return Returns the new string with the given replacements made.
	 */
	std::u16string CUtilities::Replace( const std::u16string &_s16String, char16_t _cReplaceMe, char16_t _cWithMe ) {
		std::u16string s16Copy = _s16String;
		auto aFound = s16Copy.find( _cReplaceMe );
		while ( aFound != std::string::npos ) {
			s16Copy[aFound] = _cWithMe;
			aFound = s16Copy.find( _cReplaceMe, aFound + 1 );
		}
		return s16Copy;
	}

	/**
	 * Gets the extension from a file path.
	 *
	 * \param _s16Path The file path whose extension is to be obtained.
	 * \return Returns a string containing the file extension.
	 */
	std::u16string CUtilities::GetFileExtension( const std::u16string &_s16Path ) {
		std::u16string s16File = GetFileName( _s16Path );
		std::string::size_type stFound = s16File.rfind( u'.' );
		if ( stFound == std::string::npos ) { return std::u16string(); }
		return s16File.substr( stFound + 1 );
	}

	/**
	 * Removes the extension from a file path.
	 *
	 * \param _s16Path The file path whose extension is to be removed.
	 * \return Returns a string containing the file mname without the extension.
	 */
	std::u16string CUtilities::NoExtension( const std::u16string &_s16Path ) {
		std::u16string s16File = GetFileName( _s16Path );
		std::string::size_type stFound = s16File.rfind( u'.' );
		if ( stFound == std::string::npos ) { return std::u16string(); }
		return s16File.substr( 0, stFound );
	}

	/**
	 * Gets the file name from a file path.
	 *
	 * \param _s16Path The file path whose name is to be obtained.
	 * \return Returns a string containing the file name.
	 */
	std::u16string CUtilities::GetFileName( const std::u16string &_s16Path ) {
		// If the last character is } then it is a file inside a ZIP.
		if ( _s16Path.size() && _s16Path[_s16Path.size()-1] == u'}' ) {
			std::string::size_type stFound = _s16Path.rfind( u'{' );
			std::u16string s16File = _s16Path.substr( stFound + 1 );
			s16File.pop_back();
			return s16File;
		}
		std::u16string s16Normalized = Replace( _s16Path, u'/', u'\\' );
		std::string::size_type stFound = s16Normalized.rfind( u'\\' );
		std::u16string s16File = s16Normalized.substr( stFound + 1 );

		return s16File;
	}

}	// namespace lsn

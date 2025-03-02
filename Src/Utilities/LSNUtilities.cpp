/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Useful utilitie functions.
 */


#include "LSNUtilities.h"
#include "../File/LSNFileBase.h"
#include "../OS/LSNOs.h"
#include "EEExpEval.h"

#include <cwctype>
#include <filesystem>

#ifndef LSN_WINDOWS
#include <codecvt>
#endif

#if defined( __APPLE__ )
#include <CoreFoundation/CoreFoundation.h>
#endif	// #if defined( __APPLE__ )

namespace lsn {

	// == Members.
	const float CUtilities::m_fNtscLevels[16] = {							/**< Output levels. */
		0.228f, 0.312f, 0.552f, 0.880f, // Signal low.
		0.616f, 0.840f, 1.100f, 1.100f, // Signal high.
		0.192f, 0.256f, 0.448f, 0.712f, // Signal low, attenuated.
		0.500f, 0.676f, 0.896f, 0.896f  // Signal high, attenuated.
	};

	const float CUtilities::m_fPalLevels[16] = {							/**< Output levels for PAL. */
		-0.1312201772324471937825052236803458072245121002197265625f,	0.0f,															0.3561690524880709585175964093650691211223602294921875f,	0.8000681663258351061784878766047768294811248779296875f,
		0.456646216768916202166650464278063736855983734130859375f,		0.74982958418541245659838523351936601102352142333984375f,		1.1f,														1.1f,
		-0.1994546693933197112347244228658382780849933624267578125f,	-0.09972733469665985561736221143291913904249668121337890625f,	0.16871165644171781661242448535631410777568817138671875f,	0.52488070892978877513002089472138322889804840087890625f,
		0.250443081117927734968731101616867817938327789306640625f,		0.481390593047034798246386344544589519500732421875f,			0.74982958418541245659838523351936601102352142333984375f,	0.74982958418541245659838523351936601102352142333984375f

	};

	LSN_ALIGN( 64 )
	float CUtilities::m_fNoiseBuffers[LSN_NOISE_BUFFERS][16];				/**< LSN_NOISE_BUFFERS noise buffers, each with 16 samples. */

	uint32_t CUtilities::m_ui32Rand = 194;									/**< A quick pseudo-random value updated each time it is accessed via Rand(). */

	bool CUtilities::m_bNoiseIsGaussian = false;							/**< True if the last call was to GenGaussianNoise(). */
	float CUtilities::m_fLastNoiseParm = 0.0f;								/**< The last value passed to either GenUniformNoise() or GenGaussianNoise(). */

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
		int iLen = ::MultiByteToWideChar( CP_UTF8, 0/*MB_ERR_INVALID_CHARS*/, reinterpret_cast<LPCCH>(_pcString), -1, reinterpret_cast<LPWSTR>(wsOutput.data()), static_cast<int>(wsOutput.size()) );
		if ( iLen <= 0 ) { return wsOutput; }
		wsOutput.resize( iLen + 10 );	// Pretty sure it doesn't have to be + 10; + 1 would probably be fine but an example on MSDN uses + 10 so meh, let's just go with it.
		iLen = ::MultiByteToWideChar( CP_UTF8, 0/*MB_ERR_INVALID_CHARS*/, reinterpret_cast<LPCCH>(_pcString), -1, reinterpret_cast<LPWSTR>(wsOutput.data()), static_cast<int>(wsOutput.size()) );
		if ( iLen <= 0 ) {
			wsOutput.clear();
			return wsOutput;
		}
		if ( _pbErrored != nullptr ) { (*_pbErrored) = false; }
		while ( wsOutput.size() && !wsOutput[wsOutput.size()-1] ) { wsOutput.pop_back(); }
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
	 * \param _pcString String to convert.
	 * \param _pbErrored If not nullptr, holds a returned boolean indicating success or failure of the conversion.
	 * \return Returns the converted UTF-8 string.
	 */
	std::u8string CUtilities::Utf16ToUtf8( const char16_t * _pcString, bool * _pbErrored ) {
#ifdef LSN_WINDOWS
		if ( _pbErrored != nullptr ) { (*_pbErrored) = true; }
		std::u8string sOutput;
		int iLen = ::WideCharToMultiByte( CP_UTF8, WC_ERR_INVALID_CHARS, reinterpret_cast<LPCWCH>(_pcString), -1, reinterpret_cast<LPSTR>(sOutput.data()), static_cast<int>(sOutput.size()), NULL, NULL );
		if ( iLen <= 0 ) { return sOutput; }
		sOutput.resize( iLen + 10 );
		iLen = ::WideCharToMultiByte( CP_UTF8, WC_ERR_INVALID_CHARS, reinterpret_cast<LPCWCH>(_pcString), -1, reinterpret_cast<LPSTR>(sOutput.data()), static_cast<int>(sOutput.size()), NULL, NULL );
		if ( iLen <= 0 ) {
			sOutput.clear();
			return sOutput;
		}
		if ( _pbErrored != nullptr ) { (*_pbErrored) = false; }
		while ( sOutput.size() && !sOutput[sOutput.size()-1] ) { sOutput.pop_back(); }
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

	/**
	 * Gets the file path without the file name
	 *
	 * \param _s16Path The file path whose path is to be obtained.
	 * \return Returns a string containing the file path.
	 */
	std::u16string CUtilities::GetFilePath( const std::u16string &_s16Path ) {
		if ( _s16Path.size() ) {
			std::u16string s16Normalized = Replace( _s16Path, u'/', u'\\' );
			std::string::size_type stFound = s16Normalized.rfind( u'\\' );
			if ( stFound >= s16Normalized.size() ) { return std::u16string(); }
			std::u16string s16File = s16Normalized.substr( 0, stFound + 1 );
			return s16File;
		}
		return std::u16string();
	}

	/**
	 * Gets or creates a file path for custom per-ROM settings given its CRC, name, and the per-ROM settings folder path.
	 * 
	 * \param _pwcPath Path to the per-ROM settings folder.
	 * \param _ui32Crc the CRC32 of the ROM.
	 * \param _pu16Name The name of the ROM.
	 * \return Returns a path to the per-ROM settings file for the given ROM.
	 **/
	std::u16string CUtilities::PerRomSettingsPath( const std::wstring &_pwcPath, uint32_t _ui32Crc, const std::u16string &_pu16Name ) {
		std::filesystem::path pDir = _pwcPath;
		std::filesystem::create_directories( pDir );

		std::filesystem::path pPreferred = pDir;
		pPreferred /= std::format( L"{:08X} {}.prs", _ui32Crc, reinterpret_cast<const wchar_t *>(_pu16Name.c_str()) );
		if ( std::filesystem::exists( pPreferred ) ) { return pPreferred.generic_u16string(); }
		
		std::filesystem::path pSearch = pDir;
		auto aTmp = std::format( L"{:08X} *.prs", _ui32Crc );
		std::vector<std::u16string> vRes;
		CFileBase::FindFiles( pSearch.generic_u16string().c_str(), XStringToU16String( aTmp.c_str(), aTmp.size() ).c_str(), false, vRes );
		if ( !vRes.size() ) { return pPreferred.generic_u16string(); }
		return vRes[0];
	}

	/**
	 * Deconstructs a ZIP file name formatted as zipfile{name}.  If not a ZIP file, the file name is extracted.
	 * 
	 * \param _s16Path The input file path to be deconstructed.
	 * \param _fpPaths The deconstructed file paths.
	 **/
	void CUtilities::DeconstructFilePath( const std::u16string &_s16Path, LSN_FILE_PATHS &_fpPaths ) {
		_fpPaths.u16sFullPath = _s16Path;
		if ( _s16Path.size() ) {
			if ( _s16Path[_s16Path.size()-1] == u'}' ) {
				std::string::size_type stFound = _s16Path.rfind( u'{' );
				if ( stFound < _s16Path.size() ) {
					std::u16string s16Normalized = Replace( _s16Path, u'/', u'\\' );
					_fpPaths.u16sPath = s16Normalized.substr( 0, stFound );
					_fpPaths.u16sPath += u'\\';
					_fpPaths.u16sFile = _s16Path.substr( stFound + 1, _s16Path.size() - stFound - 2 );
					return;
				}
			}
			_fpPaths.u16sPath = GetFilePath( _s16Path );
			_fpPaths.u16sFile = GetFileName( _s16Path );
		}
	}

	/**
	 * Creates an array of deconstructed file paths.
	 * 
	 * \param _s16Paths The input array of paths to be deconstructed.
	 * \return Returns an array of deconstructed file paths.
	 **/
	std::vector<CUtilities::LSN_FILE_PATHS> CUtilities::DeconstructFilePaths( const std::vector<std::u16string> &_s16Paths ) {
		std::vector<CUtilities::LSN_FILE_PATHS> vPaths;
		if ( _s16Paths.size() ) {
			vPaths.resize( _s16Paths.size() );
			for ( size_t I = 0; I < _s16Paths.size(); ++I ) {
				DeconstructFilePath( _s16Paths[I], vPaths[I] );
			}
		}
		return vPaths;
	}

	/**
	 * Finds the first duplicated name in the list and then returns an array of indices of each file name that is a duplucate of the first-found duplicate name.
	 * 
	 * \param _s16Paths The input array of paths.
	 * \param _vIndices Returned indices of paths that match.
	 * \return Returns true if any file names are duplicates of each other when compared with case-insensitivity.
	 **/
	bool CUtilities::DuplicateFiles( const std::vector<CUtilities::LSN_FILE_PATHS> &_s16Paths, std::vector<size_t> &_vIndices ) {
		_vIndices.clear();
		for ( size_t I = 0; I < _s16Paths.size(); ++I ) {
			bool bFound = false;
			for ( size_t J = I + 1; J < _s16Paths.size(); ++J ) {
				if ( StringCmpUtf16_IgnoreCase( _s16Paths[I].u16sFile, _s16Paths[J].u16sFile ) ) {
					bFound = true;
					_vIndices.push_back( J );
				}
			}
			if ( bFound ) {
				_vIndices.push_back( I );
				return true;
			}
		}
		return false;
	}

	/**
	 * Performs a case-insensitive string compare against 2 UTF-16 strings.  Returns true if they are equal.
	 *
	 * \param _u16Str0 The left operand.
	 * \param _u16Str1 The right operand.
	 * \return Returns true if the given strings are equal as compared via a case-insensitive UTF-16 compare.
	 **/
	bool CUtilities::StringCmpUtf16_IgnoreCase( const std::u16string &_u16Str0, const std::u16string &_u16Str1 ) {
#ifdef LSN_WINDOWS
		return CSTR_EQUAL == ::CompareStringEx( LOCALE_NAME_INVARIANT, NORM_IGNORECASE,
			reinterpret_cast<LPCWCH>(_u16Str0.c_str()), -1, reinterpret_cast<LPCWCH>(_u16Str1.c_str()), -1,
			NULL, NULL, NULL );
#else
		CFStringRef cfStr1 = ::CFStringCreateWithBytes(
			nullptr,
			reinterpret_cast<const UInt8 *>(_u16Str0.c_str()),
			_u16Str0.length() * sizeof( char16_t ),
			kCFStringEncodingUTF16,
			false
		);

		CFStringRef cfStr2 = ::CFStringCreateWithBytes(
			nullptr,
			reinterpret_cast<const UInt8 *>(_u16Str1.c_str()),
			_u16Str1.length() * sizeof( char16_t ),
			kCFStringEncodingUTF16,
			false
		);

		bool bResult = ::CFStringCompare(
			cfStr1, cfStr2, kCFCompareCaseInsensitive
		) == kCFCompareEqualTo;

		CFRelease( cfStr1 );
		CFRelease( cfStr2 );

		return bResult;
#endif	// #ifdef LSN_WINDOWS
	}

	/**
	 * Copies the last folder in the path given in the first string to the start of the 2nd string.
	 * 
	 * \param _u16Folders The folder path.
	 * \param _u16Path The file name.
	 **/
	void CUtilities::CopyLastFolderToFileName( std::u16string &_u16Folders, std::u16string &_u16Path ) {
		if ( _u16Path.size() && _u16Path.rfind( u'\\' ) < _u16Path.size() ) {
			if ( _u16Path[0] == u'\u2026' ) {
				_u16Path.erase( _u16Path.begin() );
			}
		}
		if ( _u16Folders.size() == 0 ) { return; }
		// Remove the trailing \.
		_u16Folders.erase( _u16Folders.begin() + _u16Folders.size() - 1 );
		std::string::size_type stFound = _u16Folders.rfind( u'\\' );
		if ( stFound >= _u16Folders.size() ) {
			// Only the root remains. Copy it all.
			_u16Path = _u16Folders + u'\\' + _u16Path;
			_u16Folders.clear();
			return;
		}
		// There is a folder.
		_u16Path.insert( _u16Path.begin(), u'\\' );
		while ( _u16Folders.size() > stFound + 1 ) {
			_u16Path.insert( _u16Path.begin(), _u16Folders[_u16Folders.size()-1] );
			_u16Folders.pop_back();
		}
		_u16Path.insert( _u16Path.begin(), u'\\' );
		_u16Path.insert( _u16Path.begin(), u'\u2026' );
	}

}	// namespace lsn

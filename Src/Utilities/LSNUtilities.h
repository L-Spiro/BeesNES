﻿/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Useful utility functions.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#if defined( __i386__ ) || defined( __x86_64__ ) || defined( _MSC_VER )
#include "../OS/LSNFeatureSet.h"
#endif	// #if defined( __i386__ ) || defined( __x86_64__ ) || defined( _MSC_VER )

#include <algorithm>
#include <cmath>
#include <cwctype>
#include <fenv.h>
//#include <intrin.h>
#include <numbers>
#include <random>
#include <string>
#include <vector>


#ifndef LSN_ELEMENTS
#define LSN_ELEMENTS( x )									((sizeof( x ) / sizeof( 0[x] )) / (static_cast<size_t>(!(sizeof( x ) % sizeof(0[x])))))
#endif	// #ifndef LSN_ELEMENTS

#ifndef LSN_PI
#define LSN_PI												3.14159265358979323846264338327950288419716939937510	// You can tell how cool a programmer is by how many digits she puts in the PI macro.
#endif	// #ifndef LSN_PI

#ifndef LSN_ROUND_UP
/** Round up to the next nearest Xth, where X is a power of 2. */
#define LSN_ROUND_UP( VALUE, X )							((VALUE) + (((X) - (VALUE) & ((X) - 1)) & ((X) - 1)))
#endif	// #ifndef LSN_ROUND_UP

#define LSN_NOISE_BUFFERS									32
#define LSN_NOISE_BUFFER( X )								((X) & 0x1F)

namespace lsn {

	/**
	 * Class CUtilities
	 * \brief Useful utility functions.
	 *
	 * Description: Useful utility functions.
	 */
	class CUtilities {
	public :
		// == Types.
		struct LSN_FILE_PATHS {
			std::u16string u16sFullPath;					/**< The full file path. */
			std::u16string u16sPath;						/**< Path to the file without the file name. */
			std::u16string u16sFile;						/**< The file name. */
		};

		/** Temporarily setting the floating-point rounding mode. */
		struct LSN_FEROUNDMODE {
			LSN_FEROUNDMODE( int _iNewMode ) :
				iPrevMode( ::fegetround() ) {
				::fesetround( _iNewMode );
			}
			~LSN_FEROUNDMODE() {
				::fesetround( iPrevMode );
			}

			int							iPrevMode;					/**< The previous rounding mode. */
		};


		// == Functions.
		/**
		 * Converts a UTF-8 string to a UTF-16 string.  The resulting string may have allocated more characters than necessary but will be terminated with a NULL.
		 *
		 * \param _pcString String to convert.
		 * \param _pbErrored If not nullptr, holds a returned boolean indicating success or failure of the conversion.
		 * \return Returns the converted UTF-16 string.
		 */
		static std::u16string								Utf8ToUtf16( const char8_t * _pcString, bool * _pbErrored = nullptr );

		/**
		 * Converts a UTF-16 string to a UTF-8 string.  The resulting string may have allocated more characters than necessary but will be terminated with a NULL.
		 *
		 * \param _pcString String to convert.
		 * \param _pbErrored If not nullptr, holds a returned boolean indicating success or failure of the conversion.
		 * \return Returns the converted UTF-8 string.
		 */
		static std::u8string								Utf16ToUtf8( const char16_t * _pcString, bool * _pbErrored = nullptr );

		/**
		 * Converts a value to a string.
		 * 
		 * \param _tVal The value to convert.
		 * \param _stDigits The number of digits to which to pad.
		 * \return Returns the string form of the given value.
		 **/
		template <typename _tType>
		static std::u16string								ToString( _tType _tVal, size_t _stDigits = 0 ) {
			std::wstring wsNumber = std::to_wstring( _tVal );
			std::u16string usNumber( wsNumber.begin(), wsNumber.end() );
			while ( usNumber.size() < _stDigits ) {
				usNumber.insert( usNumber.begin(), u'0' );
			}
			return usNumber;
		}

		/**
		 * Converts an * string to a std::wstring.  Call inside try{}catch(...){}.
		 * 
		 * \param _pwcStr The string to convert.
		 * \param _sLen The length of the string or 0.
		 * \return Returns the converted string.
		 **/
		template <typename _tCharType>
		static inline std::wstring							XStringToWString( const _tCharType * _pwcStr, size_t _sLen ) {
			std::wstring u16Tmp;
			if ( _sLen ) {
				u16Tmp.reserve( _sLen );
			}
			for ( size_t I = 0; (_sLen && I < _sLen) || (_sLen == 0 && _pwcStr[I]); ++I ) {
				u16Tmp.push_back( static_cast<wchar_t>(_pwcStr[I]) );
			}
			return u16Tmp;
		}

		/**
		 * Converts an * string to a std::u16string.  Call inside try{}catch(...){}.
		 * 
		 * \param _pwcStr The string to convert.
		 * \param _sLen The length of the string or 0.
		 * \return Returns the converted string.
		 **/
		template <typename _tCharType>
		static inline std::u16string						XStringToU16String( const _tCharType * _pwcStr, size_t _sLen ) {
			std::u16string u16Tmp;
			if ( _sLen ) {
				u16Tmp.reserve( _sLen );
			}
			for ( size_t I = 0; (_sLen && I < _sLen) || (_sLen == 0 && _pwcStr[I]); ++I ) {
				u16Tmp.push_back( static_cast<char16_t>(_pwcStr[I]) );
			}
			return u16Tmp;
		}

		/**
		 * Converts an * string to a std::u8string.  Call inside try{}catch(...){}.
		 * 
		 * \param _pwcStr The string to convert.
		 * \param _sLen The length of the string or 0.
		 * \return Returns the converted string.
		 **/
		template <typename _tCharType>
		static inline std::u8string							XStringToU8String( const _tCharType * _pwcStr, size_t _sLen ) {
			std::u8string u16Tmp;
			if ( _sLen ) {
				u16Tmp.reserve( _sLen );
			}
			for ( size_t I = 0; (_sLen && I < _sLen) || (_sLen == 0 && _pwcStr[I]); ++I ) {
				u16Tmp.push_back( static_cast<char8_t>(_pwcStr[I]) );
			}
			return u16Tmp;
		}

		/**
		 * Generates a string of random characters.
		 * 
		 * \param _pcBuffer The buffer to fill with random characters.
		 * \param _sSize The size of the buffer to which _pcBuffer points, including the NULL terminator.
		 * \return Returns _pcBuffer.
		 **/
		template <typename _tType>
		static _tType *										RandomString( _tType * _pcBuffer, size_t _sSize ) {
			if ( !_pcBuffer || !_sSize ) { return _pcBuffer; }
			std::random_device rdRand;
			std::mt19937 mGen( rdRand() );
			std::uniform_int_distribution<> uidDistLower( _tType( 'a' ), _tType( 'z' ) );
			std::uniform_int_distribution<> uidDistUpper( _tType( 'A' ), _tType( 'Z' ) );
			std::uniform_int_distribution<> uidBool( 0, 3 );
			for ( size_t I = 0; I < _sSize - 1; ++I ) {
				if ( uidBool( mGen ) == 0 ) {
					_pcBuffer[I] = _tType( uidDistUpper( mGen ) );
				}
				else {
					_pcBuffer[I] = _tType( uidDistLower( mGen ) );
				}
			}
			_pcBuffer[_sSize-1] = _tType( '\0' );
			return _pcBuffer;
		}

		/**
		 * Creates a string with _cReplaceMe replaced with _cWithMe inside _s16String.
		 *
		 * \param _s16String The string in which replacements are to be made.
		 * \param _cReplaceMe The character to replace.
		 * \param _cWithMe The character with which to replace _cReplaceMe.
		 * \return Returns the new string with the given replacements made.
		 */
		template <typename _tType = std::u16string>
		static _tType										Replace( const _tType &_s16String, const _tType &_cReplaceMe, const _tType &_cWithMe ) {
			_tType sCopy = _s16String;
			const size_t sLen = _cReplaceMe.size();
			size_t sIdx = sCopy.find( _cReplaceMe );
			while ( _tType::npos != sIdx ) {
				sCopy = sCopy.replace( sIdx, sLen, _cWithMe );
				sIdx = sCopy.find( _cReplaceMe );
			}
			return sCopy;
		}

		/**
		 * Creates a string with _cReplaceMe replaced with _cWithMe inside _s16String.
		 *
		 * \param _s16String The string in which replacements are to be made.
		 * \param _cReplaceMe The character to replace.
		 * \param _cWithMe The character with which to replace _cReplaceMe.
		 * \return Returns the new string with the given replacements made.
		 */
		template <typename _tType = std::u16string>
		static _tType										Replace( const _tType &_s16String, _tType::value_type _cReplaceMe, _tType::value_type _cWithMe ) {
			_tType s16Copy = _s16String;
			auto aFound = s16Copy.find( _cReplaceMe );
			while ( aFound != _tType::npos ) {
				s16Copy[aFound] = _cWithMe;
				aFound = s16Copy.find( _cReplaceMe, aFound + 1 );
			}
			return s16Copy;
		}

		/**
		 * Fixes the file name.  Replaces invalid file-name characters with logical alternatives.
		 * 
		 * \param _u16File The file name to replace.
		 * \return Returns the replaced file name.
		 **/
		static std::u16string								FixFile( const std::u16string &_u16File ) {
			std::u16string u16Fixed( _u16File );
			u16Fixed = Replace( u16Fixed, std::u16string( u"<"  ), std::u16string( u"("  ) );
			u16Fixed = Replace( u16Fixed, std::u16string( u">"  ), std::u16string( u")"  ) );
			u16Fixed = Replace( u16Fixed, std::u16string( u":"  ), std::u16string( u" -" ) );
			u16Fixed = Replace( u16Fixed, std::u16string( u"\"" ), std::u16string( u"\'" ) );
			u16Fixed = Replace( u16Fixed, std::u16string( u"/"  ), std::u16string( u"-"  ) );
			u16Fixed = Replace( u16Fixed, std::u16string( u"\\" ), std::u16string( u"-"  ) );
			u16Fixed = Replace( u16Fixed, std::u16string( u"|"  ), std::u16string( u"-"  ) );
			u16Fixed = Replace( u16Fixed, std::u16string( u"?"  ), std::u16string( u"？" ) );
			u16Fixed = Replace( u16Fixed, std::u16string( u"*"  ), std::u16string( u"×"  ) );
			return u16Fixed;
		}

		/**
		 * Fixes the file name.  Replaces invalid file-name characters with logical alternatives.
		 * 
		 * \param _u16File The file name to replace.
		 * \return Returns the replaced file name.
		 **/
		static std::wstring									FixFile( const std::wstring &_u16File ) {
			std::wstring u16Fixed( _u16File );
			u16Fixed = Replace( u16Fixed, std::wstring( L"<"  ), std::wstring( L"("  ) );
			u16Fixed = Replace( u16Fixed, std::wstring( L">"  ), std::wstring( L")"  ) );
			u16Fixed = Replace( u16Fixed, std::wstring( L":"  ), std::wstring( L" -" ) );
			u16Fixed = Replace( u16Fixed, std::wstring( L"\"" ), std::wstring( L"\'" ) );
			u16Fixed = Replace( u16Fixed, std::wstring( L"/"  ), std::wstring( L"-"  ) );
			u16Fixed = Replace( u16Fixed, std::wstring( L"\\" ), std::wstring( L"-"  ) );
			u16Fixed = Replace( u16Fixed, std::wstring( L"|"  ), std::wstring( L"-"  ) );
			u16Fixed = Replace( u16Fixed, std::wstring( L"?"  ), std::wstring( L"？" ) );
			u16Fixed = Replace( u16Fixed, std::wstring( L"*"  ), std::wstring( L"×"  ) );
			return u16Fixed;
		}

		/**
		 * Performs ::towlower() on the given input.
		 * 
		 * \param _pcStr The string to convert to lower-case
		 * \return Returns the lower-cased input.
		 **/
		template <typename _tType = std::u8string>
		static inline _tType								ToLower( const _tType &_Str ) {
			_tType sRet = _Str;
			std::transform( sRet.begin(), sRet.end(), sRet.begin(), []( _tType::value_type _iC ) { return ::towlower( static_cast<wint_t>(_iC) ); } );	
			return sRet;
		}

		/**
		 * Is a string entirely whitespace?
		 * 
		 * \param _tValue The string to test.
		 * \return Returns true if all characters in the given string are whitespace, false otherwise.
		 **/
		template <typename _tType = std::u16string>
		static inline bool									IsWhiteSpace( const _tType &_tValue ) {
			bool bHasNonZero = false;
			for ( auto I = _tValue.size(); I--; ) {
				if ( _tValue[I] && !std::iswspace( _tValue[I] ) ) { return false; }
				if ( _tValue[I] ) { bHasNonZero = true; }
			}
			return !bHasNonZero;
		}

		/**
		 * Adds an element to index 0 for moves it to the bottom (index 0) of an array.  Empty strings and whitespace are not added, and are removed from the array on every call.
		 * 
		 * \param _vArray The array to update.
		 * \param _tValue The value to insert or move.
		 * \param _stMax The maximum size of the array.
		 **/
		template <typename _tType = std::u16string>
		static inline void									AddOrMove( std::vector<_tType> &_vArray, const _tType &_tValue, size_t _stMax = 100 ) {
			try {
				for ( auto I = _vArray.size(); I--; ) {
					if ( !_vArray[I].size() || IsWhiteSpace( _vArray[I] ) ) { _vArray.erase( _vArray.begin() + I ); }
				}
				if ( _tValue.size() ) {
					auto aTmp = std::find( _vArray.begin(), _vArray.end(), _tValue );
					while ( aTmp != _vArray.end() ) {
						_vArray.erase( aTmp );
						aTmp = std::find( _vArray.begin(), _vArray.end(), _tValue );
					}
					_vArray.insert( _vArray.begin(), _tValue );
				}
				if ( _vArray.size() > _stMax ) {
					_vArray.resize( _stMax );
				}
			}
			catch ( ... ) {}
		}

		/**
		 * Gets the extension from a file path.
		 *
		 * \param _s16Path The file path whose extension is to be obtained.
		 * \return Returns a string containing the file extension.
		 */
		static std::u16string								GetFileExtension( const std::u16string &_s16Path );

		/**
		 * Removes the extension from a file path.
		 *
		 * \param _s16Path The file path whose extension is to be removed.
		 * \return Returns a string containing the file mname without the extension.
		 */
		static std::u16string								NoExtension( const std::u16string &_s16Path );

		/**
		 * Gets the file name from a file path.
		 *
		 * \param _s16Path The file path whose name is to be obtained.
		 * \return Returns a string containing the file name.
		 */
		static std::u16string								GetFileName( const std::u16string &_s16Path );

		/**
		 * Gets the file path without the file name
		 *
		 * \param _s16Path The file path whose path is to be obtained.
		 * \return Returns a string containing the file path.
		 */
		static std::u16string								GetFilePath( const std::u16string &_s16Path );

		/**
		 * Gets the last character in a string or std::u16string::traits_type::char_type( 0 ).
		 * 
		 * \param _s16Str The string whose last character is to be returned, if it has any characters.
		 * \return Returns the last character in the given string or std::u16string::traits_type::char_type( 0 ).
		 **/
		static std::u16string::traits_type::char_type		LastChar( const std::u16string &_s16Str ) {
			return _s16Str.size() ? _s16Str[_s16Str.size()-1] : std::u16string::traits_type::char_type( 0 );
		}

		/**
		 * Appends a char string to a char16_t string.
		 * 
		 * \param _sDst The string to which to append the string.
		 * \param _pcString The string to append to the string.
		 * \return Returns the new string.
		 **/
		static std::u16string								Append( const std::u16string &_sDst, const char * _pcString ) {
			try {
				std::u16string sTmp = _sDst;
				while ( (*_pcString) ) {
					sTmp.push_back( (*_pcString++) );
				}
				return sTmp;
			}
			catch ( ... ) { return std::u16string(); }
		}

		/**
		 * Appends a _pwcString string to a char16_t string.
		 * 
		 * \param _sDst The string to which to append the string.
		 * \param _pwcString The string to append to the string.
		 * \return Returns the new string.
		 **/
		static std::u16string								Append( const std::u16string &_sDst, const wchar_t * _pwcString ) {
			try {
				std::u16string sTmp = _sDst;
				while ( (*_pwcString) ) {
					sTmp.push_back( (*_pwcString++) );
				}
				return sTmp;
			}
			catch ( ... ) { return std::u16string(); }
		}

		/**
		 * Gets or creates a file path for custom per-ROM settings given its CRC, name, and the per-ROM settings folder path.
		 * 
		 * \param _pwcPath Path to the per-ROM settings folder.
		 * \param _ui32Crc the CRC32 of the ROM.
		 * \param _pu16Name The name of the ROM.
		 * \return Returns a path to the per-ROM settings file for the given ROM.
		 **/
		static std::u16string								PerRomSettingsPath( const std::wstring &_pwcPath, uint32_t _ui32Crc, const std::u16string &_pu16Name );

		/**
		 * Converts a single double value from sRGB space to linear space.  Performs a conversion according to the standard.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the converted value.
		 */
		static inline double LSN_FASTCALL					sRGBtoLinear( double _dVal ) {
			return _dVal <= 0.04045 ?
				_dVal * (1.0 / 12.92) :
				std::pow( (_dVal + 0.055) * (1.0 / 1.055), 2.4 );
		}

		/**
		 * Converts a single double value from linear space to sRGB space.  Performs a conversion according to the standard.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the converted value.
		 */
		static inline double LSN_FASTCALL					LinearTosRGB( double _dVal ) {
			return _dVal <= 0.0031308 ?
				_dVal * 12.92 :
				1.055 * std::pow( _dVal, 1.0 / 2.4 ) - 0.055;
		}

		/**
		 * Converts a single double value from sRGB space to linear space.  Performs a precise conversion without a gap.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the converted value.
		 */
		static inline double LSN_FASTCALL					sRGBtoLinear_Precise( double _dVal ) {
			return _dVal <= 0.039285714285714291860163172032116563059389591217041015625 ?
				_dVal * (1.0 / 12.92321018078785499483274179510772228240966796875) :
				std::pow( (_dVal + 0.055) * (1.0 / 1.055), 2.4 );
		}

		/**
		 * Converts a single double value from linear space to sRGB space.  Performs a precise conversion without a gap.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the converted value.
		 */
		static inline double LSN_FASTCALL					LinearTosRGB_Precise( double _dVal ) {
			return _dVal <= 0.003039934639778431833823102437008856213651597499847412109375 ?
				_dVal * 12.92321018078785499483274179510772228240966796875 :
				1.055 * std::pow( _dVal, 1.0 / 2.4 ) - 0.055;
		}

		/**
		 * Converts from SMPTE 170M-2004 to linear.  Performs a conversion according to the standard.
		 * 
		 * \param _dVal The value to convert.
		 * \return Returns the color value converted to linear space.
		 **/
		static inline double LSN_FASTCALL					SMPTE170MtoLinear( double _dVal ) {
			return _dVal < 0.081 ?
				_dVal * (1.0 / 4.5) :
				std::pow( (_dVal + 0.099) * (1.0 / 1.099), 1.0 / 0.45 );
		}

		/**
		 * Converts from linear to SMPTE 170M-2004.  Performs a conversion according to the standard.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the value converted to SMPTE 170M-2004 space.
		 */
		static inline double LSN_FASTCALL					LinearToSMPTE170M( double _dVal ) {
			return _dVal < 0.018 ?
				_dVal * 4.5 :
				1.099 * std::pow( _dVal, 0.45 ) - 0.099;
		}

		/**
		 * Converts from SMPTE 170M-2004 to linear.  Performs a precise conversion without a gap.
		 * 
		 * \param _dVal The value to convert.
		 * \return Returns the color value converted to linear space.
		 **/
		static inline double LSN_FASTCALL					SMPTE170MtoLinear_Precise( double _dVal ) {
			return _dVal <= 0.08124285829863515939752716121802222914993762969970703125 ?
				_dVal / 4.5 :
				std::pow( (_dVal + 0.09929682680944297568093048766968422569334506988525390625) / 1.09929682680944297568093048766968422569334506988525390625, 1.0 / 0.45 );
		}

		/**
		 * Converts from linear to SMPTE 170M-2004.  Performs a precise conversion without a gap.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the value converted to SMPTE 170M-2004 space.
		 */
		static inline double LSN_FASTCALL					LinearToSMPTE170M_Precise( double _dVal ) {
			return _dVal <= 0.0180539685108078128139563744980478077195584774017333984375 ?
				_dVal * 4.5 :
				1.09929682680944297568093048766968422569334506988525390625 * std::pow( _dVal, 0.45 ) - 0.09929682680944297568093048766968422569334506988525390625;
		}

		/**
		 * Converts from DCI-P3 to linear.
		 * 
		 * \param _dVal The value to convert.
		 * \return Returns the color value converted to linear space.
		 **/
		static inline double LSN_FASTCALL					DCIP3toLinear( double _dVal ) {
			return std::pow( _dVal, 2.6 );
		}

		/**
		 * Converts from linear to DCI-P3.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the value converted to DCI-P3 space.
		 */
		static inline double LSN_FASTCALL					LinearToDCIP3( double _dVal ) {
			return std::pow( _dVal, 1.0 / 2.6 );
		}

		/**
		 * Converts from Adobe RGB to linear.
		 * 
		 * \param _dVal The value to convert.
		 * \return Returns the color value converted to linear space.
		 **/
		static inline double LSN_FASTCALL					AdobeRGBtoLinear( double _dVal ) {
			return std::pow( _dVal, 2.19921875 );
		}

		/**
		 * Converts from linear to Adobe RGB.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the value converted to Adobe RGB space.
		 */
		static inline double LSN_FASTCALL					LinearToAdobeRGB( double _dVal ) {
			return std::pow( _dVal, 1.0 / 2.19921875 );
		}

		/**
		 * Converts from SMPTE 240M to linear.  Performs a conversion according to the standard.
		 * 
		 * \param _dVal The value to convert.
		 * \return Returns the color value converted to linear space.
		 **/
		static inline double LSN_FASTCALL					SMPTE240MtoLinear( double _dVal ) {
			return _dVal < 0.0913 ?
				_dVal / 4.0 :
				std::pow( (_dVal + 0.1115) / 1.1115, 1.0 / 0.45 );
		}

		/**
		 * Converts from linear to SMPTE 240M.  Performs a conversion according to the standard.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the value converted to SMPTE 240M space.
		 */
		static inline double LSN_FASTCALL					LinearToSMPTE240M( double _dVal ) {
			return _dVal < 0.0228 ?
				_dVal * 4.0 :
				1.1115 * std::pow( _dVal, 0.45 ) - 0.1115;
		}

		/**
		 * Converts from SMPTE 240M to linear.  Performs a precise conversion without a gap.
		 * 
		 * \param _dVal The value to convert.
		 * \return Returns the color value converted to linear space.
		 **/
		static inline double LSN_FASTCALL					SMPTE240MtoLinear_Precise( double _dVal ) {
			return _dVal < 0.0912863421177801115380390228892792947590351104736328125 ?
				_dVal / 4.0 :
				std::pow( (_dVal + 0.1115721959217312597711924126997473649680614471435546875) / 1.1115721959217312597711924126997473649680614471435546875, 1.0 / 0.45 );
		}

		/**
		 * Converts from linear to SMPTE 240M.  Performs a precise conversion without a gap.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the value converted to SMPTE 240M space.
		 */
		static inline double LSN_FASTCALL					LinearToSMPTE240M_Precise( double _dVal ) {
			return _dVal < 0.022821585529445027884509755722319823689758777618408203125 ?
				_dVal * 4.0 :
				1.1115721959217312597711924126997473649680614471435546875 * std::pow( _dVal, 0.45 ) - 0.1115721959217312597711924126997473649680614471435546875;
		}

		/**
		 * A proper CRT curve with WHITE and BRIGHTNESS controls.
		 * 
		 * \param param _dVal The value to convert.
		 * \param _dLw Screen luminance for white, reference setting is LW = 100 cd/m2.
		 * \param _dB Variable for black level lift (legacy "brightness" control).
		 *	The value of _dB is set so that the calculated luminance can be the same as the
		 *	measurement data at input signal level 0.0183 (= (80-64)/876).
		 *	The value of _dB changes depending on "brightness" control.
		 * \return Returns the corresponding value from a decent CRT curve back to linear.
		 **/
		static inline double LSN_FASTCALL					CrtProperToLinear( double _dVal, double _dLw = 1.0, double _dB = 0.0181 ) {
			constexpr double dAlpha1 = 2.6;						// Alpha1 parameter.
			constexpr double dAlpha2 = 3.0;						// Alpha2 parameter.
			constexpr double dVc = 0.35;						// Threshold value for dVc.
			double dK = _dLw / std::pow( 1.0 + _dB, dAlpha1 );	// Coefficient for normalization.

			if ( _dVal < dVc ) {
				return dK * std::pow( dVc + _dB, (dAlpha1 - dAlpha2) ) * std::pow( _dVal + _dB, dAlpha2 );
			}
			return dK * std::pow( _dVal + _dB, dAlpha1 );
		}

		/**
		 * The inverse of CrtProperToLinear().
		 * 
		 * \param _dVal The value to convert.
		 * \param _dLw Screen luminance for white, reference setting is LW = 100 cd/m2.
		 * \param _dB Variable for black level lift (legacy "brightness" control).
		 *	The value of _dB is set so that the calculated luminance can be the same as the
		 *	measurement data at input signal level 0.0183 (= (80-64)/876).
		 *	The value of _dB changes depending on "brightness" control.
		 * \return Returns the corresponding value along a decent CRT curve.
		 **/
		static inline double LSN_FASTCALL					LinearToCrtProper( double _dVal, double _dLw = 1.0, double _dB = 0.0181 ) {
			constexpr double dAlpha1 = 2.6;						// Alpha1 parameter.
			constexpr double dAlpha2 = 3.0;						// Alpha2 parameter.
			constexpr double dVc = 0.35;						// Threshold value for dVc.
			double dK = _dLw / std::pow( 1.0 + _dB, dAlpha1 );	// Coefficient for normalization.

			_dVal /= dK;
			if ( _dVal < std::pow( dVc + _dB, dAlpha1 ) ) {
				return std::pow( _dVal / std::pow( dVc + _dB, (dAlpha1 - dAlpha2) ), 1.0 / dAlpha2 ) - _dB;
			}
			return std::pow( _dVal, 1.0 / dAlpha1 ) - _dB;
		}

		/**
		 * A proper CRT curve based on measurements.
		 * 
		 * \param _dVal The value to convert.
		 * \return Returns the color value converted to linear space.
		 **/
		static inline double LSN_FASTCALL					CrtProper2ToLinear( double _dVal ) {
			constexpr double dAlpha = 0.1115721959217312597711924126997473649680614471435546875;
			constexpr double dBeta = 1.1115721959217312875267680283286608755588531494140625;
			constexpr double dCut = 0.0912863421177801115380390228892792947590351104736328125;
			if ( _dVal >= 0.36 ) { return std::pow( _dVal, 2.31 ); }
			double dFrac = _dVal / 0.36;
			return ((_dVal <= dCut ?
				_dVal / 4.0 :
				std::pow( (_dVal + dAlpha) / dBeta, 1.0 / 0.45 ))
				* (1.0 - dFrac))
				+ (dFrac * std::pow( _dVal, 2.31 ));
		}

		/**
		 * The inverse of CrtProper2ToLinear().
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the corresponding value along a decent CRT curve.
		 */
		static inline double LSN_FASTCALL					LinearToCrtProper2( double _dVal ) {
			constexpr double dAlpha = 0.1115721959217312597711924126997473649680614471435546875;
			constexpr double dBeta = 1.1115721959217312875267680283286608755588531494140625;
			constexpr double dCut = 0.022821585529445027884509755722319823689758777618408203125;
			if ( _dVal >= 0.36 ) { return std::pow( _dVal, 1.0 / 2.31 ); }
			double dFrac = _dVal / 0.36;
			return ((_dVal <= dCut ?
				_dVal * 4.0 :
				dBeta * std::pow( _dVal, 0.45 ) - dAlpha)
				* (1.0 - dFrac))
				+ (dFrac * std::pow( _dVal, 1.0 / 2.31 ));
		}

		/**
		 * Converts XYZ values to chromaticities.
		 * 
		 * \param _fX The input X.
		 * \param _fY The input Y.
		 * \param _fZ The input Z.
		 * \param _fChromaX The output chromaticity X.
		 * \param _fChromaY The output chromaticity Y.
		 **/
		static inline void									XYZtoChromaticity( float _fX, float _fY, float _fZ, float &_fChromaX, float &_fChromaY ) {
			float fX = _fX / _fY;
			constexpr float dY = 1.0f;
			float dZ = _fZ / _fY;

			_fChromaX = fX / (fX + dY + dZ);
			_fChromaY = dY / (fX + dY + dZ);
		}

		/**
		 * Converts chromaticities to XYZ values.
		 * 
		 * \param _fChromaX The input chromaticity X.
		 * \param _fChromaY The input chromaticity Y.
		 * \param _fY0 The input XYZ Y value.
		 * \param _fX0 The output XYZ Z value.
		 * \param _fZ0 The output XYZ Z value.
		 **/
		static void											ChromaticityToXYZ( float _fChromaX, float _fChromaY, float _fY0, float &_fX0, float &_fZ0 ) {
			_fX0 = _fChromaX * (_fY0 / _fChromaY);
			_fZ0 = (1.0f - _fChromaX - _fChromaY) * (_fY0 / _fChromaY);
		}

		/**
		 * Integer-based bilinear sampling.
		 *
		 * \param _ui32A The upper-left color.  0xAARRGGBB, though color order doesn't actually matter.
		 * \param _ui32B The upper-right color.  0xAARRGGBB, though color order doesn't actually matter.
		 * \param _ui32C The bottom-left color.  0xAARRGGBB, though color order doesn't actually matter.
		 * \param _ui32D The bottom-right color.  0xAARRGGBB, though color order doesn't actually matter.
		 * \param _ui32FactorX The horizontal interpolation factor (A -> B and C -> D).  0-256.
		 * \param _ui32FactorY The vertical interpolation factor (A -> C and B -> D).  0-256.
		 * \return Returns the 0xAARRGGBB (though color order doesn't actually matter) color resulting from bilinear interpolation.
		 */
		static LSN_FORCEINLINE uint32_t						BiLinearSample_Int( uint32_t _ui32A, uint32_t _ui32B, uint32_t _ui32C, uint32_t _ui32D, uint32_t _ui32FactorX, uint32_t _ui32FactorY ) {
			// Mercilessly, brutally ripped from:
			//	https://stackoverflow.com/questions/14659612/sse-bilinear-interpolation
			constexpr uint32_t ui32MaskRb = 0x00FF00FF;
			constexpr uint32_t ui32MaskAg = 0xFF00FF00;

			if LSN_UNLIKELY( _ui32A == _ui32B && _ui32C == _ui32D && _ui32A == _ui32D ) { return _ui32A; }

			const uint32_t ui32Arb        =   _ui32A & ui32MaskRb;
			const uint32_t ui32Crb        =   _ui32C & ui32MaskRb;
			const uint32_t ui32Aag        =   _ui32A & ui32MaskAg;
			const uint32_t ui32Cag        =   _ui32C & ui32MaskAg;

			const uint32_t ui32Rbdx1      =  (_ui32B & ui32MaskRb) - ui32Arb;
			const uint32_t ui32Rbdx2      =  (_ui32D & ui32MaskRb) - ui32Crb;
			const uint32_t ui32Agdx1      = ((_ui32B & ui32MaskAg) >> 8) - (ui32Aag >> 8);
			const uint32_t ui32Agdx2      = ((_ui32D & ui32MaskAg) >> 8) - (ui32Cag >> 8);

			const uint32_t ui32Rb1        = (ui32Arb      + ((ui32Rbdx1 * _ui32FactorX) >> 8)) & ui32MaskRb;
			const uint32_t ui32Ag1        = (ui32Aag      + ((ui32Agdx1 * _ui32FactorX)     )) & ui32MaskAg;
			const uint32_t ui32Rbdy       = ((ui32Crb     + ((ui32Rbdx2 * _ui32FactorX) >> 8)) & ui32MaskRb)         - ui32Rb1;
			const uint32_t ui32Agdy       = (((ui32Cag    + ((ui32Agdx2 * _ui32FactorX)     )) & ui32MaskAg) >> 8)   - (ui32Ag1 >> 8);

			const uint32_t ui32Rb         = (ui32Rb1 + ((ui32Rbdy * _ui32FactorY) >> 8)) & ui32MaskRb;
			const uint32_t ui32Ag         = (ui32Ag1 + ((ui32Agdy * _ui32FactorY)     )) & ui32MaskAg;

			return ui32Ag | ui32Rb;
		}

		/**
		 * Integer-based linear interpolation between 2 ARGB values (0xAARRGGBB, though color order doesn't actually matter).
		 *
		 * \param _ui32A The left color.  0xAARRGGBB, though color order doesn't actually matter.
		 * \param _ui32B The right color.  0xAARRGGBB, though color order doesn't actually matter.
		 * \param _ui32FactorX The interpolation factor (A -> B).  0-256, such that 0 = _ui32A and 256 = _ui32B.
		 * \return Returns the 0xAARRGGBB (though color order doesn't actually matter) color resulting from linear interpolation.
		 */
		static LSN_FORCEINLINE uint32_t						LinearSample_Int( uint32_t _ui32A, uint32_t _ui32B, uint32_t _ui32FactorX ) {
			constexpr uint32_t ui32MaskRb = 0x00FF00FF;
			constexpr uint32_t ui32MaskAg = 0xFF00FF00;

			if LSN_UNLIKELY( _ui32A == _ui32B ) { return _ui32A; }

			const uint32_t ui32Arb        =   _ui32A & ui32MaskRb;
			const uint32_t ui32Aag        =   _ui32A & ui32MaskAg;

			const uint32_t ui32Rbdx1      =  (_ui32B & ui32MaskRb) - ui32Arb;
			const uint32_t ui32Agdx1      = ((_ui32B & ui32MaskAg) >> 8) - (ui32Aag >> 8);

			const uint32_t ui32Rb1        = (ui32Arb + ((ui32Rbdx1 * _ui32FactorX) >> 8)) & ui32MaskRb;
			const uint32_t ui32Ag1        = (ui32Aag + ((ui32Agdx1 * _ui32FactorX)     )) & ui32MaskAg;

			const uint32_t ui32Rb         = ui32Rb1 & ui32MaskRb;
			const uint32_t ui32Ag         = ui32Ag1 & ui32MaskAg;

			return ui32Ag | ui32Rb;
		}

		/**
		 * 64-bit integer-based linear interpolation between 2 pairs of ARGB values (0xAARRGGBBAARRGGBB, though color order doesn't actually matter).
		 *
		 * \param _ui64A The left color.  0xAARRGGBBAARRGGBB, though color order doesn't actually matter.
		 * \param _ui64B The right color.  0xAARRGGBBAARRGGBB, though color order doesn't actually matter.
		 * \param _ui64FactorX The interpolation factor (A -> B).  0-256, such that 0 = _ui32A and 256 = _ui32B.
		 * \return Returns the 0xAARRGGBBAARRGGBB (though color order doesn't actually matter) color resulting from linear interpolation.
		 */
		static LSN_FORCEINLINE uint64_t						LinearSample_Int64( uint64_t _ui64A, uint64_t _ui64B, uint64_t _ui64FactorX ) {
			constexpr uint64_t ui64MaskRb = 0x00FF00FF00FF00FF;
			constexpr uint64_t ui64MaskAg = 0xFF00FF00FF00FF00;

			if LSN_UNLIKELY( _ui64A == _ui64B ) { return _ui64A; }

			const uint64_t ui64Arb        =   _ui64A & ui64MaskRb;
			const uint64_t ui64Aag        =   _ui64A & ui64MaskAg;

			const uint64_t ui64Rbdx1      =  (_ui64B & ui64MaskRb) - ui64Arb;
			const uint64_t ui64Agdx1      = ((_ui64B & ui64MaskAg) >> 8) - (ui64Aag >> 8);

			const uint64_t ui64Rb1        = (ui64Arb + ((ui64Rbdx1 * _ui64FactorX) >> 8)) & ui64MaskRb;
			const uint64_t ui64Ag1        = (ui64Aag + ((ui64Agdx1 * _ui64FactorX)     )) & ui64MaskAg;

			const uint64_t ui64Rb         = ui64Rb1 & ui64MaskRb;
			const uint64_t ui64Ag         = ui64Ag1 & ui64MaskAg;

			return ui64Ag | ui64Rb;
		}

#ifdef __SSE4_1__
		/**
		 * 128-bit integer-based linear interpolation between 2 sets of 4 ARGB values (0xAARRGGBBAARRGGBB, though color order doesn't actually matter).
		 * 
		 * \param _pui32A The left 8 colors.  0xAARRGGBBAARRGGBB, though color order doesn't actually matter.
		 * \param _pui32B The right 8 colors.  0xAARRGGBBAARRGGBB, though color order doesn't actually matter.
		 * \param _pui32Result The destination where the interpolated 8 colors go.
		 * \param _ui32FactorX The interpolation factor (A -> B).  0-256, such that 0 = _ui32A and 256 = _ui32B.
		 **/
		static LSN_FORCEINLINE void							LinearSample_SSE4( const uint32_t * _pui32A, const uint32_t * _pui32B, uint32_t * _pui32Result,  uint32_t _ui32FactorX ) {
			__m128i mA					= _mm_loadu_si128( reinterpret_cast<const __m128i *>(_pui32A) );
			__m128i mB					= _mm_loadu_si128( reinterpret_cast<const __m128i *>(_pui32B) );

			// Early out if both colors are the same.
			if LSN_UNLIKELY( _mm_test_all_zeros( _mm_xor_si128( mA, mB ), _mm_set1_epi32( -1 ) ) ) {
				_mm_storeu_si128( reinterpret_cast<__m128i *>(_pui32Result), mA );
				return;
			}
			
			// Define masks.
			__m128i mMaskRB				= _mm_set1_epi32( 0x00FF00FF );		// Mask for Red and Blue.
			__m128i mMaskAG				= _mm_set1_epi32( 0xFF00FF00 );		// Mask for Alpha and Green.

			// Factors for interpolation.
			uint32_t mInverseFactorX	= 256 - _ui32FactorX;
			__m128i mFactor				= _mm_set1_epi32( _ui32FactorX );
			__m128i mInverseFactor		= _mm_set1_epi32( mInverseFactorX );

			// Separate channels.
			__m128i mArb				= _mm_and_si128( mA, mMaskRB );
			__m128i mBrb				= _mm_and_si128( mB, mMaskRB );
			__m128i mAag				= _mm_and_si128( mA, mMaskAG );
			__m128i mBag				= _mm_and_si128( mB, mMaskAG );

			// Scale channels by factor.
			__m128i mScaledArb			= _mm_mullo_epi32( mArb, mInverseFactor );
			__m128i mScaledBrb			= _mm_mullo_epi32( mBrb, mFactor );
			__m128i mScaledAag			= _mm_mullo_epi32( _mm_srli_epi32( mAag, 8 ), mInverseFactor );
			__m128i mScaledBag			= _mm_mullo_epi32( _mm_srli_epi32( mBag, 8 ), mFactor );

			// Sum and shift back if necessary.
			__m128i mRbSum				= _mm_add_epi32( mScaledArb, mScaledBrb );
			__m128i mAgSum				= _mm_add_epi32( mScaledAag, mScaledBag );

			// Reconstruct the final RGBA.
			__m128i mRbResult			= _mm_and_si128( _mm_srli_epi32( mRbSum, 8 ), mMaskRB );
			__m128i mAgResult			= _mm_slli_epi32( _mm_and_si128( _mm_srli_epi32( mAgSum, 8 ), mMaskRB ), 8 );


			// Combine channels and return.
			__m128i mResult				= _mm_or_si128( mRbResult, mAgResult );

			_mm_storeu_si128( reinterpret_cast<__m128i *>(_pui32Result), mResult );
		}
#endif	// #ifdef __SSE4_1__

#ifdef __AVX2__
		/**
		 * 256-bit integer-based linear interpolation between 2 sets of 8 ARGB values (0xAARRGGBBAARRGGBB, though color order doesn't actually matter).
		 * 
		 * \param _mA The left 8 colors.  0xAARRGGBBAARRGGBB, though color order doesn't actually matter.
		 * \param _mB The right 8 colors.  0xAARRGGBBAARRGGBB, though color order doesn't actually matter.
		 * \param _pui32Result The destination where the interpolated 8 colors go.
		 * \param _ui32FactorX The interpolation factor (A -> B).  0-256, such that 0 = _mA and 256 = _mB.
		 **/
		static LSN_FORCEINLINE void							LinearSample_AVX2( const __m256i &_mA, const __m256i &_mB, uint32_t * _pui32Result,  uint32_t _ui32FactorX ) {
			constexpr uint32_t mMaskRb	= 0x00FF00FF;
			constexpr uint32_t mMaskAg	= 0xFF00FF00;

			__m256i mMaskRb256			= _mm256_set1_epi32( mMaskRb );
			__m256i mMaskAg256			= _mm256_set1_epi32( mMaskAg );
			__m256i mFactorX			= _mm256_set1_epi32( _ui32FactorX );
			__m256i mFactorXCompliment	= _mm256_set1_epi32( 256 - _ui32FactorX );

			// Separate the channels.
			__m256i mArb				= _mm256_and_si256( _mA, mMaskRb256 );
			__m256i mAag				= _mm256_and_si256( _mA, mMaskAg256 );
			__m256i mBrb				= _mm256_and_si256( _mB, mMaskRb256 );
			__m256i mBag				= _mm256_and_si256( _mB, mMaskAg256 );

			// Interpolate R and B channels.
			__m256i mRbInterp			= _mm256_add_epi32(
				_mm256_mullo_epi32( mArb, mFactorXCompliment ),
				_mm256_mullo_epi32( mBrb, mFactorX )
			);
			mRbInterp					= _mm256_srli_epi32( mRbInterp, 8 );
			mRbInterp					= _mm256_and_si256( mRbInterp, mMaskRb256 );

			// Interpolate A and G channels (factor is applied after shifting right by 8 to simulate division by 256).
			__m256i mAgInterp			= _mm256_add_epi32(
				_mm256_mullo_epi32(_mm256_srli_epi32( mAag, 8 ), mFactorXCompliment ),
				_mm256_mullo_epi32(_mm256_srli_epi32( mBag, 8 ), mFactorX )
			);
			mAgInterp					= _mm256_and_si256( mAgInterp, mMaskAg256 );

			// Combine interpolated channels.
			__m256i mResult				= _mm256_or_si256( mRbInterp, mAgInterp );

			// Store the result.
			_mm256_storeu_si256( reinterpret_cast<__m256i *>(_pui32Result), mResult );
		}

		/**
		 * 256-bit integer-based linear interpolation between 2 sets of 8 ARGB values (0xAARRGGBBAARRGGBB, though color order doesn't actually matter).
		 * 
		 * \param _mA The left 8 colors.  0xAARRGGBBAARRGGBB, though color order doesn't actually matter.
		 * \param _mB The right 8 colors.  0xAARRGGBBAARRGGBB, though color order doesn't actually matter.
		 * \param _pui32Result The destination where the interpolated 8 colors go.
		 * \param _mFactorX The interpolation factor (A -> B).  0-256, such that 0 = _mA and 256 = _mB.
		 **/
		static LSN_FORCEINLINE void							LinearSample_AVX2( const __m256i &_mA, const __m256i &_mB, uint32_t * _pui32Result, __m256i _mFactorX ) {
			constexpr uint32_t mMaskRb	= 0x00FF00FF;
			constexpr uint32_t mMaskAg	= 0xFF00FF00;

			__m256i mMaskRb256			= _mm256_set1_epi32( mMaskRb );
			__m256i mMaskAg256			= _mm256_set1_epi32( mMaskAg );
			__m256i mFactorXCompliment	= _mm256_sub_epi32( _mm256_set1_epi32( 256 ), _mFactorX );

			// Separate the channels.
			__m256i mArb				= _mm256_and_si256( _mA, mMaskRb256 );
			__m256i mAag				= _mm256_and_si256( _mA, mMaskAg256 );
			__m256i mBrb				= _mm256_and_si256( _mB, mMaskRb256 );
			__m256i mBag				= _mm256_and_si256( _mB, mMaskAg256 );

			// Interpolate R and B channels.
			__m256i mRbInterp			= _mm256_add_epi32(
				_mm256_mullo_epi32( mArb, mFactorXCompliment ),
				_mm256_mullo_epi32( mBrb, _mFactorX )
			);
			mRbInterp					= _mm256_srli_epi32( mRbInterp, 8 );
			mRbInterp					= _mm256_and_si256( mRbInterp, mMaskRb256 );

			// Interpolate A and G channels (factor is applied after shifting right by 8 to simulate division by 256).
			__m256i mAgInterp			= _mm256_add_epi32(
				_mm256_mullo_epi32(_mm256_srli_epi32( mAag, 8 ), mFactorXCompliment ),
				_mm256_mullo_epi32(_mm256_srli_epi32( mBag, 8 ), _mFactorX )
			);
			mAgInterp					= _mm256_and_si256( mAgInterp, mMaskAg256 );

			// Combine interpolated channels.
			__m256i mResult				= _mm256_or_si256( mRbInterp, mAgInterp );

			// Store the result.
			_mm256_storeu_si256( reinterpret_cast<__m256i *>(_pui32Result), mResult );
		}
#endif	// #ifdef __AVX2__

#ifdef __AVX512F__
		/**
		 * 512-bit integer-based linear interpolation between 2 sets of 16 ARGB values (0xAARRGGBBAARRGGBB, though color order doesn't actually matter).
		 * 
		 * \param _mA The left 16 colors.  0xAARRGGBBAARRGGBB, though color order doesn't actually matter.
		 * \param _mB The right 16 colors.  0xAARRGGBBAARRGGBB, though color order doesn't actually matter.
		 * \param _pui32Result The destination where the interpolated 8 colors go.
		 * \param _ui32FactorX The interpolation factor (A -> B).  0-256, such that 0 = _mA and 256 = _mB.
		 **/
		static LSN_FORCEINLINE void							LinearSample_AVX512( const __m512i &_mA, const __m512i &_mB, uint32_t * _pui32Result, uint32_t _ui32FactorX ) {
			constexpr uint32_t mMaskRb	= 0x00FF00FF;
			constexpr uint32_t mMaskAg	= 0xFF00FF00;

			__m512i mMaskRb512			= _mm512_set1_epi32( mMaskRb );
			__m512i mMaskAg512			= _mm512_set1_epi32( mMaskAg );
			__m512i mFactorX			= _mm512_set1_epi32( _ui32FactorX );
			__m512i mFactorXCompliment	= _mm512_set1_epi32( 256 - _ui32FactorX );

			// Separate the channels.
			__m512i mArb				= _mm512_and_si512( _mA, mMaskRb512 );
			__m512i mAag				= _mm512_and_si512( _mA, mMaskAg512 );
			__m512i mBrb				= _mm512_and_si512( _mB, mMaskRb512 );
			__m512i mBag				= _mm512_and_si512( _mB, mMaskAg512 );

			// Interpolate R and B channels.
			__m512i mRbInterp			= _mm512_add_epi32(
				_mm512_mullo_epi32( mArb, mFactorXCompliment ),
				_mm512_mullo_epi32( mBrb, mFactorX )
			);
			mRbInterp					= _mm512_srli_epi32( mRbInterp, 8 );
			mRbInterp					= _mm512_and_si512( mRbInterp, mMaskRb512 );

			// Interpolate A and G channels (factor is applied after shifting right by 8 to simulate division by 256).
			__m512i mAgInterp			= _mm512_add_epi32(
				_mm512_mullo_epi32(_mm512_srli_epi32( mAag, 8 ), mFactorXCompliment ),
				_mm512_mullo_epi32(_mm512_srli_epi32( mBag, 8 ), mFactorX )
			);
			mAgInterp					= _mm512_and_si512( mAgInterp, mMaskAg512 );

			// Combine interpolated channels.
			__m512i mResult				= _mm512_or_si512( mRbInterp, mAgInterp );

			// Store the result.
			_mm512_storeu_si512( reinterpret_cast<__m512i *>(_pui32Result), mResult );
		}

		/**
		 * 512-bit integer-based linear interpolation between 2 sets of 16 ARGB values (0xAARRGGBBAARRGGBB, though color order doesn't actually matter).
		 * 
		 * \param _mA The left 16 colors.  0xAARRGGBBAARRGGBB, though color order doesn't actually matter.
		 * \param _mB The right 16 colors.  0xAARRGGBBAARRGGBB, though color order doesn't actually matter.
		 * \param _pui32Result The destination where the interpolated 8 colors go.
		 * \param _mFactorX The interpolation factor (A -> B).  0-256, such that 0 = _mA and 256 = _mB.
		 **/
		static LSN_FORCEINLINE void							LinearSample_AVX512( const __m512i &_mA, const __m512i &_mB, uint32_t * _pui32Result, __m512i _mFactorX ) {
			constexpr uint32_t mMaskRb	= 0x00FF00FF;
			constexpr uint32_t mMaskAg	= 0xFF00FF00;

			__m512i mMaskRb512			= _mm512_set1_epi32( mMaskRb );
			__m512i mMaskAg512			= _mm512_set1_epi32( mMaskAg );
			__m512i mFactorXCompliment	= _mm512_sub_epi32( _mm512_set1_epi32( 256 ), _mFactorX );

			// Separate the channels.
			__m512i mArb				= _mm512_and_si512( _mA, mMaskRb512 );
			__m512i mAag				= _mm512_and_si512( _mA, mMaskAg512 );
			__m512i mBrb				= _mm512_and_si512( _mB, mMaskRb512 );
			__m512i mBag				= _mm512_and_si512( _mB, mMaskAg512 );

			// Interpolate R and B channels.
			__m512i mRbInterp			= _mm512_add_epi32(
				_mm512_mullo_epi32( mArb, mFactorXCompliment ),
				_mm512_mullo_epi32( mBrb, _mFactorX )
			);
			mRbInterp					= _mm512_srli_epi32( mRbInterp, 8 );
			mRbInterp					= _mm512_and_si512( mRbInterp, mMaskRb512 );

			// Interpolate A and G channels (factor is applied after shifting right by 8 to simulate division by 256).
			__m512i mAgInterp			= _mm512_add_epi32(
				_mm512_mullo_epi32(_mm512_srli_epi32( mAag, 8 ), mFactorXCompliment ),
				_mm512_mullo_epi32(_mm512_srli_epi32( mBag, 8 ), _mFactorX )
			);
			mAgInterp					= _mm512_and_si512( mAgInterp, mMaskAg512 );

			// Combine interpolated channels.
			__m512i mResult				= _mm512_or_si512( mRbInterp, mAgInterp );

			// Store the result.
			_mm512_storeu_si512( reinterpret_cast<__m512i *>(_pui32Result), mResult );
		}
#endif	// #ifdef __AVX512F__

		/**
		 * Performs integer-based linear interpolation across a row of ARGB pixels.
		 *
		 * \param _pui32SrcRow The input row.
		 * \param _pui32DstRow The output row.
		 * \param _pui32Factors The interpolation factors, one for each output sample.
		 * \param _ui32SrcW The input width in pixels.
		 * \param _ui32DstW The output width in pixels and the number of values to which _pui8Factors points.
		 */
		static LSN_FORCEINLINE void							LinearInterpolateRow_Int( const uint32_t * _pui32SrcRow, uint32_t * _pui32DstRow, const uint32_t * _pui32Factors, uint32_t _ui32SrcW, uint32_t _ui32DstW ) {
			// Let's be dope and prefetch the middle of the buffers.
			LSN_PREFETCH_LINE( _pui32SrcRow + (_ui32SrcW >> 1) );
			uint32_t X = 0;
#ifdef __AVX512F__
			if ( IsAvx512BWSupported() ) {
				constexpr uint32_t ui32RegSize = sizeof( __m512i ) / sizeof( uint32_t );
				if LSN_LIKELY( _ui32DstW >= ui32RegSize ) { 
					uint32_t ui32Total = _ui32DstW - ui32RegSize;
					while ( X <= ui32Total ) {
						// Load sixteen factors.
						__m512i vFactors = _mm512_loadu_si512( reinterpret_cast<const __m512i *>( _pui32Factors + X ) );
						// Compute source index: ui32SrcX = factor >> 8.
						__m512i vSrcX = _mm512_srli_epi32( vFactors, 8 );
        
						// Gather A = _pui32SrcRow[vSrcX].
						__m512i vA = _mm512_i32gather_epi32( vSrcX, reinterpret_cast<const int *>( _pui32SrcRow ), 4 );
        
						// Gather B = _pui32SrcRow[vSrcX + 1].
						__m512i vSrcXplus = _mm512_add_epi32( vSrcX, _mm512_set1_epi32( 1 ) );
						__m512i vB = _mm512_i32gather_epi32( vSrcXplus, reinterpret_cast<const int *>( _pui32SrcRow ), 4 );
						// If vSrcX equals (_ui32SrcW - 1), set B to 0.
						__m512i vLastIndex = _mm512_set1_epi32( _ui32SrcW - 1 );
						__mmask16 k = _mm512_cmpeq_epi32_mask( vSrcX, vLastIndex );
						vB = _mm512_mask_set1_epi32( vB, k, 0 );
        
						// Extract the interpolation factor (T) = factor & 0xFF.
						__m512i vT = _mm512_and_epi32( vFactors, _mm512_set1_epi32( 0xFF ) );
        
						// Perform the per-pixel linear interpolation on 16 pixels.
						LinearSample_AVX512( vA, vB, _pui32DstRow + X, vT );

						X += ui32RegSize;
					}
				}
			}
#endif	// #ifdef __AVX512F__

#ifdef __AVX2__
			if LSN_LIKELY( IsAvx2Supported() ) {
				constexpr uint32_t ui32RegSize = sizeof( __m256i ) / sizeof( uint32_t );
				if LSN_LIKELY( _ui32DstW >= ui32RegSize ) { 
					uint32_t ui32Total = _ui32DstW - ui32RegSize;
					while ( X <= ui32Total ) {
						// Load eight factors.
						__m256i vFactors	= _mm256_loadu_si256( reinterpret_cast<const __m256i *>(_pui32Factors + X) );
						// Compute source index: ui32SrcX = factor >> 8.
						__m256i vSrcX		= _mm256_srli_epi32( vFactors, 8 );
        
						// Gather A = _pui32SrcRow[vSrcX].
						__m256i vA			= _mm256_i32gather_epi32( reinterpret_cast<const int *>(_pui32SrcRow), vSrcX, 4 );
        
						// Gather B = _pui32SrcRow[vSrcX+1].
						__m256i vSrcXplus	= _mm256_add_epi32( vSrcX, _mm256_set1_epi32( 1 ) );
						__m256i vB			= _mm256_i32gather_epi32( reinterpret_cast<const int *>(_pui32SrcRow), vSrcXplus, 4 );
						// If vSrcX equals (_ui32SrcW - 1), set B to 0.
						__m256i vLastIndex	= _mm256_set1_epi32( _ui32SrcW - 1 );
						__m256i vEqMask		= _mm256_cmpeq_epi32( vSrcX, vLastIndex );
						vB					= _mm256_blendv_epi8( vB, _mm256_setzero_si256(), vEqMask );

						// Extract the interpolation factor (T) = factor & 0xFF.
						__m256i vT			= _mm256_and_si256( vFactors, _mm256_set1_epi32( 0xFF ) );
						LinearSample_AVX2( vA, vB, _pui32DstRow + X, vT );
						X += ui32RegSize;
					}
				}
			}
#endif	// #ifdef __AVX2__
			while ( X < _ui32DstW ) {
				uint32_t ui32SrcX = _pui32Factors[X] >> 8;

				uint32_t ui32A = _pui32SrcRow[ui32SrcX];
				uint32_t ui32B = (ui32SrcX == _ui32SrcW - 1) ? 0x00000000 : _pui32SrcRow[ui32SrcX+1];

				_pui32DstRow[X] = CUtilities::LinearSample_Int( ui32A, ui32B, _pui32Factors[X] & 0xFF );
				++X;
			}
		}

		/**
		 * Performs 64-bit integer-based linear interpolation of 2 rows of pixels.
		 *
		 * \param _pui32SrcRow0 The first input row.
		 * \param _pui32SrcRow1 The next input row down.
		 * \param _pui32DstRow The output row.
		 * \param _ui32Width The width of the rows in pixels.
		 * \param _ui32Factor The mix factor for combining the rows.
		 */
		static LSN_FORCEINLINE void							LinearInterpCombineRows_Int( const uint32_t * _pui32SrcRow0, const uint32_t * _pui32SrcRow1, uint32_t * _pui32DstRow, uint32_t _ui32Width, uint32_t _ui32Factor ) {
			if LSN_UNLIKELY( _ui32Factor == 0 ) {
				std::memcpy( _pui32DstRow, _pui32SrcRow0, _ui32Width * sizeof( uint32_t ) );
			}
			else {
#ifdef __AVX512F__
				if ( IsAvx512BWSupported() ) {
					while ( _ui32Width >= 16 ) {
						__m512i mA					= _mm512_loadu_si512( reinterpret_cast<const __m512i *>(_pui32SrcRow0) );
						__m512i mB					= _mm512_loadu_si512( reinterpret_cast<const __m512i *>(_pui32SrcRow1) );
						LinearSample_AVX512( mA, mB, _pui32DstRow, _ui32Factor );
						_ui32Width -= 16;
						if LSN_UNLIKELY( !_ui32Width ) { return; }
						_pui32SrcRow0 += 16;
						_pui32SrcRow1 += 16;
						_pui32DstRow += 16;
					}
					goto NoSimd;
				}
#endif	// #ifdef __AVX512F__
#ifdef __AVX2__
				if ( IsAvx2Supported() ) {
					while ( _ui32Width >= 8 ) {
						__m256i mA					= _mm256_loadu_si256( reinterpret_cast<const __m256i *>(_pui32SrcRow0) );
						__m256i mB					= _mm256_loadu_si256( reinterpret_cast<const __m256i *>(_pui32SrcRow1) );
						LinearSample_AVX2( mA, mB, _pui32DstRow, _ui32Factor );
						_ui32Width -= 8;
						if LSN_UNLIKELY( !_ui32Width ) { return; }
						_pui32SrcRow0 += 8;
						_pui32SrcRow1 += 8;
						_pui32DstRow += 8;
					}
					goto NoSimd;
				}
#endif	// #ifdef __AVX2__
#ifdef __SSE4_1__
				if ( IsSse4Supported() ) {
					while ( _ui32Width >= 4 ) {
						LinearSample_SSE4( _pui32SrcRow0, _pui32SrcRow1, _pui32DstRow, _ui32Factor );
						_ui32Width -= 4;
						//if ( !_ui32Width ) { return; }
						_pui32SrcRow0 += 4;
						_pui32SrcRow1 += 4;
						_pui32DstRow += 4;
					}
				}
#endif	// #ifdef __SSE4_1__
			NoSimd :
				while ( _ui32Width >= 2 ) {
					(*reinterpret_cast<uint64_t *>(_pui32DstRow)) = LinearSample_Int64( (*reinterpret_cast<const uint64_t *>(_pui32SrcRow0)), (*reinterpret_cast<const uint64_t *>(_pui32SrcRow1)), _ui32Factor );
					_ui32Width -= 2;
					//if ( !_ui32Width ) { return; }
					_pui32SrcRow0 += 2;
					_pui32SrcRow1 += 2;
					_pui32DstRow += 2;
				}

				while ( _ui32Width-- ) {
					(*_pui32DstRow++) = LinearSample_Int( (*_pui32SrcRow0++), (*_pui32SrcRow1++), _ui32Factor );
				}
			}
		}

		/**
		 * Adds 2 32-bit RGBA colors together using integer operations.
		 *
		 * \param _ui32Color0 The left operand.
		 * \param _ui32Color1 The right operand.
		 * \return Returns the clamped result of adding the colors together.  Each value is clamped between 0x00 and 0xFF inclusively.
		 */
		static LSN_FORCEINLINE uint32_t						AddArgb( uint32_t _ui32Color0, uint32_t _ui32Color1 ) {
			uint32_t ui32Ag0 = _ui32Color0 & 0xFF00FF00;
			uint32_t ui32Rb0 = _ui32Color0 & 0x00FF00FF;
			uint64_t ui64Ag1 = (_ui32Color1 & 0xFF00FF00ULL) + ui32Ag0;
			uint32_t ui32Rb1 = (_ui32Color1 & 0x00FF00FF) + ui32Rb0;
			uint32_t ui32AgOver = uint32_t( ((ui64Ag1 & 0x100010000ULL) * 0xFF) >> 8 );
			uint32_t ui32RbOver = ((ui32Rb1 & 0x01000100) * 0xFF) >> 8;
			return (uint32_t( ui64Ag1 | ui32AgOver ) & 0xFF00FF00) | ((ui32Rb1 | ui32RbOver) & 0x00FF00FF);
		}

		/**
		 * Gets a mask for shifting a 32-bit RGBA value right by a given number of bits.
		 *
		 * \return Returns a mask for shifting a 32-bit RGBA value right by a given number of bits.
		 */
		template <size_t _stShift>
		static constexpr uint32_t							ArgbShiftMask_Int() {
			return 0xFFFFFFFF & ~(((0x01010101 << _stShift) - 0x01010101));
		}

		/**
		 * Gets a mask for shifting a 64-bit RGBARGBA value right by a given number of bits.
		 *
		 * \return Returns a mask for shifting a 64-bit RGBARGBA value right by a given number of bits.
		 */
		template <size_t _stShift>
		static constexpr uint64_t							ArgbShiftMask_Int64() {
			return 0xFFFFFFFFFFFFFFFFULL & ~((0x0101010101010101ULL << _stShift) - 0x0101010101010101ULL);
		}

		/**
		 * Shifts a 32-bit RGBA value right a given number of places.
		 *
		 * \param _ui32Val The value to shift.
		 * \return Returns the shifted RGBA value such that each component is clamped to 0 after the shift.
		 */
		template <size_t _stShift>
		static LSN_FORCEINLINE uint32_t						ShiftArgbRight_Int( uint32_t _ui32Val ) {
			return (_ui32Val & ArgbShiftMask_Int<_stShift>()) >> _stShift;
		}

		/**
		 * Shifts a 64-bit RGBA value right a given number of places.
		 *
		 * \param _ui64Val The value to shift.
		 * \return Returns the shifted RGBA value such that each component is clamped to 0 after the shift.
		 */
		template <size_t _stShift>
		static LSN_FORCEINLINE uint64_t						ShiftArgbRight_Int64( uint64_t _ui64Val ) {
			return (_ui64Val & ArgbShiftMask_Int64<_stShift>()) >> _stShift;
		}

		/**
		 * Divides a 32-bit RGBA value by a constant
		 *
		 * \param _ui32Color The value to divide.
		 * \return Returns the RGBA value with each component divided by the given value.
		 */
		template <uint8_t _ui8Div>
		static LSN_FORCEINLINE uint32_t						DivArgb_Int( uint32_t _ui32Color ) {
			return 0;
		}

		/**
		 * Applies a quik phospher decay to the given 32-bit pixels.
		 *
		 * \param _pui32Src The pixels to decay.
		 * \param _ui32Width The row stride, in pixels, of the input/output buffer.
		 * \param _ui32Height The number of rows in the input/output buffer.
		 */
		static LSN_FORCEINLINE void							DecayArgb( uint32_t * _pui32Src, uint32_t _ui32Width, uint32_t _ui32Height ) {
			uint32_t ui32Total = _ui32Width * _ui32Height;
			uint64_t * pui64Pixels = reinterpret_cast<uint64_t *>(_pui32Src);
			uint32_t ui32Total2 = ui32Total >> 1;
			for ( uint32_t I = 0; I < ui32Total2; ++I ) {
				uint64_t ui64Tmp = pui64Pixels[I] & 0x00FFFFFF00FFFFFFULL;
				pui64Pixels[I] = /*((ui64Tmp >> 1) & 0x007F7F7F007F7F7FULL) +*/
					((ui64Tmp >> 2) & 0x003F3F3F003F3F3FULL) +
					((ui64Tmp >> 3) & 0x001F1F1F001F1F1FULL) +
					((ui64Tmp >> 4) & 0x000F0F0F000F0F0FULL);
			}
			for ( uint32_t I = ui32Total2 << 1; I < ui32Total; ++I ) {
				uint32_t ui32Tmp = _pui32Src[I] & 0x00FFFFFF;
				_pui32Src[I] = /*((ui32Tmp >> 1) & 0x7F7F7F) +*/
					((ui32Tmp >> 2) & 0x3F3F3F) +
					/*((ui32Tmp >> 3) & 0x1F1F1F) +*/
					((ui32Tmp >> 4) & 0x0F0F0F);
			}
		}

		/**
		 * Produces a sampling factor for bi-linear filtering.  Bottom 8 bits are the fraction between samples, and the rest of the upper bits are the index (X) of the
		 *	first sample to be used in the interpolation between sample[X] and sample[X+1].
		 *
		 * \param _ui32SrcLen The width/height of the source image.
		 * \param _ui32DstLen The width/height of the destination image.
		 * \param _ui32Idx The index of the destination sample for which to generate sampler factors.  In the range [0..(_ui32DstLen-1)].
		 * \return Returns the 8-bit fixed-point sampling factor where the bottom 8 bits are a fraction between sample[X] and sample[X+1] and the remaining upper bits
		 *	are the X index of the samples between which to interpolate.
		 */
		static LSN_FORCEINLINE uint32_t						SamplingFactor_BiLinear( uint32_t _ui32SrcLen, uint32_t _ui32DstLen, uint32_t _ui32Idx ) {
			return ((_ui32SrcLen * _ui32Idx) << 8) / _ui32DstLen;
		}

		/**
		 * Produces a sharper sampling factor for "scanline" filtering.  Bottom 8 bits are the fraction between samples, and the rest of the upper bits are the index (X) of the
		 *	first sample to be used in the interpolation between sample[X] and sample[X+1].
		 *
		 * \param _ui32SrcLen The width/height of the source image.
		 * \param _ui32DstLen The width/height of the destination image.
		 * \param _ui32Idx The index of the destination sample for which to generate sampler factors.  In the range [0..(_ui32DstLen-1)].
		 * \return Returns the 8-bit fixed-point sampling factor where the bottom 8 bits are a fraction between sample[X] and sample[X+1] and the remaining upper bits
		 *	are the X index of the samples between which to interpolate.
		 */
		static LSN_FORCEINLINE uint32_t						SamplingFactor_Scanline( uint32_t _ui32SrcLen, uint32_t _ui32DstLen, uint32_t _ui32Idx ) {
			uint32_t ui32Factor = ((_ui32SrcLen * _ui32Idx) << 8) / _ui32DstLen;
			uint32_t ui32Idx = ui32Factor >> 8;
			uint32_t ui32Frac = uint32_t( std::max( 0, int32_t( ((ui32Factor & 0xFF) << 1) - 0xFF ) ) );
			return (ui32Idx << 8) | ui32Frac;
		}

		/**
		 * Produces a sharper sampling factor for "scanline" filtering.  Bottom 8 bits are the fraction between samples, and the rest of the upper bits are the index (X) of the
		 *	first sample to be used in the interpolation between sample[X] and sample[X+1].
		 *
		 * \param _ui32SrcLen The width/height of the source image.
		 * \param _ui32DstLen The width/height of the destination image.
		 * \param _ui32Idx The index of the destination sample for which to generate sampler factors.  In the range [0..(_ui32DstLen-1)].
		 * \return Returns the 8-bit fixed-point sampling factor where the bottom 8 bits are a fraction between sample[X] and sample[X+1] and the remaining upper bits
		 *	are the X index of the samples between which to interpolate.
		 */
		static LSN_FORCEINLINE uint32_t						SamplingFactor_Scanline_Sharp( uint32_t _ui32SrcLen, uint32_t _ui32DstLen, uint32_t _ui32Idx ) {
			uint32_t ui32Factor = ((_ui32SrcLen * _ui32Idx) << 8) / _ui32DstLen;
			uint32_t ui32Idx = ui32Factor >> 8;
			uint32_t ui32Frac = uint32_t( std::max( 0, int32_t( ((ui32Factor & 0xFF) * 2.2) - 306 ) ) );
			return (ui32Idx << 8) | ui32Frac;
		}

		/**
		 * Deconstructs a ZIP file name formatted as zipfile{name}.  If not a ZIP file, the file name is extracted.
		 * 
		 * \param _s16Path The input file path to be deconstructed.
		 * \param _fpPaths The deconstructed file paths.
		 **/
		static void											DeconstructFilePath( const std::u16string &_s16Path, LSN_FILE_PATHS &_fpPaths );

		/**
		 * Creates an array of deconstructed file paths.
		 * 
		 * \param _s16Paths The input array of paths to be deconstructed.
		 * \return Returns an array of deconstructed file paths.
		 **/
		static std::vector<LSN_FILE_PATHS>					DeconstructFilePaths( const std::vector<std::u16string> &_s16Paths );

		/**
		 * Finds the first duplicated name in the list and then returns an array of indices of each file name that is a duplucate of the first-found duplicate name.
		 * 
		 * \param _s16Paths The input array of paths.
		 * \param _vIndices Returned indices of paths that match.
		 * \return Returns true if any file names are duplicates of each other when compared with case-insensitivity.
		 **/
		static bool											DuplicateFiles( const std::vector<CUtilities::LSN_FILE_PATHS> &_s16Paths, std::vector<size_t> &_vIndices );
		
		/**
		 * Performs a case-insensitive string compare against 2 UTF-16 strings.  Returns true if they are equal.
		 *
		 * \param _u16Str0 The left operand.
		 * \param _u16Str1 The right operand.
		 * \return Returns true if the given strings are equal as compared via a case-insensitive UTF-16 compare.
		 **/
		static bool											StringCmpUtf16_IgnoreCase( const std::u16string &_u16Str0, const std::u16string &_u16Str1 );

		/**
		 * Copies the last folder in the path given in the first string to the start of the 2nd string.
		 * 
		 * \param _u16Folders The folder path.
		 * \param _u16Path The file name.
		 **/
		static void											CopyLastFolderToFileName( std::u16string &_u16Folders, std::u16string &_u16Path );

		/**
		 * Gets the volume of a sample using Audacity's Studio Fade Out routine.
		 * 
		 * \param _tFraction The amount of fade to apply from 0 to 1.
		 * \return Returns the fade amount from 1 to 0 as _tFraction goes from 0 to 1.
		 **/
		template <typename _tType = float>
		static inline _tType								StudioFadeOut( _tType _tFraction ) {
			if LSN_UNLIKELY( _tFraction <= _tType( 0.0 ) ) { return _tType( 1.0 ); }
			if LSN_UNLIKELY( _tFraction >= _tType( 1.0 ) || std::isnan<_tType>( _tFraction ) ) { return _tType( 0.0 ); }
			return _tType( 1.0 ) - (_tType( 3.0 ) * _tFraction * _tFraction - _tType( 2.0 ) * _tFraction * _tFraction * _tFraction);
		}

#ifdef __AVX512F__
		/**
		 * Horizontally adds all the floats in a given AVX-512 register.
		 * 
		 * \param _mReg The register containing all of the values to sum.
		 * \return Returns the sum of all the floats in the given register.
		 **/
		static inline double								HorizontalSum( const __m512d &_mReg ) {
			return _mm512_reduce_add_pd( _mReg );
		}

		/**
		 * Horizontally adds all the floats in a given AVX-512 register.
		 * 
		 * \param _mReg The register containing all of the values to sum.
		 * \return Returns the sum of all the floats in the given register.
		 **/
		static inline float									HorizontalSum( const __m512 &_mReg ) {
			return _mm512_reduce_add_ps( _mReg );
		}

		/**
		 * Sums a pair of arrays of floats to a given destination (unaligned).
		 * 
		 * \param _pfOp0 THe left array of operands.
		 * \param _pfOp1 The right array of operands.
		 * \param _pfOut The output array.
		 * \param _sTotal The number of floats in each array.
		 **/
		static inline void									SumArray_AVX512_U( const float * _pfOp0, const float * _pfOp1, float * _pfOut, size_t _sTotal ) {
			size_t I = 0;

			// Prefetch the first set of data.
			LSN_PREFETCH_LINE( _pfOp0 );
			LSN_PREFETCH_LINE( _pfOp1 );

			// Process 16 floats at a time.  But like, 2 16-at-a-times at a time.
			for ( ; I + 31 < _sTotal; I += 32 ) {
				// Prefetch the next set of data.
				LSN_PREFETCH_LINE( _pfOp0 + 32 );
				LSN_PREFETCH_LINE( _pfOp1 + 32 );

				// Load and add the first 16 floats.
				__m512 mA1 = _mm512_loadu_ps( &_pfOp0[I] );
				__m512 mB1 = _mm512_loadu_ps( &_pfOp1[I] );
				__m512 mC1 = _mm512_add_ps( mA1, mB1 );
				_mm512_storeu_ps( &_pfOut[I], mC1 );

				// Load and add the next 16 floats.
				__m512 mA2 = _mm512_loadu_ps( &_pfOp0[I+16] );
				__m512 mB2 = _mm512_loadu_ps( &_pfOp1[I+16] );
				__m512 mC2 = _mm512_add_ps( mA2, mB2 );
				_mm512_storeu_ps( &_pfOut[I+16], mC2 );
			}

			// Handle remaining elements.
			for ( ; I < _sTotal; ++I ) {
				_pfOut[I] = _pfOp0[I] + _pfOp1[I];
			}
		}

		/**
		 * Sums a pair of arrays of floats to a given destination (aligned).
		 * 
		 * \param _pfOp0 THe left array of operands.
		 * \param _pfOp1 The right array of operands.
		 * \param _pfOut The output array.
		 * \param _sTotal The number of floats in each array.
		 **/
		static inline void									SumArray_AVX512( const float * _pfOp0, const float * _pfOp1, float * _pfOut, size_t _sTotal ) {
			size_t I = 0;

			// Prefetch the first set of data.
			LSN_PREFETCH_LINE( _pfOp0 );
			LSN_PREFETCH_LINE( _pfOp1 );

			// Process 16 floats at a time.  But like, 2 16-at-a-times at a time.
			for ( ; I + 31 < _sTotal; I += 32 ) {
				// Prefetch the next set of data.
				LSN_PREFETCH_LINE( _pfOp0 + 32 );
				LSN_PREFETCH_LINE( _pfOp1 + 32 );

				// Load and add the first 16 floats.
				__m512 mA1 = _mm512_load_ps( &_pfOp0[I] );
				__m512 mB1 = _mm512_load_ps( &_pfOp1[I] );
				__m512 mC1 = _mm512_add_ps( mA1, mB1 );
				_mm512_store_ps( &_pfOut[I], mC1 );

				// Load and add the next 16 floats.
				__m512 mA2 = _mm512_load_ps( &_pfOp0[I+16] );
				__m512 mB2 = _mm512_load_ps( &_pfOp1[I+16] );
				__m512 mC2 = _mm512_add_ps( mA2, mB2 );
				_mm512_store_ps( &_pfOut[I+16], mC2 );
			}

			// Handle remaining elements.
			for ( ; I < _sTotal; ++I ) {
				_pfOut[I] = _pfOp0[I] + _pfOp1[I];
			}
		}
#endif	// #ifdef __AVX512F__

#ifdef __AVX__
		/**
		 * Horizontally adds all the floats in a given AVX register.
		 * 
		 * \param _mReg The register containing all of the values to sum.
		 * \return Returns the sum of all the floats in the given register.
		 **/
		static inline double								HorizontalSum( const __m256d &_mReg ) {
			__m256d mT1 = _mm256_hadd_pd( _mReg, _mReg );
			__m128d mT2 = _mm256_extractf128_pd( mT1, 1 );
			__m128d mT3 = _mm256_castpd256_pd128( mT1 );
			return _mm_cvtsd_f64( _mm_add_pd( mT2, mT3 ) );
		}

		/**
		 * Horizontally adds all the floats in a given AVX register.
		 * 
		 * \param _mReg The register containing all of the values to sum.
		 * \return Returns the sum of all the floats in the given register.
		 **/
		static inline float									HorizontalSum( const __m256 &_mReg ) {
			__m256 mTmp = _mm256_add_ps( _mReg, _mm256_permute2f128_ps( _mReg, _mReg, 1 ) );
			mTmp = _mm256_hadd_ps( mTmp, mTmp );
			mTmp = _mm256_hadd_ps( mTmp, mTmp );
			return _mm256_cvtss_f32( mTmp );
		}

		/**
		 * Sums a pair of arrays of floats to a given destination (unaligned).
		 * 
		 * \param _pfOp0 THe left array of operands.
		 * \param _pfOp1 The right array of operands.
		 * \param _pfOut The output array.
		 * \param _sTotal The number of floats in each array.
		 **/
		static inline void									SumArray_AVX_U( const float * _pfOp0, const float * _pfOp1, float * _pfOut, size_t _sTotal ) {
			size_t I = 0;

			// Prefetch the first set of data.
			LSN_PREFETCH_LINE( _pfOp0 );
			LSN_PREFETCH_LINE( _pfOp1 );

			// Process 8 floats at a time.  But like, 2 8-at-a-times at a time
			for ( ; I + 15 < _sTotal; I += 16 ) {
				// Prefetch the next set of data.
				LSN_PREFETCH_LINE( _pfOp0 + 16 );
				LSN_PREFETCH_LINE( _pfOp1 + 16 );

				// Load and add the first 8 floats
				__m256 mA1 = _mm256_loadu_ps( &_pfOp0[I] );
				__m256 mB1 = _mm256_loadu_ps( &_pfOp1[I] );
				__m256 mC1 = _mm256_add_ps( mA1, mB1 );
				_mm256_storeu_ps( &_pfOut[I], mC1 );

				// Load and add the next 8 floats
				__m256 mA2 = _mm256_loadu_ps( &_pfOp0[I+8] );
				__m256 mB2 = _mm256_loadu_ps( &_pfOp1[I+8] );
				__m256 mC2 = _mm256_add_ps( mA2, mB2 );
				_mm256_storeu_ps( &_pfOut[I+8], mC2 );
			}

			// Handle remaining elements.
			for ( ; I < _sTotal; ++I ) {
				_pfOut[I] = _pfOp0[I] + _pfOp1[I];
			}
		}

		/**
		 * Sums a pair of arrays of floats to a given destination (aligned).
		 * 
		 * \param _pfOp0 THe left array of operands.
		 * \param _pfOp1 The right array of operands.
		 * \param _pfOut The output array.
		 * \param _sTotal The number of floats in each array.
		 **/
		static inline void									SumArray_AVX( const float * _pfOp0, const float * _pfOp1, float * _pfOut, size_t _sTotal ) {
			size_t I = 0;

			// Prefetch the first set of data.
			LSN_PREFETCH_LINE( _pfOp0 );
			LSN_PREFETCH_LINE( _pfOp1 );

			// Process 8 floats at a time.  But like, 2 8-at-a-times at a time.
			for ( ; I + 15 < _sTotal; I += 16 ) {
				// Prefetch the next set of data.
				LSN_PREFETCH_LINE( _pfOp0 + 16 );
				LSN_PREFETCH_LINE( _pfOp1 + 16 );

				// Load and add the first 8 floats.
				__m256 mA1 = _mm256_load_ps( &_pfOp0[I] );
				__m256 mB1 = _mm256_load_ps( &_pfOp1[I] );
				__m256 mC1 = _mm256_add_ps( mA1, mB1 );
				_mm256_store_ps( &_pfOut[I], mC1 );

				// Load and add the next 8 floats.
				__m256 mA2 = _mm256_load_ps( &_pfOp0[I+8] );
				__m256 mB2 = _mm256_load_ps( &_pfOp1[I+8] );
				__m256 mC2 = _mm256_add_ps( mA2, mB2 );
				_mm256_store_ps( &_pfOut[I+8], mC2 );
			}

			// Handle remaining elements.
			for ( ; I < _sTotal; ++I ) {
				_pfOut[I] = _pfOp0[I] + _pfOp1[I];
			}
		}
#endif	// #ifdef __AVX__

#ifdef __SSE4_1__
		/**
		 * Horizontally adds all the floats in a given SSE register.
		 * 
		 * \param _mReg The register containing all of the values to sum.
		 * \return Returns the sum of all the floats in the given register.
		 **/
		static inline double								HorizontalSum( const __m128d &_mReg ) {
			__m128d mAddH1 = _mm_shuffle_pd( _mReg, _mReg, 0x1 );
			__m128d mAddH2 = _mm_add_pd( _mReg, mAddH1 );
			return _mm_cvtsd_f64( mAddH2 );
		}

		/**
		 * Horizontally adds all the floats in a given SSE register.
		 * 
		 * \param _mReg The register containing all of the values to sum.
		 * \return Returns the sum of all the floats in the given register.
		 **/
		static inline float									HorizontalSum( const __m128 &_mReg ) {
			__m128 mAddH1 = _mm_hadd_ps( _mReg, _mReg );
			__m128 mAddH2 = _mm_hadd_ps( mAddH1, mAddH1 );
			return _mm_cvtss_f32( mAddH2 );
		}

		/**
		 * Sums a pair of arrays of floats to a given destination (unaligned).
		 * 
		 * \param _pfOp0 THe left array of operands.
		 * \param _pfOp1 The right array of operands.
		 * \param _pfOut The output array.
		 * \param _sTotal The number of floats in each array.
		 **/
		static inline void									SumArray_SSE4_U( const float * _pfOp0, const float * _pfOp1, float * _pfOut, size_t _sTotal ) {
			size_t I = 0;

			// Prefetch the first set of data.
			LSN_PREFETCH_LINE( _pfOp0 );
			LSN_PREFETCH_LINE( _pfOp1 );

			// Process 4 floats at a time.  But like, 2 4-at-a-times at a time.
			for ( ; I + 7 < _sTotal; I += 8 ) {
				// Prefetch the next set of data.
				LSN_PREFETCH_LINE( _pfOp0 + 8 );
				LSN_PREFETCH_LINE( _pfOp1 + 8 );

				// Load and add the first 4 floats.
				__m128 mA1 = _mm_loadu_ps( &_pfOp0[I] );
				__m128 mB1 = _mm_loadu_ps( &_pfOp1[I] );
				__m128 mC1 = _mm_add_ps( mA1, mB1 );
				_mm_storeu_ps( &_pfOut[I], mC1 );

				// Load and add the next 4 floats.
				__m128 mA2 = _mm_loadu_ps( &_pfOp0[I+4] );
				__m128 mB2 = _mm_loadu_ps( &_pfOp1[I+4] );
				__m128 mC2 = _mm_add_ps( mA2, mB2 );
				_mm_storeu_ps( &_pfOut[I+4], mC2 );
			}

			// Handle remaining elements.
			for ( ; I < _sTotal; ++I ) {
				_pfOut[I] = _pfOp0[I] + _pfOp1[I];
			}
		}

		/**
		 * Sums a pair of arrays of floats to a given destination (aligned).
		 * 
		 * \param _pfOp0 THe left array of operands.
		 * \param _pfOp1 The right array of operands.
		 * \param _pfOut The output array.
		 * \param _sTotal The number of floats in each array.
		 **/
		static inline void									SumArray_SSE4( const float * _pfOp0, const float * _pfOp1, float * _pfOut, size_t _sTotal ) {
			size_t I = 0;

			// Prefetch the first set of data.
			LSN_PREFETCH_LINE( _pfOp0 );
			LSN_PREFETCH_LINE( _pfOp1 );

			// Process 4 floats at a time.  But like, 2 4-at-a-times at a time.
			for ( ; I + 7 < _sTotal; I += 8 ) {
				// Prefetch the next set of data.
				LSN_PREFETCH_LINE( _pfOp0 + 8 );
				LSN_PREFETCH_LINE( _pfOp1 + 8 );

				// Load and add the first 4 floats.
				__m128 mA1 = _mm_load_ps( &_pfOp0[I] );
				__m128 mB1 = _mm_load_ps( &_pfOp1[I] );
				__m128 mC1 = _mm_add_ps( mA1, mB1 );
				_mm_store_ps( &_pfOut[I], mC1 );

				// Load and add the next 4 floats.
				__m128 mA2 = _mm_load_ps( &_pfOp0[I+4] );
				__m128 mB2 = _mm_load_ps( &_pfOp1[I+4] );
				__m128 mC2 = _mm_add_ps( mA2, mB2 );
				_mm_store_ps( &_pfOut[I+4], mC2 );
			}

			// Handle remaining elements.
			for ( ; I < _sTotal; ++I ) {
				_pfOut[I] = _pfOp0[I] + _pfOp1[I];
			}
		}
#endif	// #ifdef __SSE4_1__

		/**
		 * Sums a pair of arrays of floats to a given destination.
		 * 
		 * \param _pfOp0 THe left array of operands.
		 * \param _pfOp1 The right array of operands.
		 * \param _pfOut The output array.
		 * \param _sTotal The number of floats in each array.
		 **/
		static inline void									SumArray( const float * _pfOp0, const float * _pfOp1, float * _pfOut, size_t _sTotal ) {
			LSN_PREFETCH_LINE( _pfOp0 );
			LSN_PREFETCH_LINE( _pfOp1 );

			while ( _sTotal >= 8 ) {
				// Let’s do a loop of 8 at a time.
				_pfOut[0] = _pfOp0[0] + _pfOp1[0];
				_pfOut[1] = _pfOp0[1] + _pfOp1[1];
				_pfOut[2] = _pfOp0[2] + _pfOp1[2];
				_pfOut[3] = _pfOp0[3] + _pfOp1[3];
				_pfOut[4] = _pfOp0[4] + _pfOp1[4];

				LSN_PREFETCH_LINE( _pfOp0 + 8 );
				LSN_PREFETCH_LINE( _pfOp1 + 8 );

				_pfOut[5] = _pfOp0[5] + _pfOp1[5];
				_pfOut[6] = _pfOp0[6] + _pfOp1[6];
				_pfOut[7] = _pfOp0[7] + _pfOp1[7];
				_pfOp0 += 8;
				_pfOp0 += 8;
				_pfOp1 += 8;
				_sTotal -= 8;
			}
			// Finish the rest.
			for ( size_t I = 0; I < _sTotal; ++I ) {
				_pfOut[I] = _pfOp0[I] + _pfOp1[I];
			}
		}

		/**
		 * Convolvinate.
		 *
		 * \param _pdWeights The convolution weights.
		 * \param _pdValues The values to be convolved.
		 * \param _sTotal The total values to which _pdValues and _pdWeights point.
		 * \return Returns the summed weights * values.
		 **/
		static double										ConvolveAligned( const double * _pdWeights, const double * _pdValues, size_t _sTotal );

		/**
		 * Convolvinate using unaligned reads.
		 *
		 * \param _pdWeights The convolution weights.
		 * \param _pdValues The values to be convolved.
		 * \param _sTotal The total values to which _pdValues and _pdWeights point.
		 * \return Returns the summed weights * values.
		 **/
		static double										ConvolveUnaligned( const double * _pdWeights, const double * _pdValues, size_t _sTotal );

		/**
		 * Convoles a buffer of values in-place with the given coefficients.  Call within a try/catch block.
		 * 
		 * \tparam _tVecSrc The source/destination vector type.
		 * \tparam _tVecSinc The coefficients.
		 * \param _vSrc The input/output vector.
		 * \param _vFilter The coefficients to apply to _vSrc.
		 * \param _vtLeftPad The value to use to fill the empty space before the start of _vSrc.
		 * \param _vtRightPad The value to use to fill the empty space after the end of _vSrc.
		 * \throws std::runtime_error on allocation error.
		 **/
		template <typename _tVecSrc, typename _tVecSinc>
		static inline void									ApplySincFilterInPlace( _tVecSrc &_vSrc, const _tVecSinc &_vFilter, _tVecSinc::value_type _vtLeftPad, _tVecSinc::value_type _vtRightPad ) {
			size_t sFilterSize = _vFilter.size();
			size_t sM = sFilterSize / 2;

			std::vector<typename _tVecSinc::value_type> vRingBuffer;
			vRingBuffer.resize( sFilterSize );
			for ( auto & I : vRingBuffer ) { I = _vtLeftPad; }

			size_t sFilterSizeM1 = sFilterSize - 1;
			size_t sTotalSize = _vSrc.size() + sFilterSizeM1;
			
			for ( size_t I = sM; I < sTotalSize; ++I ) {
				size_t sRingIdx = I % sFilterSize;
				size_t sSrcIdx = I - sM;
				vRingBuffer[sRingIdx] = sSrcIdx < _vSrc.size() ? _vSrc[sSrcIdx] : _vtRightPad;

				if LSN_LIKELY( I >= sFilterSizeM1 ) {
					sRingIdx = (I - sFilterSizeM1) % sFilterSize;
					// Convolve the ring buffer.
					double dSum = 0.0;
					for ( size_t J = 0; J < sFilterSize; ++J ) {
						size_t sIdx = (sRingIdx + J) % sFilterSize;
						dSum += vRingBuffer[sIdx] * _vFilter[J];
					}
					_vSrc[I-sFilterSizeM1] = dSum;
				}
			}
		}

		/**
		 * Standard sinc() function.
		 * 
		 * \param _dX The operand.
		 * \return Returns sin(x) / x.
		 **/
		static inline double								Sinc( double _dX ) {
			_dX *= std::numbers::pi;
			if ( _dX < 0.01 && _dX > -0.01 ) {
				return 1.0 + _dX * _dX * (-1.0 / 6.0 + _dX * _dX * (1.0 / 120.0));
			}

			return std::sin( _dX ) / _dX;
		}

		/**
		 * A helper function.
		 *
		 * \param _dX A happy parameter.
		 * \return Returns happiness.
		 */
		static inline double								Bessel0( double _dX ) {
			static const double dEspiRatio = 1.0e-16;
			double dSum = 1.0, dPow = 1.0, dDs = 1.0, dXh = _dX * 0.5;
			uint32_t ui32K = 0;

			while ( dDs > dSum * dEspiRatio ) {
				++ui32K;
				dPow *= (dXh / ui32K);
				dDs = dPow * dPow;
				dSum += dDs;
			}

			return dSum;
		}

		/**
		 * A helper function for the Kaiser filter.
		 *
		 * \param _dAlpha A happy parameter.
		 * \param _dHalfWidth A happy parameter.
		 * \param _dX A happy parameter.
		 * \return Returns happiness.
		 */
		static inline double								KaiserHelper( double _dAlpha, double _dHalfWidth, double _dX ) {
			const double dRatio = _dX / _dHalfWidth;
			return Bessel0( _dAlpha * std::sqrt( 1.0 - dRatio * dRatio ) ) / Bessel0( _dAlpha );
		}

		/**
		 * A helper function.
		 *
		 * \param _dX A happy parameter.
		 * \return Returns happiness.
		 */
		static float										BlackmanWindow( double _dX ) {
			return static_cast<float>(0.42659071 + 0.49656062 * std::cos( std::numbers::pi * _dX ) + 0.07684867 * std::cos( 2.0 * std::numbers::pi * _dX ));
		}

		/**
		 * A helper function for the Mitchell filter.
		 *
		 * \param _dT The filter input.
		 * \param _dB A happy parameter.
		 * \param _dC A happy parameter.
		 * \return Returns happiness.
		 */
		static inline float									MitchellFilterHelper( double _dT, double _dB, double _dC ) {
			double dTt = _dT * _dT;
			_dT = std::fabs( _dT );
			if ( _dT < 1.0 ) {
				_dT = (((12.0 - 9.0 * _dB - 6.0 * _dC) * (_dT * dTt))
					+ ((-18.0 + 12.0 * _dB + 6.0 * _dC) * dTt)
					+ (6.0 - 2.0 * _dB));
				return static_cast<float>(_dT / 6.0);
			}
			else if ( _dT < 2.0 ) {
				_dT = (((-1.0 * _dB - 6.0 * _dC) * (_dT * dTt))
					+ ((6.0 * _dB + 30.0 * _dC) * dTt)
					+ ((-12.0 * _dB - 48.0 * _dC) * _dT)
					+ (8.0 * _dB + 24.0 * _dC));

				return static_cast<float>(_dT / 6.0);
			}

			return 0.0f;
		}

		/**
		 * A Bartlett window.
		 * 
		 * \param _dT The filter input.
		 * \param _dM The window size.
		 * \return Returns the windowed value.
		 **/
		static inline float									BartlettWindow( double _dT, double _dM ) {
			--_dM;
			return static_cast<float>((2.0 / _dM) * ((_dM / 2.0) - std::abs( _dT - (_dM / 2.0) )));
		}

		/**
		 * Reject values below a specific epsilon.
		 *
		 * \param _dVal The value to test.
		 * \return Returns the given value if it is above a certain epsilon or 0.
		 */
		static inline float									Clean( double _dVal ) {
			static const double dEps = 0.0;//0.0000125;
			return ::fabs( _dVal ) >= dEps ? static_cast<float>(_dVal) : 0.0f;
		}

		/**
		 * The Lanczos filter function with X samples.
		 *
		 * \param _fT The value to filter.
		 * \param _fWidth The size of the filter kernel.
		 * \return Returns the filtered value.
		 */
		template <unsigned _uWidthD, unsigned _uWidthN>
		static inline float									LanczosXFilterFunc( float _fT, float _fWidth ) {
			_fT = std::fabsf( _fT );
			constexpr float fW = float( _uWidthD ) / float( _uWidthN );
			_fT = _fT / _fWidth * fW;
			if ( _fT <= fW ) {
				return static_cast<float>(Clean( Sinc( _fT ) * Sinc( _fT / fW ) ));
			}
			return 0.0f;
		}

		/**
		 * The Kaiser filter function.
		 *
		 * \param _fT The value to filter.
		 * \param _fWidth The size of the filter kernel.
		 * \return Returns the filtered value.
		 */
		static inline float									KaiserFilterFunc( float _fT, float _fWidth ) {
			_fT = std::fabsf( _fT );
			if ( _fT <= std::ceil( _fWidth ) ) {
				static const float fAtt = 40.0f;
				static const double dAlpha = std::exp( std::log( 0.58417 * (fAtt - 20.96) ) * 0.4 ) + 0.07886 * (fAtt - 20.96);
				return static_cast<float>(Clean( Sinc( _fT ) * KaiserHelper( dAlpha, double( _fWidth ), _fT ) ));
			}
			return 0.0f;
		}

		/**
		 * The Blackman filter function.
		 *
		 * \param _fT The value to filter.
		 * \param _fWidth The size of the filter kernel.
		 * \return Returns the filtered value.
		 */
		static inline float									BlackmanFilterFunc( float _fT, float _fWidth ) {
			_fT = std::fabsf( _fT );
			if ( _fT <= std::ceil( _fWidth ) ) {
				_fT = float( std::pow( _fT / _fWidth, 16.0 ) * _fWidth );
				return Clean( Sinc( _fT ) * BlackmanWindow( _fT / double( _fWidth ) ) );
			}
			return 0.0f;
		}

		/**
		 * The Gaussian filter function.
		 *
		 * \param _fT The value to filter.
		 * \param _fWidth The size of the filter kernel.
		 * \return Returns the filtered value.
		 */
		static inline float									GaussianXFilterFunc( float _fT, float _fWidth ) {
			_fT = std::fabsf( _fT );
			if ( _fT <= std::ceil( _fWidth ) ) {

				double dSigma = (_fWidth - 1.0 ) / 6.0;
				double dExpVal = -1 * ((_fT * _fT) / (2.0 * dSigma * 2.0 * dSigma));
				double dDivider = std::sqrt( 2.0 * std::numbers::pi * (dSigma * dSigma) );
				return static_cast<float>(std::exp( dExpVal ) / dDivider);

				//return Clean( std::exp( -2.0 * _fT * _fT ) * std::sqrt( 2.0 / std::numbers::pi ) * BlackmanWindow( _fT / double( _fWidth ) ) );
			}
			return 0.0f;
		}

		/**
		 * The Cardinal Spline Uniform filter function.
		 *
		 * \param _fT The value to filter.
		 * \param _fWidth The size of the filter kernel
		 * \return Returns the filtered value.
		 */
		static inline float									CardinalSplineUniformFilterFunc( float _fT, float _fWidth ) {
			return MitchellFilterHelper( _fT * 2.0 / _fWidth, 0.0, 1.0 );
		}

		/**
		 * The  Bartlett filter function.
		 *
		 * \param _fT The value to filter.
		 * \param _fWidth The size of the filter kernel
		 * \return Returns the filtered value.
		 */
		static inline float									BartlettFilterFunc( float _fT, float _fWidth ) {
			return BartlettWindow( (_fT + _fWidth) / (_fWidth * 2.0) * ((_fWidth * 2.0) - 1.0), _fWidth * 2.0 );
		}

		/**
		 * The box filter function.
		 *
		 * \param _fT The value to filter.
		 * \param _fWidth The size of the filter kernel.
		 * \return Returns the filtered value.
		 */
		static inline float									BoxFilterFunc( float _fT, float _fWidth ) {
			_fT = std::fabsf( _fT );
			return (_fT <= std::ceil( _fWidth )) ? 1.0f : 0.0f;
		}
		/**
		 * A custom filter with a flat middle and fall-offs on the sides.
		 * 
		 * \param _fT The value to filter.
		 * \param _fWidth The size of the filter kernel.
		 * \return Returns the filtered value.
		 **/
		template <unsigned _uNotchWTime100, unsigned _uPowTimes100>
		static inline float									CrtHumpFunc( float _fT, float _fWidth ) {
			_fT = std::fabsf( _fT );
			constexpr double dNotchW = double( _uNotchWTime100 ) / (2.0 * 100.0);
			if ( _fT <= dNotchW ) { return 1.0f; }
			double dTmp = _fT - dNotchW;
			double dNorm = dTmp / (_fWidth - dNotchW);
			return float( std::pow( 1.0 - std::min( dNorm, 1.0 ), (_uPowTimes100 / 100.0) ) );
		}

		/**
		 * Is AVX supported?
		 *
		 * \return Returns true if AVX is supported.
		 **/
		static inline bool									IsAvxSupported() {
#if defined( __i386__ ) || defined( __x86_64__ ) || defined( _MSC_VER )
			return CFeatureSet::AVX();
#else
			return false;
#endif	// #if defined( __i386__ ) || defined( __x86_64__ ) || defined( _MSC_VER )
		}

		/**
		 * Is AVX 2 supported?
		 *
		 * \return Returns true if AVX 2 is supported.
		 **/
		static inline bool									IsAvx2Supported() {
#if defined( __i386__ ) || defined( __x86_64__ ) || defined( _MSC_VER )
			return CFeatureSet::AVX2();
#else
			return false;
#endif	// #if defined( __i386__ ) || defined( __x86_64__ ) || defined( _MSC_VER )
		}

		/**
		 * Is FMA supported?
		 *
		 * \return Returns true if FMA is supported.
		 **/
		static inline bool									IsFmaSupported() {
#if defined( __i386__ ) || defined( __x86_64__ ) || defined( _MSC_VER )
			return CFeatureSet::FMA();
#else
			return false;
#endif	// #if defined( __i386__ ) || defined( __x86_64__ ) || defined( _MSC_VER )
		}

		/**
		 * Is AVX-512F supported?
		 *
		 * \return Returns true if AVX-512F is supported.
		 **/
		static inline bool									IsAvx512FSupported() {
#if defined( __i386__ ) || defined( __x86_64__ ) || defined( _MSC_VER )
			return CFeatureSet::AVX512F();
#else
			return false;
#endif	// #if defined( __i386__ ) || defined( __x86_64__ ) || defined( _MSC_VER )
		}

		/**
		 * Is AVX-512BW supported?
		 *
		 * \return Returns true if AVX-512BW is supported.
		 **/
		static inline bool									IsAvx512BWSupported() {
#if defined( __i386__ ) || defined( __x86_64__ ) || defined( _MSC_VER )
			return CFeatureSet::AVX512BW();
#else
			return false;
#endif	// #if defined( __i386__ ) || defined( __x86_64__ ) || defined( _MSC_VER )
		}

		/**
		 * Is SSE 4 supported?
		 *
		 * \return Returns true if SSE 4 is supported.
		 **/
		static inline bool									IsSse4Supported() {
#if defined( __i386__ ) || defined( __x86_64__ ) || defined( _MSC_VER )
			return CFeatureSet::SSE41();
#else
			return false;
#endif	// #if defined( __i386__ ) || defined( __x86_64__ ) || defined( _MSC_VER )
		}
		
		/**
		 * Converts a sample from a floating-point format to a uint8_t.  8-bit PCM data is expressed as an unsigned value over the range 0 to 255, 128 being an
		 *	audio output level of zero.
		 *
		 * \param _fSample The sample to convert.
		 * \return Returns the converted sample.
		 **/
		static inline uint8_t								SampleToUi8( float _fSample ) {
			float fClampedSample;
			if LSN_UNLIKELY( _fSample < -1.0f ) { fClampedSample = -1.0f; }
			else if LSN_UNLIKELY( _fSample > 1.0f ) { fClampedSample = 1.0f; }
			else { fClampedSample = _fSample; }
			//float fClampedSample = std::clamp( _fSample, -1.0f, 1.0f );
			float fScaledSample = (fClampedSample + 1.0f) * 0.5f * 255.0f;
			return static_cast<uint8_t>(std::round( fScaledSample ));
		}

		/**
		 * Converts a sample from a floating-point format to an int16_t.  16-bit PCM data is expressed as a signed value over the
		 *	range -32768 to 32767, 0 being an audio output level of zero.  Note that both -32768 and -32767 are -1.0; a proper
		 *	conversion never generates -32768.
		 *
		 * \param _fSample The sample to convert.
		 * \return Returns the converted sample.
		 **/
		static inline int16_t								SampleToI16( float _fSample ) {
			float fClampedSample;
			if LSN_UNLIKELY( _fSample < -1.0f ) { fClampedSample = -1.0f; }
			else if LSN_UNLIKELY( _fSample > 1.0f ) { fClampedSample = 1.0f; }
			else { fClampedSample = _fSample; }
			//float fClampedSample = std::clamp( _fSample, -1.0f, 1.0f );
			float fScaledSample = fClampedSample * 32767.0f;
			return static_cast<int16_t>(std::round( fScaledSample ));
		}

		/**
		 * Converts a sample from a floating-point format to an int16_t while applying error-diffusion dithering.  16-bit PCM data is expressed as a signed value over the
		 *	range -32768 to 32767, 0 being an audio output level of zero.  Note that both -32768 and -32767 are -1.0; a proper
		 *	conversion never generates -32768.
		 * 
		 * \param _fSample The sample to convert.
		 * \param _fError The running error state.
		 * \return Returns the converted sample.
		 **/
		static inline int16_t								SampleToI16_Dither( float _fSample, float &_fError ) {
			double dThis = _fSample + _fError;
			if LSN_UNLIKELY( dThis < -1.0 ) { dThis = -1.0; }
			else if LSN_UNLIKELY( dThis > 1.0 ) { dThis = 1.0; }
		
			int16_t i16Final = static_cast<int16_t>(std::round( static_cast<float>(dThis * 32767.0) ));

			double dScaled = double( i16Final );
			double dQuantized = dScaled * (1.0 / 32767.0);
			_fError = float( dThis - dQuantized );

			return i16Final;
		}

		/**
		 * Converts a sample from a floating-point format to an int16_t while applying error-diffusion dithering.  16-bit PCM data is expressed as a signed value over the
		 *	range -32768 to 32767, 0 being an audio output level of zero.  Note that both -32768 and -32767 are -1.0; a proper
		 *	conversion never generates -32768.
		 * 
		 * \param _dSample The sample to convert.
		 * \param _dError The running error state.
		 * \return Returns the converted sample.
		 **/
		static inline int16_t								SampleToI16_Dither( double _dSample, double &_dError ) {
			double dThis = _dSample + _dError;
			if LSN_UNLIKELY( dThis < -1.0 ) { dThis = -1.0; }
			else if LSN_UNLIKELY( dThis > 1.0 ) { dThis = 1.0; }
		
			int16_t i16Final = static_cast<int16_t>(std::round( static_cast<double>(dThis * 32767.0) ));

			double dScaled = double( i16Final );
			double dQuantized = dScaled * (1.0 / 32767.0);
			_dError = double( dThis - dQuantized );

			return i16Final;
		}
		
		/**
		 * Converts a sample from a floating-point format to an int32_t.  24-bit PCM data is expressed as a signed value over the
		 *	range -8388607 to 8388607, 0 being an audio output level of zero.  Note that both -8388608 and -8388607 are -1.0; a proper
		 *	conversion never generates -8388608.
		 *
		 * \param _fSample The sample to convert.
		 * \return Returns the converted sample.
		 **/
		static inline int32_t								SampleToI24( float _fSample ) {
			double dClampedSample;
			if LSN_UNLIKELY( _fSample < -1.0f ) { dClampedSample = -1.0; }
			else if LSN_UNLIKELY( _fSample > 1.0f ) { dClampedSample = 1.0; }
			else { dClampedSample = _fSample; }
			//float fClampedSample = std::clamp( _fSample, -1.0f, 1.0f );
			double dScaledSample = dClampedSample * 8388607.0;
			return static_cast<int32_t>(std::round( dScaledSample ));
		}

#ifdef __AVX2__
		/**
		 * Converts a sample from a floating-point format to a uint8_t.  8-bit PCM data is expressed as an unsigned value over the range 0 to 255, 128 being an
		 *	audio output level of zero.
		 * 
		 * \param _pfSample Pointer to the samples to convert.
		 * \param _pui8Dst Pointer to the output.
		 * \return Returns the converted sample.
		 **/
		static inline void									SampleToUi8_AVX2( const float * _pfSample, uint8_t * _pui8Dst ) {
			auto vSamples	= _mm256_loadu_ps( _pfSample );
			auto vClamped	= _mm256_max_ps( _mm256_set1_ps( -1.0f ), _mm256_min_ps( vSamples, _mm256_set1_ps( 1.0f ) ) );
			auto vScaled	= _mm256_mul_ps( _mm256_mul_ps( _mm256_add_ps( vClamped, _mm256_set1_ps( 1.0f ) ), _mm256_set1_ps( 0.5f ) ), _mm256_set1_ps( 255.0f ) );
			auto vRounded	= _mm256_round_ps( vScaled, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC );
			// Paack into 8-bit PCM values.
			auto vInt32Vals = _mm256_cvtps_epi32( vRounded );

			auto vLo		= _mm256_castsi256_si128( vInt32Vals );				// Lower 128 bits.
			auto vHi		= _mm256_extracti128_si256( vInt32Vals, 1 );			// Upper 128 bits.
			auto vPacked16	= _mm_packus_epi32( vLo, vHi );

			// Finally, pack the 16-bit integers into 8-bit integers.
			auto vPacked8	= _mm_packus_epi16( vPacked16, vPacked16 );
			_mm_storeu_epi8( _pui8Dst, vPacked8 );
		}

		/**
		 * Converts a sample from a floating-point format to an int16_t.  16-bit PCM data is expressed as a signed value over the
		 *	range -32768 to 32767, 0 being an audio output level of zero.  Note that both -32768 and -32767 are -1.0; a proper
		 *	conversion never generates -32768.
		 * 
		 * \param _pfSample Pointer to the samples to convert.
		 * \param _pi16Dst Pointer to the output.
		 * \return Returns the converted sample.
		 **/
		static inline void									SampleToI16_AVX2( const float * _pfSample, int16_t * _pi16Dst ) {
			auto vSamples	= _mm256_loadu_ps( _pfSample );
			auto vClamped	= _mm256_max_ps( _mm256_set1_ps( -1.0f ), _mm256_min_ps( vSamples, _mm256_set1_ps( 1.0f ) ) );
			auto vScaled	= _mm256_mul_ps( vClamped, _mm256_set1_ps( 32767.0f ) );
			auto vRounded	= _mm256_round_ps( vScaled, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC );

			auto vInt32Vals	= _mm256_cvtps_epi32( vRounded );
    
			// Extract the lower and upper 128-bit lanes.
			auto vLo		= _mm256_castsi256_si128( vInt32Vals );				// Lower 4 integers.
			auto vHi		= _mm256_extracti128_si256( vInt32Vals, 1 );			// Upper 4 integers.
			auto vPacked16	= _mm_packs_epi32( vLo, vHi );
    
			_mm_storeu_epi16( _pi16Dst, vPacked16 );
		}

		/**
		 * Converts a sample from a floating-point format to an int32_t.  24-bit PCM data is expressed as a signed value over the
		 *	range -8388607 to 8388607, 0 being an audio output level of zero.  Note that both -8388608 and -8388607 are -1.0; a proper
		 *	conversion never generates -8388608.
		 *
		 * \param _pfSample Pointer to the samples to convert.
		 * \param _pi32Dst Pointer to the output.  Must be aligned to a 32-byte boundary.
		 * \return Returns the converted sample.
		 **/
		static inline void									SampleToI24_AVX2( const float * _pfSample, int32_t * _pi32Dst ) {
			auto vSamples	= _mm256_loadu_ps( _pfSample );
			auto vClamped	= _mm256_max_ps( _mm256_set1_ps( -1.0f ), _mm256_min_ps( vSamples, _mm256_set1_ps( 1.0f ) ) );
			auto vScaled	= _mm256_mul_ps( vClamped, _mm256_set1_ps( 8388607.0f ) );
			auto vRounded	= _mm256_round_ps( vScaled, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC );

			auto vInt32Vals	= _mm256_cvtps_epi32( vRounded );
			_mm256_store_si256( reinterpret_cast<__m256i *>(_pi32Dst), vInt32Vals );
		}
#endif	// #ifdef __AVX2__

#ifdef __AVX512F__
		/**
		 * Converts a sample from a floating-point format to a uint8_t.  8-bit PCM data is expressed as an unsigned value over the range 0 to 255, 128 being an
		 *	audio output level of zero.
		 * 
		 * \param _pfSample Pointer to the samples to convert.
		 * \param _pui8Dst Pointer to the output.
		 * \return Returns the converted sample.
		 **/
		static inline void									SampleToUi8_AVX512( const float * _pfSample, uint8_t * _pui8Dst ) {
			auto vSamples   = _mm512_loadu_ps( _pfSample );
			auto vClamped   = _mm512_max_ps( _mm512_set1_ps( -1.0f ), _mm512_min_ps( vSamples, _mm512_set1_ps( 1.0f ) ) );
			auto vScaled    = _mm512_mul_ps( _mm512_mul_ps( _mm512_add_ps( vClamped, _mm512_set1_ps( 1.0f ) ), _mm512_set1_ps( 0.5f ) ), _mm512_set1_ps( 255.0f ) );
			auto vRounded	= _mm512_roundscale_ps( vScaled, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC );
			// Pack into 8-bit PCM values.
			auto vInt32Vals = _mm512_cvtps_epi32( vRounded );
			
			// Extract lower and upper 256-bit lanes.
			auto vLo256     = _mm512_castsi512_si256( vInt32Vals );					// Lower 256 bits.
			auto vHi256     = _mm512_extracti64x4_epi64( vInt32Vals, 1 );			// Upper 256 bits.
			auto vPacked16  = _mm256_packus_epi32( vLo256, vHi256 );				// 16-bit integers.
			
			// Pack the 16-bit integers into 8-bit integers.
			auto vPacked16Lo = _mm256_castsi256_si128( vPacked16 );
			auto vPacked16Hi = _mm256_extracti128_si256( vPacked16, 1 );
			auto vPacked8    = _mm_packus_epi16( vPacked16Lo, vPacked16Hi );
			_mm_storeu_si128( reinterpret_cast<__m128i *>( _pui8Dst ), vPacked8 );
		}

		/**
		 * Converts a sample from a floating-point format to an int16_t.  16-bit PCM data is expressed as a signed value over the
		 *	range -32768 to 32767, 0 being an audio output level of zero.  Note that both -32768 and -32767 are -1.0; a proper
		 *	conversion never generates -32768.
		 * 
		 * \param _pfSample Pointer to the samples to convert.
		 * \param _pi16Dst Pointer to the output.
		 * \return Returns the converted sample.
		 **/
		static inline void									SampleToI16_AVX512( const float * _pfSample, int16_t * _pi16Dst ) {
			auto vSamples   = _mm512_loadu_ps( _pfSample );
			auto vClamped   = _mm512_max_ps( _mm512_set1_ps( -1.0f ), _mm512_min_ps( vSamples, _mm512_set1_ps( 1.0f ) ) );
			auto vScaled    = _mm512_mul_ps( vClamped, _mm512_set1_ps( 32767.0f ) );
			auto vRounded	= _mm512_roundscale_ps( vScaled, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC );
			
			auto vInt32Vals = _mm512_cvtps_epi32( vRounded );
			// Extract lower and upper 256-bit lanes.
			auto vLo256     = _mm512_castsi512_si256( vInt32Vals );					// Lower 256 bits.
			auto vHi256     = _mm512_extracti64x4_epi64( vInt32Vals, 1 );			// Upper 256 bits.
			auto vPacked16  = _mm256_packs_epi32( vLo256, vHi256 );					// 16-bit integers.
			
			_mm256_storeu_si256( reinterpret_cast<__m256i *>( _pi16Dst ), vPacked16 );
		}

		/**
		 * Converts a sample from a floating-point format to an int32_t.  24-bit PCM data is expressed as a signed value over the
		 *	range -8388607 to 8388607, 0 being an audio output level of zero.  Note that both -8388608 and -8388607 are -1.0; a proper
		 *	conversion never generates -8388608.
		 *
		 * \param _pfSample Pointer to the samples to convert.
		 * \param _pi32Dst Pointer to the output.  Must be aligned to a 64-byte boundary.
		 * \return Returns the converted sample.
		 **/
		static inline void									SampleToI24_AVX512( const float * _pfSample, int32_t * _pi32Dst ) {
			auto vSamples   = _mm512_loadu_ps( _pfSample );
			auto vClamped   = _mm512_max_ps( _mm512_set1_ps( -1.0f ), _mm512_min_ps( vSamples, _mm512_set1_ps( 1.0f ) ) );
			auto vScaled    = _mm512_mul_ps( vClamped, _mm512_set1_ps( 8388607.0f ) );
			auto vRounded	= _mm512_roundscale_ps( vScaled, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC );
			
			auto vInt32Vals = _mm512_cvtps_epi32( vRounded );
			_mm512_store_si512( reinterpret_cast<__m512i *>( _pi32Dst ), vInt32Vals );
		}
#endif	// #ifdef __AVX512F__

		/**
		 * Gets a decay multiplier given a starting point, a target point, the duration to reach the target point in seconds, and the number of iterations per second to take.
		 * 
		 * \param _dStart The starting point of the decay.
		 * \param _dTarget The target point of the decay.
		 * \param _dTime The duration of the decay in seconds.
		 * \param _dRate The number of times per-second that the returned multiplier will be applied.
		 * \return Returns a multiplier that, when applied to _dStart repeatedly at _dRate, reaches _dTarget in _dTime seconds.
		 **/
		static double										DecayMultiplier( double _dStart, double _dTarget, double _dTime, double _dRate ) {
			double dSteps = _dRate * _dTime;
			return std::exp( std::log( _dTarget / _dStart ) / dSteps );
		}

		/**
		 * Generates uniformly distributed noise.
		 * 
		 * \param _pfTarget The buffer to fill with random values.
		 * \param _sSize The size of the buffer to fill in samples.
		 * \param _fAmplitude The noise amplitude.
		 **/
		static inline void									UniformNoise( float * _pfTarget, size_t _sSize, float _fAmplitude = 0.1f ) {
			std::random_device rdDev;
			std::mt19937 mGen( rdDev() );
			// Generate uniform noise in [-amplitude, +amplitude].
			std::uniform_real_distribution<float> urdDist( -_fAmplitude, _fAmplitude );

			while ( _sSize-- ) {
				_pfTarget[_sSize] = urdDist( mGen );
			}
		}

		/**
		 * Generates Gaussian noise.
		 * 
		 * \param _pfTarget The buffer to fill with random values.
		 * \param _sSize The size of the buffer to fill in samples.
		 * \param _fStdDev The standard deviation.
		 **/
		static inline void									GaussianNoise( float * _pfTarget, size_t _sSize, float _fStdDev = 0.05f ) {
			std::random_device rdDev;
			std::mt19937 mGen( rdDev() );
			std::normal_distribution<float> urdDist( 0.0f, _fStdDev );

			while ( _sSize-- ) {
				_pfTarget[_sSize] = urdDist( mGen );
			}
		}

		/**
		 * Fills the noise buffers with uniform noise.
		 * 
		 * \param _fAmplitude The noise amplitude.
		 **/
		static void inline									GenUniformNoise( float _fAmplitude = 0.1f ) {
			if LSN_LIKELY( !m_bNoiseIsGaussian && m_fLastNoiseParm == _fAmplitude ) { return; }
			for ( auto I = LSN_ELEMENTS( m_fNoiseBuffers ); I--; ) {
				UniformNoise( m_fNoiseBuffers[I], LSN_ELEMENTS( m_fNoiseBuffers[I] ), _fAmplitude );
			}
			m_bNoiseIsGaussian = false;
			m_fLastNoiseParm = _fAmplitude;
		}

		/**
		 * Fills the noise buffers with Gaussian noise.
		 * 
		 * \param _fStdDev The standard deviation.
		 **/
		static void inline									GenGaussianNoise( float _fStdDev = 0.05f ) {
			if LSN_LIKELY( m_bNoiseIsGaussian && m_fLastNoiseParm == _fStdDev ) { return; }
			for ( auto I = LSN_ELEMENTS( m_fNoiseBuffers ); I--; ) {
				GaussianNoise( m_fNoiseBuffers[I], LSN_ELEMENTS( m_fNoiseBuffers[I] ), _fStdDev );
			}
			m_bNoiseIsGaussian = true;
			m_fLastNoiseParm = _fStdDev;
		}

		/**
		 * Gets a pseudo-random number quickly.
		 * 
		 * \return Returns a random value using a super cheap and efficient random pattern.
		 **/
		static inline uint32_t								Rand() {
			uint32_t ui32Ret = m_ui32Rand;
			m_ui32Rand = bswap_32( 214019 * m_ui32Rand + 140327895 );
			return ui32Ret;
		}

		/**
		 * \brief Update a running average of the last N samples without a buffer.
		 *        Implements: avg_{k} = avg_{k-1} + (newSample - avg_{k-1})/N
		 * 
		 * \param _fPrevAvg  Previous average value.
		 * \param _fNew      New sample value.
		 * \param _sWindow   Number of items to average (N).  Must not be 0.
		 * \returns          Updated average over the implicit last N samples.
		 *
		 * This exponential‐style update approximates a moving average of window N
		 * without storing all samples.
		 */
		static float										UpdateRunningAvg( float _fPrevAvg, float _fNew, size_t _sWindow ) {
			return _fPrevAvg + (_fNew - _fPrevAvg) / float( _sWindow );
		}

		/**
		 * \brief Update a running average of the last N samples without a buffer.
		 *        Implements: avg_{k} = avg_{k-1} + (newSample - avg_{k-1})/N
		 * 
		 * \param _dPrevAvg  Previous average value.
		 * \param _dNew      New sample value.
		 * \param _sWindow   Number of items to average (N).  Must not be 0.
		 * \returns          Updated average over the implicit last N samples.
		 *
		 * This exponential‐style update approximates a moving average of window N
		 * without storing all samples.
		 */
		static double										UpdateRunningAvg( double _dPrevAvg, double _dNew, size_t _sWindow ) {
			return _dPrevAvg + (_dNew - _dPrevAvg) / double( _sWindow );
		}

		/**
		 * \brief Update a running average of the last N samples without a buffer.
		 *        Implements: avg_{k} = avg_{k-1} + (newSample - avg_{k-1})/N
		 * 
		 * \param _fPrevAvg  Previous average value.
		 * \param _fNew      New sample value.
		 * \param _fWindow   Number of items to average (N).  Must not be 0.
		 * \returns          Updated average over the implicit last N samples.
		 *
		 * This exponential‐style update approximates a moving average of window N
		 * without storing all samples.
		 */
		static float										UpdateRunningAvg( float _fPrevAvg, float _fNew, float _fWindow ) {
			return _fPrevAvg + (_fNew - _fPrevAvg) / _fWindow;
		}

		/**
		 * \brief Update a running average of the last N samples without a buffer.
		 *        Implements: avg_{k} = avg_{k-1} + (newSample - avg_{k-1})/N
		 * 
		 * \param _dPrevAvg  Previous average value.
		 * \param _dNew      New sample value.
		 * \param _dWindow   Number of items to average (N).  Must not be 0.
		 * \returns          Updated average over the implicit last N samples.
		 *
		 * This exponential‐style update approximates a moving average of window N
		 * without storing all samples.
		 */
		static double										UpdateRunningAvg( double _dPrevAvg, double _dNew, double _dWindow ) {
			return _dPrevAvg + (_dNew - _dPrevAvg) / _dWindow;
		}


		// == Members.
		LSN_ALIGN( 64 )
		static const float									m_fNtscLevels[16];							/**< Output levels for NTSC. */
		LSN_ALIGN( 64 )
		static const float									m_fPalLevels[16];							/**< Output levels for PAL. */
		LSN_ALIGN( 64 )
		static float										m_fNoiseBuffers[LSN_NOISE_BUFFERS][16];		/**< LSN_NOISE_BUFFERS noise buffers, each with 16 samples. */
		static uint32_t										m_ui32Rand;									/**< A quick pseudo-random value updated each time it is accessed via Rand(). */
		static bool											m_bNoiseIsGaussian;							/**< True if the last call was to GenGaussianNoise(). */
		static float										m_fLastNoiseParm;							/**< The last value passed to either GenUniformNoise() or GenGaussianNoise(). */
	};

}	// namespace lsn


#if 0
#include <immintrin.h>
#include <iostream>

/**
 * @brief Multiplies RGBA pixels by a 4x4 matrix using AVX2 intrinsics with FMA, keeping RGBA values interleaved on output.
 * @param rgba_input Pointer to input RGBA pixels (multiple of 2 pixels, 8 floats per 2 pixels).
 * @param matrix 4x4 transformation matrix (16 floats, row-major order).
 * @param rgba_output Pointer to output RGBA pixels (same size as input).
 * @param pixel_count Number of pixels to process (must be even).
 */
void multiply_rgba_by_matrix_avx2_fma(const float* rgba_input, const float* matrix, float* rgba_output, int pixel_count) {
    // Ensure that pixel_count is even
    if (pixel_count % 2 != 0) {
        std::cerr << "Pixel count must be an even number for AVX2 processing." << std::endl;
        return;
    }

    // Process two pixels (8 floats) at a time
    for (int i = 0; i < pixel_count; i += 2) {
        // Load 8 floats (2 RGBA pixels) into AVX2 register
        __m256 rgba = _mm256_loadu_ps(rgba_input + i * 4);

        // Deinterleave RGBA components
        __m256 r = _mm256_shuffle_ps(rgba, rgba, _MM_SHUFFLE(0,0,0,0));
        __m256 g = _mm256_shuffle_ps(rgba, rgba, _MM_SHUFFLE(1,1,1,1));
        __m256 b = _mm256_shuffle_ps(rgba, rgba, _MM_SHUFFLE(2,2,2,2));
        __m256 a = _mm256_shuffle_ps(rgba, rgba, _MM_SHUFFLE(3,3,3,3));

        // Load matrix rows and broadcast elements
        __m256 mat_row0 = _mm256_loadu_ps(matrix);        // Elements 0-7
        __m256 mat_row1 = _mm256_loadu_ps(matrix + 4);    // Elements 4-7
        __m256 mat_row2 = _mm256_loadu_ps(matrix + 8);    // Elements 8-11
        __m256 mat_row3 = _mm256_loadu_ps(matrix + 12);   // Elements 12-15

        // Compute new R component using FMA
        __m256 new_r = _mm256_fmadd_ps(r, _mm256_set1_ps(matrix[0]),
                            _mm256_fmadd_ps(g, _mm256_set1_ps(matrix[1]),
                                _mm256_fmadd_ps(b, _mm256_set1_ps(matrix[2]),
                                    _mm256_mul_ps(a, _mm256_set1_ps(matrix[3])))));

        // Compute new G component using FMA
        __m256 new_g = _mm256_fmadd_ps(r, _mm256_set1_ps(matrix[4]),
                            _mm256_fmadd_ps(g, _mm256_set1_ps(matrix[5]),
                                _mm256_fmadd_ps(b, _mm256_set1_ps(matrix[6]),
                                    _mm256_mul_ps(a, _mm256_set1_ps(matrix[7])))));

        // Compute new B component using FMA
        __m256 new_b = _mm256_fmadd_ps(r, _mm256_set1_ps(matrix[8]),
                            _mm256_fmadd_ps(g, _mm256_set1_ps(matrix[9]),
                                _mm256_fmadd_ps(b, _mm256_set1_ps(matrix[10]),
                                    _mm256_mul_ps(a, _mm256_set1_ps(matrix[11])))));

        // Compute new A component using FMA
        __m256 new_a = _mm256_fmadd_ps(r, _mm256_set1_ps(matrix[12]),
                            _mm256_fmadd_ps(g, _mm256_set1_ps(matrix[13]),
                                _mm256_fmadd_ps(b, _mm256_set1_ps(matrix[14]),
                                    _mm256_mul_ps(a, _mm256_set1_ps(matrix[15])))));

        // Interleave components back into RGBA format
        __m256 rg = _mm256_unpacklo_ps(new_r, new_g); // Interleave R and G
        __m256 ba = _mm256_unpacklo_ps(new_b, new_a); // Interleave B and A

        __m256 rgba_result = _mm256_unpacklo_pd(_mm256_castps_pd(rg), _mm256_castps_pd(ba));
        rgba_result = _mm256_castpd_ps(rgba_result);

        // Store the results back to output
        _mm256_storeu_ps(rgba_output + i * 4, rgba_result);
    }
}

#include <immintrin.h>
#include <iostream>

/**
 * @brief Multiplies RGBA pixels by a 4x4 matrix using AVX-512 intrinsics with FMA, keeping RGBA values interleaved on output.
 * @param rgba_input Pointer to input RGBA pixels (multiple of 4 pixels, 16 floats per 4 pixels).
 * @param matrix 4x4 transformation matrix (16 floats, row-major order).
 * @param rgba_output Pointer to output RGBA pixels (same size as input).
 * @param pixel_count Number of pixels to process (must be a multiple of 4).
 */
void multiply_rgba_by_matrix_avx512_fma(const float* rgba_input, const float* matrix, float* rgba_output, int pixel_count) {
    // Ensure that pixel_count is a multiple of 4
    if (pixel_count % 4 != 0) {
        std::cerr << "Pixel count must be a multiple of 4 for AVX-512 processing." << std::endl;
        return;
    }

    // Process four pixels (16 floats) at a time
    for (int i = 0; i < pixel_count; i += 4) {
        // Load 16 floats (4 RGBA pixels) into AVX-512 register
        __m512 rgba = _mm512_loadu_ps(rgba_input + i * 4);

        // Deinterleave RGBA components
        __m512 r = _mm512_permute_ps(rgba, _MM_PERM_AAAA);
        __m512 g = _mm512_permute_ps(rgba, _MM_PERM_BBBB);
        __m512 b = _mm512_permute_ps(rgba, _MM_PERM_CCCC);
        __m512 a = _mm512_permute_ps(rgba, _MM_PERM_DDDD);

        // Load matrix rows and broadcast elements
        __m512 mat_row0_r = _mm512_set1_ps(matrix[0]);
        __m512 mat_row0_g = _mm512_set1_ps(matrix[1]);
        __m512 mat_row0_b = _mm512_set1_ps(matrix[2]);
        __m512 mat_row0_a = _mm512_set1_ps(matrix[3]);

        __m512 mat_row1_r = _mm512_set1_ps(matrix[4]);
        __m512 mat_row1_g = _mm512_set1_ps(matrix[5]);
        __m512 mat_row1_b = _mm512_set1_ps(matrix[6]);
        __m512 mat_row1_a = _mm512_set1_ps(matrix[7]);

        __m512 mat_row2_r = _mm512_set1_ps(matrix[8]);
        __m512 mat_row2_g = _mm512_set1_ps(matrix[9]);
        __m512 mat_row2_b = _mm512_set1_ps(matrix[10]);
        __m512 mat_row2_a = _mm512_set1_ps(matrix[11]);

        __m512 mat_row3_r = _mm512_set1_ps(matrix[12]);
        __m512 mat_row3_g = _mm512_set1_ps(matrix[13]);
        __m512 mat_row3_b = _mm512_set1_ps(matrix[14]);
        __m512 mat_row3_a = _mm512_set1_ps(matrix[15]);

        // Compute new R component using FMA
        __m512 new_r = _mm512_fmadd_ps(r, mat_row0_r,
                                _mm512_fmadd_ps(g, mat_row0_g,
                                    _mm512_fmadd_ps(b, mat_row0_b,
                                        _mm512_mul_ps(a, mat_row0_a))));

        // Compute new G component using FMA
        __m512 new_g = _mm512_fmadd_ps(r, mat_row1_r,
                                _mm512_fmadd_ps(g, mat_row1_g,
                                    _mm512_fmadd_ps(b, mat_row1_b,
                                        _mm512_mul_ps(a, mat_row1_a))));

        // Compute new B component using FMA
        __m512 new_b = _mm512_fmadd_ps(r, mat_row2_r,
                                _mm512_fmadd_ps(g, mat_row2_g,
                                    _mm512_fmadd_ps(b, mat_row2_b,
                                        _mm512_mul_ps(a, mat_row2_a))));

        // Compute new A component using FMA
        __m512 new_a = _mm512_fmadd_ps(r, mat_row3_r,
                                _mm512_fmadd_ps(g, mat_row3_g,
                                    _mm512_fmadd_ps(b, mat_row3_b,
                                        _mm512_mul_ps(a, mat_row3_a))));

        // Interleave the components back into RGBA format
        // Interleave R and G
        __m512 rg_lo = _mm512_unpacklo_ps(new_r, new_g);
        __m512 rg_hi = _mm512_unpackhi_ps(new_r, new_g);

        // Interleave B and A
        __m512 ba_lo = _mm512_unpacklo_ps(new_b, new_a);
        __m512 ba_hi = _mm512_unpackhi_ps(new_b, new_a);

        // Interleave RG and BA to get RGBA
        __m512 rgba_lo = _mm512_shuffle_ps(rg_lo, ba_lo, _MM_SHUFFLE(2, 0, 2, 0));
        __m512 rgba_hi = _mm512_shuffle_ps(rg_hi, ba_hi, _MM_SHUFFLE(2, 0, 2, 0));

        // Store the results back to output
        _mm512_storeu_ps(rgba_output + i * 4, rgba_lo);
        _mm512_storeu_ps(rgba_output + i * 4 + 16, rgba_hi);
    }
}


#endif
/**
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

#include <cmath>
//#include <intrin.h>
#include <numbers>
#include <string>
#include <vector>


#ifndef LSN_ELEMENTS
#define LSN_ELEMENTS( x )									((sizeof( x ) / sizeof( 0[x] )) / (static_cast<size_t>(!(sizeof( x ) % sizeof(0[x])))))
#endif	// #ifndef LSN_ELEMENTS

#ifndef LSN_PI
#define LSN_PI												3.1415926535897932384626433832795
#endif	// #ifndef LSN_PI

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
			std::u16string u16sFile;						/**< THe file name. */
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
		static std::string									Utf16ToUtf8( const char16_t * _pcString, bool * _pbErrored = nullptr );

		/**
		 * Creates a string with _cReplaceMe replaced with _cWithMe inside _s16String.
		 *
		 * \param _s16String The string in which replacements are to be made.
		 * \param _cReplaceMe The character to replace.
		 * \param _cWithMe The character with which to replace _cReplaceMe.
		 * \return Returns the new string with the given replacements made.
		 */
		static std::u16string								Replace( const std::u16string &_s16String, char16_t _cReplaceMe, char16_t _cWithMe );

		/**
		 * Lower-cases a Unicode string.
		 * 
		 * \param _u16Input THe string to lower-case.
		 * \return Returns a copy of the given string in lower-case.
		 **/
		static std::u16string								ToLower( const std::u16string &_u16Input );

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
		 * Converts from sRGB to linear.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the color value converted to linear space.
		 */
		static double										sRGBtoLinear( double _dVal ) {
			return _dVal <= 0.04045 ?
				_dVal * (1.0 / 12.92) :
				std::pow( (_dVal + 0.055) * (1.0 / 1.055), 2.4 );
		}

		/**
		 * Converts from linear to sRGB.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the value converted to sRGB space.
		 */
		static double										LinearTosRGB( double _dVal ) {
			return _dVal <= 0.00313080495356037151702786377709 ?	// (1.0 / 12.92) / (1.0 / 0.04045)
				_dVal * 12.92 :
				1.055 * std::pow( _dVal, 1.0 / 2.4 ) - 0.055;
		}

		/**
		 * Converts from SMPTE 170M-2004 to linear.
		 * 
		 * \param _dVal The value to convert.
		 * \return Returns the color value converted to linear space.
		 **/
		static double										SMPTE170MtoLinear( double _dVal ) {
			return _dVal < 0.0812428582986315 ?						// 4.5 / (1.0 / 0.018053968510807)
				_dVal / 4.5 :
				std::pow( (_dVal + 0.099) / 1.09929682680944, 1.0 / 0.45 );
		}

		/**
		 * Converts from linear to SMPTE 170M-2004.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the value converted to SMPTE 170M-2004 space.
		 */
		static double										LinearToSMPTE170M( double _dVal ) {
			return _dVal < 0.018053968510807 ?
				4.5 * _dVal :
				1.09929682680944 * std::pow( _dVal, 0.45 ) - 0.099;
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

			if ( _ui32A == _ui32B && _ui32C == _ui32D && _ui32A == _ui32D ) { return _ui32A; }

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

			if ( _ui32A == _ui32B ) { return _ui32A; }

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

			if ( _ui64A == _ui64B ) { return _ui64A; }

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

		/**
		 * 128-bit integer-based linear interpolation between 2 sets of 4 ARGB values (0xAARRGGBBAARRGGBB, though color order doesn't actually matter).
		 * 
		 * \param _pui32A The left 8 colors.  0xAARRGGBBAARRGGBB, though color order doesn't actually matter.
		 * \param _pui32B The right 8 colors.  0xAARRGGBBAARRGGBB, though color order doesn't actually matter.
		 * \param _pui32Result The destination where the interpolated 8 colors go.
		 * \param _ui32FactorX The interpolation factor (A -> B).  0-256, such that 0 = _ui32A and 256 = _ui32B.
		 **/
		static LSN_FORCEINLINE void							LinearSample_SSE4( const uint32_t * _pui32A, const uint32_t * _pui32B, uint32_t * _pui32Result,  uint32_t _ui32FactorX ) {
#ifdef __SSE4_1__
			__m128i mA					= _mm_loadu_si128( reinterpret_cast<const __m128i *>(_pui32A) );
			__m128i mB					= _mm_loadu_si128( reinterpret_cast<const __m128i *>(_pui32B) );

			// Early out if both colors are the same.
			if ( _mm_test_all_zeros( _mm_xor_si128( mA, mB ), _mm_set1_epi32( -1 ) ) ) {
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
#endif	// #ifdef __SSE4_1__
		}

		/**
		 * 256-bit integer-based linear interpolation between 2 sets of 8 ARGB values (0xAARRGGBBAARRGGBB, though color order doesn't actually matter).
		 * 
		 * \param _pui32A The left 8 colors.  0xAARRGGBBAARRGGBB, though color order doesn't actually matter.
		 * \param _pui32B The right 8 colors.  0xAARRGGBBAARRGGBB, though color order doesn't actually matter.
		 * \param _pui32Result The destination where the interpolated 8 colors go.
		 * \param _ui32FactorX The interpolation factor (A -> B).  0-256, such that 0 = _ui32A and 256 = _ui32B.
		 **/
		static LSN_FORCEINLINE void							LinearSample_AVX2( const uint32_t * _pui32A, const uint32_t * _pui32B, uint32_t * _pui32Result,  uint32_t _ui32FactorX ) {
#ifdef __AVX2__
			constexpr uint32_t mMaskRb	= 0x00FF00FF;
			constexpr uint32_t mMaskAg	= 0xFF00FF00;

			__m256i mMaskRb256			= _mm256_set1_epi32( mMaskRb );
			__m256i mMaskAg256			= _mm256_set1_epi32( mMaskAg );
			__m256i mFactorX			= _mm256_set1_epi32( _ui32FactorX );
			__m256i mFactorXCompliment	= _mm256_set1_epi32( 256 - _ui32FactorX );

			__m256i mA					= _mm256_loadu_si256( reinterpret_cast<const __m256i *>(_pui32A) );
			__m256i mB					= _mm256_loadu_si256( reinterpret_cast<const __m256i *>(_pui32B) );

			// Separate the channels.
			__m256i mArb				= _mm256_and_si256( mA, mMaskRb256 );
			__m256i mAag				= _mm256_and_si256( mA, mMaskAg256 );
			__m256i mBrb				= _mm256_and_si256( mB, mMaskRb256 );
			__m256i mBag				= _mm256_and_si256( mB, mMaskAg256 );

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
#endif	// #ifdef __AVX2__
		}

		/**
		 * 512-bit integer-based linear interpolation between 2 sets of 8 ARGB values (0xAARRGGBBAARRGGBB, though color order doesn't actually matter).
		 * 
		 * \param _pui32A The left 8 colors.  0xAARRGGBBAARRGGBB, though color order doesn't actually matter.
		 * \param _pui32B The right 8 colors.  0xAARRGGBBAARRGGBB, though color order doesn't actually matter.
		 * \param _pui32Result The destination where the interpolated 8 colors go.
		 * \param _ui32FactorX The interpolation factor (A -> B).  0-256, such that 0 = _ui32A and 256 = _ui32B.
		 **/
		static LSN_FORCEINLINE void							LinearSample_AVX512( const uint32_t * _pui32A, const uint32_t * _pui32B, uint32_t * _pui32Result, uint32_t _ui32FactorX ) {
#ifdef __AVX512F__
			constexpr uint32_t mMaskRb	= 0x00FF00FF;
			constexpr uint32_t mMaskAg	= 0xFF00FF00;

			__m512i mMaskRb512			= _mm512_set1_epi32( mMaskRb );
			__m512i mMaskAg512			= _mm512_set1_epi32( mMaskAg );
			__m512i mFactorX			= _mm512_set1_epi32( _ui32FactorX );
			__m512i mFactorXCompliment	= _mm512_set1_epi32( 256 - _ui32FactorX );

			__m512i mA					= _mm512_loadu_si512( reinterpret_cast<const __m512i *>(_pui32A) );
			__m512i mB					= _mm512_loadu_si512( reinterpret_cast<const __m512i *>(_pui32B) );

			// Separate the channels.
			__m512i mArb				= _mm512_and_si512( mA, mMaskRb512 );
			__m512i mAag				= _mm512_and_si512( mA, mMaskAg512 );
			__m512i mBrb				= _mm512_and_si512( mB, mMaskRb512 );
			__m512i mBag				= _mm512_and_si512( mB, mMaskAg512 );

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
#endif	// #ifdef __AVX512F__
		}


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
			for ( uint32_t X = _ui32DstW; X--; ) {
				uint32_t ui32SrcX = _pui32Factors[X] >> 8;

				uint32_t ui32A = _pui32SrcRow[ui32SrcX];
				uint32_t ui32B = (ui32SrcX == _ui32SrcW - 1) ? 0x00000000 : _pui32SrcRow[ui32SrcX+1];

				_pui32DstRow[X] = CUtilities::LinearSample_Int( ui32A, ui32B, _pui32Factors[X] & 0xFF );
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
			if ( _ui32Factor == 0 ) {
				std::memcpy( _pui32DstRow, _pui32SrcRow0, _ui32Width * sizeof( uint32_t ) );
			}
			else {
				if ( IsAvx512FSupported() ) {
					while ( _ui32Width >= 16 ) {
						LinearSample_AVX512( _pui32SrcRow0, _pui32SrcRow1, _pui32DstRow, _ui32Factor );
						_ui32Width -= 16;
						if ( !_ui32Width ) { return; }
						_pui32SrcRow0 += 16;
						_pui32SrcRow1 += 16;
						_pui32DstRow += 16;
					}
				}

				if ( IsAvx2Supported() ) {
					while ( _ui32Width >= 8 ) {
						LinearSample_AVX2( _pui32SrcRow0, _pui32SrcRow1, _pui32DstRow, _ui32Factor );
						_ui32Width -= 8;
						if ( !_ui32Width ) { return; }
						_pui32SrcRow0 += 8;
						_pui32SrcRow1 += 8;
						_pui32DstRow += 8;
					}
				}

				if ( IsSse4Supported() ) {
					while ( _ui32Width >= 4 ) {
						LinearSample_SSE4( _pui32SrcRow0, _pui32SrcRow1, _pui32DstRow, _ui32Factor );
						_ui32Width -= 4;
						if ( !_ui32Width ) { return; }
						_pui32SrcRow0 += 4;
						_pui32SrcRow1 += 4;
						_pui32DstRow += 4;
					}
				}

				while ( _ui32Width >= 2 ) {
					(*reinterpret_cast<uint64_t *>(_pui32DstRow)) = LinearSample_Int64( (*reinterpret_cast<const uint64_t *>(_pui32SrcRow0)), (*reinterpret_cast<const uint64_t *>(_pui32SrcRow1)), _ui32Factor );
					_ui32Width -= 2;
					if ( !_ui32Width ) { return; }
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
			return (((_ui32SrcLen - 1) * _ui32Idx) << 8) / (_ui32DstLen - 1);
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
		static LSN_FORCEINLINE uint32_t						SamplingFactor_Scanline( uint32_t _ui32SrcLen, uint32_t _ui32DstLen, uint32_t _ui32Idx ) {
			uint32_t ui32Factor = (((_ui32SrcLen - 1) * _ui32Idx) << 8) / (_ui32DstLen - 1);
			uint32_t ui32Idx = ui32Factor >> 8;
			uint32_t ui32Frac = uint32_t( std::max( 0, int32_t( ((ui32Factor & 0xFF) << 1) - 0xFF ) ) );
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

#ifdef __AVX512F__
		/**
		 * Horizontally adds all the floats in a given AVX-512 register.
		 * 
		 * \param _mReg The register containing all of the values to sum.
		 * \return Returns the sum of all the floats in the given register.
		 **/
		static inline float									HorizontalSum( __m512 _mReg ) {
			// Step 1: Reduce 512 bits to 256 bits by adding high and low 256 bits.
			__m256 mLow256 = _mm512_castps512_ps256( _mReg );			// Low 256 bits.
			__m256 mHigh256 = _mm512_extractf32x8_ps( _mReg, 1 );		// High 256 bits.
			__m256 mSum256 = _mm256_add_ps( mLow256, mHigh256 );

			// Step 2: Reduce 256 bits to 128 bits (similar to AVX version).
			__m128 mHigh128 = _mm256_extractf128_ps( mSum256, 1 );		// High 128 bits.
			__m128 mLow128 = _mm256_castps256_ps128( mSum256 );			// Low 128 bits.
			__m128 mSum128 = _mm_add_ps( mHigh128, mLow128 );			// Add them.

			// Step 3: Perform horizontal addition on 128 bits.
			__m128 mAddH1 = _mm_hadd_ps( mSum128, mSum128 );
			__m128 mAddH2 = _mm_hadd_ps( mAddH1, mAddH1 );

			// Step 4: Extract the scalar value.
			return _mm_cvtss_f32( mAddH2 );
		}
#endif	// #ifdef __AVX512F__

#ifdef __AVX__
		/**
		 * Horizontally adds all the floats in a given AVX register.
		 * 
		 * \param _mReg The register containing all of the values to sum.
		 * \return Returns the sum of all the floats in the given register.
		 **/
		static inline float									HorizontalSum( __m256 &_mReg ) {
			// Step 1 & 2: Shuffle and add the high 128 to the low 128.
			__m128 mHigh128 = _mm256_extractf128_ps( _mReg, 1 );		// Extract high 128 bits.
			__m128 mLow128 = _mm256_castps256_ps128( _mReg );			// Directly use low 128 bits.
			__m128 mSum128 = _mm_add_ps( mHigh128, mLow128 );			// Add them.

			// Step 3: Perform horizontal addition.
			__m128 mAddH1 = _mm_hadd_ps( mSum128, mSum128 );
			__m128 mAddH2 = _mm_hadd_ps( mAddH1, mAddH1 );

			// Step 4: Extract the scalar value.
			return _mm_cvtss_f32( mAddH2 );
		}
#endif	// #ifdef __AVX__

#ifdef __SSE4_1__
		/**
		 * Horizontally adds all the floats in a given SSE register.
		 * 
		 * \param _mReg The register containing all of the values to sum.
		 * \return Returns the sum of all the floats in the given register.
		 **/
		static inline float									HorizontalSum( __m128 &_mReg ) {
			__m128 mAddH1 = _mm_hadd_ps( _mReg, _mReg );
			__m128 mAddH2 = _mm_hadd_ps( mAddH1, mAddH1 );
			return _mm_cvtss_f32( mAddH2 );
		}
#endif	// #ifdef __SSE4_1__

		/**
		 * A helper function.
		 *
		 * \param _dX A happy parameter.
		 * \return Returns happiness.
		 */
		static inline double								SinC( double _dX ) {
			_dX *= std::numbers::pi;
			if ( _dX < 0.01 && _dX > -0.01 ) {
				return 1.0 + _dX * _dX * (-1.0 / 6.0 + _dX * _dX * 1.0 / 120.0);
			}

			return ::sin( _dX ) / _dX;
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
			return Bessel0( _dAlpha * ::sqrt( 1.0 - dRatio * dRatio ) ) / Bessel0( _dAlpha );
		}

		/**
		 * A helper function.
		 *
		 * \param _dX A happy parameter.
		 * \return Returns happiness.
		 */
		static float										BlackmanWindow( double _dX ) {
			return static_cast<float>(42659071 + 0.49656062 * ::cos( std::numbers::pi * _dX ) + 0.07684867 * ::cos( 2.0 * std::numbers::pi * _dX ));
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
		 * The Lanczos filter function with 2 samples.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline float									Lanczos2FilterFunc( float _fT ) {
			_fT = ::fabsf( _fT );
			if ( _fT <= 2.0 ) {
				return static_cast<float>(Clean( SinC( _fT ) * SinC( _fT / 2.0 ) ));
			}
			return 0.0f;
		}

		/**
		 * The Lanczos filter function with 3 samples.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline float									Lanczos3FilterFunc( float _fT ) {
			_fT = ::fabsf( _fT );
			if ( _fT <= 3.0 ) {
				return static_cast<float>(Clean( SinC( _fT ) * SinC( _fT / 3.0 ) ));
			}
			return 0.0f;
		}

		/**
		 * The Lanczos filter function with 6 samples.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline float									Lanczos6FilterFunc( float _fT ) {
			_fT = ::fabsf( _fT );
			if ( _fT <= 6.0 ) {
				return static_cast<float>(Clean( SinC( _fT ) * SinC( _fT / 6.0 ) ));
			}
			return 0.0f;
		}

		/**
		 * The Lanczos filter function with 12 samples.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline float									Lanczos12FilterFunc( float _fT ) {
			_fT = ::fabsf( _fT );
			if ( _fT <= 12.0 ) {
				return static_cast<float>(Clean( SinC( _fT ) * SinC( _fT / 12.0 ) ));
			}
			return 0.0f;
		}

		/**
		 * The Lanczos filter function with X samples.
		 *
		 * \param _fT The value to filter.
		 * \param _fWidth The size of the filter kernel.
		 * \return Returns the filtered value.
		 */
		static inline float									LanczosXFilterFunc( float _fT, float _fWidth ) {
			_fT = ::fabsf( _fT );
			if ( _fT <= std::ceil( _fWidth ) ) {
				_fT = float( ::pow( _fT / _fWidth, 12.0 ) * _fWidth );
				return static_cast<float>(Clean( SinC( _fT ) * SinC( _fT / _fWidth ) ));
			}
			return 0.0f;
		}

		/**
		 * The Lanczos filter function with X samples.
		 *
		 * \param _fT The value to filter.
		 * \param _fWidth The size of the filter kernel.
		 * \return Returns the filtered value.
		 */
		template <unsigned _uPow>
		static inline float									LanczosXFilterFunc_Pow( float _fT, float _fWidth ) {
			_fT = ::fabsf( _fT );
			if ( _fT <= std::ceil( _fWidth ) ) {
				_fT = float( ::pow( _fT / _fWidth, double( _uPow ) ) * _fWidth );
				return static_cast<float>(Clean( SinC( _fT ) * SinC( _fT / _fWidth ) ));
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
			_fT = ::fabsf( _fT );
			if ( _fT <= std::ceil( _fWidth ) ) {
				static const float fAtt = 40.0f;
				static const double dAlpha = ::exp( ::log( 0.58417 * (fAtt - 20.96) ) * 0.4 ) + 0.07886 * (fAtt - 20.96);
				return static_cast<float>(Clean( SinC( _fT ) * KaiserHelper( dAlpha, double( _fWidth ), _fT ) ));
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
			_fT = ::fabsf( _fT );
			if ( _fT <= std::ceil( _fWidth ) ) {
				_fT = float( ::pow( _fT / _fWidth, 16.0 ) * _fWidth );
				return Clean( SinC( _fT ) * BlackmanWindow( _fT / double( _fWidth ) ) );
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
			_fT = ::fabsf( _fT );
			if ( _fT <= std::ceil( _fWidth ) ) {
				return Clean( ::exp( -2.0 * _fT * _fT ) * ::sqrt( 2.0 / std::numbers::pi ) * BlackmanWindow( _fT / double( _fWidth ) ) );
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
		static inline float									Gaussian12Over30FilterFunc( float _fT, float _fWidth ) {
			_fT = ::fabsf( _fT );
			if ( _fT <= std::ceil( _fWidth ) ) {
				_fT = float( ::pow( _fT / _fWidth, 30.0 / 12.0 ) * _fWidth );
				//return Clean( ::exp( -2.0 * _fT * _fT ) * ::sqrt( 2.0 / std::numbers::pi ) * BlackmanWindow( _fT / double( _fWidth ) ) );
				
				float fX = _fT - _fWidth;
				return float( (1.0 / (::sqrt( 2.0 * std::numbers::pi ) * _fT)) * (::exp( -(fX * fX) / (2.0 * _fT * _fT) )) );
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
		static inline float									Gaussian1FilterFunc( float _fT, float _fWidth ) {
			_fT = ::fabsf( _fT );
			if ( _fT <= std::ceil( _fWidth ) ) {
				_fT = float( ::pow( _fT / _fWidth, 1.0 / 1.0 ) * _fWidth );
				//return Clean( ::exp( -2.0 * _fT * _fT ) * ::sqrt( 2.0 / std::numbers::pi ) * BlackmanWindow( _fT / double( _fWidth ) ) );
				
				float fX = _fT - _fWidth;
				return float( (1.0 / (::sqrt( 2.0 * std::numbers::pi ) * _fT)) * (::exp( -(fX * fX) / (2.0 * _fT * _fT) )) );
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
		static inline float									Gaussian1_5FilterFunc( float _fT, float _fWidth ) {
			_fT = ::fabsf( _fT );
			if ( _fT <= std::ceil( _fWidth ) ) {
				_fT = float( ::pow( _fT / _fWidth, 1.0 / 1.5 ) * _fWidth );
				//return Clean( ::exp( -2.0 * _fT * _fT ) * ::sqrt( 2.0 / std::numbers::pi ) * BlackmanWindow( _fT / double( _fWidth ) ) );
				
				float fX = _fT - _fWidth;
				return float( (1.0 / (::sqrt( 2.0 * std::numbers::pi ) * _fT)) * (::exp( -(fX * fX) / (2.0 * _fT * _fT) )) );
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
		static inline float									Gaussian2FilterFunc( float _fT, float _fWidth ) {
			_fT = ::fabsf( _fT );
			if ( _fT <= std::ceil( _fWidth ) ) {
				_fT = float( ::pow( _fT / _fWidth, 1.0 / 2.0 ) * _fWidth );
				//return Clean( ::exp( -2.0 * _fT * _fT ) * ::sqrt( 2.0 / std::numbers::pi ) * BlackmanWindow( _fT / double( _fWidth ) ) );
				
				float fX = _fT - _fWidth;
				return float( (1.0 / (::sqrt( 2.0 * std::numbers::pi ) * _fT)) * (::exp( -(fX * fX) / (2.0 * _fT * _fT) )) );
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
		static inline float									Gaussian4FilterFunc( float _fT, float _fWidth ) {
			_fT = ::fabsf( _fT );
			if ( _fT <= std::ceil( _fWidth ) ) {
				_fT = float( ::pow( _fT / _fWidth, 1.0 / 4.0 ) * _fWidth );
				//return Clean( ::exp( -2.0 * _fT * _fT ) * ::sqrt( 2.0 / std::numbers::pi ) * BlackmanWindow( _fT / double( _fWidth ) ) );
				
				float fX = _fT - _fWidth;
				return float( (1.0 / (::sqrt( 2.0 * std::numbers::pi ) * _fT)) * (::exp( -(fX * fX) / (2.0 * _fT * _fT) )) );
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
		static inline float									Gaussian8FilterFunc( float _fT, float _fWidth ) {
			_fT = ::fabsf( _fT );
			if ( _fT <= std::ceil( _fWidth ) ) {
				_fT = float( ::pow( _fT / _fWidth, 1.0 / 8.0 ) * _fWidth );
				//return Clean( ::exp( -2.0 * _fT * _fT ) * ::sqrt( 2.0 / std::numbers::pi ) * BlackmanWindow( _fT / double( _fWidth ) ) );
				
				float fX = _fT - _fWidth;
				return float( (1.0 / (::sqrt( 2.0 * std::numbers::pi ) * _fT)) * (::exp( -(fX * fX) / (2.0 * _fT * _fT) )) );
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
		static inline float									Gaussian16FilterFunc( float _fT, float _fWidth ) {
			_fT = ::fabsf( _fT );
			if ( _fT <= std::ceil( _fWidth ) ) {
				_fT = float( ::pow( _fT / _fWidth, 1.0 / 16.0 ) * _fWidth );
				//return Clean( ::exp( -2.0 * _fT * _fT ) * ::sqrt( 2.0 / std::numbers::pi ) * BlackmanWindow( _fT / double( _fWidth ) ) );
				
				float fX = _fT - _fWidth;
				return float( (1.0 / (::sqrt( 2.0 * std::numbers::pi ) * _fT)) * (::exp( -(fX * fX) / (2.0 * _fT * _fT) )) );
			}
			return 0.0f;
		}

		/**
		 * The box filter function.
		 *
		 * \param _fT The value to filter.
		 * \param _fWidth The size of the filter kernel.
		 * \return Returns the filtered value.
		 */
		static inline float									BoxFilterFunc( float _fT, float _fWidth ) {
			_fT = ::fabsf( _fT );
			return (_fT <= std::ceil( _fWidth )) ? 1.0f : 0.0f;
		}
		/**
		 * A custom filter with a flat middle and fall-offs on the sides.
		 * 
		 * \param _fT The value to filter.
		 * \param _fWidth The size of the filter kernel.
		 * \return Returns the filtered value.
		 **/
		template <unsigned _uNotchW, unsigned _uPowTimes100>
		static inline float									CrtHumpFunc( float _fT, float _fWidth ) {
			_fT = ::fabsf( _fT );
			constexpr double dNotchW = double( _uNotchW ) / 2.0;
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
#endif	// #if defined( __i386__ ) || defined( __x86_64__ )
		}

		/**
		 * Is AVX 2 supported?
		 *
		 * \return Returns true if AVX is supported.
		 **/
		static inline bool									IsAvx2Supported() {
#if defined( __i386__ ) || defined( __x86_64__ ) || defined( _MSC_VER )
			return CFeatureSet::AVX2();
#else
			return false;
#endif	// #if defined( __i386__ ) || defined( __x86_64__ )
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
#endif	// #if defined( __i386__ ) || defined( __x86_64__ )
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
#endif	// #if defined( __i386__ ) || defined( __x86_64__ )
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
#endif	// #if defined( __i386__ ) || defined( __x86_64__ )
		}


		// == Members.
		LSN_ALIGN( 64 )
		static const float									m_fNtscLevels[16];							/**< Output levels for NTSC. */
		LSN_ALIGN( 64 )
		static const float									m_fPalLevels[16];							/**< Output levels for PAL. */
	};

}	// namespace lsn

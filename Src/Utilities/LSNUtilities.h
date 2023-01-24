/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Useful utility functions.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include <intrin.h>
#include <string>


#ifndef LSN_ELEMENTS
#define LSN_ELEMENTS( x )									((sizeof( x ) / sizeof( 0[x] )) / (static_cast<size_t>(!(sizeof( x ) % sizeof(0[x])))))
#endif	// #ifndef LSN_ELEMENTS

namespace lsn {

	/**
	 * Class CUtilities
	 * \brief Useful utility functions.
	 *
	 * Description: Useful utility functions.
	 */
	class CUtilities {
	public :
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
		static __forceinline uint32_t						BiLinearSample_Int( uint32_t _ui32A, uint32_t _ui32B, uint32_t _ui32C, uint32_t _ui32D, uint32_t _ui32FactorX, uint32_t _ui32FactorY ) {
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
		static __forceinline uint32_t						LinearSample_Int( uint32_t _ui32A, uint32_t _ui32B, uint32_t _ui32FactorX ) {
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
		 * \param _ui32A The left color.  0xAARRGGBBAARRGGBB, though color order doesn't actually matter.
		 * \param _ui32B The right color.  0xAARRGGBBAARRGGBB, though color order doesn't actually matter.
		 * \param _ui32FactorX The interpolation factor (A -> B).  0-256, such that 0 = _ui32A and 256 = _ui32B.
		 * \return Returns the 0xAARRGGBBAARRGGBB (though color order doesn't actually matter) color resulting from linear interpolation.
		 */
		static __forceinline uint64_t						LinearSample_Int64( uint64_t _ui64A, uint64_t _ui64B, uint64_t _ui64FactorX ) {
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
		 * Performs integer-based linear interpolation across a row of ARGB pixels.
		 *
		 * \param _pui32SrcRow The input row.
		 * \param _pui32DstRow The output row.
		 * \param _pui32Factors The interpolation factors, one for each output sample.
		 * \param _ui32SrcW The input width in pixels.
		 * \param _ui32DstW The output width in pixels and the number of values to which _pui8Factors points.
		 */
		static __forceinline void							LinearInterpolateRow_Int( const uint32_t * _pui32SrcRow, uint32_t * _pui32DstRow, const uint32_t * _pui32Factors, uint32_t _ui32SrcW, uint32_t _ui32DstW ) {
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
		static __forceinline void							LinearInterpCombineRows_Int( const uint32_t * _pui32SrcRow0, const uint32_t * _pui32SrcRow1, uint32_t * _pui32DstRow, uint32_t _ui32Width, uint32_t _ui32Factor ) {
			if ( _ui32Factor == 0 ) {
				std::memcpy( _pui32DstRow, _pui32SrcRow0, _ui32Width * sizeof( uint32_t ) );
			}
			else {
				const uint64_t * pui64Row0 = reinterpret_cast<const uint64_t *>(_pui32SrcRow0);
				const uint64_t * pui64Row1 = reinterpret_cast<const uint64_t *>(_pui32SrcRow1);
				uint64_t * pui64Dst = reinterpret_cast<uint64_t *>(_pui32DstRow);
				uint32_t ui32Total;


				ui32Total = _ui32Width >> 1;
				for ( uint32_t I = 0; I < ui32Total; ++I ) {
					pui64Dst[I] = LinearSample_Int64( pui64Row0[I], pui64Row1[I], _ui32Factor );
				}
				if ( _ui32Width & 1 ) {
					--_ui32Width;
					_pui32DstRow[_ui32Width] = LinearSample_Int( _pui32SrcRow0[_ui32Width], _pui32SrcRow1[_ui32Width], _ui32Factor );
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
		static __forceinline uint32_t						AddArgb( uint32_t _ui32Color0, uint32_t _ui32Color1 ) {
			uint32_t ui32Ag0 = _ui32Color0 & 0xFF00FF00;
			uint32_t ui32Rb0 = _ui32Color0 & 0x00FF00FF;
			uint64_t ui64Ag1 = (_ui32Color1 & 0xFF00FF00ULL) + ui32Ag0;
			uint32_t ui32Rb1 = (_ui32Color1 & 0x00FF00FF) + ui32Rb0;
#if 0
			uint32_t ui32AgOver = uint32_t( (ui64Ag1 & 0x100010000ULL) >> 1 );
			ui32AgOver = ui32AgOver | (ui32AgOver >> 1);
			ui32AgOver = ui32AgOver | (ui32AgOver >> 2);
			ui32AgOver = ui32AgOver | (ui32AgOver >> 4);

			uint32_t ui32RbOver = ui32Rb1 & 0x01000100;
			ui32RbOver = ui32RbOver | (ui32RbOver >> 1);
			ui32RbOver = ui32RbOver | (ui32RbOver >> 2);
			ui32RbOver = ui32RbOver | (ui32RbOver >> 4);
			ui32RbOver = ui32RbOver >> 1;
#else
			uint32_t ui32AgOver = uint32_t( ((ui64Ag1 & 0x100010000ULL) * 0xFF) >> 8 );
			uint32_t ui32RbOver = ((ui32Rb1 & 0x01000100) * 0xFF) >> 8;
#endif

			return (uint32_t( ui64Ag1 | ui32AgOver ) & 0xFF00FF00) | ((ui32Rb1 | ui32RbOver) & 0x00FF00FF);
		}
	};

}	// namespace lsn

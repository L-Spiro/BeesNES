/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Useful utility functions.
 */


#pragma once

#include "../LSNLSpiroNes.h"

#include <cmath>
#include <intrin.h>
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
			return _dVal <= 0.0031308 ?
				_dVal * 12.92 :
				1.055 * std::pow( _dVal, 1.0 / 2.4 ) - 0.055;
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
				uint32_t I;
				uint32_t ui32ThisTotal = ui32Total - 1;
				for ( I = 0; I < ui32ThisTotal; ++I ) {
					pui64Dst[I] = LinearSample_Int64( pui64Row0[I], pui64Row1[I], _ui32Factor );
				}
				for ( I = ui32ThisTotal * 2; I < _ui32Width; ++I ) {
					_pui32DstRow[I] = LinearSample_Int( _pui32SrcRow0[I], _pui32SrcRow1[I], _ui32Factor );
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
		static __forceinline uint32_t						ShiftArgbRight_Int( uint32_t _ui32Val ) {
			return (_ui32Val & ArgbShiftMask_Int<_stShift>()) >> _stShift;
		}

		/**
		 * Shifts a 64-bit RGBA value right a given number of places.
		 *
		 * \param _ui64Val The value to shift.
		 * \return Returns the shifted RGBA value such that each component is clamped to 0 after the shift.
		 */
		template <size_t _stShift>
		static __forceinline uint64_t						ShiftArgbRight_Int64( uint64_t _ui64Val ) {
			return (_ui64Val & ArgbShiftMask_Int64<_stShift>()) >> _stShift;
		}

		/**
		 * Divides a 32-bit RGBA value by a constant
		 *
		 * \param _ui32Color The value to divide.
		 * \return Returns the RGBA value with each component divided by the given value.
		 */
		template <uint8_t _ui8Div>
		static __forceinline uint32_t						DivArgb_Int( uint32_t _ui32Color ) {
			return 0;
		}

		/**
		 * Applies a quik phospher decay to the given 32-bit pixels.
		 *
		 * \param _pui32Src The pixels to decay.
		 * \param _ui32Width The row stride, in pixels, of the input/output buffer.
		 * \param _ui32Height The number of rows in the input/output buffer.
		 */
		static __forceinline void							DecayArgb( uint32_t * _pui32Src, uint32_t _ui32Width, uint32_t _ui32Height ) {
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
		static __forceinline uint32_t						SamplingFactor_BiLinear( uint32_t _ui32SrcLen, uint32_t _ui32DstLen, uint32_t _ui32Idx ) {
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
		static __forceinline uint32_t						SamplingFactor_Scanline( uint32_t _ui32SrcLen, uint32_t _ui32DstLen, uint32_t _ui32Idx ) {
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
		 * Copies the last folder in the path given in the first string to the start of the 2nd string.
		 * 
		 * \param _u16Folders The folder path.
		 * \param _u16Path The file name.
		 **/
		static void											CopyLastFolderToFileName( std::u16string &_u16Folders, std::u16string &_u16Path );

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
			if ( _fT <= _fWidth ) {
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
			if ( _fT <= _fWidth ) {
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
			if ( _fT <= _fWidth ) {
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
		static inline float									GaussianFilterFunc( float _fT, float _fWidth ) {
			_fT = ::fabsf( _fT );
			if ( _fT <= _fWidth ) {
				return Clean( ::exp( -2.0 * _fT * _fT ) * ::sqrt( 2.0 / std::numbers::pi ) * BlackmanWindow( _fT / double( _fWidth ) ) );
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
			return (_fT >= -double( _fWidth ) && _fT <= _fWidth) ? 1.0f : 0.0f;
		}
	};

}	// namespace lsn

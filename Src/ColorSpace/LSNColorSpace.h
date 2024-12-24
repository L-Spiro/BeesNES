/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Colorspace functionality.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Math/LSNMatrix4x4.h"

namespace lsn {

	/**
	 * Class CColorSpace
	 * \brief Colorspace functionality.
	 *
	 * Description: Colorspace functionality.
	 */
	class CColorSpace {
	public :
		// == Functions.
		/**
		 * Creates a conversion matrix given a color-space's chromaticities and white point.
		 *
		 * \param _fRedX X coordinate of red primary.
		 * \param _fRedY Y coordinate of red primary.
		 * \param _fGreenX X coordinate of green primary.
		 * \param _fGreenY Y coordinate of green primary.
		 * \param _fBlueX X coordinate of blue primary.
		 * \param _fBlueY Y coordinate of blue primary.
		 * \param _fWhiteX X coordinate of white point.
		 * \param _fWhiteY Y coordinate of white point.
		 * \return The RGB to XYZ transformation matrix.
		 */
		static CMatrix4x4<LSN_ST_AVX512>									ComputeRgbToXyzMatrix( float _fRedX, float _fRedY,
																				float _fGreenX, float _fGreenY,
																				float _fBlueX, float _fBlueY,
																				float _fWhiteX, float _fWhiteY );

		/**
		 * Creates a conversion matrix given a color-space's chromaticities and white point.
		 *
		 * \param _fRedX X coordinate of red primary.
		 * \param _fRedY Y coordinate of red primary.
		 * \param _fGreenX X coordinate of green primary.
		 * \param _fGreenY Y coordinate of green primary.
		 * \param _fBlueX X coordinate of blue primary.
		 * \param _fBlueY Y coordinate of blue primary.
		 * \param _fWhiteX X coordinate of white point.
		 * \param _fWhiteY Y coordinate of white point.
		 * \return The XYZ to RGB transformation matrix.
		 */
		static CMatrix4x4<LSN_ST_AVX512>									ComputeXyzToRgbMatrix( float _fRedX, float _fRedY,
																				float _fGreenX, float _fGreenY,
																				float _fBlueX, float _fBlueY,
																				float _fWhiteX, float _fWhiteY );

		/**
		 * Creates a conversion matrix given a color-space's chromaticities and white point.
		 *
		 * \param _pfChromas the chroma and white-point values.  Source from the members of this class.
		 * \return The RGB to XYZ transformation matrix.
		 */
		static inline CMatrix4x4<LSN_ST_AVX512>								ComputeRgbToXyzMatrix( const float * _pfChromas );

		/**
		 * Creates a conversion matrix given a color-space's chromaticities and white point.
		 *
		 * \param _pfChromas the chroma and white-point values.  Source from the members of this class.
		 * \return The XYZ to RGB transformation matrix.
		 */
		static inline CMatrix4x4<LSN_ST_AVX512>								ComputeXyzToRgbMatrix( const float * _pfChromas );


		// == Members.
		static float														m_fSmpte[8];						/**< Chromaticities and white point for SMPTE 170M, SMPTE 240M, BT.601 525. */
		static float														m_f1953[8];							/**< Chromaticities and white point for NTSC 1953. */
		static float														m_fBt2020[8];						/**< Chromaticities and white point for BT.2020. */
		static float														m_fBt709[8];						/**< Chromaticities and white point for BT.709 (and sRGB). */
		static float														m_fBt601_625[8];					/**< Chromaticities and white point for BT.601 625. */
		static float														m_fBt470_NTSC[8];					/**< Chromaticities and white point for BT.470 NTSC. */
		static float														m_fBt470_PAL[8];					/**< Chromaticities and white point for BT.470 PAL. */
		static float														m_fsRgb[8];							/**< Chromaticities and white point for sRGB. */
	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Creates a conversion matrix given a color-space's chromaticities and white point.
	 *
	 * \param _pfChromas the chroma and white-point values.  Source from the members of this class.
	 * \return The RGB to XYZ transformation matrix.
	 */
	inline CMatrix4x4<LSN_ST_AVX512> CColorSpace::ComputeRgbToXyzMatrix( const float * _pfChromas ) {
		return ComputeRgbToXyzMatrix( _pfChromas[0], _pfChromas[1], _pfChromas[2], _pfChromas[3], _pfChromas[4], _pfChromas[5], _pfChromas[6], _pfChromas[7] );
	}

	/**
	 * Creates a conversion matrix given a color-space's chromaticities and white point.
	 *
	 * \param _pfChromas the chroma and white-point values.  Source from the members of this class.
	 * \return The XYZ to RGB transformation matrix.
	 */
	inline CMatrix4x4<LSN_ST_AVX512> CColorSpace::ComputeXyzToRgbMatrix( const float * _pfChromas ) {
		return ComputeXyzToRgbMatrix( _pfChromas[0], _pfChromas[1], _pfChromas[2], _pfChromas[3], _pfChromas[4], _pfChromas[5], _pfChromas[6], _pfChromas[7] );
	}
	

}	// namespace lsn

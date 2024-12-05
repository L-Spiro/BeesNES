/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Colorspace functionality.
 */


#include "LSNColorSpace.h"

namespace lsn {

	// == Members.
	float CColorSpace::m_fSmpte[8] = {																/**< Chromaticities and white point for SMPTE 170M, SMPTE 240M, BT.601 525. */
		0.630f, 0.340f,
		0.310f, 0.595f,
		0.155f, 0.070f,
		0.3127f, 0.3290f
	};

	float CColorSpace::m_f1953[8] = {																/**< Chromaticities and white point for NTSC 1953. */
		0.670f, 0.330f,
		0.210f, 0.710f,
		0.140f, 0.080f,
		0.3100f, 0.3160f
	};

	float CColorSpace::m_fBt2020[8] = {																/**< Chromaticities and white point for BT.2020. */
		0.708f, 0.292f,
		0.170f, 0.797f,
		0.131f, 0.046f,
		0.3127f, 0.3290f
	};

	float CColorSpace::m_fBt709[8] = {																/**< Chromaticities and white point for BT.709 (and sRGB). */
		0.640f, 0.330f,
		0.300f, 0.600f,
		0.150f, 0.060f,
		0.3127f, 0.3290f
	};

	float CColorSpace::m_fBt601_625[8] = {															/**< Chromaticities and white point for BT.601 625. */
		0.640f, 0.330f,
		0.290f, 0.600f,
		0.150f, 0.060f,
		0.3127f, 0.3290f
	};

	float CColorSpace::m_fBt470_NTSC[8] = {															/**< Chromaticities and white point for BT.470 NTSC. */
		0.670f, 0.330f,
		0.210f, 0.710f,
		0.140f, 0.080f,
		0.3100f, 0.3160f
	};
	float CColorSpace::m_fBt470_PAL[8] = {															/**< Chromaticities and white point for BT.470 PAL. */
		0.640f, 0.330f,
		0.290f, 0.600f,
		0.150f, 0.060f,
		0.3127f, 0.3290f
	};

	float CColorSpace::m_fsRgb[8] = {																/**< Chromaticities and white point for sRGB. */
		0.640f, 0.330f,
		0.300f, 0.600f,
		0.150f, 0.060f,
		0.3127f, 0.3290f
	};

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
	CMatrix4x4<LSN_ST_AVX512> CColorSpace::ComputeRgbToXyzMatrix( float _fRedX, float _fRedY,
		float _fGreenX, float _fGreenY,
		float _fBlueX, float _fBlueY,
		float _fWhiteX, float _fWhiteY ) {
		// Compute Z coordinates.
		float fRedZ = 1.0f - _fRedX - _fRedY;
		float fGreenZ = 1.0f - _fGreenX - _fGreenY;
		float fBlueZ = 1.0f - _fBlueX - _fBlueY;
		float fWhiteZ = 1.0f - _fWhiteX - _fWhiteY;

		// Create the chromaticity matrix.
		CMatrix4x4<LSN_ST_AVX512> mChromaMtx(
			{ _fRedX,	_fGreenX,	_fBlueX,	0.0f },
			{ _fRedY,	_fGreenY,	_fBlueY,	0.0f },
			{ fRedZ,	fGreenZ,	fBlueZ,		0.0f } );

		// Compute the white point in XYZ.
		CVector4<LSN_ST_AVX512> vWhitePoint = {
			_fWhiteX / _fWhiteY,
			1.0f,
			fWhiteZ / _fWhiteY,
			0.0f
		};

		// Invert the chromaticity matrix.
		CMatrix4x4<LSN_ST_AVX512> mInvChromaMtx = mChromaMtx.Inverse();

		// Compute the scaling factors S = inv(M) * W.
		CVector4<LSN_ST_AVX512> S = mInvChromaMtx * vWhitePoint;

		// Scale the columns of the chromaticity matrix.
		CMatrix4x4<LSN_ST_AVX512> mRgbToXyzMat;
		mRgbToXyzMat.Identity();
		for ( int I = 0; I < 3; ++I ) {
			mRgbToXyzMat[0][I] = mChromaMtx[0][I] * S[I];
			mRgbToXyzMat[1][I] = mChromaMtx[1][I] * S[I];
			mRgbToXyzMat[2][I] = mChromaMtx[2][I] * S[I];
		}

		return mRgbToXyzMat;
	}

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
	CMatrix4x4<LSN_ST_AVX512> CColorSpace::ComputeXyzToRgbMatrix( float _fRedX, float _fRedY,
		float _fGreenX, float _fGreenY,
		float _fBlueX, float _fBlueY,
		float _fWhiteX, float _fWhiteY ) {
		return ComputeRgbToXyzMatrix( _fRedX, _fRedY,
			_fGreenX, _fGreenY,
			_fBlueX, _fBlueY,
			_fWhiteX, _fWhiteY ).Inverse();
	}

}	// namespace lsn

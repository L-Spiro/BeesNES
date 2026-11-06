/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Colorspace functionality.
 */


#include "LSNColorSpace.h"
#include "../Localization/LSNLocalization.h"
#include "../Math/LSNMatrix4x4.h"

#include <algorithm>
#include <cstring>
#include <format>
#include <numbers>

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

	float CColorSpace::m_fNtscJ[8] = {																/**< Chromaticities and white point for NTSC-J. */
		0.618f, 0.350f,
		0.280f, 0.605f,
		0.152f, 0.063f,
		0.28315f, 0.29711f
	};

	float CColorSpace::m_fToshibaCF2005[8] = {														/**< Chromaticities and white point for a Toshiba CF2005 as measured by patchy68k: https://forums.nesdev.org/viewtopic.php?p=302438. */
		0.642388f, 0.339424f,
		0.301f, 0.601f,
		0.143599f, 0.070975f,
		0.281f, 0.311f
	};

	float CColorSpace::m_fNtscJ_Sony_PVM_20M2U[8] = {												/**< Chromaticities and white point for NTSC-J ala Sony PVM 20M2U. */
		0.630f, 0.345f,
		0.285f, 0.605f,
		0.150f, 0.065f,
		0.2838f, 0.2984f
	};

	float CColorSpace::m_fNtscJ_Sony_PVM_20L2MDU[8] = {												/**< Chromaticities and white point for NTSC-J ala Sony PVM 20L2MDU. */
		0.625f, 0.345f,
		0.280f, 0.605f,
		0.150f, 0.065f,
		0.2838f, 0.2984f
	};

	float CColorSpace::m_fNtscJ_Sony_P22_Trinitron[8] = {											/**< Chromaticities and white point for NTSC-J ala Sony Trinitron P22. */
		0.621f, 0.340f,
		0.281f, 0.606f,
		0.152f, 0.067f,
		0.2838f, 0.2984f
	};

	/** The hue rotation of the Toshiba CF2005.  patchy68k gives it as -0.39 (https://forums.nesdev.org/viewtopic.php?p=302438), a value for
	 *	gamutthingy (https://github.com/ChthonVII/gamutthingy), whose hue knob (--crt-hue-knob) is a global rotation of the demodulation
	 *	axes in degrees.  The L. Spiro filters take radians and their hue also rotates the demodulation axes, in the same direction. */
#define LSN_TOSHIBA_CF2005_HUE					float( -0.39 * std::numbers::pi / 180.0 )

	const CColorSpace::LSN_COLOR_SPACE_DATA CColorSpace::m_csdData[LSN_CSP_TOTAL] = {									/**< The information about each color space, in LSN_COLOR_SPACE order. */
		{ m_fsRgb,							LSN_LSTR( LSN_STR_CSP_SRGB ),							"srgb",						0.0f },
		{ m_fBt709,							LSN_LSTR( LSN_STR_CSP_BT709 ),							"bt709",					0.0f },
		{ m_fSmpte,							LSN_LSTR( LSN_STR_CSP_SMPTE_C ),						"smpte_c",					0.0f },
		{ m_f1953,							LSN_LSTR( LSN_STR_CSP_NTSC_1953 ),						"ntsc_1953",				0.0f },
		{ m_fBt470_NTSC,					LSN_LSTR( LSN_STR_CSP_BT470_NTSC ),						"bt470_ntsc",				0.0f },
		{ m_fBt470_PAL,						LSN_LSTR( LSN_STR_CSP_BT470_PAL ),						"bt470_pal",				0.0f },
		{ m_fBt601_625,						LSN_LSTR( LSN_STR_CSP_BT601_625 ),						"bt601_625",				0.0f },
		{ m_fBt2020,						LSN_LSTR( LSN_STR_CSP_BT2020 ),							"bt2020",					0.0f },
		{ m_fNtscJ,							LSN_LSTR( LSN_STR_CSP_NTSC_J ),							"ntsc_j",					0.0f },
		{ m_fToshibaCF2005,					LSN_LSTR( LSN_STR_CSP_TOSHIBA_CF2005 ),					"toshiba_cf2005",			LSN_TOSHIBA_CF2005_HUE },
		{ m_fNtscJ_Sony_PVM_20M2U,			LSN_LSTR( LSN_STR_CSP_SONY_PVM_20M2U ),					"sony_pvm_20m2u",			0.0f },
		{ m_fNtscJ_Sony_PVM_20L2MDU,		LSN_LSTR( LSN_STR_CSP_SONY_PVM_20L2MDU ),				"sony_pvm_20l2mdu",			0.0f },
		{ m_fNtscJ_Sony_P22_Trinitron,		LSN_LSTR( LSN_STR_CSP_SONY_P22_TRINITRON ),				"sony_trinitron_p22",		0.0f },
	};

	// == Functions.
	/**
	 * Creates a conversion matrix given a color-space's chromaticities and white point.
	 *
	 * \param _pfChromas the chroma and white-point values.  Source from the members of this class.
	 * \return The RGB to XYZ transformation matrix.
	 */
	CMatrix4x4<LSN_ST_AVX512> CColorSpace::ComputeRgbToXyzMatrix( const float * _pfChromas ) {
		return ComputeRgbToXyzMatrix( _pfChromas[0], _pfChromas[1], _pfChromas[2], _pfChromas[3], _pfChromas[4], _pfChromas[5], _pfChromas[6], _pfChromas[7] );
	}

	/**
	 * Creates a conversion matrix given a color-space's chromaticities and white point.
	 *
	 * \param _pfChromas the chroma and white-point values.  Source from the members of this class.
	 * \return The XYZ to RGB transformation matrix.
	 */
	CMatrix4x4<LSN_ST_AVX512> CColorSpace::ComputeXyzToRgbMatrix( const float * _pfChromas ) {
		return ComputeXyzToRgbMatrix( _pfChromas[0], _pfChromas[1], _pfChromas[2], _pfChromas[3], _pfChromas[4], _pfChromas[5], _pfChromas[6], _pfChromas[7] );
	}

	/**
	 * Gets information about a color space.
	 *
	 * \param _cspColorSpace The color space.
	 * \return Returns the information about the color space, or the information about sRGB if _cspColorSpace is invalid.
	 */
	const CColorSpace::LSN_COLOR_SPACE_DATA & CColorSpace::Data( LSN_COLOR_SPACE _cspColorSpace ) {
		return m_csdData[(_cspColorSpace < LSN_CSP_TOTAL) ? _cspColorSpace : LSN_CSP_SRGB];
	}

	/**
	 * Finds a color space by the ID returned in LSN_COLOR_SPACE_DATA::pcId.
	 *
	 * \param _pcId The ID of the color space.
	 * \return Returns the color space or LSN_CSP_TOTAL if there is no color space with that ID.
	 */
	CColorSpace::LSN_COLOR_SPACE CColorSpace::FromId( const char * _pcId ) {
		if ( !_pcId ) { return LSN_CSP_TOTAL; }
		for ( uint32_t I = 0; I < LSN_CSP_TOTAL; ++I ) {
			if ( std::strcmp( m_csdData[I].pcId, _pcId ) == 0 ) { return LSN_COLOR_SPACE( I ); }
		}
		return LSN_CSP_TOTAL;
	}

	/**
	 * Creates the matrix that converts linear RGB in one color space to linear RGB in another.  RGB is converted to CIE XYZ through the
	 *	source primaries and back through the destination primaries.  White points are not adapted, so a source white that is bluer than
	 *	the destination white stays bluer (as it was on the original screens).  If source white would then exceed 1.0 in any channel,
	 *	the matrix is scaled so that source white just reaches 1.0 in its brightest channel instead of clipping.
	 *
	 * \param _cspSrc The color space of the input.
	 * \param _cspDst The color space of the output (normally the display, sRGB).
	 * \return Returns the conversion matrix.  It is exactly the identity matrix if the color spaces are the same.
	 */
	CColorSpace::LSN_COLOR_MATRIX CColorSpace::ConversionMatrix( LSN_COLOR_SPACE _cspSrc, LSN_COLOR_SPACE _cspDst ) {
		LSN_COLOR_MATRIX cmRet;
		const float * pfSrc = Data( _cspSrc ).pfChromas;
		const float * pfDst = Data( _cspDst ).pfChromas;
		if ( pfSrc == pfDst || std::memcmp( pfSrc, pfDst, sizeof( float ) * 8 ) == 0 ) { return cmRet; }

		CMatrix4x4<LSN_ST_AVX512> mConv = ComputeXyzToRgbMatrix( pfDst ) * ComputeRgbToXyzMatrix( pfSrc );

		// Source white is (1, 1, 1); scale so that it does not exceed 1.0 in any channel.
		float fMaxWhite = 0.0f;
		for ( size_t I = 0; I < 3; ++I ) {
			fMaxWhite = std::max( fMaxWhite, mConv[I][0] + mConv[I][1] + mConv[I][2] );
		}
		float fScale = (fMaxWhite > 1.0f) ? (1.0f / fMaxWhite) : 1.0f;
		for ( size_t I = 0; I < 3; ++I ) {
			for ( size_t J = 0; J < 3; ++J ) {
				cmRet.fM[I][J] = mConv[I][J] * fScale;
			}
		}
		return cmRet;
	}

	/**
	 * Creates shader code that converts the linear color in c.rgb by a matrix and then maps the result into [0, 1] without changing
	 *	its hue:  rather than clamping each channel (which shifts the hue of out-of-gamut colors), the color is desaturated toward
	 *	its own luminance just far enough for every channel to fit.  If the luminance itself is outside [0, 1], it is clamped first.
	 *	The code is a single block, so it can be placed anywhere a statement can.
	 *
	 * \param _cmMatrix The conversion matrix.
	 * \param _bHlsl If true, HLSL is generated (float3), otherwise GLSL (vec3).
	 * \return Returns the code, or an empty string if the matrix is the identity matrix.
	 */
	std::string CColorSpace::ShaderStatement( const LSN_COLOR_MATRIX &_cmMatrix, bool _bHlsl ) {
		if ( _cmMatrix.IsIdentity() ) { return std::string(); }
		const std::string sVec3 = _bHlsl ? "float3" : "vec3";
		// 9 significant digits round-trip a float exactly; the exponent form is valid in both languages.
		auto aNum = []( float _fVal ) { return std::format( "{:.8e}", _fVal ); };
		std::string sRet = "{\n";
		// The conversion (the output is linear sRGB).
		sRet += "        c.rgb = " + sVec3 + "(\n";
		for ( size_t I = 0; I < 3; ++I ) {
			sRet += "            dot( " + sVec3 + "( " + aNum( _cmMatrix.fM[I][0] ) + ", " + aNum( _cmMatrix.fM[I][1] ) + ", " + aNum( _cmMatrix.fM[I][2] ) + " ), c.rgb )";
			sRet += (I < 2) ? ",\n" : " );\n";
		}
		// Gamut mapping.  For each channel, find how much of its distance from the luminance (vD) can be kept:  (1 - Y) / D above the
		//	luminance and Y / -D below it.  step() picks between them in both languages (GLSL mix() and HLSL lerp() differ).
		sRet +=
			"        float fCsY = clamp( dot( c.rgb, " + sVec3 + "( 2.12639006e-01, 7.15168679e-01, 7.21923187e-02 ) ), 0.0, 1.0 );\n"
			"        " + sVec3 + " vCsD = c.rgb - " + sVec3 + "( fCsY, fCsY, fCsY );\n"
			"        " + sVec3 + " vCsUp = step( " + sVec3 + "( 0.0, 0.0, 0.0 ), vCsD );\n"
			"        " + sVec3 + " vCsT = vCsUp * ((1.0 - fCsY) / max( vCsD, " + sVec3 + "( 1.0e-6, 1.0e-6, 1.0e-6 ) )) +\n"
			"            (" + sVec3 + "( 1.0, 1.0, 1.0 ) - vCsUp) * (fCsY / max( -vCsD, " + sVec3 + "( 1.0e-6, 1.0e-6, 1.0e-6 ) ));\n"
			"        float fCsT = clamp( min( vCsT.x, min( vCsT.y, vCsT.z ) ), 0.0, 1.0 );\n"
			"        c.rgb = clamp( " + sVec3 + "( fCsY, fCsY, fCsY ) + vCsD * fCsT, " + sVec3 + "( 0.0, 0.0, 0.0 ), " + sVec3 + "( 1.0, 1.0, 1.0 ) );\n"
			"    }\n";
		return sRet;
	}

	/**
	 * Wraps a gamma-curve call so that it works on values outside [0, 1].  The composite decoders (the L. Spiro filters in
	 *	particular) produce negative values for colors outside the gamut of the source, and the gamma curves return NaN for them
	 *	(pow() of a negative number).  NaN in one channel then spreads to the whole pixel through the color-space matrix.  The
	 *	wrapped call applies the curve to the magnitude and restores the sign, so the color reaches the matrix (and the gamut
	 *	mapping after it) intact.
	 *
	 * \param _sGammaCall The gamma-curve call, which reads its input from c.rgb.
	 * \return Returns the sign-preserving form of the call (or _sGammaCall if it does not change c.rgb).
	 */
	std::string CColorSpace::SignedGammaCall( const std::string &_sGammaCall ) {
		if ( _sGammaCall == "c.rgb" ) { return _sGammaCall; }
		std::string sRet = _sGammaCall;
		const std::string sFind = "c.rgb", sReplace = "abs( c.rgb )";
		for ( size_t sPos = sRet.find( sFind ); sPos != std::string::npos; sPos = sRet.find( sFind, sPos + sReplace.size() ) ) {
			sRet.replace( sPos, sFind.size(), sReplace );
		}
		return "sign( c.rgb ) * " + sRet;
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

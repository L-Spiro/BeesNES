/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Colorspace functionality.
 */


#include "LSNColorSpace.h"
#include "../Localization/LSNLocalization.h"
#include "../Math/LSNMatrix4x4.h"

#include <algorithm>
#include <cmath>
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
		{ m_fsRgb,							LSN_LSTR( LSN_STR_CSP_SRGB ),							"srgb",						0.0f,	1.0f,	false },
		{ m_fBt709,							LSN_LSTR( LSN_STR_CSP_BT709 ),							"bt709",					0.0f,	1.0f,	false },
		{ m_fSmpte,							LSN_LSTR( LSN_STR_CSP_SMPTE_C ),						"smpte_c",					0.0f,	1.0f,	false },
		{ m_f1953,							LSN_LSTR( LSN_STR_CSP_NTSC_1953 ),						"ntsc_1953",				0.0f,	1.0f,	false },
		{ m_fBt470_NTSC,					LSN_LSTR( LSN_STR_CSP_BT470_NTSC ),						"bt470_ntsc",				0.0f,	1.0f,	false },
		{ m_fBt470_PAL,						LSN_LSTR( LSN_STR_CSP_BT470_PAL ),						"bt470_pal",				0.0f,	1.0f,	false },
		{ m_fBt601_625,						LSN_LSTR( LSN_STR_CSP_BT601_625 ),						"bt601_625",				0.0f,	1.0f,	false },
		{ m_fBt2020,						LSN_LSTR( LSN_STR_CSP_BT2020 ),							"bt2020",					0.0f,	1.0f,	false },
		{ m_fNtscJ,							LSN_LSTR( LSN_STR_CSP_NTSC_J ),							"ntsc_j",					0.0f,	1.0f,	false },
		{ m_fToshibaCF2005,					LSN_LSTR( LSN_STR_CSP_TOSHIBA_CF2005 ),					"toshiba_cf2005",			LSN_TOSHIBA_CF2005_HUE,	1.1155f,	true },
		{ m_fNtscJ_Sony_PVM_20M2U,			LSN_LSTR( LSN_STR_CSP_SONY_PVM_20M2U ),					"sony_pvm_20m2u",			0.0f,	1.0f,	true },
		{ m_fNtscJ_Sony_PVM_20L2MDU,		LSN_LSTR( LSN_STR_CSP_SONY_PVM_20L2MDU ),				"sony_pvm_20l2mdu",			0.0f,	1.0f,	true },
		{ m_fNtscJ_Sony_P22_Trinitron,		LSN_LSTR( LSN_STR_CSP_SONY_P22_TRINITRON ),				"sony_trinitron_p22",		0.0f,	1.0f,	true },
	};

	/**
	 * Multiplies 2 3-by-3 matrices (_dRet = _dA * _dB).
	 *
	 * \param _dA The left matrix.
	 * \param _dB The right matrix.
	 * \param _dRet Holds the returned product.  Must not be either input.
	 */
	static void Multiply3x3( const double _dA[3][3], const double _dB[3][3], double _dRet[3][3] ) {
		for ( size_t I = 0; I < 3; ++I ) {
			for ( size_t J = 0; J < 3; ++J ) {
				_dRet[I][J] = _dA[I][0] * _dB[0][J] + _dA[I][1] * _dB[1][J] + _dA[I][2] * _dB[2][J];
			}
		}
	}

	/**
	 * Inverts a 3-by-3 matrix.
	 *
	 * \param _dM The matrix to invert.
	 * \param _dRet Holds the returned inverse (the identity matrix if _dM is singular).
	 */
	static void Invert3x3( const double _dM[3][3], double _dRet[3][3] ) {
		const double dDet = _dM[0][0] * (_dM[1][1] * _dM[2][2] - _dM[1][2] * _dM[2][1]) -
			_dM[0][1] * (_dM[1][0] * _dM[2][2] - _dM[1][2] * _dM[2][0]) +
			_dM[0][2] * (_dM[1][0] * _dM[2][1] - _dM[1][1] * _dM[2][0]);
		if ( dDet == 0.0 ) {
			for ( size_t I = 0; I < 3; ++I ) {
				for ( size_t J = 0; J < 3; ++J ) { _dRet[I][J] = (I == J) ? 1.0 : 0.0; }
			}
			return;
		}
		const double dInvDet = 1.0 / dDet;
		_dRet[0][0] = (_dM[1][1] * _dM[2][2] - _dM[1][2] * _dM[2][1]) * dInvDet;
		_dRet[0][1] = (_dM[0][2] * _dM[2][1] - _dM[0][1] * _dM[2][2]) * dInvDet;
		_dRet[0][2] = (_dM[0][1] * _dM[1][2] - _dM[0][2] * _dM[1][1]) * dInvDet;
		_dRet[1][0] = (_dM[1][2] * _dM[2][0] - _dM[1][0] * _dM[2][2]) * dInvDet;
		_dRet[1][1] = (_dM[0][0] * _dM[2][2] - _dM[0][2] * _dM[2][0]) * dInvDet;
		_dRet[1][2] = (_dM[0][2] * _dM[1][0] - _dM[0][0] * _dM[1][2]) * dInvDet;
		_dRet[2][0] = (_dM[1][0] * _dM[2][1] - _dM[1][1] * _dM[2][0]) * dInvDet;
		_dRet[2][1] = (_dM[0][1] * _dM[2][0] - _dM[0][0] * _dM[2][1]) * dInvDet;
		_dRet[2][2] = (_dM[0][0] * _dM[1][1] - _dM[0][1] * _dM[1][0]) * dInvDet;
	}

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
	 * \param _waAdaptation How the source white point is adapted to the destination white point.  With LSN_WA_NONE, white points are
	 *	not adapted and the scaling described above applies; otherwise source white becomes destination white exactly.
	 * \return Returns the conversion matrix.  It is exactly the identity matrix if the color spaces are the same.
	 */
	CColorSpace::LSN_COLOR_MATRIX CColorSpace::ConversionMatrix( LSN_COLOR_SPACE _cspSrc, LSN_COLOR_SPACE _cspDst, LSN_WHITE_ADAPTATION _waAdaptation ) {
		LSN_COLOR_MATRIX cmRet;
		const float * pfSrc = Data( _cspSrc ).pfChromas;
		const float * pfDst = Data( _cspDst ).pfChromas;
		if ( pfSrc == pfDst || std::memcmp( pfSrc, pfDst, sizeof( float ) * 8 ) == 0 ) { return cmRet; }

		CMatrix4x4<LSN_ST_AVX512> mSrcToXyz = ComputeRgbToXyzMatrix( pfSrc );
		CMatrix4x4<LSN_ST_AVX512> mXyzToDst = ComputeXyzToRgbMatrix( pfDst );
		double dSrcToXyz[3][3], dXyzToDst[3][3];
		for ( size_t I = 0; I < 3; ++I ) {
			for ( size_t J = 0; J < 3; ++J ) {
				dSrcToXyz[I][J] = mSrcToXyz[I][J];
				dXyzToDst[I][J] = mXyzToDst[I][J];
			}
		}

		// The chromatic-adaptation transform (identity for none).
		double dCat[3][3] = {
			{ 1.0, 0.0, 0.0 },
			{ 0.0, 1.0, 0.0 },
			{ 0.0, 0.0, 1.0 },
		};
		if ( _waAdaptation != LSN_WA_NONE && _waAdaptation < LSN_WA_TOTAL ) {
			// The cone-response matrices.
			static const double dBradford[3][3] = {
				{ 0.8951, 0.2664, -0.1614 },
				{ -0.7502, 1.7135, 0.0367 },
				{ 0.0389, -0.0685, 1.0296 },
			};
			static const double dVonKries[3][3] = {
				{ 0.40024, 0.70760, -0.08081 },
				{ -0.22630, 1.16532, 0.04570 },
				{ 0.0, 0.0, 0.91822 },
			};
			static const double dXyzScaling[3][3] = {
				{ 1.0, 0.0, 0.0 },
				{ 0.0, 1.0, 0.0 },
				{ 0.0, 0.0, 1.0 },
			};
			static const double dCat02[3][3] = {
				{ 0.7328, 0.4296, -0.1624 },
				{ -0.7036, 1.6975, 0.0061 },
				{ 0.0030, 0.0136, 0.9834 },
			};
			const double (* pdCone)[3] = dBradford;
			switch ( _waAdaptation ) {
				case LSN_WA_VON_KRIES : { pdCone = dVonKries; break; }
				case LSN_WA_XYZ_SCALING : { pdCone = dXyzScaling; break; }
				case LSN_WA_CAT02 : { pdCone = dCat02; break; }
				default : {}
			}
			double dConeInv[3][3];
			Invert3x3( pdCone, dConeInv );

			// The white points in XYZ (Y = 1).
			const double dSrcW[3] = { double( pfSrc[6] ) / pfSrc[7], 1.0, (1.0 - pfSrc[6] - pfSrc[7]) / pfSrc[7] };
			const double dDstW[3] = { double( pfDst[6] ) / pfDst[7], 1.0, (1.0 - pfDst[6] - pfDst[7]) / pfDst[7] };
			double dSrcCone[3], dDstCone[3];
			for ( size_t I = 0; I < 3; ++I ) {
				dSrcCone[I] = pdCone[I][0] * dSrcW[0] + pdCone[I][1] * dSrcW[1] + pdCone[I][2] * dSrcW[2];
				dDstCone[I] = pdCone[I][0] * dDstW[0] + pdCone[I][1] * dDstW[1] + pdCone[I][2] * dDstW[2];
			}
			// CAT = inverse(Cone) * diag(Dst / Src) * Cone.
			double dScaled[3][3];
			for ( size_t I = 0; I < 3; ++I ) {
				const double dFactor = dSrcCone[I] != 0.0 ? dDstCone[I] / dSrcCone[I] : 1.0;
				for ( size_t J = 0; J < 3; ++J ) {
					dScaled[I][J] = pdCone[I][J] * dFactor;
				}
			}
			Multiply3x3( dConeInv, dScaled, dCat );
		}

		double dTmp[3][3], dConv[3][3];
		Multiply3x3( dCat, dSrcToXyz, dTmp );
		Multiply3x3( dXyzToDst, dTmp, dConv );

		// Without adaptation, source white is (1, 1, 1) and may exceed 1.0 in a channel; scale so that it does not.
		double dScale = 1.0;
		if ( _waAdaptation == LSN_WA_NONE || _waAdaptation >= LSN_WA_TOTAL ) {
			double dMaxWhite = 0.0;
			for ( size_t I = 0; I < 3; ++I ) {
				dMaxWhite = std::max( dMaxWhite, dConv[I][0] + dConv[I][1] + dConv[I][2] );
			}
			dScale = (dMaxWhite > 1.0) ? (1.0 / dMaxWhite) : 1.0;
		}
		for ( size_t I = 0; I < 3; ++I ) {
			for ( size_t J = 0; J < 3; ++J ) {
				cmRet.fM[I][J] = float( dConv[I][J] * dScale );
			}
		}
		return cmRet;
	}

	/**
	 * Gets the luminance coefficients of a color space (the Y row of its RGB-to-XYZ matrix).
	 *
	 * \param _cspColorSpace The color space.
	 * \param _pfLuma Holds the returned 3 coefficients.
	 */
	void CColorSpace::LuminanceCoefficients( LSN_COLOR_SPACE _cspColorSpace, float * _pfLuma ) {
		CMatrix4x4<LSN_ST_AVX512> mToXyz = ComputeRgbToXyzMatrix( Data( _cspColorSpace ).pfChromas );
		for ( size_t I = 0; I < 3; ++I ) {
			_pfLuma[I] = mToXyz[1][I];
		}
	}

	/**
	 * Creates the shader code of the gamma pass:  the CRT gamma curve (sign-preserving if there is a color-space conversion), then
	 *	the conversion and gamut mapping.  The code reads and writes c.rgb.  HLSL code calls the functions in LSNGamma.hlsl; GLSL code
	 *	calls the functions of the same names that the Vulkan gamma pass embeds.
	 *
	 * \param _gpPass The parameters of the pass.
	 * \param _bHlsl If true, HLSL is generated, otherwise GLSL.
	 * \return Returns the code.
	 */
	std::string CColorSpace::GammaPassStatements( const LSN_GAMMA_PASS &_gpPass, bool _bHlsl ) {
		const std::string sVec3 = _bHlsl ? "float3" : "vec3";
		auto aNum = []( double _dVal ) { return std::format( "{:.8e}", _dVal ); };
		auto aPow = [&]( const char * _pcExp ) { return "pow( c.rgb, " + sVec3 + "( " + _pcExp + ", " + _pcExp + ", " + _pcExp + " ) )"; };
		std::string sGammaCall = "c.rgb";
		switch ( _gpPass.gGamma ) {
			case CNesPalette::LSN_G_CRT1 :			{ sGammaCall = "CrtProperToLinear3( c.rgb, " + aNum( _gpPass.fCrtWhite ) + ", " + aNum( _gpPass.fCrtBrightness ) + " )"; break; }
			case CNesPalette::LSN_G_CRT2 :			{ sGammaCall = "CrtProper2ToLinear3( c.rgb )"; break; }
			case CNesPalette::LSN_G_sRGB :			{ sGammaCall = "sRGBtoLinear3_Precise( c.rgb )"; break; }
			case CNesPalette::LSN_G_SMPTE170M :		{ sGammaCall = "SMPTE170MtoLinear3_Precise( c.rgb )"; break; }
			case CNesPalette::LSN_G_DCIP3 :			{ sGammaCall = "DCIP3toLinear3( c.rgb )"; break; }
			case CNesPalette::LSN_G_ADOBERGB :		{ sGammaCall = "AdobeRGBtoLinear3( c.rgb )"; break; }
			case CNesPalette::LSN_G_SMPTE240M :		{ sGammaCall = "SMPTE240MtoLinear3_Precise( c.rgb )"; break; }
			case CNesPalette::LSN_G_POW_1_96 :		{ sGammaCall = aPow( "1.96" ); break; }
			case CNesPalette::LSN_G_POW_2_0 :		{ sGammaCall = aPow( "2.0" ); break; }
			case CNesPalette::LSN_G_POW_2_2 :		{ sGammaCall = aPow( "2.2222222222222223" ); break; }
			case CNesPalette::LSN_G_POW_2_35 :		{ sGammaCall = aPow( "2.35" ); break; }
			case CNesPalette::LSN_G_POW_2_4 :		{ sGammaCall = aPow( "2.4" ); break; }
			case CNesPalette::LSN_G_POW_2_5 :		{ sGammaCall = aPow( "2.5" ); break; }
			case CNesPalette::LSN_G_POW_2_7 :		{ sGammaCall = aPow( "2.7" ); break; }
			case CNesPalette::LSN_G_POW_2_8 :		{ sGammaCall = aPow( "2.8" ); break; }
			default : { break; }
		}
		// With a color-space conversion, out-of-gamut (negative) inputs must survive the gamma curve (it would make them NaN).
		if ( !_gpPass.cmMatrix.IsIdentity() ) { sGammaCall = SignedGammaCall( sGammaCall ); }
		return "    c.rgb = " + sGammaCall + ";\n    " + ShaderStatement( _gpPass.cmMatrix, _bHlsl, _gpPass.fLuma, _gpPass.gmGamutMapping );
	}

	/**
	 * Creates shader code that converts the linear color in c.rgb to the values sent to the monitor (see LSN_OUTPUT_TRANSFORM).
	 *	The code needs no functions, so it can be placed in any shader.
	 *
	 * \param _otTransform The output transform.
	 * \param _bHlsl If true, HLSL is generated, otherwise GLSL.
	 * \return Returns the code, or an empty string if the sRGB hardware does the whole conversion on its own.
	 */
	std::string CColorSpace::OutputTransformStatement( const LSN_OUTPUT_TRANSFORM &_otTransform, bool _bHlsl ) {
		if ( _otTransform.IsHardwareSrgb() ) { return std::string(); }
		const std::string sVec3 = _bHlsl ? "float3" : "vec3";
		auto aNum = []( double _dVal ) { return std::format( "{:.8e}", _dVal ); };
		auto aSplat = [&]( double _dVal ) { std::string sNum = aNum( _dVal ); return sVec3 + "( " + sNum + ", " + sNum + ", " + sNum + " )"; };
		// A piecewise curve:  _dSlope * x below _dThresh, otherwise _dA * pow( x, _dExp ) - (_dA - 1).
		auto aPiecewise = [&]( double _dThresh, double _dSlope, double _dA, double _dExp ) {
			return "        vOtS = step( " + aSplat( _dThresh ) + ", c.rgb );\n"
				"        c.rgb = (" + sVec3 + "( 1.0, 1.0, 1.0 ) - vOtS) * (c.rgb * " + aNum( _dSlope ) + ") + vOtS * (pow( c.rgb, " + aSplat( _dExp ) + " ) * " + aNum( _dA ) + " - " + aSplat( _dA - 1.0 ) + ");\n";
		};
		auto aPow = [&]( double _dExp ) { return "        c.rgb = pow( c.rgb, " + aSplat( 1.0 / _dExp ) + " );\n"; };

		std::string sRet = "{\n"
			"        " + sVec3 + " vOtS;\n"
			"        c.rgb = clamp( c.rgb, " + sVec3 + "( 0.0, 0.0, 0.0 ), " + sVec3 + "( 1.0, 1.0, 1.0 ) );\n";
		// Encode with the monitor's curve.
		switch ( _otTransform.gGamma ) {
			case CNesPalette::LSN_G_NONE : { break; }
			case CNesPalette::LSN_G_CRT1 : {
				// CUtilities::LinearToCrtProper() with its default white (1.0) and brightness (0.0181 * 8.0).
				constexpr double dLw = 1.0, dB = 0.0181 * 8.0, dAlpha1 = 2.6, dAlpha2 = 3.0, dVc = 0.35;
				const double dK = dLw / std::pow( 1.0 + dB, dAlpha1 );
				const double dSplit = std::pow( dVc + dB, dAlpha1 );
				const double dLow = std::pow( dVc + dB, dAlpha1 - dAlpha2 );
				sRet += "        c.rgb = c.rgb * " + aNum( 1.0 / dK ) + ";\n"
					"        vOtS = step( " + aSplat( dSplit ) + ", c.rgb );\n"
					"        c.rgb = (" + sVec3 + "( 1.0, 1.0, 1.0 ) - vOtS) * pow( c.rgb * " + aNum( 1.0 / dLow ) + ", " + aSplat( 1.0 / dAlpha2 ) + " ) + vOtS * pow( c.rgb, " + aSplat( 1.0 / dAlpha1 ) + " ) - " + aSplat( dB ) + ";\n";
				break;
			}
			case CNesPalette::LSN_G_CRT2 : {
				// The toe of this curve cannot be inverted in closed form (see CUtilities::LinearToCrtProper2()); its power part is used for the whole range.
				sRet += aPow( 2.31 );
				break;
			}
			case CNesPalette::LSN_G_SMPTE170M : {
				sRet += aPiecewise( 0.0180539685108078128139563744980, 4.5, 1.09929682680944297568093048767, 0.45 );
				break;
			}
			case CNesPalette::LSN_G_SMPTE240M : {
				sRet += aPiecewise( 0.0228215855294450278845097557223, 4.0, 1.1115721959217312597711924126997, 0.45 );
				break;
			}
			case CNesPalette::LSN_G_DCIP3 :			{ sRet += aPow( 2.6 ); break; }
			case CNesPalette::LSN_G_ADOBERGB :		{ sRet += aPow( 2.19921875 ); break; }
			case CNesPalette::LSN_G_POW_1_96 :		{ sRet += aPow( 1.96 ); break; }
			case CNesPalette::LSN_G_POW_2_0 :		{ sRet += aPow( 2.0 ); break; }
			case CNesPalette::LSN_G_POW_2_2 :		{ sRet += aPow( 2.22222222222222232090871330001391470432281494140625 ); break; }
			case CNesPalette::LSN_G_POW_2_35 :		{ sRet += aPow( 2.35 ); break; }
			case CNesPalette::LSN_G_POW_2_4 :		{ sRet += aPow( 2.4 ); break; }
			case CNesPalette::LSN_G_POW_2_5 :		{ sRet += aPow( 2.5 ); break; }
			case CNesPalette::LSN_G_POW_2_7 :		{ sRet += aPow( 2.7 ); break; }
			case CNesPalette::LSN_G_POW_2_8 :		{ sRet += aPow( 2.8 ); break; }
			default : {
				// sRGB (and automatic).
				sRet += aPiecewise( 0.0030399346397784318338231024370, 12.9232101807878549948327417951, 1.055, 1.0 / 2.4 );
			}
		}
		// The output range.
		if ( _otTransform.orOutputRange == LSN_OR_LIMITED ) {
			sRet += "        c.rgb = c.rgb * " + aNum( 219.0 / 255.0 ) + " + " + aSplat( 16.0 / 255.0 ) + ";\n";
		}
		// Undo the sRGB curve the hardware is about to apply (IEC 61966-2-1, as the hardware implements it).
		sRet += "        c.rgb = clamp( c.rgb, " + sVec3 + "( 0.0, 0.0, 0.0 ), " + sVec3 + "( 1.0, 1.0, 1.0 ) );\n"
			"        vOtS = step( " + aSplat( 0.04045 ) + ", c.rgb );\n"
			"        c.rgb = (" + sVec3 + "( 1.0, 1.0, 1.0 ) - vOtS) * (c.rgb * " + aNum( 1.0 / 12.92 ) + ") + vOtS * pow( (c.rgb + " + aSplat( 0.055 ) + ") * " + aNum( 1.0 / 1.055 ) + ", " + aSplat( 2.4 ) + " );\n"
			"    }\n";
		return sRet;
	}

	/**
	 * Creates shader code that converts the linear color in c.rgb by a matrix and then maps the result into [0, 1] without changing
	 *	its hue:  rather than clamping each channel (which shifts the hue of out-of-gamut colors), the color is desaturated toward
	 *	its own luminance just far enough for every channel to fit.  If the luminance itself is outside [0, 1], it is clamped first.
	 *	The code is a single block, so it can be placed anywhere a statement can.
	 *
	 * \param _cmMatrix The conversion matrix.
	 * \param _bHlsl If true, HLSL is generated (float3), otherwise GLSL (vec3).
	 * \param _pfLuma The 3 luminance coefficients of the destination color space, or nullptr for those of sRGB.
	 * \param _gmGamutMapping How out-of-gamut colors are handled.
	 * \return Returns the code, or an empty string if the matrix is the identity matrix.
	 */
	std::string CColorSpace::ShaderStatement( const LSN_COLOR_MATRIX &_cmMatrix, bool _bHlsl, const float * _pfLuma, LSN_GAMUT_MAPPING _gmGamutMapping ) {
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
		if ( _gmGamutMapping == LSN_GM_CLIP ) {
			sRet += "        c.rgb = clamp( c.rgb, " + sVec3 + "( 0.0, 0.0, 0.0 ), " + sVec3 + "( 1.0, 1.0, 1.0 ) );\n"
				"    }\n";
			return sRet;
		}
		static const float fSrgbLuma[3] = { 2.12639006e-01f, 7.15168679e-01f, 7.21923187e-02f };
		const float * pfLuma = _pfLuma ? _pfLuma : fSrgbLuma;
		// Gamut mapping.  For each channel, find how much of its distance from the luminance (vD) can be kept:  (1 - Y) / D above the
		//	luminance and Y / -D below it.  step() picks between them in both languages (GLSL mix() and HLSL lerp() differ).
		sRet +=
			"        float fCsY = clamp( dot( c.rgb, " + sVec3 + "( " + aNum( pfLuma[0] ) + ", " + aNum( pfLuma[1] ) + ", " + aNum( pfLuma[2] ) + " ) ), 0.0, 1.0 );\n"
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
	 * Wraps a gamma-curve call so that it works on values outside [0, 1].  The composite decoders (my filters in
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

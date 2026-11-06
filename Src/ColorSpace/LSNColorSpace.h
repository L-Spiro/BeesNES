/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Colorspace functionality.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Math/LSNSimdTypes.h"

#include <cstring>
#include <string>

namespace lsn {

	// CMatrix4x4 is only declared here so that this header can be included next to Utilities/LSNVector4.h (whose CVector4 clashes
	//	with the one in Math/LSNVector4.h).  Include Math/LSNMatrix4x4.h to call the Compute*Matrix() functions.
	template <unsigned _uSimd>
	class CMatrix4x4;

	/**
	 * Class CColorSpace
	 * \brief Colorspace functionality.
	 *
	 * Description: Colorspace functionality.
	 */
	class CColorSpace {
	public :
		// == Enumerations.
		/** The color spaces.  The order is the order in which they are listed in the UI. */
		enum LSN_COLOR_SPACE : uint32_t {
			LSN_CSP_SRGB,																						/**< sRGB (the display; no conversion). */
			LSN_CSP_BT709,																						/**< BT.709. */
			LSN_CSP_SMPTE_C,																					/**< SMPTE C (SMPTE 170M, SMPTE 240M, BT.601 525). */
			LSN_CSP_NTSC_1953,																					/**< NTSC 1953. */
			LSN_CSP_BT470_NTSC,																					/**< BT.470 NTSC. */
			LSN_CSP_BT470_PAL,																					/**< BT.470 PAL. */
			LSN_CSP_BT601_625,																					/**< BT.601 625. */
			LSN_CSP_BT2020,																						/**< BT.2020. */
			LSN_CSP_NTSC_J,																						/**< NTSC-J. */
			LSN_CSP_TOSHIBA_CF2005,																				/**< A Toshiba CF2005 as measured by patchy68k. */
			LSN_CSP_SONY_PVM_20M2U,																				/**< NTSC-J on a Sony PVM 20M2U. */
			LSN_CSP_SONY_PVM_20L2MDU,																			/**< NTSC-J on a Sony PVM 20L2MDU. */
			LSN_CSP_SONY_P22_TRINITRON,																			/**< NTSC-J on a Sony Trinitron (P22 phosphors). */
			LSN_CSP_TOTAL,																						/**< The number of color spaces. */
		};


		// == Types.
		/**
		 * Struct LSN_COLOR_SPACE_DATA
		 * \brief Information about a color space.
		 *
		 * Description: Information about a color space.
		 */
		struct LSN_COLOR_SPACE_DATA {
			const float *												pfChromas;							/**< The chromaticities and white point (8 values:  red, green, blue, and white x and y). */
			const wchar_t *												pwcName;							/**< The name shown to the user. */
			const char *												pcId;								/**< A name that never changes, for saving settings. */
		};

		/**
		 * Struct LSN_COLOR_MATRIX
		 * \brief A 3-by-3 matrix that converts linear RGB from one color space to another.
		 *
		 * Description: A 3-by-3 matrix that converts linear RGB from one color space to another.  Each output channel is the dot product of
		 *	its row with the input (R, G, B).
		 */
		struct LSN_COLOR_MATRIX {
			float														fM[3][3] = {						/**< The rows of the matrix. */
				{ 1.0f, 0.0f, 0.0f },
				{ 0.0f, 1.0f, 0.0f },
				{ 0.0f, 0.0f, 1.0f },
			};


			// == Operators.
			/**
			 * Determines whether 2 matrices are exactly the same.
			 *
			 * \param _cmOther The matrix against which to compare.
			 * \return Returns true if every element matches exactly.
			 */
			inline bool													operator == ( const LSN_COLOR_MATRIX &_cmOther ) const {
				for ( size_t I = 0; I < 3; ++I ) {
					for ( size_t J = 0; J < 3; ++J ) {
						if ( fM[I][J] != _cmOther.fM[I][J] ) { return false; }
					}
				}
				return true;
			}

			/**
			 * Determines whether 2 matrices are different.
			 *
			 * \param _cmOther The matrix against which to compare.
			 * \return Returns true if any element differs.
			 */
			inline bool													operator != ( const LSN_COLOR_MATRIX &_cmOther ) const { return !((*this) == _cmOther); }


			// == Functions.
			/**
			 * Determines whether this is the identity matrix (no conversion), in which case shaders skip it.
			 *
			 * \return Returns true if this is exactly the identity matrix.
			 */
			inline bool													IsIdentity() const { return (*this) == LSN_COLOR_MATRIX(); }
		};


		// == Functions.
		/**
		 * Gets information about a color space.
		 *
		 * \param _cspColorSpace The color space.
		 * \return Returns the information about the color space, or the information about sRGB if _cspColorSpace is invalid.
		 */
		static const LSN_COLOR_SPACE_DATA &									Data( LSN_COLOR_SPACE _cspColorSpace );

		/**
		 * Finds a color space by the ID returned in LSN_COLOR_SPACE_DATA::pcId.
		 *
		 * \param _pcId The ID of the color space.
		 * \return Returns the color space or LSN_CSP_TOTAL if there is no color space with that ID.
		 */
		static LSN_COLOR_SPACE												FromId( const char * _pcId );

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
		static LSN_COLOR_MATRIX												ConversionMatrix( LSN_COLOR_SPACE _cspSrc, LSN_COLOR_SPACE _cspDst = LSN_CSP_SRGB );

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
		static std::string													ShaderStatement( const LSN_COLOR_MATRIX &_cmMatrix, bool _bHlsl );

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
		static std::string													SignedGammaCall( const std::string &_sGammaCall );

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
		static CMatrix4x4<LSN_ST_AVX512>									ComputeRgbToXyzMatrix( const float * _pfChromas );

		/**
		 * Creates a conversion matrix given a color-space's chromaticities and white point.
		 *
		 * \param _pfChromas the chroma and white-point values.  Source from the members of this class.
		 * \return The XYZ to RGB transformation matrix.
		 */
		static CMatrix4x4<LSN_ST_AVX512>									ComputeXyzToRgbMatrix( const float * _pfChromas );


		// == Members.
		static float														m_fSmpte[8];						/**< Chromaticities and white point for SMPTE 170M, SMPTE 240M, BT.601 525. */
		static float														m_f1953[8];							/**< Chromaticities and white point for NTSC 1953. */
		static float														m_fBt2020[8];						/**< Chromaticities and white point for BT.2020. */
		static float														m_fBt709[8];						/**< Chromaticities and white point for BT.709 (and sRGB). */
		static float														m_fBt601_625[8];					/**< Chromaticities and white point for BT.601 625. */
		static float														m_fBt470_NTSC[8];					/**< Chromaticities and white point for BT.470 NTSC. */
		static float														m_fBt470_PAL[8];					/**< Chromaticities and white point for BT.470 PAL. */
		static float														m_fsRgb[8];							/**< Chromaticities and white point for sRGB. */
		static float														m_fNtscJ[8];						/**< Chromaticities and white point for NTSC-J. */

		// The hue rotation is -0.39, and the saturation factor is 1.1155. The video standard is 1953 NTSC with illuminant C and 2.2 gamma, and the CRT is also assumed to be approximately 2.2 gamma.
		// https://github.com/ChthonVII/gamutthingy/blob/master/src/constants.h
		static float														m_fToshibaCF2005[8];				/**< Chromaticities and white point for a Toshiba CF2005 as measured by patchy68k: https://forums.nesdev.org/viewtopic.php?p=302438. */

		// About this, based on my experimentation with the JP modes of Sony's decoder chips, item 2.2 of table 2 in that document should have 9300K+8MPCD / D93 as Japan's system's white point. It's not just the studio monitors; it's the actual system too. 
		// That's unlike the rest of the world where, despite the system being illuminant C, studio monitors have white set to D65, and their approximate color correction still tries to match the rest of the colors (especially red/yellow/green hues) based on illuminant C.
		static float														m_fNtscJ_Sony_PVM_20M2U[8];			/**< Chromaticities and white point for NTSC-J ala Sony PVM 20M2U. */
		static float														m_fNtscJ_Sony_PVM_20L2MDU[8];		/**< Chromaticities and white point for NTSC-J ala Sony PVM 20L2MDU. */

		static float														m_fNtscJ_Sony_P22_Trinitron[8];		/**< Chromaticities and white point for NTSC-J ala Sony Trinitron P22. */


	protected :
		// == Members.
		static const LSN_COLOR_SPACE_DATA									m_csdData[LSN_CSP_TOTAL];			/**< The information about each color space, in LSN_COLOR_SPACE order. */

	};
	


}	// namespace lsn

/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: My own implementation of an NTSC filter.
 */

#include "LSNLSpiroNtscFilterBase.h"

#include "../Utilities/LSNScopedNoSubnormals.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <numbers>

#define LSN_FINAL_BRIGHT							m_fBrightnessSetting// * (1.0f - (m_fPhosphorDecayRateGreen /*m_fPhosphorDecayRateGreen * (m_fInitPhosphorDecay * 2.0f)*/))

namespace lsn {

	// == Members.
	CLSpiroNtscFilterBase::CLSpiroNtscFilterBase() {
		m_fHueSetting = 8.0f * std::numbers::pi / 180.0f;														/**< The hue. */
		m_fGammaSetting = 1.0f / 0.45f;																			/**< The CRT gamma curve. */
		m_fBrightnessSetting = (1.0f - (0.070710678118654752440084436210485f));									/**< The brightness setting. */
		m_fSaturationSetting = -0.40f + 1.0f;																	/**< The saturation setting. */

		// RetroTink?
		//m_fHueSetting = 14.5f * std::numbers::pi / 180.0f;					/**< The hue. */
		//m_fGammaSetting = 2.5f;					/**< The CRT gamma curve. */
		//m_fBrightnessSetting = 1.0f - 0.005f;	/**< The brightness setting. */
		//m_fSaturationSetting = -0.50f + 1.0f;								/**< The saturation setting. */

		//m_fHueSetting = float( 33.0 * std::numbers::pi / 180.0 );
		GenPhaseTables( m_fHueSetting + m_fHueOffset );			// Generate phase table.
		SetGamma( m_fGammaSetting );				// Generate gamma table.
		GenNormalizedSignals();						// Generate black/white normalization levels.
		SetKernelSize( m_ui32FilterKernelSize );	// Generate filter kernel weights.
		SetWidth( LSN_PM_NTSC_M_RENDER_WIDTH );		// Allocate buffers.
		SetHeight( 240 );							// Allocate buffers.		
	}
	CLSpiroNtscFilterBase::~CLSpiroNtscFilterBase() {
		
	}

	// == Functions.
	/**
	 * Sets the filter kernel size for both Y and chroma.
	 * 
	 * \param _ui32Size The new size of the filter.
	 * \return Returns true if the memory for the internal buffer(s) was allocated.
	 **/
	bool CLSpiroNtscFilterBase::SetKernelSize( uint32_t _ui32Size ) {
		m_ui32FilterKernelSize = m_ui32FilterKernelSizeY = _ui32Size;
		GenFilterKernel( m_ui32FilterKernelSize );
		GenFilterKernelY( m_ui32FilterKernelSizeY );
		if ( !AllocYiqBuffers( m_ui16Width, m_ui16Height, m_ui16WidthScale ) ) { return false; }
		return true;
	}

	/**
	 * Sets the Y filter kernel size.
	 * 
	 * \param _ui32Size The new size of the Y filter.
	 * \return Returns true if the memory for the internal buffer(s) was allocated.
	 **/
	bool CLSpiroNtscFilterBase::SetKernelSizeY( uint32_t _ui32Size ) {
		m_ui32FilterKernelSizeY = _ui32Size;
		GenFilterKernelY( m_ui32FilterKernelSizeY );
		if ( !AllocYiqBuffers( m_ui16Width, m_ui16Height, m_ui16WidthScale ) ) { return false; }
		return true;
	}

	/**
	 * Sets the chroma (I/Q) filter kernel size.
	 * 
	 * \param _ui32Size The new size of the chroma filter.
	 * \return Returns true if the memory for the internal buffer(s) was allocated.
	 **/
	bool CLSpiroNtscFilterBase::SetKernelSizeChroma( uint32_t _ui32Size ) {
		m_ui32FilterKernelSize = _ui32Size;
		GenFilterKernel( m_ui32FilterKernelSize );
		if ( !AllocYiqBuffers( m_ui16Width, m_ui16Height, m_ui16WidthScale ) ) { return false; }
		return true;
	}

	/**
	 * Sets the width of the input.
	 * 
	 * \param _ui16Width The width to set.
	 * \return Returns true if the memory for the internal buffer(s) was allocated.
	 **/
	bool CLSpiroNtscFilterBase::SetWidth( uint16_t _ui16Width ) {
		if ( m_ui16Width != _ui16Width ) {
			if ( !AllocYiqBuffers( _ui16Width, m_ui16Height, m_ui16WidthScale ) ) { return false; }
			m_ui16Width = _ui16Width;
			m_ui16ScaledWidth = m_ui16Width * m_ui16WidthScale;
		}
		return true;
	}

	/**
	 * Sets the width scale.
	 * 
	 * \param _ui16WidthScale The width scale to set.
	 * \return Returns true if the memory for the internal buffer(s) was allocated.
	 **/
	bool CLSpiroNtscFilterBase::SetWidthScale( uint16_t _ui16WidthScale ) {
		if ( m_ui16WidthScale != _ui16WidthScale ) {
			if ( !AllocYiqBuffers( m_ui16Width, m_ui16Height, _ui16WidthScale ) ) { return false; }
			m_ui16WidthScale = _ui16WidthScale;
			m_ui16ScaledWidth = m_ui16Width * m_ui16WidthScale;
		}
		return true;
	}

	/**
	 * Sets the height of the input.
	 * 
	 * \param _ui16Height The height to set.
	 * \return Returns true if the memory for the internal buffer(s) was allocated.
	 **/
	bool CLSpiroNtscFilterBase::SetHeight( uint16_t _ui16Height ) {
		if ( m_ui16Height != _ui16Height ) {
			if ( !AllocYiqBuffers( m_ui16Width, _ui16Height, m_ui16WidthScale ) ) { return false; }
			m_ui16Height = _ui16Height;
		}
		return true;
	}

	/**
	 * Sets the CRT gamma.
	 * 
	 * \param _fGamma The gamma to set.
	 **/
	void CLSpiroNtscFilterBase::SetGamma( float _fGamma ) {
		m_fGammaSetting = _fGamma;
		for (size_t I = 0; I < LSN_SRGB_RES; ++I ) {
			if ( m_bHandleMonitorGamma ) {
				double dVal = m_bHandleMonitorGamma ? CUtilities::LinearTosRGB_Precise( CUtilities::CrtProperToLinear( (I / (uint32_t( LSN_SRGB_RES ) - 1.0)), 1.0, 0.0181 * 8.0 ) ) * 255.0 :
					CUtilities::CrtProperToLinear( (I / (uint32_t( LSN_SRGB_RES ) - 1.0)), 1.0, 0.0181 * 8.0 ) * 255.0;
				//double dVal = CUtilities::LinearTosRGB_Precise( CUtilities::CrtProper2ToLinear( I / (uint32_t( LSN_SRGB_RES ) - 1.0) ) ) * 255;
				//double dVal = CUtilities::LinearTosRGB_Precise( std::pow( (I / (uint32_t( LSN_SRGB_RES ) - 1.0)), m_fGammaSetting ) ) * 255.0;
			
				//double dVal = CUtilities::LinearTosRGB_Precise( CUtilities::SMPTE170MtoLinear_Precise( (I / (uint32_t( LSN_SRGB_RES ) - 1.0)) ) ) * 255.0;
				//double dVal = (I / (uint32_t( LSN_SRGB_RES ) - 1.0)) * 255.0;

				dVal = std::min( dVal, 255.0 );
				dVal = std::max( dVal, 0.0 );
				m_ui8Gamma[I] = uint8_t( std::round( dVal ) );
				m_ui32Gamma[I] = m_ui8Gamma[I];

				dVal = m_bHandleMonitorGamma ? CUtilities::LinearTosRGB_Precise( CUtilities::CrtProperToLinear( (I / (uint32_t( LSN_SRGB_RES ) - 1.0)), 1.0, 0.0181 * 8.0 ) ) * 255.0 :
					CUtilities::CrtProperToLinear( (I / (uint32_t( LSN_SRGB_RES ) - 1.0)), 1.0, 0.0181 * 8.0 ) * 255.0;
				//dVal = CUtilities::LinearTosRGB_Precise( CUtilities::CrtProper2ToLinear( I / (uint32_t( LSN_SRGB_RES ) - 1.0) ) ) * 255;
				//dVal = CUtilities::LinearTosRGB_Precise( std::pow( (I / (uint32_t( LSN_SRGB_RES ) - 1.0)), m_fGammaSetting ) ) * 255.0;
			
				//dVal = CUtilities::LinearTosRGB_Precise( CUtilities::SMPTE170MtoLinear_Precise( (I / (uint32_t( LSN_SRGB_RES ) - 1.0)) ) ) * 255.0;
				//dVal = (I / (uint32_t( LSN_SRGB_RES ) - 1.0)) * 255.0;

				dVal = std::min( dVal, 255.0 );
				dVal = std::max( dVal, 0.0 );

				m_ui8GammaG[I] = uint8_t( std::round( dVal ) );
				m_ui32GammaG[I] = m_ui8GammaG[I];
			}
			else {
				m_ui32Gamma[I] = m_ui32GammaG[I] = m_ui8Gamma[I] = m_ui8GammaG[I] = uint8_t( std::round( (I / (uint32_t( LSN_SRGB_RES ) - 1.0)) * 255.0 ) );
			}
		}
	}

	/**
	 * Enables or disables baking of monitor gamma into the gamma table.
	 * 
	 * \param _bApplyMonitorGamma If true, an sRGB curve to compensate for the monitor is applied to the gamma table.
	 **/
	void CLSpiroNtscFilterBase::SetMonitorGammaApply( bool _bApplyMonitorGamma ) {
		m_bHandleMonitorGamma = _bApplyMonitorGamma;
		SetGamma( m_fGammaSetting );
	}

	/**
	 * Sets the hue.
	 * 
	 * \param _fHue The hue to set.
	 **/
	void CLSpiroNtscFilterBase::SetHue( float _fHue ) {
		m_fHueSetting = _fHue;
		GenPhaseTables( m_fHueSetting + m_fHueOffset );
	}

	/**
	 * Sets a hue offset that is added to the hue (see SetHue()).  Used for a color space's hue rotation (CColorSpace::HueOffset()) so
	 *	that it is applied relative to the hue setting instead of replacing it.  The phase tables are only regenerated if it changes.
	 * 
	 * \param _fHueOffset The hue offset, in radians.
	 **/
	void CLSpiroNtscFilterBase::SetHueOffset( float _fHueOffset ) {
		if ( m_fHueOffset == _fHueOffset ) { return; }
		m_fHueOffset = _fHueOffset;
		GenPhaseTables( m_fHueSetting + m_fHueOffset );
	}

	/**
	 * Sets the brightness.
	 * 
	 * \param _fBrightness The brightness to set.
	 **/
	void CLSpiroNtscFilterBase::SetBrightness( float _fBrightness ) {
		m_fBrightnessSetting = _fBrightness;
		GenPhaseTables( m_fHueSetting + m_fHueOffset );
	}

	/**
	 * Sets the saturation.
	 * 
	 * \param _fSat The saturation to set.
	 **/
	void CLSpiroNtscFilterBase::SetSaturation( float _fSat ) {
		m_fSaturationSetting = _fSat;
		GenPhaseTables( m_fHueSetting + m_fHueOffset );
	}

	/**
	 * Sets the contrast:  a gain around the middle of the signal range (0.5), applied to luma and chroma alike.  1.0 leaves the
	 *	picture unchanged.
	 * 
	 * \param _fContrast The contrast to set.
	 **/
	void CLSpiroNtscFilterBase::SetContrast( float _fContrast ) {
		m_fContrastSetting = _fContrast;
		GenPhaseTables( m_fHueSetting + m_fHueOffset );
	}

	/**
	 * Sets the black level.
	 * 
	 * \param _fBlack The black level to set.
	 **/
	void CLSpiroNtscFilterBase::SetBlackLevel( float _fBlack ) {
		m_fBlackSetting = _fBlack;
		GenNormalizedSignals();
	}
	
	/**
	 * Sets the white level.
	 * 
	 * \param _fWhite The white level to set.
	 **/
	void CLSpiroNtscFilterBase::SetWhiteLevel( float _fWhite ) {
		m_fWhiteSetting = _fWhite;
		GenNormalizedSignals();
	}

	/**
	 * Creates a single scanline from 9-bit PPU output to a float buffer of YIQ values.
	 * 
	 * \param _pfDstY The destination for where to begin storing the YIQ Y values.  Must be aligned to a 16-byte boundary.
	 * \param _pfDstI The destination for where to begin storing the YIQ I values.  Must be aligned to a 16-byte boundary.
	 * \param _pfDstQ The destination for where to begin storing the YIQ Q values.  Must be aligned to a 16-byte boundary.
	 * \param _pui16Pixels The start of the 9-bit PPU output for this scanline.
	 * \param _ui16Cycle The cycle count at the start of the scanline.
	 * \param _sRowIdx The scanline index.
	 **/
	void CLSpiroNtscFilterBase::ScanlineToYiq( float * _pfDstY, float * _pfDstI, float * _pfDstQ, const uint16_t * _pui16Pixels, uint16_t _ui16Cycle, size_t _sRowIdx ) {
		GenerateSignals( _pui16Pixels, _ui16Cycle, _sRowIdx );
		ConvolveScanline( _pfDstY, _pfDstI, _pfDstQ, _ui16Cycle, m_vSignalStart[_sRowIdx], m_vSignalStart[_sRowIdx] );
	}

	/**
	 * Generates the composite signals of a scanline (with its padding, noise, and normalization) into m_vSignalStart[_sRowIdx].
	 * 
	 * \param _pui16Pixels The start of the 9-bit PPU output for this scanline.
	 * \param _ui16Cycle The cycle count at the start of the scanline.
	 * \param _sRowIdx The scanline index.
	 **/
	void CLSpiroNtscFilterBase::GenerateSignals( const uint16_t * _pui16Pixels, uint16_t _ui16Cycle, size_t _sRowIdx ) {
		float * pfSignalStart = m_vSignalStart[_sRowIdx];
		float * pfSignals = pfSignalStart;
		for ( uint16_t I = 0; I < m_ui16Width; ++I ) {
			PixelToNtscSignals( pfSignals, _pui16Pixels[I], uint16_t( _ui16Cycle + I * 8 ) );
			pfSignals += 8;
		}

		// The kernels reach past both ends of the scanline, so the signal continues past them (with the edge pixels, in phase) far
		//	enough for the largest kernel.  Without this, the kernels would read whatever was in the padding.
		const int32_t i32Signals = int32_t( m_ui16Width * 8 );
		const int32_t i32Pad = int32_t( m_ui32SignalPad );
		if LSN_LIKELY( m_ui16Width ) {
			// Keeps the phase positive (a multiple of 12 at least as large as the padding).
			const int32_t i32PadPhase = (i32Pad + 11) / 12 * 12;
			const uint16_t ui16Left = _pui16Pixels[0];
			const uint16_t ui16Right = _pui16Pixels[m_ui16Width-1];
			for ( int32_t S = -i32Pad; S < 0; ++S ) {
				pfSignalStart[S] = IndexToNtscSignal( ui16Left, uint16_t( _ui16Cycle + i32PadPhase + S ) );
			}
			for ( int32_t S = i32Signals; S < i32Signals + i32Pad; ++S ) {
				pfSignalStart[S] = IndexToNtscSignal( ui16Right, uint16_t( _ui16Cycle + i32PadPhase + S ) );
			}
		}

		// Add noise.
		pfSignals = pfSignalStart - i32Pad;
		float * pfSignalEnd = pfSignalStart + i32Signals + i32Pad;
		// Prefetch the middle of the buffer.
		LSN_PREFETCH_LINE( pfSignals + m_ui16Width * 4 );
#ifdef __AVX512F__
		if ( CUtilities::IsAvx512FSupported() ) {
			while ( pfSignals < pfSignalEnd ) {
				__m512 vNoise = _mm512_load_ps( CUtilities::m_fNoiseBuffers[LSN_NOISE_BUFFER(CUtilities::Rand())] );
				__m512 vSig = _mm512_loadu_ps( pfSignals );
				vSig = _mm512_add_ps( vSig, vNoise );
				_mm512_storeu_ps( pfSignals, vSig );
				pfSignals += 16;
			}
		}
#endif	// #ifdef __AVX512F__
#ifdef __AVX__
		if LSN_LIKELY( CUtilities::IsAvxSupported() ) {
			while ( pfSignals < pfSignalEnd ) {
				size_t sIdx = LSN_NOISE_BUFFER( CUtilities::Rand() );
				__m256 vNoise = _mm256_load_ps( CUtilities::m_fNoiseBuffers[sIdx] );
				__m256 vSig = _mm256_loadu_ps( pfSignals );
				vSig = _mm256_add_ps( vSig, vNoise );
				_mm256_storeu_ps( pfSignals, vSig );
				pfSignals += 8;

				vNoise = _mm256_load_ps( CUtilities::m_fNoiseBuffers[sIdx] + 8 );
				vSig = _mm256_loadu_ps( pfSignals );
				vSig = _mm256_add_ps( vSig, vNoise );
				_mm256_storeu_ps( pfSignals, vSig );
				pfSignals += 8;
			}
		}
#endif	// #ifdef __AVX__
#ifdef __SSE4_1__
		if LSN_LIKELY( CUtilities::IsSse4Supported() ) {
			while ( pfSignals < pfSignalEnd ) {
				size_t sIdx = LSN_NOISE_BUFFER( CUtilities::Rand() );
				
				__m128 vNoise = _mm_load_ps( CUtilities::m_fNoiseBuffers[sIdx] );
				__m128 vSig = _mm_loadu_ps( pfSignals );
				vSig = _mm_add_ps( vSig, vNoise );
				_mm_storeu_ps( pfSignals, vSig );
				pfSignals += 4;

				vNoise = _mm_load_ps( CUtilities::m_fNoiseBuffers[sIdx] + 4 );
				vSig = _mm_loadu_ps( pfSignals );
				vSig = _mm_add_ps( vSig, vNoise );
				_mm_storeu_ps( pfSignals, vSig );
				pfSignals += 4;

				vNoise = _mm_load_ps( CUtilities::m_fNoiseBuffers[sIdx] + 8 );
				vSig = _mm_loadu_ps( pfSignals );
				vSig = _mm_add_ps( vSig, vNoise );
				_mm_storeu_ps( pfSignals, vSig );
				pfSignals += 4;

				vNoise = _mm_load_ps( CUtilities::m_fNoiseBuffers[sIdx] + 12 );
				vSig = _mm_loadu_ps( pfSignals );
				vSig = _mm_add_ps( vSig, vNoise );
				_mm_storeu_ps( pfSignals, vSig );
				pfSignals += 4;
			}
		}
#endif	// #ifdef __SSE4_1__

		
		if ( !m_bPreProcessNormalization ) {
			// Normalize.
			pfSignals = pfSignalStart - i32Pad;
			// Prefetch the middle of the buffer.
			LSN_PREFETCH_LINE( pfSignals + m_ui16Width * 4 );
			
			float fScale = m_fWhiteSetting - m_fBlackSetting;
			float fInvScale = 1.0f / fScale;
			float fBlack = m_fBlackSetting;

#ifdef __AVX512F__
			if ( CUtilities::IsAvx512FSupported() ) {
				__m512 vBlack = _mm512_set1_ps( fBlack );
				__m512 vInvScale = _mm512_set1_ps( fInvScale );
				while ( pfSignals < pfSignalEnd ) {
					__m512 vSig = _mm512_loadu_ps( pfSignals );
					vSig = _mm512_sub_ps( vSig, vBlack );
					vSig = _mm512_mul_ps( vSig, vInvScale );
					_mm512_storeu_ps( pfSignals, vSig );
					pfSignals += 16;
				}
			}
#endif	// #ifdef __AVX512F__
#ifdef __AVX__
			if LSN_LIKELY( CUtilities::IsAvxSupported() ) {
				__m256 vBlack = _mm256_set1_ps( fBlack );
				__m256 vInvScale = _mm256_set1_ps( fInvScale );
				while ( pfSignals < pfSignalEnd ) {
					__m256 vSig = _mm256_loadu_ps( pfSignals );
					vSig = _mm256_sub_ps( vSig, vBlack );
					vSig = _mm256_mul_ps( vSig, vInvScale );
					_mm256_storeu_ps( pfSignals, vSig );
					pfSignals += 8;

					vSig = _mm256_loadu_ps( pfSignals );
					vSig = _mm256_sub_ps( vSig, vBlack );
					vSig = _mm256_mul_ps( vSig, vInvScale );
					_mm256_storeu_ps( pfSignals, vSig );
					pfSignals += 8;
				}
			}
#endif	// #ifdef __AVX__
#ifdef __SSE4_1__
			if LSN_LIKELY( CUtilities::IsSse4Supported() ) {
				__m128 vBlack = _mm_set1_ps( fBlack );
				__m128 vInvScale = _mm_set1_ps( fInvScale );
				while ( pfSignals < pfSignalEnd ) {
					__m128 vSig = _mm_loadu_ps( pfSignals );
					vSig = _mm_sub_ps( vSig, vBlack );
					vSig = _mm_mul_ps( vSig, vInvScale );
					_mm_storeu_ps( pfSignals, vSig );
					pfSignals += 4;

					vSig = _mm_loadu_ps( pfSignals );
					vSig = _mm_sub_ps( vSig, vBlack );
					vSig = _mm_mul_ps( vSig, vInvScale );
					_mm_storeu_ps( pfSignals, vSig );
					pfSignals += 4;

					vSig = _mm_loadu_ps( pfSignals );
					vSig = _mm_sub_ps( vSig, vBlack );
					vSig = _mm_mul_ps( vSig, vInvScale );
					_mm_storeu_ps( pfSignals, vSig );
					pfSignals += 4;

					vSig = _mm_loadu_ps( pfSignals );
					vSig = _mm_sub_ps( vSig, vBlack );
					vSig = _mm_mul_ps( vSig, vInvScale );
					_mm_storeu_ps( pfSignals, vSig );
					pfSignals += 4;
				}
			}
#endif	// #ifdef __SSE4_1__

			// Software fallback.
			while ( pfSignals < pfSignalEnd ) {
				(*pfSignals) = ((*pfSignals) - fBlack) * fInvScale;
				++pfSignals;
			}
		}
	}

	/**
	 * Decodes the signals of a scanline to YIQ values.
	 * 
	 * \param _pfDstY The destination for where to begin storing the YIQ Y values.  Must be aligned to a 16-byte boundary.
	 * \param _pfDstI The destination for where to begin storing the YIQ I values.  Must be aligned to a 16-byte boundary.
	 * \param _pfDstQ The destination for where to begin storing the YIQ Q values.  Must be aligned to a 16-byte boundary.
	 * \param _ui16Cycle The cycle count at the start of the scanline.
	 * \param _pfLuma The signals from which the Y values are filtered (the first signal of the scanline, with padding on both sides).
	 * \param _pfChroma The signals from which the I and Q values are demodulated (the first signal of the scanline, with padding on both sides).
	 **/
	void CLSpiroNtscFilterBase::ConvolveScanline( float * _pfDstY, float * _pfDstI, float * _pfDstQ, uint16_t _ui16Cycle, float * _pfLuma, float * _pfChroma ) {
		// Contrast is a gain around 0.5:  Y' = (Y * Brightness - 0.5) * Contrast + 0.5.
		float fBrightness = LSN_FINAL_BRIGHT * m_fContrastSetting;
		float fLumaOffset = 0.5f * (1.0f - m_fContrastSetting);
		const int16_t i16HalfLeft = int16_t( std::floorf( m_ui32FilterKernelSize / 2.0f ) );
		const int16_t i16HalfRight = int16_t( std::ceilf( m_ui32FilterKernelSize / 2.0f ) );
		const int16_t i16HalfLeftY = int16_t( std::floorf( m_ui32FilterKernelSizeY / 2.0f ) );
		const int16_t i16HalfRightY = int16_t( std::ceilf( m_ui32FilterKernelSizeY / 2.0f ) );
		for ( uint16_t I = 0; I < m_ui16ScaledWidth; ++I ) {
			//float fIdx = (float( I ) / (m_ui16ScaledWidth - 1) * (256.0f * 8.0f - 1.0f));
			//int16_t i16Center = int16_t( std::round( fIdx ) ) + 4;
			int16_t i16Center = int16_t( I * 8 / m_ui16WidthScale ) + 4;
			// Chroma.
			int16_t i16Start = i16Center - i16HalfLeft;
			int16_t i16End = i16Center + i16HalfRight;
			// Y.
			int16_t i16StartY = i16Center - i16HalfLeftY;
			int16_t i16EndY = i16Center + i16HalfRightY;

			(*_pfDstY) = (*_pfDstI) = (*_pfDstQ) = 0.0f;
			int16_t J = i16Start;
			int16_t K = i16StartY;
#ifdef __AVX512F__
			if ( CUtilities::IsAvx512FSupported() ) {
				__m512 mSin = _mm512_set1_ps( 0.0f ), mCos = _mm512_set1_ps( 0.0f ), mSig = _mm512_set1_ps( 0.0f );
				if LSN_LIKELY( CUtilities::IsFmaSupported() ) {
					while ( i16End - J >= 16 ) {
						// Can do 16 at a time.
						Convolution16_Fma( &_pfChroma[J], J - i16Start, (_ui16Cycle + (12 * 16) + J) % 12, mCos, mSin );
						J += 16;
					}
					while ( i16EndY - K >= 16 ) {
						Convolution16Y_Fma( &_pfLuma[K], K - i16StartY, mSig );
						K += 16;
					}
				}
				else {
					while ( i16End - J >= 16 ) {
						// Can do 16 at a time.
						Convolution16( &_pfChroma[J], J - i16Start, (_ui16Cycle + (12 * 16) + J) % 12, mCos, mSin );
						J += 16;
					}
					while ( i16EndY - K >= 16 ) {
						Convolution16Y( &_pfLuma[K], K - i16StartY, mSig );
						K += 16;
					}
				}
				(*_pfDstI) += CUtilities::HorizontalSum( mCos );
				(*_pfDstQ) += CUtilities::HorizontalSum( mSin );
				(*_pfDstY) += CUtilities::HorizontalSum( mSig );
			}
#endif	// #ifdef __AVX512F__
#ifdef __AVX__
			if LSN_LIKELY( CUtilities::IsAvxSupported() ) {
				__m256 mSin = _mm256_set1_ps( 0.0f ), mCos = _mm256_set1_ps( 0.0f ), mSig = _mm256_set1_ps( 0.0f );
				if LSN_LIKELY( CUtilities::IsFmaSupported() ) {
					while ( i16End - J >= 8 ) {
						// Can do 8 at a time.
						Convolution8_Fma( &_pfChroma[J], J - i16Start, (_ui16Cycle + (12 * 16) + J) % 12, mCos, mSin );
						J += 8;
					}
					while ( i16EndY - K >= 8 ) {
						Convolution8Y_Fma( &_pfLuma[K], K - i16StartY, mSig );
						K += 8;
					}
				}
				else {
					while ( i16End - J >= 8 ) {
						// Can do 8 at a time.
						Convolution8( &_pfChroma[J], J - i16Start, (_ui16Cycle + (12 * 16) + J) % 12, mCos, mSin );
						J += 8;
					}
					while ( i16EndY - K >= 8 ) {
						Convolution8Y( &_pfLuma[K], K - i16StartY, mSig );
						K += 8;
					}
				}
				(*_pfDstI) += CUtilities::HorizontalSum( mCos );
				(*_pfDstQ) += CUtilities::HorizontalSum( mSin );
				(*_pfDstY) += CUtilities::HorizontalSum( mSig );
			}
#endif	// #ifdef __AVX__
#ifdef __SSE4_1__
			if ( CUtilities::IsSse4Supported() ) {
				__m128 mSin = _mm_set1_ps( 0.0f ), mCos = _mm_set1_ps( 0.0f ), mSig = _mm_set1_ps( 0.0f );
				while ( i16End - J >= 4 ) {
					// Can do 4 at a time.
					Convolution4( &_pfChroma[J], J - i16Start, (_ui16Cycle + (12 * 16) + J) % 12, mCos, mSin );
					J += 4;
				}
				while ( i16EndY - K >= 4 ) {
					Convolution4Y( &_pfLuma[K], K - i16StartY, mSig );
					K += 4;
				}
				(*_pfDstI) += CUtilities::HorizontalSum( mCos );
				(*_pfDstQ) += CUtilities::HorizontalSum( mSin );
				(*_pfDstY) += CUtilities::HorizontalSum( mSig );
			}
#endif	// #ifdef __SSE4_1__
			{
				while ( i16End - J >= 1 ) {
					size_t sIdx = (_ui16Cycle + (12 * 16) + J) % 12;
					float fLevel = _pfChroma[J] * m_fFilter[J-i16Start];
					(*_pfDstI) += m_fPhaseCosTable[sIdx] * fLevel;
					(*_pfDstQ) += m_fPhaseSinTable[sIdx] * fLevel;
					++J;
				}
				while ( i16EndY - K >= 1 ) {
					(*_pfDstY) += _pfLuma[K] * m_fFilterY[K-i16StartY];
					++K;
				}
			}
			(*_pfDstY) = (*_pfDstY) * fBrightness + fLumaOffset;
			++_pfDstY;
			++_pfDstI;
			++_pfDstQ;
		}
	}

	/**
	 * Separates the luma and chroma of a scanline with the vertical comb filter, from the generated signals of the scanline and its
	 *	neighbors into m_vLumaStart[_sRowIdx] and m_vChromaStart[_sRowIdx].  Every scanline must have been generated first.
	 * 
	 * \param _sRowIdx The scanline index.
	 **/
	void CLSpiroNtscFilterBase::CombScanline( size_t _sRowIdx ) {
		const size_t sRows = m_vSignalStart.size();
		if LSN_UNLIKELY( _sRowIdx >= sRows || _sRowIdx >= m_vLumaStart.size() || _sRowIdx >= m_vChromaStart.size() ) { return; }
		// The range covered by the kernels:  the scanline and its padding.
		const int32_t i32Lo = -int32_t( m_ui32SignalPad );
		const int32_t i32Hi = int32_t( m_ui16Width * 8 ) + int32_t( m_ui32SignalPad );
		const float * pfCur = m_vSignalStart[_sRowIdx];
		float * pfLuma = m_vLumaStart[_sRowIdx];
		float * pfChroma = m_vChromaStart[_sRowIdx];

		// A 1H delay is 227.5 subcarrier cycles (2,730 signals) but an NES line is 2,728 signals long, so the line above lines up 2
		//	signals to the left (and the line below 2 to the right), where its chroma is inverted.  A missing neighbor (at the top or bottom
		//	of the picture) is replaced by the other one.
		const bool bUp = _sRowIdx > 0;
		const bool bDown = _sRowIdx + 1 < sRows;
		const float * pfUp = bUp ? m_vSignalStart[_sRowIdx-1] : (bDown ? m_vSignalStart[_sRowIdx+1] : pfCur);
		const int32_t i32UpOff = bUp ? -2 : (bDown ? 2 : 0);
		const float * pfDown = bDown ? m_vSignalStart[_sRowIdx+1] : (bUp ? m_vSignalStart[_sRowIdx-1] : pfCur);
		const int32_t i32DownOff = bDown ? 2 : (bUp ? -2 : 0);
		auto Clamp = [i32Lo, i32Hi]( int32_t _i32Idx ) { return std::clamp( _i32Idx, i32Lo, i32Hi - 1 ); };
		// The average over one subcarrier cycle (12 signals), which removes the chroma and keeps the luma.
		auto CycleMean = [&Clamp]( const float * _pfSrc, int32_t _i32Idx ) {
			float fSum = 0.0f;
			for ( int32_t T = -6; T < 6; ++T ) { fSum += _pfSrc[Clamp( _i32Idx + T )]; }
			return fSum * (1.0f / 12.0f);
		};

		// Pass 1:  the combed chroma (scaled by how much of it is used) into pfChroma and the share of the uncombed signal into pfLuma.
		constexpr float fMatch = 0.05f;			// Neighboring luma within this of the line combs fully.
		constexpr float fMismatch = 0.15f;		// Neighboring luma beyond this on both sides is not combed.
		for ( int32_t S = i32Lo; S < i32Hi; ++S ) {
			const float fCur = pfCur[S];
			const float fUp = pfUp[Clamp( S + i32UpOff )];
			const float fDown = pfDown[Clamp( S + i32DownOff )];
			float fComb = 0.0f, fUncombed = 0.0f;
			switch ( m_cfCombFilter ) {
				case LSN_CF_2_LINE : {
					fComb = (fCur - fUp) * 0.5f;
					break;
				}
				case LSN_CF_3_LINE : {
					fComb = (2.0f * fCur - fUp - fDown) * 0.25f;
					break;
				}
				case LSN_CF_ADAPTIVE_3_LINE : {
					// Comb with the neighbor whose luma matches better (in proportion), and fall back to no comb where neither matches.
					const float fMean = CycleMean( pfCur, S );
					const float fDiffUp = std::fabs( fMean - CycleMean( pfUp, S + i32UpOff ) );
					const float fDiffDown = std::fabs( fMean - CycleMean( pfDown, S + i32DownOff ) );
					const float fUpper = (fCur - fUp) * 0.5f;
					const float fLower = (fCur - fDown) * 0.5f;
					const float fTotal = fDiffUp + fDiffDown;
					fComb = (fTotal < 1.0e-6f) ? ((fUpper + fLower) * 0.5f) : ((fDiffDown * fUpper + fDiffUp * fLower) / fTotal);
					const float fNoComb = std::clamp( (std::min( fDiffUp, fDiffDown ) - fMatch) / (fMismatch - fMatch), 0.0f, 1.0f );
					fComb *= 1.0f - fNoComb;
					fUncombed = fNoComb * fCur;
					break;
				}
				default : {}
			}
			pfChroma[S] = fComb;
			pfLuma[S] = fUncombed;
		}

		// Pass 2:  keep only the chroma band of the combed chroma (removing what is below the subcarrier, which is vertical luma detail),
		//	then luma = signal - chroma and chroma = chroma + the uncombed share of the signal.  The window reads combed chroma that has
		//	already been overwritten to its left, so those values are kept in a small ring.
		float fRing[8] = {};
		for ( int32_t S = i32Lo; S < i32Hi; ++S ) {
			float fSum = 0.0f;
			for ( int32_t T = -6; T < 6; ++T ) {
				const int32_t i32Idx = Clamp( S + T );
				fSum += (i32Idx < S) ? fRing[i32Idx&7] : pfChroma[i32Idx];
			}
			const float fComb = pfChroma[S];
			fRing[S&7] = fComb;
			const float fChroma = fComb - fSum * (1.0f / 12.0f);
			const float fCur = pfCur[S];
			pfChroma[S] = fChroma + pfLuma[S];
			pfLuma[S] = fCur - fChroma;
		}
	}

	/**
	 * Sets the vertical comb filter.
	 * 
	 * \param _cfFilter The comb filter to use.  LSN_CF_NONE disables comb filtering.
	 * \return Returns true if the memory for the internal buffer(s) was allocated.
	 **/
	bool CLSpiroNtscFilterBase::SetCombFilter( LSN_COMB_FILTER _cfFilter ) {
		if ( _cfFilter >= LSN_CF_TOTAL ) { _cfFilter = LSN_CF_NONE; }
		if ( _cfFilter == m_cfCombFilter ) { return true; }
		LSN_COMB_FILTER cfBackup = m_cfCombFilter;
		m_cfCombFilter = _cfFilter;
		if ( !AllocYiqBuffers( m_ui16Width, m_ui16Height, m_ui16WidthScale ) ) {
			m_cfCombFilter = cfBackup;
			return false;
		}
		return true;
	}

	/**
	 * Generates the phase sin/cos tables.
	 * 
	 * \param _fHue The hue offset.
	 **/
	void CLSpiroNtscFilterBase::GenPhaseTables( float _fHue ) {
		for ( size_t I = 0; I < 12; ++I ) {
			double dSin, dCos;
			// 0.5 = 90 degrees.
			::sincos( std::numbers::pi * ((I - 0.5) / 6.0 + 0.5) + _fHue, &dSin, &dCos );
			dCos *= (LSN_FINAL_BRIGHT) * m_fSaturationSetting * m_fContrastSetting * 2.0;
			dSin *= (LSN_FINAL_BRIGHT) * m_fSaturationSetting * m_fContrastSetting * 2.0;

			// Straight version.
			m_fPhaseCosTable[I] = float( dCos );
			m_fPhaseSinTable[I] = float( dSin );
		}
#ifdef __AVX__
		for ( size_t J = 0; J < sizeof( __m256 ) / sizeof( float ); ++J ) {
			for ( size_t I = 0; I < 12; ++I ) {
				float * pfThis = reinterpret_cast<float *>(&m_mStackedCosTable[I]);
				pfThis[J] = m_fPhaseCosTable[(J+I)%12];

				pfThis = reinterpret_cast<float *>(&m_mStackedSinTable[I]);
				pfThis[J] = m_fPhaseSinTable[(J+I)%12];
			}
		}
#endif	// #ifdef __AVX__
#ifdef __AVX512F__
		for ( size_t J = 0; J < sizeof( __m512 ) / sizeof( float ); ++J ) {
			for ( size_t I = 0; I < 12; ++I ) {
				float * pfThis = reinterpret_cast<float *>(&m_mStackedCosTable512[I]);
				pfThis[J] = m_fPhaseCosTable[(J+I)%12];

				pfThis = reinterpret_cast<float *>(&m_mStackedSinTable512[I]);
				pfThis[J] = m_fPhaseSinTable[(J+I)%12];
			}
		}
#endif	// #ifdef __AVX512F__
	}

	/**
	 * Fills the __m128 registers with the black level and (white-black) level.
	 **/
	void CLSpiroNtscFilterBase::GenNormalizedSignals() {
		if ( m_bPreProcessNormalization ) {
			for ( size_t I = 0; I < 16; ++I ) {
				m_NormalizedLevels[I] = (CUtilities::m_fNtscLevels[I] - m_fBlackSetting) / (m_fWhiteSetting - m_fBlackSetting);
			}
		}
		else {
			for ( size_t I = 0; I < 16; ++I ) {
				m_NormalizedLevels[I] = CUtilities::m_fNtscLevels[I];
			}
		}
	}

	/**
	 * Generates the chroma filter kernel.
	 * 
	 * \param _ui32Width The width of the kernel.
	 **/
	void CLSpiroNtscFilterBase::GenFilterKernel( uint32_t _ui32Width ) {
		//double dSum = 0.0;
		for ( size_t I = 0; I < _ui32Width; ++I ) {
			m_fFilter[I] = CUtilities::FilterFunc( m_ffFilterFunc, I / (_ui32Width - 1.0f) * _ui32Width - (_ui32Width / 2.0f), _ui32Width / 2.0f, m_fFilterFuncParms[0], m_fFilterFuncParms[1] );
			//dSum += m_fFilter[I];
		}
		CUtilities::BakeBleedIntoKernel( m_fFilter, _ui32Width, m_fBleed );
		/*double dNorm = 1.0 / dSum;
		for ( size_t I = 0; I < _ui32Width; ++I ) {
			m_fFilter[I] = float( m_fFilter[I] * dNorm );
		}*/
#ifdef __AVX__
		for ( size_t J = 0; J < sizeof( __m256 ) / sizeof( float ); ++J ) {
			for ( size_t I = 0; I < LSN_MAX_FILTER_SIZE; ++I ) {
				float * pfDst = reinterpret_cast<float *>(&m_mStackedFilterTable[I]);
				pfDst[J] = m_fFilter[(J+I)%_ui32Width];
			}
		}
#endif	// #ifdef __AVX__
#ifdef __AVX512F__
		for ( size_t J = 0; J < sizeof( __m512 ) / sizeof( float ); ++J ) {
			for ( size_t I = 0; I < LSN_MAX_FILTER_SIZE; ++I ) {
				float * pfDst = reinterpret_cast<float *>(&m_mStackedFilterTable512[I]);
				pfDst[J] = m_fFilter[(J+I)%_ui32Width];
			}
		}
#endif	// #ifdef __AVX512F__
	}

	/**
	 * Generates the Y filter kernel.
	 * 
	 * \param _ui32Width The width of the kernel.
	 **/
	void CLSpiroNtscFilterBase::GenFilterKernelY( uint32_t _ui32Width ) {
		//double dSum = 0.0;
		for ( size_t I = 0; I < _ui32Width; ++I ) {
			m_fFilterY[I] = CUtilities::FilterFunc( m_ffFilterFuncY, I / (_ui32Width - 1.0f) * _ui32Width - (_ui32Width / 2.0f), _ui32Width / 2.0f, m_fFilterFuncParmsY[0], m_fFilterFuncParmsY[1] );
			//dSum += m_fFilterY[I];
		}
		CUtilities::BakeBleedIntoKernel( m_fFilterY, _ui32Width, m_fBleed );
		/*double dNorm = 1.0 / dSum;
		for ( size_t I = 0; I < _ui32Width; ++I ) {
			m_fFilterY[I] = float( m_fFilterY[I] * dNorm );
		}*/

#ifdef __AVX__
		for ( size_t J = 0; J < sizeof( __m256 ) / sizeof( float ); ++J ) {
			for ( size_t I = 0; I < LSN_MAX_FILTER_SIZE; ++I ) {
				float * pfDst = reinterpret_cast<float *>(&m_mStackedFilterTableY[I]);
				pfDst[J] = m_fFilterY[(J+I)%_ui32Width];
			}
		}
#endif	// #ifdef __AVX__
#ifdef __AVX512F__
		for ( size_t J = 0; J < sizeof( __m512 ) / sizeof( float ); ++J ) {
			for ( size_t I = 0; I < LSN_MAX_FILTER_SIZE; ++I ) {
				float * pfDst = reinterpret_cast<float *>(&m_mStackedFilterTable512Y[I]);
				pfDst[J] = m_fFilterY[(J+I)%_ui32Width];
			}
		}
#endif	// #ifdef __AVX512F__
	}

	/**
	 * Allocates the YIQ buffers for a given width and height.
	 * 
	 * \param _ui16W The width of the buffers.
	 * \param _ui16H The height of the buffers.
	 * \param _ui16Scale The width scale factor.
	 * \return Returns true if the allocations succeeded.
	 **/
	bool CLSpiroNtscFilterBase::AllocYiqBuffers( uint16_t _ui16W, uint16_t _ui16H, uint16_t _ui16Scale ) {
		try {
			// Buffer size:
			// [Padding][Signals][Padding][Slack for Alignment to 64 Bytes]
			// The padding covers half of the largest kernel plus a pixel (the kernels are centered on the middles of the pixels), in whole
			//	64-byte blocks so that the noise and normalization passes can run over it in blocks of 16.
			const uint32_t ui32Kernel = std::max( m_ui32FilterKernelSize, m_ui32FilterKernelSizeY );
			m_ui32SignalPad = ((ui32Kernel >> 1) + (ui32Kernel & 1) + uint32_t( 8 ) + 15) / 16 * 16;
			size_t sRowSize = LSN_PM_NTSC_M_RENDER_WIDTH * 8 + m_ui32SignalPad * 2 + 16;
			// Cleared so that nothing from a previous layout is left behind.
			m_vSignalBuffer.assign( sRowSize * _ui16H, 0.0f );
			m_vSignalStart.resize( _ui16H );
			for ( uint16_t H = 0; H < _ui16H; ++H ) {
				uintptr_t uiptrStart = reinterpret_cast<uintptr_t>(m_vSignalBuffer.data() + (sRowSize * H) + m_ui32SignalPad);
				uiptrStart = (uiptrStart + 63) / 64 * 64;
				m_vSignalStart[H] = reinterpret_cast<float *>(uiptrStart);
			}

			// The combed luma and chroma, laid out the same way (only with a comb filter).
			if ( CombFilterEnabled() ) {
				m_vCombBuffer.assign( sRowSize * _ui16H * 2, 0.0f );
				m_vLumaStart.resize( _ui16H );
				m_vChromaStart.resize( _ui16H );
				for ( uint16_t H = 0; H < _ui16H; ++H ) {
					uintptr_t uiptrStart = reinterpret_cast<uintptr_t>(m_vCombBuffer.data() + (sRowSize * H) + m_ui32SignalPad);
					m_vLumaStart[H] = reinterpret_cast<float *>((uiptrStart + 63) / 64 * 64);
					uiptrStart = reinterpret_cast<uintptr_t>(m_vCombBuffer.data() + (sRowSize * (H + size_t( _ui16H ))) + m_ui32SignalPad);
					m_vChromaStart[H] = reinterpret_cast<float *>((uiptrStart + 63) / 64 * 64);
				}
			}
			else {
				m_vCombBuffer = std::vector<float, CAlignmentAllocator<float>>();
				m_vLumaStart.clear();
				m_vChromaStart.clear();
			}


			size_t sSize = _ui16W * _ui16Scale * _ui16H;
			if ( !sSize ) { return true; }
			m_vY.resize( sSize );
			m_vI.resize( sSize );
			m_vQ.resize( sSize );

			//m_vRgbBuffer.resize( sSize * 4 );
			m_vBlendBuffer.resize( sSize * 3 );
			return true;
		}
		catch ( ... ) { return false; }
	}

}	// namespace lsn

#undef LSN_FINAL_BRIGHT

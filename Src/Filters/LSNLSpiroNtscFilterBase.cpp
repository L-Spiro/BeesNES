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

#define LSN_FINAL_BRIGHT							m_fBrightnessSetting// * (1.0f - (m_fPhosphorDecayRate /*m_fPhosphorDecayRate * (m_fInitPhosphorDecay * 2.0f)*/))

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
		GenPhaseTables( m_fHueSetting );			// Generate phase table.
		SetGamma( m_fGammaSetting );				// Generate gamma table.
		GenNormalizedSignals();						// Generate black/white normalization levels.
		SetKernelSize( m_ui32FilterKernelSize );	// Generate filter kernel weights.
		SetWidth( LSN_PM_NTSC_RENDER_WIDTH );		// Allocate buffers.
		SetHeight( 240 );							// Allocate buffers.		
	}
	CLSpiroNtscFilterBase::~CLSpiroNtscFilterBase() {
		
	}

	// == Functions.
	/**
	 * Sets the filter kernel size.
	 * 
	 * \param _ui32Size The new size of the filter.
	 * \return Returns true if the memory for the internal buffer(s) was allocated.
	 **/
	bool CLSpiroNtscFilterBase::SetKernelSize( uint32_t _ui32Size ) {
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
				double dVal = m_bHandleMonitorGamma ? CUtilities::LinearTosRGB_Precise( CUtilities::CrtProperToLinear( (I / (uint32_t( LSN_SRGB_RES ) - 1.0)) ) ) * 255.0 :
					CUtilities::CrtProperToLinear( (I / (uint32_t( LSN_SRGB_RES ) - 1.0)) ) * 255.0;
				//double dVal = CUtilities::LinearTosRGB_Precise( CUtilities::CrtProper2ToLinear( I / (uint32_t( LSN_SRGB_RES ) - 1.0) ) ) * 255;
				//double dVal = CUtilities::LinearTosRGB_Precise( std::pow( (I / (uint32_t( LSN_SRGB_RES ) - 1.0)), m_fGammaSetting ) ) * 255.0;
			
				//double dVal = CUtilities::LinearTosRGB_Precise( CUtilities::SMPTE170MtoLinear_Precise( (I / (uint32_t( LSN_SRGB_RES ) - 1.0)) ) ) * 255.0;
				//double dVal = (I / (uint32_t( LSN_SRGB_RES ) - 1.0)) * 255.0;

				dVal = std::min( dVal, 255.0 );
				dVal = std::max( dVal, 0.0 );
				m_ui8Gamma[I] = uint8_t( std::round( dVal ) );
				m_ui32Gamma[I] = m_ui8Gamma[I];

				dVal = m_bHandleMonitorGamma ? CUtilities::LinearTosRGB_Precise( CUtilities::CrtProperToLinear( (I / (uint32_t( LSN_SRGB_RES ) - 1.0)), 1.0, 0.0181 * 0.5 ) ) * 255.0 :
					CUtilities::CrtProperToLinear( (I / (uint32_t( LSN_SRGB_RES ) - 1.0)), 1.0, 0.0181 * 0.5 ) * 255.0;
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
		GenPhaseTables( _fHue );
	}

	/**
	 * Sets the brightness.
	 * 
	 * \param _fBrightness The brightness to set.
	 **/
	void CLSpiroNtscFilterBase::SetBrightness( float _fBrightness ) {
		m_fBrightnessSetting = _fBrightness;
		GenPhaseTables( m_fHueSetting );
	}

	/**
	 * Sets the saturation.
	 * 
	 * \param _fSat The saturation to set.
	 **/
	void CLSpiroNtscFilterBase::SetSaturation( float _fSat ) {
		m_fSaturationSetting = _fSat;
		GenPhaseTables( m_fHueSetting );
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
		float * pfSignalStart = m_vSignalStart[_sRowIdx];
		float * pfSignals = pfSignalStart;
		for ( uint16_t I = 0; I < m_ui16Width; ++I ) {
			PixelToNtscSignals( pfSignals, (*_pui16Pixels++), uint16_t( _ui16Cycle + I * 8 ) );
			pfSignals += 8;
		}

		// Add noise.
		pfSignals = pfSignalStart;
		float * pfSignalEnd = pfSignals + m_ui16Width * 8;
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

		float fBrightness = LSN_FINAL_BRIGHT;
		for ( uint16_t I = 0; I < m_ui16ScaledWidth; ++I ) {
			//float fIdx = (float( I ) / (m_ui16ScaledWidth - 1) * (256.0f * 8.0f - 1.0f));
			//int16_t i16Center = int16_t( std::round( fIdx ) ) + 4;
			int16_t i16Center = int16_t( I * 8 / m_ui16WidthScale ) + 4;
			int16_t i16Start = i16Center - int16_t( std::floorf( m_ui32FilterKernelSize / 2.0f ) );
			int16_t i16End = i16Center + int16_t( std::ceilf( m_ui32FilterKernelSize / 2.0f ) );

			(*_pfDstY) = (*_pfDstI) = (*_pfDstQ) = 0.0f;
			int16_t J = i16Start;
#ifdef __AVX512F__
			if ( CUtilities::IsAvx512FSupported() ) {
				__m512 mSin = _mm512_set1_ps( 0.0f ), mCos = _mm512_set1_ps( 0.0f ), mSig = _mm512_set1_ps( 0.0f );
				while ( i16End - J >= 16 ) {
					// Can do 16 at a time.
					Convolution16( &pfSignalStart[J], J - i16Start, (_ui16Cycle + (12 * 4) + J) % 12, mCos, mSin, mSig );
					J += 16;
				}
				(*_pfDstI) += CUtilities::HorizontalSum( mCos );
				(*_pfDstQ) += CUtilities::HorizontalSum( mSin );
				(*_pfDstY) += CUtilities::HorizontalSum( mSig );
			}
#endif	// #ifdef __AVX512F__
#ifdef __AVX__
			if LSN_LIKELY( CUtilities::IsAvxSupported() ) {
				__m256 mSin = _mm256_set1_ps( 0.0f ), mCos = _mm256_set1_ps( 0.0f ), mSig = _mm256_set1_ps( 0.0f );
				while ( i16End - J >= 8 ) {
					// Can do 8 at a time.
					Convolution8( &pfSignalStart[J], J - i16Start, (_ui16Cycle + (12 * 4) + J) % 12, mCos, mSin, mSig );
					J += 8;
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
					Convolution4( &pfSignalStart[J], J - i16Start, (_ui16Cycle + (12 * 4) + J) % 12, mCos, mSin, mSig );
					J += 4;
				}
				(*_pfDstI) += CUtilities::HorizontalSum( mCos );
				(*_pfDstQ) += CUtilities::HorizontalSum( mSin );
				(*_pfDstY) += CUtilities::HorizontalSum( mSig );
			}
#endif	// #ifdef __SSE4_1__
			{
				while ( i16End - J >= 1 ) {
					size_t sIdx = (_ui16Cycle + (12 * 4) + J) % 12;
					float fLevel = pfSignalStart[J] * m_fFilter[J-i16Start];
					(*_pfDstY) += pfSignalStart[J] * m_fFilterY[J-i16Start];
					(*_pfDstI) += m_fPhaseCosTable[sIdx] * fLevel;
					(*_pfDstQ) += m_fPhaseSinTable[sIdx] * fLevel;
					++J;
				}
			}
			(*_pfDstY++) *= fBrightness;
			++_pfDstI;
			++_pfDstQ;
		}
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
			dCos *= (LSN_FINAL_BRIGHT) * m_fSaturationSetting * 2.0;
			dSin *= (LSN_FINAL_BRIGHT) * m_fSaturationSetting * 2.0;

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
		for ( size_t I = 0; I < 16; ++I ) {
			m_NormalizedLevels[I] = (CUtilities::m_fNtscLevels[I] - m_fBlackSetting) / (m_fWhiteSetting - m_fBlackSetting);
		}
	}

	/**
	 * Generates the filter kernel.
	 * 
	 * \param _ui32Width The width of the kernel.
	 **/
	void CLSpiroNtscFilterBase::GenFilterKernel( uint32_t _ui32Width ) {
		double dSum = 0.0;
		for ( size_t I = 0; I < _ui32Width; ++I ) {
			m_fFilter[I] = m_pfFilterFunc( I / (_ui32Width - 1.0f) * _ui32Width - (_ui32Width / 2.0f), _ui32Width / 2.0f );
			dSum += m_fFilter[I];
		}
		double dNorm = 1.0 / dSum;
		for ( size_t I = 0; I < _ui32Width; ++I ) {
			m_fFilter[I] = float( m_fFilter[I] * dNorm );
		}
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


		dSum = 0.0;
		for ( size_t I = 0; I < _ui32Width; ++I ) {
			m_fFilterY[I] = m_pfFilterFuncY( I / (_ui32Width - 1.0f) * _ui32Width - (_ui32Width / 2.0f), _ui32Width / 2.0f );
			dSum += m_fFilterY[I];
		}
		dNorm = 1.0 / dSum;
		for ( size_t I = 0; I < _ui32Width; ++I ) {
			m_fFilterY[I] = float( m_fFilterY[I] * dNorm );
		}

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
			// [m_ui32FilterKernelSize/2][m_ui16Width*8][m_ui32FilterKernelSize/2][Padding for Alignment to 64 Bytes]
			size_t sRowSize = LSN_PM_NTSC_RENDER_WIDTH * 8 + m_ui32FilterKernelSize + 16;
			m_vSignalBuffer.resize( sRowSize * _ui16H );
			m_vSignalStart.resize( _ui16H );
			for ( uint16_t H = 0; H < _ui16H; ++H ) {
				uintptr_t uiptrStart = reinterpret_cast<uintptr_t>(m_vSignalBuffer.data() + (sRowSize * H) + ((m_ui32FilterKernelSize >> 1) + (m_ui32FilterKernelSize & 1)) );
				uiptrStart = (uiptrStart + 63) / 64 * 64;
				m_vSignalStart[H] = reinterpret_cast<float *>(uiptrStart);
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

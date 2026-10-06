/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: My own implementation of a PAL filter.
 */

#include "LSNLSpiroPalFilterBase.h"

#include "../Utilities/LSNScopedNoSubnormals.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <numbers>

#define LSN_FINAL_BRIGHT							m_fBrightnessSetting// * (1.0f - (m_fPhosphorDecayRateGreen /*m_fPhosphorDecayRateGreen * (m_fInitPhosphorDecay * 2.0f)*/))

namespace lsn {

	// == Members.
	CLSpiroPalFilterBase::CLSpiroPalFilterBase() {
		m_fHueSetting = (8.0f + 0.0f) * std::numbers::pi / 180.0f;					/**< The hue. */
		m_fGammaSetting = 2.8f;														/**< The CRT gamma curve. */
		m_fBrightnessSetting = 1.0f - 0.070710678118654752440084436210485f;			/**< The brightness setting. */
		m_fSaturationSetting = -0.40f + 1.0f;										/**< The saturation setting. */

		//m_fHueSetting = float( 33.0 * std::numbers::pi / 180.0 );
		GenPhaseTables( m_fHueSetting );			// Generate phase table.
		SetGamma( m_fGammaSetting );				// Generate gamma table.
		GenNormalizedSignals();						// Generate black/white normalization levels.
		SetKernelSize( m_ui32FilterKernelSize );	// Generate filter kernel weights.
		SetWidth( LSN_PM_PAL_RENDER_WIDTH );		// Allocate buffers.
		SetHeight( 240 );							// Allocate buffers.
	}
	CLSpiroPalFilterBase::~CLSpiroPalFilterBase() {
		
	}

	// == Functions.
	/**
	 * Sets the filter kernel size for both Y and chroma.
	 * 
	 * \param _ui32Size The new size of the filter.
	 * \return Returns true if the memory for the internal buffer(s) was allocated.
	 **/
	bool CLSpiroPalFilterBase::SetKernelSize( uint32_t _ui32Size ) {
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
	bool CLSpiroPalFilterBase::SetKernelSizeY( uint32_t _ui32Size ) {
		m_ui32FilterKernelSizeY = _ui32Size;
		GenFilterKernelY( m_ui32FilterKernelSizeY );
		if ( !AllocYiqBuffers( m_ui16Width, m_ui16Height, m_ui16WidthScale ) ) { return false; }
		return true;
	}

	/**
	 * Sets the chroma (U/V) filter kernel size.
	 * 
	 * \param _ui32Size The new size of the chroma filter.
	 * \return Returns true if the memory for the internal buffer(s) was allocated.
	 **/
	bool CLSpiroPalFilterBase::SetKernelSizeChroma( uint32_t _ui32Size ) {
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
	bool CLSpiroPalFilterBase::SetWidth( uint16_t _ui16Width ) {
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
	bool CLSpiroPalFilterBase::SetWidthScale( uint16_t _ui16WidthScale ) {
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
	bool CLSpiroPalFilterBase::SetHeight( uint16_t _ui16Height ) {
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
	void CLSpiroPalFilterBase::SetGamma( float _fGamma ) {
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
	void CLSpiroPalFilterBase::SetMonitorGammaApply( bool _bApplyMonitorGamma ) {
		m_bHandleMonitorGamma = _bApplyMonitorGamma;
		SetGamma( m_fGammaSetting );
	}

	/**
	 * Sets the hue.
	 * 
	 * \param _fHue The hue to set.
	 **/
	void CLSpiroPalFilterBase::SetHue( float _fHue ) {
		m_fHueSetting = _fHue;
		GenPhaseTables( _fHue );
	}

	/**
	 * Sets the brightness.
	 * 
	 * \param _fBrightness The brightness to set.
	 **/
	void CLSpiroPalFilterBase::SetBrightness( float _fBrightness ) {
		m_fBrightnessSetting = _fBrightness;
		GenPhaseTables( m_fHueSetting );
	}

	/**
	 * Sets the saturation.
	 * 
	 * \param _fSat The saturation to set.
	 **/
	void CLSpiroPalFilterBase::SetSaturation( float _fSat ) {
		m_fSaturationSetting = _fSat;
		GenPhaseTables( m_fHueSetting );
	}

	/**
	 * Sets the black level.
	 * 
	 * \param _fBlack The black level to set.
	 **/
	void CLSpiroPalFilterBase::SetBlackLevel( float _fBlack ) {
		m_fBlackSetting = _fBlack;
		GenNormalizedSignals();
	}
	
	/**
	 * Sets the white level.
	 * 
	 * \param _fWhite The white level to set.
	 **/
	void CLSpiroPalFilterBase::SetWhiteLevel( float _fWhite ) {
		m_fWhiteSetting = _fWhite;
		GenNormalizedSignals();
	}

	/**
	 * Sets the vertical comb filter.
	 * 
	 * \param _cfFilter The comb filter to use.  LSN_CF_NONE disables comb filtering.
	 * \return Returns true if the memory for the internal buffer(s) was allocated.
	 **/
	bool CLSpiroPalFilterBase::SetCombFilter( LSN_COMB_FILTER _cfFilter ) {
		if ( _cfFilter >= LSN_CF_TOTAL ) { _cfFilter = LSN_CF_NONE; }
		LSN_COMB_FILTER cfBackup = m_cfCombFilter;
		m_cfCombFilter = _cfFilter;
		if ( !AllocYiqBuffers( m_ui16Width, m_ui16Height, m_ui16WidthScale ) ) {
			m_cfCombFilter = cfBackup;
			return false;
		}
		return true;
	}

	/**
	 * Sets the number of signals per pixel.  10 for PAL, 8 for Dendy.
	 * 
	 * \param _ui16Value The value to set.
	 * \return Returns true if the memory for the internal buffer(s) was allocated.
	 **/
	bool CLSpiroPalFilterBase::SetPixelToSignal( uint16_t _ui16Value ) {
		uint16_t ui16Backup = m_ui16PixelToSignal;
		m_ui16PixelToSignal = _ui16Value;
		if ( !AllocYiqBuffers( m_ui16Width, m_ui16Height, m_ui16WidthScale ) ) {
			m_ui16PixelToSignal = ui16Backup;
			return false;
		}
		return true;
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
	void CLSpiroPalFilterBase::ScanlineToYiq( float * _pfDstY, float * _pfDstI, float * _pfDstQ, const uint16_t * _pui16Pixels, uint16_t _ui16Cycle, size_t _sRowIdx ) {
		float * pfSignalStart = m_vSignalStart[_sRowIdx];
		float * pfSignals = pfSignalStart;
		for ( uint16_t I = 0; I < m_ui16Width; ++I ) {
			PixelToPalSignals( pfSignals, (*_pui16Pixels++), uint16_t( _ui16Cycle + I * m_ui16PixelToSignal ), _sRowIdx );
			
			pfSignals += m_ui16PixelToSignal;
		}

		// Add noise.
		pfSignals = pfSignalStart;
		float * pfSignalEnd = pfSignals + m_ui16Width * m_ui16PixelToSignal;
		// Prefetch the middle of the buffer.
		LSN_PREFETCH_LINE( pfSignals + ((m_ui16Width * m_ui16PixelToSignal) >> 1) );
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
			pfSignals = pfSignalStart;
			// Prefetch the middle of the buffer.
			LSN_PREFETCH_LINE( pfSignals + ((m_ui16Width * m_ui16PixelToSignal) >> 1) );
			
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


		float fBrightness = LSN_FINAL_BRIGHT;
		uint16_t ui16HalfSig = m_ui16PixelToSignal >> 1;
		const int16_t i16HalfLeft = int16_t( std::floorf( m_ui32FilterKernelSize / 2.0f ) );
		const int16_t i16HalfRight = int16_t( std::ceilf( m_ui32FilterKernelSize / 2.0f ) );
		const int16_t i16HalfLeftY = int16_t( std::floorf( m_ui32FilterKernelSizeY / 2.0f ) );
		const int16_t i16HalfRightY = int16_t( std::ceilf( m_ui32FilterKernelSizeY / 2.0f ) );
		for ( uint16_t I = 0; I < m_ui16ScaledWidth; ++I ) {
			int16_t i16Center = int16_t( I * m_ui16PixelToSignal / m_ui16WidthScale ) + ui16HalfSig;
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
						uint16_t ui16CosIdx;
						/*if ( (_sRowIdx & 1) == 0 ) {
							ui16CosIdx = (_ui16Cycle + (12 * 4) + J + 6) % 12;
						}
						else {
							ui16CosIdx = (_ui16Cycle + (12 * 4) + J) % 12;
						}*/
						ui16CosIdx = (_ui16Cycle + (12 * 4) + J + (6 * ((_sRowIdx & 1) == 0))) % 12;
						// Can do 16 at a time.
						Convolution16_Fma( &pfSignalStart[J], J - i16Start, ui16CosIdx, (_ui16Cycle + (12 * 4) + J) % 12, mCos, mSin );
						J += 16;
					}
					while ( i16EndY - K >= 16 ) {
						Convolution16Y_Fma( &pfSignalStart[K], K - i16StartY, mSig );
						K += 16;
					}
				}
				else {
					while ( i16End - J >= 16 ) {
						uint16_t ui16CosIdx;
						/*if ( (_sRowIdx & 1) == 0 ) {
							ui16CosIdx = (_ui16Cycle + (12 * 4) + J + 6) % 12;
						}
						else {
							ui16CosIdx = (_ui16Cycle + (12 * 4) + J) % 12;
						}*/
						ui16CosIdx = (_ui16Cycle + (12 * 4) + J + (6 * ((_sRowIdx & 1) == 0))) % 12;
						// Can do 16 at a time.
						Convolution16( &pfSignalStart[J], J - i16Start, ui16CosIdx, (_ui16Cycle + (12 * 4) + J) % 12, mCos, mSin );
						J += 16;
					}
					while ( i16EndY - K >= 16 ) {
						Convolution16Y( &pfSignalStart[K], K - i16StartY, mSig );
						K += 16;
					}
				}
				(*_pfDstI) += CUtilities::HorizontalSum( mCos );
				(*_pfDstQ) += CUtilities::HorizontalSum( mSin );
				(*_pfDstY) += CUtilities::HorizontalSum( mSig );
			}
#endif	// #ifdef __AVX512F__
#ifdef __AVX__
			if ( CUtilities::IsAvxSupported() ) {
				__m256 mSin = _mm256_set1_ps( 0.0f ), mCos = _mm256_set1_ps( 0.0f ), mSig = _mm256_set1_ps( 0.0f );
				if LSN_LIKELY( CUtilities::IsFmaSupported() ) {
					while ( i16End - J >= 8 ) {
						uint16_t ui16CosIdx;
						/*if ( (_sRowIdx & 1) == 0 ) {
							ui16CosIdx = (_ui16Cycle + (12 * 4) + J + 6) % 12;
						}
						else {
							ui16CosIdx = (_ui16Cycle + (12 * 4) + J) % 12;
						}*/
						ui16CosIdx = (_ui16Cycle + (12 * 4) + J + (6 * ((_sRowIdx & 1) == 0))) % 12;
						// Can do 8 at a time.
						Convolution8_Fma( &pfSignalStart[J], J - i16Start, ui16CosIdx, (_ui16Cycle + (12 * 4) + J) % 12, mCos, mSin );
						J += 8;
					}
					while ( i16EndY - K >= 8 ) {
						Convolution8Y_Fma( &pfSignalStart[K], K - i16StartY, mSig );
						K += 8;
					}
				}
				else {
					while ( i16End - J >= 8 ) {
						uint16_t ui16CosIdx;
						/*if ( (_sRowIdx & 1) == 0 ) {
							ui16CosIdx = (_ui16Cycle + (12 * 4) + J + 6) % 12;
						}
						else {
							ui16CosIdx = (_ui16Cycle + (12 * 4) + J) % 12;
						}*/
						ui16CosIdx = (_ui16Cycle + (12 * 4) + J + (6 * ((_sRowIdx & 1) == 0))) % 12;
						// Can do 8 at a time.
						Convolution8( &pfSignalStart[J], J - i16Start, ui16CosIdx, (_ui16Cycle + (12 * 4) + J) % 12, mCos, mSin );
						J += 8;
					}
					while ( i16EndY - K >= 8 ) {
						Convolution8Y( &pfSignalStart[K], K - i16StartY, mSig );
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
					uint16_t ui16CosIdx;
					/*if ( (_sRowIdx & 1) == 0 ) {
						ui16CosIdx = (_ui16Cycle + (12 * 4) + J + 6) % 12;
					}
					else {
						ui16CosIdx = (_ui16Cycle + (12 * 4) + J) % 12;
					}*/
					ui16CosIdx = (_ui16Cycle + (12 * 4) + J + (6 * ((_sRowIdx & 1) == 0))) % 12;
					// Can do 4 at a time.
					Convolution4( &pfSignalStart[J], J - i16Start, ui16CosIdx, (_ui16Cycle + (12 * 4) + J) % 12, mCos, mSin );
					J += 4;
				}
				while ( i16EndY - K >= 4 ) {
					Convolution4Y( &pfSignalStart[K], K - i16StartY, mSig );
					K += 4;
				}
				(*_pfDstI) += CUtilities::HorizontalSum( mCos );
				(*_pfDstQ) += CUtilities::HorizontalSum( mSin );
				(*_pfDstY) += CUtilities::HorizontalSum( mSig );
			}
#endif	// #ifdef __SSE4_1__
			{
				while ( i16End - J >= 1 ) {
					uint16_t ui16CosIdx;
					/*if ( (_sRowIdx & 1) == 0 ) {
						ui16CosIdx = (_ui16Cycle + (12 * 4) + J + 6) % 12;
					}
					else {
						ui16CosIdx = (_ui16Cycle + (12 * 4) + J) % 12;
					}*/
					ui16CosIdx = (_ui16Cycle + (12 * 4) + J + (6 * ((_sRowIdx & 1) == 0))) % 12;
					float fLevel = pfSignalStart[J] * m_fFilter[J-i16Start];
					(*_pfDstI) += m_fPhaseCosTable[ui16CosIdx] * fLevel;
					(*_pfDstQ) += m_fPhaseSinTable[(_ui16Cycle+(12*4)+J)%12] * fLevel;
					++J;
				}
				while ( i16EndY - K >= 1 ) {
					(*_pfDstY) += pfSignalStart[K] * m_fFilterY[K-i16StartY];
					++K;
				}
			}
			(*_pfDstY++) *= fBrightness;
			++_pfDstI;
			++_pfDstQ;
		}
	}

	/**
	 * Decodes a range of scanlines to Y/U/V without converting them.
	 * 
	 * \param _pui8Pixels The input array of 9-bit PPU outputs.
	 * \param _ui16Start Index of the first scanline to decode.
	 * \param _ui16End Index of the end scanline.
	 * \param _ui64RenderStartCycle The PPU cycle at the start of the frame being rendered.
	 **/
	void CLSpiroPalFilterBase::DecodeScanlineRange( const uint8_t * _pui8Pixels, uint16_t _ui16Start, uint16_t _ui16End, uint64_t _ui64RenderStartCycle ) {
		size_t sYiqStride = m_ui16ScaledWidth;
		float * pfY = reinterpret_cast<float *>(m_vY.data()) + sYiqStride * _ui16Start;
		float * pfI = reinterpret_cast<float *>(m_vI.data()) + sYiqStride * _ui16Start;
		float * pfQ = reinterpret_cast<float *>(m_vQ.data()) + sYiqStride * _ui16Start;
		for ( uint16_t H = _ui16Start; H < _ui16End; ++H ) {
			const uint16_t * pui6PixelRow = reinterpret_cast<const uint16_t *>(_pui8Pixels + (m_ui16Width * sizeof( uint16_t )) * H);
			ScanlineToYiq( pfY, pfI, pfQ, pui6PixelRow, uint16_t( ((_ui64RenderStartCycle + LSN_PM_PAL_DOTS_X * H) * 8) % 12 ), H );
			pfY += sYiqStride;
			pfI += sYiqStride;
			pfQ += sYiqStride;
		}
	}

	/**
	 * Applies the 1H (delay-line) comb filter to the chroma of a decoded scanline, writing the result to m_vICombed/m_vQCombed.  Only decoded
	 *	(uncombed) scanlines are read.  The scanline above the first scanline is taken as the first scanline itself.
	 * 
	 * \param _sScanline The scanline to comb.
	 **/
	void CLSpiroPalFilterBase::CombScanline1H( size_t _sScanline ) {
		const size_t sAbove = _sScanline ? (_sScanline - 1) : _sScanline;
		const size_t sStride = m_ui16ScaledWidth;
		const float * pfSrc[2] = { reinterpret_cast<const float *>(m_vI.data()), reinterpret_cast<const float *>(m_vQ.data()) };
		float * pfDst[2] = { m_vICombed.data() + sStride * _sScanline, m_vQCombed.data() + sStride * _sScanline };
		const float fAbove = m_fCombWeight;
		const float fThis = 1.0f - m_fCombWeight;

		for ( size_t C = 0; C < 2; ++C ) {
			const float * pfA = pfSrc[C] + sStride * sAbove;
			const float * pfT = pfSrc[C] + sStride * _sScanline;
			float * pfD = pfDst[C];
			size_t I = 0;
#ifdef __AVX512F__
			if LSN_LIKELY( CUtilities::IsAvx512FSupported() ) {
				const __m512 mAbove = _mm512_set1_ps( fAbove ), mThis = _mm512_set1_ps( fThis );
				if LSN_LIKELY( CUtilities::IsFmaSupported() ) {
					for ( ; I + 16 <= sStride; I += 16 ) {
						_mm512_storeu_ps( pfD + I, _mm512_fmadd_ps( _mm512_loadu_ps( pfA + I ), mAbove, _mm512_mul_ps( _mm512_loadu_ps( pfT + I ), mThis ) ) );
					}
				}
				else {
					for ( ; I + 16 <= sStride; I += 16 ) {
						_mm512_storeu_ps( pfD + I, _mm512_add_ps( _mm512_mul_ps( _mm512_loadu_ps( pfA + I ), mAbove ), _mm512_mul_ps( _mm512_loadu_ps( pfT + I ), mThis ) ) );
					}
				}
			}
#endif	// #ifdef __AVX512F__
#ifdef __AVX__
			if LSN_LIKELY( CUtilities::IsAvxSupported() ) {
				const __m256 mAbove = _mm256_set1_ps( fAbove ), mThis = _mm256_set1_ps( fThis );
				if LSN_LIKELY( CUtilities::IsFmaSupported() ) {
					for ( ; I + 8 <= sStride; I += 8 ) {
						_mm256_storeu_ps( pfD + I, _mm256_fmadd_ps( _mm256_loadu_ps( pfA + I ), mAbove, _mm256_mul_ps( _mm256_loadu_ps( pfT + I ), mThis ) ) );
					}
				}
				else {
					for ( ; I + 8 <= sStride; I += 8 ) {
						_mm256_storeu_ps( pfD + I, _mm256_add_ps( _mm256_mul_ps( _mm256_loadu_ps( pfA + I ), mAbove ), _mm256_mul_ps( _mm256_loadu_ps( pfT + I ), mThis ) ) );
					}
				}
			}
#endif	// #ifdef __AVX__
#ifdef __SSE4_1__
			if LSN_LIKELY( CUtilities::IsSse4Supported() ) {
				const __m128 mAbove = _mm_set1_ps( fAbove ), mThis = _mm_set1_ps( fThis );
				for ( ; I + 4 <= sStride; I += 4 ) {
					_mm_storeu_ps( pfD + I, _mm_add_ps( _mm_mul_ps( _mm_loadu_ps( pfA + I ), mAbove ), _mm_mul_ps( _mm_loadu_ps( pfT + I ), mThis ) ) );
				}
			}
#endif	// #ifdef __SSE4_1__
			for ( ; I < sStride; ++I ) {
				pfD[I] = pfA[I] * fAbove + pfT[I] * fThis;
			}
		}
	}

	/**
	 * Applies the 2H (3-line) comb filter to the chroma of a decoded scanline, writing the result to m_vICombed/m_vQCombed.  Only decoded
	 *	(uncombed) scanlines are read.  Missing neighbors (above the first or below the last scanline) are taken as the scanline itself.
	 * 
	 * \param _sScanline The scanline to comb.
	 **/
	void CLSpiroPalFilterBase::CombScanline2H( size_t _sScanline ) {
		const size_t sAbove = _sScanline ? (_sScanline - 1) : _sScanline;
		const size_t sBelow = (_sScanline + 1 < m_ui16Height) ? (_sScanline + 1) : _sScanline;
		const size_t sStride = m_ui16ScaledWidth;
		const float * pfSrc[2] = { reinterpret_cast<const float *>(m_vI.data()), reinterpret_cast<const float *>(m_vQ.data()) };
		float * pfDst[2] = { m_vICombed.data() + sStride * _sScanline, m_vQCombed.data() + sStride * _sScanline };
		const float fSide = m_fCombWeight * 0.5f;
		const float fThis = 1.0f - m_fCombWeight;

		for ( size_t C = 0; C < 2; ++C ) {
			const float * pfA = pfSrc[C] + sStride * sAbove;
			const float * pfT = pfSrc[C] + sStride * _sScanline;
			const float * pfB = pfSrc[C] + sStride * sBelow;
			float * pfD = pfDst[C];
			size_t I = 0;
#ifdef __AVX512F__
			if LSN_LIKELY( CUtilities::IsAvx512FSupported() ) {
				const __m512 mSide = _mm512_set1_ps( fSide ), mThis = _mm512_set1_ps( fThis );
				if LSN_LIKELY( CUtilities::IsFmaSupported() ) {
					for ( ; I + 16 <= sStride; I += 16 ) {
						_mm512_storeu_ps( pfD + I, _mm512_fmadd_ps( _mm512_add_ps( _mm512_loadu_ps( pfA + I ), _mm512_loadu_ps( pfB + I ) ), mSide, _mm512_mul_ps( _mm512_loadu_ps( pfT + I ), mThis ) ) );
					}
				}
				else {
					for ( ; I + 16 <= sStride; I += 16 ) {
						_mm512_storeu_ps( pfD + I, _mm512_add_ps( _mm512_mul_ps( _mm512_add_ps( _mm512_loadu_ps( pfA + I ), _mm512_loadu_ps( pfB + I ) ), mSide ), _mm512_mul_ps( _mm512_loadu_ps( pfT + I ), mThis ) ) );
					}
				}
			}
#endif	// #ifdef __AVX512F__
#ifdef __AVX__
			if LSN_LIKELY( CUtilities::IsAvxSupported() ) {
				const __m256 mSide = _mm256_set1_ps( fSide ), mThis = _mm256_set1_ps( fThis );
				if LSN_LIKELY( CUtilities::IsFmaSupported() ) {
					for ( ; I + 8 <= sStride; I += 8 ) {
						_mm256_storeu_ps( pfD + I, _mm256_fmadd_ps( _mm256_add_ps( _mm256_loadu_ps( pfA + I ), _mm256_loadu_ps( pfB + I ) ), mSide, _mm256_mul_ps( _mm256_loadu_ps( pfT + I ), mThis ) ) );
					}
				}
				else {
					for ( ; I + 8 <= sStride; I += 8 ) {
						_mm256_storeu_ps( pfD + I, _mm256_add_ps( _mm256_mul_ps( _mm256_add_ps( _mm256_loadu_ps( pfA + I ), _mm256_loadu_ps( pfB + I ) ), mSide ), _mm256_mul_ps( _mm256_loadu_ps( pfT + I ), mThis ) ) );
					}
				}
			}
#endif	// #ifdef __AVX__
#ifdef __SSE4_1__
			if LSN_LIKELY( CUtilities::IsSse4Supported() ) {
				const __m128 mSide = _mm_set1_ps( fSide ), mThis = _mm_set1_ps( fThis );
				for ( ; I + 4 <= sStride; I += 4 ) {
					_mm_storeu_ps( pfD + I, _mm_add_ps( _mm_mul_ps( _mm_add_ps( _mm_loadu_ps( pfA + I ), _mm_loadu_ps( pfB + I ) ), mSide ), _mm_mul_ps( _mm_loadu_ps( pfT + I ), mThis ) ) );
				}
			}
#endif	// #ifdef __SSE4_1__
			for ( ; I < sStride; ++I ) {
				pfD[I] = (pfA[I] + pfB[I]) * fSide + pfT[I] * fThis;
			}
		}
	}

	/**
	 * Generates the phase sin/cos tables.
	 * 
	 * \param _fHue The hue offset.
	 **/
	void CLSpiroPalFilterBase::GenPhaseTables( float _fHue ) {
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
	void CLSpiroPalFilterBase::GenNormalizedSignals() {
		if ( m_bPreProcessNormalization ) {
			for ( size_t I = 0; I < 16; ++I ) {
				m_NormalizedLevels[I] = (CUtilities::m_fPalLevels[I] - m_fBlackSetting) / (m_fWhiteSetting - m_fBlackSetting);
			}
		}
		else {
			for ( size_t I = 0; I < 16; ++I ) {
				m_NormalizedLevels[I] = CUtilities::m_fPalLevels[I];
			}
		}
	}

	/**
	 * Generates the chroma filter kernel.
	 * 
	 * \param _ui32Width The width of the kernel.
	 **/
	void CLSpiroPalFilterBase::GenFilterKernel( uint32_t _ui32Width ) {
		//double dSum = 0.0;
		for ( size_t I = 0; I < _ui32Width; ++I ) {
			m_fFilter[I] = m_pfFilterFunc( I / (_ui32Width - 1.0f) * _ui32Width - (_ui32Width / 2.0f), _ui32Width / 2.0f );
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
	void CLSpiroPalFilterBase::GenFilterKernelY( uint32_t _ui32Width ) {
		//double dSum = 0.0;
		for ( size_t I = 0; I < _ui32Width; ++I ) {
			m_fFilterY[I] = m_pfFilterFuncY( I / (_ui32Width - 1.0f) * _ui32Width - (_ui32Width / 2.0f), _ui32Width / 2.0f );
			//dSum += m_fFilterY[I];
		}
		/*double dNorm = 1.0 / dSum;
		for ( size_t I = 0; I < _ui32Width; ++I ) {
			m_fFilterY[I] = float( m_fFilterY[I] * dNorm );
		}*/
		CUtilities::BakeBleedIntoKernel( m_fFilterY, _ui32Width, m_fBleed );

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
	bool CLSpiroPalFilterBase::AllocYiqBuffers( uint16_t _ui16W, uint16_t _ui16H, uint16_t _ui16Scale ) {
		try {
			// Buffer size:
			// [ui32Kernel/2][m_ui16Width*m_ui16PixelToSignal][ui32Kernel/2][Padding for Alignment to 64 Bytes]
			const uint32_t ui32Kernel = std::max( m_ui32FilterKernelSize, m_ui32FilterKernelSizeY );
			size_t sRowSize = LSN_PM_PAL_RENDER_WIDTH * m_ui16PixelToSignal + ui32Kernel + 16;
			m_vSignalBuffer.resize( sRowSize * _ui16H );
			m_vSignalStart.resize( _ui16H );
			for ( uint16_t H = 0; H < _ui16H; ++H ) {
				uintptr_t uiptrStart = reinterpret_cast<uintptr_t>(m_vSignalBuffer.data() + (sRowSize * H) + ((ui32Kernel >> 1) + (ui32Kernel & 1)) );
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
			if ( CombFilterEnabled() ) {
				m_vICombed.resize( sSize );
				m_vQCombed.resize( sSize );
			}
			return true;
		}
		catch ( ... ) { return false; }
	}

}	// namespace lsn

#undef LSN_FINAL_BRIGHT

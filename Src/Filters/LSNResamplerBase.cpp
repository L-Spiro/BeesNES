/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Base GPU resampler setup.
 */

#include "LSNResamplerBase.h"
#include "../OS/LSNOs.h"
#include "../Utilities/LSNUtilities.h"


namespace lsn {

	// == Members.
	/** Filter parameters. */
	CResamplerBase::LSN_FILTER CResamplerBase::m_fFilter[] = {
		{ CResamplerBase::PointFilterFunc,					0.5f,		L"Point" },
		{ CResamplerBase::BilinearFilterFunc,				1.0f,		L"Linear" },
		{ CResamplerBase::QuadraticSharpFilterFunc,			1.0f,		L"Quadratic Sharp" },
		{ CResamplerBase::QuadraticInterpolFilterFunc,		1.5f,		L"Quadratic Interpolation" },
		{ CResamplerBase::QuadraticApproxFilterFunc,		1.5f,		L"Quadratic Approximation" },
		{ CResamplerBase::QuadraticMixFilterFunc,			1.5f,		L"Quadratic Mix" },
		{ CResamplerBase::KaiserFilterFunc,					3.0f,		L"Kaiser" },
		{ CResamplerBase::LanczosXFilterFunc<2>,			2.0f,		L"Lanczos 2" },
		{ CResamplerBase::LanczosXFilterFunc<3>,			3.0f,		L"Lanczos 3" },
		{ CResamplerBase::LanczosXFilterFunc<4>,			4.0f,		L"Lanczos 4" },
		{ CResamplerBase::LanczosXFilterFunc<6>,			6.0f,		L"Lanczos 6" },
		{ CResamplerBase::LanczosXFilterFunc<8>,			8.0f,		L"Lanczos 8" },
		{ CResamplerBase::LanczosXFilterFunc<12>,			12.0f,		L"Lanczos 12" },
		{ CResamplerBase::LanczosXFilterFunc<64>,			64.0f,		L"Lanczos 64" },
		{ CResamplerBase::MitchellFilterFunc,				2.0f,		L"Mitchell" },
		{ CResamplerBase::RobidouxFilterFunc,				2.0f,		L"Robidoux" },
		{ CResamplerBase::RobidouxSharpFilterFunc,			2.0f,		L"Robidoux Sharp" },
		{ CResamplerBase::RobidouxSoftFilterFunc,			2.0f,		L"Robidoux Soft" },
		{ CResamplerBase::CatmullRomFilterFunc,				2.0f,		L"Catmull-Rom" },
		{ CResamplerBase::BSplineFilterFunc,				2.0f,		L"B-Spline" },
		{ CResamplerBase::AdobeBicubicFilterFunc,			2.0f,		L"Adobe® Cubic" },
		{ CResamplerBase::CardinalSplineUniformFilterFunc,	2.0f,		L"L. Spiro/Adobe® Cubic Sharp" },
		{ CResamplerBase::HermiteFilterFunc,				1.0f,		L"Hermite" },
		{ CResamplerBase::HammingFilterFunc,				1.0f,		L"Hamming" },
		{ CResamplerBase::HanningFilterFunc,				1.0f,		L"Hanning" },
		{ CResamplerBase::BlackmanFilterFunc,				3.0f,		L"Blackman" },
		{ CResamplerBase::JincFilterFunc,					3.0f,		L"Jinc" },
		{ CResamplerBase::GaussianSharpFilterFunc,			1.25f,		L"Gaussian Sharp" },
		{ CResamplerBase::GaussianFilterFunc,				1.25f,		L"Gaussian" },
		{ CResamplerBase::BellFilterFunc,					1.5f,		L"Bell" },
	};

	CResamplerBase::CResamplerBase() {
	}
	CResamplerBase::~CResamplerBase() {
	}

	// == Functions.
	/**
	 * Resamples an image.
	 * 
	 * \param _pfIn The input buffer.
	 * \param _pfOut The output buffer.
	 * \param _pParms Image/resampling parameters.
	 * \return Returns true if all allocations succeed.
	 */
	bool CResamplerBase::Resample( const float * _pfIn, float * _pfOut, const LSN_RESAMPLE &_pParms ) {
		LSN_RESAMPLE rRes = _pParms;

		std::vector<float> fBufferR;
		std::vector<float> fBufferG;
		std::vector<float> fBufferB;
		std::vector<float> fBufferA;
		std::vector<float> fBufferR2;
		std::vector<float> fBufferG2;
		std::vector<float> fBufferB2;
		std::vector<float> fBufferA2;

		uint32_t ui32NewW = std::max( 1U, rRes.ui32NewW );
		uint32_t ui32NewH = std::max( 1U, rRes.ui32NewH );
		uint32_t ui32NewD = std::max( 1U, rRes.ui32NewD );
		uint32_t ui32W = std::max( 1U, rRes.ui32W );
		uint32_t ui32H = std::max( 1U, rRes.ui32H );
		uint32_t ui32D = std::max( 1U, rRes.ui32D );
		try {
			size_t sSize = ui32NewW * ui32H * ui32D;
			fBufferR.resize( sSize );
			fBufferG.resize( sSize );
			fBufferB.resize( sSize );
			if ( rRes.bAlpha ) {
				fBufferA.resize( sSize );
			}
			if ( ui32D > 1 || ui32NewD > 1 ) {
				sSize = ui32NewW * ui32NewH * ui32D;
				fBufferR2.resize( sSize );
				fBufferG2.resize( sSize );
				fBufferB2.resize( sSize );
				if ( rRes.bAlpha ) {
					fBufferA2.resize( sSize );
				}
			}
		}
		catch ( ... ) { return false; }

		// Resize width first for best caching.
		if ( !CreateContribList( ui32W, ui32NewW, rRes.taColorW, rRes.fFilterW.pfFunc, rRes.fFilterW.fSupport, rRes.fFilterScale ) ) { return false; }
		float * pfDst[4] = { fBufferR.data(), fBufferG.data(), fBufferB.data(), fBufferA.data() };
		// Resize W.
		size_t sPagesSize = ui32W * ui32H * 4;
		size_t sNewPagesSize = ui32NewW * ui32H;

		/*uint32_t ui32DstW = ui32H;
		uint32_t ui32DstH = ui32NewW;*/
		for ( size_t I = 0; I < (rRes.bAlpha ? 4 : 3); ++I ) {
			if ( I == 3 ) {
				// Alpha channel.
				if ( !CreateContribList( ui32W, ui32NewW, rRes.taAlphaW, rRes.fAlphaFilterW.pfFunc, rRes.fAlphaFilterW.fSupport, rRes.fFilterScale ) ) { return false; }
			}
			for ( size_t D = 0; D < ui32D; ++D ) {
				for ( size_t H = 0; H < ui32H; ++H ) {
					const float * pfRowStart = _pfIn + I + ((sPagesSize * D) + (H * ui32W * 4));
					for ( size_t W = 0; W < ui32NewW; ++W ) {
						for ( size_t J = 0; J < m_cContribs[W].i32Indices.size(); ++J ) {
							int32_t i32Index = m_cContribs[W].i32Indices[J];
							if ( i32Index == -1 ) {
								m_fBuffer[J] = rRes.fBorderColor[I];
							}
							else {
								m_fBuffer[J] = pfRowStart[i32Index*4];
							}
						}
						float fConvolved = ConvolveAligned( m_cContribs[W].fContributions.data(), m_fBuffer.data(), m_cContribs[W].fContributions.size() );
						size_t sDstIdx = (sNewPagesSize * D) + (W * ui32H) + H;
						pfDst[I][sDstIdx] = fConvolved;
					}
				}
			}
		}

		// Resize H, now aligned horizontally in the buffers.
		if ( !CreateContribList( ui32H, ui32NewH, rRes.taColorH, rRes.fFilterH.pfFunc, rRes.fFilterH.fSupport, rRes.fFilterScale ) ) { return false; }
		sPagesSize = sNewPagesSize;
		sNewPagesSize = ui32NewW * ui32NewH;
		uint32_t tW = ui32H;
		uint32_t tH = ui32NewW;
		float * pfDst2[4] = { fBufferR2.data(), fBufferG2.data(), fBufferB2.data(), fBufferA2.data() };
		for ( size_t I = 0; I < (rRes.bAlpha ? 4 : 3); ++I ) {
			if ( I == 3 ) {
				// Alpha channel.
				if ( !CreateContribList( ui32H, ui32NewH, rRes.taAlphaH, rRes.fAlphaFilterH.pfFunc, rRes.fAlphaFilterH.fSupport, rRes.fFilterScale ) ) { return false; }
			}
			for ( size_t D = 0; D < ui32D; ++D ) {
				for ( size_t H = 0; H < tH; ++H ) {
					const float * pfRowStart = pfDst[I] + (sPagesSize * D) + (H * tW);
					for ( size_t W = 0; W < ui32NewH; ++W ) {
						float fConvolved;
						if ( m_cContribs[W].bInsideBounds ) {
							fConvolved = ConvolveUnaligned( m_cContribs[W].fContributions.data(), pfRowStart + m_cContribs[W].i32Indices[0], m_cContribs[W].fContributions.size() );
						}
						else {
							for ( size_t J = 0; J < m_cContribs[W].i32Indices.size(); ++J ) {
								int32_t i32Index = m_cContribs[W].i32Indices[J];
								if ( i32Index == -1 ) {
									m_fBuffer[J] = rRes.fBorderColor[I];
								}
								else {
									m_fBuffer[J] = (*(pfRowStart + i32Index));
								}
							}
							fConvolved = ConvolveAligned( m_cContribs[W].fContributions.data(), m_fBuffer.data(), m_cContribs[W].fContributions.size() );
						}
						if ( ui32D > 1 || ui32NewD > 1 ) {
							size_t sDstIdx = ((ui32NewW * ui32D) * W) + (H * ui32D) + D;
							pfDst2[I][sDstIdx] = fConvolved;
						}
						else {
							size_t sDstIdx = ((sNewPagesSize * D) + (W * ui32NewW) + H) * 4 + I;
							_pfOut[sDstIdx] = fConvolved;
						}
					}
				}
			}
		}

		if ( ui32D > 1 || ui32NewD > 1 ) {
			// Resize D, now aligned horizontally in the buffers.
			if ( !CreateContribList( ui32D, ui32NewD, rRes.taColorD, rRes.fFilterD.pfFunc, rRes.fFilterD.fSupport, rRes.fFilterScale ) ) { return false; }
			sPagesSize = ui32NewW * ui32D;
			sNewPagesSize = ui32NewW * ui32NewH;
			tW = ui32NewD;
			tH = ui32NewW;
			uint32_t tD = ui32NewH;

			for ( size_t I = 0; I < (rRes.bAlpha ? 4 : 3); ++I ) {
				if ( I == 3 ) {
					// Alpha channel.
					if ( !CreateContribList( ui32D, ui32NewD, rRes.taAlphaD, rRes.fAlphaFilterD.pfFunc, rRes.fAlphaFilterD.fSupport, rRes.fFilterScale ) ) { return false; }
				}
				for ( size_t D = 0; D < tD; ++D ) {
					for ( size_t H = 0; H < tH; ++H ) {
						const float * pfRowStart = pfDst2[I] + (sPagesSize * D) + (H * ui32D);
						for ( size_t W = 0; W < tW; ++W ) {
							float fConvolved;
							if ( m_cContribs[W].bInsideBounds ) {
								fConvolved = ConvolveUnaligned( m_cContribs[W].fContributions.data(), pfRowStart + m_cContribs[W].i32Indices[0], m_cContribs[W].fContributions.size() );
							}
							else {
								for ( size_t J = 0; J < m_cContribs[W].i32Indices.size(); ++J ) {
									int32_t i32Index = m_cContribs[W].i32Indices[J];
									if ( i32Index == -1 ) {
										m_fBuffer[J] = rRes.fBorderColor[I];
									}
									else {
										m_fBuffer[J] = (*(pfRowStart + i32Index));
									}
								}
								fConvolved = ConvolveAligned( m_cContribs[W].fContributions.data(), m_fBuffer.data(), m_cContribs[W].fContributions.size() );
							}
							//size_t sDstIdx = ((sNewPagesSize * W) + (D * ui32NewW) + H) * 4 + I;
							size_t sDstIdx = ((sNewPagesSize * W) + (D * ui32NewW) + H) * 4 + I;
							_pfOut[sDstIdx] = fConvolved;
						}
					}
				}
			}

		}

		{
			// Add alpha to the output.
			if ( !rRes.bAlpha ) {
				sPagesSize = ui32NewH * ui32NewW;
				for ( size_t D = 0; D < ui32NewD; ++D ) {
					for ( size_t H = 0; H < ui32NewH; ++H ) {
						for ( size_t W = 0; W < ui32NewW; ++W ) {
							size_t sDstIdx = ((sPagesSize * D) + (H * ui32NewW) + W) * 4;
							_pfOut[sDstIdx+3] = 1.0f;
						}
					}
				}
			}
		}
		return true;
	}

	/**
	 * Convolvinate.
	 *
	 * \param _pfWeights The convolution weights.
	 * \param _pfTexels The texels to be convolved.
	 * \param _sTotal The total values to which _pfTexels and _pfWeights point.
	 * \return Returns the summed weights * texels.
	 */
	float CResamplerBase::ConvolveAligned( const float * _pfWeights, const float * _pfTexels, size_t _sTotal ) {
		LSN_ALIGN( 64 )
		float fSum[8] = { 0.0f };
#ifdef __AVX512F__
		if ( CUtilities::IsAvx512FSupported() ) {
			__m512 mAcc = _mm512_setzero_ps();
			while ( _sTotal >= sizeof( __m512 ) / sizeof( float ) ) {
				__m512 mW = _mm512_load_ps( _pfWeights );
				__m512 dT = _mm512_load_ps( _pfTexels );
				mAcc = _mm512_fmadd_ps( mW, dT, mAcc );
				_pfWeights += sizeof( __m512 ) / sizeof( float );
				_pfTexels += sizeof( __m512 ) / sizeof( float );
				_sTotal -= sizeof( __m512 ) / sizeof( float );
			}
			__m256 mLow256 = _mm512_castps512_ps256( mAcc );			// Low 256 bits.
			__m256 mHigh256 = _mm512_extractf32x8_ps( mAcc, 1 );		// High 256 bits.
			__m256 mSum = _mm256_add_ps( mLow256, mHigh256 );
			_mm256_store_ps( fSum, mSum );
			fSum[0] += fSum[1] + fSum[2] + fSum[3] + fSum[4] + fSum[5] + fSum[6] + fSum[7];
		}
#endif	// #ifdef __AVX512F__

#ifdef __AVX__
		if ( CUtilities::IsAvxSupported() ) {
			__m256 mAcc = _mm256_setzero_ps();
			while ( _sTotal >= sizeof( __m256 ) / sizeof( float ) ) {
				__m256 mW = _mm256_load_ps( _pfWeights );
				__m256 dT = _mm256_load_ps( _pfTexels );
				mAcc = _mm256_add_ps( _mm256_mul_ps( mW, dT ), mAcc );
				_pfWeights += sizeof( __m256 ) / sizeof( float );
				_pfTexels += sizeof( __m256 ) / sizeof( float );
				_sTotal -= sizeof( __m256 ) / sizeof( float );
			}
			fSum[0] += CUtilities::HorizontalSum( mAcc );
		}
#endif	// #ifdef __AVX__

#ifdef __SSE4_1__
		if ( CUtilities::IsSse4Supported() ) {
			__m128 mAcc = _mm_setzero_ps();
			while ( _sTotal >= sizeof( __m128 ) / sizeof( float ) ) {
				__m128 mW = _mm_load_ps( _pfWeights );
				__m128 dT = _mm_load_ps( _pfTexels );
				mAcc = _mm_add_ps( _mm_mul_ps( mW, dT ), mAcc );
				_pfWeights += sizeof( __m128 ) / sizeof( float );
				_pfTexels += sizeof( __m128 ) / sizeof( float );
				_sTotal -= sizeof( __m128 ) / sizeof( float );
			}
			fSum[0] += CUtilities::HorizontalSum( mAcc );
		}
#endif	// #ifdef __SSE4_1__

		while ( _sTotal >= 1 ) {
			fSum[0] += (*_pfWeights++) * (*_pfTexels++);
			--_sTotal;
		}
		return fSum[0];
	}

	/**
	 * Convolvinate using unaligned reads.
	 *
	 * \param _pfWeights The convolution weights.
	 * \param _pfTexels The texels to be convolved.
	 * \param _sTotal The total values to which _pfTexels and _pfWeights point.
	 * \return Returns the summed weights * texels.
	 */
	float CResamplerBase::ConvolveUnaligned( const float * _pfWeights, const float * _pfTexels, size_t _sTotal ) {
		LSN_ALIGN( 64 )
		float fSum[8] = { 0.0f };
#ifdef __AVX512F__
		if ( CUtilities::IsAvx512FSupported() ) {
			__m512 mAcc = _mm512_setzero_ps();
			while ( _sTotal >= sizeof( __m512 ) / sizeof( float ) ) {
				__m512 mW = _mm512_load_ps( _pfWeights );
				__m512 dT = _mm512_loadu_ps( _pfTexels );
				mAcc = _mm512_fmadd_ps( mW, dT, mAcc );
				_pfWeights += sizeof( __m512 ) / sizeof( float );
				_pfTexels += sizeof( __m512 ) / sizeof( float );
				_sTotal -= sizeof( __m512 ) / sizeof( float );
			}
			__m256 mLow256 = _mm512_castps512_ps256( mAcc );			// Low 256 bits.
			__m256 mHigh256 = _mm512_extractf32x8_ps( mAcc, 1 );		// High 256 bits.
			__m256 mSum = _mm256_add_ps( mLow256, mHigh256 );
			_mm256_store_ps( fSum, mSum );
			fSum[0] += fSum[1] + fSum[2] + fSum[3] + fSum[4] + fSum[5] + fSum[6] + fSum[7];
		}
#endif	// #ifdef __AVX512F__

#ifdef __AVX__
		if ( CUtilities::IsAvxSupported() ) {
			__m256 mAcc = _mm256_setzero_ps();
			while ( _sTotal >= sizeof( __m256 ) / sizeof( float ) ) {
				__m256 mW = _mm256_load_ps( _pfWeights );
				__m256 dT = _mm256_loadu_ps( _pfTexels );
				mAcc = _mm256_add_ps( _mm256_mul_ps( mW, dT ), mAcc );
				_pfWeights += sizeof( __m256 ) / sizeof( float );
				_pfTexels += sizeof( __m256 ) / sizeof( float );
				_sTotal -= sizeof( __m256 ) / sizeof( float );
			}
			fSum[0] += CUtilities::HorizontalSum( mAcc );
		}
#endif	// #ifdef __AVX__

#ifdef __SSE4_1__
		if ( CUtilities::IsSse4Supported() ) {
			__m128 mAcc = _mm_setzero_ps();
			while ( _sTotal >= sizeof( __m128 ) / sizeof( float ) ) {
				__m128 mW = _mm_load_ps( _pfWeights );
				__m128 dT = _mm_loadu_ps( _pfTexels );
				mAcc = _mm_add_ps( _mm_mul_ps( mW, dT ), mAcc );
				_pfWeights += sizeof( __m128 ) / sizeof( float );
				_pfTexels += sizeof( __m128 ) / sizeof( float );
				_sTotal -= sizeof( __m128 ) / sizeof( float );
			}
			fSum[0] += CUtilities::HorizontalSum( mAcc );
		}
#endif	// #ifdef __SSE4_1__

		while ( _sTotal >= 1 ) {
			fSum[0] += (*_pfWeights++) * (*_pfTexels++);
			--_sTotal;
		}
		return fSum[0];
	}

	/**
	 * Creates a new contribution list.
	 *
	 * \param _ui32SrcSize Size of the source.
	 * \param _ui32DstSize Size of the destination.
	 * \param _taAddressMode Texture addressing mode.
	 * \param _pfFilter Filter function.
	 * \param _fFilterSupport Filter support value.
	 * \param _fFilterScale Filter scale.
	 * \param _bStoreLocal If true, software resampling is assumed and m_fBuffer is allocated to fit the largest contribution list's samples.
	 * \return Returns true if all allocations succeeded.
	 */
	bool CResamplerBase::CreateContribList( uint32_t _ui32SrcSize, uint32_t _ui32DstSize,
		LSN_TEXTURE_ADDRESSING _taAddressMode,
		PfFilterFunc _pfFilter, float _fFilterSupport, float _fFilterScale, bool _bStoreLocal ) {

		std::vector<LSN_CONTRIB_BOUNDS> vBounds;
		try {
			m_cContribs.clear();
			m_cContribs.resize( _ui32DstSize );
			vBounds.resize( _ui32DstSize );
		}
		catch ( ... ) { return false; }


		// For fast division.
		const float fInvFilterScale = 1.0f / _fFilterScale;
		constexpr float fNudge = 0.5f;

		float fScale = static_cast<float>(_ui32DstSize) / static_cast<float>(_ui32SrcSize);
		float fOrigScale = fScale;

		fScale = std::min( fScale, 1.0f );

		float fHalfWidth = (_fFilterSupport / fScale) * fInvFilterScale;


		for ( uint32_t I = 0; I < _ui32DstSize; ++I ) {
			// Convert from discrete to continuous coordinates, then back.
			vBounds[I].fCenter = (static_cast<float>(I) + fNudge) / fOrigScale;
			vBounds[I].fCenter -= fNudge;
			vBounds[I].i32Left = static_cast<int32_t>(std::floor( vBounds[I].fCenter - fHalfWidth ));
			vBounds[I].i32Right = static_cast<int32_t>(std::ceil( vBounds[I].fCenter + fHalfWidth ));
		}

		size_t sMaxSize = 0;
		for ( uint32_t I = 0; I < _ui32DstSize; ++I ) {
			int32_t i32MaxK = -1;
			float fMaxW = -1.0e20f;
			//float fCenter = vBounds[I].fCenter;
			int32_t i32Left = vBounds[I].i32Left;
			int32_t i32Right = vBounds[I].i32Right;

			try {
				m_cContribs[I].fContributions.resize( i32Right - i32Left + 1 );
				m_cContribs[I].i32Indices.resize( i32Right - i32Left + 1 );
				sMaxSize = std::max( sMaxSize, static_cast<size_t>(i32Right - i32Left + 1) );
			}
			catch ( ... ) { return false; }

			float fTotalWeight = 0.0f;
			for ( int32_t J = i32Left; J <= i32Right; ++J ) {
				if ( CTextureAddressing::m_pfFuncs[_taAddressMode]( _ui32SrcSize, J ) != -2 ) {
					fTotalWeight += (*_pfFilter)( (vBounds[I].fCenter - J) * fScale * _fFilterScale );
				}
			}

			const float fNorm = 1.0f / fTotalWeight;
			fTotalWeight = 0.0f;

			for ( int32_t J = i32Left; J <= i32Right; ++J ) {
				float fThisWeight = (*_pfFilter)( (vBounds[I].fCenter - J) * fScale * _fFilterScale ) * fNorm;

				m_cContribs[I].i32Indices[J-i32Left] = CTextureAddressing::m_pfFuncs[_taAddressMode]( _ui32SrcSize, J );
				if ( m_cContribs[I].i32Indices[J-i32Left] == -2 ) {
					fThisWeight = 0.0f;
				}

				if ( fThisWeight == 0.0f ) { continue; }

				m_cContribs[I].fContributions[J-i32Left] = fThisWeight;

				fTotalWeight += fThisWeight;

				if ( fThisWeight > fMaxW ) {
					fMaxW = fThisWeight;
					i32MaxK = J - i32Left;
				}
			}
			
			if ( i32MaxK == -1 ) { return false; }

			if ( fTotalWeight != 1.0f && i32MaxK >= 0 ) {
				m_cContribs[I].fContributions[i32MaxK] += 1.0f - fTotalWeight;
			}

			// Trim 0 values.
			for ( size_t J = m_cContribs[I].fContributions.size(); J--; ) {
				if ( 0.0f == m_cContribs[I].fContributions[J] ) {
					m_cContribs[I].fContributions.erase( m_cContribs[I].fContributions.begin() + J );
					m_cContribs[I].i32Indices.erase( m_cContribs[I].i32Indices.begin() + J );
				}
			}

			// We can take a shortcut to gathering samples if all of the samples are in-range and sequential.
			m_cContribs[I].bInsideBounds = vBounds[I].i32Left >= 0 && (vBounds[I].i32Right) < static_cast<int32_t>(_ui32SrcSize);
			for ( int32_t J = 1; J < m_cContribs[I].fContributions.size(); ++J ) {
				if ( m_cContribs[I].i32Indices[J] != (m_cContribs[I].i32Indices[J-1] + 1) ) { 
					m_cContribs[I].bInsideBounds = false; 
					break; 
				}
			}
		}

		if ( sMaxSize && _bStoreLocal ) {
			try {
				m_fBuffer.resize( sMaxSize );
			}
			catch ( ... ) { return false; }
		}

		return true;
	}

}	// namespace lsn

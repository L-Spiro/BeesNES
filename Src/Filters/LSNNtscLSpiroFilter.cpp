/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: My own implementation of an NTSC filter.
 */

#include "LSNNtscLSpiroFilter.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <numbers>


namespace lsn {

	// == Members.
	CNtscLSpiroFilter::CNtscLSpiroFilter() {
		m_fHueSetting = 8.0f * std::numbers::pi / 180.0f;					/**< The hue. */
		m_fGammaSetting = 1.0f / 0.45f;										/**< The CRT gamma curve. */
		m_fBrightnessSetting = 1.0f - 0.070710678118654752440084436210485f;	/**< The brightness setting. */
		m_fSaturationSetting = -0.40f + 1.0f;								/**< The saturation setting. */

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
	CNtscLSpiroFilter::~CNtscLSpiroFilter() {
		StopThread();
	}

	// == Functions.
	/**
	 * Sets the basic parameters for the filter.
	 *
	 * \param _stBuffers The number of render targets to create.
	 * \param _ui16Width The console screen width.  Typically 256.
	 * \param _ui16Height The console screen height.  Typically 240.
	 * \return Returns the input format requested of the PPU.
	 */
	CDisplayClient::LSN_PPU_OUT_FORMAT CNtscLSpiroFilter::Init( size_t _stBuffers, uint16_t _ui16Width, uint16_t _ui16Height ) {
		m_vBasicRenderTarget.resize( _stBuffers );

		m_ui32OutputWidth = m_ui16ScaledWidth;
		m_ui32OutputHeight = _ui16Height;
		m_stStride = size_t( _ui16Width * sizeof( uint16_t ) );

		for ( auto I = m_vBasicRenderTarget.size(); I--; ) {
			m_vBasicRenderTarget[I].resize( m_stStride * _ui16Height );
			for ( auto J = m_vBasicRenderTarget[I].size() / sizeof( uint16_t ); J--; ) {
				(*reinterpret_cast<uint16_t *>(&m_vBasicRenderTarget[I][J*sizeof( uint16_t )])) = 0x0F;
			}
		}
		m_ui32FinalStride = RowStride( m_ui32OutputWidth, OutputBits() );


		StopThread();
		m_tdThreadData.ui16LinesDone = 0;
		m_tdThreadData.ui16EndLine = 0;
		m_tdThreadData.bEndThread = false;
		m_tdThreadData.pnlsfThis = this;
		m_ptThread = std::make_unique<std::thread>( WorkThread, &m_tdThreadData );
		return InputFormat();
	}

	/**
	 * Tells the filter that rendering to the source buffer has completed and that it should filter the results.  The final buffer, along with
	 *	its width, height, bit-depth, and stride, are returned.
	 *
	 * \param _pui8Input The buffer to be filtered, which will be a pointer to one of the buffers returned by OutputBuffer() previously.  Its format will be that returned in InputFormat().
	 * \param _ui32Width On input, this is the width of the buffer in pixels.  On return, it is filled with the final width, in pixels, of the result.
	 * \param _ui32Height On input, this is the height of the buffer in pixels.  On return, it is filled with the final height, in pixels, of the result.
	 * \param _ui16BitDepth On input, this is the bit depth of the buffer.  On return, it is filled with the final bit depth of the result.
	 * \param _ui32Stride On input, this is the stride of the buffer.  On return, it is filled with the final stride, in bytes, of the result.
	 * \param _ui64PpuFrame The PPU frame associated with the input data.
	 * \param _ui64RenderStartCycle The cycle at which rendering of the first pixel began.
	 * \return Returns a pointer to the filtered output buffer.
	 */
	uint8_t * CNtscLSpiroFilter::ApplyFilter( uint8_t * _pui8Input, uint32_t &_ui32Width, uint32_t &_ui32Height, uint16_t &_ui16BitDepth, uint32_t &_ui32Stride, uint64_t /*_ui64PpuFrame*/, uint64_t _ui64RenderStartCycle ) {
		FilterFrame( _pui8Input, _ui64RenderStartCycle + 2 );

		_ui16BitDepth = uint16_t( OutputBits() );
		_ui32Width = m_ui16ScaledWidth;
		_ui32Height = m_ui16Height;
		_ui32Stride = m_ui32FinalStride;
		return m_vRgbBuffer.data();
	}

	/**
	 * Sets the filter kernel size.
	 * 
	 * \param _ui32Size The new size of the filter.
	 * \return Returns true if the memory for the internal buffer(s) was allocated.
	 **/
	bool CNtscLSpiroFilter::SetKernelSize( uint32_t _ui32Size ) {
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
	bool CNtscLSpiroFilter::SetWidth( uint16_t _ui16Width ) {
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
	bool CNtscLSpiroFilter::SetWidthScale( uint16_t _ui16WidthScale ) {
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
	bool CNtscLSpiroFilter::SetHeight( uint16_t _ui16Height ) {
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
	void CNtscLSpiroFilter::SetGamma( float _fGamma ) {
		m_fGammaSetting = _fGamma;
		for (size_t I = 0; I < 300; ++I ) {
			double dVal = CUtilities::LinearTosRGB_Precise( CUtilities::CrtProperToLinear( (I / 299.0) ) ) * 255.0;
			//double dVal = CUtilities::LinearTosRGB_Precise( CUtilities::CrtProper2ToLinear( I / 299.0 ) ) * 255;
			//double dVal = CUtilities::LinearTosRGB_Precise( std::pow( (I / 299.0), m_fGammaSetting ) ) * 255.0;
			
			//double dVal = CUtilities::LinearTosRGB_Precise( CUtilities::SMPTE170MtoLinear_Precise( (I / 299.0) ) ) * 255.0;
			//double dVal = (I / 299.0) * 255.0;

			dVal = std::min( dVal, 255.0 );
			dVal = std::max( dVal, 0.0 );
			m_ui8Gamma[I] = uint8_t( std::round( dVal ) );
			m_ui32Gamma[I] = m_ui8Gamma[I];

			dVal = CUtilities::LinearTosRGB_Precise( CUtilities::CrtProperToLinear( (I / 299.0), 1.0, 0.0181 * 0.5 ) ) * 255.0;
			//dVal = CUtilities::LinearTosRGB_Precise( CUtilities::CrtProper2ToLinear( I / 299.0 ) ) * 255;
			//dVal = CUtilities::LinearTosRGB_Precise( std::pow( (I / 299.0), m_fGammaSetting ) ) * 255.0;
			
			//dVal = CUtilities::LinearTosRGB_Precise( CUtilities::SMPTE170MtoLinear_Precise( (I / 299.0) ) ) * 255.0;
			//dVal = (I / 299.0) * 255.0;

			dVal = std::min( dVal, 255.0 );
			dVal = std::max( dVal, 0.0 );

			m_ui8GammaG[I] = uint8_t( std::round( dVal ) );
			m_ui32GammaG[I] = m_ui8GammaG[I];
		}
	}

	/**
	 * Sets the hue.
	 * 
	 * \param _fHue The hue to set.
	 **/
	void CNtscLSpiroFilter::SetHue( float _fHue ) {
		m_fHueSetting = _fHue;
		GenPhaseTables( _fHue );
	}

	/**
	 * Sets the brightness.
	 * 
	 * \param _fBrightness The brightness to set.
	 **/
	void CNtscLSpiroFilter::SetBrightness( float _fBrightness ) {
		m_fBrightnessSetting = _fBrightness;
		GenPhaseTables( m_fHueSetting );
	}

	/**
	 * Sets the saturation.
	 * 
	 * \param _fSat The saturation to set.
	 **/
	void CNtscLSpiroFilter::SetSaturation( float _fSat ) {
		m_fSaturationSetting = _fSat;
		GenPhaseTables( m_fHueSetting );
	}

	/**
	 * Sets the black level.
	 * 
	 * \param _fBlack The black level to set.
	 **/
	void CNtscLSpiroFilter::SetBlackLevel( float _fBlack ) {
		m_fBlackSetting = _fBlack;
		GenNormalizedSignals();
	}
	
	/**
	 * Sets the white level.
	 * 
	 * \param _fWhite The white level to set.
	 **/
	void CNtscLSpiroFilter::SetWhiteLevel( float _fWhite ) {
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
	void CNtscLSpiroFilter::ScanlineToYiq( float * _pfDstY, float * _pfDstI, float * _pfDstQ, const uint16_t * _pui16Pixels, uint16_t _ui16Cycle, size_t _sRowIdx ) {
		float * pfSignalStart = m_vSignalStart[_sRowIdx];
		float * pfSignals = pfSignalStart;
		for ( uint16_t I = 0; I < LSN_PM_NTSC_RENDER_WIDTH; ++I ) {
			PixelToNtscSignals( pfSignals, (*_pui16Pixels++), uint16_t( _ui16Cycle + I * 8 ) );
			pfSignals += 8;
		}

		// Add noise.
		pfSignals = pfSignalStart;
		float * pfSignalEnd = pfSignals + LSN_PM_NTSC_RENDER_WIDTH * 8;
#ifdef __AVX512F__
		if ( CUtilities::IsAvx512FSupported() ) {
			while ( pfSignals < pfSignalEnd ) {
				__m512 vNoise = _mm512_load_ps( CUtilities::m_fNoiseBuffers[CUtilities::Rand()%LSN_NOISE_BUFFERS] );
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
				__m256 vNoise = _mm256_load_ps( CUtilities::m_fNoiseBuffers[CUtilities::Rand()%LSN_NOISE_BUFFERS] );
				__m256 vSig = _mm256_loadu_ps( pfSignals );
				vSig = _mm256_add_ps( vSig, vNoise );
				_mm256_storeu_ps( pfSignals, vSig );
				pfSignals += 8;
			}
		}
#endif	// #ifdef __AVX__


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
			(*_pfDstY++) *= m_fBrightnessSetting;
			++_pfDstI;
			++_pfDstQ;
		}
	}

	/**
	 * Renders a full frame of PPU 9-bit (stored in uint16_t's) palette indices to a given 32-bit RGBX buffer.
	 * 
	 * \param _pui8Pixels The input array of 9-bit PPU outputs.
	 * \param _ui64RenderStartCycle The PPU cycle at the start of the block being rendered.
	 **/
	void CNtscLSpiroFilter::FilterFrame( const uint8_t * _pui8Pixels, uint64_t _ui64RenderStartCycle ) {
		
		m_tdThreadData.ui16LinesDone = m_ui16Height / 2;
		m_tdThreadData.ui16EndLine = m_ui16Height;
		m_tdThreadData.ui64RenderStartCycle = _ui64RenderStartCycle;
		m_tdThreadData.pui8Pixels = _pui8Pixels;
		m_eGo.Signal();
		RenderScanlineRange( _pui8Pixels, 0, m_ui16Height / 2, _ui64RenderStartCycle );

		//m_eDone.WaitForSignal();
	}

	/**
	 * Renders a range of scanlines.
	 * 
	 * \param _pui8Pixels The input array of 9-bit PPU outputs.
	 * \param _ui16Start Index of the first scanline to render.
	 * \param _ui16End INdex of the end scanline.
	 * \param _ui64RenderStartCycle The PPU cycle at the start of the frame being rendered.
	 **/
	void CNtscLSpiroFilter::RenderScanlineRange( const uint8_t * _pui8Pixels, uint16_t _ui16Start, uint16_t _ui16End, uint64_t _ui64RenderStartCycle ) {
		float * pfY = reinterpret_cast<float *>(m_vY.data());
		float * pfI = reinterpret_cast<float *>(m_vI.data());
		float * pfQ = reinterpret_cast<float *>(m_vQ.data());
		size_t sYiqStride = m_ui16ScaledWidth * (sizeof( simd_4 ) / sizeof( float ));
		pfY += sYiqStride * _ui16Start;
		pfI += sYiqStride * _ui16Start;
		pfQ += sYiqStride * _ui16Start;
		for ( uint16_t H = _ui16Start; H < _ui16End; ++H ) {
			const uint16_t * pui6PixelRow = reinterpret_cast<const uint16_t *>(_pui8Pixels + (LSN_PM_NTSC_RENDER_WIDTH * 2) * H);
			ScanlineToYiq( pfY, pfI, pfQ, pui6PixelRow, uint16_t( ((_ui64RenderStartCycle + LSN_PM_NTSC_DOTS_X * H) * 8) % 12 ), H );
			ConvertYiqToBgra( H );
			pfY += sYiqStride;
			pfI += sYiqStride;
			pfQ += sYiqStride;
		}
	}

	/**
	 * Generates the phase sin/cos tables.
	 * 
	 * \param _fHue The hue offset.
	 **/
	void CNtscLSpiroFilter::GenPhaseTables( float _fHue ) {
		for ( size_t I = 0; I < 12; ++I ) {
			double dSin, dCos;
			// 0.5 = 90 degrees.
			::sincos( std::numbers::pi * ((I - 0.5) / 6.0 + 0.5) + _fHue, &dSin, &dCos );
			dCos *= m_fBrightnessSetting * m_fSaturationSetting * 2.0;
			dSin *= m_fBrightnessSetting * m_fSaturationSetting * 2.0;

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
	void CNtscLSpiroFilter::GenNormalizedSignals() {
		for ( size_t I = 0; I < 16; ++I ) {
			m_NormalizedLevels[I] = (CUtilities::m_fNtscLevels[I] - m_fBlackSetting) / (m_fWhiteSetting - m_fBlackSetting);
		}
	}

	/**
	 * Generates the filter kernel.
	 * 
	 * \param _ui32Width The width of the kernel.
	 **/
	void CNtscLSpiroFilter::GenFilterKernel( uint32_t _ui32Width ) {
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
	bool CNtscLSpiroFilter::AllocYiqBuffers( uint16_t _ui16W, uint16_t _ui16H, uint16_t _ui16Scale ) {
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

			m_vRgbBuffer.resize( sSize * 4 );
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Converts a single scanline of YIQ values in m_vY/m_vI/m_vQ to BGRA values in the same scanline of m_vRgbBuffer.
	 * 
	 * \param _sScanline The scanline to convert
	 **/
	void CNtscLSpiroFilter::ConvertYiqToBgra( size_t _sScanline ) {
		size_t sYiqStride = m_ui16ScaledWidth * 4 * _sScanline;
		float * pfY = reinterpret_cast<float *>(m_vY.data());
		float * pfI = reinterpret_cast<float *>(m_vI.data());
		float * pfQ = reinterpret_cast<float *>(m_vQ.data());
		pfY += sYiqStride;
		pfI += sYiqStride;
		pfQ += sYiqStride;

		uint8_t * pui8Bgra = m_vRgbBuffer.data() + m_ui16ScaledWidth * 4 * _sScanline;
#ifdef __AVX512F__
		if ( CUtilities::IsAvx512BWSupported() ) {
			__m512 m0 = _mm512_set1_ps( 0.0f );
			__m512 m299 = _mm512_set1_ps( 299.0f );
			for ( uint16_t I = 0; I < m_ui16ScaledWidth; I += sizeof( __m512 ) / sizeof( float ) ) {
				// YIQ-to-YUV is just a matter of hue rotation, so it is handled in GenPhaseTables().
				__m512 mY = _mm512_load_ps( pfY );
				__m512 mU = _mm512_load_ps( pfQ );
				__m512 mV = _mm512_load_ps( pfI );

				// Convert YUV to RGB.
				// R = Y + (1.139883025203f * V)
				// G = Y + (-0.394642233974f * U) + (-0.580621847591f * V)
				// B = Y + (2.032061872219f * U)
				__m512 mR = _mm512_add_ps( mY, _mm512_mul_ps( mV, _mm512_set1_ps( 1.139883025203f ) ) );
				__m512 mG = _mm512_add_ps( mY, _mm512_add_ps( _mm512_mul_ps( mU, _mm512_set1_ps( -0.394642233974f ) ), _mm512_mul_ps( mV, _mm512_set1_ps( -0.580621847591f ) ) ) );
				__m512 mB = _mm512_add_ps( mY, _mm512_mul_ps( mU, _mm512_set1_ps( 2.032061872219f ) ) );

				// Scale and clamp. clamp( RGB * 299.0, 0, 299 ).
				mR = _mm512_min_ps( _mm512_max_ps( _mm512_mul_ps( mR, m299 ), m0 ), m299 );
				mG = _mm512_min_ps( _mm512_max_ps( _mm512_mul_ps( mG, m299 ), m0 ), m299 );
				mB = _mm512_min_ps( _mm512_max_ps( _mm512_mul_ps( mB, m299 ), m0 ), m299 );

				// Convert to integers.
				__m512i mRi = _mm512_cvtps_epi32( mR );
				__m512i mGi = _mm512_cvtps_epi32( mG );
				__m512i mBi = _mm512_cvtps_epi32( mB );

#if 1
				// Gamma.
				mRi = _mm512_i32gather_epi32( mRi, m_ui32Gamma, 4 );
				mGi = _mm512_i32gather_epi32( mGi, m_ui32GammaG, 4 );
				mBi = _mm512_i32gather_epi32( mBi, m_ui32Gamma, 4 );

				// Combine sRGB channels into packed format.
				__m512i mSrgb = _mm512_or_si512(
					_mm512_or_si512( mBi, _mm512_slli_epi32( mGi, 8 ) ),
					_mm512_slli_epi32( mRi, 16 )
				);

				_mm512_storeu_si512( pui8Bgra, mSrgb );
				pui8Bgra += 512 / 8;

#else

				// Pack 32-bit integers to 16-bit.  BGRA order for Windows.
				__m512i mBgi = _mm512_packus_epi32( mBi, mGi );
				__m512i mRai = _mm512_packus_epi32( mRi, mGi );

				LSN_ALIGN( 64 )
				uint16_t ui16Tmp0[32];
				LSN_ALIGN( 64 )
				uint16_t ui16Tmp1[32];
				_mm512_store_si512( reinterpret_cast<__m512i *>(ui16Tmp0), mBgi );
				_mm512_store_si512( reinterpret_cast<__m512i *>(ui16Tmp1), mRai );
				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp0[0]];		// B0;
				(*pui8Bgra++) = m_ui8GammaG[ui16Tmp0[0+4]];		// G0;
				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp1[0]];		// R0;
				(*pui8Bgra++) = 255;							// A0;

				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp0[1]];		// B1;
				(*pui8Bgra++) = m_ui8GammaG[ui16Tmp0[1+4]];		// G1;
				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp1[1]];		// R1;
				(*pui8Bgra++) = 255;							// A1;

				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp0[2]];		// B2;
				(*pui8Bgra++) = m_ui8GammaG[ui16Tmp0[2+4]];		// G2;
				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp1[2]];		// R2;
				(*pui8Bgra++) = 255;							// A2;

				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp0[3]];		// B3;
				(*pui8Bgra++) = m_ui8GammaG[ui16Tmp0[3+4]];		// G3;
				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp1[3]];		// R3;
				(*pui8Bgra++) = 255;							// A3;

				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp0[0+4+4]];	// B4;
				(*pui8Bgra++) = m_ui8GammaG[ui16Tmp0[0+4+4+4]];	// G4;
				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp1[0+4+4]];	// R4;
				(*pui8Bgra++) = 255;							// A4;

				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp0[1+4+4]];	// B5;
				(*pui8Bgra++) = m_ui8GammaG[ui16Tmp0[1+4+4+4]];	// G5;
				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp1[1+4+4]];	// R5;
				(*pui8Bgra++) = 255;							// A5;

				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp0[2+4+4]];	// B6;
				(*pui8Bgra++) = m_ui8GammaG[ui16Tmp0[2+4+4+4]];	// G6;
				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp1[2+4+4]];	// R6;
				(*pui8Bgra++) = 255;							// A6;

				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp0[3+4+4]];	// B7;
				(*pui8Bgra++) = m_ui8GammaG[ui16Tmp0[3+4+4+4]];	// G7;
				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp1[3+4+4]];	// R7;
				(*pui8Bgra++) = 255;							// A7;


				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp0[0+4+4+4+4]];	// B8;
				(*pui8Bgra++) = m_ui8GammaG[ui16Tmp0[0+4+4+4+4+4]];	// G8;
				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp1[0+4+4+4+4]];	// R8;
				(*pui8Bgra++) = 255;								// A8;

				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp0[1+4+4+4+4]];	// B9;
				(*pui8Bgra++) = m_ui8GammaG[ui16Tmp0[1+4+4+4+4+4]];	// G9;
				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp1[1+4+4+4+4]];	// R9;
				(*pui8Bgra++) = 255;								// A9;

				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp0[2+4+4+4+4]];	// B10;
				(*pui8Bgra++) = m_ui8GammaG[ui16Tmp0[2+4+4+4+4+4]];	// G10;
				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp1[2+4+4+4+4]];	// R10;
				(*pui8Bgra++) = 255;								// A10;

				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp0[3+4+4+4+4]];	// B11;
				(*pui8Bgra++) = m_ui8GammaG[ui16Tmp0[3+4+4+4+4+4]];	// G11;
				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp1[3+4+4+4+4]];	// R11;
				(*pui8Bgra++) = 255;								// A11;

				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp0[0+4+4+4+4+4+4]];	// B12;
				(*pui8Bgra++) = m_ui8GammaG[ui16Tmp0[0+4+4+4+4+4+4+4]];	// G12;
				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp1[0+4+4+4+4+4+4]];	// R12;
				(*pui8Bgra++) = 255;									// A12;

				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp0[1+4+4+4+4+4+4]];	// B13;
				(*pui8Bgra++) = m_ui8GammaG[ui16Tmp0[1+4+4+4+4+4+4+4]];	// G13;
				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp1[1+4+4+4+4+4+4]];	// R13;
				(*pui8Bgra++) = 255;									// A13;

				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp0[2+4+4+4+4+4+4]];	// B14;
				(*pui8Bgra++) = m_ui8GammaG[ui16Tmp0[2+4+4+4+4+4+4+4]];	// G14;
				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp1[2+4+4+4+4+4+4]];	// R14;
				(*pui8Bgra++) = 255;									// A14;

				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp0[3+4+4+4+4+4+4]];	// B15;
				(*pui8Bgra++) = m_ui8GammaG[ui16Tmp0[3+4+4+4+4+4+4+4]];	// G15;
				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp1[3+4+4+4+4+4+4]];	// R15;
				(*pui8Bgra++) = 255;									// A15;
#endif	// #if 0

				pfY += sizeof( __m512 ) / sizeof( float );
				pfI += sizeof( __m512 ) / sizeof( float );
				pfQ += sizeof( __m512 ) / sizeof( float );

			}
		}
		else
#endif	// #ifdef __AVX512F__
#ifdef __AVX2__
		if ( CUtilities::IsAvx2Supported() ) {
			__m256 m0 = _mm256_set1_ps( 0.0f );
			__m256 m299 = _mm256_set1_ps( 299.0f );
			for ( uint16_t I = 0; I < m_ui16ScaledWidth; I += sizeof( __m256 ) / sizeof( float ) ) {
				// YIQ-to-YUV is just a matter of hue rotation, so it is handled in GenPhaseTables().
				__m256 mY = _mm256_load_ps( pfY );
				__m256 mU = _mm256_load_ps( pfQ );
				__m256 mV = _mm256_load_ps( pfI );

				// Convert YUV to RGB.
				// R = Y + (1.139883025203f * V)
				// G = Y + (-0.394642233974f * U) + (-0.580621847591f * V)
				// B = Y + (2.032061872219f * U)
				__m256 mR = _mm256_add_ps( mY, _mm256_mul_ps( mV, _mm256_set1_ps( 1.139883025203f ) ) );
				__m256 mG = _mm256_add_ps( mY, _mm256_add_ps( _mm256_mul_ps( mU, _mm256_set1_ps( -0.394642233974f ) ), _mm256_mul_ps( mV, _mm256_set1_ps( -0.580621847591f ) ) ) );
				__m256 mB = _mm256_add_ps( mY, _mm256_mul_ps( mU, _mm256_set1_ps( 2.032061872219f ) ) );

				// Scale and clamp. clamp( RGB * 299.0, 0, 299 ).
				mR = _mm256_min_ps( _mm256_max_ps( _mm256_mul_ps( mR, m299 ), m0 ), m299 );
				mG = _mm256_min_ps( _mm256_max_ps( _mm256_mul_ps( mG, m299 ), m0 ), m299 );
				mB = _mm256_min_ps( _mm256_max_ps( _mm256_mul_ps( mB, m299 ), m0 ), m299 );

				// Convert to integers.
				__m256i mRi = _mm256_cvtps_epi32( mR );
				__m256i mGi = _mm256_cvtps_epi32( mG );
				__m256i mBi = _mm256_cvtps_epi32( mB );

#if 1
				// Gamma.
				mRi = _mm256_i32gather_epi32( reinterpret_cast<const int *>(m_ui32Gamma), mRi, 4 );
				mGi = _mm256_i32gather_epi32( reinterpret_cast<const int *>(m_ui32GammaG), mGi, 4 );
				mBi = _mm256_i32gather_epi32( reinterpret_cast<const int *>(m_ui32Gamma), mBi, 4 );

				// Combine sRGB channels into packed format.
				__m256i mSrgb = _mm256_or_si256(
					_mm256_or_si256( mBi, _mm256_slli_epi32( mGi, 8 ) ),
					_mm256_slli_epi32( mRi, 16 )
				);

				_mm256_storeu_si256( reinterpret_cast<__m256i *>(pui8Bgra), mSrgb );
				pui8Bgra += 256 / 8;

#else
				// Pack 32-bit integers to 16-bit.  BGRA order for Windows.
				__m256i mBgi = _mm256_packus_epi32( mBi, mGi );
				__m256i mRai = _mm256_packus_epi32( mRi, mGi );

				LSN_ALIGN( 32 )
				uint16_t ui16Tmp0[16];
				LSN_ALIGN( 32 )
				uint16_t ui16Tmp1[16];
				_mm256_store_si256( reinterpret_cast<__m256i *>(ui16Tmp0), mBgi );
				_mm256_store_si256( reinterpret_cast<__m256i *>(ui16Tmp1), mRai );
				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp0[0]];		// B0;
				(*pui8Bgra++) = m_ui8GammaG[ui16Tmp0[0+4]];		// G0;
				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp1[0]];		// R0;
				(*pui8Bgra++) = 255;							// A0;

				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp0[1]];		// B1;
				(*pui8Bgra++) = m_ui8GammaG[ui16Tmp0[1+4]];		// G1;
				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp1[1]];		// R1;
				(*pui8Bgra++) = 255;							// A1;

				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp0[2]];		// B2;
				(*pui8Bgra++) = m_ui8GammaG[ui16Tmp0[2+4]];		// G2;
				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp1[2]];		// R2;
				(*pui8Bgra++) = 255;							// A2;

				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp0[3]];		// B3;
				(*pui8Bgra++) = m_ui8GammaG[ui16Tmp0[3+4]];		// G3;
				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp1[3]];		// R3;
				(*pui8Bgra++) = 255;							// A3;

				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp0[0+4+4]];	// B4;
				(*pui8Bgra++) = m_ui8GammaG[ui16Tmp0[0+4+4+4]];	// G4;
				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp1[0+4+4]];	// R4;
				(*pui8Bgra++) = 255;							// A4;

				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp0[1+4+4]];	// B5;
				(*pui8Bgra++) = m_ui8GammaG[ui16Tmp0[1+4+4+4]];	// G5;
				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp1[1+4+4]];	// R5;
				(*pui8Bgra++) = 255;							// A5;

				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp0[2+4+4]];	// B6;
				(*pui8Bgra++) = m_ui8GammaG[ui16Tmp0[2+4+4+4]];	// G6;
				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp1[2+4+4]];	// R6;
				(*pui8Bgra++) = 255;							// A6;

				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp0[3+4+4]];	// B7;
				(*pui8Bgra++) = m_ui8GammaG[ui16Tmp0[3+4+4+4]];	// G7;
				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp1[3+4+4]];	// R7;
				(*pui8Bgra++) = 255;							// A7;
#endif	// #if 0

				pfY += sizeof( __m256 ) / sizeof( float );
				pfI += sizeof( __m256 ) / sizeof( float );
				pfQ += sizeof( __m256 ) / sizeof( float );

			}
		}
		else
#endif	// #ifdef __AVX__
#ifdef __SSE4_1__
		if ( CUtilities::IsSse4Supported() ) {
			__m128 m0 = _mm_set1_ps( 0.0f );
			__m128 m299 = _mm_set1_ps( 299.0f );
			for ( uint16_t I = 0; I < m_ui16ScaledWidth; I += sizeof( __m128 ) / sizeof( float ) ) {
				// YIQ-to-YUV is just a matter of hue rotation, so it is handled in GenPhaseTables().
				__m128 mY = _mm_load_ps( pfY );
				__m128 mU = _mm_load_ps( pfQ );
				__m128 mV = _mm_load_ps( pfI );

				// Convert YUV to RGB.
				// R = Y + (1.139883025203f * V)
				// G = Y + (-0.394642233974f * U) + (-0.580621847591f * V)
				// B = Y + (2.032061872219f * U)
				__m128 mR = _mm_add_ps( mY, _mm_mul_ps( mV, _mm_set1_ps( 1.139883025203f ) ) );
				__m128 mG = _mm_add_ps( mY, _mm_add_ps( _mm_mul_ps( mU, _mm_set1_ps( -0.394642233974f ) ), _mm_mul_ps( mV, _mm_set1_ps( -0.580621847591f ) ) ) );
				__m128 mB = _mm_add_ps( mY, _mm_mul_ps( mU, _mm_set1_ps( 2.032061872219f ) ) );

				// Scale and clamp. clamp( RGB * 299.0, 0, 299 ).
				mR = _mm_min_ps( _mm_max_ps( _mm_mul_ps( mR, m299 ), m0 ), m299 );
				mG = _mm_min_ps( _mm_max_ps( _mm_mul_ps( mG, m299 ), m0 ), m299 );
				mB = _mm_min_ps( _mm_max_ps( _mm_mul_ps( mB, m299 ), m0 ), m299 );

				// Convert to integers.
				__m128i mRi = _mm_cvtps_epi32( mR );
				__m128i mGi = _mm_cvtps_epi32( mG );
				__m128i mBi = _mm_cvtps_epi32( mB );

				// Pack 32-bit integers to 16-bit.  BGRA order for Windows.
				__m128i mBgi = _mm_packus_epi32( mBi, mGi );
				__m128i mRai = _mm_packus_epi32( mRi, mGi );

				LSN_ALIGN( 32 )
				uint16_t ui16Tmp0[8];
				LSN_ALIGN( 32 )
				uint16_t ui16Tmp1[8];
				_mm_store_si128( reinterpret_cast<__m128i *>(ui16Tmp0), mBgi );
				_mm_store_si128( reinterpret_cast<__m128i *>(ui16Tmp1), mRai );
				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp0[0]];		// B0;
				(*pui8Bgra++) = m_ui8GammaG[ui16Tmp0[0+4]];		// G0;
				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp1[0]];		// R0;
				(*pui8Bgra++) = 255;							// A0;

				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp0[1]];		// B1;
				(*pui8Bgra++) = m_ui8GammaG[ui16Tmp0[1+4]];		// G1;
				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp1[1]];		// R1;
				(*pui8Bgra++) = 255;							// A1;

				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp0[2]];		// B2;
				(*pui8Bgra++) = m_ui8GammaG[ui16Tmp0[2+4]];		// G2;
				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp1[2]];		// R2;
				(*pui8Bgra++) = 255;							// A2;

				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp0[3]];		// B3;
				(*pui8Bgra++) = m_ui8GammaG[ui16Tmp0[3+4]];		// G3;
				(*pui8Bgra++) = m_ui8Gamma[ui16Tmp1[3]];		// R3;
				(*pui8Bgra++) = 255;							// A3;

				pfY += sizeof( __m128 ) / sizeof( float );
				pfI += sizeof( __m128 ) / sizeof( float );
				pfQ += sizeof( __m128 ) / sizeof( float );
			}
		}
		else
#endif	// #ifdef __SSE4_1__
		{
			for ( uint16_t I = 0; I < m_ui16ScaledWidth; ++I ) {
				// Convert YUV to RGB.
				// R = Y + (1.139883025203f * V)
				// G = Y + (-0.394642233974f * U) + (-0.580621847591f * V)
				// B = Y + (2.032061872219f * U)
				float fR = (*pfY) + (1.139883025203f * (*pfI));
				float fG = (*pfY) + (-0.394642233974f * (*pfQ)) + (-0.580621847591f * (*pfI));
				float fB = (*pfY) + (2.032061872219f * (*pfQ));

				// Scale and clamp. clamp( RGB * 299.0, 0, 299 ).
				fR = std::clamp( fR * 299.0f, 0.0f, 299.0f );
				fG = std::clamp( fG * 299.0f, 0.0f, 299.0f );
				fB = std::clamp( fB * 299.0f, 0.0f, 299.0f );

				// Convert to integers.
				uint16_t ui16Ri = static_cast<uint16_t>(std::round( fR ));
				uint16_t ui16Gi = static_cast<uint16_t>(std::round( fG ));
				uint16_t ui16Bi = static_cast<uint16_t>(std::round( fB ));

				(*pui8Bgra++) = m_ui8Gamma[ui16Bi];			// B0;
				(*pui8Bgra++) = m_ui8GammaG[ui16Gi];		// G0;
				(*pui8Bgra++) = m_ui8Gamma[ui16Ri];			// R0;
				(*pui8Bgra++) = 255;						// A0;

				++pfY;
				++pfI;
				++pfQ;
			}
		}
	}

	/**
	 * Stops the worker thread.
	 **/
	void CNtscLSpiroFilter::StopThread() {
		m_tdThreadData.bEndThread = true;
		if ( m_ptThread.get() ) {
			m_eGo.Signal();
			m_eDone.WaitForSignal();
			m_ptThread->join();
			m_ptThread.reset();
		}
		m_tdThreadData.bEndThread = false;
	}

	/**
	 * The worker thread.
	 * 
	 * \param _ptdData Parameters passed to the thread.
	 **/
	void CNtscLSpiroFilter::WorkThread( LSN_THREAD_DATA * _ptdData ) {
		::SetThreadHighPriority();
		while ( !_ptdData->bEndThread ) {
			_ptdData->pnlsfThis->m_eDone.Signal();
			_ptdData->pnlsfThis->m_eGo.WaitForSignal();
			if ( _ptdData->bEndThread ) { break; }

			_ptdData->pnlsfThis->RenderScanlineRange( _ptdData->pui8Pixels, _ptdData->ui16LinesDone, _ptdData->ui16EndLine, _ptdData->ui64RenderStartCycle );
		}

		_ptdData->pnlsfThis->m_eDone.Signal();
	}

}	// namespace lsn

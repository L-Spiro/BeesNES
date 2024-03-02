/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: My own implementation of an NTSC filter.
 */

#include "LSNNtscLSpiroFilter.h"
#include "../Utilities/LSNUtilities.h"
#include <Helpers/LSWHelpers.h>

#include <cmath>
#include <emmintrin.h>
#include <numbers>


namespace lsn {

	// == Members.
	const float CNtscLSpiroFilter::m_fLevels[16] = {							/**< Output levels. */
		0.228f, 0.312f, 0.552f, 0.880f, // Signal low.
		0.616f, 0.840f, 1.100f, 1.100f, // Signal high.
		0.192f, 0.256f, 0.448f, 0.712f, // Signal low, attenuated.
		0.500f, 0.676f, 0.896f, 0.896f  // Signal high, attenuated.
	};

	uint8_t CNtscLSpiroFilter::m_ui8Gamma[256];						/**< The gamma curve. */

	CNtscLSpiroFilter::CNtscLSpiroFilter() {
		m_fHue = std::sin( 110.0 * std::numbers::pi / 180.0 );
		GenPhaseTables( m_fHue );
		GenSseNormalizers();
		SetKernelSize( m_ui32FilterKernelSize );
		SetWidth( 256 );
		SetHeight( 240 );

		m_mSin33 = _mm_set1_ps( float( std::sin( -33 * std::numbers::pi / 180.0 ) ) );
		m_mCos33 = _mm_set1_ps( float( std::cos( -33 * std::numbers::pi / 180.0 ) ) );
		m_mNegSin33 = _mm_set1_ps( float( -std::sin( -33 * std::numbers::pi / 180.0 ) ) );

		m_1_14 = _mm_set1_ps( 1.139883025203f );
		m_2_03 = _mm_set1_ps( 2.032061872219f );
		m_n0_394242 = _mm_set1_ps( -0.394642233974f );
		m_n0_580681 = _mm_set1_ps( -0.580621847591f );
		m_1 = _mm_set1_ps( 1.0f );
		m_0 = _mm_set1_ps( 0.0f );
		m_255 = _mm_set1_ps( 255.0f );
		m_255_5 = _mm_set1_ps( 255.5f );
		m_255i = _mm_set1_epi16( 255 );

		for (size_t I = 0; I < 256; ++I ) {
			m_ui8Gamma[I] = uint8_t( std::round( lsw::CHelpers::sRGBtoLinear( std::pow( (I / 255.0), 1.0 / 2.2 ) ) * 255.0 ) );
		}
	}
	CNtscLSpiroFilter::~CNtscLSpiroFilter() {
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

		m_ui32OutputWidth = _ui16Width;
		m_ui32OutputHeight = _ui16Height;
		m_stStride = size_t( _ui16Width * sizeof( uint16_t ) );

		for ( auto I = m_vBasicRenderTarget.size(); I--; ) {
			m_vBasicRenderTarget[I].resize( m_stStride * _ui16Height );
			for ( auto J = m_vBasicRenderTarget[I].size() / sizeof( uint16_t ); J--; ) {
				(*reinterpret_cast<uint16_t *>(&m_vBasicRenderTarget[I][J*sizeof( uint16_t )])) = 0x0F;
			}
		}
		m_ui32FinalStride = RowStride( _ui16Width, OutputBits() );
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
		if ( !FilterFrame( _pui8Input, uint16_t( _ui32Width ), uint16_t( _ui32Height ), _ui32Stride, _ui64RenderStartCycle ) ) { return nullptr; }

		_ui16BitDepth = uint16_t( OutputBits() );
		_ui32Stride = m_ui32FinalStride;
		return m_vRgbBuffer.data();
	}

	/**
	 * Sets the filter kernel size.
	 * 
	 * \param _ui32Size The new size of the filter.
	 **/
	void CNtscLSpiroFilter::SetKernelSize( uint32_t _ui32Size ) {
		m_ui32FilterKernelSize = _ui32Size;
		GenFilterKernel( m_ui32FilterKernelSize * 2 );
		SetWidth( m_ui16Width );
	}

	/**
	 * Sets the width of the input.
	 * 
	 * \param _ui16Width The width to set.
	 * \return Returns true if the memory for the internal buffer(s) was allocated.
	 **/
	bool CNtscLSpiroFilter::SetWidth( uint16_t _ui16Width ) {
		if ( m_ui16Width != _ui16Width ) {
			// Buffer size:
			// [m_ui32FilterKernelSize/2][m_ui16Width*8][m_ui32FilterKernelSize/2][Padding for Alignment to 32 Bytes]
			try {
				m_vSignalBuffer.resize( _ui16Width * 8 + m_ui32FilterKernelSize + 8 );
			}
			catch ( ... ) { return false; }
			uintptr_t uiptrStart = reinterpret_cast<uintptr_t>(m_vSignalBuffer.data() + m_ui32FilterKernelSize / 2 );
			uiptrStart = (uiptrStart + 31) / 32 * 32;
			m_pfSignalStart = reinterpret_cast<float *>(uiptrStart);
			if ( !AllocYiqBuffers( _ui16Width, m_ui16Height ) ) { return false; }
			m_ui16Width = _ui16Width;
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
			if ( !AllocYiqBuffers( m_ui16Width, _ui16Height ) ) { return false; }
			m_ui16Height = _ui16Height;
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
	 **/
	void CNtscLSpiroFilter::ScanlineToYiq( float * _pfDstY, float * _pfDstI, float * _pfDstQ, const uint16_t * _pui16Pixels, uint16_t _ui16Cycle ) {
		float * pfSignals = m_pfSignalStart;
		for ( uint16_t I = 0; I < m_ui16Width; ++I ) {
			PixelToNtscSignals( pfSignals, (*_pui16Pixels++), uint16_t( _ui16Cycle + I * 8 ) );
			pfSignals += 8;
		}

		for ( uint16_t I = 0; I < m_ui16Width; ++I ) {
			int16_t i16Center = (I * 8) + 4;
			int16_t i16Start = i16Center - int16_t( m_ui32FilterKernelSize );
			int16_t i16End = i16Center + int16_t( m_ui32FilterKernelSize );
			__m128 mYiq = _mm_setzero_ps();
			for ( int16_t J = i16Start; J < i16End; ++J ) {
				__m128 mLevel = _mm_set1_ps( m_pfSignalStart[J] * m_fFilter[J-i16Start] );
				// Negative indexing into m_pfSignalStart is allowed, since it points no fewer than (m_ui32FilterKernelSize/2) floats into the buffer.
				mYiq = _mm_add_ps(
					mYiq,
					_mm_mul_ps( mLevel, m_mCosSinTable[(_ui16Cycle+(12*4)+J)%12] ) );
				// (_ui16Cycle+J) can result in negative numbers, which you would fix by adding 12 or some other multiple of 12 as long as that value is large enough to always
				//	bring the final result back into the positive.
				// We just add that value pre-emptively before adding J so that the value can never go below 0 in the first place.  (12*4) is used because it should always be
				//	enough to offset J back into the positive regardless of the kernel size.
				// If LSN_MAX_FILTER_SIZE were to be increased significantly, this might need to be adjusted accordingly, but currently it allows LSN_MAX_FILTER_SIZE to go as
				//	high as 96, which is ridiculously high.
				// See GenFilterKernel() for the creation of m_fFilter.
				// See GenPhaseTables() for the creation of m_mCosSinTable.

				/**
				 * This is the same as:
				 * float fLevel = m_pfSignalStart[J] * m_fFilter[J+i16Start];
				 * fY += fLevel;
				 * fI += fLevel * m_fPhaseCosTable[(_ui16Cycle+(12*4)+J)%12];
				 * fQ += fLevel * m_fPhaseSinTable[(_ui16Cycle+(12*4)+J)%12];
				 */
			}
			__declspec(align(32))
			float fTmp[4];
			_mm_store_ps( fTmp, mYiq );
			(*_pfDstY++) = fTmp[0];
			(*_pfDstI++) = fTmp[1];
			(*_pfDstQ++) = fTmp[2];
		}
	}

	/**
	 * Renders a full frame of PPU 9-bit (stored in uint16_t's) palette indices to a given 32-bit RGBX buffer.
	 * 
	 * \param _pui8Pixels The input array of 9-bit PPU outputs.
	 * \param _ui16Width Width of the input in pixels.
	 * \param _ui16Height Height of the input in pixels.
	 * \param _ui32Stride Bytes between pixel rows.
	 * \param _ui64RenderStartCycle The PPU cycle at the start of the block being rendered.
	 * \return Returns true if all internal buffers could be allocated.
	 **/
	bool CNtscLSpiroFilter::FilterFrame( const uint8_t * _pui8Pixels, uint16_t _ui16Width, uint16_t _ui16Height, uint32_t &_ui32Stride, uint64_t _ui64RenderStartCycle ) {
		if ( !SetWidth( _ui16Width ) ) { return false; }
		if ( !SetHeight( _ui16Height ) ) { return false; }

		float * pfY = reinterpret_cast<float *>(m_vY.data());
		float * pfI = reinterpret_cast<float *>(m_vI.data());
		float * pfQ = reinterpret_cast<float *>(m_vQ.data());
		size_t sYiqStride = _ui16Width * (sizeof( __m128 ) / sizeof( float ));
		for ( uint16_t H = 0; H < _ui16Height; ++H ) {
			const uint16_t * pui6PixelRow = reinterpret_cast<const uint16_t *>(_pui8Pixels + _ui32Stride * H);
			ScanlineToYiq( pfY, pfI, pfQ, pui6PixelRow, uint16_t( ((_ui64RenderStartCycle + 341 * H) * 8) % 12 ) );
			ConvertYiqToBgra( H );
			pfY += sYiqStride;
			pfI += sYiqStride;
			pfQ += sYiqStride;
		}

		return true;
	}

	/**
	 * Generates the phase sin/cos tables.
	 * 
	 * \param _fHue The hue offset.
	 **/
	void CNtscLSpiroFilter::GenPhaseTables( float _fHue ) {
		for ( size_t I = 0; I < 12; ++I ) {
			double dSin, dCos;
			::sincos( std::numbers::pi * (I - 0.5) / 6.0 + _fHue, &dSin, &dCos );
			dCos *= 2.0;
			dSin *= 2.0;

			// Straight version.
			m_fPhaseCosTable[I] = float( dCos );
			m_fPhaseSinTable[I] = float( dSin );

			// SIMD version.
			float fSimd[4] = {
				1.0f,				// Y
				float( dCos ),		// cos( M_PI * (phase+p) / 6 )
				float( dSin ),		// sin( M_PI * (phase+p) / 6 )
				0.0f,
			};
			m_mCosSinTable[I] = _mm_loadu_ps( fSimd );

			/** Using the code on the Wiki as an illustration, the idea is:
			 *         for(int p = begin; p < end; ++p) // Collect and accumulate samples
			 *         {
			 *             float level = signal_levels[p] / 12.f;
			 *             y  =  y + level;
			 *             i  =  i + level * cos( M_PI * (phase+p) / 6 );
			 *             q  =  q + level * sin( M_PI * (phase+p) / 6 );
			 *         }
			 * 
			 * Load level into 1 vector, multiply across with m_mCosSinTable[phase+p%12], and sum across, accumulating into a [y, i, q, 0] vector in the end.
			 * IE:
			 * [y, i, q, 0] += [level, level, level, level] * [1, COS, SIN, 0]
			 **/
		}
	}

	/**
	 * Fills the __m128 registers with the black level and (white-black) level.
	 **/
	void CNtscLSpiroFilter::GenSseNormalizers() {
		m_mBlack = _mm_set1_ps( m_fBlack );
		m_mWhiteMinusBlack = _mm_set1_ps( m_fWhite - m_fBlack );
	}

	/**
	 * Generates the filter kernel.
	 * 
	 * \param _ui32Width The width of the kernel.
	 **/
	void CNtscLSpiroFilter::GenFilterKernel( uint32_t _ui32Width ) {
		double dSum = 0.0;
		for ( size_t I = 0; I < _ui32Width; ++I ) {
			m_fFilter[I] = CUtilities::BoxFilterFunc( I / (_ui32Width - 1.0f) * _ui32Width - (_ui32Width / 2), _ui32Width / 2 );
			dSum += m_fFilter[I];
		}
		double dNorm = 1.0 / dSum;
		for ( size_t I = 0; I < _ui32Width; ++I ) {
			m_fFilter[I] = float( m_fFilter[I] * dNorm );
		}
	}

	/**
	 * Allocates the YIQ buffers for a given width and height.
	 * 
	 * \param _ui16W The width of the buffers.
	 * \param _ui16H The height of the buffers.
	 * \return Returns true if the allocations succeeded.
	 **/
	bool CNtscLSpiroFilter::AllocYiqBuffers( uint16_t _ui16W, uint16_t _ui16H ) {
		try {
			size_t sSize = _ui16W * _ui16H;
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
		size_t sYiqStride = m_ui16Width * 4 * _sScanline;
		float * pfY = reinterpret_cast<float *>(m_vY.data());
		float * pfI = reinterpret_cast<float *>(m_vI.data());
		float * pfQ = reinterpret_cast<float *>(m_vQ.data());
		pfY += sYiqStride;
		pfI += sYiqStride;
		pfQ += sYiqStride;

		uint8_t * pui8Bgra = m_vRgbBuffer.data() + m_ui16Width * 4 * _sScanline;

		for ( uint16_t I = 0; I < m_ui16Width; I += 4 ) {
			__m128 mI = _mm_load_ps( pfI );
			__m128 mQ = _mm_load_ps( pfQ );

			// U = -(I * sin(-33 deg)) + (Q * cos(-33 deg))
			// V =  (I * cos(-33 deg)) + (Q * sin(-33 deg))
			// Convert I and Q to U.
			__m128 mU = _mm_add_ps( _mm_mul_ps( mI, m_mNegSin33 ), 
				_mm_mul_ps( mQ, m_mCos33 ) );

			__m128 mY = _mm_load_ps( pfY );

			// Convert I and Q to V.
			__m128 mV = _mm_add_ps( _mm_mul_ps( mI, m_mCos33 ), 
				_mm_mul_ps( mQ, m_mSin33 ) );



			// Convert YUV to RGB.
			// R = Y + (1.139883025203f * V)
			// G = Y + (-0.394642233974f * U) + (-0.580621847591f * V)
			// B = Y + (2.032061872219f * U)
			__m128 mR = _mm_add_ps( mY, _mm_mul_ps( mV, m_1_14 ) );
			__m128 mG = _mm_add_ps( mY, _mm_add_ps( _mm_mul_ps( mU, m_n0_394242 ), _mm_mul_ps( mV, m_n0_580681 ) ) );
			__m128 mB = _mm_add_ps( mY, _mm_mul_ps( mU, m_2_03 ) );

			// Scale and clamp. clamp( RGB * 255.5, 0, 255 ).
			mR = _mm_min_ps( _mm_max_ps( _mm_mul_ps( mR, m_255_5 ), m_0), m_255 );
			mG = _mm_min_ps( _mm_max_ps( _mm_mul_ps( mG, m_255_5 ), m_0), m_255 );
			mB = _mm_min_ps( _mm_max_ps( _mm_mul_ps( mB, m_255_5 ), m_0), m_255 );

			// Convert to integers.
			__m128i mRi = _mm_cvtps_epi32( mR );
			__m128i mGi = _mm_cvtps_epi32( mG );
			__m128i mBi = _mm_cvtps_epi32( mB );

			// Pack 32-bit integers to 16-bit.  BGRA order for Windows.
			__m128i mBgi = _mm_packus_epi32( mBi, mGi );
			__m128i mRai = _mm_packus_epi32( mRi, m_255i );

			// Pack 16-bit to 8-bit.
			__m128i mBgra = _mm_packus_epi16( mBgi, mRai );

			__declspec(align(32))
			uint8_t ui8Tmp[16];
			//_mm_storeu_si128( reinterpret_cast<__m128i *>(pui8Bgra), mBgra );
			_mm_store_si128( reinterpret_cast<__m128i *>(ui8Tmp), mBgra );
			(*pui8Bgra++) = m_ui8Gamma[ui8Tmp[0]];		// B0;
			(*pui8Bgra++) = m_ui8Gamma[ui8Tmp[0+4]];	// G0;
			(*pui8Bgra++) = m_ui8Gamma[ui8Tmp[0+4+4]];	// R0;
			(*pui8Bgra++) = 255;			// A0;

			(*pui8Bgra++) = m_ui8Gamma[ui8Tmp[1]];		// B1;
			(*pui8Bgra++) = m_ui8Gamma[ui8Tmp[1+4]];	// G1;
			(*pui8Bgra++) = m_ui8Gamma[ui8Tmp[1+4+4]];	// R1;
			(*pui8Bgra++) = 255;			// A1;

			(*pui8Bgra++) = m_ui8Gamma[ui8Tmp[2]];		// B2;
			(*pui8Bgra++) = m_ui8Gamma[ui8Tmp[2+4]];	// G2;
			(*pui8Bgra++) = m_ui8Gamma[ui8Tmp[2+4+4]];	// R2;
			(*pui8Bgra++) = 255;			// A2;

			(*pui8Bgra++) = m_ui8Gamma[ui8Tmp[3]];		// B3;
			(*pui8Bgra++) = m_ui8Gamma[ui8Tmp[3+4]];	// G3;
			(*pui8Bgra++) = m_ui8Gamma[ui8Tmp[3+4+4]];	// R3;
			(*pui8Bgra++) = 255;			// A3;

			pfY += sizeof( __m128 ) / sizeof( float );
			pfI += sizeof( __m128 ) / sizeof( float );
			pfQ += sizeof( __m128 ) / sizeof( float );

			//pui8Bgra += sizeof( __m128i ) / sizeof( uint8_t );
		}
	}
}	// namespace lsn

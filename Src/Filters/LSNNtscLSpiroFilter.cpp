/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: My own implementation of an NTSC filter.
 */

#include "LSNNtscLSpiroFilter.h"
#include "../Utilities/LSNUtilities.h"
#include <numbers>


namespace lsn {

	// == Members.
	const float CNtscLSpiroFilter::m_fLevels[16] = {							/**< Output levels. */
		0.228f, 0.312f, 0.552f, 0.880f, // Signal low.
		0.616f, 0.840f, 1.100f, 1.100f, // Signal high.
		0.192f, 0.256f, 0.448f, 0.712f, // Signal low, attenuated.
		0.500f, 0.676f, 0.896f, 0.896f  // Signal high, attenuated.
	};

	CNtscLSpiroFilter::CNtscLSpiroFilter() {
		GenPhaseTables( m_fHue );
		GenSseNormalizers();
		GenFilterKernel( m_ui32FilterKernelSize * 2 );
	}
	CNtscLSpiroFilter::~CNtscLSpiroFilter() {
	}

	// == Functions.
	/**
	 * Creates a single scanline from 9-bit PPU output to a float buffer of YIQ values.
	 * 
	 * \param _pfDstY The destination for where to begin storing the YIQ Y values.  Must be aligned to a 16-byte boundary.
	 * \param _pfDstI The destination for where to begin storing the YIQ I values.  Must be aligned to a 16-byte boundary.
	 * \param _pfDstQ The destination for where to begin storing the YIQ Q values.  Must be aligned to a 16-byte boundary.
	 * \param _pui16Pixels The start of the 9-bit PPU output for this scanline.
	 * \param _ui16Width Number of pixels in this scanline.
	 * \param _ui16Cycle The cycle count at the start of the scanline.
	 **/
	void CNtscLSpiroFilter::ScanlineToYiq( float * _pfDstY, float * _pfDstI, float * _pfDstQ, const uint16_t * _pui16Pixels, uint16_t _ui16Width, uint16_t _ui16Cycle ) {
		float * pfSignals = m_pfSignalStart;
		for ( uint16_t I = 0; I < _ui16Width; ++I ) {
			PixelToNtscSignals( pfSignals, (*_pui16Pixels++), uint16_t( _ui16Cycle + I * 8 ) );
			pfSignals += 8;
		}

		for ( uint16_t I = 0; I < _ui16Width; ++I ) {
			int16_t i16Center = (I * 8) + 4;
			int16_t i16Start = i16Center - int16_t( m_ui32FilterKernelSize );
			int16_t i16End = i16Center + int16_t( m_ui32FilterKernelSize );
			__m128 mYiq = _mm_setzero_ps();
			for ( int16_t J = i16Start; J < i16End; ++J ) {
				__m128 mLevel = _mm_set1_ps( m_pfSignalStart[J] * m_fFilter[J+i16Start] );
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
			m_fFilter[I] = CUtilities::LanczosXFilterFunc( I / (_ui32Width - 1.0f) * _ui32Width - (_ui32Width / 2), _ui32Width / 2 );
			dSum += m_fFilter[I];
		}
		double dNorm = 1.0 / dSum;
		for ( size_t I = 0; I < _ui32Width; ++I ) {
			m_fFilter[I] = float( m_fFilter[I] * dNorm );
		}
	}

}	// namespace lsn

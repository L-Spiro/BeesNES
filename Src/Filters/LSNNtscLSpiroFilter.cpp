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
		GenFilterKernel( 12 );
	}
	CNtscLSpiroFilter::~CNtscLSpiroFilter() {
	}

	// == Functions.
	/**
	 * Converts a 9-bit PPU output palette value to 8 signals.
	 * 
	 * \param _pfDst The destination for the 8 signals.
	 * \param _ui16Pixel The PPU output index to convert.
	 * \param _ui16Cycle The cycle count for the pixel (modulo 12).
	 **/
	void CNtscLSpiroFilter::PixelToNtscSignals( float * _pfDst, uint16_t _ui16Pixel, uint16_t _ui16Cycle ) {
		__m128 * pmDst = reinterpret_cast<__m128 *>(_pfDst);		// _pfDst will always be properly aligned.
		for ( size_t I = 0; I < 8; ++I ) {
			(*_pfDst++) = IndexToNtscSignal( _ui16Pixel, uint16_t( _ui16Cycle + I ) );
		}
		__m128 mNumerator = _mm_sub_ps( (*pmDst), m_mBlack );		// signal - black.
		(*pmDst++) = _mm_div_ps( mNumerator, m_mWhiteMinusBlack );	// (signal - black) / (white - black).

		mNumerator = _mm_sub_ps( (*pmDst), m_mBlack );				// signal - black.
		(*pmDst) = _mm_div_ps( mNumerator, m_mWhiteMinusBlack );	// (signal - black) / (white - black).
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
			m_fPhaseCosTable[I] = float( dCos * 2.0 );
			m_fPhaseSinTable[I] = float( dSin * 2.0 );
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

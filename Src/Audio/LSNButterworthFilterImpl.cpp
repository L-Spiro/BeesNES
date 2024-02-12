/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Butterworth filter.
 */

/*
 
 This file is part of Butterworth Filter Design, a pair C++ classes and an
 accompanying suite of unit tests for designing high order Butterworth IIR &
 EQ filters using the bilinear transform.
 The generated filter coefficients are split out into cascaded biquad sections,
 for easy use in your garden variety biquad or second-order section (SOS).
 
 Reference: http://en.wikipedia.org/wiki/Butterworth_filter
 http://www.musicdsp.org/files/Audio-EQ-Cookbook.txt
 
 
 Copyright (C) 2013,  iroro orife
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 */


#include "LSNButterworthFilterImpl.h"
#include "../Utilities/LSNUtilities.h"


namespace lsn {

	// == Fuctions.
	/**
	 * Generic coefficients.
	 *
	 * \param _ftFilter The filter type.
	 * \param _dFs The sample rate of the data to which the filter will be applied.
	 * \param _dFreq1_Cutoff The first cut-off value.
	 * \param _dFreq2_Cutoff The second cut-off value (usually unused).
	 * \param _ui32FilterOrder The filter order.
	 * \param _vCoeffs The generated chain of coefficients.
	 * \param _dOverallGain The gain for adjustment purposes.
	 * \return Returns true if there were no errors gnerating the coefficients.
	 */
	bool CButterworthFilterImpl::Coefficients( LSN_FILTER_TYPE _ftFilter, const double _dFs, const double _dFreq1_Cutoff, const double _dFreq2_Cutoff, const uint32_t _ui32FilterOrder,
		std::vector<CBiQuadFilter> &_vCoeffs, double &_dOverallGain ) {
		//******************************************************************************
		// Init internal state based on filter design requirements.
    
		m_vZeros.resize( 2 * _ui32FilterOrder );
		m_vPoles.resize( 2 * _ui32FilterOrder );
    
		m_dF1 = _dFreq1_Cutoff;
		m_dF2 = _dFreq2_Cutoff;
    
		m_dWc = 0.0;  // Omega cutoff = passband edge frequency.
		m_dBw = 0.0;
    
    
		//******************************************************************************
		// Prewarp.
    
		m_dF1 = 2.0 * std::tan( LSN_PI * m_dF1 / _dFs );
		m_dF2 = 2.0 * std::tan( LSN_PI * m_dF2 / _dFs );
    
    
    
		//******************************************************************************
		// Design basic S-plane m_vPoles-only analogue LP prototype.
    
		// Get zeroes & poles of prototype analogue low pass.
		std::vector<std::complex<double>> vTmpPoles = PrototypeAnalogLowPass( _ui32FilterOrder );
    
		// Copy vTmpPoles into m_vPoles.
		size_t stIdx = 0;
		m_vPoles.resize( vTmpPoles.size() );
		for ( auto I = vTmpPoles.begin(); I != vTmpPoles.end(); I++, stIdx++ ) {
			m_vPoles[stIdx] = (*I);
		}
    
		m_vZeros.clear();			// Butterworth LP prototype has no zeroes.
		m_dGain = 1.0;				// Always 1 for the butterworth prototype lowpass.
    
    
		//******************************************************************************
		// Convert prototype to target filter type (LP/HP/BP/BS) - S-plane
    
		// Re-orient BP/BS corner frequencies if necessary
		if ( m_dF1 > m_dF2 ) {
			double dTmp = m_dF2;
			m_dF2 = m_dF1;
			m_dF1 = dTmp;
		}
    
		// Cutoff m_dWc = m_dF2
		switch ( _ftFilter ) {
			case LSN_FT_LOPASS : {
				ConvertToLoPass();
				break;
			}
            case LSN_FT_HIPASS : {
				ConvertToHiPass();
				break;
			}
            case LSN_FT_BANDPASS : {
				ConvertToBandPass();
				break;
			}
			case LSN_FT_BANDSTOP : {
				ConvertToBandStop();
				break;
			}
			default: {
				return false;
			}
		}
    
    
		//******************************************************************************
		// SANITY CHECK: Ensure m_vPoles are in the left half of the S-plane.
		for ( size_t I = 0; I < m_vPoles.size(); I++ ) {
			if ( m_vPoles[I].real() > 0 ) { return false; }
		}
    
    
		//******************************************************************************
		// Map zeroes & poles from S-plane to Z-plane.
    
		//nba = 0;
		m_vBa.resize( 2 * std::max( m_vPoles.size(), m_vZeros.size() ) + 5 );
		m_dPreBltGain = m_dGain;
    
		if ( !S2Z() ) { return false; }
    
    
		//******************************************************************************
		// Split up Z-plane m_vPoles and m_vZeros into SOS.
    
		if ( !Zp2Sos() ) { return false; }
    
		// correct the overall gain
		if ( _ftFilter == LSN_FT_LOPASS || _ftFilter == LSN_FT_BANDPASS ) {		// Pre-BLT is okay for S-plane.
			m_vBa[0] = m_dPreBltGain * (m_dPreBltGain / m_dGain);				// 2nd term is how much BLT boosts.
		}
		else if ( _ftFilter == LSN_FT_HIPASS || _ftFilter == LSN_FT_BANDSTOP ) {// HF gain != DC gain.
			m_vBa[0] = 1.0 / m_vBa[0];
		}
    
    
		//******************************************************************************
		// Init biquad chain with coefficients from SOS
    
		_dOverallGain = m_vBa[0];
		uint32_t uiNumFilters = _ui32FilterOrder / 2;
		if ( _ftFilter == LSN_FT_BANDPASS || _ftFilter == LSN_FT_BANDSTOP ) {
			uiNumFilters = _ui32FilterOrder;									// We have double the number of biquad sections.
		}
    
		_vCoeffs.resize( uiNumFilters );
		for ( size_t I = 0; I < uiNumFilters; I++ ) {
			(_vCoeffs)[I].DF2TBiquad( 1.0,						// b0.
				m_vBa[4*I+1],									// b1.
				m_vBa[4*I+2],									// b2.
				1.0,											// a0.
				m_vBa[4*I+3],									// a1.
				m_vBa[4*I+4] );									// a2.
		}
    
		return true;
	}

	/**
	 * Lowpass analogue prototype. Places Butterworth poles evenly around
	 * the S-plane unit circle.
	 *
	 * Reference: MATLAB buttap(_uiFilterOrder)
	 *
	 * \param _uiFilterOrder The filter order.
	 * \return Returns the Butterworth poles evenly placed around the S-plane unit circle.
	 */
	std::vector<std::complex<double>> CButterworthFilterImpl::PrototypeAnalogLowPass( uint32_t _uiFilterOrder ) {
		std::vector<std::complex<double>> vPoles;
    
		for ( uint32_t I = 0; I < (_uiFilterOrder + 1) / 2; I++ ) {
			double dTheta = (2 * I + 1.0) * LSN_PI / (2 * _uiFilterOrder);
			double dReal = -std::sin( dTheta );
			double dImag = std::cos( dTheta );
			vPoles.push_back( std::complex<double>( dReal,  dImag ) );
			vPoles.push_back( std::complex<double>( dReal, -dImag ) ); // Conjugate.
		}
    
		return vPoles;
	}

	/**
	 * Convert filter poles and zeros to second-order sections
	 *
	 * Reference: http://www.mathworks.com/help/signal/ref/zp2sos.html
	 *
	 * \return Returns true.
	 */
	bool CButterworthFilterImpl::Zp2Sos() {
		size_t stFilterOrder = std::max( m_vZeros.size(), m_vPoles.size() );
		std::vector<std::complex<double>> vZeroesTmp;
		vZeroesTmp.resize( stFilterOrder );
		std::vector<std::complex<double>> vPolesTmp;
		vPolesTmp.resize( stFilterOrder );
    
		// Copy.
		for ( size_t I = 0; I < m_vZeros.size(); I++ ) {
			vZeroesTmp[I] = m_vZeros[I];
		}
    
		// Add zeros at -1, so if S-plane degenerate case where
		// m_vZeros.size() = 0 will map to -1 in Z-plane.
		for ( size_t I = m_vZeros.size(); I < stFilterOrder; I++ ) {
			vZeroesTmp[I] = std::complex<double>( -1, 0 );
		}
    
		// Copy.
		for ( size_t I = 0; I < m_vPoles.size(); I++ ) {
			vPolesTmp[I] = m_vPoles[I];
		}
    
		m_vBa[0] = m_dGain; // Store the gain.
    
		uint32_t uiNumSos = 0;
		for ( size_t I = 0; I + 1 < stFilterOrder; I += 2, uiNumSos++ ) {
			m_vBa[4*uiNumSos+1] = -(vZeroesTmp[I] + vZeroesTmp[I+1]).real();
			m_vBa[4*uiNumSos+2] =  (vZeroesTmp[I] * vZeroesTmp[I+1]).real();
			m_vBa[4*uiNumSos+3] = -(vPolesTmp[I] + vPolesTmp[I+1]).real();
			m_vBa[4*uiNumSos+4] =  (vPolesTmp[I] * vPolesTmp[I+1]).real();
		}
    
		// Odd filter order thus one pair of poles/zeros remains
		if ( stFilterOrder % 2 == 1 ) {
			m_vBa[4*uiNumSos+1] = -vZeroesTmp[stFilterOrder-1].real();
			m_vBa[4*uiNumSos+2] = m_vBa[4*uiNumSos+4] = 0;
			m_vBa[4*uiNumSos+3] = -vPolesTmp[stFilterOrder-1].real();
			uiNumSos++;
		}
    
		// Set output param.
		m_ui32Nba = 1 + 4 * uiNumSos;
    
		return true;
	}

	/**
	 * Convert analog lowpass prototype poles to lowpass.
	 */
	void CButterworthFilterImpl::ConvertToLoPass() {
		m_dWc = m_dF2;										// Critical frequency.
    
		m_dGain *= std::pow( m_dWc, m_vPoles.size() );
    
		m_vZeros.clear();									// Poles ONLY here!
		for ( size_t I = 0; I < m_vPoles.size(); I++ ) {		// Scale poles by the cutoff Wc.
			m_vPoles[I] = m_dWc * m_vPoles[I];
		}
	}

	/**
	 * Convert lowpass poles & zeros to highpass.
	 *	with Wc = f2, use:  hp_S = Wc / lp_S;
	 */
	void CButterworthFilterImpl::ConvertToHiPass() {
		m_dWc = m_dF2;										// Critical frequency.
    
		// Calculate gain.
		std::complex<double> cdProdZ( 1.0, 0.0 );
		std::complex<double> cdProdP( 1.0, 0.0 );
    
		for ( size_t I = 0; I < m_vZeros.size(); I++ ) {
			cdProdZ *= -m_vZeros[I];
		}
    
		for ( size_t I = 0; I < m_vPoles.size(); I++ ) {
			cdProdP *= -m_vPoles[I];
		}
    
		m_dGain *= cdProdZ.real() / cdProdP.real();
    
		// Convert LP poles to HP.
		for ( size_t I = 0; I < m_vPoles.size(); I++ ) {
			if( std::abs( m_vPoles[I] ) ) {
				m_vPoles[I] = std::complex<double>(m_dWc) / m_vPoles[I];	//  hp_S = m_dWc / lp_S;
			}
		}
    
		// Init with zeroes, no non-zero values to convert.
		m_vZeros.resize( m_vPoles.size() );
		for ( size_t I = 0; I < m_vZeros.size(); I++ ) {
			m_vZeros[I] = std::complex<double>(0.0);
		}
	}

	/**
	 * Convert lowpass poles to bandpass.
	 * use:  bp_S = 0.5 * lp_S * BW +
	 *	0.5 * sqrt ( BW^2 * lp_S^2 - 4*Wc^2 )
	 * where   BW = W2 - W1
	 *	Wc^2 = W2 * W1
	 */
	void CButterworthFilterImpl::ConvertToBandPass() {
		m_dBw = m_dF2 - m_dF1;
		m_dWc = std::sqrt( m_dF1 * m_dF2 );
    
		// Calculate bandpass gain
		m_dGain *= std::pow( m_dBw, m_vPoles.size() - m_vZeros.size() );
    
		// Convert LP poles to BP: these two sets of for-loops result in an ordered
		// list of poles and their complex conjugates
		std::vector<std::complex<double>> vTmpPoles;
		for ( size_t I = 0; I < m_vPoles.size(); I++ ) {				// First set of poles + conjugates.
			if ( std::abs( m_vPoles[I] ) ) {
				std::complex<double> cdFirstTerm = 0.5 * m_vPoles[I] * m_dBw;
				std::complex<double> cdSecondTerm = 0.5 * std::sqrt( (m_dBw * m_dBw) * (m_vPoles[I] * m_vPoles[I]) - 4 * m_dWc * m_dWc );
				vTmpPoles.push_back( cdFirstTerm + cdSecondTerm );
			}
		}
    
		for ( size_t I = 0; I < m_vPoles.size(); I++ ) {				// Second set of poles + conjugates.
			if ( std::abs( m_vPoles[I] ) ) {
				std::complex<double> cdFirstTerm = 0.5 * m_vPoles[I] * m_dBw;
				std::complex<double> cdSecondTerm = 0.5 * std::sqrt( (m_dBw * m_dBw) * (m_vPoles[I] * m_vPoles[I]) - 4 * m_dWc * m_dWc );
				vTmpPoles.push_back( cdFirstTerm - cdSecondTerm );		// Complex conjugate.
			}
		}
    
		// Init zeros, no non-zero values to convert.
		m_vZeros.resize( m_vPoles.size() );
		for ( size_t I = 0; I < m_vZeros.size(); I++ ) {
			m_vZeros[I] = std::complex<double>( 0.0 );
		}
    
		// Copy converted poles to output array.
		size_t stIdx = 0;
		m_vPoles.resize( vTmpPoles.size() );
		for ( auto I = vTmpPoles.begin(); I != vTmpPoles.end(); I++, stIdx++ ) {
			m_vPoles[stIdx] = (*I);
		}
	}

	/**
	 * Convert lowpass poles to bandstop.
	 * use:  bs_S = 0.5 * BW / lp_S +
	 *	0.5 * sqrt ( BW^2 / lp_S^2 - 4*Wc^2 )
	 * where   BW = W2 - W1
	 *	Wc^2 = W2 * W1
	 */
	void CButterworthFilterImpl::ConvertToBandStop() {
		m_dBw = m_dF2 - m_dF1;
		m_dWc = std::sqrt( m_dF1 * m_dF2 );
    
		// Compute gain
		std::complex<double> cdProdZ( 1.0, 0.0 );
		std::complex<double> cdProdP( 1.0, 0.0 );
		for ( size_t I = 0; I < m_vZeros.size(); I++ ) {
			cdProdZ *= -m_vZeros[I];
		}
    
		for ( size_t I = 0; I < m_vPoles.size(); I++ ) {
			cdProdP *= -m_vPoles[I];
		}
    
		m_dGain *= cdProdZ.real() / cdProdP.real();
    
		// Convert LP zeros to band stop
		m_vZeros.reserve( m_vPoles.size() );
		std::vector<std::complex<double>> vTmpZ;
		for ( size_t I = 0; I < m_vZeros.size(); I++ ) {
			vTmpZ.push_back( std::complex<double>( 0.0,  m_dWc ) );
			vTmpZ.push_back( std::complex<double>( 0.0, -m_dWc ) );			// Complex conjugate.
		}
    
		std::vector<std::complex<double>> vTmpPoles;
		for ( size_t I = 0; I < m_vPoles.size(); I++ ) {					// First set of poles + conjugates
			if ( std::abs( m_vPoles[I] ) ) {
				std::complex<double> cdTerm1 = 0.5 * m_dBw / m_vPoles[I];
				std::complex<double> cdTerm2 = 0.5 * std::sqrt( (m_dBw * m_dBw) / (m_vPoles[I] * m_vPoles[I]) - (4 * m_dWc * m_dWc) );
				vTmpPoles.push_back( cdTerm1 + cdTerm2 );
			}
		}
    
		for ( size_t I = 0; I < m_vPoles.size(); I++ ) {					// Second set of poles + conjugates.
			if ( std::abs( m_vPoles[I] ) ) {
				std::complex<double> cdTerm1 = 0.5 * m_dBw / m_vPoles[I];
				std::complex<double> cdTerm2 = 0.5 * std::sqrt( (m_dBw * m_dBw) / (m_vPoles[I] * m_vPoles[I]) - (4 * m_dWc * m_dWc) );
				vTmpPoles.push_back( cdTerm1 - cdTerm2 );					// Complex conjugate.
			}
		}
    
		// Copy converted zeros to output array.
		size_t stIdx = 0;
		m_vZeros.resize( vTmpZ.size() );
		for ( auto I = vTmpZ.begin(); I != vTmpZ.end(); I++, stIdx++ ) {
			m_vZeros[stIdx] = (*I);
		}
    
		// Copy converted poles to output array.
		stIdx = 0;
		m_vPoles.resize( vTmpPoles.size() );
		for ( auto I = vTmpPoles.begin(); I != vTmpPoles.end(); I++, stIdx++ ) {
			m_vPoles[stIdx] = (*I);
		}
	}

}	// namespace lsn

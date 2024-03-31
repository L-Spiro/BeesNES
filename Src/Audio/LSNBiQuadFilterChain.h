/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A chain of bi-quad filters for a Butterworth filter implementation.
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


#pragma once

//#include "../LSNLSpiroNes.h"
#include "LSNBiQuadFilter.h"

#include <vector>


namespace lsn {

	/**
	 * Class CBiQuadFilterChain
	 * \brief A chain of bi-quad filters for a Butterworth filter implementation.
	 *
	 * Description: A chain of bi-quad filters for a Butterworth filter implementation.
	 */
	template <unsigned _uFilters>
	class CBiQuadFilterChain {
	public :


		// == Functions.
		/**
		 * Sets the number of orders for the filter.  Multiply this by 6 to get the rate of fall-off for each octave (1st-order = 6dB per octave, 10th-order = 60dB per octave).
		 *
		 * \return Returns true if no exceptions were thrown during allocation.
		 */
		bool								SetOrder() {
			return Allocate();
		}

		/**
		 * Fills values with defaults.
		 */
		void								MakeDefault() {
			m_dXn1 = m_dXn2 = m_dXn3 = m_dXn4 = 0.0;
			for ( auto I = m_vYn0.size(); I--; ) {
				m_vYn0[I] = m_vYn1[I] = m_vYn2[I] = 0.0;
				m_vYn3[I] = m_vYn4[I] = 0.0;
			}
			m_pdYn = m_vYn0.data();
			m_pdYn1 = m_vYn1.data();
			m_pdYn2 = m_vYn2.data();
		}
    
		/**
		 * Processes the biquad chain on the input, creating the output.
		 *
		 * \param _vInput The input buffer to process.
		 * \param _vOutput The result.
		 * \param _pbqfCoeffs Array of at _uFilters coefficients.
		 */
		void								ProcessBiQuad( const std::vector<double> &_vInput, std::vector<double> &_vOutput, const CBiQuadFilter * _pbqfCoeffs ) {
			double * pdYn = &m_vYn0[0];
			double * pdYn1 = &m_vYn1[0];
			double * pdYn2 = &m_vYn2[0];
			try {
				_vOutput.resize( _vInput.size() );
			}
			catch ( const std::bad_alloc & /*_eE*/ ) { return; }
    
			for ( size_t I = 0; I < _vInput.size(); I++ ) {
				double dXn = _vInput[I];
        
				pdYn[0] = _pbqfCoeffs[0].m_dB0 * dXn + _pbqfCoeffs[0].m_dB1 * m_dXn1 + _pbqfCoeffs[0].m_dB2 * m_dXn2
					+ _pbqfCoeffs[0].m_dA1 * pdYn1[0] + _pbqfCoeffs[0].m_dA2 * pdYn2[0];
        
				for ( size_t J = 1; J < _uFilters; J++ ) {
					pdYn[J] = _pbqfCoeffs[J].m_dB0 * pdYn[J-1] + _pbqfCoeffs[J].m_dB1 * pdYn1[J-1] + _pbqfCoeffs[J].m_dB2 * pdYn2[J-1]
						+ _pbqfCoeffs[J].m_dA1 * pdYn1[J] + _pbqfCoeffs[J].m_dA2 * pdYn2[J];
				}
        
				// Shift delay line elements.
				for ( size_t J = 0; J < _uFilters; J++ ) {
					pdYn2[J] = pdYn1[J];
					pdYn1[J] = pdYn[J];
				}
				m_dXn2 = m_dXn1;
				m_dXn1 = dXn;
        
				// Store result and stride
				_vOutput[I] = pdYn[_uFilters-1];
			}
		}

		/**
		 * Processes the biquad chain on the input, creating the output.
		 *
		 * \param _dInput The input sample to process.
		 * \param _pbqfCoeffs Array of at _uFilters coefficients.
		 * \return Returns the processed sample.
		 */
		double								ProcessBiQuad( double _dInput, const CBiQuadFilter * _pbqfCoeffs ) {        
			m_pdYn[0] = _pbqfCoeffs[0].m_dB0 * _dInput + _pbqfCoeffs[0].m_dB1 * m_dXn1 + _pbqfCoeffs[0].m_dB2 * m_dXn2
				+ _pbqfCoeffs[0].m_dA1 * m_pdYn1[0] + _pbqfCoeffs[0].m_dA2 * m_pdYn2[0];

			if constexpr ( _uFilters >= 2 ) {
				m_pdYn[1] = _pbqfCoeffs[1].m_dB0 * m_pdYn[1-1] + _pbqfCoeffs[1].m_dB1 * m_pdYn1[1-1] + _pbqfCoeffs[1].m_dB2 * m_pdYn2[1-1]
					+ _pbqfCoeffs[1].m_dA1 * m_pdYn1[1] + _pbqfCoeffs[1].m_dA2 * m_pdYn2[1];
			}
			if constexpr ( _uFilters >= 3 ) {
				m_pdYn[2] = _pbqfCoeffs[2].m_dB0 * m_pdYn[2-1] + _pbqfCoeffs[2].m_dB1 * m_pdYn1[2-1] + _pbqfCoeffs[2].m_dB2 * m_pdYn2[2-1]
					+ _pbqfCoeffs[2].m_dA1 * m_pdYn1[2] + _pbqfCoeffs[2].m_dA2 * m_pdYn2[2];
			}
			if constexpr ( _uFilters >= 4 ) {
				m_pdYn[3] = _pbqfCoeffs[3].m_dB0 * m_pdYn[3-1] + _pbqfCoeffs[3].m_dB1 * m_pdYn1[3-1] + _pbqfCoeffs[3].m_dB2 * m_pdYn2[3-1]
					+ _pbqfCoeffs[3].m_dA1 * m_pdYn1[3] + _pbqfCoeffs[3].m_dA2 * m_pdYn2[3];
			}
			if constexpr ( _uFilters >= 5 ) {
				m_pdYn[4] = _pbqfCoeffs[4].m_dB0 * m_pdYn[4-1] + _pbqfCoeffs[4].m_dB1 * m_pdYn1[4-1] + _pbqfCoeffs[4].m_dB2 * m_pdYn2[4-1]
					+ _pbqfCoeffs[4].m_dA1 * m_pdYn1[4] + _pbqfCoeffs[4].m_dA2 * m_pdYn2[4];
			}
        
			// Shift delay line elements.
			if constexpr ( _uFilters >= 1 ) {
				m_pdYn2[0] = m_pdYn1[0];
				m_pdYn1[0] = m_pdYn[0];
			}
			if constexpr ( _uFilters >= 2 ) {
				m_pdYn2[1] = m_pdYn1[1];
				m_pdYn1[1] = m_pdYn[1];
			}
			if constexpr ( _uFilters >= 3 ) {
				m_pdYn2[2] = m_pdYn1[2];
				m_pdYn1[2] = m_pdYn[2];
			}
			if constexpr ( _uFilters >= 4 ) {
				m_pdYn2[3] = m_pdYn1[3];
				m_pdYn1[3] = m_pdYn[3];
			}
			if constexpr ( _uFilters >= 5 ) {
				m_pdYn2[4] = m_pdYn1[4];
				m_pdYn1[4] = m_pdYn[4];
			}
			m_dXn2 = m_dXn1;
			m_dXn1 = _dInput;
        
			// Store result and stride
			return m_pdYn[_uFilters-1];
		}
    
		/**
		 * Processes the 4th-order biquad chain on the input, creating the output.
		 *
		 * \param _vInput The input buffer to process.
		 * \param _vOutput The result.
		 * \param _pbqfCoeffs Array of at _uFilters coefficients.
		 */
		void								ProcessFourthOrderSections( const std::vector<double> &_vInput, std::vector<double> &_vOutput, const CBiQuadFilter * _pbqfCoeffs ) {
			double * pdYn = &m_vYn0[0];
			double * pdYn1 = &m_vYn1[0];
			double * pdYn2 = &m_vYn2[0];
			double * pdYn3 = &m_vYn3[0];
			double * pdYn4 = &m_vYn4[0];

			try {
				_vOutput.resize( _vInput.size() );
			}
			catch ( const std::bad_alloc & /*_eE*/ ) { return; }
    
			for ( size_t I = 0; I < _vInput.size(); I++ ) {
				double dXn = _vInput[I];
        
				pdYn[0] = _pbqfCoeffs[0].m_dB0 * dXn
					+ _pbqfCoeffs[0].m_dB1 * m_dXn1
					+ _pbqfCoeffs[0].m_dB2 * m_dXn2
					+ _pbqfCoeffs[0].m_dB3 * m_dXn3
					+ _pbqfCoeffs[0].m_dB4 * m_dXn4 +
        
					_pbqfCoeffs[0].m_dA1 * pdYn1[0]
						+ _pbqfCoeffs[0].m_dA2 * pdYn2[0]
						+ _pbqfCoeffs[0].m_dA3 * pdYn3[0]
						+ _pbqfCoeffs[0].m_dA4 * pdYn4[0];
        
				for ( size_t J = 1; J < _uFilters; J++ ) {
					pdYn[J] = _pbqfCoeffs[J].m_dB0 * pdYn[J - 1]
						+ _pbqfCoeffs[J].m_dB1 * pdYn1[J-1]
						+ _pbqfCoeffs[J].m_dB2 * pdYn2[J-1]
						+ _pbqfCoeffs[J].m_dB3 * pdYn3[J-1]
						+ _pbqfCoeffs[J].m_dB4 * pdYn4[J-1] +
            
						_pbqfCoeffs[J].m_dA1 * pdYn1[J]
							+ _pbqfCoeffs[J].m_dA2 * pdYn2[J]
							+ _pbqfCoeffs[J].m_dA3 * pdYn3[J]
							+ _pbqfCoeffs[J].m_dA4 * pdYn4[J];
				}
        
				// Shift delay line elements.
				for ( size_t J = 0; J < _uFilters; J++ ) {
					pdYn4[J] = pdYn3[J];
					pdYn3[J] = pdYn2[J];
					pdYn2[J] = pdYn1[J];
					pdYn1[J] = pdYn[J];
				}
        
				m_dXn4 = m_dXn3;
				m_dXn3 = m_dXn2;
				m_dXn2 = m_dXn1;
				m_dXn1 = dXn;
        
				// Store result and stride
				_vOutput[I] = pdYn[_uFilters-1];        
			}
		}


	protected :
		// == Members.
		/** NUmber of filters. */
		//uint32_t							_uFilters;
		/** Coefficients. */
		double								m_dXn1, m_dXn2;
		std::vector<double>					m_vYn0, m_vYn1, m_vYn2;

		/** 4th-order variables. */
		double								m_dXn3, m_dXn4;
		std::vector<double>					m_vYn3, m_vYn4;

		/* Fast pointer access. */
		double *							m_pdYn;
		double *							m_pdYn1;
		double *							m_pdYn2;


		// == Functions.
		/**
		 * Allocates the necessary arrays.
		 *
		 * \return Returns true if no exceptions were thrown during allocation.
		 */
		bool								Allocate() {
			try {
				m_vYn0.resize( _uFilters );
				m_vYn1.resize( _uFilters );
				m_vYn2.resize( _uFilters );
    
				// Fourth-order sections.
				m_vYn3.resize( _uFilters );
				m_vYn4.resize( _uFilters );
			}
			catch ( const std::bad_alloc & /*_eE*/ ) {
				return false;
			}
			return true;
		}
	};

}	// namespace lsn

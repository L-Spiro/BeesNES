/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Handles coefficients for a biquad filter.
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

#include "../LSNLSpiroNes.h"


namespace lsn {

	/**
	 * Class CBiQuadFilter
	 * \brief Handles coefficients for a biquad filter.
	 *
	 * Description: Handles coefficients for a biquad filter.
	 */
	class CBiQuadFilter {
		template <unsigned _uFilters>
		friend class						CBiQuadFilterChain;
	public :
		CBiQuadFilter() :
			m_dB0( 0.0 ),
			m_dB1( 0.0 ),
			m_dB2( 0.0 ),
			m_dB3( 0.0 ),
			m_dB4( 0.0 ),
			m_dA0( 0.0 ),
			m_dA1( 0.0 ),
			m_dA2( 0.0 ),
			m_dA3( 0.0 ),
			m_dA4( 0.0 ) {
		}


		// == Functions.
		/**
		 * Coefficients for a DF2T biquad section.
		 *
		 * \param _dB0 A coefficient.
		 * \param _dB1 A coefficient.
		 * \param _dB2 A coefficient.
		 * \param _dA0 A coefficient.
		 * \param _dA1 A coefficient.
		 * \param _dA2 A coefficient.
		 */
		void									DF2TBiquad( double _dB0, double _dB1, double _dB2,
			double _dA0, double _dA1, double _dA2 ) {
			m_dB0 = _dB0 / _dA0;
			m_dB1 = _dB1 / _dA0;
			m_dB2 = _dB2 / _dA0;
			m_dA1 = (-_dA1) / _dA0;  // The negation conforms the Direct-Form II Transposed discrete-time
			m_dA2 = (-_dA2) / _dA0;  // filter (DF2T) coefficients to the expectations of the process function.
		}

		/**
		 * Coefficients for a DF2T fourth order section (Used for EQ filters).
		 *
		 * \param _dB0 A coefficient.
		 * \param _dB1 A coefficient.
		 * \param _dB2 A coefficient.
		 * \param _dB3 A coefficient.
		 * \param _dB4 A coefficient.
		 * \param _dA0 A coefficient.
		 * \param _dA1 A coefficient.
		 * \param _dA2 A coefficient.
		 * \param _dA3 A coefficient.
		 * \param _dA4 A coefficient.
		 */
		void									DF2TFourthOrderSection( double _dB0, double _dB1, double _dB2, double _dB3, double _dB4,
			double _dA0, double _dA1, double _dA2, double _dA3, double _dA4 ) {
			m_dB0 = _dB0 / _dA0;
			m_dB1 = _dB1 / _dA0;
			m_dB2 = _dB2 / _dA0;
			m_dB3 = _dB3 / _dA0;
			m_dB4 = _dB4 / _dA0;
    
			m_dA1 = (-_dA1) / _dA0;  // The negation conforms the Direct-Form II Trm_dAnsposed discrete-time
			m_dA2 = (-_dA2) / _dA0;  // filter (DF2T) coefficients to the expectm_dAtions of the process function.
			m_dA3 = (-_dA3) / _dA0;
			m_dA4 = (-_dA4) / _dA0;
		}
		

	protected :
		// == Members.
		/** Coefficients. */
		double									m_dB0, m_dB1, m_dB2, m_dB3, m_dB4;
		double									m_dA0, m_dA1, m_dA2, m_dA3, m_dA4;
	};

}	// namespace lsn

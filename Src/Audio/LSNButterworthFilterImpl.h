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


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNBiQuadFilterChain.h"

#include <cmath>
#include <complex>
#include <vector>


namespace lsn {

	/**
	 * Class CButterworthFilterImpl
	 * \brief A Butterworth filter.
	 *
	 * Description: A Butterworth filter.
	 */
	class CButterworthFilterImpl {
	public :
		// == Types.
		enum LSN_FILTER_TYPE {
			LSN_FT_LOPASS,
			LSN_FT_HIPASS,
			LSN_FT_BANDPASS,
			LSN_FT_BANDSTOP,
			LSN_FT_LOSHELF,
			LSN_FT_HISHELF,
			LSN_FT_PARAMETRIC,
		};


		// == Fuctions.
		/**
		 * Creates a low-pass filter.
		 *
		 * \param _dFs The sample rate of the data to which the filter will be applied.
		 * \param _dFreq1_Cutoff The first cut-off value.
		 * \param _ui32FilterOrder The filter order.
		 * \param _vCoeffs The generated chain of coefficients.
		 * \param _dOverallGain The gain for adjustment purposes.
		 * \return Returns true if there were no errors gnerating the coefficients.
		 */
		bool										LoPass( const double _dFs, double _dFreq1_Cutoff, uint32_t _ui32FilterOrder,
			std::vector<CBiQuadFilter> &_vCoeffs, double &_dOverallGain ) {
			return Coefficients( LSN_FT_LOPASS, _dFs, _dFreq1_Cutoff, 0.0, _ui32FilterOrder, _vCoeffs, _dOverallGain );
		}
    
		/**
		 * Creates a high-pass filter.
		 *
		 * \param _dFs The sample rate of the data to which the filter will be applied.
		 * \param _dFreq1_Cutoff The first cut-off value.
		 * \param _ui32FilterOrder The filter order.
		 * \param _vCoeffs The generated chain of coefficients.
		 * \param _dOverallGain The gain for adjustment purposes.
		 * \return Returns true if there were no errors gnerating the coefficients.
		 */
		bool										HiPass( const double _dFs, double _dFreq1_Cutoff, uint32_t _ui32FilterOrder,
			std::vector<CBiQuadFilter> &_vCoeffs, double &_dOverallGain ) {
			return Coefficients( LSN_FT_HIPASS, _dFs, _dFreq1_Cutoff, 0.0, _ui32FilterOrder, _vCoeffs, _dOverallGain );
		}
    
		/**
		 * Creates a band-pass filter.
		 *
		 * \param _dFs The sample rate of the data to which the filter will be applied.
		 * \param _dFreq1_Cutoff The first cut-off value.
		 * \param _dFreq2_Cutoff The second cut-off value.
		 * \param _ui32FilterOrder The filter order.
		 * \param _vCoeffs The generated chain of coefficients.
		 * \param _dOverallGain The gain for adjustment purposes.
		 * \return Returns true if there were no errors gnerating the coefficients.
		 */
		bool										BandPass( const double _dFs, double _dFreq1_Cutoff, double _dFreq2_Cutoff, uint32_t _ui32FilterOrder,
			std::vector<CBiQuadFilter> &_vCoeffs, double &_dOverallGain ) {
			return Coefficients( LSN_FT_BANDPASS, _dFs, _dFreq1_Cutoff, _dFreq2_Cutoff, _ui32FilterOrder, _vCoeffs, _dOverallGain );
		}
    
		/**
		 * Creates a band-stop filter.
		 *
		 * \param _dFs The sample rate of the data to which the filter will be applied.
		 * \param _dFreq1_Cutoff The first cut-off value.
		 * \param _dFreq2_Cutoff The second cut-off value.
		 * \param _ui32FilterOrder The filter order.
		 * \param _vCoeffs The generated chain of coefficients.
		 * \param _dOverallGain The gain for adjustment purposes.
		 * \return Returns true if there were no errors gnerating the coefficients.
		 */
		bool										BandStop( const double _dFs, double _dFreq1_Cutoff, double _dFreq2_Cutoff, uint32_t _ui32FilterOrder,
			std::vector<CBiQuadFilter> &_vCoeffs, double &_dOverallGain ) {
			return Coefficients( LSN_FT_BANDSTOP, _dFs, _dFreq1_Cutoff, _dFreq2_Cutoff, _ui32FilterOrder, _vCoeffs, _dOverallGain );
		}

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
		bool										Coefficients( LSN_FILTER_TYPE _ftFilter, const double _dFs, const double _dFreq1_Cutoff, const double _dFreq2_Cutoff, const uint32_t _ui32FilterOrder,
			std::vector<CBiQuadFilter> &_vCoeffs, double &_dOverallGain );
		/**
		 * Lowpass analogue prototype. Places Butterworth poles evenly around
		 * the S-plane unit circle.
		 *
		 * Reference: MATLAB buttap(_uiFilterOrder)
		 *
		 * \param _uiFilterOrder The filter order.
		 * \return Returns the Butterworth poles evenly placed around the S-plane unit circle.
		 */
		std::vector<std::complex<double>>			PrototypeAnalogLowPass( uint32_t _uiFilterOrder );



	protected :
		// == Members.
		/** Internal state. */
		double										m_dF1, m_dF2;
		/** Number of poles and zeroes. */
		//uint32_t									m_ui32NumPoles, m_ui32NumZeros;
		/** The zeroes. */
		std::vector<std::complex<double>>			m_vZeros;
		/** The poles. */
		std::vector<std::complex<double>>			m_vPoles;
		/** Omega cut-off (passband edge frequency). */
		double										m_dWc;
		/** Bandwidth. */
		double										m_dBw;
		/** Gain. */
		double										m_dGain;
		/** Pre-transform gain. */
		double										m_dPreBltGain;
		uint32_t									m_ui32Nba;
		std::vector<double>							m_vBa;


		// == Functions.
		/**
		 * Z = (2 + S) / (2 - S) is the S-plane to Z-plane bilinear transform.
		 *
		 * Reference: http://en.wikipedia.org/wiki/Bilinear_transform
		 *
		 * \param _cdSz The plane to convert.
		 * \return Returns the gain.
		 */
		double										Blt( std::complex<double> &_cdSz ) {
    
			std::complex<double> cdTwo( 2.0, 0 );
			std::complex<double> cdS = _cdSz;      // _cdSz is an input(S-plane) & output(Z-plane) arg.
			_cdSz = (cdTwo + cdS) / (cdTwo - cdS);
    
			// return the gain.
			return std::abs( (cdTwo - cdS) );
		}

		/**
		 * Convert poles & zeros from S-plane to Z-plane via Bilinear Tranform (BLT).
		 *
		 * \return Returns true.
		 */
		bool										S2Z() {
			// BLT zeros.
			for ( size_t I = 0; I < m_vZeros.size(); I++ ) {
				m_dGain /= Blt( m_vZeros[I] );
			}
    
			// BLT poles.
			for ( size_t I = 0; I < m_vPoles.size(); I++ ) {
				m_dGain *= Blt( m_vPoles[I] );
			}
    
			return true;
		}

		/**
		 * Convert filter poles and zeros to second-order sections
		 *
		 * Reference: http://www.mathworks.com/help/signal/ref/zp2sos.html
		 *
		 * \return Returns true.
		 */
		bool										Zp2Sos();

		/**
		 * Convert analog lowpass prototype poles to lowpass.
		 */
		void										ConvertToLoPass();

		/**
		 * Convert lowpass poles & zeros to highpass.
		 *	with Wc = f2, use:  hp_S = Wc / lp_S;
		 */
		void										ConvertToHiPass();

		/**
		 * Convert lowpass poles to bandpass.
		 * use:  bp_S = 0.5 * lp_S * BW +
		 *	0.5 * sqrt ( BW^2 * lp_S^2 - 4*Wc^2 )
		 * where   BW = W2 - W1
		 *	Wc^2 = W2 * W1
		 */
		void										ConvertToBandPass();

		/**
		 * Convert lowpass poles to bandstop.
		 * use:  bs_S = 0.5 * BW / lp_S +
		 *	0.5 * sqrt ( BW^2 / lp_S^2 - 4*Wc^2 )
		 * where   BW = W2 - W1
		 *	Wc^2 = W2 * W1
		 */
		void										ConvertToBandStop();

	};

}	// namespace lsn

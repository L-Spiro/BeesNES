/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A sinc filter.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include <cmath>
#include <numbers>
#include <vector>

namespace lsn {

	/**
	 * Class CSincFilter
	 * \brief A sinc filter.
	 *
	 * Description: A sinc filter.
	 */
	class CSincFilter {
	public :
		inline CSincFilter();
		inline ~CSincFilter();


		// == Types.
		/** The window function. */
		typedef void (*				PfWindowFunc)( std::vector<double> & );


		// == Functions.
		/**
		 * Creates an IR filter given the sampling rate, the cut-off frequency, and the bandwidth.
		 * 
		 * \param _dFs The sample frequency.
		 * \param _dFc The cut-off frequency.
		 * \param _dBw The bandwidth of the roll-off.
		 * \param _pfSynthFunc The synthesis function (IE SynthesizeBlackmanWindow or SynthesizeHammingWindow).
		 **/
		inline void					Create( double _dFs, double _dFc, double _dBw, PfWindowFunc _pfSynthFunc );

		/**
		 * Creates an IR filter given the sampling rate, the cut-off frequency, and the bandwidth.
		 * 
		 * \param _dFs The sample frequency.
		 * \param _dFc The cut-off frequency.
		 * \param _stM The bandwidth of the roll-off.
		 * \param _pfSynthFunc The synthesis function (IE SynthesizeBlackmanWindow or SynthesizeHammingWindow).
		 **/
		void						Create( double _dFs, double _dFc, size_t _stM, PfWindowFunc _pfSynthFunc );

		/**
		 * Synthesizes a Blackman window.
		 *
		 * \param _vTaps The array of taps to fill.
		 */
		static void					SynthesizeBlackmanWindow( std::vector<double> &_vTaps );

		/**
		 * Synthesizes a Hamming window.
		 *
		 * \param _vTaps The array of taps to fill.
		 */
		static void					SynthesizeHammingWindow( std::vector<double> &_vTaps );

		/**
		 * Synthesizes a Hanning window.
		 *
		 * \param _vTaps The array of taps to fill.
		 */
		static void					SynthesizeHanningWindow( std::vector<double> &_vTaps );



	protected :
		// == Members.
		/** The array of coefficients. */
		std::vector<double>			m_vCeof;
		/** The ring buffer of past samples. */
		std::vector<double>			m_vRing;
		/** The current index into the ring buffer. */
		size_t						m_stRing;
	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	inline CSincFilter::CSincFilter() :
		m_stRing( 0 ) {
	}
	inline CSincFilter::~CSincFilter() {
	}

	// == Functions.
	/**
	 * Creates an IR filter given the sampling rate, the cut-off frequency, and the bandwidth.
	 * 
	 * \param _dFs The sample frequency.
	 * \param _dFc The cut-off frequency.
	 * \param _dBw The bandwidth of the roll-off.
	 * \param _pfSynthFunc The synthesis function (IE SynthesizeBlackmanWindow or SynthesizeHammingWindow).
	 **/
	inline void CSincFilter::Create( double _dFs, double _dFc, double _dBw, PfWindowFunc _pfSynthFunc ) {
		Create( _dFs, _dFc, size_t( std::ceil( 4.0 / (_dBw / _dFs) ) ), _pfSynthFunc );
	}

	/**
	 * Creates an IR filter given the sampling rate, the cut-off frequency, and the bandwidth.
	 * 
	 * \param _dFs The sample frequency.
	 * \param _dFc The cut-off frequency.
	 * \param _stM The bandwidth of the roll-off.
	 * \param _pfSynthFunc The synthesis function (IE SynthesizeBlackmanWindow or SynthesizeHammingWindow).
	 **/
	void CSincFilter::Create( double _dFs, double _dFc, size_t _stM, PfWindowFunc _pfSynthFunc ) {
		_dFc /= _dFs;										// Cut-off ratio.
		
		if ( !(_stM & 1) ) { ++_stM; }						// Must have an odd number of samples for symmetry.

		m_vCeof.resize( _stM );
		size_t stL = _stM / 2;								// The center sample is the latency.
		for ( auto I = m_vCeof.size(); I--; ) {
			m_vCeof[I] = 1.0;
		}
		// Create window.
		_pfSynthFunc( m_vCeof );


		// Apply sinc function.
		double dFc2 = 2.0 * _dFc;
		const double dTau = 2.0 * std::numbers::pi;
		int64_t i64SignedL = int64_t( stL );
		for ( auto I = m_vCeof.size(); I--; ) {
			int64_t N = int64_t( I ) - i64SignedL;
			if ( N ) {
				//m_vCeof[I] *= Sinc( double( N ), dFc2 );
				m_vCeof[I] *= std::sin( dTau * _dFc * N ) / (std::numbers::pi * N);
			}
			else {
				//m_vCeof[I] *= dTau * _dFc;
				m_vCeof[I] *= dFc2;
			}
		}

		// Normalize.
		{
			double dSum = 0.0;
			for ( auto I = m_vCeof.size(); I--; ) {
				dSum += m_vCeof[I];
			}
			double dNorm = 1.0 / dSum;
			for ( auto I = m_vCeof.size(); I--; ) {
				m_vCeof[I] *= dNorm;
			}
		}
	}

	/**
	 * Synthesizes a Blackman window.
	 *
	 * \param _vTaps The array of taps to fill.
	 */
	inline void CSincFilter::SynthesizeBlackmanWindow( std::vector<double> &_vTaps ) {
		const double dTau = 2.0 * std::numbers::pi;
		size_t stMax = _vTaps.size() - 1;
		double dInvMax = 1.0 / stMax;
		double dTauInvMax = dTau * dInvMax;
		double dTauInvMax2 = 2.0 * dTauInvMax;
		for ( auto I = _vTaps.size(); I--; ) {
			_vTaps[I] = 0.42 - 0.5 * std::cos( dTauInvMax * I ) + 0.08 * std::cos( dTauInvMax2 * I );
		}
	}

	/**
	 * Synthesizes a Hamming window.
	 *
	 * \param _vTaps The array of taps to fill.
	 */
	inline void CSincFilter::SynthesizeHammingWindow( std::vector<double> &_vTaps ) {
		const double dTau = 2.0 * std::numbers::pi;
		size_t stMax = _vTaps.size() - 1;
		double dInvMax = 1.0 / stMax;
		double dTauInvMax = dTau * dInvMax;
		for ( auto I = _vTaps.size(); I--; ) {
			_vTaps[I] = 0.54 - 0.46 * std::cos( dTauInvMax * I );
		}
	}

	/**
	 * Synthesizes a Hanning window.
	 *
	 * \param _vTaps The array of taps to fill.
	 */
	inline void CSincFilter::SynthesizeHanningWindow( std::vector<double> &_vTaps ) {
		const double dTau = 2.0 * std::numbers::pi;
		size_t stMax = _vTaps.size() - 1;
		double dInvMax = 1.0 / stMax;
		double dTauInvMax = dTau * dInvMax;
		for ( auto I = _vTaps.size(); I--; ) {
			_vTaps[I] = 0.50 - 0.50 * std::cos( dTauInvMax * I );
		}
	}

}	// namespace lsn

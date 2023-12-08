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
		 * Creates an LPF IR filter given the sampling rate, the cut-off frequency, and the bandwidth.
		 * 
		 * \param _dFs The sample frequency.
		 * \param _dFc The cut-off frequency.
		 * \param _dBw The bandwidth of the roll-off.
		 * \param _pfSynthFunc The synthesis function (IE SynthesizeBlackmanWindow or SynthesizeHammingWindow).
		 **/
		inline void					CreateLpf( double _dFs, double _dFc, double _dBw, PfWindowFunc _pfSynthFunc );

		/**
		 * Creates an LPF IR filter given the sampling rate, the cut-off frequency, and the bandwidth.
		 * 
		 * \param _dFs The sample frequency.
		 * \param _dFc The cut-off frequency.
		 * \param _stM The bandwidth of the roll-off.
		 * \param _pfSynthFunc The synthesis function (IE SynthesizeBlackmanWindow or SynthesizeHammingWindow).
		 **/
		void						CreateLpf( double _dFs, double _dFc, size_t _stM, PfWindowFunc _pfSynthFunc );

		/**
		 * Creates an HPF IR filter given the sampling rate, the cut-off frequency, and the bandwidth.
		 * 
		 * \param _dFs The sample frequency.
		 * \param _dFc The cut-off frequency.
		 * \param _dBw The bandwidth of the roll-off.
		 * \param _pfSynthFunc The synthesis function (IE SynthesizeBlackmanWindow or SynthesizeHammingWindow).
		 **/
		inline void					CreateHpf( double _dFs, double _dFc, double _dBw, PfWindowFunc _pfSynthFunc );

		/**
		 * Creates an HPF IR filter given the sampling rate, the cut-off frequency, and the bandwidth.
		 * 
		 * \param _dFs The sample frequency.
		 * \param _dFc The cut-off frequency.
		 * \param _stM The bandwidth of the roll-off.
		 * \param _pfSynthFunc The synthesis function (IE SynthesizeBlackmanWindow or SynthesizeHammingWindow).
		 **/
		void						CreateHpf( double _dFs, double _dFc, size_t _stM, PfWindowFunc _pfSynthFunc );

		/**
		 * Filters a single sample.  Call after calling CreateHpf() or CreateLpf().
		 * 
		 * \param _dSample The input sample.
		 * \return Returns the filtered sample.
		 **/
		double						Process( double _dSample ) const;

		/**
		 * Returns true if there are coefficients in the buffer, indicating that the filter has already been set up.
		 * 
		 * \return Returns true if the coefficient buffer is non-0 in size.
		 **/
		inline bool					HasCoefficients() const;

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
		mutable std::vector<double>	m_vRing;
		/** The current index into the ring buffer. */
		mutable size_t				m_stRing;
		/** The Fs at creation. */
		double						m_dFs;
		/** The Fc at creation. */
		double						m_dFc;
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
	 * Creates an LPF IR filter given the sampling rate, the cut-off frequency, and the bandwidth.
	 * 
	 * \param _dFs The sample frequency.
	 * \param _dFc The cut-off frequency.
	 * \param _dBw The bandwidth of the roll-off.
	 * \param _pfSynthFunc The synthesis function (IE SynthesizeBlackmanWindow or SynthesizeHammingWindow).
	 **/
	inline void CSincFilter::CreateLpf( double _dFs, double _dFc, double _dBw, PfWindowFunc _pfSynthFunc ) {
		CreateLpf( _dFs, _dFc, size_t( std::ceil( 4.0 / (_dBw / _dFs) ) ), _pfSynthFunc );
	}

	/**
	 * Creates an LPF IR filter given the sampling rate, the cut-off frequency, and the bandwidth.
	 * 
	 * \param _dFs The sample frequency.
	 * \param _dFc The cut-off frequency.
	 * \param _stM The bandwidth of the roll-off.
	 * \param _pfSynthFunc The synthesis function (IE SynthesizeBlackmanWindow or SynthesizeHammingWindow).
	 **/
	inline void CSincFilter::CreateLpf( double _dFs, double _dFc, size_t _stM, PfWindowFunc _pfSynthFunc ) {
		if ( !(_stM & 1) ) { ++_stM; }						// Must have an odd number of samples for symmetry.
		if ( m_dFs == _dFs && m_dFc == _dFc && m_vCeof.size() == _stM ) { return; }
		m_dFs = _dFs;
		m_dFc = _dFc;
		_dFc /= _dFs;										// Cut-off ratio.

		m_vCeof.resize( _stM );
		m_vRing.resize( _stM );
		m_stRing = 0;
		size_t stL = _stM / 2;								// The center sample is the latency.
		for ( auto I = m_vCeof.size(); I--; ) {
			m_vCeof[I] = 1.0;
			m_vRing[I] = 0.0;
		}
		// Create window.
		if ( _pfSynthFunc ) {
			_pfSynthFunc( m_vCeof );
		}


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
	 * Creates an HPF IR filter given the sampling rate, the cut-off frequency, and the bandwidth.
	 * 
	 * \param _dFs The sample frequency.
	 * \param _dFc The cut-off frequency.
	 * \param _dBw The bandwidth of the roll-off.
	 * \param _pfSynthFunc The synthesis function (IE SynthesizeBlackmanWindow or SynthesizeHammingWindow).
	 **/
	inline void CSincFilter::CreateHpf( double _dFs, double _dFc, double _dBw, PfWindowFunc _pfSynthFunc ) {
		CreateHpf( _dFs, _dFc, size_t( std::ceil( 4.0 / (_dBw / _dFs) ) ), _pfSynthFunc );
	}

	/**
	 * Creates an HPF IR filter given the sampling rate, the cut-off frequency, and the bandwidth.
	 * 
	 * \param _dFs The sample frequency.
	 * \param _dFc The cut-off frequency.
	 * \param _stM The bandwidth of the roll-off.
	 * \param _pfSynthFunc The synthesis function (IE SynthesizeBlackmanWindow or SynthesizeHammingWindow).
	 **/
	inline void CSincFilter::CreateHpf( double _dFs, double _dFc, size_t _stM, PfWindowFunc _pfSynthFunc ) {
		if ( !(_stM & 1) ) { ++_stM; }						// Must have an odd number of samples for symmetry.
		if ( m_dFs == _dFs && m_dFc == _dFc && m_vCeof.size() == _stM ) { return; }
		m_dFs = _dFs;
		m_dFc = _dFc;
		_dFc /= _dFs;										// Cut-off ratio.

		m_vCeof.resize( _stM );
		m_vRing.resize( _stM );
		m_stRing = 0;
		size_t stL = _stM / 2;								// The center sample is the latency.
		for ( auto I = m_vCeof.size(); I--; ) {
			m_vCeof[I] = 1.0;
			m_vRing[I] = 0.0;
		}
		// Create window.
		if ( _pfSynthFunc ) {
			_pfSynthFunc( m_vCeof );
		}


		// Apply sinc function.
		double dFc2 = 2.0 * _dFc;
		const double dTau = 2.0 * std::numbers::pi;
		int64_t i64SignedL = int64_t( stL );
		for ( auto I = m_vCeof.size(); I--; ) {
			int64_t N = int64_t( I ) - i64SignedL;
			if ( N ) {
				//m_vCeof[I] *= Sinc( double( N ), dFc2 );
				m_vCeof[I] *= -std::sin( dTau * _dFc * N ) / (std::numbers::pi * N);
			}
			else {
				//m_vCeof[I] *= dTau * _dFc;
				m_vCeof[I] *= 1.0 - dFc2;
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
	 * Filters a single sample.  Call after calling CreateHpf() or CreateLpf().
	 * 
	 * \param _dSample The input sample.
	 * \return Returns the filtered sample.
	 **/
	inline double CSincFilter::Process( double _dSample ) const {
		--m_stRing;
		if ( m_stRing >= m_vRing.size() ) {
			m_stRing += m_vRing.size();
		}
		m_vRing[m_stRing] = _dSample;
		double dSample = 0.0;
		for ( size_t I = m_vCeof.size(); I--; ) {
			dSample += m_vCeof[I] * m_vRing[(I+m_stRing)%m_vCeof.size()];
		}
		return dSample;
	}

	/**
	 * Returns true if there are coefficients in the buffer, indicating that the filter has already been set up.
	 * 
	 * \return Returns true if the coefficient buffer is non-0 in size.
	 **/
	inline bool CSincFilter::HasCoefficients() const {
		return m_vCeof.size() != 0;
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

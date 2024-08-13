/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A sinc filter.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNAudioFilterBase.h"
#include <vector>

namespace lsn {

	/**
	 * Class CSincFilter
	 * \brief A sinc filter.
	 *
	 * Description: A sinc filter.
	 */
	class CSincFilter : public CAudioFilterBase {
	public :
		inline CSincFilter();
		virtual ~CSincFilter();


		// == Functions.
		/**
		 * Creates an LPF IR filter given the sampling rate, the cut-off frequency, and the bandwidth.
		 * 
		 * \param _fFc The cut-off frequency.
		 * \param _fSampleRate The sample frequency.
		 * \param _stM The bandwidth of the roll-off.
		 * \param _pfSynthFunc The synthesis function (IE SynthesizeBlackmanWindow or SynthesizeHammingWindow).
		 **/
		virtual bool					CreateLpf( float _fFc, float _fSampleRate, size_t _stM, PfWindowFunc _pfSynthFunc );

		/**
		 * Creates an HPF IR filter given the sampling rate, the cut-off frequency, and the bandwidth.
		 * 
		 * \param _fFc The cut-off frequency.
		 * \param _fSampleRate The sample frequency.
		 * \param _stM The bandwidth of the roll-off.
		 * \param _pfSynthFunc The synthesis function (IE SynthesizeBlackmanWindow or SynthesizeHammingWindow).
		 **/
		virtual bool					CreateHpf( float _fFc, float _fSampleRate, size_t _stM, PfWindowFunc _pfSynthFunc );

		/**
		 * Filters a single sample.  Call after calling CreateHpf() or CreateLpf().
		 * 
		 * \param _dSample The input sample.
		 * \return Returns the filtered sample.
		 **/
		virtual double					Process( double _dSample ) const;

		/**
		 * Returns true if there are coefficients in the buffer, indicating that the filter has already been set up.
		 * 
		 * \return Returns true if the coefficient buffer is non-0 in size.
		 **/
		inline bool						HasCoefficients() const;

		/**
		 * Standard sinc() function.
		 * 
		 * \param _dX The operand.
		 * \return Returns sin(x) / x.
		 **/
		static inline double			Sinc( double _dX );


	protected :
		// == Members.
		/** The array of coefficients. */
		std::vector<double>				m_vCeof;
		/** The ring buffer of past samples. */
		mutable std::vector<double>		m_vRing;
		/** The current index into the ring buffer. */
		mutable size_t					m_stRing;
	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	inline CSincFilter::CSincFilter() :
		CAudioFilterBase(),
		m_stRing( 0 ) {
	}
	inline CSincFilter::~CSincFilter() {
	}

	// == Functions.
	/**
	 * Creates an LPF IR filter given the sampling rate, the cut-off frequency, and the bandwidth.
	 * 
	 * \param _fSampleRate The sample frequency.
	 * \param _fFc The cut-off frequency.
	 * \param _stM The bandwidth of the roll-off.
	 * \param _pfSynthFunc The synthesis function (IE SynthesizeBlackmanWindow or SynthesizeHammingWindow).
	 **/
	inline bool CSincFilter::CreateLpf( float _fFc, float _fSampleRate, size_t _stM, PfWindowFunc _pfSynthFunc ) {
		if ( !(_stM & 1) ) { ++_stM; }						// Must have an odd number of samples for symmetry.
		if ( m_fSampleRate == _fSampleRate && m_fFc == _fFc && m_vCeof.size() == _stM ) { return true; }
		m_fSampleRate = _fSampleRate;
		m_fFc = _fFc;
		double dFc = double( _fFc ) / _fSampleRate;			// Cut-off ratio.

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
		int64_t i64SignedL = int64_t( stL );
		for ( auto I = m_vCeof.size(); I--; ) {
			int64_t N = int64_t( I ) - i64SignedL;
			m_vCeof[I] *= Sinc( 2.0 * dFc * N );
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
		return true;
	}

	/**
	 * Creates an HPF IR filter given the sampling rate, the cut-off frequency, and the bandwidth.
	 * 
	 * \param _fSampleRate The sample frequency.
	 * \param _fFc The cut-off frequency.
	 * \param _stM The bandwidth of the roll-off.
	 * \param _pfSynthFunc The synthesis function (IE SynthesizeBlackmanWindow or SynthesizeHammingWindow).
	 **/
	inline bool CSincFilter::CreateHpf( float _fFc, float _fSampleRate, size_t _stM, PfWindowFunc _pfSynthFunc ) {
		if ( !(_stM & 1) ) { ++_stM; }						// Must have an odd number of samples for symmetry.
		if ( m_fSampleRate == _fSampleRate && m_fFc == _fFc && m_vCeof.size() == _stM ) { return true; }
		m_fSampleRate = _fSampleRate;
		m_fFc = _fFc;
		double dFc = double( _fFc ) / _fSampleRate;			// Cut-off ratio.

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
		double dFc2 = 2.0 * dFc;
		const double dTau = 2.0 * std::numbers::pi;
		int64_t i64SignedL = int64_t( stL );
		for ( auto I = m_vCeof.size(); I--; ) {
			int64_t N = int64_t( I ) - i64SignedL;
			if ( N ) {
				//m_vCeof[I] *= Sinc( double( N ), dFc2 );
				m_vCeof[I] *= -std::sin( dTau * dFc * N ) / (std::numbers::pi * N);
			}
			else {
				//m_vCeof[I] *= dTau * dFc;
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

		return true;
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
	 * Standard sinc() function.
	 * 
	 * \param _dX The operand.
	 * \return Returns sin(x) / x.
	 **/
	inline double CSincFilter::Sinc( double _dX ) {
		_dX *= std::numbers::pi;
		if ( _dX < 0.01 && _dX > -0.01 ) {
			return 1.0 + _dX * _dX * (-1.0 / 6.0 + _dX * _dX * 1.0 / 120.0);
		}

		return std::sin( _dX ) / _dX;
	}

}	// namespace lsn

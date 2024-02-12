/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for audio filters.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include <cmath>
#include <numbers>
#include <vector>

namespace lsn {

	/**
	 * Class CAudioFilterBase
	 * \brief An HPF filter.
	 *
	 * Description: The base class for audio filters.
	 */
	class CAudioFilterBase {
	public :
		inline CAudioFilterBase();
		virtual ~CAudioFilterBase() {
		}


		// == Types.
		/** The window function. */
		typedef void (*					PfWindowFunc)( std::vector<double> & );


		// == Functions.
		/**
		 * Sets the cut-off given a non-normalized cut-off frequency for an LPF and sample rate.
		 * 
		 * \param _fFc The non-normalized cut-off frequency.
		 * \param _fSampleRate The input/output sample rate.
		 **/
		virtual bool					CreateLpf( float /*_fFc*/, float /*_fSampleRate*/ ) { return false; }

		/**
		 * Sets the cut-off given a non-normalized cut-off frequency for an HPF and sample rate.
		 * 
		 * \param _fFc The non-normalized cut-off frequency.
		 * \param _fSampleRate The input/output sample rate.
		 **/
		virtual bool					CreateHpf( float /*_fFc*/, float /*_fSampleRate*/ ) { return false; }

		/**
		 * Processes a single sample.
		 * 
		 * \param _dSample The sample to process.
		 * \return Returns the filtered sample.
		 **/
		virtual double					Process( double /*_dSample*/ ) { return 0.0; }

		/**
		 * If either input value differs from m_fFc or m_fSampleRate, true is returned and m_fFc and m_fSampleRate are updated, otherwise false is returned.
		 * 
		 * \param _fFc The cut-off frequency to check.
		 * \param _fSampleRate The sample rate to check.
		 * \return Returns true if either m_fFc or m_fSampleRate differs from _fFc or _fSampleRate.
		 **/
		inline bool						Dirty( float _fFc, float _fSampleRate );

		/**
		 * Creates an LPF IR filter given the sampling rate, the cut-off frequency, and the bandwidth.
		 * 
		 * \param _fFc The cut-off frequency.
		 * \param _fSampleRate The sample frequency.
		 * \param _dBw The bandwidth of the roll-off.
		 * \param _pfSynthFunc The synthesis function (IE SynthesizeBlackmanWindow or SynthesizeHammingWindow).
		 **/
		virtual bool					CreateLpf( float _fFc, float _fSampleRate, double _dBw, PfWindowFunc _pfSynthFunc ) {
			return CreateLpf( _fFc, _fSampleRate, size_t( std::ceil( 4.0 / (_dBw / _fSampleRate) ) ), _pfSynthFunc );
		}

		/**
		 * Creates an LPF IR filter given the sampling rate, the cut-off frequency, and the bandwidth.
		 * 
		 * \param _fFc The cut-off frequency.
		 * \param _fSampleRate The sample frequency.
		 * \param _stM The bandwidth of the roll-off.
		 * \param _pfSynthFunc The synthesis function (IE SynthesizeBlackmanWindow or SynthesizeHammingWindow).
		 **/
		virtual bool					CreateLpf( float /*_fFc*/, float /*_fSampleRate*/, size_t /*_stM*/, PfWindowFunc /*_pfSynthFunc*/ ) { return false; }

		/**
		 * Creates an HPF IR filter given the sampling rate, the cut-off frequency, and the bandwidth.
		 * 
		 * \param _fFc The cut-off frequency.
		 * \param _fSampleRate The sample frequency.
		 * \param _dBw The bandwidth of the roll-off.
		 * \param _pfSynthFunc The synthesis function (IE SynthesizeBlackmanWindow or SynthesizeHammingWindow).
		 **/
		virtual bool					CreateHpf( float _fFc, float _fSampleRate, double _dBw, PfWindowFunc _pfSynthFunc ) {
			return CreateHpf( _fFc, _fSampleRate, size_t( std::ceil( 4.0 / (_dBw / _fSampleRate) ) ), _pfSynthFunc );
		}

		/**
		 * Creates an HPF IR filter given the sampling rate, the cut-off frequency, and the bandwidth.
		 * 
		 * \param _fFc The cut-off frequency.
		 * \param _fSampleRate The sample frequency.
		 * \param _stM The bandwidth of the roll-off.
		 * \param _pfSynthFunc The synthesis function (IE SynthesizeBlackmanWindow or SynthesizeHammingWindow).
		 **/
		virtual bool					CreateHpf( float /*_fFc*/, float /*_fSampleRate*/, size_t /*_stM*/, PfWindowFunc /*_pfSynthFunc*/ ) { return false; }

		/**
		 * Synthesizes a Blackman window.
		 *
		 * \param _vTaps The array of taps to fill.
		 */
		static inline void				SynthesizeBlackmanWindow( std::vector<double> &_vTaps );

		/**
		 * Synthesizes a Hamming window.
		 *
		 * \param _vTaps The array of taps to fill.
		 */
		static inline void				SynthesizeHammingWindow( std::vector<double> &_vTaps );

		/**
		 * Synthesizes a Hanning window.
		 *
		 * \param _vTaps The array of taps to fill.
		 */
		static inline void				SynthesizeHanningWindow( std::vector<double> &_vTaps );


	protected :
		// == Members.
		float							m_fFc;					/**< The original cut-off frequency. */
		float							m_fSampleRate;			/**< The original sample rate. */

	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	inline CAudioFilterBase::CAudioFilterBase() :
		m_fFc( 0.0f ),
		m_fSampleRate( 0.0f ) {
	}

	/**
	 * If either input value differs from m_fFc or m_fSampleRate, true is returned and m_fFc and m_fSampleRate are updated, otherwise false is returned.
	 * 
	 * \param _fFc The cut-off frequency to check.
	 * \param _fSampleRate The sample rate to check.
	 * \return Returns true if either m_fFc or m_fSampleRate differs from _fFc or _fSampleRate.
	 **/
	inline bool CAudioFilterBase::Dirty( float _fFc, float _fSampleRate ) {
		if ( (_fFc != m_fFc) || (_fSampleRate != m_fSampleRate) ) {
			m_fFc = _fFc;
			m_fSampleRate = _fSampleRate;
			return true;
		}
		return false;
	}

		/**
	 * Synthesizes a Blackman window.
	 *
	 * \param _vTaps The array of taps to fill.
	 */
	inline void CAudioFilterBase::SynthesizeBlackmanWindow( std::vector<double> &_vTaps ) {
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
	inline void CAudioFilterBase::SynthesizeHammingWindow( std::vector<double> &_vTaps ) {
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
	inline void CAudioFilterBase::SynthesizeHanningWindow( std::vector<double> &_vTaps ) {
		const double dTau = 2.0 * std::numbers::pi;
		size_t stMax = _vTaps.size() - 1;
		double dInvMax = 1.0 / stMax;
		double dTauInvMax = dTau * dInvMax;
		for ( auto I = _vTaps.size(); I--; ) {
			_vTaps[I] = 0.50 - 0.50 * std::cos( dTauInvMax * I );
		}
	}

}	// namespace lsn

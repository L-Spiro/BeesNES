/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A 1-pole filter.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNAudioFilterBase.h"

namespace lsn {

	/**
	 * Class CPoleFilter
	 * \brief A 1-pole filter.
	 *
	 * Description: A 1-pole filter.
	 */
	class CPoleFilter : public CAudioFilterBase {
	public :
		inline CPoleFilter();
		virtual ~CPoleFilter() {}


		// == Functions.
		/**
		 * Sets the cut-off given a non-normalized cut-off frequency for an LPF and sample rate.
		 * 
		 * \param _fFc The normalized cut-off frequency.
		 * \param _fSampleRate The input/output sample rate.
		 **/
		virtual bool				CreateLpf( float _fFc, float _fSampleRate ) {
			if LSN_UNLIKELY( Dirty( _fFc, _fSampleRate ) ) {
				m_dCornerFreq = ( 2.0 * std::numbers::pi * (double( _fFc ) / _fSampleRate) );
				m_dGain = 1.0 - m_dCornerFreq;
				m_dCornerFreq *= 0.5;
				//m_dGain *= 0.5;
				if ( _fFc >= _fSampleRate / 2.0f ) { m_bEnabled = false; }
			}
			return true;
		}

		/**
		 * Sets the cut-off given a non-normalized cut-off frequency for an HPF and sample rate.
		 * 
		 * \param _fFc The normalized cut-off frequency.
		 * \param _fSampleRate The input/output sample rate.
		 **/
		virtual bool				CreateHpf( float _fFc, float _fSampleRate ) {
			if LSN_UNLIKELY( Dirty( _fFc, _fSampleRate ) ) {
				m_dCornerFreq = -std::exp( -2.0 * std::numbers::pi * (0.5 - double( _fFc ) / _fSampleRate) );
				m_dGain = 1.0 + m_dCornerFreq;
				m_dCornerFreq *= 0.5;
				//m_dGain *= 0.5;
				if ( _fFc >= _fSampleRate / 2.0f ) { m_bEnabled = false; }
			}
			return true;
		}

		/**
		 * Processes a single sample.
		 * 
		 * \param _dSample The sample to process.
		 * \return Returns the filtered sample.
		 **/
		inline double				Process( double _dSample ) {
			if LSN_LIKELY( m_bEnabled ) {
				m_dLastSample = m_dLastSample * m_dGain + (_dSample + m_dLastInput) * m_dCornerFreq;
				//m_dLastSample = (_dSample + m_dLastInput) * m_dGain + m_dLastSample * m_dCornerFreq;
				m_dLastInput = _dSample;
				return m_dLastSample;
			}
			return _dSample;
		}

		/**
		 * Resets the sate of the LPF without resetting the parameters.
		 **/
		virtual void					ResetState() {
			m_dLastSample = m_dLastInput = 0.0;
		}


	protected :
		// == Members.
		/** The last output sample. */
		double						m_dLastSample;
		/** The last input. */
		double						m_dLastInput;
		/** The gain control (a0). */
		double						m_dGain;
		/** The corner frequency (b1). */
		double						m_dCornerFreq;

	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	inline CPoleFilter::CPoleFilter() :
		CAudioFilterBase(),
		m_dLastSample( 0.0 ),
		m_dLastInput( 0.0 ),
		m_dGain( 0.0 ),
		m_dCornerFreq( 0.0 ) {
	}

}	// namespace lsn

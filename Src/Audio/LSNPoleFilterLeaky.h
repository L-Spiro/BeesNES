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
	 * Class CPoleFilterLeaky
	 * \brief A 1-pole filter.
	 *
	 * Description: A 1-pole filter.
	 */
	class CPoleFilterLeaky : public CAudioFilterBase {
	public :
		inline CPoleFilterLeaky();
		virtual ~CPoleFilterLeaky() {}


		// == Functions.
		/**
		 * Sets the cut-off given a non-normalized cut-off frequency for an LPF and sample rate.
		 * 
		 * \param _fFc The normalized cut-off frequency.
		 **/
		virtual bool				CreateLpf( float _fFc, float _fSampleRate ) {
			if ( Dirty( _fFc, _fSampleRate ) ) {
				m_dCornerFreq = std::exp( -2.0 * 3.1415926535897932384626433832795 * (double( _fFc ) / _fSampleRate) );
				m_dGain = 1.0 - m_dCornerFreq;
			}
			return true;
		}

		/**
		 * Sets the cut-off given a non-normalized cut-off frequency for an HPF and sample rate.
		 * 
		 * \param _fFc The normalized cut-off frequency.
		 **/
		virtual bool				CreateHpf( float _fFc, float _fSampleRate ) {
			if ( Dirty( _fFc, _fSampleRate ) ) {
				m_dCornerFreq = -std::exp( -2.0 * 3.1415926535897932384626433832795 * (0.5 - double( _fFc ) / _fSampleRate) );
				m_dGain = 1.0 + m_dCornerFreq;
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
			return m_dLastSample = _dSample * m_dGain + m_dLastSample * m_dCornerFreq;
		}


	protected :
		// == Members.
		/** The last sample. */
		double						m_dLastSample;
		/** The gain control (a0). */
		double						m_dGain;
		/** The corner frequency (b1). */
		double						m_dCornerFreq;
	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	inline CPoleFilterLeaky::CPoleFilterLeaky() :
		CAudioFilterBase(),
		m_dLastSample( 0.0 ),
		m_dGain( 0.0 ),
		m_dCornerFreq( 0.0 ) {
	}

}	// namespace lsn

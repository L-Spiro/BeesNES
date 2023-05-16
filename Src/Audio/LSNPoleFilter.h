/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A 1-pole filter.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include <cmath>

namespace lsn {

	/**
	 * Class CPoleFilter
	 * \brief A 1-pole filter.
	 *
	 * Description: A 1-pole filter.
	 */
	class CPoleFilter {
	public :
		inline CPoleFilter();
		inline ~CPoleFilter();


		// == Functions.
		/**
		 * Sets the cut-off given a normalized cut-off frequency for an LPF.
		 * 
		 * \param _fFc The normalized cut-off frequency.
		 **/
		inline void					CreateLpf( float _fFc );

		/**
		 * Sets the cut-off given a normalized cut-off frequency for an HPF.
		 * 
		 * \param _fFc The normalized cut-off frequency.
		 **/
		inline void					CreateHpf( float _fFc );

		/**
		 * Processes a single sample.
		 * 
		 * \param _fSample The sample to process.
		 * \return Returns the filtered sample.
		 **/
		inline float				Process( float _fSample );


	protected :
		// == Members.
		/** The last sample. */
		float						m_fLastSample;
		/** The gain control (a0). */
		float						m_fGain;
		/** The corner frequency (b1). */
		float						m_fCornerFreq;
		/** The last cut-off frquency. */
		float						m_fLastCutoff;
	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	inline CPoleFilter::CPoleFilter() :
		m_fLastSample( 0.0f ),
		m_fGain( 0.0f ),
		m_fCornerFreq( 0.0f ),
		m_fLastCutoff( 0.0f ) {
	}
	inline CPoleFilter::~CPoleFilter() {
	}

	// == Functions.
	/**
	 * Sets the cut-off given a normalized cut-off frequency for an LPF.
	 * 
	 * \param _fFc The normalized cut-off frequency.
	 **/
	inline void CPoleFilter::CreateLpf( float _fFc ) {
		if ( m_fLastCutoff != _fFc ) {
			m_fCornerFreq = std::exp( -2.0f * 3.1415926535897932384626433832795f * _fFc );
			m_fGain = 1.0f - m_fCornerFreq;

			m_fLastCutoff = _fFc;
		}
	}

	/**
	 * Sets the cut-off given a normalized cut-off frequency for an HPF.
	 * 
	 * \param _fFc The normalized cut-off frequency.
	 **/
	inline void CPoleFilter::CreateHpf( float _fFc ) {
		if ( m_fLastCutoff != _fFc ) {
			m_fCornerFreq = -std::exp( -2.0f * 3.1415926535897932384626433832795f * (0.5f - _fFc) );
			m_fGain = 1.0f + m_fCornerFreq;

			m_fLastCutoff = _fFc;
		}
	}

	/**
	 * Processes a single sample.
	 * 
	 * \param _fSample The sample to process.
	 * \return Returns the filtered sample.
	 **/
	inline float CPoleFilter::Process( float _fSample ) {
		return m_fLastSample = _fSample * m_fGain + m_fLastSample * m_fCornerFreq;
	}

}	// namespace lsn

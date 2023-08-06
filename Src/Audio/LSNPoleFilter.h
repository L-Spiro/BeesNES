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
		 * \param _dSample The sample to process.
		 * \return Returns the filtered sample.
		 **/
		inline double				Process( double _dSample );


	protected :
		// == Members.
		/** The last sample. */
		double						m_dLastSample;
		/** The gain control (a0). */
		double						m_dGain;
		/** The corner frequency (b1). */
		double						m_dCornerFreq;
		/** The last cut-off frquency. */
		double						m_dLastCutoff;
	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	inline CPoleFilter::CPoleFilter() :
		m_dLastSample( 0.0 ),
		m_dGain( 0.0 ),
		m_dCornerFreq( 0.0 ),
		m_dLastCutoff( 0.0 ) {
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
		if ( m_dLastCutoff != _fFc ) {
			m_dCornerFreq = std::exp( -2.0 * 3.1415926535897932384626433832795 * _fFc );
			m_dGain = 1.0f - m_dCornerFreq;

			m_dLastCutoff = _fFc;
		}
	}

	/**
	 * Sets the cut-off given a normalized cut-off frequency for an HPF.
	 * 
	 * \param _fFc The normalized cut-off frequency.
	 **/
	inline void CPoleFilter::CreateHpf( float _fFc ) {
		if ( m_dLastCutoff != _fFc ) {
			m_dCornerFreq = -std::exp( -2.0 * 3.1415926535897932384626433832795 * (0.5 - _fFc) );
			m_dGain = 1.0f + m_dCornerFreq;

			m_dLastCutoff = _fFc;
		}
	}

	/**
	 * Processes a single sample.
	 * 
	 * \param _dSample The sample to process.
	 * \return Returns the filtered sample.
	 **/
	inline double CPoleFilter::Process( double _dSample ) {
		return m_dLastSample = _dSample * m_dGain + m_dLastSample * m_dCornerFreq;
	}

}	// namespace lsn

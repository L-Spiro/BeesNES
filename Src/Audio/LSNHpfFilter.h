/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: An HPF filter.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include <cmath>

namespace lsn {

	/**
	 * Class CHpfFilter
	 * \brief An HPF filter.
	 *
	 * Description: An HPF filter.
	 */
	class CHpfFilter {
	public :
		inline CHpfFilter();
		inline ~CHpfFilter();


		// == Functions.
		/**
		 * Sets the cut-off given a non-normalized cut-off frequency for an HPF and sample rate.
		 * 
		 * \param _fFc The non-normalized cut-off frequency.
		 * \param _fSampleRate The input/output sample rate.
		 **/
		inline void					CreateHpf( float _fFc, float _fSampleRate );

		/**
		 * Processes a single sample.
		 * 
		 * \param _dSample The sample to process.
		 * \return Returns the filtered sample.
		 **/
		inline double				Process( double _dSample );


	protected :
		// == Members.
		double						m_dAlpha;
		double						m_dPreviousOutput;
		double						m_dPrevInput;
		double						m_dDelta;
		double						m_dOutput;				/**< The current filtered output sample. */
		float						m_fFc;					/**< The original cut-off frequency. */
		float						m_fSampleRate;			/**< The original sample rate. */
	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	inline CHpfFilter::CHpfFilter() :
		m_dAlpha( 0.0 ),
		m_dPreviousOutput( 0.0 ),
		m_dPrevInput( 0.0 ),
		m_dDelta( 0.0 ),
		m_dOutput( 0.0 ),
		m_fFc( 0.0f ),
		m_fSampleRate( 0.0f ) {
	}
	inline CHpfFilter::~CHpfFilter() {
	}

	// == Functions.
	/**
	 * Sets the cut-off given a non-normalized cut-off frequency for an HPF and sample rate.
	 * 
	 * \param _fFc The non-normalized cut-off frequency.
	 * \param _fSampleRate The input/output sample rate.
	 **/
	inline void CHpfFilter::CreateHpf( float _fFc, float _fSampleRate ) {
		if ( m_fFc != _fFc || m_fSampleRate != _fSampleRate ) {
			double dDelta = (_fSampleRate != 0.0f) ? (1.0 / _fSampleRate) : 0.0;
			double dTimeConstant = (_fFc != 0.0f) ? (1.0 / _fFc) : 0.0;
			m_dAlpha = ((dTimeConstant + dDelta) != 0.0) ? (dTimeConstant / (dTimeConstant + dDelta)) : 0.0;
			m_dOutput = 0.0;
			m_dPreviousOutput = m_dOutput;
			m_dPrevInput = 0.0;
			m_dDelta = 0.0;

			m_fFc = _fFc;
			m_fSampleRate = _fSampleRate;
		}
	}

	/**
	 * Processes a single sample.
	 * 
	 * \param _dSample The sample to process.
	 * \return Returns the filtered sample.
	 **/
	inline double CHpfFilter::Process( double _dSample ) {
		m_dPreviousOutput = m_dOutput;
        m_dDelta = _dSample - m_dPrevInput;
        m_dPrevInput = _dSample;

		m_dOutput = m_dAlpha * m_dPreviousOutput + m_dAlpha * m_dDelta;
		return m_dOutput;
	}

}	// namespace lsn

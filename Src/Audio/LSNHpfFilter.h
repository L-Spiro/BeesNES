/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: An HPF filter.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNAudioFilterBase.h"

namespace lsn {

	/**
	 * Class CHpfFilter
	 * \brief An HPF filter.
	 *
	 * Description: An HPF filter.
	 */
	class CHpfFilter : public CAudioFilterBase {
	public :
		inline CHpfFilter();
		virtual ~CHpfFilter() {}


		// == Functions.
		/**
		 * Sets the cut-off given a non-normalized cut-off frequency for an HPF and sample rate.
		 * 
		 * \param _fFc The non-normalized cut-off frequency.
		 * \param _fSampleRate The input/output sample rate.
		 **/
		virtual bool				CreateHpf( float _fFc, float _fSampleRate ) {
			if LSN_UNLIKELY( Dirty( _fFc, _fSampleRate ) ) {
				double dDelta = (_fSampleRate != 0.0f) ? (1.0 / _fSampleRate) : 0.0;
				double dTimeConstant = (_fFc != 0.0f) ? (1.0 / _fFc) : 0.0;
				m_dAlpha = ((dTimeConstant + dDelta) != 0.0) ? (dTimeConstant / (dTimeConstant + dDelta)) : 0.0;
				m_dOutput = 0.0;
				m_dPreviousOutput = m_dOutput;
				m_dPrevInput = 0.0;
				m_dDelta = 0.0;
			}
			return true;
		}

		/**
		 * Processes a single sample.
		 * 
		 * \param _dSample The sample to process.
		 * \return Returns the filtered sample.
		 **/
		virtual double				Process( double _dSample ) {
			m_dPreviousOutput = m_dOutput;
			m_dDelta = _dSample - m_dPrevInput;
			m_dPrevInput = _dSample;

			m_dOutput = m_dAlpha * m_dPreviousOutput + m_dAlpha * m_dDelta;
			return m_dOutput;
		}


	protected :
		// == Members.
		double						m_dAlpha;				/**< Alpha. */
		double						m_dPreviousOutput;		/**< The previous output sample. */
		double						m_dPrevInput;			/**< The previous input sample. */
		double						m_dDelta;				/**< Delta. */
		double						m_dOutput;				/**< The current filtered output sample. */
	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	inline CHpfFilter::CHpfFilter() :
		CAudioFilterBase(),
		m_dAlpha( 0.0 ),
		m_dPreviousOutput( 0.0 ),
		m_dPrevInput( 0.0 ),
		m_dDelta( 0.0 ),
		m_dOutput( 0.0 ) {
	}

	// == Functions.

}	// namespace lsn

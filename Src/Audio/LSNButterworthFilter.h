/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Butterworth filter.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNAudioFilterBase.h"
#include "LSNButterworthFilterImpl.h"

namespace lsn {

	/**
	 * Class CButterworthFilter
	 * \brief A Butterworth filter.
	 *
	 * Description: A Butterworth filter.
	 */
	template <unsigned _uFilters>
	class CButterworthFilter : public CAudioFilterBase {
	public :
		inline CButterworthFilter() :
			m_dGain( 1.0 ) {
		}
		virtual ~CButterworthFilter() {}


		// == Functions.
		/**
		 * Sets the cut-off given a non-normalized cut-off frequency for an LPF and sample rate.
		 * 
		 * \param _fFc The normalized cut-off frequency.
		 * \param _fSampleRate The input/output sample rate.
		 **/
		virtual bool								CreateLpf( float _fFc, float _fSampleRate ) {
			if ( Dirty( _fFc, _fSampleRate ) ) {
				CButterworthFilterImpl bfiButterworth;
				bfiButterworth.LoPass( _fSampleRate, _fFc, _uFilters * 2, m_vCoefs, m_dGain );
				m_bqfcChain.SetOrder();
				m_bqfcChain.MakeDefault();
				m_pbqfFilter = m_vCoefs.data();
			}
			return true;
		}

		/**
		 * Sets the cut-off given a non-normalized cut-off frequency for an HPF and sample rate.
		 * 
		 * \param _fFc The non-normalized cut-off frequency.
		 * \param _fSampleRate The input/output sample rate.
		 **/
		virtual bool								CreateHpf( float _fFc, float _fSampleRate ) {
			if ( Dirty( _fFc, _fSampleRate ) ) {
				CButterworthFilterImpl bfiButterworth;
				bfiButterworth.HiPass( _fSampleRate, _fFc, _uFilters * 2, m_vCoefs, m_dGain );
				m_bqfcChain.SetOrder();
				m_bqfcChain.MakeDefault();
				m_pbqfFilter = m_vCoefs.data();
			}
			return true;
		}

		/**
		 * Processes a single sample.
		 * 
		 * \param _dSample The sample to process.
		 * \return Returns the filtered sample.
		 **/
		inline double								Process( double _dSample ) {
			return m_bqfcChain.ProcessBiQuad( _dSample, m_pbqfFilter ) * m_dGain;
		}


	protected :
		// == Members.
		CBiQuadFilterChain<_uFilters>				m_bqfcChain;						/**< The bi-quad chain. */
		std::vector<CBiQuadFilter>					m_vCoefs;							/**< The bi-quad coefficients. */
		double										m_dGain;							/**< The post-filter gain. */
		CBiQuadFilter *								m_pbqfFilter;						/**< Quick access to the coefficients. */

	};

}	// namespace lsn

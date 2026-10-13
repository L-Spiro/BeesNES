/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A 1-pole, 1-zero low-pass (first-order high shelf).  Models an RC low-pass whose capacitor has a small series resistance.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNAudioFilterBase.h"

#include <cmath>
#include <numbers>

namespace lsn {

	/**
	 * Class CPoleZeroFilter
	 * \brief A 1-pole, 1-zero low-pass filter.
	 *
	 * Description: A 1-pole, 1-zero low-pass filter, H(s) = (1 + s / Wz) / (1 + s / Wp), discretized with the bilinear transform with each corner
	 *	pre-warped so that the pole and zero land exactly on the requested frequencies.  DC gain is 1; the gain at Nyquist approaches Fp / Fz.
	 *	With the zero disabled (Fz <= 0 or Fz >= Nyquist) it becomes a pre-warped bilinear 1-pole low-pass.
	 *
	 *	Note: for the same response this filter's pole value differs from CPoleFilter's cut-off (CPoleFilter's w = 2 * pi * Fc / Rate is not
	 *	pre-warped).  Use values measured for this class.
	 */
	class CPoleZeroFilter : public CAudioFilterBase {
	public :
		inline CPoleZeroFilter();
		virtual ~CPoleZeroFilter() {}


		// == Functions.
		/**
		 * Sets the pole only (the zero is disabled), making this a pre-warped bilinear 1-pole low-pass.
		 *
		 * \param _fFc The non-normalized cut-off (pole) frequency.
		 * \param _fSampleRate The input/output sample rate.
		 * \return Returns true.
		 **/
		virtual bool				CreateLpf( float _fFc, float _fSampleRate ) {
			return CreatePoleZero( double( _fFc ), 0.0, double( _fSampleRate ) );
		}

		/**
		 * Sets the pole and zero frequencies.  Coefficients are only recomputed when a parameter changes.
		 *
		 * \param _dPoleHz The pole (low-pass corner) frequency in Hz.
		 * \param _dZeroHz The zero frequency in Hz.  0 (or any value at or above Nyquist) disables the zero.
		 * \param _dSampleRate The input/output sample rate.
		 * \return Returns true.
		 **/
		bool						CreatePoleZero( double _dPoleHz, double _dZeroHz, double _dSampleRate ) {
			if LSN_UNLIKELY( _dPoleHz != m_dPoleHz || _dZeroHz != m_dZeroHz || _dSampleRate != m_dSampleRate ) {
				m_dPoleHz = _dPoleHz;
				m_dZeroHz = _dZeroHz;
				m_dSampleRate = _dSampleRate;

				const double dNyquist = _dSampleRate * 0.5;
				if ( _dPoleHz <= 0.0 || _dPoleHz >= dNyquist ) {
					m_bEnabled = false;
					return true;
				}
				m_bEnabled = true;

				// Pre-warped bilinear transform of (1 + s / Wz) / (1 + s / Wp): s / W -> (1 - z^-1) / (K * (1 + z^-1)), K = tan( pi * F / Rate ).
				const double dInvKp = 1.0 / std::tan( std::numbers::pi * _dPoleHz / _dSampleRate );
				const double dInvKz = (_dZeroHz > 0.0 && _dZeroHz < dNyquist) ? (1.0 / std::tan( std::numbers::pi * _dZeroHz / _dSampleRate )) : 0.0;
				const double dA0 = 1.0 + dInvKp;
				m_dB0 = (1.0 + dInvKz) / dA0;
				m_dB1 = (1.0 - dInvKz) / dA0;
				m_dA1 = (1.0 - dInvKp) / dA0;
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
				m_dLastSample = m_dB0 * _dSample + m_dB1 * m_dLastInput - m_dA1 * m_dLastSample;
				m_dLastInput = _dSample;
				return m_dLastSample;
			}
			return _dSample;
		}

		/**
		 * Resets the state of the filter without resetting the parameters.
		 **/
		virtual void				ResetState() {
			m_dLastSample = m_dLastInput = 0.0;
		}


	protected :
		// == Members.
		/** The last output sample. */
		double						m_dLastSample;
		/** The last input. */
		double						m_dLastInput;
		/** Numerator coefficient for the current input (normalized by a0). */
		double						m_dB0;
		/** Numerator coefficient for the previous input (normalized by a0). */
		double						m_dB1;
		/** Denominator coefficient for the previous output (normalized by a0). */
		double						m_dA1;
		/** The pole frequency last passed to CreatePoleZero(). */
		double						m_dPoleHz;
		/** The zero frequency last passed to CreatePoleZero(). */
		double						m_dZeroHz;
		/** The sample rate last passed to CreatePoleZero(). */
		double						m_dSampleRate;
	};



	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	inline CPoleZeroFilter::CPoleZeroFilter() :
		CAudioFilterBase(),
		m_dLastSample( 0.0 ),
		m_dLastInput( 0.0 ),
		m_dB0( 1.0 ),
		m_dB1( 0.0 ),
		m_dA1( 0.0 ),
		m_dPoleHz( -1.0 ),
		m_dZeroHz( -1.0 ),
		m_dSampleRate( -1.0 ) {
	}

}	// namespace lsn

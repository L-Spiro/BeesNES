/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A sample box.  Samples in, band-passed samples out.  This class strives to produce a super clean down-sampled result as fast as possible.
 * The process (assumes the input is more than 3 times the sample rate of the output):
 *	#1: Down-sample to 3 times the output rate.  Uses an interpolation function.  If the input rate is signficantly higher than the output rate, the
 *		interpolator is one designed for over-sampling, which will help to reduce the high frequencies in the source signal for less aliasing.
 *	#2: Applies a sinc-filter at the desired LPF cut-off frequency.  Ideally it should be at min( 20000.0 Hz, final.Niquist Hz ).
 *	#3: Down-samples to the final output rate using nearest-neighbor, since the intermediate rate is exactly 3 times the final rate.
 *	#4: HPF's at the final Hz.
 */

#pragma once

#include <vector>

namespace lsn {

	// == Enumerations.
	/** SIMD types. */
	enum LSN_SIMD {
		LSN_S_SSE4,
		LSN_S_AVX,
	};

	/**
	 * Class CSincFilter
	 * \brief A sample box.  Samples in, band-passed samples out.
	 *
	 * Description: A sample box.  Samples in, band-passed samples out.  This class strives to produce a super clean down-sampled result as fast as possible.
	 * The process (assumes the input is more than 3 times the sample rate of the output):
	 *	#1: Down-sample to 3 times the output rate.  Uses an interpolation function.  If the input rate is signficantly higher than the output rate, the
	 *		interpolator is one designed for over-sampling, which will help to reduce the high frequencies in the source signal for less aliasing.
	 *	#2: Applies a sinc-filter at the desired LPF cut-off frequency.  Ideally it should be at min( 20000.0 Hz, final.Niquist Hz ).
	 *	#3: Down-samples to the final output rate using nearest-neighbor, since the intermediate rate is exactly 3 times the final rate.
	 *	#4: HPF's at the final Hz.
	 */
	template <unsigned _uSimdType>
	class CSampleBox {
	public :
		inline CSampleBox() :
			m_fLpf( 0.0f ),
			m_fHpf( 0.0f ),
			m_fInputHz( 0.0f ),
			m_ui32OutputHz( 0 ),
			m_sBufferIdx( 0 ),
			m_sRing( 0 ),
			m_fAlpha( 0.0f ),
			m_fPreviousOutput( 0.0f ),
			m_fPrevInput( 0.0f ),
			m_fDelta( 0.0f ),
			m_fOutput( 0.0f ) {
		}
		virtual ~CSampleBox() {
		}


		// == Functions.
		/**
		 * Initializes the sample box, preparing to deliver nice clean band-limited samples as fast as alien technology can deliver.
		 * 
		 * \param _dLpf The LPF frequency.
		 * \param _dHpf The HPF frequency.
		 * \param _sM The LPF bandwidth.  Higher numbers result in sharper cut-offs.
		 * \param _dInputRate The input sample rate.
		 * \param _ui32OutputRate The output sample rate.
		 * \return Returns true if the internal buffers could be allocated.
		 **/
		bool												Init( double _dLpf, double _dHpf, size_t _sM, double _dInputRate, uint32_t _ui32OutputRate ) {
			if ( !(_sM & 1) ) { ++_sM; }						// Must have an odd number of samples for symmetry.
			if ( m_fInputHz == float( _dInputRate ) &&
				m_fLpf == float( _dLpf ) &&
				m_fHpf == float( _dHpf ) &&
				m_ui32OutputHz == _ui32OutputRate &&
				m_vCeof.size() == _sM ) { return true; }
			
			double dFc = _dLpf / _dInputRate;					// Cut-off ratio.

			try {
				m_vCeof.resize( _sM );
				m_vRing.resize( _sM );
			}
			catch ( ... ) { return false; }
			m_sRing = 0;

			size_t sL = _sM / 2;								// The center sample is the latency.
			for ( auto I = m_vCeof.size(); I--; ) {
				m_vCeof[I] = 1.0f;
			}
			for ( auto I = m_vRing.size(); I--; ) {
				m_vRing[I] = 0.0f;
			}

			SynthesizeBlackmanWindow( m_vCeof );

			// Apply sinc function.
			double dFc2 = 2.0 * dFc;
			const double dTau = 2.0 * std::numbers::pi;
			int64_t i64SignedL = int64_t( sL );
			for ( auto I = m_vCeof.size(); I--; ) {
				int64_t N = int64_t( I ) - i64SignedL;
				if ( N ) {
					m_vCeof[I] = float( m_vCeof[I] * std::sin( dTau * dFc * N ) / (std::numbers::pi * N) );
				}
				else {
					m_vCeof[I] = float( m_vCeof[I] * dFc2 );
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
					m_vCeof[I] = float( m_vCeof[I] * dNorm );
				}
			}


			// Prepare the HPF.
			double dDelta = (_ui32OutputRate != 0) ? (1.0 / (_ui32OutputRate * 3)) : 0.0;
			double dTimeConstant = (_dHpf != 0.0f) ? (1.0 / _dHpf) : 0.0;
			m_fAlpha = float( ((dTimeConstant + dDelta) != 0.0) ? (dTimeConstant / (dTimeConstant + dDelta)) : 0.0 );
			m_fOutput = 0.0f;
			m_fPreviousOutput = m_fOutput;
			m_fPrevInput = 0.0f;
			m_fDelta = 0.0f;


			m_fInputHz = float( _dInputRate );
			m_fLpf = float( _dLpf );
			m_fHpf = float( _dHpf );
			m_ui32OutputHz = _ui32OutputRate;
			return true;
		}

		/**
		 * Processes a single sample.
		 * 
		 * \param _fSample The sample to process.
		 * \return Returns the filtered sample.
		 **/
		inline float										ProcessHpf( float _fSample ) {
			m_fPreviousOutput = m_fOutput;
			m_fDelta = _fSample - m_fPrevInput;
			m_fPrevInput = _fSample;

			m_fOutput = m_fAlpha * m_fPreviousOutput + m_fAlpha * m_fDelta;
			return m_fOutput;
		}

		/**
		 * Returns true if there are coefficients in the buffer, indicating that the filter has already been set up.
		 * 
		 * \return Returns true if the coefficient buffer is non-0 in size.
		 **/
		inline bool											HasCoefficients() const { return m_vCeof.size() != 0; }

		/**
		 * Calculates the _sM parameter for Init() given a transition bandwidth (the range of frequencies to transition from max volume to silence).
		 * 
		 * \param _dBw The frequency range to transition to silence.
		 * \param _dSourceHz The frequency of the source samples.
		 * \return Returns the _sM parameter for use with Init().
		 **/
		inline size_t										TransitionRangeToBandwidth( double _dBw, double _dSourceHz ) {
			_dBw /= _dSourceHz;										// Bandwidth ratio.
			size_t stM = size_t( std::ceil( 4.0 / _dBw ) );
			if ( !(stM & 1) ) { return stM + 1; }
			return stM;
		}

		/**
		 * Synthesizes a Blackman window.
		 *
		 * \param _vTaps The array of taps to fill.
		 */
		static inline void									SynthesizeBlackmanWindow( std::vector<float> &_vTaps ) {
			const double dTau = 2.0 * std::numbers::pi;
			size_t stMax = _vTaps.size() - 1;
			double dInvMax = 1.0 / stMax;
			double dTauInvMax = dTau * dInvMax;
			double dTauInvMax2 = 2.0 * dTauInvMax;
			for ( auto I = _vTaps.size(); I--; ) {
				_vTaps[I] = float( 0.42 - 0.5 * std::cos( dTauInvMax * I ) + 0.08 * std::cos( dTauInvMax2 * I ) );
			}
		}

		/**
		 * Synthesizes a Hamming window.
		 *
		 * \param _vTaps The array of taps to fill.
		 */
		static inline void									SynthesizeHammingWindow( std::vector<float> &_vTaps ) {
			const double dTau = 2.0 * std::numbers::pi;
			size_t stMax = _vTaps.size() - 1;
			double dInvMax = 1.0 / stMax;
			double dTauInvMax = dTau * dInvMax;
			for ( auto I = _vTaps.size(); I--; ) {
				_vTaps[I] = float( 0.54 - 0.46 * std::cos( dTauInvMax * I ) );
			}
		}

		/**
		 * Synthesizes a Hanning window.
		 *
		 * \param _vTaps The array of taps to fill.
		 */
		static inline void									SynthesizeHanningWindow( std::vector<float> &_vTaps ) {
			const double dTau = 2.0 * std::numbers::pi;
			size_t stMax = _vTaps.size() - 1;
			double dInvMax = 1.0 / stMax;
			double dTauInvMax = dTau * dInvMax;
			for ( auto I = _vTaps.size(); I--; ) {
				_vTaps[I] = float( 0.50 - 0.50 * std::cos( dTauInvMax * I ) );
			}
		}



	protected :
		// == Members.
		// General.
		float												m_fLpf;										/**< The LPF frequency. */
		float												m_fHpf;										/**< The HPF frequency. */
		float												m_fInputHz;									/**< The source frequency. */
		uint32_t											m_ui32OutputHz;								/**< The output frequency. */
		size_t												m_sBufferIdx;								/**< Index into m_vBuffer of the next sample. */
		std::vector<float>									m_vBuffer;									/**< Ring buffer of the last few input samples. */
		// Sinc filter (LPF).
		std::vector<float>									m_vCeof;									/**< The array of coefficients. */
		mutable std::vector<float>							m_vRing;									/**< The ring buffer of past samples. */
		mutable size_t										m_sRing;									/**< The current index into the ring buffer. */
		// HPF.
		float												m_fAlpha;									/**< Alpha. */
		float												m_fPreviousOutput;							/**< The previous output sample. */
		float												m_fPrevInput;								/**< The previous input sample. */
		float												m_fDelta;									/**< Delta. */
		float												m_fOutput;									/**< The current filtered output sample. */
	};

}	// namespace lsn

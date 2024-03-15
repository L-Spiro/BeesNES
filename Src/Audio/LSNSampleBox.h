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

#include <cmath>
#include <cstdint>
#include <immintrin.h>
#include <smmintrin.h>
#include <vector>

#if defined( _MSC_VER )
    // Microsoft Visual Studio Compiler
    #define LSN_ALN		__declspec( align( 32 ) )
#elif defined( __GNUC__ ) || defined( __clang__ )
    // GNU Compiler Collection (GCC) or Clang
    #define LSN_ALN		__attribute__( (aligned( 32 )) )
#else
    #error "Unsupported compiler"
#endif

namespace lsn {

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
	class CSampleBox {
	public :
		inline CSampleBox() {
			m_gGen.ui64SrcSampleCnt = 0ULL;
			m_gGen.ui64SampleCnt = 0ULL;
			m_gGen.ui64SamplesBuffered = 0ULL;
			m_gGen.fLpf = 0.0f;
			m_gGen.fHpf = 0.0f;
			m_gGen.dInputHz = 0.0;
			m_gGen.ui32OutputHz = 0;
			m_gGen.pfStoreSample = &CSampleBox::StoreSample;
			m_gGen.pfConvolve = &CSampleBox::Convolve;

			m_sSinc.sM = 0;

			m_hHpf.fAlpha = 0.0f;
			m_hHpf.fPreviousOutput = 0.0f;
			m_hHpf.fPrevInput = 0.0f;
			m_hHpf.fDelta = 0.0f;
			m_hHpf.fOutput = 0.0f;

			m_pPoints.sSimdStackSize = 0;

		}
		virtual ~CSampleBox() {
		}


		/** A function pointer for the functions that stores a sample from the full-sized buffer to the intermediate buffer. */
		typedef void (CSampleBox:: *						PfStoreSample)( size_t, float );

		/** A function pointer for the functions that convolves a sample (sinc filter). */
		typedef float (CSampleBox:: *						PfConvolve)( size_t );


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
			_dLpf = std::min( _dLpf, _ui32OutputRate / 2.0 - (_ui32OutputRate / 100.0) );
			// Round up to the nearest 8.
			_sM = (_sM + 7) / 8 * 8;
			// Subtract 1 to make it odd.
			--_sM;
			if ( m_gGen.dInputHz == _dInputRate &&
				m_gGen.fLpf == float( _dLpf ) &&
				m_gGen.fHpf == float( _dHpf ) &&
				m_gGen.ui32OutputHz == _ui32OutputRate &&
				m_sSinc.vCeof.size() - 1 == _sM ) { return true; }
			
			double dFc = _dLpf / (_ui32OutputRate * 3.0);	// Cut-off ratio.  The sinc is applied to the buffer that is (_ui32OutputRate * 3).

			try {
				// + 1 makes it a multiple of 8.
				m_sSinc.vCeof.resize( _sM + 1 );
				m_sSinc.vRing.resize( (_sM + 1) * 10 );
				m_gGen.vBuffer.resize( size_t( ::ceil( _dInputRate / (_ui32OutputRate * 3) ) + 6 ) );
				m_gGen.vOutputBuffer.clear();
			}
			catch ( ... ) { return false; }
			
			size_t sL = _sM / 2;							// The center sample is the latency.
			for ( auto I = m_sSinc.vCeof.size(); I--; ) {
				m_sSinc.vCeof[I] = 1.0f;
			}
			for ( auto I = m_sSinc.vRing.size(); I--; ) {
				m_sSinc.vRing[I] = 0.0f;
			}

			SynthesizeBlackmanWindow( m_sSinc.vCeof, _sM );
			

			// Apply sinc function.
			double dFc2 = 2.0 * dFc;
			const double dTau = 2.0 * std::numbers::pi;
			int64_t i64SignedL = int64_t( sL );
			for ( auto I = m_sSinc.vCeof.size(); I--; ) {
				int64_t N = int64_t( I ) - i64SignedL;
				if ( N ) {
					m_sSinc.vCeof[I] = float( m_sSinc.vCeof[I] * std::sin( dTau * dFc * N ) / (std::numbers::pi * N) );
				}
				else {
					m_sSinc.vCeof[I] = float( m_sSinc.vCeof[I] * dFc2 );
				}
			}

			// Normalize.
			{
				double dSum = 0.0;
				for ( auto I = m_sSinc.vCeof.size(); I--; ) {
					dSum += m_sSinc.vCeof[I];
				}
				double dNorm = 1.0 / dSum;
				for ( auto I = m_sSinc.vCeof.size(); I--; ) {
					m_sSinc.vCeof[I] = float( m_sSinc.vCeof[I] * dNorm );
				}
			}


			// Prepare the HPF.
			double dDelta = (_ui32OutputRate != 0) ? (1.0 / _ui32OutputRate) : 0.0;
			double dTimeConstant = (_dHpf != 0.0) ? (1.0 / _dHpf) : 0.0;
			m_hHpf.fAlpha = float( ((dTimeConstant + dDelta) != 0.0) ? (dTimeConstant / (dTimeConstant + dDelta)) : 0.0 );
			m_hHpf.fOutput = 0.0f;
			m_hHpf.fPreviousOutput = m_hHpf.fOutput;
			m_hHpf.fPrevInput = 0.0f;
			m_hHpf.fDelta = 0.0f;

			m_gGen.ui64SrcSampleCnt = 0ULL;
			m_gGen.ui64SampleCnt = 0ULL;
			m_gGen.ui64SamplesBuffered = 0ULL;
			m_gGen.dInputHz = _dInputRate;
			m_gGen.fLpf = float( _dLpf );
			m_gGen.fHpf = float( _dHpf );
			m_gGen.ui32OutputHz = _ui32OutputRate;

			m_sSinc.sM = _sM >> 1;

			m_pPoints.sSimdStackSize = 0;
			return true;
		}

		/**
		 * Adds a sample. Called at the input Hz.
		 * 
		 * \param _fSample The sample to add.
		 **/
		inline void											AddSample( float _fSample ) {
			m_gGen.vBuffer[(m_gGen.ui64SrcSampleCnt++)%m_gGen.vBuffer.size()] = _fSample;
			if ( m_gGen.ui64SrcSampleCnt >= 4 ) {
				// How many samples should we have processed until now?
				uint64_t ui64SamplesUntilNow = uint64_t( (m_gGen.ui64SrcSampleCnt - 4.0) * (m_gGen.ui32OutputHz * 3.0) / m_gGen.dInputHz );
				// Process as many as needed to catch up to where we should be.
				while ( m_gGen.ui64SamplesBuffered <= ui64SamplesUntilNow ) {
					double dIdx = m_gGen.ui64SamplesBuffered / (m_gGen.ui32OutputHz * 3.0) * m_gGen.dInputHz;
					double dFrac = std::fmod( dIdx, 1.0 );
					size_t sIdx = size_t( dIdx );
					(this->*m_gGen.pfStoreSample)( sIdx, float( dFrac ) );
				}
			}
		}

		/**
		 * Processes a single sample.
		 * 
		 * \param _fSample The sample to process.
		 * \return Returns the filtered sample.
		 **/
		inline float										ProcessHpf( float _fSample ) {
			m_hHpf.fPreviousOutput = m_hHpf.fOutput;
			m_hHpf.fDelta = _fSample - m_hHpf.fPrevInput;
			m_hHpf.fPrevInput = _fSample;

			m_hHpf.fOutput = m_hHpf.fAlpha * m_hHpf.fPreviousOutput + m_hHpf.fAlpha * m_hHpf.fDelta;
			return m_hHpf.fOutput;
		}

		/**
		 * Returns true if there are coefficients in the buffer, indicating that the filter has already been set up.
		 * 
		 * \return Returns true if the coefficient buffer is non-0 in size.
		 **/
		inline bool											HasCoefficients() const { return m_sSinc.vCeof.size() != 0; }

		/**
		 * Gets the output sample buffer, which should be cleared when teh samples are consumed.
		 * 
		 * \return Returns a reference to the output samples.
		 **/
		inline std::vector<float> &							Output() { return m_gGen.vOutputBuffer; }

		/**
		 * Sets the AVX/SSE 4 feature set.
		 * 
		 * \param _bAvx2 Specifies whether the AVX 2 feature set is available.
		 * \param _bSse4 Specifies whether the SSE 4 feature set is available.
		 **/
		void												SetFeatureSet( bool _bAvx2, bool _bSse4 ) {
			if ( _bAvx2 ) {
				m_gGen.pfStoreSample = &CSampleBox::StoreSample_AVX;
				m_gGen.pfConvolve = &CSampleBox::Convolve_AVX;
			}
			else if ( _bSse4 ) {
				m_gGen.pfStoreSample = &CSampleBox::StoreSample_SSE;
				m_gGen.pfConvolve = &CSampleBox::Convolve_SSE;
			}
			else {
				m_gGen.pfStoreSample = &CSampleBox::StoreSample;
				m_gGen.pfConvolve = &CSampleBox::Convolve;
			}
		}

		/**
		 * Calculates the _sM parameter for Init() given a transition bandwidth (the range of frequencies to transition from max volume to silence).
		 * 
		 * \param _dBw The frequency range to transition to silence.
		 * \param _ui32OutputRate The frequency of the output, not the input.
		 * \return Returns the _sM parameter for use with Init().
		 **/
		static inline size_t								TransitionRangeToBandwidth( double _dBw, uint32_t _ui32OutputRate ) {
			_dBw /= (_ui32OutputRate * 3.0);				// Bandwidth ratio.
			size_t stM = size_t( std::ceil( 4.0 / _dBw ) );
			if ( !(stM & 1) ) { return stM + 1; }
			return stM;
		}

		/**
		 * Calculates a recommended transition range to pass to TransitionRangeToBandwidth() to pass to Init().  The calculated range is based off the desired output Hz.
		 * 
		 * \param _ui32OutputRate The frequency of the output, not the input.
		 * \return Returns the recommended transition range to pass to TransitionRangeToBandwidth().
		 **/
		static inline double								TransitionRange( uint32_t _ui32OutputRate ) {
			return (_ui32OutputRate * 3.0) - (_ui32OutputRate / 2.0);
		}

		/**
		 * Synthesizes a Blackman window.
		 *
		 * \param _vTaps The array of taps to fill.
		 * \param _sSize The size of the actual window inside the given buffer.  The size normally matches the buffer size, but the buffer could be over-allocated
		 *	to allow for faster SIMD or other reasons.  Indices beyond this are set to 0.
		 */
		static inline void									SynthesizeBlackmanWindow( std::vector<float> &_vTaps, size_t _sSize ) {
			const double dTau = 2.0 * std::numbers::pi;
			size_t stMax = _sSize - 1;
			double dInvMax = 1.0 / stMax;
			double dTauInvMax = dTau * dInvMax;
			double dTauInvMax2 = 2.0 * dTauInvMax;
			for ( auto I = _vTaps.size(); I--; ) {
				if ( I >= _sSize ) {
					_vTaps[I] = 0.0f;
				}
				else {
					_vTaps[I] = float( 0.42 - 0.5 * std::cos( dTauInvMax * I ) + 0.08 * std::cos( dTauInvMax2 * I ) );
				}
			}
		}

		/**
		 * Synthesizes a Hamming window.
		 *
		 * \param _vTaps The array of taps to fill.
		 * \param _sSize The size of the actual window inside the given buffer.  The size normally matches the buffer size, but the buffer could be over-allocated
		 *	to allow for faster SIMD or other reasons.  Indices beyond this are set to 0.
		 */
		static inline void									SynthesizeHammingWindow( std::vector<float> &_vTaps, size_t _sSize ) {
			const double dTau = 2.0 * std::numbers::pi;
			size_t stMax = _sSize - 1;
			double dInvMax = 1.0 / stMax;
			double dTauInvMax = dTau * dInvMax;
			for ( auto I = _vTaps.size(); I--; ) {
				if ( I >= _sSize ) {
					_vTaps[I] = 0.0f;
				}
				else {
					_vTaps[I] = float( 0.54 - 0.46 * std::cos( dTauInvMax * I ) );
				}
			}
		}

		/**
		 * Synthesizes a Hanning window.
		 *
		 * \param _vTaps The array of taps to fill.
		 * \param _sSize The size of the actual window inside the given buffer.  The size normally matches the buffer size, but the buffer could be over-allocated
		 *	to allow for faster SIMD or other reasons.  Indices beyond this are set to 0.
		 */
		static inline void									SynthesizeHanningWindow( std::vector<float> &_vTaps, size_t _sSize ) {
			const double dTau = 2.0 * std::numbers::pi;
			size_t stMax = _sSize - 1;
			double dInvMax = 1.0 / stMax;
			double dTauInvMax = dTau * dInvMax;
			for ( auto I = _vTaps.size(); I--; ) {
				if ( I >= _sSize ) {
					_vTaps[I] = 0.0f;
				}
				else {
					_vTaps[I] = float( 0.50 - 0.50 * std::cos( dTauInvMax * I ) );
				}
			}
		}



	protected :
		// == Types.
		/** General members. */
		struct LSN_GENERAL {
			uint64_t										ui64SrcSampleCnt;							/**< Total samples submitted so far. */
			uint64_t										ui64SampleCnt;								/**< Total samples sent to the (ui32OutputHz * 3) buffer. */
			uint64_t										ui64SamplesBuffered;						/**< Similar to ui64SampleCnt, but it counts how many samples have been interpolated.  Samples sent from the input buffer to the (ui32OutputHz * 3) buffer might temporarily be hold in a buffer in order to perform batch interpolations, so this number is always equal to or higher than ui64SampleCnt. */
			double											dInputHz;									/**< The source frequency. */
			PfStoreSample									pfStoreSample;								/**< The function for stoing a sample from the main input buffer to the intermediate buffer. */
			PfConvolve										pfConvolve;									/**< The function for convolving a sample. */
			std::vector<float>								vBuffer;									/**< Ring buffer of the last few input samples. */
			std::vector<float>								vOutputBuffer;								/**< The final bass-banded output sample. */
			float											fLpf;										/**< The LPF frequency. */
			float											fHpf;										/**< The HPF frequency. */
			uint32_t										ui32OutputHz;								/**< The output frequency. */
		};

		/** Sinc filter (LPF). */
		struct LSN_SINC {
			std::vector<float>								vCeof;										/**< The array of coefficients. */
			std::vector<float>								vRing;										/**< The ring buffer of past samples. */
			size_t											sM;											/**< The midpoint of vCeof. */
		};

		/** HPF filter. */
		struct LSN_HPF {
			float											fAlpha;										/**< Alpha. */
			float											fPreviousOutput;							/**< The previous output sample. */
			float											fPrevInput;									/**< The previous input sample. */
			float											fDelta;										/**< Delta. */
			float											fOutput;									/**< The current filtered output sample. */
		};

		/** An aligned sampling buffer. */
		LSN_ALN
		struct LSN_SAMPLE_STACK {
			float											fStack[8];									/**< Stack of samples, fit for an __m256 register. */
		};

		/** The temporary buffers for sampling multiple points at a time. */
		struct LSN_POINTS {
			LSN_ALN
			LSN_SAMPLE_STACK								fSimdSamples[6];							/**< The SIMD buffers. */
			LSN_ALN
			float											fFractions[sizeof(LSN_SAMPLE_STACK)/sizeof(float)];	/**< The fraction values for each sample. */
			size_t											sSimdStackSize;								/**< The SIMD buffer counter. */
		};


		// == Members.
		LSN_GENERAL											m_gGen;										/**< General members. */
		LSN_SINC											m_sSinc;									/**< Sinc members. */
		LSN_HPF												m_hHpf;										/**< HPF members. */
		LSN_POINTS											m_pPoints;									/**< Interpolation points. */


		// == Functions.
		/**
		 * Interpolates and stores a single sample.  Called at (Output Hz * 3).  Samples get stored into the intermediate buffer
		 *	where sinc filtering will happen.  Every 3rd of these samples gets pushed to the final output.
		 * 
		 * \param _sBufferIdx The index inside the main input buffer where the sample to add lies.
		 * \param _fFrac The interpolation amount between the _sBufferIdx'th sample and the next sample.
		 **/
		void												StoreSample( size_t _sBufferIdx, float _fFrac ) {
			_sBufferIdx %= m_gGen.vBuffer.size();
			float fSample;
			if ( _sBufferIdx >= 2 && _sBufferIdx < (m_gGen.vBuffer.size() - 3) ) {
				// We can interpolate directly out of the ring buffer.
				fSample = Sample_4Point_2ndOrder_Parabolic_2X_X( &m_gGen.vBuffer[_sBufferIdx-2], _fFrac );
			}
			else {
				// The 6 samples cross from the end back to the start, so we have to copy them.
				float fTmp[6];
				int32_t i32AddMe = int32_t( m_gGen.vBuffer.size() ) - 2;
				for ( int32_t I = 0; I < 6; ++I ) {
					fTmp[I] = m_gGen.vBuffer[(int32_t(_sBufferIdx+i32AddMe)+I)%m_gGen.vBuffer.size()];
				}
				fSample = Sample_4Point_2ndOrder_Parabolic_2X_X( fTmp, _fFrac );
			}
			++m_gGen.ui64SamplesBuffered;
			AddSampleToIntermediateBuffer( fSample );
		}

		/**
		 * Interpolates and stores 4 samples.  Called at (Output Hz * 3).  Samples get stored into the intermediate buffer
		 *	where sinc filtering will happen.  Every 3rd of these samples gets pushed to the final output.
		 * 
		 * \param _sBufferIdx The index inside the main input buffer where the sample to add lies.
		 * \param _fFrac The interpolation amount between the _sBufferIdx'th sample and the next sample.
		 **/
		void												StoreSample_SSE( size_t _sBufferIdx, float _fFrac ) {
			
			// Copy the 6 samples to the next row in the SIMD buffer.
			int32_t i32AddMe = int32_t( m_gGen.vBuffer.size() ) - 2;
			for ( int32_t I = 0; I < 6; ++I ) {
				m_pPoints.fSimdSamples[I].fStack[m_pPoints.sSimdStackSize] = m_gGen.vBuffer[(int32_t(_sBufferIdx+i32AddMe)+I)%m_gGen.vBuffer.size()];
			}
			m_pPoints.fFractions[m_pPoints.sSimdStackSize++] = _fFrac;
			m_pPoints.sSimdStackSize &= (sizeof( __m128 ) / sizeof( float ) - 1);
			++m_gGen.ui64SamplesBuffered;
			if ( 0 == m_pPoints.sSimdStackSize ) {
				// The counter overflowed, so the stack is full.
				LSN_ALN
				float fTmp[8];
				Sample_4Point_2ndOrder_Parabolic_2X_X_SSE( &m_pPoints.fSimdSamples[0].fStack[0], &m_pPoints.fSimdSamples[1].fStack[0],
					&m_pPoints.fSimdSamples[2].fStack[0], &m_pPoints.fSimdSamples[3].fStack[0],
					&m_pPoints.fSimdSamples[4].fStack[0], &m_pPoints.fSimdSamples[5].fStack[0],
					m_pPoints.fFractions, fTmp );
				for ( size_t I = 0; I < (sizeof( __m128 ) / sizeof( float )); ++I ) {
					AddSampleToIntermediateBuffer( fTmp[I] );
				}
			}
		}

		/**
		 * Interpolates and stores 8 samples.  Called at (Output Hz * 3).  Samples get stored into the intermediate buffer
		 *	where sinc filtering will happen.  Every 3rd of these samples gets pushed to the final output.
		 * 
		 * \param _sBufferIdx The index inside the main input buffer where the sample to add lies.
		 * \param _fFrac The interpolation amount between the _sBufferIdx'th sample and the next sample.
		 **/
		void												StoreSample_AVX( size_t _sBufferIdx, float _fFrac ) {
			
			// Copy the 6 samples to the next row in the SIMD buffer.
			int32_t i32AddMe = int32_t( m_gGen.vBuffer.size() ) - 2;
			for ( int32_t I = 0; I < 6; ++I ) {
				m_pPoints.fSimdSamples[I].fStack[m_pPoints.sSimdStackSize] = m_gGen.vBuffer[(int32_t(_sBufferIdx+i32AddMe)+I)%m_gGen.vBuffer.size()];
			}
			m_pPoints.fFractions[m_pPoints.sSimdStackSize++] = _fFrac;
			m_pPoints.sSimdStackSize &= (sizeof( __m256 ) / sizeof( float ) - 1);
			++m_gGen.ui64SamplesBuffered;
			if ( 0 == m_pPoints.sSimdStackSize ) {
				// The counter overflowed, so the stack is full.
				LSN_ALN
				float fTmp[8];
				Sample_4Point_2ndOrder_Parabolic_2X_X_AVX( &m_pPoints.fSimdSamples[0].fStack[0], &m_pPoints.fSimdSamples[1].fStack[0],
					&m_pPoints.fSimdSamples[2].fStack[0], &m_pPoints.fSimdSamples[3].fStack[0],
					&m_pPoints.fSimdSamples[4].fStack[0], &m_pPoints.fSimdSamples[5].fStack[0],
					m_pPoints.fFractions, fTmp );
				for ( size_t I = 0; I < (sizeof( __m256 ) / sizeof( float )); ++I ) {
					AddSampleToIntermediateBuffer( fTmp[I] );
				}
			}
		}

		/**
		 * Passes an interpolated sample down the pipeline.  Every 3rd sample sent here gets sent to the final output buffer.
		 * 
		 * \param _fSample The sample to add to the intermediate buffer.
		 **/
		inline void											AddSampleToIntermediateBuffer( float _fSample ) {
			m_sSinc.vRing[m_gGen.ui64SampleCnt%m_sSinc.vRing.size()] = _fSample;
			if ( m_gGen.ui64SampleCnt++ % 3 == 0 ) {
				// Every 3rd sample goes to the output (passing through a sinc filter and HPF).
				if ( m_gGen.ui64SampleCnt > m_sSinc.sM ) {
					size_t sIdx = size_t( (m_gGen.ui64SampleCnt - m_sSinc.sM - 1) % m_sSinc.vRing.size() );
					// Convolutionatizinatify and HPFify this sample.  The next m_sSinc.sM samples following sIdx and the m_sSinc.sM samples before sIdx are valid.
					float fConvolvinated = ProcessHpf( (this->*m_gGen.pfConvolve)( sIdx ) );
					// Send that bad momma-hamma to the output buffer to be retrieved by the user.
					m_gGen.vOutputBuffer.push_back( fConvolvinated );
				}
			}
		}

		/**
		 * Performs convolution on the given sample (indexed into m_sSinc.vRing).
		 * 
		 * \param _sIdx The index of the sample to convolvify.
		 * \return Returns the convolved sample.
		 **/
		float												Convolve( size_t _sIdx ) {
			const float * pfFilter = m_sSinc.vCeof.data();
			const float * pfSamples = m_sSinc.vRing.data();
			float fSum = 0.0f;
			size_t sMod = m_sSinc.vRing.size();
			_sIdx += sMod * 2 - m_sSinc.sM;
			size_t sTotal = m_sSinc.vCeof.size() - 1;
			for ( size_t I = 0; I < sTotal; ++I ) {
				fSum += pfFilter[I] * pfSamples[(_sIdx+I)%sMod];
			}
			return fSum;
		}

		/**
		 * Performs convolution on the given sample (indexed into m_sSinc.vRing).
		 * 
		 * \param _sIdx The index of the sample to convolvify.
		 * \return Returns the convolved sample.
		 **/
		float												Convolve_SSE( size_t _sIdx ) {
			const float * pfFilter = m_sSinc.vCeof.data();
			const float * pfSamples = m_sSinc.vRing.data();
			float fSum = 0.0f;
			size_t sMod = m_sSinc.vRing.size();
			_sIdx += sMod * 2 - m_sSinc.sM;
			size_t sTotal = m_sSinc.vCeof.size() - 1;
			for ( size_t I = 0; I < sTotal; ) {
				size_t sIdx = (_sIdx + I) % sMod;
				if ( (sMod - sIdx) < (sizeof( __m128 ) / sizeof( float )) ) {
					// Copy into a temporary.
					LSN_ALN
					float fTmp[(sizeof(__m128)/sizeof(float))];
					for ( size_t J = 0; J < (sizeof( __m128 ) / sizeof( float )); ++J ) {
						fTmp[J] = pfSamples[(_sIdx+I+J)%sMod];
					}
					fSum += Convolve_SSE( &pfFilter[I], fTmp );
					I += (sizeof( __m128 ) / sizeof( float ));
				}
				else {
					fSum += Convolve_SSE( &pfFilter[I], &pfSamples[sIdx] );
					I += (sizeof( __m128 ) / sizeof( float ));
				}
			}
			return fSum;
		}

		/**
		 * Performs convolution on the given sample (indexed into m_sSinc.vRing).
		 * 
		 * \param _sIdx The index of the sample to convolvify.
		 * \return Returns the convolved sample.
		 **/
		float												Convolve_AVX( size_t _sIdx ) {
			const float * pfFilter = m_sSinc.vCeof.data();
			const float * pfSamples = m_sSinc.vRing.data();
			float fSum = 0.0f;
			size_t sMod = m_sSinc.vRing.size();
			_sIdx += sMod * 2 - m_sSinc.sM;
			size_t sTotal = m_sSinc.vCeof.size() - 1;
			for ( size_t I = 0; I < sTotal; ) {
				size_t sIdx = (_sIdx + I) % sMod;
				if ( (sMod - sIdx) < (sizeof( __m256 ) / sizeof( float )) ) {
					// Copy into a temporary.
					LSN_ALN
					float fTmp[(sizeof(__m256)/sizeof(float))];
					for ( size_t J = 0; J < (sizeof( __m256 ) / sizeof( float )); ++J ) {
						fTmp[J] = pfSamples[(_sIdx+I+J)%sMod];
					}
					fSum += Convolve_AVX( &pfFilter[I], fTmp );
					I += (sizeof( __m256 ) / sizeof( float ));
				}
				else {
					fSum += Convolve_AVX( &pfFilter[I], &pfSamples[sIdx] );
					I += (sizeof( __m256 ) / sizeof( float ));
				}
			}
			return fSum;
		}

		/**
		 * Convolves the given weights with the given samples.  The weights (_pfWeights) must be aligned to a 32-byte boundary.
		 * 
		 * \param _pfWeights The pointer to the 32-byte-aligned 8 weights .
		 * \param _pfSamples Pointer to the samples to convolve.
		 * \return Returns the convolution of the given samples.
		 **/
		float												Convolve_SSE( const float * _pfWeights, const float * _pfSamples ) {
			__m128 mWeights = _mm_load_ps( _pfWeights );
			__m128 mSamples = _mm_loadu_ps( _pfSamples );
			__m128 mMul = _mm_mul_ps( mWeights, mSamples );
			return HorizontalSum( mMul );
		}

		/**
		 * Convolves the given weights with the given samples.  The weights (_pfWeights) must be aligned to a 32-byte boundary.
		 * 
		 * \param _pfWeights The pointer to the 32-byte-aligned 8 weights .
		 * \param _pfSamples Pointer to the samples to convolve.
		 * \return Returns the convolution of the given samples.
		 **/
		float												Convolve_AVX( const float * _pfWeights, const float * _pfSamples ) {
			__m256 mWeights = _mm256_load_ps( _pfWeights );
			__m256 mSamples = _mm256_loadu_ps( _pfSamples );
			__m256 mMul = _mm256_mul_ps( mWeights, mSamples );
			return HorizontalSum( mMul );
		}

		/**
		 * 4-point, 2nd-order parabolic 2x x-form sampling.
		 *
		 * \param _pfsSamples The array of 6 input samples, indices -2, -1, 0, 1, 2, and 3.
		 * \param _fFrac The interpolation amount.
		 * \return Returns the interpolated point.
		 */
		inline float										Sample_4Point_2ndOrder_Parabolic_2X_X( const float * _pfsSamples, float _fFrac ) {
			// 4-point, 2nd-order parabolic 2x (X-form).
			float fY1mM1 = _pfsSamples[1+2] - _pfsSamples[-1+2];
			float fC0 = (1.0f / 2.0f) * _pfsSamples[0+2] + (1.0f / 4.0f) * (_pfsSamples[-1+2] + _pfsSamples[1+2]);
			float fC1 = (1.0f / 2.0f) * fY1mM1;
			float fC2 = (1.0f / 4.0f) * (_pfsSamples[2+2] - _pfsSamples[0+2] - fY1mM1);
			return (fC2 * _fFrac + fC1) * _fFrac + fC0;
		}

		/**
		 * 4-point, 2nd-order parabolic 2x x-form sampling.
		 *
		 * \param _pfsSamples0 The 32-byte-aligned pointer to the 8 1st points.
		 * \param _pfsSamples1 The 32-byte-aligned pointer to the 8 2nd points.
		 * \param _pfsSamples2 The 32-byte-aligned pointer to the 8 3rd points.
		 * \param _pfsSamples3 The 32-byte-aligned pointer to the 8 4th points.
		 * \param _pfsSamples4 The 32-byte-aligned pointer to the 8 5th points.
		 * \param _pfsSamples5 The 32-byte-aligned pointer to the 8 6th points.
		 * \param _pfFrac The interpolation amounts (array of 8 fractions).  Must be aligned to 32 bytes.
		 * \param _pfOut The output pointer.
		 */
		inline void											Sample_4Point_2ndOrder_Parabolic_2X_X_AVX( const float * /*_pfsSamples0*/, const float * _pfsSamples1,
			const float * _pfsSamples2, const float * _pfsSamples3,
			const float * _pfsSamples4, const float * /*_pfsSamples5*/,
			const float * _pfFrac,
			float * _pfOut ) {
			// _pfsSamples[1+2] = _pfsSamples3.
			// _pfsSamples[-1+2] = _pfsSamples1.
			// _pfsSamples[0+2] = _pfsSamples2.
			// _pfsSamples[2+2] = _pfsSamples4.

			// Load the inputs/constants.
			__m256 mS_1p2 = _mm256_load_ps( _pfsSamples3 );
			__m256 m1o2 = _mm256_set1_ps( 1.0f / 2.0f );
			__m256 mS_n1p2 = _mm256_load_ps( _pfsSamples1 );
			__m256 m1o4 = _mm256_set1_ps( 1.0f / 4.0f );

			// float fY1mM1 = _pfsSamples[1+2] - _pfsSamples[-1+2];
			__m256 mY1mM1 = _mm256_sub_ps( mS_1p2, mS_n1p2 );

			// Load the inputs.
			__m256 mS_0p2 = _mm256_load_ps( _pfsSamples2 );

			// float fC0 = (1.0f / 2.0f) * _pfsSamples[0+2] + (1.0f / 4.0f) * (_pfsSamples[-1+2] + _pfsSamples[1+2]);
			__m256 mC0 = _mm256_add_ps( _mm256_mul_ps( m1o2, mS_0p2 ), _mm256_mul_ps( m1o4, _mm256_add_ps( mS_n1p2, mS_1p2 ) ) );

			// Load the inputs.
			__m256 mS_2p2 = _mm256_load_ps( _pfsSamples4 );

			// float fC1 = (1.0f / 2.0f) * fY1mM1;
			__m256 mC1 = _mm256_mul_ps( m1o2, mY1mM1 );

			// Load the inputs.
			__m256 mFrac = _mm256_load_ps( _pfFrac );

			// float fC2 = (1.0f / 4.0f) * (_pfsSamples[2+2] - _pfsSamples[0+2] - fY1mM1);
			__m256 mC2 = _mm256_mul_ps( m1o4, _mm256_sub_ps( _mm256_sub_ps( mS_2p2, mS_0p2 ), mY1mM1 ) );

			// return (fC2 * _fFrac + fC1) * _fFrac + fC0;
			__m256 mRet = _mm256_add_ps( _mm256_mul_ps( _mm256_add_ps( _mm256_mul_ps( mC2, mFrac ), mC1 ), mFrac ), mC0 );
			_mm256_store_ps( _pfOut, mRet );
		}

		/**
		 * 4-point, 2nd-order parabolic 2x x-form sampling.
		 *
		 * \param _pfsSamples0 The 16-byte-aligned pointer to the 4 1st points.
		 * \param _pfsSamples1 The 16-byte-aligned pointer to the 4 2nd points.
		 * \param _pfsSamples2 The 16-byte-aligned pointer to the 4 3rd points.
		 * \param _pfsSamples3 The 16-byte-aligned pointer to the 4 4th points.
		 * \param _pfsSamples4 The 16-byte-aligned pointer to the 4 5th points.
		 * \param _pfsSamples5 The 16-byte-aligned pointer to the 4 6th points.
		 * \param _pfFrac The interpolation amounts (array of 4 fractions).  Must be aligned to 16 bytes.
		 * \param _pfOut The output pointer.
		 */
		inline void											Sample_4Point_2ndOrder_Parabolic_2X_X_SSE( const float * /*_pfsSamples0*/, const float * _pfsSamples1,
			const float * _pfsSamples2, const float * _pfsSamples3,
			const float * _pfsSamples4, const float * /*_pfsSamples5*/,
			const float * _pfFrac,
			float * _pfOut ) {
			// _pfsSamples[1+2] = _pfsSamples3.
			// _pfsSamples[-1+2] = _pfsSamples1.
			// _pfsSamples[0+2] = _pfsSamples2.
			// _pfsSamples[2+2] = _pfsSamples4.

			// Load the inputs/constants.
			__m128 mS_1p2 = _mm_load_ps( _pfsSamples3 );
			__m128 m1o2 = _mm_set1_ps( 1.0f / 2.0f );
			__m128 mS_n1p2 = _mm_load_ps( _pfsSamples1 );
			__m128 m1o4 = _mm_set1_ps( 1.0f / 4.0f );

			// float fY1mM1 = _pfsSamples[1+2] - _pfsSamples[-1+2];
			__m128 mY1mM1 = _mm_sub_ps( mS_1p2, mS_n1p2 );

			// Load the inputs.
			__m128 mS_0p2 = _mm_load_ps( _pfsSamples2 );

			// float fC0 = (1.0f / 2.0f) * _pfsSamples[0+2] + (1.0f / 4.0f) * (_pfsSamples[-1+2] + _pfsSamples[1+2]);
			__m128 mC0 = _mm_add_ps( _mm_mul_ps( m1o2, mS_0p2 ), _mm_mul_ps( m1o4, _mm_add_ps( mS_n1p2, mS_1p2 ) ) );

			// Load the inputs.
			__m128 mS_2p2 = _mm_load_ps( _pfsSamples4 );

			// float fC1 = (1.0f / 2.0f) * fY1mM1;
			__m128 mC1 = _mm_mul_ps( m1o2, mY1mM1 );

			// Load the inputs.
			__m128 mFrac = _mm_load_ps( _pfFrac );

			// float fC2 = (1.0f / 4.0f) * (_pfsSamples[2+2] - _pfsSamples[0+2] - fY1mM1);
			__m128 mC2 = _mm_mul_ps( m1o4, _mm_sub_ps( _mm_sub_ps( mS_2p2, mS_0p2 ), mY1mM1 ) );

			// return (fC2 * _fFrac + fC1) * _fFrac + fC0;
			__m128 mRet = _mm_add_ps( _mm_mul_ps( _mm_add_ps( _mm_mul_ps( mC2, mFrac ), mC1 ), mFrac ), mC0 );
			_mm_store_ps( _pfOut, mRet );
		}

		/**
		 * Horizontally adds all the floats in a given AVX register.
		 * 
		 * \param _mReg The register containing all of the values to sum.
		 * \return Returns the sum of all the floats in the given register.
		 **/
		static inline float									HorizontalSum( __m256 &_mReg ) {
			// Step 1 & 2: Shuffle and add the high 128 to the low 128.
			__m128 mHigh128 = _mm256_extractf128_ps( _mReg, 1 );		// Extract high 128 bits.
			__m128 mLow128 = _mm256_castps256_ps128( _mReg );			// Directly use low 128 bits.
			__m128 mSum128 = _mm_add_ps( mHigh128, mLow128 );			// Add them.

			// Step 3: Perform horizontal addition.
			__m128 mAddH1 = _mm_hadd_ps( mSum128, mSum128 );
			__m128 mAddH2 = _mm_hadd_ps( mAddH1, mAddH1 );

			// Step 4: Extract the scalar value.
			return _mm_cvtss_f32( mAddH2 );
		}

		/**
		 * Horizontally adds all the floats in a given SSE register.
		 * 
		 * \param _mReg The register containing all of the values to sum.
		 * \return Returns the sum of all the floats in the given register.
		 **/
		static inline float									HorizontalSum( __m128 &_mReg ) {
			__m128 mAddH1 = _mm_hadd_ps( _mReg, _mReg );
			__m128 mAddH2 = _mm_hadd_ps( mAddH1, mAddH1 );
			return _mm_cvtss_f32( mAddH2 );
		}
	};

}	// namespace lsn
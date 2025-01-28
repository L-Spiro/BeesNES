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
#include <numbers>
#include <vector>


#include <format>	// TMP

#if defined( _MSC_VER )
    // Microsoft Visual Studio Compiler.
    #define LSN_ALN		__declspec( align( 64 ) )
#elif defined( __GNUC__ ) || defined( __clang__ )
    // GNU Compiler Collection (GCC) or Clang.
    #define LSN_ALN		__attribute__( (aligned( 64 )) )
#else
    #error "Unsupported compiler."
#endif

#pragma warning( push )
#pragma warning( disable : 4324 )	// warning C4324: 'lsn::CSampleBox': structure was padded due to alignment specifier

namespace lsn {

	/**
	 * Class CSampleBox
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
			m_gGen.pfFinalCallback = &CSampleBox::PassThrough;

			m_sSinc.sM = 0;

			m_lLpf.dLastSample = 0.0;
			m_lLpf.dLastInput = 0.0;
			m_lLpf.dGain = 0.0;
			m_lLpf.dCornerFreq = 0.0;

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

		/** A callback function used to make any final modifications to the sample before it goes into the output buffer. */
		typedef float (*									PfFinalCallback)( void *, float, uint32_t );


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
			// Round up to the nearest 16.
			_sM = (_sM + 15) / 16 * 16;
			// Subtract 1 to make it odd.
			--_sM;
			if ( m_gGen.dInputHz == _dInputRate &&
				m_gGen.fLpf == float( _dLpf ) &&
				m_gGen.fHpf == float( _dHpf ) &&
				m_gGen.ui32OutputHz == _ui32OutputRate &&
				m_sSinc.vCeof.size() - 1 == _sM ) { return true; }
			
			::OutputDebugStringA( std::format( "Kernel size: {}\r\n", _sM ).c_str() );
			double dFc = _dLpf / (_ui32OutputRate * 3.0);	// Cut-off ratio.  The sinc is applied to the buffer that is (_ui32OutputRate * 3).

			try {
				// + 1 makes it a multiple of 16.
				m_sSinc.vCeof.resize( _sM + 1 );
				m_sSinc.vRing.resize( (_sM + 1) * 10 );
				m_gGen.vBuffer.resize( size_t( std::ceil( _dInputRate / (_ui32OutputRate * 3) ) + 6 ) );
				m_gGen.vOutputBuffer.clear();
			}
			catch ( ... ) { return false; }
			
			size_t sL = _sM / 2;							// The center sample is the latency.
			for ( auto I = m_sSinc.vCeof.size(); I--; ) {
				m_sSinc.vCeof[I] = 0.0f;
			}
			for ( auto I = m_sSinc.vRing.size(); I--; ) {
				m_sSinc.vRing[I] = 0.0f;
			}

			SynthesizeBlackmanWindow( m_sSinc.vCeof, _sM );
			

			// Apply sinc function.
			double dFc2 = 2.0 * dFc;
			int64_t i64SignedL = int64_t( sL );
			for ( auto I = _sM; I--; ) {
				int64_t N = int64_t( I ) - i64SignedL;
				m_sSinc.vCeof[I] = float( m_sSinc.vCeof[I] * dFc2 * Sinc( dFc2 * N ) );
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


			// Prepare the LPF.
			{
				m_lLpf.dCornerFreq = 2.0 * std::numbers::pi * (_dLpf / _dInputRate);
				m_lLpf.dGain = 1.0 - m_lLpf.dCornerFreq;
				m_lLpf.dCornerFreq *= 0.5;
			}


			// Prepare the HPF.
			double dDelta = (_ui32OutputRate != 0) ? (1.0 / _ui32OutputRate) : 0.0;
			double dTimeConstant = (_dHpf != 0.0) ? (1.0 / _dHpf) : 0.0;
			m_hHpf.fAlpha = float( ((dTimeConstant + dDelta) != 0.0) ? (dTimeConstant / (dTimeConstant + dDelta)) : 0.0 );
			m_hHpf.fOutput = 0.0f;
			//m_hHpf.fPreviousOutput = m_hHpf.fOutput;
			//m_hHpf.fPrevInput = 0.0f;
			m_hHpf.fDelta = 0.0f;


			// General preparation.
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
			m_gGen.vBuffer[(m_gGen.ui64SrcSampleCnt++)%m_gGen.vBuffer.size()] = ProcessLpf( _fSample );
			//m_gGen.vBuffer[(m_gGen.ui64SrcSampleCnt++)%m_gGen.vBuffer.size()] = _fSample;
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
		inline float										ProcessLpf( float _fSample ) {
			m_lLpf.dLastSample = m_lLpf.dLastSample * m_lLpf.dGain + (_fSample + m_lLpf.dLastInput) * m_lLpf.dCornerFreq;
			m_lLpf.dLastInput = _fSample;
			return float( m_lLpf.dLastSample );
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
		 * Sets the AVX/SSE feature set.
		 * 
		 * \param _bAvx512 Specifies whether the AVX-512F feature set is available.
		 * \param _bAvx Specifies whether the AVX feature set is available.
		 * \param _bSse4 Specifies whether the SSE 4.1 feature set is available.
		 **/
		void												SetFeatureSet( bool _bAvx512, bool _bAvx, bool _bSse4 ) {
			m_gGen.pfStoreSample = &CSampleBox::StoreSample;
			m_gGen.pfConvolve = &CSampleBox::Convolve;
			
#ifdef __SSE4_1__
			if ( _bSse4 ) {
				m_gGen.pfStoreSample = &CSampleBox::StoreSample_SSE;
				m_gGen.pfConvolve = &CSampleBox::Convolve_SSE;
			}
#endif  // #ifdef __SSE4_1__

#ifdef __AVX__
			if ( _bAvx ) {
				m_gGen.pfStoreSample = &CSampleBox::StoreSample_AVX;
				m_gGen.pfConvolve = &CSampleBox::Convolve_AVX;
			}
#endif  // #ifdef __AVX__

#ifdef __AVX512F__
			if ( _bAvx512 ) {
				m_gGen.pfStoreSample = &CSampleBox::StoreSample_AVX512;
				m_gGen.pfConvolve = &CSampleBox::Convolve_AVX512;
			}
#endif  // #ifdef __AVX512F__
		}

		/**
		 * Sets the AVX/SSE feature set.
		 * 
		 * \param _bAvx512 Specifies whether the AVX-512F feature set is available.
		 * \param _bAvx Specifies whether the AVX feature set is available.
		 * \param _bSse4 Specifies whether the SSE 4.1 feature set is available.
		 **/
		void												SetOutputCallback( PfFinalCallback _pfFunc, void * _pfParm ) {
			m_gGen.pfFinalCallback = nullptr == _pfFunc ? &CSampleBox::PassThrough : _pfFunc;
			m_gGen.pvFinalParm = _pfParm;
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
			return ((_ui32OutputRate * 1.5) - (_ui32OutputRate / 2.0)) / 2.5;
		}

		/**
		 * Synthesizes a Blackman window.
		 *
		 * \param _vTaps The array of taps to fill.
		 * \param _sSize The size of the actual window inside the given buffer.  The size normally matches the buffer size, but the buffer could be over-allocated
		 *	to allow for faster SIMD or other reasons.  Indices beyond this are set to 0.
		 */
		static inline void									SynthesizeBlackmanWindow( std::vector<float> &_vTaps, size_t _sSize ) {
			// These exact values place zeros at the third and fourth sidelobes, but result in a discontinuity at the edges and a 6 dB/oct fall-off.
			//constexpr double dA0 = 7938.0 / 18608.0;	// 0.42659071367153911236158592146239243447780609130859375
			//constexpr double dA1 = 9240.0 / 18608.0;	// 0.4965606190885640813803547644056379795074462890625
			//constexpr double dA2 = 1430.0 / 18608.0;	// 0.07684866723989682013584712194642634131014347076416015625
			
			// The truncated coefficients do not null the sidelobes as well, but have an improved 18 dB/oct fall-off.
			constexpr double dA = 0.16;
			constexpr double dA0 = (1.0 - dA) / 2.0;	// 0.419999999999999984456877655247808434069156646728515625
			constexpr double dA1 = 1.0 / 2.0;			// 0.5
			constexpr double dA2 = dA / 2.0;			// 0.08000000000000000166533453693773481063544750213623046875

			constexpr double dTau = 2.0 * std::numbers::pi;
			size_t stMax = _sSize - 1;
			double dInvMax = 1.0 / stMax;
			double dTauInvMax = dTau * dInvMax;
			double dTauInvMax2 = 2.0 * dTauInvMax;
			for ( auto I = _vTaps.size(); I--; ) {
				if ( I >= _sSize ) {
					_vTaps[I] = 0.0f;
				}
				else {
					_vTaps[I] = float( dA0 - dA1 * std::cos( dTauInvMax * I ) + dA2 * std::cos( dTauInvMax2 * I ) );
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
			constexpr double dTau = 2.0 * std::numbers::pi;
			size_t stMax = _sSize - 1;
			double dInvMax = 1.0 / stMax;
			double dTauInvMax = dTau * dInvMax;
			for ( auto I = _vTaps.size(); I--; ) {
				if ( I >= _sSize ) {
					_vTaps[I] = 0.0f;
				}
				else {
					_vTaps[I] = float( 0.53836 - 0.46164 * std::cos( dTauInvMax * I ) );
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
			constexpr double dTau = 2.0 * std::numbers::pi;
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

		/**
		 * Standard sinc() function.
		 * 
		 * \param _dX The operand.
		 * \return Returns sin(x*PI) / x*PI.
		 **/
		static inline double								Sinc( double _dX ) {
			_dX *= std::numbers::pi;
			if ( _dX < 0.01 && _dX > -0.01 ) {
				return 1.0 + _dX * _dX * (-1.0 / 6.0 + _dX * _dX * 1.0 / 120.0);
			}

			return std::sin( _dX ) / _dX;
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
			PfFinalCallback									pfFinalCallback;							/**< Allow the user to make any final custom changes to the sample before it goes into the output buffer. */
			void *											pvFinalParm;								/**< The parameter to pass to pfFinalCallback. */
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

		/** LPF filter.  Uses doubles because it is meant to operate on the high-frequency input
		 *	data, which will require extra precision when down-sampling to a significantly smaller output Hz.
		 */
		struct LSN_LPF {
			double											dLastSample;								/**, The last output sample. */
			double											dLastInput;									/**< The last input. */
			double											dGain;										/**< The gain control (a0). */
			double											dCornerFreq;								/**< The corner frequency (b1). */
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
		struct LSN_SAMPLE_STACK {
			LSN_ALN
			float											fStack[16];									/**< Stack of samples, fit for an __m512 register. */
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
		LSN_LPF												m_lLpf;										/**< The LPF applied to the high-frequency data. */
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

#ifdef __SSE4_1__
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
				float fTmp[sizeof(__m128)/sizeof(float)];
				Sample_4Point_2ndOrder_Parabolic_2X_X_SSE( &m_pPoints.fSimdSamples[0].fStack[0], &m_pPoints.fSimdSamples[1].fStack[0],
					&m_pPoints.fSimdSamples[2].fStack[0], &m_pPoints.fSimdSamples[3].fStack[0],
					&m_pPoints.fSimdSamples[4].fStack[0], &m_pPoints.fSimdSamples[5].fStack[0],
					m_pPoints.fFractions, fTmp );
				for ( size_t I = 0; I < (sizeof( __m128 ) / sizeof( float )); ++I ) {
					AddSampleToIntermediateBuffer( fTmp[I] );
				}
			}
		}
#endif  // #ifdef __SSE4_1__
        
#ifdef __AVX__
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
				float fTmp[sizeof(__m256)/sizeof(float)];
				Sample_4Point_2ndOrder_Parabolic_2X_X_AVX( &m_pPoints.fSimdSamples[0].fStack[0], &m_pPoints.fSimdSamples[1].fStack[0],
					&m_pPoints.fSimdSamples[2].fStack[0], &m_pPoints.fSimdSamples[3].fStack[0],
					&m_pPoints.fSimdSamples[4].fStack[0], &m_pPoints.fSimdSamples[5].fStack[0],
					m_pPoints.fFractions, fTmp );
				for ( size_t I = 0; I < (sizeof( __m256 ) / sizeof( float )); ++I ) {
					AddSampleToIntermediateBuffer( fTmp[I] );
				}
			}
		}
#endif  // #ifdef __AVX__

#ifdef __AVX512F__
		/**
		 * Interpolates and stores 16 samples.  Called at (Output Hz * 3).  Samples get stored into the intermediate buffer
		 *	where sinc filtering will happen.  Every 3rd of these samples gets pushed to the final output.
		 * 
		 * \param _sBufferIdx The index inside the main input buffer where the sample to add lies.
		 * \param _fFrac The interpolation amount between the _sBufferIdx'th sample and the next sample.
		 **/
		void												StoreSample_AVX512( size_t _sBufferIdx, float _fFrac ) {
			// Copy the 6 samples to the next row in the SIMD buffer.
			int32_t i32AddMe = int32_t( m_gGen.vBuffer.size() ) - 2;
			for ( int32_t I = 0; I < 6; ++I ) {
				m_pPoints.fSimdSamples[I].fStack[m_pPoints.sSimdStackSize] = m_gGen.vBuffer[(int32_t(_sBufferIdx+i32AddMe)+I)%m_gGen.vBuffer.size()];
			}
			m_pPoints.fFractions[m_pPoints.sSimdStackSize++] = _fFrac;
			m_pPoints.sSimdStackSize &= (sizeof( __m512 ) / sizeof( float ) - 1);
			++m_gGen.ui64SamplesBuffered;
			if ( 0 == m_pPoints.sSimdStackSize ) {
				// The counter overflowed, so the stack is full.
				LSN_ALN
				float fTmp[sizeof(__m512)/sizeof(float)];
				Sample_4Point_2ndOrder_Parabolic_2X_X_AVX512( &m_pPoints.fSimdSamples[0].fStack[0], &m_pPoints.fSimdSamples[1].fStack[0],
					&m_pPoints.fSimdSamples[2].fStack[0], &m_pPoints.fSimdSamples[3].fStack[0],
					&m_pPoints.fSimdSamples[4].fStack[0], &m_pPoints.fSimdSamples[5].fStack[0],
					m_pPoints.fFractions, fTmp );
				for ( size_t I = 0; I < (sizeof( __m512 ) / sizeof( float )); ++I ) {
					AddSampleToIntermediateBuffer( fTmp[I] );
				}
			}
		}
#endif  // #ifdef __AVX512F__

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
					fConvolvinated = (m_gGen.pfFinalCallback)( m_gGen.pvFinalParm, fConvolvinated, m_gGen.ui32OutputHz );
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
		float                                               Convolve( size_t _sIdx ) {
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

#ifdef __SSE4_1__
		/**
		 * Performs convolution on the given sample (indexed into m_sSinc.vRing).
		 * 
		 * \param _sIdx The index of the sample to convolvify.
		 * \return Returns the convolved sample.
		 **/
		float                                               Convolve_SSE( size_t _sIdx ) {
			const float * pfFilter = m_sSinc.vCeof.data();
			const float * pfSamples = m_sSinc.vRing.data();
			__m128 mSum = _mm_set1_ps( 0.0f );
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
					Convolve_SSE( &pfFilter[I], fTmp, mSum );
					I += (sizeof( __m128 ) / sizeof( float ));
				}
				else {
					Convolve_SSE( &pfFilter[I], &pfSamples[sIdx], mSum );
					I += (sizeof( __m128 ) / sizeof( float ));
				}
			}
			return HorizontalSum( mSum );
		}
        
        /**
         * Convolves the given weights with the given samples.  The weights (_pfWeights) must be aligned to a 32-byte boundary.
         *
         * \param _pfWeights The pointer to the 32-byte-aligned 8 weights.
         * \param _pfSamples Pointer to the samples to convolve.
         * \param _mSum Maintains the sum of convolution over many iterations.
         **/
        void                                               Convolve_SSE( const float * _pfWeights, const float * _pfSamples, __m128 &_mSum ) {
            __m128 mWeights = _mm_load_ps( _pfWeights );
            __m128 mSamples = _mm_loadu_ps( _pfSamples );
            _mSum = _mm_add_ps( _mSum, _mm_mul_ps( mWeights, mSamples ) );
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
        inline void                                        Sample_4Point_2ndOrder_Parabolic_2X_X_SSE( const float * /*_pfsSamples0*/, const float * _pfsSamples1,
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
         * 6-point, 5th-order Hermite X-form sampling.
         *
         * \param _pfsSamples0 The 16-byte-aligned pointer to the 8 1st points.
         * \param _pfsSamples1 The 16-byte-aligned pointer to the 8 2nd points.
         * \param _pfsSamples2 The 16-byte-aligned pointer to the 8 3rd points.
         * \param _pfsSamples3 The 16-byte-aligned pointer to the 8 4th points.
         * \param _pfsSamples4 The 16-byte-aligned pointer to the 8 5th points.
         * \param _pfsSamples5 The 16-byte-aligned pointer to the 8 6th points.
         * \param _pfFrac The interpolation amounts (array of 8 fractions).  Must be aligned to 32 bytes.
         * \param _pfOut The output pointer.
         */
        inline void                                        Sample_6Point_5thOrder_Hermite_X_SSE( const float * _pfsSamples0, const float * _pfsSamples1,
            const float * _pfsSamples2, const float * _pfsSamples3,
            const float * _pfsSamples4, const float * _pfsSamples5,
            const float * _pfFrac,
            float * _pfOut ) {
            // 6-point, 5th-order Hermite (X-form).
            // _pfsSamples[-2+2] = _pfsSamples0.
            // _pfsSamples[-1+2] = _pfsSamples1.
            // _pfsSamples[0+2] = _pfsSamples2.
            // _pfsSamples[1+2] = _pfsSamples3.
            // _pfsSamples[2+2] = _pfsSamples4.
            // _pfsSamples[3+2] = _pfsSamples5.

            // Load the inputs/constants.
            __m128 mS_n2p2 = _mm_load_ps( _pfsSamples0 );
            __m128 m1o8 = _mm_set1_ps( 1.0f / 8.0f );
            __m128 mS_2p2 = _mm_load_ps( _pfsSamples4 );
            __m128 m11o24 = _mm_set1_ps( 11.0f / 24.0f );

            // float fEightThym2 = 1.0f / 8.0f * _pfsSamples[-2+2];
            __m128 mEightThym2 = _mm_mul_ps( m1o8, mS_n2p2 );

            // Load the inputs/constants.
            __m128 mS_3p2 = _mm_load_ps( _pfsSamples5 );
            __m128 m1o12 = _mm_set1_ps( 1.0f / 12.0f );

            // float fElevenTwentyFourThy2 = 11.0f / 24.0f * _pfsSamples[2+2];
            __m128 mElevenTwentyFourThy2 = _mm_mul_ps( m11o24, mS_2p2 );

            // float fTwelvThy3 = 1.0f / 12.0f * _pfsSamples[3+2];
            __m128 mTwelvThy3 = _mm_mul_ps( m1o12, mS_3p2 );

            // float fC0 = _pfsSamples[0+2];
            __m128 mC0 = _mm_load_ps( _pfsSamples2 );

            // float fC1 = 1.0f / 12.0f * (_pfsSamples[-2+2] - _pfsSamples[2+2]) + 2.0f / 3.0f * (_pfsSamples[1+2] - _pfsSamples[-1+2]);
            __m128 mS_1p2 = _mm_load_ps( _pfsSamples3 );
            __m128 mS_n1p2 = _mm_load_ps( _pfsSamples1 );
            __m128 mC1 = _mm_add_ps( _mm_mul_ps( m1o12, _mm_sub_ps( mS_n2p2, mS_2p2 ) ), _mm_mul_ps( _mm_set1_ps( 2.0f / 3.0f ), _mm_sub_ps( mS_1p2, mS_n1p2 ) ) );

            // float fC2 = 13.0f / 12.0f * _pfsSamples[-1+2] - 25.0f / 12.0f * _pfsSamples[0+2] + 3.0f / 2.0f * _pfsSamples[1+2] -
            //    fElevenTwentyFourThy2 + fTwelvThy3 - fEightThym2;
            __m128 m13o12 = _mm_set1_ps( 13.0f / 12.0f );
            __m128 mC2 = _mm_sub_ps( _mm_add_ps( _mm_sub_ps( _mm_add_ps( _mm_sub_ps( _mm_mul_ps( m13o12, mS_n1p2 ), _mm_mul_ps( _mm_set1_ps( 25.0f / 12.0f ), mC0 ) ), _mm_mul_ps( _mm_set1_ps( 3.0f / 2.0f ), mS_1p2 ) ),
                mElevenTwentyFourThy2 ), mTwelvThy3 ), mEightThym2 );

            // float fC3 = 5.0f / 12.0f * _pfsSamples[0+2] - 7.0f / 12.0f * _pfsSamples[1+2] + 7.0f / 24.0f * _pfsSamples[2+2] -
            //    1.0f / 24.0f * (_pfsSamples[-2+2] + _pfsSamples[-1+2] + _pfsSamples[3+2]);
            __m128 m5o12 = _mm_set1_ps( 5.0f / 12.0f );
            __m128 m7o12 = _mm_set1_ps( 7.0f / 12.0f );
            __m128 m1o24 = _mm_set1_ps( 1.0f / 24.0f );
            __m128 mC3 = _mm_sub_ps( _mm_add_ps( _mm_sub_ps( _mm_mul_ps( m5o12, mC0 ), _mm_mul_ps( m7o12, mS_1p2 ) ), _mm_mul_ps( _mm_set1_ps( 7.0f / 24.0f ), mS_2p2 ) ), _mm_mul_ps( m1o24, _mm_add_ps( _mm_add_ps( mS_n2p2, mS_n1p2 ), mS_3p2 ) ) );

            // float fC4 = fEightThym2 - 7.0f / 12.0f * _pfsSamples[-1+2] + 13.0f / 12.0f * _pfsSamples[0+2] - _pfsSamples[1+2] +
            //    fElevenTwentyFourThy2 - fTwelvThy3;
            __m128 mC4 = _mm_sub_ps( _mm_add_ps( _mm_sub_ps( _mm_add_ps( _mm_sub_ps( mEightThym2, _mm_mul_ps( m7o12, mS_n1p2 ) ), _mm_mul_ps( m13o12, mC0 ) ), mS_1p2 ), mElevenTwentyFourThy2 ), mTwelvThy3 );

            // Load the inputs.
            __m128 mFrac = _mm_load_ps( _pfFrac );

            // float fC5 = 1.0f / 24.0f * (_pfsSamples[3+2] - _pfsSamples[-2+2]) + 5.0f / 24.0f * (_pfsSamples[-1+2] - _pfsSamples[2+2]) +
            //    5.0f / 12.0f * (_pfsSamples[1+2] - _pfsSamples[0+2]);
            __m128 mC5 = _mm_add_ps( _mm_add_ps( _mm_mul_ps( m1o24, _mm_sub_ps( mS_3p2, mS_n2p2 ) ), _mm_mul_ps( _mm_set1_ps( 5.0f / 24.0f ), _mm_sub_ps( mS_n1p2, mS_2p2 ) ) ), _mm_mul_ps( m5o12, _mm_sub_ps( mS_1p2, mC0 ) ) );

            // return ((((fC5 * _fFrac + fC4) * _fFrac + fC3) * _fFrac + fC2) * _fFrac + fC1) * _fFrac + fC0;
            __m128 mRet = _mm_add_ps( _mm_mul_ps(
                _mm_add_ps( _mm_mul_ps(
                _mm_add_ps( _mm_mul_ps(
                _mm_add_ps( _mm_mul_ps(
                _mm_add_ps( _mm_mul_ps( mC5,
                    mFrac ), mC4 ),
                    mFrac ), mC3 ),
                    mFrac ), mC2 ),
                    mFrac ), mC1 ),
                    mFrac ), mC0 );
            _mm_store_ps( _pfOut, mRet );
        }
        
        /**
         * Horizontally adds all the floats in a given SSE register.
         *
         * \param _mReg The register containing all of the values to sum.
         * \return Returns the sum of all the floats in the given register.
         **/
        static inline float                                 HorizontalSum( const __m128 &_mReg ) {
            __m128 mAddH1 = _mm_hadd_ps( _mReg, _mReg );
            __m128 mAddH2 = _mm_hadd_ps( mAddH1, mAddH1 );
            return _mm_cvtss_f32( mAddH2 );
        }
#endif  // #ifdef __SSE4_1__

#ifdef __AVX__
		/**
		 * Performs convolution on the given sample (indexed into m_sSinc.vRing).
		 * 
		 * \param _sIdx The index of the sample to convolvify.
		 * \return Returns the convolved sample.
		 **/
		float                                               Convolve_AVX( size_t _sIdx ) {
			const float * pfFilter = m_sSinc.vCeof.data();
			const float * pfSamples = m_sSinc.vRing.data();
			__m256 mSum = _mm256_set1_ps( 0.0f );
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
					Convolve_AVX( &pfFilter[I], fTmp, mSum );
					I += (sizeof( __m256 ) / sizeof( float ));
				}
				else {
					Convolve_AVX( &pfFilter[I], &pfSamples[sIdx], mSum );
					I += (sizeof( __m256 ) / sizeof( float ));
				}
			}
			return HorizontalSum( mSum );
		}
        
        /**
         * Convolves the given weights with the given samples.  The weights (_pfWeights) must be aligned to a 32-byte boundary.
         *
         * \param _pfWeights The pointer to the 32-byte-aligned 8 weights.
         * \param _pfSamples Pointer to the samples to convolve.
         * \param _mSum Maintains the sum of convolution over many iterations.
         **/
        void                                                Convolve_AVX( const float * _pfWeights, const float * _pfSamples, __m256 &_mSum ) {
            __m256 mWeights = _mm256_load_ps( _pfWeights );
            __m256 mSamples = _mm256_loadu_ps( _pfSamples );
            _mSum = _mm256_add_ps( _mSum, _mm256_mul_ps( mWeights, mSamples ) );
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
        inline void                                         Sample_4Point_2ndOrder_Parabolic_2X_X_AVX( const float * /*_pfsSamples0*/, const float * _pfsSamples1,
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
         * 6-point, 5th-order Hermite X-form sampling.
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
        inline void                                        Sample_6Point_5thOrder_Hermite_X_AVX( const float * _pfsSamples0, const float * _pfsSamples1,
            const float * _pfsSamples2, const float * _pfsSamples3,
            const float * _pfsSamples4, const float * _pfsSamples5,
            const float * _pfFrac,
            float * _pfOut ) {
            // 6-point, 5th-order Hermite (X-form).
            // _pfsSamples[-2+2] = _pfsSamples0.
            // _pfsSamples[-1+2] = _pfsSamples1.
            // _pfsSamples[0+2] = _pfsSamples2.
            // _pfsSamples[1+2] = _pfsSamples3.
            // _pfsSamples[2+2] = _pfsSamples4.
            // _pfsSamples[3+2] = _pfsSamples5.

            // Load the inputs/constants.
            __m256 mS_n2p2 = _mm256_load_ps( _pfsSamples0 );
            __m256 m1o8 = _mm256_set1_ps( 1.0f / 8.0f );
            __m256 mS_2p2 = _mm256_load_ps( _pfsSamples4 );
            __m256 m11o24 = _mm256_set1_ps( 11.0f / 24.0f );

            // float fEightThym2 = 1.0f / 8.0f * _pfsSamples[-2+2];
            __m256 mEightThym2 = _mm256_mul_ps( m1o8, mS_n2p2 );

            // Load the inputs/constants.
            __m256 mS_3p2 = _mm256_load_ps( _pfsSamples5 );
            __m256 m1o12 = _mm256_set1_ps( 1.0f / 12.0f );

            // float fElevenTwentyFourThy2 = 11.0f / 24.0f * _pfsSamples[2+2];
            __m256 mElevenTwentyFourThy2 = _mm256_mul_ps( m11o24, mS_2p2 );

            // float fTwelvThy3 = 1.0f / 12.0f * _pfsSamples[3+2];
            __m256 mTwelvThy3 = _mm256_mul_ps( m1o12, mS_3p2 );

            // float fC0 = _pfsSamples[0+2];
            __m256 mC0 = _mm256_load_ps( _pfsSamples2 );

            // float fC1 = 1.0f / 12.0f * (_pfsSamples[-2+2] - _pfsSamples[2+2]) + 2.0f / 3.0f * (_pfsSamples[1+2] - _pfsSamples[-1+2]);
            __m256 mS_1p2 = _mm256_load_ps( _pfsSamples3 );
            __m256 mS_n1p2 = _mm256_load_ps( _pfsSamples1 );
            __m256 mC1 = _mm256_add_ps( _mm256_mul_ps( m1o12, _mm256_sub_ps( mS_n2p2, mS_2p2 ) ), _mm256_mul_ps( _mm256_set1_ps( 2.0f / 3.0f ), _mm256_sub_ps( mS_1p2, mS_n1p2 ) ) );

            // float fC2 = 13.0f / 12.0f * _pfsSamples[-1+2] - 25.0f / 12.0f * _pfsSamples[0+2] + 3.0f / 2.0f * _pfsSamples[1+2] -
            //    fElevenTwentyFourThy2 + fTwelvThy3 - fEightThym2;
            __m256 m13o12 = _mm256_set1_ps( 13.0f / 12.0f );
            __m256 mC2 = _mm256_sub_ps( _mm256_add_ps( _mm256_sub_ps( _mm256_add_ps( _mm256_sub_ps( _mm256_mul_ps( m13o12, mS_n1p2 ), _mm256_mul_ps( _mm256_set1_ps( 25.0f / 12.0f ), mC0 ) ), _mm256_mul_ps( _mm256_set1_ps( 3.0f / 2.0f ), mS_1p2 ) ),
                mElevenTwentyFourThy2 ), mTwelvThy3 ), mEightThym2 );

            // float fC3 = 5.0f / 12.0f * _pfsSamples[0+2] - 7.0f / 12.0f * _pfsSamples[1+2] + 7.0f / 24.0f * _pfsSamples[2+2] -
            //    1.0f / 24.0f * (_pfsSamples[-2+2] + _pfsSamples[-1+2] + _pfsSamples[3+2]);
            __m256 m5o12 = _mm256_set1_ps( 5.0f / 12.0f );
            __m256 m7o12 = _mm256_set1_ps( 7.0f / 12.0f );
            __m256 m1o24 = _mm256_set1_ps( 1.0f / 24.0f );
            __m256 mC3 = _mm256_sub_ps( _mm256_add_ps( _mm256_sub_ps( _mm256_mul_ps( m5o12, mC0 ), _mm256_mul_ps( m7o12, mS_1p2 ) ), _mm256_mul_ps( _mm256_set1_ps( 7.0f / 24.0f ), mS_2p2 ) ), _mm256_mul_ps( m1o24, _mm256_add_ps( _mm256_add_ps( mS_n2p2, mS_n1p2 ), mS_3p2 ) ) );

            // float fC4 = fEightThym2 - 7.0f / 12.0f * _pfsSamples[-1+2] + 13.0f / 12.0f * _pfsSamples[0+2] - _pfsSamples[1+2] +
            //    fElevenTwentyFourThy2 - fTwelvThy3;
            __m256 mC4 = _mm256_sub_ps( _mm256_add_ps( _mm256_sub_ps( _mm256_add_ps( _mm256_sub_ps( mEightThym2, _mm256_mul_ps( m7o12, mS_n1p2 ) ), _mm256_mul_ps( m13o12, mC0 ) ), mS_1p2 ), mElevenTwentyFourThy2 ), mTwelvThy3 );

            // Load the inputs.
            __m256 mFrac = _mm256_load_ps( _pfFrac );

            // float fC5 = 1.0f / 24.0f * (_pfsSamples[3+2] - _pfsSamples[-2+2]) + 5.0f / 24.0f * (_pfsSamples[-1+2] - _pfsSamples[2+2]) +
            //    5.0f / 12.0f * (_pfsSamples[1+2] - _pfsSamples[0+2]);
            __m256 mC5 = _mm256_add_ps( _mm256_add_ps( _mm256_mul_ps( m1o24, _mm256_sub_ps( mS_3p2, mS_n2p2 ) ), _mm256_mul_ps( _mm256_set1_ps( 5.0f / 24.0f ), _mm256_sub_ps( mS_n1p2, mS_2p2 ) ) ), _mm256_mul_ps( m5o12, _mm256_sub_ps( mS_1p2, mC0 ) ) );

            // return ((((fC5 * _fFrac + fC4) * _fFrac + fC3) * _fFrac + fC2) * _fFrac + fC1) * _fFrac + fC0;
            __m256 mRet = _mm256_add_ps( _mm256_mul_ps(
                _mm256_add_ps( _mm256_mul_ps(
                _mm256_add_ps( _mm256_mul_ps(
                _mm256_add_ps( _mm256_mul_ps(
                _mm256_add_ps( _mm256_mul_ps( mC5,
                    mFrac ), mC4 ),
                    mFrac ), mC3 ),
                    mFrac ), mC2 ),
                    mFrac ), mC1 ),
                    mFrac ), mC0 );
            _mm256_store_ps( _pfOut, mRet );
        }
        
        /**
         * Horizontally adds all the floats in a given AVX register.
         *
         * \param _mReg The register containing all of the values to sum.
         * \return Returns the sum of all the floats in the given register.
         **/
        static inline float                                 HorizontalSum( const __m256 &_mReg ) {
            __m256 mTmp = _mm256_permute2f128_ps(_mReg, _mReg, 1);	// Shuffle high 128 to low.
			mTmp = _mm256_add_ps( _mReg, mTmp );					// Add high and low parts.

			mTmp = _mm256_hadd_ps( mTmp, mTmp );					// First horizontal add.
			mTmp = _mm256_hadd_ps( mTmp, mTmp );					// Second horizontal add.

			// Extract the lower float which now contains the sum.
			return _mm256_cvtss_f32( mTmp );
        }
#endif  // #ifdef __AVX__

#ifdef __AVX512F__
		/**
		 * Performs convolution on the given sample (indexed into m_sSinc.vRing).
		 * 
		 * \param _sIdx The index of the sample to convolvify.
		 * \return Returns the convolved sample.
		 **/
		float												Convolve_AVX512( size_t _sIdx ) {
			const float * pfFilter = m_sSinc.vCeof.data();
			const float * pfSamples = m_sSinc.vRing.data();
			size_t sMod = m_sSinc.vRing.size();
			_sIdx += sMod * 2 - m_sSinc.sM;
			size_t sTotal = m_sSinc.vCeof.size() - 1;
			__m512 mSum = _mm512_set1_ps( 0.0f );
			for ( size_t I = 0; I < sTotal; ) {
				size_t sIdx = (_sIdx + I) % sMod;
				if ( (sMod - sIdx) < (sizeof( __m512 ) / sizeof( float )) ) {
					// Copy into a temporary.
					LSN_ALN
					float fTmp[(sizeof(__m512)/sizeof(float))];
					for ( size_t J = 0; J < (sizeof( __m512 ) / sizeof( float )); ++J ) {
						fTmp[J] = pfSamples[(_sIdx+I+J)%sMod];
					}
					Convolve_AVX512( &pfFilter[I], fTmp, mSum );
					I += (sizeof( __m512 ) / sizeof( float ));
				}
				else {
					Convolve_AVX512( &pfFilter[I], &pfSamples[sIdx], mSum );
					I += (sizeof( __m512 ) / sizeof( float ));
				}
			}
			return HorizontalSum( mSum );
		}
        
        /**
         * Convolves the given weights with the given samples.  The weights (_pfWeights) must be aligned to a 64-byte boundary.
         *
         * \param _pfWeights The pointer to the 64-byte-aligned 8 weights.
         * \param _pfSamples Pointer to the samples to convolve.
         * \param _mSum Maintains the sum of convolution over many iterations.
         **/
        void                                                Convolve_AVX512( const float * _pfWeights, const float * _pfSamples, __m512 &_mSum ) {
            __m512 mWeights = _mm512_load_ps( _pfWeights );
            __m512 mSamples = _mm512_loadu_ps( _pfSamples );
            _mSum = _mm512_add_ps( _mSum, _mm512_mul_ps( mWeights, mSamples ) );
        }
        
        /**
         * 4-point, 2nd-order parabolic 2x x-form sampling.
         *
         * \param _pfsSamples0 The 64-byte-aligned pointer to the 8 1st points.
         * \param _pfsSamples1 The 64-byte-aligned pointer to the 8 2nd points.
         * \param _pfsSamples2 The 64-byte-aligned pointer to the 8 3rd points.
         * \param _pfsSamples3 The 64-byte-aligned pointer to the 8 4th points.
         * \param _pfsSamples4 The 64-byte-aligned pointer to the 8 5th points.
         * \param _pfsSamples5 The 64-byte-aligned pointer to the 8 6th points.
         * \param _pfFrac The interpolation amounts (array of 8 fractions).  Must be aligned to 64 bytes.
         * \param _pfOut The output pointer.
         */
        inline void                                         Sample_4Point_2ndOrder_Parabolic_2X_X_AVX512( const float * /*_pfsSamples0*/, const float * _pfsSamples1,
            const float * _pfsSamples2, const float * _pfsSamples3,
            const float * _pfsSamples4, const float * /*_pfsSamples5*/,
            const float * _pfFrac,
            float * _pfOut ) {
            // _pfsSamples[1+2] = _pfsSamples3.
            // _pfsSamples[-1+2] = _pfsSamples1.
            // _pfsSamples[0+2] = _pfsSamples2.
            // _pfsSamples[2+2] = _pfsSamples4.

            // Load the inputs/constants.
            __m512 mS_1p2 = _mm512_load_ps( _pfsSamples3 );
            __m512 m1o2 = _mm512_set1_ps( 1.0f / 2.0f );
            __m512 mS_n1p2 = _mm512_load_ps( _pfsSamples1 );
            __m512 m1o4 = _mm512_set1_ps( 1.0f / 4.0f );

            // float fY1mM1 = _pfsSamples[1+2] - _pfsSamples[-1+2];
            __m512 mY1mM1 = _mm512_sub_ps( mS_1p2, mS_n1p2 );

            // Load the inputs.
            __m512 mS_0p2 = _mm512_load_ps( _pfsSamples2 );

            // float fC0 = (1.0f / 2.0f) * _pfsSamples[0+2] + (1.0f / 4.0f) * (_pfsSamples[-1+2] + _pfsSamples[1+2]);
            __m512 mC0 = _mm512_add_ps( _mm512_mul_ps( m1o2, mS_0p2 ), _mm512_mul_ps( m1o4, _mm512_add_ps( mS_n1p2, mS_1p2 ) ) );

            // Load the inputs.
            __m512 mS_2p2 = _mm512_load_ps( _pfsSamples4 );

            // float fC1 = (1.0f / 2.0f) * fY1mM1;
            __m512 mC1 = _mm512_mul_ps( m1o2, mY1mM1 );

            // Load the inputs.
            __m512 mFrac = _mm512_load_ps( _pfFrac );

            // float fC2 = (1.0f / 4.0f) * (_pfsSamples[2+2] - _pfsSamples[0+2] - fY1mM1);
            __m512 mC2 = _mm512_mul_ps( m1o4, _mm512_sub_ps( _mm512_sub_ps( mS_2p2, mS_0p2 ), mY1mM1 ) );

            // return (fC2 * _fFrac + fC1) * _fFrac + fC0;
            __m512 mRet = _mm512_add_ps( _mm512_mul_ps( _mm512_add_ps( _mm512_mul_ps( mC2, mFrac ), mC1 ), mFrac ), mC0 );
            _mm512_store_ps( _pfOut, mRet );
        }
        
        /**
         * 6-point, 5th-order Hermite X-form sampling.
         *
         * \param _pfsSamples0 The 64-byte-aligned pointer to the 8 1st points.
         * \param _pfsSamples1 The 64-byte-aligned pointer to the 8 2nd points.
         * \param _pfsSamples2 The 64-byte-aligned pointer to the 8 3rd points.
         * \param _pfsSamples3 The 64-byte-aligned pointer to the 8 4th points.
         * \param _pfsSamples4 The 64-byte-aligned pointer to the 8 5th points.
         * \param _pfsSamples5 The 64-byte-aligned pointer to the 8 6th points.
         * \param _pfFrac The interpolation amounts (array of 8 fractions).  Must be aligned to 64 bytes.
         * \param _pfOut The output pointer.
         */
        inline void                                         Sample_6Point_5thOrder_Hermite_X_AVX512( const float * _pfsSamples0, const float * _pfsSamples1,
            const float * _pfsSamples2, const float * _pfsSamples3,
            const float * _pfsSamples4, const float * _pfsSamples5,
            const float * _pfFrac,
            float * _pfOut ) {
            // 6-point, 5th-order Hermite (X-form).
            // _pfsSamples[-2+2] = _pfsSamples0.
            // _pfsSamples[-1+2] = _pfsSamples1.
            // _pfsSamples[0+2] = _pfsSamples2.
            // _pfsSamples[1+2] = _pfsSamples3.
            // _pfsSamples[2+2] = _pfsSamples4.
            // _pfsSamples[3+2] = _pfsSamples5.

            // Load the inputs/constants.
            __m512 mS_n2p2 = _mm512_load_ps( _pfsSamples0 );
            __m512 m1o8 = _mm512_set1_ps( 1.0f / 8.0f );
            __m512 mS_2p2 = _mm512_load_ps( _pfsSamples4 );
            __m512 m11o24 = _mm512_set1_ps( 11.0f / 24.0f );

            // float fEightThym2 = 1.0f / 8.0f * _pfsSamples[-2+2];
            __m512 mEightThym2 = _mm512_mul_ps( m1o8, mS_n2p2 );

            // Load the inputs/constants.
            __m512 mS_3p2 = _mm512_load_ps( _pfsSamples5 );
            __m512 m1o12 = _mm512_set1_ps( 1.0f / 12.0f );

            // float fElevenTwentyFourThy2 = 11.0f / 24.0f * _pfsSamples[2+2];
            __m512 mElevenTwentyFourThy2 = _mm512_mul_ps( m11o24, mS_2p2 );

            // float fTwelvThy3 = 1.0f / 12.0f * _pfsSamples[3+2];
            __m512 mTwelvThy3 = _mm512_mul_ps( m1o12, mS_3p2 );

            // float fC0 = _pfsSamples[0+2];
            __m512 mC0 = _mm512_load_ps( _pfsSamples2 );

            // float fC1 = 1.0f / 12.0f * (_pfsSamples[-2+2] - _pfsSamples[2+2]) + 2.0f / 3.0f * (_pfsSamples[1+2] - _pfsSamples[-1+2]);
            __m512 mS_1p2 = _mm512_load_ps( _pfsSamples3 );
            __m512 mS_n1p2 = _mm512_load_ps( _pfsSamples1 );
            __m512 mC1 = _mm512_add_ps( _mm512_mul_ps( m1o12, _mm512_sub_ps( mS_n2p2, mS_2p2 ) ), _mm512_mul_ps( _mm512_set1_ps( 2.0f / 3.0f ), _mm512_sub_ps( mS_1p2, mS_n1p2 ) ) );

            // float fC2 = 13.0f / 12.0f * _pfsSamples[-1+2] - 25.0f / 12.0f * _pfsSamples[0+2] + 3.0f / 2.0f * _pfsSamples[1+2] -
            //    fElevenTwentyFourThy2 + fTwelvThy3 - fEightThym2;
            __m512 m13o12 = _mm512_set1_ps( 13.0f / 12.0f );
            __m512 mC2 = _mm512_sub_ps( _mm512_add_ps( _mm512_sub_ps( _mm512_add_ps( _mm512_sub_ps( _mm512_mul_ps( m13o12, mS_n1p2 ), _mm512_mul_ps( _mm512_set1_ps( 25.0f / 12.0f ), mC0 ) ), _mm512_mul_ps( _mm512_set1_ps( 3.0f / 2.0f ), mS_1p2 ) ),
                mElevenTwentyFourThy2 ), mTwelvThy3 ), mEightThym2 );

            // float fC3 = 5.0f / 12.0f * _pfsSamples[0+2] - 7.0f / 12.0f * _pfsSamples[1+2] + 7.0f / 24.0f * _pfsSamples[2+2] -
            //    1.0f / 24.0f * (_pfsSamples[-2+2] + _pfsSamples[-1+2] + _pfsSamples[3+2]);
            __m512 m5o12 = _mm512_set1_ps( 5.0f / 12.0f );
            __m512 m7o12 = _mm512_set1_ps( 7.0f / 12.0f );
            __m512 m1o24 = _mm512_set1_ps( 1.0f / 24.0f );
            __m512 mC3 = _mm512_sub_ps( _mm512_add_ps( _mm512_sub_ps( _mm512_mul_ps( m5o12, mC0 ), _mm512_mul_ps( m7o12, mS_1p2 ) ), _mm512_mul_ps( _mm512_set1_ps( 7.0f / 24.0f ), mS_2p2 ) ), _mm512_mul_ps( m1o24, _mm512_add_ps( _mm512_add_ps( mS_n2p2, mS_n1p2 ), mS_3p2 ) ) );

            // float fC4 = fEightThym2 - 7.0f / 12.0f * _pfsSamples[-1+2] + 13.0f / 12.0f * _pfsSamples[0+2] - _pfsSamples[1+2] +
            //    fElevenTwentyFourThy2 - fTwelvThy3;
            __m512 mC4 = _mm512_sub_ps( _mm512_add_ps( _mm512_sub_ps( _mm512_add_ps( _mm512_sub_ps( mEightThym2, _mm512_mul_ps( m7o12, mS_n1p2 ) ), _mm512_mul_ps( m13o12, mC0 ) ), mS_1p2 ), mElevenTwentyFourThy2 ), mTwelvThy3 );

            // Load the inputs.
            __m512 mFrac = _mm512_load_ps( _pfFrac );

            // float fC5 = 1.0f / 24.0f * (_pfsSamples[3+2] - _pfsSamples[-2+2]) + 5.0f / 24.0f * (_pfsSamples[-1+2] - _pfsSamples[2+2]) +
            //    5.0f / 12.0f * (_pfsSamples[1+2] - _pfsSamples[0+2]);
            __m512 mC5 = _mm512_add_ps( _mm512_add_ps( _mm512_mul_ps( m1o24, _mm512_sub_ps( mS_3p2, mS_n2p2 ) ), _mm512_mul_ps( _mm512_set1_ps( 5.0f / 24.0f ), _mm512_sub_ps( mS_n1p2, mS_2p2 ) ) ), _mm512_mul_ps( m5o12, _mm512_sub_ps( mS_1p2, mC0 ) ) );

            // return ((((fC5 * _fFrac + fC4) * _fFrac + fC3) * _fFrac + fC2) * _fFrac + fC1) * _fFrac + fC0;
            __m512 mRet = _mm512_add_ps( _mm512_mul_ps(
                _mm512_add_ps( _mm512_mul_ps(
                _mm512_add_ps( _mm512_mul_ps(
                _mm512_add_ps( _mm512_mul_ps(
                _mm512_add_ps( _mm512_mul_ps( mC5,
                    mFrac ), mC4 ),
                    mFrac ), mC3 ),
                    mFrac ), mC2 ),
                    mFrac ), mC1 ),
                    mFrac ), mC0 );
            _mm512_store_ps( _pfOut, mRet );
        }
        
        /**
         * Horizontally adds all the floats in a given AVX-512 register.
         *
         * \param _mReg The register containing all of the values to sum.
         * \return Returns the sum of all the floats in the given register.
         **/
        static inline float                                 HorizontalSum( const __m512 _mReg ) {
			return _mm512_reduce_add_ps( _mReg );
        }
#endif  // #ifdef __AVX512F__

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
		 *  6-point, 5th-order Hermite X-form sampling.
		 *
		 * \param _pfsSamples The array of 6 input samples, indices -2, -1, 0, 1, 2, and 3.
		 * \param _fFrac The interpolation amount.
		 * \return Returns the interpolated point.
		 */
		inline float										Sample_6Point_5thOrder_Hermite_X( const float * _pfsSamples, float _fFrac ) {
			// 6-point, 5th-order Hermite (X-form).
			float fEightThym2 = 1.0f / 8.0f * _pfsSamples[-2+2];
			float fElevenTwentyFourThy2 = 11.0f / 24.0f * _pfsSamples[2+2];
			float fTwelvThy3 = 1.0f / 12.0f * _pfsSamples[3+2];
			float fC0 = _pfsSamples[0+2];
			float fC1 = 1.0f / 12.0f * (_pfsSamples[-2+2] - _pfsSamples[2+2]) + 2.0f / 3.0f * (_pfsSamples[1+2] - _pfsSamples[-1+2]);
			float fC2 = 13.0f / 12.0f * _pfsSamples[-1+2] - 25.0f / 12.0f * _pfsSamples[0+2] + 3.0f / 2.0f * _pfsSamples[1+2] -
				fElevenTwentyFourThy2 + fTwelvThy3 - fEightThym2;
			float fC3 = 5.0f / 12.0f * _pfsSamples[0+2] - 7.0f / 12.0f * _pfsSamples[1+2] + 7.0f / 24.0f * _pfsSamples[2+2] -
				1.0f / 24.0f * (_pfsSamples[-2+2] + _pfsSamples[-1+2] + _pfsSamples[3+2]);
			float fC4 = fEightThym2 - 7.0f / 12.0f * _pfsSamples[-1+2] + 13.0f / 12.0f * _pfsSamples[0+2] - _pfsSamples[1+2] +
				fElevenTwentyFourThy2 - fTwelvThy3;
			float fC5 = 1.0f / 24.0f * (_pfsSamples[3+2] - _pfsSamples[-2+2]) + 5.0f / 24.0f * (_pfsSamples[-1+2] - _pfsSamples[2+2]) +
				5.0f / 12.0f * (_pfsSamples[1+2] - _pfsSamples[0+2]);
			return ((((fC5 * _fFrac + fC4) * _fFrac + fC3) * _fFrac + fC2) * _fFrac + fC1) * _fFrac + fC0;
		}

		/**
		 *  A pass-through function for the sample before final output.
		 *
		 * \param _fSample The input and output sample.
		 * \return Returns _fSample.
		 */
		static float										PassThrough( void *, float _fSample, uint32_t ) { return _fSample; }

	};

}	// namespace lsn

#pragma warning( pop )

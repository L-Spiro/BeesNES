/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Uses OpenAL to handle all for our audio needs.  As an emulator, we just need a single device, context, and source.
 *	This class owns and manages the objects and provides the interface we need to generate audio.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Event/LSNEvent.h"
#include "LSNAudioBase.h"
#include "LSNAudioCoreAudio.h"
#include "LSNAudioOpenAl.h"
#include "LSNSampleBox.h"

#include <thread>

namespace lsn {

	/**
	 * Class CAudio
	 * \brief Uses OpenAL to handle all for our audio needs.
	 *
	 * Description: Uses OpenAL to handle all for our audio needs.  As an emulator, we just need a single device, context, and source.
	 *	This class owns and manages the objects and provides the interface we need to generate audio.
	 */
	class CAudio {
	public :
		// == Functions.
		/**
		 * Initializes audio.
		 * 
		 * \return Returns true if initialization was successful.
		 **/
		static bool											InitializeAudio();

		/**
		 * Shuts down the audio.
		 * 
		 * \return Returns true if shutdown was successful.
		 **/
		static bool											ShutdownAudio();
		
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
		static inline bool									InitSampleBox( double _dLpf, double _dHpf, size_t _sM, double _dInputRate, uint32_t _ui32OutputRate ) {
			return m_sbSampleBox.Init( _dLpf, _dHpf, _sM, _dInputRate, _ui32OutputRate );
		}

		/**
		 * Gets the output frequency.
		 * 
		 * \return Returns the output frequency.
		 **/
		static inline uint32_t								GetOutputFrequency() { return m_adAudioDevice.GetOutputFrequency(); }

		/**
		 * Sets the output frequency in Hz.
		 * 
		 * \param _ui32Hz The new output frequency.
		 **/
		static void											SetOutputFrequency( uint32_t _ui32Hz ) {
			m_adAudioDevice.SetOutputFrequency( _ui32Hz );
		}

		/**
		 * Sets the output format.
		 * 
		 * \param _fFormat The new output format.
		 **/
		static void											SetOutputFormat( CAudioBase::LSN_FORMAT _fFormat ) {
			m_adAudioDevice.SetOutputFormat( _fFormat );
		}

		/**
		 *  6-point, 5th-order Hermite Z-form sampling.
		 *
		 * \param _pfsSamples The array of 6 input samples, indices -2, -1, 0, 1, 2, and 3.
		 * \param _fFrac The interpolation amount.
		 * \return Returns the interpolated point.
		 */
		static inline float									Sample_6Point_5thOrder_Hermite_Z( const float * _pfsSamples, float _fFrac );

		/**
		 *  6-point, 5th-order Hermite X-form sampling.
		 *
		 * \param _pfsSamples The array of 6 input samples, indices -2, -1, 0, 1, 2, and 3.
		 * \param _fFrac The interpolation amount.
		 * \return Returns the interpolated point.
		 */
		static inline float									Sample_6Point_5thOrder_Hermite_X( const float * _pfsSamples, float _fFrac );

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
		static inline void									Sample_6Point_5thOrder_Hermite_X_AVX( const float * _pfsSamples0, const float * _pfsSamples1,
			const float * _pfsSamples2, const float * _pfsSamples3,
			const float * _pfsSamples4, const float * _pfsSamples5,
			const float * _pfFrac,
			float * _pfOut );

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
		static inline void									Sample_6Point_5thOrder_Hermite_X_SSE4( const float * _pfsSamples0, const float * _pfsSamples1,
			const float * _pfsSamples2, const float * _pfsSamples3,
			const float * _pfsSamples4, const float * _pfsSamples5,
			const float * _pfFrac,
			float * _pfOut );

		/**
		 * 4-point, 2nd-order parabolic 2x x-form sampling.
		 *
		 * \param _pfsSamples The array of 6 input samples, indices -2, -1, 0, 1, 2, and 3.
		 * \param _fFrac The interpolation amount.
		 * \return Returns the interpolated point.
		 */
		static inline float									Sample_4Point_2ndOrder_Parabolic_2X_X( const float * _pfsSamples, float _fFrac );

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
		static inline void									Sample_4Point_2ndOrder_Parabolic_2X_X_AVX( const float * _pfsSamples0, const float * _pfsSamples1,
			const float * _pfsSamples2, const float * _pfsSamples3,
			const float * _pfsSamples4, const float * _pfsSamples5,
			const float * _pfFrac,
			float * _pfOut );

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
		static inline void									Sample_4Point_2ndOrder_Parabolic_2X_X_SSE4( const float * _pfsSamples0, const float * _pfsSamples1,
			const float * _pfsSamples2, const float * _pfsSamples3,
			const float * _pfsSamples4, const float * _pfsSamples5,
			const float * _pfFrac,
			float * _pfOut );

		/**
		 * 6-point, 4th-order optimal 2x z-form sampling.
		 *
		 * \param _pfsSamples The array of 6 input samples, indices -2, -1, 0, 1, 2, and 3.
		 * \param _fFrac The interpolation amount.
		 * \return Returns the interpolated point.
		 */
		static inline float									Sample_6Point_4thOrder_2X_Z( const float * _pfsSamples, float _fFrac );

		/**
		 * 6-point, 4th-order optimal 4x z-form sampling.
		 *
		 * \param _pfsSamples The array of 6 input samples, indices -2, -1, 0, 1, 2, and 3.
		 * \param _fFrac The interpolation amount.
		 * \return Returns the interpolated point.
		 */
		static inline float									Sample_6Point_4thOrder_4X_Z( const float * _pfsSamples, float _fFrac );

		/**
		 * 6-point, 5th-order optimal 32x z-form sampling.
		 *
		 * \param _pfsSamples The array of 6 input samples, indices -2, -1, 0, 1, 2, and 3.
		 * \param _fFrac The interpolation amount.
		 * \return Returns the interpolated point.
		 */
		static inline float									Sample_6Point_5thOrder_32X_Z( const float * _pfsSamples, float _fFrac );

		/**
		 * Called when emulation begins.  Resets the ring buffer of buckets.
		 **/
		static void											BeginEmulation();

		/**
		 * Adds a sample to all buckets that need it.
		 *
		 * \param _fSample The audio sample to be added.
		 **/
		static void											AddSample( float _fSample );


	protected :
        // == Types.
#ifdef LSN_WINDOWS
        typedef CAudioOpenAl                                CAudioDevice;
#elif defined( LSN_APPLE )
        typedef CAudioCoreAudio                             CAudioDevice;
#endif  // #ifdef LSN_WINDOWS
        
		// == Members.
		/** The audio interface object. */
		static CAudioDevice									m_adAudioDevice;
		/** The audio thread. */
		static std::unique_ptr<std::thread>					m_ptAudioThread;
		/** Boolean to stop the audio thread. */
		static std::atomic<bool>							m_bRunThread;
		/** The signal that the thread has finished. */
		static CEvent										m_eThreadClosed;
		/** The sample box for band-passed output. */
		static CSampleBox									m_sbSampleBox;

		
		// == Functions.
		/**
		 * Starts the audio thread.
		 * 
		 * \return Returns true if the audio thread is started.
		 **/
		static bool											StartThread();

		/**
		 * Stops the audio thread.
		 **/
		static void											StopThread();

		/**
		 * The audio thread.
		 *
		 * \param _pvParm Unused.
		 */
		static void											AudioThread( void * _pvParm );

	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 *  6-point, 5th-order Hermite Z-form sampling.
	 *
	 * \param _pfsSamples The array of 6 input samples, indices -2, -1, 0, 1, 2, and 3.
	 * \param _fFrac The interpolation amount.
	 * \return Returns the interpolated point.
	 */
	inline float CAudio::Sample_6Point_5thOrder_Hermite_Z( const float * _pfsSamples, float _fFrac ) {
		//  6-point, 5th-order Hermite (Z-form).
		float fZ = _fFrac - 1.0f / 2.0f;
		float fEven1 = _pfsSamples[-2+2] + _pfsSamples[3+2], fOdd1 = _pfsSamples[-2+2] - _pfsSamples[3+2];
		float fEven2 = _pfsSamples[-1+2] + _pfsSamples[2+2], fOdd2 = _pfsSamples[-1+2] - _pfsSamples[2+2];
		float fEven3 = _pfsSamples[0+2] + _pfsSamples[1+2], fOdd3 = _pfsSamples[0+2] - _pfsSamples[1+2];
		float fC0 = 3.0f / 256.0f * fEven1 - 25.0f / 256.0f * fEven2 + 75.0f / 128.0f * fEven3;
		float fC1 = -3.0f / 128.0f * fOdd1 + 61.0f / 384.0f * fOdd2 - 87.0f / 64.0f * fOdd3;
		float fC2 = -5.0f / 96.0f * fEven1 + 13.0f / 32.0f * fEven2 - 17.0f / 48.0f * fEven3;
		float fC3 = 5.0f / 48.0f * fOdd1 - 11.0f / 16.0f * fOdd2 + 37.0f / 24.0f * fOdd3;
		float fC4 = 1.0f / 48.0f * fEven1 - 1.0f / 16.0f * fEven2 + 1.0f / 24.0f * fEven3;
		float fC5 = -1.0f / 24.0f * fOdd1 + 5.0f / 24.0f * fOdd2 - 5.0f / 12.0f * fOdd3;
		return ((((fC5 * fZ + fC4) * fZ + fC3) * fZ + fC2) * fZ + fC1) * fZ + fC0;
	}

	/**
	 *  6-point, 5th-order Hermite X-form sampling.
	 *
	 * \param _pfsSamples The array of 6 input samples, indices -2, -1, 0, 1, 2, and 3.
	 * \param _fFrac The interpolation amount.
	 * \return Returns the interpolated point.
	 */
	inline float CAudio::Sample_6Point_5thOrder_Hermite_X( const float * _pfsSamples, float _fFrac ) {
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

#ifdef __AVX__
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
	inline void CAudio::Sample_6Point_5thOrder_Hermite_X_AVX( const float * _pfsSamples0, const float * _pfsSamples1,
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
		//	fElevenTwentyFourThy2 + fTwelvThy3 - fEightThym2;
		__m256 m13o12 = _mm256_set1_ps( 13.0f / 12.0f );
		__m256 mC2 = _mm256_sub_ps( _mm256_add_ps( _mm256_sub_ps( _mm256_add_ps( _mm256_sub_ps( _mm256_mul_ps( m13o12, mS_n1p2 ), _mm256_mul_ps( _mm256_set1_ps( 25.0f / 12.0f ), mC0 ) ), _mm256_mul_ps( _mm256_set1_ps( 3.0f / 2.0f ), mS_1p2 ) ),
			mElevenTwentyFourThy2 ), mTwelvThy3 ), mEightThym2 );

		// float fC3 = 5.0f / 12.0f * _pfsSamples[0+2] - 7.0f / 12.0f * _pfsSamples[1+2] + 7.0f / 24.0f * _pfsSamples[2+2] -
		//	1.0f / 24.0f * (_pfsSamples[-2+2] + _pfsSamples[-1+2] + _pfsSamples[3+2]);
		__m256 m5o12 = _mm256_set1_ps( 5.0f / 12.0f );
		__m256 m7o12 = _mm256_set1_ps( 7.0f / 12.0f );
		__m256 m1o24 = _mm256_set1_ps( 1.0f / 24.0f );
		__m256 mC3 = _mm256_sub_ps( _mm256_add_ps( _mm256_sub_ps( _mm256_mul_ps( m5o12, mC0 ), _mm256_mul_ps( m7o12, mS_1p2 ) ), _mm256_mul_ps( _mm256_set1_ps( 7.0f / 24.0f ), mS_2p2 ) ), _mm256_mul_ps( m1o24, _mm256_add_ps( _mm256_add_ps( mS_n2p2, mS_n1p2 ), mS_3p2 ) ) );

		// float fC4 = fEightThym2 - 7.0f / 12.0f * _pfsSamples[-1+2] + 13.0f / 12.0f * _pfsSamples[0+2] - _pfsSamples[1+2] +
		//	fElevenTwentyFourThy2 - fTwelvThy3;
		__m256 mC4 = _mm256_sub_ps( _mm256_add_ps( _mm256_sub_ps( _mm256_add_ps( _mm256_sub_ps( mEightThym2, _mm256_mul_ps( m7o12, mS_n1p2 ) ), _mm256_mul_ps( m13o12, mC0 ) ), mS_1p2 ), mElevenTwentyFourThy2 ), mTwelvThy3 );

		// Load the inputs.
		__m256 mFrac = _mm256_load_ps( _pfFrac );

		// float fC5 = 1.0f / 24.0f * (_pfsSamples[3+2] - _pfsSamples[-2+2]) + 5.0f / 24.0f * (_pfsSamples[-1+2] - _pfsSamples[2+2]) +
		//	5.0f / 12.0f * (_pfsSamples[1+2] - _pfsSamples[0+2]);
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
#endif  // #ifdef __AVX__

#ifdef __SSE4_1__
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
	inline void CAudio::Sample_6Point_5thOrder_Hermite_X_SSE4( const float * _pfsSamples0, const float * _pfsSamples1,
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
		//	fElevenTwentyFourThy2 + fTwelvThy3 - fEightThym2;
		__m128 m13o12 = _mm_set1_ps( 13.0f / 12.0f );
		__m128 mC2 = _mm_sub_ps( _mm_add_ps( _mm_sub_ps( _mm_add_ps( _mm_sub_ps( _mm_mul_ps( m13o12, mS_n1p2 ), _mm_mul_ps( _mm_set1_ps( 25.0f / 12.0f ), mC0 ) ), _mm_mul_ps( _mm_set1_ps( 3.0f / 2.0f ), mS_1p2 ) ),
			mElevenTwentyFourThy2 ), mTwelvThy3 ), mEightThym2 );

		// float fC3 = 5.0f / 12.0f * _pfsSamples[0+2] - 7.0f / 12.0f * _pfsSamples[1+2] + 7.0f / 24.0f * _pfsSamples[2+2] -
		//	1.0f / 24.0f * (_pfsSamples[-2+2] + _pfsSamples[-1+2] + _pfsSamples[3+2]);
		__m128 m5o12 = _mm_set1_ps( 5.0f / 12.0f );
		__m128 m7o12 = _mm_set1_ps( 7.0f / 12.0f );
		__m128 m1o24 = _mm_set1_ps( 1.0f / 24.0f );
		__m128 mC3 = _mm_sub_ps( _mm_add_ps( _mm_sub_ps( _mm_mul_ps( m5o12, mC0 ), _mm_mul_ps( m7o12, mS_1p2 ) ), _mm_mul_ps( _mm_set1_ps( 7.0f / 24.0f ), mS_2p2 ) ), _mm_mul_ps( m1o24, _mm_add_ps( _mm_add_ps( mS_n2p2, mS_n1p2 ), mS_3p2 ) ) );

		// float fC4 = fEightThym2 - 7.0f / 12.0f * _pfsSamples[-1+2] + 13.0f / 12.0f * _pfsSamples[0+2] - _pfsSamples[1+2] +
		//	fElevenTwentyFourThy2 - fTwelvThy3;
		__m128 mC4 = _mm_sub_ps( _mm_add_ps( _mm_sub_ps( _mm_add_ps( _mm_sub_ps( mEightThym2, _mm_mul_ps( m7o12, mS_n1p2 ) ), _mm_mul_ps( m13o12, mC0 ) ), mS_1p2 ), mElevenTwentyFourThy2 ), mTwelvThy3 );

		// Load the inputs.
		__m128 mFrac = _mm_load_ps( _pfFrac );

		// float fC5 = 1.0f / 24.0f * (_pfsSamples[3+2] - _pfsSamples[-2+2]) + 5.0f / 24.0f * (_pfsSamples[-1+2] - _pfsSamples[2+2]) +
		//	5.0f / 12.0f * (_pfsSamples[1+2] - _pfsSamples[0+2]);
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
#endif  // #ifdef __SSE4_1__

	/**
	 * 4-point, 2nd-order parabolic 2x x-form sampling.
	 *
	 * \param _pfsSamples The array of 6 input samples, indices -2, -1, 0, 1, 2, and 3.
	 * \param _fFrac The interpolation amount.
	 * \return Returns the interpolated point.
	 */
	inline float CAudio::Sample_4Point_2ndOrder_Parabolic_2X_X( const float * _pfsSamples, float _fFrac ) {
		// 4-point, 2nd-order parabolic 2x (X-form).
		float fY1mM1 = _pfsSamples[1+2] - _pfsSamples[-1+2];
		float fC0 = (1.0f / 2.0f) * _pfsSamples[0+2] + (1.0f / 4.0f) * (_pfsSamples[-1+2] + _pfsSamples[1+2]);
		float fC1 = (1.0f / 2.0f) * fY1mM1;
		float fC2 = (1.0f / 4.0f) * (_pfsSamples[2+2] - _pfsSamples[0+2] - fY1mM1);
		return (fC2 * _fFrac + fC1) * _fFrac + fC0;
	}

#ifdef __AVX__
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
	inline void CAudio::Sample_4Point_2ndOrder_Parabolic_2X_X_AVX( const float * /*_pfsSamples0*/, const float * _pfsSamples1,
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
#endif  // #ifdef __AVX__

#ifdef __SSE4_1__
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
	inline void CAudio::Sample_4Point_2ndOrder_Parabolic_2X_X_SSE4( const float * /*_pfsSamples0*/, const float * _pfsSamples1,
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
#endif  // #ifdef __SSE4_1__

	/**
	 * 6-point, 4th-order optimal 2x z-form sampling.
	 *
	 * \param _pfsSamples The array of 6 input samples, indices -2, -1, 0, 1, 2, and 3.
	 * \param _fFrac The interpolation amount.
	 * \return Returns the interpolated point.
	 */
	inline float CAudio::Sample_6Point_4thOrder_2X_Z( const float * _pfsSamples, float _fFrac ) {
		//  6-point, 5th-order Hermite (Z-form).
		float fZ = _fFrac - 1.0f / 2.0f;
		float fEven1 = _pfsSamples[-2+2] + _pfsSamples[3+2], fOdd1 = _pfsSamples[-2+2] - _pfsSamples[3+2];
		float fEven2 = _pfsSamples[-1+2] + _pfsSamples[2+2], fOdd2 = _pfsSamples[-1+2] - _pfsSamples[2+2];
		float fEven3 = _pfsSamples[0+2] + _pfsSamples[1+2], fOdd3 = _pfsSamples[0+2] - _pfsSamples[1+2];
		float fC0 = fEven1 * 0.37484203669443822f + fEven2 * 0.11970939637439368f
			+ fEven3 * 0.00544862268096358f;
		float fC1 = fOdd1 * 0.19253897284651597f + fOdd2 * 0.22555179040018719f
			+ fOdd3 * 0.02621377625620669f;
		float fC2 = fEven1 * -0.154026006475653071f + fEven2 * 0.10546111301131367f
			+ fEven3 * 0.04856757454258609f;
		float fC3 = fOdd1 * -0.06523685579716083f + fOdd2 * -0.04867197815057284f
			+ fOdd3 * 0.04200764942718964f;
		float fC4 = fEven1 * 0.03134095684084392f + fEven2 * -0.04385804833432710f
			+ fEven3 * 0.01249475765486819f;
		return (((fC4 * fZ + fC3) * fZ + fC2) * fZ + fC1) * fZ + fC0;
	}

	/**
	 * 6-point, 4th-order optimal 4x z-form sampling.
	 *
	 * \param _pfsSamples The array of 6 input samples, indices -2, -1, 0, 1, 2, and 3.
	 * \param _fFrac The interpolation amount.
	 * \return Returns the interpolated point.
	 */
	inline float CAudio::Sample_6Point_4thOrder_4X_Z( const float * _pfsSamples, float _fFrac ) {
		//  6-point, 5th-order Hermite (Z-form).
		float fZ = _fFrac - 1.0f / 2.0f;
		float fEven1 = _pfsSamples[-2+2] + _pfsSamples[3+2], fOdd1 = _pfsSamples[-2+2] - _pfsSamples[3+2];
		float fEven2 = _pfsSamples[-1+2] + _pfsSamples[2+2], fOdd2 = _pfsSamples[-1+2] - _pfsSamples[2+2];
		float fEven3 = _pfsSamples[0+2] + _pfsSamples[1+2], fOdd3 = _pfsSamples[0+2] - _pfsSamples[1+2];
		float fC0 = fEven1 * 0.26148143200222657f + fEven2 * 0.22484494681472966f
			+ fEven3 * 0.01367360612950508f;
		float fC1 = fOdd1 * -0.20245593827436142f + fOdd2 * 0.29354348112881601f
			+ fOdd3 * 0.06436924057941607f;
		float fC2 = fEven1 * -0.022982104451679701f + fEven2 * -0.09068617668887535f
			+ fEven3 * 0.11366875749521399f;
		float fC3 = fOdd1 * 0.36296419678970931f + fOdd2 * -0.26421064520663945f
			+ fOdd3 * 0.08591542869416055f;
		float fC4 = fEven1 * 0.02881527997393852f + fEven2 * -0.04250898918476453f
			+ fEven3 * 0.01369173779618459f;
		return (((fC4 * fZ + fC3) * fZ + fC2) * fZ + fC1) * fZ + fC0;
	}

	/**
	 * 6-point, 5th-order optimal 32x z-form sampling.
	 *
	 * \param _pfsSamples The array of 6 input samples, indices -2, -1, 0, 1, 2, and 3.
	 * \param _fFrac The interpolation amount.
	 * \param _dPreEmphHzInRadians The pre-emphasis x value.
	 * \return Returns the interpolated point.
	 */
	inline float CAudio::Sample_6Point_5thOrder_32X_Z( const float * _pfsSamples, float _fFrac ) {
		// Optimal 32x (6-point, 5th-order) (Z-form).
		float fZ = _fFrac - 1.0f / 2.0f;
		float fEven1 = _pfsSamples[1+2] + _pfsSamples[0+2], fOdd1 = _pfsSamples[1+2] - _pfsSamples[0+2];
		float fEven2 = _pfsSamples[2+2] + _pfsSamples[-1+2], fOdd2 = _pfsSamples[2+2] - _pfsSamples[-1+2];
		float fEven3 = _pfsSamples[3+2] + _pfsSamples[-2+2], fOdd3 = _pfsSamples[3+2] - _pfsSamples[-2+2];
		float fC0 = fEven1 * 0.42685983409379380f + fEven2 * 0.07238123511170030f
			+ fEven3 * 0.00075893079450573f;
		float fC1 = fOdd1 * 0.35831772348893259f + fOdd2 * 0.20451644554758297f
			+ fOdd3 * 0.00562658797241955f;
		float fC2 = fEven1 * -0.217009177221292431f + fEven2 * 0.20051376594086157f
			+ fEven3 * 0.01649541128040211f;
		float fC3 = fOdd1 * -0.25112715343740988f + fOdd2 * 0.04223025992200458f
			+ fOdd3 * 0.02488727472995134f;
		float fC4 = fEven1 * 0.04166946673533273f + fEven2 * -0.06250420114356986f
			+ fEven3 * 0.02083473440841799f;
		float fC5 = fOdd1 * 0.08349799235675044f + fOdd2 * -0.04174912841630993f
			+ fOdd3 * 0.00834987866042734f;
		return ((((fC5 * fZ + fC4) * fZ + fC3) * fZ + fC2) * fZ + fC1) * fZ + fC0;
	}

}	// namespace lsn

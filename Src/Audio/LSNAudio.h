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
#include "LSNOpenAlBuffer.h"
#include "LSNOpenAlContext.h"
#include "LSNOpenAlDevice.h"
#include "LSNOpenAlSource.h"
#include "LSNSampleBucket.h"
#include <thread>

#define LSN_AUDIO_BUFFERS									288
#define LSN_BUCKETS											(LSN_SAMPLER_BUCKET_SIZE * 3)

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
		 * Gets the global OpenAL device.
		 * 
		 * \return Returns the global OpenAL device.
		 **/
		static COpenAlDevice &								Device() { return m_oadDevice; }

		/**
		 * Gets the global OpenAL source.
		 * 
		 * \return Returns the global OpenAL source.
		 **/
		static COpenAlSource &								Source() { return m_oasSource; }

		/**
		 * Gets the output frequency.
		 * 
		 * \return Returns the output frequency.
		 **/
		static inline uint32_t								GetOutputFrequency() { return uint32_t( m_sFrequency ); }

		/**
		 * Sets the output frequency in Hz.
		 * 
		 * \param _ui32Hz The new output frequency.
		 **/
		static void											SetOutputFrequency( uint32_t _ui32Hz ) {
			m_sNextFrequency = _ui32Hz;
		}

		/**
		 * Sets the output format.
		 * 
		 * \param _eFormat The new output format.
		 **/
		static void											SetOutputFormat( ALenum _eFormat ) {
			m_eNextFormat = _eFormat;
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
		 * 6-point, 5th-order optimal 32x z-form sampling.
		 *
		 * \param _pfsSamples The array of 6 input samples, indices -2, -1, 0, 1, 2, and 3.
		 * \param _fFrac The interpolation amount.
		 * \param _dPreEmphHzInRadians The pre-emphasis x value.
		 * \return Returns the interpolated point.
		 */
		static inline float									Sample_6Point_5thOrder_32X_Z( const float * _pfsSamples, float _fFrac );

		/**
		 * Called when emulation begins.  Resets the ring buffer of buckets.
		 **/
		static void											BeginEmulation();

		/**
		 * Adds a bucket to the ring buffer to be filled in during later APU cycles.
		 * 
		 * \param _ui64Cycle The APU cycle of the sample to be surrounded by the bucket.
		 * \param _fInterp The interpolation factor to be used during sampling.
		 **/
		static void											RegisterBucket( uint64_t _ui64Cycle, float _fInterp );

		/**
		 * Adds a sample to all buckets that need it.
		 * 
		 * \param _ui64Cycle The APU cycle of the sample.
		 * \param _fSample The audio sample to be added.
		 **/
		static void											AddSample( uint64_t _ui64Cycle, float _fSample );


	protected :
		// == Types.
		/** The linked list of sample buckets. */
		struct LSN_SAMPLE_BUCKET_LIST {
			/** The current low index inside the ring buffer. */
			uint64_t										ui64Idx;
			/** The current high index inside the ring buffer. */
			uint64_t										ui64HiIdx;
			/** The ring buffer of buckets. */
			LSN_SAMPLE_BUCKET								sbBuckets[LSN_BUCKETS];
		};



		// == Members.
		/** The total number of buffers uploaded during the lifetime of the source. */
		static uint64_t										m_ui64TotalLifetimeQueues;
		/** The total number of buffers unloaded during the lifetime of the source. */
		static uint64_t										m_ui64TotalLifetimeUnqueueds;
		/** The current buffer. */
		static std::vector<uint8_t>							m_vLocalBuffer;
		/** The current buffer being filled. */
		static size_t										m_sBufferIdx;
		/** The primary OpenAL device. */
		static COpenAlDevice								m_oadDevice;
		/** The context. */
		static COpenAlContext								m_oacContext;
		/** For emulation purposes, we only have 1 source. */
		static COpenAlSource								m_oasSource;
		/** The audio buffers. */
		static COpenAlBuffer								m_oabBuffers[LSN_AUDIO_BUFFERS];
		/** The size of each buffer in samples. */
		static ALsizei										m_sBufferSizeInSamples;
		/** The current buffer position. */
		static ALsizei										m_sCurBufferSize;
		/** The frequency of the current buffer.  Flush to set (flushing copies from the "Next" value into this one). */
		static ALsizei										m_sFrequency;
		/** The frequency to set after the next flush. */
		static ALsizei										m_sNextFrequency;
		/** The current output format. Flush to set (flushing copies from the "Next" value into this one). */
		static ALenum										m_eFormat;
		/** The output format to apply on the next flush. */
		static ALenum										m_eNextFormat;
		/** The audio thread. */
		static std::unique_ptr<std::thread>					m_ptAudioThread;
		/** Boolean to stop the audio thread. */
		static std::atomic<bool>							m_bRunThread;
		/** The signal that the thread has finished. */
		static CEvent										m_eThreadClosed;
		/** The ring buffer of buckets. */
		static LSN_SAMPLE_BUCKET_LIST						m_sblBuckets;
		/** Temporary float-format storage of samples. */
		static std::vector<float>							m_vTmpBuffer;
		/** The position within the temporary buffer of the current sample. */
		static size_t										m_sTmpBufferIdx;
		

		
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
		 * Flushes any audio currently buffered locally to the OpenAL API.
		 * 
		 * \return Returns true if the audio was buffered into OpenAL.
		 **/
		static bool											Flush();

		/**
		 * Buffers samples from floating-point to the AL_FORMAT_MONO8 format.
		 * 
		 * \param _pfSamples The samples to buffer.
		 * \param _sTotal The number of samples to which _pfSamples points.
		 * \return Returns true if the buffer succeeded.
		 **/
		static bool											BufferMono8( const float * _pfSamples, size_t _sTotal );

		/**
		 * Buffers samples from floating-point to the AL_FORMAT_MONO16 format.
		 * 
		 * \param _pfSamples The samples to buffer.
		 * \param _sTotal The number of samples to which _pfSamples points.
		 * \return Returns true if the buffer succeeded.
		 **/
		static bool											BufferMono16( const float * _pfSamples, size_t _sTotal );

		/**
		 * Buffers samples from floating-point to the AL_FORMAT_MONO_FLOAT32 format.
		 * 
		 * \param _pfSamples The samples to buffer.
		 * \param _sTotal The number of samples to which _pfSamples points.
		 * \return Returns true if the buffer succeeded.
		 **/
		static bool											BufferMonoF32( const float * _pfSamples, size_t _sTotal );

		/**
		 * Copies from m_vTmpBuffer into the local buffer passed to OpenAL, performing the format conversion as necessary.
		 * 
		 * \return Returns true if the transfer succeeded.
		 **/
		static bool											TransferTmpToLocal();

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

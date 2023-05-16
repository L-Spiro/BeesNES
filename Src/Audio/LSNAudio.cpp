/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Uses OpenAL to handle all for our audio needs.  As an emulator, we just need a single device, context, and source.
 *	This class owns and manages the objects and provides the interface we need to generate audio.
 */

#include "LSNAudio.h"
#include "LSNOpenAl.h"

namespace lsn {

	// == Members.
	/** The total number of buffers uploaded during the lifetime of the source. */
	uint64_t CAudio::m_ui64TotalLifetimeQueues = 0;

	/** The total number of buffers unloaded during the lifetime of the source. */
	uint64_t CAudio::m_ui64TotalLifetimeUnqueueds = 0;

	/** The current buffer. */
	std::vector<uint8_t> CAudio::m_vLocalBuffer;

	/** The current buffer being filled. */
	size_t CAudio::m_sBufferIdx = 0;

	/** The primary OpenAL device. */
	COpenAlDevice CAudio::m_oadDevice;

	/** The context. */
	COpenAlContext CAudio::m_oacContext;

	/** For emulation purposes, we only have 1 source. */
	COpenAlSource CAudio::m_oasSource;

	/** The audio buffers. */
	COpenAlBuffer CAudio::m_oabBuffers[LSN_AUDIO_BUFFERS];

	/** The size of each buffer in samples. */
	ALsizei CAudio::m_sBufferSizeInSamples = 
#ifdef LSN_WIN64
		768
#else
		1024
#endif
		;

	/** The current buffer position. */
	ALsizei CAudio::m_sCurBufferSize = 0;

	/** The frequency of the current buffer.  Flush to set (flushing copies from the “Next” value into this one). */
	ALsizei CAudio::m_sFrequency = 48000;

	/** The frequency to set after the next flush. */
	ALsizei CAudio::m_sNextFrequency = 48000;

	/** The current output format. Flush to set (flushing copies from the “Next” value into this one). */
	ALenum CAudio::m_eFormat = AL_FORMAT_MONO16;

	/** The output format to apply on the next flush. */
	ALenum CAudio::m_eNextFormat = AL_FORMAT_MONO16;

	/** The audio thread. */
	std::unique_ptr<std::thread> CAudio::m_ptAudioThread;

	/** Boolean to stop the audio thread. */
	std::atomic<bool> CAudio::m_bRunThread = true;

	/** The signal that the thread has finished. */
	CEvent CAudio::m_eThreadClosed;

	/** The ring buffer of buckets. */
	CAudio::LSN_SAMPLE_BUCKET_LIST CAudio::m_sblBuckets;

	/** Temporary float-format storage of samples. */
	std::vector<float> CAudio::m_vTmpBuffer;

	/** The position within the temporary buffer of the current sample. */
	size_t CAudio::m_sTmpBufferIdx = 0;

	// == Functions.
	/**
	 * Initializes audio.
	 * 
	 * \return Returns true if initialization was successful.
	 **/
	bool CAudio::InitializeAudio() {
		std::vector<std::string> vDevices;
		// We need a dummy device to enumerate the audio devices.
		COpenAlDevice oadDummy;
		if ( !COpenAlDevice::GetAudioDevices( vDevices, oadDummy ) ) { return false; }
		if ( !vDevices.size() ) { return false; }
		if ( !m_oadDevice.CreateDevice( vDevices[0].c_str() ) ) { return false; }
		if ( !m_oacContext.CreateContext( m_oadDevice, nullptr ) ) { return false; }
		if ( !m_oacContext.MakeCurrent() ) { return false; }

		if ( !COpenAl::InitializeOpenAl( m_oadDevice.Device() ) ) { return false; }

		if ( !COpenAl::DistanceModel( AL_NONE ) ) { return false; }

		if ( !m_oasSource.CreateSource() ) { return false; }
		if ( !m_oasSource.SetLooping( AL_FALSE ) ) { return false; }
		/*for ( auto I = LSN_AUDIO_BUFFERS; I--; ) {
			if ( !m_oabBuffers[I].CreateBuffer() ) { return false; }
		}*/

		m_vTmpBuffer.resize( m_sBufferSizeInSamples );
		m_vLocalBuffer.resize( m_sBufferSizeInSamples * 4 );
		if ( !StartThread() ) { return false; }
		return true;
	}

	/**
	 * Shuts down the audio.
	 * 
	 * \return Returns true if shutdown was successful.
	 **/
	bool CAudio::ShutdownAudio() {
		StopThread();

		m_oasSource.Reset();
		for ( auto I = LSN_AUDIO_BUFFERS; I--; ) {
			m_oabBuffers[I].Reset();
		}
		m_oacContext.Reset();
		return m_oadDevice.Reset();
	}

	/**
	 * Called when emulation begins.  Resets the ring buffer of buckets.
	 **/
	void CAudio::BeginEmulation() {
		StopThread();
		m_sblBuckets.ui64Idx = m_sblBuckets.ui64HiIdx = 0;
		for ( size_t I = LSN_BUCKETS; I--; ) {
			m_sblBuckets.sbBuckets[I].ui64ApuStartCycle = uint64_t( -LSN_SAMPLER_BUCKET_SIZE );
			m_sblBuckets.sbBuckets[I].fInterp = 0.0f;
			std::memset( m_sblBuckets.sbBuckets[I].fBucket, 0, sizeof( m_sblBuckets.sbBuckets[I].fBucket ) );
		}
		m_sblBuckets.sbBuckets[0].ui64ApuStartCycle = uint64_t( -2 );
		m_sblBuckets.ui64HiIdx = 1;
		StartThread();
	}

	/**
	 * Adds a bucket to the ring buffer to be filled in during later APU cycles.
	 * 
	 * \param _ui64Cycle The APU cycle of the sample to be surrounded by the bucket.
	 * \param _fInterp The interpolation factor to be used during sampling.
	 **/
	void CAudio::RegisterBucket( uint64_t _ui64Cycle, float _fInterp ) {
		uint64_t ui64Idx = m_sblBuckets.ui64HiIdx % LSN_BUCKETS;
		LSN_SAMPLE_BUCKET & sbBucket = m_sblBuckets.sbBuckets[ui64Idx];
		sbBucket.ui64ApuStartCycle = _ui64Cycle - (LSN_SAMPLER_BUCKET_SIZE / 2) + 1;
		sbBucket.fInterp = _fInterp;
		++m_sblBuckets.ui64HiIdx;
	}

	/**
	 * Adds a sample to all buckets that need it.
	 * 
	 * \param _ui64Cycle The APU cycle of the sample.
	 * \param _fSample The audio sample to be added.
	 **/
	void CAudio::AddSample( uint64_t _ui64Cycle, float _fSample ) {
		uint64_t ui64Hi = m_sblBuckets.ui64HiIdx;
		for ( uint64_t I = m_sblBuckets.ui64Idx; I < ui64Hi; ++I ) {
			uint64_t ui64Idx = I % LSN_BUCKETS;
			LSN_SAMPLE_BUCKET & sbBucket = m_sblBuckets.sbBuckets[ui64Idx];
			if ( int64_t( sbBucket.ui64ApuStartCycle ) > int64_t( _ui64Cycle ) ) { break; }
			uint32_t ui32BucketIdx = uint32_t( _ui64Cycle - sbBucket.ui64ApuStartCycle );
			if ( ui32BucketIdx >= LSN_SAMPLER_BUCKET_SIZE ) {
				// Bucket filled.
				++m_sblBuckets.ui64Idx;
				float fSample = Sample_6Point_5thOrder_Hermite_Z( sbBucket.fBucket, sbBucket.fInterp );
				m_vTmpBuffer[m_sTmpBufferIdx++] = fSample;
				if ( m_sTmpBufferIdx == m_vTmpBuffer.size() ) {
					TransferTmpToLocal();
				}
				continue;
			}
			sbBucket.fBucket[ui32BucketIdx] = _fSample;
		}
	}

	/**
	 * Starts the audio thread.
	 * 
	 * \return Returns true if the audio thread is started.
	 **/
	bool CAudio::StartThread() {
		StopThread();
		m_bRunThread = true;
		m_ptAudioThread = std::make_unique<std::thread>( AudioThread, nullptr );
		return m_ptAudioThread.get() != nullptr;
	}

	/**
	 * Stops the audio thread.
	 **/
	void CAudio::StopThread() {
		if ( nullptr == m_ptAudioThread.get() ) { return; }
		m_bRunThread = false;
		m_eThreadClosed.WaitForSignal();
		m_ptAudioThread->join();
		m_ptAudioThread.reset();
	}

	/**
	 * Flushes any audio currently buffered locally to the OpenAL API.
	 * 
	 * \return Returns true if the audio was buffered into OpenAL.
	 **/
	bool CAudio::Flush() {
		{
			//do {
				// Unqueue buffers before queuing the next.
				uint32_t ui32Processed = m_oasSource.BuffersProcessed();
			
				for ( uint32_t I = 0; I < ui32Processed; ++I ) {
					if ( m_oasSource.UnqueueBuffer( m_oabBuffers[(m_ui64TotalLifetimeUnqueueds)%LSN_AUDIO_BUFFERS].Id() ) ) {
						//m_oabBuffers[(m_ui64TotalLifetimeUnqueueds)%LSN_AUDIO_BUFFERS].Reset();
						++m_ui64TotalLifetimeUnqueueds;
					}
				}
			//} while ( m_ui64TotalLifetimeQueues - m_ui64TotalLifetimeUnqueueds >= LSN_AUDIO_BUFFERS );
		}
		
		if ( m_sCurBufferSize == 0 ) {		// Nothing to flush, no action to take.
			m_sFrequency = m_sNextFrequency;
			m_eFormat = m_eNextFormat;
			return true;
		}

		if ( m_ui64TotalLifetimeQueues - m_ui64TotalLifetimeUnqueueds >= LSN_AUDIO_BUFFERS ) {
			//m_sBufferIdx = (m_sBufferIdx + 1) % LSN_AUDIO_BUFFERS;
			m_sCurBufferSize = 0;
			m_sFrequency = m_sNextFrequency;
			m_eFormat = m_eNextFormat;
			return false;
		}

		//if ( !m_oabBuffers[m_sBufferIdx].CreateBuffer() ) { return false; }
		bool bRet = m_oabBuffers[m_sBufferIdx].BufferData( m_eFormat, m_vLocalBuffer.data(), m_sCurBufferSize, m_sFrequency );
		if ( bRet ) {
			if ( m_oasSource.QueueBuffer( m_oabBuffers[m_sBufferIdx].Id() ) ) {
				++m_ui64TotalLifetimeQueues;
				m_sBufferIdx = size_t( m_ui64TotalLifetimeQueues % LSN_AUDIO_BUFFERS );
				m_sCurBufferSize = 0;
				if ( m_oasSource.GetState() != AL_PLAYING && m_ui64TotalLifetimeQueues >= 2 ) {
					bRet = m_oasSource.Play();
				}
			}
			else {
				bRet = false;
			}
			//if ( !m_oasSource.SetLooping( AL_FALSE ) ) { return false; }
		}
		
		m_sFrequency = m_sNextFrequency;
		m_eFormat = m_eNextFormat;

		return bRet;
	}

	/**
	 * Buffers samples from floating-point to the AL_FORMAT_MONO8 format.
	 * 
	 * \param _pfSamples The samples to buffer.
	 * \param _sTotal The number of samples to which _pfSamples points.
	 * \return Returns true if the buffer succeeded.
	 **/
	bool CAudio::BufferMono8( const float * _pfSamples, size_t _sTotal ) {
		// Determine how many samples we can do before we need to flush.
		size_t stMax = m_sBufferSizeInSamples - (m_sCurBufferSize/* / sizeof( uint8_t )*/);
		while ( _sTotal >= stMax ) {
			// Convert and flush.
			uint8_t * pui8Dst = reinterpret_cast<uint8_t *>(&m_vLocalBuffer.data()[m_sCurBufferSize]);
			for ( size_t I = 0; I < stMax; ++I ) {
				(*pui8Dst++) = COpenAl::SampleToUi8( (*_pfSamples++) );
				++m_sCurBufferSize;
			}


			if ( !Flush() ) { return false; }
			_sTotal -= stMax;
			// Flushing can change the output format.
			switch ( m_eFormat ) {
				case AL_FORMAT_MONO16 : {
					return BufferMono16( _pfSamples, _sTotal );
				}
				case AL_FORMAT_MONO_FLOAT32 : {
					return BufferMonoF32( _pfSamples, _sTotal );
				}
				default : {}
			}
			stMax = m_sBufferSizeInSamples - (m_sCurBufferSize/* / sizeof( uint8_t )*/);
		}
		// Finish converting.
		uint8_t * pui8Dst = reinterpret_cast<uint8_t *>(&m_vLocalBuffer.data()[m_sCurBufferSize]);
		for ( size_t I = 0; I < _sTotal; ++I ) {
			(*pui8Dst++) = COpenAl::SampleToUi8( (*_pfSamples++) );
			++m_sCurBufferSize;
		}
		return true;
	}

	/**
	 * Buffers samples from floating-point to the AL_FORMAT_MONO16 format.
	 * 
	 * \param _pfSamples The samples to buffer.
	 * \param _sTotal The number of samples to which _pfSamples points.
	 * \return Returns true if the buffer succeeded.
	 **/
	bool CAudio::BufferMono16( const float * _pfSamples, size_t _sTotal ) {
		// Determine how many samples we can do before we need to flush.
		size_t stMax = m_sBufferSizeInSamples - (m_sCurBufferSize / sizeof( int16_t ));
		while ( _sTotal >= stMax ) {
			// Convert and flush.
			int16_t * pi16Dst = reinterpret_cast<int16_t *>(&m_vLocalBuffer.data()[m_sCurBufferSize]);
			for ( size_t I = 0; I < stMax; ++I ) {
				(*pi16Dst++) = COpenAl::SampleToI16( (*_pfSamples++) );
				m_sCurBufferSize += sizeof( int16_t );
			}


			if ( !Flush() ) { return false; }
			_sTotal -= stMax;
			// Flushing can change the output format.
			switch ( m_eFormat ) {
				case AL_FORMAT_MONO8 : {
					return BufferMono8( _pfSamples, _sTotal );
				}
				case AL_FORMAT_MONO_FLOAT32 : {
					return BufferMonoF32( _pfSamples, _sTotal );
				}
				default : {}
			}
			stMax = m_sBufferSizeInSamples - (m_sCurBufferSize / sizeof( int16_t ));
		}
		// Finish converting.
		int16_t * pi16Dst = reinterpret_cast<int16_t *>(&m_vLocalBuffer.data()[m_sCurBufferSize]);
		for ( size_t I = 0; I < _sTotal; ++I ) {
			(*pi16Dst++) = COpenAl::SampleToI16( (*_pfSamples++) );
			m_sCurBufferSize += sizeof( int16_t );
		}
		return true;
	}

	/**
	 * Buffers samples from floating-point to the AL_FORMAT_MONO_FLOAT32 format.
	 * 
	 * \param _pfSamples The samples to buffer.
	 * \param _sTotal The number of samples to which _pfSamples points.
	 * \return Returns true if the buffer succeeded.
	 **/
	bool CAudio::BufferMonoF32( const float * _pfSamples, size_t _sTotal ) {
		// Determine how many samples we can do before we need to flush.
		size_t stMax = m_sBufferSizeInSamples - (m_sCurBufferSize / sizeof( int16_t ));
		while ( _sTotal >= stMax ) {
			// Convert and flush.
			float * pfDst = reinterpret_cast<float *>(&m_vLocalBuffer.data()[m_sCurBufferSize]);
			for ( size_t I = 0; I < stMax; ++I ) {
				(*pfDst++) = (*_pfSamples++);
				m_sCurBufferSize += sizeof( float );
			}


			if ( !Flush() ) { return false; }
			_sTotal -= stMax;

			// Flushing can change the output format.
			switch ( m_eFormat ) {
				case AL_FORMAT_MONO8 : {
					return BufferMono8( _pfSamples, _sTotal );
				}
				case AL_FORMAT_MONO16 : {
					return BufferMono16( _pfSamples, _sTotal );
				}
				default : {}
			}
			
			stMax = m_sBufferSizeInSamples - (m_sCurBufferSize / sizeof( float ));
		}
		// Finish converting.
		float * pfDst = reinterpret_cast<float *>(&m_vLocalBuffer.data()[m_sCurBufferSize]);
		for ( size_t I = 0; I < _sTotal; ++I ) {
			(*pfDst++) = (*_pfSamples++);
			m_sCurBufferSize += sizeof( float );
		}
		return true;
	}

	/**
	 * Copies from m_vTmpBuffer into the local buffer passed to OpenAL, performing the format conversion as necessary.
	 * 
	 * \return Returns true if the transfer succeeded.
	 **/
	bool CAudio::TransferTmpToLocal() {
		auto aSize = m_sTmpBufferIdx;
		m_sTmpBufferIdx = 0;
		switch ( m_eFormat ) {
			case AL_FORMAT_MONO8 : {
				if ( !BufferMono8( m_vTmpBuffer.data(), aSize ) ) { return false; }
				break;
			}
			case AL_FORMAT_MONO16 : {
				if ( !BufferMono16( m_vTmpBuffer.data(), aSize ) ) { return false; }
				break;
			}
			case AL_FORMAT_MONO_FLOAT32 : {
				if ( !BufferMonoF32( m_vTmpBuffer.data(), aSize ) ) { return false; }
				break;
			}
			default : { return false; }
		}
		return true;
	}

	/**
	 * The audio thread.
	 *
	 * \param _pvParm Unused.
	 */
	void CAudio::AudioThread( void * /*_pvParm*/ ) {
		while ( m_bRunThread ) {
		}
		m_eThreadClosed.Signal();
	}

}	// namespace lsn

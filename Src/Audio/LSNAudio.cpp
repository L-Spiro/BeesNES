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
	ALsizei CAudio::m_sBufferSizeInSamples = 16;

	/** The current buffer position. */
	ALsizei CAudio::m_sCurBufferSize = 0;

	/** The frequency of the current buffer.  Flush to set (flushing copies from the “Next” value into this one). */
	ALsizei CAudio::m_sFrequency = 44100;

	/** The frequency to set after the next flush. */
	ALsizei CAudio::m_sNextFrequency = 44100;

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


		m_ptAudioThread = std::make_unique<std::thread>( AudioThread, nullptr );
		return true;
	}

	/**
	 * Shuts down the audio.
	 * 
	 * \return Returns true if shutdown was successful.
	 **/
	bool CAudio::ShutdownAudio() {
		m_bRunThread = false;
		m_eThreadClosed.WaitForSignal();
		m_ptAudioThread->join();
		m_ptAudioThread.reset();

		m_oasSource.Reset();
		m_oacContext.Reset();
		return m_oadDevice.Reset();
	}

	/**
	 * Flushes any audio currently buffered locally to the OpenAL API.
	 * 
	 * \return Returns true if the audio was buffered into OpenAL.
	 **/
	bool CAudio::Flush() {
		if ( m_sCurBufferSize == 0 ) { return true; }		// Nothing to flush, no action to take.

		bool bRet = m_oabBuffers[m_sBufferIdx].BufferData( m_eFormat, m_vLocalBuffer.data(), m_sCurBufferSize, m_sFrequency );

		m_sBufferIdx = (m_sBufferIdx + 1) % LSN_AUDIO_BUFFERS;
		m_sCurBufferSize = 0;
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
		}
		m_sCurBufferSize += static_cast<ALsizei>(_sTotal * sizeof( uint8_t ));
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
		}
		m_sCurBufferSize += static_cast<ALsizei>(_sTotal * sizeof( int16_t ));
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
		}
		m_sCurBufferSize += static_cast<ALsizei>(_sTotal * sizeof( float ));
		return true;
	}

	/**
	 * The audio thread.
	 *
	 * \param _pvParm Unused.
	 */
	void CAudio::AudioThread( void * _pvParm ) {
		while ( m_bRunThread ) {
		}
		m_eThreadClosed.Signal();
	}

}	// namespace lsn

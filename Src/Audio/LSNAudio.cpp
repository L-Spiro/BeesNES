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

	/** The frequency of the current buffer.  Flush before changing. */
	ALsizei CAudio::m_sFrequency = 44100;

	/** The current output format. Flush before changing. */
	ALenum CAudio::m_eFormat = AL_FORMAT_MONO16;

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
		if ( !COpenAl::DistanceModel( AL_NONE ) ) { return false; }

		if ( !m_oasSource.CreateSource() ) { return false; }

		return true;
	}

	/**
	 * Shuts down the audio.
	 * 
	 * \return Returns true if shutdown was successful.
	 **/
	bool CAudio::ShutdownAudio() {
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

		return bRet;
	}

	/**
	 * Buffers samples from floating-point to the AL_FORMAT_MONO8 format.
	 * 
	 * \return Returns true if the buffer succeeded.
	 **/
	bool CAudio::BufferMono8() {
		return true;
	}

}	// namespace lsn

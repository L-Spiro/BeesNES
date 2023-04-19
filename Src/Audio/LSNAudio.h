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
#include "LSNOpenAlBuffer.h"
#include "LSNOpenAlContext.h"
#include "LSNOpenAlDevice.h"
#include "LSNOpenAlSource.h"
#include "../Event/LSNEvent.h"
#include <thread>

#define LSN_AUDIO_BUFFERS									4

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


	protected :
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
		/** The frequency of the current buffer.  Flush to set (flushing copies from the “Next” value into this one). */
		static ALsizei										m_sFrequency;
		/** The frequency to set after the next flush. */
		static ALsizei										m_sNextFrequency;
		/** The current output format. Flush to set (flushing copies from the “Next” value into this one). */
		static ALenum										m_eFormat;
		/** The output format to apply on the next flush. */
		static ALenum										m_eNextFormat;
		/** The audio thread. */
		static std::unique_ptr<std::thread>					m_ptAudioThread;
		/** Boolean to stop the audio thread. */
		static std::atomic<bool>							m_bRunThread;
		/** The signal that the thread has finished. */
		static CEvent										m_eThreadClosed;
		

		
		// == Functions.
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
		 * The audio thread.
		 *
		 * \param _pvParm Unused.
		 */
		static void											AudioThread( void * _pvParm );

	};

}	// namespace lsn

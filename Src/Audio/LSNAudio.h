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

#define LSN_AUDIO_BUFFERS									3

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


	protected :
		// == Members.
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
		/** The frequency of the current buffer.  Flush before changing. */
		static ALsizei										m_sFrequency;
		/** The current output format. Flush before changing. */
		static ALenum										m_eFormat;
		

		
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
		 * \return Returns true if the buffer succeeded.
		 **/
		static bool											BufferMono8();
	};

}	// namespace lsn

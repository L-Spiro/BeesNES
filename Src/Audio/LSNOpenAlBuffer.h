/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Wraps an ALuint buffer ID.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNOpenAlDevice.h"
#include "LSNOpenAlInclude.h"

namespace lsn {

	/**
	 * Class COpenAlBuffer
	 * \brief Wraps an ALuint buffer ID.
	 *
	 * Description: Wraps an ALuint buffer ID.
	 */
	class COpenAlBuffer {
	public :
		COpenAlBuffer();
		~COpenAlBuffer();


		// == Functions.
		/**
		 * Gets the buffer ID.
		 * 
		 * \return Returns the buffer ID.
		 **/
		inline ALuint						Id() { return m_uiId; }

		/**
		 * Creates a buffer.
		 * 
		 * \return Returns true if the buffer was created.
		 **/
		bool								CreateBuffer();

		/**
		 * Destroys the buffer.
		 * 
		 * \return Returns true if the buffer was destroyed.
		 **/
		bool								Reset();

		/**
		 * Sets the buffer data.
		 * 
		 * \param _eFormat format type from among the following:
		 *	AL_FORMAT_MONO8
		 *	AL_FORMAT_MONO16
		 *	AL_FORMAT_STEREO8
		 *	AL_FORMAT_STEREO16 
		 * \param _pvData Pointer to the audio data.
		 * \param _sSize The size of the audio data in bytes.
		 * \param _sFreq The frequency of the audio data.
		 * \return Returns true if the buffer data was set.
		 **/
		bool								BufferData( ALenum _eFormat,
			const ALvoid * _pvData,
			ALsizei _sSize,
			ALsizei _sFreq );

	protected :
		// == Members.
		/** The buffer ID. */
		ALuint								m_uiId;
	};

}	// namespace lsn

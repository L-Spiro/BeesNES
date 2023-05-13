/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Wraps an ALuint buffer ID.
 */

#include "LSNOpenAlBuffer.h"
#include "LSNOpenAl.h"


namespace lsn {

	COpenAlBuffer::COpenAlBuffer() :
		m_uiId( 0 ) {
	}
	COpenAlBuffer::~COpenAlBuffer() {
		Reset();
	}


	// == Functions.
	/**
	 * Creates a buffer.
	 * 
	 * \return Returns true if the buffer was created.
	 **/
	bool COpenAlBuffer::CreateBuffer() {
		Reset();
		ALuint uiId = 0;
		if ( !COpenAl::alCall( ::alGenBuffers, ALsizei( 1 ), &uiId ) ) {
			return false;
		}
		m_uiId = uiId;
		return true;
	}

	/**
	 * Destroys the buffer.
	 * 
	 * \return Returns true if the buffer was destroyed.
	 **/
	bool COpenAlBuffer::Reset() {
		if ( m_uiId != 0 ) {
			if ( !COpenAl::alCall( ::alDeleteBuffers, ALsizei( 1 ), &m_uiId ) ) {
				return false;
			}

			m_uiId = 0;
		}
		return true;
	}

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
	bool COpenAlBuffer::BufferData( ALenum _eFormat,
		const ALvoid * _pvData,
		ALsizei _sSize,
		ALsizei _sFreq ) {
		if ( m_uiId == 0 ) {
			if ( !CreateBuffer() ) { return false; }
		}
		if ( m_uiId != 0 ) {
			if ( COpenAl::alCall( ::alBufferData, m_uiId, _eFormat, _pvData, _sSize, _sFreq ) ) {
				return true;
			}
		}
		return false;
	}

}	// namespace lsn

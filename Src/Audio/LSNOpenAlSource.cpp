/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Wraps an ALuint source ID.
 */

#include "LSNOpenAlSource.h"
#include "LSNOpenAl.h"

 namespace lsn {

	COpenAlSource::COpenAlSource() :
		m_uiId( 0 ) {
	}
	COpenAlSource::~COpenAlSource() {
		Reset();
	}


	// == Functions.
	/**
	 * Creates a source.
	 * 
	 * \return Returns true if the source was created.
	 **/
	bool COpenAlSource::CreateSource() {
		Reset();
		ALuint uiId = 0;
		if ( !COpenAl::alCall( ::alGenSources, ALsizei( 1 ), &uiId ) ) {
			return false;
		}
		m_uiId = uiId;
		return true;
	}

	/**
	 * Destroys the source.
	 * 
	 * \return Returns true if the source was destroyed.
	 **/
	bool COpenAlSource::Reset() {
		if ( m_uiId != 0 ) {
			if ( !COpenAl::alCall( ::alDeleteSources, ALsizei( 1 ), &m_uiId ) ) {
				return false;
			}

			m_uiId = 0;
		}
		return true;
	}

	/**
	 * Sets the source pitch.
	 * 
	 * \param _fValue The new value to set.
	 * \return Returns true if the value is set.
	 **/
	bool COpenAlSource::SetPitch( float _fValue ) {
		if ( m_uiId != 0 && COpenAl::alCall( ::alSourcef, m_uiId, AL_PITCH, ALfloat( _fValue ) ) ) {
			return true;
		}
		return false;
	}

	/**
	 * Sets the source gain.
	 * 
	 * \param _fValue The new value to set.
	 * \return Returns true if the value is set.
	 **/
	bool COpenAlSource::SetGain( float _fValue ) {
		if ( m_uiId != 0 && COpenAl::alCall( ::alSourcef, m_uiId, AL_GAIN, ALfloat( _fValue ) ) ) {
			return true;
		}
		return false;
	}

	/**
	 * Sets the source minimum gain.
	 * 
	 * \param _fValue The new value to set.
	 * \return Returns true if the value is set.
	 **/
	bool COpenAlSource::SetMinGain( float _fValue ) {
		if ( m_uiId != 0 && COpenAl::alCall( ::alSourcef, m_uiId, AL_MIN_GAIN, ALfloat( _fValue ) ) ) {
			return true;
		}
		return false;
	}

	/**
	 * Sets the source maximum gain.
	 * 
	 * \param _fValue The new value to set.
	 * \return Returns true if the value is set.
	 **/
	bool COpenAlSource::SetMaxGain( float _fValue ) {
		if ( m_uiId != 0 && COpenAl::alCall( ::alSourcef, m_uiId, AL_MAX_GAIN, ALfloat( _fValue ) ) ) {
			return true;
		}
		return false;
	}

	/**
	 * Sets the source maximum distance.
	 * 
	 * \param _fValue The new value to set.
	 * \return Returns true if the value is set.
	 **/
	bool COpenAlSource::SetMaxDistance( float _fValue ) {
		if ( m_uiId != 0 && COpenAl::alCall( ::alSourcef, m_uiId, AL_MAX_DISTANCE, ALfloat( _fValue ) ) ) {
			return true;
		}
		return false;
	}

	/**
	 * Sets the source roll-off factor.
	 * 
	 * \param _fValue The new value to set.
	 * \return Returns true if the value is set.
	 **/
	bool COpenAlSource::SetRolloffFactor( float _fValue ) {
		if ( m_uiId != 0 && COpenAl::alCall( ::alSourcef, m_uiId, AL_ROLLOFF_FACTOR, ALfloat( _fValue ) ) ) {
			return true;
		}
		return false;
	}

	/**
	 * Sets the source cone outer gain.
	 * 
	 * \param _fValue The new value to set.
	 * \return Returns true if the value is set.
	 **/
	bool COpenAlSource::SetConeOuterGain( float _fValue ) {
		if ( m_uiId != 0 && COpenAl::alCall( ::alSourcef, m_uiId, AL_CONE_OUTER_GAIN, ALfloat( _fValue ) ) ) {
			return true;
		}
		return false;
	}

	/**
	 * Sets the source cone inner angle in degrees.
	 * 
	 * \param _fValue The new value to set.
	 * \return Returns true if the value is set.
	 **/
	bool COpenAlSource::SetConeInnerAngle( float _fValue ) {
		if ( m_uiId != 0 && COpenAl::alCall( ::alSourcef, m_uiId, AL_CONE_INNER_ANGLE, ALfloat( _fValue ) ) ) {
			return true;
		}
		return false;
	}

	/**
	 * Sets the source cone outer angle in degrees.
	 * 
	 * \param _fValue The new value to set.
	 * \return Returns true if the value is set.
	 **/
	bool COpenAlSource::SetConeOuterAngle( float _fValue ) {
		if ( m_uiId != 0 && COpenAl::alCall( ::alSourcef, m_uiId, AL_CONE_OUTER_ANGLE, ALfloat( _fValue ) ) ) {
			return true;
		}
		return false;
	}

	/**
	 * Sets the source reference distance.
	 * 
	 * \param _fValue The new value to set.
	 * \return Returns true if the value is set.
	 **/
	bool COpenAlSource::SetReferenceDistance( float _fValue ) {
		if ( m_uiId != 0 && COpenAl::alCall( ::alSourcef, m_uiId, AL_REFERENCE_DISTANCE, ALfloat( _fValue ) ) ) {
			return true;
		}
		return false;
	}

	/**
	 * Sets the source position.
	 * 
	 * \param _fX The new X value to set.
	 * \param _fY The new Y value to set.
	 * \param _fZ The new Z value to set.
	 * \return Returns true if the value is set.
	 **/
	bool COpenAlSource::SetPosition( float _fX, float _fY, float _fZ ) {
		if ( m_uiId != 0 && COpenAl::alCall( ::alSource3f, m_uiId, AL_POSITION, ALfloat( _fX ), ALfloat( _fY ), ALfloat( _fZ ) ) ) {
			return true;
		}
		return false;
	}

	/**
	 * Sets the source velocity.
	 * 
	 * \param _fX The new X value to set.
	 * \param _fY The new Y value to set.
	 * \param _fZ The new Z value to set.
	 * \return Returns true if the value is set.
	 **/
	bool COpenAlSource::SetVelocity( float _fX, float _fY, float _fZ ) {
		if ( m_uiId != 0 && COpenAl::alCall( ::alSource3f, m_uiId, AL_VELOCITY, ALfloat( _fX ), ALfloat( _fY ), ALfloat( _fZ ) ) ) {
			return true;
		}
		return false;
	}

	/**
	 * Sets the source direction.
	 * 
	 * \param _fX The new X value to set.
	 * \param _fY The new Y value to set.
	 * \param _fZ The new Z value to set.
	 * \return Returns true if the value is set.
	 **/
	bool COpenAlSource::SetDirection( float _fX, float _fY, float _fZ ) {
		if ( m_uiId != 0 && COpenAl::alCall( ::alSource3f, m_uiId, AL_DIRECTION, ALfloat( _fX ), ALfloat( _fY ), ALfloat( _fZ ) ) ) {
			return true;
		}
		return false;
	}

	/**
	 * Sets whether the source is relative or not.
	 * 
	 * \param _i32Value The new value to set.
	 * \return Returns true if the value is set.
	 **/
	bool COpenAlSource::SetSourceIsRelative( int32_t _i32Value ) {
		if ( m_uiId != 0 && COpenAl::alCall( ::alSourcei, m_uiId, AL_SOURCE_RELATIVE, ALint( _i32Value ) ) ) {
			return true;
		}
		return false;
	}

	/**
	 * Sets the cone inner angle in degrees.
	 * 
	 * \param _i32Value The new value to set.
	 * \return Returns true if the value is set.
	 **/
	bool COpenAlSource::SetConeInnerAngle( int32_t _i32Value ) {
		if ( m_uiId != 0 && COpenAl::alCall( ::alSourcei, m_uiId, AL_CONE_INNER_ANGLE, ALint( _i32Value ) ) ) {
			return true;
		}
		return false;
	}

	/**
	 * Sets the cone outer angle in degrees.
	 * 
	 * \param _i32Value The new value to set.
	 * \return Returns true if the value is set.
	 **/
	bool COpenAlSource::SetConeOuterAngle( int32_t _i32Value ) {
		if ( m_uiId != 0 && COpenAl::alCall( ::alSourcei, m_uiId, AL_CONE_OUTER_ANGLE, ALint( _i32Value ) ) ) {
			return true;
		}
		return false;
	}

	/**
	 * Sets looping on or off.
	 * 
	 * \param _i32Value The new value to set.
	 * \return Returns true if the value is set.
	 **/
	bool COpenAlSource::SetLooping( int32_t _i32Value ) {
		if ( m_uiId != 0 && COpenAl::alCall( ::alSourcei, m_uiId, AL_LOOPING, ALint( _i32Value ) ) ) {
			return true;
		}
		return false;
	}

	/**
	 * Sets the buffer.
	 * 
	 * \param _uiValue The new value to set.
	 * \return Returns true if the value is set.
	 **/
	bool COpenAlSource::SetBuffer( ALuint _uiValue ) {
		if ( m_uiId != 0 && COpenAl::alCall( ::alSourcei, m_uiId, AL_BUFFER, ALint( _uiValue ) ) ) {
			return true;
		}
		return false;
	}

	/**
	 * Gets the current state of the source.  If the source is invalid, ALuint( -1 ) is returned.
	 * 
	 * \return Returns the current state of the source or ALuint( -1 ).
	 **/
	ALuint COpenAlSource::GetBuffer() const {
		ALint iVal;
		if ( m_uiId != 0 && COpenAl::alCall( ::alGetSourcei, m_uiId, AL_BUFFER, &iVal ) ) {
			return ALuint( iVal );
		}
		return ALuint( -1 );
	}

	/**
	 * Sets the source state.
	 * 
	 * \param _i32Value The new value to set.
	 * \return Returns true if the value is set.
	 **/
	bool COpenAlSource::SetState( int32_t _i32Value ) {
		if ( m_uiId != 0 && COpenAl::alCall( ::alSourcei, m_uiId, AL_SOURCE_STATE, ALint( _i32Value ) ) ) {
			return true;
		}
		return false;
	}

	/**
	 * Gets the current state of the source.  If the source is invalid, -1 is returned.
	 * 
	 * \return Returns the current state of the source or -1.
	 **/
	int32_t COpenAlSource::GetState() const {
		ALint iVal;
		if ( m_uiId != 0 && COpenAl::alCall( ::alGetSourcei, m_uiId, AL_SOURCE_STATE, &iVal ) ) {
			return int32_t( iVal );
		}
		return -1;
	}

	/**
	 * Starts playing its buffer.
	 *	The playing source will have its state changed to AL_PLAYING. When called on a source
	 *	which is already playing, the source will restart at the beginning. When the attached
	 *	buffer(s) are done playing, the source will progress to the AL_STOPPED state.
	 * 
	 * \return Returns true if the buffer associated with this source starts playing.
	 **/
	bool COpenAlSource::Play() {
		if ( m_uiId != 0 && COpenAl::alCall( ::alSourcePlay, m_uiId ) ) {
			return true;
		}
		return false;
	}

	/**
	 * Pauses playing its buffer.  Internal state becomes AL_PAUSED.
	 * 
	 * \return Returns true if the buffer associated with this source pauses playing.
	 **/
	bool COpenAlSource::Pause() {
		if ( m_uiId != 0 && COpenAl::alCall( ::alSourcePause, m_uiId ) ) {
			return true;
		}
		return false;
	}

	/**
	 * Stops playing its buffer.  Internal state becomes AL_STOPPED.
	 * 
	 * \return Returns true if the buffer associated with this source stops playing.
	 **/
	bool COpenAlSource::Stop() {
		if ( m_uiId != 0 && COpenAl::alCall( ::alSourceStop, m_uiId ) ) {
			return true;
		}
		return false;
	}

	/**
	 * Rewinds buffer.  Internal state becomes AL_INITIAL.
	 * 
	 * \return Returns true if the buffer associated with this source is rewound.
	 **/
	bool COpenAlSource::Rewind() {
		if ( m_uiId != 0 && COpenAl::alCall( ::alSourceRewind, m_uiId ) ) {
			return true;
		}
		return false;
	}

	/**
	 * Queues a buffer for playback.
	 * 
	 * \param _uiId The buffer ID to enqueue.
	 * \return Returns true if the buffer is enqueued.
	 **/
	bool COpenAlSource::QueueBuffer( ALuint _uiId ) {
		if ( m_uiId != 0 && COpenAl::alCall( ::alSourceQueueBuffers, m_uiId, 1, &_uiId ) ) {
			return true;
		}
		return false;
	}

	/**
	 * Unqueues a buffer.
	 * 
	 * \param _uiId The buffer ID to unqueue.
	 * \return Returns true if the buffer is unqueued.
	 **/
	bool COpenAlSource::UnqueueBuffer( ALuint _uiId ) {
		if ( m_uiId != 0 && COpenAl::alCall( ::alSourceUnqueueBuffers, m_uiId, 1, &_uiId ) ) {
			return true;
		}
		return false;
	}

	/**
	 * Gets the number of buffers queued.  If the source is invalid, uint32_t( -1 ) is returned.
	 * 
	 * \return Returns the number of buffers queued or uint32_t( -1 ).
	 **/
	uint32_t COpenAlSource::BuffersQueued() const {
		ALint iVal;
		if ( m_uiId != 0 && COpenAl::alCall( ::alGetSourcei, m_uiId, AL_BUFFERS_QUEUED, &iVal ) ) {
			return uint32_t( iVal );
		}
		return uint32_t( -1 );
	}

	/**
	 * Gets the number of buffers processed.  If the source is invalid, uint32_t( -1 ) is returned.
	 * 
	 * \return Returns the number of buffers processed or uint32_t( -1 ).
	 **/
	uint32_t COpenAlSource::BuffersProcessed() const {
		ALint iVal;
		if ( m_uiId != 0 && COpenAl::alCall( ::alGetSourcei, m_uiId, AL_BUFFERS_PROCESSED, &iVal ) ) {
			return uint32_t( iVal );
		}
		return uint32_t( -1 );
	}

}	// namespace lsn

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Wraps an ALuint source ID.
 */


#pragma once

#include "../../LSNLSpiroNes.h"

#ifdef LSN_WINDOWS

#include "LSNOpenAlDevice.h"
#include "LSNOpenAlInclude.h"

namespace lsn {

	/**
	 * Class COpenAlSource
	 * \brief Wraps an ALuint source ID.
	 *
	 * Description: Wraps an ALuint source ID.
	 */
	class COpenAlSource {
	public :
		COpenAlSource();
		~COpenAlSource();


		// == Functions.
		/**
		 * Gets the source ID.
		 * 
		 * \return Returns the source ID.
		 **/
		inline ALuint						Id() { return m_uiId; }

		/**
		 * Creates a source.
		 * 
		 * \return Returns true if the source was created.
		 **/
		bool								CreateSource();

		/**
		 * Destroys the source.
		 * 
		 * \return Returns true if the source was destroyed.
		 **/
		bool								Reset();

		/**
		 * Sets the source pitch.
		 * 
		 * \param _fValue The new value to set.
		 * \return Returns true if the value is set.
		 **/
		bool								SetPitch( float _fValue );

		/**
		 * Sets the source gain.
		 * 
		 * \param _fValue The new value to set.
		 * \return Returns true if the value is set.
		 **/
		bool								SetGain( float _fValue );

		/**
		 * Sets the source minimum gain.
		 * 
		 * \param _fValue The new value to set.
		 * \return Returns true if the value is set.
		 **/
		bool								SetMinGain( float _fValue );

		/**
		 * Sets the source maximum gain.
		 * 
		 * \param _fValue The new value to set.
		 * \return Returns true if the value is set.
		 **/
		bool								SetMaxGain( float _fValue );

		/**
		 * Sets the source maximum distance.
		 * 
		 * \param _fValue The new value to set.
		 * \return Returns true if the value is set.
		 **/
		bool								SetMaxDistance( float _fValue );

		/**
		 * Sets the source roll-off factor.
		 * 
		 * \param _fValue The new value to set.
		 * \return Returns true if the value is set.
		 **/
		bool								SetRolloffFactor( float _fValue );

		/**
		 * Sets the source cone outer gain.
		 * 
		 * \param _fValue The new value to set.
		 * \return Returns true if the value is set.
		 **/
		bool								SetConeOuterGain( float _fValue );

		/**
		 * Sets the source cone inner angle in degrees.
		 * 
		 * \param _fValue The new value to set.
		 * \return Returns true if the value is set.
		 **/
		bool								SetConeInnerAngle( float _fValue );

		/**
		 * Sets the source cone outer angle in degrees.
		 * 
		 * \param _fValue The new value to set.
		 * \return Returns true if the value is set.
		 **/
		bool								SetConeOuterAngle( float _fValue );

		/**
		 * Sets the source reference distance.
		 * 
		 * \param _fValue The new value to set.
		 * \return Returns true if the value is set.
		 **/
		bool								SetReferenceDistance( float _fValue );

		/**
		 * Sets the source position.
		 * 
		 * \param _fX The new X value to set.
		 * \param _fY The new Y value to set.
		 * \param _fZ The new Z value to set.
		 * \return Returns true if the value is set.
		 **/
		bool								SetPosition( float _fX, float _fY, float _fZ );

		/**
		 * Sets the source velocity.
		 * 
		 * \param _fX The new X value to set.
		 * \param _fY The new Y value to set.
		 * \param _fZ The new Z value to set.
		 * \return Returns true if the value is set.
		 **/
		bool								SetVelocity( float _fX, float _fY, float _fZ );

		/**
		 * Sets the source direction.
		 * 
		 * \param _fX The new X value to set.
		 * \param _fY The new Y value to set.
		 * \param _fZ The new Z value to set.
		 * \return Returns true if the value is set.
		 **/
		bool								SetDirection( float _fX, float _fY, float _fZ );

		/**
		 * Sets whether the source is relative or not.
		 * 
		 * \param _i32Value The new value to set.
		 * \return Returns true if the value is set.
		 **/
		bool								SetSourceIsRelative( int32_t _i32Value );

		/**
		 * Sets the cone inner angle in degrees.
		 * 
		 * \param _i32Value The new value to set.
		 * \return Returns true if the value is set.
		 **/
		bool								SetConeInnerAngle( int32_t _i32Value );

		/**
		 * Sets the cone outer angle in degrees.
		 * 
		 * \param _i32Value The new value to set.
		 * \return Returns true if the value is set.
		 **/
		bool								SetConeOuterAngle( int32_t _i32Value );

		/**
		 * Sets looping on or off.
		 * 
		 * \param _i32Value The new value to set.
		 * \return Returns true if the value is set.
		 **/
		bool								SetLooping( int32_t _i32Value );

		/**
		 * Sets the buffer.
		 * 
		 * \param _uiValue The new value to set.
		 * \return Returns true if the value is set.
		 **/
		bool								SetBuffer( ALuint _uiValue );

		/**
		 * Gets the current state of the source.  If the source is invalid, ALuint( -1 ) is returned.
		 * 
		 * \return Returns the current state of the source or ALuint( -1 ).
		 **/
		ALuint								GetBuffer() const;

		/**
		 * Sets the source state.
		 * 
		 * \param _i32Value The new value to set.  Values are:
		 *	AL_INITIAL
		 *	AL_PLAYING
		 *	AL_PAUSED
		 *	AL_STOPPED
		 * \return Returns true if the value is set.
		 **/
		bool								SetState( int32_t _i32Value );

		/**
		 * Gets the current state of the source.  If the source is invalid, -1 is returned.
		 * 
		 * \return Returns the current state of the source or -1.
		 **/
		int32_t								GetState() const;

		/**
		 * Starts playing its buffer.
		 *	The playing source will have its state changed to AL_PLAYING. When called on a source
		 *	which is already playing, the source will restart at the beginning. When the attached
		 *	buffer(s) are done playing, the source will progress to the AL_STOPPED state.
		 * 
		 * \return Returns true if the buffer associated with this source starts playing.
		 **/
		bool								Play();

		/**
		 * Pauses playing its buffer.  Internal state becomes AL_PAUSED.
		 * 
		 * \return Returns true if the buffer associated with this source pauses playing.
		 **/
		bool								Pause();

		/**
		 * Stops playing its buffer.  Internal state becomes AL_STOPPED.
		 * 
		 * \return Returns true if the buffer associated with this source stops playing.
		 **/
		bool								Stop();

		/**
		 * Rewinds buffer.  Internal state becomes AL_INITIAL.
		 * 
		 * \return Returns true if the buffer associated with this source is rewound.
		 **/
		bool								Rewind();

		/**
		 * Queues a buffer for playback.
		 * 
		 * \param _uiId The buffer ID to enqueue.
		 * \return Returns true if the buffer is enqueued.
		 **/
		bool								QueueBuffer( ALuint _uiId );

		/**
		 * Unqueues a buffer.
		 * 
		 * \param _uiId The buffer ID to unqueue.
		 * \return Returns true if the buffer is unqueued.
		 **/
		bool								UnqueueBuffer( ALuint _uiId );

		/**
		 * Gets the number of buffers queued.  If the source is invalid, uint32_t( -1 ) is returned.
		 * 
		 * \return Returns the number of buffers queued or uint32_t( -1 ).
		 **/
		uint32_t							BuffersQueued() const;

		/**
		 * Gets the number of buffers processed.  If the source is invalid, uint32_t( -1 ) is returned.
		 * 
		 * \return Returns the number of buffers processed or uint32_t( -1 ).
		 **/
		uint32_t							BuffersProcessed() const;

	protected :
		// == Members.
		/** The source ID. */
		ALuint								m_uiId;
	};

}	// namespace lsn

#endif	// #ifdef LSN_WINDOWS

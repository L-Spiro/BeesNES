/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The implementation of the OpenAL audio system.
 */


#pragma once

#include "../LSNLSpiroNes.h"

#ifdef LSN_WINDOWS

#include "LSNAudioBase.h"
#include "OpenAL/LSNOpenAlBuffer.h"
#include "OpenAL/LSNOpenAlContext.h"
#include "OpenAL/LSNOpenAlDevice.h"
#include "OpenAL/LSNOpenAlSource.h"

namespace lsn {

	/**
	 * Class CAudioOpenAl
	 * \brief The implementation of the OpenAL audio system.
	 *
	 * Description: The implementation of the OpenAL audio system.
	 */
	class CAudioOpenAl : public CAudioBase {
	public :
		// == Functions.
		/**
		 * Initializes audio.
		 * 
		 * \param _ui32Device The audio device to use.
		 * \return Returns true if initialization was successful.
		 **/
		virtual bool										InitializeAudio( uint32_t _ui32Device );

		/**
		 * Shuts down the audio.
		 * 
		 * \return Returns true if shutdown was successful.
		 **/
		virtual bool										ShutdownAudio();

		/**
		 * Gets the global OpenAL device.
		 * 
		 * \return Returns the global OpenAL device.
		 **/
		COpenAlDevice &										Device() { return m_oadDevice; }

		/**
		 * Gets the global OpenAL source.
		 * 
		 * \return Returns the global OpenAL source.
		 **/
		COpenAlSource &										Source() { return m_oasSource; }

		/**
		 * Called when emulation begins.  Resets the ring buffer of buckets.
		 **/
		virtual void										BeginEmulation();

		/**
		 * Gets a list of audio devices.
		 * 
		 * \return Returns a vector of strings listing the audio devices.
		 **/
		virtual std::vector<std::u16string>					GetAudioDevices();

		/**
		 * Gets all available audio formats and Hz.  The top byte contains the sample format while the bottom 3 bytes contain the Hz in 25ths.
		 * 
		 * \return Returns a vector of formats and Hz formatted such that the high 8 bits are the format index and the lower 24 bits are the Hz in units of 25.
		 **/
		virtual std::vector<uint32_t>						GetAudioFormatsAndHz();


	protected :
		// == Members.
		/** The primary OpenAL device. */
		COpenAlDevice										m_oadDevice;
		/** The context. */
		COpenAlContext										m_oacContext;
		/** For emulation purposes, we only have 1 source. */
		COpenAlSource										m_oasSource;
		/** The audio buffers. */
		COpenAlBuffer										m_oabBuffers[LSN_AUDIO_BUFFERS];
		/** The current output format. Flush to set (flushing copies from the "Next" value into this one). */
		ALenum												m_eFormat = AL_FORMAT_MONO16;


		// == Functions.
		/**
		 * Undirties the format.  Called only when the new format differs from the old.
		 * 
		 * \param _fFormat The new format to set.
		 **/
		virtual void										UndirtyFormat( const LSN_AUDIO_FORMAT &_fFormat );

		/**
		 * Unqueues a buffer by index.
		 * 
		 * \param _ui64Idx The buffer index to unqueue.  It is derived from the number of buffers queued and will almost always be larger than LSN_AUDIO_BUFFERS.
		 * \return Returns true if the unqueing succeeded.
		 **/
		virtual bool										UnqueueBuffer( uint64_t _ui64Idx ) {
			return m_oasSource.UnqueueBuffer( m_oabBuffers[_ui64Idx%LSN_AUDIO_BUFFERS].Id() );
		}

		/**
		 * Buffers and queues the given local data.
		 * 
		 * \param _ui64Idx The global number of queues that have been made, which can be used to derive a buffer index into which to queue the given data.
		 * \param _pvData The data to buffer and queue.
		 * \param _sSizeInBytes The size, in bytes, of the data to which _pvData points.
		 * \param _ui32Freq The frequency of the given data.
		 * \return Returns true if buffering and queuing succeed.
		 **/
		virtual bool										QueueBuffer( uint64_t _ui64Idx, const void * _pvData, size_t _sSizeInBytes, uint32_t _ui32Freq );

		/**
		 * Are the buffers playing?
		 *
		 * \return Returns true if the audio source is streaming through the queued buffers at the moment.
		 **/
		virtual bool										IsPlaying() const { return m_oasSource.GetState() == AL_PLAYING; }

		/**
		 * Begins playing the queued buffers.
		 * 
		 * \return Returns false if there is an error playing the buffers.
		 **/
		virtual bool										Play() { return m_oasSource.Play(); }

		/**
		 * Buffers samples from floating-point to the AL_FORMAT_MONO8 format.
		 * 
		 * \param _pfSamples The samples to buffer.
		 * \param _sTotal The number of samples to which _pfSamples points.
		 * \return Returns true if the buffer succeeded.
		 **/
		virtual bool										BufferMono8( const float * _pfSamples, size_t _sTotal );

		/**
		 * Buffers samples from floating-point to the AL_FORMAT_MONO16 format.
		 * 
		 * \param _pfSamples The samples to buffer.
		 * \param _sTotal The number of samples to which _pfSamples points.
		 * \return Returns true if the buffer succeeded.
		 **/
		virtual bool										BufferMono16( const float * _pfSamples, size_t _sTotal );

		/**
		 * Buffers samples from floating-point to the AL_FORMAT_MONO_FLOAT32 format.
		 * 
		 * \param _pfSamples The samples to buffer.
		 * \param _sTotal The number of samples to which _pfSamples points.
		 * \return Returns true if the buffer succeeded.
		 **/
		virtual bool										BufferMonoF32( const float * _pfSamples, size_t _sTotal );

		/**
		 * Gets the total number of buffers processed.
		 *
		 * \return Returns the total number of buffers processed.
		 **/
		virtual uint32_t									BuffersProcessed() const { return m_oasSource.BuffersProcessed(); }

		/**
		 * Gets the OpenAL format enum given a general format descriptor.
		 * 
		 * \param _sfFormat The format descriptor.
		 * \return Returns the OpenAL enum that represents _fFormat.
		 **/
		static ALenum										FormatToEnum( const LSN_SAMPLE_FORMAT &_sfFormat );


	private :
		// == Types.
		typedef CAudioBase									Parent;
	};

}	// namespace lsn

#endif  // #ifdef LSN_WINDOWS

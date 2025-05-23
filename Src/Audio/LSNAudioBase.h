/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Base class for audio.  Provides an interface for generic audio functions such as converting samples, flushing buffers, etc.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNAudioOptions.h"
#include "OpenAL/LSNOpenAlBuffer.h"
#include "OpenAL/LSNOpenAlContext.h"
#include "OpenAL/LSNOpenAlDevice.h"
#include "OpenAL/LSNOpenAlSource.h"

#include <vector>

#define LSN_AUDIO_BUFFERS									288

#define LSN_BUFFER_DELAY									2		// How many buffers to fill before beginning to play audio.  Reduces early crackling when set higher.
//#define LSN_BUFFER_DELAY									4		// For personal recording needs where audio delay is not important.

namespace lsn {

	/**
	 * Class CAudioBase
	 * \brief Base class for audio.
	 *
	 * Description: Base class for audio.  Provides an interface for generic audio functions such as converting samples, flushing buffers, etc.
	 */
	class CAudioBase {
	public :
		// == Functions.
		/**
		 * Initializes audio.
		 * 
		 * \param _ui32Device The audio device to use.
		 * \return Returns true if initialization was successful.
		 **/
		virtual bool										InitializeAudio( uint32_t /*_ui32Device*/ ) {
			try {
				m_vTmpBuffer.resize( m_sBufferSizeInSamples );
				m_vLocalBuffer.resize( m_sBufferSizeInSamples * 4 );
			}
			catch ( ... ) { return false; }
			return true;
		}

		/**
		 * Shuts down the audio.
		 * 
		 * \return Returns true if shutdown was successful.
		 **/
		virtual bool										ShutdownAudio() { return false; }

		/**
		 * Gets the output frequency.
		 * 
		 * \return Returns the output frequency.
		 **/
		inline uint32_t										GetOutputFrequency() { return m_ui32Frequency; }

		/**
		 * Sets the output frequency in Hz.
		 * 
		 * \param _ui32Hz The new output frequency.
		 **/
		void												SetOutputFrequency( uint32_t _ui32Hz ) {
			m_ui32NextFrequency = _ui32Hz;
		}

		/**
		 * Sets the output format.
		 * 
		 * \param _fFormat The new output format.
		 **/
		void												SetOutputFormat( LSN_AUDIO_FORMAT _fFormat ) {
			m_fNextFormat = _fFormat;
		}

		/**
		 * Sets whether to dither 16-bit PCM or not.
		 * 
		 * \param _bDither Whether to dither 16-bit PCM.
		 **/
		void												SetDither( bool _bDither ) {
			m_bDither = _bDither;
		}

		/**
		 * Called when emulation begins.  Resets the ring buffer of buckets.
		 **/
		virtual void										BeginEmulation();

		/**
		 * Adds a sample to the audio device.
		 *
		 * \param _fSample The audio sample to be added.
		 **/
		virtual void										AddSample( float _fSample );

		/**
		 * Gets a list of audio devices.
		 * 
		 * \return Returns a vector of strings listing the audio devices.
		 **/
		virtual std::vector<std::u16string>					GetAudioDevices() { return std::vector<std::u16string>(); }

		/**
		 * Gets all available audio formats and Hz.  The top byte contains the sample format while the bottom 3 bytes contain the Hz in thousands.
		 * 
		 * \return Returns a vector of formats and Hz formatted such that the high 8 bits are the format index and the lower 24 bits are the Hz in units of 1,000.
		 **/
		virtual std::vector<uint32_t>						GetAudioFormatsAndHz() { return std::vector<uint32_t>(); }


	protected :
		// == Members.
		/** The total number of buffers uploaded during the lifetime of the source. */
		uint64_t											m_ui64TotalLifetimeQueues = 0;
		/** The total number of buffers unloaded during the lifetime of the source. */
		uint64_t											m_ui64TotalLifetimeUnqueueds = 0;
		/** The current buffer. */
		std::vector<uint8_t>								m_vLocalBuffer;
		/** Temporary float-format storage of samples. */
		std::vector<float>									m_vTmpBuffer;
		/** The position within the temporary buffer of the current sample. */
		size_t												m_sTmpBufferIdx = 0;
		/** The frequency of the current buffer.  Flush to set (flushing copies from the "Next" value into this one). */
		uint32_t											m_ui32Frequency = 44100;
		/** The frequency to set after the next flush. */
		uint32_t											m_ui32NextFrequency = 44100;
		/** The size of each buffer in samples. */
		size_t												m_sBufferSizeInSamples = 
#ifdef LSN_WIN64
			//768
			128
#else
			//1024 
			256
#endif
		;
		/** The current buffer position. */
		size_t												m_sCurBufferSize = 0;
		/** The current format. */
		LSN_AUDIO_FORMAT									m_fFormat;
		/** The next format. */
		LSN_AUDIO_FORMAT									m_fNextFormat;
		/** The current device. */
		uint32_t											m_ui32CurDevice = 0;
		/** The next device. */
		uint32_t											m_ui32Device = 0;
		/** The error-diffusion state. */
		float												m_fDitherError = 0.0f;
		/** Use dithering. */
		bool												m_bDither = false;


		// == Functions.
		/**
		 * Flushes any audio currently buffered locally to the native audio API.
		 * 
		 * \return Returns true if the audio was buffered into the native audio API.
		 **/
		virtual bool										Flush();

		/**
		 * Undirties the settings state (frequency and format).
		 **/
		virtual void										UndirtyState();

		/**
		 * Undirties the frequency.  Called only when the new frequency differs from the old.
		 * 
		 * \param _ui32Freq The new frequency to set.
		 **/
		virtual void										UndirtyFreq( uint32_t _ui32Freq );

		/**
		 * Undirties the format.  Called only when the new format differs from the old.
		 * 
		 * \param _fFormat The new format to set.
		 **/
		virtual void										UndirtyFormat( const LSN_AUDIO_FORMAT &_fFormat );

		/**
		 * Copies from m_vTmpBuffer into the local buffer passed to the native audio API, performing the format conversion as necessary.
		 * 
		 * \return Returns true if the transfer succeeded.
		 **/
		virtual bool										TransferTmpToLocal();

		/**
		 * Unqueues a buffer by index.
		 * 
		 * \param _ui64Idx The buffer index to unqueue.  It is derived from the number of buffers queued and will almost always be larger than LSN_AUDIO_BUFFERS.
		 * \return Returns true if the unqueing succeeded.
		 **/
		virtual bool										UnqueueBuffer( uint64_t /*_ui64Idx*/ ) { return false; }

		/**
		 * Buffers and queues the given local data.
		 * 
		 * \param _ui64Idx The global number of queues that have been made, which can be used to derive a buffer index into which to queue the given data.
		 * \param _pvData The data to buffer and queue.
		 * \param _sSizeInBytes The size, in bytes, of the data to which _pvData points.
		 * \param _ui32Freq The frequency of the given data.
		 * \return Returns true if buffering and queuing succeed.
		 **/
		virtual bool										QueueBuffer( uint64_t /*_ui64Idx*/, const void * /*_pvData*/, size_t /*_sSizeInBytes*/, uint32_t /*_ui32Freq*/ ) { return false; }

		/**
		 * Are the buffers playing?
		 *
		 * \return Returns true if the audio source is streaming through the queued buffers at the moment.
		 **/
		virtual bool										IsPlaying() const { return false; }

		/**
		 * Begins playing the queued buffers.
		 * 
		 * \return Returns false if there is an error playing the buffers.
		 **/
		virtual bool										Play() { return false; }

		/**
		 * Buffers samples from floating-point to the s8 format.
		 * 
		 * \param _pfSamples The samples to buffer.
		 * \param _sTotal The number of samples to which _pfSamples points.
		 * \return Returns true if the buffer succeeded.
		 **/
		virtual bool										BufferMono8( const float * /*_pfSamples*/, size_t /*_sTotal*/ ) { return false; }

		/**
		 * Buffers samples from floating-point to the s16 format.
		 * 
		 * \param _pfSamples The samples to buffer.
		 * \param _sTotal The number of samples to which _pfSamples points.
		 * \return Returns true if the buffer succeeded.
		 **/
		virtual bool										BufferMono16( const float * /*_pfSamples*/, size_t /*_sTotal*/ ) { return false; }

		/**
		 * Buffers samples from floating-point to the f32 format.
		 * 
		 * \param _pfSamples The samples to buffer.
		 * \param _sTotal The number of samples to which _pfSamples points.
		 * \return Returns true if the buffer succeeded.
		 **/
		virtual bool										BufferMonoF32( const float * /*_pfSamples*/, size_t /*_sTotal*/ ) { return false; }

		/**
		 * Gets the total number of buffers processed.
		 *
		 * \return Returns the total number of buffers processed.
		 **/
		virtual uint32_t									BuffersProcessed() const { return 0; }
	};

}	// namespace lsn

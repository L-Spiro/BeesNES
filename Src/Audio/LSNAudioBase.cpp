/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Base class for audio.  Provides an interface for generic audio functions such as converting samples, flushing buffers, etc.
 */


#include "LSNAudioBase.h"

namespace lsn {

	// == Functions.
	/**
	 * Called when emulation begins.  Resets the ring buffer of buckets.
	 **/
	void CAudioBase::BeginEmulation() {
		if ( m_ui64TotalLifetimeUnqueueds == m_ui64TotalLifetimeQueues ) {
			m_ui64TotalLifetimeUnqueueds = m_ui64TotalLifetimeQueues = 0;
			m_sCurBufferSize = 0;
			UndirtyState();
		}
	}

	/**
	 * Adds a sample to the audio device.
	 *
	 * \param _fSample The audio sample to be added.
	 **/
	void CAudioBase::AddSample( float _fSample ) {
		m_vTmpBuffer[m_sTmpBufferIdx++] = _fSample;
		if ( m_sTmpBufferIdx == m_vTmpBuffer.size() ) {
			TransferTmpToLocal();
		}
	}

	/**
	 * Flushes any audio currently buffered locally to the native audio API.
	 * 
	 * \return Returns true if the audio was buffered into the native audio API.
	 **/
	bool CAudioBase::Flush() {
		{
			// Unqueue buffers before queuing the next.
			uint32_t ui32Processed = BuffersProcessed();
			
			for ( uint32_t I = 0; I < ui32Processed; ++I ) {
				if ( UnqueueBuffer( m_ui64TotalLifetimeUnqueueds ) ) {
					++m_ui64TotalLifetimeUnqueueds;
				}
			}
		}
		
		if ( m_sCurBufferSize == 0 ) {
			// Nothing to flush, no action to take.
			UndirtyState();
			return true;
		}

		if ( m_ui64TotalLifetimeQueues - m_ui64TotalLifetimeUnqueueds >= LSN_AUDIO_BUFFERS ) {
			// No buffers available.  Samples are being dropped.
			m_sCurBufferSize = 0;
			UndirtyState();
			return false;
		}

		bool bRet = false;
		if ( QueueBuffer( m_ui64TotalLifetimeQueues, m_vLocalBuffer.data(), m_sCurBufferSize, m_ui32Frequency ) ) {
			++m_ui64TotalLifetimeQueues;
			m_sCurBufferSize = 0;
			if ( !IsPlaying() && m_ui64TotalLifetimeQueues >= LSN_BUFFER_DELAY ) {
				bRet = Play();
			}
		}
		
		UndirtyState();
		return bRet;
	}

	/**
	 * Undirties the settings state (frequency and format).
	 **/
	void CAudioBase::UndirtyState() {
		if ( m_ui32Frequency != m_ui32NextFrequency ) { UndirtyFreq( m_ui32NextFrequency ); }
		if ( m_fFormat != m_fNextFormat ) { UndirtyFormat( m_fNextFormat ); }
	}

	/**
	 * Undirties the frequency.  Called only when the new frequency differs from the old.
	 * 
	 * \param _ui32Freq The new frequency to set.
	 **/
	void CAudioBase::UndirtyFreq( uint32_t _ui32Freq ) {
		m_ui32Frequency = _ui32Freq;
	}

	/**
	 * Undirties the format.  Called only when the new format differs from the old.
	 * 
	 * \param _fFormat The new format to set.
	 **/
	void CAudioBase::UndirtyFormat( const LSN_AUDIO_FORMAT &_fFormat ) {
		m_fFormat = _fFormat;
	}

	/**
	 * Copies from m_vTmpBuffer into the local buffer passed to OpenAL, performing the format conversion as necessary.
	 * 
	 * \return Returns true if the transfer succeeded.
	 **/
	bool CAudioBase::TransferTmpToLocal() {
		auto aSize = m_sTmpBufferIdx;
		m_sTmpBufferIdx = 0;
		switch ( m_fFormat.sfFormat ) {
			case LSN_SF_PCM : {
				switch ( m_fFormat.ui16BitsPerChannel ) {
					case 8 : {
						if ( !BufferMono8( m_vTmpBuffer.data(), aSize ) ) { return false; }
						break;
					}
					case 16 : {
						if ( !BufferMono16( m_vTmpBuffer.data(), aSize ) ) { return false; }
						break;
					}
				}
				break;
			}
			case LSN_SF_FLOAT : {
				switch ( m_fFormat.ui16BitsPerChannel ) {
					case 32 : {
						if ( !BufferMonoF32( m_vTmpBuffer.data(), aSize ) ) { return false; }
						break;
					}
				}
				break;
			}
			default : { return false; }
			
		}
		return true;
	}

}	// namespace lsn

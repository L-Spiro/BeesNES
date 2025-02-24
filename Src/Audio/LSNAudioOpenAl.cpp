/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The implementation of the OpenAL audio system.
 */



#include "LSNAudioOpenAl.h"

#ifdef LSN_WINDOWS

#include "../Utilities/LSNUtilities.h"
#include "LSNAudioOptions.h"
#include "OpenAL/LSNOpenAl.h"

namespace lsn {

	// == Functions.
	/**
	 * Initializes audio.
	 * 
	 * \return Returns true if initialization was successful.
	 **/
	bool CAudioOpenAl::InitializeAudio() {
		ShutdownAudio();
		if ( !Parent::InitializeAudio() ) { return false; }
		m_eFormat = FormatToEnum( m_fNextFormat.sfFormat );

		std::vector<std::string> vDevices;
		// We need a dummy device to enumerate the audio devices.
		COpenAlDevice oadDummy;
		if ( !COpenAlDevice::GetAudioDevices( vDevices, oadDummy ) ) { return false; }
		if ( !vDevices.size() ) { return false; }
		if ( !m_oadDevice.CreateDevice( vDevices[0].c_str() ) ) { return false; }
		if ( !m_oacContext.CreateContext( m_oadDevice, nullptr ) ) { return false; }
		if ( !m_oacContext.MakeCurrent() ) { return false; }

		if ( !COpenAl::InitializeOpenAl( m_oadDevice.Device() ) ) { return false; }

		if ( !COpenAl::DistanceModel( AL_NONE ) ) { return false; }

		if ( !m_oasSource.CreateSource() ) { return false; }
		if ( !m_oasSource.SetLooping( AL_FALSE ) ) { return false; }

		
		return true;
	}

	/**
	 * Shuts down the audio.
	 * 
	 * \return Returns true if shutdown was successful.
	 **/
	bool CAudioOpenAl::ShutdownAudio() {
		if ( m_oasSource.GetState() == AL_PLAYING || m_oasSource.GetState() == AL_PAUSED ) {
			m_oasSource.Stop();
		}
		m_oasSource.Reset();
		for ( auto I = LSN_AUDIO_BUFFERS; I--; ) {
			m_oabBuffers[I].Reset();
		}
		m_oacContext.Reset();
		return m_oadDevice.Reset();
	}

	/**
	 * Called when emulation begins.  Resets the ring buffer of buckets.
	 **/
	void CAudioOpenAl::BeginEmulation() {
		Parent::BeginEmulation();
	}

	/**
	 * Gets a list of audio devices.
	 * 
	 * \return Returns a vector of strings listing the audio devices.
	 **/
	std::vector<std::u16string> CAudioOpenAl::GetAudioDevices() {
		try {
			std::vector<std::string> vDevices;
			// We need a dummy device to enumerate the audio devices.
			COpenAlDevice oadDummy;
			if ( !COpenAlDevice::GetAudioDevices( vDevices, oadDummy ) ) { return std::vector<std::u16string>(); }
			std::vector<std::u16string> vRet;
			vRet.resize( vDevices.size() );
			for ( size_t I = 0; I < vDevices.size(); ++I ) {
				vRet[I] = CUtilities::Utf8ToUtf16( reinterpret_cast<const char8_t *>(vDevices[I].c_str()) );
			}
			return vRet;
		}
		catch ( ... ) { return std::vector<std::u16string>(); }
	}

	/**
	 * Gets all available audio formats and Hz.  The top byte contains the sample format while the bottom 3 bytes contain the Hz in 25ths.
	 * 
	 * \return Returns a vector of formats and Hz formatted such that the high 8 bits are the format index and the lower 24 bits are the Hz in units of 25.
	 **/
	std::vector<uint32_t> CAudioOpenAl::GetAudioFormatsAndHz() {
		std::vector<uint32_t> vRet;
		try {
			std::vector<LSN_SAMPLE_FORMAT> vFormats = {
				LSN_SF_MONO_8,
				LSN_SF_MONO_16,
				LSN_SF_MONO_24,
				LSN_SF_MONO_F32,
			};

			std::vector<ALint> vSampleRates = {
				8000, 11025, 16000, 22050, 32000, 44100, 48000, 88200, 96000, 192000
			};

			if ( m_oacContext.MakeCurrent() ) {
				for ( const auto & sfFormat : vFormats ) {
					for ( const auto & ui32Rate : vSampleRates ) {
						::alGetError();
						COpenAlBuffer oabBuffer;
						uint32_t ui32Value;
						if ( oabBuffer.BufferData( FormatToEnum( sfFormat ), &ui32Value, sizeof( ui32Value ), ui32Rate ) ) {
							vRet.push_back( (sfFormat << 24) | (ui32Rate / 25) );
						}
					}
				}
			}
		}
		catch ( ... ) {}
		return vRet;
	}

	/**
	 * Undirties the format.  Called only when the new format differs from the old.
	 * 
	 * \param _fFormat The new format to set.
	 **/
	void CAudioOpenAl::UndirtyFormat( const LSN_AUDIO_FORMAT &_fFormat ) {
		Parent::UndirtyFormat( _fFormat );
		m_eFormat = FormatToEnum( _fFormat.sfFormat );
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
	bool CAudioOpenAl::QueueBuffer( uint64_t _ui64Idx, const void * _pvData, size_t _sSizeInBytes, uint32_t _ui32Freq ) {
		size_t sIdx = size_t( _ui64Idx % LSN_AUDIO_BUFFERS );
		if ( m_oabBuffers[sIdx].BufferData( m_eFormat, _pvData, static_cast<ALsizei>(_sSizeInBytes), static_cast<ALsizei>(_ui32Freq) ) ) {
			return m_oasSource.QueueBuffer( m_oabBuffers[sIdx].Id() );
		}
		return false;
	}

	/**
	 * Buffers samples from floating-point to the AL_FORMAT_MONO8 format.
	 * 
	 * \param _pfSamples The samples to buffer.
	 * \param _sTotal The number of samples to which _pfSamples points.
	 * \return Returns true if the buffer succeeded.
	 **/
	bool CAudioOpenAl::BufferMono8( const float * _pfSamples, size_t _sTotal ) {
		// Determine how many samples we can do before we need to flush.
		size_t stMax = m_sBufferSizeInSamples - (m_sCurBufferSize);
		while ( _sTotal >= stMax ) {
			// Convert and flush.
			uint8_t * pui8Dst = reinterpret_cast<uint8_t *>(&m_vLocalBuffer.data()[m_sCurBufferSize]);
			for ( size_t I = 0; I < stMax; ++I ) {
				(*pui8Dst++) = COpenAl::SampleToUi8( (*_pfSamples++) );
				++m_sCurBufferSize;
			}


			if ( !Flush() ) { return false; }
			_sTotal -= stMax;
			// Flushing can change the output format.
			switch ( m_eFormat ) {
				case AL_FORMAT_MONO16 : {
					return BufferMono16( _pfSamples, _sTotal );
				}
				case AL_FORMAT_MONO_FLOAT32 : {
					return BufferMonoF32( _pfSamples, _sTotal );
				}
				default : {}
			}
			stMax = m_sBufferSizeInSamples - (m_sCurBufferSize/* / sizeof( uint8_t )*/);
		}
		// Finish converting.
		uint8_t * pui8Dst = reinterpret_cast<uint8_t *>(&m_vLocalBuffer.data()[m_sCurBufferSize]);
		for ( size_t I = 0; I < _sTotal; ++I ) {
			(*pui8Dst++) = COpenAl::SampleToUi8( (*_pfSamples++) );
			++m_sCurBufferSize;
		}
		return true;
	}

	/**
	 * Buffers samples from floating-point to the AL_FORMAT_MONO16 format.
	 * 
	 * \param _pfSamples The samples to buffer.
	 * \param _sTotal The number of samples to which _pfSamples points.
	 * \return Returns true if the buffer succeeded.
	 **/
	bool CAudioOpenAl::BufferMono16( const float * _pfSamples, size_t _sTotal ) {
		// Determine how many samples we can do before we need to flush.
		size_t stMax = m_sBufferSizeInSamples - (m_sCurBufferSize / sizeof( int16_t ));
		while ( _sTotal >= stMax ) {
			// Convert and flush.
			int16_t * pi16Dst = reinterpret_cast<int16_t *>(&m_vLocalBuffer.data()[m_sCurBufferSize]);
			size_t I = 0;
#ifdef __AVX__
			if LSN_LIKELY( CUtilities::IsAvxSupported() ) {
				constexpr auto sRegSize = sizeof( __m256 ) / sizeof( float );
				int32_t i32Total = int32_t( stMax - sRegSize );
				for ( ; int32_t( I ) <= i32Total; I += sRegSize ) {
					COpenAl::SampleToI16_AVX( _pfSamples, pi16Dst );
					_pfSamples += sRegSize;
					pi16Dst += sRegSize;
				}
			}
#endif	// #ifdef __AVX__
			for ( ; I < stMax; ++I ) {
				(*pi16Dst++) = COpenAl::SampleToI16( (*_pfSamples++) );
			}
			m_sCurBufferSize += (sizeof( int16_t ) * stMax);


			if ( !Flush() ) { return false; }
			_sTotal -= stMax;
			// Flushing can change the output format.
			switch ( m_eFormat ) {
				case AL_FORMAT_MONO8 : {
					return BufferMono8( _pfSamples, _sTotal );
				}
				case AL_FORMAT_MONO_FLOAT32 : {
					return BufferMonoF32( _pfSamples, _sTotal );
				}
				default : {}
			}
			stMax = m_sBufferSizeInSamples - (m_sCurBufferSize / sizeof( int16_t ));
		}
		// Finish converting.
		int16_t * pi16Dst = reinterpret_cast<int16_t *>(&m_vLocalBuffer.data()[m_sCurBufferSize]);
		size_t I = 0;
#ifdef __AVX__
		if LSN_LIKELY( CUtilities::IsAvxSupported() ) {
			constexpr auto sRegSize = sizeof( __m256 ) / sizeof( float );
			int32_t i32Total = int32_t( _sTotal - sRegSize );
			for ( ; int32_t( I ) <= i32Total; I += sRegSize ) {
				COpenAl::SampleToI16_AVX( _pfSamples, pi16Dst );
				_pfSamples += sRegSize;
				pi16Dst += sRegSize;
			}
		}
#endif	// #ifdef __AVX__
		for ( ; I < _sTotal; ++I ) {
			(*pi16Dst++) = COpenAl::SampleToI16( (*_pfSamples++) );
		}
		m_sCurBufferSize += sizeof( int16_t ) * _sTotal;
		return true;
	}

	/**
	 * Buffers samples from floating-point to the AL_FORMAT_MONO_FLOAT32 format.
	 * 
	 * \param _pfSamples The samples to buffer.
	 * \param _sTotal The number of samples to which _pfSamples points.
	 * \return Returns true if the buffer succeeded.
	 **/
	bool CAudioOpenAl::BufferMonoF32( const float * _pfSamples, size_t _sTotal ) {
		// Determine how many samples we can do before we need to flush.
		size_t stMax = m_sBufferSizeInSamples - (m_sCurBufferSize / sizeof( int16_t ));
		while ( _sTotal >= stMax ) {
			// Convert and flush.
			float * pfDst = reinterpret_cast<float *>(&m_vLocalBuffer.data()[m_sCurBufferSize]);
			for ( size_t I = 0; I < stMax; ++I ) {
				(*pfDst++) = (*_pfSamples++);
				m_sCurBufferSize += sizeof( float );
			}


			if ( !Flush() ) { return false; }
			_sTotal -= stMax;

			// Flushing can change the output format.
			switch ( m_eFormat ) {
				case AL_FORMAT_MONO8 : {
					return BufferMono8( _pfSamples, _sTotal );
				}
				case AL_FORMAT_MONO16 : {
					return BufferMono16( _pfSamples, _sTotal );
				}
				default : {}
			}
			
			stMax = m_sBufferSizeInSamples - (m_sCurBufferSize / sizeof( float ));
		}
		// Finish converting.
		float * pfDst = reinterpret_cast<float *>(&m_vLocalBuffer.data()[m_sCurBufferSize]);
		for ( size_t I = 0; I < _sTotal; ++I ) {
			(*pfDst++) = (*_pfSamples++);
			m_sCurBufferSize += sizeof( float );
		}
		return true;
	}

	/**
	 * Gets the OpenAL format enum given a general format descriptor.
	 * 
	 * \param _sfFormat The format descriptor.
	 * \return Returns the OpenAL enum that represents _fFormat.
	 **/
	ALenum CAudioOpenAl::FormatToEnum( const LSN_SAMPLE_FORMAT &_sfFormat ) {
		switch ( _sfFormat ) {
			case LSN_SF_MONO_8 : { return AL_FORMAT_MONO8; }
			case LSN_SF_MONO_16 : { return AL_FORMAT_MONO16; }
			case LSN_SF_MONO_F32 : { return AL_FORMAT_MONO_FLOAT32; }
		}
		return AL_INVALID;
	}

}	// namespace lsn

#endif  // #ifdef LSN_WINDOWS

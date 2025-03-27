/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A WAV file.
 */

#include "LSNWavFile.h"
#include "../OS/LSNOs.h"
#include "../Utilities/LSNUtilities.h"

#include <codecvt>
#include <filesystem>
#include <string>

// warning C4309: 'static_cast': truncation of constant value
#pragma warning( disable : 4309 )


namespace lsn {

	CWavFile::CWavFile() :
		m_uiNumChannels( 0 ),
		m_uiSampleRate( 0 ),
		m_uiBitsPerSample( 0 ),
		m_uiBytesPerSample( 0 ),
		m_uiBaseNote( 64 ) {
	}
	CWavFile::~CWavFile() {
		StopStream();
		Reset();
	}

	// == Functions.
	/**
	 * Loads a WAV file.
	 *
	 * \param _pcPath The UTF-8 path to open.
	 * \return Returns true if the file was opened.
	 */
	bool CWavFile::Open( const char8_t * _pcPath ) {
		std::vector<uint8_t> vFile;
		if ( !CStdFile::LoadToMemory( _pcPath, vFile ) ) { return false; }
		return LoadFromMemory( vFile );
		//return Open( std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.from_bytes( _pcPath ).c_str() );
	}

	/**
	 * Loads a WAV file.
	 *
	 * \param _pwcPath The UTF-16 path to open.
	 * \return Returns true if the file was opened.
	 */
	bool CWavFile::Open( const char16_t * _pwcPath ) {
		std::vector<uint8_t> vFile;
		if ( !CStdFile::LoadToMemory( _pwcPath, vFile ) ) { return false; }
		return LoadFromMemory( vFile );
	}

	/**
	 * Loads a WAV file from memory.  This is just an in-memory version of the file.
	 *
	 * \param _vData The in-memory file to load.
	 * \return Returns true if the file is a valid WAV file.
	 */
	bool CWavFile::LoadFromMemory( const std::vector<uint8_t> &_vData ) {
		Reset();
		size_t stOffset = 0;
		const uint32_t * pui32Scratch;

#define LSN_PTR_SIZE( TYPE, OFFSET, SIZE )			((OFFSET) + SIZE) > _vData.size() ? nullptr : reinterpret_cast<const TYPE *>(&_vData[OFFSET])
#define LSN_PTR( TYPE, OFFSET )						LSN_PTR_SIZE( TYPE, OFFSET, sizeof( TYPE ) )
#define LSN_READ_32( VAL )							pui32Scratch = LSN_PTR( uint32_t, stOffset ); if ( !pui32Scratch ) { return false; } stOffset += sizeof( uint32_t ); VAL = (*pui32Scratch)
#define LSN_READ_STRUCT( TYPE, VAL )				VAL = LSN_PTR( TYPE, stOffset ); if ( !VAL ) { return false; } stOffset += sizeof( TYPE )


		do {
			LSN_CHUNK cCurChunk;
			LSN_READ_32( cCurChunk.u.uiName );
			if ( cCurChunk.u.uiName != LSN_C_RIFF ) { return false; }
			LSN_READ_32( cCurChunk.uiSize );
			LSN_READ_32( cCurChunk.u2.uiFormat );
			if ( cCurChunk.u2.uiFormat != LSN_C_WAVE ) { return false; }
			size_t stStartOff = stOffset;
			std::vector<LSN_CHUNK_ENTRY> ceChunks;
			LSN_CHUNK_ENTRY ceThis = { 0 };
			while ( (stOffset - stStartOff) < cCurChunk.uiSize && stOffset < _vData.size() ) {
				ceThis.uiOffset = static_cast<uint32_t>(stOffset);
				LSN_READ_32( ceThis.u.uiName );
				if ( ceThis.u.uiName == 0 ) { break; }
				LSN_READ_32( ceThis.uiSize );

				ceChunks.push_back( ceThis );
				stOffset += ceThis.uiSize;
			}

			for ( size_t I = 0; I < ceChunks.size(); ++I ) {
				switch ( ceChunks[I].u.uiName ) {
					case LSN_C_FMT_ : {		// "fmt "
						const LSN_FMT_CHUNK * pfcFmt = LSN_PTR_SIZE( LSN_FMT_CHUNK, ceChunks[I].uiOffset, ceChunks[I].uiSize );
						if ( !pfcFmt ) { return false; }

						if ( !LoadFmt( pfcFmt ) ) { return false; }
						break;
					}
					case LSN_C_DATA : {		// "data"
						const LSN_DATA_CHUNK * pfcData = LSN_PTR_SIZE( LSN_DATA_CHUNK, ceChunks[I].uiOffset, ceChunks[I].uiSize );
						if ( !pfcData ) { return false; }

						if ( !LoadData( pfcData ) ) { return false; }
						break;
					}
					case LSN_C_SMPL : {		// "smpl"
						const LSN_SMPL_CHUNK * pfcSmpl = LSN_PTR_SIZE( LSN_SMPL_CHUNK, ceChunks[I].uiOffset, ceChunks[I].uiSize );
						if ( !pfcSmpl ) { return false; }

						if ( !LoadSmpl( pfcSmpl ) ) { return false; }
						break;
					}
					case LSN_C_LIST : {		// "LIST"
						const LSN_LIST_CHUNK * plcList = LSN_PTR_SIZE( LSN_LIST_CHUNK, ceChunks[I].uiOffset, ceChunks[I].uiSize );
						if ( !plcList ) { return false; }

						if ( !LoadList( plcList ) ) { return false; }
						break;
					}
					case LSN_C_ID3_ : {		// "id3 "
						const LSN_ID3_CHUNK * picList = LSN_PTR_SIZE( LSN_ID3_CHUNK, ceChunks[I].uiOffset, ceChunks[I].uiSize );
						if ( !picList ) { return false; }

						if ( !LoadId3( picList ) ) { return false; }
						break;
					}
					case LSN_C_INST : {		// "inst"
						const LSN_INST_CHUNK * picList = LSN_PTR_SIZE( LSN_INST_CHUNK, ceChunks[I].uiOffset, ceChunks[I].uiSize );
						if ( !picList ) { return false; }

						if ( !LoadInst( picList ) ) { return false; }
						break;
					}

				}
			}

		} while ( stOffset < _vData.size() );


#undef LSN_READ_STRUCT
#undef LSN_READ_32
#undef LSN_PTR
		return true;
	}

	/**
	 * Saves as a PCM WAV file.
	 *
	 * \param _pcPath The path to where the file will be saved.
	 * \param _vSamples The samples to convert and write to the file.
	 * \param _psdSaveSettings Settings to override this class's settings.
	 * \return Returns true if the file was created and saved.
	 */
	bool CWavFile::SaveAsPcm( const char8_t * _pcPath, const lwaudio &_vSamples,
		const LSN_SAVE_DATA * _psdSaveSettings ) const {
		if ( !_vSamples.size() ) { return false; }
		std::u8string sPath = _pcPath;
		std::u8string sFolder = CFileBase::GetFilePath( sPath );
		std::u8string sName = CFileBase::GetFileName( sPath );
		

		std::u8string sCopy = sName;
		const struct LSN_TABLE {
			const char8_t *					pcReplaceMe;
			const char8_t *					pcWithMe;
		} tTable[] = {
			{ u8"?", u8"-" },
			{ u8"*", u8"˙" },
			{ u8":", u8" -" },
			{ u8"\\", u8"-" },
			{ u8"/", u8"∕" },
			{ u8"<", u8"‹" },
			{ u8">", u8"›" },
			{ u8"|", u8"¦" },
			{ u8"\"", u8"‟" },
		};
		for ( auto I = sizeof( tTable ) / sizeof( tTable[0] ); I--; ) {
			sCopy = CUtilities::Replace<std::u8string>( sCopy, reinterpret_cast<const char8_t *>(tTable[I].pcReplaceMe), reinterpret_cast<const char8_t *>(tTable[I].pcWithMe) );
		};

		sPath = sFolder + sCopy;

		CStdFile sfFile;
		if ( !sfFile.Create( sPath.c_str() ) ) {
			std::wprintf( L"Failed to create PCM file: %s.\r\n", reinterpret_cast<const wchar_t *>(CUtilities::Utf8ToUtf16( sPath.c_str() ).c_str()) );
			return false;
		}
		LSN_FMT_CHUNK fcChunk = CreateFmt( LSN_F_PCM, static_cast<uint16_t>(_vSamples.size()),
			_psdSaveSettings );
		std::vector<uint8_t> vLoops, vList;
		if ( m_vLoops.size() ) {
			vLoops = CreateSmpl();
		}
		if ( m_vListEntries.size() ) {
			vList = CreateList();
		}
		

		uint32_t uiFmtSize = fcChunk.chHeader.uiSize + 8;
		uint32_t ui32DataSize = CalcSize( static_cast<LSN_FORMAT>(fcChunk.uiAudioFormat), static_cast<uint32_t>(_vSamples[0].size()), static_cast<uint16_t>(_vSamples.size()), fcChunk.uiBitsPerSample );

		uint32_t ui32Size = 4 +							// "WAVE".
			uiFmtSize +									// "fmt " chunk.
			ui32DataSize + 8 +							// "data" chunk.
			static_cast<uint32_t>(vLoops.size()) +		// "smpl" chunk.
			static_cast<uint32_t>(vList.size()) +		// "LIST" chunk.
			0;

		std::vector<uint8_t> vRet;
		vRet.reserve( ui32Size + 8 );
#define LSN_PUSH32( VAL )		vRet.push_back( static_cast<uint8_t>((VAL) >> 0) ); vRet.push_back( static_cast<uint8_t>((VAL) >> 8) ); vRet.push_back( static_cast<uint8_t>((VAL) >> 16) ); vRet.push_back( static_cast<uint8_t>((VAL) >> 24) )
		LSN_PUSH32( LSN_C_RIFF );						// "RIFF"
		
		LSN_PUSH32( ui32Size );
		LSN_PUSH32( LSN_C_WAVE );
		
		// Append the "fmt " chunk.
		for ( size_t I = 0; I < uiFmtSize; ++I ) {
			vRet.push_back( reinterpret_cast<const uint8_t *>(&fcChunk)[I] );
		}

		// Append the "data" chunk.
		LSN_PUSH32( LSN_C_DATA );
		LSN_PUSH32( ui32DataSize );
		switch ( fcChunk.uiAudioFormat ) {
			case LSN_F_PCM : {
				switch ( fcChunk.uiBitsPerSample ) {
					case 8 : {
						if ( !BatchF64ToPcm8( _vSamples, vRet ) ) {
							return false;
						}
						break;
					}
					case 16 : {
						if ( !BatchF64ToPcm16( _vSamples, vRet ) ) {
							return false;
						}
						break;
					}
					case 24 : {
						if ( !BatchF64ToPcm24( _vSamples, vRet ) ) {
							return false;
						}
						break;
					}
					case 32 : {
						if ( !BatchF64ToPcm32( _vSamples, vRet ) ) {
							return false;
						}
						break;
					}
					default : {
						return false;
					}
				}
				break;
			}
			default : {
				return false;
			}
		}

		// Append "smpl" chunk.
		for ( size_t I = 0; I < vLoops.size(); ++I ) {
			vRet.push_back( vLoops[I] );
		}

		// Append "LIST" chunk.
		for ( size_t I = 0; I < vList.size(); ++I ) {
			vRet.push_back( vList[I] );
		}
		if ( !sfFile.WriteToFile( vRet ) ) { return false; }

#undef LSN_PUSH32
		return true;
	}

	/**
	 * Opens a stream to a WAV file.  Samples can be written over time.
	 * 
	 * \param _stfoFileOptions Contains all of the settings for streaming to a file.
	 * \param _ui32Hz The WAV Hz.
	 * \param _stBufferSize The buffer size (in samples).
	 * \return Returns true if the file could initially be created and all parameters are correct.
	 **/
	bool CWavFile::StreamToFile( const LSN_STREAM_TO_FILE_OPTIONS &_stfoFileOptions, uint32_t _ui32Hz,
		size_t _stBufferSize ) {
		LSN_STREAM_COND_PARM scpStartParm;
		scpStartParm.ui64Parm = 0;
		switch ( _stfoFileOptions.scStartCondition ) {
			case LSN_SC_START_AT_SAMPLE : {
				scpStartParm.ui64Parm = _stfoFileOptions.ui64StartParm;
				break;
			}
			case LSN_SC_ZERO_FOR_DURATION : {
				scpStartParm.dParm = _stfoFileOptions.dStartParm;
				break;
			}
		}
		LSN_STREAM_COND_PARM scpStopParm;
		scpStopParm.ui64Parm = 0;
		switch ( _stfoFileOptions.seEndCondition ) {
			case LSN_EC_END_AT_SAMPLE : {
				scpStopParm.ui64Parm = _stfoFileOptions.ui64EndParm;
				break;
			}
			case LSN_EC_ZERO_FOR_DURATION : {
				scpStopParm.dParm = _stfoFileOptions.dEndParm;
				break;
			}
			case LSN_EC_DURATION : {
				scpStopParm.dParm = _stfoFileOptions.dEndParm;
				break;
			}
		}

		std::filesystem::path pAbsolutePath = std::filesystem::absolute( std::filesystem::path( _stfoFileOptions.wsPath ) );

		StopStream();
		m_sStream.bStreaming = false;
		m_sStream.bMeta = false;

		std::unique_lock<std::mutex> ulLock( m_sStream.mMutex );
		
		switch ( _stfoFileOptions.scStartCondition ) {
			case LSN_SC_NONE : {
				m_sStream.pfStartCondFunc = &CWavFile::StartCondFunc_None;
				break;
			}
			case LSN_SC_START_AT_SAMPLE : {
				m_sStream.pfStartCondFunc = &CWavFile::StartCondFunc_StartAtSample;
				m_sStream.cdStartData.ui64Parm0 = scpStartParm.ui64Parm;
				break;
			}
			case LSN_SC_FIRST_NON_ZERO : {
				m_sStream.pfStartCondFunc = &CWavFile::StartCondFunc_FirstNonZero;
				break;
			}
			case LSN_SC_ZERO_FOR_DURATION : {
				m_sStream.pfStartCondFunc = &CWavFile::StartCondFunc_ZeroForDuration;
				m_sStream.cdStartData.ui64Counter = 0;
				m_sStream.cdStartData.ui64Parm0 = uint64_t( std::round( scpStartParm.dParm * _ui32Hz ) );
				break;
			}
			default : {
				std::wprintf( L"Unrecognized start condition %X: %s.\r\n", _stfoFileOptions.scStartCondition, _stfoFileOptions.wsPath.c_str() );
				return false;
			}
		}
		
		switch ( _stfoFileOptions.seEndCondition ) {
			case LSN_EC_NONE : {
				m_sStream.pfEndCondFunc = &CWavFile::EndCondFunc_None;
				break;
			}
			case LSN_EC_END_AT_SAMPLE : {
				m_sStream.pfEndCondFunc = &CWavFile::EndCondFunc_EndAtSample;
				m_sStream.cdStopData.ui64Parm0 = scpStopParm.ui64Parm;
				break;
			}
			case LSN_EC_ZERO_FOR_DURATION : {
				m_sStream.pfEndCondFunc = &CWavFile::EndCondFunc_ZeroForDuration;
				m_sStream.cdStopData.ui64Counter = 0;
				m_sStream.cdStopData.ui64Parm0 = uint64_t( std::round( scpStopParm.dParm * _ui32Hz ) );
				break;
			}
			case LSN_EC_DURATION : {
				m_sStream.pfEndCondFunc = &CWavFile::EndCondFunc_Duration;
				m_sStream.cdStopData.ui64Parm0 = uint64_t( std::round( scpStopParm.dParm * _ui32Hz ) );
				break;
			}
			default : {
				std::wprintf( L"Unrecognized stop condition %X: %s.\r\n", _stfoFileOptions.seEndCondition, _stfoFileOptions.wsPath.c_str() );
				return false;
			}
		}
		m_sStream.pfAddSampleFunc = &CWavFile::AddSample_CheckStartCond;

		m_sStream.fFormat = _stfoFileOptions.fFormat;
		m_sStream.ui16Bits = static_cast<uint16_t>(_stfoFileOptions.ui32Bits);
		m_sStream.ui16Channels = 1;
		m_sStream.bDither = _stfoFileOptions.bDither;
		m_sStream.ui32Hz = _ui32Hz;
		m_sStream.dDitherError = 0.0;

		if ( m_sStream.fFormat == LSN_F_IEEE_FLOAT ) {
			m_sStream.ui16Bits = 32;
		}
		
		if ( !CreateStreamFile( pAbsolutePath.generic_u8string().c_str() ) ) {
			m_sStream.sfFile.Close();
			std::wprintf( L"Failed to create WAV stream file: %s.\r\n", _stfoFileOptions.wsPath.c_str() );
			return false;
		}

		
		if ( _stfoFileOptions.bMetaEnabled && _stfoFileOptions.wsMetaPath.size() && _stfoFileOptions.pfMetaFunc && _stfoFileOptions.pfMetaThreadFunc ) {
			std::filesystem::path pAbsoluteMetaPath = std::filesystem::absolute( std::filesystem::path( _stfoFileOptions.wsMetaPath ) );
			if ( !CreateStreamMetaFile( pAbsoluteMetaPath.generic_u8string().c_str() ) ) {
				m_sStream.sfFile.Close();
				m_sStream.sfMetaFile.Close();
				return false;
			}
			m_sStream.pfMetaFunc = _stfoFileOptions.pfMetaFunc;
			m_sStream.pfMetaThreadFunc = _stfoFileOptions.pfMetaThreadFunc;
			m_sStream.pvMetaParm = _stfoFileOptions.pvMetaParm;
			m_sStream.ui64MetaParm = _stfoFileOptions.ui64MetaParm;
			m_sStream.i32MetaFormat = _stfoFileOptions.i32MetaFormat;
			m_sStream.ui64MetaWritten = 0;
			m_sStream.ui64MetaThreadParm = 0;
			m_sStream.vMetaThreadScratch.clear();
			m_sStream.bMeta = true;
		}
		
		try {
			m_sStream.stBufferSize = _stBufferSize * m_sStream.ui16Channels;
			m_sStream.vCurBuffer.clear();
			m_sStream.vCurBuffer.reserve( m_sStream.stBufferSize );
		}
		catch ( ... ) {
			m_sStream.sfFile.Close();
			m_sStream.sfMetaFile.Close();
			std::wprintf( L"Out of memory creating buffers for WAV stream: %s.\r\n", _stfoFileOptions.wsPath.c_str() );
			return false;
		}
		m_sStream.bEnd = false;
		switch ( m_sStream.fFormat ) {
			case LSN_F_PCM : {
				switch ( m_sStream.ui16Bits ) {
					case 8 : {
						m_sStream.pfCvtAndWriteFunc = &CWavFile::BatchF32ToPcm8;
#ifdef __AVX2__
						if ( CUtilities::IsAvx2Supported() ) {
							m_sStream.pfCvtAndWriteFunc = &CWavFile::BatchF32ToPcm8_AVX2;
						}
#endif	// #ifdef __AVX2__
#ifdef __AVX512F__
						if ( CUtilities::IsAvx512FSupported() ) {
							m_sStream.pfCvtAndWriteFunc = &CWavFile::BatchF32ToPcm8_AVX512;
						}
#endif	// #ifdef __AVX512F__
						break;
					}
					case 16 : {
						m_sStream.pfCvtAndWriteFunc = &CWavFile::BatchF32ToPcm16;
#ifdef __AVX2__
						if ( CUtilities::IsAvx2Supported() ) {
							m_sStream.pfCvtAndWriteFunc = &CWavFile::BatchF32ToPcm16_AVX2;
						}
#endif	// #ifdef __AVX2__
#ifdef __AVX512F__
						if ( CUtilities::IsAvx512FSupported() ) {
							m_sStream.pfCvtAndWriteFunc = &CWavFile::BatchF32ToPcm16_AVX512;
						}
#endif	// #ifdef __AVX512F__
						if ( m_sStream.bDither ) {
							m_sStream.pfCvtAndWriteFunc = &CWavFile::BatchF32ToPcm16_Dither;
						}
						break;
					}
					case 24 : {
						m_sStream.pfCvtAndWriteFunc = &CWavFile::BatchF32ToPcm24;
#ifdef __AVX2__
						if ( CUtilities::IsAvx2Supported() ) {
							m_sStream.pfCvtAndWriteFunc = &CWavFile::BatchF32ToPcm24_AVX2;
						}
#endif	// #ifdef __AVX2__
#ifdef __AVX512F__
						if ( CUtilities::IsAvx512FSupported() ) {
							m_sStream.pfCvtAndWriteFunc = &CWavFile::BatchF32ToPcm24_AVX512;
						}
#endif	// #ifdef __AVX512F__
						break;
					}
					default : {
						m_sStream.sfFile.Close();
						m_sStream.sfMetaFile.Close();
						std::wprintf( L"Unrecognized PCM bits %X: %s.\r\n", m_sStream.ui16Bits, _stfoFileOptions.wsPath.c_str() );
						return false;
					}
				}
				break;
			}
			case LSN_F_IEEE_FLOAT : {
				m_sStream.pfCvtAndWriteFunc = &CWavFile::BatchF32ToF32;
				break;
			}
			default : {
				m_sStream.sfFile.Close();
				m_sStream.sfMetaFile.Close();
				std::wprintf( L"Unrecognized WAV format %X: %s.\r\n", m_sStream.fFormat, _stfoFileOptions.wsPath.c_str() );
				return false;
			}
		}
		
		m_sStream.wsPath = _stfoFileOptions.wsPath;

		m_sStream.tThread = std::thread( &CWavFile::StreamWriterThread, this );
		if ( m_sStream.bMeta ) {
			m_sStream.tMetaThread = std::thread( &CWavFile::MetadataWriterThread, this );
		}

		m_sStream.ui64SamplesReceived = m_sStream.ui64SamplesWritten = 0;
		m_sStream.bAdding = false;
		m_sStream.ui64MetaWritten = 0;
		m_sStream.bStreaming = true;
		return true;
	}

	/**
	 * Stops the streaming file.
	 **/
	void CWavFile::StopStream() {
		{
			std::unique_lock<std::mutex> ulLock( m_sStream.mMutex );
			if ( m_sStream.sfFile.IsOpen() ) {
				if ( m_sStream.bStreaming ) {
					if ( !m_sStream.vCurBuffer.empty() ) {
						m_sStream.qBufferQueue.push( std::move( m_sStream.vCurBuffer ) );
						m_sStream.vCurBuffer.clear();
					}
					m_sStream.bEnd = true;	// File will be closed in the writer thread.
				}
			}
		}

		m_sStream.cvCondition.notify_one();
        if ( m_sStream.tThread.joinable()) {
            m_sStream.tThread.join();
        }


		std::unique_lock<std::mutex> ulLock( m_sStream.mMutex );
		{
			m_sStream.bStreaming = false;
		}
	}

	/**
	 * Adds a sample to the stream.
	 * 
	 * \param _fSample The sample to add.
	 **/
	void CWavFile::AddStreamSample( float _fSample ) {
		std::unique_lock<std::mutex> ulLock( m_sStream.mMutex );

		if LSN_LIKELY( m_sStream.bStreaming && !m_sStream.bEnd ) {
			if ( (*m_sStream.pfAddSampleFunc)( _fSample, m_sStream ) ) {
				m_sStream.bAdding = true;
				m_sStream.vCurBuffer.push_back( _fSample );
				uint64_t ui64TotalWillWrite = ++m_sStream.ui64SamplesWritten;
				m_sStream.ui32WavFile_DSize += sizeof( float );

				if ( m_sStream.bMeta && m_sStream.pfMetaFunc ) {
					std::unique_lock<std::mutex> ulLockMeta( m_sStream.mMetaMutex );
					AddMetaData();
				}

				if LSN_UNLIKELY( ui64TotalWillWrite == ((UINT_MAX - m_sStream.ui32WavFile_Size - 4) / sizeof( float )) ||		// Maximum a WAV file can contain (4294967264/0xFFFFFFE0 samples).
					m_sStream.vCurBuffer.size() == m_sStream.stBufferSize ) {													// Buffer limit reached.
					// Efficiently pass the buffer off to the writer thread.
					m_sStream.qBufferQueue.push( std::move( m_sStream.vCurBuffer ) );
					// Create a new buffer and reserve space for efficiency.
					m_sStream.vCurBuffer.clear();
					m_sStream.vCurBuffer.reserve( m_sStream.stBufferSize );
					// Notify the writer thread that a full buffer is ready.
					m_sStream.cvCondition.notify_one();
				}
			}
			else if ( m_sStream.bEnd ) {
				m_sStream.qBufferQueue.push( std::move( m_sStream.vCurBuffer ) );
				// Create a new buffer and reserve space for efficiency.
				m_sStream.vCurBuffer.clear();
				// Notify the writer thread that a full buffer is ready.
				m_sStream.cvCondition.notify_one();
			}
			++m_sStream.ui64SamplesReceived;
		}
	}

	/**
	 * Called to update the metadata stream output.  Call immediately after calling AddStreamSample().
	 **/
	void CWavFile::AddMetaData() {
		//if LSN_LIKELY( m_sStream.bAdding && m_sStream.bMeta ) {
			if LSN_LIKELY( m_sStream.bStreaming && !m_sStream.bEnd ) {
				if ( (*m_sStream.pfMetaFunc)( m_sStream.pvMetaParm, m_sStream ) ) {
					if LSN_UNLIKELY( m_sStream.vMetaBuffer.size() >= (1024 * 0x20) ) {
						m_sStream.qMetaBufferQueue.push( std::move( m_sStream.vMetaBuffer ) );
						// Create a new buffer and reserve space for efficiency.
						m_sStream.vMetaBuffer.clear();
						m_sStream.vMetaBuffer.reserve( (1024 * 0x20) );
						// Notify the writer thread that a full buffer is ready.
						m_sStream.cvMetaCondition.notify_one();
					}

					++m_sStream.ui64MetaWritten;
				}
			}
			else {
				// Stream is closing.  Push anything remaining.
				if ( m_sStream.vMetaBuffer.size() ) {
					m_sStream.qMetaBufferQueue.push( std::move( m_sStream.vMetaBuffer ) );
					// Create a new buffer and reserve space for efficiency.
					m_sStream.vMetaBuffer.clear();
					// Notify the writer thread that a full buffer is ready.
					m_sStream.cvMetaCondition.notify_one();
				}
			}
		//}
	}

	/**
	 * Resets the object back to scratch.
	 */
	void CWavFile::Reset() {
		//m_vChunks.clear();
		m_vSamples.clear();
		m_vLoops.clear();
		m_vListEntries.clear();
		m_vId3Entries.clear();
		m_vDisp.clear();
		m_uiNumChannels = 0;
		m_uiSampleRate = 0;
		m_uiBytesPerSample = 0;
		m_uiBitsPerSample = 0;
		m_uiBaseNote = 64;
	}

#pragma optimize( "gt", on )

	/**
	 * Fills a vector with the whole range of samples for a given channel.
	 *
	 * \param _uiChan The channel whose sample data is to be obtained.
	 * \param _vResult The vector containing the samples.
	 * \return Returns true if the vector was able to fit all samples.
	 */
	bool CWavFile::GetAllSamples( uint16_t _uiChan, lwtrack &_vResult ) const {
		if ( _uiChan >= m_uiNumChannels ) { return false; }
		switch ( m_uiBitsPerSample ) {
			case 8 : {
				return Pcm8ToF64( 0, static_cast<uint32_t>(TotalSamples()), _uiChan, _vResult );
			}
			case 16 : {
				return Pcm16ToF64( 0, static_cast<uint32_t>(TotalSamples()), _uiChan, _vResult );
			}
			case 24 : {
				return Pcm24ToF64( 0, static_cast<uint32_t>(TotalSamples()), _uiChan, _vResult );
			}
			case 32 : {
				switch ( m_fFormat ) {
					case LSN_F_IEEE_FLOAT : {
						return F32ToF64( 0, static_cast<uint32_t>(TotalSamples()), _uiChan, _vResult );
					}
					case LSN_F_PCM : {
						return Pcm32ToF64( 0, static_cast<uint32_t>(TotalSamples()), _uiChan, _vResult );
					}
				}
			}

		}
		return false;
	}

	/**
	 * Fills a vector with the whole range of samples for a given channel.
	 *
	 * \param _uiChan The channel whose sample data is to be obtained.
	 * \param _vResult The vector containing the samples.
	 * \param _stFrom The starting sample to get.
	 * \param _stTo The ending sample to get, exclusive.
	 * \return Returns true if the vector was able to fit all samples.
	 */
	bool CWavFile::GetSamples( uint16_t _uiChan, lwtrack &_vResult, size_t _stFrom, size_t _stTo ) const {
		if ( _uiChan >= m_uiNumChannels ) { return false; }
		switch ( m_uiBitsPerSample ) {
			case 8 : {
				return Pcm8ToF64( static_cast<uint32_t>(_stFrom), static_cast<uint32_t>(_stTo), _uiChan, _vResult );
			}
			case 16 : {
				return Pcm16ToF64( static_cast<uint32_t>(_stFrom), static_cast<uint32_t>(_stTo), _uiChan, _vResult );
			}
			case 24 : {
				return Pcm24ToF64( static_cast<uint32_t>(_stFrom), static_cast<uint32_t>(_stTo), _uiChan, _vResult );
			}
			case 32 : {
				switch ( m_fFormat ) {
					case LSN_F_IEEE_FLOAT : {
						return F32ToF64( static_cast<uint32_t>(_stFrom), static_cast<uint32_t>(_stTo), _uiChan, _vResult );
					}
					case LSN_F_PCM : {
						return Pcm32ToF64( static_cast<uint32_t>(_stFrom), static_cast<uint32_t>(_stTo), _uiChan, _vResult );
					}
				}
			}

		}
		return false;
	}

	/**
	 * Fills an array of vectors.  There is an array of vectors for each channel, and each vector contains all
	 *	of the samples for that channel.
	 *
	 * \param _vResult The array of vectors to be filled with all samples in this file.
	 * \return Returns true if the vector(s) was/were able to fit all samples.
	 */
	bool CWavFile::GetAllSamples( lwaudio &_vResult ) const {
		try {
			_vResult.resize( m_uiNumChannels );
			if ( _vResult.size() != m_uiNumChannels ) { return false; }
			for ( auto I = m_uiNumChannels; I--; ) {
				if ( !GetAllSamples( I, _vResult[I] ) ) { return false; }
			}
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Adds a LIST entry.
	 *
	 * \param _uiId The ID of the entry.
	 * \param _sVal The value of the entry.
	 * \return Returns true if the entry was added.
	 */
	bool CWavFile::AddListEntry( uint32_t _uiId, const std::u8string &_sVal ) {
		try {
			std::u8string sCopy = _sVal;
			const struct LSN_TABLE {
				const char8_t *					pcReplaceMe;
				const char8_t *					pcWithMe;
			} tTable[] = {
				{ u8"’", u8"'" },
				{ u8"‘", u8"'" },
				{ u8"…", u8"..." },
				{ u8"Ō", u8"O" },
				{ u8" ", u8" " },
				{ u8"é", u8"e" },
				{ u8"“", u8"\"" },
				{ u8"”", u8"\"" },
				{ u8"⅓", u8"(1/3rd)" },
				{ u8"\u016B", u8"u" },
				{ u8"\u014D", u8"o" },

				/*{ u8"ぁ", "a" },
				{ u8"あ", "A" },

				{ u8"ぃ", "i" },
				{ u8"い", "I" },

				{ u8"ぅ", "u" },
				{ u8"う", "U" },

				{ u8"ぇ", "e" },
				{ u8"え", "E" },

				{ u8"ぉ", "o" },
				{ u8"お", "O" },

				{ u8"か", "Ka" },
				{ u8"が", "Ga" },

				{ u8"き", "Ki" },
				{ u8"ぎ", "Gi" },

				{ u8"く", "Ku" },
				{ u8"ぐ", "Gu" },

				{ u8"け", "Ke" },
				{ u8"げ", "Ge" },

				{ u8"こ", "Ko" },
				{ u8"ご", "Go" },

				{ u8"さ", "Sa" },
				{ u8"ざ", "Za" },

				{ u8"し", "Shi" },
				{ u8"じ", "Ji" },

				{ u8"す", "Su" },
				{ u8"ず", "Zu" },

				{ u8"せ", "Se" },
				{ u8"ぜ", "Ze" },

				{ u8"そ", "So" },
				{ u8"ぞ", "Zo" },

				{ u8"た", "Ta" },
				{ u8"だ", "Da" },

				{ u8"ち", "Chi" },
				{ u8"ぢ", "Ji" },

				{ u8"っ", "-" },
				{ u8"つ", "Tsu" },
				{ u8"づ", "Zu" },

				{ u8"て", "Te" },
				{ u8"で", "De" },

				{ u8"と", "To" },
				{ u8"ど", "Do" },

				{ u8"な", "Na" },
				{ u8"に", "Ni" },
				{ u8"ぬ", "Nu" },
				{ u8"ね", "Ne" },
				{ u8"の", "No" },

				{ u8"は", "Ha" },
				{ u8"ば", "Ba" },
				{ u8"ぱ", "Pa" },

				{ u8"ひ", "Hi" },
				{ u8"び", "Bi" },
				{ u8"ぴ", "Pi" },

				{ u8"ふ", "Hu" },
				{ u8"ぶ", "Bu" },
				{ u8"ぷ", "Pu" },

				{ u8"へ", "He" },
				{ u8"べ", "Be" },
				{ u8"ぺ", "Pe" },

				{ u8"ほ", "Ho" },
				{ u8"ぼ", "Bo" },
				{ u8"ぽ", "Po" },

				{ u8"ま", "Ma" },
				{ u8"み", "Mi" },
				{ u8"む", "Mu" },
				{ u8"め", "Me" },
				{ u8"も", "Mo" },

				{ u8"ゃ", "ya" },
				{ u8"や", "Ya" },

				{ u8"ゅ", "yu" },
				{ u8"ゆ", "Yu" },

				{ u8"ょ", "yo" },
				{ u8"よ", "Yo" },

				{ u8"ら", "Ra" },
				{ u8"り", "Ri" },
				{ u8"る", "Ru" },
				{ u8"れ", "Re" },
				{ u8"ろ", "Ro" },

				{ u8"ゎ", "wa" },
				{ u8"わ", "Wa" },

				{ u8"ゐ", "Wi" },

				{ u8"ゑ", "We" },

				{ u8"を", "Wo" },

				{ u8"ん", "N" },

				{ u8"ゔ", "Vu" },

				{ u8"ゕ", "ka" },
				{ u8"ゖ", "ke" },			

				{ u8"ぷ", "Pu" },
				{ u8"よ", "Yo" },


				{ u8"ワ", "Wa" },
				{ u8"ン", "N" },
				{ u8"ダ", "Da" },
				{ u8"ー", "-" },
				{ u8"プ", "Pu" },
				{ u8"ロ", "Ro" },
				{ u8"ジ", "Ji" },
				{ u8"ェ", "e" },

				{ u8"ジェ", "Je" },

				{ u8"ク", "Ku" },
				{ u8"ト", "To" },
				{ u8"コ", "Ko" },
				{ u8"ル", "Ru" },
				{ u8"ジョ", "Jo" },
				{ u8"ゼ", "Ze" },
				{ u8"ゼ", "Ze" },
				{ u8"ゼッ", "Ze" },
				{ u8"ゼッ", "Ze" },
				{ u8"ズ", "Zu" },

				{ u8"森", "Mori" },
				{ u8"彰", "Aki" },
				{ u8"彦", "Hiko" },
				{ u8"彰彦", "Akihiko" },
				{ u8"魔獣使い伝説", "Maju Tsukai Densetsu" },
				{ u8"一郎", "Ichiro" },
				{ u8"根本", "Nemoto" },

				{ u8"Ｒ", "R" },
				{ u8"Ｅ", "E" },
				{ u8"Ｓ", "S" },
				{ u8"Ｕ", "U" },
				{ u8"Ｌ", "L" },
				{ u8"Ｔ", "T" },*/
			};
			for ( auto I = sizeof( tTable ) / sizeof( tTable[0] ); I--; ) {
				sCopy = CUtilities::Replace<std::u8string>( sCopy, tTable[I].pcReplaceMe, tTable[I].pcWithMe );
			};

			LSN_LIST_ENTRY lsEntry;
			lsEntry.u.uiIfoId = _uiId;
			lsEntry.sText = sCopy;
			lsEntry.sText.push_back( '\0' );
			if ( lsEntry.sText.size() & 1 ) {
				// Make it an odd number of characters because a hard-coded 0 will be printed into the file.
				lsEntry.sText.push_back( '\0' );
			}
			for ( auto I = m_vListEntries.size(); I--; ) {
				if ( m_vListEntries[I].u.uiIfoId == _uiId ) {
					m_vListEntries[I] = lsEntry;
					return true;
				}
			}
			m_vListEntries.push_back( lsEntry );
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Adds a DISP image.
	 * 
	 * \param _ui32Type The image type.  One of the CF_* values.
	 * \param _vImage The image data.
	 * \return Returns true if the entry was added.
	 **/
	bool CWavFile::AddImage( uint32_t _ui32Type, const std::vector<uint8_t> &_vImage ) {
		try {
			if ( _vImage.size() != uint32_t( _vImage.size() ) ) { return false; }
			LSN_DISP_ENTRY deEntry;
			deEntry.u.uiIfoId = LSN_C_DISP;
			deEntry.ui32Size = uint32_t( _vImage.size() );
			deEntry.ui32Type = _ui32Type;
			deEntry.vValue = _vImage;
			m_vDisp.push_back( deEntry );
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Loads data from the "fmt ".
	 *
	 * \param _pfcChunk The chunk of data to load
	 * \return Returns true if everything loaded fine.
	 */
	bool CWavFile::LoadFmt( const LSN_FMT_CHUNK * _pfcChunk ) {
		m_uiNumChannels = _pfcChunk->uiNumChannels;
		m_uiSampleRate = _pfcChunk->uiSampleRate;
		m_uiBitsPerSample = _pfcChunk->uiBitsPerSample;
		m_uiBytesPerSample = m_uiBitsPerSample / 8;
		m_fFormat = static_cast<LSN_FORMAT>(_pfcChunk->uiAudioFormat);
		return true;
	}

	/**
	 * Loads the "data" chunk.
	 *
	 * \param _pdcChunk The chunk of data to load
	 * \return Returns true if everything loaded fine.
	 */
	bool CWavFile::LoadData( const LSN_DATA_CHUNK * _pdcChunk ) {
		m_vSamples.resize( _pdcChunk->chHeader.uiSize );
		if ( m_vSamples.size() != _pdcChunk->chHeader.uiSize ) { return false; }
		std::memcpy( m_vSamples.data(), _pdcChunk->ui8Data, m_vSamples.size() );
		return true;
	}

	/**
	 * Loads the "smpl" chunk.
	 *
	 * \param _pscChunk The chunk of data to load
	 * \return Returns true if everything loaded fine.
	 */
	bool CWavFile::LoadSmpl( const LSN_SMPL_CHUNK * _pscChunk ) {
		m_uiBaseNote = _pscChunk->uiMIDIUnityNote;
		for ( size_t I = 0; I < _pscChunk->uiNumSampleLoops; ++I ) {
			if ( _pscChunk->lpLoops[I].uiStart < m_vSamples.size() && _pscChunk->lpLoops[I].uiEnd < m_vSamples.size() ) {
				m_vLoops.push_back( _pscChunk->lpLoops[I] );
			}
		}
		return true;
	}

	/**
	 * Loads a "LIST" chunk.
	 *
	 * \param _plcChunk The chunk of data to load
	 * \return Returns true if everything loaded fine.
	 */
	bool CWavFile::LoadList( const LSN_LIST_CHUNK * _plcChunk ) {
		const uint8_t * pui8Data = _plcChunk->ui8Data;
		const uint8_t * pui8End = pui8Data + _plcChunk->chHeader.uiSize - sizeof( _plcChunk->u );
		switch ( _plcChunk->u.uiTypeId ) {
			case LSN_C_INFO : {				// INFO
				while ( pui8Data < pui8End ) {
					LSN_LIST_ENTRY lsThis;
					if ( (pui8Data + sizeof( uint32_t )) > pui8End ) { return false; }
					lsThis.u.uiIfoId = (*reinterpret_cast<const uint32_t *>(pui8Data));
					pui8Data += sizeof( uint32_t );
					if ( (pui8Data + sizeof( uint32_t )) > pui8End ) { return false; }
					uint32_t ui32Size = (*reinterpret_cast<const uint32_t *>(pui8Data));
					pui8Data += sizeof( uint32_t );
					for ( uint32_t I = 0; I < ui32Size; ++I ) {
						if ( (pui8Data + sizeof( uint8_t )) > pui8End ) { return false; }
						lsThis.sText.push_back( (*pui8Data++) );
					}
					m_vListEntries.push_back( lsThis );
				}
				break;
			}
			case LSN_C_ADTL : {
				break;
			}
			default : {
				return false;
			}
		}
		

		return true;
	}

	/**
	 * Loads an "id3 " chunk.
	 *
	 * \param _plcChunk The chunk of data to load
	 * \return Returns true if everything loaded fine.
	 */
	bool CWavFile::LoadId3( const LSN_ID3_CHUNK * _picChunk ) {
		if ( _picChunk->ui16Version == 0x3 ) {
			uint32_t ui32Len = DecodeSize(_picChunk->ui32Size );

			for ( uint32_t ui32Offset = 0; ui32Offset < ui32Len; ) {
				uint32_t ui32Id = (*reinterpret_cast<const uint32_t *>(&_picChunk->ui8Data[ui32Offset]));
				ui32Offset += sizeof( uint32_t );
				uint32_t ui32Size = DecodeSize( (*reinterpret_cast<const uint32_t *>(&_picChunk->ui8Data[ui32Offset])) );
				ui32Offset += sizeof( uint32_t );
				uint16_t ui16Flags = (*reinterpret_cast<const uint16_t *>(&_picChunk->ui8Data[ui32Offset]));
				ui32Offset += sizeof( uint16_t );
				LSN_ID3_ENTRY ieEntry;
				ieEntry.u.uiIfoId = ui32Id;
				ieEntry.ui16Flags = ui16Flags;
				for ( uint32_t J = 0; J < ui32Size; ++J ) {
					ieEntry.sValue.push_back( _picChunk->ui8Data[ui32Offset++] );
				}
				m_vId3Entries.push_back( ieEntry );
			}
			return true;
		}

		return false;
	}

	/**
	 * Loads an "inst" chunk.
	 *
	 * \param _plcChunk The chunk of data to load
	 * \return Returns true if everything loaded fine.
	 */
	bool CWavFile::LoadInst( const LSN_INST_CHUNK * _picChunk ) {
#define LSN_COPY( VAL )		m_ieInstEntry.VAL = _picChunk->VAL
		LSN_COPY( ui8UnshiftedNote );
		LSN_COPY( ui8FineTune );
		LSN_COPY( ui8Gain );
		LSN_COPY( ui8LowNote );
		LSN_COPY( ui8HiNote );
		LSN_COPY( ui8LowVel );
		LSN_COPY( ui8HiVel );

#undef LSN_COPY
		return true;
	}

	/**
	 * Converts a bunch of 8-bit PCM samples to double.
	 *
	 * \param _ui32From Starting sample index.
	 * \param _ui32To Ending sample index.
	 * \param _uiChan The channel whose sample data is to be obtained.
	 * \param _vResult The vector containing the samples.
	 * \return Returns true if the vector was able to hold all of the values.
	 */
	bool CWavFile::Pcm8ToF64( uint32_t _ui32From, uint32_t _ui32To, uint16_t _uiChan, lwtrack &_vResult ) const {
		size_t sFinalSize = _vResult.size() + (_ui32To - _ui32From);
		_vResult.reserve( sFinalSize );
		uint32_t uiStride;
		size_t sIdx = CalcOffsetsForSample( _uiChan, _ui32From, uiStride );
		const int8_t * pi8Samples = reinterpret_cast<const int8_t *>(&m_vSamples.data()[sIdx]);
		while ( _ui32From < _ui32To ) {
			_vResult.push_back( (static_cast<int32_t>((*pi8Samples)) - 128) / 127.0 );

			pi8Samples += m_uiNumChannels;
			++_ui32From;
		}
		return true;
	}

	/**
	 * Converts a bunch of 16-bit PCM samples to double.
	 *
	 * \param _ui32From Starting sample index.
	 * \param _ui32To Ending sample index.
	 * \param _uiChan The channel whose sample data is to be obtained.
	 * \param _vResult The vector containing the samples.
	 * \return Returns true if the vector was able to hold all of the values.
	 */
	bool CWavFile::Pcm16ToF64( uint32_t _ui32From, uint32_t _ui32To, uint16_t _uiChan, lwtrack &_vResult ) const {
		const double dFactor = std::pow( 2.0, 16.0 - 1.0 ) - 1.0;
		size_t sFinalSize = _vResult.size() + (_ui32To - _ui32From);
		_vResult.reserve( sFinalSize );
		uint32_t uiStride;
		size_t sIdx = CalcOffsetsForSample( _uiChan, _ui32From, uiStride );
		const int16_t * pi16Samples = reinterpret_cast<const int16_t *>(&m_vSamples.data()[sIdx]);
		while ( _ui32From < _ui32To ) {
			_vResult.push_back( (*pi16Samples) / dFactor );

			pi16Samples += m_uiNumChannels;
			++_ui32From;
		}
		return true;
	}

	/**
	 * Converts a bunch of 24-bit PCM samples to double.
	 *
	 * \param _ui32From Starting sample index.
	 * \param _ui32To Ending sample index.
	 * \param _uiChan The channel whose sample data is to be obtained.
	 * \param _vResult The vector containing the samples.
	 * \return Returns true if the vector was able to hold all of the values.
	 */
	bool CWavFile::Pcm24ToF64( uint32_t _ui32From, uint32_t _ui32To, uint16_t _uiChan, lwtrack &_vResult ) const {
		const double dFactor = (std::pow( 2.0, 24.0 - 1.0 ) - 1.0) * 256.0;
		size_t sFinalSize = _vResult.size() + (_ui32To - _ui32From);
		_vResult.reserve( sFinalSize );
		uint32_t uiStride;
		size_t sIdx = CalcOffsetsForSample( _uiChan, _ui32From, uiStride );
		while ( _ui32From < _ui32To ) {
			if ( sIdx >= m_vSamples.size() ) {
				_vResult.push_back( 0.0 );
			}
			else {
				const int32_t * pi32Samples = reinterpret_cast<const int32_t *>(&m_vSamples.data()[sIdx]);

				_vResult.push_back( ((*pi32Samples) << 8) / dFactor );
			}

			sIdx += uiStride;
			++_ui32From;
		}
		return true;
	}

	/**
	 * Converts a bunch of 32-bit PCM samples to double.
	 *
	 * \param _ui32From Starting sample index.
	 * \param _ui32To Ending sample index.
	 * \param _uiChan The channel whose sample data is to be obtained.
	 * \param _vResult The vector containing the samples.
	 * \return Returns true if the vector was able to hold all of the values.
	 */
	bool CWavFile::Pcm32ToF64( uint32_t _ui32From, uint32_t _ui32To, uint16_t _uiChan, lwtrack &_vResult ) const {
		const double dFactor = std::pow( 2.0, 32.0 - 1.0 ) - 1.0;
		size_t sFinalSize = _vResult.size() + (_ui32To - _ui32From);
		_vResult.reserve( sFinalSize );
		uint32_t uiStride;
		size_t sIdx = CalcOffsetsForSample( _uiChan, _ui32From, uiStride );
		const int32_t * pi32Samples = reinterpret_cast<const int32_t *>(&m_vSamples.data()[sIdx]);
		while ( _ui32From < _ui32To ) {
			_vResult.push_back( (*pi32Samples) / dFactor );

			pi32Samples += m_uiNumChannels;
			++_ui32From;
		}
		return true;
	}

	/**
	 * Converts a bunch of 32-bit float samples to double.
	 *
	 * \param _ui32From Starting sample index.
	 * \param _ui32To Ending sample index.
	 * \param _uiChan The channel whose sample data is to be obtained.
	 * \param _vResult The vector containing the samples.
	 * \return Returns true if the vector was able to hold all of the values.
	 */
	bool CWavFile::F32ToF64( uint32_t _ui32From, uint32_t _ui32To, uint16_t _uiChan, lwtrack &_vResult ) const {
		size_t sFinalSize = _vResult.size() + (_ui32To - _ui32From);
		_vResult.reserve( sFinalSize );
		uint32_t uiStride;
		size_t sIdx = CalcOffsetsForSample( _uiChan, _ui32From, uiStride );
		const float * pfSamples = reinterpret_cast<const float *>(&m_vSamples.data()[sIdx]);
		while ( _ui32From < _ui32To ) {
			_vResult.push_back( (*pfSamples) );

			pfSamples += m_uiNumChannels;
			++_ui32From;
		}
		return true;
	}

	/**
	 * Converts a batch of F64 samples to PCM samples.
	 *
	 * \param _vSrc The samples to convert.
	 * \param _vDst The buffer to which to convert the samples.
	 * \return Returns trye if all samples were added to the buffer.
	 */
	bool CWavFile::BatchF64ToPcm8( const lwaudio &_vSrc, std::vector<uint8_t> &_vDst ) {
		for ( size_t I = 0; I < _vSrc[0].size(); ++I ) {
			for ( size_t J = 0; J < _vSrc.size(); ++J ) {
				int8_t iSample = static_cast<int8_t>(std::round( std::clamp( _vSrc[J][I], -1.0, 1.0 ) * 127.0 + 128.0 ));
				_vDst.push_back( static_cast<uint8_t>(iSample) );
			}
		}
		return true;
	}

	/**
	 * Converts a batch of F64 samples to PCM samples.
	 *
	 * \param _vSrc The samples to convert.
	 * \param _vDst The buffer to which to convert the samples.
	 * \return Returns trye if all samples were added to the buffer.
	 */
	bool CWavFile::BatchF64ToPcm16( const lwaudio &_vSrc, std::vector<uint8_t> &_vDst ) {
		try {
			const double dFactor = std::pow( 2.0, 16.0 - 1.0 ) - 1.0;
			auto stNumSamples = _vSrc[0].size();
			auto stNumChannels = _vSrc.size();
			auto aSize = _vDst.size();
			_vDst.reserve( _vDst.size() + stNumSamples * stNumChannels * sizeof( int16_t ) );
			int16_t * pi16Dst = reinterpret_cast<int16_t *>(_vDst.data() + aSize);

#ifdef __AVX512BW__
			if ( CUtilities::IsAvx512BWSupported() ) {
				// Constants for AVX-512 operations
				__m512d vdFactor = _mm512_set1_pd( dFactor );
				__m512d vdMin = _mm512_set1_pd( -1.0 );
				__m512d vdMax = _mm512_set1_pd( 1.0 );
				std::vector<std::vector<uint16_t>, CAlignmentAllocator<std::vector<uint16_t>, 64>> vSamples;
				vSamples.resize( stNumChannels );
				for ( auto I = stNumChannels; I--; ) {
					vSamples[I].resize( stNumSamples );
				}
				
				for ( auto C = stNumChannels; C--; ) {
					lwaudio::size_type stIdx = 0;
					while ( stIdx < stNumSamples - (8 - 1) ) {
						__m512d vdSamples = _mm512_load_pd( &_vSrc[C][stIdx] );

						// Clamp samples between -1.0 and 1.0.
						vdSamples = _mm512_max_pd( vdSamples, vdMin );
						vdSamples = _mm512_min_pd( vdSamples, vdMax );

						// Multiply by scaling factor.
						vdSamples = _mm512_mul_pd( vdSamples, vdFactor );

						// Round to nearest integer and convert to int32_t
						__m256i viInt32 = _mm512_cvt_roundpd_epi32( vdSamples, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC );

						// Convert int32_t to int16_t with saturation
						__m128i viInt16 = _mm256_cvtsepi32_epi16( viInt32 );

						_mm_store_si128( reinterpret_cast<__m128i *>(&vSamples[C][stIdx]), viInt16 );


						stIdx += sizeof( __m512d ) / sizeof( double );
					}

					while ( stIdx < stNumSamples ) {
						vSamples[C][stIdx++] = static_cast<int16_t>(std::round( std::clamp( _vSrc[C][stIdx], -1.0, 1.0 ) * dFactor ));
					}
				}

				for ( size_t I = 0; I < _vSrc[0].size(); ++I ) {
					for ( size_t J = 0; J < _vSrc.size(); ++J ) {
						(*pi16Dst++) = vSamples[J][I];
					}
				}

				return true;
			}
#endif	// #ifdef __AVX512BW__

			for ( size_t I = 0; I < _vSrc[0].size(); ++I ) {
				for ( size_t J = 0; J < _vSrc.size(); ++J ) {
					(*pi16Dst++) = static_cast<int16_t>(std::round( std::clamp( _vSrc[J][I], -1.0, 1.0 ) * dFactor ));
				}
			}
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Converts a batch of F64 samples to PCM samples.
	 *
	 * \param _vSrc The samples to convert.
	 * \param _vDst The buffer to which to convert the samples.
	 * \return Returns trye if all samples were added to the buffer.
	 */
	bool CWavFile::BatchF64ToPcm24( const lwaudio &_vSrc, std::vector<uint8_t> &_vDst ) {
		try {
			const double dFactor = std::pow( 2.0, 24.0 - 1.0 ) - 1.0;
			auto stNumSamples = _vSrc[0].size();
			auto stNumChannels = _vSrc.size();
			auto aSize = _vDst.size();
			_vDst.resize( _vDst.size() + stNumSamples * stNumChannels * 3 );
			int8_t * pi8Dst = reinterpret_cast<int8_t *>(_vDst.data() + aSize);

			for ( size_t I = 0; I < _vSrc[0].size(); ++I ) {
				for ( size_t J = 0; J < _vSrc.size(); ++J ) {
					int32_t iSample = static_cast<int32_t>(std::round( std::clamp( _vSrc[J][I], -1.0, 1.0 ) * dFactor ));
					(*pi8Dst++) = static_cast<uint8_t>(iSample);
					(*pi8Dst++) = static_cast<uint8_t>(iSample >> 8);
					(*pi8Dst++) = static_cast<uint8_t>(iSample >> 16);
					/*_vDst.push_back( static_cast<uint8_t>(iSample) );
					_vDst.push_back( static_cast<uint8_t>(iSample >> 8) );
					_vDst.push_back( static_cast<uint8_t>(iSample >> 16) );*/
				}
			}
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Converts a batch of F64 samples to PCM samples.
	 *
	 * \param _vSrc The samples to convert.
	 * \param _vDst The buffer to which to convert the samples.
	 * \return Returns trye if all samples were added to the buffer.
	 */
	bool CWavFile::BatchF64ToPcm32( const lwaudio &_vSrc, std::vector<uint8_t> &_vDst ) {
		try {
			const double dFactor = std::pow( 2.0, 32.0 - 1.0 ) - 1.0;
			auto stNumSamples = _vSrc[0].size();
			auto stNumChannels = _vSrc.size();
			auto aSize = _vDst.size();
			_vDst.resize( _vDst.size() + stNumSamples * stNumChannels * sizeof( int32_t ) );
			int32_t * pi32Dst = reinterpret_cast<int32_t *>(_vDst.data() + aSize);

#ifdef __AVX512BW__
			if ( CUtilities::IsAvx512BWSupported() ) {
				// Constants for AVX-512 operations
				__m512d vdFactor = _mm512_set1_pd( dFactor );
				__m512d vdMin = _mm512_set1_pd( -1.0 );
				__m512d vdMax = _mm512_set1_pd( 1.0 );
				std::vector<std::vector<uint32_t>, CAlignmentAllocator<std::vector<uint32_t>, 64>> vSamples;
				vSamples.resize( stNumChannels );
				for ( auto I = stNumChannels; I--; ) {
					vSamples[I].resize( stNumSamples );
				}
				
				for ( auto C = stNumChannels; C--; ) {
					lwaudio::size_type stIdx = 0;
					while ( stIdx < stNumSamples - (8 - 1) ) {
						__m512d vdSamples = _mm512_load_pd( &_vSrc[C][stIdx] );

						// Clamp samples between -1.0 and 1.0.
						vdSamples = _mm512_max_pd( vdSamples, vdMin );
						vdSamples = _mm512_min_pd( vdSamples, vdMax );

						// Multiply by scaling factor.
						vdSamples = _mm512_mul_pd( vdSamples, vdFactor );

						// Round to nearest integer and convert to int32_t
						__m256i viInt32 = _mm512_cvt_roundpd_epi32( vdSamples, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC );

						// Store the int32_t samples.
						_mm256_store_si256( reinterpret_cast<__m256i*>(&vSamples[C][stIdx]), viInt32 );


						stIdx += sizeof( __m512d ) / sizeof( double );
					}

					while ( stIdx < stNumSamples ) {
						vSamples[C][stIdx++] = static_cast<int32_t>(std::round( std::clamp( _vSrc[C][stIdx], -1.0, 1.0 ) * dFactor ));
					}
				}

				for ( size_t I = 0; I < _vSrc[0].size(); ++I ) {
					for ( size_t J = 0; J < _vSrc.size(); ++J ) {
						(*pi32Dst++) = vSamples[J][I];
					}
				}

				return true;
			}
#endif	// #ifdef __AVX512BW__

			for ( size_t I = 0; I < _vSrc[0].size(); ++I ) {
				for ( size_t J = 0; J < _vSrc.size(); ++J ) {
					(*pi32Dst++) = static_cast<int32_t>(std::round( std::clamp( _vSrc[J][I], -1.0, 1.0 ) * dFactor ));
				}
			}
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Gets the byte indices of PCM data given an offset and channel.
	 *
	 * \param _uiChan The channel.
	 * \param _uiIdx The sample index.
	 * \param _uiStride The returned stride.
	 * \return Returns the byte index calculated using the given information and the sample depth.
	 */
	size_t CWavFile::CalcOffsetsForSample( uint16_t _uiChan, uint32_t _uiIdx, uint32_t &_uiStride ) const {
		_uiStride = m_uiNumChannels * m_uiBytesPerSample;
		return _uiStride * _uiIdx + (_uiChan * m_uiBytesPerSample);
	}

	/**
	 * Converts a 28-bit size value from ID3 into regular 32-bit.
	 *
	 * \param _uiSize The size value to decode.
	 * \return Returns the decoded size value.
	 */
	uint32_t CWavFile::DecodeSize( uint32_t _uiSize ) {
		return ((_uiSize >> 24) & 0x7F) |
			(((_uiSize >> 16) & 0x7F) << 7) |
			(((_uiSize >> 8) & 0x7F) << 14) |
			(((_uiSize >> 0) & 0x7F) << 21);
	}

	/**
	 * Creates an "fmt " chunk based off either this object's parameters or optional given overrides.
	 *
	 * \param _fFormat The format to set.
	 * \param _uiChannels The total channels.
	 * \param _psdSaveSettings Optional overrides.
	 * \return Returns a created object.
	 */
	CWavFile::LSN_FMT_CHUNK CWavFile::CreateFmt( LSN_FORMAT _fFormat, uint16_t _uiChannels, const LSN_SAVE_DATA * _psdSaveSettings ) const {
		LSN_FMT_CHUNK fcChunk = { 0 };
		fcChunk.chHeader.u.uiId = LSN_C_FMT_;
		fcChunk.chHeader.uiSize = 16;
		fcChunk.uiAudioFormat = _fFormat;
		fcChunk.uiBitsPerSample = (_psdSaveSettings && _psdSaveSettings->uiBitsPerSample) ? _psdSaveSettings->uiBitsPerSample : BitsPerSample();
		fcChunk.uiNumChannels = _uiChannels;
		fcChunk.uiSampleRate = (_psdSaveSettings && _psdSaveSettings->uiHz) ? _psdSaveSettings->uiHz : Hz();
		fcChunk.uiBlockAlign = fcChunk.uiBitsPerSample * fcChunk.uiNumChannels / 8;
		fcChunk.uiByteRate = fcChunk.uiBlockAlign * fcChunk.uiSampleRate;

		return fcChunk;
	}

	/**
	 * Writes file-image loop-point bytes to a vector.
	 *
	 * \return Returns the bytes that represent the "smpl" chunk in a file.
	 */
	std::vector<uint8_t> CWavFile::CreateSmpl() const {
		std::vector<uint8_t> vRet;
#define LSN_PUSH32( VAL )		vRet.push_back( static_cast<uint8_t>((VAL) >> 0) ); vRet.push_back( static_cast<uint8_t>((VAL) >> 8) ); vRet.push_back( static_cast<uint8_t>((VAL) >> 16) ); vRet.push_back( static_cast<uint8_t>((VAL) >> 24) )
		LSN_PUSH32( LSN_C_SMPL );			// "smpl"
		uint32_t uiVal = static_cast<uint32_t>(36 + m_vLoops.size() * 24 + 0);
		LSN_PUSH32( uiVal );				// 36 + (Num Sample Loops * 24) + Sampler Data
		LSN_PUSH32( 0 );					// Manufacturer.
		LSN_PUSH32( 0 );					// Product.
		LSN_PUSH32( 0 );					// Sample Period.
		LSN_PUSH32( m_uiBaseNote );		// MIDI Unity Note.
		LSN_PUSH32( 0 );					// MIDI Pitch Fraction.
		LSN_PUSH32( 0 );					// SMPTE Format.
		LSN_PUSH32( 0 );					// SMPTE Offset.
		LSN_PUSH32( m_vLoops.size() );	// Num Sample Loops.
		LSN_PUSH32( 0 );					// Sampler Data.
		for ( size_t I = 0; I < m_vLoops.size(); ++I ) {
			LSN_PUSH32( m_vLoops[I].uiCuePointID );			// Cue Point ID.

			/**
			0	Loop forward (normal)
			1	Alternating loop (forward/backward, also known as Ping Pong)
			2	Loop backward (reverse)
			3 - 31	Reserved for future standard types
			32 - 0xFFFFFFFF	Sampler specific types (defined by manufacturer) */
			LSN_PUSH32( m_vLoops[I].uiType );				// Type.
			LSN_PUSH32( m_vLoops[I].uiStart );				// Start.
			LSN_PUSH32( m_vLoops[I].uiEnd );					// End.
			LSN_PUSH32( m_vLoops[I].uiFraction );			// Fraction.
			LSN_PUSH32( m_vLoops[I].uiPlayCount );			// Play Count.
		}
#undef LSN_PUSH32
		return vRet;
	}

	/**
	 * Writes file-image "LIST" chunk to a vector.
	 *
	 * \return Returns the bytes that represent the "LIST" chunk in a file.
	 */
	std::vector<uint8_t> CWavFile::CreateList() const {
		std::vector<uint8_t> vRet;
#define LSN_PUSH32( VAL )		vRet.push_back( static_cast<uint8_t>((VAL) >> 0) ); vRet.push_back( static_cast<uint8_t>((VAL) >> 8) ); vRet.push_back( static_cast<uint8_t>((VAL) >> 16) ); vRet.push_back( static_cast<uint8_t>((VAL) >> 24) )
		LSN_PUSH32( LSN_C_LIST );			// "LIST"
		uint32_t ui32Size = 4;
		for ( auto I = m_vListEntries.size(); I--; ) {
			ui32Size += static_cast<uint32_t>(m_vListEntries[I].sText.size()) + 8;
		}
		LSN_PUSH32( ui32Size );			// Size.
		LSN_PUSH32( LSN_C_INFO );			// "INFO"
		for ( size_t I = 0; I < m_vListEntries.size(); ++I ) {
			LSN_PUSH32( m_vListEntries[I].u.uiIfoId );
			LSN_PUSH32( static_cast<uint32_t>(m_vListEntries[I].sText.size()) );
			for ( size_t J = 0; J < m_vListEntries[I].sText.size(); ++J ) {
				vRet.push_back( static_cast<uint8_t>(m_vListEntries[I].sText[J]) );
			}
		}
#undef LSN_PUSH32
		return vRet;
	}

	/**
	 * Creates the file for streaming and writes the header data to it, preparing it for writing samples.
	 * 
	 * \param _pcPath Uses data loaded into m_sStream to create a new file.
	 * \return Returns true if the file was created and the header was written to it.
	 **/
	bool CWavFile::CreateStreamFile( const char8_t * _pcPath ) {
		LSN_SAVE_DATA sdSaveSettings( m_sStream.ui32Hz, m_sStream.ui16Bits );
		
		if ( !m_sStream.sfFile.Create( _pcPath ) ) {
			std::wprintf( L"Failed to create stream file: %s.\r\n", reinterpret_cast<const wchar_t *>(CUtilities::Utf8ToUtf16( _pcPath ).c_str()) );
			return false;
		}

		LSN_FMT_CHUNK fcChunk = CreateFmt( m_sStream.fFormat, m_sStream.ui16Channels,
			&sdSaveSettings );

		uint32_t uiFmtSize = fcChunk.chHeader.uiSize + 8;

		uint32_t ui32Samples = 0;//uint32_t( std::min<uint64_t>( UINT_MAX, ui64Samples ));
		uint32_t ui32DataSize = CalcSize( m_sStream.fFormat, ui32Samples, m_sStream.ui16Channels, fcChunk.uiBitsPerSample );

		uint32_t ui32Size = 4 +							// "WAVE".
			uiFmtSize +									// "fmt " chunk.
			ui32DataSize + 8 +							// "data" chunk.
			0;

		if ( !m_sStream.sfFile.WriteUi32( LSN_C_RIFF ) ) { return false; }

		m_sStream.ui64WavFileOffset_Size = m_sStream.sfFile.GetPos();
		if ( !m_sStream.sfFile.Write( ui32Size ) ) { return false; }
		
		if ( !m_sStream.sfFile.WriteUi32( LSN_C_WAVE ) ) { return false; }

		if ( !m_sStream.sfFile.WriteToFile( reinterpret_cast<const uint8_t *>(&fcChunk), uiFmtSize ) ) { return false; }
		// Append the "data" chunk.
		if ( !m_sStream.sfFile.WriteUi32( LSN_C_DATA ) ) { return false; }
		
		m_sStream.ui64WavFileOffset_DSize = m_sStream.sfFile.GetPos();
		if ( !m_sStream.sfFile.Write( ui32DataSize ) ) { return false; }
		
		// File now ready for streaming.
		
		m_sStream.ui32WavFile_Size = 4 + uiFmtSize;
		m_sStream.ui32WavFile_DSize = 0;

		//m_sStream.ui64FinalSampleCount = ui64Samples;
		return true;
	}

	/**
	 * Closes the current streaming file.
	 **/
	void CWavFile::CloseStreamFile() {
		if LSN_LIKELY( m_sStream.sfFile.IsOpen() ) {
			if LSN_LIKELY( m_sStream.bStreaming ) {
				uint32_t ui32Size = uint32_t( m_sStream.ui32WavFile_DSize * uint64_t( m_sStream.ui16Bits ) / (sizeof( float ) * 8) );

				m_sStream.sfFile.MovePointerTo( m_sStream.ui64WavFileOffset_Size );
				m_sStream.sfFile.Write( m_sStream.ui32WavFile_Size + ui32Size );

				m_sStream.sfFile.MovePointerTo( m_sStream.ui64WavFileOffset_DSize );
				m_sStream.sfFile.Write( ui32Size );
			}
		
			m_sStream.sfFile.Close();
			m_sStream.ui32WavFile_DSize = 0;
		}
		

		{
			std::unique_lock<std::mutex> ulLockMeta( m_sStream.mMetaMutex );
			if ( m_sStream.bMeta && m_sStream.pfMetaFunc ) {
				AddMetaData();
			}
		}

		if ( m_sStream.bMeta ) {
			m_sStream.cvMetaCondition.notify_one();
			if ( m_sStream.tMetaThread.joinable()) {
				m_sStream.tMetaThread.join();
			}
			m_sStream.bMeta = false;
		}
	}

	/**
	 * Creates the file for streaming metadata.  All writes to the file are handled by a callback.
	 * 
	 * \param _pcPath Uses data loaded into m_sStream to create a new file.
	 * \return Returns true if the file was created.
	 **/
	bool CWavFile::CreateStreamMetaFile( const char8_t * _pcPath ) {
		if ( !m_sStream.sfMetaFile.Create( _pcPath ) ) {
			std::wprintf( L"Failed to create stream metadata file: %s.\r\n", reinterpret_cast<const wchar_t *>(CUtilities::Utf8ToUtf16( _pcPath ).c_str()) );
			return false;
		}
		return true;
	}

	/**
	 * Closes the current streaming metadata file.
	 **/
	void CWavFile::CloseStreamMetaFile() {
		if ( m_sStream.sfMetaFile.IsOpen() ) {
			m_sStream.sfMetaFile.Close();
		}
	}

	/**
	 * The stream-to-file writer thread.
	 **/
	void CWavFile::StreamWriterThread() {
		std::vector<uint8_t> vConversionBuffer;
		while ( true ) {
			std::vector<float> vBufferToWrite;
			{
				std::unique_lock<std::mutex> ulLock( m_sStream.mMutex );
				// Wait until there is a full buffer or a shutdown is signaled.
                m_sStream.cvCondition.wait( ulLock, [this] {
                    return !m_sStream.qBufferQueue.empty() || m_sStream.bEnd;
                });

				// If there’s data to write, retrieve the next buffer.
                if ( !m_sStream.qBufferQueue.empty() ) {
                    vBufferToWrite = std::move( m_sStream.qBufferQueue.front() );
                    m_sStream.qBufferQueue.pop();
                }
				else if ( m_sStream.bEnd ) {
                    // No more buffers and shutdown requested.
                    break;
                }
			}
			// Write the buffer to disk (if any).
            if LSN_UNLIKELY( !vBufferToWrite.empty() ) {
				(*m_sStream.pfCvtAndWriteFunc)( vBufferToWrite, vConversionBuffer, m_sStream );
            }
		}
		CloseStreamFile();
	}

	/**
	 * The metadata-to-file writer thread.
	 **/
	void CWavFile::MetadataWriterThread() {
		std::vector<uint8_t> vConversionBuffer;
		while ( true ) {
			std::vector<uint8_t, CAlignmentAllocator<uint8_t, 64>> vTmpBuffer;
			{
				std::unique_lock<std::mutex> ulLock( m_sStream.mMetaMutex );
				// Wait until there is a full buffer or a shutdown is signaled.
				m_sStream.cvMetaCondition.wait( ulLock, [this] {
					return !m_sStream.qMetaBufferQueue.empty() || m_sStream.bEnd;
				});

				// If there’s data to write, retrieve the next buffer.
				if ( !m_sStream.qMetaBufferQueue.empty() ) {
					vTmpBuffer = std::move( m_sStream.qMetaBufferQueue.front() );
					m_sStream.qMetaBufferQueue.pop();
				}
				else if ( m_sStream.bEnd ) {
					// No more buffers and shutdown requested.
					break;
				}
			}
			// Write the buffer to disk (if any).
            if LSN_UNLIKELY( !vTmpBuffer.empty() ) {
				(*m_sStream.pfMetaThreadFunc)( this, m_sStream.pvMetaParm, m_sStream, vTmpBuffer, vConversionBuffer );
            }
		}
		CloseStreamMetaFile();
	}

	/**
	 * The None start condition.  Returns true.
	 * 
	 * \param _ui64AbsoluteIdx The absolute index of the sample being tested.
	 * \param _fSample The value of the sample being tested.
	 * \param _cdData Function-specific data.
	 * \return Returns true if the start condition has been met.
	 **/
	bool LSN_STDCALL CWavFile::StartCondFunc_None( uint64_t /*_ui64AbsoluteIdx*/, float /*_fSample*/, LSN_CONDITIONS_DATA &/*_cdData*/ ) { return true; }

	/**
	 * The Start-at-Sample start condition.  Returns true if _ui64AbsoluteIdx >= _cdData.ui64Parm0.
	 * 
	 * \param _ui64AbsoluteIdx The absolute index of the sample being tested.
	 * \param _fSample The value of the sample being tested.
	 * \param _cdData Function-specific data.
	 * \return Returns true if the start condition has been met.
	 **/
	bool LSN_STDCALL CWavFile::StartCondFunc_StartAtSample( uint64_t _ui64AbsoluteIdx, float /*_fSample*/, LSN_CONDITIONS_DATA &_cdData ) { return _ui64AbsoluteIdx >= _cdData.ui64Parm0; }

	/**
	 * The First-Non-Zero start condition.  Returns true if _fSample != 0.0f.
	 * 
	 * \param _ui64AbsoluteIdx The absolute index of the sample being tested.
	 * \param _fSample The value of the sample being tested.
	 * \param _cdData Function-specific data.
	 * \return Returns true if the start condition has been met.
	 **/
	bool LSN_STDCALL CWavFile::StartCondFunc_FirstNonZero( uint64_t /*_ui64AbsoluteIdx*/, float _fSample, LSN_CONDITIONS_DATA &/*_cdData*/ ) { return _fSample != 0.0f; }

	/**
	 * The Zero-For-Duration start condition.  Returns true if _cdData.ui64Counter >= _cdData.ui64Parm0, with _cdData.ui64Counter being counted on each 0 sample.
	 * 
	 * \param _ui64AbsoluteIdx The absolute index of the sample being tested.
	 * \param _fSample The value of the sample being tested.
	 * \param _cdData Function-specific data.
	 * \return Returns true if the start condition has been met.
	 **/
	bool LSN_STDCALL CWavFile::StartCondFunc_ZeroForDuration( uint64_t /*_ui64AbsoluteIdx*/, float _fSample, LSN_CONDITIONS_DATA &_cdData ) {
		if ( _fSample && _cdData.ui64Counter >= _cdData.ui64Parm0 ) { return true; }
		if ( _fSample ) { _cdData.ui64Counter = 0; }
		else { ++_cdData.ui64Counter; }
		return false;
	}
		
	/**
	 * The None end condition.  Returns true.
	 * 
	 * \param _ui64AbsoluteIdx The absolute index of the sample being tested.
	 * \param _ui64IdxSinceStart The index of the sample being tested since recording began.
	 * \param _fSample The value of the sample being tested.
	 * \param _cdData Function-specific data.
	 * \return Returns false if the end condition has been met, at which point recording should stop.
	 **/
	bool LSN_STDCALL CWavFile::EndCondFunc_None( uint64_t /*_ui64AbsoluteIdx*/, uint64_t /*_ui64IdxSinceStart*/, float /*_fSample*/, LSN_CONDITIONS_DATA &/*_cdData*/ ) { return true; }

	/**
	 * The End-At-Sample end condition.  Returns true if _ui64AbsoluteIdx < _cdData.ui64Parm0.
	 * 
	 * \param _ui64AbsoluteIdx The absolute index of the sample being tested.
	 * \param _ui64IdxSinceStart The index of the sample being tested since recording began.
	 * \param _fSample The value of the sample being tested.
	 * \param _cdData Function-specific data.
	 * \return Returns false if the end condition has been met, at which point recording should stop.
	 **/
	bool LSN_STDCALL CWavFile::EndCondFunc_EndAtSample( uint64_t _ui64AbsoluteIdx, uint64_t /*_ui64IdxSinceStart*/, float /*_fSample*/, LSN_CONDITIONS_DATA &_cdData ) { return _ui64AbsoluteIdx < _cdData.ui64Parm0; }

	/**
	 * The Zero-For-Duration end condition.  Returns true if _cdData.ui64Counter < _cdData.ui64Parm0, with _cdData.ui64Counter counting consecutive 0's.
	 * 
	 * \param _ui64AbsoluteIdx The absolute index of the sample being tested.
	 * \param _ui64IdxSinceStart The index of the sample being tested since recording began.
	 * \param _fSample The value of the sample being tested.
	 * \param _cdData Function-specific data.
	 * \return Returns false if the end condition has been met, at which point recording should stop.
	 **/
	bool LSN_STDCALL CWavFile::EndCondFunc_ZeroForDuration( uint64_t /*_ui64AbsoluteIdx*/, uint64_t /*_ui64IdxSinceStart*/, float _fSample, LSN_CONDITIONS_DATA &_cdData ) {
		if LSN_LIKELY( _fSample ) { _cdData.ui64Counter = 0; return false; }
		return ++_cdData.ui64Counter < _cdData.ui64Parm0;
	}

	/**
	 * The Duration end condition.  Returns true if _ui64IdxSinceStart < _cdData.ui64Parm0.
	 * 
	 * \param _ui64AbsoluteIdx The absolute index of the sample being tested.
	 * \param _ui64IdxSinceStart The index of the sample being tested since recording began.
	 * \param _fSample The value of the sample being tested.
	 * \param _cdData Function-specific data.
	 * \return Returns false if the end condition has been met, at which point recording should stop.
	 **/
	bool LSN_STDCALL CWavFile::EndCondFunc_Duration( uint64_t /*_ui64AbsoluteIdx*/, uint64_t _ui64IdxSinceStart, float /*_fSample*/, LSN_CONDITIONS_DATA &_cdData ) { return _ui64IdxSinceStart < _cdData.ui64Parm0; }

	/**
	 * Checks the starting condition before adding a sample.  Once the starting condition has been reached, _sStream.pfAddSampleFunc is changed to AddSample_CheckStopCond().
	 * 
	 * \param _fSample The sampple to add.
	 * \param _sStream The stream data.
	 * \return Returns true if the starting condition has been reached.
	 **/
	bool LSN_STDCALL CWavFile::AddSample_CheckStartCond( float _fSample, LSN_STREAMING &_sStream ) {
		if ( (*_sStream.pfStartCondFunc)( _sStream.ui64SamplesReceived, _fSample, _sStream.cdStartData ) ) {
			_sStream.pfAddSampleFunc = &CWavFile::AddSample_CheckStopCond;
			return (*_sStream.pfAddSampleFunc)( _fSample, _sStream );
		}
		return false;
	}

	/**
	 * Checks the stopping condition before adding a sample.  Once the stopping condition has been reached, the stream ends. 
	 * 
	 * \param _fSample The sampple to add.
	 * \param _sStream The stream data.
	 * \return Returns true for as long as samples should be output.  Once false is returned, the ending condition has been reached and the stream should close without adding the sample.
	 **/
	bool LSN_STDCALL CWavFile::AddSample_CheckStopCond( float _fSample, LSN_STREAMING &_sStream ) {
		if LSN_UNLIKELY( !(*_sStream.pfEndCondFunc)( _sStream.ui64SamplesReceived, _sStream.ui64SamplesWritten, _fSample, _sStream.cdStopData ) ) {
			_sStream.bEnd = true;
			return false;
		}
		return true;
	}

}	// namespace lsn

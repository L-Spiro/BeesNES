/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A WAV file.
 */


#pragma once

#include "../File/LSNStdFile.h"
#include "../Utilities/LSNAlignmentAllocator.h"
#include "../Utilities/LSNStreamBase.h"
#include "../Utilities/LSNUtilities.h"

#include <cinttypes>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>



namespace lsn {

	/**
	 * Class CWavFile
	 * \brief A WAV file.
	 *
	 * Description: A WAV file.
	 */
	class CWavFile {
	public :
		struct LSN_STREAMING;
		CWavFile();
		~CWavFile();


		// == Enumerations.
		/** Formats. */
		enum LSN_FORMAT : uint16_t {
			LSN_F_PCM													= 0x1,
			LSN_F_ADPCM													= 0x2,
			LSN_F_IEEE_FLOAT											= 0x3,
			LSN_F_ALAW													= 0x6,
			LSN_F_MULAW													= 0x7,
			LSN_F_DVI_ADPCM												= 0x11,
			LSN_F_YAMAHA_ADPCM											= 0x16,
			LSN_F_GSM_6_10												= 0x31,
			LSN_F_ITU_G_721_ADPCM										= 0x40,
			LSN_F_MPEG													= 0x50,
			LSN_F_EXTENSIBLE											= 0xFFFE,
		};

		/** Loading options. */
		enum LSN_LOAD_FLAGS : uint32_t {
			LSN_LF_DATA													= (1 << 0),
			LSN_LF_SMPL													= (1 << 1),
			LSN_LF_LIST													= (1 << 2),
			LSN_LF_ID3													= (1 << 3),
			LSN_LF_INST													= (1 << 4),
		};

		/** Chunks. */
		enum LSN_CHUNKS : uint32_t {
			LSN_C_RIFF													= 0x46464952,
			LSN_C_WAVE													= 0x45564157,
			LSN_C_FMT_													= 0x20746D66,
			LSN_C_DATA													= 0x61746164,
			LSN_C_SMPL													= 0x6C706D73,
			LSN_C_LIST													= 0x5453494C,
			LSN_C_ID3_													= 0x20336469,
			LSN_C_INST													= 0x74736E69,
			LSN_C_INFO													= 0x4F464E49,
			LSN_C_LABL													= 0x6C62616C,
			LSN_C_ADTL													= 0x6C746461,
			LSN_C_DISP													= 0x70736964,			// Or 0x64697370?
		};

		/** Metadata. */
		enum LSN_META {
			LSN_M_INAM													= 0x4D414E49,			// Track name.
			LSN_M_IPRD													= 0x44525049,			// Album title.
			LSN_M_IART													= 0x54524149,			// Artist name.
			LSN_M_ICMT													= 0x544D4349,			// Comments.
			LSN_M_ICRD													= 0x44524349,			// Year.
			LSN_M_IGNR													= 0x524E4749,			// Genre.
			LSN_M_ITRK													= 0x4B525449,			// Track number.
			LSN_M_IENG													= 0x474E4549,			// The engineer.
		};

		/** Start conditions. */
		enum LSN_START_CONDITIONS {
			LSN_SC_NONE,
			LSN_SC_START_AT_SAMPLE,
			LSN_SC_FIRST_NON_ZERO,
			LSN_SC_ZERO_FOR_DURATION,
		};

		/** End conditions. */
		enum LSN_END_CONDITIONS {
			LSN_EC_NONE,
			LSN_EC_END_AT_SAMPLE,
			LSN_EC_ZERO_FOR_DURATION,
			LSN_EC_DURATION,
		};

		/** Metadata formats. */
		enum LSN_META_FORMAT {
			LSN_MF_AUDACITY,
		};


		// == Types.
		typedef std::vector<double, CAlignmentAllocator<double, 64>>	lwtrack;
		typedef std::vector<lwtrack>									lwaudio;
		typedef bool (LSN_STDCALL *										PfAddSampleFunc)( float, struct CWavFile::LSN_STREAMING & );
		typedef void (LSN_STDCALL *										PfBatchConvNWrite)( const std::vector<float> &, std::vector<uint8_t> &, struct CWavFile::LSN_STREAMING & );
		typedef bool (LSN_STDCALL *										PfAddMetaDataFunc)( void *, struct CWavFile::LSN_STREAMING & );
		typedef void (LSN_STDCALL *										PfMetaDataThreadFunc)( CWavFile *, void *, struct CWavFile::LSN_STREAMING &, std::vector<uint8_t, CAlignmentAllocator<uint8_t, 64>> &, std::vector<uint8_t> & );
		

		/** The save data. */
		struct LSN_SAVE_DATA {
			LSN_FORMAT													fFormat = LSN_F_PCM;
			uint32_t													uiHz = 0;					// Only overrides if not 0.
			uint16_t													uiBitsPerSample = 0;		// Only overrides if not 0.
			bool														bDither = false;

			LSN_SAVE_DATA() :
				fFormat( LSN_F_PCM ),
				uiHz( 0 ),
				uiBitsPerSample( 0 ) {}
			LSN_SAVE_DATA( uint32_t _ui32Hz, uint16_t _ui16Bits ) :
				fFormat( LSN_F_PCM ),
				uiHz( _ui32Hz ),
				uiBitsPerSample( _ui16Bits ) {}
		};

		/** Loop points. */
		struct LSN_LOOP_POINT {
			uint32_t													uiCuePointID;
			uint32_t													uiType;
			uint32_t													uiStart;
			uint32_t													uiEnd;
			uint32_t													uiFraction;
			uint32_t													uiPlayCount;
		};

		/** Stream-to-file options. */
		struct LSN_STREAM_TO_FILE_OPTIONS {
			std::wstring												wsPath;
			std::wstring												wsMetaPath;
			LSN_FORMAT													fFormat = LSN_F_IEEE_FLOAT;
			uint32_t													ui32Bits = 24;
			uint32_t													ui32Hz = 44100;			// Not set by the user.  Only used to pass information to the set-options functions.
			bool														bEnabled = false;
			bool														bDither = false;

			LSN_START_CONDITIONS										scStartCondition = LSN_SC_NONE;
			uint64_t													ui64StartParm = 0;
			double														dStartParm = 0.0;
			LSN_END_CONDITIONS											seEndCondition = LSN_EC_NONE;
			uint64_t													ui64EndParm = 0;
			double														dEndParm = 0.0;

			uint64_t													ui64MetaParm = 0;
			void *														pvMetaParm = nullptr;
			PfAddMetaDataFunc											pfMetaFunc = nullptr;
			PfMetaDataThreadFunc										pfMetaThreadFunc = nullptr;
			int32_t														i32MetaFormat = 0;
			bool														bMetaEnabled = false;
		};

		/** Stream-to-file condition parameters. */
		union LSN_STREAM_COND_PARM {
			uint64_t													ui64Parm;
			double														dParm;
		};

		/** Condition data. */
		struct LSN_CONDITIONS_DATA {
			uint64_t													ui64Counter = 0;
			uint64_t													ui64Parm0 = 0;
			float														fParm1 = 0.0f;
		};
		
		typedef bool (LSN_STDCALL *										PfStartConditionFunc)( uint64_t, float, LSN_CONDITIONS_DATA & );
		typedef bool (LSN_STDCALL *										PfEndConditionFunc)( uint64_t, uint64_t, float, LSN_CONDITIONS_DATA & );
		
#pragma pack( push, 1 )
		/** Chunk. */
		struct LSN_CHUNK {
			union {
				char8_t													cName[4];
				uint32_t												uiName;
			}															u;
			uint32_t													uiSize;
			union {
				char8_t													cName[4];
				uint32_t												uiFormat;
			}															u2;
			std::vector<uint8_t>										vData;
		};

		/** Chunk header. */
		struct LSN_CHUNK_HEADER {
			union {
				char8_t													cName[4];
				uint32_t												uiId;
			}															u;
			uint32_t													uiSize;
		};

		/** FMT chunk. */
		struct LSN_FMT_CHUNK {
			LSN_CHUNK_HEADER											chHeader;
			//uint32_t													uiSubchunk1ID;			// "fmt ".
			//uint32_t													uiSubchunk1Size;		// Normally 16 (for PCM).
			uint16_t													uiAudioFormat;			// PCM = 1 (linear quantization).
			uint16_t													uiNumChannels;			// 1 = mono, 2 = stereo.
			uint32_t													uiSampleRate;			// 44,100, 48,000, etc.
			uint32_t													uiByteRate;				// SampleRate * NumChannels * BitsPerSample / 8.
			uint16_t													uiBlockAlign;			// NumChannels * BitsPerSample / 8.
			uint16_t													uiBitsPerSample;		// 8, 16, etc.
			uint16_t													uiExtraParamSize;		// Invalid on PCM.
			uint8_t														ui8ExtraParams[1];		// uiExtraParamSize bytes of extra data.
		};

		/** DATA chunk. */
		struct LSN_DATA_CHUNK {
			LSN_CHUNK_HEADER											chHeader;
			//uint32_t													uiSubchunk2ID;			// "data".
			//uint32_t													uiSubchunk2Size;		// NumSamples * NumChannels * BitsPerSample / 8.
			uint8_t														ui8Data[1];				// Sample data (length = uiSubchunk2Size).
		};

		/** SAMPL chunk. */
		struct LSN_SMPL_CHUNK {
			LSN_CHUNK_HEADER											chHeader;
			uint32_t													uiManufacturer;
			uint32_t													uiProduct;
			uint32_t													uiSamplePeriod;
			uint32_t													uiMIDIUnityNote;
			uint32_t													uiMIDIPitchFraction;
			uint32_t													uiSMPTEFormat;
			uint32_t													uiSMPTEOffset;
			uint32_t													uiNumSampleLoops;
			uint32_t													uiSamplerData;
			LSN_LOOP_POINT												lpLoops[1];
		};

		/** LIST chunk. */
		struct LSN_LIST_CHUNK {
			LSN_CHUNK_HEADER											chHeader;
			union {
				char8_t													cName[4];
				uint32_t												uiTypeId;
			}															u;
			uint8_t														ui8Data[1];
		};

		/** ID3  chunk. */
		struct LSN_ID3_CHUNK {
			LSN_CHUNK_HEADER											chHeader;
			char8_t														sName[3];
			uint16_t													ui16Version;
			uint8_t														ui8Flags;
			uint32_t													ui32Size;
			uint8_t														ui8Data[1];
		};

		/** INST chunk. */
		struct LSN_INST_CHUNK {
			LSN_CHUNK_HEADER											chHeader;
			uint8_t														ui8UnshiftedNote;
			uint8_t														ui8FineTune;
			uint8_t														ui8Gain;
			uint8_t														ui8LowNote;
			uint8_t														ui8HiNote;
			uint8_t														ui8LowVel;
			uint8_t														ui8HiVel;
		};
#pragma pack( pop )

		/** A stream-to-file structure. */
		struct LSN_STREAMING {
			uint64_t													ui64SamplesReceived = 0;	/**< The number of samples sent to the stream. */
			uint64_t													ui64SamplesWritten = 0;		/**< The number of samples written to the file. */
			uint64_t													ui64MetaWritten = 0;		/**< The number of metadata items written. */
			uint64_t													ui64WavFileOffset_Size = 0;	/**< The offset of the size value in the WAV file. */
			uint64_t													ui64WavFileOffset_DSize = 0;/**< The offset of the data-size value in the WAV file. */
			uint64_t													ui64MetaParm = 0;			/**< The metadata uint64_t parameter. */
			uint64_t													ui64MetaThreadParm = 0;		/**< The metadata thread data used by the thread function to keep track of how many items it has processed, or for any other reason. */
			double														dDitherError = 0.0;			/**< The dither error. */
			size_t														stBufferSize = 1024 * 10;	/**< The size of the buffer to fill before flushing. */
			std::vector<float>											vCurBuffer;					/**< The current buffer awaiting samples. */
			std::vector<uint8_t, CAlignmentAllocator<uint8_t, 64>>		vMetaBuffer;				/**< The buffer for gathering metadata. */
			std::wstring												wsPath;						/**< The path to the file to which we are streaming. */
			CStdFile													sfFile;						/**< The file to which to write the WAV data. */

			std::wstring												wsMetaPath;					/**< The path to the metadata file to which we are streaming. */
			CStdFile													sfMetaFile;					/**< The metadata file to which to write the Wmetadata. */
			void *														pvMetaParm = nullptr;		/**< The metadata pointer parameter. */
			PfAddMetaDataFunc											pfMetaFunc = nullptr;		/**< The function for adding metadata. */
			PfMetaDataThreadFunc										pfMetaThreadFunc = nullptr;	/**< The function for parsing metadata into the final file output. */

			std::queue<std::vector<float>>								qBufferQueue;				/**< The queue of buffers handled by the thread. */
			std::mutex													mMutex;						/**< The thread mutex for accessing qBufferQueue and bStreaming. */
			std::condition_variable										cvCondition;				/**< The conditional variable for the lock. */
			std::thread													tThread;					/**< The thread for writing to the file. */

			std::queue<std::vector<uint8_t, CAlignmentAllocator<uint8_t, 64>>>
																		qMetaBufferQueue;			/**< The queue of buffers handled by the metadata thread. */
			std::mutex													mMetaMutex;					/**< The thread mutex for metadata streaming. */
			std::condition_variable										cvMetaCondition;			/**< The conditional variable for the metadata lock. */
			std::thread													tMetaThread;				/**< The thread for writing to the metadata file. */
			std::vector<uint8_t, CAlignmentAllocator<uint8_t, 64>>		vMetaThreadScratch;			/**< A scratch buffer for any data the thread needs to manage. */
			
			LSN_CONDITIONS_DATA											cdStartData;				/**< The start-condition data. */
			LSN_CONDITIONS_DATA											cdStopData;					/**< The stop-condition data. */
			PfStartConditionFunc										pfStartCondFunc = nullptr;	/**< The start-condition function. */
			PfEndConditionFunc											pfEndCondFunc = nullptr;	/**< The end-condition function. */
			PfBatchConvNWrite											pfCvtAndWriteFunc = nullptr;/**< The function for batch conversion and writing to the WAV file. */
			PfAddSampleFunc												pfAddSampleFunc = nullptr;	/**< The function for adding a sample.  There is one that checks the starting condition and then one that keeps adding samples until the stopping condition is reached. */
			uint32_t													ui32WavFile_Size = 0;		/**< The final file size to write to the WAV file. */
			uint32_t													ui32WavFile_DSize = 0;		/**< The final data size to write to the WAV file. */
			int32_t														i32MetaFormat = 0;			/**< The metadata format. */
			uint32_t													ui32Hz = 44100;				/**< The file Hz. */
			LSN_FORMAT													fFormat = LSN_F_PCM;		/**< The WAV-file format. */
			uint16_t													ui16Bits = 16;				/**< The number of bits-per-sample. */			
			uint16_t													ui16Channels = 2;			/**< Total channels to output. */
			bool														bEnd = true;				/**< Tells the thread to stop. */
			bool														bStreaming = false;			/**< If true, the file must be closed either manually or in the destructor. */
			bool														bAdding = false;			/**< Set to true after the starting condition is met.  Indicates that samples are being added. */
			bool														bDither = false;			/**< To dither 16-bit PCM or not. */
			bool														bMeta = false;				/**< Whether metadata is being streamed or not. */
		};


		// == Functions.
		/**
		 * Loads a WAV file.
		 *
		 * \param _pcPath The UTF-8 path to open.
		 * \param _ui32LoadFlags The loading flags, which allow to not load large portions of the file.
		 * \param _ui32StartSample The first sample to load.
		 * \param _ui32EndSample The last sample (exclusive) to load.
		 * \return Returns true if the file was opened.
		 */
		bool															Open( const char8_t * _pcPath, uint32_t _ui32LoadFlags = 0xFFFFFFFF, uint32_t _ui32StartSample = 0, uint32_t _ui32EndSample = 0xFFFFFFFF );

		/**
		 * Loads a WAV file.
		 *
		 * \param _pwcPath The UTF-16 path to open.
		 * \param _ui32LoadFlags The loading flags, which allow to not load large portions of the file.
		 * \param _ui32StartSample The first sample to load.
		 * \param _ui32EndSample The last sample (exclusive) to load.
		 * \return Returns true if the file was opened.
		 */
		bool															Open( const char16_t * _pwcPath, uint32_t _ui32LoadFlags = 0xFFFFFFFF, uint32_t _ui32StartSample = 0, uint32_t _ui32EndSample = 0xFFFFFFFF );

		/**
		 * Loads a WAV file from a stream.  Will usually be a file stream but can be any stream.
		 * 
		 * \param _sbStream The stream from which to load the file.
		 * \param _ui32LoadFlags The loading flags, which allow to not load large portions of the file.
		 * \param _ui32StartSample The first sample to load.
		 * \param _ui32EndSample The last sample (exclusive) to load.
		 * \return Returns true if the file is a valid WAV file and there are no memory problems loading it.
		 **/
		bool															LoadFromStream( CStreamBase &_sbStream, uint32_t _ui32LoadFlags = 0xFFFFFFFF, uint32_t _ui32StartSample = 0, uint32_t _ui32EndSample = 0xFFFFFFFF );

		/**
		 * Loads a WAV file from memory.  This is just an in-memory version of the file.
		 *
		 * \param _vData The in-memory file to load.
		 * \param _ui32LoadFlags The loading flags, which allow to not load large portions of the file.
		 * \param _ui32StartSample The first sample to load.
		 * \param _ui32EndSample The last sample (exclusive) to load.
		 * \return Returns true if the file is a valid WAV file.
		 */
		bool															LoadFromMemory( const std::vector<uint8_t> &_vData, uint32_t _ui32LoadFlags = 0xFFFFFFFF, uint32_t _ui32StartSample = 0, uint32_t _ui32EndSample = 0xFFFFFFFF );

		/**
		 * Saves as a PCM WAV file.
		 *
		 * \param _pcPath The path to where the file will be saved.
		 * \param _vSamples The samples to convert and write to the file.
		 * \param _psdSaveSettings Settings to override this class's settings.
		 * \return Returns true if the file was created and saved.
		 */
		template <typename _tType = lwaudio>
		bool															SaveAsPcm( const char8_t * _pcPath, const _tType &_vSamples,
			const LSN_SAVE_DATA * _psdSaveSettings = nullptr ) const {
			if ( !_vSamples.size() ) { return false; }

			auto sPath = FixFileName( _pcPath );

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
			if ( ui32DataSize & 1 ) {
				++ui32DataSize;
			}

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
							if ( !BatchF64ToPcm8( _vSamples, vRet ) ) { return false; }
							break;
						}
						case 16 : {
							if ( _psdSaveSettings && _psdSaveSettings->bDither ) {
							}
							else {
								if ( !BatchF64ToPcm16( _vSamples, vRet ) ) { return false; }
							}
							break;
						}
						case 24 : {
							if ( !BatchF64ToPcm24( _vSamples, vRet ) ) { return false; }
							break;
						}
						case 32 : {
							if ( !BatchF64ToPcm32( _vSamples, vRet ) ) { return false; }
							break;
						}
						default : { return false; }
					}
					break;
				}
				case LSN_F_IEEE_FLOAT : {
					if ( !BatchF64ToF32( _vSamples, vRet ) ) { return false; }
					break;
				}
				default : { return false; }
			}
			if ( vRet.size() & 1 ) {
				vRet.push_back( 0 );
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
		 * Saves as a PCM WAV file.
		 *
		 * \param _pcPath The path to where the file will be saved.
		 * \param _vSamples The samples to convert and write to the file.
		 * \param _psdSaveSettings Settings to override this class's settings.
		 * \return Returns true if the file was created and saved.
		 */
		template <typename _tType = lwaudio>
		bool															SaveAsPcm( const char16_t * _pcPath, const lwaudio &_vSamples,
			const LSN_SAVE_DATA * _psdSaveSettings = nullptr ) const {
			return SaveAsPcm<_tType>( CUtilities::Utf16ToUtf8( _pcPath ).c_str(), _vSamples, _psdSaveSettings );
		}

		/**
		 * Saves as a PCM WAV file.
		 *
		 * \param _pcPath The path to where the file will be saved.
		 * \param _vSamples The samples to convert and write to the file.
		 * \param _psdSaveSettings Settings to override this class's settings.
		 * \return Returns true if the file was created and saved.
		 */
		template <typename _tType = lwaudio>
		bool															SaveAsPcm( const char8_t * _pcPath, const lwtrack &_vSamples,
			const LSN_SAVE_DATA * _psdSaveSettings = nullptr ) const {
			lwaudio aAudio;
			aAudio.push_back( _vSamples );
			return SaveAsPcm<_tType>( _pcPath, aAudio,
				_psdSaveSettings );
		}

		/**
		 * Saves as a PCM WAV file.
		 *
		 * \param _pcPath The path to where the file will be saved.
		 * \param _vSamples The samples to convert and write to the file.
		 * \param _psdSaveSettings Settings to override this class's settings.
		 * \return Returns true if the file was created and saved.
		 */
		template <typename _tType = lwaudio>
		bool															SaveAsPcm( const char16_t * _pcPath, const lwtrack &_vSamples,
			const LSN_SAVE_DATA * _psdSaveSettings = nullptr ) const {
			return SaveAsPcm<_tType>( CUtilities::Utf16ToUtf8( _pcPath ).c_str(), _vSamples, _psdSaveSettings );
		}

		/**
		 * Opens a stream to a WAV file.  Samples can be written over time.
		 * 
		 * \param _stfoFileOptions Contains all of the settings for streaming to a file.
		 * \param _ui32Hz The WAV Hz.
		 * \param _stBufferSize The buffer size (in samples).
		 * \return Returns true if the file could initially be created and all parameters are correct.
		 **/
		bool															StreamToFile( const LSN_STREAM_TO_FILE_OPTIONS &_stfoFileOptions, uint32_t _ui32Hz,
			size_t _stBufferSize = 10 * 1024 );

		/**
		 * Stops the streaming file.
		 **/
		void															StopStream();

		/**
		 * Adds a sample to the stream.
		 * 
		 * \param _fSample The sample to add.
		 **/
		void															AddStreamSample( float _fSample );

		/**
		 * Called to update the metadata stream output.
		 **/
		void															AddMetaData();

		/**
		 * Gets a constant reference to the current streaming data.
		 * 
		 * \return Returns a constant reference to the current streaming data.
		 **/
		const LSN_STREAMING &											GetStreamData() const { return m_sStream; }

		/**
		 * Resets the object back to scratch.
		 */
		void															Reset();

		/**
		 * Gets the number of samples in the loaded file.
		 *
		 * \return Returns the number of samples in the loaded file.
		 */
		inline uint32_t													TotalSamples() const { return static_cast<uint32_t>(m_vSamples.size() / (m_uiNumChannels * m_uiBytesPerSample)); }

		/**
		 * Gets the number of samples originally in the WAV file.
		 * 
		 * \return Returns the number of samples that were originally in the WAV file.
		 **/
		inline uint32_t													FileSampleCnt() const { return m_ui32OriginalSampleCount; }

#pragma optimize( "gt", on )
		/**
		 * Fills a vector with the whole range of samples for a given channel.
		 *
		 * \param _uiChan The channel whose sample data is to be obtained.
		 * \param _vResult The vector containing the samples.
		 * \return Returns true if the vector was able to fit all samples.
		 */
		template <typename _tType = lwtrack>
		bool															GetAllSamples( uint16_t _uiChan, _tType &_vResult ) const {
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
		template <typename _tType = lwtrack>
		bool															GetSamples( uint16_t _uiChan, _tType &_vResult, size_t _stFrom, size_t _stTo ) const {
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
		template <typename _tType = lwaudio>
		bool															GetAllSamples( _tType &_vResult ) const {
			try {
				if constexpr ( std::is_same<_tType, std::vector<double>>::value ) {
					if ( m_uiNumChannels > _vResult.size() ) {
						_vResult.resize( m_uiNumChannels );
					}
				}
				if ( _vResult.size() != m_uiNumChannels ) { return false; }
				for ( auto I = m_uiNumChannels; I--; ) {
					if ( !GetAllSamples( I, _vResult[I] ) ) { return false; }
				}
				return true;
			}
			catch ( ... ) { return false; }
		}

		/**
		 * Fills an array of vectors.  There is an array of vectors for each channel, and each vector contains all
		 *	of the selected samples for that channel.
		 *
		 * \param _vResult The array of vectors to be filled with all samples in this file.
		 * \param _stFrom The starting sample to get.
		 * \param _stTo The ending sample to get, exclusive.
		 * \return Returns true if the vector(s) was/were able to fit all samples.
		 */
		template <typename _tType = lwaudio>
		bool															GetSamples( _tType &_vResult, size_t _stFrom, size_t _stTo ) const {
			try {
				if ( m_uiNumChannels > _vResult.size() ) {
					_vResult.resize( m_uiNumChannels );
				}
				if ( _vResult.size() != m_uiNumChannels ) { return false; }
				for ( auto I = m_uiNumChannels; I--; ) {
					if ( !GetSamples( I, _vResult[I], _stFrom, _stTo ) ) { return false; }
				}
				return true;
			}
			catch ( ... ) { return false; }
		}

		/**
		 * Releases all samples from memory.  Used to conserve RAM.
		 **/
		void															FreeSamples() {
			m_vSamples.clear();
		}

		/**
		 * Gets the Hz.
		 *
		 * \return Returns the Hz.
		 */
		inline uint32_t													Hz() const { return m_uiSampleRate; }

		/**
		 * Gets the number of channels.
		 *
		 * \return Returns the number of channels.
		 */
		inline uint16_t													Channels() const { return m_uiNumChannels; }

		/**
		 * Gets the number of bits per sample.
		 *
		 * \return Returns the number of bits per sample.
		 */
		inline uint16_t													BitsPerSample() const { return m_uiBitsPerSample; }

		/**
		 * Gets the format of the WAV file.
		 * 
		 * \return Returns the WAV-file format.
		 **/
		inline LSN_FORMAT												Format() const { return m_fFormat; }

		/**
		 * Calculates the size of a buffer needed to hold the given number of samples on the given number of channels in te given format.
		 *
		 * \param _fFormat The format of the samples.
		 * \param _uiSamples The number of samples per channel.
		 * \param _uiChannels The number of channels.
		 * \param _uiBitsPerPixel The number of bits per sample.
		 * \return Returns the size of a buffer needed to hold the given number of samples on the given number of channels in te given format.
		 */
		static inline uint32_t											CalcSize( LSN_FORMAT _fFormat, uint32_t _uiSamples, uint16_t _uiChannels, uint16_t _uiBitsPerPixel ) {
			switch ( _fFormat ) {
				case LSN_F_PCM : {
					return uint32_t( uint64_t( _uiSamples ) * _uiChannels * _uiBitsPerPixel / 8 );
				}
				case LSN_F_IEEE_FLOAT : {
					return uint32_t( uint64_t( _uiSamples ) * _uiChannels * sizeof( float ) );
				}
			}
			return 0;
		}

		/**
		 * Adds a LIST entry.
		 *
		 * \param _uiId The ID of the entry.
		 * \param _sVal The value of the entry.
		 * \return Returns true if the entry was added.
		 */
		bool															AddListEntry( uint32_t _uiId, const std::u8string &_sVal );

		/**
		 * Adds a DISP image.
		 * 
		 * \param _ui32Type The image type.  One of the CF_* values.
		 * \param _vImage The image data.
		 * \return Returns true if the entry was added.
		 **/
		bool															AddImage( uint32_t _ui32Type, const std::vector<uint8_t> &_vImage );

		/**
		 * Gets the loop array.
		 *
		 * \return Returns the loop array.
		 */
		const std::vector<LSN_LOOP_POINT> &								Loops() const { return m_vLoops; }


	protected :
		// == Types.
		/** A chunk entry. */
		struct LSN_CHUNK_ENTRY {
			union {
				char8_t													cName[4];
				uint32_t												uiName;
			}															u;
			uint32_t													uiOffset;
			uint32_t													uiSize;
		};

		/** A LIST entry. */
		struct LSN_LIST_ENTRY {
			union {
				char8_t													cName[4];
				uint32_t												uiIfoId;
			}															u;
			std::u8string												sText;
		};

		/** An ID3 entry. */
		struct LSN_ID3_ENTRY {
			union {
				char8_t													cName[4];
				uint32_t												uiIfoId;
			}															u;
			uint16_t													ui16Flags;
			std::u8string												sValue;
		};

		/** A DISP entry. */
		struct LSN_DISP_ENTRY {
			union {
				char8_t													cName[4];
				uint32_t												uiIfoId;
			}															u;
			uint32_t													ui32Size;
			uint32_t													ui32Type;
			std::vector<uint8_t>										vValue;
		};

		/** An INST entry. */
		struct LSN_INST_ENTRY {
			uint8_t														ui8UnshiftedNote = 0;
			uint8_t														ui8FineTune = 0;
			uint8_t														ui8Gain = 0;
			uint8_t														ui8LowNote = 0;
			uint8_t														ui8HiNote = 0;
			uint8_t														ui8LowVel = 0;
			uint8_t														ui8HiVel = 0;
		};

		
		// == Members.
		/** The number of samples in the original file.  How many samples are actually loaded can vary. */
		uint32_t														m_ui32OriginalSampleCount;
		/** The format. */
		LSN_FORMAT														m_fFormat;
		/** The number of channels. */
		uint16_t														m_uiNumChannels;			// 1 = mono, 2 = stereo, etc.
		/** The sample rate. */
		uint32_t														m_uiSampleRate;				// 44,100, 48,000, etc.
		/** The bits per sample. */
		uint16_t														m_uiBitsPerSample;			// 8, 16, etc.
		/** The bytes per sample. */
		uint16_t														m_uiBytesPerSample;			// m_uiBitsPerSample / 8.
		/** Base note. */
		uint32_t														m_uiBaseNote;
		/** The raw sample data. */
		std::vector<uint8_t>											m_vSamples;
		/** Loop points. */
		std::vector<LSN_LOOP_POINT>										m_vLoops;
		/** "LIST" metadata. */
		std::vector<LSN_LIST_ENTRY>										m_vListEntries;
		/** "id3 " metadata. */
		std::vector<LSN_ID3_ENTRY>										m_vId3Entries;
		/** "DISP" metadata. */
		std::vector<LSN_DISP_ENTRY>										m_vDisp;
		/** Instrument metadata. */
		LSN_INST_ENTRY													m_ieInstEntry;
		/** Streaming data. */
		LSN_STREAMING													m_sStream;


		// == Functions.
		/**
		 * Loads data from the "fmt ".
		 *
		 * \param _pfcChunk The chunk of data to load
		 * \return Returns true if everything loaded fine.
		 */
		bool															LoadFmt( const LSN_FMT_CHUNK * _pfcChunk );

		/**
		 * Loads the "data" chunk.
		 *
		 * \param _pdcChunk The chunk of data to load
		 * \return Returns true if everything loaded fine.
		 */
		bool															LoadData( const LSN_DATA_CHUNK * _pdcChunk, uint32_t _ui32StartSample = 0, uint32_t _ui32EndSample = 0xFFFFFFFF );

		/**
		 * Loads the "smpl" chunk.
		 *
		 * \param _pscChunk The chunk of data to load
		 * \return Returns true if everything loaded fine.
		 */
		bool															LoadSmpl( const LSN_SMPL_CHUNK * _pscChunk );

		/**
		 * Loads a "LIST" chunk.
		 *
		 * \param _plcChunk The chunk of data to load
		 * \return Returns true if everything loaded fine.
		 */
		bool															LoadList( const LSN_LIST_CHUNK * _plcChunk );

		/**
		 * Loads an "id3 " chunk.
		 *
		 * \param _plcChunk The chunk of data to load
		 * \return Returns true if everything loaded fine.
		 */
		bool															LoadId3( const LSN_ID3_CHUNK * _picChunk );

		/**
		 * Loads an "inst" chunk.
		 *
		 * \param _plcChunk The chunk of data to load
		 * \return Returns true if everything loaded fine.
		 */
		bool															LoadInst( const LSN_INST_CHUNK * _picChunk );

		/**
		 * Converts a bunch of 8-bit PCM samples to double.
		 *
		 * \param _ui32From Starting sample index.
		 * \param _ui32To Ending sample index.
		 * \param _uiChan The channel whose sample data is to be obtained.
		 * \param _vResult The vector containing the samples.
		 * \tparam _tType The vector tpe to which to write the result.
		 * \return Returns true if the vector was able to hold all of the values.
		 */
		template <typename _tType = lwtrack>
		bool															Pcm8ToF64( uint32_t _ui32From, uint32_t _ui32To, uint16_t _uiChan, _tType &_vResult ) const {
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
		 * \tparam _tType The vector tpe to which to write the result.
		 * \return Returns true if the vector was able to hold all of the values.
		 */
		template <typename _tType = lwtrack>
		bool															Pcm16ToF64( uint32_t _ui32From, uint32_t _ui32To, uint16_t _uiChan, _tType &_vResult ) const {
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
		 * \tparam _tType The vector tpe to which to write the result.
		 * \return Returns true if the vector was able to hold all of the values.
		 */
		template <typename _tType = lwtrack>
		bool															Pcm24ToF64( uint32_t _ui32From, uint32_t _ui32To, uint16_t _uiChan, _tType &_vResult ) const {
			const double dFactor = (std::pow( 2.0, 24.0 - 1.0 ) - 1.0) * 256.0;
			size_t sFinalSize = _vResult.size() + (_ui32To - _ui32From);
			_vResult.reserve( sFinalSize );
			uint32_t uiStride;
			size_t sIdx = CalcOffsetsForSample( _uiChan, _ui32From, uiStride );
			while ( _ui32From < _ui32To ) {
				if LSN_UNLIKELY( sIdx >= m_vSamples.size() ) {
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
		 * \tparam _tType The vector tpe to which to write the result.
		 * \return Returns true if the vector was able to hold all of the values.
		 */
		template <typename _tType = lwtrack>
		bool															Pcm32ToF64( uint32_t _ui32From, uint32_t _ui32To, uint16_t _uiChan, _tType &_vResult ) const {
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
		 * \tparam _tType The vector tpe to which to write the result.
		 * \return Returns true if the vector was able to hold all of the values.
		 */
		template <typename _tType = lwtrack>
		bool															F32ToF64( uint32_t _ui32From, uint32_t _ui32To, uint16_t _uiChan, _tType &_vResult ) const {
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
		template <typename _tType = lwtrack>
		static bool														BatchF64ToPcm8( const _tType &_vSrc, std::vector<uint8_t> &_vDst ) {
			for ( size_t I = 0; I < _vSrc[0].size(); ++I ) {
				for ( size_t J = 0; J < _vSrc.size(); ++J ) {
					int8_t iSample = static_cast<int8_t>(std::round( std::clamp<double>( _vSrc[J][I], -1.0, 1.0 ) * 127.0 + 128.0 ));
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
		template <typename _tType = lwtrack>
		static bool														BatchF64ToPcm16( const _tType &_vSrc, std::vector<uint8_t> &_vDst ) {
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
		template <typename _tType = lwtrack>
		static bool														BatchF64ToPcm16_Dither( const _tType &_vSrc, std::vector<uint8_t> &_vDst ) {
			try {
				const double dFactor = std::pow( 2.0, 16.0 - 1.0 ) - 1.0;
				auto stNumSamples = _vSrc[0].size();
				auto stNumChannels = _vSrc.size();
				auto aSize = _vDst.size();
				_vDst.reserve( _vDst.size() + stNumSamples * stNumChannels * sizeof( int16_t ) );
				int16_t * pi16Dst = reinterpret_cast<int16_t *>(_vDst.data() + aSize);
				double dError = 0.0;

				for ( size_t I = 0; I < _vSrc[0].size(); ++I ) {
					for ( size_t J = 0; J < _vSrc.size(); ++J ) {
						(*pi16Dst++) = CUtilities::SampleToI16_Dither( _vSrc[J][I], dError );
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
		template <typename _tType = lwtrack>
		static bool														BatchF64ToPcm24( const _tType &_vSrc, std::vector<uint8_t> &_vDst ) {
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
		template <typename _tType = lwtrack>
		static bool														BatchF64ToPcm32( const _tType &_vSrc, std::vector<uint8_t> &_vDst ) {
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
		 * Converts a batch of F64 samples to F32 samples.
		 *
		 * \param _vSrc The samples to convert.
		 * \param _vDst The buffer to which to convert the samples.
		 * \return Returns trye if all samples were added to the buffer.
		 */
		template <typename _tType = lwtrack>
		static bool														BatchF64ToF32( const _tType &_vSrc, std::vector<uint8_t> &_vDst ) {
			try {
				auto stNumSamples = _vSrc[0].size();
				auto stNumChannels = _vSrc.size();
				auto aSize = _vDst.size();
				_vDst.resize( aSize + stNumSamples * stNumChannels * sizeof( float ) );
				float * pfDst = reinterpret_cast<float *>(_vDst.data() + aSize);

				for ( size_t I = 0; I < _vSrc[0].size(); ++I ) {
					for ( size_t J = 0; J < _vSrc.size(); ++J ) {
						(*pfDst++) = static_cast<float>(_vSrc[J][I]);
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
		size_t															CalcOffsetsForSample( uint16_t _uiChan, uint32_t _uiIdx, uint32_t &_uiStride ) const;

		/**
		 * Converts a 28-bit size value from ID3 into regular 32-bit.
		 *
		 * \param _uiSize The size value to decode.
		 * \return Returns the decoded size value.
		 */
		static uint32_t													DecodeSize( uint32_t _uiSize );

		/**
		 * Creates an "fmt " chunk based off either this object's parameters or optional given overrides.
		 *
		 * \param _fFormat The format to set.
		 * \param _uiChannels The total channels.
		 * \param _psdSaveSettings Optional overrides.
		 * \return Returns a created object.
		 */
		LSN_FMT_CHUNK													CreateFmt( LSN_FORMAT _fFormat, uint16_t _uiChannels, const LSN_SAVE_DATA * _psdSaveSettings ) const;

		/**
		 * Writes file-image loop-point bytes to a vector.
		 *
		 * \return Returns the bytes that represent the "smpl" chunk in a file.
		 */
		std::vector<uint8_t>											CreateSmpl() const;

		/**
		 * Writes file-image "LIST" chunk to a vector.
		 *
		 * \return Returns the bytes that represent the "LIST" chunk in a file.
		 */
		std::vector<uint8_t>											CreateList() const;

		/**
		 * Fixes the output file name of a given full path.
		 * 
		 * \param _pcFullpath The full path whose file name is to fix.
		 * \return Returns the fixed file path.
		 **/
		static std::u8string											FixFileName( const char8_t * _pcFullpath );

		/**
		 * Creates the file for streaming and writes the header data to it, preparing it for writing samples.
		 * 
		 * \param _pcPath Uses data loaded into m_sStream to create a new file.
		 * \return Returns true if the file was created and the header was written to it.
		 **/
		bool															CreateStreamFile( const char8_t * _pcPath );

		/**
		 * Closes the current streaming file.
		 **/
		void															CloseStreamFile();

		/**
		 * Creates the file for streaming metadata.  All writes to the file are handled by a callback.
		 * 
		 * \param _pcPath Uses data loaded into m_sStream to create a new file.
		 * \return Returns true if the file was created.
		 **/
		bool															CreateStreamMetaFile( const char8_t * _pcPath );

		/**
		 * Closes the current streaming metadata file.
		 **/
		void															CloseStreamMetaFile();

		/**
		 * The stream-to-file writer thread.
		 **/
		void															StreamWriterThread();

		/**
		 * The metadata-to-file writer thread.
		 **/
		void 															MetadataWriterThread();

		/**
		 * The None start condition.  Returns true.
		 * 
		 * \param _ui64AbsoluteIdx The absolute index of the sample being tested.
		 * \param _fSample The value of the sample being tested.
		 * \param _cdData Function-specific data.
		 * \return Returns true if the start condition has been met.
		 **/
		static bool LSN_STDCALL											StartCondFunc_None( uint64_t /*_ui64AbsoluteIdx*/, float /*_fSample*/, LSN_CONDITIONS_DATA &/*_cdData*/ );

		/**
		 * The Start-at-Sample start condition.  Returns true if _ui64AbsoluteIdx >= _cdData.ui64Parm0.
		 * 
		 * \param _ui64AbsoluteIdx The absolute index of the sample being tested.
		 * \param _fSample The value of the sample being tested.
		 * \param _cdData Function-specific data.
		 * \return Returns true if the start condition has been met.
		 **/
		static bool LSN_STDCALL											StartCondFunc_StartAtSample( uint64_t _ui64AbsoluteIdx, float /*_fSample*/, LSN_CONDITIONS_DATA &_cdData );

		/**
		 * The First-Non-Zero start condition.  Returns true if _fSample != 0.0f.
		 * 
		 * \param _ui64AbsoluteIdx The absolute index of the sample being tested.
		 * \param _fSample The value of the sample being tested.
		 * \param _cdData Function-specific data.
		 * \return Returns true if the start condition has been met.
		 **/
		static bool LSN_STDCALL											StartCondFunc_FirstNonZero( uint64_t /*_ui64AbsoluteIdx*/, float _fSample, LSN_CONDITIONS_DATA &/*_cdData*/ );

		/**
		 * The Zero-For-Duration start condition.  Returns true if _cdData.ui64Counter >= _cdData.ui64Parm0, with _cdData.ui64Counter being counted on each 0 sample.
		 * 
		 * \param _ui64AbsoluteIdx The absolute index of the sample being tested.
		 * \param _fSample The value of the sample being tested.
		 * \param _cdData Function-specific data.
		 * \return Returns true if the start condition has been met.
		 **/
		static bool LSN_STDCALL											StartCondFunc_ZeroForDuration( uint64_t /*_ui64AbsoluteIdx*/, float _fSample, LSN_CONDITIONS_DATA &_cdData );

		/**
		 * The None end condition.  Returns true.
		 * 
		 * \param _ui64AbsoluteIdx The absolute index of the sample being tested.
		 * \param _ui64IdxSinceStart The index of the sample being tested since recording began.
		 * \param _fSample The value of the sample being tested.
		 * \param _cdData Function-specific data.
		 * \return Returns false if the end condition has been met, at which point recording should stop.
		 **/
		static bool LSN_STDCALL											EndCondFunc_None( uint64_t /*_ui64AbsoluteIdx*/, uint64_t /*_ui64IdxSinceStart*/, float /*_fSample*/, LSN_CONDITIONS_DATA &/*_cdData*/ );

		/**
		 * The End-At-Sample end condition.  Returns true if _ui64AbsoluteIdx < _cdData.ui64Parm0.
		 * 
		 * \param _ui64AbsoluteIdx The absolute index of the sample being tested.
		 * \param _ui64IdxSinceStart The index of the sample being tested since recording began.
		 * \param _fSample The value of the sample being tested.
		 * \param _cdData Function-specific data.
		 * \return Returns false if the end condition has been met, at which point recording should stop.
		 **/
		static bool LSN_STDCALL											EndCondFunc_EndAtSample( uint64_t _ui64AbsoluteIdx, uint64_t /*_ui64IdxSinceStart*/, float /*_fSample*/, LSN_CONDITIONS_DATA &_cdData );

		/**
		 * The Zero-For-Duration end condition.  Returns true if _cdData.ui64Counter < _cdData.ui64Parm0, with _cdData.ui64Counter counting consecutive 0's.
		 * 
		 * \param _ui64AbsoluteIdx The absolute index of the sample being tested.
		 * \param _ui64IdxSinceStart The index of the sample being tested since recording began.
		 * \param _fSample The value of the sample being tested.
		 * \param _cdData Function-specific data.
		 * \return Returns false if the end condition has been met, at which point recording should stop.
		 **/
		static bool LSN_STDCALL											EndCondFunc_ZeroForDuration( uint64_t /*_ui64AbsoluteIdx*/, uint64_t /*_ui64IdxSinceStart*/, float _fSample, LSN_CONDITIONS_DATA &_cdData );

		/**
		 * The Duration end condition.  Returns true if _ui64IdxSinceStart < _cdData.ui64Parm0.
		 * 
		 * \param _ui64AbsoluteIdx The absolute index of the sample being tested.
		 * \param _ui64IdxSinceStart The index of the sample being tested since recording began.
		 * \param _fSample The value of the sample being tested.
		 * \param _cdData Function-specific data.
		 * \return Returns false if the end condition has been met, at which point recording should stop.
		 **/
		static bool LSN_STDCALL											EndCondFunc_Duration( uint64_t /*_ui64AbsoluteIdx*/, uint64_t _ui64IdxSinceStart, float /*_fSample*/, LSN_CONDITIONS_DATA &_cdData );

		/**
		 * Checks the starting condition before adding a sample.  Once the starting condition has been reached, _sStream.pfAddSampleFunc is changed to AddSample_CheckStopCond().
		 * 
		 * \param _fSample The sampple to add.
		 * \param _sStream The stream data.
		 **/
		static bool LSN_STDCALL											AddSample_CheckStartCond( float _fSample, LSN_STREAMING &_sStream );

		/**
		 * Checks the stopping condition before adding a sample.  Once the stopping condition has been reached, the stream ends. 
		 * 
		 * \param _fSample The sampple to add.
		 * \param _sStream The stream data.
		 * \return Returns true if the starting condition has been reached.
		 **/
		static bool LSN_STDCALL											AddSample_CheckStopCond( float _fSample, LSN_STREAMING &_sStream );

		/**
		 * Performs no conversion.  Input is directly sent to the file stream.
		 * 
		 * \param _vSrc The input samples.
		 * \param _vOut The output samples.
		 * \param _sStream The stream data.
		 * \return Returns true for as long as samples should be output.  Once false is returned, the ending condition has been reached and the stream should close without adding the sample.
		 **/
		static inline void LSN_STDCALL									BatchF32ToF32( const std::vector<float> &_vSrc, std::vector<uint8_t> &_vOut, LSN_STREAMING &_sStream );

		/**
		 * Converts a batch of floats to 8-bit PCM using raw C++.
		 * 
		 * \param _vSrc The input samples.
		 * \param _vOut The output samples.
		 * \param _sStream The stream data.
		 **/
		static inline void LSN_STDCALL									BatchF32ToPcm8( const std::vector<float> &_vSrc, std::vector<uint8_t> &_vOut, LSN_STREAMING &_sStream );

		/**
		 * Converts a batch of floats to 16-bit PCM using raw C++.
		 * 
		 * \param _vSrc The input samples.
		 * \param _vOut The output samples.
		 * \param _sStream The stream data.
		 **/
		static inline void LSN_STDCALL									BatchF32ToPcm16( const std::vector<float> &_vSrc, std::vector<uint8_t> &_vOut, LSN_STREAMING &_sStream );

		/**
		 * Converts a batch of floats to 16-bit PCM using raw C++.  Applies dither (state stored in _sStream).
		 * 
		 * \param _vSrc The input samples.
		 * \param _vOut The output samples.
		 * \param _sStream The stream data.
		 **/
		static inline void LSN_STDCALL									BatchF32ToPcm16_Dither( const std::vector<float> &_vSrc, std::vector<uint8_t> &_vOut, LSN_STREAMING &_sStream );

		/**
		 * Converts a batch of floats to 24-bit PCM using raw C++.
		 * 
		 * \param _vSrc The input samples.
		 * \param _vOut The output samples.
		 * \param _sStream The stream data.
		 **/
		static inline void LSN_STDCALL									BatchF32ToPcm24( const std::vector<float> &_vSrc, std::vector<uint8_t> &_vOut, LSN_STREAMING &_sStream );

#ifdef __AVX2__
		/**
		 * Converts a batch of floats to 8-bit PCM using AVX 2.
		 * 
		 * \param _vSrc The input samples.
		 * \param _vOut The output samples.
		 * \param _sStream The stream data.
		 **/
		static inline void LSN_STDCALL									BatchF32ToPcm8_AVX2( const std::vector<float> &_vSrc, std::vector<uint8_t> &_vOut, LSN_STREAMING &_sStream );

		/**
		 * Converts a batch of floats to 16-bit PCM using AVX 2.
		 * 
		 * \param _vSrc The input samples.
		 * \param _vOut The output samples.
		 * \param _sStream The stream data.
		 **/
		static inline void LSN_STDCALL									BatchF32ToPcm16_AVX2( const std::vector<float> &_vSrc, std::vector<uint8_t> &_vOut, LSN_STREAMING &_sStream );

		/**
		 * Converts a batch of floats to 24-bit PCM using AVX 2.
		 * 
		 * \param _vSrc The input samples.
		 * \param _vOut The output samples.
		 * \param _sStream The stream data.
		 **/
		static inline void LSN_STDCALL									BatchF32ToPcm24_AVX2( const std::vector<float> &_vSrc, std::vector<uint8_t> &_vOut, LSN_STREAMING &_sStream );
#endif	// #ifdef __AVX2__

#ifdef __AVX512F__
		/* Converts a batch of floats to 8-bit PCM using AVX-512.
		 * 
		 * \param _vSrc The input samples.
		 * \param _vOut The output samples.
		 * \param _sStream The stream data.
		 **/
		static inline void LSN_STDCALL									BatchF32ToPcm8_AVX512( const std::vector<float> &_vSrc, std::vector<uint8_t> &_vOut, LSN_STREAMING &_sStream );

		/**
		 * Converts a batch of floats to 16-bit PCM using AVX-512.
		 * 
		 * \param _vSrc The input samples.
		 * \param _vOut The output samples.
		 * \param _sStream The stream data.
		 **/
		static inline void LSN_STDCALL									BatchF32ToPcm16_AVX512( const std::vector<float> &_vSrc, std::vector<uint8_t> &_vOut, LSN_STREAMING &_sStream );

		/**
		 * Converts a batch of floats to 24-bit PCM using AVX-512.
		 * 
		 * \param _vSrc The input samples.
		 * \param _vOut The output samples.
		 * \param _sStream The stream data.
		 **/
		static inline void LSN_STDCALL									BatchF32ToPcm24_AVX512( const std::vector<float> &_vSrc, std::vector<uint8_t> &_vOut, LSN_STREAMING &_sStream );
#endif	// #ifdef __AVX512F__
		
#pragma optimize( "", on )
	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Performs no conversion.  Input is directly sent to the file stream.
	 * 
	 * \param _vSrc The input samples.
	 * \param _vOut The output samples.
	 * \param _sStream The stream data.
	 **/
	inline void LSN_STDCALL CWavFile::BatchF32ToF32( const std::vector<float> &_vSrc, std::vector<uint8_t> &/*_vOut*/, LSN_STREAMING &_sStream ) {
		if LSN_LIKELY( _vSrc.size() ) {
			_sStream.sfFile.WriteToFile( reinterpret_cast<const uint8_t *>(_vSrc.data()),
				_vSrc.size() * sizeof( float ) );
		}
	}

	/**
	 * Converts a batch of floats to 8-bit PCM using raw C++.
	 * 
	 * \param _vSrc The input samples.
	 * \param _vOut The output samples.
	 * \param _sStream The stream data.
	 **/
	inline void LSN_STDCALL CWavFile::BatchF32ToPcm8( const std::vector<float> &_vSrc, std::vector<uint8_t> &_vOut, LSN_STREAMING &_sStream ) {
		try {
			if LSN_UNLIKELY( !_vSrc.size() ) { _vOut.clear(); return; }
			size_t sSize = _vSrc.size();
			if LSN_UNLIKELY( _vOut.size() < sSize ) {
				_vOut.resize( sSize );
			}
			LSN_PREFETCH_LINE( _vSrc.data() + (sSize >> 1) );
			for ( size_t I = 0; I < sSize; ++I ) {
				_vOut[I] = CUtilities::SampleToUi8( _vSrc[I] );
			}

			_sStream.sfFile.WriteToFile( _vOut.data(),
				_vSrc.size() );
		}
		catch ( ... ) {}
	}

	/**
	 * Converts a batch of floats to 16-bit PCM using raw C++.
	 * 
	 * \param _vSrc The input samples.
	 * \param _vOut The output samples.
	 * \param _sStream The stream data.
	 **/
	inline void LSN_STDCALL CWavFile::BatchF32ToPcm16( const std::vector<float> &_vSrc, std::vector<uint8_t> &_vOut, LSN_STREAMING &_sStream ) {
		try {
			if LSN_UNLIKELY( !_vSrc.size() ) { _vOut.clear(); return; }
			size_t sSize = _vSrc.size();
			if LSN_UNLIKELY( _vOut.size() < sSize << 1 ) {
				_vOut.resize( _vSrc.size() << 1 );
			}
			LSN_PREFETCH_LINE( _vSrc.data() + (sSize >> 1) );
			for ( size_t I = 0; I < sSize; ++I ) {
				reinterpret_cast<int16_t *>(_vOut.data())[I] = CUtilities::SampleToI16( _vSrc[I] );
			}

			_sStream.sfFile.WriteToFile( _vOut.data(),
				_vSrc.size() * sizeof( uint16_t ) );
		}
		catch ( ... ) {}
	}

	/**
	 * Converts a batch of floats to 16-bit PCM using raw C++.  Applies dither (state stored in _sStream).
	 * 
	 * \param _vSrc The input samples.
	 * \param _vOut The output samples.
	 * \param _sStream The stream data.
	 **/
	inline void LSN_STDCALL CWavFile::BatchF32ToPcm16_Dither( const std::vector<float> &_vSrc, std::vector<uint8_t> &_vOut, LSN_STREAMING &_sStream ) {
		try {
			if LSN_UNLIKELY( !_vSrc.size() ) { _vOut.clear(); return; }
			size_t sSize = _vSrc.size();
			if LSN_UNLIKELY( _vOut.size() < sSize << 1 ) {
				_vOut.resize( _vSrc.size() << 1 );
			}
			LSN_PREFETCH_LINE( _vSrc.data() + (sSize >> 1) );
			for ( size_t I = 0; I < sSize; ++I ) {
				double dThis = _vSrc[I] + _sStream.dDitherError;
				int16_t i16Final = CUtilities::SampleToI16( static_cast<float>(dThis) );
				double dScaled = double( i16Final );
				double dQuantized = dScaled * (1.0 / 32767.0);
				_sStream.dDitherError = dThis - dQuantized;

				reinterpret_cast<int16_t *>(_vOut.data())[I] = i16Final;
			}

			_sStream.sfFile.WriteToFile( _vOut.data(),
				_vSrc.size() * sizeof( uint16_t ) );
		}
		catch ( ... ) {}
	}

	/**
	 * Converts a batch of floats to 24-bit PCM using raw C++.
	 * 
	 * \param _vSrc The input samples.
	 * \param _vOut The output samples.
	 * \param _sStream The stream data.
	 **/
	inline void LSN_STDCALL CWavFile::BatchF32ToPcm24( const std::vector<float> &_vSrc, std::vector<uint8_t> &_vOut, LSN_STREAMING &_sStream ) {
		try {
			if LSN_UNLIKELY( !_vSrc.size() ) { _vOut.clear(); return; }
			size_t sSize = _vSrc.size();
			if LSN_UNLIKELY( _vOut.size() < sSize * 3 ) {
				_vOut.resize( sSize * 3 );
			}
			LSN_PREFETCH_LINE( _vSrc.data() + (sSize >> 1) );
			
			// Assuming penalty for misaligned writes is minor.  Some platforms may benefit from a slower algorithm
			//	that does only aligned writes.
			sSize--;
			size_t I = 0;
			for ( ; I < sSize; ++I ) {
				int32_t i32Tmp = CUtilities::SampleToI24( _vSrc[I] );
				auto sIdx = I * 3;
				(*reinterpret_cast<int32_t *>(&_vOut[sIdx])) = i32Tmp;
			}

			// Last one has to be carefully constructed.
			int32_t i32Tmp = CUtilities::SampleToI24( _vSrc[I] );
			auto sIdx = I * 3;
			(*reinterpret_cast<int16_t *>(&_vOut[sIdx])) = int16_t( i32Tmp );
			_vOut[sIdx+2] = uint8_t( i32Tmp >> 16 );

			_sStream.sfFile.WriteToFile( _vOut.data(),
				_vSrc.size() * 3 );
		}
		catch ( ... ) {}
	}

#ifdef __AVX2__
	/**
	 * Converts a batch of floats to 8-bit PCM using AVX 2.
	 * 
	 * \param _vSrc The input samples.
	 * \param _vOut The output samples.
	 * \param _sStream The stream data.
	 **/
	inline void LSN_STDCALL CWavFile::BatchF32ToPcm8_AVX2( const std::vector<float> &_vSrc, std::vector<uint8_t> &_vOut, LSN_STREAMING &_sStream ) {
		try {
			if LSN_UNLIKELY( !_vSrc.size() ) { _vOut.clear(); return; }
			size_t sSize = _vSrc.size();
			if LSN_UNLIKELY( _vOut.size() < sSize ) {
				_vOut.resize( sSize );
			}
			LSN_PREFETCH_LINE( _vSrc.data() + (sSize >> 1) );
			size_t I = 0;
			constexpr size_t sRegSize = sizeof( __m256 ) / sizeof( float );
			if LSN_LIKELY( sSize >= sRegSize ) {
				size_t sTotal = sSize - sRegSize;
				while ( I <= sTotal ) {
					CUtilities::SampleToUi8_AVX2( &_vSrc[I], &_vOut[I] );
					I += sRegSize;
				}
			}

			for ( ; I < sSize; ++I ) {
				_vOut[I] = CUtilities::SampleToUi8( _vSrc[I] );
			}

			_sStream.sfFile.WriteToFile( _vOut.data(),
				_vSrc.size() );
		}
		catch ( ... ) {}
	}

	/**
	 * Converts a batch of floats to 16-bit PCM using AVX 2.
	 * 
	 * \param _vSrc The input samples.
	 * \param _vOut The output samples.
	 * \param _sStream The stream data.
	 **/
	inline void LSN_STDCALL CWavFile::BatchF32ToPcm16_AVX2( const std::vector<float> &_vSrc, std::vector<uint8_t> &_vOut, LSN_STREAMING &_sStream ) {
		try {
			if LSN_UNLIKELY( !_vSrc.size() ) { _vOut.clear(); return; }
			size_t sSize = _vSrc.size();
			if LSN_UNLIKELY( _vOut.size() < sSize << 1 ) {
				_vOut.resize( _vSrc.size() << 1 );
			}
			LSN_PREFETCH_LINE( _vSrc.data() + (sSize >> 1) );
			size_t I = 0;
			constexpr size_t sRegSize = sizeof( __m256 ) / sizeof( float );
			if LSN_LIKELY( sSize >= sRegSize ) {
				size_t sTotal = sSize - sRegSize;
				while ( I <= sTotal ) {
					CUtilities::SampleToI16_AVX2( &_vSrc[I], reinterpret_cast<int16_t *>(&_vOut[I<<1]) );
					I += sRegSize;
				}
			}

			for ( ; I < sSize; ++I ) {
				(*reinterpret_cast<int16_t *>(&_vOut[I<<1])) = CUtilities::SampleToI16( _vSrc[I] );
			}

			_sStream.sfFile.WriteToFile( _vOut.data(),
				_vSrc.size() * sizeof( uint16_t ) );
		}
		catch ( ... ) {}
	}

	/**
	 * Converts a batch of floats to 24-bit PCM using AVX 2.
	 * 
	 * \param _vSrc The input samples.
	 * \param _vOut The output samples.
	 * \param _sStream The stream data.
	 **/
	inline void LSN_STDCALL CWavFile::BatchF32ToPcm24_AVX2( const std::vector<float> &_vSrc, std::vector<uint8_t> &_vOut, LSN_STREAMING &_sStream ) {
		try {
			if LSN_UNLIKELY( !_vSrc.size() ) { _vOut.clear(); return; }
			size_t sSize = _vSrc.size();
			if LSN_UNLIKELY( _vOut.size() < sSize * 3 ) {
				_vOut.resize( sSize * 3 );
			}
			LSN_PREFETCH_LINE( _vSrc.data() + (sSize >> 1) );
			

			size_t I = 0;
			constexpr size_t sRegSize = sizeof( __m256 ) / sizeof( float );
			LSN_ALIGN( 32 )
			int32_t i32uffer[sRegSize];
			if LSN_LIKELY( sSize >= sRegSize ) {
				size_t sTotal = sSize - sRegSize;
				while ( I <= sTotal ) {
					CUtilities::SampleToI24_AVX2( &_vSrc[I], i32uffer );
					for ( size_t J = 0; J < (sRegSize - 1); ++J ) {
						auto sIdx = (I + J) * 3;
						(*reinterpret_cast<int32_t *>(&_vOut[sIdx])) = i32uffer[J];
					}
					(*reinterpret_cast<int16_t *>(&_vOut[(I+(sRegSize - 1))*3])) = int16_t( i32uffer[(sRegSize-1)] );
					_vOut[(I+(sRegSize - 1))*3+2] = int8_t( i32uffer[(sRegSize-1)] >> 16 );
					I += sRegSize;
				}
				
			}
			if LSN_UNLIKELY( sSize > I ) {	// Unlikely because almost always we will be passing in buffers that are a size divisible by 8 (sRegSize).
				--sSize;
				for ( ; I < sSize; ++I ) {
					int32_t i32Tmp = CUtilities::SampleToI24( _vSrc[I] );
					auto sIdx = I * 3;
					(*reinterpret_cast<int32_t *>(&_vOut[sIdx])) = i32Tmp;
				}

				// Last one has to be carefully constructed.
				int32_t i32Tmp = CUtilities::SampleToI24( _vSrc[I] );
				auto sIdx = I * 3;
				(*reinterpret_cast<int16_t *>(&_vOut[sIdx])) = int16_t( i32Tmp );
				_vOut[sIdx+2] = uint8_t( i32Tmp >> 16 );
			}

			_sStream.sfFile.WriteToFile( _vOut.data(),
				_vSrc.size() * 3 );
		}
		catch ( ... ) {}
	}
#endif	// #ifdef __AVX2__

#ifdef __AVX512F__
		/* Converts a batch of floats to 8-bit PCM using AVX-512.
		 * 
		 * \param _vSrc The input samples.
		 * \param _vOut The output samples.
		 * \param _sStream The stream data.
		 **/
		inline void LSN_STDCALL CWavFile::BatchF32ToPcm8_AVX512( const std::vector<float> &_vSrc, std::vector<uint8_t> &_vOut, LSN_STREAMING &_sStream ) {
			try {
			if LSN_UNLIKELY( !_vSrc.size() ) { _vOut.clear(); return; }
			size_t sSize = _vSrc.size();
			if LSN_UNLIKELY( _vOut.size() < sSize ) {
				_vOut.resize( sSize );
			}
			LSN_PREFETCH_LINE( _vSrc.data() + (sSize >> 1) );
			size_t I = 0;
			constexpr size_t sRegSize = sizeof( __m512 ) / sizeof( float );
			if LSN_LIKELY( sSize >= sRegSize ) {
				size_t sTotal = sSize - sRegSize;
				while ( I <= sTotal ) {
					CUtilities::SampleToUi8_AVX512( &_vSrc[I], &_vOut[I] );
					I += sRegSize;
				}
			}

			for ( ; I < sSize; ++I ) {
				_vOut[I] = CUtilities::SampleToUi8( _vSrc[I] );
			}

			_sStream.sfFile.WriteToFile( _vOut.data(),
				_vSrc.size() );
		}
		catch ( ... ) {}
		}

		/**
		 * Converts a batch of floats to 16-bit PCM using AVX-512.
		 * 
		 * \param _vSrc The input samples.
		 * \param _vOut The output samples.
		 * \param _sStream The stream data.
		 **/
		inline void LSN_STDCALL CWavFile::BatchF32ToPcm16_AVX512( const std::vector<float> &_vSrc, std::vector<uint8_t> &_vOut, LSN_STREAMING &_sStream ) {
			try {
			if LSN_UNLIKELY( !_vSrc.size() ) { _vOut.clear(); return; }
			size_t sSize = _vSrc.size();
			if LSN_UNLIKELY( _vOut.size() < sSize << 1 ) {
				_vOut.resize( _vSrc.size() << 1 );
			}
			LSN_PREFETCH_LINE( _vSrc.data() + (sSize >> 1) );
			size_t I = 0;
			constexpr size_t sRegSize = sizeof( __m512 ) / sizeof( float );
			if LSN_LIKELY( sSize >= sRegSize ) {
				size_t sTotal = sSize - sRegSize;
				while ( I <= sTotal ) {
					CUtilities::SampleToI16_AVX512( &_vSrc[I], reinterpret_cast<int16_t *>(&_vOut[I<<1]) );
					I += sRegSize;
				}
			}

			for ( ; I < sSize; ++I ) {
				(*reinterpret_cast<int16_t *>(&_vOut[I<<1])) = CUtilities::SampleToI16( _vSrc[I] );
			}

			_sStream.sfFile.WriteToFile( _vOut.data(),
				_vSrc.size() * sizeof( uint16_t ) );
		}
		catch ( ... ) {}
		}

		/**
		 * Converts a batch of floats to 24-bit PCM using AVX-512.
		 * 
		 * \param _vSrc The input samples.
		 * \param _vOut The output samples.
		 * \param _sStream The stream data.
		 **/
		inline void LSN_STDCALL CWavFile::BatchF32ToPcm24_AVX512( const std::vector<float> &_vSrc, std::vector<uint8_t> &_vOut, LSN_STREAMING &_sStream ) {
			try {
			if LSN_UNLIKELY( !_vSrc.size() ) { _vOut.clear(); return; }
			size_t sSize = _vSrc.size();
			if LSN_UNLIKELY( _vOut.size() < sSize * 3 ) {
				_vOut.resize( sSize * 3 );
			}
			LSN_PREFETCH_LINE( _vSrc.data() + (sSize >> 1) );
			

			size_t I = 0;
			constexpr size_t sRegSize = sizeof( __m512 ) / sizeof( float );
			LSN_ALIGN( 64 )
			int32_t i32uffer[sRegSize];
			if LSN_LIKELY( sSize >= sRegSize ) {
				size_t sTotal = sSize - sRegSize;
				while ( I <= sTotal ) {
					CUtilities::SampleToI24_AVX512( &_vSrc[I], i32uffer );
					for ( size_t J = 0; J < (sRegSize - 1); ++J ) {
						auto sIdx = (I + J) * 3;
						(*reinterpret_cast<int32_t *>(&_vOut[sIdx])) = i32uffer[J];
					}
					(*reinterpret_cast<int16_t *>(&_vOut[(I+(sRegSize - 1))*3])) = int16_t( i32uffer[(sRegSize-1)] );
					_vOut[(I+(sRegSize - 1))*3+2] = int8_t( i32uffer[(sRegSize-1)] >> 16 );
					I += sRegSize;
				}
				
			}
			if LSN_UNLIKELY( sSize ) {	// Unlikely because almost always we will be passing in buffers that are a size divisible by 8 (sRegSize).
				--sSize;
				for ( ; I < sSize; ++I ) {
					int32_t i32Tmp = CUtilities::SampleToI24( _vSrc[I] );
					auto sIdx = I * 3;
					(*reinterpret_cast<int32_t *>(&_vOut[sIdx])) = i32Tmp;
				}

				// Last one has to be carefully constructed.
				int32_t i32Tmp = CUtilities::SampleToI24( _vSrc[I] );
				auto sIdx = I * 3;
				(*reinterpret_cast<int16_t *>(&_vOut[sIdx])) = int16_t( i32Tmp );
				_vOut[sIdx+2] = uint8_t( i32Tmp >> 16 );
			}

			_sStream.sfFile.WriteToFile( _vOut.data(),
				_vSrc.size() * 3 );
		}
		catch ( ... ) {}
		}
#endif	// #ifdef __AVX512F__

}	// namespace lsn

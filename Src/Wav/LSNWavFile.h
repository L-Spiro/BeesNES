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


		// == Types.
		typedef std::vector<double, CAlignmentAllocator<double, 64>>	lwtrack;
		typedef std::vector<lwtrack>									lwaudio;
		

		/** The save data. */
		struct LSN_SAVE_DATA {
			uint32_t													uiHz;					// Only overrides if not 0.
			uint16_t													uiBitsPerSample;		// Only overrides if not 0.

			LSN_SAVE_DATA() :
				uiHz( 0 ),
				uiBitsPerSample( 0 ) {}
			LSN_SAVE_DATA( uint32_t _ui32Hz, uint16_t _ui16Bits ) :
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
		typedef bool (LSN_STDCALL *										PfStartConditionFunc)( uint64_t _ui64AbsoluteIdx, float _fSample, LSN_CONDITIONS_DATA &_cdData );
		typedef bool (LSN_STDCALL *										PfEndConditionFunc)( uint64_t _ui64AbsoluteIdx, uint64_t _ui64IdxSinceStart, float _fSample, LSN_CONDITIONS_DATA &_cdData );

		/** A stream-to-file structure. */
		struct LSN_STREAMING {
			uint64_t													ui64SamplesReceived = 0;	/**< The number of samples sent to the stream. */
			uint64_t													ui64SamplesWritten = 0;		/**< The number of samples written to the file. */
			//uint64_t													ui64FinalSampleCount = 0;	/**< If not 0, indicates the known final sample count. */
			uint64_t													ui64WavFileOffset_Size = 0;	/**< The offset of the size value in the WAV file. */
			uint64_t													ui64WavFileOffset_DSize = 0;/**< The offset of the data-size value in the WAV file. */
			size_t														stBufferSize = 1024 * 10;	/**< The size of the buffer to fill before flushing. */
			std::vector<float>											vCurBuffer;					/**< The current buffer awaiting samples. */
			std::wstring												wsPath;						/**< The path to the file to which we are streaming. */
			CStdFile													sfFile;						/**< The file to which to write the WAV data. */
			std::queue<std::vector<float>>								qBufferQueue;				/**< The queue of buffers handled by the thread. */
			std::mutex													mMutex;						/**< The thread mutex for accessing qBufferQueue and bStreaming. */
			std::condition_variable										cvCondition;				/**< The conditional variable for the lock. */
			std::thread													tThread;					/**< The thread for writing to the file. */
			LSN_CONDITIONS_DATA											cdStartData;				/**< The start-condition data. */
			LSN_CONDITIONS_DATA											cdStopData;					/**< The stop-condition data. */
			PfStartConditionFunc										pfStartCondFunc = nullptr;	/**< The start-condition function. */
			PfStartConditionFunc										pfEndCondFunc = nullptr;	/**< The end-condition function. */
			uint32_t													ui32WavFile_Size = 0;		/**< The final file size to write to the WAV file. */
			uint32_t													ui32WavFile_DSize = 0;		/**< The final data size to write to the WAV file. */
			uint32_t													ui32Hz = 44100;				/**< The file Hz. */
			LSN_FORMAT													fFormat = LSN_F_PCM;		/**< The WAV-file format. */
			uint16_t													ui16Bits = 16;				/**< The number of bits-per-sample. */			
			uint16_t													ui16Channels = 2;			/**< Total channels to output. */
			bool														bEnd = true;				/**< Tells the thread to stop. */
			bool														bStreaming = false;			/**< If true, the file must be closed either manually or in the destructor. */
			bool														bDither = false;			/**< To dither 16-bit PCM or not. */
		};


		// == Functions.
		/**
		 * Loads a WAV file.
		 *
		 * \param _pcPath The UTF-8 path to open.
		 * \return Returns true if the file was opened.
		 */
		bool															Open( const char8_t * _pcPath );

		/**
		 * Loads a WAV file.
		 *
		 * \param _pwcPath The UTF-16 path to open.
		 * \return Returns true if the file was opened.
		 */
		bool															Open( const char16_t * _pwcPath );

		/**
		 * Loads a WAV file from memory.  This is just an in-memory version of the file.
		 *
		 * \param _vData The in-memory file to load.
		 * \return Returns true if the file is a valid WAV file.
		 */
		bool															LoadFromMemory( const std::vector<uint8_t> &_vData );

		/**
		 * Saves as a PCM WAV file.
		 *
		 * \param _pcPath The path to where the file will be saved.
		 * \param _vSamples The samples to convert and write to the file.
		 * \param _psdSaveSettings Settings to override this class's settings.
		 * \return Returns true if the file was created and saved.
		 */
		bool															SaveAsPcm( const char8_t * _pcPath, const lwaudio &_vSamples,
			const LSN_SAVE_DATA * _psdSaveSettings = nullptr ) const;

		/**
		 * Saves as a PCM WAV file.
		 *
		 * \param _pcPath The path to where the file will be saved.
		 * \param _vSamples The samples to convert and write to the file.
		 * \param _psdSaveSettings Settings to override this class's settings.
		 * \return Returns true if the file was created and saved.
		 */
		bool															SaveAsPcm( const char16_t * _pcPath, const lwaudio &_vSamples,
			const LSN_SAVE_DATA * _psdSaveSettings = nullptr ) const {
			return SaveAsPcm( CUtilities::Utf16ToUtf8( _pcPath ).c_str(), _vSamples, _psdSaveSettings );
		}

		/**
		 * Saves as a PCM WAV file.
		 *
		 * \param _pcPath The path to where the file will be saved.
		 * \param _vSamples The samples to convert and write to the file.
		 * \param _psdSaveSettings Settings to override this class's settings.
		 * \return Returns true if the file was created and saved.
		 */
		bool															SaveAsPcm( const char8_t * _pcPath, const lwtrack &_vSamples,
			const LSN_SAVE_DATA * _psdSaveSettings = nullptr ) const {
			lwaudio aAudio;
			aAudio.push_back( _vSamples );
			return SaveAsPcm( _pcPath, aAudio,
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
		bool															SaveAsPcm( const char16_t * _pcPath, const lwtrack &_vSamples,
			const LSN_SAVE_DATA * _psdSaveSettings = nullptr ) const {
			return SaveAsPcm( CUtilities::Utf16ToUtf8( _pcPath ).c_str(), _vSamples, _psdSaveSettings );
		}

		/**
		 * Opens a stream to a WAV file.  Samples can be written over time.
		 * 
		 * \param _pcPath The path to which to stream WAV data.
		 * \param _ui32Hz The WAV Hz.
		 * \param _fFormat The WAV format.
		 * \param _ui16Bits The PCM bits.
		 * \param _ui16Channels The channel count.
		 * \param _bDither Whether to dither 16-bit PCM data or not.
		 * \param _scStartCondition The starting condition.
		 * \param _scpStartParm The starting condition paramater.
		 * \param _ecEndCondition The stopping condition.
		 * \param _scpEndParm The stopping condition paramater.
		 * \param _stBufferSize The buffer size (in samples).
		 * \return Returns true if the file could initially be created and all parameters are correct.
		 **/
		bool															StreamToFile( const char8_t * _pcPath, uint32_t _ui32Hz, LSN_FORMAT _fFormat, uint16_t _ui16Bits, uint16_t _ui16Channels, bool _bDither,
			LSN_START_CONDITIONS _scStartCondition, LSN_STREAM_COND_PARM _scpStartParm,
			LSN_END_CONDITIONS _ecEndCondition, LSN_STREAM_COND_PARM _scpEndParm,
			size_t _stBufferSize = 10 * 1024 );

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
		 * Fills a vector with the whole range of samples for a given channel.
		 *
		 * \param _uiChan The channel whose sample data is to be obtained.
		 * \param _vResult The vector containing the samples.
		 * \return Returns true if the vector was able to fit all samples.
		 */
		bool															GetAllSamples( uint16_t _uiChan, lwtrack &_vResult ) const;

		/**
		 * Fills a vector with the whole range of samples for a given channel.
		 *
		 * \param _uiChan The channel whose sample data is to be obtained.
		 * \param _vResult The vector containing the samples.
		 * \param _stFrom The starting sample to get.
		 * \param _stTo The ending sample to get, exclusive.
		 * \return Returns true if the vector was able to fit all samples.
		 */
		bool															GetSamples( uint16_t _uiChan, lwtrack &_vResult, size_t _stFrom, size_t _stTo ) const;

		/**
		 * Fills an array of vectors.  There is an array of vectors for each channel, and each vector contains all
		 *	of the samples for that channel.
		 *
		 * \param _vResult The array of vectors to be filled with all samples in this file.
		 * \return Returns true if the vector(s) was/were able to fit all samples.
		 */
		bool															GetAllSamples( lwaudio &_vResult ) const;

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
		 * Calculates the size of a buffer needed to hold the given number of samples on the given number of channels in te given format.
		 *
		 * \param _fFormat The format of the samples.
		 * \param _uiSamples The number of samples per channel.
		 * \param _uiChannels The number of channels.
		 * \param _uiBitsPerPixel The number of bits per sample.
		 * \return Returns the size of a buffer needed to hold the given number of samples on the given number of channels in te given format.
		 */
		inline uint32_t													CalcSize( LSN_FORMAT _fFormat, uint32_t _uiSamples, uint16_t _uiChannels, uint16_t _uiBitsPerPixel ) const {
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
			uint8_t														ui8UnshiftedNote;
			uint8_t														ui8FineTune;
			uint8_t														ui8Gain;
			uint8_t														ui8LowNote;
			uint8_t														ui8HiNote;
			uint8_t														ui8LowVel;
			uint8_t														ui8HiVel;
		};

		
		// == Members.
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
		bool															LoadData( const LSN_DATA_CHUNK * _pdcChunk );

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
		 * \return Returns true if the vector was able to hold all of the values.
		 */
		bool															Pcm8ToF64( uint32_t _ui32From, uint32_t _ui32To, uint16_t _uiChan, lwtrack &_vResult ) const;

		/**
		 * Converts a bunch of 16-bit PCM samples to double.
		 *
		 * \param _ui32From Starting sample index.
		 * \param _ui32To Ending sample index.
		 * \param _uiChan The channel whose sample data is to be obtained.
		 * \param _vResult The vector containing the samples.
		 * \return Returns true if the vector was able to hold all of the values.
		 */
		bool															Pcm16ToF64( uint32_t _ui32From, uint32_t _ui32To, uint16_t _uiChan, lwtrack &_vResult ) const;

		/**
		 * Converts a bunch of 24-bit PCM samples to double.
		 *
		 * \param _ui32From Starting sample index.
		 * \param _ui32To Ending sample index.
		 * \param _uiChan The channel whose sample data is to be obtained.
		 * \param _vResult The vector containing the samples.
		 * \return Returns true if the vector was able to hold all of the values.
		 */
		bool															Pcm24ToF64( uint32_t _ui32From, uint32_t _ui32To, uint16_t _uiChan, lwtrack &_vResult ) const;

		/**
		 * Converts a bunch of 32-bit PCM samples to double.
		 *
		 * \param _ui32From Starting sample index.
		 * \param _ui32To Ending sample index.
		 * \param _uiChan The channel whose sample data is to be obtained.
		 * \param _vResult The vector containing the samples.
		 * \return Returns true if the vector was able to hold all of the values.
		 */
		bool															Pcm32ToF64( uint32_t _ui32From, uint32_t _ui32To, uint16_t _uiChan, lwtrack &_vResult ) const;

		/**
		 * Converts a bunch of 32-bit float samples to double.
		 *
		 * \param _ui32From Starting sample index.
		 * \param _ui32To Ending sample index.
		 * \param _uiChan The channel whose sample data is to be obtained.
		 * \param _vResult The vector containing the samples.
		 * \return Returns true if the vector was able to hold all of the values.
		 */
		bool															F32ToF64( uint32_t _ui32From, uint32_t _ui32To, uint16_t _uiChan, lwtrack &_vResult ) const;

		/**
		 * Converts a batch of F64 samples to PCM samples.
		 *
		 * \param _vSrc The samples to convert.
		 * \param _vDst The buffer to which to convert the samples.
		 * \return Returns trye if all samples were added to the buffer.
		 */
		static bool														BatchF64ToPcm8( const lwaudio &_vSrc, std::vector<uint8_t> &_vDst );

		/**
		 * Converts a batch of F64 samples to PCM samples.
		 *
		 * \param _vSrc The samples to convert.
		 * \param _vDst The buffer to which to convert the samples.
		 * \return Returns trye if all samples were added to the buffer.
		 */
		static bool														BatchF64ToPcm16( const lwaudio &_vSrc, std::vector<uint8_t> &_vDst );

		/**
		 * Converts a batch of F64 samples to PCM samples.
		 *
		 * \param _vSrc The samples to convert.
		 * \param _vDst The buffer to which to convert the samples.
		 * \return Returns trye if all samples were added to the buffer.
		 */
		static bool														BatchF64ToPcm24( const lwaudio &_vSrc, std::vector<uint8_t> &_vDst );

		/**
		 * Converts a batch of F64 samples to PCM samples.
		 *
		 * \param _vSrc The samples to convert.
		 * \param _vDst The buffer to which to convert the samples.
		 * \return Returns trye if all samples were added to the buffer.
		 */
		static bool														BatchF64ToPcm32( const lwaudio &_vSrc, std::vector<uint8_t> &_vDst );

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
		 * The stream-to-file writer thread.
		 **/
		void															StreamWriterThread();
	};

}	// namespace lsn

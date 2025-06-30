/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Processes batches of WAV files to make hardware-accurate recordings, complete with fade-out etc.  Helps to make OST's.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Localization/LSNLocalization.h"
#include "../Options/LSNWavEditorWindowOptions.h"
#include "../Utilities/LSNUtilities.h"
#include "LSNWavFile.h"

#include <atomic>
#include <filesystem>
#include <format>
#include <map>
#include <set>
#include <string>
#include <vector>


#define LSN_WAV_EDITOR_VERSION											3


namespace lsn {

	/**
	 * Class CWavEditor
	 * \brief Processes batches of WAV files to make hardware-accurate recordings, complete with fade-out etc.
	 *
	 * Description: Processes batches of WAV files to make hardware-accurate recordings, complete with fade-out etc.  Helps to make OST's.
	 */
	class CWavEditor {
	public :
		CWavEditor();
		~CWavEditor();


		// == Enumerations.
		/** Actual Hz types. */
		enum LSN_ACTUAL_HZ : int32_t {
			LSN_AH_NTSC													= int32_t( -LSN_PM_NTSC ),
			LSN_AH_PAL													= int32_t( -LSN_PM_PAL ),
			LSN_AH_DENDY												= int32_t( -LSN_PM_DENDY ),
			LSN_AH_PALM													= int32_t( -LSN_PM_PALM ),
			LSN_AH_PALN													= int32_t( -LSN_PM_PALN ),
			LSN_AH_BY_FILE												= int32_t( -100 ),
			LSN_AH_CUSTOM												= int32_t( -1000 ),
		};

		/** Mains-hum types. */
		enum LSN_MAINS_HUM {
			LSN_MH_XX_BLACK,
			LSN_MH_X_BLACK,
			LSN_MH_BLACK,
			LSN_MH_GREY_00,
			LSN_MH_GREY_10,
			LSN_MH_GREY_20EM,
			LSN_MH_WHITE,
			LSN_MH_RED,
			LSN_MH_GREEN,
			LSN_MH_BLUE,
		};

		/** White-noise types. */
		enum LSN_WHITE_NOISE {
			LSN_WN_GAUSSIAN,
			LSN_WN_UNIFORM,
		};

		/** Filter types. */
		enum LSN_FILTER {
			LSN_F_POLE,
			LSN_F_SINC,
		};

		/** Channel types. */
		enum LSN_CHANNELS {
			LSN_C_MONO													= 1,
			LSN_C_STEREO												= 2,
			LSN_C_SURROUND												= 3,
		};

		/** Volume type. */
		enum LSN_VOL_TYPE {
			LSN_VT_ABS,
			LSN_VT_NORM,
			LSN_VT_LOUDNESS,
		};


		// == Types.
		/** Per-file data. */
		struct LSN_PER_FILE {
			double														dStartTime = 0.0;								/**< The starting point in the file.  Used for clipping and the start of the music. */
			//double														dEndTime = 0.0;									/**< The ending point in the file.  Used for clipping the source WAV. */
			double														dStopTime = 0.0;								/**< Time for when to stop the music. */
			double														dDelayTime = 0.0;								/**< How long to continue before fading out. */
			double														dFadeTime = 0.0;								/**< The fade duration. */
			double														dOpeningSilence = 0.2;							/**< Opening silence. */
			double														dTrailingSilence = 0.5;							/**< Trailing silence. */
			double														dActualHz = 0.0;								/**< The actual HZ of the file. */
			double														dVolume = 1.0;									/**< The characteristics volume. */
			double														dLpf = 0.0;										/**< The characteristics LPF. */
			double														dHpf0 = 0.0;									/**< The characteristics HPF 1. */
			double														dHpf1 = 0.0;									/**< The characteristics HPF 2. */
			double														dHpf2 = 0.0;									/**< The characteristics HPF 3. */
			double														dFalloffLpf = 0.0;								/**< The LPF fall-off. */
			double														dFalloffHpf0 = 0.0;								/**< The HPF 1 fall-off. */
			double														dFalloffHpf1 = 0.0;								/**< The HPF 2 fall-off. */
			double														dFalloffHpf2 = 0.0;								/**< The HPF 3 fall-off. */
			std::wstring												wsFilePrefix;									/**< The file name prefix. */
			std::wstring												wsFilePostFix;									/**< The file name postfix. */
			std::wstring												wsName;											/**< The name of the track. */
			std::wstring												wsArtist;										/**< The artist of the track. */
			std::wstring												wsAlbum;										/**< The album of the track. */
			std::wstring												wsYear;											/**< The year of the track. */
			std::wstring												wsComment;										/**< The comment of the track. */
			uint32_t													ui32Id = 0;										/**< ID of the associated WAV file. */
			LSN_FILTER													fLpfType = LSN_F_POLE;							/**< The LPF filter type. */
			LSN_FILTER													fHpf0Type = LSN_F_POLE;							/**< The HPF 1 filter type. */
			LSN_FILTER													fHpf1Type = LSN_F_POLE;							/**< The HPF 2 filter type. */
			LSN_FILTER													fHpf2Type = LSN_F_POLE;							/**< The HPF 3 filter type. */
			LSN_START_STOP_TIME											sstStartMod = LSN_SST_EXACT;					/**< The start-time modifier. */
			LSN_START_STOP_TIME											sstStopMod = LSN_SST_EXACT;						/**< The stop-time modifier. */
			bool														bLoop = false;									/**< Whether to loop or 1-short. */
			bool														bFade = false;									/**< Whether to fade or not. */
			bool														bInvert = true;									/**< Whether to invert the waveform or not. */
			bool														bLpf = false;									/**< Whether to LPF or not. */
			bool														bHpf0 = false;									/**< Whether to HPF 1 or not. */
			bool														bHpf1 = false;									/**< Whether to HPF 2 or not. */
			bool														bHpf2 = false;									/**< Whether to HPF 3 or not. */

			bool														operator < ( const LSN_PER_FILE & _pfOther ) const {
				return ui32Id < _pfOther.ui32Id;
			}

		};

		/** The output data. */
		struct LSN_OUTPUT {
			double														dAbsoluteVol = 5.0;								/**< Absolute volume. */
			double														dNormalizeTo = std::pow( 10, -5.5 / 20.0 );		/**< Normalize-to level. */
			double														dLoudness = std::pow( 10, -5.5 / 20.0 );		/**< Loudness level. */
			double														dMainsHumVol = 1.0;								/**< Mains Hum volume. */
			double														dWhiteNoiseVol = 1.0;							/**< White Noise volume. */
			double														dWhiteNoiseTemp = 290.0;						/**< White-noise temperature. */
			double														dWhiteNoiseRes = 50.0;							/**< White-noise resistance. */
			std::wstring												wsFolder;										/**< Path to the output folder. */
			uint32_t													ui32Hz = 48000;									/**< The final output Hz. */
			int32_t														i32MainsHumType = LSN_MH_BLACK;					/**< The color of mains hum. */
			int32_t														i32WhiteNoiseType = LSN_WN_GAUSSIAN;			/**< The type of white noise. */

			int32_t														i32VolType = LSN_VT_ABS;						/**< Volume type. */

			int32_t														i32Format = CWavFile::LSN_F_PCM;				/**< Output format. */
			uint16_t													ui16Bits = 32;									/**< PCM bits. */
			int32_t														i32Channels = LSN_C_MONO;						/**< Channel format. */

			bool														bMainsHum = false;								/**< Add Mains Hum? */
			bool														bWhiteNoise = false;							/**< Add White Noise? */

			bool														bDither = true;									/**< Dither 16-but output? */
			bool														bNumbered = false;								/**< Number the output files? */
		};

		/** A metadata entry. */
		struct LSN_METADATA {
			double														dTime;											/**< The time of the event. */
			uint32_t													ui32Idx;										/**< Event ID. */
			std::string													sText;											/**< Even text. */


			LSN_METADATA() = default;
			LSN_METADATA( const LSN_METADATA & ) = default;
			LSN_METADATA( LSN_METADATA && ) noexcept = default;
			LSN_METADATA & operator = ( const LSN_METADATA & ) = default;
			~LSN_METADATA() = default;


			// == Operators.
			LSN_METADATA &												operator = ( LSN_METADATA &&_mdOther ) noexcept {
				dTime = _mdOther.dTime;
				ui32Idx = _mdOther.ui32Idx;
				sText = std::move( _mdOther.sText );
				return (*this);
			}


			// == Functions.
			void														swap( LSN_METADATA &_mdOther ) noexcept {
				std::swap( dTime, _mdOther.dTime );
				std::swap( ui32Idx, _mdOther.ui32Idx );
				std::swap( sText, _mdOther.sText );
			}
		};

		/** A single WAV file. */
		struct LSN_WAV_FILE {
			std::wstring												wsPath;											/**< Path to the file. */
			std::wstring												wsInputPath;									/**< Original input path to the file. */
			CWavFile::LSN_FMT_CHUNK										fcFormat;										/**< The WAV-file format. */
			uint64_t													ui64Samples = 0;								/**< Total samples per channel. */


			LSN_WAV_FILE() = default;
			LSN_WAV_FILE( const LSN_WAV_FILE & ) = default;
			LSN_WAV_FILE( LSN_WAV_FILE && ) noexcept = default;
			LSN_WAV_FILE & operator = ( const LSN_WAV_FILE & ) = default;
			~LSN_WAV_FILE() = default;


			// == Operators.
			LSN_WAV_FILE &												operator = ( LSN_WAV_FILE &&_wfOther ) noexcept {
				fcFormat = _wfOther.fcFormat;
				ui64Samples = _wfOther.ui64Samples;
				wsPath = std::move( _wfOther.wsPath );
				return (*this);
			}


			// == Functions.
			void														swap( LSN_WAV_FILE &_mdOther ) noexcept {
				std::swap( fcFormat, _mdOther.fcFormat );
				std::swap( ui64Samples, _mdOther.ui64Samples );
				std::swap( wsPath, _mdOther.wsPath );
			}
		};

		/** A WAV file. */
		struct LSN_WAV_FILE_SET {
			LSN_WAV_FILE												wfFile;											/**< The file data. */
			std::vector<LSN_WAV_FILE>									vExtensions;									/**< Additional files to append to the main file. */
			std::wstring												wsMetaPath;										/**< Path to the file's metadata. */
			std::vector<LSN_METADATA>									vMetadata;										/**< Associated metadata. */
			uint64_t													ui64FullSampleCnt = 0;							/**< The total samples across all loaded files. */
			uint32_t													ui32Id = 0;										/**< The unique ID assigned to this set. */


			// == Operators.
			LSN_WAV_FILE_SET &											operator = ( LSN_WAV_FILE_SET &&_wfsOther ) noexcept {
				ui32Id = _wfsOther.ui32Id;
				ui64FullSampleCnt = _wfsOther.ui64FullSampleCnt;
				wfFile = std::move( _wfsOther.wfFile );
				vExtensions = std::move( _wfsOther.vExtensions );
				wsMetaPath = std::move( _wfsOther.wsMetaPath );
				vMetadata = std::move( _wfsOther.vMetadata );
				return (*this);
			}
		};


		// == Functions.
		/**
		 * Executes the conversion operations.
		 * 
		 * \param _wsMsg The error message upon failure.
		 * \return Returns true if the operation completes successfully.  If false, _wsMsg contains the reason for failure.
		 **/
		bool															Execute( std::wstring &_wsMsg );

		/**
		 * Adds a WAV file.  Automatically detects file sequences and metadata files.
		 * 
		 * \param _wsPath The path to the original WAV file to load.  Should be the start of a sequence.
		 * \param _wsMetaPath Optional path to metadata.
		 * \return Returns ID of the added file set or 0.
		 **/
		uint32_t														AddWavFileSet( const std::wstring &_wsPath, const std::wstring &_wsMetaPath = L"" );

		/**
		 * Removes a WAV file or Metadata file by ID.
		 * 
		 * \param _ui32Id The unique of the item to remove.
		 **/
		void															RemoveFile( uint32_t _ui32Id );

		/**
		 * Sets the order of the WAV files by ID.
		 * 
		 * \param _vOrder An array of valid ID's in the order to be set internally.
		 * \return Returns true if every unique ID is in the given vector and all ID's are valid and part of this CWavEditor instance.
		 **/
		bool															Order( const std::vector<uint32_t> &_vOrder );

		/**
		 * Sets the parameters for conversion.
		 * 
		 * \param _vpfPerFile The per-file settings.
		 * \param _oOutput The output parameters.
		 * \return Returns true if the settings were copied.
		 **/
		bool															SetParms( const std::vector<CWavEditor::LSN_PER_FILE> &_vpfPerFile, const LSN_OUTPUT &_oOutput ) {
			m_sPerFile = std::set<CWavEditor::LSN_PER_FILE>( _vpfPerFile.begin(), _vpfPerFile.end() );
			m_oOutput = _oOutput;
			return true;
		}

		/**
		 * Gets a pointer to a WAV file given an ID.
		 * 
		 * \param _ui32Id The ID of the WAV-file structure to find.
		 * \return Returns a pointer to the associated WAV-file structure or nullptr.
		 **/
		LSN_WAV_FILE_SET *												WavById( uint32_t _ui32Id ) {
			auto aFound = m_mFileMapping.find( _ui32Id );
			if ( aFound == m_mFileMapping.end() ) { return nullptr; }
			return &aFound->second;
		}

		/**
		 * Gets a pointer to a WAV file given an index.
		 * 
		 * \param _sIdx The index of the WAV-file set.
		 * \return Returns a pointer to the associated WAV-file structure or nullptr.
		 **/
		LSN_WAV_FILE_SET *												WavByIdx( size_t _sIdx ) {
			if LSN_UNLIKELY( _sIdx >= m_vFileList.size() ) { return nullptr; }
			return WavById( m_vFileList[_sIdx] );
		}

		/**
		 * Gets the number of WAV sets loaded.
		 * 
		 * \return Returns the number of WAV sets loaded.
		 **/
		size_t															Total() const { return m_vFileList.size(); }

		/**
		 * Saves the file paths to the given structure in the order they are inside this class.
		 * 
		 * \param _wewoOptions The structure to which to save the file paths.
		 * \return Returns true if all the paths could be copied.  False always indicates a memory error.
		 **/
		bool															SaveToStruct( LSN_WAV_EDITOR_WINDOW_OPTIONS &_wewoOptions );

		/**
		 * Clears the current data and loads from a structure.
		 * 
		 * \param _wewoOptions The structure from which to load WAV/metadata files.
		 * \return Returns true if all files could be loaded.
		 **/
		bool															LoadFromStruct( const LSN_WAV_EDITOR_WINDOW_OPTIONS &_wewoOptions );


	protected :
		// == Members.
		/** The per-file set. */
		std::set<LSN_PER_FILE>											m_sPerFile;
		/** The output state. */
		LSN_OUTPUT														m_oOutput;
		/** The map of ID's to files. */
		std::map<uint32_t, LSN_WAV_FILE_SET>							m_mFileMapping;
		/** The in-order array of WAV files. */
		std::vector<uint32_t>											m_vFileList;

		/** Path ID's. */
		static std::atomic<uint32_t>									m_aPathId;


		// == Functions.
		/**
		 * Opens a WAV file and fills out its data.
		 * 
		 * \param _wsPath The path to the file to load.
		 * \param _wfData The data structure to fill out for the given WAV file.
		 * \return Returns false if the file does not exist or could not be accessed or is not a WAV file.
		 **/
		bool															CreateWavFile( const std::wstring &_wsPath, LSN_WAV_FILE &_wfData );

		/**
		 * Loads and parses a metadata file.
		 * 
		 * \param _wsPath The path to the metadata file.
		 * \param _vResult Stores the result of the parsed metadata file.
		 * \return Returns true if the file was successfully loaded and parsed.
		 **/
		bool															AddMetadata( const std::wstring &_wsPath, std::vector<LSN_METADATA> &_vResult );

		/**
		 * Creates a single file.
		 * 
		 * \param _wfsSet The file set.
		 * \param _pfFile The per-file data settings.
		 * \param _oOutput The output settings.
		 * \param _stIdx The index of the track being exported.
		 * \param _sTotal The total number fo tracks being exported.
		 * \param _wsMsg Error message upon failure.
		 * \param _wsBatFile The contents of a .BAT file for creating videos.
		 * \param _wsMetadata The contents of a .TXT file containing information about each track.
		 * \return Returns true if the file was created.  If false is returned, _wsMsg will be filled with error text.
		 **/
		bool															DoFile( const LSN_WAV_FILE_SET &_wfsSet, const LSN_PER_FILE &_pfFile, const LSN_OUTPUT &_oOutput, size_t &_stIdx, size_t _sTotal, std::wstring &_wsMsg,
			std::wstring &_wsBatFile, std::wstring &_wsMetadata );

		/**
		 * Writes the BAT data to a given file.
		 * 
		 * \param _wsBatPath The file where to write the .BAT data.
		 * \param _wsBatFile The .BAT file contents to write.
		 * \param _wsMsg Error message upon failure.
		 * \return Returns true if there were no failures writing the file.
		 **/
		bool															WriteBat( std::wstring _wsBatPath, const std::wstring &_wsBatFile, std::wstring &_wsMsg ) {
			try {
				if ( !_wsBatPath.size() ) {
					_wsBatPath = m_oOutput.wsFolder;
					while ( _wsBatPath.size() && _wsBatPath[_wsBatPath.size()-1] == L'\\' || _wsBatPath[_wsBatPath.size()-1] == L'/' ) {
						_wsBatPath.pop_back();
					}
					_wsBatPath = std::filesystem::path( _wsBatPath ).filename().generic_wstring();
				}
				_wsBatPath += L".bat";
				std::filesystem::path pBatFile = m_oOutput.wsFolder;
				pBatFile /= _wsBatPath;
				{
					CStdFile sfBatFile;
					if ( !sfBatFile.Create( pBatFile.generic_u16string().c_str() ) ) {
						_wsMsg = std::format( LSN_LSTR( LSN_WE_FAILED_TO_CREATE_BAT ), pBatFile.generic_wstring() );
						return false;
					}
					/*if ( !sfBatFile.Write<uint16_t>( 0xFEFF ) ) {
						_wsMsg = std::format( LSN_LSTR( LSN_WE_FAILED_TO_WRITE_BAT ), pBatFile.generic_wstring() );
						return false;
					}*/
					auto u16Tmp = CUtilities::XStringToU16String( _wsBatFile.data(), _wsBatFile.size() );
					auto sUtf8 = CUtilities::Utf16ToUtf8( u16Tmp.data() );
					for ( size_t I = 0; I < sUtf8.size(); ++I ) {
						if ( !sfBatFile.Write<char>( sUtf8[I] ) ) {
							_wsMsg = std::format( LSN_LSTR( LSN_WE_FAILED_TO_WRITE_BAT ), pBatFile.generic_wstring() );
							return false;
						}
					}
				}
				return true;
			}
			catch ( ... ) {
				_wsMsg = LSN_LSTR( LSN_OUT_OF_MEMORY );
				return false;
			}
		}

		/**
		 * Writes the metadata file to a given path.
		 * 
		 * \param _wsFileName The file where to write the metadata.
		 * \param _wsMetadata The file contents to write.
		 * \param _wsMsg Error message upon failure.
		 * \return Returns true if there were no failures writing the file.
		 **/
		bool															WriteMetadata( std::wstring _wsFileName, const std::wstring &_wsMetadata, std::wstring &_wsMsg ) {
			try {
				std::filesystem::path pfMetaFile = m_oOutput.wsFolder;
				pfMetaFile /= _wsFileName;

				CStdFile sfMetaFile;
				if ( !sfMetaFile.Create( pfMetaFile.generic_u16string().c_str() ) ) {
					_wsMsg = std::format( LSN_LSTR( LSN_WE_FAILED_TO_CREATE_METADATA ), pfMetaFile.generic_wstring() );
					return false;
				}
				if ( !sfMetaFile.Write<uint16_t>( 0xFEFF ) ) {
					_wsMsg = std::format( LSN_LSTR( LSN_WE_FAILED_TO_WRITE_METADATA ), pfMetaFile.generic_wstring() );
					return false;
				}
				for ( size_t I = 0; I < _wsMetadata.size(); ++I ) {
					if ( !sfMetaFile.Write<wchar_t>( _wsMetadata[I] ) ) {
						_wsMsg = std::format( LSN_LSTR( LSN_WE_FAILED_TO_WRITE_METADATA ), pfMetaFile.generic_wstring() );
						return false;
					}
				}
				return true;
			}
			catch ( ... ) {
				_wsMsg = LSN_LSTR( LSN_OUT_OF_MEMORY );
				return false;
			}
		}
	};

}	// namespace lsn

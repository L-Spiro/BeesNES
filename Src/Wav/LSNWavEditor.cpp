/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Processes batches of WAV files to make hardware-accurate recordings, complete with fade-out etc.  Helps to make OST's.
 */
 
#include "LSNWavEditor.h"
#include "../Localization/LSNLocalization.h"
#include "../Utilities/LSNAlignmentAllocator.h"
#include "../Utilities/LSNLargeVector.h"
#include "../Utilities/LSNStream.h"

#include <filesystem>
#include <format>
#include <utility>


namespace lsn {

	/** Path ID's. */
	std::atomic<uint32_t> CWavEditor::m_aPathId = 1;

	CWavEditor::CWavEditor() {
	}
	CWavEditor::~CWavEditor() {
	}

	// == Functions.
	/**
	 * Executes the conversion operations.
	 * 
	 * \param _wsMsg The error message upon failure.
	 * \return Returns true if the operation completes successfully.  If false, _wsMsg contains the reason for failure.
	 **/
	bool CWavEditor::Execute( std::wstring &_wsMsg ) {
		if ( !m_vFileList.size() ) { return true; }																	// Nothing to do.

		try {
			std::filesystem::create_directories( m_oOutput.wsFolder );
		}
		catch ( ... ) {
			_wsMsg = std::format( LSN_LSTR( LSN_WE_FAILED_TO_CREATE_DIR ), m_oOutput.wsFolder );
			return false;
		}

		try {
			size_t sIdx = 0;
			for ( size_t I = 0; I < m_vFileList.size(); ++I ) {
				LSN_PER_FILE pfFindMe;
				pfFindMe.ui32Id = m_vFileList[I];
				auto aPerFile = m_sPerFile.find( pfFindMe );
				auto pwfsSet = WavById( pfFindMe.ui32Id );
				if ( aPerFile == m_sPerFile.end() || !pwfsSet ) {
					_wsMsg = LSN_LSTR( LSN_INTERNAL_ERROR );
					return false;
				}
				if ( !DoFile( (*pwfsSet), (*aPerFile), m_oOutput, sIdx, _wsMsg ) ) { return false; }
			}
		}
		catch ( ... ) {
			_wsMsg = LSN_LSTR( LSN_INTERNAL_ERROR );
			return false;
		}
		return true;
	}

	/**
	 * Adds a WAV file.  Automatically detects file sequences and metadata files.
	 * 
	 * \param _wsPath The path to the original WAV file to load.  Should be the start of a sequence.
	 * \return Returns ID of the added file set or 0.
	 **/
	uint32_t CWavEditor::AddWavFileSet( const std::wstring &_wsPath ) {
		try {
			LSN_WAV_FILE_SET wfsSet;
			if ( !CreateWavFile( _wsPath, wfsSet.wfFile ) ) { return 0; }

			std::filesystem::path pPath = std::filesystem::absolute( std::filesystem::path( _wsPath ) );
			auto aFileName = pPath.filename().replace_extension( "" );
			for ( size_t I = 1; true; ++I ) {
				auto aThisFileName = aFileName;
				aThisFileName += ("." + std::to_string( I ) + ".wav");
				auto pFinalNewPath = pPath;
				pFinalNewPath.replace_filename( aThisFileName );
				std::error_code ecErr;
				if ( std::filesystem::exists( pFinalNewPath, ecErr ) ) {
					LSN_WAV_FILE wfThis;
					if ( !CreateWavFile( pFinalNewPath.generic_wstring(), wfThis ) ) { return 0; }
					wfsSet.vExtensions.push_back( wfThis );
				}
				else { break; }
			}
			auto pMetaPath = pPath;
			pMetaPath += ".txt";
			std::error_code ecErr;
			if ( std::filesystem::exists( pMetaPath, ecErr ) ) {
				wfsSet.wsMetaPath = pMetaPath.generic_wstring();
				if ( !AddMetadata( wfsSet.wsMetaPath, wfsSet.vMetadata ) ) {
					wfsSet.vMetadata.clear();
					wfsSet.wsMetaPath.clear();
				}
			}

			uint32_t uID = 0;
			while ( !uID ) {
				uID = m_aPathId.fetch_add( 1, std::memory_order_relaxed ) & 0xFFFF;
			}
			wfsSet.ui32Id = uID;
			m_mFileMapping[uID] = std::move( wfsSet );
			m_vFileList.push_back( uID );

			return uID;
		}
		catch ( ... ) { return 0; }
	}

	/**
	 * Removes a WAV file or Metadata file by ID.
	 * 
	 * \param _ui32Id The unique of the item to remove.
	 **/
	void CWavEditor::RemoveFile( uint32_t _ui32Id ) {
		auto pwfsSet = WavById( _ui32Id & 0xFFFF );
		if ( pwfsSet ) {
			if ( (_ui32Id & 0xFFFF0000) == 0xFFFF0000 ) {
				// Remove the metadata.
				pwfsSet->wsMetaPath = L"";
			}
			else if ( (_ui32Id & 0x80000000) ) {
				size_t sIdx = (_ui32Id >> 16) & 0x7FFF;
				if ( sIdx < pwfsSet->vExtensions.size() ) {
					pwfsSet->vExtensions.erase( pwfsSet->vExtensions.begin() + sIdx );
				}
			}
			else {
				// erase from the map and vector.
				m_mFileMapping.erase( _ui32Id & 0xFFFF );
				m_vFileList.erase(
					std::remove( m_vFileList.begin(), m_vFileList.end(), _ui32Id & 0xFFFF ),
					m_vFileList.end()
				);
			}
		}
	}

	/**
	 * Moves items up 1 based on their unique ID's.
	 * 
	 * \param _vItems The items to move up one.
	 **/
	void CWavEditor::MoveUp( const std::vector<LPARAM> &_vItems ) {
		auto sSet = std::set<LPARAM>( _vItems.begin(), _vItems.end() );

		int64_t i64Idx = -2;
		int64_t i64ThisIdx = 0;
		for ( size_t I = 0; I < m_vFileList.size(); ++I ) {
			auto ptThis = WavByIdx( I );
			if ( ptThis ) {
				MoveUp( (*ptThis), sSet );
				if ( std::find( sSet.begin(), sSet.end(), ptThis->ui32Id ) != sSet.end() ) {
					// Can we move this one?  If the previous item was just moved or there is nothing before this one, it canft be moved.
					if ( I != 0 && i64ThisIdx - i64Idx > 1 ) {
						std::swap( m_vFileList[I], m_vFileList[I-1] );
					}
					i64Idx = i64ThisIdx;
				}

				++i64ThisIdx;
			}
		}
	}

	/**
	 * Moves items down 1 based on their unique ID's.
	 * 
	 * \param _vItems The items to move down one.
	 **/
	void CWavEditor::MoveDown( const std::vector<LPARAM> &_vItems ) {
		auto sSet = std::set<LPARAM>( _vItems.begin(), _vItems.end() );

		int64_t i64Idx = -2;
		int64_t i64ThisIdx = 0;
		for ( auto I = m_vFileList.size(); I--; ) {
			auto ptThis = WavByIdx( I );
			if ( ptThis ) {
				MoveDown( (*ptThis), sSet );
				if ( std::find( sSet.begin(), sSet.end(), ptThis->ui32Id ) != sSet.end() ) {
					// Can we move this one?  If the previous item was just moved or there is nothing before this one, it canft be moved.
					if ( I != m_vFileList.size() - 1 && i64ThisIdx - i64Idx > 1 ) {
						std::swap( m_vFileList[I], m_vFileList[I+1] );
					}
					i64Idx = i64ThisIdx;
				}

				++i64ThisIdx;
			}
		}
	}

	/**
	 * Opens a WAV file and fills out its data.
	 * 
	 * \param _wsPath The path to the file to load.
	 * \param _wfData The data structure to fill out for the given WAV file.
	 * \return Returns false if the file does not exist or could not be accessed or is not a WAV file.
	 **/
	bool CWavEditor::CreateWavFile( const std::wstring &_wsPath, LSN_WAV_FILE &_wfData ) {
		try {
			std::filesystem::path pPath = std::filesystem::absolute( std::filesystem::path( _wsPath ) );
			CWavFile wfFile;
			if ( !wfFile.Open( pPath.generic_u16string().c_str(), 0 ) ) { return false; }
			_wfData.fcFormat.uiAudioFormat = wfFile.Format();
			_wfData.fcFormat.uiNumChannels = wfFile.Channels();
			_wfData.fcFormat.uiBitsPerSample = wfFile.BitsPerSample();
			_wfData.fcFormat.uiSampleRate = wfFile.Hz();
			_wfData.fcFormat.uiByteRate = uint64_t( _wfData.fcFormat.uiSampleRate ) * _wfData.fcFormat.uiNumChannels * _wfData.fcFormat.uiBitsPerSample / 8;
			_wfData.wsPath = pPath.generic_wstring();
			_wfData.ui64Samples = wfFile.FileSampleCnt();
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Moves up 1 any selected children in the given WAV set.
	 * 
	 * \param _wfsSet The WAV set to modify.
	 * \param _sItems The list of children to move up by 1.
	 **/
	void CWavEditor::MoveUp( LSN_WAV_FILE_SET &_wfsSet, const std::set<LPARAM> &_sItems ) {
		int64_t i64Idx = -2;
		int64_t i64ThisIdx = 0;
		for ( size_t I = 0; I < _wfsSet.vExtensions.size(); ++I ) {
			
			uint32_t ui32ThisId = uint32_t( (_wfsSet.ui32Id & 0xFFFF) | (I << 16) | 0x80000000 );
			if ( std::find( _sItems.begin(), _sItems.end(), ui32ThisId ) != _sItems.end() ) {
				// Can we move this one?  If the previous item was just moved or there is nothing before this one, it canft be moved.
				if ( I != 0 && i64ThisIdx - i64Idx > 1 ) {
					_wfsSet.vExtensions[I].swap( _wfsSet.vExtensions[I-1] );
				}
				i64Idx = i64ThisIdx;
			}

			++i64ThisIdx;
		}
	}

	/**
	 * Moves down 1 any selected children in the given WAV set.
	 * 
	 * \param _wfsSet The WAV set to modify.
	 * \param _sItems The list of children to move down by 1.
	 **/
	void CWavEditor::MoveDown( LSN_WAV_FILE_SET &_wfsSet, const std::set<LPARAM> &_sItems ) {
		int64_t i64Idx = -2;
		int64_t i64ThisIdx = 0;
		for ( size_t I = _wfsSet.vExtensions.size(); I--; ) {
			uint32_t ui32ThisId = uint32_t( (_wfsSet.ui32Id & 0xFFFF) | (I << 16) | 0x80000000 );
			if ( std::find( _sItems.begin(), _sItems.end(), ui32ThisId ) != _sItems.end() ) {
				// Can we move this one?  If the previous item was just moved or there is nothing before this one, it canft be moved.
				if ( I != _wfsSet.vExtensions.size() -1 && i64ThisIdx - i64Idx > 1 ) {
					_wfsSet.vExtensions[I].swap( _wfsSet.vExtensions[I+1] );
				}
				i64Idx = i64ThisIdx;
			}
			++i64ThisIdx;
		}
	}

	/**
	 * Loads and parses a metadata file.
	 * 
	 * \param _wsPath The path to the metadata file.
	 * \param _vResult Stores the result of the parsed metadata file.
	 * \return Returns true if the file was successfully loaded and parsed.
	 **/
	bool CWavEditor::AddMetadata( const std::wstring &_wsPath, std::vector<LSN_METADATA> &_vResult ) {
		try {
			_vResult.clear();
			CStdFile sfFile;
			if ( !sfFile.Open( CUtilities::XStringToU16String( _wsPath.c_str(), _wsPath.size() ).c_str() ) ) { return false; }
		
			std::vector<uint8_t> vFile;
			if ( !sfFile.LoadToMemory( vFile ) ) { return false; }

			CStream sStream( vFile );

			std::string sTmp;
			LSN_METADATA mdMeta;
			double dTime2;
			while ( sStream.ReadLine( sTmp ) ) {
				if ( sTmp.size() >= 511 ) { return false; }
				char szBuffer[512];
				auto iConveretd = std::sscanf(
					sTmp.c_str(),
					"%lf%*[\t ]%lf%*[\t ][%u: %[^]]",
					&mdMeta.dTime, &dTime2, &mdMeta.ui32Idx, szBuffer );
				if ( iConveretd != 4 ) { return false; }
				szBuffer[511] = '\0';
				mdMeta.sText = szBuffer;
				_vResult.push_back( std::move( mdMeta ) );
			}
		}
		catch ( ... ) { return false; }

		return true;
	}

	/**
	 * Creates a single file.
	 * 
	 * \param _wfsSet The file set.
	 * \param _pfFile The per-file data settings.
	 * \param _oOutput The output settings.
	 * \param _stIdx The index of the track being exported.
	 * \param _wsMsg Error message upon failure.
	 * \return Returns true if the file was created.  If false is returned, _wsMsg will be filled with error text.
	 **/
	bool CWavEditor::DoFile( const LSN_WAV_FILE_SET &_wfsSet, const LSN_PER_FILE &_pfFile, const LSN_OUTPUT &_oOutput, size_t &/*_stIdx*/, std::wstring &_wsMsg ) {
		// Determine the sample range to load.
		int64_t i64StartSample = int64_t( std::round( _wfsSet.wfFile.fcFormat.uiSampleRate * _pfFile.dStartTime ) );
		int64_t i64EndSample = int64_t( std::round( _wfsSet.wfFile.fcFormat.uiSampleRate * _pfFile.dEndTime ) );
		std::vector<large_vector<double, CAlignmentAllocator<double, 64>>> vSamples;
		for ( size_t I = 0; I < _wfsSet.wfFile.fcFormat.uiNumChannels; ++I ) {
			try {
				// Keep 32 megabytes in RAM at a time.
				vSamples.push_back( large_vector<double, CAlignmentAllocator<double, 64>>( 32 * 1024 * 1024 / sizeof( double ), 0 ) );
			}
			catch ( ... ) {
				_wsMsg = LSN_LSTR( LSN_OUT_OF_MEMORY );
				return false;
			}
		}
		{
			lsn::CWavFile wfWav;
			if ( uint64_t( i64StartSample ) < _wfsSet.wfFile.ui64Samples ) {
				uint32_t ui32End = uint32_t( std::min<uint64_t>( i64EndSample, _wfsSet.wfFile.ui64Samples ) );
				
				if ( !wfWav.Open( CUtilities::XStringToU16String( _wfsSet.wfFile.wsPath.c_str(), _wfsSet.wfFile.wsPath.size() ).c_str(), lsn::CWavFile::LSN_LF_DATA, uint32_t( i64StartSample ), ui32End ) ) {
					_wsMsg = std::format( LSN_LSTR( LSN_WE_FAILED_TO_LOAD_SAMPLES ), _wfsSet.wfFile.wsPath );
					return false;
				}
				if ( !wfWav.GetAllSamples( vSamples ) ) {
					_wsMsg = LSN_LSTR( LSN_OUT_OF_MEMORY );
					return false;
				}
			}
			i64StartSample -= _wfsSet.wfFile.ui64Samples;
			i64EndSample -= _wfsSet.wfFile.ui64Samples;
			for ( size_t I = 0; I < _wfsSet.vExtensions.size(); ++I ) {
				if ( i64EndSample <= 0 ) { break; }
				int64_t i64Start = std::max( i64StartSample, 0LL );
				if ( uint64_t( i64Start ) < _wfsSet.wfFile.ui64Samples ) {
					uint32_t ui32End = uint32_t( std::min<uint64_t>( i64EndSample, _wfsSet.vExtensions[I].ui64Samples ) );
					lsn::CWavFile wfThisFile;
					if ( !wfThisFile.Open( CUtilities::XStringToU16String( _wfsSet.vExtensions[I].wsPath.c_str(), _wfsSet.vExtensions[I].wsPath.size() ).c_str(), lsn::CWavFile::LSN_LF_DATA, uint32_t( i64Start ), ui32End ) ) {
						_wsMsg = std::format( LSN_LSTR( LSN_WE_FAILED_TO_LOAD_SAMPLES ), _wfsSet.vExtensions[I].wsPath );
						return false;
					}
					if ( wfThisFile.Channels() != vSamples.size() ) {
						_wsMsg = std::format( LSN_LSTR( LSN_WE_CHANNEL_COUNT_ERROR ), _wfsSet.vExtensions[I].wsPath, _wfsSet.wfFile.wsPath );
						return false;
					}
					if ( wfThisFile.Hz() != wfWav.Hz() ) {
						_wsMsg = std::format( LSN_LSTR( LSN_WE_SAMPLE_RATE_ERROR ), _wfsSet.vExtensions[I].wsPath, _wfsSet.wfFile.wsPath );
						return false;
					}
					if ( !wfThisFile.GetAllSamples( vSamples ) ) {
						_wsMsg = LSN_LSTR( LSN_OUT_OF_MEMORY );
						return false;
					}
				}
				i64StartSample -= _wfsSet.vExtensions[I].ui64Samples;
				i64EndSample -= _wfsSet.vExtensions[I].ui64Samples;
			}
		}

		// For each channel.
		for ( size_t J = 0; J < vSamples.size(); ++J ) {
			// Apply anti-aliasing.
		}

		return true;
	}

}	// namespace lsn

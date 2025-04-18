/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Processes batches of WAV files to make hardware-accurate recordings, complete with fade-out etc.  Helps to make OST's.
 */
 
#include "LSNWavEditor.h"

#include <filesystem>


namespace lsn {

	/** Path ID's. */
	std::atomic<uint32_t> CWavEditor::m_aPathId = 1;

	CWavEditor::CWavEditor() {
	}
	CWavEditor::~CWavEditor() {
	}

	// == Functions.
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
			auto pMetaPath = pPath.replace_extension( ".txt" );
			std::error_code ecErr;
			if ( std::filesystem::exists( pMetaPath, ecErr ) ) {
				wfsSet.wsMetaPath = pMetaPath.generic_wstring();
			}

			uint32_t uID = 0;
			while ( !uID ) {
				uID = m_aPathId.fetch_add( 1, std::memory_order_relaxed ) & 0xFFFF;
			}
			wfsSet.ui32Id = uID;
			m_mFileMapping[uID] = wfsSet;
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
					std::swap( _wfsSet.vExtensions[I], _wfsSet.vExtensions[I-1] );
				}
				i64Idx = i64ThisIdx;
			}

			++i64ThisIdx;
		}
	}

}	// namespace lsn

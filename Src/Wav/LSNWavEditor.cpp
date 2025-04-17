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
				uID = m_aPathId.fetch_add( 1, std::memory_order_relaxed );
			}
			m_mFileMapping[uID] = wfsSet;
			m_vFileList.push_back( uID );

			return uID;
		}
		catch ( ... ) { return 0; }
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

}	// namespace lsn

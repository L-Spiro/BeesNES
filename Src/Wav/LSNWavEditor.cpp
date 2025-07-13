/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Processes batches of WAV files to make hardware-accurate recordings, complete with fade-out etc.  Helps to make OST's.
 */
 
#include "LSNWavEditor.h"
#include "../Audio/LSNAudio.h"
#include "../Audio/LSNHpfFilter.h"
#include "../Audio/LSNPoleFilter.h"
#include "../Utilities/LSNAlignmentAllocator.h"
#include "../Utilities/LSNLargeVector.h"
#include "../Utilities/LSNStream.h"

#include <EEExpEval.h>

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
			std::wstring wsMeta, wsBatFile;
			std::wstring wsBatName;
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
				if ( !DoFile( (*pwfsSet), (*aPerFile), m_oOutput, sIdx, m_vFileList.size(), _wsMsg, wsBatFile, wsMeta ) ) { return false; }
				if ( wsBatName.empty() ) {
					if ( aPerFile->wsAlbum.size() ) {
						wsBatName = CUtilities::FixFile( aPerFile->wsAlbum );
					}
				}
				if ( m_oOutput.bNumbered && !wsBatFile.empty() ) {
					WriteBat( L"Tmp.bat", wsBatFile, _wsMsg );
				}
				if ( !wsMeta.empty() ) {
					WriteMetadata( L"Tmp.txt", wsMeta, _wsMsg );
				}
			}

			if ( m_oOutput.bNumbered && !wsBatFile.empty() ) {
				if ( !WriteBat( wsBatName, wsBatFile, _wsMsg ) ) { return false; }
				try {
					auto pDeleteMe = std::filesystem::path( m_oOutput.wsFolder );
					pDeleteMe /= L"Tmp.bat.bat";
					std::filesystem::remove( pDeleteMe );
				}
				catch ( ... ) {}
			}
			if ( !wsMeta.empty() ) {
				if ( !WriteMetadata( L"Tracks.txt", wsMeta, _wsMsg ) ) { return false; }
				try {
					auto pDeleteMe = std::filesystem::path( m_oOutput.wsFolder );
					pDeleteMe /= L"Tmp.txt";
					std::filesystem::remove( pDeleteMe );
				}
				catch ( ... ) {}
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
	 * \param _wsMetaPath Optional path to metadata.
	 * \return Returns ID of the added file set or 0.
	 **/
	uint32_t CWavEditor::AddWavFileSet( const std::wstring &_wsPath, const std::wstring &_wsMetaPath ) {
		try {
			LSN_WAV_FILE_SET wfsSet;
			if ( !CreateWavFile( _wsPath, wfsSet.wfFile ) ) { return 0; }
			wfsSet.ui64FullSampleCnt += wfsSet.wfFile.ui64Samples;
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
					wfsSet.ui64FullSampleCnt += wfThis.ui64Samples;
				}
				else { break; }
			}
			auto pMetaPath = pPath;
			pMetaPath += ".txt";
			if ( _wsMetaPath.size() ) {
				pMetaPath = _wsMetaPath;
			}
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
	 * Sets the order of the WAV files by ID.
	 * 
	 * \param _vOrder An array of valid ID's in the order to be set internally.
	 * \return Returns true if every unique ID is in the given vector and all ID's are valid and part of this CWavEditor instance.
	 **/
	bool CWavEditor::Order( const std::vector<uint32_t> &_vOrder ) {
		try {
			if ( _vOrder.size() != m_vFileList.size() ) { return false; }
			// Correct number of entries.

			{
				auto sSet = std::set<uint32_t>( _vOrder.begin(), _vOrder.end() );
				if ( _vOrder.size() != sSet.size() ) { return false; }
				// No duplicates.
			}

			// Check each ID for being valid.
			for ( auto & I : _vOrder ) {
				auto pwfsSet = WavById( I );
				if ( !pwfsSet ) { return false; }
			}

			// Correct nummber of entries, no duplicate entries, and all entry is valid.
			m_vFileList = _vOrder;
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Saves the file paths to the given structure in the order they are inside this class.
	 * 
	 * \param _wewoOptions The structure to which to save the file paths.
	 * \return Returns true if all the paths could be copied.  False always indicates a memory error.
	 **/
	bool CWavEditor::SaveToStruct( LSN_WAV_EDITOR_WINDOW_OPTIONS &_wewoOptions ) {
		try {
			_wewoOptions.vPerFileOptions.resize( m_vFileList.size() );
			for ( size_t I = 0; I < m_vFileList.size(); ++I ) {
				auto ppfPerFile = WavByIdx( I );
				if ( !ppfPerFile ) { return false; }
				_wewoOptions.vPerFileOptions[I].wsMetaPath = ppfPerFile->wsMetaPath;
				_wewoOptions.vPerFileOptions[I].vWavPaths.resize( 1 + ppfPerFile->vExtensions.size() );
				_wewoOptions.vPerFileOptions[I].vWavInputPaths.resize( 1 + ppfPerFile->vExtensions.size() );
				_wewoOptions.vPerFileOptions[I].vWavPaths[0] = ppfPerFile->wfFile.wsPath;
				_wewoOptions.vPerFileOptions[I].vWavInputPaths[0] = ppfPerFile->wfFile.wsInputPath;
				for ( size_t J = 0; J < ppfPerFile->vExtensions.size(); ++J ) {
					_wewoOptions.vPerFileOptions[I].vWavPaths[J+1] = ppfPerFile->vExtensions[J].wsPath;
					_wewoOptions.vPerFileOptions[I].vWavInputPaths[J+1] = ppfPerFile->vExtensions[J].wsInputPath;
				}
			}
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Clears the current data and loads from a structure.
	 * 
	 * \param _wewoOptions The structure from which to load WAV/metadata files.
	 * \return Returns true if all files could be loaded.
	 **/
	bool CWavEditor::LoadFromStruct( const LSN_WAV_EDITOR_WINDOW_OPTIONS &_wewoOptions ) {
		try {
			m_vFileList.clear();
			m_sPerFile.clear();
			m_mFileMapping.clear();

			for ( size_t I = 0; I < _wewoOptions.vPerFileOptions.size(); ++I ) {
				if ( !_wewoOptions.vPerFileOptions[I].vWavPaths.size() ) { return true; }
				if ( !_wewoOptions.vPerFileOptions[I].vWavInputPaths.size() ) { return true; }


				uint32_t ui32Id = AddWavFileSet( _wewoOptions.vPerFileOptions[I].vWavInputPaths[0], _wewoOptions.vPerFileOptions[I].wsMetaPath );
				if ( !ui32Id ) {
					ui32Id = AddWavFileSet( _wewoOptions.vPerFileOptions[I].vWavPaths[0], _wewoOptions.vPerFileOptions[I].wsMetaPath );
					if ( !ui32Id ) { return false; }
				}
				auto pwfsSet = WavById( ui32Id );
				pwfsSet->wfFile.wsInputPath = _wewoOptions.vPerFileOptions[I].vWavInputPaths[0];

				pwfsSet->vExtensions.resize( _wewoOptions.vPerFileOptions[I].vWavPaths.size() - 1 );
				for ( size_t J = 0; J < pwfsSet->vExtensions.size(); ++J ) {
					if ( !CreateWavFile( _wewoOptions.vPerFileOptions[I].vWavInputPaths[J+1], pwfsSet->vExtensions[J] ) ) {
						if ( !CreateWavFile( _wewoOptions.vPerFileOptions[I].vWavPaths[J+1], pwfsSet->vExtensions[J] ) ) {
							return false;
						}
					}
				}
			}
			return true;
		}
		catch ( ... ) { return false; }
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
			_wfData.wsInputPath = _wsPath;
			_wfData.ui64Samples = wfFile.FileSampleCnt();
			return true;
		}
		catch ( ... ) { return false; }
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
			
			double dTime2;
			while ( sStream.ReadLine( sTmp ) ) {
				if ( sTmp.size() >= 511 ) { return false; }
				LSN_METADATA mdMeta;
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
	 * \param _sTotal The total number fo tracks being exported.
	 * \param _wsMsg Error message upon failure.
	 * \param _wsBatFile The contents of a .BAT file for creating videos.
	 * \param _wsMetadata The contents of a .TXT file containing information about each track.
	 * \return Returns true if the file was created.  If false is returned, _wsMsg will be filled with error text.
	 **/
	bool CWavEditor::DoFile( const LSN_WAV_FILE_SET &_wfsSet, const LSN_PER_FILE &_pfFile, const LSN_OUTPUT &_oOutput, size_t &_stIdx, size_t _sTotal, std::wstring &_wsMsg,
		std::wstring &_wsBatFile, std::wstring &_wsMetadata ) {
		CUtilities::LSN_FEROUNDMODE frmMode( FE_TONEAREST );

		using large_vec = large_vector<double, CAlignmentAllocator<double, 64>>;
		// Determine the sample range to load.
		double dStartTime = _pfFile.dStartTime;
		double dStopTime = _pfFile.dStopTime;
		if ( _pfFile.sstStartMod == LSN_SST_MINUS_ONE ) {
			dStartTime = std::round( _wfsSet.wfFile.fcFormat.uiSampleRate * dStartTime );	// To samples.
			dStartTime = std::max( dStartTime - 1.0, 0.0 );
			dStartTime /= double( _wfsSet.wfFile.fcFormat.uiSampleRate );
		}
		if ( _pfFile.sstStopMod == LSN_SST_MINUS_ONE && !_pfFile.bLoop ) {
			dStopTime = std::round( _wfsSet.wfFile.fcFormat.uiSampleRate * dStopTime );	// To samples.
			dStopTime = dStopTime + 1.0;
			dStopTime /= double( _wfsSet.wfFile.fcFormat.uiSampleRate );
		}
		int64_t i64StartSample = int64_t( std::round( _wfsSet.wfFile.fcFormat.uiSampleRate * dStartTime ) );

		double dAdjustedStopTime = dStopTime * double( _wfsSet.wfFile.fcFormat.uiSampleRate ) / _pfFile.dActualHz;

		double dFileLen = (dStopTime - dStartTime);
		double dLen = (dAdjustedStopTime - dStartTime);
		double dFadeStart = 0.0;
		if ( _pfFile.bLoop ) {
			dFadeStart = (dAdjustedStopTime - dStartTime) + _pfFile.dDelayTime;
			dFileLen += (_pfFile.dDelayTime + _pfFile.dFadeTime) / double( _wfsSet.wfFile.fcFormat.uiSampleRate ) * _pfFile.dActualHz;
			dLen += _pfFile.dDelayTime + _pfFile.dFadeTime;
		}
		size_t sEndSample = size_t( std::round( dFileLen * _wfsSet.wfFile.fcFormat.uiSampleRate ) );

		//int64_t i64EndSample = int64_t( std::round( _wfsSet.wfFile.fcFormat.uiSampleRate * _pfFile.dEndTime ) );
		int64_t i64EndSample = int64_t( sEndSample );
		std::vector<large_vec> vSamples;
		for ( size_t I = 0; I < _wfsSet.wfFile.fcFormat.uiNumChannels; ++I ) {
			try {
				// Keep 32 megabytes in RAM at a time.
				vSamples.push_back( large_vec( 32 * 1024 * 1024 / sizeof( double ), 0 ) );
			}
			catch ( ... ) {
				_wsMsg = LSN_LSTR( LSN_OUT_OF_MEMORY );
				return false;
			}
		}
		uint32_t ui32FileSampleRate = 0;
		{
			lsn::CWavFile wfWav;
			if ( uint64_t( i64StartSample ) < _wfsSet.wfFile.ui64Samples ) {
				uint32_t ui32End = uint32_t( std::min<uint64_t>( i64EndSample + i64StartSample, _wfsSet.wfFile.ui64Samples ) );
				
				if ( !wfWav.Open( CUtilities::XStringToU16String( _wfsSet.wfFile.wsPath.c_str(), _wfsSet.wfFile.wsPath.size() ).c_str(), lsn::CWavFile::LSN_LF_DATA, uint32_t( i64StartSample ), ui32End ) ) {
					_wsMsg = std::format( LSN_LSTR( LSN_WE_FAILED_TO_LOAD_SAMPLES ), _wfsSet.wfFile.wsPath );
					return false;
				}
				if ( !wfWav.GetAllSamples( vSamples ) ) {
					_wsMsg = LSN_LSTR( LSN_OUT_OF_MEMORY );
					return false;
				}
				wfWav.FreeSamples();
				ui32FileSampleRate = wfWav.Hz();
			}
			i64StartSample -= _wfsSet.wfFile.ui64Samples;
			i64EndSample -= _wfsSet.wfFile.ui64Samples;
			for ( size_t I = 0; I < _wfsSet.vExtensions.size(); ++I ) {
				if ( i64EndSample <= 0 ) { break; }
				int64_t i64Start = std::max( i64StartSample, 0LL );
				if ( uint64_t( i64Start ) < _wfsSet.wfFile.ui64Samples ) {
					uint32_t ui32End = uint32_t( std::min<uint64_t>( i64EndSample + i64Start, _wfsSet.wfFile.ui64Samples ) );
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

		dLen += _pfFile.dOpeningSilence;
		dFadeStart += _pfFile.dOpeningSilence;

		double dFileVol = 1.0;
		// For each channel.
		for ( size_t J = 0; J < vSamples.size(); ++J ) {
			vSamples[J].resize( size_t( sEndSample ) );

			if ( _pfFile.sstStartMod == LSN_SST_SNAP && vSamples[J].size() >= 2 ) {
				auto aLeftSample = vSamples[J][0];
				int64_t i64EraseMe = 0;
				for ( int64_t I = 0; I < int64_t( vSamples[J].size() ) && vSamples[J][size_t(I)] == aLeftSample; ++I ) {
					++i64EraseMe;
				}
				--i64EraseMe;
				vSamples[J].erase_front( size_t( i64EraseMe ) );
				dLen = (std::round( dLen * _pfFile.dActualHz ) - double( i64EraseMe )) / _pfFile.dActualHz;
				dFadeStart = (std::round( dFadeStart * _pfFile.dActualHz ) - double( i64EraseMe )) / _pfFile.dActualHz;
			}
			if ( !_pfFile.bLoop ) {
				if ( _pfFile.sstStopMod == LSN_SST_SNAP && vSamples[J].size() >= 2 ) {
					auto aRightSample = vSamples[J][vSamples[J].size()-1];
					int64_t i64EraseMe = 0;
					for ( int64_t I = vSamples[J].size() - 1; I >= 0 && vSamples[J][size_t(I)] == aRightSample; --I ) {
						++i64EraseMe;
					}
					i64EraseMe -= 1;
					// The LPF and HPF's smudge the waveform, so remove fewer samples depending on how many are set.
					int32_t i32FilterCnt = 0;
					if ( _pfFile.bLpf && _pfFile.dLpf < _pfFile.dActualHz / 2.0 ) { ++i32FilterCnt; }
					if ( _pfFile.dHpf0 ) { ++i32FilterCnt; }
					if ( _pfFile.dHpf1 ) { ++i32FilterCnt; }
					if ( _pfFile.dHpf2 ) { ++i32FilterCnt; }
					i64EraseMe -= int64_t( std::round( i32FilterCnt * (_pfFile.dActualHz / _oOutput.ui32Hz) / 2.0 ) );
					if ( i64EraseMe > 0 ) {
						vSamples[J].pop_back( size_t( i64EraseMe ) );
						dLen = _pfFile.dOpeningSilence + (vSamples[J].size() / _pfFile.dActualHz);
					}
				}
			}

			// Apply anti-aliasing.
			large_vec & vThis = vSamples[J];
			try {
				// Add the opening and trailing silences.
				{
					size_t sOpening = size_t( std::round( _pfFile.dActualHz * _pfFile.dOpeningSilence ) );
					if ( sOpening ) {
						std::vector<double> vTmp;
						vTmp.resize( sOpening );
						double dVal = vThis.size() ? vThis[0] : 0.0;
						for ( auto I = vTmp.size(); I--; ) {
							vTmp[I] = dVal;
						}
						vThis.insert( 0, vTmp.data(), vTmp.size() );
					}

					size_t sTrailing = size_t( std::round( _pfFile.dActualHz * _pfFile.dTrailingSilence ) );
					if ( sTrailing ) {
						std::vector<double> vTmp;
						vTmp.resize( sTrailing );
						double dVal = vThis.size() ? vThis[vThis.size()-1] : 0.0;
						for ( auto I = vTmp.size(); I--; ) {
							vTmp[I] = dVal;
						}
						vThis.push_back( vTmp.data(), vTmp.size() );
					}
				}
				{
					//size_t sM = ee::CExpEval::GetSincFilterM( _pfFile.dActualHz, double( _oOutput.ui32Hz ) / 2.0, 1.505149978319905823553881418774835765361785888671875 ) / 2;
					//size_t sM = ee::CExpEval::CalcIdealSincM( _pfFile.dActualHz, 200.0, 4.0 );
					// Apply anti-aliasing.
					size_t sM = 100;
					std::vector<double> vSincFilter = ee::CExpEval::SincFilterLpf( _pfFile.dActualHz, double( _oOutput.ui32Hz ) / 2.0 * 1.0095, sM );
					CUtilities::ApplySincFilterInPlace<large_vec, std::vector<double>>( vThis, vSincFilter, vThis[0], vThis[vThis.size()-1] );
				}

				// Apply the LPF if any.
				if ( _pfFile.bLpf && _pfFile.dLpf < _pfFile.dActualHz / 2.0 && vThis.size() ) {
					CPoleFilter pfLpf;
					pfLpf.CreateLpf( float( _pfFile.dLpf ), float( _pfFile.dActualHz ) );
					// Prime the LPF.
					double dLeft = vThis[0];
					double dLpf = pfLpf.Process( dLeft );
					double dLastLpf = 0.0;
					size_t sCnt = 0;
					while ( dLpf != dLeft ) {
						dLpf = pfLpf.Process( dLeft );
						if ( dLastLpf == dLpf ) {
							if ( ++sCnt == 1024 ) { break; }
						}
						else { sCnt = 0; }
						dLastLpf = dLpf;
					}
					// LPF is primed.
					size_t sTotal = vThis.size();
					for ( size_t I = 0; I < sTotal; ++I ) {
						vThis[I] = pfLpf.Process( vThis[I] );
					}
				}
				

				// Down-sample to OUT*4.
				std::vector<double> vDownSampled;
				size_t sOutHz = _oOutput.ui32Hz * 4;
				if ( vThis.size() ) {
					size_t sSrcMax = vThis.size();
					size_t sNewSize = size_t( std::round( sSrcMax / _pfFile.dActualHz * sOutHz ) );
					vDownSampled.resize( sNewSize );
					double dSampleMe[6];
					double dLeft = vThis[0], dRight = vThis[sSrcMax-1];
					
					for ( size_t I = 0; I < sNewSize; ++I ) {
						double dSample = I / double( sOutHz ) * _pfFile.dActualHz;
						int64_t i64Idx = int64_t( std::floor( dSample ) );
						dSampleMe[0] = (i64Idx - 2) < 0 ? dLeft : vThis[size_t(i64Idx-2)];
						dSampleMe[1] = (i64Idx - 1) < 0 ? dLeft : vThis[size_t(i64Idx-1)];
						dSampleMe[2] = (i64Idx + 0) >= int64_t( sSrcMax ) ? dRight : vThis[size_t(i64Idx+0)];
						dSampleMe[3] = (i64Idx + 1) >= int64_t( sSrcMax ) ? dRight : vThis[size_t(i64Idx+1)];
						dSampleMe[4] = (i64Idx + 2) >= int64_t( sSrcMax ) ? dRight : vThis[size_t(i64Idx+2)];
						dSampleMe[5] = (i64Idx + 3) >= int64_t( sSrcMax ) ? dRight : vThis[size_t(i64Idx+3)];
						vDownSampled[I] = CAudio::Sample_6Point_5thOrder_Hermite_X( dSampleMe, dSample - double( i64Idx ) );
					}
				}

				// Anti-alias again.
				if ( vDownSampled.size() ) {
					size_t sM = 1200;
					std::vector<double> vSincFilter = ee::CExpEval::SincFilterLpf( double( sOutHz ), double( _oOutput.ui32Hz ) / 2.0 * 1.0/*0.9909*/, sM );
					CUtilities::ApplySincFilterInPlace<std::vector<double>, std::vector<double>>( vDownSampled, vSincFilter, vDownSampled[0], vDownSampled[vDownSampled.size()-1] );
				}
				
				// Down-sample to OUT * 2.
				if ( vDownSampled.size() ) {
					std::vector<double> vDownSampled_2;
					size_t sSrcMax = vDownSampled.size();
					size_t sNewSize = size_t( std::round( sSrcMax / double( sOutHz ) * _oOutput.ui32Hz * 2.0 ) );
					vDownSampled_2.resize( sNewSize );
					double dRight = vDownSampled[sSrcMax-1];
					
					for ( size_t I = 0; I < sNewSize; ++I ) {
						size_t sIds = I * 2;
						vDownSampled_2[I] = sIds < sSrcMax ? vDownSampled[sIds] : dRight;
					}
					sOutHz = _oOutput.ui32Hz * 2;
					vDownSampled = std::move( vDownSampled_2 );
				}

				// Anti-alias again.
				if ( vDownSampled.size() ) {
					size_t sM = 1200;
					std::vector<double> vSincFilter = ee::CExpEval::SincFilterLpf( double( sOutHz ), double( _oOutput.ui32Hz ) / 2.0 * 1.0/*0.9909*/, sM );
					CUtilities::ApplySincFilterInPlace<std::vector<double>, std::vector<double>>( vDownSampled, vSincFilter, vDownSampled[0], vDownSampled[vDownSampled.size()-1] );
				}
				
				// Down-sample to OUT.
				if ( vDownSampled.size() ) {
					size_t sSrcMax = vDownSampled.size();
					size_t sNewSize = size_t( std::round( sSrcMax / double( sOutHz ) * _oOutput.ui32Hz ) );
					vThis.resize( sNewSize );
					double dRight = vDownSampled[sSrcMax-1];

					double dLast = 0.0;
					for ( size_t I = 0; I < sNewSize; ++I ) {
						size_t sIds = I * 2;
						dLast = sIds < sSrcMax ? vDownSampled[sIds] : dRight;
						vThis[I] = dLast;
					}

					sOutHz = _oOutput.ui32Hz;
				}


				// Apply HPF 0.
				if ( _pfFile.dHpf0 && vThis.size() ) {
					CHpfFilter hfHpf;
					//hfHpf.CreateHpf( float( _pfFile.dHpf0 ), float( _pfFile.dActualHz ) );
					hfHpf.CreateHpf( float( _pfFile.dHpf0 ), float( sOutHz ) );
					if ( hfHpf.Enabled() ) {
						// Prime the HPF.
						double dLeft = vThis[0];
						while ( std::fabs( hfHpf.Process( dLeft ) ) >= DBL_EPSILON ) {}

						// HPF is primed.
						size_t sTotal = vThis.size();
						for ( size_t I = 0; I < sTotal; ++I ) {
							vThis[I] = hfHpf.Process( vThis[I] );
						}
					}
				}

				// Apply HPF 1.
				if ( _pfFile.dHpf1 && vThis.size() ) {
					CHpfFilter hfHpf;
					//hfHpf.CreateHpf( float( _pfFile.dHpf1 ), float( _pfFile.dActualHz ) );
					hfHpf.CreateHpf( float( _pfFile.dHpf1 ), float( sOutHz ) );
					if ( hfHpf.Enabled() ) {
						// Prime the HPF.
						double dLeft = vThis[0];
						while ( std::fabs( hfHpf.Process( dLeft ) ) >= DBL_EPSILON ) {}

						// HPF is primed.
						size_t sTotal = vThis.size();
						for ( size_t I = 0; I < sTotal; ++I ) {
							vThis[I] = hfHpf.Process( vThis[I] );
						}
					}
				}

				// Sunsoft 5B volume curve.
				if ( _pfFile.bSunsoft5b ) {
					double dRunAvg = 1.0;
					double dTime = 0.03125 * double( sOutHz );
					size_t sTotal = vThis.size();
					for ( size_t I = 0; I < sTotal; ++I ) {
						constexpr double dReNorm = 1.5 * (1.0 / 0.417751808638574306797863755491562187671661376953125);
						//constexpr double dInvReNorm = 1.0 / dReNorm;
						double dAbs = std::abs( vThis[I] );
						if ( dAbs > 0.0 ) {
							auto dThis = dAbs * (dReNorm);
							double dXsqr = dThis * dThis;
							double dX1 = -0.1712609231472015380859375 * dXsqr * dThis - 0.0505211390554904937744140625 * dXsqr + 0.9762413501739501953125 * dThis;
							double dX2 = (1.0 - std::exp( -1.399 * dThis ));
							double dF = std::clamp( (dThis - 0.5) / (1.28 - 0.5), 0.0, 1.0 );
							double dThisScale = (dX1 * (1.0f - dF) + dX2 * dF);
							dThisScale = dThisScale / dThis;
							//dRunAvg = dThisScale;//CUtilities::UpdateRunningAvg( dRunAvg, dThisScale, dTime );
							dRunAvg = std::min( dThisScale, CUtilities::UpdateRunningAvg( dRunAvg, dThisScale, dTime ) );
							vThis[I] *= dRunAvg /** (dInvReNorm) * (dAbs / vThis[I])*/;
//							vThis[I] *= dThisScale /** (dInvReNorm) * (dAbs / vThis[I])*/;
						}
						else {
							dRunAvg = CUtilities::UpdateRunningAvg( dRunAvg, 1.0, dTime );
						}
					}
				}

				// Apply HPF 2.
				if ( _pfFile.dHpf2 && vThis.size() ) {
					CHpfFilter hfHpf;
					//hfHpf.CreateHpf( float( _pfFile.dHpf2 ), float( _pfFile.dActualHz ) );
					hfHpf.CreateHpf( float( _pfFile.dHpf2 ), float( sOutHz ) );
					if ( hfHpf.Enabled() ) {
						// Prime the HPF.
						double dLeft = vThis[0];
						while ( std::fabs( hfHpf.Process( dLeft ) ) >= DBL_EPSILON ) {}

						// HPF is primed.
						size_t sTotal = vThis.size();
						for ( size_t I = 0; I < sTotal; ++I ) {
							vThis[I] = hfHpf.Process( vThis[I] );
						}
					}
				}


				{
					// Apply any fading.
					if ( _pfFile.bLoop && vThis.size() ) {
						size_t sFadeStart = size_t( std::round( dFadeStart * sOutHz ) );
						size_t sFadeEnd = size_t( std::round( (dFadeStart + _pfFile.dFadeTime) * sOutHz ) );
						size_t sMax = vThis.size();

						vDownSampled.resize( sFadeEnd - sFadeStart );
						double dRight = vThis[sMax-1];
						for ( size_t I = 0; I < vDownSampled.size(); ++I ) {
							size_t sIdx = I + sFadeStart;
							vDownSampled[I] = sIdx < sMax ? vThis[sIdx] : dRight;
						}
						if ( sOutHz > 200 && vDownSampled.size() ) {
							size_t sM = 1500;
							std::vector<double> vSincFilter = ee::CExpEval::SincFilterLpf( double( sOutHz ), 100.0, sM );
							CUtilities::ApplySincFilterInPlace<std::vector<double>, std::vector<double>>( vDownSampled, vSincFilter, vDownSampled[0], vDownSampled[vDownSampled.size()-1] );
						}

						for ( size_t I = 0; I < sMax; ++I ) {
							if LSN_LIKELY( I <= sFadeStart ) {
								//vThis[I] = vThis[I];
							}
							else if ( I >= sFadeEnd ) {
								vThis[I] = 0.0;
							}
							else {
								double dFrac = (I - sFadeStart) / double( sFadeEnd - sFadeStart );
								dFrac = CUtilities::StudioFadeOut( dFrac );
								double dSin, dCos;
								//::sincos( dFrac * (EE_PI / 2.0), &dSin, &dCos );
								dCos = dFrac;
								dSin = 1.0 - dFrac;
								vThis[I] = ((vThis[I] * dCos) + (vDownSampled[I-sFadeStart] * dSin)) * dFrac;
								//vThis[I] *= CUtilities::StudioFadeOut( dFrac );
							}
						}
					}
					else {
						// Fade out the trailing silence.
						size_t sTrailStart = size_t( std::round( dLen * sOutHz ) );
						size_t sTrailEnd = size_t( std::round( (dLen + _pfFile.dTrailingSilence) * sOutHz ) );
						size_t sMax = vThis.size();
						for ( size_t I = sTrailStart; I < sMax; ++I ) {
							double dFrac = double( I - sTrailStart ) / double( sTrailEnd - sTrailStart );
							dFrac = std::min( dFrac, 1.0 );
							vThis[I] = vThis[I] * (1.0 - dFrac);
						}
					}
				}

				size_t sMax = vThis.size();
				dFileVol = _pfFile.dVolume;
				if ( _pfFile.bInvert ) {
					dFileVol *= -1.0;
				}
				for ( size_t I = 0; I < sMax; ++I ) {
					vThis[I] *= dFileVol;
				}
			}
			catch ( ... ) {
				_wsMsg = LSN_LSTR( LSN_OUT_OF_MEMORY );
				return false;
			}
		}
		

		switch ( _oOutput.i32Channels ) {
			case 2 : {}			LSN_FALLTHROUGH
			case 3 : {
				// Make stereo.  Only works if we have 1 or 2 channels.
				switch ( vSamples.size() ) {
					case 1 : {
						try {
							vSamples.push_back( large_vec( 32 * 1024 * 1024 / sizeof( double ), 0 ) );
							vSamples[1].resize( vSamples[0].size() );
							size_t sSamples = vSamples[0].size();
							for ( size_t I = 0; I < sSamples; ++I ) {
								vSamples[1][I] = vSamples[0][I];
							}
						}
						catch ( ... ) {
							_wsMsg = LSN_LSTR( LSN_OUT_OF_MEMORY );
							return false;
						}
						break;
					}
					case 2 : { break; }
					default : {
						_wsMsg = LSN_LSTR( LSN_WE_BAD_CHANNEL_MIX );
						return false;
					}
				}
				if ( _oOutput.i32Channels == 3 ) {
					// Surround.  Invert the right side.
					size_t sSamples = vSamples[1].size();
					for ( size_t I = 0; I < sSamples; ++I ) {
						vSamples[1][I] = -vSamples[1][I];
					}
				}
				break;
			}
			default : {
				// Anything else is a mono mix-down.
				if ( vSamples.size() > 1 ) {
					size_t sSamples = vSamples[0].size();
					double dChans = double( vSamples.size() );
					for ( size_t I = 0; I < sSamples; ++I ) {
						double dThis = vSamples[0][I];
						for ( size_t J = 1; J < vSamples.size(); ++J ) {
							dThis += vSamples[J][I];
						}
						vSamples[0][I] = dThis / dChans;
					}
					while ( vSamples.size() != 1 ) { vSamples.pop_back(); }
					//vSamples.resize( 1 );
				}
			}
		}

		
		// Apply volume.
		double dApplyVol = 1.0;
		switch ( _oOutput.i32VolType ) {
			case LSN_VT_ABS : {
				dApplyVol = _oOutput.dAbsoluteVol;
				break;
			}
			case LSN_VT_NORM : {
				double dMax = 0.0;
				for ( auto J = vSamples.size(); J--; ) {
					double dChannelMax = ee::CExpEval::MaxVec<large_vec>( vSamples[J] );

					dMax = std::max( dMax, dChannelMax );
				}
				if ( dMax != 0.0 ) {
					dApplyVol = 1.0 / dMax * _oOutput.dNormalizeTo;
				}
				break;
			}
			case LSN_VT_LOUDNESS : {
				double dMax = 0.0;
				for ( auto J = vSamples.size(); J--; ) {
					double dChannelMax = ee::CExpEval::CalcRmsGated<large_vec>( vSamples[J], 0.1 );

					dMax = std::max( dMax, dChannelMax );
				}
				if ( dMax != 0.0 ) {
					dApplyVol = 1.0 / dMax * _oOutput.dLoudness;
				}
				break;
			}
		}
		for ( auto J = vSamples.size(); J--; ) {
			for ( auto I = vSamples[J].size(); I--; ) {
				vSamples[J][I] *= dApplyVol;
			}
		}

		// Create a file path.
		std::filesystem::path pPath( _oOutput.wsFolder );
		std::wstring wsFileName = CUtilities::FixFile( _pfFile.wsFilePrefix + _pfFile.wsName + _pfFile.wsFilePostFix + L".wav" );
		if ( _oOutput.bNumbered ) {
			size_t sDigits = size_t( std::ceil( std::log10( _sTotal + 1 ) ) );
			std::ostringstream ossTmp;
			ossTmp << std::setw( sDigits ) << std::setfill( '0' ) << (_stIdx + 1) << " ";
			pPath /= ossTmp.str();
			pPath += wsFileName;
		}
		else {
			pPath /= wsFileName;
		}

		CWavFile wfFile;
		wfFile.AddListEntry( CWavFile::LSN_M_INAM, CUtilities::Utf16ToUtf8( CUtilities::XStringToU16String( _pfFile.wsName.c_str(), _pfFile.wsName.size() ).c_str() ) );
		if ( _pfFile.wsArtist.size() ) {
			wfFile.AddListEntry( CWavFile::LSN_M_IART, CUtilities::Utf16ToUtf8( CUtilities::XStringToU16String( _pfFile.wsArtist.c_str(), _pfFile.wsArtist.size() ).c_str() ) );
		}
		if ( _pfFile.wsAlbum.size() ) {
			wfFile.AddListEntry( CWavFile::LSN_M_IPRD, CUtilities::Utf16ToUtf8( CUtilities::XStringToU16String( _pfFile.wsAlbum.c_str(), _pfFile.wsAlbum.size() ).c_str() ) );
		}
		if ( _pfFile.wsYear.size() ) {
			wfFile.AddListEntry( CWavFile::LSN_M_ICRD, CUtilities::Utf16ToUtf8( CUtilities::XStringToU16String( _pfFile.wsYear.c_str(), _pfFile.wsYear.size() ).c_str() ) );
		}
		if ( _pfFile.wsComment.size() ) {
			wfFile.AddListEntry( CWavFile::LSN_M_ICMT, CUtilities::Utf16ToUtf8( CUtilities::XStringToU16String( _pfFile.wsComment.c_str(), _pfFile.wsComment.size() ).c_str() ) );
		}
		if ( _oOutput.bNumbered ) {
			auto sNumber = std::to_string( _stIdx + 1 );
			wfFile.AddListEntry( CWavFile::LSN_M_ITRK, CUtilities::XStringToU8String( sNumber.c_str(), sNumber.size() ) );
		}

		CWavFile::LSN_SAVE_DATA sdData;
		sdData.fFormat = static_cast<CWavFile::LSN_FORMAT>(_oOutput.i32Format);
		sdData.uiBitsPerSample = _oOutput.ui16Bits;
		sdData.uiHz = _oOutput.ui32Hz;
		sdData.bDither = _oOutput.bDither;
		if ( !wfFile.SaveAsPcm( pPath.generic_u8string().c_str(), vSamples, &sdData ) ) {
			_wsMsg = std::format( LSN_LSTR( LSN_WE_FAILED_TO_SAVE_WAV ), _wfsSet.wfFile.wsPath );
			return false;
		}
		++_stIdx;


		{
			pPath = CUtilities::Replace( pPath.generic_u16string(), std::u16string( u"\"" ), std::u16string( u"\\\"" ) );
			std::filesystem::path pImage = pPath;
			pImage.replace_extension( ".png" );
			std::filesystem::path pMovie = pPath;
			pMovie.replace_extension( " HD.avi" );

			// Remove the number part from the movie name and replace it with the album name.
			std::wstring wsOldStem	= pMovie.stem().wstring();
			size_t sIdx = 0;
			while ( sIdx < wsOldStem.size() && std::iswdigit( wsOldStem[sIdx] ) ) { ++sIdx; }

			std::wstring wsNewStem	= _pfFile.wsAlbum + wsOldStem.substr( sIdx );
			auto pNewFilename		= std::filesystem::path( wsNewStem + pMovie.extension().wstring() );
			pMovie					= pMovie.parent_path() / pNewFilename;


			_wsBatFile += std::format( L"ffmpeg -r 1 -f image2 -loop 1 -i \"{}\" -i \"{}\" -vcodec mpeg4 -qscale:v 1 -acodec copy -shortest \"{}\"\r\n",
				pImage.generic_wstring(), pPath.generic_wstring(), pMovie.generic_wstring() );
		}
		
		{
			std::filesystem::path pImage = wsFileName;
			_wsMetadata += std::format( L"{}\r\n{}\r\nVolumes (Track, Global, Final): {:.27g} ({}{:.27f} dB), {:.27g} ({}{:.27f} dB), {:.27g} ({}{:.27f} dB)\r\nTrack Start/Stop: {:.27f} - {:.27f} ({} - {}) ({:.27f} seconds, {} samples)\r\n",
				pImage.replace_extension( "" ).generic_wstring(),
				_pfFile.wsName,

				dFileVol, (dFileVol < 0.0 ? L"Invert " : L""), std::log10( std::abs( dFileVol ) ) * 20.0,
				dApplyVol, (dApplyVol < 0.0 ? L"Invert " : L""), std::log10( std::abs( dApplyVol ) ) * 20.0,
				dFileVol * dApplyVol, (dFileVol * dApplyVol < 0.0 ? L"Invert " : L""), std::log10( std::abs( dFileVol * dApplyVol ) ) * 20.0,

				(_pfFile.dOpeningSilence * _oOutput.ui32Hz) / _oOutput.ui32Hz, (dLen * _oOutput.ui32Hz) / _oOutput.ui32Hz,
				uint64_t( std::round( _pfFile.dOpeningSilence * _oOutput.ui32Hz ) ), uint64_t( std::round( dLen * _oOutput.ui32Hz ) ),
				((dLen * _oOutput.ui32Hz) / _oOutput.ui32Hz) - ((_pfFile.dOpeningSilence * _oOutput.ui32Hz) / _oOutput.ui32Hz), uint64_t( std::round( dLen * _oOutput.ui32Hz ) ) - uint64_t( std::round( _pfFile.dOpeningSilence * _oOutput.ui32Hz ) ) );
			if ( _pfFile.bLoop ) {
				_wsMetadata += std::format( L"Prefade Start: {:.27f} ({})\r\nFade Start: {:.27f} ({})\r\n",
					(((dAdjustedStopTime - dStartTime) + _pfFile.dOpeningSilence) * _oOutput.ui32Hz) / _oOutput.ui32Hz, uint64_t( std::round( ((dAdjustedStopTime - dStartTime) + _pfFile.dOpeningSilence) * _oOutput.ui32Hz ) ),
					(dFadeStart * _oOutput.ui32Hz) / _oOutput.ui32Hz, uint64_t( std::round( dFadeStart * _oOutput.ui32Hz ) ) );
			}
			_wsMetadata += L"\r\n\r\n";
		}
		return true;
	}

}	// namespace lsn

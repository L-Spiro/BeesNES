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
#include "../Localization/LSNLocalization.h"
#include "../Utilities/LSNAlignmentAllocator.h"
#include "../Utilities/LSNLargeVector.h"
#include "../Utilities/LSNStream.h"

#include <EEExpEval.h>

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
				if ( !DoFile( (*pwfsSet), (*aPerFile), m_oOutput, sIdx, m_vFileList.size(), _wsMsg ) ) { return false; }
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
	 * \return Returns true if the file was created.  If false is returned, _wsMsg will be filled with error text.
	 **/
	bool CWavEditor::DoFile( const LSN_WAV_FILE_SET &_wfsSet, const LSN_PER_FILE &_pfFile, const LSN_OUTPUT &_oOutput, size_t &_stIdx, size_t _sTotal, std::wstring &_wsMsg ) {
		using large_vec = large_vector<double, CAlignmentAllocator<double, 64>>;
		// Determine the sample range to load.
		int64_t i64StartSample = int64_t( std::round( _wfsSet.wfFile.fcFormat.uiSampleRate * _pfFile.dStartTime ) );
		int64_t i64EndSample = int64_t( std::round( _wfsSet.wfFile.fcFormat.uiSampleRate * _pfFile.dEndTime ) );
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
				uint32_t ui32End = uint32_t( std::min<uint64_t>( i64EndSample, _wfsSet.wfFile.ui64Samples ) );
				
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

		double dLen = (_pfFile.dStopTime - _pfFile.dStartTime);
		double dFadeStart = 0.0;
		if ( _pfFile.bLoop ) {
			dFadeStart = dLen + _pfFile.dDelayTime;
			dLen += _pfFile.dDelayTime + _pfFile.dFadeTime;
		}
		size_t sEndSample = size_t( std::round( dLen * ui32FileSampleRate ) );
		dLen += _pfFile.dOpeningSilence;
		dFadeStart += _pfFile.dOpeningSilence;
		//dLen += _pfFile.dTrailingSilence;
		

		//std::vector<std::vector<double>> vFinalTracks;
		//vFinalTracks.resize( vSamples.size() );
		// For each channel.
		for ( size_t J = 0; J < vSamples.size(); ++J ) {
			vSamples[J].resize( sEndSample );
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
					size_t sM = 300;
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
				// Apply HPF 0.
				if ( _pfFile.dHpf0 && vThis.size() ) {
					CHpfFilter hfHpf;
					hfHpf.CreateHpf( float( _pfFile.bHpf0 ), float( _pfFile.dActualHz ) );
					if ( hfHpf.Enabled() ) {
						// Prime the HPF.
						double dLeft = vThis[0];
						while ( hfHpf.Process( dLeft ) >= DBL_EPSILON ) {}

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
					hfHpf.CreateHpf( float( _pfFile.bHpf1 ), float( _pfFile.dActualHz ) );
					if ( hfHpf.Enabled() ) {
						// Prime the HPF.
						double dLeft = vThis[0];
						while ( hfHpf.Process( dLeft ) >= DBL_EPSILON ) {}

						// HPF is primed.
						size_t sTotal = vThis.size();
						for ( size_t I = 0; I < sTotal; ++I ) {
							vThis[I] = hfHpf.Process( vThis[I] );
						}
					}
				}
				// Apply HPF 2.
				if ( _pfFile.dHpf2 && vThis.size() ) {
					CHpfFilter hfHpf;
					hfHpf.CreateHpf( float( _pfFile.bHpf2 ), float( _pfFile.dActualHz ) );
					if ( hfHpf.Enabled() ) {
						// Prime the HPF.
						double dLeft = vThis[0];
						while ( hfHpf.Process( dLeft ) >= DBL_EPSILON ) {}

						// HPF is primed.
						size_t sTotal = vThis.size();
						for ( size_t I = 0; I < sTotal; ++I ) {
							vThis[I] = hfHpf.Process( vThis[I] );
						}
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
						dSampleMe[0] = (i64Idx - 2) < 0 ? dLeft : vThis[(i64Idx-2)];
						dSampleMe[1] = (i64Idx - 1) < 0 ? dLeft : vThis[(i64Idx-1)];
						dSampleMe[2] = (i64Idx + 0) >= int64_t( sSrcMax ) ? dRight : vThis[(i64Idx+0)];
						dSampleMe[3] = (i64Idx + 1) >= int64_t( sSrcMax ) ? dRight : vThis[(i64Idx+1)];
						dSampleMe[4] = (i64Idx + 2) >= int64_t( sSrcMax ) ? dRight : vThis[(i64Idx+2)];
						dSampleMe[5] = (i64Idx + 3) >= int64_t( sSrcMax ) ? dRight : vThis[(i64Idx+3)];
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
				
				// Down-sample to OUT and apply opening and trailing silence.
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
						/*if ( vThis.size() > sTrailEnd ) {
							
						}*/
						for ( size_t I = sTrailStart; I < sMax; ++I ) {
							double dFrac = double( I - sTrailStart ) / double( sTrailEnd - sTrailStart );
							dFrac = std::min( dFrac, 1.0 );
							vThis[I] = vThis[I] * (1.0 - dFrac);
						}
					}
				}

				size_t sMax = vThis.size();
				double dVol = _pfFile.dVolume;
				if ( _pfFile.bInvert ) {
					dVol *= -1.0;
				}
				for ( size_t I = 0; I < sMax; ++I ) {
					vThis[I] *= dVol;
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
			}
			default : {
				// Anything else is a ono mix-down.
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
		switch ( _oOutput.i32VolType ) {
			case LSN_VT_ABS : {
				for ( auto J = vSamples.size(); J--; ) {
					for ( auto I = vSamples[J].size(); I--; ) {
						vSamples[J][I] *= _oOutput.dAbsoluteVol;
					}
				}
				break;
			}
			case LSN_VT_NORM : {
				double dMax = 0.0;
				for ( auto J = vSamples.size(); J--; ) {
					double dChannelMax = ee::CExpEval::MaxVec<large_vec>( vSamples[J] );

					dMax = std::max( dMax, dChannelMax );
				}
				if ( dMax != 0.0 ) {
					for ( auto J = vSamples.size(); J--; ) {
						for ( auto I = vSamples[J].size(); I--; ) {
							vSamples[J][I] = vSamples[J][I] / dMax * _oOutput.dNormalizeTo;
						}
					}
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
					for ( auto J = vSamples.size(); J--; ) {
						for ( auto I = vSamples[J].size(); I--; ) {
							vSamples[J][I] = vSamples[J][I] / dMax * _oOutput.dLoudness;
						}
					}
				}
				break;
			}
		}

		// Create a file path.
		std::filesystem::path pPath( _oOutput.wsFolder );
		if ( _oOutput.bNumbered ) {
			size_t sDigits = size_t( std::ceil( std::log10( _sTotal + 1 ) ) );
			std::ostringstream ossTmp;
			ossTmp << std::setw( sDigits ) << std::setfill( '0' ) << (_stIdx + 1) << " ";
			pPath /= ossTmp.str();
			pPath += (_pfFile.wsName + L".wav");
		}
		else {
			pPath /= (_pfFile.wsName + L".wav");
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
		if ( !wfFile.SaveAsPcm( pPath.generic_u8string().c_str(), vSamples, &sdData ) ) {
			_wsMsg = std::format( LSN_LSTR( LSN_WE_FAILED_TO_SAVE_WAV ), _wfsSet.wfFile.wsPath );
			return false;
		}
		++_stIdx;
		return true;
	}

}	// namespace lsn

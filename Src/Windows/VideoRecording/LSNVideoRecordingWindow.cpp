#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The Video Recording Settings dialog.
 */

#include "LSNVideoRecordingWindow.h"
#include "../../Localization/LSNLocalization.h"
#include "../../Utilities/LSNUtilities.h"
#include "../../Video/LSNFfmpegProcess.h"
#include "../Layout/LSNLayoutMacros.h"

#include <CheckButton/LSWCheckButton.h>
#include <ComboBox/LSWComboBox.h>
#include <TrackBar/LSWTrackBar.h>

#include <cmath>
#include <commdlg.h>
#include <filesystem>
#include <format>


namespace lsn {

	CVideoRecordingWindow::CVideoRecordingWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pvrdData( reinterpret_cast<CVideoRecordingWindowLayout::LSN_VIDEO_RECORDING_DATA *>(_ui64Data) ) {
		if ( m_pvrdData && m_pvrdData->pveoOptions ) {
			m_veoWorking = (*m_pvrdData->pveoOptions);
		}
	}

	// == Functions.
	/**
	 * The WM_INITDIALOG handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CVideoRecordingWindow::InitDialog() {
		Parent::InitDialog();
		if ( !m_pvrdData || !m_pvrdData->pveoOptions ) { return LSW_H_CONTINUE; }
		m_bUpdating = true;

		// Output.
		SetText( Layout::LSN_VRWI_FOLDER_EDIT, m_veoWorking.wsOutputFolder );
		CWidget * pwWidget = FindChild( Layout::LSN_VRWI_FOLDER_EDIT );
		if ( pwWidget ) { ::SendMessageW( pwWidget->Wnd(), EM_SETCUEBANNER, FALSE, reinterpret_cast<LPARAM>(m_pvrdData->wsDefaultFolder.c_str()) ); }
		SetText( Layout::LSN_VRWI_FFMPEG_EDIT, m_veoWorking.wsFfmpegPath );
		pwWidget = FindChild( Layout::LSN_VRWI_FFMPEG_EDIT );
		if ( pwWidget ) { ::SendMessageW( pwWidget->Wnd(), EM_SETCUEBANNER, FALSE, reinterpret_cast<LPARAM>(LSN_LSTR( LSN_VR_FFMPEG_CUE )) ); }

		// Video formats.
		lsw::CComboBox * pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_VRWI_FORMAT_COMBO ));
		if ( pcbCombo ) {
			for ( uint32_t I = 0; I < LSN_VF_TOTAL; ++I ) {
				INT iIdx = pcbCombo->AddString( CVideoFormat::ToStrPWStr( LSN_VIDEO_FORMAT( I ) ) );
				if ( iIdx >= 0 ) { pcbCombo->SetItemData( iIdx, LPARAM( I ) ); }
			}
			if ( m_veoWorking.vfFormat >= LSN_VF_TOTAL ) { m_veoWorking.vfFormat = LSN_VF_H264; }
			pcbCombo->SetCurSelByItemData( LPARAM( m_veoWorking.vfFormat ) );
			pcbCombo->AutoSetMinListWidth();
		}
		SetBoxChecked( Layout::LSN_VRWI_BLEND_CHECK, m_veoWorking.bInterpolateFrames );
		pwWidget = FindChild( Layout::LSN_VRWI_FPS_EDIT );
		if ( pwWidget ) {
			try {
				static std::wstring wsCue = std::format( LSN_LSTR( LSN_VR_FPS_CUE ), L"60", L"{60000,1001}" );
				::SendMessageW( pwWidget->Wnd(), EM_SETCUEBANNER, FALSE, reinterpret_cast<LPARAM>(wsCue.c_str()) );
			}
			catch ( ... ) {}
		}

		// Conditions.
		pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_VRWI_START_COND_COMBO ));
		if ( pcbCombo ) {
			const CWinUtilities::LSN_COMBO_ENTRY ceEntries[] = {
				{ LSN_LSTR( LSN_VR_NONE ),					LSN_VIDEO_EXPORT_OPTIONS::LSN_VSC_NONE },
				{ LSN_LSTR( LSN_VR_START_AT_FRAME ),		LSN_VIDEO_EXPORT_OPTIONS::LSN_VSC_START_AT_FRAME },
			};
			CWinUtilities::FillComboBox( pcbCombo, ceEntries, std::size( ceEntries ), m_veoWorking.scStart, LSN_VIDEO_EXPORT_OPTIONS::LSN_VSC_NONE );
		}
		pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_VRWI_END_COND_COMBO ));
		if ( pcbCombo ) {
			const CWinUtilities::LSN_COMBO_ENTRY ceEntries[] = {
				{ LSN_LSTR( LSN_VR_NONE ),					LSN_VIDEO_EXPORT_OPTIONS::LSN_VEC_NONE },
				{ LSN_LSTR( LSN_VR_END_AT_FRAME ),			LSN_VIDEO_EXPORT_OPTIONS::LSN_VEC_END_AT_FRAME },
				{ LSN_LSTR( LSN_VR_DURATION_SECONDS ),		LSN_VIDEO_EXPORT_OPTIONS::LSN_VEC_DURATION },
			};
			CWinUtilities::FillComboBox( pcbCombo, ceEntries, std::size( ceEntries ), m_veoWorking.ecEnd, LSN_VIDEO_EXPORT_OPTIONS::LSN_VEC_NONE );
		}
		pwWidget = FindChild( Layout::LSN_VRWI_START_COMBO );
		if ( pwWidget ) {
			CWinUtilities::FillComboWithStrings( pwWidget, m_veoWorking.vStartHistory, 0 );
			if ( m_veoWorking.vStartHistory.empty() && m_veoWorking.scStart == LSN_VIDEO_EXPORT_OPTIONS::LSN_VSC_START_AT_FRAME ) {
				SetText( Layout::LSN_VRWI_START_COMBO, std::to_wstring( m_veoWorking.ui64StartFrame ) );
			}
		}
		pwWidget = FindChild( Layout::LSN_VRWI_END_COMBO );
		if ( pwWidget ) {
			CWinUtilities::FillComboWithStrings( pwWidget, m_veoWorking.vEndHistory, 0 );
			if ( m_veoWorking.vEndHistory.empty() ) {
				if ( m_veoWorking.ecEnd == LSN_VIDEO_EXPORT_OPTIONS::LSN_VEC_END_AT_FRAME ) {
					SetText( Layout::LSN_VRWI_END_COMBO, std::to_wstring( m_veoWorking.ui64EndFrame ) );
				}
				else if ( m_veoWorking.ecEnd == LSN_VIDEO_EXPORT_OPTIONS::LSN_VEC_DURATION ) {
					try {
						SetText( Layout::LSN_VRWI_END_COMBO, std::format( L"{}", m_veoWorking.dEndSeconds ) );
					}
					catch ( ... ) {}
				}
			}
		}

		m_bUpdating = false;
		m_vfShown = m_veoWorking.vfFormat;
		LoadFormat( m_vfShown );
		return LSW_H_CONTINUE;
	}

	/**
	 * The WM_CLOSE handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CVideoRecordingWindow::Close() {
		::EndDialog( Wnd(), 0 );
		return LSW_H_HANDLED;
	}

	/**
	 * Handles the WM_COMMAND message.
	 *
	 * \param _wCtrlCode 0 = from menu, 1 = from accelerator, otherwise it is a Control-defined notification code.
	 * \param _wId The ID of the control if _wCtrlCode is not 0 or 1.
	 * \param _pwSrc The source control if _wCtrlCode is not 0 or 1.
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CVideoRecordingWindow::Command( WORD _wCtrlCode, WORD _wId, CWidget * /*_pwSrc*/ ) {
		if ( m_bUpdating ) { return LSW_H_CONTINUE; }
		switch ( _wId ) {
			case Layout::LSN_VRWI_OK : {
				SaveAndClose();
				return LSW_H_HANDLED;
			}
			case Layout::LSN_VRWI_CANCEL : {
				return Close();
			}
			case Layout::LSN_VRWI_DEFAULTS : {
				m_veoWorking.foFormat[m_vfShown] = LSN_VIDEO_EXPORT_OPTIONS::LSN_FORMAT_OPTIONS::Defaults( m_vfShown );
				m_wsCustomFps.clear();
				LoadFormat( m_vfShown );
				return LSW_H_HANDLED;
			}
			case Layout::LSN_VRWI_FORMAT_COMBO : {
				if ( _wCtrlCode == CBN_SELCHANGE ) {
					LSN_VIDEO_FORMAT vfNew = SelectedFormat();
					if ( vfNew != m_vfShown ) {
						// Keep the settings of the format being left.  They must be valid first.
						std::wstring wsError;
						CWidget * pwError = ReadFormat( m_vfShown, wsError );
						if ( pwError ) {
							m_bUpdating = true;
							lsw::CComboBox * pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_VRWI_FORMAT_COMBO ));
							if ( pcbCombo ) { pcbCombo->SetCurSelByItemData( LPARAM( m_vfShown ) ); }
							m_bUpdating = false;
							lsw::CBase::MessageBoxError( Wnd(), wsError.c_str(), LSN_LSTR( LSN_ERROR ) );
							pwError->SetFocus();
							return LSW_H_HANDLED;
						}
						m_vfShown = vfNew;
						m_veoWorking.vfFormat = vfNew;
						m_wsCustomFps.clear();
						LoadFormat( m_vfShown );
					}
				}
				break;
			}
			case Layout::LSN_VRWI_CODEC_COMBO : {
				if ( _wCtrlCode == CBN_SELCHANGE ) {
					// Keep the bitrate if the new codec supports it.
					int32_t i32Bitrate = 0;
					const LSN_VIDEO_AUDIO_CODEC_DATA * pvacdData = CVideoFormat::AudioCodecData( SelectedCodec() );
					if ( pvacdData && !ReadRangedValue( Layout::LSN_VRWI_BITRATE_EDIT, int32_t( (*pvacdData).ui32MinBitrate ), int32_t( (*pvacdData).ui32MaxBitrate ), i32Bitrate ) ) {
						i32Bitrate = 0;
					}
					LoadBitrate( SelectedCodec(), uint32_t( i32Bitrate ) );
				}
				break;
			}
			case Layout::LSN_VRWI_QUALITY_EDIT : {
				if ( _wCtrlCode == EN_CHANGE ) {
					const LSN_VIDEO_FORMAT_DATA * pvfdData = CVideoFormat::Data( m_vfShown );
					if ( pvfdData ) {
						TrackBarFromEdit( Layout::LSN_VRWI_QUALITY_TRACKBAR, Layout::LSN_VRWI_QUALITY_EDIT,
							(*pvfdData).vqdQuality.i32Min, (*pvfdData).vqdQuality.i32Max, (*pvfdData).vqdQuality.i32Step );
					}
				}
				break;
			}
			case Layout::LSN_VRWI_ALT_QUALITY_EDIT : {
				if ( _wCtrlCode == EN_CHANGE ) {
					const LSN_VIDEO_FORMAT_DATA * pvfdData = CVideoFormat::Data( m_vfShown );
					if ( pvfdData ) {
						TrackBarFromEdit( Layout::LSN_VRWI_ALT_QUALITY_TRACKBAR, Layout::LSN_VRWI_ALT_QUALITY_EDIT,
							(*pvfdData).vqdAltQuality.i32Min, (*pvfdData).vqdAltQuality.i32Max, (*pvfdData).vqdAltQuality.i32Step );
					}
				}
				break;
			}
			case Layout::LSN_VRWI_BITRATE_EDIT : {
				if ( _wCtrlCode == EN_CHANGE ) {
					const LSN_VIDEO_AUDIO_CODEC_DATA * pvacdData = CVideoFormat::AudioCodecData( SelectedCodec() );
					if ( pvacdData && !(*pvacdData).bLossless ) {
						TrackBarFromEdit( Layout::LSN_VRWI_BITRATE_TRACKBAR, Layout::LSN_VRWI_BITRATE_EDIT,
							int32_t( (*pvacdData).ui32MinBitrate ), int32_t( (*pvacdData).ui32MaxBitrate ), 1 );
					}
				}
				break;
			}
			case Layout::LSN_VRWI_MATCH_CONSOLE_CHECK : {
				if ( _wCtrlCode == BN_CLICKED ) {
					if ( IsBoxChecked( Layout::LSN_VRWI_MATCH_CONSOLE_CHECK ) ) {
						// Show the console's rate as an example of how to enter a fraction.
						CWidget * pwEdit = FindChild( Layout::LSN_VRWI_FPS_EDIT );
						if ( pwEdit ) {
							try {
								m_wsCustomFps = pwEdit->GetTextW();
							}
							catch ( ... ) {}
						}
						SetText( Layout::LSN_VRWI_FPS_EDIT, FrameRateText( m_pvrdData->rConsoleRate.ui32Num, m_pvrdData->rConsoleRate.ui32Den ) );
					}
					else if ( !m_wsCustomFps.empty() ) {
						SetText( Layout::LSN_VRWI_FPS_EDIT, m_wsCustomFps );
					}
				}
				break;
			}
			case Layout::LSN_VRWI_FOLDER_BUTTON : {
				try {
					std::wstring wsDisplayName;
					wsDisplayName.resize( 0xFFFF + 2 );
					std::wstring wsInitial;
					CWidget * pwEdit = FindChild( Layout::LSN_VRWI_FOLDER_EDIT );
					if ( pwEdit ) { wsInitial = pwEdit->GetTextW(); }
					if ( wsInitial.empty() ) { wsInitial = m_pvrdData->wsDefaultFolder; }

					BROWSEINFOW biBrowseInfo = {};
					biBrowseInfo.hwndOwner = Wnd();
					biBrowseInfo.pidlRoot = nullptr;
					biBrowseInfo.pszDisplayName = wsDisplayName.data();
					biBrowseInfo.lpszTitle = LSN_LSTR( LSN_VR_BROWSE_FOLDER );
					biBrowseInfo.ulFlags = BIF_RETURNONLYFSDIRS;
					biBrowseInfo.iImage = 0;
					biBrowseInfo.lpfn = CWinUtilities::BrowseCallbackProc;
					biBrowseInfo.lParam = reinterpret_cast<LPARAM>(wsInitial.c_str());
					{
						CWinUtilities::LSN_OLEINITIALIZE oOle;
						if ( oOle.Success() ) {
							biBrowseInfo.ulFlags |= BIF_NEWDIALOGSTYLE;
						}

						PIDLIST_ABSOLUTE pidlSelected = ::SHBrowseForFolderW( &biBrowseInfo );
						if ( pidlSelected ) {
							std::wstring wsFolder;
							wsFolder.resize( 0xFFFF + 2 );
							if ( ::SHGetPathFromIDListW( pidlSelected, wsFolder.data() ) ) {
								SetText( Layout::LSN_VRWI_FOLDER_EDIT, std::wstring( wsFolder.c_str() ) );
							}
							::CoTaskMemFree( pidlSelected );
						}
					}
				}
				catch ( ... ) {}
				break;
			}
			case Layout::LSN_VRWI_FFMPEG_BUTTON : {
				try {
					OPENFILENAMEW ofnOpenFile = { sizeof( ofnOpenFile ) };
					std::wstring wsFileName;
					wsFileName.resize( 0xFFFF + 2 );
					std::wstring wsInitialDir;
					CWidget * pwEdit = FindChild( Layout::LSN_VRWI_FFMPEG_EDIT );
					if ( pwEdit ) {
						std::wstring wsFound = CFfmpegProcess::ResolveFfmpeg( pwEdit->GetTextW() );
						if ( !wsFound.empty() ) { wsInitialDir = std::filesystem::path( wsFound ).parent_path().wstring(); }
					}

					std::wstring wsFilter = std::wstring( LSN_LSTR( LSN_VR_FFMPEG_TYPES ), std::size( LSN_LSTR( LSN_VR_FFMPEG_TYPES ) ) - 1 );
					ofnOpenFile.hwndOwner = Wnd();
					ofnOpenFile.lpstrFilter = wsFilter.c_str();
					ofnOpenFile.lpstrFile = wsFileName.data();
					ofnOpenFile.nMaxFile = DWORD( wsFileName.size() );
					ofnOpenFile.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
					ofnOpenFile.lpstrInitialDir = wsInitialDir.empty() ? nullptr : wsInitialDir.c_str();

					if ( ::GetOpenFileNameW( &ofnOpenFile ) ) {
						SetText( Layout::LSN_VRWI_FFMPEG_EDIT, std::wstring( ofnOpenFile.lpstrFile ) );
					}
				}
				catch ( ... ) {}
				break;
			}
		}
		Update();
		return LSW_H_CONTINUE;
	}

	/**
	 * Handles WM_COMMAND from a menu.
	 * \brief Invoked for menu command selections.
	 *
	 * \param _wId The menu command identifier.
	 * \return Returns a LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CVideoRecordingWindow::MenuCommand( WORD _wId ) {
		switch ( _wId ) {
			case IDCANCEL : {}													LSN_FALLTHROUGH
			case Layout::LSN_VRWI_CANCEL : {
				return Close();
			}
		}
		return Parent::MenuCommand( _wId );
	}

	/**
	 * WM_HSCROLL.
	 *
	 * \param _uScrollPos Specifies the current position of the scroll box if the LOWORD is SB_THUMBPOSITION or SB_THUMBTRACK; otherwise, this word is not used.
	 * \param _uScrollType Specifies a scroll bar value that indicates the user's scrolling request.
	 * \param _pwWidget The trackbar that scrolled.
	 * \return Returns an LSW_HANDLED code.
	 **/
	CWidget::LSW_HANDLED CVideoRecordingWindow::HScroll( USHORT /*_uScrollPos*/, USHORT /*_uScrollType*/, CWidget * _pwWidget ) {
		if ( !_pwWidget ) { return LSW_H_CONTINUE; }
		const LSN_VIDEO_FORMAT_DATA * pvfdData = CVideoFormat::Data( m_vfShown );
		if ( !pvfdData ) { return LSW_H_CONTINUE; }
		switch ( _pwWidget->Id() ) {
			case Layout::LSN_VRWI_QUALITY_TRACKBAR : {
				EditFromTrackBar( Layout::LSN_VRWI_QUALITY_TRACKBAR, Layout::LSN_VRWI_QUALITY_EDIT,
					(*pvfdData).vqdQuality.i32Min, (*pvfdData).vqdQuality.i32Max, (*pvfdData).vqdQuality.i32Step );
				break;
			}
			case Layout::LSN_VRWI_ALT_QUALITY_TRACKBAR : {
				EditFromTrackBar( Layout::LSN_VRWI_ALT_QUALITY_TRACKBAR, Layout::LSN_VRWI_ALT_QUALITY_EDIT,
					(*pvfdData).vqdAltQuality.i32Min, (*pvfdData).vqdAltQuality.i32Max, (*pvfdData).vqdAltQuality.i32Step );
				break;
			}
			case Layout::LSN_VRWI_BITRATE_TRACKBAR : {
				const LSN_VIDEO_AUDIO_CODEC_DATA * pvacdData = CVideoFormat::AudioCodecData( SelectedCodec() );
				if ( pvacdData && !(*pvacdData).bLossless ) {
					EditFromTrackBar( Layout::LSN_VRWI_BITRATE_TRACKBAR, Layout::LSN_VRWI_BITRATE_EDIT,
						int32_t( (*pvacdData).ui32MinBitrate ), int32_t( (*pvacdData).ui32MaxBitrate ), 1 );
				}
				break;
			}
		}
		Update();
		return LSW_H_CONTINUE;
	}

	/**
	 * Verifies the inputs, copies them to the options, and closes the dialog.  If an input is invalid, the error is shown instead.
	 */
	void CVideoRecordingWindow::SaveAndClose() {
		if ( !m_pvrdData || !m_pvrdData->pveoOptions ) {
			Close();
			return;
		}
		std::wstring wsError;
		CWidget * pwError = ReadFormat( m_vfShown, wsError );
		if ( !pwError ) { pwError = ReadGlobal( wsError ); }
		if ( pwError ) {
			lsw::CBase::MessageBoxError( Wnd(), wsError.c_str(), LSN_LSTR( LSN_ERROR ) );
			pwError->SetFocus();
			return;
		}
		m_veoWorking.vfFormat = m_vfShown;
		try {
			(*m_pvrdData->pveoOptions) = m_veoWorking;
		}
		catch ( ... ) {
			lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_OUT_OF_MEMORY ), LSN_LSTR( LSN_ERROR ) );
			return;
		}
		::EndDialog( Wnd(), 1 );
	}

	/**
	 * Shows the settings of a video format.
	 *
	 * \param _vfFormat The video format whose settings (in m_veoWorking) are to be shown.
	 */
	void CVideoRecordingWindow::LoadFormat( LSN_VIDEO_FORMAT _vfFormat ) {
		const LSN_VIDEO_FORMAT_DATA * pvfdData = CVideoFormat::Data( _vfFormat );
		if ( !pvfdData ) { return; }
		LSN_VIDEO_EXPORT_OPTIONS::LSN_FORMAT_OPTIONS &foOptions = m_veoWorking.foFormat[_vfFormat];
		foOptions.Validate( _vfFormat );
		m_bUpdating = true;

		// Presets.
		lsw::CComboBox * pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_VRWI_PRESET_COMBO ));
		if ( pcbCombo ) {
			pcbCombo->ResetContent();
			for ( uint32_t I = 0; (*pvfdData).pvpPresets && I < (*pvfdData).ui32TotalPresets; ++I ) {
				std::wstring wsName;
				for ( const char * pcChar = (*pvfdData).pvpPresets[I].pcName; pcChar && (*pcChar); ++pcChar ) { wsName.push_back( wchar_t( uint8_t( (*pcChar) ) ) ); }
				INT iIdx = pcbCombo->AddString( wsName.c_str() );
				if ( iIdx >= 0 ) { pcbCombo->SetItemData( iIdx, LPARAM( I ) ); }
			}
			if ( !(*pvfdData).ui32TotalPresets ) {
				INT iIdx = pcbCombo->AddString( LSN_LSTR( LSN_VR_NO_PRESETS ) );
				if ( iIdx >= 0 ) { pcbCombo->SetItemData( iIdx, 0 ); }
			}
			pcbCombo->SetCurSelByItemData( LPARAM( foOptions.ui32Preset ) );
		}

		// Quality.
		SetUpTrackBar( Layout::LSN_VRWI_QUALITY_TRACKBAR, Layout::LSN_VRWI_QUALITY_EDIT,
			(*pvfdData).vqdQuality.i32Min, (*pvfdData).vqdQuality.i32Max, (*pvfdData).vqdQuality.i32Step, foOptions.i32Quality );
		SetUpTrackBar( Layout::LSN_VRWI_ALT_QUALITY_TRACKBAR, Layout::LSN_VRWI_ALT_QUALITY_EDIT,
			(*pvfdData).vqdAltQuality.i32Min, (*pvfdData).vqdAltQuality.i32Max, (*pvfdData).vqdAltQuality.i32Step, foOptions.i32AltQuality );
		SetBoxChecked( Layout::LSN_VRWI_LOSSLESS_CHECK, (*pvfdData).bAlwaysLossless || foOptions.bLossless );
		SetBoxChecked( Layout::LSN_VRWI_ALT_QUALITY_CHECK, foOptions.bUseAltQuality );

		// Pixel formats.
		pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_VRWI_PIXEL_FORMAT_COMBO ));
		if ( pcbCombo ) {
			pcbCombo->ResetContent();
			for ( uint32_t I = 0; I < LSN_VPF_TOTAL; ++I ) {
				if ( !CVideoFormat::SupportsPixelFormat( _vfFormat, LSN_VIDEO_PIXEL_FORMAT( I ) ) ) { continue; }
				const LSN_VIDEO_PIXEL_FORMAT_DATA * pvpfdData = CVideoFormat::PixelFormatData( LSN_VIDEO_PIXEL_FORMAT( I ) );
				if ( !pvpfdData ) { continue; }
				INT iIdx = pcbCombo->AddString( (*pvpfdData).pwcName );
				if ( iIdx >= 0 ) { pcbCombo->SetItemData( iIdx, LPARAM( I ) ); }
			}
			pcbCombo->SetCurSelByItemData( LPARAM( foOptions.vpfPixelFormat ) );
		}

		// Frame rate.
		SetBoxChecked( Layout::LSN_VRWI_MATCH_CONSOLE_CHECK, foOptions.MatchesConsole() );
		if ( foOptions.MatchesConsole() ) {
			SetText( Layout::LSN_VRWI_FPS_EDIT, FrameRateText( m_pvrdData->rConsoleRate.ui32Num, m_pvrdData->rConsoleRate.ui32Den ) );
		}
		else {
			SetText( Layout::LSN_VRWI_FPS_EDIT, FrameRateText( foOptions.ui32FrameRateNum, foOptions.ui32FrameRateDen ) );
		}

		// Audio codecs.
		pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_VRWI_CODEC_COMBO ));
		if ( pcbCombo ) {
			pcbCombo->ResetContent();
			for ( uint32_t I = 0; I < LSN_VAC_TOTAL; ++I ) {
				if ( !CVideoFormat::SupportsAudioCodec( _vfFormat, LSN_VIDEO_AUDIO_CODEC( I ) ) ) { continue; }
				const LSN_VIDEO_AUDIO_CODEC_DATA * pvacdData = CVideoFormat::AudioCodecData( LSN_VIDEO_AUDIO_CODEC( I ) );
				if ( !pvacdData ) { continue; }
				INT iIdx = pcbCombo->AddString( (*pvacdData).pwcName );
				if ( iIdx >= 0 ) { pcbCombo->SetItemData( iIdx, LPARAM( I ) ); }
			}
			pcbCombo->SetCurSelByItemData( LPARAM( foOptions.vacAudioCodec ) );
		}
		m_bUpdating = false;
		LoadBitrate( foOptions.vacAudioCodec, foOptions.ui32AudioBitrate );
		Update();
	}

	/**
	 * Reads the shown settings of a video format into m_veoWorking.
	 *
	 * \param _vfFormat The video format whose settings are shown.
	 * \param _wsError Receives an error message if an input is invalid.
	 * \return Returns the control with the invalid input or nullptr if every input is valid.
	 */
	CWidget * CVideoRecordingWindow::ReadFormat( LSN_VIDEO_FORMAT _vfFormat, std::wstring &_wsError ) {
		const LSN_VIDEO_FORMAT_DATA * pvfdData = CVideoFormat::Data( _vfFormat );
		if ( !pvfdData ) { return nullptr; }
		LSN_VIDEO_EXPORT_OPTIONS::LSN_FORMAT_OPTIONS foTmp = m_veoWorking.foFormat[_vfFormat];
		try {
			lsw::CComboBox * pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_VRWI_PRESET_COMBO ));
			if ( pcbCombo && (*pvfdData).ui32TotalPresets && pcbCombo->GetCurSel() >= 0 ) { foTmp.ui32Preset = uint32_t( pcbCombo->GetCurSelItemData() ); }

			foTmp.bLossless = !(*pvfdData).bAlwaysLossless && (*pvfdData).pcLosslessArgs && IsBoxChecked( Layout::LSN_VRWI_LOSSLESS_CHECK );
			foTmp.bUseAltQuality = (*pvfdData).vqdAltQuality.vqmMode != LSN_VQM_NONE && IsBoxChecked( Layout::LSN_VRWI_ALT_QUALITY_CHECK );
			bool bQualityUsed = !(*pvfdData).bAlwaysLossless && !foTmp.bLossless;

			// Each quality value is checked only if it is used; an unused value that is invalid is left as it was.
			if ( (*pvfdData).vqdQuality.vqmMode != LSN_VQM_NONE ) {
				int32_t i32Value;
				if ( ReadRangedValue( Layout::LSN_VRWI_QUALITY_EDIT, (*pvfdData).vqdQuality.i32Min, (*pvfdData).vqdQuality.i32Max, i32Value ) ) {
					foTmp.i32Quality = i32Value;
				}
				else if ( bQualityUsed && !foTmp.bUseAltQuality ) {
					_wsError = std::format( LSN_LSTR( LSN_VR_ERR_RANGE ), LSN_LSTR( LSN_VR_QUALITY ), (*pvfdData).vqdQuality.i32Min, (*pvfdData).vqdQuality.i32Max );
					return FindChild( Layout::LSN_VRWI_QUALITY_EDIT );
				}
			}
			if ( (*pvfdData).vqdAltQuality.vqmMode != LSN_VQM_NONE ) {
				int32_t i32Value;
				if ( ReadRangedValue( Layout::LSN_VRWI_ALT_QUALITY_EDIT, (*pvfdData).vqdAltQuality.i32Min, (*pvfdData).vqdAltQuality.i32Max, i32Value ) ) {
					foTmp.i32AltQuality = i32Value;
				}
				else if ( bQualityUsed && foTmp.bUseAltQuality ) {
					_wsError = std::format( LSN_LSTR( LSN_VR_ERR_RANGE ), LSN_LSTR( LSN_VR_ALT_QUALITY ), (*pvfdData).vqdAltQuality.i32Min, (*pvfdData).vqdAltQuality.i32Max );
					return FindChild( Layout::LSN_VRWI_ALT_QUALITY_EDIT );
				}
			}

			pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_VRWI_PIXEL_FORMAT_COMBO ));
			if ( pcbCombo && pcbCombo->GetCurSel() >= 0 ) { foTmp.vpfPixelFormat = LSN_VIDEO_PIXEL_FORMAT( pcbCombo->GetCurSelItemData() ); }

			if ( IsBoxChecked( Layout::LSN_VRWI_MATCH_CONSOLE_CHECK ) ) {
				foTmp.ui32FrameRateNum = 0;
				foTmp.ui32FrameRateDen = 1;
			}
			else {
				CWidget * pwEdit = FindChild( Layout::LSN_VRWI_FPS_EDIT );
				uint32_t ui32Num, ui32Den;
				if ( !pwEdit || !ParseFrameRate( pwEdit->GetTextW(), ui32Num, ui32Den ) ) {
					_wsError = std::format( LSN_LSTR( LSN_VR_ERR_FPS ), L"60", L"{60000,1001}", L"59.94", uint32_t( LSN_VIDEO_EXPORT_OPTIONS::LSN_MAX_FPS ) );
					return pwEdit;
				}
				foTmp.ui32FrameRateNum = ui32Num;
				foTmp.ui32FrameRateDen = ui32Den;
			}

			LSN_VIDEO_AUDIO_CODEC vacCodec = SelectedCodec();
			const LSN_VIDEO_AUDIO_CODEC_DATA * pvacdData = CVideoFormat::AudioCodecData( vacCodec );
			if ( pvacdData ) {
				foTmp.vacAudioCodec = vacCodec;
				if ( !(*pvacdData).bLossless ) {
					int32_t i32Value;
					if ( !ReadRangedValue( Layout::LSN_VRWI_BITRATE_EDIT, int32_t( (*pvacdData).ui32MinBitrate ), int32_t( (*pvacdData).ui32MaxBitrate ), i32Value ) ) {
						_wsError = std::format( LSN_LSTR( LSN_VR_ERR_RANGE ), LSN_LSTR( LSN_VR_AUDIO_BITRATE ), (*pvacdData).ui32MinBitrate, (*pvacdData).ui32MaxBitrate );
						return FindChild( Layout::LSN_VRWI_BITRATE_EDIT );
					}
					foTmp.ui32AudioBitrate = uint32_t( i32Value );
				}
			}
		}
		catch ( ... ) {
			_wsError = LSN_LSTR( LSN_OUT_OF_MEMORY );
			return this;
		}
		foTmp.Validate( _vfFormat );
		m_veoWorking.foFormat[_vfFormat] = foTmp;
		return nullptr;
	}

	/**
	 * Reads the settings shared by every video format (the output, the frame blending, and the conditions) into m_veoWorking.
	 *
	 * \param _wsError Receives an error message if an input is invalid.
	 * \return Returns the control with the invalid input or nullptr if every input is valid.
	 */
	CWidget * CVideoRecordingWindow::ReadGlobal( std::wstring &_wsError ) {
		try {
			LSN_VIDEO_EXPORT_OPTIONS veoTmp = m_veoWorking;

			CWidget * pwEdit = FindChild( Layout::LSN_VRWI_FOLDER_EDIT );
			if ( pwEdit ) { veoTmp.wsOutputFolder = Trim( pwEdit->GetTextW() ); }
			pwEdit = FindChild( Layout::LSN_VRWI_FFMPEG_EDIT );
			if ( pwEdit ) {
				veoTmp.wsFfmpegPath = Trim( pwEdit->GetTextW() );
				if ( !veoTmp.wsFfmpegPath.empty() && CFfmpegProcess::ResolveFfmpeg( veoTmp.wsFfmpegPath ).empty() ) {
					_wsError = std::format( LSN_LSTR( LSN_VR_ERR_FFMPEG ), veoTmp.wsFfmpegPath );
					return pwEdit;
				}
			}
			veoTmp.bInterpolateFrames = IsBoxChecked( Layout::LSN_VRWI_BLEND_CHECK );

			lsw::CComboBox * pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_VRWI_START_COND_COMBO ));
			if ( pcbCombo && pcbCombo->GetCurSel() >= 0 ) { veoTmp.scStart = LSN_VIDEO_EXPORT_OPTIONS::LSN_START_CONDITION( pcbCombo->GetCurSelItemData() ); }
			pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_VRWI_END_COND_COMBO ));
			if ( pcbCombo && pcbCombo->GetCurSel() >= 0 ) { veoTmp.ecEnd = LSN_VIDEO_EXPORT_OPTIONS::LSN_END_CONDITION( pcbCombo->GetCurSelItemData() ); }

			ee::CExpEvalContainer::EE_RESULT eRes;
			CWidget * pwStart = FindChild( Layout::LSN_VRWI_START_COMBO );
			if ( veoTmp.scStart == LSN_VIDEO_EXPORT_OPTIONS::LSN_VSC_START_AT_FRAME && pwStart ) {
				if ( !pwStart->GetTextAsUInt64Expression( eRes ) ) {
					_wsError = std::format( LSN_LSTR( LSN_VR_ERR_START_FRAME ), L"60*60" );
					return pwStart;
				}
				veoTmp.ui64StartFrame = eRes.u.ui64Val;
				CUtilities::AddOrMove( veoTmp.vStartHistory, pwStart->GetTextW() );
			}
			CWidget * pwEnd = FindChild( Layout::LSN_VRWI_END_COMBO );
			if ( veoTmp.ecEnd == LSN_VIDEO_EXPORT_OPTIONS::LSN_VEC_END_AT_FRAME && pwEnd ) {
				if ( !pwEnd->GetTextAsUInt64Expression( eRes ) ) {
					_wsError = std::format( LSN_LSTR( LSN_VR_ERR_END_FRAME ), L"60*60*2" );
					return pwEnd;
				}
				if ( veoTmp.scStart == LSN_VIDEO_EXPORT_OPTIONS::LSN_VSC_START_AT_FRAME && eRes.u.ui64Val <= veoTmp.ui64StartFrame ) {
					_wsError = LSN_LSTR( LSN_VR_ERR_END_BEFORE_START );
					return pwEnd;
				}
				veoTmp.ui64EndFrame = eRes.u.ui64Val;
				CUtilities::AddOrMove( veoTmp.vEndHistory, pwEnd->GetTextW() );
			}
			else if ( veoTmp.ecEnd == LSN_VIDEO_EXPORT_OPTIONS::LSN_VEC_DURATION && pwEnd ) {
				if ( !pwEnd->GetTextAsDoubleExpression( eRes ) || !(eRes.u.dVal > 0.0 && eRes.u.dVal < 1.0e9) ) {
					_wsError = std::format( LSN_LSTR( LSN_VR_ERR_DURATION ), L"60*2 + 30" );
					return pwEnd;
				}
				veoTmp.dEndSeconds = eRes.u.dVal;
				CUtilities::AddOrMove( veoTmp.vEndHistory, pwEnd->GetTextW() );
			}

			m_veoWorking = std::move( veoTmp );
		}
		catch ( ... ) {
			_wsError = LSN_LSTR( LSN_OUT_OF_MEMORY );
			return this;
		}
		return nullptr;
	}

	/**
	 * Fills the bitrate controls for an audio codec.
	 *
	 * \param _vacCodec The audio codec.
	 * \param _ui32Bitrate The bitrate to show.  It is replaced by the codec's best bitrate if the codec does not support it.
	 */
	void CVideoRecordingWindow::LoadBitrate( LSN_VIDEO_AUDIO_CODEC _vacCodec, uint32_t _ui32Bitrate ) {
		const LSN_VIDEO_AUDIO_CODEC_DATA * pvacdData = CVideoFormat::AudioCodecData( _vacCodec );
		if ( !pvacdData ) { return; }
		if ( (*pvacdData).bLossless ) {
			SetUpTrackBar( Layout::LSN_VRWI_BITRATE_TRACKBAR, Layout::LSN_VRWI_BITRATE_EDIT, 0, 0, 1, 0 );
			SetText( Layout::LSN_VRWI_BITRATE_EDIT, std::wstring() );
			return;
		}
		if ( _ui32Bitrate < (*pvacdData).ui32MinBitrate || _ui32Bitrate > (*pvacdData).ui32MaxBitrate ) { _ui32Bitrate = (*pvacdData).ui32BestBitrate; }
		SetUpTrackBar( Layout::LSN_VRWI_BITRATE_TRACKBAR, Layout::LSN_VRWI_BITRATE_EDIT,
			int32_t( (*pvacdData).ui32MinBitrate ), int32_t( (*pvacdData).ui32MaxBitrate ), 1, int32_t( _ui32Bitrate ) );
	}

	/**
	 * Enables and disables controls and updates the hints to match the current selections.
	 */
	void CVideoRecordingWindow::Update() {
		const LSN_VIDEO_FORMAT_DATA * pvfdData = CVideoFormat::Data( m_vfShown );
		if ( !pvfdData ) { return; }

		bool bLosslessAvailable = !(*pvfdData).bAlwaysLossless && (*pvfdData).pcLosslessArgs != nullptr;
		bool bLossless = (*pvfdData).bAlwaysLossless || (bLosslessAvailable && IsBoxChecked( Layout::LSN_VRWI_LOSSLESS_CHECK ));
		bool bHasQuality = (*pvfdData).vqdQuality.vqmMode != LSN_VQM_NONE;
		bool bHasAlt = (*pvfdData).vqdAltQuality.vqmMode != LSN_VQM_NONE;
		bool bUseAlt = bHasAlt && IsBoxChecked( Layout::LSN_VRWI_ALT_QUALITY_CHECK );
		bool bMatchConsole = IsBoxChecked( Layout::LSN_VRWI_MATCH_CONSOLE_CHECK );
		const LSN_VIDEO_AUDIO_CODEC_DATA * pvacdData = CVideoFormat::AudioCodecData( SelectedCodec() );
		bool bHasBitrate = pvacdData && !(*pvacdData).bLossless;

		LPARAM lpStart = LSN_VIDEO_EXPORT_OPTIONS::LSN_VSC_NONE;
		lsw::CComboBox * pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_VRWI_START_COND_COMBO ));
		if ( pcbCombo ) { lpStart = pcbCombo->GetCurSelItemData(); }
		LPARAM lpEnd = LSN_VIDEO_EXPORT_OPTIONS::LSN_VEC_NONE;
		pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_VRWI_END_COND_COMBO ));
		if ( pcbCombo ) { lpEnd = pcbCombo->GetCurSelItemData(); }

		struct LSN_CONTROLS {
			WORD																wId;
			bool																bEnabled;
		} cControls[] = {
			{ Layout::LSN_VRWI_PRESET_COMBO,									(*pvfdData).ui32TotalPresets > 1 },
			{ Layout::LSN_VRWI_LOSSLESS_CHECK,									bLosslessAvailable },
			{ Layout::LSN_VRWI_QUALITY_TRACKBAR,								bHasQuality && !bLossless && !bUseAlt },
			{ Layout::LSN_VRWI_QUALITY_EDIT,									bHasQuality && !bLossless && !bUseAlt },
			{ Layout::LSN_VRWI_ALT_QUALITY_CHECK,								bHasAlt && !bLossless },
			{ Layout::LSN_VRWI_ALT_QUALITY_TRACKBAR,							bUseAlt && !bLossless },
			{ Layout::LSN_VRWI_ALT_QUALITY_EDIT,								bUseAlt && !bLossless },
			{ Layout::LSN_VRWI_FPS_EDIT,										!bMatchConsole },
			{ Layout::LSN_VRWI_BLEND_CHECK,										!bMatchConsole },
			{ Layout::LSN_VRWI_BITRATE_TRACKBAR,								bHasBitrate },
			{ Layout::LSN_VRWI_BITRATE_EDIT,									bHasBitrate },
			{ Layout::LSN_VRWI_START_COMBO,										lpStart != LSN_VIDEO_EXPORT_OPTIONS::LSN_VSC_NONE },
			{ Layout::LSN_VRWI_END_COMBO,										lpEnd != LSN_VIDEO_EXPORT_OPTIONS::LSN_VEC_NONE },
		};
		for ( size_t I = 0; I < std::size( cControls ); ++I ) {
			CWidget * pwThis = FindChild( cControls[I].wId );
			if ( pwThis ) { pwThis->SetEnabled( cControls[I].bEnabled ); }
		}

		// Hints.
		try {
			int32_t i32Value = 0;
			std::wstring wsHint;
			if ( (*pvfdData).bAlwaysLossless ) { wsHint = LSN_LSTR( LSN_VR_ALWAYS_LOSSLESS ); }
			else {
				if ( !ReadRangedValue( Layout::LSN_VRWI_QUALITY_EDIT, (*pvfdData).vqdQuality.i32Min, (*pvfdData).vqdQuality.i32Max, i32Value ) ) {
					i32Value = m_veoWorking.foFormat[m_vfShown].i32Quality;
				}
				wsHint = QualityHint( (*pvfdData).vqdQuality, i32Value );
			}
			SetText( Layout::LSN_VRWI_QUALITY_HINT, wsHint );

			if ( !ReadRangedValue( Layout::LSN_VRWI_ALT_QUALITY_EDIT, (*pvfdData).vqdAltQuality.i32Min, (*pvfdData).vqdAltQuality.i32Max, i32Value ) ) {
				i32Value = m_veoWorking.foFormat[m_vfShown].i32AltQuality;
			}
			SetText( Layout::LSN_VRWI_ALT_QUALITY_HINT, QualityHint( (*pvfdData).vqdAltQuality, i32Value ) );

			if ( !pvacdData ) { wsHint.clear(); }
			else if ( (*pvacdData).bLossless ) { wsHint = LSN_LSTR( LSN_VR_NO_BITRATE ); }
			else {
				wsHint = std::format( LSN_LSTR( LSN_VR_RANGE_HIGHER_BETTER ), LSN_LSTR( LSN_VR_KBPS ), (*pvacdData).ui32MinBitrate, (*pvacdData).ui32MaxBitrate );
			}
			SetText( Layout::LSN_VRWI_BITRATE_HINT, wsHint );

			// Examples of what to enter for each condition.
			CWidget * pwCombo = FindChild( Layout::LSN_VRWI_START_COMBO );
			if ( pwCombo ) {
				static std::wstring wsStartCue = std::format( LSN_LSTR( LSN_VR_START_FRAME_CUE ), L"60*60" );
				::SendMessageW( pwCombo->Wnd(), CB_SETCUEBANNER, 0, reinterpret_cast<LPARAM>(wsStartCue.c_str()) );
			}
			pwCombo = FindChild( Layout::LSN_VRWI_END_COMBO );
			if ( pwCombo ) {
				static std::wstring wsEndFrameCue = std::format( LSN_LSTR( LSN_VR_END_FRAME_CUE ), L"60*60*2" );
				static std::wstring wsSecondsCue = std::format( LSN_LSTR( LSN_VR_SECONDS_CUE ), L"60*2 + 30" );
				::SendMessageW( pwCombo->Wnd(), CB_SETCUEBANNER, 0,
					reinterpret_cast<LPARAM>(lpEnd == LSN_VIDEO_EXPORT_OPTIONS::LSN_VEC_DURATION ? wsSecondsCue.c_str() : wsEndFrameCue.c_str()) );
			}
		}
		catch ( ... ) {}
	}

	/**
	 * Gets the video format selected in the Format combo box.
	 *
	 * \return Returns the selected video format.
	 */
	LSN_VIDEO_FORMAT CVideoRecordingWindow::SelectedFormat() const {
		const lsw::CComboBox * pcbCombo = reinterpret_cast<const lsw::CComboBox *>(FindChild( Layout::LSN_VRWI_FORMAT_COMBO ));
		if ( !pcbCombo || pcbCombo->GetCurSel() < 0 ) { return m_vfShown; }
		LPARAM lpData = pcbCombo->GetCurSelItemData();
		return (lpData >= 0 && lpData < LSN_VF_TOTAL) ? LSN_VIDEO_FORMAT( lpData ) : m_vfShown;
	}

	/**
	 * Gets the audio codec selected in the Codec combo box.
	 *
	 * \return Returns the selected audio codec or LSN_VAC_TOTAL.
	 */
	LSN_VIDEO_AUDIO_CODEC CVideoRecordingWindow::SelectedCodec() const {
		const lsw::CComboBox * pcbCombo = reinterpret_cast<const lsw::CComboBox *>(FindChild( Layout::LSN_VRWI_CODEC_COMBO ));
		if ( !pcbCombo || pcbCombo->GetCurSel() < 0 ) { return LSN_VAC_TOTAL; }
		LPARAM lpData = pcbCombo->GetCurSelItemData();
		return (lpData >= 0 && lpData < LSN_VAC_TOTAL) ? LSN_VIDEO_AUDIO_CODEC( lpData ) : LSN_VAC_TOTAL;
	}

	/**
	 * Determines whether a check box is checked.
	 *
	 * \param _wId The ID of the check box.
	 * \return Returns true if the check box exists and is checked.
	 */
	bool CVideoRecordingWindow::IsBoxChecked( WORD _wId ) const {
		const lsw::CCheckButton * pcbCheck = reinterpret_cast<const lsw::CCheckButton *>(FindChild( _wId ));
		return pcbCheck && pcbCheck->IsChecked();
	}

	/**
	 * Checks or unchecks a check box.
	 *
	 * \param _wId The ID of the check box.
	 * \param _bChecked Whether to check the box.
	 */
	void CVideoRecordingWindow::SetBoxChecked( WORD _wId, bool _bChecked ) {
		lsw::CCheckButton * pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( _wId ));
		if ( pcbCheck ) { pcbCheck->SetCheck( _bChecked ? TRUE : FALSE ); }
	}

	/**
	 * Sets the text of a control without reacting to its change notification.
	 *
	 * \param _wId The ID of the control.
	 * \param _wsText The text to set.
	 */
	void CVideoRecordingWindow::SetText( WORD _wId, const std::wstring &_wsText ) {
		CWidget * pwWidget = FindChild( _wId );
		if ( !pwWidget ) { return; }
		bool bOld = m_bUpdating;
		m_bUpdating = true;
		pwWidget->SetTextW( _wsText.c_str() );
		m_bUpdating = bOld;
	}

	/**
	 * Sets up a trackbar and its edit control for a range of values.
	 *
	 * \param _wTrackBar The ID of the trackbar.
	 * \param _wEdit The ID of the edit control.
	 * \param _i32Min The minimum value.
	 * \param _i32Max The maximum value.
	 * \param _i32Step The step between trackbar positions.
	 * \param _i32Value The value to show.
	 */
	void CVideoRecordingWindow::SetUpTrackBar( WORD _wTrackBar, WORD _wEdit, int32_t _i32Min, int32_t _i32Max, int32_t _i32Step, int32_t _i32Value ) {
		if ( _i32Step <= 0 ) { _i32Step = 1; }
		if ( _i32Max < _i32Min ) { _i32Max = _i32Min; }
		lsw::CTrackBar * ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( _wTrackBar ));
		if ( ptbTrackBar ) {
			DWORD dwPositions = DWORD( (_i32Max - _i32Min) / _i32Step );
			ptbTrackBar->SetRangeMin( FALSE, 0 );
			ptbTrackBar->SetRangeMax( TRUE, dwPositions );
			ptbTrackBar->SetTicFreq( std::max<DWORD>( 1, dwPositions / 10 ) );
			ptbTrackBar->SetPageSize( std::max<DWORD>( 1, dwPositions / 10 ) );
			int32_t i32Clamped = std::clamp( _i32Value, _i32Min, _i32Max );
			ptbTrackBar->SetPos( TRUE, LPARAM( (i32Clamped - _i32Min + (_i32Step >> 1)) / _i32Step ) );
		}
		SetText( _wEdit, std::to_wstring( _i32Value ) );
	}

	/**
	 * Moves a trackbar to match the value in its edit control.
	 *
	 * \param _wTrackBar The ID of the trackbar.
	 * \param _wEdit The ID of the edit control.
	 * \param _i32Min The minimum value.
	 * \param _i32Max The maximum value.
	 * \param _i32Step The step between trackbar positions.
	 */
	void CVideoRecordingWindow::TrackBarFromEdit( WORD _wTrackBar, WORD _wEdit, int32_t _i32Min, int32_t _i32Max, int32_t _i32Step ) {
		int32_t i32Value;
		if ( !ReadRangedValue( _wEdit, _i32Min, _i32Max, i32Value ) ) { return; }
		if ( _i32Step <= 0 ) { _i32Step = 1; }
		lsw::CTrackBar * ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( _wTrackBar ));
		if ( ptbTrackBar ) {
			ptbTrackBar->SetPos( TRUE, LPARAM( (i32Value - _i32Min + (_i32Step >> 1)) / _i32Step ) );
		}
	}

	/**
	 * Sets an edit control to the value of its trackbar.
	 *
	 * \param _wTrackBar The ID of the trackbar.
	 * \param _wEdit The ID of the edit control.
	 * \param _i32Min The minimum value.
	 * \param _i32Max The maximum value.
	 * \param _i32Step The step between trackbar positions.
	 */
	void CVideoRecordingWindow::EditFromTrackBar( WORD _wTrackBar, WORD _wEdit, int32_t _i32Min, int32_t _i32Max, int32_t _i32Step ) {
		const lsw::CTrackBar * ptbTrackBar = reinterpret_cast<const lsw::CTrackBar *>(FindChild( _wTrackBar ));
		if ( !ptbTrackBar ) { return; }
		if ( _i32Step <= 0 ) { _i32Step = 1; }
		int64_t i64Value = int64_t( _i32Min ) + int64_t( ptbTrackBar->GetPos() ) * _i32Step;
		SetText( _wEdit, std::to_wstring( std::clamp<int64_t>( i64Value, _i32Min, _i32Max ) ) );
	}

	/**
	 * Reads a whole number in a range from an edit control.
	 *
	 * \param _wEdit The ID of the edit control.
	 * \param _i32Min The minimum value.
	 * \param _i32Max The maximum value.
	 * \param _i32Value Receives the value.
	 * \return Returns true if the edit control holds a whole number from _i32Min to _i32Max.
	 */
	bool CVideoRecordingWindow::ReadRangedValue( WORD _wEdit, int32_t _i32Min, int32_t _i32Max, int32_t &_i32Value ) const {
		const CWidget * pwEdit = FindChild( _wEdit );
		if ( !pwEdit ) { return false; }
		ee::CExpEvalContainer::EE_RESULT eRes;
		if ( !pwEdit->GetTextAsInt64Expression( eRes ) ) { return false; }
		if ( eRes.u.i64Val < _i32Min || eRes.u.i64Val > _i32Max ) { return false; }
		_i32Value = int32_t( eRes.u.i64Val );
		return true;
	}

	/**
	 * Gets the text shown for a frame rate:  the number of frames per second if it is a whole number, otherwise {numerator,denominator}.
	 *
	 * \param _ui32Num The numerator.
	 * \param _ui32Den The denominator.
	 * \return Returns the text for the frame rate.
	 */
	std::wstring CVideoRecordingWindow::FrameRateText( uint32_t _ui32Num, uint32_t _ui32Den ) {
		try {
			if ( _ui32Den == 1 ) { return std::to_wstring( _ui32Num ); }
			return L"{" + std::to_wstring( _ui32Num ) + L"," + std::to_wstring( _ui32Den ) + L"}";
		}
		catch ( ... ) { return std::wstring(); }
	}

	/**
	 * Parses a frame rate:  a whole number of frames per second or a {numerator,denominator} pair of whole numbers.
	 *
	 * \param _wsText The text to parse.
	 * \param _ui32Num Receives the numerator.
	 * \param _ui32Den Receives the denominator.
	 * \return Returns true if the text is a valid frame rate.
	 */
	bool CVideoRecordingWindow::ParseFrameRate( const std::wstring &_wsText, uint32_t &_ui32Num, uint32_t &_ui32Den ) {
		std::vector<double> vValues;
		if ( !CWinUtilities::TextAsDoubleVectorExpression( _wsText, vValues ) ) { return false; }
		if ( vValues.size() != 1 && vValues.size() != 2 ) { return false; }
		uint32_t ui32Values[2] = { 0, 1 };
		for ( size_t I = 0; I < vValues.size(); ++I ) {
			// Whole numbers only; fractions are given as {numerator,denominator}.
			if ( !(vValues[I] >= 1.0 && vValues[I] <= double( UINT32_MAX )) || std::floor( vValues[I] ) != vValues[I] ) { return false; }
			ui32Values[I] = uint32_t( vValues[I] );
		}
		if ( !LSN_VIDEO_EXPORT_OPTIONS::LSN_FORMAT_OPTIONS::ValidFrameRate( ui32Values[0], ui32Values[1] ) ) { return false; }
		_ui32Num = ui32Values[0];
		_ui32Den = ui32Values[1];
		return true;
	}

	/**
	 * Removes the whitespace from the start and end of a string.
	 *
	 * \param _wsText The string to trim.
	 * \return Returns the trimmed string.
	 */
	std::wstring CVideoRecordingWindow::Trim( const std::wstring &_wsText ) {
		size_t sStart = _wsText.find_first_not_of( L" \t\r\n" );
		if ( sStart == std::wstring::npos ) { return std::wstring(); }
		size_t sEnd = _wsText.find_last_not_of( L" \t\r\n" );
		return _wsText.substr( sStart, sEnd - sStart + 1 );
	}

	/**
	 * Gets the hint that describes a quality setting:  its name and range (or the name of its value).
	 *
	 * \param _vqdQuality The quality setting.
	 * \param _i32Value The current value.
	 * \return Returns the hint.
	 */
	std::wstring CVideoRecordingWindow::QualityHint( const LSN_VIDEO_QUALITY_DATA &_vqdQuality, int32_t _i32Value ) {
		try {
			const wchar_t * pwcMode = CVideoFormat::ToStrPWStr( _vqdQuality.vqmMode );
			switch ( _vqdQuality.vqmMode ) {
				case LSN_VQM_NONE : { return pwcMode; }
				case LSN_VQM_PROFILE : {}											LSN_FALLTHROUGH
				case LSN_VQM_BITRATE : {
					if ( _vqdQuality.ppwcValueNames && _i32Value >= _vqdQuality.i32Min && _i32Value <= _vqdQuality.i32Max ) {
						return std::format( LSN_LSTR( LSN_VR_NAMED_VALUE ), pwcMode, _i32Value, _vqdQuality.ppwcValueNames[_i32Value - _vqdQuality.i32Min] );
					}
					return std::format( LSN_LSTR( LSN_VR_RANGE_HIGHER_BETTER ), pwcMode, _vqdQuality.i32Min, _vqdQuality.i32Max );
				}
				default : {
					if ( _vqdQuality.ppwcValueNames && _i32Value >= _vqdQuality.i32Min && _i32Value <= _vqdQuality.i32Max ) {
						return std::format( LSN_LSTR( LSN_VR_NAMED_VALUE ), pwcMode, _i32Value, _vqdQuality.ppwcValueNames[_i32Value - _vqdQuality.i32Min] );
					}
					return std::format( LSN_LSTR( LSN_VR_RANGE_LOWER_BETTER ), pwcMode, _vqdQuality.i32Min, _vqdQuality.i32Max );
				}
			}
		}
		catch ( ... ) { return std::wstring(); }
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

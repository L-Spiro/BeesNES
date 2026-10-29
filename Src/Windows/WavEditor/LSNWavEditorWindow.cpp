#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The WAV-Editor window.
 */

#include "LSNWavEditorWindow.h"
#include "../../File/LSNFileStream.h"
#include "../../Localization/LSNLocalization.h"
#include "../../Utilities/LSNUtilities.h"
#include "../WinUtilities/LSNWinUtilities.h"

#include <Base/LSWWndClassEx.h>
#include <Rebar/LSWRebar.h>
#include <ToolBar/LSWToolBar.h>
#include <TreeListView/LSWTreeListView.h>

#include <commdlg.h>
#include <filesystem>

#include "../../../resource.h"


namespace lsn {

	// == Members.
	/** The main window class. */
	ATOM CWavEditorWindow::m_aAtom = 0;

	CWavEditorWindow::CWavEditorWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget , HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout.ChangeClass( reinterpret_cast<LPCWSTR>(m_aAtom) ), _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_poOptions( reinterpret_cast<LSN_OPTIONS *>(_ui64Data) ) {
		m_wewoWindowOptions = m_poOptions->wewoWavEditorWindow;


		static const struct {
			WORD				wImageName;
			DWORD				dwConst;
		} sImages[] = {
			{ IDB_SAVE_BITMAP_24,			LSN_I_SAVE },
			{ IDB_OPEN_BITMAP_24,			LSN_I_LOAD },
			{ IDB_ADD_WAV_BITMAP_24,		LSN_I_ADD_WAV },
			{ IDB_REMOVE_WAV_BITMAP_24,		LSN_I_REM_WAV },
			{ IDB_UP_WAV_BITMAP_24,			LSN_I_UP_WAV },
			{ IDB_DOWN_WAV_BITMAP_24,		LSN_I_DOWN_WAV },
			{ IDB_EXIT_BITMAP_24,			LSN_I_EXIT },
		};
		m_iImages.Create( 24, 24, ILC_COLOR32, LSN_I_TOTAL, LSN_I_TOTAL );

		for ( size_t I = 0; I < std::size( sImages ); ++I ) {
			m_bBitmaps[sImages[I].dwConst].LoadFromResource( sImages[I].wImageName, IMAGE_BITMAP, 0, 0, LR_LOADTRANSPARENT | LR_CREATEDIBSECTION );
			m_iImageMap[sImages[I].dwConst] = m_iImages.Add( m_bBitmaps[sImages[I].dwConst].Handle() );
		}
	}
	CWavEditorWindow::~CWavEditorWindow() {
		delete m_pwespSequencing;
		delete m_pwefspSettings;
		delete m_pwefFiles;
		delete m_pweopOutput;
	}


	// == Functions.
	/**
	 * The WM_INITDIALOG handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CWavEditorWindow::InitDialog() {
		SetIcons( reinterpret_cast<HICON>(::LoadImageW( CBase::GetModuleHandleW( nullptr ), MAKEINTRESOURCEW( IDI_SOUND_MIXER_ICON_16 ), IMAGE_ICON, 0, 0, LR_LOADTRANSPARENT )),
			reinterpret_cast<HICON>(::LoadImageW( CBase::GetModuleHandleW( nullptr ), MAKEINTRESOURCEW( IDI_SOUND_MIXER_ICON_32 ), IMAGE_ICON, 0, 0, LR_LOADTRANSPARENT )) );

		CToolBar * plvToolBar = static_cast<CToolBar *>(FindChild( Layout::LSN_WEWI_TOOLBAR0 ));
		CRebar * plvRebar = static_cast<CRebar *>(FindChild( Layout::LSN_WEWI_REBAR0 ));

		// ==== TOOL BAR ==== //
		if ( plvToolBar ) {
			plvToolBar->SetImageList( 0, m_iImages );

#define LSN_TOOL_STR( TXT )					reinterpret_cast<INT_PTR>(TXT)
//#define LSN_TOOL_STR( TXT )					0
			const TBBUTTON bButtons[] = {
				// iBitmap									idCommand									fsState				fsStyle			bReserved	dwData	iString
				{ m_iImageMap[LSN_I_LOAD],					Layout::LSN_WEWI_LOAD,						TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		LSN_TOOL_STR( LSN_LSTR( LSN_WE_LOAD ) ) },
				{ m_iImageMap[LSN_I_SAVE],					Layout::LSN_WEWI_SAVE,						TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		LSN_TOOL_STR( LSN_LSTR( LSN_WE_SAVE ) ) },
				{ -1,										0,											TBSTATE_ENABLED,	BTNS_SEP,		{ 0 },		0,		0 },
				{ m_iImageMap[LSN_I_ADD_WAV],				Layout::LSN_WEWI_FILES_ADD_BUTTON,			TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		LSN_TOOL_STR( LSN_LSTR( LSN_WE_ADD_WAV ) ) },
				{ m_iImageMap[LSN_I_REM_WAV],				Layout::LSN_WEWI_FILES_REMOVE_BUTTON,		TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		LSN_TOOL_STR( LSN_LSTR( LSN_WE_REMOVE ) ) },
				{ m_iImageMap[LSN_I_UP_WAV],				Layout::LSN_WEWI_FILES_UP_BUTTON,			TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		LSN_TOOL_STR( LSN_LSTR( LSN_WE_MOVE_UP ) ) },
				{ m_iImageMap[LSN_I_DOWN_WAV],				Layout::LSN_WEWI_FILES_DOWN_BUTTON,			TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		LSN_TOOL_STR( LSN_LSTR( LSN_WE_MOVE_DOWN ) ) },
				{ -1,										0,											TBSTATE_ENABLED,	BTNS_SEP,		{ 0 },		0,		0 },
				{ m_iImageMap[LSN_I_EXIT],					Layout::LSN_WEWI_CANCEL,					TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		LSN_TOOL_STR( LSN_LSTR( LSN_WE_EXIT ) ) },
			};
#undef LSN_TOOL_STR

			plvToolBar->AddButtons( bButtons, std::size( bButtons ) );

			if ( plvRebar ) {
				plvRebar->SetImageList( m_iImages );
				{
					LSW_REBARBANDINFO riRebarInfo;
					riRebarInfo.SetColors( ::GetSysColor( COLOR_BTNTEXT ), ::GetSysColor( COLOR_BTNFACE ) );
					riRebarInfo.SetStyle( RBBS_CHILDEDGE |
						RBBS_FIXEDBMP );
					riRebarInfo.SetChild( plvToolBar->Wnd() );
					riRebarInfo.SetChildSize( plvToolBar->GetMinBoundingRect().Width(), plvToolBar->GetMinBoundingRect().Height() );
					riRebarInfo.SetId( Layout::LSN_WEWI_TOOLBAR0 );
					plvRebar->InsertBand( -1, riRebarInfo );
				}
			}
		}

		LSW_RECT rRebarRect = LSW_RECT().Zero();
		if ( plvRebar ) {
			rRebarRect = ClientRect( this );
			::MoveWindow( plvRebar->Wnd(), 0, 0, rRebarRect.Width(), plvRebar->WindowRect( this ).Height(), FALSE );

			plvRebar->UpdateRects();

			rRebarRect = plvRebar->ClientRect( this );
		}

		m_pwefFiles = static_cast<CWavEditorFilesPage *>(CWavEditorWindowLayout::CreateFiles( this, m_wewoWindowOptions ));
		if ( !m_pwefFiles ) { return LSW_H_CONTINUE; }
		m_pwefFiles->SetWavEditor( m_weEditor );

		// There is only one sequencing page and one settings page.  They show the values of the selected track(s), which are stored in m_mTracks.
		CWidget * pwSeqPage = CWavEditorWindowLayout::CreateSequencer( this, m_wewoWindowOptions );
		if ( !pwSeqPage ) { return LSW_H_CONTINUE; }
		m_pwespSequencing = static_cast<CWavEditorSequencingPage *>(pwSeqPage);
		m_pwespSequencing->SetWavEditorAndId( m_weEditor, 0, m_pwefFiles );

		CWidget * pwSettingsPage = CWavEditorWindowLayout::CreateFileSettings( this, m_wewoWindowOptions );
		if ( !pwSettingsPage ) { return LSW_H_CONTINUE; }
		m_pwefspSettings = static_cast<CWavEditorFileSettingsPage *>(pwSettingsPage);
		m_pwefspSettings->SetWavEditorAndId( m_weEditor, 0, m_pwefFiles );

		m_pweopOutput = static_cast<CWavEditorOutputPage *>(CWavEditorWindowLayout::CreateOutput( this, m_wewoWindowOptions ));
		if ( !m_pweopOutput ) { return LSW_H_CONTINUE; }
		m_pweopOutput->SetWavEditor( m_weEditor );

		{
			auto rFilesRect = m_pwefFiles->WindowRect().ScreenToClient( Wnd() );
			rFilesRect.MoveBy( 0, rRebarRect.Height() );
			::MoveWindow( m_pwefFiles->Wnd(), rFilesRect.left, rFilesRect.top, rFilesRect.Width(), rFilesRect.Height(), TRUE );
		}

		auto * pwGroup = m_pwefFiles->FindChild( Layout::LSN_WEWI_FILES_GROUP );
		if ( pwGroup ) {
			LONG lMaxHeight = 0;
			auto aGroupRect = pwGroup->WindowRect().ScreenToClient( Wnd() );
			auto aFileSettingsRect = pwSettingsPage->WindowRect().ScreenToClient( Wnd() );
			auto aSeqRect = pwSeqPage->WindowRect().ScreenToClient( Wnd() );
			auto aOutputRect = m_pweopOutput->WindowRect().ScreenToClient( Wnd() );

			::MoveWindow( pwSeqPage->Wnd(), aGroupRect.left, aGroupRect.bottom, aSeqRect.Width(), aSeqRect.Height(), TRUE );
			::MoveWindow( pwSettingsPage->Wnd(), aSeqRect.Width() + aGroupRect.left * 3, aGroupRect.top, aFileSettingsRect.Width(), aFileSettingsRect.Height(), TRUE );
			::MoveWindow( m_pweopOutput->Wnd(), aSeqRect.Width() + aGroupRect.left * 3, aGroupRect.top + aFileSettingsRect.Height(), aOutputRect.Width(), aOutputRect.Height(), TRUE );


			aSeqRect = pwSeqPage->WindowRect().ScreenToClient( Wnd() );
			aOutputRect = m_pweopOutput->WindowRect().ScreenToClient( Wnd() );
			lMaxHeight = std::max( aSeqRect.top + aSeqRect.Height(), aOutputRect.top + aOutputRect.Height() );


			auto pwOk = FindChild( Layout::LSN_WEWI_OK );
			if ( pwOk ) {
				auto rOkRect = pwOk->WindowRect().ScreenToClient( Wnd() );
				::MoveWindow( pwOk->Wnd(), rOkRect.left, aOutputRect.bottom - rOkRect.Height(), aGroupRect.right - rOkRect.left, rOkRect.Height(), TRUE );
			}
			auto pwCncel = FindChild( Layout::LSN_WEWI_CANCEL );
			if ( pwCncel ) {
				auto rCancelRect = pwCncel->WindowRect().ScreenToClient( Wnd() );
				::MoveWindow( pwCncel->Wnd(), rCancelRect.left, aOutputRect.bottom - rCancelRect.Height(), rCancelRect.Width(), rCancelRect.Height(), TRUE );
			}

			aSeqRect = pwSeqPage->WindowRect();
			aFileSettingsRect = pwSettingsPage->WindowRect();
			auto aWindowRect = WindowRect();
			LSW_RECT aAdjusted = aGroupRect;
			aAdjusted.right = aAdjusted.left + aFileSettingsRect.right - aSeqRect.left;
			aAdjusted.bottom = aAdjusted.top + lMaxHeight;
			::AdjustWindowRectEx( &aAdjusted, GetStyle(), FALSE, GetStyleEx() );

			::MoveWindow( Wnd(), aWindowRect.left, aWindowRect.top, aAdjusted.Width() + aGroupRect.left * 2, aAdjusted.Height() + aGroupRect.left, TRUE );
		}

		UpdateRects();
		m_bInit = true;
		return LSW_H_CONTINUE;
	}

	/**
	 * The WM_CLOSE handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CWavEditorWindow::Close() {
		if ( m_pwParent ) {
			::PostMessageW( m_pwParent->Wnd(), CWinUtilities::LSN_CLOSE_WAV_EDITOR, 0, 0 );
		}
		::DestroyWindow( Wnd() );
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
	CWidget::LSW_HANDLED CWavEditorWindow::Command( WORD _wCtrlCode, WORD _wId, CWidget * _pwSrc ) {
		switch ( _wId ) {
			case Layout::LSN_WEWI_OK : {
				if ( Verify( false ) ) {
					// Save window settings and export.
					if ( Save( m_wewoWindowOptions, &m_weEditor ) ) {
						m_poOptions->wewoWavEditorWindow = m_wewoWindowOptions;
						//Close();
					}
					else {
						m_poOptions->wewoWavEditorWindow = m_wewoWindowOptions;
					}
				}
				break;
			}
			case Layout::LSN_WEWI_CANCEL : {
				//if ( Verify( false ) ) {
					Save( m_wewoWindowOptions, nullptr );
					m_poOptions->wewoWavEditorWindow = m_wewoWindowOptions;
				//}
				Close();
				break;
			}

			case Layout::LSN_WEWI_LOAD : {
				LoadProject();
				break;
			}
			case Layout::LSN_WEWI_SAVE : {
				SaveProject();
				break;
			}

			case Layout::LSN_WEWI_FILES_ADD_BUTTON : {}			LSN_FALLTHROUGH
			case Layout::LSN_WEWI_FILES_REMOVE_BUTTON : {}		LSN_FALLTHROUGH
			case Layout::LSN_WEWI_FILES_UP_BUTTON : {}			LSN_FALLTHROUGH
			case Layout::LSN_WEWI_FILES_DOWN_BUTTON : {
				return m_pwefFiles->Command( _wCtrlCode, _wId, _pwSrc );
			}
		}
		return LSW_H_CONTINUE;
	}

	/**
	 * Handles the WM_GETMINMAXINFO message.
	 * 
	 * \param _pmmiInfo The min/max info structure to fill out.
	 * \return Returns an LSW_HANDLED code.
	 **/
	CWidget::LSW_HANDLED CWavEditorWindow::GetMinMaxInfo( MINMAXINFO * _pmmiInfo ) {
		if ( m_bInit ) {
			_pmmiInfo->ptMinTrackSize.x = m_rStartingRect.Width();
			_pmmiInfo->ptMinTrackSize.y = m_rStartingRect.Height();
			_pmmiInfo->ptMaxTrackSize.x = _pmmiInfo->ptMinTrackSize.x;
			_pmmiInfo->ptMaxTrackSize.y = _pmmiInfo->ptMinTrackSize.y;
			return LSW_H_HANDLED;
		}
		return LSW_H_CONTINUE;
	}

	/**
	 * Load a .WAV file via the Add .WAV Files button.
	 * 
	 * \param _wsPath The path to the WAV file to load.  Spliced WAV files and associated metadata are also automatically loaded.
	 * \param _ui32Id The ID of the already-loaded file data.  If 0, the file will be added to the tree.
	 * \return Returns true if all given files are valid (exist and of the appropriate type).
	 **/
	bool CWavEditorWindow::AddWavFiles( const std::wstring &_wsPath, uint32_t _ui32Id ) {
		auto ui32Id = _ui32Id == 0 ? m_weEditor.AddWavFileSet( _wsPath ) : _ui32Id;
		auto pNoFile = std::filesystem::path( _wsPath ).remove_filename();
		if ( m_pwefFiles->AddToTree( ui32Id ) ) {
			if ( !m_pwespSequencing || !m_pwefspSettings ) { return false; }
			// New tracks start with the last-used values.
			LSN_TRACK tTrack;
			LSN_WAV_EDITOR_WINDOW_OPTIONS::LSN_PER_FILE pfDefault;
			const auto & pfLast = m_wewoWindowOptions.vPerFileOptions.size() ? m_wewoWindowOptions.vPerFileOptions[0] : pfDefault;
			m_pwespSequencing->Load( pfLast, tTrack.tSequencing );
			m_pwefspSettings->Load( pfLast, tTrack.tSettings );
			m_pwespSequencing->InitTrack( ui32Id, tTrack.tSequencing );
			m_pwefspSettings->InitTrack( ui32Id, tTrack.tSettings );
			try {
				m_mTracks[ui32Id] = std::move( tTrack );
			}
			catch ( ... ) { return false; }

			Update( true );
		}
		return true;
	}

	/**
	 * Removes files by unique ID.
	 * 
	 * \param _vIds The array of unique ID's to remove.
	 **/
	void CWavEditorWindow::Remove( const std::vector<LPARAM> &_vIds ) {
		for ( auto I = _vIds.size(); I--; ) {
			m_weEditor.RemoveFile( uint32_t( _vIds[I] ) );
			
			if ( uint32_t( _vIds[I] ) ) {
				m_mTracks.erase( uint32_t( _vIds[I] ) );
			}
		}
		Update( true );
	}

	/**
	 * Moves files up 1 by unique ID.
	 * 
	 * \param _vIds The array of unique ID's to move.
	 **/
	void CWavEditorWindow::MoveUp( const std::vector<LPARAM> &/*_vIds*/ ) {
		//m_weEditor.MoveUp( _vIds );
		Update();
	}

	/**
	 * Moves files down 1 by unique ID.
	 * 
	 * \param _vIds The array of unique ID's to move.
	 **/
	void CWavEditorWindow::MoveDown( const std::vector<LPARAM> &/*_vIds*/ ) {
		//m_weEditor.MoveDown( _vIds );
		Update();
	}

	/**
	 * Indicates that the file selection has changed.
	 * 
	 * \param _vIds The now-selected ID's.
	 **/
	void CWavEditorWindow::SelectionChanged( const std::vector<LPARAM> &/*_vIds*/ ) {
		Update( true );
	}

	/**
	 * Sets the stored sequencer edit text of the given ID in each given track.
	 * 
	 * \param _wId The ID of the edits to update.
	 * \param _wsText The text to apply.
	 * \param _vUpdateMe The unique ID's of the tracks to modify.
	 **/
	void CWavEditorWindow::SetAllSeqEditTexts( WORD _wId, const std::wstring &_wsText, const std::vector<LPARAM> &_vUpdateMe ) {
		for ( auto I = _vUpdateMe.size(); I--; ) {
			auto ptTrack = TrackById( uint32_t( _vUpdateMe[I] ) );
			auto pwsText = ptTrack ? ptTrack->tSequencing.Text( _wId ) : nullptr;
			if ( pwsText ) {
				(*pwsText) = _wsText;
			}
		}
	}

	/**
	 * Sets the stored sequencer check/radio state of the given ID in each given track.
	 * 
	 * \param _wId The ID of the checks/radios to update.
	 * \param _bChecked to check or not.
	 * \param _vUpdateMe The unique ID's of the tracks to modify.
	 **/
	void CWavEditorWindow::SetAllSeqCheckStates( WORD _wId, bool _bChecked, const std::vector<LPARAM> &_vUpdateMe ) {
		for ( auto I = _vUpdateMe.size(); I--; ) {
			auto ptTrack = TrackById( uint32_t( _vUpdateMe[I] ) );
			auto pbCheck = ptTrack ? ptTrack->tSequencing.Check( _wId ) : nullptr;
			if ( pbCheck ) {
				(*pbCheck) = _bChecked;
			}
		}
	}

	/**
	 * Sets the stored sequencer combo-box selection of the given ID in each given track.
	 * 
	 * \param _wId The ID of the combos to update.
	 * \param _i32Selection The index of the item to select within each combo box.
	 * \param _vUpdateMe The unique ID's of the tracks to modify.
	 **/
	void CWavEditorWindow::SetAllSeqComboSels( WORD _wId, int32_t _i32Selection, const std::vector<LPARAM> &_vUpdateMe ) {
		for ( auto I = _vUpdateMe.size(); I--; ) {
			auto ptTrack = TrackById( uint32_t( _vUpdateMe[I] ) );
			auto pi32Sel = ptTrack ? ptTrack->tSequencing.Selection( _wId ) : nullptr;
			if ( pi32Sel ) {
				(*pi32Sel) = _i32Selection;
			}
		}
	}

	/**
	 * Sets the stored settings edit text of the given ID in each given track.
	 * 
	 * \param _wId The ID of the edits to update.
	 * \param _wsText The text to apply.
	 * \param _vUpdateMe The unique ID's of the tracks to modify.
	 **/
	void CWavEditorWindow::SetAllSettingsEditTexts( WORD _wId, const std::wstring &_wsText, const std::vector<LPARAM> &_vUpdateMe ) {
		for ( auto I = _vUpdateMe.size(); I--; ) {
			auto ptTrack = TrackById( uint32_t( _vUpdateMe[I] ) );
			auto pwsText = ptTrack ? ptTrack->tSettings.Text( _wId ) : nullptr;
			if ( pwsText ) {
				(*pwsText) = _wsText;
			}
		}
	}

	/**
	 * Sets the stored settings check state of the given ID in each given track.
	 * 
	 * \param _wId The ID of the checks to update.
	 * \param _bChecked to check or not.
	 * \param _vUpdateMe The unique ID's of the tracks to modify.
	 **/
	void CWavEditorWindow::SetAllSettingsCheckStates( WORD _wId, bool _bChecked, const std::vector<LPARAM> &_vUpdateMe ) {
		for ( auto I = _vUpdateMe.size(); I--; ) {
			auto ptTrack = TrackById( uint32_t( _vUpdateMe[I] ) );
			auto pbCheck = ptTrack ? ptTrack->tSettings.Check( _wId ) : nullptr;
			if ( pbCheck ) {
				(*pbCheck) = _bChecked;
				// RF 1 and RF 2 are mutually exclusive, with RF 1 taking priority (as in CWavEditorFileSettingsPage::Update()).
				if ( ptTrack->tSettings.bRf1 ) { ptTrack->tSettings.bRf2 = false; }
			}
		}
	}

	/**
	 * Sets the stored settings combo-box selection of the given ID in each given track.
	 * 
	 * \param _wId The ID of the combos to update.
	 * \param _i32Selection The index of the item to select within each combo box.
	 * \param _vUpdateMe The unique ID's of the tracks to modify.
	 **/
	void CWavEditorWindow::SetAllSettingsComboSels( WORD _wId, int32_t _i32Selection, const std::vector<LPARAM> &_vUpdateMe ) {
		for ( auto I = _vUpdateMe.size(); I--; ) {
			auto ptTrack = TrackById( uint32_t( _vUpdateMe[I] ) );
			auto pi32Sel = ptTrack ? ptTrack->tSettings.Selection( _wId ) : nullptr;
			if ( pi32Sel ) {
				(*pi32Sel) = _i32Selection;
			}
		}
	}

	/**
	 * Gets the stored sequencer edit text of the given ID from each of the tracks specified by _vPages.  If the text values do not match exactly, the return string is empty.
	 * 
	 * \param _wId The ID of the edit control.
	 * \param _vPages The tracks to inspect for a string.
	 * \return Returns the string to set based on the stored text of each track.
	 **/
	std::wstring CWavEditorWindow::GetAllSeqEditTexts( WORD _wId, const std::vector<LPARAM> &_vPages ) {
		const std::wstring * pwsOut = nullptr;
		for ( auto I = _vPages.size(); I--; ) {
			auto ptTrack = TrackById( uint32_t( _vPages[I] ) );
			auto pwsText = ptTrack ? ptTrack->tSequencing.Text( _wId ) : nullptr;
			if ( pwsText ) {
				if ( !pwsOut ) {
					pwsOut = pwsText;
				}
				else if ( (*pwsText) != (*pwsOut) ) { return std::wstring(); }
			}
		}
		return pwsOut ? (*pwsOut) : std::wstring();
	}

	/**
	 * Gets the check state derived from the stored sequencer check/radio states of the given ID in each of the tracks specified by _vPages.  If any check/radio is checked, true is returned.
	 * 
	 * \param _wId The ID of the check/radio.
	 * \param _vPages The tracks to inspect.
	 * \return Returns the check state to use based on the stored check states of all involved tracks.
	 **/
	bool CWavEditorWindow::GetAllSeqCheckStates( WORD _wId, const std::vector<LPARAM> &_vPages ) {
		for ( auto I = _vPages.size(); I--; ) {
			auto ptTrack = TrackById( uint32_t( _vPages[I] ) );
			auto pbCheck = ptTrack ? ptTrack->tSequencing.Check( _wId ) : nullptr;
			if ( pbCheck && (*pbCheck) ) { return true; }
		}
		return false;
	}

	/**
	 * Gets the stored settings edit text of the given ID from each of the tracks specified by _vPages.  If the text values do not match exactly, the return string is empty.
	 * 
	 * \param _wId The ID of the edit control.
	 * \param _vPages The tracks to inspect for a string.
	 * \return Returns the string to set based on the stored text of each track.
	 **/
	std::wstring CWavEditorWindow::GetAllSettingsEditTexts( WORD _wId, const std::vector<LPARAM> &_vPages ) {
		const std::wstring * pwsOut = nullptr;
		for ( auto I = _vPages.size(); I--; ) {
			auto ptTrack = TrackById( uint32_t( _vPages[I] ) );
			auto pwsText = ptTrack ? ptTrack->tSettings.Text( _wId ) : nullptr;
			if ( pwsText ) {
				if ( !pwsOut ) {
					pwsOut = pwsText;
				}
				else if ( (*pwsText) != (*pwsOut) ) { return std::wstring(); }
			}
		}
		return pwsOut ? (*pwsOut) : std::wstring();
	}

	/**
	 * Gets the check state derived from the stored settings check states of the given ID in each of the tracks specified by _vPages.
	 * 
	 * \param _wId The ID of the check.
	 * \param _vPages The tracks to inspect.
	 * \return Returns BST_CHECKED or BST_UNCHECKED if all tracks match, otherwise BST_INDETERMINATE.
	 **/
	int CWavEditorWindow::GetAllSettingsCheckStates( WORD _wId, const std::vector<LPARAM> &_vPages ) {
		bool bFoundOne = false;
		int iValue = BST_INDETERMINATE;
		for ( auto I = _vPages.size(); I--; ) {
			auto ptTrack = TrackById( uint32_t( _vPages[I] ) );
			auto pbCheck = ptTrack ? ptTrack->tSettings.Check( _wId ) : nullptr;
			if ( pbCheck ) {
				int iTmp = (*pbCheck) ? BST_CHECKED : BST_UNCHECKED;
				if ( !bFoundOne ) {
					iValue = iTmp;
					bFoundOne = true;
				}
				else if ( iValue != iTmp ) { return BST_INDETERMINATE; }
			}
		}
		return iValue;
	}

	/**
	 * Gets the stored settings combo-box selection of the given ID in each of the tracks specified by _vPages.  If they are the same, the selection is returned, otherwise -1 is returned.
	 * 
	 * \param _wId The ID of the combo box.
	 * \param _vPages The tracks to inspect.
	 * \return Returns the index of the selection if all tracks match, otherwise -1.
	 **/
	int32_t CWavEditorWindow::GetAllSettingsComboSel( WORD _wId, const std::vector<LPARAM> &_vPages ) {
		bool bFoundOne = false;
		int32_t i32Value = -1;
		for ( auto I = _vPages.size(); I--; ) {
			auto ptTrack = TrackById( uint32_t( _vPages[I] ) );
			auto pi32Sel = ptTrack ? ptTrack->tSettings.Selection( _wId ) : nullptr;
			if ( pi32Sel ) {
				if ( !bFoundOne ) {
					i32Value = (*pi32Sel);
					bFoundOne = true;
				}
				else if ( i32Value != (*pi32Sel) ) { return -1; }
			}
		}
		return i32Value;
	}
		
	/**
	 * Updates the window.
	 * 
	 * \param _bSelchanged If true, the controls are refilled with the values of the selected track, or with the combined values of the tracks
	 *	being edited together if there is not exactly 1 selected track.
	 **/
	void CWavEditorWindow::Update( bool _bSelchanged ) {
		auto ptlTree = reinterpret_cast<CTreeListView *>(m_pwefFiles->FindChild( Layout::LSN_WEWI_FILES_TREELISTVIEW ));
		if ( ptlTree && m_pwespSequencing && m_pwefspSettings ) {
			std::vector<LPARAM> vSelected;
			ptlTree->GatherSelectedLParam( vSelected );
			// Remove nested selections.
			for ( auto I = vSelected.size(); I--; ) {
				if ( vSelected[I] & 0x80000000 ) {
					vSelected.erase( vSelected.begin() + I );
				}
			}
			auto ptTrack = vSelected.size() == 1 ? TrackById( uint32_t( vSelected[0] ) ) : nullptr;
			if ( ptTrack ) {
				// Select a specific WAV file.
				if ( _bSelchanged || m_pwespSequencing->UniqueId() != uint32_t( vSelected[0] ) ) {
					auto pwGroup = m_pwespSequencing->FindChild( Layout::LSN_WEWI_SEQ_GROUP );
					if ( pwGroup ) { pwGroup->SetTextW( LSN_LSTR( LSN_WE_SEQUENCING ) ); }
					pwGroup = m_pwefspSettings->FindChild( Layout::LSN_WEWI_SEQ_GROUP );
					if ( pwGroup ) { pwGroup->SetTextW( LSN_LSTR( LSN_WE_FILE_SETTINGS ) ); }

					m_pwespSequencing->LoadTrack( uint32_t( vSelected[0] ), ptTrack->tSequencing );
					m_pwefspSettings->LoadTrack( uint32_t( vSelected[0] ), ptTrack->tSettings );
				}
			}
			else {
				auto pwGroup = m_pwespSequencing->FindChild( Layout::LSN_WEWI_SEQ_GROUP );
				if ( pwGroup ) {
					std::wstring wsGroupSeq;
					if ( vSelected.size() ) {
						// Create the group name.
						size_t I = 0;
						for ( ; I < vSelected.size() && wsGroupSeq.size() < 50; ++I ) {
							if ( wsGroupSeq.size() ) {
								wsGroupSeq += L", ";
							}
							auto ptWavSet = m_weEditor.WavById( uint32_t( vSelected[I] ) );
							if ( ptWavSet ) {
								wsGroupSeq += std::filesystem::path( ptWavSet->wfFile.wsPath ).filename().generic_wstring();
							}
							else {
								wsGroupSeq += LSN_LSTR( LSN_ERROR );
							}
						}
						
						if ( I != vSelected.size() ) {
							wsGroupSeq += L"\u2026";
						}
					}
					else {
						wsGroupSeq = LSN_LSTR( LSN_WE_SEQUENCING_ALL );
					}
					pwGroup->SetTextW( wsGroupSeq.c_str() );
				}

				pwGroup = m_pwefspSettings->FindChild( Layout::LSN_WEWI_SEQ_GROUP );
				if ( pwGroup ) {
					std::wstring wsGroupSeq;
					if ( vSelected.size() ) {
						// Create the group name.
						size_t I = 0;
						for ( ; I < vSelected.size() && wsGroupSeq.size() < 50; ++I ) {
							if ( wsGroupSeq.size() ) {
								wsGroupSeq += L", ";
							}
							auto ptWavSet = m_weEditor.WavById( uint32_t( vSelected[I] ) );
							if ( ptWavSet ) {
								wsGroupSeq += std::filesystem::path( ptWavSet->wfFile.wsPath ).filename().generic_wstring();
							}
							else {
								wsGroupSeq += LSN_LSTR( LSN_ERROR );
							}
						}
						
						if ( I != vSelected.size() ) {
							wsGroupSeq += L"\u2026";
						}
					}
					else {
						wsGroupSeq = LSN_LSTR( LSN_WE_FILE_SETTINGS_ALL );
					}
					pwGroup->SetTextW( wsGroupSeq.c_str() );
				}


				if ( _bSelchanged || m_pwespSequencing->UniqueId() != 0 ) {
					m_pwespSequencing->Activate();
					m_pwefspSettings->Activate();
				}
			}
			m_pwespSequencing->Update();
			m_pwefspSettings->Update();
		}

		if ( m_pwefFiles ) {
			m_pwefFiles->Update();
		}
		if ( m_pweopOutput ) {
			m_pweopOutput->Update();
		}
	}

	/**
	 * Virtual client rectangle.  Can be used for things that need to be adjusted based on whether or not status bars, toolbars, etc. are present.
	 * 
	 * \param _pwChild The child whose virtual client is to be obtained or nullptr to obtain this window's.
	 * \return Returns the selected virtual client.
	 **/
	const LSW_RECT CWavEditorWindow::VirtualClientRect( const CWidget * /*_pwChild*/ ) const {
		LSW_RECT rTemp = ClientRect( this );
		const CRebar * plvRebar = static_cast<const CRebar *>(FindChild( Layout::LSN_WEWI_REBAR0 ));
		if ( plvRebar ) {
			LSW_RECT rRebar = plvRebar->ClientRect( this );
			rTemp.top += rRebar.Height();
		}
		return rTemp;
	}

	/**
	 * Saves the project.
	 **/
	void CWavEditorWindow::SaveProject() {
#define LSN_FILE_OPEN_FORMAT				LSN_LSTR( LSN_BWAV_FILES____BWAV____BWAV_ ) LSN_LSTR( LSN_ALL_FILES___________ ) L"\0"
		try {
			OPENFILENAMEW ofnOpenFile = { sizeof( ofnOpenFile ) };
			std::u16string szFileName;
			szFileName.resize( 0xFFFF + 2 );

			ofnOpenFile.hwndOwner = Wnd();
			ofnOpenFile.lpstrFilter = LSN_FILE_OPEN_FORMAT;
			ofnOpenFile.lpstrFile = reinterpret_cast<LPWSTR>(&szFileName[0]);
			ofnOpenFile.nMaxFile = DWORD( szFileName.size() );
			ofnOpenFile.nFilterIndex = 1;
			ofnOpenFile.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
			ofnOpenFile.lpstrInitialDir = m_wewoWindowOptions.wsLastProjectsFolder.c_str();
	
			std::u16string u16Path, u16FileName;
			if ( ::GetSaveFileNameW( &ofnOpenFile ) ) {
				m_wewoWindowOptions.wsLastProjectsFolder = std::filesystem::path( ofnOpenFile.lpstrFile ).remove_filename();
				auto pPath = std::filesystem::path( ofnOpenFile.lpstrFile );
				if ( !pPath.has_extension() ) {
					pPath += ".bwav";
				}

				LSN_WAV_EDITOR_WINDOW_OPTIONS wewoSaveMe;
				if ( !SaveProject( wewoSaveMe ) ) {
					lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_INTERNAL_ERROR ), LSN_LSTR( LSN_ERROR ) );
					return;
				}
				std::error_code ecErr;
				if ( !std::filesystem::create_directories( m_wewoWindowOptions.wsLastProjectsFolder, ecErr ) && ecErr ) {
					lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_FAILED_TO_CREATE_DIRECTORIES ), LSN_LSTR( LSN_ERROR ) );
					return;
				}
				CStdFile sfFile;
				if ( !sfFile.Create( pPath.generic_u16string().c_str() ) ) {
					lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_FAILED_TO_CREATE_FILE ), LSN_LSTR( LSN_ERROR ) );
					return;
				}
				CFileStream sStream( sfFile );

				// Header.
				if ( !sStream.WriteUi8( 'B' ) || !sStream.WriteUi8( 'W' ) || !sStream.WriteUi8( 'A' ) || !sStream.WriteUi8( 'V' ) ) {
					lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_FAILED_TO_WRITE_TO_FILE ), LSN_LSTR( LSN_ERROR ) );
					return;
				}
				// Version.
				if ( !sStream.WriteUi8( LSN_WAV_EDITOR_VERSION ) ) {
					lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_FAILED_TO_WRITE_TO_FILE ), LSN_LSTR( LSN_ERROR ) );
					return;
				}
				if ( !wewoSaveMe.Save( sStream ) ) {
					lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_FAILED_TO_WRITE_TO_FILE ), LSN_LSTR( LSN_ERROR ) );
					return;
				}
			}
		}
		catch ( ... ) {
			lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_OUT_OF_MEMORY ), LSN_LSTR( LSN_ERROR ) );
		}
#undef LSN_FILE_OPEN_FORMAT
	}

	/**
	 * Loads a project.
	 **/
	void CWavEditorWindow::LoadProject() {
#define LSN_FILE_OPEN_FORMAT				LSN_LSTR( LSN_BWAV_FILES____BWAV____BWAV_ ) LSN_LSTR( LSN_ALL_FILES___________ ) L"\0"

		OPENFILENAMEW ofnOpenFile = { sizeof( ofnOpenFile ) };
		std::u16string szFileName;
		szFileName.resize( 0xFFFF + 2 );

		ofnOpenFile.hwndOwner = Wnd();
		ofnOpenFile.lpstrFilter = LSN_FILE_OPEN_FORMAT;
		ofnOpenFile.lpstrFile = reinterpret_cast<LPWSTR>(&szFileName[0]);
		ofnOpenFile.nMaxFile = DWORD( szFileName.size() );
		ofnOpenFile.nFilterIndex = 1;
		ofnOpenFile.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
		ofnOpenFile.lpstrInitialDir = m_wewoWindowOptions.wsLastProjectsFolder.c_str();
	
		std::u16string u16Path, u16FileName;
		if ( ::GetOpenFileNameW( &ofnOpenFile ) ) {
			m_wewoWindowOptions.wsLastProjectsFolder = std::filesystem::path( ofnOpenFile.lpstrFile ).remove_filename();

			auto pPath = std::filesystem::path( ofnOpenFile.lpstrFile );
			CStdFile sfFile;
			if ( !sfFile.Open( pPath.generic_u16string().c_str() ) ) {
				lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_FAILED_TO_OPEN_FILE ), LSN_LSTR( LSN_ERROR ) );
				return;
			}

			CFileStream sStream( sfFile );
			uint8_t ui8Tmp;
			if ( !sStream.ReadUi8( ui8Tmp ) ) {
				lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_FAILED_TO_READ_FROM_FILE ), LSN_LSTR( LSN_ERROR ) );
				return;
			}
			if ( ui8Tmp != 'B' ) {
				lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_INVALID_FILE_FORMAT ), LSN_LSTR( LSN_ERROR ) );
				return;
			}

			if ( !sStream.ReadUi8( ui8Tmp ) ) {
				lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_FAILED_TO_READ_FROM_FILE ), LSN_LSTR( LSN_ERROR ) );
				return;
			}
			if ( ui8Tmp != 'W' ) {
				lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_INVALID_FILE_FORMAT ), LSN_LSTR( LSN_ERROR ) );
				return;
			}

			if ( !sStream.ReadUi8( ui8Tmp ) ) {
				lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_FAILED_TO_READ_FROM_FILE ), LSN_LSTR( LSN_ERROR ) );
				return;
			}
			if ( ui8Tmp != 'A' ) {
				lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_INVALID_FILE_FORMAT ), LSN_LSTR( LSN_ERROR ) );
				return;
			}

			if ( !sStream.ReadUi8( ui8Tmp ) ) {
				lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_FAILED_TO_READ_FROM_FILE ), LSN_LSTR( LSN_ERROR ) );
				return;
			}
			if ( ui8Tmp != 'V' ) {
				lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_INVALID_FILE_FORMAT ), LSN_LSTR( LSN_ERROR ) );
				return;
			}


			if ( !sStream.ReadUi8( ui8Tmp ) ) {
				lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_FAILED_TO_READ_FROM_FILE ), LSN_LSTR( LSN_ERROR ) );
				return;
			}
			if ( ui8Tmp > LSN_WAV_EDITOR_VERSION ) {
				lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_UNSUPPORTED_VERSION ), LSN_LSTR( LSN_ERROR ) );
				return;
			}

			LSN_WAV_EDITOR_WINDOW_OPTIONS wewoLoadMe;
			if ( !wewoLoadMe.Load( sStream, ui8Tmp ) ) {
				lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_INVALID_FILE_FORMAT ), LSN_LSTR( LSN_ERROR ) );
				return;
			}

			if ( m_weEditor.Total() ) {
				if ( !lsw::CBase::PromptYesNo( Wnd(), LSN_LSTR( LSN_WE_LOAD_VERIFY ), LSN_LSTR( LSN_CONTINUE ) ) ) {
					return;
				}
			}


			// TODO: Try to fix paths.
			if ( !m_weEditor.LoadFromStruct( wewoLoadMe ) ) {
				lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_WE_FAILED_TO_LOAD_PROJECT ), LSN_LSTR( LSN_ERROR ) );
				return;
			}


			

			m_mTracks.clear();
			if ( m_pwefFiles ) {
				auto ptlTree = reinterpret_cast<CTreeListView *>(m_pwefFiles->FindChild( Layout::LSN_WEWI_FILES_TREELISTVIEW ));
				ptlTree->DeleteAll();
			}

			// AddWavFiles
			if ( !wewoLoadMe.vPerFileOptions.size() ) { return; }

			for ( size_t I = 0; I < wewoLoadMe.vPerFileOptions.size(); ++I ) {
				auto pwfsSet = m_weEditor.WavByIdx( I );
				if ( !pwfsSet ) { return; }
				uint32_t ui32Id = pwfsSet->ui32Id;
				if ( wewoLoadMe.vPerFileOptions[I].vWavPaths.size() ) {
					AddWavFiles( wewoLoadMe.vPerFileOptions[I].vWavPaths[0], ui32Id );
					auto ptTrack = TrackById( ui32Id );
					if ( ptTrack ) {
						m_pwespSequencing->Load( wewoLoadMe.vPerFileOptions[I], ptTrack->tSequencing );
						m_pwefspSettings->Load( wewoLoadMe.vPerFileOptions[I], ptTrack->tSettings );
					}
				}
			}
			if ( m_pweopOutput ) {
				m_pweopOutput->Load( wewoLoadMe );
			}
			Update( true );
		}
#undef LSN_FILE_OPEN_FORMAT
	}

	/**
	 * Prepares to create the window.  Creates the atom if necessary.
	 **/
	void CWavEditorWindow::PrepareWavEditor() {
		if ( !m_aAtom ) {
			lsw::CWndClassEx wceEx;
			wceEx.SetInstance( lsw::CBase::GetThisHandle() );
			WCHAR szStr[23];
			lsn::CUtilities::RandomString( szStr, std::size( szStr ) );
			wceEx.SetClassName( szStr );
			wceEx.SetBackgroundBrush( reinterpret_cast<HBRUSH>(CTLCOLOR_DLG + 1) );
			wceEx.SetWindPro( CWidget::WindowProc );
			m_aAtom = lsw::CBase::RegisterClassExW( wceEx.Obj() );
		}
	}

	/**
	 * Verifies each of the dialog contents.
	 *
	 * \param _bSilent If true, errors are not reported.
	 * \return Returns true if no dialog failed verification and there is at least one loaded WAV file.
	 **/
	bool CWavEditorWindow::Verify( bool _bSilent ) {
		try {
			// TODO: Check for loaded file.
			CWidget * pwBaddy = nullptr;
			std::wstring wsError;
			if ( (pwBaddy = m_pwefFiles->Verify( wsError )) ) {
				if ( !_bSilent ) {
					lsw::CBase::MessageBoxError( Wnd(), wsError.c_str(), LSN_LSTR( LSN_ERROR ) );
					pwBaddy->SetFocus();
				}
				return false;
			}
			if ( (pwBaddy = m_pweopOutput->Verify( wsError )) ) {
				if ( !_bSilent ) {
					lsw::CBase::MessageBoxError( Wnd(), wsError.c_str(), LSN_LSTR( LSN_ERROR ) );
					pwBaddy->SetFocus();
				}
				return false;
			}
			for ( size_t I = 0; I < m_weEditor.Total(); ++I ) {
				auto pwfsSet = m_weEditor.WavByIdx( I );
				auto ptTrack = pwfsSet ? TrackById( pwfsSet->ui32Id ) : nullptr;
				if ( !ptTrack ) { continue; }
				pwBaddy = m_pwespSequencing->Verify( pwfsSet->ui32Id, ptTrack->tSequencing, wsError );
				if ( !pwBaddy ) {
					pwBaddy = m_pwefspSettings->Verify( ptTrack->tSettings, wsError );
				}
				if ( pwBaddy ) {
					if ( !_bSilent ) {
						lsw::CBase::MessageBoxError( Wnd(), wsError.c_str(), LSN_LSTR( LSN_ERROR ) );
						auto ptlTree = reinterpret_cast<CTreeListView *>(m_pwefFiles->FindChild( Layout::LSN_WEWI_FILES_TREELISTVIEW ));
						if ( ptlTree ) {
							ptlTree->UnselectAll();
							ptlTree->SetCurSelByItemData( pwfsSet->ui32Id );
						}
						// Show the bad track's values so the bad control can be fixed.
						Update( true );
						pwBaddy->SetFocus();
					}
					return false;
				}
			}
		}
		catch ( ... ) {
			lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_INTERNAL_ERROR ), LSN_LSTR( LSN_ERROR ) );
			return false;
		}

		return true;
	}

	/**
	 * Saves the window state and optionally filled out the structures to use for actually performing the operations.  Verify() SHOULD be called if only filling out the window state, but
	 *	MUST be called if filling out the execution state.
	 * 
	 * \param _wewoWindowState The window state to fill out.
	 * \param _pweEditor The optional execution state to fill out.
	 * \return Returns true if the files were all created successfully.
	 **/
	bool CWavEditorWindow::Save( LSN_WAV_EDITOR_WINDOW_OPTIONS &_wewoWindowState, CWavEditor * _pweEditor ) {
		try {
			// Order the WAV files.
			if ( m_pwefFiles && _pweEditor ) {
				auto ptlTree = reinterpret_cast<CTreeListView *>(m_pwefFiles->FindChild( Layout::LSN_WEWI_FILES_TREELISTVIEW ));
				if ( ptlTree ) {
					std::vector<LPARAM> vSelected;
					if ( ptlTree->GatherAllLParam( vSelected, true ) ) {
						// Remove extra WAVS and metadata.
						for ( auto I = vSelected.size(); I--; ) {
							if ( (vSelected[I] & 0x80000000) ) {
								vSelected.erase( vSelected.begin() + I );
							}
						}
						std::vector<uint32_t> vU32;
						vU32.reserve( vSelected.size() );

						// transform version
						std::transform( vSelected.begin(), vSelected.end(), std::back_inserter( vU32 ), []( LPARAM _lpParam ) {
							return static_cast<uint32_t>(_lpParam);
						});
						_pweEditor->Order( vU32 );
					}
				}
			}
			std::vector<CWavEditor::LSN_PER_FILE> vPerFile;
			CWavEditor::LSN_OUTPUT oOutput;
			vPerFile.resize( m_mTracks.size() );
			//m_pwefFiles->Save( _wewoWindowState, _pweEditor );
			m_pweopOutput->Save( _wewoWindowState, _pweEditor ? &oOutput : nullptr );
			size_t sIdx = 0;
			for ( auto & aTrack : m_mTracks ) {
				// The shown track's values are remembered for the next time.
				LSN_WAV_EDITOR_WINDOW_OPTIONS::LSN_PER_FILE pfTmp;
				bool bShown = aTrack.first == m_pwespSequencing->UniqueId() && _wewoWindowState.vPerFileOptions.size();
				auto & pfPerFileOptions = bShown ? _wewoWindowState.vPerFileOptions[0] : pfTmp;
				vPerFile[sIdx].ui32Id = aTrack.first;
				m_pwespSequencing->Save( aTrack.second.tSequencing, pfPerFileOptions, &vPerFile[sIdx] );
				m_pwefspSettings->Save( aTrack.first, aTrack.second.tSettings, pfPerFileOptions, &vPerFile[sIdx] );
				++sIdx;
			}
			if ( !m_mTracks.size() && _wewoWindowState.vPerFileOptions.size() ) {
				// With no tracks, the values in the controls are remembered.
				LSN_TRACK tTrack;
				m_pwespSequencing->SaveTrack( tTrack.tSequencing );
				m_pwefspSettings->SaveTrack( tTrack.tSettings );
				m_pwespSequencing->Save( tTrack.tSequencing, _wewoWindowState.vPerFileOptions[0], nullptr );
				m_pwefspSettings->Save( 0, tTrack.tSettings, _wewoWindowState.vPerFileOptions[0], nullptr );
			}

			if ( _pweEditor ) {
				_pweEditor->SetParms( vPerFile, oOutput );
				std::wstring wsError;
				if ( !_pweEditor->Execute( wsError ) ) {
					lsw::CBase::MessageBoxError( Wnd(), wsError.c_str(), LSN_LSTR( LSN_ERROR ) );
					return false;
				}
			}
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Saves the project.
	 * 
	 * \param _wewoWindowState The window state to fill out.
	 * \return Returns true if there are no weird memory errors.
	 **/
	bool CWavEditorWindow::SaveProject( LSN_WAV_EDITOR_WINDOW_OPTIONS &_wewoWindowState ) {
		try {
			// Order the WAV files.
			if ( m_pwefFiles ) {
				auto ptlTree = reinterpret_cast<CTreeListView *>(m_pwefFiles->FindChild( Layout::LSN_WEWI_FILES_TREELISTVIEW ));
				if ( ptlTree ) {
					std::vector<LPARAM> vSelected;
					if ( ptlTree->GatherAllLParam( vSelected, true ) ) {
						// Remove extra WAVS and metadata.
						for ( auto I = vSelected.size(); I--; ) {
							if ( (vSelected[I] & 0x80000000) ) {
								vSelected.erase( vSelected.begin() + I );
							}
						}
						std::vector<uint32_t> vU32;
						vU32.reserve( vSelected.size() );

						// transform version
						std::transform( vSelected.begin(), vSelected.end(), std::back_inserter( vU32 ), []( LPARAM _lpParam ) {
							return static_cast<uint32_t>(_lpParam);
						});
						m_weEditor.Order( vU32 );
					}
				}
			}

			if ( !m_weEditor.SaveToStruct( _wewoWindowState ) ) { return false; }

			for ( size_t I = 0; I < _wewoWindowState.vPerFileOptions.size(); ++I ) {
				auto ppfThis = m_weEditor.WavByIdx( I );
				if ( !ppfThis ) { return false; }
				auto ptTrack = TrackById( ppfThis->ui32Id );
				if ( !ptTrack ) { return false; }

				m_pwespSequencing->Save( ptTrack->tSequencing, _wewoWindowState.vPerFileOptions[I], nullptr );
				m_pwefspSettings->Save( ppfThis->ui32Id, ptTrack->tSettings, _wewoWindowState.vPerFileOptions[I], nullptr );
			}
			//m_pwefFiles->Save( _wewoWindowState, _pweEditor );
			m_pweopOutput->Save( _wewoWindowState, nullptr );
			return true;
		}
		catch ( ... ) { return false; }
	}

 }	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

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

		for ( size_t I = 0; I < LSN_ELEMENTS( sImages ); ++I ) {
			m_bBitmaps[sImages[I].dwConst].LoadFromResource( sImages[I].wImageName, IMAGE_BITMAP, 0, 0, LR_LOADTRANSPARENT | LR_CREATEDIBSECTION );
			m_iImageMap[sImages[I].dwConst] = m_iImages.Add( m_bBitmaps[sImages[I].dwConst].Handle() );
		}
	}
	CWavEditorWindow::~CWavEditorWindow() {
		for ( auto J = m_vSequencePages.size(); J--; ) {
			delete m_vSequencePages[J];
			m_vSequencePages.erase( m_vSequencePages.begin() + J );
		}
		for ( auto J = m_vSettingsPages.size(); J--; ) {
			delete m_vSettingsPages[J];
			m_vSettingsPages.erase( m_vSettingsPages.begin() + J );
		}
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

			plvToolBar->AddButtons( bButtons, LSN_ELEMENTS( bButtons ) );

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

		m_pwefFiles = static_cast<CWavEditorFilesPage *>(Layout::CreateFiles( this, m_wewoWindowOptions ));
		if ( !m_pwefFiles ) { return LSW_H_CONTINUE; }
		m_pwefFiles->SetWavEditor( m_weEditor );

		CWidget * pwSeqPage = Layout::CreateSequencer( this, m_wewoWindowOptions );
		if ( !pwSeqPage ) { return LSW_H_CONTINUE; }
		m_vSequencePages.push_back( static_cast<CWavEditorSequencingPage *>(pwSeqPage) );
		static_cast<CWavEditorSequencingPage *>(pwSeqPage)->SetWavEditorAndId( m_weEditor, 0, m_pwefFiles );

		CWidget * pwSettingsPage = Layout::CreateFileSettings( this, m_wewoWindowOptions );
		if ( !pwSettingsPage ) { return LSW_H_CONTINUE; }
		m_vSettingsPages.push_back( static_cast<CWavEditorFileSettingsPage *>(pwSettingsPage) );
		static_cast<CWavEditorFileSettingsPage *>(pwSettingsPage)->SetWavEditorAndId( m_weEditor, 0, m_pwefFiles );

		m_pweopOutput = static_cast<CWavEditorOutputPage *>(Layout::CreateOutput( this, m_wewoWindowOptions ));
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

			

			/*auto pwSeqGroup = pwSeqPage->FindChild( Layout::LSN_WEWI_SEQ_RANGE_GROUP );
			if ( pwSeqGroup ) {
				auto rTmpGrpRect = pwSeqGroup->WindowRect().ScreenToClient( Wnd() );

				

				LONG lTreeRight = 100;
				static const WORD dwIds[] = {
					Layout::LSN_WEWI_FILES_ADD_BUTTON,
					Layout::LSN_WEWI_FILES_ADD_META_BUTTON,
					Layout::LSN_WEWI_FILES_REMOVE_BUTTON,
					Layout::LSN_WEWI_FILES_UP_BUTTON,
					Layout::LSN_WEWI_FILES_DOWN_BUTTON,
				};
				for ( size_t I = 0; I < LSN_ELEMENTS( dwIds ); ++I ) {
					auto pwButton = m_pwefFiles->FindChild( dwIds[I] );
					if ( pwButton ) {
						auto rTmpThisRect = pwButton->WindowRect().ScreenToClient( Wnd() );
						lTreeRight = rTmpGrpRect.right - rTmpThisRect.Width() - aGroupRect.left;
						::MoveWindow( pwButton->Wnd(), rTmpGrpRect.right - rTmpThisRect.Width(), rTmpThisRect.top, rTmpThisRect.Width(), rTmpThisRect.Height(), TRUE );
					}
				}
				

				auto pwTree = m_pwefFiles->FindChild( Layout::LSN_WEWI_FILES_TREELISTVIEW );
				if ( pwTree ) {
					auto rTmpTreeRect = pwTree->WindowRect().ScreenToClient( Wnd() );
					
					::MoveWindow( pwTree->Wnd(), rTmpGrpRect.left, rTmpTreeRect.top, lTreeRight - rTmpGrpRect.left, rTmpTreeRect.Height(), TRUE );
				}
			}*/


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

			m_rSeqRect = pwSeqPage->WindowRect().ScreenToClient( Wnd() );
			m_rSetRect = pwSettingsPage->WindowRect().ScreenToClient( Wnd() );
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
				if ( Verify( true ) ) {
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
			CWidget * pwSeqPage = Layout::CreateSequencer( this, m_wewoWindowOptions );
			if ( !pwSeqPage ) { return false; }
			m_vSequencePages.push_back( static_cast<CWavEditorSequencingPage *>(pwSeqPage) );
			static_cast<CWavEditorSequencingPage *>(pwSeqPage)->SetWavEditorAndId( m_weEditor, ui32Id, m_pwefFiles );
			//if ( m_vSequencePages.size() == 2 ) {
				// Populate the 0th page.
				m_vSequencePages[0]->Activate();
			//}

			CWidget * pwSettingsPage = Layout::CreateFileSettings( this, m_wewoWindowOptions );
			if ( !pwSettingsPage ) { return false; }
			m_vSettingsPages.push_back( static_cast<CWavEditorFileSettingsPage *>(pwSettingsPage) );
			static_cast<CWavEditorFileSettingsPage *>(pwSettingsPage)->SetWavEditorAndId( m_weEditor, ui32Id, m_pwefFiles );
			m_vSettingsPages[0]->Activate();

			::MoveWindow( pwSeqPage->Wnd(), m_rSeqRect.left, m_rSeqRect.top, m_rSeqRect.Width(), m_rSeqRect.Height(), TRUE );
			::MoveWindow( pwSettingsPage->Wnd(), m_rSetRect.left, m_rSetRect.top, m_rSetRect.Width(), m_rSetRect.Height(), TRUE );
			Update();
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
				for ( auto J = m_vSequencePages.size(); J--; ) {
					if ( m_vSequencePages[J]->UniqueId() == uint32_t( _vIds[I] ) ) {
						delete m_vSequencePages[J];
						m_vSequencePages.erase( m_vSequencePages.begin() + J );
					}
				}
				for ( auto J = m_vSettingsPages.size(); J--; ) {
					if ( m_vSettingsPages[J]->UniqueId() == uint32_t( _vIds[I] ) ) {
						delete m_vSettingsPages[J];
						m_vSettingsPages.erase( m_vSettingsPages.begin() + J );
					}
				}
			}
		}
		Update();
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
	 * Sets the texts of all sequencer edits of the given ID.
	 * 
	 * \param _wId The ID of the edits to update.
	 * \param _wsText The text to apply.
	 * \param _vUpdateMe The unique ID's of the pages to modify.
	 **/
	void CWavEditorWindow::SetAllSeqEditTexts( WORD _wId, const std::wstring &_wsText, const std::vector<LPARAM> &_vUpdateMe ) {
		auto sSet = std::set<LPARAM>( _vUpdateMe.begin(), _vUpdateMe.end() );
		for ( auto I = m_vSequencePages.size(); I--; ) {
			auto aFindMe = std::set<LPARAM>::value_type( m_vSequencePages[I]->UniqueId() );
			if ( m_vSequencePages[I] && std::find( sSet.begin(), sSet.end(), aFindMe ) != sSet.end() ) {
				auto pwEdit = m_vSequencePages[I]->FindChild( _wId );
				if ( pwEdit ) {
					pwEdit->SetTextW( _wsText.c_str() );
					m_vSequencePages[I]->Update();
				}
			}
		}
	}

	/**
	 * Sets the check state of all sequencer checks/radio of the given ID.
	 * 
	 * \param _wId The ID of the edits to update.
	 * \param _bChecked to check or not.
	 * \param _vUpdateMe The unique ID's of the pages to modify.
	 **/
	void CWavEditorWindow::SetAllSeqCheckStates( WORD _wId, bool _bChecked, const std::vector<LPARAM> &_vUpdateMe ) {
		auto sSet = std::set<LPARAM>( _vUpdateMe.begin(), _vUpdateMe.end() );
		for ( auto I = m_vSequencePages.size(); I--; ) {
			auto aFindMe = std::set<LPARAM>::value_type( m_vSequencePages[I]->UniqueId() );
			if ( m_vSequencePages[I] && std::find( sSet.begin(), sSet.end(), aFindMe ) != sSet.end() ) {
				auto pwEdit = m_vSequencePages[I]->FindChild( _wId );
				if ( pwEdit ) {
					pwEdit->SetCheck( _bChecked );
					m_vSequencePages[I]->Update();
				}
			}
		}
	}

	/**
	 * Sets the texts of all settings edits of the given ID.
	 * 
	 * \param _wId The ID of the edits to update.
	 * \param _wsText The text to apply.
	 * \param _vUpdateMe The unique ID's of the pages to modify.
	 **/
	void CWavEditorWindow::SetAllSettingsEditTexts( WORD _wId, const std::wstring &_wsText, const std::vector<LPARAM> &_vUpdateMe ) {
		auto sSet = std::set<LPARAM>( _vUpdateMe.begin(), _vUpdateMe.end() );
		for ( auto I = m_vSettingsPages.size(); I--; ) {
			if ( m_vSettingsPages[I] && std::find( sSet.begin(), sSet.end(), m_vSettingsPages[I]->UniqueId() ) != sSet.end() ) {
				auto pwEdit = m_vSettingsPages[I]->FindChild( _wId );
				if ( pwEdit ) {
					pwEdit->SetTextW( _wsText.c_str() );
					m_vSettingsPages[I]->Update();
				}
			}
		}
	}

	/**
	 * Sets the check state of all settings checks/radio of the given ID.
	 * 
	 * \param _wId The ID of the edits to update.
	 * \param _bChecked to check or not.
	 * \param _vUpdateMe The unique ID's of the pages to modify.
	 **/
	void CWavEditorWindow::SetAllSettingsCheckStates( WORD _wId, bool _bChecked, const std::vector<LPARAM> &_vUpdateMe ) {
		auto sSet = std::set<LPARAM>( _vUpdateMe.begin(), _vUpdateMe.end() );
		for ( auto I = m_vSettingsPages.size(); I--; ) {
			if ( m_vSettingsPages[I] && std::find( sSet.begin(), sSet.end(), m_vSettingsPages[I]->UniqueId() ) != sSet.end() ) {
				auto pwEdit = m_vSettingsPages[I]->FindChild( _wId );
				if ( pwEdit ) {
					pwEdit->SetCheck( _bChecked );
					m_vSettingsPages[I]->Update();
				}
			}
		}
	}

	/**
	 * Sets the combo selections of all settings combos of the given ID.
	 * 
	 * \param _wId The ID of the combos to update.
	 * \param _lpSelection The data to select within each combo box.
	 * \param _vUpdateMe The unique ID's of the pages to modify.
	 * \param _lpBadSel In the case of conflicting data selections, this selection is returned.
	 **/
	void CWavEditorWindow::SetAllSettingsComboSels( WORD _wId, LPARAM _lpSelection, const std::vector<LPARAM> &_vUpdateMe ) {
		auto sSet = std::set<LPARAM>( _vUpdateMe.begin(), _vUpdateMe.end() );
		for ( auto I = m_vSettingsPages.size(); I--; ) {
			if ( m_vSettingsPages[I] && std::find( sSet.begin(), sSet.end(), m_vSettingsPages[I]->UniqueId() ) != sSet.end() ) {
				auto pwCombo = m_vSettingsPages[I]->FindChild( _wId );
				if ( pwCombo ) {
					pwCombo->SetCurSelByItemData( _lpSelection );
					m_vSettingsPages[I]->Update();
				}
			}
		}
	}

	/**
	 * Gets the text from all edits with the given ID on each of the pages specified by _vPages.  If the text values do not match exactly, the return string is empty.
	 * 
	 * \param _wId The ID of the edit controls to inspect on each page.
	 * \param _vPages The pages to inspect for a string.
	 * \return Returns the string to set based on the contents of each edit in each page.
	 **/
	std::wstring CWavEditorWindow::GetAllSeqEditTexts( WORD _wId, const std::vector<LPARAM> &_vPages ) {
		auto sSet = std::set<LPARAM>( _vPages.begin(), _vPages.end() );
		bool bFoundOne = false;
		std::wstring wsOut;
		for ( auto I = m_vSequencePages.size(); I--; ) {
			if ( std::find( sSet.begin(), sSet.end(), m_vSequencePages[I]->UniqueId() ) != sSet.end() ) {
				auto pwEdit = m_vSequencePages[I]->FindChild( _wId );
				if ( pwEdit ) {
					if ( !bFoundOne ) {
						wsOut = pwEdit->GetTextW();
						bFoundOne = true;
					}
					else {
						auto wsTmp = pwEdit->GetTextW();
						if ( wsTmp != wsOut ) { return std::wstring(); }
					}
				}
			}
		}
		return wsOut;
	}

	/**
	 * Gets the check state derived from the check states of each check/radio across all pages.  If any check/radio is checked, true is returned.
	 * 
	 * \param _wId The ID of the edit controls to inspect on each page.
	 * \param _vPages The pages to inspect for a string.
	 * \return Returns the check state to use based on the check states of all involved checks/radios across each page.
	 **/
	bool CWavEditorWindow::GetAllSeqCheckStates( WORD _wId, const std::vector<LPARAM> &_vPages ) {
		auto sSet = std::set<LPARAM>( _vPages.begin(), _vPages.end() );
		for ( auto I = m_vSequencePages.size(); I--; ) {
			if ( std::find( sSet.begin(), sSet.end(), m_vSequencePages[I]->UniqueId() ) != sSet.end() ) {
				auto pwCheck = m_vSequencePages[I]->FindChild( _wId );
				if ( pwCheck ) {
					if ( pwCheck->IsChecked() ) { return true; }
				}
			}
		}
		return false;
	}

	/**
	 * Gets the text from all edits with the given ID on each of the pages specified by _vPages.  If the text values do not match exactly, the return string is empty.
	 * 
	 * \param _wId The ID of the edit controls to inspect on each page.
	 * \param _vPages The pages to inspect for a string.
	 * \return Returns the string to set based on the contents of each edit in each page.
	 **/
	std::wstring CWavEditorWindow::GetAllSettingsEditTexts( WORD _wId, const std::vector<LPARAM> &_vPages ) {
		auto sSet = std::set<LPARAM>( _vPages.begin(), _vPages.end() );
		bool bFoundOne = false;
		std::wstring wsOut;
		for ( auto I = m_vSettingsPages.size(); I--; ) {
			if ( std::find( sSet.begin(), sSet.end(), m_vSettingsPages[I]->UniqueId() ) != sSet.end() ) {
				auto pwEdit = m_vSettingsPages[I]->FindChild( _wId );
				if ( pwEdit ) {
					if ( !bFoundOne ) {
						wsOut = pwEdit->GetTextW();
						bFoundOne = true;
					}
					else {
						auto wsTmp = pwEdit->GetTextW();
						if ( wsTmp != wsOut ) { return std::wstring(); }
					}
				}
			}
		}
		return wsOut;
	}

	/**
	 * Gets the check state derived from the check states of each check/radio across all pages.  If any check/radio is checked, true is returned.
	 * 
	 * \param _wId The ID of the edit controls to inspect on each page.
	 * \param _vPages The pages to inspect for a string.
	 * \return Returns the check state to use based on the check states of all involved checks/radios across each page.
	 **/
	int CWavEditorWindow::GetAllSettingsCheckStates( WORD _wId, const std::vector<LPARAM> &_vPages ) {
		auto sSet = std::set<LPARAM>( _vPages.begin(), _vPages.end() );
		bool bFoundOne = false;
		int iValue = BST_INDETERMINATE;
		for ( auto I = m_vSettingsPages.size(); I--; ) {
			if ( std::find( sSet.begin(), sSet.end(), m_vSettingsPages[I]->UniqueId() ) != sSet.end() ) {
				auto pwCheck = m_vSettingsPages[I]->FindChild( _wId );
				if ( pwCheck ) {
					if ( !bFoundOne ) {
						iValue = pwCheck->IsChecked() ? BST_CHECKED : BST_UNCHECKED;
						bFoundOne = true;
					}
					else {
						int iTmp = pwCheck->IsChecked() ? BST_CHECKED : BST_UNCHECKED;
						if ( iValue != iTmp ) { return BST_INDETERMINATE; }
					}
				}
			}
		}
		return iValue;
	}

	/**
	 * Gets the selected item of each combobox across all pages.  If they are the same, the value is returned, otherwise _lpBad is returned.
	 * 
	 * \param _wId The ID of the combobox controls to inspect on each page.
	 * \param _vPages The pages to inspect for a string.
	 * \param _lpBad The value to return upon inconsistent combobox selections.
	 * \return Returns the combobox data selection if all combos match, otherwise _lpBad.
	 **/
	LPARAM CWavEditorWindow::GetAllSettingsComboSel( WORD _wId, const std::vector<LPARAM> &_vPages, LPARAM _lpBad ) {
		auto sSet = std::set<LPARAM>( _vPages.begin(), _vPages.end() );
		bool bFoundOne = false;
		LPARAM lpValue = _lpBad;
		for ( auto I = m_vSettingsPages.size(); I--; ) {
			if ( std::find( sSet.begin(), sSet.end(), m_vSettingsPages[I]->UniqueId() ) != sSet.end() ) {
				auto pwComboBox = m_vSettingsPages[I]->FindChild( _wId );
				if ( pwComboBox ) {
					if ( !bFoundOne ) {
						lpValue = pwComboBox->GetCurSelItemData();
						bFoundOne = true;
					}
					else {
						if ( lpValue != pwComboBox->GetCurSelItemData() ) { return _lpBad; }
					}
				}
			}
		}
		return lpValue;
	}
		
	/**
	 * Updates the window.
	 * 
	 * \param _bSelchanged If true, thew newly focused sequencer page will receive a notification to tell it that it has just become active, which it will use
	 *	to gather the text from all the pages it affects to decide how to fill in its own edit texts.
	 **/
	void CWavEditorWindow::Update( bool _bSelchanged ) {
		auto ptlTree = reinterpret_cast<CTreeListView *>(m_pwefFiles->FindChild( Layout::LSN_WEWI_FILES_TREELISTVIEW ));
		if ( ptlTree ) {
			std::vector<LPARAM> vSelected;
			ptlTree->GatherSelectedLParam( vSelected );
			// Remove nested selections.
			for ( auto I = vSelected.size(); I--; ) {
				if ( vSelected[I] & 0x80000000 ) {
					vSelected.erase( vSelected.begin() + I );
				}
			}
			if ( vSelected.size() == 1 ) {
				// Select a specific WAV file.
				for ( auto I = m_vSequencePages.size(); I--; ) {
					m_vSequencePages[I]->SetVisible( m_vSequencePages[I]->UniqueId() == uint32_t( vSelected[0] ) );
				}
				for ( auto I = m_vSettingsPages.size(); I--; ) {
					m_vSettingsPages[I]->SetVisible( m_vSettingsPages[I]->UniqueId() == uint32_t( vSelected[0] ) );
				}
			}
			else {
				// Select a specific WAV file.
				for ( auto I = m_vSequencePages.size(); I--; ) {
					m_vSequencePages[I]->SetVisible( I == 0 );
				}
				for ( auto I = m_vSettingsPages.size(); I--; ) {
					m_vSettingsPages[I]->SetVisible( I == 0 );
				}

				auto pwGroup = m_vSequencePages[0]->FindChild( Layout::LSN_WEWI_SEQ_GROUP );
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
								wsGroupSeq += L"Error";
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

				pwGroup = m_vSettingsPages[0]->FindChild( Layout::LSN_WEWI_SEQ_GROUP );
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
								wsGroupSeq += L"Error";
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


				for ( auto I = m_vSequencePages.size(); I--; ) {
					if ( m_vSequencePages[I]->Visible() ) {
						if ( _bSelchanged && m_vSequencePages[I]->UniqueId() == 0 ) {
							m_vSequencePages[I]->Activate();
						}
						m_vSequencePages[I]->Update();
					}
				}
				for ( auto I = m_vSettingsPages.size(); I--; ) {
					if ( m_vSettingsPages[I]->Visible() ) {
						if ( _bSelchanged && m_vSettingsPages[I]->UniqueId() == 0 ) {
							m_vSettingsPages[I]->Activate();
						}
						m_vSettingsPages[I]->Update();
					}
				}
			}
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


			

			if ( m_pwefFiles ) {
				auto ptlTree = reinterpret_cast<CTreeListView *>(m_pwefFiles->FindChild( Layout::LSN_WEWI_FILES_TREELISTVIEW ));
				ptlTree->DeleteAll();
			}
			for ( size_t I = 1; I < m_vSequencePages.size(); ++I ) {
				delete m_vSequencePages[I];
				m_vSequencePages[I] = nullptr;
			}
			m_vSequencePages.resize( 1 );
			for ( size_t I = 1; I < m_vSettingsPages.size(); ++I ) {
				delete m_vSettingsPages[I];
				m_vSettingsPages[I] = nullptr;
			}
			m_vSettingsPages.resize( 1 );

			// AddWavFiles
			if ( !wewoLoadMe.vPerFileOptions.size() ) { return; }

			for ( size_t I = 0; I < wewoLoadMe.vPerFileOptions.size(); ++I ) {
				auto pwfsSet = m_weEditor.WavByIdx( I );
				if ( !pwfsSet ) { return; }
				uint32_t ui32Id = pwfsSet->ui32Id;
				if ( wewoLoadMe.vPerFileOptions[I].vWavPaths.size() ) {
					AddWavFiles( wewoLoadMe.vPerFileOptions[I].vWavPaths[0], ui32Id );
					auto pwespSeqPage = SequencePageById( ui32Id );
					auto pwespSetPage = SettingsPageById( ui32Id );
					if ( pwespSeqPage ) {
						pwespSeqPage->Load( wewoLoadMe.vPerFileOptions[I] );
						pwespSeqPage->Update();
					}
					if ( pwespSetPage ) {
						pwespSetPage->Load( wewoLoadMe.vPerFileOptions[I] );
						pwespSetPage->Update();
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
			lsn::CUtilities::RandomString( szStr, LSN_ELEMENTS( szStr ) );
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
			for ( size_t I = 1; I < m_vSequencePages.size(); ++I ) {
				if ( (pwBaddy = m_vSequencePages[I]->Verify( wsError )) ) {
					if ( !_bSilent ) {
						lsw::CBase::MessageBoxError( Wnd(), wsError.c_str(), LSN_LSTR( LSN_ERROR ) );
						auto ptlTree = reinterpret_cast<CTreeListView *>(m_pwefFiles->FindChild( Layout::LSN_WEWI_FILES_TREELISTVIEW ));
						if ( ptlTree ) {
							ptlTree->UnselectAll();
							ptlTree->SetCurSelByItemData( m_vSequencePages[I]->UniqueId() );
						}
						Update();
						pwBaddy->SetFocus();
					}
					return false;
				}
			}

			for ( size_t I = 1; I < m_vSettingsPages.size(); ++I ) {
				if ( (pwBaddy = m_vSettingsPages[I]->Verify( wsError )) ) {
					if ( !_bSilent ) {
						lsw::CBase::MessageBoxError( Wnd(), wsError.c_str(), LSN_LSTR( LSN_ERROR ) );
						auto ptlTree = reinterpret_cast<CTreeListView *>(m_pwefFiles->FindChild( Layout::LSN_WEWI_FILES_TREELISTVIEW ));
						if ( ptlTree ) {
							ptlTree->UnselectAll();
							ptlTree->SetCurSelByItemData( m_vSequencePages[I]->UniqueId() );
						}
						Update();
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
			vPerFile.resize( std::max( m_vSequencePages.size(), m_vSettingsPages.size() ) );
			//m_pwefFiles->Save( _wewoWindowState, _pweEditor );
			m_pweopOutput->Save( _wewoWindowState, _pweEditor ? &oOutput : nullptr );
			for ( size_t I = 1; I < m_vSequencePages.size(); ++I ) {
				vPerFile[I].ui32Id = m_vSequencePages[I]->UniqueId();
				auto aTmp = _wewoWindowState.vPerFileOptions[0];
				m_vSequencePages[I]->Save( (m_vSequencePages[I]->Visible() ? _wewoWindowState.vPerFileOptions[0] : aTmp), &vPerFile[I] );
			}
			if ( m_vSequencePages.size() == 1 ) {
				m_vSequencePages[0]->Save( _wewoWindowState.vPerFileOptions[0], nullptr );
			}

			for ( size_t I = 1; I < m_vSettingsPages.size(); ++I ) {
				CWavEditor::LSN_PER_FILE * pfPerFile = nullptr;
				for ( auto J = vPerFile.size(); J--; ) {
					if ( vPerFile[J].ui32Id == m_vSettingsPages[I]->UniqueId() ) {
						pfPerFile = &vPerFile[J];
						break;
					}
				}
				auto aTmp = _wewoWindowState.vPerFileOptions[0];
				m_vSettingsPages[I]->Save( (m_vSequencePages[I]->Visible() ? _wewoWindowState.vPerFileOptions[0] : aTmp), pfPerFile );
			}
			if ( m_vSettingsPages.size() == 1 ) {
				m_vSettingsPages[0]->Save( _wewoWindowState.vPerFileOptions[0], nullptr );
			}

			vPerFile.erase( vPerFile.begin() );
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
				auto pwespSeqPage = SequencePageById( ppfThis->ui32Id );
				auto pwespSetPage = SettingsPageById( ppfThis->ui32Id );
				if ( !pwespSeqPage || !pwespSetPage ) { return false; }

				pwespSeqPage->Save( _wewoWindowState.vPerFileOptions[I], nullptr );
				pwespSetPage->Save( _wewoWindowState.vPerFileOptions[I], nullptr );
			}
			//m_pwefFiles->Save( _wewoWindowState, _pweEditor );
			m_pweopOutput->Save( _wewoWindowState, nullptr );
			return true;
		}
		catch ( ... ) { return false; }
	}

 }	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

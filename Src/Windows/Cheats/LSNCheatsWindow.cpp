#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A template for dialog pages embedded into other dialogs/windows.
 */

#include "LSNCheatsWindow.h"
#include "LSNCheatEditWindowLayout.h"
#include "../../Cheats/LSNCheatConverter.h"
#include "../../File/LSNStdFile.h"
#include "../../Localization/LSNLocalization.h"
#include "../Layout/LSNLayoutMacros.h"
#include "../Layout/LSNLayoutManager.h"

#include <Rebar/LSWRebar.h>
#include <ToolBar/LSWToolBar.h>
#include <TreeListView/LSWTreeListView.h>

#include "../../../resource.h"

#include <Helpers/LSWHelpers.h>

#include <commdlg.h>
#include <cwchar>
#include <filesystem>
#include <map>

namespace lsn {

	CCheatsWindow::CCheatsWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pcmCheatManager( reinterpret_cast<CCheatManager *>(_ui64Data) ) {

		{
			static const struct {
				WORD							wImageName;
				DWORD							dwConst;
			} sImages[] = {
				{ IDB_CHEAT_OPEN_FILE_16,		LSN_I_IMPORT },
				{ IDB_CHEAT_SAVE_16,			LSN_I_EXPORT },
				{ IDB_CHEAT_ADD_16,				LSN_I_ADD },
				{ IDB_CHEAT_DELETE_16,			LSN_I_DELETE },
				{ IDB_CHEAT_EDIT_16,			LSN_I_EDIT },
				{ IDB_CHEAT_DUPLICATE_16,		LSN_I_DUPLICATE },
				{ IDB_CHEAT_FILTER_16,			LSN_I_FILTER },
				{ IDB_CHEAT_SEARCH_16,			LSN_I_SEARCH },
			};
			m_iImages.Create( 16, 16, ILC_COLOR32, LSN_I_TOTAL, LSN_I_TOTAL );
			for ( size_t I = 0; I < std::size( sImages ); ++I ) {
				m_bBitmaps16[sImages[I].dwConst].LoadFromResource( sImages[I].wImageName, IMAGE_BITMAP, 0, 0, LR_LOADTRANSPARENT | LR_CREATEDIBSECTION );
				m_iImageMap[sImages[I].dwConst] = m_iImages.Add( m_bBitmaps16[sImages[I].dwConst].Handle() );
			}
		}
	}

	// == Functions.
	/**
	 * The WM_INITDIALOG handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CCheatsWindow::InitDialog() {
		Parent::InitDialog();
		LSW_SETREDRAW srRedraw( this );

		SetIcons( reinterpret_cast<HICON>(::LoadImageW( CBase::GetModuleHandleW( nullptr ), MAKEINTRESOURCEW( IDI_SHIELD_16 ), IMAGE_ICON, 0, 0, LR_LOADTRANSPARENT )),
			reinterpret_cast<HICON>(::LoadImageW( CBase::GetModuleHandleW( nullptr ), MAKEINTRESOURCEW( IDI_SHIELD_32 ), IMAGE_ICON, 0, 0, LR_LOADTRANSPARENT )) );


		CToolBar * plvToolBar = static_cast<CToolBar *>(FindChild( Layout::LSN_CWI_TOOLBAR ));
		CRebar * plvRebar = static_cast<CRebar *>(FindChild( Layout::LSN_CWI_REBAR ));

		// ==== TOOL BAR ==== //
		if ( plvToolBar ) {
			plvToolBar->SetImageList( 0, m_iImages );

#define LSN_TOOL_STR( TXT )					reinterpret_cast<INT_PTR>(TXT)
//#define LSN_TOOL_STR( TXT )					0
			const TBBUTTON bButtons[] = {
				// iBitmap									idCommand									fsState				fsStyle			bReserved	dwData	iString
				{ m_iImageMap[LSN_I_IMPORT],				Layout::LSN_CWI_IMPORT_BUTTON,				TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		LSN_TOOL_STR( LSN_LSTR( LSN_STR_IMPORT ) ) },
				{ m_iImageMap[LSN_I_EXPORT],				Layout::LSN_CWI_EXPORT_BUTTON,				TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		LSN_TOOL_STR( LSN_LSTR( LSN_STR_EXPORT_SELECTED ) ) },
				{ -1,										0,											TBSTATE_ENABLED,	BTNS_SEP,		{ 0 },		0,		0 },
				{ m_iImageMap[LSN_I_ADD],					Layout::LSN_CWI_ADD_BUTTON,					TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		LSN_TOOL_STR( LSN_LSTR( LSN_STR_ADD ) ) },
				{ m_iImageMap[LSN_I_DELETE],				Layout::LSN_CWI_DELETE_BUTTON,				TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		LSN_TOOL_STR( LSN_LSTR( LSN_STR_DELETE ) ) },
				{ -1,										0,											TBSTATE_ENABLED,	BTNS_SEP,		{ 0 },		0,		0 },
				{ m_iImageMap[LSN_I_EDIT],					Layout::LSN_CWI_EDIT_BUTTON,				TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		LSN_TOOL_STR( LSN_LSTR( LSN_STR_EDIT ) ) },
				{ m_iImageMap[LSN_I_DUPLICATE],				Layout::LSN_CWI_DUPLICATE_BUTTON,			TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		LSN_TOOL_STR( LSN_LSTR( LSN_STR_DUPLICATE ) ) },
				{ -1,										0,											TBSTATE_ENABLED,	BTNS_SEP,		{ 0 },		0,		0 },
				/*{ m_iImageMap[LSN_I_FILTER],				Layout::LSN_CWI_FILTER_BUTTON,				TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		LSN_TOOL_STR( LSN_LSTR( LSN_STR_FILTER ) ) },
				{ m_iImageMap[LSN_I_SEARCH],				Layout::LSN_CWI_SEARCH_BUTTON,				TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		LSN_TOOL_STR( LSN_LSTR( LSN_STR_SEARCH ) ) },*/
				{ 200,										Layout::LSN_CWI_FILTER_BUTTON_PLACHOLDER,	TBSTATE_ENABLED,	BTNS_SEP,		{ 0 },		0,		0 },
				{ 200,										Layout::LSN_CWI_SEARCH_BUTTON_PLACHOLDER,	TBSTATE_ENABLED,	BTNS_SEP,		{ 0 },		0,		0 },
			};
#undef LSN_TOOL_STR

			plvToolBar->AddButtons( bButtons, std::size( bButtons ) );
			{
				CWidget * pwEdit;
				if ( plvToolBar->CreateIconEditBox( Layout::LSN_CWI_FILTER_BUTTON_PLACHOLDER, Layout::LSN_CWI_FILTER_BUTTON,
					IMAGE_BITMAP, m_bBitmaps16[LSN_I_FILTER], LSN_LSTR( LSN_STR_FILTER_ ), pwEdit ) ) {
					m_peFilterEdit = reinterpret_cast<CEdit *>(pwEdit);
					if ( m_pcmCheatManager ) {
						m_peFilterEdit->SetTextW( m_pcmCheatManager->GetCheatFilter().c_str() );
					}
				}
			}
			{
				CWidget * pwEdit;
				if ( plvToolBar->CreateIconEditBox( Layout::LSN_CWI_SEARCH_BUTTON_PLACHOLDER, Layout::LSN_CWI_SEARCH_BUTTON,
					IMAGE_BITMAP, m_bBitmaps16[LSN_I_SEARCH], LSN_LSTR( LSN_STR_SEARCH_ ), pwEdit ) ) {
					m_peSearchEdit = reinterpret_cast<CEdit *>(pwEdit);
					if ( m_pcmCheatManager ) {
						// The search from the last time the dialog was open.  m_wsSearch is set first so that the EN_CHANGE this causes does
						//	not rebuild the list.
						m_wsSearch = CUtilities::ToLower( m_pcmCheatManager->GetCheatSearch() );
						m_peSearchEdit->SetTextW( m_pcmCheatManager->GetCheatSearch().c_str() );
					}
				}
			}

			if ( plvRebar ) {
				plvRebar->SetImageList( m_iImages );
				{
					LSW_REBARBANDINFO riRebarInfo;
					riRebarInfo.SetColors( ::GetSysColor( COLOR_BTNTEXT ), ::GetSysColor( COLOR_BTNFACE ) );
					riRebarInfo.SetStyle( RBBS_CHILDEDGE |
						RBBS_FIXEDBMP );
					riRebarInfo.SetChild( plvToolBar->Wnd() );
					riRebarInfo.SetChildSize( plvToolBar->GetMinBoundingRect().Width(), plvToolBar->GetMinBoundingRect().Height() );
					riRebarInfo.SetId( Layout::LSN_CWI_TOOLBAR );
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
		// Move all child controls down.
		lsw::LSW_RECT rThisRect = WindowRect();
		for ( size_t I = 0; I < m_vChildren.size(); ++I ) {
			if ( m_vChildren[I]->Id() != Layout::LSN_CWI_TOOLBAR && m_vChildren[I]->Id() != Layout::LSN_CWI_REBAR ) {
				lsw::LSW_RECT rRect = m_vChildren[I]->WindowRect().ScreenToClient( Wnd() );
			
				::MoveWindow( m_vChildren[I]->Wnd(), int( rRect.left ), int( rRect.top + rRebarRect.Height() ), int( rRect.Width() ), int( rRect.Height() ), FALSE );
			}
		}
		{
			::MoveWindow( Wnd(), int( rThisRect.left ), int( rThisRect.top ), int( rThisRect.Width() ), int( rThisRect.Height() + rRebarRect.Height() ), FALSE );
		}
		FitToToolBar();

		{
			auto ptlvTree = reinterpret_cast<CTreeListView *>(FindChild( Layout::LSN_CWI_CHEAT_TREELISTVIEW ));
			if ( ptlvTree ) {
				ptlvTree->SetColumnText( LSN_LSTR( LSN_STR_CODES ), 0 );
				ptlvTree->SetColumnWidth( 0, 287 );
				ptlvTree->InsertColumn( LSN_LSTR( LSN_STR_HOTKEY_S_ ), 100, -1 );
				ptlvTree->FitColumndsToControlWidth( 0 );

				AddCheats();
				ptlvTree->FitColumndsToControlWidth( 1 );
			}
		}

		
		CWidget * pwCodes = FindChild( Layout::LSN_CWI_CODE_EDIT );
		if ( pwCodes ) {
			pwCodes->SetSystemFixedFont();
			::SendMessageW( pwCodes->Wnd(), EM_SETCUEBANNER, FALSE, reinterpret_cast<LPARAM>(LSN_LSTR( LSN_STR_CODE_S_ ) ) );
		}
		CWidget * pwNotes = FindChild( Layout::LSN_CWI_NOTES_EDIT );
		if ( pwNotes ) {
			::SendMessageW( pwNotes->Wnd(), EM_SETCUEBANNER, FALSE, reinterpret_cast<LPARAM>(LSN_LSTR( LSN_STR_NOTES ) ) );
		}

		UpdateRects();
		UpdateSelection();
		return LSW_H_CONTINUE;
	}

	/**
	 * The WM_CLOSE handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CCheatsWindow::Close() {
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
	CWidget::LSW_HANDLED CCheatsWindow::Command( WORD _wCtrlCode, WORD _wId, CWidget * _pwSrc ) {
		switch ( _wId ) {
			case Layout::LSN_CWI_CANCEL : {
				return Close();
			}
			case Layout::LSN_CWI_OK : {
				::EndDialog( Wnd(), 1 );
				return LSW_H_HANDLED;
			}
			case Layout::LSN_CWI_FILTER_BUTTON : {
				if ( _pwSrc && _wCtrlCode == EN_CHANGE && m_pcmCheatManager ) {
					auto wsFilter = _pwSrc->GetTextW();
					m_pcmCheatManager->SetCheatFilter( wsFilter );
					AddCheats();
				}
				break;
			}
			case Layout::LSN_CWI_SEARCH_BUTTON : {
				if ( _pwSrc && _wCtrlCode == EN_CHANGE ) {
					std::wstring wsText = _pwSrc->GetTextW();
					if ( m_pcmCheatManager ) { m_pcmCheatManager->SetCheatSearch( wsText ); }
					std::wstring wsSearch = CUtilities::ToLower( wsText );
					if ( wsSearch != m_wsSearch ) {
						m_wsSearch = std::move( wsSearch );
						AddCheats();
					}
				}
				break;
			}

			case Layout::LSN_CWI_IMPORT_BUTTON : {
				ImportCheats();
				break;
			}
			case Layout::LSN_CWI_EXPORT_BUTTON : {
				ExportCheats();
				break;
			}
			case Layout::LSN_CWI_ADD_BUTTON : {
				AddCheat();
				break;
			}
			case Layout::LSN_CWI_EDIT_BUTTON : {
				EditCheat( false );
				break;
			}
			case Layout::LSN_CWI_DUPLICATE_BUTTON : {
				EditCheat( true );
				break;
			}
			case Layout::LSN_CWI_DELETE_BUTTON : {
				DeleteCheats();
				break;
			}
			case Layout::LSN_CWI_ACTIVATE_BUTTON : {
				ActivateCheats();
				break;
			}
			case Layout::LSN_CWI_ACTIVATE_WITH_HOTKEY_BUTTON : {
				ActivateWithHotkey();
				break;
			}
			case Layout::LSN_CWI_CLEAR_HOTKEYS : {
				ClearHotkeys();
				break;
			}
			case Layout::LSN_CWI_DISABLE_ALL : {
				DisableAll();
				break;
			}

			case Layout::LSN_CWI_SHOW_ENABLED : {
				RevealEnabled();
				break;
			}
			case Layout::LSN_CWI_SELECT_ENABLED : {
				RevealEnabled( true );
				break;
			}
			case Layout::LSN_CWI_EXPAND_SELECTED : {
				lsw::CTreeListView * ptlvTree = static_cast<lsw::CTreeListView *>(FindChild( Layout::LSN_CWI_CHEAT_TREELISTVIEW ));
				if ( ptlvTree ) {
					ptlvTree->ExpandSelected();
				}
				break;
			}
			case Layout::LSN_CWI_EXPAND_ALL : {
				lsw::CTreeListView * ptlvTree = static_cast<lsw::CTreeListView *>(FindChild( Layout::LSN_CWI_CHEAT_TREELISTVIEW ));
				if ( ptlvTree ) {
					ptlvTree->ExpandAll();
				}
				break;
			}
			case Layout::LSN_CWI_COLLAPSE_SELECTED : {
				lsw::CTreeListView * ptlvTree = static_cast<lsw::CTreeListView *>(FindChild( Layout::LSN_CWI_CHEAT_TREELISTVIEW ));
				if ( ptlvTree ) {
					ptlvTree->CollapseSelected();
				}
				break;
			}
			case Layout::LSN_CWI_COLLAPSE_ALL : {
				lsw::CTreeListView * ptlvTree = static_cast<lsw::CTreeListView *>(FindChild( Layout::LSN_CWI_CHEAT_TREELISTVIEW ));
				if ( ptlvTree ) {
					ptlvTree->CollapseAll();
				}
				break;
			}
		}
		
		return LSW_H_CONTINUE;
	}

	/**
	 * Handles WM_COMMAND from a menu.
	 * \brief Invoked for menu command selections.
	 *
	 * \param _wId The menu command identifier.
	 * \return Returns a LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CCheatsWindow::MenuCommand( WORD _wId ) {
		switch ( _wId ) {
			case IDCANCEL : {}													LSN_FALLTHROUGH
			case Layout::LSN_AOWI_CANCEL : {
				return Close();
			}
		}
		
		return Parent::MenuCommand( _wId );
	}

	/**
	 * The WM_NOTIFY -> LVN_ITEMCHANGED handler.
	 *
	 * \param _lplvParm The notifacation structure.
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CCheatsWindow::Notify_ItemChanged( LPNMLISTVIEW /*_lplvParm*/ ) {
		UpdateSelection();
		return LSW_H_CONTINUE;
	}

	/**
	 * The WM_NOTIFY -> LVN_ODSTATECHANGED handler.
	 *
	 * \param _lposcParm The notifacation structure.
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CCheatsWindow::Notify_OdStateChange( LPNMLVODSTATECHANGE /*_lposcParm*/ ) {
		UpdateSelection();
		return LSW_H_CONTINUE;
	}

	/**
	 * Handles the WM_CONTEXTMENU message.
	 * 
	 * \param _pwControl The control that was clicked.
	 * \param _iX The horizontal position of the cursor, in screen coordinates, at the time of the mouse click.
	 * \param _iY The vertical position of the cursor, in screen coordinates, at the time of the mouse click.
	 * \return Returns an LSW_HANDLED code.
	 **/
	CWidget::LSW_HANDLED CCheatsWindow::ContextMenu( CWidget * _pwControl, INT _iX, INT _iY ) {
		if ( _pwControl->Id() == Layout::LSN_CWI_CHEAT_TREELISTVIEW ) {
			auto ptlvTree = static_cast<CTreeListView *>(_pwControl);
			bool bExpSel = ptlvTree->AnySelectedHasUnexpandedChildren();
			bool bExpAll = ptlvTree->AnyHasUnexpandedChildren();
			bool bColSel = ptlvTree->AnySelectedHasExpandedChildren();
			bool bColAll = ptlvTree->AnyHasExpandedChildren();
			bool bHasItems = ptlvTree->HasItem();
			std::vector<LPARAM> vSelected = GatherSelectedGames()/*, vFullselection*/;
			//ptlvTree->GatherSelectedLParam( vFullselection );

			bool bSelected = vSelected.size() != 0;
			bool bAllSelectedAreCustom = !HasBuiltIn( vSelected );
			// Enable becomes Disable when every selected cheat is enabled (the same toggle as the Activate button).
			bool bAllEnabled = bSelected;
			if ( m_pcmCheatManager ) {
				for ( auto lpThis : vSelected ) {
					if ( !m_pcmCheatManager->IsEnabled( uint32_t( lpThis ) ) ) {
						bAllEnabled = false;
						break;
					}
				}
			}
			lsw::LSW_MENU_ITEM miMenuBar[] = {
				{ FALSE,		Layout::LSN_CWI_ADD_BUTTON,						FALSE,		FALSE,		TRUE,												LSN_LSTR( LSN_STR__ADD_CHEAT ),				FALSE },
				{ FALSE,		Layout::LSN_CWI_DELETE_BUTTON,					FALSE,		FALSE,		bSelected && bHasItems && bAllSelectedAreCustom,	LSN_LSTR( LSN_STR_DE_LETE_CHEAT ),			FALSE },
				{ FALSE,		Layout::LSN_CWI_DUPLICATE_BUTTON,				FALSE,		FALSE,		bSelected && bHasItems,								LSN_LSTR( LSN_STR__DUPLICATE_CHEAT ),		FALSE },
				{ FALSE,		Layout::LSN_CWI_EDIT_BUTTON,					FALSE,		FALSE,		bSelected && bHasItems && bAllSelectedAreCustom,	LSN_LSTR( LSN_STR__EDIT_CHEAT ),			FALSE },
				{ TRUE,			0,												FALSE,		FALSE,		bSelected && bHasItems,								nullptr,									FALSE },
				{ FALSE,		Layout::LSN_CWI_ACTIVATE_BUTTON,				FALSE,		FALSE,		bSelected && bHasItems,								bAllEnabled ? LSN_LSTR( LSN_STR_DISA_BLE_CHEATS ) : LSN_LSTR( LSN_STR_E_NABLE_CHEATS ),	FALSE },
				{ FALSE,		Layout::LSN_CWI_ACTIVATE_WITH_HOTKEY_BUTTON,	FALSE,		FALSE,		vSelected.size() == 1 && bHasItems,					LSN_LSTR( LSN_STR_ENABLE_WITH__HOTKEY ),	FALSE },
				{ FALSE,		Layout::LSN_CWI_CLEAR_HOTKEYS,					FALSE,		FALSE,		bSelected && bHasItems && HasHotkey( vSelected ),	LSN_LSTR( LSN_STR_CLEAR_HOT_KEYS ),			FALSE },
				{ FALSE,		Layout::LSN_CWI_DISABLE_ALL,					FALSE,		FALSE,		m_pcmCheatManager && m_pcmCheatManager->AnyEnabled(),	LSN_LSTR( LSN_STR_DISABLE_A_LL_CHEATS ),	FALSE },
				{ TRUE,			0,												FALSE,		FALSE,		bHasItems,											nullptr,									FALSE },
				{ FALSE,		Layout::LSN_CWI_SHOW_ENABLED,					FALSE,		FALSE,		TRUE,												LSN_LSTR( LSN_STR__REVEAL_ENABLED_CHEATS ),	FALSE },
				{ FALSE,		Layout::LSN_CWI_SELECT_ENABLED,					FALSE,		FALSE,		TRUE,												LSN_LSTR( LSN_STR__SELECT_ENABLED_CHEATS ),	FALSE },
				{ TRUE,			0,												FALSE,		FALSE,		bHasItems,											nullptr,									FALSE },
				{ FALSE,		Layout::LSN_CWI_EXPAND_SELECTED,				FALSE,		FALSE,		bHasItems && bExpSel,								LSN_LSTR( LSN_PATCH_EXPAND_SELECTED ),		FALSE },
				{ FALSE,		Layout::LSN_CWI_EXPAND_ALL,						FALSE,		FALSE,		bHasItems && bExpAll,								LSN_LSTR( LSN_PATCH_EXPAND_ALL ),			FALSE },
				{ FALSE,		Layout::LSN_CWI_COLLAPSE_SELECTED,				FALSE,		FALSE,		bHasItems && bColSel,								LSN_LSTR( LSN_PATCH_COLLAPSE_SELECTED ),	FALSE },
				{ FALSE,		Layout::LSN_CWI_COLLAPSE_ALL,					FALSE,		FALSE,		bHasItems && bColAll,								LSN_LSTR( LSN_PATCH_COLLAPSE_ALL ),			FALSE },
				
			};

			const lsw::LSW_MENU_LAYOUT miMenus[] = {
				{
					LSN_M_CONTEXT_MENU,
					0,
					0,
					std::size( miMenuBar ),
					miMenuBar
				},
			};
			lsn::CLayoutManager * plmLayout = static_cast<lsn::CLayoutManager *>(lsw::CBase::LayoutManager());
			Command( 0, static_cast<WORD>(plmLayout->CreatePopupMenuEx( this, miMenus, std::size( miMenus ), _iX, _iY, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD )), ptlvTree );
			return LSW_H_HANDLED;
		}
		return Parent::ContextMenu( _pwControl, _iX, _iY );
	}

	/**
	 * Fills the TreeListView with filtered cheats.  Games that were already shown keep their expanded/collapsed states.
	 * 
	 * \param _bKeepScroll If true, the list keeps its scroll position (used when cheats are added or deleted), otherwise it scrolls to
	 *	the top (used when the filter or search changes).
	 * \return Returns the number of cheats added to the TreeListView.
	 **/
	size_t CCheatsWindow::AddCheats( bool _bKeepScroll ) {
		if ( nullptr == m_pcmCheatManager ) { return 0; }
		auto ptlvTree = reinterpret_cast<CTreeListView *>(FindChild( Layout::LSN_CWI_CHEAT_TREELISTVIEW ));
		if ( !ptlvTree ) { return 0; }
		try {
			// Remember the view:  the first visible row and which games are expanded (by name, since game IDs are reassigned below).
			INT iTopIdx = _bKeepScroll ? INT( ::SendMessageW( ptlvTree->Wnd(), LVM_GETTOPINDEX, 0, 0 ) ) : 0;
			std::map<std::wstring, BOOL> mExpanded;
			for ( auto hThis = ptlvTree->GetNext( TVI_ROOT ); hThis; hThis = ptlvTree->GetNext( hThis ) ) {
				auto aFound = m_mGameNameAssociation.find( ptlvTree->GetItemLParam( hThis ) );
				if ( aFound != m_mGameNameAssociation.end() ) {
					mExpanded[aFound->second] = ptlvTree->ItemIsExpanded( hThis );
				}
			}

			ptlvTree->BeginLargeUpdate();
			std::vector<LPARAM> vSelected = GatherSelectedGames();
			ptlvTree->DeleteAll();
			m_mGameNameAssociation.clear();
			uint32_t ui32GameIdx = 0;

			auto sGames = m_pcmCheatManager->GatherGamesWithFilter();
			// Gather the cheats of each game whose text contains the search string (case-insensitive).
			std::vector<decltype(sGames.begin())> vGameIters;
			std::vector<std::vector<std::pair<uint32_t, std::wstring>>> vGameCheats;
			for ( auto I = sGames.begin(); I != sGames.end(); ++I ) {
				auto vCheats = m_pcmCheatManager->GatherCheatsForGame( (*I) );
				std::vector<std::pair<uint32_t, std::wstring>> vMatches;
				for ( size_t J = 0; J < vCheats.size(); ++J ) {
					std::wstring wsText = CheatEntryToString( m_pcmCheatManager->CheatByIdx( vCheats[J] ), m_pcmCheatManager->IsEnabled( vCheats[J] ),
						CCheatManager::IsCustom( uint32_t( vCheats[J] ) ) );
					if ( !m_wsSearch.empty() && CUtilities::ToLower( wsText ).find( m_wsSearch ) == std::wstring::npos ) { continue; }
					vMatches.emplace_back( uint32_t( vCheats[J] ), std::move( wsText ) );
				}
				if ( vMatches.empty() && !m_wsSearch.empty() ) { continue; }
				vGameIters.push_back( I );
				vGameCheats.push_back( std::move( vMatches ) );
			}

			for ( size_t G = 0; G < vGameIters.size(); ++G ) {
				auto I = vGameIters[G];
				const auto & vMatches = vGameCheats[G];
				HTREEITEM hParent = TVI_ROOT;
				// If there is only one game after the filter and search, no need to add a heirarchy.  Just make a flat list.
				if ( vGameIters.size() > 1 ) {
					// Add the game names with children.
					std::wstring wsTmp = CUtilities::XStringToWString( (*I).c_str(), (*I).size() );
					uint32_t ui32Id = 0x80000000U + ui32GameIdx++;
					TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( reinterpret_cast<const WCHAR *>(wsTmp.c_str()),
						ui32Id, TVI_ROOT );
					hParent = ptlvTree->InsertItem( &isInsertMe );
					auto aFound = mExpanded.find( wsTmp );
					if ( aFound != mExpanded.end() ) { ptlvTree->SetItemExpand( hParent, aFound->second ); }
					m_mGameNameAssociation.insert( std::make_pair( ui32Id, wsTmp ) );
				}
				for ( size_t J = 0; J < vMatches.size(); ++J ) {
					TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( reinterpret_cast<const WCHAR *>(vMatches[J].second.c_str()),
						vMatches[J].first, hParent );
					HTREEITEM htiItem = ptlvTree->InsertItem( &isInsertMe );
					if ( htiItem ) {
						ptlvTree->SetItemText( htiItem, HotkeyText( vMatches[J].first ).c_str(), 1 );
						if ( CCheatManager::IsCustom( vMatches[J].first ) ) {
							ptlvTree->SetItemColor( htiItem, RGBQUAD{ .rgbBlue = 0xFF, .rgbGreen = 0xC8, .rgbRed = 0xB4, .rgbReserved = 0x5F } );
						}
					}
				}
			}

			ptlvTree->SetItemSelectionByLParam( vSelected );
			ptlvTree->FinishUpdate();
			if ( iTopIdx > 0 ) {
				// Scroll back to the same first visible row (LVM_SCROLL scrolls report views by pixels).
				RECT rRow = {};
				rRow.left = LVIR_BOUNDS;
				if ( ::SendMessageW( ptlvTree->Wnd(), LVM_GETITEMRECT, 0, reinterpret_cast<LPARAM>(&rRow) ) ) {
					::SendMessageW( ptlvTree->Wnd(), LVM_SCROLL, 0, LPARAM( iTopIdx * (rRow.bottom - rRow.top) ) );
				}
			}
			UpdateSelection();
		}
		catch ( ... ) {}
		return ptlvTree->GetItemCount();
	}

	/**
	 * Updates the texts (both columns) of the cheats already in the TreeListView without rebuilding it, so that the scroll position,
	 *	selection, and expanded/collapsed states do not change.  Use after cheats are edited, enabled, or disabled, or have their hotkeys
	 *	changed.
	 **/
	void CCheatsWindow::RefreshCheatItems() {
		if ( nullptr == m_pcmCheatManager ) { return; }
		auto ptlvTree = reinterpret_cast<CTreeListView *>(FindChild( Layout::LSN_CWI_CHEAT_TREELISTVIEW ));
		if ( !ptlvTree ) { return; }
		try {
			for ( auto hThis = ptlvTree->GetNext( TVI_ROOT ); hThis; hThis = ptlvTree->GetNext( hThis ) ) {
				auto lpParm = ptlvTree->GetItemLParam( hThis );
				if ( lpParm & 0x80000000U ) { continue; }
				uint32_t ui32Idx = uint32_t( lpParm );
				if ( !m_pcmCheatManager->IsValidIdx( ui32Idx ) ) { continue; }
				ptlvTree->SetItemText( hThis, CheatEntryToString( m_pcmCheatManager->CheatByIdx( ui32Idx ), m_pcmCheatManager->IsEnabled( ui32Idx ),
					CCheatManager::IsCustom( ui32Idx ) ).c_str(), 0 );
				ptlvTree->SetItemText( hThis, HotkeyText( ui32Idx ).c_str(), 1 );
			}
		}
		catch ( ... ) {}
		// The list view is virtual and asks for its texts when it paints.
		::InvalidateRect( ptlvTree->Wnd(), nullptr, FALSE );
		UpdateSelection();
	}

	/**
	 * Creates the text of the Hotkey(s) column for a cheat.
	 * 
	 * \param _ui32Idx The extended index of the cheat.
	 * \return Returns the text describing the hotkeys of the cheat, or an empty string if it has none.
	 **/
	std::wstring CCheatsWindow::HotkeyText( uint32_t _ui32Idx ) const {
		std::wstring wsRet;
		if ( nullptr == m_pcmCheatManager ) { return wsRet; }
		try {
			CCheatManager::LSN_ACTIVE_CHEAT acState;
			if ( !m_pcmCheatManager->GetActiveCheat( _ui32Idx, acState ) || !acState.HasHotkey() ) { return wsRet; }
			if ( acState.IsToggle() ) {
				if ( acState.kToggleOrEnable.bKeyCode ) {
					wsRet = std::wstring( LSN_LSTR( LSN_STR_TOGGLE_ ) ) + L" " + lsw::CHelpers::ToString( acState.kToggleOrEnable, false );
				}
			}
			else {
				if ( acState.kToggleOrEnable.bKeyCode ) {
					wsRet = std::wstring( LSN_LSTR( LSN_STR_ENABLE_ ) ) + L" " + lsw::CHelpers::ToString( acState.kToggleOrEnable, false );
				}
				if ( acState.kDisable.bKeyCode ) {
					if ( wsRet.size() ) { wsRet += L"; "; }
					wsRet += std::wstring( LSN_LSTR( LSN_STR_DISABLE_ ) ) + L" " + lsw::CHelpers::ToString( acState.kDisable, false );
				}
			}
		}
		catch ( ... ) {}
		return wsRet;
	}

	/**
	 * Updates the addresses and notes edits based on the TreeListView selection.
	 **/
	void CCheatsWindow::UpdateSelection() {
		try {
			auto ptlvTree = reinterpret_cast<CTreeListView *>(FindChild( Layout::LSN_CWI_CHEAT_TREELISTVIEW ));
			if ( !ptlvTree || !m_pcmCheatManager ) { return; }
			std::vector<LPARAM> vSelected = GatherSelectedGames();
			bool bSelected = !vSelected.empty();
			//bool bHasBuiltIn = false;
			bool bHasCustom = false;

			auto pwCodes = FindChild( Layout::LSN_CWI_CODE_EDIT );
			if ( !pwCodes ) { return; }
			auto pwNotes = FindChild( Layout::LSN_CWI_NOTES_EDIT );
			if ( !pwNotes ) { return; }

			std::wstring wsCodes;
			std::wstring wsNotes;
			for ( size_t I = 0; I < vSelected.size(); ++I ) {
				//bHasBuiltIn |= (uint32_t( vSelected[I] ) & 0x800000) == 0;
				bHasCustom |= CCheatManager::IsCustom( uint32_t( vSelected[I] ) );
				auto ceEntry = m_pcmCheatManager->CheatByIdx( uint32_t( vSelected[I] ) );
				if ( vSelected.size() > 1 ) {
					if ( wsCodes.size() ) {
						wsCodes += L"\r\n\r\n";
						wsNotes += L"\r\n\r\n";
					}
					if ( wsNotes.size() ) {
						wsNotes += L"\r\n\r\n";
					}
					wsCodes += ceEntry.wsDescription;
					wsCodes += L":\r\n";

					wsNotes += ceEntry.wsDescription;
					wsNotes += L":\r\n";
				}
				for ( size_t J = 0; J < ceEntry.vAddresses.size(); ++J ) {
					if ( J > 0 ) { wsCodes += L"\r\n"; }
					if ( ceEntry.ctType == LSN_CHEAT_ENTRY::LSN_CT_GAME_GENIE ) {
						if ( ceEntry.vAddresses[J].bUseCompare ) {
							wsCodes += std::format( L"{}  ({:04X}:{:02X}:{:02X})",
								CUtilities::XStringToWString( ceEntry.vAddresses[J].sCode.c_str(), ceEntry.vAddresses[J].sCode.size() ),
								ceEntry.vAddresses[J].ui16Address, ceEntry.vAddresses[J].ui8Value, ceEntry.vAddresses[J].ui8CompareValue );
						}
						else {
							wsCodes += std::format( L"{}    ({:04X}:{:02X})",
								CUtilities::XStringToWString( ceEntry.vAddresses[J].sCode.c_str(), ceEntry.vAddresses[J].sCode.size() ),
								ceEntry.vAddresses[J].ui16Address, ceEntry.vAddresses[J].ui8Value );
						}
					}
					else {
						wsCodes += CUtilities::XStringToWString( ceEntry.vAddresses[J].sCode.c_str(), ceEntry.vAddresses[J].sCode.size() );
					}
				}
				wsNotes += ceEntry.wsNotes;
			}
			pwCodes->SetTextW( wsCodes.c_str() );
			pwNotes->SetTextW( wsNotes.c_str() );

			CToolBar * plvToolBar = static_cast<CToolBar *>(FindChild( Layout::LSN_CWI_TOOLBAR ));
			if ( plvToolBar ) {
				struct LSN_CONTROLS {
					Layout::LSN_CONTROL_IDS								wId;
					bool												bCloseCondition0;
				} cControls[] = {
					{ Layout::LSN_CWI_EXPORT_BUTTON,					bSelected },
					{ Layout::LSN_CWI_DELETE_BUTTON,					bSelected && bHasCustom },
					{ Layout::LSN_CWI_DUPLICATE_BUTTON,					bSelected && vSelected.size() == 1 },
					{ Layout::LSN_CWI_EDIT_BUTTON,						bSelected && bHasCustom && vSelected.size() == 1 },
				};
				for ( auto I = std::size( cControls ); I--; ) {
					plvToolBar->EnableButton( WORD( cControls[I].wId ), cControls[I].bCloseCondition0 );
				}
			}

			// Activate becomes Deactivate when every selected cheat is enabled.
			bool bAllEnabled = bSelected;
			for ( auto lpThis : vSelected ) {
				if ( !m_pcmCheatManager->IsEnabled( uint32_t( lpThis ) ) ) {
					bAllEnabled = false;
					break;
				}
			}
			auto pwActivate = FindChild( Layout::LSN_CWI_ACTIVATE_BUTTON );
			if ( pwActivate ) {
				pwActivate->SetEnabled( bSelected );
				pwActivate->SetTextW( bAllEnabled ? LSN_LSTR( LSN_STR_DEACTIVATE ) : LSN_LSTR( LSN_STR_ACTIVATE ) );
			}
			auto pwActivateHotkey = FindChild( Layout::LSN_CWI_ACTIVATE_WITH_HOTKEY_BUTTON );
			if ( pwActivateHotkey ) { pwActivateHotkey->SetEnabled( vSelected.size() == 1 ); }
		}
		catch ( ... ) {}
	}

	/**
	 * Creates the string representation for a given cheat entry intended for use in the TreeListView.
	 * 
	 * \param _ceEntry The cheat entry whose string representation is to be generated.
	 * \param _bEnabled Whether the cheat is enabled.
	 * \param _bCustom Whether the cheat was created by the user (it is then tagged).
	 * \return Returns the string representation of the given cheat entry.
	 **/
	std::wstring CCheatsWindow::CheatEntryToString( const LSN_CHEAT_ENTRY &_ceEntry, bool _bEnabled, bool _bCustom ) {
		std::wstring wsText = _bEnabled ? L"\u25A0 " : L"\u25A1 ";
		if ( _bCustom ) {
			wsText += LSN_LSTR( LSN_STR_CHEATS_CUSTOM_TAG );
			wsText += L' ';
		}
		if ( _ceEntry.wsNotes.size() ) {
			wsText += std::format( L"{}: {}",
				_ceEntry.wsDescription, _ceEntry.wsNotes );
		}
		else {
			wsText += _ceEntry.wsDescription;
		}
		return wsText;
	}

	/**
	 * Reveals enabled cheats.
	 * 
	 * \param _bSelect If true, the items are also selected.
	 **/
	void CCheatsWindow::RevealEnabled( bool _bSelect ) {
		if ( nullptr == m_pcmCheatManager ) { return; }
		auto ptlvTree = reinterpret_cast<CTreeListView *>(FindChild( Layout::LSN_CWI_CHEAT_TREELISTVIEW ));
		if ( !ptlvTree ) { return; }
		if ( ptlvTree ) {
			ptlvTree->BeginLargeUpdate();
			auto hThis = ptlvTree->GetNext( TVI_ROOT );
			HTREEITEM htiParent = NULL;
			while ( hThis ) {
				auto lpParm = ptlvTree->GetItemLParam( hThis );
				if ( (lpParm & 0x80000000U) == 0 ) {
					bool bEnabled = m_pcmCheatManager->IsEnabled( uint32_t( lpParm ) );
					if ( htiParent ) {
						ptlvTree->SetItemExpand( htiParent, bEnabled );
						if ( bEnabled ) {
							htiParent = NULL;	// Don't collapse it back down on the next item.
						}
					}
					if ( _bSelect ) {
						ptlvTree->SetItemSelection( hThis, bEnabled && ptlvTree->CountChildren( hThis ) == 0 );
					}
				}
				else {
					htiParent = hThis;
					ptlvTree->SetItemSelection( hThis, false );
				}
				hThis = ptlvTree->GetNext( hThis );
			}
			ptlvTree->FinishUpdate();
		}
	}

	/**
	 * Adds a custom cheat via the Add Cheat dialog.  The cheat goes to the game of the selection, else the only game shown, else the game
	 *	named by the filter.
	 **/
	void CCheatsWindow::AddCheat() {
		if ( nullptr == m_pcmCheatManager ) { return; }
		try {
			CCheatEditWindowLayout::LSN_CHEAT_EDIT_DATA cedData;
			cedData.ceEntry.u16sFile = GameForNewCheat();
			cedData.pwcTitle = LSN_LSTR( LSN_STR_ADD_CHEAT );
			if ( !CCheatEditWindowLayout::CreateCheatEditDialog( this, cedData ) ) { return; }
			uint32_t ui32Idx = m_pcmCheatManager->AddCustomCheat( cedData.ceEntry, &cedData.acActive );
			if ( ui32Idx == LSN_CHEAT_MAN_INVALID_IDX ) {
				lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_OUT_OF_MEMORY ), LSN_LSTR( LSN_ERROR ) );
				return;
			}
			m_pcmCheatManager->CheatsChanged();
			AddCheats( true );
			SelectCheat( ui32Idx );
			if ( cedData.acActive.bEnabled && !m_pcmCheatManager->IsEnabled( ui32Idx ) ) { ReportConflicts( { ui32Idx } ); }
		}
		catch ( ... ) {}
	}

	/**
	 * Edits the selected custom cheat or duplicates the selected cheat (built-in or custom) via the Edit/Duplicate Cheat dialog.  A duplicate
	 *	is a new custom cheat for the same game.
	 * 
	 * \param _bDuplicate If true, the selected cheat is duplicated, otherwise it is edited.
	 **/
	void CCheatsWindow::EditCheat( bool _bDuplicate ) {
		if ( nullptr == m_pcmCheatManager ) { return; }
		try {
			std::vector<LPARAM> vSelected = GatherSelectedGames();
			if ( vSelected.size() != 1 ) { return; }
			uint32_t ui32Idx = uint32_t( vSelected[0] );
			if ( !m_pcmCheatManager->IsValidIdx( ui32Idx ) ) { return; }
			// Built-in cheats can only be duplicated.
			if ( !_bDuplicate && !CCheatManager::IsCustom( ui32Idx ) ) { return; }

			CCheatEditWindowLayout::LSN_CHEAT_EDIT_DATA cedData;
			// Editing and duplicating keep the cheat's game (ceEntry.u16sFile).
			cedData.ceEntry = m_pcmCheatManager->CheatByIdx( ui32Idx );
			m_pcmCheatManager->GetActiveCheat( ui32Idx, cedData.acActive );
			cedData.pwcTitle = _bDuplicate ? LSN_LSTR( LSN_STR_DUPLICATE_CHEAT ) : LSN_LSTR( LSN_STR_EDIT_CHEAT );
			if ( !CCheatEditWindowLayout::CreateCheatEditDialog( this, cedData ) ) { return; }

			if ( _bDuplicate ) {
				ui32Idx = m_pcmCheatManager->AddCustomCheat( cedData.ceEntry, &cedData.acActive );
				if ( ui32Idx == LSN_CHEAT_MAN_INVALID_IDX ) {
					lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_OUT_OF_MEMORY ), LSN_LSTR( LSN_ERROR ) );
					return;
				}
			}
			else if ( !m_pcmCheatManager->UpdateCustomCheat( ui32Idx, cedData.ceEntry, &cedData.acActive ) ) {
				lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_OUT_OF_MEMORY ), LSN_LSTR( LSN_ERROR ) );
				return;
			}
			m_pcmCheatManager->CheatsChanged();
			if ( _bDuplicate ) {
				// A new item:  rebuild (keeping the view) and select it.
				AddCheats( true );
				SelectCheat( ui32Idx );
			}
			else {
				// The same item with new texts:  the view does not change.
				RefreshCheatItems();
			}
			if ( cedData.acActive.bEnabled && !m_pcmCheatManager->IsEnabled( ui32Idx ) ) { ReportConflicts( { ui32Idx } ); }
		}
		catch ( ... ) {}
	}

	/**
	 * Imports Mesen .JSON cheat files as custom cheats.  The name of each file (without its extension) is the game of its cheats.
	 **/
	void CCheatsWindow::ImportCheats() {
		if ( nullptr == m_pcmCheatManager ) { return; }
		try {
			OPENFILENAMEW ofnOpenFile = { sizeof( ofnOpenFile ) };
			std::wstring wsFileName;
			wsFileName.resize( 0x1FFFF + 2 );
			std::wstring wsFilter = std::wstring( LSN_LSTR( LSN_STR_CHEAT_JSON_TYPES ), std::size( LSN_LSTR( LSN_STR_CHEAT_JSON_TYPES ) ) - 1 );
			const std::wstring & wsInitial = m_pcmCheatManager->LastImportFolder();
			ofnOpenFile.hwndOwner = Wnd();
			ofnOpenFile.lpstrFilter = wsFilter.c_str();
			ofnOpenFile.nFilterIndex = 1;
			ofnOpenFile.lpstrFile = wsFileName.data();
			ofnOpenFile.nMaxFile = DWORD( wsFileName.size() );
			ofnOpenFile.lpstrTitle = LSN_LSTR( LSN_STR_IMPORT_CHEATS );
			ofnOpenFile.Flags = OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
			ofnOpenFile.lpstrInitialDir = wsInitial.size() ? wsInitial.c_str() : nullptr;
			if ( !::GetOpenFileNameW( &ofnOpenFile ) ) { return; }

			// One file is a full path; several are the folder followed by the file names.
			std::vector<std::filesystem::path> vFiles;
			std::wstring wsDir = ofnOpenFile.lpstrFile;
			const wchar_t * pwcTmp = ofnOpenFile.lpstrFile + wsDir.size() + 1;
			if ( (*pwcTmp) == L'\0' ) { vFiles.push_back( wsDir ); }
			else {
				while ( (*pwcTmp) ) {
					std::wstring wsFile = pwcTmp;
					vFiles.push_back( std::filesystem::path( wsDir ) / wsFile );
					pwcTmp += wsFile.size() + 1;
				}
			}
			if ( vFiles.empty() ) { return; }
			m_pcmCheatManager->SetLastImportFolder( vFiles[0].parent_path().native() );

			// Same cheat:  same game, description, notes, and decoded codes.
			auto SameCheat = []( const LSN_CHEAT_ENTRY &_ceA, const LSN_CHEAT_ENTRY &_ceB ) {
				if ( _ceA.u16sFile != _ceB.u16sFile || _ceA.wsDescription != _ceB.wsDescription || _ceA.wsNotes != _ceB.wsNotes ||
					_ceA.vAddresses.size() != _ceB.vAddresses.size() ) { return false; }
				for ( size_t I = _ceA.vAddresses.size(); I--; ) {
					const auto & aA = _ceA.vAddresses[I];
					const auto & aB = _ceB.vAddresses[I];
					if ( aA.ui16Address != aB.ui16Address || aA.ui8Value != aB.ui8Value || aA.bUseCompare != aB.bUseCompare ||
						(aA.bUseCompare && aA.ui8CompareValue != aB.ui8CompareValue) ) { return false; }
				}
				return true;
			};

			size_t stImported = 0, stDuplicates = 0, stInvalid = 0;
			std::wstring wsErrors;
			std::map<std::u16string, std::vector<LSN_CHEAT_ENTRY>> mExisting;
			for ( const auto & pFile : vFiles ) {
				std::vector<uint8_t> vJson;
				std::wstring wsError;
				std::vector<LSN_CHEAT_ENTRY> vCheats;
				if ( !CStdFile::LoadToMemory( pFile.native().c_str(), vJson ) ) { wsError = LSN_LSTR( LSN_FAILED_TO_OPEN_FILE ); }
				else {
					if ( vJson.size() >= 3 && vJson[0] == 0xEF && vJson[1] == 0xBB && vJson[2] == 0xBF ) { vJson.erase( vJson.begin(), vJson.begin() + 3 ); }
					vJson.push_back( 0 );
					vJson.push_back( 0 );
					// Errors about single codes are handled below (the cheat is skipped), so only a failure that loaded nothing is reported.
					if ( !CCheatConverter::LoadMesenJson( vJson, vCheats, pFile.filename().u16string(), wsError ) && vCheats.size() ) { wsError.clear(); }
				}
				if ( wsError.size() ) {
					if ( wsErrors.size() ) { wsErrors += L"\r\n"; }
					wsErrors += pFile.filename().wstring() + L": " + wsError;
					continue;
				}

				for ( auto & ceThis : vCheats ) {
					// Every code must be a valid NES code (other systems' cheats and unsupported types are skipped).
					LSN_CHEAT_ENTRY ceNew = ceThis;
					ceNew.vAddresses.clear();
					ceNew.bEnabled = false;
					bool bAllGameGenie = true, bValid = ceThis.vAddresses.size() != 0;
					for ( size_t I = 0; I < ceThis.vAddresses.size() && bValid; ++I ) {
						LSN_CHEAT_ENTRY::LSN_ADDRESS aTmp;
						bool bGameGenie = false;
						bValid = CCheatConverter::ParseAnyCode( ceThis.vAddresses[I].sCode, aTmp, bGameGenie );
						bAllGameGenie = bAllGameGenie && bGameGenie;
						ceNew.vAddresses.push_back( std::move( aTmp ) );
					}
					if ( !bValid ) {
						++stInvalid;
						continue;
					}
					ceNew.ctType = bAllGameGenie ? LSN_CHEAT_ENTRY::LSN_CT_GAME_GENIE : LSN_CHEAT_ENTRY::LSN_CT_CUSTOM;

					auto aGame = mExisting.find( ceNew.u16sFile );
					if ( aGame == mExisting.end() ) {
						std::vector<LSN_CHEAT_ENTRY> vGame;
						for ( auto ui32Idx : m_pcmCheatManager->GatherCheatsForGame( ceNew.u16sFile ) ) {
							vGame.push_back( m_pcmCheatManager->CheatByIdx( ui32Idx ) );
						}
						aGame = mExisting.insert( std::make_pair( ceNew.u16sFile, std::move( vGame ) ) ).first;
					}
					bool bDuplicate = false;
					for ( const auto & ceOld : aGame->second ) {
						if ( SameCheat( ceOld, ceNew ) ) {
							bDuplicate = true;
							break;
						}
					}
					if ( bDuplicate ) {
						++stDuplicates;
						continue;
					}
					if ( m_pcmCheatManager->AddCustomCheat( ceNew ) == LSN_CHEAT_MAN_INVALID_IDX ) {
						lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_OUT_OF_MEMORY ), LSN_LSTR( LSN_ERROR ) );
						break;
					}
					aGame->second.push_back( std::move( ceNew ) );
					++stImported;
				}
			}
			if ( stImported ) {
				m_pcmCheatManager->CheatsChanged();
				AddCheats( true );
			}
			std::wstring wsMsg = std::vformat( std::wstring( LSN_LSTR( LSN_STR_CHEAT_IMPORT_DONE ) ), std::make_wformat_args( stImported, stDuplicates, stInvalid ) );
			if ( wsErrors.size() ) {
				wsMsg += L"\r\n\r\n";
				wsMsg += wsErrors;
			}
			::MessageBoxW( Wnd(), wsMsg.c_str(), LSN_LSTR( LSN_STR_IMPORT_CHEATS ), MB_OK | (wsErrors.size() ? MB_ICONWARNING : MB_ICONINFORMATION) );
		}
		catch ( ... ) {
			lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_OUT_OF_MEMORY ), LSN_LSTR( LSN_ERROR ) );
		}
	}

	/**
	 * Exports the selected cheats to Mesen .JSON files (one per game, named after the game) in a folder chosen by the user.
	 **/
	void CCheatsWindow::ExportCheats() {
		if ( nullptr == m_pcmCheatManager ) { return; }
		try {
			std::vector<LPARAM> vSelected = GatherSelectedGames();
			// Grouped by file name (two games could make the same file name once invalid characters are replaced).
			std::map<std::wstring, std::vector<LSN_CHEAT_ENTRY>> mFiles;
			size_t stTotal = 0;
			for ( auto lpThis : vSelected ) {
				uint32_t ui32Idx = uint32_t( lpThis );
				if ( !m_pcmCheatManager->IsValidIdx( ui32Idx ) ) { continue; }
				LSN_CHEAT_ENTRY ceEntry = m_pcmCheatManager->CheatByIdx( ui32Idx );
				ceEntry.bEnabled = m_pcmCheatManager->IsEnabled( ui32Idx );
				std::wstring wsName = CUtilities::XStringToWString( ceEntry.u16sFile.c_str(), ceEntry.u16sFile.size() );
				for ( auto & wcThis : wsName ) {
					if ( wcThis < L' ' || std::wcschr( L"<>:\"/\\|?*", wcThis ) ) { wcThis = L'_'; }
				}
				while ( wsName.size() && (wsName.back() == L'.' || wsName.back() == L' ') ) { wsName.pop_back(); }
				if ( wsName.empty() ) { wsName = LSN_LSTR( LSN_STR_CHEATS ); }
				mFiles[wsName+L".json"].push_back( std::move( ceEntry ) );
				++stTotal;
			}
			if ( mFiles.empty() ) { return; }

			// Choose the folder.
			std::filesystem::path pFolder;
			{
				std::wstring wsDisplayName;
				wsDisplayName.resize( 0xFFFF + 2 );
				BROWSEINFOW biBrowseInfo = {};
				biBrowseInfo.hwndOwner = Wnd();
				biBrowseInfo.pszDisplayName = wsDisplayName.data();
				biBrowseInfo.lpszTitle = LSN_LSTR( LSN_STR_EXPORT_CHEATS_FOLDER );
				biBrowseInfo.ulFlags = BIF_RETURNONLYFSDIRS;
				biBrowseInfo.lpfn = CWinUtilities::BrowseCallbackProc;
				biBrowseInfo.lParam = reinterpret_cast<LPARAM>(m_pcmCheatManager->LastExportFolder().c_str());
				CWinUtilities::LSN_OLEINITIALIZE oOle;
				if ( oOle.Success() ) { biBrowseInfo.ulFlags |= BIF_NEWDIALOGSTYLE; }
				PIDLIST_ABSOLUTE pidlSelected = ::SHBrowseForFolderW( &biBrowseInfo );
				if ( !pidlSelected ) { return; }
				std::wstring wsPath;
				wsPath.resize( 0xFFFF + 2 );
				BOOL bGot = ::SHGetPathFromIDListW( pidlSelected, wsPath.data() );
				::CoTaskMemFree( pidlSelected );
				if ( !bGot ) { return; }
				pFolder = wsPath.c_str();
			}
			m_pcmCheatManager->SetLastExportFolder( pFolder.native() );

			// Ask before replacing files.
			std::wstring wsExisting;
			for ( const auto & aThis : mFiles ) {
				std::error_code ecErr;
				if ( std::filesystem::exists( pFolder / aThis.first, ecErr ) ) {
					if ( wsExisting.size() ) { wsExisting += L"\r\n"; }
					wsExisting += aThis.first;
				}
			}
			if ( wsExisting.size() ) {
				std::wstring wsMsg = std::vformat( std::wstring( LSN_LSTR( LSN_STR_CHEAT_EXPORT_OVERWRITE ) ), std::make_wformat_args( wsExisting ) );
				if ( ::MessageBoxW( Wnd(), wsMsg.c_str(), LSN_LSTR( LSN_STR_EXPORT_SELECTED ), MB_YESNO | MB_ICONQUESTION ) != IDYES ) { return; }
			}

			size_t stFiles = 0, stCheats = 0;
			std::wstring wsErrors;
			for ( const auto & aThis : mFiles ) {
				std::string sJson;
				if ( !CCheatConverter::CreateMesenJson( aThis.second, sJson ) ) {
					lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_OUT_OF_MEMORY ), LSN_LSTR( LSN_ERROR ) );
					return;
				}
				std::filesystem::path pFile = pFolder / aThis.first;
				if ( !CStdFile::WriteToFile( pFile.native().c_str(), reinterpret_cast<const uint8_t *>(sJson.data()), sJson.size() ) ) {
					if ( wsErrors.size() ) { wsErrors += L"\r\n"; }
					wsErrors += aThis.first + L": " + LSN_LSTR( LSN_FAILED_TO_WRITE_TO_FILE );
					continue;
				}
				++stFiles;
				stCheats += aThis.second.size();
			}
			std::wstring wsMsg = std::vformat( std::wstring( LSN_LSTR( LSN_STR_CHEAT_EXPORT_DONE ) ), std::make_wformat_args( stCheats, stFiles ) );
			if ( wsErrors.size() ) {
				wsMsg += L"\r\n\r\n";
				wsMsg += wsErrors;
			}
			::MessageBoxW( Wnd(), wsMsg.c_str(), LSN_LSTR( LSN_STR_EXPORT_SELECTED ), MB_OK | (wsErrors.size() ? MB_ICONWARNING : MB_ICONINFORMATION) );
		}
		catch ( ... ) {
			lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_OUT_OF_MEMORY ), LSN_LSTR( LSN_ERROR ) );
		}
	}

	/**
	 * Widens or narrows the outer group box so that it ends where the toolbar ends, moving the controls on the right side and resizing
	 *	the TreeListView and the dialog to match.
	 **/
	void CCheatsWindow::FitToToolBar() {
		CToolBar * ptbToolBar = static_cast<CToolBar *>(FindChild( Layout::LSN_CWI_TOOLBAR ));
		CWidget * pwGroup = FindChild( Layout::LSN_CWI_GROUP );
		if ( !ptbToolBar || !pwGroup ) { return; }
		DWORD dwButtons = ptbToolBar->GetButtonCount();
		if ( !dwButtons ) { return; }
		LONG lToolW = 0;
		for ( DWORD I = dwButtons; I--; ) {
			lToolW += ptbToolBar->GetButtonRect( I ).Width();
		}
		LONG lToolRight = ptbToolBar->WindowRect().ScreenToClient( Wnd() ).left + ptbToolBar->GetButtonRect( 0 ).left + lToolW;
		LONG lDelta = lToolRight - pwGroup->WindowRect().ScreenToClient( Wnd() ).right;
		if ( !lDelta ) { return; }

		static const Layout::LSN_CONTROL_IDS ciMove[] = {
			Layout::LSN_CWI_CODE_EDIT,
			Layout::LSN_CWI_NOTES_EDIT,
			Layout::LSN_CWI_ACTIVATE_BUTTON,
			Layout::LSN_CWI_ACTIVATE_WITH_HOTKEY_BUTTON,
			Layout::LSN_CWI_OK,
			Layout::LSN_CWI_CANCEL,
		};
		static const Layout::LSN_CONTROL_IDS ciWiden[] = {
			Layout::LSN_CWI_CHEAT_TREELISTVIEW,
			Layout::LSN_CWI_GROUP,
		};
		for ( auto ciId : ciMove ) {
			CWidget * pwThis = FindChild( ciId );
			if ( !pwThis ) { continue; }
			lsw::LSW_RECT rRect = pwThis->WindowRect().ScreenToClient( Wnd() );
			::MoveWindow( pwThis->Wnd(), int( rRect.left + lDelta ), int( rRect.top ), int( rRect.Width() ), int( rRect.Height() ), FALSE );
		}
		for ( auto ciId : ciWiden ) {
			CWidget * pwThis = FindChild( ciId );
			if ( !pwThis ) { continue; }
			lsw::LSW_RECT rRect = pwThis->WindowRect().ScreenToClient( Wnd() );
			::MoveWindow( pwThis->Wnd(), int( rRect.left ), int( rRect.top ), int( std::max<LONG>( rRect.Width() + lDelta, 1 ) ), int( rRect.Height() ), FALSE );
		}
		lsw::LSW_RECT rThisRect = WindowRect();
		::MoveWindow( Wnd(), int( rThisRect.left - lDelta / 2 ), int( rThisRect.top ), int( rThisRect.Width() + lDelta ), int( rThisRect.Height() ), FALSE );

		CRebar * prRebar = static_cast<CRebar *>(FindChild( Layout::LSN_CWI_REBAR ));
		if ( prRebar ) {
			::MoveWindow( prRebar->Wnd(), 0, 0, ClientRect( this ).Width(), prRebar->WindowRect( this ).Height(), FALSE );
			prRebar->UpdateRects();
		}
	}

	/**
	 * Enables the selected cheats, or disables them if they are all enabled already.
	 **/
	void CCheatsWindow::ActivateCheats() {
		if ( nullptr == m_pcmCheatManager ) { return; }
		try {
			std::vector<LPARAM> vSelected = GatherSelectedGames();
			std::vector<uint32_t> vIdx;
			bool bAllEnabled = true;
			for ( auto lpThis : vSelected ) {
				if ( !m_pcmCheatManager->IsValidIdx( uint32_t( lpThis ) ) ) { continue; }
				vIdx.push_back( uint32_t( lpThis ) );
				bAllEnabled = bAllEnabled && m_pcmCheatManager->IsEnabled( uint32_t( lpThis ) );
			}
			if ( vIdx.empty() ) { return; }
			std::vector<uint32_t> vConflicts;
			m_pcmCheatManager->SetEnabled( vIdx, !bAllEnabled, vConflicts );
			m_pcmCheatManager->CheatsChanged();
			RefreshCheatItems();
			if ( vConflicts.size() ) { ReportConflicts( vConflicts ); }
		}
		catch ( ... ) {}
	}

	/**
	 * Opens the cheat dialog for the selected cheat so that it can be enabled with hotkeys.  The content of built-in cheats is shown but
	 *	cannot be changed.
	 **/
	void CCheatsWindow::ActivateWithHotkey() {
		if ( nullptr == m_pcmCheatManager ) { return; }
		try {
			std::vector<LPARAM> vSelected = GatherSelectedGames();
			if ( vSelected.size() != 1 ) { return; }
			uint32_t ui32Idx = uint32_t( vSelected[0] );
			if ( !m_pcmCheatManager->IsValidIdx( ui32Idx ) ) { return; }
			bool bCustom = CCheatManager::IsCustom( ui32Idx );

			CCheatEditWindowLayout::LSN_CHEAT_EDIT_DATA cedData;
			cedData.ceEntry = m_pcmCheatManager->CheatByIdx( ui32Idx );
			m_pcmCheatManager->GetActiveCheat( ui32Idx, cedData.acActive );
			cedData.acActive.bEnabled = true;
			cedData.bReadOnlyContent = !bCustom;
			cedData.pwcTitle = LSN_LSTR( LSN_STR_ACTIVATE_WITH_HOTKEY );
			if ( !CCheatEditWindowLayout::CreateCheatEditDialog( this, cedData ) ) { return; }

			cedData.acActive.ui32Idx = ui32Idx;
			if ( bCustom ) {
				if ( !m_pcmCheatManager->UpdateCustomCheat( ui32Idx, cedData.ceEntry, &cedData.acActive ) ) {
					lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_OUT_OF_MEMORY ), LSN_LSTR( LSN_ERROR ) );
					return;
				}
			}
			else {
				m_pcmCheatManager->SetActiveCheat( cedData.acActive );
			}
			m_pcmCheatManager->CheatsChanged();
			RefreshCheatItems();
			if ( cedData.acActive.bEnabled && !m_pcmCheatManager->IsEnabled( ui32Idx ) ) { ReportConflicts( { ui32Idx } ); }
		}
		catch ( ... ) {}
	}

	/**
	 * Removes the hotkeys of the selected cheats (built-in or custom).  Their enabled states are not changed.
	 **/
	void CCheatsWindow::ClearHotkeys() {
		if ( nullptr == m_pcmCheatManager ) { return; }
		try {
			std::vector<LPARAM> vSelected = GatherSelectedGames();
			std::vector<uint32_t> vIdx;
			for ( auto lpThis : vSelected ) {
				if ( m_pcmCheatManager->IsValidIdx( uint32_t( lpThis ) ) ) { vIdx.push_back( uint32_t( lpThis ) ); }
			}
			if ( !m_pcmCheatManager->ClearHotkeys( vIdx ) ) { return; }
			m_pcmCheatManager->CheatsChanged();
			RefreshCheatItems();
		}
		catch ( ... ) {}
	}

	/**
	 * Disables every enabled cheat (including cheats enabled for other ROMs).  Hotkeys are kept.
	 **/
	void CCheatsWindow::DisableAll() {
		if ( nullptr == m_pcmCheatManager ) { return; }
		if ( !m_pcmCheatManager->DisableAll() ) { return; }
		m_pcmCheatManager->CheatsChanged();
		RefreshCheatItems();
	}

	/**
	 * Determines whether any of the given cheats has a hotkey.
	 * 
	 * \param _vSelected The extended indices of the cheats to check.
	 * \return Returns true if at least one of the cheats has a hotkey.
	 **/
	bool CCheatsWindow::HasHotkey( const std::vector<LPARAM> &_vSelected ) const {
		if ( nullptr == m_pcmCheatManager ) { return false; }
		for ( auto lpThis : _vSelected ) {
			CCheatManager::LSN_ACTIVE_CHEAT acState;
			if ( m_pcmCheatManager->GetActiveCheat( uint32_t( lpThis ), acState ) && acState.HasHotkey() ) { return true; }
		}
		return false;
	}

	/**
	 * Tells the user which cheats could not be enabled because another enabled cheat uses the same address.
	 * 
	 * \param _vIdx The extended indices of the cheats that could not be enabled.
	 **/
	void CCheatsWindow::ReportConflicts( const std::vector<uint32_t> &_vIdx ) {
		if ( nullptr == m_pcmCheatManager ) { return; }
		try {
			std::wstring wsMsg;
			for ( auto ui32Idx : _vIdx ) {
				if ( !m_pcmCheatManager->IsValidIdx( ui32Idx ) ) { continue; }
				const LSN_CHEAT_ENTRY & ceEntry = m_pcmCheatManager->CheatByIdx( ui32Idx );
				uint32_t ui32Other = 0;
				uint16_t ui16Addr = 0;
				if ( !m_pcmCheatManager->FindConflict( ui32Idx, ceEntry, ui32Other, ui16Addr ) || !m_pcmCheatManager->IsValidIdx( ui32Other ) ) { continue; }
				const std::wstring & wsMine = ceEntry.wsDescription;
				const std::wstring & wsTheirs = m_pcmCheatManager->CheatByIdx( ui32Other ).wsDescription;
				if ( wsMsg.size() ) { wsMsg += L"\r\n\r\n"; }
				wsMsg += std::vformat( std::wstring( LSN_LSTR( LSN_STR_CHEAT_CONFLICT ) ), std::make_wformat_args( wsMine, ui16Addr, wsTheirs ) );
			}
			if ( wsMsg.size() ) {
				lsw::CBase::MessageBoxError( Wnd(), wsMsg.c_str(), LSN_LSTR( LSN_ERROR ) );
			}
		}
		catch ( ... ) {}
	}

	/**
	 * Deletes the selected custom cheats after asking.  Built-in cheats in the selection are left alone.
	 **/
	void CCheatsWindow::DeleteCheats() {
		if ( nullptr == m_pcmCheatManager ) { return; }
		try {
			std::vector<LPARAM> vSelected = GatherSelectedGames();
			if ( !HasCustom( vSelected ) ) { return; }
			if ( ::MessageBoxW( Wnd(), LSN_LSTR( LSN_STR_DELETE_CHEATS_CONFIRM ), LSN_LSTR( LSN_STR_DELETE_CHEAT ), MB_YESNO | MB_ICONQUESTION ) != IDYES ) { return; }
			for ( auto I = vSelected.size(); I--; ) {
				if ( CCheatManager::IsCustom( uint32_t( vSelected[I] ) ) ) {
					m_pcmCheatManager->DeleteCustomCheat( uint32_t( vSelected[I] ) );
				}
			}
			m_pcmCheatManager->CheatsChanged();
			AddCheats( true );
		}
		catch ( ... ) {}
	}

	/**
	 * Selects a single cheat in the TreeListView, expanding its game if needed.
	 * 
	 * \param _ui32Idx The extended index of the cheat to select.
	 **/
	void CCheatsWindow::SelectCheat( uint32_t _ui32Idx ) {
		auto ptlvTree = reinterpret_cast<CTreeListView *>(FindChild( Layout::LSN_CWI_CHEAT_TREELISTVIEW ));
		if ( !ptlvTree ) { return; }
		ptlvTree->BeginLargeUpdate();
		HTREEITEM htiParent = NULL;
		for ( auto hThis = ptlvTree->GetNext( TVI_ROOT ); hThis; hThis = ptlvTree->GetNext( hThis ) ) {
			auto lpParm = ptlvTree->GetItemLParam( hThis );
			if ( lpParm & 0x80000000U ) {
				htiParent = hThis;
				ptlvTree->SetItemSelection( hThis, false );
				continue;
			}
			bool bThis = uint32_t( lpParm ) == _ui32Idx;
			if ( bThis && htiParent ) { ptlvTree->SetItemExpand( htiParent, TRUE ); }
			ptlvTree->SetItemSelection( hThis, bThis );
		}
		ptlvTree->FinishUpdate();
		// Scroll as little as possible to show it.
		try {
			std::vector<size_t> vRows;
			ptlvTree->GatherSelected( vRows );
			if ( vRows.size() && vRows[0] < size_t( INT_MAX ) ) {
				::SendMessageW( ptlvTree->Wnd(), LVM_ENSUREVISIBLE, static_cast<WPARAM>(vRows[0]), FALSE );
			}
		}
		catch ( ... ) {}
		UpdateSelection();
	}

	/**
	 * Gets the game to which a new cheat is added:  the game of the selected cheat or game, else the only game shown, else the filter text.
	 * 
	 * \return Returns the name of the game for a new cheat.
	 **/
	std::u16string CCheatsWindow::GameForNewCheat() const {
		std::u16string u16sGame;
		if ( nullptr == m_pcmCheatManager ) { return u16sGame; }
		try {
			if ( m_pcmCheatManager->RunningGame().size() ) { return m_pcmCheatManager->RunningGame(); }
			auto ptlvTree = reinterpret_cast<const CTreeListView *>(FindChild( Layout::LSN_CWI_CHEAT_TREELISTVIEW ));
			if ( ptlvTree ) {
				std::vector<LPARAM> vSelected;
				ptlvTree->GatherSelectedLParam( vSelected, true );
				for ( auto lpThis : vSelected ) {
					if ( lpThis & 0x80000000U ) {
						auto aFound = m_mGameNameAssociation.find( lpThis );
						if ( aFound != m_mGameNameAssociation.end() ) {
							return CUtilities::XStringToU16String( aFound->second.c_str(), aFound->second.size() );
						}
					}
					else if ( m_pcmCheatManager->IsValidIdx( uint32_t( lpThis ) ) ) {
						return m_pcmCheatManager->CheatByIdx( uint32_t( lpThis ) ).u16sFile;
					}
				}
			}
			auto sGames = m_pcmCheatManager->GatherGamesWithFilter();
			if ( sGames.size() == 1 ) { return (*sGames.begin()); }
			const auto & wsFilter = m_pcmCheatManager->GetCheatFilter();
			u16sGame = CUtilities::XStringToU16String( wsFilter.c_str(), wsFilter.size() );
		}
		catch ( ... ) {}
		return u16sGame;
	}

	/**
	 * Gathers selected cheats, omitting the game namess from the return.
	 * 
	 * \return Returns an array of cheat ID's with the game names omitted.
	 **/
	std::vector<LPARAM> CCheatsWindow::GatherSelectedGames() const {
		auto ptlvTree = reinterpret_cast<const CTreeListView *>(FindChild( Layout::LSN_CWI_CHEAT_TREELISTVIEW ));
		if ( !ptlvTree ) { return std::vector<LPARAM>(); }
		std::vector<LPARAM> vSelected;
		ptlvTree->GatherSelectedLParam( vSelected, true );
		for ( auto I = vSelected.size(); I--; ) {
			if ( vSelected[I] & 0x80000000U ) { vSelected.erase( vSelected.begin() + I ); }
		}
		return vSelected;
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A template for dialog pages embedded into other dialogs/windows.
 */

#include "LSNCheatsWindow.h"
#include "../../Localization/LSNLocalization.h"
#include "../Layout/LSNLayoutMacros.h"
#include "../Layout/LSNLayoutManager.h"

#include <Rebar/LSWRebar.h>
#include <ToolBar/LSWToolBar.h>
#include <TreeListView/LSWTreeListView.h>

#include "../../../resource.h"

namespace lsn {

	CCheatsWindow::CCheatsWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pcmCheatManager( reinterpret_cast<CCheatManager *>(_ui64Data) ) {

		static const struct {
			WORD				wImageName;
			DWORD				dwConst;
		} sImages[] = {
			{ IDB_CHEAT_ADD_24,				LSN_I_ADD },
			{ IDB_CHEAT_DELETE_24,			LSN_I_DELETE },
			{ IDB_CHEAT_EDIT_24,			LSN_I_EDIT },
			{ IDB_CHEAT_DUPLICATE_24,		LSN_I_DUPLICATE },
			{ IDB_CHEAT_FILTER_24,			LSN_I_FILTER },
			{ IDB_CHEAT_SEARCH_24,			LSN_I_SEARCH },
		};
		m_iImages.Create( 24, 24, ILC_COLOR32, LSN_I_TOTAL, LSN_I_TOTAL );

		for ( size_t I = 0; I < std::size( sImages ); ++I ) {
			m_bBitmaps[sImages[I].dwConst].LoadFromResource( sImages[I].wImageName, IMAGE_BITMAP, 0, 0, LR_LOADTRANSPARENT | LR_CREATEDIBSECTION );
			m_iImageMap[sImages[I].dwConst] = m_iImages.Add( m_bBitmaps[sImages[I].dwConst].Handle() );
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
				{ m_iImageMap[LSN_I_ADD],					Layout::LSN_CWI_ADD_BUTTON,					TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		LSN_TOOL_STR( LSN_LSTR( LSN_STR_ADD ) ) },
				{ m_iImageMap[LSN_I_DELETE],				Layout::LSN_CWI_DELETE_BUTTON,				TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		LSN_TOOL_STR( LSN_LSTR( LSN_STR_DELETE ) ) },
				{ -1,										0,											TBSTATE_ENABLED,	BTNS_SEP,		{ 0 },		0,		0 },
				{ m_iImageMap[LSN_I_EDIT],					Layout::LSN_CWI_EDIT_BUTTON,				TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		LSN_TOOL_STR( LSN_LSTR( LSN_STR_EDIT ) ) },
				{ m_iImageMap[LSN_I_DUPLICATE],				Layout::LSN_CWI_DUPLICATE_BUTTON,			TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		LSN_TOOL_STR( LSN_LSTR( LSN_STR_DUPLICATE ) ) },
				{ -1,										0,											TBSTATE_ENABLED,	BTNS_SEP,		{ 0 },		0,		0 },
				{ m_iImageMap[LSN_I_FILTER],				Layout::LSN_CWI_FILTER_BUTTON,				TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		LSN_TOOL_STR( LSN_LSTR( LSN_STR_FILTER ) ) },
				{ m_iImageMap[LSN_I_SEARCH],				Layout::LSN_CWI_SEARCH_BUTTON,				TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		LSN_TOOL_STR( LSN_LSTR( LSN_STR_SEARCH ) ) },
				/*{ 150,										Layout::LSN_CWI_FILTER_BUTTON,			TBSTATE_ENABLED,	BTNS_SEP,		{ 0 },		0,		0 },
				{ 150,										Layout::LSN_CWI_SEARCH_BUTTON,					TBSTATE_ENABLED,	BTNS_SEP,		{ 0 },		0,		0 },*/
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

		{
			auto ptlTree = reinterpret_cast<CTreeListView *>(FindChild( Layout::LSN_CWI_CHEAT_TREELISTVIEW ));
			if ( ptlTree ) {
				ptlTree->SetColumnText( LSN_LSTR( LSN_STR_CODES ), 0 );
				ptlTree->SetColumnWidth( 0, 287 );
				ptlTree->InsertColumn( LSN_LSTR( LSN_STR_HOTKEY_S_ ), 100, -1 );
				ptlTree->FitColumndsToControlWidth( 0 );

				AddCheats();
				ptlTree->FitColumndsToControlWidth( 1 );
			}
		}


		UpdateRects();
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
	CWidget::LSW_HANDLED CCheatsWindow::Command( WORD /*_wCtrlCode*/, WORD _wId, CWidget * /*_pwSrc*/ ) {
		switch ( _wId ) {
			case Layout::LSN_CWI_CANCEL : {
				return Close();
			}
			case Layout::LSN_CWI_OK : {
				::EndDialog( Wnd(), 1 );
				return LSW_H_HANDLED;
			}

			case Layout::LSN_CWI_EXPAND_SELECTED : {
				lsw::CTreeListView * ptlTree = static_cast<lsw::CTreeListView *>(FindChild( Layout::LSN_CWI_CHEAT_TREELISTVIEW ));
				if ( ptlTree ) {
					ptlTree->ExpandSelected();
				}
				break;
			}
			case Layout::LSN_CWI_EXPAND_ALL : {
				lsw::CTreeListView * ptlTree = static_cast<lsw::CTreeListView *>(FindChild( Layout::LSN_CWI_CHEAT_TREELISTVIEW ));
				if ( ptlTree ) {
					ptlTree->ExpandAll();
				}
				break;
			}
			case Layout::LSN_CWI_COLLAPSE_SELECTED : {
				lsw::CTreeListView * ptlTree = static_cast<lsw::CTreeListView *>(FindChild( Layout::LSN_CWI_CHEAT_TREELISTVIEW ));
				if ( ptlTree ) {
					ptlTree->CollapseSelected();
				}
				break;
			}
			case Layout::LSN_CWI_COLLAPSE_ALL : {
				lsw::CTreeListView * ptlTree = static_cast<lsw::CTreeListView *>(FindChild( Layout::LSN_CWI_CHEAT_TREELISTVIEW ));
				if ( ptlTree ) {
					ptlTree->CollapseAll();
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
			std::vector<LPARAM> vSelected;
			ptlvTree->GatherSelectedLParam( vSelected );
			for ( auto I = vSelected.size(); I--; ) {
				if ( vSelected[I] == -1 ) { vSelected.erase( vSelected.begin() + I ); }
			}

			bool bSelected = vSelected.size() != 0;
			bool bAllSelectedAreCustom = true;
			for ( auto I = vSelected.size(); I--; ) {
				if ( uint32_t( vSelected[I] & 0x80000000 ) == 0 ) {
					bAllSelectedAreCustom = false;
					break;
				}
			}
			lsw::LSW_MENU_ITEM miMenuBar[] = {
				{ FALSE,		Layout::LSN_CWI_ADD_BUTTON,						FALSE,		FALSE,		TRUE,												LSN_LSTR( LSN_STR__ADD_CHEAT ),				FALSE },
				{ FALSE,		Layout::LSN_CWI_DELETE_BUTTON,					FALSE,		FALSE,		bSelected && bHasItems && bAllSelectedAreCustom,	LSN_LSTR( LSN_STR_DE_LETE_CHEAT ),			FALSE },
				{ FALSE,		Layout::LSN_CWI_DUPLICATE_BUTTON,				FALSE,		FALSE,		bSelected && bHasItems,								LSN_LSTR( LSN_STR__DUPLICATE_CHEAT ),		FALSE },
				{ FALSE,		Layout::LSN_CWI_EDIT_BUTTON,					FALSE,		FALSE,		bSelected && bHasItems && bAllSelectedAreCustom,	LSN_LSTR( LSN_STR__EDIT_CHEAT ),			FALSE },
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
	 * Fills the TreeListView with filtered cheats.
	 * 
	 * \return Returns the number of cheats added to the TreeListView.
	 **/
	size_t CCheatsWindow::AddCheats() {
		if ( nullptr == m_pcmCheatManager ) { return 0; }
		auto ptlTree = reinterpret_cast<CTreeListView *>(FindChild( Layout::LSN_CWI_CHEAT_TREELISTVIEW ));
		if ( !ptlTree ) { return 0; }
		try {
			std::vector<LPARAM> vSelected;
			ptlTree->GatherSelectedLParam( vSelected, true );
			ptlTree->DeleteAll();

			auto sGames = m_pcmCheatManager->GatherGamesWithFilter();
			for ( auto I = sGames.begin(); I != sGames.end(); ++I ) {
				auto vCheats = m_pcmCheatManager->GatherCheatsForGame( (*I) );
				HTREEITEM hParent = NULL;
				// If there is only one game after the filter, no need to add a heirarchy.  Just make a flat list.
				if ( sGames.size() > 1 ) {
					// Add the game names with children.
					std::wstring pwsTmp = CUtilities::XStringToWString( (*I).c_str(), (*I).size() );
					TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( reinterpret_cast<const WCHAR *>(pwsTmp.c_str()),
						-1, TVI_ROOT );
					hParent = ptlTree->InsertItem( &isInsertMe );
				}
				for ( size_t J = 0; J < vCheats.size(); ++J ) {
					auto cCheat = m_pcmCheatManager->CheatByIdx( vCheats[J] );
					std::wstring wsText;
					if ( cCheat.wsNotes.size() ) {
						wsText = std::format( L"{}: {}", cCheat.wsDescription, cCheat.wsNotes );
					}
					else {
						wsText = cCheat.wsDescription;
					}
					TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( reinterpret_cast<const WCHAR *>(wsText.c_str()),
						vCheats[J], hParent );
						ptlTree->InsertItem( &isInsertMe );
				}
			}

		}
		catch ( ... ) {}
		return ptlTree->GetItemCount();
	}

	/**
	 * Updates the addresses and notes edits based on the TreeListView selection.
	 **/
	void CCheatsWindow::UpdateSelection() {
		try {
			auto ptlTree = reinterpret_cast<CTreeListView *>(FindChild( Layout::LSN_CWI_CHEAT_TREELISTVIEW ));
			if ( !ptlTree || !m_pcmCheatManager ) { return; }
			std::vector<LPARAM> vSelected;
			ptlTree->GatherSelectedLParam( vSelected, true );
			for ( auto I = vSelected.size(); I--; ) {
				if ( vSelected[I] == -1 ) { vSelected.erase( vSelected.begin() + I ); }
			}

			auto pwCodes = FindChild( Layout::LSN_CWI_CODE_EDIT );
			if ( !pwCodes ) { return; }
			auto pwNotes = FindChild( Layout::LSN_CWI_NOTES_EDIT );
			if ( !pwNotes ) { return; }

			std::wstring wsCodes;
			std::wstring wsNotes;
			for ( size_t I = 0; I < vSelected.size(); ++I ) {
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
							wsCodes += std::format( L"{} ({:04X}:{:02X}:{:02X})",
								CUtilities::XStringToWString( ceEntry.vAddresses[J].sCode.c_str(), ceEntry.vAddresses[J].sCode.size() ),
								ceEntry.vAddresses[J].ui16Address, ceEntry.vAddresses[J].ui8Value, ceEntry.vAddresses[J].ui8CompareValue );
						}
						else {
							wsCodes += std::format( L"{} ({:04X}:{:02X})",
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
		}
		catch ( ... ) {}
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

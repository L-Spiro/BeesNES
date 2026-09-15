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

#include <Rebar/LSWRebar.h>
#include <ToolBar/LSWToolBar.h>

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

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

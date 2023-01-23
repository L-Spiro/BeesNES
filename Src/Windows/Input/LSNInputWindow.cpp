#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The input window.
 */

#include "LSNInputWindow.h"
#include "../../Localization/LSNLocalization.h"
#include <ListBox/LSWListBox.h>
#include <Tab/LSWTab.h>


namespace lsn {

	CInputWindow::CInputWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
	}

	// == Functions.
	/**
	 * The WM_INITDIALOG handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CInputWindow::InitDialog() {
		Parent::InitDialog();

		CTab * ptTab = static_cast<CTab *>(FindChild( Layout::LSN_IWI_TAB ));
		// Order of pushing them here determines the order in the window/list.
		/*CWidget * pwGlobal = Layout::CreateGlobalPage( this );
		CWidget * pwPerGame = Layout::CreatePerGamePage( this );*/
		CWidget * pwGlobal = Layout::CreateGlobalPage( ptTab );
		CWidget * pwPerGame = Layout::CreatePerGamePage( ptTab );
		m_vPages.push_back( static_cast<CInputPage *>(pwGlobal) );
		m_vPages.push_back( static_cast<CInputPage *>(pwPerGame) );

		if ( ptTab ) {
			ptTab->SetShowCloseBoxes( false );
			TCITEMW tciItem = { 0 };
			tciItem.mask = TCIF_TEXT;
			for ( size_t I = 0; I < m_vPages.size(); ++I ) {
				std::wstring wsTitle = m_vPages[I]->GetName();
				tciItem.pszText = const_cast<LPWSTR>(wsTitle.c_str());
				ptTab->InsertItem( int( I ), &tciItem, m_vPages[I] );
			}

			LSW_RECT rPanelClient = pwGlobal->WindowRect();
			ptTab->AdjustRect( TRUE, &rPanelClient );
			LSW_RECT rWindow = ptTab->WindowRect();
			::MoveWindow( ptTab->Wnd(), rWindow.left, rWindow.top, rPanelClient.Width(), rPanelClient.Height(), FALSE );
		

			LSW_RECT rTab = ptTab->WindowRect();
			::AdjustWindowRectEx( &rTab, GetStyle(), FALSE, GetStyleEx() );
			rWindow = WindowRect();
			::MoveWindow( Wnd(), rWindow.left, rWindow.top, rTab.Width(), rTab.Height(), FALSE );
		}
		
		ForceSizeUpdate();
		return CMainWindow::InitDialog();
	}

	/**
	 * The WM_CLOSE handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CInputWindow::Close() {
		::EndDialog( Wnd(), -1 );
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
	CWidget::LSW_HANDLED CInputWindow::Command( WORD _wCtrlCode, WORD _wId, CWidget * _pwSrc ) {
		return LSW_H_CONTINUE;
	}

	// Virtual client rectangle.  Can be used for things that need to be adjusted based on whether or not status bars, toolbars, etc. are present.
	// pwChild can be nullptr.  If not nullptr, it is assumed to be a child of the widget, and this widget might create a specific rectangle for
	//	the pwChild, such as for splitter controls.
	const LSW_RECT CInputWindow::VirtualClientRect( const CWidget * pwChild ) const {
		if ( pwChild->Id() == CInputWindowLayout::LSN_IWI_PER_GAME_SETTINGS_PANEL || pwChild->Id() == CInputWindowLayout::LSN_IWI_GLOBAL_SETTINGS_PANEL ) {
			const CTab * ptTab = static_cast<const CTab *>(FindChild( Layout::LSN_IWI_TAB ));
			if ( ptTab ) {
				return ptTab->VirtualClientRect( pwChild );
			}
		}
		return ClientRect( pwChild );
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS
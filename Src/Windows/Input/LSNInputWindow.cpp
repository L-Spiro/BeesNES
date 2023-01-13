#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The input window.
 */

#include "LSNInputWindow.h"
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
		CWidget * pwGlobal = Layout::CreateGlobalPage( ptTab );
		m_vPages.push_back( static_cast<CInputPage *>(pwGlobal) );

		if ( ptTab ) {
			
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

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS
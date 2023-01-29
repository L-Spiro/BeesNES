#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The controller-configuration dialog.
 */

#include "LSNControllerSetupWindow.h"


namespace lsn {

	CControllerSetupWindow::CControllerSetupWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
	}

	/**
	 * The WM_INITDIALOG handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CControllerSetupWindow::InitDialog() {


		return CMainWindow::InitDialog();
	}

	/**
	 * The WM_CLOSE handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CControllerSetupWindow::Close() {
		::EndDialog( Wnd(), -1 );
		return LSW_H_HANDLED;
	}

	/**
	 * Called after focus on the control is lost or a key has been registered.
	 *
	 * \param _pwControl The control used for listening.
	 * \param _bSuccess Set to true if the call is due to a key being registered.
	 * \return Returns true if the listening state is LSW_LS_LISTENING and the old control procedure was restored.
	 */
	bool CControllerSetupWindow::StopListening_Keyboard( CWidget * _pwControl, bool _bSuccess ) {
		if ( !lsw::CInputListenerBase::StopListening_Keyboard( _pwControl, _bSuccess ) ) { return false; }


		return true;
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

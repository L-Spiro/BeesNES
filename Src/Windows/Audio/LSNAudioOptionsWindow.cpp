#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The input window.
 */

#include "LSNAudioOptionsWindow.h"
#include "../../Localization/LSNLocalization.h"
#include "../Layout/LSNLayoutMacros.h"
#include <ListBox/LSWListBox.h>
#include <Tab/LSWTab.h>


namespace lsn {

	CAudioOptionsWindow::CAudioOptionsWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data )/*,
		m_poOptions( reinterpret_cast<LSN_CONTROLLER_SETUP_DATA *>(_ui64Data)->poOptions ),
		m_pmwMainWindow( reinterpret_cast<LSN_CONTROLLER_SETUP_DATA *>(_ui64Data)->pmwMainWindow )*/ {
	}

	// == Functions.
	/**
	 * The WM_INITDIALOG handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CAudioOptionsWindow::InitDialog() {
		Parent::InitDialog();

		CTab * ptTab = reinterpret_cast<CTab *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_TAB ));
		if ( ptTab ) {
			ptTab->SetShowCloseBoxes( false );

			CWidget * pwGlobalPage = CAudioOptionsWindowLayout::CreateGlobalPage( ptTab, (*m_poOptions) );
			CWidget * pwPerGamePage = CAudioOptionsWindowLayout::CreatePerGamePage( ptTab, (*m_poOptions) );
		
			m_vPages.push_back( pwGlobalPage );
			m_vPages.push_back( pwPerGamePage );

			TCITEMW tciItem = { 0 };
			tciItem.mask = TCIF_TEXT;
			tciItem.pszText = const_cast<LPWSTR>(LSN_LSTR( LSN_AUDIO_OPTIONS_GENERAL ));
			if ( ptTab->InsertItem( 0, &tciItem, pwGlobalPage ) != -1 ) {
				LSW_RECT rTabWindow = ptTab->WindowRect();
				ptTab->AdjustRect( FALSE, &rTabWindow );
				rTabWindow = rTabWindow.ScreenToClient( ptTab->Wnd() );
				//rPanelClient = pscpPage->WindowRect();
				//LSW_RECT rWindow = ptTab->WindowRect().ScreenToClient( pscpPage->Wnd() );
				//LSW_RECT rWindow = ptTab->ClientRect();
				//rWindow = rWindow.MapWindowPoints( pscpPage->Wnd(), Wnd() );
				::MoveWindow( pwGlobalPage->Wnd(), rTabWindow.left, rTabWindow.top, rTabWindow.Width(), rTabWindow.Height(), FALSE );
			}
			if ( ptTab->InsertItem( 1, &tciItem, pwPerGamePage ) != -1 ) {
			}
		}

		//ForceSizeUpdate();
		return CMainWindow::InitDialog();
	}

	/**
	 * The WM_CLOSE handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CAudioOptionsWindow::Close() {
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
	CWidget::LSW_HANDLED CAudioOptionsWindow::Command( WORD /*_wCtrlCode*/, WORD _wId, CWidget * /*_pwSrc*/ ) {
		switch ( _wId ) {
			case Layout::LSN_AOWI_CANCEL : {
				return Close();
			}
			case Layout::LSN_AOWI_OK : {
				SaveAndClose();
				return LSW_H_HANDLED;
			}
		}
		return LSW_H_CONTINUE;
	}

	/**
	 * Virtual client rectangle.  Can be used for things that need to be adjusted based on whether or not status bars, toolbars, etc. are present.
	 *	pwChild can be nullptr.  If not nullptr, it is assumed to be a child of the widget, and this widget might create a specific rectangle for
	 *	the pwChild, such as for splitter controls.
	 *
	 * \param pwChild The child window for whom the virtual client rectangle should be returned.
	 * \return Returns the virtual client rectangle for this window for the given widget.
	 */
	const LSW_RECT CAudioOptionsWindow::VirtualClientRect( const CWidget * pwChild ) const {
		/*if ( pwChild->Id() == CInputWindowLayout::LSN_IWI_PER_GAME_SETTINGS_PANEL || pwChild->Id() == CInputWindowLayout::LSN_IWI_GLOBAL_SETTINGS_PANEL ) {
			const CTab * ptTab = static_cast<const CTab *>(FindChild( Layout::LSN_IWI_TAB ));
			if ( ptTab ) {
				return ptTab->VirtualClientRect( pwChild );
			}
		}*/
		return ClientRect( pwChild );
	}

	/**
	 * Saves the current input configuration and closes the dialog.
	 */
	void CAudioOptionsWindow::SaveAndClose() {
		/*for ( auto I = m_vPages.size(); I--; ) {
			m_vPages[I]->Save();
		}*/
		::EndDialog( Wnd(), 0 );
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS
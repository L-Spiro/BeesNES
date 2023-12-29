#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The controller-configuration dialog.
 */

#include "LSNControllerSetupWindow.h"
#include "LSNStdControllerPageLayout.h"


namespace lsn {

	CControllerSetupWindow::CControllerSetupWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
	}
	CControllerSetupWindow::~CControllerSetupWindow() {
		m_vPages.clear();
	}

	/**
	 * The WM_INITDIALOG handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CControllerSetupWindow::InitDialog() {
		CTab * ptTab = GetTab();
		if ( ptTab ) {
			ptTab->SetShowCloseBoxes( false );
			LSW_RECT rPanelClient;
			
			static LPWSTR const lpwstrTabTitles[] = {
				const_cast<LPWSTR>(LSN_LSTR( LSN_STD_INPUT_BUTTONS )),
				const_cast<LPWSTR>(LSN_LSTR( LSN_STD_INPUT_ALTTERNATIVE_BUTTONS_1 )),
				const_cast<LPWSTR>(LSN_LSTR( LSN_STD_INPUT_ALTTERNATIVE_BUTTONS_2 )),
				const_cast<LPWSTR>(LSN_LSTR( LSN_STD_INPUT_ALTTERNATIVE_BUTTONS_3 )),
				//const_cast<LPWSTR>(LSN_LSTR( LSN_STD_INPUT_RAPID )),
			};
			for ( size_t I = LSN_ELEMENTS( lpwstrTabTitles ); I--; ) {
				CStdControllerPage * pscpPage = static_cast<CStdControllerPage *>(CStdControllerPageLayout::CreatePage( this ));
				if ( pscpPage ) {
					TCITEMW tciItem = { 0 };
					tciItem.mask = TCIF_TEXT;
					tciItem.pszText = lpwstrTabTitles[I];
					if ( ptTab->InsertItem( 0, &tciItem, pscpPage ) != -1 ) {
						m_vPages.push_back( pscpPage );
						LSW_RECT rTabWindow = ptTab->WindowRect();
						ptTab->AdjustRect( FALSE, &rTabWindow );
						rTabWindow = rTabWindow.ScreenToClient( pscpPage->Wnd() );
						//rPanelClient = pscpPage->WindowRect();
						//LSW_RECT rWindow = ptTab->WindowRect().ScreenToClient( pscpPage->Wnd() );
						//LSW_RECT rWindow = ptTab->ClientRect();
						//rWindow = rWindow.MapWindowPoints( pscpPage->Wnd(), Wnd() );
						::MoveWindow( pscpPage->Wnd(), rTabWindow.left, rTabWindow.top, rTabWindow.Width(), rTabWindow.Height(), FALSE );
						pscpPage = nullptr;
					}
				}
				delete pscpPage;
			}

			
			
			LSW_RECT rSelected = ptTab->GetItemRect( ptTab->GetCurSel() );
			LSW_RECT rWindow = ptTab->WindowRect();
			LSW_RECT rClient = ptTab->ClientRect();
			//::MoveWindow( ptTab->Wnd(), rWindow.left, rWindow.top, rPanelClient.Width(), rPanelClient.Height(), FALSE );
		}
		ForceSizeUpdate();
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
	 * Window rectangle.
	 * 
	 * \param _pwChild If not nullptr, this is the child control for which to determine the window rectangle.
	 * \return Returns the window rectangle for this control or of a given child control.
	 **/
	LSW_RECT CControllerSetupWindow::WindowRect( const CWidget * /*_pwChild*/ ) const {
		
		return Parent::WindowRect();
	}

	/**
	 * Client rectangle.
	 * 
	 * \param _pwChild If not nullptr, this is the child control for which to determine the client rectangle.
	 * \return Returns the client rectangle for this control or of a given child control.
	 **/
	LSW_RECT CControllerSetupWindow::ClientRect( const CWidget * _pwChild ) const {
		if ( _pwChild && _pwChild->Id() == CStdControllerPageLayout::LSN_SCPI_MAINWINDOW ) {
			CTab * ptTab = GetTab();
			LSW_RECT rClient = ptTab->ClientRect();
			LSW_RECT rItem = ptTab->GetItemRect( ptTab->GetCurSel() );
			rClient.left += rItem.left;
			rClient.right -= rItem.left;
			rClient.top += rItem.Height() + rItem.top * 2;
			rClient.bottom -= rItem.top;
			//rClient.MoveBy( rItem.left, rItem.Height() + rItem.top * 2 );
			//return rClient;
			/*if ( ptTab && ptTab->GetItemCount() ) {
				return ptTab->GetItemRect( ptTab->GetCurSel() );
			}*/
		}
		return Parent::ClientRect();
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

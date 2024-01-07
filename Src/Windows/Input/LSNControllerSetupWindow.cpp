#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The controller-configuration dialog.
 */

#include "LSNControllerSetupWindow.h"
#include "../MainWindow/LSNMainWindow.h"
#include "../WinUtilities/LSNWinUtilities.h"
#include "LSNStdControllerPageLayout.h"
#include <ComboBox/LSWComboBox.h>
#include <TrackBar/LSWTrackBar.h>


namespace lsn {

	CControllerSetupWindow::CControllerSetupWindow( const lsw::LSW_WIDGET_LAYOUT &_wlLayout, lsw::CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pioOptions( reinterpret_cast<LSN_CONTROLLER_SETUP_DATA *>(_ui64Data)->pioOptions ),
		m_pmwMainWindow( reinterpret_cast<LSN_CONTROLLER_SETUP_DATA *>(_ui64Data)->pmwMainWindow ),
		m_stPlayerIdx( reinterpret_cast<LSN_CONTROLLER_SETUP_DATA *>(_ui64Data)->stIdx ) {
	}
	CControllerSetupWindow::~CControllerSetupWindow() {
		// Unnecessary; if not deleted here they are deleted when the tab closes.
		for ( auto I = m_vPages.size(); I--; ) {
			delete m_vPages[I];
			m_vPages[I] = nullptr;
		}
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
			};
			for ( size_t I = LSN_ELEMENTS( lpwstrTabTitles ); I--; ) {
				CStdControllerPage * pscpPage = static_cast<CStdControllerPage *>(CStdControllerPageLayout::CreatePage( this, (*m_pioOptions), m_pmwMainWindow, (m_stPlayerIdx << 16) | (I & 0xFFFF) ));
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
	 * Handles the WM_COMMAND message.
	 *
	 * \param _wCtrlCode 0 = from menu, 1 = from accelerator, otherwise it is a Control-defined notification code.
	 * \param _wId The ID of the control if _wCtrlCode is not 0 or 1.
	 * \param _pwSrc The source control if _wCtrlCode is not 0 or 1.
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CControllerSetupWindow::Command( WORD /*_wCtrlCode*/, WORD _wId, CWidget * /*_pwSrc*/ ) {
		switch ( _wId ) {
			case Layout::LSN_CSW_OK : {
				SaveAndClose();
				return LSW_H_HANDLED;
			}
			case Layout::LSN_CSW_CANCEL : {
				Close();
				return LSW_H_HANDLED;
			}
		}
		return LSW_H_CONTINUE;
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

	/**
	 * Saves the current input configuration and closes the dialog.
	 */
	void CControllerSetupWindow::SaveAndClose() {
		for ( auto I = m_vPages.size(); I--; ) {
			m_vPages[I]->Save();
		}
		::EndDialog( Wnd(), 0 );
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

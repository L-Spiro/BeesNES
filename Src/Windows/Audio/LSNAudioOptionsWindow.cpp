#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The audio options window.
 */

#include "LSNAudioOptionsWindow.h"
#include "../../Localization/LSNLocalization.h"
#include "../Layout/LSNLayoutMacros.h"
#include "LSNAudioOptionsGeneralPage.h"
#include "LSNAudioOptionsRecordingPage.h"

#include <ListBox/LSWListBox.h>
#include <Tab/LSWTab.h>

#include "../../../resource.h"


namespace lsn {

	CAudioOptionsWindow::CAudioOptionsWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_poOptions( reinterpret_cast<LSN_OPTIONS *>(_ui64Data) ) {
	}

	// == Functions.
	/**
	 * The WM_INITDIALOG handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CAudioOptionsWindow::InitDialog() {
		Parent::InitDialog();

		SetIcons( reinterpret_cast<HICON>(::LoadImageW( CBase::GetModuleHandleW( nullptr ), MAKEINTRESOURCEW( IDI_AUDIO_ICON_16 ), IMAGE_ICON, 0, 0, LR_LOADTRANSPARENT )),
			reinterpret_cast<HICON>(::LoadImageW( CBase::GetModuleHandleW( nullptr ), MAKEINTRESOURCEW( IDI_AUDIO_ICON_32 ), IMAGE_ICON, 0, 0, LR_LOADTRANSPARENT )) );

		lsw::CTab * ptTab = reinterpret_cast<lsw::CTab *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_TAB ));
		LONG lBottomSpace = 0;

		if ( ptTab ) {
			ptTab->SetShowCloseBoxes( false );


			CWidget * pwGlobalPage = CAudioOptionsWindowLayout::CreateGlobalPage( this, (*m_poOptions) );
			CWidget * pwPerGamePage = CAudioOptionsWindowLayout::CreatePerGamePage( this, (*m_poOptions) );
			CWidget * pwRecordingPage = CAudioOptionsWindowLayout::CreateRecordingPage( this, (*m_poOptions) );
			if ( !pwGlobalPage || !pwPerGamePage || !pwRecordingPage ) { return LSW_H_HANDLED; }
		
			m_vPages.push_back( pwGlobalPage );
			m_vPages.push_back( pwPerGamePage );
			m_vPages.push_back( pwRecordingPage );

			LSW_RECT rWindow = WindowRect();
			lBottomSpace = ClientRect().bottom - ptTab->ClientRect().bottom;


			TCITEMW tciItem = { 0 };
			tciItem.mask = TCIF_TEXT;
			static LPWSTR const lpwstrTabTitles[] = {
				const_cast<LPWSTR>(LSN_LSTR( LSN_AUDIO_OPTIONS_GLOBAL_SETTINGS )),
				const_cast<LPWSTR>(LSN_LSTR( LSN_AUDIO_OPTIONS_PERGAME_SETTINGS )),
				const_cast<LPWSTR>(LSN_LSTR( LSN_AUDIO_OPTIONS_STREAM_TO_FILE )),
			};

			ptTab->SetShowCloseBoxes( false );
			for ( size_t I = 0; I < m_vPages.size(); ++I ) {
				std::wstring wsTitle = lpwstrTabTitles[I];
				tciItem.pszText = const_cast<LPWSTR>(wsTitle.c_str());
				ptTab->InsertItem( int( I ), &tciItem, m_vPages[I] );

				if ( I == 0 ) {
					LSW_RECT rThisRect = m_vPages[I]->WindowRect();
					LSW_RECT rTabWindow = rThisRect;
					ptTab->AdjustRect( TRUE, &rTabWindow );
					rTabWindow = rTabWindow.ScreenToClient( Wnd() );
					::MoveWindow( ptTab->Wnd(), 0, 0, rTabWindow.Width(), rTabWindow.Height(), FALSE );
				}
				LSW_RECT rTabWindow = ptTab->WindowRect();
				ptTab->AdjustRect( FALSE, &rTabWindow );
				rTabWindow = rTabWindow.ScreenToClient( ptTab->Wnd() );
				::MoveWindow( m_vPages[I]->Wnd(), rTabWindow.left, rTabWindow.top, rTabWindow.Width(), rTabWindow.Height(), FALSE );
			}
			ptTab->SetCheckable( 1 );
			ptTab->SetChecked( 1, !m_poOptions->aoThisGameAudioOptions.bUseGlobal );
			if ( m_poOptions && !m_poOptions->aoThisGameAudioOptions.bUseGlobal ) {
				ptTab->SetCurSel( 1 );
			}
			else {
				ptTab->SetCurSel( 0 );
			}

			LSW_RECT rTab = ptTab->WindowRect();
			::AdjustWindowRectEx( &rTab, GetStyle(), FALSE, GetStyleEx() );
			rWindow = WindowRect();
			::MoveWindow( Wnd(), rWindow.left, rWindow.top, rTab.Width(), rTab.Height() + lBottomSpace, FALSE );
		}
		if ( m_vPages.size() >= 2 ) {
			reinterpret_cast<CAudioOptionsGeneralPage<false> *>(m_vPages[1])->Update();
		}
		//ForceSizeUpdate();
		return LSW_H_CONTINUE;
	}

	/**
	 * The WM_CLOSE handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CAudioOptionsWindow::Close() {
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
	 * Saves the current input configuration and closes the dialog.
	 */
	void CAudioOptionsWindow::SaveAndClose() {
		if ( m_vPages.size() >= 3 ) {
			std::wstring wsErr;
			auto pwErrorWidget = reinterpret_cast<CAudioOptionsRecordingPage *>(m_vPages[2])->Verify( wsErr );
			if ( pwErrorWidget ) {
				lsw::CTab * ptTab = reinterpret_cast<lsw::CTab *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_TAB ));
				if ( ptTab ) {
					ptTab->SetCurSel( 2 );
					pwErrorWidget->SetFocus();
				}
				lsw::CBase::MessageBoxError( Wnd(), wsErr.c_str(), LSN_LSTR( LSN_ERROR ) );
				return;
			}
			reinterpret_cast<CAudioOptionsRecordingPage *>(m_vPages[2])->Save();
		}
		// Not so graceful.
		if ( m_vPages.size() >= 1 ) {
			reinterpret_cast<CAudioOptionsGeneralPage<true> *>(m_vPages[0])->Save();
		}
		if ( m_vPages.size() >= 2 ) {
			reinterpret_cast<CAudioOptionsGeneralPage<false> *>(m_vPages[1])->Save();
		}
		::EndDialog( Wnd(), 1 );
	}

	/**
	 * Informs the control that a child tab control has just had a check toggled.
	 * 
	 * \param _pwTab A pointer to the tab control.
	 * \param _iTab The index of the tab that was just toggled.
	 **/
	void CAudioOptionsWindow::TabToggled( CWidget * _pwTab, int /*_iTab*/ ) {
		if ( _pwTab ) {
			lsw::CTab * ptTab = reinterpret_cast<lsw::CTab *>(_pwTab);
			ptTab->SetCurSel( ptTab->IsChecked( 1 ) ? 1 : 0 );
			if ( m_vPages.size() >= 2 ) {
				reinterpret_cast<CAudioOptionsGeneralPage<false> *>(m_vPages[1])->Update();
			}

			::InvalidateRect( Wnd(), NULL, FALSE );
			Paint();
		}
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS
#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The Video Options dialog.
 */

#include "LSNVideoOptionsWindow.h"
#include "../../Localization/LSNLocalization.h"
#include "../MainWindow/LSNMainWindow.h"
#include "LSNVideoOptionsPage.h"
#include "LSNVideoOptionsSettingsPage.h"

#include <Tab/LSWTab.h>

#include <algorithm>
#include <string>


namespace lsn {

	CVideoOptionsWindow::CVideoOptionsWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pvodData( reinterpret_cast<CVideoOptionsWindowLayout::LSN_VIDEO_OPTIONS_DATA *>(_ui64Data) ) {
	}

	// == Functions.
	/**
	 * The WM_INITDIALOG handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CVideoOptionsWindow::InitDialog() {
		Parent::InitDialog();
		if ( !m_pvodData || !m_pvodData->poOptions ) { return LSW_H_CONTINUE; }
		LSW_SETREDRAW srRedraw( this );

		// Remember the settings so that Cancel can restore them.
		LSN_OPTIONS & oOptions = (*m_pvodData->poOptions);
		m_vfoGlobalBackup = oOptions.vfoGlobalFilters;
		m_vfoPerGameBackup = oOptions.vfoThisGameFilters;
		m_hdoHostBackup = oOptions.hdoHostDisplay;
		for ( size_t I = 0; I < LSN_PM_CONSOLE_TOTAL; ++I ) {
			m_poGlobalPaletteBackup[I] = oOptions.poGlobalPalettes[I];
			m_poPerGamePaletteBackup[I] = oOptions.poThisGamePalette[I];
		}

		lsw::CTab * ptTab = static_cast<lsw::CTab *>(FindChild( CWinUtilities::LSN_VOWI_TAB ));
		if ( !ptTab ) { return LSW_H_CONTINUE; }
		ptTab->SetShowCloseBoxes( false );

		m_pdHost.pvodData = m_pvodData;
		m_pdHost.vpkKind = CVideoOptionsFields::LSN_VPK_HOST;
		m_pdHost.bGlobal = true;
		m_pdGlobal.pvodData = m_pvodData;
		m_pdGlobal.vpkKind = CVideoOptionsFields::LSN_VPK_PALETTE;
		m_pdGlobal.bGlobal = true;
		m_pdPerGame.pvodData = m_pvodData;
		m_pdPerGame.vpkKind = CVideoOptionsFields::LSN_VPK_PALETTE;
		m_pdPerGame.bGlobal = false;

		CWidget * pwGlobal = CVideoOptionsWindowLayout::CreateSettingsPage( this, &m_pdGlobal );
		CWidget * pwPerGame = CVideoOptionsWindowLayout::CreateSettingsPage( this, &m_pdPerGame );
		// The Host Machine page sizes its columns with the font of the filter pages (the font of a settings page).
		CWidget * pwHost = CVideoOptionsWindowLayout::CreateFieldPage( this, &m_pdHost, pwGlobal );
		if ( !pwHost || !pwGlobal || !pwPerGame ) { return LSW_H_CONTINUE; }
		m_vPages.push_back( pwHost );
		m_vPages.push_back( pwGlobal );
		m_vPages.push_back( pwPerGame );

		const LONG lBottomSpace = ClientRect().bottom - ptTab->ClientRect().bottom;

		// Fit the tab around the largest page.
		LONG lW = 0, lH = 0;
		for ( auto pwPage : m_vPages ) {
			LSW_RECT rPage = pwPage->WindowRect();
			lW = std::max<LONG>( lW, rPage.Width() );
			lH = std::max<LONG>( lH, rPage.Height() );
		}

		static const wchar_t * pwcTitles[] = {
			LSN_LSTR( LSN_STR_VO_HOST_MACHINE ),
			LSN_LSTR( LSN_STR_VO_GLOBAL_SETTINGS ),
			LSN_LSTR( LSN_STR_VO_PERGAME_SETTINGS ),
		};
		TCITEMW tciItem = { 0 };
		tciItem.mask = TCIF_TEXT;
		for ( size_t I = 0; I < m_vPages.size(); ++I ) {
			std::wstring wsTitle = pwcTitles[I];
			tciItem.pszText = const_cast<LPWSTR>(wsTitle.c_str());
			ptTab->InsertItem( int( I ), &tciItem, m_vPages[I] );
		}
		LSW_RECT rTab = { 0, 0, lW, lH };
		ptTab->AdjustRect( TRUE, &rTab );
		::MoveWindow( ptTab->Wnd(), 0, 0, rTab.Width(), rTab.Height(), FALSE );
		LSW_RECT rDisplay = ptTab->ClientRect();
		ptTab->AdjustRect( FALSE, &rDisplay );
		for ( auto pwPage : m_vPages ) {
			::MoveWindow( pwPage->Wnd(), rDisplay.left, rDisplay.top, rDisplay.Width(), rDisplay.Height(), FALSE );
		}

		ptTab->SetCheckable( LSN_T_PER_GAME );
		ptTab->SetChecked( LSN_T_PER_GAME, PerGameInUse() );
		ptTab->SetCurSel( PerGameInUse() ? LSN_T_PER_GAME : LSN_T_GLOBAL );

		// Fit the window around the tab (the buttons follow the bottom).
		LSW_RECT rTabWindow = ptTab->WindowRect();
		::AdjustWindowRectEx( &rTabWindow, GetStyle(), FALSE, GetStyleEx() );
		LSW_RECT rWindow = WindowRect();
		::MoveWindow( Wnd(), rWindow.left, rWindow.top, rTabWindow.Width(), rTabWindow.Height() + lBottomSpace, FALSE );
		return LSW_H_CONTINUE;
	}

	/**
	 * The WM_CLOSE handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CVideoOptionsWindow::Close() {
		return CancelAndClose();
	}

	/**
	 * Handles the WM_COMMAND message.
	 *
	 * \param _wCtrlCode 0 = from menu, 1 = from accelerator, otherwise it is a Control-defined notification code.
	 * \param _wId The ID of the control if _wCtrlCode is not 0 or 1.
	 * \param _pwSrc The source control if _wCtrlCode is not 0 or 1.
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CVideoOptionsWindow::Command( WORD /*_wCtrlCode*/, WORD _wId, CWidget * /*_pwSrc*/ ) {
		switch ( _wId ) {
			case CWinUtilities::LSN_VOWI_CANCEL : {
				return CancelAndClose();
			}
			case CWinUtilities::LSN_VOWI_OK : {
				// The settings are already in place.
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
	CWidget::LSW_HANDLED CVideoOptionsWindow::MenuCommand( WORD _wId ) {
		switch ( _wId ) {
			case IDCANCEL : {}													LSN_FALLTHROUGH
			case CWinUtilities::LSN_VOWI_CANCEL : {
				return CancelAndClose();
			}
		}
		return Parent::MenuCommand( _wId );
	}

	/**
	 * Informs the control that a child tab control has just had a check toggled.
	 *
	 * \param _pwTab A pointer to the tab control.
	 * \param _iTab The index of the tab that was just toggled.
	 **/
	void CVideoOptionsWindow::TabToggled( CWidget * _pwTab, int _iTab ) {
		if ( !_pwTab || _iTab != LSN_T_PER_GAME || !m_pvodData || !m_pvodData->poOptions ) { return; }
		lsw::CTab * ptTab = static_cast<lsw::CTab *>(_pwTab);
		const bool bPerGame = ptTab->IsChecked( LSN_T_PER_GAME );
		LSN_OPTIONS & oOptions = (*m_pvodData->poOptions);
		for ( size_t I = 0; I < LSN_PM_CONSOLE_TOTAL; ++I ) {
			oOptions.vfoThisGameFilters.froRegions[I].bUseGlobal = !bPerGame;
			oOptions.poThisGamePalette[I].bUseGlobal = !bPerGame;
		}
		ApplyChanges();
		ptTab->SetCurSel( bPerGame ? LSN_T_PER_GAME : LSN_T_GLOBAL );
		if ( m_vPages.size() > LSN_T_PER_GAME ) {
			static_cast<CVideoOptionsSettingsPage *>(m_vPages[LSN_T_PER_GAME])->Refresh();
		}
		::InvalidateRect( Wnd(), NULL, FALSE );
	}

	/**
	 * Restores the settings the dialog started with, applies them, and closes the dialog.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CVideoOptionsWindow::CancelAndClose() {
		if ( m_pvodData && m_pvodData->poOptions ) {
			LSN_OPTIONS & oOptions = (*m_pvodData->poOptions);
			oOptions.vfoGlobalFilters = m_vfoGlobalBackup;
			oOptions.vfoThisGameFilters = m_vfoPerGameBackup;
			oOptions.hdoHostDisplay = m_hdoHostBackup;
			for ( size_t I = 0; I < LSN_PM_CONSOLE_TOTAL; ++I ) {
				oOptions.poGlobalPalettes[I] = m_poGlobalPaletteBackup[I];
				oOptions.poThisGamePalette[I] = m_poPerGamePaletteBackup[I];
			}
			ApplyChanges();
		}
		::EndDialog( Wnd(), 0 );
		return LSW_H_HANDLED;
	}

	/**
	 * Determines whether the per-game settings are in use.
	 *
	 * \return Returns true if any region uses its per-game settings.
	 */
	bool CVideoOptionsWindow::PerGameInUse() const {
		if ( !m_pvodData || !m_pvodData->poOptions ) { return false; }
		for ( const auto & froThis : m_pvodData->poOptions->vfoThisGameFilters.froRegions ) {
			if ( !froThis.bUseGlobal ) { return true; }
		}
		return false;
	}

	/**
	 * Applies the options to the emulator.
	 */
	void CVideoOptionsWindow::ApplyChanges() {
		if ( m_pvodData && m_pvodData->pmwMainWindow ) {
			m_pvodData->pmwMainWindow->ApplyVideoOptions();
		}
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

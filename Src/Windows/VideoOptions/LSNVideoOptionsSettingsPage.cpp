#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A settings page of the Video Options dialog (Global Settings or Per-Game Settings), which holds a tab for each filter.
 */

#include "LSNVideoOptionsSettingsPage.h"
#include "LSNVideoOptionsPage.h"

#include <Tab/LSWTab.h>

#include <algorithm>
#include <string>


namespace lsn {

	CVideoOptionsSettingsPage::CVideoOptionsSettingsPage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_ppdData( reinterpret_cast<CVideoOptionsWindowLayout::LSN_PAGE_DATA *>(_ui64Data) ) {
	}

	// == Functions.
	/**
	 * The WM_INITDIALOG handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CVideoOptionsSettingsPage::InitDialog() {
		Parent::InitDialog();
		lsw::CTab * ptTab = static_cast<lsw::CTab *>(FindChild( CWinUtilities::LSN_VOWI_SETTINGS_TAB ));
		if ( !ptTab || !m_ppdData ) { return LSW_H_CONTINUE; }
		ptTab->SetShowCloseBoxes( false );

		static const CVideoOptionsFields::LSN_VIDEO_PAGE_KIND vpkKinds[] = {
			CVideoOptionsFields::LSN_VPK_PALETTE,
			CVideoOptionsFields::LSN_VPK_BLARGG,
			CVideoOptionsFields::LSN_VPK_LSPIRO_NTSC,
			CVideoOptionsFields::LSN_VPK_LSPIRO_PAL,
			CVideoOptionsFields::LSN_VPK_LMP_NTSC,
			CVideoOptionsFields::LSN_VPK_LMP_PAL,
		};
		try {
			// Create the filter pages.
			for ( auto vpkKind : vpkKinds ) {
				auto pdData = std::make_unique<CVideoOptionsWindowLayout::LSN_PAGE_DATA>();
				pdData->pvodData = m_ppdData->pvodData;
				pdData->vpkKind = vpkKind;
				pdData->bGlobal = m_ppdData->bGlobal;
				CWidget * pwPage = CVideoOptionsWindowLayout::CreateFieldPage( this, pdData.get() );
				if ( !pwPage ) { continue; }
				m_vPageData.push_back( std::move( pdData ) );
				m_vPages.push_back( static_cast<CVideoOptionsPage *>(pwPage) );
			}
			if ( m_vPages.empty() ) { return LSW_H_CONTINUE; }

			// Fit the tab around the largest page.
			LONG lW = 0, lH = 0;
			for ( auto pPage : m_vPages ) {
				LSW_RECT rPage = pPage->WindowRect();
				lW = std::max<LONG>( lW, rPage.Width() );
				lH = std::max<LONG>( lH, rPage.Height() );
			}
			LSW_RECT rTab = { 0, 0, lW, lH };
			ptTab->AdjustRect( TRUE, &rTab );
			::MoveWindow( ptTab->Wnd(), 0, 0, rTab.Width(), rTab.Height(), FALSE );

			TCITEMW tciItem = { 0 };
			tciItem.mask = TCIF_TEXT;
			for ( size_t I = 0; I < m_vPages.size(); ++I ) {
				std::wstring wsTitle = CVideoOptionsFields::TabName( m_vPageData[I]->vpkKind );
				tciItem.pszText = const_cast<LPWSTR>(wsTitle.c_str());
				ptTab->InsertItem( int( I ), &tciItem, m_vPages[I] );
			}
			// Insertion can change the height of the tab strip; place the pages after all of the tabs exist.
			LSW_RECT rDisplay = ptTab->ClientRect();
			ptTab->AdjustRect( FALSE, &rDisplay );
			if ( rDisplay.Width() < lW || rDisplay.Height() < lH ) {
				rTab = { 0, 0, lW, lH };
				ptTab->AdjustRect( TRUE, &rTab );
				::MoveWindow( ptTab->Wnd(), 0, 0, rTab.Width(), rTab.Height(), FALSE );
				rDisplay = ptTab->ClientRect();
				ptTab->AdjustRect( FALSE, &rDisplay );
			}
			for ( auto pPage : m_vPages ) {
				::MoveWindow( pPage->Wnd(), rDisplay.left, rDisplay.top, rDisplay.Width(), rDisplay.Height(), FALSE );
			}
			ptTab->SetCurSel( 0 );

			// Fit this page around the tab.
			LSW_RECT rTabWindow = ptTab->WindowRect();
			::SetWindowPos( Wnd(), NULL, 0, 0, rTabWindow.Width(), rTabWindow.Height(), SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE );
		}
		catch ( ... ) {}
		return LSW_H_CONTINUE;
	}

	/**
	 * Reloads every filter page from the options.
	 */
	void CVideoOptionsSettingsPage::Refresh() {
		for ( auto pPage : m_vPages ) {
			pPage->Refresh();
		}
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

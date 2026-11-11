#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The About dialog:  the creator, the copyright, the link to the GitHub repository, and the credits and licenses of the
 *	third-party code, data, and icons.
 */

#include "LSNAboutWindow.h"
#include "../../Localization/LSNLocalization.h"

#include <shellapi.h>

namespace lsn {

	/** The URL of the BeesNES repository. */
	static constexpr const wchar_t *							LSN_ABOUT_GITHUB_URL = L"https://github.com/L-Spiro/BeesNES";
	/** The URL of Icons8 (its license asks for a visible link in the About section of desktop applications). */
	static constexpr const wchar_t *							LSN_ABOUT_ICONS8_URL = L"https://icons8.com";

	CAboutWindow::CAboutWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
	}

	// == Functions.
	/**
	 * The WM_INITDIALOG handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CAboutWindow::InitDialog() {
		Parent::InitDialog();

		CWidget * pwGitHub = FindChild( CWinUtilities::LSN_ABWI_GITHUB );
		if ( pwGitHub ) { pwGitHub->SetTextW( LSN_ABOUT_GITHUB_URL ); }
		CWidget * pwIcons8 = FindChild( CWinUtilities::LSN_ABWI_ICONS8 );
		if ( pwIcons8 ) { pwIcons8->SetTextW( (std::wstring( LSN_LSTR( LSN_STR_ABOUT_ICONS8 ) ) + L":  " + LSN_ABOUT_ICONS8_URL).c_str() ); }

		CWidget * pwCredits = FindChild( CWinUtilities::LSN_ABWI_CREDITS_EDIT );
		if ( pwCredits ) {
			// The notices of the third-party code and data (names and licenses are not translated).
			pwCredits->SetTextW(
				L"Icons by Icons8 (https://icons8.com), used under the Icons8 free license.\r\n"
				L"\r\n"
				L"nes_ntsc 0.2.2 (the Blargg NTSC filter), Copyright (C) 2006 Shay Green (blargg).  Used under the GNU Lesser "
					L"General Public License, version 2.1.\r\n"
				L"\r\n"
				L"NTSC/CRT and PAL/CRT (the LMP88959 filters) by EMMIR, 2018-2023.\r\n"
				L"\r\n"
				L"palgen-persune, Copyright 2023 Persune.  Used under the MIT No Attribution license.\r\n"
				L"\r\n"
				L"miniz by Rich Geldreich.  Released into the public domain (the Unlicense).\r\n"
				L"\r\n"
				L"OpenAL Soft.  Used under the GNU Lesser General Public License.\r\n"
				L"\r\n"
				L"The ROM database is nes20db, the NES 2.0 XML database by NewRisingSun.\r\n"
				L"\r\n"
								L"CRT and decoder-chip color data:  ChthonVII (gamutthingy), patchy68k, PlainOldPants, Keith Raney, and DariusG.\r\n" );
		}
		return LSW_H_CONTINUE;
	}

	/**
	 * The WM_CLOSE handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CAboutWindow::Close() {
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
	CWidget::LSW_HANDLED CAboutWindow::Command( WORD /*_wCtrlCode*/, WORD _wId, CWidget * /*_pwSrc*/ ) {
		switch ( _wId ) {
			case CWinUtilities::LSN_ABWI_OK : {}		LSN_FALLTHROUGH
			case IDCANCEL : {
				::EndDialog( Wnd(), 1 );
				return LSW_H_HANDLED;
			}
			case CWinUtilities::LSN_ABWI_GITHUB : {
				// STN_CLICKED (the labels have SS_NOTIFY).
				OpenUrl( LSN_ABOUT_GITHUB_URL );
				return LSW_H_HANDLED;
			}
			case CWinUtilities::LSN_ABWI_ICONS8 : {
				OpenUrl( LSN_ABOUT_ICONS8_URL );
				return LSW_H_HANDLED;
			}
			default : {}
		}
		return LSW_H_CONTINUE;
	}

	/**
	 * Handles WM_CTLCOLORSTATIC:  draws the links in the link color.
	 *
	 * \param _hDc Device context for the static control.
	 * \param _pwControl The static control widget.
	 * \param _hBrush Output brush used to paint the background.
	 * \return Returns a LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CAboutWindow::CtlColorStatic( HDC _hDc, CWidget * _pwControl, HBRUSH &_hBrush ) {
		if ( !IsLink( _pwControl ) ) { return LSW_H_CONTINUE; }
		::SetTextColor( _hDc, ::GetSysColor( COLOR_HOTLIGHT ) );
		::SetBkMode( _hDc, TRANSPARENT );
		_hBrush = ::GetSysColorBrush( COLOR_BTNFACE );
		return LSW_H_HANDLED;
	}

	/**
	 * Handles WM_SETCURSOR:  shows the hand cursor over the links.
	 *
	 * \param _pwControl The control under the cursor, or nullptr.
	 * \param _wHitTest Hit-test code (HT*).
	 * \param _wIdent Mouse message identifier (e.g., WM_MOUSEMOVE).
	 * \return Returns a LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CAboutWindow::SetCursor( CWidget * _pwControl, WORD /*_wHitTest*/, WORD /*_wIdent*/ ) {
		if ( !IsLink( _pwControl ) ) { return LSW_H_CONTINUE; }
		::SetCursor( ::LoadCursorW( NULL, IDC_HAND ) );
		return LSW_H_HANDLED;
	}

	/**
	 * Determines whether a control is one of the links.
	 *
	 * \param _pwControl The control to check.
	 * \return Returns true if the control is a link.
	 */
	bool CAboutWindow::IsLink( const CWidget * _pwControl ) {
		if ( !_pwControl ) { return false; }
		return _pwControl->Id() == CWinUtilities::LSN_ABWI_GITHUB || _pwControl->Id() == CWinUtilities::LSN_ABWI_ICONS8;
	}

	/**
	 * Opens a URL in the default browser.
	 *
	 * \param _pwcUrl The URL to open.
	 */
	void CAboutWindow::OpenUrl( const wchar_t * _pwcUrl ) {
		::ShellExecuteW( Wnd(), L"open", _pwcUrl, nullptr, nullptr, SW_SHOWNORMAL );
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

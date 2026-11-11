#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The About dialog:  the creator, the copyright, the link to the GitHub repository, and the credits and licenses of the
 *	third-party code, data, and icons.
 */

#pragma once

#include "../../LSNLSpiroNes.h"
#include "../WinUtilities/LSNWinUtilities.h"

#include <MainWindow/LSWMainWindow.h>

using namespace lsw;

namespace lsn {

	/**
	 * Class CAboutWindow
	 * \brief The About dialog.
	 *
	 * Description: The About dialog:  the creator, the copyright, the link to the GitHub repository, and the credits and licenses of the
	 *	third-party code, data, and icons (the Icons8 license asks for a visible link to icons8.com in the About section of desktop
	 *	applications).
	 */
	class CAboutWindow : public lsw::CMainWindow {
	public :
		CAboutWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Types.
		/** The parent type. */
		typedef lsw::CMainWindow								Parent;


		// == Functions.
		/**
		 * The WM_INITDIALOG handler.
		 *
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED										InitDialog();

		/**
		 * The WM_CLOSE handler.
		 *
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED										Close();

		/**
		 * Handles the WM_COMMAND message.
		 *
		 * \param _wCtrlCode 0 = from menu, 1 = from accelerator, otherwise it is a Control-defined notification code.
		 * \param _wId The ID of the control if _wCtrlCode is not 0 or 1.
		 * \param _pwSrc The source control if _wCtrlCode is not 0 or 1.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED										Command( WORD _wCtrlCode, WORD _wId, CWidget * _pwSrc );

		/**
		 * Handles WM_CTLCOLORSTATIC:  draws the links in the link color.
		 *
		 * \param _hDc Device context for the static control.
		 * \param _pwControl The static control widget.
		 * \param _hBrush Output brush used to paint the background.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED										CtlColorStatic( HDC _hDc, CWidget * _pwControl, HBRUSH &_hBrush );

		/**
		 * Handles WM_SETCURSOR:  shows the hand cursor over the links.
		 *
		 * \param _pwControl The control under the cursor, or nullptr.
		 * \param _wHitTest Hit-test code (HT*).
		 * \param _wIdent Mouse message identifier (e.g., WM_MOUSEMOVE).
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED										SetCursor( CWidget * _pwControl, WORD _wHitTest, WORD _wIdent );


	protected :
		// == Functions.
		/**
		 * Determines whether a control is one of the links.
		 *
		 * \param _pwControl The control to check.
		 * \return Returns true if the control is a link.
		 */
		static bool												IsLink( const CWidget * _pwControl );

		/**
		 * Opens a URL in the default browser.
		 *
		 * \param _pwcUrl The URL to open.
		 */
		void													OpenUrl( const wchar_t * _pwcUrl );
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

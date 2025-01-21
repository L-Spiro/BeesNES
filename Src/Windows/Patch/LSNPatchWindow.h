#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The patch window.
 */

#pragma once

#include "LSNPatchWindowLayout.h"
#include <MainWindow/LSWMainWindow.h>

using namespace lsw;

namespace lsn {

	class											CMainWindow;

	/**
	 * Class CPatchWindow
	 * \brief The patch window.
	 *
	 * Description: The patch window.
	 */
	class CPatchWindow : public lsw::CMainWindow {
	public :
		CPatchWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Functions.
		/**
		 * The WM_INITDIALOG handler.
		 *
		 * \return Returns an LSW_HANDLED code.
		 */
		LSW_HANDLED									InitDialog();

		/**
		 * The WM_CLOSE handler.
		 *
		 * \return Returns an LSW_HANDLED code.
		 */
		LSW_HANDLED									Close();


	private :
		typedef CPatchWindowLayout					Layout;
		typedef lsw::CMainWindow					Parent;

	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

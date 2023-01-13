#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The input window.
 */

#pragma once

#include "LSNInputWindowLayout.h"
#include "LSNInputPage.h"
#include <MainWindow/LSWMainWindow.h>

using namespace lsw;

namespace lsn {

	/**
	 * Class CInputWindow
	 * \brief The input window.
	 *
	 * Description: The input window.
	 */
	class CInputWindow : public lsw::CMainWindow {
	public :
		CInputWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Functions.
		/**
		 * The WM_INITDIALOG handler.
		 *
		 * \return Returns an LSW_HANDLED code.
		 */
		LSW_HANDLED							InitDialog();

		/**
		 * The WM_CLOSE handler.
		 *
		 * \return Returns an LSW_HANDLED code.
		 */
		LSW_HANDLED							Close();




	protected :
		// == Members.
		/** The input-settings pages. */
		std::vector<CInputPage *>			m_vPages;

	private :
		typedef CInputWindowLayout			Layout;
		typedef lsw::CMainWindow			Parent;
	};


}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

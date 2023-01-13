#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: An input-settings page (a tab in the main input-configuration dialog).
 */

#pragma once

#include "LSNInputWindowLayout.h"
#include <MainWindow/LSWMainWindow.h>

using namespace lsw;

namespace lsn {

	/**
	 * Class CInputPage
	 * \brief An input-settings page (a tab in the main input-configuration dialog).
	 *
	 * Description: An input-settings page (a tab in the main input-configuration dialog).
	 */
	class CInputPage : public lsw::CWidget {
		CInputPage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 ) :
			lsw::CWidget( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
		}


		// == Functions.
		/**
		 * Get the name of the page.
		 *
		 * \return Returns the name of the page.
		 */
		virtual std::wstring					GetName() const { return std::wstring(); }
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

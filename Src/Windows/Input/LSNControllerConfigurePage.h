#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A dialog template for assigning controls for a standard controller.
 */

#pragma once

#include <MainWindow/LSWMainWindow.h>

using namespace lsw;

namespace lsn {

	/**
	 * Class CControllerConfigurePage
	 * \brief A template for dialog pages embedded into other dialogs/windows.
	 *
	 * Description: A template for dialog pages embedded into other dialogs/windows.
	 */
	class CControllerConfigurePage : public lsw::CWidget {
	public :
		CControllerConfigurePage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 ) :
			lsw::CWidget( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
		}


		// == Functions.


	protected :
		// == Members.
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

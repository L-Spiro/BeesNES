#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A template for dialog pages embedded into other dialogs/windows.
 */

#pragma once

#include <MainWindow/LSWMainWindow.h>

using namespace lsw;

namespace lsn {

	/**
	 * Class CTemplatePage
	 * \brief A template for dialog pages embedded into other dialogs/windows.
	 *
	 * Description: A template for dialog pages embedded into other dialogs/windows.
	 */
	class CTemplatePage : public lsw::CWidget {
	public :
		CTemplatePage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 ) :
			lsw::CWidget( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
		}


		// == Functions.


	protected :
		// == Members.
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

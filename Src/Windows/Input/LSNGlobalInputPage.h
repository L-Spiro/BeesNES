#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The global input settings page.
 */

#pragma once

#include "LSNInputPage.h"
#include "../../Localization/LSNLocalization.h"


namespace lsn {

	/**
	 * Class CGlobalInputPage
	 * \brief The global input settings page.
	 *
	 * Description: The global input settings page.
	 */
	class CGlobalInputPage : public CInputPage {
	public :
		CGlobalInputPage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 ) :
			CInputPage( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
		}


		// == Functions.
		/**
		 * Get the name of the page.
		 *
		 * \return Returns the name of the page.
		 */
		virtual std::wstring					GetName() const { return LSN_LSTR( LSN_INPUT_GLOBAL_SETTINGS ); }
	};

}	// namespace lsn


#endif	// #ifdef LSN_USE_WINDOWS

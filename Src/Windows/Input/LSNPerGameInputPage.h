#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The per-game input settings page.
 */

#pragma once

#include "LSNInputPage.h"
#include "../../Localization/LSNLocalization.h"


namespace lsn {

	/**
	 * Class CPerGameInputPage
	 * \brief The per-game input settings page.
	 *
	 * Description: The per-game input settings page.
	 */
	class CPerGameInputPage : public CInputPage {
	public :
		CPerGameInputPage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 ) :
			CInputPage( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
		}


		// == Functions.
		/**
		 * Get the name of the page.
		 *
		 * \return Returns the name of the page.
		 */
		virtual std::wstring					GetName() const { return LSN_LSTR( LSN_INPUT_PER_GAME_SETTINGS ); }
	};

}	// namespace lsn


#endif	// #ifdef LSN_USE_WINDOWS

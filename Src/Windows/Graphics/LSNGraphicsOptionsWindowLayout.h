#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The layout for the graphics options dialog.
 */

#pragma once

#include "../../LSNLSpiroNes.h"
#include "../../Options/LSNOptions.h"
#include "../WinUtilities/LSNWinUtilities.h"

#include <Layout/LSWMenuLayout.h>
#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>


namespace lsn {

	/**
	 * Class CGraphicsOptionsWindowLayout
	 * \brief The layout for the graphics options dialog.
	 *
	 * Description: The layout for the graphics options dialog.
	 */
	class CGraphicsOptionsWindowLayout {
	public :
		// == Enumerations.
		// Control ID's.
		enum LSN_GRAPHICS_OPTIONS_WINDOW_IDS : WORD {
			LSN_GOWI_NONE,
			LSN_GOWI_MAINWINDOW					= 1890,
			LSN_GOWI_TAB,
			LSN_GOWI_OK,
			LSN_GOWI_CANCEL,
		};
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

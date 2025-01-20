#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The patch window layout.
 */

#pragma once

#include "../../LSNLSpiroNes.h"
#include "../../Options/LSNOptions.h"
#include <Layout/LSWMenuLayout.h>
#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace lsn {

	/**
	 * Class CPatchWindowLayout
	 * \brief The patch window layout.
	 *
	 * Description: The patch window layout.
	 */
	class CPatchWindowLayout {
	public :
		// == Enumerations.
		// Control ID's.
		enum LSN_PATCH_WINDOW_IDS : WORD {
			LSN_PWI_NONE,
			LSN_PWI_MAINWINDOW,

		};


		// == Functions.
		// Creates the window.
		static CWidget *						CreatePatchWindow( CWidget * _pwParent );


	protected :
		// == Members.
		/** The layout for the main window. */
		static LSW_WIDGET_LAYOUT				m_wlPatchWindow[];
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

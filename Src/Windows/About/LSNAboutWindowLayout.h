#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The layout for the About dialog.
 */

#pragma once

#include "../../LSNLSpiroNes.h"
#include "../WinUtilities/LSNWinUtilities.h"

#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace lsn {
	
	/**
	 * Class CAboutWindowLayout
	 * \brief The layout for the About dialog.
	 *
	 * Description: The layout for the About dialog.
	 */
	class CAboutWindowLayout {
	public :
		// == Functions.
		/**
		 * Creates the About dialog.
		 *
		 * \param _pwParent The parent of the dialog.
		 * \return Returns TRUE if the dialog was created successfully.
		 */
		static BOOL								CreateAboutDialog( CWidget * _pwParent );


	protected :
		// == Members.
		/** The layout for the About dialog. */
		static LSW_WIDGET_LAYOUT				m_wlDialog[];
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

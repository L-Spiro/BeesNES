#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The layout for the Cheats dialog.
 */

#pragma once

#include "../../LSNLSpiroNes.h"
#include "../../Cheats/LSNCheatManager.h"
#include "../../Options/LSNOptions.h"
#include "../WinUtilities/LSNWinUtilities.h"


#include <Layout/LSWMenuLayout.h>
#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace lsn {
	
	/**
	 * Class CCheatsWindowLayout
	 * \brief The layout for the Cheats dialog.
	 *
	 * Description: The layout for the Cheats dialog.
	 */
	class CCheatsWindowLayout {
	public :
		// == Functions.
		/**
		 * Creates the Cheats dialog.
		 *
		 * \param _pwParent The parent of the page.
		 * \param _pcmCheatManager A pointer to the cheat manager.
		 * \return TRUE if the dialog was created successfully.
		 */
		static BOOL								CreateCheatsDialog( CWidget * _pwParent, CCheatManager * _pcmCheatManager );


	protected :
		// == Members.
		/** The layout for the per-game-setup panel. */
		static LSW_WIDGET_LAYOUT				m_wlPage[];


		// == Functions.
		/**
		 * Creates the pages.
		 *
		 * \param _pwParent The parent widget.
		 * \param _pwlLayout The page layout.
		 * \param _sTotal The number of items to which _pwlLayout points.
		 * \param _pcmCheatManager A pointer to the cheat manager.
		 * \return TRUE if the dialog was created successfully.
		 */
		static BOOL							CreatePage( CWidget * _pwParent, const LSW_WIDGET_LAYOUT * _pwlLayout, size_t _sTotal, CCheatManager * _pcmCheatManager );
 		
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

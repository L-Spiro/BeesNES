#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The layout for the Add/Edit/Duplicate Cheat dialog.
 */

#pragma once

#include "../../LSNLSpiroNes.h"
#include "../../Cheats/LSNCheatManager.h"
#include "../WinUtilities/LSNWinUtilities.h"

#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace lsn {

	/**
	 * Class CCheatEditWindowLayout
	 * \brief The layout for the Add/Edit/Duplicate Cheat dialog.
	 *
	 * Description: The layout for the Add/Edit/Duplicate Cheat dialog.
	 */
	class CCheatEditWindowLayout {
	public :
		// == Types.
		/** The data passed to and returned from the Add/Edit/Duplicate Cheat dialog. */
		struct LSN_CHEAT_EDIT_DATA {
			LSN_CHEAT_ENTRY											ceEntry;									/**< The cheat to show and the returned cheat.  u16sFile is passed through unchanged. */
			CCheatManager::LSN_ACTIVE_CHEAT							acActive;									/**< The cheat's enabled state and hotkeys, shown and returned.  ui32Idx is passed through unchanged. */
			const wchar_t *											pwcTitle = nullptr;							/**< The dialog title. */
		};


		// == Functions.
		/**
		 * Creates the Add/Edit/Duplicate Cheat dialog.
		 *
		 * \param _pwParent The parent of the dialog.
		 * \param _cedData The cheat to show (and the dialog title) and the returned cheat.
		 * \return Returns TRUE if the user accepted the changes, in which case _cedData holds the edited cheat.
		 */
		static BOOL													CreateCheatEditDialog( CWidget * _pwParent, LSN_CHEAT_EDIT_DATA &_cedData );


	protected :
		// == Members.
		/** The layout for the Add/Edit/Duplicate Cheat dialog. */
		static LSW_WIDGET_LAYOUT									m_wlDialog[];
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

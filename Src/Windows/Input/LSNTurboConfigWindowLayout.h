#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The layout for the Turbo Button Configuration dialog.
 */

#pragma once

#include "../../LSNLSpiroNes.h"
#include "../../Input/LSNTurboPattern.h"
#include "../WinUtilities/LSNWinUtilities.h"

#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

#include <string>

using namespace lsw;

namespace lsn {

	/**
	 * Class CTurboConfigWindowLayout
	 * \brief The layout for the Turbo Button Configuration dialog.
	 *
	 * Description: The layout for the Turbo Button Configuration dialog.
	 */
	class CTurboConfigWindowLayout {
	public :
		// == Types.
		/** The data passed to the Turbo Button Configuration dialog. */
		struct LSN_TURBO_CONFIG_DATA {
			CTurboPatterns *										ptpPatterns = nullptr;					/**< The presets to edit (saved to disk as they are changed). */
			std::u16string											u16Select;								/**< The preset to select when the dialog opens (if it exists). */
		};


		// == Functions.
		/**
		 * Creates the Turbo Button Configuration dialog.
		 *
		 * \param _pwParent The parent of the dialog.
		 * \param _tcdData The presets to edit.
		 * \return Returns TRUE if the dialog was shown.
		 */
		static BOOL													CreateTurboConfigDialog( CWidget * _pwParent, LSN_TURBO_CONFIG_DATA &_tcdData );


	protected :
		// == Members.
		/** The fixed part of the layout (the 64 state check boxes and their index labels are added when the dialog is created). */
		static LSW_WIDGET_LAYOUT									m_wlDialog[];
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

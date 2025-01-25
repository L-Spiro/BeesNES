#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2025
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

			LSN_PWI_FILE_GROUP,
			LSN_PWI_FILE_IN_LABEL,
			LSN_PWI_FILE_IN_EDIT,
			LSN_PWI_FILE_IN_BUTTON,

			LSN_PWI_FILE_PATCH_LABEL,
			LSN_PWI_FILE_PATCH_EDIT,
			LSN_PWI_FILE_PATCH_BUTTON,

			LSN_PWI_FILE_OUT_LABEL,
			LSN_PWI_FILE_OUT_EDIT,
			LSN_PWI_FILE_OUT_BUTTON,


			LSN_PWI_INFO_GROUP,
			LSN_PWI_INFO_CRC_LABEL,
			LSN_PWI_INFO_ROM_CRC_LABEL,
			LSN_PWI_INFO_ROM_PGM_SIZE_LABEL,
			LSN_PWI_INFO_ROM_CHR_SIZE_LABEL,
			LSN_PWI_INFO_ROM_MIRROR_LABEL,
		};


		// == Functions.
		// Creates the window.
		static CWidget *						CreatePatchWindow( CWidget * _pwParent, LSN_OPTIONS &_oOptions );


	protected :
		// == Members.
		/** The layout for the main window. */
		static LSW_WIDGET_LAYOUT				m_wlPatchWindow[];
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

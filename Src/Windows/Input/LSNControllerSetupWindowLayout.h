#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The input-configuration window layout.
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
	 * Class CControllerSetupWindowLayout
	 * \brief The input-configuration window layout.
	 *
	 * Description: The input-configuration window layout.
	 */
	class CControllerSetupWindowLayout {
	public :
		// == Enumerations.
		// Control ID's.
		enum LSN_CONTROLLER_SETUP_WINDOW_IDS : WORD {
			LSN_CSW_NONE,
			LSN_CSW_MAINWINDOW,
			LSN_CSW_TAB,
			LSN_CSW_OK,
			LSN_CSW_CANCEL,

			LSN_CSW_MAIN_PANEL,
			LSN_CSW_RAPID_PANEL,
		};


		// == Functions.
		/**
		 * Creates the input-configuration window.
		 *
		 * \param _pwParent The parent of the dialog.
		 * \param _oOptions A reference to the options object.
		 * \return Returns TRUE if the dialog was created successfully.
		 */
		static BOOL								CreateInputDialog( CWidget * _pwParent, LSN_OPTIONS &_oOptions );

		/**
		 * Creates the global-settings page.
		 *
		 * \param _pwParent the parent of the page.
		 * \param _oOptions A reference to the options object.
		 * \return Returns the created widget.
		 */
		static CWidget *						CreateGlobalPage( CWidget * _pwParent, LSN_OPTIONS &_oOptions );

		/**
		 * Creates the per-game-settings page.
		 *
		 * \param _pwParent the parent of the page.
		 * \param _oOptions A reference to the options object.
		 * \return Returns the created widget.
		 */
		static CWidget *						CreatePerGamePage( CWidget * _pwParent, LSN_OPTIONS &_oOptions );


	protected :
		// == Members.
		/** The layout for the main window. */
		static LSW_WIDGET_LAYOUT				m_wlInputWindow[];
		/** The layout for the global-setup panel. */
		static LSW_WIDGET_LAYOUT				m_wlGlobalPanel[];
		/** The layout for the per-game-setup panel. */
		static LSW_WIDGET_LAYOUT				m_wlPerGamePanel[];


		// == Functions.
		/**
		 * Creates the tab pages.
		 *
		 * \param _pwParent The parent widget.
		 * \param _pwlLayout The page layout.
		 * \param _sTotal The number of items to which _pwlLayout points.
		 * \param _oOptions A reference to the options object.
		 * \return Returns the created page.
		 */
		static CWidget *						CreatePage( CWidget * _pwParent, const LSW_WIDGET_LAYOUT * _pwlLayout, size_t _sTotal, LSN_OPTIONS &_oOptions );
 		
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

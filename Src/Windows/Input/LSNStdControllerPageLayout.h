#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A dialog shaped like a standard controller.
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
	 * Class CStdControllerPageLayout
	 * \brief A dialog shaped like a standard controller.
	 *
	 * Description: A dialog shaped like a standard controller.
	 */
	class CStdControllerPageLayout {
	public :
		// == Enumerations.
		// Control ID's.
		enum LSN_TEMPLATE_WINDOW_IDS : WORD {
			LSN_TWI_NONE,
			LSN_TWI_MAINWINDOW,
			LSN_TWI_TAB,
			LSN_TWI_OK,
			LSN_TWI_CANCEL,
		};


		// == Functions.
		/**
		 * Creates the page.
		 *
		 * \param _pwParent the parent of the page.
		 * \return Returns the created widget.
		 */
		static CWidget *						CreatePage( CWidget * _pwParent );


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
		 * \return Returns the created page.
		 */
		static CWidget *						CreatePage( CWidget * _pwParent, const LSW_WIDGET_LAYOUT * _pwlLayout, size_t _sTotal );
 		
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

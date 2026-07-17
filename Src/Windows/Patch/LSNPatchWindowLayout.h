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
#include <Splitter/LSWSplitter.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

//#define LSN_PATCHER_TAB_BOTTOM

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
		// Menu items
		enum LSN_MAINWINDOW_MENU_IDS {
			LSN_PWI_REVEAL_COMPATIBLE			= 855,
			LSN_PWI_SELECT_COMPATIBLE,
			LSN_PWI_REVEAL_CRC,
			LSN_PWI_COPY_FIELD_VALUE,
			LSN_PWI_COPY_EXPAND_SELECTED,
			LSN_PWI_COPY_EXPAND_ALL,
			LSN_PWI_COPY_COLLAPSE_SELECTED,
			LSN_PWI_COPY_COLLAPSE_ALL
			
		};


		// == Functions.
		// Creates the window.
		static CWidget *						CreatePatchWindow( CWidget * _pwParent, LSN_OPTIONS &_oOptions );

		/**
		 * Creates the top page.
		 *
		 * \param _pwParent The parent widget.
		 * \param _oOptions Parameter to pass to the created cotnrol.
		 * \return Returns the created page.
		 */
		static CWidget *						CreateTopPage( CWidget * _pwParent, LSN_OPTIONS &_oOptions );

		/**
		 * Creates the bottom page.
		 *
		 * \param _pwParent The parent widget.
		 * \param _oOptions Parameter to pass to the created cotnrol.
		 * \return Returns the created page.
		 */
		static CWidget *						CreateBottomPage( CWidget * _pwParent, LSN_OPTIONS &_oOptions );


	protected :
		// == Members.
		/** The layout for the main window. */
		static LSW_WIDGET_LAYOUT				m_wlPatchWindow[];
		/** The layout for the top page. */
		static LSW_WIDGET_LAYOUT				m_wlPatchWindowTopPage[];
		/** The layout for the bottom page. */
		static LSW_WIDGET_LAYOUT				m_wlPatchWindowBottomPage[];


		// == Functions.
		/**
		 * Creates the pages.
		 *
		 * \param _pwParent The parent widget.
		 * \param _pwlLayout The page layout.
		 * \param _sTotal The number of items to which _pwlLayout points.
		 * \param _oOptions Parameter to pass to the created cotnrol.
		 * \return Returns the created page.
		 */
		static CWidget *						CreatePage( CWidget * _pwParent, const LSW_WIDGET_LAYOUT * _pwlLayout, size_t _sTotal, LSN_OPTIONS &_oOptions );
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

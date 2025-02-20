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
			LSN_PWI_MAINWINDOW				= 3450,
			LSN_PWI_TOP_PAGE,
			LSN_PWI_BOTTOM_PAGE,
			LSN_PWI_SPLITTER,

			LSN_PWI_FILE_GROUP,
			LSN_PWI_FILE_IN_LABEL,
			LSN_PWI_FILE_IN_EDIT,
			LSN_PWI_FILE_IN_BUTTON,

			LSN_PWI_FILE_PATCH_LABEL,
			LSN_PWI_FILE_PATCH_TREELISTVIEW,
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


			LSN_PWI_BOTTOM_TABS,
		};

		// Menu items
		enum LSN_MAINWINDOW_MENU_IDS {
			LSN_PWI_COPY_FIELD_VALUE				= 855,
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
		 * \return Returns the created page.
		 */
		static CWidget *						CreateTopPage( CWidget * _pwParent, LSN_OPTIONS &_oOptions );

		/**
		 * Creates the bottom page.
		 *
		 * \param _pwParent The parent widget.
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
		 * \return Returns the created page.
		 */
		static CWidget *						CreatePage( CWidget * _pwParent, const LSW_WIDGET_LAYOUT * _pwlLayout, size_t _sTotal, LSN_OPTIONS &_oOptions );
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

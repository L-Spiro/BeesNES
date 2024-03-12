#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The main window layout.
 */

#pragma once

#include <Layout/LSWMenuLayout.h>
#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace lsn {
	
	/**
	 * Class CMainWindowLayout
	 * \brief The main window layout.
	 *
	 * Description: The main window layout.
	 */
	class CMainWindowLayout {
	public :
		// == Enumerations.
		// Control ID's.
		enum LSN_MAIN_WINDOW_IDS {
			LSN_MWI_NONE,
			LSN_MWI_MAINWINDOW,
			LSN_MWI_REBAR0,
			LSN_MWI_TOOLBAR0,
			LSN_MWI_STATUSBAR,

		};

		// Menu items
		enum LSN_MAINWINDOW_MENU_IDS {
			LSN_MWMI_MENU_BAR					= 300,
			LSN_MWMI_MENU_FILE,
			LSN_MWMI_MENU_OPTIONS,
			LSN_MWMI_MENU_TOOLS,
			LSN_MWMI_MENU_WINDOW,
			LSN_MWMI_MENU_HELP,

			LSN_MWMI_FILE						= 100,
			LSN_MWMI_SEARCH,
			LSN_MWMI_OPTIONS,
			LSN_MWMI_TOOLS,
			LSN_MWMI_WINDOW,
			LSN_MWMI_HELP,

			LSN_MWMI_OPENROM,
			LSN_MWMI_OPENRECENT,
			LSN_MWMI_OPENRECENT_MENU,
			LSN_MWMI_OPENFORDEBUG,
			LSN_MWMI_ADDENTRY,
			LSN_MWMI_OPENSAVEFILE,
			LSN_MWMI_SAVE,
			LSN_MWMI_SAVEAS,
			LSN_MWMI_DELETE,
			LSN_MWMI_DELETEALL,
			LSN_MWMI_LOCK,
			LSN_MWMI_UNLOCK,
			LSN_MWMI_EDIT,

			LSN_MWMI_VIDEO_SIZE,
			LSN_MWMI_VIDEO_FILTER,
			LSN_MWMI_INPUT,
			LSN_MWMI_VIDEO,

			LSN_MWMI_VIDEO_SIZES,
			LSN_MWMI_VIDEO_SIZE_1X,
			LSN_MWMI_VIDEO_SIZE_2X,
			LSN_MWMI_VIDEO_SIZE_3X,
			LSN_MWMI_VIDEO_SIZE_4X,
			LSN_MWMI_VIDEO_SIZE_5X,
			LSN_MWMI_VIDEO_SIZE_6X,

			LSN_MWMI_VIDEO_FILTERS,
			LSN_MWMI_VIDEO_FILTER_NONE,
			LSN_MWMI_VIDEO_FILTER_NTSC_BLARGG,
			LSN_MWMI_VIDEO_FILTER_PAL_BLARGG,
			LSN_MWMI_VIDEO_FILTER_NTSC_LSPIRO,
			LSN_MWMI_VIDEO_FILTER_PAL_LSPIRO,
			LSN_MWMI_VIDEO_FILTER_PALM_LSPIRO,
			LSN_MWMI_VIDEO_FILTER_PALN_LSPIRO,
			LSN_MWMI_VIDEO_FILTER_AUTO_BLARGG,
			LSN_MWMI_VIDEO_FILTER_NTSC_CRT,
			LSN_MWMI_VIDEO_FILTER_NTSC_CRT_FULL,
			LSN_MWMI_VIDEO_FILTER_PAL_CRT_FULL,
			LSN_MWMI_VIDEO_FILTER_AUTO_CRT,
			LSN_MWMI_VIDEO_FILTER_AUTO_CRT_FULL,
			LSN_MWMI_VIDEO_FILTER_AUTO_LSPIRO,

			LSN_MWMI_SHOWFOUNDADDR,
			LSN_MWMI_SHOW_EXPEVAL,
			LSN_MWMI_SHOW_CONVERTER,
			LSN_MWMI_SHOW_ALL,

			LSN_MWMI_SHOW_RECENT_BASE			= 600,
		};


		// == Functions.
		/**
		 * Creates the main window.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc., and registers the main window class.
		 *
		 * \param _pabIsAlive An atomic that tells the main loop that the window has closed and its pointer is no longer valid.
		 * \return Returns the main window widget.
		 */
		static CWidget *						CreateMainWindow( std::atomic_bool * _pabIsAlive );

		/**
		 * Creates the main menu and adds it to the window.
		 *
		 * \param _pwMainWindow The main window, used for parenting the menu.
		 * \return Returns TRUE if the menu was created.
		 */
		static BOOL								CreateMenu( CWidget * _pwMainWindow );

		/**
		 * Gets the main window widget.
		 *
		 * \return Returns the main window widget.
		 */
		static CWidget *						MainWindow() { return m_pwMainWindow; }


	protected :
		// == Members.
		/** The layout for the main window. */
		static LSW_WIDGET_LAYOUT				m_wlMainWindow[];

		/** Menu bar items for the main window. */
		static LSW_MENU_ITEM					m_miMenuBar[];

		/** File menu. */
		static LSW_MENU_ITEM					m_miFileMenu[];

		/** Options menu. */
		static LSW_MENU_ITEM					m_miOptionsMenu[];

		/** Tools menu. */
		static LSW_MENU_ITEM					m_miToolsMenu[];

		/** Windows menu. */
		static LSW_MENU_ITEM					m_miWindowsMenu[];

		/** Video-size sub-menu. */
		static LSW_MENU_ITEM					m_miVideoSizeMenu[];

		/** Video-filter sub-menu. */
		static LSW_MENU_ITEM					m_miVideoFilterMenu[];

		/** Menus. */
		static LSW_MENU_LAYOUT					m_miMenus[];

		/** The class for the main window. */
		static ATOM								m_aMainClass;

		/** The main window. */
		static CWidget *						m_pwMainWindow;


	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

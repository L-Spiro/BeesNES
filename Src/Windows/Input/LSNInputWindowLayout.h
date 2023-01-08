#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The input window layout.
 */

#pragma once

#include <Layout/LSWMenuLayout.h>
#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace lsn {
	
	/**
	 * Class CInputWindowLayout
	 * \brief Layout for the main MHS window.
	 *
	 * Description: Layout for the main MHS window.
	 */
	class CInputWindowLayout {
	public :
		// == Enumerations.
		// Control ID's.
		enum LSN_INPUT_WINDOW_IDS {
			LSN_IWI_NONE,
			LSN_IWI_MAINWINDOW,
			LSN_IWI_REBAR0,
			LSN_IWI_TOOLBAR0,
			LSN_IWI_STATUSBAR,

		};

		// Menu items
		enum LSN_INPUT_WINDOW_MENU_IDS {
			LSN_IWMI_MENU_BAR					= 300,
			LSN_IWMI_MENU_FILE,
			LSN_IWMI_MENU_SEARCH,
			LSN_IWMI_MENU_TOOLS,
			LSN_IWMI_MENU_WINDOW,
			LSN_IWMI_MENU_HELP,

			LSN_IWMI_FILE						= 100,
			LSN_IWMI_SEARCH,
			LSN_IWMI_TOOLS,
			LSN_IWMI_WINDOW,
			LSN_IWMI_HELP,

			LSN_IWMI_OPENROM,
			LSN_IWMI_OPENRECENT,
			LSN_IWMI_OPENFORDEBUG,
			LSN_IWMI_ADDENTRY,
			LSN_IWMI_OPENSAVEFILE,
			LSN_IWMI_SAVE,
			LSN_IWMI_SAVEAS,
			LSN_IWMI_DELETE,
			LSN_IWMI_DELETEALL,
			LSN_IWMI_LOCK,
			LSN_IWMI_UNLOCK,
			LSN_IWMI_EDIT,

			LSN_IWMI_DATATYPE,
			LSN_IWMI_POINTER,
			LSN_IWMI_STRING,
			LSN_IWMI_GROUP,
			LSN_IWMI_EXPRESSION,
			LSN_IWMI_SUB,
			LSN_IWMI_INSERT,
			LSN_IWMI_SEARCHOPTIONS,

			LSN_IWMI_DATATYPESEARCH,

			LSN_IWMI_OPTIONS,
			LSN_IWMI_PEWORKS,
			LSN_IWMI_STRINGTHEORY,
			LSN_IWMI_FLOATINGPOINTSTUDIO,

			LSN_IWMI_SHOWFOUNDADDR,
			LSN_IWMI_SHOW_EXPEVAL,
			LSN_IWMI_SHOW_CONVERTER,
			LSN_IWMI_SHOW_ALL,
		};


		// == Functions.
		// Creates the main window.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc., and registers the main window class.
		static CWidget *						CreateInputWindow( std::atomic_bool * _pabIsAlive );

		// Creates the main menu and adds it to the window.
		static BOOL								CreateMenu( CWidget * _pwInputWindow );

		// Gets the main window widget.
		static CWidget *						InputWindow() { return m_pwInputWindow; }


	protected :
		// == Members.
		// The layout for the main window.
		static LSW_WIDGET_LAYOUT				m_wlInputWindow[];

		// Menu bar items for the main window.
		static LSW_MENU_ITEM					m_miMenuBar[];

		// File menu.
		static LSW_MENU_ITEM					m_miFileMenu[];

		// Options menu.
		static LSW_MENU_ITEM					m_miOptionsMenu[];

		// Windows menu.
		static LSW_MENU_ITEM					m_miWindowsMenu[];

		// Menus.
		static LSW_MENU_LAYOUT					m_miMenus[];

		// The class for the main window.
		static ATOM								m_aInputClass;

		// The main window.
		static CWidget *						m_pwInputWindow;

	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

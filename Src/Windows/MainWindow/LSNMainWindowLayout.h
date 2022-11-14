#ifdef LSN_USE_WINDOWS

#pragma once

#include <Layout/LSWMenuLayout.h>
#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace lsn {

	class CMemHack;
	
	// Layout for the main MHS window.
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
			LSN_MWMI_MENU_SEARCH,
			LSN_MWMI_MENU_TOOLS,
			LSN_MWMI_MENU_WINDOW,
			LSN_MWMI_MENU_HELP,

			LSN_MWMI_FILE						= 100,
			LSN_MWMI_SEARCH,
			LSN_MWMI_TOOLS,
			LSN_MWMI_WINDOW,
			LSN_MWMI_HELP,

			LSN_MWMI_OPENPROCESS,
			LSN_MWMI_OPENRECENT,
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

			LSN_MWMI_DATATYPE,
			LSN_MWMI_POINTER,
			LSN_MWMI_STRING,
			LSN_MWMI_GROUP,
			LSN_MWMI_EXPRESSION,
			LSN_MWMI_SUB,
			LSN_MWMI_INSERT,
			LSN_MWMI_SEARCHOPTIONS,

			LSN_MWMI_DATATYPESEARCH,

			LSN_MWMI_OPTIONS,
			LSN_MWMI_PEWORKS,
			LSN_MWMI_STRINGTHEORY,
			LSN_MWMI_FLOATINGPOINTSTUDIO,

			LSN_MWMI_SHOWFOUNDADDR,
			LSN_MWMI_SHOW_EXPEVAL,
			LSN_MWMI_SHOW_CONVERTER,
			LSN_MWMI_SHOW_ALL,
		};


		// == Functions.
		// Creates the main window.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc., and registers the main window class.
		static CWidget *						CreateMainWindow();

		// Creates the main menu and adds it to the window.
		static BOOL								CreateMenu( CWidget * _pwMainWindow );

		// Gets the main window widget.
		static CWidget *						MainWindow() { return m_pwMainWindow; }


	protected :
		// == Members.
		// The layout for the main window.
		static LSW_WIDGET_LAYOUT				m_wlMainWindow[];

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
		static ATOM								m_aMainClass;

		// The main window.
		static CWidget *						m_pwMainWindow;

	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

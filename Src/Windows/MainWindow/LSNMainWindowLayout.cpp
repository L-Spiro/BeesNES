#ifdef LSN_USE_WINDOWS

#include "LSNMainWindowLayout.h"
#include "../Layout/LSNLayoutMacros.h"
#include "../Layout/LSNLayoutManager.h"
#include "../../Utilities/LSNUtilities.h"
#include <Base/LSWWndClassEx.h>


namespace lsn {

	// == Members.
	LSW_WIDGET_LAYOUT CMainWindowLayout::m_wlMainWindow[] = {
		{
			LSN_MAIN_WINDOW,						// ltType
			LSN_MWI_MAINWINDOW,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			64,										// iLeft
			64,										// iTop
			LSN_MAIN_WINDOW_W,						// dwWidth
			LSN_MAIN_WINDOW_H,						// dwHeight
			WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,							// dwStyle
			0,										// dwStyleEx
			L"10 NES",								// pwcText
			0,										// sTextLen
			LSN_MWI_NONE,							// dwParentId
		},
		{
			LSW_LT_STATUSBAR,						// ltType
			LSN_MWI_STATUSBAR,						// wId
			STATUSCLASSNAMEW,						// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			0,										// dwWidth
			0,										// dwHeight
			WS_VISIBLE | WS_CHILD | SBARS_SIZEGRIP,	// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			LSN_MWI_MAINWINDOW,						// dwParentId

			nullptr, 0,								// pcLeftSizeExp
			nullptr, 0,								// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
		},
		{
			LSW_LT_REBAR,							// ltType
			LSN_MWI_REBAR0,							// wId
			REBARCLASSNAMEW,						// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			m_wlMainWindow[0].dwWidth,				// dwWidth
			150,									// dwHeight
			//WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | TBSTYLE_TRANSPARENT | TBSTYLE_REGISTERDROP | TBSTYLE_LIST | TBSTYLE_FLAT | CCS_NODIVIDER | CCS_NOPARENTALIGN | CCS_NORESIZE | CCS_NOMOVEY,
			WS_VISIBLE | WS_BORDER | WS_CHILD | WS_CLIPCHILDREN |
				WS_CLIPSIBLINGS | CCS_NODIVIDER | CCS_NOPARENTALIGN |
				RBS_VARHEIGHT | RBS_BANDBORDERS,	// dwStyle
			0,//WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_TOOLWINDOW,// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			LSN_MWI_MAINWINDOW,						// dwParentId

			LSN_PARENT_CLEFT,//LSN_LOCK_LEFT,			// pcLeftSizeExp
			LSN_PARENT_CRIGHT,						// pcRightSizeExp
			LSN_PARENT_CTOP,							// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			LSN_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_TOOLBAR,							// ltType
			LSN_MWI_TOOLBAR0,						// wId
			TOOLBARCLASSNAMEW,						// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			0,										// dwWidth
			0,										// dwHeight
			WS_CHILD | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | WS_CLIPCHILDREN |
				WS_CLIPSIBLINGS | CCS_NODIVIDER | CCS_NORESIZE | WS_VISIBLE | CCS_ADJUSTABLE,		// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			LSN_MWI_MAINWINDOW,						// dwParentId
		},
	};

	// Menu bar items for the main window.
	LSW_MENU_ITEM CMainWindowLayout::m_miMenuBar[] = {
		//bIsSeperator	dwId						bCheckable	bChecked	bEnabled	
		{ FALSE,		LSN_MWMI_FILE,				FALSE,		FALSE,		TRUE,		L"&File" },
		{ FALSE,		LSN_MWMI_TOOLS,				FALSE,		FALSE,		TRUE,		L"&Tools" },
		{ FALSE,		LSN_MWMI_WINDOW,			FALSE,		FALSE,		TRUE,		L"&Window" },
		{ FALSE,		LSN_MWMI_HELP,				FALSE,		FALSE,		TRUE,		L"&Help" },
	};

	// File menu.
	LSW_MENU_ITEM CMainWindowLayout::m_miFileMenu[] {
		//bIsSeperator	dwId						bCheckable	bChecked	bEnabled	
		{ FALSE,		LSN_MWMI_OPENPROCESS,		FALSE,		FALSE,		TRUE,		L"Open &ROM" },
		{ FALSE,		LSN_MWMI_OPENRECENT,		FALSE,		FALSE,		TRUE,		L"Open Re&cent" },
		{ TRUE,			0,							FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		LSN_MWMI_OPENSAVEFILE,		FALSE,		FALSE,		TRUE,		L"Dummy &0" },
		{ FALSE,		LSN_MWMI_SAVE,				FALSE,		FALSE,		TRUE,		L"Dummy &1" },
		{ FALSE,		LSN_MWMI_SAVEAS,			FALSE,		FALSE,		TRUE,		L"Dummy &2" },
	};

	// Options menu.
	LSW_MENU_ITEM CMainWindowLayout::m_miOptionsMenu[] = {
		//bIsSeperator	dwId						bCheckable	bChecked	bEnabled	
		{ FALSE,		LSN_MWMI_OPTIONS,			FALSE,		FALSE,		TRUE,		L"Dummy &0" },
		{ FALSE,		LSN_MWMI_PEWORKS,			FALSE,		FALSE,		TRUE,		L"Dummy &1" },
		{ FALSE,		LSN_MWMI_STRINGTHEORY,		FALSE,		FALSE,		TRUE,		L"Dummy &2" },
		{ FALSE,		LSN_MWMI_FLOATINGPOINTSTUDIO,FALSE,		FALSE,		TRUE,		L"Dummy &3" },
	};

	// Windows menu.
	LSW_MENU_ITEM CMainWindowLayout::m_miWindowsMenu[] = {
		//bIsSeperator	dwId						bCheckable	bChecked	bEnabled	
		{ FALSE,		LSN_MWMI_SHOW_EXPEVAL,		TRUE,		FALSE,		TRUE,		L"Dummy &0" },
		{ FALSE,		LSN_MWMI_SHOWFOUNDADDR,		TRUE,		FALSE,		TRUE,		L"Dummy &1" },
		{ FALSE,		LSN_MWMI_SHOW_CONVERTER,	TRUE,		FALSE,		TRUE,		L"Dummy &2" },
		{ TRUE,			0,							FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		LSN_MWMI_SHOW_ALL,			TRUE,		FALSE,		TRUE,		L"Dummy &3" },
	};

	// Menus.
	LSW_MENU_LAYOUT CMainWindowLayout::m_miMenus[] = {
		{
			LSN_MWMI_MENU_BAR,
			0,
			0,
			LSN_ELEMENTS( m_miMenuBar ),
			m_miMenuBar
		},
		{
			LSN_MWMI_MENU_FILE,
			LSN_MWMI_MENU_BAR,
			LSN_MWMI_FILE,
			LSN_ELEMENTS( m_miFileMenu ),
			m_miFileMenu
		},
		{
			LSN_MWMI_MENU_TOOLS,
			LSN_MWMI_MENU_BAR,
			LSN_MWMI_TOOLS,
			LSN_ELEMENTS( m_miOptionsMenu ),
			m_miOptionsMenu
		},
		{
			LSN_MWMI_MENU_WINDOW,
			LSN_MWMI_MENU_BAR,
			LSN_MWMI_WINDOW,
			LSN_ELEMENTS( m_miWindowsMenu ),
			m_miWindowsMenu
		},
	};

	// The class for the main window.
	ATOM CMainWindowLayout::m_aMainClass = 0;

	// The main window.
	CWidget * CMainWindowLayout::m_pwMainWindow = nullptr;

	// == Functions.
	// Creates the main window.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc., and registers the main window class.
	CWidget * CMainWindowLayout::CreateMainWindow() {
		if ( !m_aMainClass ) {
			// Register the window classes we need.
			lsw::CWndClassEx wceEx( lsw::CWidget::WindowProc, L"LSNMAIN" );
			wceEx.SetBackgroundBrush( reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1) );
			m_aMainClass = lsw::CBase::RegisterClassExW( wceEx.Obj() );	
		}

		//std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		std::vector<LSW_MENU_LAYOUT> vMenus;
		std::vector<std::vector<LSW_MENU_ITEM> *> vMenuItems;

		LSW_WIDGET_LAYOUT * _pwMain = CHelpers::FindLayout( m_wlMainWindow, LSN_ELEMENTS( m_wlMainWindow ), LSN_MWI_MAINWINDOW );

		_pwMain->lpwcClass = reinterpret_cast<LPCWSTR>(m_aMainClass);
		m_pwMainWindow = lsw::CBase::LayoutManager()->CreateWindowX( m_wlMainWindow, LSN_ELEMENTS( m_wlMainWindow ),
			LSN_ELEMENTS( m_miMenus ) ? m_miMenus : nullptr, LSN_ELEMENTS( m_miMenus ),
			nullptr,
			0 );
		_pwMain->lpwcClass = nullptr;
		return m_pwMainWindow;
	}

	// Creates the main menu and adds it to the window.
	BOOL CMainWindowLayout::CreateMenu( CWidget * _pwMainWindow ) {
		if ( !_pwMainWindow ) { return FALSE; }
		HMENU hMenu = lsw::CBase::LayoutManager()->CreateMenu( m_miMenus, LSN_ELEMENTS( m_miMenus ) );
		if ( !hMenu ) { return FALSE; }
		return ::SetMenu( _pwMainWindow->Wnd(), hMenu );
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS
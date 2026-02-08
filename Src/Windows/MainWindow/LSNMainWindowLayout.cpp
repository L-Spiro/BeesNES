#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The main window layout.
 */

#include "LSNMainWindowLayout.h"
#include "../Layout/LSNLayoutMacros.h"
#include "../Layout/LSNLayoutManager.h"
#include "../../Localization/LSNLocalization.h"
#include "../../Utilities/LSNUtilities.h"
#include <Base/LSWWndClassEx.h>


namespace lsn {

	// == Members.
	/** The layout for the main window. */
	LSW_WIDGET_LAYOUT CMainWindowLayout::m_wlMainWindow[] = {
		{
			LSN_LT_MAIN_WINDOW,						// ltType
			LSN_MWI_MAINWINDOW,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			64,										// iLeft
			64,										// iTop
			LSN_MAIN_WINDOW_W,						// dwWidth
			LSN_MAIN_WINDOW_H,						// dwHeight
			WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,							// dwStyle
			0,										// dwStyleEx
			L"BeesNES",								// pwcText
			0,										// sTextLen
			LSN_MWI_NONE,							// dwParentId
		},
		//{
		//	LSW_LT_STATUSBAR,						// ltType
		//	LSN_MWI_STATUSBAR,						// wId
		//	STATUSCLASSNAMEW,						// lpwcClass
		//	TRUE,									// bEnabled
		//	FALSE,									// bActive
		//	0,										// iLeft
		//	0,										// iTop
		//	0,										// dwWidth
		//	0,										// dwHeight
		//	WS_VISIBLE | WS_CHILD | SBARS_SIZEGRIP,	// dwStyle
		//	0,										// dwStyleEx
		//	nullptr,								// pwcText
		//	0,										// sTextLen
		//	LSN_MWI_MAINWINDOW,						// dwParentId

		//	nullptr, 0,								// pcLeftSizeExp
		//	nullptr, 0,								// pcRightSizeExp
		//	nullptr, 0,								// pcTopSizeExp
		//	nullptr, 0,								// pcBottomSizeExp
		//	nullptr, 0,								// pcWidthSizeExp
		//	nullptr, 0,								// pcHeightSizeExp
		//},
	};

	/** Menu bar items for the main window. */
	LSW_MENU_ITEM CMainWindowLayout::m_miMenuBar[] = {
		//bIsSeperator	dwId						bCheckable	bChecked	bEnabled	
		{ FALSE,		LSN_MWMI_FILE,				FALSE,		FALSE,		TRUE,		LSN_LSTR( LSN__FILE ) },
		{ FALSE,		LSN_MWMI_GAME,				FALSE,		FALSE,		TRUE,		LSN_LSTR( LSN__GAME ) },
		{ FALSE,		LSN_MWMI_OPTIONS,			FALSE,		FALSE,		TRUE,		LSN_LSTR( LSN__OPTIONS ) },
		{ FALSE,		LSN_MWMI_TOOLS,				FALSE,		FALSE,		TRUE,		LSN_LSTR( LSN__TOOLS ) },
		//{ FALSE,		LSN_MWMI_WINDOW,			FALSE,		FALSE,		TRUE,		LSN_LSTR( LSN__WINDOW ) },
		{ FALSE,		LSN_MWMI_HELP,				FALSE,		FALSE,		TRUE,		LSN_LSTR( LSN__HELP ) },
	};

	/** File menu. */
	LSW_MENU_ITEM CMainWindowLayout::m_miFileMenu[] {
		//bIsSeperator	dwId						bCheckable	bChecked	bEnabled	
		{ FALSE,		LSN_MWMI_OPENROM,			FALSE,		FALSE,		TRUE,		LSN_LSTR( LSN_OPEN__ROM ) },
		{ FALSE,		LSN_MWMI_OPENRECENT,		FALSE,		FALSE,		TRUE,		LSN_LSTR( LSN_OPEN_REC_ENT ) },/*
		{ TRUE,			0,							FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		LSN_MWMI_OPENSAVEFILE,		FALSE,		FALSE,		TRUE,		L"Dummy &0" },
		{ FALSE,		LSN_MWMI_SAVE,				FALSE,		FALSE,		TRUE,		L"Dummy &1" },
		{ FALSE,		LSN_MWMI_SAVEAS,			FALSE,		FALSE,		TRUE,		L"Dummy &2" },*/
	};

	/** Game menu. */
	LSW_MENU_ITEM CMainWindowLayout::m_miGameMenu[] = {
		//bIsSeperator	dwId						bCheckable	bChecked	bEnabled	
		{ FALSE,		LSN_MWMI_PAUSE,				FALSE,		FALSE,		TRUE,		LSN_LSTR( LSN_GAME_PAUSE ) },
		{ TRUE,			0,										FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		LSN_MWMI_RESET,				FALSE,		FALSE,		TRUE,		LSN_LSTR( LSN_GAME_RESET ) },
		{ FALSE,		LSN_MWMI_POWER_CYCLE,		FALSE,		FALSE,		TRUE,		LSN_LSTR( LSN_GAME_POWER_CYCLE ) },
		{ FALSE,		LSN_MWMI_RELOAD_ROM,		FALSE,		FALSE,		TRUE,		LSN_LSTR( LSN_GAME_RELOAD_ROM ) },
		{ TRUE,			0,										FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		LSN_MWMI_POWER_OFF,			FALSE,		FALSE,		TRUE,		LSN_LSTR( LSN_GAME_POWER_OFF ) },
	};

	/** Options menu. */
	LSW_MENU_ITEM CMainWindowLayout::m_miOptionsMenu[] = {
		//bIsSeperator	dwId						bCheckable	bChecked	bEnabled
		{ FALSE,		LSN_MWMI_VIDEO_SIZE,		FALSE,		FALSE,		TRUE,		LSN_LSTR( LSN_VIDEO_SIZE ) },
		{ FALSE,		LSN_MWMI_VIDEO_FILTER,		FALSE,		FALSE,		TRUE,		LSN_LSTR( LSN_VIDEO_FILTER ) },
		{ FALSE,		LSN_MWMI_REGION,			FALSE,		FALSE,		TRUE,		LSN_LSTR( LSN_REGION ) },
		{ TRUE,			0,							FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		LSN_MWMI_INPUT,				FALSE,		FALSE,		TRUE,		LSN_LSTR( LSN_INPUT ) },
		{ FALSE,		LSN_MWMI_VIDEO,				FALSE,		FALSE,		TRUE,		LSN_LSTR( LSN_VIDEO ) },
		{ FALSE,		LSN_MWMI_AUDIO,				FALSE,		FALSE,		TRUE,		LSN_LSTR( LSN_AUDIO ) },
	};

	/** Tools menu. */
	LSW_MENU_ITEM CMainWindowLayout::m_miToolsMenu[] = {
		//bIsSeperator	dwId						bCheckable	bChecked	bEnabled	
		{ FALSE,		LSN_MWMI_TOOLS_PATCH,		FALSE,		FALSE,		TRUE,		LSN_LSTR( LSN_TOOLS_PATCH ) },
		{ FALSE,		LSN_MWMI_TOOLS_WAV_EDIT,	FALSE,		FALSE,		TRUE,		LSN_LSTR( LSN_TOOLS_WAV_EDIT ) },
	};

	/** Windows menu. */
	//LSW_MENU_ITEM CMainWindowLayout::m_miWindowsMenu[] = {
	//	//bIsSeperator	dwId						bCheckable	bChecked	bEnabled	
	//	{ FALSE,		LSN_MWMI_DELETE,			FALSE,		FALSE,		TRUE,		L"Dummy &0" },
	//	{ FALSE,		LSN_MWMI_SHOWFOUNDADDR,		TRUE,		FALSE,		TRUE,		L"Dummy &1" },
	//	{ FALSE,		LSN_MWMI_SHOW_CONVERTER,	TRUE,		FALSE,		TRUE,		L"Dummy &2" },
	//	{ TRUE,			0,							FALSE,		FALSE,		TRUE,		nullptr },
	//	{ FALSE,		LSN_MWMI_SHOW_ALL,			TRUE,		FALSE,		TRUE,		L"Dummy &3" },
	//};

	/** Video-size sub-menu. */
	LSW_MENU_ITEM CMainWindowLayout::m_miVideoSizeMenu[] = {
		//bIsSeperator	dwId						bCheckable	bChecked	bEnabled	
		{ FALSE,		LSN_MWMI_VIDEO_SIZE_1X,		TRUE,		FALSE,		TRUE,		LSN_LSTR( LSN_VIDEO_SIZE_1X ) },
		{ FALSE,		LSN_MWMI_VIDEO_SIZE_2X,		TRUE,		FALSE,		TRUE,		LSN_LSTR( LSN_VIDEO_SIZE_2X ) },
		{ FALSE,		LSN_MWMI_VIDEO_SIZE_3X,		TRUE,		FALSE,		TRUE,		LSN_LSTR( LSN_VIDEO_SIZE_3X ) },
		{ FALSE,		LSN_MWMI_VIDEO_SIZE_4X,		TRUE,		FALSE,		TRUE,		LSN_LSTR( LSN_VIDEO_SIZE_4X ) },
		{ FALSE,		LSN_MWMI_VIDEO_SIZE_5X,		TRUE,		FALSE,		TRUE,		LSN_LSTR( LSN_VIDEO_SIZE_5X ) },
		{ FALSE,		LSN_MWMI_VIDEO_SIZE_6X,		TRUE,		FALSE,		TRUE,		LSN_LSTR( LSN_VIDEO_SIZE_6X ) },
	};

	/** Video-filter sub-menu. */
	LSW_MENU_ITEM CMainWindowLayout::m_miVideoFilterMenu[] = {
		//bIsSeperator	dwId												bCheckable	bChecked	bEnabled	
		{ FALSE,		LSN_MWMI_VIDEO_FILTER_NONE,							TRUE,		FALSE,		TRUE,		LSN_LSTR( LSN_VIDEO_FILTER_NONE ) },
#ifdef LSN_DX9
		{ FALSE,		LSN_MWMI_VIDEO_FILTER_DX9_PALETTE,					TRUE,		FALSE,		TRUE,		LSN_LSTR( LSN_VIDEO_FILTER_DX9_PALETTE ) },
#endif	// #ifdef LSN_DX9
		{ FALSE,		LSN_MWMI_VIDEO_FILTER_NTSC_BLARGG,					TRUE,		FALSE,		TRUE,		LSN_LSTR( LSN_VIDEO_FILTER_NTSC_BLARGG ) },
		{ FALSE,		LSN_MWMI_VIDEO_FILTER_NTSC_LSPIRO,					TRUE,		FALSE,		TRUE,		LSN_LSTR( LSN_VIDEO_FILTER_NTSC_L_SPIRO ) },
#ifdef LSN_DX9
		{ FALSE,		LSN_MWMI_VIDEO_FILTER_DX9_NTSC_LSPIRO_UPSCALED,		TRUE,		FALSE,		TRUE,		LSN_LSTR( LSN_VIDEO_FILTER_LSPIRO_NTSC_DX9_UPSCALE ) },
#endif	// #ifdef LSN_DX9
		{ FALSE,		LSN_MWMI_VIDEO_FILTER_PAL_LSPIRO,					TRUE,		FALSE,		TRUE,		LSN_LSTR( LSN_VIDEO_FILTER_PAL_L_SPIRO ) },
		{ FALSE,		LSN_MWMI_VIDEO_FILTER_DENDY_LSPIRO,					TRUE,		FALSE,		TRUE,		LSN_LSTR( LSN_VIDEO_FILTER_DENDY_L_SPIRO ) },
		{ FALSE,		LSN_MWMI_VIDEO_FILTER_PALM_LSPIRO,					TRUE,		FALSE,		TRUE,		LSN_LSTR( LSN_VIDEO_FILTER_PALM_L_SPIRO ) },
		{ FALSE,		LSN_MWMI_VIDEO_FILTER_PALN_LSPIRO,					TRUE,		FALSE,		TRUE,		LSN_LSTR( LSN_VIDEO_FILTER_PALN_L_SPIRO ) },
		{ FALSE,		LSN_MWMI_VIDEO_FILTER_NTSC_CRT_FULL,				TRUE,		FALSE,		TRUE,		LSN_LSTR( LSN_VIDEO_FILTER_NTSC_CRT_FULL ) },
		{ FALSE,		LSN_MWMI_VIDEO_FILTER_PAL_CRT_FULL,					TRUE,		FALSE,		TRUE,		LSN_LSTR( LSN_VIDEO_FILTER_PAL_CRT_FULL ) },
		{ TRUE,			0,													FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		LSN_MWMI_VIDEO_FILTER_AUTO_CRT_FULL,				TRUE,		FALSE,		TRUE,		LSN_LSTR( LSN_VIDEO_FILTER_AUTO_CRT_FULL ) },
		{ FALSE,		LSN_MWMI_VIDEO_FILTER_AUTO_LSPIRO,					TRUE,		FALSE,		TRUE,		LSN_LSTR( LSN_VIDEO_FILTER_AUTO_LSPIRO ) },
	};

	/** Region sub-menu. */
	LSW_MENU_ITEM CMainWindowLayout::m_miRegionMenu[] = {
		//bIsSeperator	dwId									bCheckable	bChecked	bEnabled	
		{ FALSE,		LSN_MWMI_REGION_AUTO,					TRUE,		FALSE,		TRUE,		LSN_LSTR( LSN_REGION_AUTO ) },
		{ TRUE,			0,										FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		LSN_MWMI_REGION_NTSC,					TRUE,		FALSE,		TRUE,		LSN_LSTR( LSN_REGION_NTSC ) },
		{ FALSE,		LSN_MWMI_REGION_PAL,					TRUE,		FALSE,		TRUE,		LSN_LSTR( LSN_REGION_PAL ) },
		{ FALSE,		LSN_MWMI_REGION_DENDY,					TRUE,		FALSE,		TRUE,		LSN_LSTR( LSN_REGION_DENDY ) },
		{ FALSE,		LSN_MWMI_REGION_PALM,					TRUE,		FALSE,		TRUE,		LSN_LSTR( LSN_REGION_PALM ) },
		{ FALSE,		LSN_MWMI_REGION_PALN,					TRUE,		FALSE,		TRUE,		LSN_LSTR( LSN_REGION_PALN ) },
	};

	/** Menus. */
	LSW_MENU_LAYOUT CMainWindowLayout::m_miMenus[] = {
		{
			LSN_MWMI_MENU_BAR,
			0,
			0,
			std::size( m_miMenuBar ),
			m_miMenuBar
		},
		{
			LSN_MWMI_MENU_FILE,
			LSN_MWMI_MENU_BAR,
			LSN_MWMI_FILE,
			std::size( m_miFileMenu ),
			m_miFileMenu
		},
		{
			LSN_MWMI_MENU_GAME,
			LSN_MWMI_MENU_BAR,
			LSN_MWMI_GAME,
			std::size( m_miGameMenu ),
			m_miGameMenu
		},
		{
			LSN_MWMI_MENU_OPTIONS,
			LSN_MWMI_MENU_BAR,
			LSN_MWMI_OPTIONS,
			std::size( m_miOptionsMenu ),
			m_miOptionsMenu
		},
		{
			LSN_MWMI_MENU_TOOLS,
			LSN_MWMI_MENU_BAR,
			LSN_MWMI_TOOLS,
			std::size( m_miToolsMenu ),
			m_miToolsMenu
		},
		/*{
			LSN_MWMI_MENU_WINDOW,
			LSN_MWMI_MENU_BAR,
			LSN_MWMI_WINDOW,
			std::size( m_miWindowsMenu ),
			m_miWindowsMenu
		},*/

		// Video Sizes.
		{
			LSN_MWMI_VIDEO_SIZES,
			LSN_MWMI_MENU_OPTIONS,
			LSN_MWMI_VIDEO_SIZE,
			std::size( m_miVideoSizeMenu ),
			m_miVideoSizeMenu
		},
		// Video Filters.
		{
			LSN_MWMI_VIDEO_FILTERS,
			LSN_MWMI_MENU_OPTIONS,
			LSN_MWMI_VIDEO_FILTER,
			std::size( m_miVideoFilterMenu ),
			m_miVideoFilterMenu
		},
		// Regions.
		{
			LSN_MWMI_REGIONS,
			LSN_MWMI_MENU_OPTIONS,
			LSN_MWMI_REGION,
			std::size( m_miRegionMenu ),
			m_miRegionMenu
		},
	};

	/** The class for the main window. */
	ATOM CMainWindowLayout::m_aMainClass = 0;

	/** The main window. */
	CWidget * CMainWindowLayout::m_pwMainWindow = nullptr;

	// == Functions.
	/**
	 * Creates the main window.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc., and registers the main window class.
	 *
	 * \param _pabIsAlive An atomic that tells the main loop that the window has closed and its pointer is no longer valid.
	 * \return Returns the main window widget.
	 */
	CWidget * CMainWindowLayout::CreateMainWindow( std::atomic_bool * _pabIsAlive ) {
		if ( !m_aMainClass ) {
			// Register the window classes we need.
			lsw::CWndClassEx wceEx( lsw::CWidget::WindowProc, L"LSNMAIN" );
			//wceEx.SetBackgroundBrush( reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1) );
			wceEx.SetBackgroundBrush( NULL );
			m_aMainClass = lsw::CBase::RegisterClassExW( wceEx.Obj() );	
		}

		//std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		std::vector<LSW_MENU_LAYOUT> vMenus;
		std::vector<std::vector<LSW_MENU_ITEM> *> vMenuItems;

		LSW_WIDGET_LAYOUT * _pwMain = CHelpers::FindLayout( m_wlMainWindow, std::size( m_wlMainWindow ), LSN_MWI_MAINWINDOW );

		_pwMain->lpwcClass = reinterpret_cast<LPCWSTR>(m_aMainClass);
		m_pwMainWindow = lsw::CBase::LayoutManager()->CreateWindowX( m_wlMainWindow, std::size( m_wlMainWindow ),
			std::size( m_miMenus ) ? m_miMenus : nullptr, std::size( m_miMenus ),
			nullptr,
			reinterpret_cast<uint64_t>(_pabIsAlive) );
		_pwMain->lpwcClass = nullptr;
		return m_pwMainWindow;
	}

	/**
	 * Creates the main menu and adds it to the window.
	 *
	 * \param _pwMainWindow The main window, used for parenting the menu.
	 * \return Returns TRUE if the menu was created.
	 */
	BOOL CMainWindowLayout::CreateMenu( CWidget * _pwMainWindow ) {
		if ( !_pwMainWindow ) { return FALSE; }
		std::vector<ACCEL> vHotkeys;
		HMENU hMenu = lsw::CBase::LayoutManager()->CreateMenu( m_miMenus, std::size( m_miMenus ), vHotkeys );
		if ( !hMenu ) { return FALSE; }
		if ( vHotkeys.size() ) {
			CBase::GetAccelHandler().CreateAndRegister( _pwMainWindow->Wnd(), vHotkeys.data(), int( vHotkeys.size() ) );
		}
		return ::SetMenu( _pwMainWindow->Wnd(), hMenu );
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS
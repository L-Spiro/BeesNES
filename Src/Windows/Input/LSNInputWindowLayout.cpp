#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The input window layout.
 */

#include "LSNInputWindowLayout.h"
#include "../../Localization/LSNLocalization.h"
#include "../Layout/LSNLayoutMacros.h"
#include "../Layout/LSNLayoutManager.h"



namespace lsn {

#define LSN_FIRST_SECTION_W							(80 * 60 / 100)
#define LSN_CONBO_W									(190 * 60 / 100)
#define LSN_THIRD_SECTION_W							(82 * 60 / 100)

#define LSN_INPUT_W									(LSN_LEFT_JUST + LSN_GROUP_LEFT + LSN_FIRST_SECTION_W + LSN_LEFT_JUST + LSN_CONBO_W + LSN_LEFT_JUST + LSN_THIRD_SECTION_W + LSN_GROUP_RIGHT + LSN_LEFT_JUST)

	// == Members.
	/** The layout for the main window. */
	LSW_WIDGET_LAYOUT CInputWindowLayout::m_wlInputWindow[] = {
		{
			LSN_LT_INPUT_DIALOG,					// ltType
			LSN_IWI_MAINWINDOW,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			LSN_INPUT_W,							// dwWidth
			150,							// dwHeight
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | DS_3DLOOK | DS_FIXEDSYS | DS_MODALFRAME | DS_CENTER,					// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,												// dwStyleEx
			LSN_LSTR( LSN_INPUT_DIALOG ),			// pwcText
			0,										// sTextLen
			LSN_IWI_NONE,							// dwParentId
		},

		{
			LSW_LT_TAB,								// ltType
			LSN_IWI_TAB,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			255,									// dwWidth
			110,									// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | TCS_HOTTRACK,				// dwStyle
			WS_EX_ACCEPTFILES,														// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			LSN_IWI_MAINWINDOW,						// dwParentId

			LSN_PARENT_VCLEFT,						// pcLeftSizeExp
			LSN_PARENT_VCRIGHT,						// pcRightSizeExp
			LSN_PARENT_VCTOP,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			LSN_FIXED_HEIGHT,						// pcHeightSizeExp
		},



		// Per-Game Page.

	};

	/** The layout for the global-setup panel. */
	LSW_WIDGET_LAYOUT CInputWindowLayout::m_wlGlobalPanel[] = {
		{
			LSW_LT_CHILDWINDOW,						// ltType
			LSN_IWI_GLOBAL( PANEL ),				// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			50, 50, 300, 300,
			WS_CHILDWINDOW | WS_VISIBLE | DS_3DLOOK | DS_FIXEDSYS | DS_SETFONT | DS_CONTROL,										// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_CONTROLPARENT,												// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			LSN_IWI_NONE,							// dwParentId

			LSN_PARENT_VCLEFT,						// pcLeftSizeExp
			LSN_PARENT_VCRIGHT,						// pcRightSizeExp
			LSN_PARENT_VCTOP,						// pcTopSizeExp
			LSN_PARENT_VCBOTTOM,					// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
		},
	};


#undef LSN_INPUT_W

#undef LSN_THIRD_SECTION_W
#undef LSN_CONBO_W
#undef LSN_FIRST_SECTION_W


	// == Functions.
	/**
	 * Creates the input-configuration window.
	 *
	 * \param _pwParent The parent of the dialog.
	 * \return Returns TRUE if the dialog was created successfully.
	 */
	BOOL CInputWindowLayout::CreateInputDialog( CWidget * _pwParent ) {
		lsn::CLayoutManager * plmLayout = static_cast<lsn::CLayoutManager *>(lsw::CBase::LayoutManager());
		INT_PTR ipProc = plmLayout->DialogBoxX( m_wlInputWindow, LSN_ELEMENTS( m_wlInputWindow ), _pwParent, /*reinterpret_cast<uint64_t>(0)*/0 );
		if ( ipProc != 0 ) {
			// Success.  Do stuff.
			return TRUE;
		}
		return FALSE;
	}

	/**
	 * Creates the global-settings page.
	 *
	 * \param _pwParent the parent of the page.
	 * \return Returns the created widget.
	 */
	CWidget * CInputWindowLayout::CreateGlobalPage( CWidget * _pwParent ) {
		return CreatePage( _pwParent, m_wlGlobalPanel, LSN_ELEMENTS( m_wlGlobalPanel ) );
	}

	/**
	 * Creates the tab pages.
	 *
	 * \param _pwParent The parent widget.
	 * \param _pwlLayout The page layout.
	 * \param _sTotal The number of items to which _pwlLayout points.
	 * \return Returns the created page.
	 */
	CWidget * CInputWindowLayout::CreatePage( CWidget * _pwParent, const LSW_WIDGET_LAYOUT * _pwlLayout, size_t _sTotal ) {
		lsn::CLayoutManager * plmLayout = static_cast<lsn::CLayoutManager *>(lsw::CBase::LayoutManager());
		CWidget * pwWidget = plmLayout->CreateDialogX( _pwlLayout, _sTotal, _pwParent, 0 );
		if ( pwWidget ) {
			// Success.  Do stuff.
		}
		return pwWidget;
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

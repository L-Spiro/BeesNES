#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A layout template for "pages" (self-contained borderless dialogs meant for embedding into other dialogs).
 */

#include "LSNCheatsWindowLayout.h"
#include "../../Localization/LSNLocalization.h"
#include "../Layout/LSNLayoutMacros.h"
#include "../Layout/LSNLayoutManager.h"



namespace lsn {

#define LSN_CHEAT_GROUP_W								(LSN_LEFT_JUST + LSN_LEFT_JUST + 200 + LSN_LEFT_JUST + LSN_LEFT_JUST)
#define LSN_CHEAT_GROUP_H								(50 + 150 + 20)
#define LSN_CHEAT_GROUP_T								LSN_GROUP_TOP

#define LSN_CHEAT_W										(LSN_LEFT_JUST + LSN_CHEAT_GROUP_W + LSN_LEFT_JUST)
#define LSN_CHEAT_H										(LSN_CHEAT_GROUP_H + LSN_GROUP_TOP + 2 + LSN_DEF_BUTTON_HEIGHT + 2)

	// == Members.
	/** The layout for the template window. */
	LSW_WIDGET_LAYOUT CCheatsWindowLayout::m_wlPage[] = {
		{
			LSN_LT_CHEATS,									// ltType
			CWinUtilities::LSN_CWI_CHEAT_DIALOG,			// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			0,												// iLeft
			0,												// iTop
			LSN_CHEAT_W,									// dwWidth
			LSN_CHEAT_H,									// dwHeight
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | DS_3DLOOK | DS_FIXEDSYS | DS_MODALFRAME | DS_CENTER,																							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,																														// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_CWI_NONE,					// dwParentId

			//LSN_PARENT_VCLEFT,								// pcLeftSizeExp
			//nullptr, 0,										// pcRightSizeExp
			//LSN_PARENT_VCTOP,								// pcTopSizeExp
			//nullptr, 0,										// pcBottomSizeExp
			//LSN_FIXED_WIDTH,								// pcWidthSizeExp
			//LSN_FIXED_HEIGHT,								// pcHeightSizeExp
		},
		{
			LSW_LT_TOOLBAR,									// ltType
			CWinUtilities::LSN_CWI_TOOLBAR,					// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			0,												// iLeft
			0,												// iTop
			0,												// dwWidth
			0,												// dwHeight
			WS_CHILD | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | WS_CLIPCHILDREN |
				WS_CLIPSIBLINGS | CCS_NODIVIDER | CCS_NORESIZE | WS_VISIBLE | CCS_ADJUSTABLE | TBSTYLE_LIST,																													// dwStyle
			0,																																																					// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_CWI_CHEAT_DIALOG,			// dwParentId
		},
		
		{
			LSW_LT_GROUPBOX,								// ltType
			CWinUtilities::LSN_AOWI_PAGE_GENERAL_GROUP,		// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST,									// iLeft
			LSN_CHEAT_GROUP_T,								// iTop
			LSN_CHEAT_GROUP_W,								// dwWidth
			LSN_CHEAT_GROUP_H,								// dwHeight
			LSN_GROUPSTYLE,									// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																																		// dwStyleEx
			LSN_LSTR( LSN_STR_AVAILABLE_CHEATS ),			// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_CWI_CHEAT_DIALOG,			// dwParentId
		},
	};

#undef LSN_CHEAT_H
#undef LSN_CHEAT_W


	// == Functions.
	/**
	 * Creates the Cheats dialog.
	 *
	 * \param _pwParent The parent of the page.
	 * \param _pcmCheatManager A pointer to the cheat manager.
	 * \return TRUE if the dialog was created successfully.
	 */
	BOOL CCheatsWindowLayout::CreateCheatsDialog( CWidget * _pwParent, CCheatManager * _pcmCheatManager ) {
		return CreatePage( _pwParent, m_wlPage, std::size( m_wlPage ), _pcmCheatManager );
	}

	/**
	 * Creates the tab pages.
	 *
	 * \param _pwParent The parent widget.
	 * \param _pwlLayout The page layout.
	 * \param _sTotal The number of items to which _pwlLayout points.
	 * \param _pcmCheatManager A pointer to the cheat manager.
	 * \return TRUE if the dialog was created successfully.
	 */
	BOOL CCheatsWindowLayout::CreatePage( CWidget * _pwParent, const LSW_WIDGET_LAYOUT * _pwlLayout, size_t _sTotal, CCheatManager * _pcmCheatManager ) {
		lsn::CLayoutManager * plmLayout = static_cast<lsn::CLayoutManager *>(lsw::CBase::LayoutManager());
		INT_PTR ipProc = plmLayout->DialogBoxX( _pwlLayout, _sTotal, _pwParent, reinterpret_cast<uint64_t>(_pcmCheatManager) );
		if ( ipProc != 0 ) {
			// Success.  Do stuff.
			return TRUE;
		}
		return FALSE;
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

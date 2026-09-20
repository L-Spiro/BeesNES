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

#define LSN_SECTION_W											250
#define LSN_CODE_EDIT_H											50
#define LSN_NOTES_EDIT_H										154
#define LSN_TREELISTVIEW_H										(LSN_NOTES_EDIT_H + LSN_CODE_EDIT_H + LSN_TOP_JUST)

#define LSN_CHEAT_GROUP_W										(LSN_GROUP_LEFT + LSN_SECTION_W + LSN_LEFT_JUST + LSN_SECTION_W + LSN_GROUP_LEFT)
#define LSN_CHEAT_GROUP_H										(LSN_GROUP_TOP + (LSN_CODE_EDIT_H + LSN_TOP_JUST + LSN_NOTES_EDIT_H) + LSN_TOP_JUST + LSN_GROUP_BOTTOM)
#define LSN_CHEAT_GROUP_T										(LSN_TOP_JUST)
#define LSN_CHEAT_GROUP_L										LSN_LEFT_JUST

#define LSN_CHEAT_W												(LSN_LEFT_JUST + LSN_CHEAT_GROUP_W + LSN_LEFT_JUST)
#define LSN_CHEAT_H												(LSN_CHEAT_GROUP_H + LSN_CHEAT_GROUP_T + 2 + LSN_DEF_BUTTON_HEIGHT + 2)

	// == Members.
	/** The layout for the Cheats window. */
	LSW_WIDGET_LAYOUT CCheatsWindowLayout::m_wlPage[] = {
		{
			LSN_LT_CHEATS,										// ltType
			CWinUtilities::LSN_CWI_CHEAT_DIALOG,				// wId
			nullptr,											// lpwcClass
			TRUE,												// bEnabled
			FALSE,												// bActive
			0,													// iLeft
			0,													// iTop
			LSN_CHEAT_W,										// dwWidth
			LSN_CHEAT_H,										// dwHeight
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | DS_3DLOOK | DS_FIXEDSYS | DS_MODALFRAME | DS_CENTER,		// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,									// dwStyleEx
			LSN_LSTR( LSN_STR_CHEATS ),							// pwcText
			0,													// sTextLen
			CWinUtilities::LSN_CWI_NONE,						// dwParentId
		},
		
		{
			LSW_LT_REBAR,										// ltType
			CWinUtilities::LSN_CWI_REBAR,						// wId
			nullptr,											// lpwcClass
			TRUE,												// bEnabled
			FALSE,												// bActive
			0,													// iLeft
			0,													// iTop
			LSN_CHEAT_W,										// dwWidth
			150,												// dwHeight
			WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN |
				WS_CLIPSIBLINGS | CCS_NODIVIDER | CCS_NOPARENTALIGN |
				RBS_VARHEIGHT | RBS_BANDBORDERS,				// dwStyle
			0,													// dwStyleEx
			nullptr,											// pwcText
			0,													// sTextLen
			CWinUtilities::LSN_CWI_CHEAT_DIALOG,				// dwParentId
		},
		{
			LSW_LT_TOOLBAR,										// ltType
			CWinUtilities::LSN_CWI_TOOLBAR,						// wId
			nullptr,											// lpwcClass
			TRUE,												// bEnabled
			FALSE,												// bActive
			0,													// iLeft
			0,													// iTop
			0,													// dwWidth
			0,													// dwHeight
			WS_CHILD | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | WS_CLIPCHILDREN |
				WS_CLIPSIBLINGS | CCS_NODIVIDER | CCS_NORESIZE | WS_VISIBLE | CCS_ADJUSTABLE | TBSTYLE_LIST,								// dwStyle
			0,													// dwStyleEx
			nullptr,											// pwcText
			0,													// sTextLen
			CWinUtilities::LSN_CWI_CHEAT_DIALOG,				// dwParentId
		},
		
		{
			LSW_LT_GROUPBOX,									// ltType
			CWinUtilities::LSN_CWI_GROUP,						// wId
			WC_BUTTONW,											// lpwcClass
			TRUE,												// bEnabled
			FALSE,												// bActive
			LSN_CHEAT_GROUP_L,									// iLeft
			LSN_CHEAT_GROUP_T,									// iTop
			LSN_CHEAT_GROUP_W,									// dwWidth
			LSN_CHEAT_GROUP_H,									// dwHeight
			LSN_GROUPSTYLE,										// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,													// dwStyleEx
			LSN_LSTR( LSN_STR_AVAILABLE_CHEATS ),				// pwcText
			0,													// sTextLen
			CWinUtilities::LSN_CWI_CHEAT_DIALOG,				// dwParentId
		},
		{
			LSW_LT_TREELISTVIEW,								// ltType
			CWinUtilities::LSN_CWI_CHEAT_TREELISTVIEW,			// wId
			nullptr,											// lpwcClass
			TRUE,												// bEnabled
			FALSE,												// bActive
			(LSN_CHEAT_GROUP_L + LSN_GROUP_LEFT),				// iLeft
			LSN_CHEAT_GROUP_T + LSN_GROUP_TOP,					// iTop
			LSN_SECTION_W,										// dwWidth
			LSN_TREELISTVIEW_H,									// dwHeight
			LVS_REPORT | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_OWNERDATA | WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP,						// dwStyle
			LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER,			// dwStyleEx
			nullptr,											// pwcText
			0,													// sTextLen
			CWinUtilities::LSN_CWI_CHEAT_DIALOG,				// dwParentId
		},
		{
			LSW_LT_EDIT,										// ltType
			CWinUtilities::LSN_CWI_CODE_EDIT,					// wId
			nullptr,											// lpwcClass
			TRUE,												// bEnabled
			FALSE,												// bActive
			(LSN_CHEAT_GROUP_L + LSN_GROUP_LEFT) + LSN_SECTION_W + LSN_LEFT_JUST,															// iLeft
			LSN_CHEAT_GROUP_T + LSN_GROUP_TOP,					// iTop
			LSN_SECTION_W,										// dwWidth
			LSN_CODE_EDIT_H,									// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY | WS_VSCROLL,					// dwStyle
			WS_EX_CLIENTEDGE,									// dwStyleEx
			nullptr,											// pwcText
			0,													// sTextLen
			CWinUtilities::LSN_CWI_CHEAT_DIALOG,				// dwParentId
		},
		{
			LSW_LT_EDIT,										// ltType
			CWinUtilities::LSN_CWI_NOTES_EDIT,					// wId
			nullptr,											// lpwcClass
			TRUE,												// bEnabled
			FALSE,												// bActive
			(LSN_CHEAT_GROUP_L + LSN_GROUP_LEFT) + LSN_SECTION_W + LSN_LEFT_JUST,															// iLeft
			LSN_CHEAT_GROUP_T + LSN_GROUP_TOP + LSN_CODE_EDIT_H + LSN_TOP_JUST,																// iTop
			LSN_SECTION_W,										// dwWidth
			LSN_NOTES_EDIT_H + LSN_TOP_JUST,					// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY | WS_VSCROLL,					// dwStyle
			WS_EX_CLIENTEDGE,									// dwStyleEx
			nullptr,											// pwcText
			0,													// sTextLen
			CWinUtilities::LSN_CWI_CHEAT_DIALOG,				// dwParentId
		},


		{
			LSW_LT_BUTTON,										// ltType
			CWinUtilities::LSN_CWI_OK,							// wId
			WC_BUTTONW,											// lpwcClass
			TRUE,												// bEnabled
			TRUE,												// bActive
			LSN_CHEAT_W - (LSN_DEF_BUTTON_WIDTH * 1 + LSN_LEFT_JUST) * 2,																	// iLeft
			LSN_CHEAT_H - LSN_DEF_BUTTON_HEIGHT - LSN_TOP_JUST,	// iTop
			LSN_DEF_BUTTON_WIDTH,								// dwWidth
			LSN_DEF_BUTTON_HEIGHT,								// dwHeight
			LSN_DEFBUTTONSTYLE,									// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,													// dwStyleEx
			LSN_LSTR( LSN_OK ),									// pwcText
			0,													// sTextLen
			CWinUtilities::LSN_CWI_CHEAT_DIALOG,				// dwParentId
		},
		{
			LSW_LT_BUTTON,										// ltType
			CWinUtilities::LSN_CWI_CANCEL,						// wId
			WC_BUTTONW,											// lpwcClass
			TRUE,												// bEnabled
			FALSE,												// bActive
			LSN_CHEAT_W - LSN_DEF_BUTTON_WIDTH * 1 - LSN_LEFT_JUST,																			// iLeft
			LSN_CHEAT_H - LSN_DEF_BUTTON_HEIGHT	- LSN_TOP_JUST,	// iTop
			LSN_DEF_BUTTON_WIDTH,								// dwWidth
			LSN_DEF_BUTTON_HEIGHT,								// dwHeight
			LSN_BUTTONSTYLE,									// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,													// dwStyleEx
			LSN_LSTR( LSN_CANCEL ),								// pwcText
			0,													// sTextLen
			CWinUtilities::LSN_CWI_CHEAT_DIALOG,				// dwParentId
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

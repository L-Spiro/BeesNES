#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The layout for the About dialog.
 */

#include "LSNAboutWindowLayout.h"
#include "../../Localization/LSNLocalization.h"
#include "../Layout/LSNLayoutMacros.h"
#include "../Layout/LSNLayoutManager.h"

namespace lsn {

#define LSN_ABOUT_GROUP_W										300
#define LSN_ABOUT_INFO_GROUP_H									(LSN_GROUP_TOP + (LSN_DEF_STATIC_HEIGHT + LSN_TOP_JUST) * 4 + LSN_GROUP_BOTTOM)
#define LSN_ABOUT_CREDITS_GROUP_T								(LSN_TOP_JUST + LSN_ABOUT_INFO_GROUP_H + LSN_TOP_JUST)
#define LSN_ABOUT_CREDITS_EDIT_H								110
#define LSN_ABOUT_CREDITS_GROUP_H								(LSN_GROUP_TOP + LSN_DEF_STATIC_HEIGHT + LSN_TOP_JUST + LSN_ABOUT_CREDITS_EDIT_H + LSN_GROUP_BOTTOM)
#define LSN_ABOUT_W												(LSN_LEFT_JUST + LSN_ABOUT_GROUP_W + LSN_LEFT_JUST)
#define LSN_ABOUT_H												(LSN_ABOUT_CREDITS_GROUP_T + LSN_ABOUT_CREDITS_GROUP_H + LSN_TOP_JUST + LSN_DEF_BUTTON_HEIGHT + LSN_TOP_JUST)

	// == Members.
	/** The layout for the About dialog. */
	LSW_WIDGET_LAYOUT CAboutWindowLayout::m_wlDialog[] = {
		{
			LSN_LT_ABOUT_DIALOG,							// ltType
			CWinUtilities::LSN_ABWI_DIALOG,					// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			0,												// iLeft
			0,												// iTop
			LSN_ABOUT_W,									// dwWidth
			LSN_ABOUT_H,									// dwHeight
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | DS_3DLOOK | DS_FIXEDSYS | DS_MODALFRAME | DS_CENTER,	// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,	// dwStyleEx
			LSN_LSTR( LSN_STR_ABOUT_BEESNES ),				// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_ABWI_NONE,					// dwParentId
		},
		{
			LSW_LT_GROUPBOX,								// ltType
			CWinUtilities::LSN_ABWI_GROUP,					// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST,									// iLeft
			LSN_TOP_JUST,									// iTop
			LSN_ABOUT_GROUP_W,								// dwWidth
			LSN_ABOUT_INFO_GROUP_H,							// dwHeight
			LSN_GROUPSTYLE,									// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			LSN_LSTR( LSN_STR_ABOUT_TITLE ),				// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_ABWI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			CWinUtilities::LSN_ABWI_DESC,					// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_TOP_JUST + LSN_GROUP_TOP + (LSN_DEF_STATIC_HEIGHT + LSN_TOP_JUST) * 0,// iTop
			LSN_ABOUT_GROUP_W - LSN_GROUP_LEFT * 2,			// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_STR_ABOUT_DESC ),					// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_ABWI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			CWinUtilities::LSN_ABWI_CREATOR,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_TOP_JUST + LSN_GROUP_TOP + (LSN_DEF_STATIC_HEIGHT + LSN_TOP_JUST) * 1,// iTop
			LSN_ABOUT_GROUP_W - LSN_GROUP_LEFT * 2,			// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_STR_ABOUT_CREATOR ),				// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_ABWI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			CWinUtilities::LSN_ABWI_COPYRIGHT,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_TOP_JUST + LSN_GROUP_TOP + (LSN_DEF_STATIC_HEIGHT + LSN_TOP_JUST) * 2,// iTop
			LSN_ABOUT_GROUP_W - LSN_GROUP_LEFT * 2,			// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_STR_ABOUT_COPYRIGHT ),			// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_ABWI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			CWinUtilities::LSN_ABWI_GITHUB,					// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_TOP_JUST + LSN_GROUP_TOP + (LSN_DEF_STATIC_HEIGHT + LSN_TOP_JUST) * 3,// iTop
			LSN_ABOUT_GROUP_W - LSN_GROUP_LEFT * 2,			// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE | SS_NOTIFY,					// dwStyle
			0,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_ABWI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_GROUPBOX,								// ltType
			CWinUtilities::LSN_ABWI_CREDITS_GROUP,			// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST,									// iLeft
			LSN_ABOUT_CREDITS_GROUP_T,						// iTop
			LSN_ABOUT_GROUP_W,								// dwWidth
			LSN_ABOUT_CREDITS_GROUP_H,						// dwHeight
			LSN_GROUPSTYLE,									// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			LSN_LSTR( LSN_STR_ABOUT_CREDITS ),				// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_ABWI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			CWinUtilities::LSN_ABWI_ICONS8,					// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_ABOUT_CREDITS_GROUP_T + LSN_GROUP_TOP,		// iTop
			LSN_ABOUT_GROUP_W - LSN_GROUP_LEFT * 2,			// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE | SS_NOTIFY,					// dwStyle
			0,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_ABWI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			CWinUtilities::LSN_ABWI_CREDITS_EDIT,			// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_ABOUT_CREDITS_GROUP_T + LSN_GROUP_TOP + LSN_DEF_STATIC_HEIGHT + LSN_TOP_JUST,// iTop
			LSN_ABOUT_GROUP_W - LSN_GROUP_LEFT * 2,			// dwWidth
			LSN_ABOUT_CREDITS_EDIT_H,						// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,	// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_ABWI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_BUTTON,									// ltType
			CWinUtilities::LSN_ABWI_OK,						// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			TRUE,											// bActive
			LSN_ABOUT_W - LSN_DEF_BUTTON_WIDTH - LSN_LEFT_JUST,// iLeft
			LSN_ABOUT_H - LSN_DEF_BUTTON_HEIGHT - LSN_TOP_JUST,// iTop
			LSN_DEF_BUTTON_WIDTH,							// dwWidth
			LSN_DEF_BUTTON_HEIGHT,							// dwHeight
			LSN_DEFBUTTONSTYLE,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			LSN_LSTR( LSN_OK ),								// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_ABWI_DIALOG,					// dwParentId
		},
	};

#undef LSN_ABOUT_H
#undef LSN_ABOUT_W
#undef LSN_ABOUT_CREDITS_GROUP_H
#undef LSN_ABOUT_CREDITS_EDIT_H
#undef LSN_ABOUT_CREDITS_GROUP_T
#undef LSN_ABOUT_INFO_GROUP_H
#undef LSN_ABOUT_GROUP_W


	// == Functions.
	/**
	 * Creates the About dialog.
	 *
	 * \param _pwParent The parent of the dialog.
	 * \return Returns TRUE if the dialog was created successfully.
	 */
	BOOL CAboutWindowLayout::CreateAboutDialog( CWidget * _pwParent ) {
		lsn::CLayoutManager * plmLayout = static_cast<lsn::CLayoutManager *>(lsw::CBase::LayoutManager());
		INT_PTR ipProc = plmLayout->DialogBoxX( m_wlDialog, std::size( m_wlDialog ), _pwParent, 0 );
		return ipProc != 0 ? TRUE : FALSE;
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

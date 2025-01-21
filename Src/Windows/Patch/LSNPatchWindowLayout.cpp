#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The patch window layout.
 */
 
#include "LSNPatchWindowLayout.h"
#include "../../Localization/LSNLocalization.h"
#include "../Layout/LSNLayoutMacros.h"
#include "../Layout/LSNLayoutManager.h"
 

 namespace lsn {

 #define LSN_PATCH_W								500
 #define LSN_PATCH_CONTENT_W						(LSN_PATCH_W - LSN_LEFT_JUST * 2)
 #define LSN_PATCH_FILE_GROUP_TOP					LSN_TOP_JUST
 #define LSN_PATCH_FILE_GROUP_H						(LSN_GROUP_TOP + LSN_GROUP_BOTTOM + LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST + LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST + LSN_DEF_EDIT_HEIGHT)
 
 #define LSN_PATCH_LABEL0_W							50

 #define LSN_PATCH_H								(LSN_PATCH_FILE_GROUP_TOP + LSN_PATCH_FILE_GROUP_H + LSN_TOP_JUST)

	// == Members.
	/** The layout for the main window. */
	LSW_WIDGET_LAYOUT CPatchWindowLayout::m_wlPatchWindow[] = {
		{
			LSN_LT_PATCH_DIALOG,					// ltType
			LSN_PWI_MAINWINDOW,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			LSN_PATCH_W,							// dwWidth
			LSN_PATCH_H,							// dwHeight
			(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME) | WS_VISIBLE | WS_CLIPSIBLINGS | DS_3DLOOK | DS_SETFONT,		// dwStyle
			WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,																					// dwStyleEx
			LSN_LSTR( LSN_PATCH_PATCH_ROM ),		// pwcText
			0,										// sTextLen
			LSN_PWI_NONE,							// dwParentId

			
		},

		// File.
		{
			LSW_LT_GROUPBOX,						// ltType
			LSN_PWI_FILE_GROUP,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_LEFT_JUST,							// iLeft
			LSN_PATCH_FILE_GROUP_TOP,				// iTop
			LSN_PATCH_CONTENT_W,					// dwWidth
			LSN_PATCH_FILE_GROUP_H,					// dwHeight
			LSN_GROUPSTYLE,																											// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,											// dwStyleEx
			LSN_LSTR( LSN_PATCH_FILE ),				// pwcText
			0,										// sTextLen
			LSN_PWI_NONE,							// dwParentId

			LSN_LOCK_LEFT,							// pcLeftSizeExp
			LSN_LOCK_RIGHT,							// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
		},

		{
			LSW_LT_LABEL,							// ltType
			LSN_PWI_FILE_IN_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,			// iLeft
			LSN_PATCH_FILE_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),						// iTop
			LSN_PATCH_LABEL0_W,						// dwWidth
			LSN_DEF_STATIC_HEIGHT,					// dwHeight
			LSN_STATICSTYLE,						// dwStyle
			0,										// dwStyleEx
			LSN_LSTR( LSN_PATCH_ROM_TO_PATCH ),		// pwcText
			0,										// sTextLen
			LSN_PWI_NONE,							// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			LSN_PWI_FILE_IN_EDIT,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			(LSN_LEFT_JUST + LSN_GROUP_LEFT) + LSN_PATCH_LABEL0_W,																	// iLeft
			LSN_PATCH_FILE_GROUP_TOP + LSN_GROUP_TOP,																				// iTop
			LSN_PATCH_W - ((LSN_LEFT_JUST + LSN_GROUP_LEFT) * 2 + LSN_PATCH_LABEL0_W) - (LSN_DEF_BUTTON_WIDTH) - LSN_LEFT_JUST,		// dwWidth
			LSN_DEF_EDIT_HEIGHT,					// dwHeight
			LSN_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			LSN_PWI_NONE,							// dwParentId

			LSN_LOCK_LEFT,							// pcLeftSizeExp
			LSN_LOCK_RIGHT,							// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
		},
		{
			LSW_LT_BUTTON,							// ltType
			LSN_PWI_FILE_IN_BUTTON,					// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_PATCH_W - (LSN_LEFT_JUST + LSN_GROUP_LEFT) - LSN_DEF_BUTTON_WIDTH,													// iLeft
			LSN_PATCH_FILE_GROUP_TOP + LSN_GROUP_TOP,																				// iTop
			LSN_DEF_BUTTON_WIDTH,					// dwWidth
			LSN_DEF_BUTTON_HEIGHT,					// dwHeight
			LSN_BUTTONSTYLE,						// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,											// dwStyleEx
			LSN_LSTR( LSN_PATCH_BROWSE ),			// pwcText
			0,										// sTextLen
			LSN_PWI_NONE,							// dwParentId

			nullptr, 0,								// pcLeftSizeExp
			LSN_LOCK_RIGHT,							// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			LSN_FIXED_WIDTH,						// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
		},

		{
			LSW_LT_LABEL,							// ltType
			LSN_PWI_FILE_PATCH_LABEL,				// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,			// iLeft
			LSN_PATCH_FILE_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1) + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST),						// iTop
			LSN_PATCH_LABEL0_W,						// dwWidth
			LSN_DEF_STATIC_HEIGHT,					// dwHeight
			LSN_STATICSTYLE,						// dwStyle
			0,										// dwStyleEx
			LSN_LSTR( LSN_PATCH_PATCH_FILE ),		// pwcText
			0,										// sTextLen
			LSN_PWI_NONE,							// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			LSN_PWI_FILE_PATCH_EDIT,				// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			(LSN_LEFT_JUST + LSN_GROUP_LEFT) + LSN_PATCH_LABEL0_W,																	// iLeft
			LSN_PATCH_FILE_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST),										// iTop
			LSN_PATCH_W - ((LSN_LEFT_JUST + LSN_GROUP_LEFT) * 2 + LSN_PATCH_LABEL0_W) - (LSN_DEF_BUTTON_WIDTH) - LSN_LEFT_JUST,		// dwWidth
			LSN_DEF_EDIT_HEIGHT,					// dwHeight
			LSN_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			LSN_PWI_NONE,							// dwParentId

			LSN_LOCK_LEFT,							// pcLeftSizeExp
			LSN_LOCK_RIGHT,							// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
		},
		{
			LSW_LT_BUTTON,							// ltType
			LSN_PWI_FILE_PATCH_BUTTON,				// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_PATCH_W - (LSN_LEFT_JUST + LSN_GROUP_LEFT) - LSN_DEF_BUTTON_WIDTH,													// iLeft
			LSN_PATCH_FILE_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST),										// iTop
			LSN_DEF_BUTTON_WIDTH,					// dwWidth
			LSN_DEF_BUTTON_HEIGHT,					// dwHeight
			LSN_BUTTONSTYLE,						// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,											// dwStyleEx
			LSN_LSTR( LSN_PATCH_BROWSE ),			// pwcText
			0,										// sTextLen
			LSN_PWI_NONE,							// dwParentId

			nullptr, 0,								// pcLeftSizeExp
			LSN_LOCK_RIGHT,							// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			LSN_FIXED_WIDTH,						// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
		},

		{
			LSW_LT_LABEL,							// ltType
			LSN_PWI_FILE_OUT_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,			// iLeft
			LSN_PATCH_FILE_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1) + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2,						// iTop
			LSN_PATCH_LABEL0_W,						// dwWidth
			LSN_DEF_STATIC_HEIGHT,					// dwHeight
			LSN_STATICSTYLE,						// dwStyle
			0,										// dwStyleEx
			LSN_LSTR( LSN_PATCH_OUTPUT_FILE ),		// pwcText
			0,										// sTextLen
			LSN_PWI_NONE,							// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			LSN_PWI_FILE_OUT_EDIT,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			(LSN_LEFT_JUST + LSN_GROUP_LEFT) + LSN_PATCH_LABEL0_W,																	// iLeft
			LSN_PATCH_FILE_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2,									// iTop
			LSN_PATCH_W - ((LSN_LEFT_JUST + LSN_GROUP_LEFT) * 2 + LSN_PATCH_LABEL0_W) - (LSN_DEF_BUTTON_WIDTH) - LSN_LEFT_JUST,		// dwWidth
			LSN_DEF_EDIT_HEIGHT,					// dwHeight
			LSN_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			LSN_PWI_NONE,							// dwParentId

			LSN_LOCK_LEFT,							// pcLeftSizeExp
			LSN_LOCK_RIGHT,							// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
		},
		{
			LSW_LT_BUTTON,							// ltType
			LSN_PWI_FILE_OUT_BUTTON,				// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_PATCH_W - (LSN_LEFT_JUST + LSN_GROUP_LEFT) - LSN_DEF_BUTTON_WIDTH,													// iLeft
			LSN_PATCH_FILE_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2,									// iTop
			LSN_DEF_BUTTON_WIDTH,					// dwWidth
			LSN_DEF_BUTTON_HEIGHT,					// dwHeight
			LSN_BUTTONSTYLE,						// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,											// dwStyleEx
			LSN_LSTR( LSN_PATCH_BROWSE ),			// pwcText
			0,										// sTextLen
			LSN_PWI_NONE,							// dwParentId

			nullptr, 0,								// pcLeftSizeExp
			LSN_LOCK_RIGHT,							// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			LSN_FIXED_WIDTH,						// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
		},
	};


	// == Functions.
	// Creates the window.
	CWidget * CPatchWindowLayout::CreatePatchWindow( CWidget * _pwParent ) {
		lsn::CLayoutManager * plmLayout = static_cast<lsn::CLayoutManager *>(lsw::CBase::LayoutManager());
			CWidget * pwWidget = plmLayout->CreateDialogX( m_wlPatchWindow, LSN_ELEMENTS( m_wlPatchWindow ), _pwParent, 0 );
			if ( pwWidget ) {
				// Success.  Do stuff.
			}
			return pwWidget;
		/*lsn::CLayoutManager * plmLayout = static_cast<lsn::CLayoutManager *>(lsw::CBase::LayoutManager());
		CWidget * pwThis = plmLayout->CreateWindowX( m_wlPatchWindow, LSN_ELEMENTS( m_wlPatchWindow ),
			nullptr, 0,
			_pwParent, 0 );

		return pwThis;*/
	}

 }	// namespace lsn

 #endif	// #ifdef LSN_USE_WINDOWS

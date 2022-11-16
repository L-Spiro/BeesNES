#ifdef LSN_USE_WINDOWS

#include "LSNSelectRomDialogLayout.h"
#include "../../Utilities/LSNUtilities.h"
#include "../Layout/LSNLayoutMacros.h"
#include "../Layout/LSNLayoutManager.h"

#define LSN_SF_W									350
#define LSN_SF_SELECT_GROUP_TOP						LSN_TOP_JUST
#define LSN_SF_VIEW_HEIGHT							50
#define LSN_SF_SEARCH_LABEL_W						22
#define LSN_SF_SELECT_GROUP_HEIGHT					(LSN_SF_VIEW_HEIGHT + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) + LSN_GROUP_TOP + LSN_GROUP_BOTTOM)
#define LSN_SF_H									(LSN_SF_SELECT_GROUP_HEIGHT + LSN_TOP_JUST * 4 + LSN_DEF_BUTTON_HEIGHT)

namespace lsn {

		// == Members.
	// The layout for the Open Process dialog.
	LSW_WIDGET_LAYOUT CSelectRomDialogLayout::m_wlSelectRomDialog[] = {
		{
			LSN_LT_SELECT_ROM_DIALOG,				// ltType
			LSN_SFI_DIALOG,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			64,										// iLeft
			64,										// iTop
			LSN_SF_W,								// dwWidth
			LSN_SF_H,								// dwHeight
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | WS_THICKFRAME | DS_3DLOOK | DS_FIXEDSYS | DS_CENTER | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,					// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,												// dwStyleEx
			L"Select ROM",							// pwcText
			0,										// sTextLen
			LSN_SFI_NONE,							// dwParentId
		},

		// Select ROM.
		{
			LSW_LT_GROUPBOX,						// ltType
			LSN_SFI_GROUP,							// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_LEFT_JUST,							// iLeft
			LSN_SF_SELECT_GROUP_TOP,				// iTop
			LSN_SF_W - (LSN_LEFT_JUST * 2),		// dwWidth
			LSN_SF_SELECT_GROUP_HEIGHT,			// dwHeight
			LSN_GROUPSTYLE,																																// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																// dwStyleEx
			L"File Listing",						// pwcText
			0,										// sTextLen
			LSN_SFI_DIALOG,							// dwParentId

			LSN_LOCK_LEFT,							// pcLeftSizeExp
			LSN_LOCK_RIGHT,							// pcRightSizeExp
			LSN_LOCK_TOP,							// pcTopSizeExp
			LSN_LOCK_BOTTOM,						// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
		},
		{
			LSW_LT_LISTBOX,							// ltType
			LSN_SFI_LISTBOX,						// wId
			WC_LISTBOXW,							// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,			// iLeft
			LSN_SF_SELECT_GROUP_TOP + LSN_GROUP_TOP,	// iTop
			LSN_SF_W - (LSN_LEFT_JUST + LSN_GROUP_LEFT) * 2,																							// dwWidth
			LSN_SF_VIEW_HEIGHT,					// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | WS_HSCROLL | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT | LBS_HASSTRINGS,					// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY | WS_EX_CLIENTEDGE,												// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			LSN_SFI_DIALOG,							// dwParentId

			LSN_LOCK_LEFT,							// pcLeftSizeExp
			LSN_LOCK_RIGHT,							// pcRightSizeExp
			LSN_LOCK_TOP,							// pcTopSizeExp
			LSN_LOCK_BOTTOM,						// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
		},
		{
			LSW_LT_LABEL,							// ltType
			LSN_SFI_LABEL_SEARCH,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,			// iLeft
			LSN_SF_SELECT_GROUP_TOP + LSN_GROUP_TOP + LSN_SF_VIEW_HEIGHT + LSN_TOP_JUST + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) / 2),																// iTop
			LSN_SF_SEARCH_LABEL_W,					// dwWidth
			LSN_DEF_STATIC_HEIGHT,					// dwHeight
			LSN_STATICSTYLE,						// dwStyle
			0,										// dwStyleEx
			L"Search:",								// pwcText
			0,										// sTextLen
			LSN_SFI_DIALOG,							// dwParentId

			LSN_LOCK_LEFT,							// pcLeftSizeExp
			nullptr, 0,								// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			LSN_LOCK_BOTTOM,						// pcBottomSizeExp
			LSN_FIXED_WIDTH,						// pcWidthSizeExp
			LSN_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			LSN_SFI_EDIT_SEARCH,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT + LSN_SF_SEARCH_LABEL_W + LSN_LEFT_JUST,							// iLeft
			LSN_SF_SELECT_GROUP_TOP + LSN_GROUP_TOP + LSN_SF_VIEW_HEIGHT + LSN_TOP_JUST,																// iTop
			LSN_SF_W - (LSN_LEFT_JUST + LSN_GROUP_LEFT) * 2 - (LSN_SF_SEARCH_LABEL_W + LSN_LEFT_JUST),		// dwWidth
			LSN_DEF_EDIT_HEIGHT,					// dwHeight
			LSN_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			LSN_SFI_DIALOG,							// dwParentId

			LSN_LOCK_LEFT,							// pcLeftSizeExp
			LSN_LOCK_RIGHT,							// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			LSN_LOCK_BOTTOM,						// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			LSN_FIXED_HEIGHT,						// pcHeightSizeExp
		},

		{
			LSW_LT_BUTTON,							// ltType
			LSN_SFI_BUTTON_OK,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			LSN_SF_W - LSN_DEF_BUTTON_WIDTH * 2 - 2 - 2,	// iLeft
			LSN_SF_H - LSN_DEF_BUTTON_HEIGHT - 2,		// iTop
			LSN_DEF_BUTTON_WIDTH,					// dwWidth
			LSN_DEF_BUTTON_HEIGHT,					// dwHeight
			LSN_DEFBUTTONSTYLE,						// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,						// dwStyleEx
			L"OK",									// pwcText
			0,										// sTextLen
			LSN_SFI_DIALOG,							// dwParentId

			nullptr, 0,								// pcLeftSizeExp
			LSN_LOCK_RIGHT,							// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			LSN_LOCK_BOTTOM,						// pcBottomSizeExp
			LSN_FIXED_WIDTH,						// pcWidthSizeExp
			LSN_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_BUTTON,							// ltType
			LSN_SFI_BUTTON_CANCEL,					// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_SF_W - LSN_DEF_BUTTON_WIDTH * 1 - 2,// iLeft
			LSN_SF_H - LSN_DEF_BUTTON_HEIGHT - 2,	// iTop
			LSN_DEF_BUTTON_WIDTH,					// dwWidth
			LSN_DEF_BUTTON_HEIGHT,					// dwHeight
			LSN_BUTTONSTYLE,						// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,						// dwStyleEx
			L"Cancel",								// pwcText
			0,										// sTextLen
			LSN_SFI_DIALOG,							// dwParentId

			nullptr, 0,								// pcLeftSizeExp
			LSN_LOCK_RIGHT,							// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			LSN_LOCK_BOTTOM,						// pcBottomSizeExp
			LSN_FIXED_WIDTH,						// pcWidthSizeExp
			LSN_FIXED_HEIGHT,						// pcHeightSizeExp
		},
	};

#undef LSN_SF_H
#undef LSN_SF_LAST_SECTION_H
#undef LSN_SF_LAST_SECTION_TOP
#undef LSN_SF_ALL_CHECK_W
#undef LSN_SF_MAIN_CHECK_W
#undef LSN_SF_SHOW_GROUP_HEIGHT
#undef LSN_SF_SHOW_GROUP_TOP
#undef LSN_SF_SELECT_GROUP_HEIGHT
#undef LSN_SF_VIEW_HEIGHT
#undef LSN_SF_SELECT_GROUP_TOP
#undef LSN_SF_W

	// == Functions.
	// Creates the Select ROM dialog.
	DWORD CSelectRomDialogLayout::CreateSelectRomDialog( CWidget * _pwParent, const std::vector<std::u16string> * _pvFiles ) {
		lsn::CLayoutManager * plmLayout = static_cast<lsn::CLayoutManager *>(lsw::CBase::LayoutManager());
		INT_PTR ipProc = plmLayout->DialogBoxX( m_wlSelectRomDialog, LSN_ELEMENTS( m_wlSelectRomDialog ), _pwParent, reinterpret_cast<uint64_t>(_pvFiles) );

		return static_cast<DWORD>(ipProc);
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

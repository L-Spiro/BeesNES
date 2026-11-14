#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The layout for the Add/Edit/Duplicate Cheat dialog.
 */

#include "LSNCheatEditWindowLayout.h"
#include "../../Localization/LSNLocalization.h"
#include "../Layout/LSNLayoutMacros.h"
#include "../Layout/LSNLayoutManager.h"

namespace lsn {

#define LSN_CE_W												360
#define LSN_CE_GROUP_W											(LSN_CE_W - LSN_LEFT_JUST * 2)
#define LSN_CE_L												(LSN_LEFT_JUST + LSN_GROUP_LEFT)
#define LSN_CE_INNER_W											(LSN_CE_GROUP_W - LSN_GROUP_LEFT * 2)
#define LSN_CE_LABEL_OFF										((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) / 2)
#define LSN_CE_BUTTON_LABEL_OFF									((LSN_DEF_BUTTON_HEIGHT - LSN_DEF_STATIC_HEIGHT) / 2)
#define LSN_CE_TITLE_LABEL_W									30
#define LSN_CE_TITLE_T											(LSN_TOP_JUST + LSN_GROUP_TOP)
#define LSN_CE_DESC_LABEL_T										(LSN_CE_TITLE_T + LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST * 2)
#define LSN_CE_DESC_T											(LSN_CE_DESC_LABEL_T + LSN_DEF_STATIC_HEIGHT + LSN_TOP_JUST)
#define LSN_CE_DESC_H											64
#define LSN_CE_CODES_LABEL_T									(LSN_CE_DESC_T + LSN_CE_DESC_H + LSN_TOP_JUST * 2)
#define LSN_CE_CODES_T											(LSN_CE_CODES_LABEL_T + LSN_DEF_STATIC_HEIGHT + LSN_TOP_JUST)
#define LSN_CE_CODES_H											72
#define LSN_CE_CODE_W											((LSN_CE_INNER_W - LSN_LEFT_JUST * 2) / 2)
#define LSN_CE_RAW_L											(LSN_CE_L + LSN_CE_INNER_W - LSN_CE_CODE_W)
#define LSN_CE_HINT_T											(LSN_CE_CODES_T + LSN_CE_CODES_H + LSN_TOP_JUST)
#define LSN_CE_ENABLED_T										(LSN_CE_HINT_T + LSN_DEF_STATIC_HEIGHT + LSN_TOP_JUST * 2)
#define LSN_CE_CHEAT_GROUP_H									(LSN_CE_ENABLED_T + LSN_DEF_CHECK_HEIGHT + LSN_GROUP_BOTTOM - LSN_TOP_JUST)
#define LSN_CE_HOTKEY_GROUP_T									(LSN_TOP_JUST + LSN_CE_CHEAT_GROUP_H + LSN_TOP_JUST)
#define LSN_CE_RADIO_T											(LSN_CE_HOTKEY_GROUP_T + LSN_GROUP_TOP + 1)
#define LSN_CE_RADIO_W											70
#define LSN_CE_CLEAR_W											(LSN_DEF_BUTTON_WIDTH + 20)
#define LSN_CE_KEY_T											(LSN_CE_RADIO_T + LSN_DEF_RADIO_HEIGHT + LSN_TOP_JUST * 2)
#define LSN_CE_KEY_LABEL_W										44
#define LSN_CE_KEY_BUTTON_W										((LSN_CE_INNER_W - (LSN_CE_KEY_LABEL_W + LSN_LEFT_JUST) * 2 - LSN_LEFT_JUST * 4) / 2)
#define LSN_CE_KEY2_L											(LSN_CE_L + LSN_CE_KEY_LABEL_W + LSN_LEFT_JUST + LSN_CE_KEY_BUTTON_W + LSN_LEFT_JUST * 4)
#define LSN_CE_HOTKEY_GROUP_H									(LSN_CE_KEY_T + LSN_DEF_BUTTON_HEIGHT + LSN_GROUP_BOTTOM - LSN_CE_HOTKEY_GROUP_T)
#define LSN_CE_H												(LSN_CE_HOTKEY_GROUP_T + LSN_CE_HOTKEY_GROUP_H + LSN_TOP_JUST + LSN_DEF_BUTTON_HEIGHT + LSN_TOP_JUST)

	// == Members.
	/** The layout for the Add/Edit/Duplicate Cheat dialog. */
	LSW_WIDGET_LAYOUT CCheatEditWindowLayout::m_wlDialog[] = {
		{
			LSN_LT_CHEAT_EDIT,								// ltType
			CWinUtilities::LSN_CEWI_DIALOG,					// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			0,												// iLeft
			0,												// iTop
			LSN_CE_W,										// dwWidth
			LSN_CE_H,										// dwHeight
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | DS_3DLOOK | DS_FIXEDSYS | DS_MODALFRAME | DS_CENTER,	// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,	// dwStyleEx
			LSN_LSTR( LSN_STR_EDIT_CHEAT ),					// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_CEWI_NONE,					// dwParentId
		},
		{
			LSW_LT_GROUPBOX,								// ltType
			CWinUtilities::LSN_CEWI_CHEAT_GROUP,			// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST,									// iLeft
			LSN_TOP_JUST,									// iTop
			LSN_CE_GROUP_W,									// dwWidth
			LSN_CE_CHEAT_GROUP_H,							// dwHeight
			LSN_GROUPSTYLE,									// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			LSN_LSTR( LSN_STR_CHEATS ),						// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_CEWI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			CWinUtilities::LSN_CEWI_TITLE_LABEL,			// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_CE_L,										// iLeft
			LSN_CE_TITLE_T + LSN_CE_LABEL_OFF,				// iTop
			LSN_CE_TITLE_LABEL_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_STR_TITLE_ ),						// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_CEWI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			CWinUtilities::LSN_CEWI_TITLE_EDIT,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			TRUE,											// bActive
			LSN_CE_L + LSN_CE_TITLE_LABEL_W + LSN_LEFT_JUST,	// iLeft
			LSN_CE_TITLE_T,									// iTop
			LSN_CE_INNER_W - LSN_CE_TITLE_LABEL_W - LSN_LEFT_JUST,	// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_CEWI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			CWinUtilities::LSN_CEWI_DESC_LABEL,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_CE_L,										// iLeft
			LSN_CE_DESC_LABEL_T,							// iTop
			LSN_CE_INNER_W,									// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_STR_DESCRIPTION_ ),				// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_CEWI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			CWinUtilities::LSN_CEWI_DESC_EDIT,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_CE_L,										// iLeft
			LSN_CE_DESC_T,									// iTop
			LSN_CE_INNER_W,									// dwWidth
			LSN_CE_DESC_H,									// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN | WS_VSCROLL,	// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_CEWI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			CWinUtilities::LSN_CEWI_GG_LABEL,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_CE_L,										// iLeft
			LSN_CE_CODES_LABEL_T,							// iTop
			LSN_CE_CODE_W,									// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_STR_GAME_GENIE ),					// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_CEWI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			CWinUtilities::LSN_CEWI_RAW_LABEL,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_CE_RAW_L,									// iLeft
			LSN_CE_CODES_LABEL_T,							// iTop
			LSN_CE_CODE_W,									// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_STR_RAW_CODE_FORMAT ),			// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_CEWI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			CWinUtilities::LSN_CEWI_GG_EDIT,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_CE_L,										// iLeft
			LSN_CE_CODES_T,									// iTop
			LSN_CE_CODE_W,									// dwWidth
			LSN_CE_CODES_H,									// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_UPPERCASE | ES_WANTRETURN | WS_VSCROLL,	// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_CEWI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			CWinUtilities::LSN_CEWI_RAW_EDIT,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_CE_RAW_L,									// iLeft
			LSN_CE_CODES_T,									// iTop
			LSN_CE_CODE_W,									// dwWidth
			LSN_CE_CODES_H,									// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_UPPERCASE | ES_WANTRETURN | WS_VSCROLL,	// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_CEWI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			CWinUtilities::LSN_CEWI_HINT_LABEL,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_CE_L,										// iLeft
			LSN_CE_HINT_T,									// iTop
			LSN_CE_INNER_W,									// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_STR_CHEAT_EDIT_HINT ),			// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_CEWI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_CHECK,									// ltType
			CWinUtilities::LSN_CEWI_ENABLED_CHECK,			// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_CE_L,										// iLeft
			LSN_CE_ENABLED_T,								// iTop
			80,												// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_STR_ENABLED ),					// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_CEWI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_GROUPBOX,								// ltType
			CWinUtilities::LSN_CEWI_HOTKEY_GROUP,			// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST,									// iLeft
			LSN_CE_HOTKEY_GROUP_T,							// iTop
			LSN_CE_GROUP_W,									// dwWidth
			LSN_CE_HOTKEY_GROUP_H,							// dwHeight
			LSN_GROUPSTYLE,									// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			LSN_LSTR( LSN_STR_HOTKEYS ),					// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_CEWI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_RADIO,									// ltType
			CWinUtilities::LSN_CEWI_TOGGLE_RADIO,			// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_CE_L,										// iLeft
			LSN_CE_RADIO_T,									// iTop
			LSN_CE_RADIO_W,									// dwWidth
			LSN_DEF_RADIO_HEIGHT,							// dwHeight
			LSN_RADIOSTYLE | WS_GROUP,						// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_STR_TOGGLE ),						// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_CEWI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_RADIO,									// ltType
			CWinUtilities::LSN_CEWI_ENABLE_DISABLE_RADIO,	// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_CE_L + LSN_CE_RADIO_W + LSN_LEFT_JUST,		// iLeft
			LSN_CE_RADIO_T,									// iTop
			LSN_CE_RADIO_W,									// dwWidth
			LSN_DEF_RADIO_HEIGHT,							// dwHeight
			LSN_RADIOSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_STR_ENABLE_DISABLE ),				// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_CEWI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_BUTTON,									// ltType
			CWinUtilities::LSN_CEWI_CLEAR_BUTTON,			// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_CE_L + LSN_CE_INNER_W - LSN_CE_CLEAR_W,		// iLeft
			LSN_CE_RADIO_T + ((LSN_DEF_RADIO_HEIGHT - LSN_DEF_BUTTON_HEIGHT) / 2),	// iTop
			LSN_CE_CLEAR_W,									// dwWidth
			LSN_DEF_BUTTON_HEIGHT,							// dwHeight
			LSN_BUTTONSTYLE,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			LSN_LSTR( LSN_STR_CLEAR_HOTKEYS ),				// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_CEWI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			CWinUtilities::LSN_CEWI_KEY1_LABEL,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_CE_L,										// iLeft
			LSN_CE_KEY_T + LSN_CE_BUTTON_LABEL_OFF,			// iTop
			LSN_CE_KEY_LABEL_W,								// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_STR_TOGGLE_KEY_ ),				// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_CEWI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_BUTTON,									// ltType
			CWinUtilities::LSN_CEWI_KEY1_BUTTON,			// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_CE_L + LSN_CE_KEY_LABEL_W + LSN_LEFT_JUST,	// iLeft
			LSN_CE_KEY_T,									// iTop
			LSN_CE_KEY_BUTTON_W,							// dwWidth
			LSN_DEF_BUTTON_HEIGHT,							// dwHeight
			LSN_BUTTONSTYLE,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			LSN_LSTR( LSN_STR_NO_HOTKEY ),					// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_CEWI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			CWinUtilities::LSN_CEWI_KEY2_LABEL,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_CE_KEY2_L,									// iLeft
			LSN_CE_KEY_T + LSN_CE_BUTTON_LABEL_OFF,			// iTop
			LSN_CE_KEY_LABEL_W,								// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_STR_DISABLE_KEY_ ),				// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_CEWI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_BUTTON,									// ltType
			CWinUtilities::LSN_CEWI_KEY2_BUTTON,			// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_CE_KEY2_L + LSN_CE_KEY_LABEL_W + LSN_LEFT_JUST,	// iLeft
			LSN_CE_KEY_T,									// iTop
			LSN_CE_KEY_BUTTON_W,							// dwWidth
			LSN_DEF_BUTTON_HEIGHT,							// dwHeight
			LSN_BUTTONSTYLE,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			LSN_LSTR( LSN_STR_NO_HOTKEY ),					// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_CEWI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_BUTTON,									// ltType
			CWinUtilities::LSN_CEWI_OK,						// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_CE_W - (LSN_DEF_BUTTON_WIDTH + LSN_LEFT_JUST) * 2,	// iLeft
			LSN_CE_H - LSN_DEF_BUTTON_HEIGHT - LSN_TOP_JUST,	// iTop
			LSN_DEF_BUTTON_WIDTH,							// dwWidth
			LSN_DEF_BUTTON_HEIGHT,							// dwHeight
			LSN_DEFBUTTONSTYLE,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			LSN_LSTR( LSN_OK ),								// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_CEWI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_BUTTON,									// ltType
			CWinUtilities::LSN_CEWI_CANCEL,					// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_CE_W - LSN_DEF_BUTTON_WIDTH - LSN_LEFT_JUST,	// iLeft
			LSN_CE_H - LSN_DEF_BUTTON_HEIGHT - LSN_TOP_JUST,	// iTop
			LSN_DEF_BUTTON_WIDTH,							// dwWidth
			LSN_DEF_BUTTON_HEIGHT,							// dwHeight
			LSN_BUTTONSTYLE,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			LSN_LSTR( LSN_CANCEL ),							// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_CEWI_DIALOG,					// dwParentId
		},
	};

#undef LSN_CE_H
#undef LSN_CE_HOTKEY_GROUP_H
#undef LSN_CE_KEY2_L
#undef LSN_CE_KEY_BUTTON_W
#undef LSN_CE_KEY_LABEL_W
#undef LSN_CE_KEY_T
#undef LSN_CE_CLEAR_W
#undef LSN_CE_RADIO_W
#undef LSN_CE_RADIO_T
#undef LSN_CE_HOTKEY_GROUP_T
#undef LSN_CE_CHEAT_GROUP_H
#undef LSN_CE_ENABLED_T
#undef LSN_CE_HINT_T
#undef LSN_CE_RAW_L
#undef LSN_CE_CODE_W
#undef LSN_CE_CODES_H
#undef LSN_CE_CODES_T
#undef LSN_CE_CODES_LABEL_T
#undef LSN_CE_DESC_H
#undef LSN_CE_DESC_T
#undef LSN_CE_DESC_LABEL_T
#undef LSN_CE_TITLE_T
#undef LSN_CE_TITLE_LABEL_W
#undef LSN_CE_BUTTON_LABEL_OFF
#undef LSN_CE_LABEL_OFF
#undef LSN_CE_INNER_W
#undef LSN_CE_L
#undef LSN_CE_GROUP_W
#undef LSN_CE_W


	// == Functions.
	/**
	 * Creates the Add/Edit/Duplicate Cheat dialog.
	 *
	 * \param _pwParent The parent of the dialog.
	 * \param _cedData The cheat to show (and the dialog title) and the returned cheat.
	 * \return Returns TRUE if the user accepted the changes, in which case _cedData holds the edited cheat.
	 */
	BOOL CCheatEditWindowLayout::CreateCheatEditDialog( CWidget * _pwParent, LSN_CHEAT_EDIT_DATA &_cedData ) {
		lsn::CLayoutManager * plmLayout = static_cast<lsn::CLayoutManager *>(lsw::CBase::LayoutManager());
		INT_PTR ipProc = plmLayout->DialogBoxX( m_wlDialog, std::size( m_wlDialog ), _pwParent, reinterpret_cast<uint64_t>(&_cedData) );
		return ipProc == 1 ? TRUE : FALSE;
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

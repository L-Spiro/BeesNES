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
#define LSN_FIRST_SECTION_L							(LSN_LEFT_JUST + LSN_GROUP_LEFT)
#define LSN_COMBO_W									(195 * 60 / 100)
#define LSN_COMBO_L									(LSN_FIRST_SECTION_L + LSN_FIRST_SECTION_W + LSN_LEFT_JUST)
#define LSN_THIRD_SECTION_W							(82 * 60 / 100)
#define LSN_THIRD_SECTION_L							(LSN_COMBO_L + LSN_COMBO_W + LSN_LEFT_JUST)
#define LSN_GENERAL_GROUP_H							(LSN_GROUP_TOP + LSN_DEF_COMBO_HEIGHT + LSN_GROUP_BOTTOM)
#define LSN_SETTINGS_GROUP_H						(LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT * 5) + (LSN_TOP_JUST * 4) + LSN_GROUP_BOTTOM)

#define LSN_INPUT_GROUP_W							(LSN_GROUP_LEFT + LSN_FIRST_SECTION_W + LSN_LEFT_JUST + LSN_COMBO_W + LSN_LEFT_JUST + LSN_THIRD_SECTION_W + LSN_GROUP_RIGHT)
#define LSN_INPUT_W									(LSN_LEFT_JUST + LSN_LEFT_JUST + LSN_INPUT_GROUP_W + LSN_LEFT_JUST + LSN_LEFT_JUST)
#define LSN_INPUT_H									(LSN_GENERAL_GROUP_H + LSN_SETTINGS_GROUP_H + 20)

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
			LSN_INPUT_H,							// dwHeight
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
			(LSN_GENERAL_GROUP_H + LSN_SETTINGS_GROUP_H) + 22,						// dwHeight
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
			LSN_LT_GLOBAL_INPUT_PAGE,				// ltType
			LSN_IWI_GLOBAL_SETTINGS_PANEL,			// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0, 0,									// iLeft, iTop
			LSN_LEFT_JUST + LSN_INPUT_GROUP_W + LSN_LEFT_JUST,
			(LSN_GENERAL_GROUP_H + LSN_SETTINGS_GROUP_H),
			WS_CHILDWINDOW | WS_VISIBLE | DS_3DLOOK | DS_FIXEDSYS | DS_SETFONT | DS_CONTROL,										// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_CONTROLPARENT,												// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			LSN_IWI_NONE,							// dwParentId

			LSN_PARENT_VCLEFT,						// pcLeftSizeExp
			nullptr, 0,								// pcRightSizeExp
			LSN_PARENT_VCTOP,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			LSN_FIXED_WIDTH,						// pcWidthSizeExp
			LSN_FIXED_HEIGHT,						// pcHeightSizeExp
		},


		// General.
		{
			LSW_LT_GROUPBOX,						// ltType
			LSN_IWI_GLOBAL( GENERAL_GROUP ),		// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_LEFT_JUST,							// iLeft
			0,										// iTop
			LSN_INPUT_GROUP_W,						// dwWidth
			LSN_GENERAL_GROUP_H,					// dwHeight
			LSN_GROUPSTYLE,																											// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,											// dwStyleEx
			LSN_LSTR( LSN_INPUT_GENERAL ),			// pwcText
			0,										// sTextLen
			LSN_IWI_GLOBAL_SETTINGS_PANEL,			// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			LSN_IWI_GLOBAL( CONSOLE_TYPE_LABEL ),	// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_FIRST_SECTION_L,					// iLeft
			LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),							// iTop
			LSN_FIRST_SECTION_W,					// dwWidth
			LSN_DEF_STATIC_HEIGHT,					// dwHeight
			LSN_STATICSTYLE,						// dwStyle
			0,										// dwStyleEx
			LSN_LSTR( LSN_INPUT_CONSOLE_TYPE_ ),	// pwcText
			0,										// sTextLen
			LSN_IWI_GLOBAL_SETTINGS_PANEL,			// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			LSN_IWI_GLOBAL( CONSOLE_TYPE_COMBO ),	// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_COMBO_L,							// iLeft
			LSN_GROUP_TOP,							// iTop
			LSN_COMBO_W,							// dwWidth
			LSN_DEF_COMBO_HEIGHT,					// dwHeight
			LSN_COMBOSTYLE_LIST,					// dwStyle
			LSN_COMBOSTYLEEX_LIST,					// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			LSN_IWI_GLOBAL_SETTINGS_PANEL,			// dwParentId
		},


		// Settings.
		{
			LSW_LT_GROUPBOX,						// ltType
			LSN_IWI_GLOBAL( SETTINGS_GROUP ),		// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_LEFT_JUST,							// iLeft
			LSN_GENERAL_GROUP_H,					// iTop
			LSN_INPUT_GROUP_W,						// dwWidth
			LSN_SETTINGS_GROUP_H,					// dwHeight
			LSN_GROUPSTYLE,																											// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,											// dwStyleEx
			LSN_LSTR( LSN_INPUT_SETTINGS ),			// pwcText
			0,										// sTextLen
			LSN_IWI_GLOBAL_SETTINGS_PANEL,			// dwParentId
		},

		{
			LSW_LT_LABEL,							// ltType
			LSN_IWI_GLOBAL( PLAYER_1_LABEL ),		// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,			// iLeft
			LSN_GENERAL_GROUP_H + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),							// iTop
			LSN_FIRST_SECTION_W,					// dwWidth
			LSN_DEF_STATIC_HEIGHT,					// dwHeight
			LSN_STATICSTYLE,						// dwStyle
			0,										// dwStyleEx
			LSN_LSTR( LSN_INPUT_PLAYER_1_ ),		// pwcText
			0,										// sTextLen
			LSN_IWI_GLOBAL_SETTINGS_PANEL,			// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			LSN_IWI_GLOBAL( PLAYER_1_COMBO ),		// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_COMBO_L,							// iLeft
			LSN_GENERAL_GROUP_H + LSN_GROUP_TOP,	// iTop
			LSN_COMBO_W,							// dwWidth
			LSN_DEF_COMBO_HEIGHT,					// dwHeight
			LSN_COMBOSTYLE_LIST,					// dwStyle
			LSN_COMBOSTYLEEX_LIST,					// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			LSN_IWI_GLOBAL_SETTINGS_PANEL,			// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			LSN_IWI_GLOBAL( PLAYER_1_BUTTON ),		// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_THIRD_SECTION_L,					// iLeft
			LSN_GENERAL_GROUP_H + LSN_GROUP_TOP,	// iTop
			LSN_THIRD_SECTION_W,					// dwWidth
			LSN_DEF_BUTTON_HEIGHT,					// dwHeight
			LSN_BUTTONSTYLE,						// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																								// dwStyleEx
			LSN_LSTR( LSN_INPUT_CONFIGURE ),		// pwcText
			0,										// sTextLen
			LSN_IWI_GLOBAL_SETTINGS_PANEL,			// dwParentId
		},

		{
			LSW_LT_LABEL,							// ltType
			LSN_IWI_GLOBAL( PLAYER_2_LABEL ),		// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,			// iLeft
			LSN_GENERAL_GROUP_H + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 1,							// iTop
			LSN_FIRST_SECTION_W,					// dwWidth
			LSN_DEF_STATIC_HEIGHT,					// dwHeight
			LSN_STATICSTYLE,						// dwStyle
			0,										// dwStyleEx
			LSN_LSTR( LSN_INPUT_PLAYER_2_ ),		// pwcText
			0,										// sTextLen
			LSN_IWI_GLOBAL_SETTINGS_PANEL,			// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			LSN_IWI_GLOBAL( PLAYER_2_COMBO ),		// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_COMBO_L,							// iLeft
			LSN_GENERAL_GROUP_H + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 1,	// iTop
			LSN_COMBO_W,							// dwWidth
			LSN_DEF_COMBO_HEIGHT,					// dwHeight
			LSN_COMBOSTYLE_LIST,					// dwStyle
			LSN_COMBOSTYLEEX_LIST,					// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			LSN_IWI_GLOBAL_SETTINGS_PANEL,			// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			LSN_IWI_GLOBAL( PLAYER_2_BUTTON ),		// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_THIRD_SECTION_L,					// iLeft
			LSN_GENERAL_GROUP_H + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 1,	// iTop
			LSN_THIRD_SECTION_W,					// dwWidth
			LSN_DEF_BUTTON_HEIGHT,					// dwHeight
			LSN_BUTTONSTYLE,						// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																					// dwStyleEx
			LSN_LSTR( LSN_INPUT_CONFIGURE ),		// pwcText
			0,										// sTextLen
			LSN_IWI_GLOBAL_SETTINGS_PANEL,			// dwParentId
		},

		// Use Four Score.
		{
			LSW_LT_CHECK,							// ltType
			LSN_IWI_PER_GAME( USE_FOUR_SCORE_CHECK ),																														// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,			// iLeft
			LSN_GENERAL_GROUP_H + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 2,							// iTop
			LSN_INPUT_GROUP_W - LSN_GROUP_LEFT * 2,	// dwWidth
			LSN_DEF_CHECK_HEIGHT,					// dwHeight
			LSN_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			LSN_LSTR( LSN_INPUT_USE_4_SCORE ),		// pwcText
			0,										// sTextLen
			LSN_IWI_GLOBAL_SETTINGS_PANEL,			// dwParentId
		},

		// Expension Port.
		{
			LSW_LT_LABEL,							// ltType
			LSN_IWI_GLOBAL( EXPAND_LABEL ),			// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,			// iLeft
			LSN_GENERAL_GROUP_H + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 2,							// iTop
			LSN_FIRST_SECTION_W,					// dwWidth
			LSN_DEF_STATIC_HEIGHT,					// dwHeight
			LSN_STATICSTYLE,						// dwStyle
			0,										// dwStyleEx
			LSN_LSTR( LSN_INPUT_EXPANSION_PORT ),	// pwcText
			0,										// sTextLen
			LSN_IWI_GLOBAL_SETTINGS_PANEL,			// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			LSN_IWI_GLOBAL( EXPAND_COMBO ),			// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_COMBO_L,							// iLeft
			LSN_GENERAL_GROUP_H + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 2,	// iTop
			LSN_COMBO_W,							// dwWidth
			LSN_DEF_COMBO_HEIGHT,					// dwHeight
			LSN_COMBOSTYLE_LIST,					// dwStyle
			LSN_COMBOSTYLEEX_LIST,					// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			LSN_IWI_GLOBAL_SETTINGS_PANEL,			// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			LSN_IWI_GLOBAL( EXPAND_BUTTON ),		// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_THIRD_SECTION_L,					// iLeft
			LSN_GENERAL_GROUP_H + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 2,	// iTop
			LSN_THIRD_SECTION_W,					// dwWidth
			LSN_DEF_BUTTON_HEIGHT,					// dwHeight
			LSN_BUTTONSTYLE,						// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																								// dwStyleEx
			LSN_LSTR( LSN_INPUT_CONFIGURE ),		// pwcText
			0,										// sTextLen
			LSN_IWI_GLOBAL_SETTINGS_PANEL,			// dwParentId
		},

		// Player 3.
		{
			LSW_LT_LABEL,							// ltType
			LSN_IWI_GLOBAL( PLAYER_3_LABEL ),		// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,			// iLeft
			LSN_GENERAL_GROUP_H + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 3,							// iTop
			LSN_FIRST_SECTION_W,					// dwWidth
			LSN_DEF_STATIC_HEIGHT,					// dwHeight
			LSN_STATICSTYLE,						// dwStyle
			0,										// dwStyleEx
			LSN_LSTR( LSN_INPUT_PLAYER_3_ ),		// pwcText
			0,										// sTextLen
			LSN_IWI_GLOBAL_SETTINGS_PANEL,			// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			LSN_IWI_GLOBAL( PLAYER_3_COMBO ),		// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_COMBO_L,							// iLeft
			LSN_GENERAL_GROUP_H + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 3,	// iTop
			LSN_COMBO_W,							// dwWidth
			LSN_DEF_COMBO_HEIGHT,					// dwHeight
			LSN_COMBOSTYLE_LIST,					// dwStyle
			LSN_COMBOSTYLEEX_LIST,					// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			LSN_IWI_GLOBAL_SETTINGS_PANEL,			// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			LSN_IWI_GLOBAL( PLAYER_3_BUTTON ),		// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_THIRD_SECTION_L,					// iLeft
			LSN_GENERAL_GROUP_H + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 3,	// iTop
			LSN_THIRD_SECTION_W,					// dwWidth
			LSN_DEF_BUTTON_HEIGHT,					// dwHeight
			LSN_BUTTONSTYLE,						// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																								// dwStyleEx
			LSN_LSTR( LSN_INPUT_CONFIGURE ),		// pwcText
			0,										// sTextLen
			LSN_IWI_GLOBAL_SETTINGS_PANEL,			// dwParentId
		},

		// Player 4.
		{
			LSW_LT_LABEL,							// ltType
			LSN_IWI_GLOBAL( PLAYER_4_LABEL ),		// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,			// iLeft
			LSN_GENERAL_GROUP_H + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 4,							// iTop
			LSN_FIRST_SECTION_W,					// dwWidth
			LSN_DEF_STATIC_HEIGHT,					// dwHeight
			LSN_STATICSTYLE,						// dwStyle
			0,										// dwStyleEx
			LSN_LSTR( LSN_INPUT_PLAYER_4_ ),		// pwcText
			0,										// sTextLen
			LSN_IWI_GLOBAL_SETTINGS_PANEL,			// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			LSN_IWI_GLOBAL( PLAYER_4_COMBO ),		// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_COMBO_L,							// iLeft
			LSN_GENERAL_GROUP_H + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 4,	// iTop
			LSN_COMBO_W,							// dwWidth
			LSN_DEF_COMBO_HEIGHT,					// dwHeight
			LSN_COMBOSTYLE_LIST,					// dwStyle
			LSN_COMBOSTYLEEX_LIST,					// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			LSN_IWI_GLOBAL_SETTINGS_PANEL,			// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			LSN_IWI_GLOBAL( PLAYER_4_BUTTON ),		// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_THIRD_SECTION_L,					// iLeft
			LSN_GENERAL_GROUP_H + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 4,	// iTop
			LSN_THIRD_SECTION_W,					// dwWidth
			LSN_DEF_BUTTON_HEIGHT,					// dwHeight
			LSN_BUTTONSTYLE,						// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR /*| WS_EX_NOPARENTNOTIFY*/,																								// dwStyleEx
			LSN_LSTR( LSN_INPUT_CONFIGURE ),		// pwcText
			0,										// sTextLen
			LSN_IWI_GLOBAL_SETTINGS_PANEL,			// dwParentId
		},
	};

	/** The layout for the per-game-setup panel. */
	LSW_WIDGET_LAYOUT CInputWindowLayout::m_wlPerGamePanel[] = {
		{
			LSN_LT_PER_GAME_INPUT_PAGE,				// ltType
			LSN_IWI_PER_GAME_SETTINGS_PANEL,		// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0, 0,									// iLeft, iTop
			LSN_LEFT_JUST + LSN_INPUT_GROUP_W + LSN_LEFT_JUST,
			(LSN_GENERAL_GROUP_H + LSN_SETTINGS_GROUP_H),
			WS_CHILDWINDOW | WS_VISIBLE | DS_3DLOOK | DS_FIXEDSYS | DS_SETFONT | DS_CONTROL,										// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_CONTROLPARENT,												// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			LSN_IWI_NONE,							// dwParentId

			LSN_PARENT_VCLEFT,						// pcLeftSizeExp
			nullptr, 0,								// pcRightSizeExp
			LSN_PARENT_VCTOP,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			LSN_FIXED_WIDTH,						// pcWidthSizeExp
			LSN_FIXED_HEIGHT,						// pcHeightSizeExp
		},


		// General.
		{
			LSW_LT_GROUPBOX,						// ltType
			LSN_IWI_PER_GAME( GENERAL_GROUP ),		// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_LEFT_JUST,							// iLeft
			0,										// iTop
			LSN_INPUT_GROUP_W,						// dwWidth
			LSN_GENERAL_GROUP_H,					// dwHeight
			LSN_GROUPSTYLE,																											// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,											// dwStyleEx
			LSN_LSTR( LSN_INPUT_GENERAL ),			// pwcText
			0,										// sTextLen
			LSN_IWI_PER_GAME_SETTINGS_PANEL,		// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			LSN_IWI_PER_GAME( CONSOLE_TYPE_LABEL ),	// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_FIRST_SECTION_L,					// iLeft
			LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),							// iTop
			LSN_FIRST_SECTION_W,					// dwWidth
			LSN_DEF_STATIC_HEIGHT,					// dwHeight
			LSN_STATICSTYLE,						// dwStyle
			0,										// dwStyleEx
			LSN_LSTR( LSN_INPUT_CONSOLE_TYPE_ ),	// pwcText
			0,										// sTextLen
			LSN_IWI_PER_GAME_SETTINGS_PANEL,		// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			LSN_IWI_PER_GAME( CONSOLE_TYPE_COMBO ),	// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_COMBO_L,							// iLeft
			LSN_GROUP_TOP,							// iTop
			LSN_COMBO_W,							// dwWidth
			LSN_DEF_COMBO_HEIGHT,					// dwHeight
			LSN_COMBOSTYLE_LIST,					// dwStyle
			LSN_COMBOSTYLEEX_LIST,					// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			LSN_IWI_PER_GAME_SETTINGS_PANEL,		// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			LSN_IWI_PER_GAME( USE_GLOBAL_CHECK ),	// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_THIRD_SECTION_L,					// iLeft
			LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1),							// iTop
			LSN_THIRD_SECTION_W,					// dwWidth
			LSN_DEF_CHECK_HEIGHT,					// dwHeight
			LSN_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			LSN_LSTR( LSN_INPUT_USE_GLOBAL ),		// pwcText
			0,										// sTextLen
			LSN_IWI_PER_GAME_SETTINGS_PANEL,		// dwParentId
		},


		// Settings.
		{
			LSW_LT_GROUPBOX,						// ltType
			LSN_IWI_PER_GAME( SETTINGS_GROUP ),		// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_LEFT_JUST,							// iLeft
			LSN_GENERAL_GROUP_H,					// iTop
			LSN_INPUT_GROUP_W,						// dwWidth
			LSN_SETTINGS_GROUP_H,					// dwHeight
			LSN_GROUPSTYLE,																											// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,											// dwStyleEx
			LSN_LSTR( LSN_INPUT_SETTINGS ),			// pwcText
			0,										// sTextLen
			LSN_IWI_PER_GAME_SETTINGS_PANEL,		// dwParentId
		},

		{
			LSW_LT_LABEL,							// ltType
			LSN_IWI_PER_GAME( PLAYER_1_LABEL ),		// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,			// iLeft
			LSN_GENERAL_GROUP_H + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),							// iTop
			LSN_FIRST_SECTION_W,					// dwWidth
			LSN_DEF_STATIC_HEIGHT,					// dwHeight
			LSN_STATICSTYLE,						// dwStyle
			0,										// dwStyleEx
			LSN_LSTR( LSN_INPUT_PLAYER_1_ ),		// pwcText
			0,										// sTextLen
			LSN_IWI_PER_GAME_SETTINGS_PANEL,		// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			LSN_IWI_PER_GAME( PLAYER_1_COMBO ),		// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_COMBO_L,							// iLeft
			LSN_GENERAL_GROUP_H + LSN_GROUP_TOP,	// iTop
			LSN_COMBO_W,							// dwWidth
			LSN_DEF_COMBO_HEIGHT,					// dwHeight
			LSN_COMBOSTYLE_LIST,					// dwStyle
			LSN_COMBOSTYLEEX_LIST,					// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			LSN_IWI_PER_GAME_SETTINGS_PANEL,		// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			LSN_IWI_PER_GAME( PLAYER_1_BUTTON ),		// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_THIRD_SECTION_L,					// iLeft
			LSN_GENERAL_GROUP_H + LSN_GROUP_TOP,	// iTop
			LSN_THIRD_SECTION_W,					// dwWidth
			LSN_DEF_BUTTON_HEIGHT,					// dwHeight
			LSN_BUTTONSTYLE,						// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																								// dwStyleEx
			LSN_LSTR( LSN_INPUT_CONFIGURE ),		// pwcText
			0,										// sTextLen
			LSN_IWI_PER_GAME_SETTINGS_PANEL,		// dwParentId
		},

		{
			LSW_LT_LABEL,							// ltType
			LSN_IWI_PER_GAME( PLAYER_2_LABEL ),		// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,			// iLeft
			LSN_GENERAL_GROUP_H + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 1,							// iTop
			LSN_FIRST_SECTION_W,					// dwWidth
			LSN_DEF_STATIC_HEIGHT,					// dwHeight
			LSN_STATICSTYLE,						// dwStyle
			0,										// dwStyleEx
			LSN_LSTR( LSN_INPUT_PLAYER_2_ ),		// pwcText
			0,										// sTextLen
			LSN_IWI_PER_GAME_SETTINGS_PANEL,		// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			LSN_IWI_PER_GAME( PLAYER_2_COMBO ),		// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_COMBO_L,							// iLeft
			LSN_GENERAL_GROUP_H + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 1,	// iTop
			LSN_COMBO_W,							// dwWidth
			LSN_DEF_COMBO_HEIGHT,					// dwHeight
			LSN_COMBOSTYLE_LIST,					// dwStyle
			LSN_COMBOSTYLEEX_LIST,					// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			LSN_IWI_PER_GAME_SETTINGS_PANEL,		// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			LSN_IWI_PER_GAME( PLAYER_2_BUTTON ),		// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_THIRD_SECTION_L,					// iLeft
			LSN_GENERAL_GROUP_H + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 1,	// iTop
			LSN_THIRD_SECTION_W,					// dwWidth
			LSN_DEF_BUTTON_HEIGHT,					// dwHeight
			LSN_BUTTONSTYLE,						// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																								// dwStyleEx
			LSN_LSTR( LSN_INPUT_CONFIGURE ),		// pwcText
			0,										// sTextLen
			LSN_IWI_PER_GAME_SETTINGS_PANEL,		// dwParentId
		},

		// Use Four Score.
		{
			LSW_LT_CHECK,							// ltType
			LSN_IWI_PER_GAME( USE_FOUR_SCORE_CHECK ),																														// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,			// iLeft
			LSN_GENERAL_GROUP_H + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 2,							// iTop
			LSN_INPUT_GROUP_W - LSN_GROUP_LEFT * 2,	// dwWidth
			LSN_DEF_CHECK_HEIGHT,					// dwHeight
			LSN_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			LSN_LSTR( LSN_INPUT_USE_4_SCORE ),		// pwcText
			0,										// sTextLen
			LSN_IWI_PER_GAME_SETTINGS_PANEL,		// dwParentId
		},

		// Expension Port.
		{
			LSW_LT_LABEL,							// ltType
			LSN_IWI_PER_GAME( EXPAND_LABEL ),		// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,			// iLeft
			LSN_GENERAL_GROUP_H + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 2,						// iTop
			LSN_FIRST_SECTION_W,					// dwWidth
			LSN_DEF_STATIC_HEIGHT,					// dwHeight
			LSN_STATICSTYLE,						// dwStyle
			0,										// dwStyleEx
			LSN_LSTR( LSN_INPUT_EXPANSION_PORT ),	// pwcText
			0,										// sTextLen
			LSN_IWI_PER_GAME_SETTINGS_PANEL,		// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			LSN_IWI_PER_GAME( EXPAND_COMBO ),			// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_COMBO_L,							// iLeft
			LSN_GENERAL_GROUP_H + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 2,	// iTop
			LSN_COMBO_W,							// dwWidth
			LSN_DEF_COMBO_HEIGHT,					// dwHeight
			LSN_COMBOSTYLE_LIST,					// dwStyle
			LSN_COMBOSTYLEEX_LIST,					// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			LSN_IWI_PER_GAME_SETTINGS_PANEL,		// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			LSN_IWI_PER_GAME( EXPAND_BUTTON ),		// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_THIRD_SECTION_L,					// iLeft
			LSN_GENERAL_GROUP_H + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 2,	// iTop
			LSN_THIRD_SECTION_W,					// dwWidth
			LSN_DEF_BUTTON_HEIGHT,					// dwHeight
			LSN_BUTTONSTYLE,						// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																								// dwStyleEx
			LSN_LSTR( LSN_INPUT_CONFIGURE ),		// pwcText
			0,										// sTextLen
			LSN_IWI_PER_GAME_SETTINGS_PANEL,		// dwParentId
		},

		// Player 3.
		{
			LSW_LT_LABEL,							// ltType
			LSN_IWI_PER_GAME( PLAYER_3_LABEL ),		// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,			// iLeft
			LSN_GENERAL_GROUP_H + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 3,							// iTop
			LSN_FIRST_SECTION_W,					// dwWidth
			LSN_DEF_STATIC_HEIGHT,					// dwHeight
			LSN_STATICSTYLE,						// dwStyle
			0,										// dwStyleEx
			LSN_LSTR( LSN_INPUT_PLAYER_3_ ),		// pwcText
			0,										// sTextLen
			LSN_IWI_PER_GAME_SETTINGS_PANEL,		// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			LSN_IWI_PER_GAME( PLAYER_3_COMBO ),		// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_COMBO_L,							// iLeft
			LSN_GENERAL_GROUP_H + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 3,	// iTop
			LSN_COMBO_W,							// dwWidth
			LSN_DEF_COMBO_HEIGHT,					// dwHeight
			LSN_COMBOSTYLE_LIST,					// dwStyle
			LSN_COMBOSTYLEEX_LIST,					// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			LSN_IWI_PER_GAME_SETTINGS_PANEL,		// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			LSN_IWI_PER_GAME( PLAYER_3_BUTTON ),		// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_THIRD_SECTION_L,					// iLeft
			LSN_GENERAL_GROUP_H + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 3,	// iTop
			LSN_THIRD_SECTION_W,					// dwWidth
			LSN_DEF_BUTTON_HEIGHT,					// dwHeight
			LSN_BUTTONSTYLE,						// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																								// dwStyleEx
			LSN_LSTR( LSN_INPUT_CONFIGURE ),		// pwcText
			0,										// sTextLen
			LSN_IWI_PER_GAME_SETTINGS_PANEL,		// dwParentId
		},

		// Player 4.
		{
			LSW_LT_LABEL,							// ltType
			LSN_IWI_PER_GAME( PLAYER_4_LABEL ),		// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,			// iLeft
			LSN_GENERAL_GROUP_H + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 4,							// iTop
			LSN_FIRST_SECTION_W,					// dwWidth
			LSN_DEF_STATIC_HEIGHT,					// dwHeight
			LSN_STATICSTYLE,						// dwStyle
			0,										// dwStyleEx
			LSN_LSTR( LSN_INPUT_PLAYER_4_ ),		// pwcText
			0,										// sTextLen
			LSN_IWI_PER_GAME_SETTINGS_PANEL,		// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			LSN_IWI_PER_GAME( PLAYER_4_COMBO ),		// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_COMBO_L,							// iLeft
			LSN_GENERAL_GROUP_H + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 4,	// iTop
			LSN_COMBO_W,							// dwWidth
			LSN_DEF_COMBO_HEIGHT,					// dwHeight
			LSN_COMBOSTYLE_LIST,					// dwStyle
			LSN_COMBOSTYLEEX_LIST,					// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			LSN_IWI_PER_GAME_SETTINGS_PANEL,		// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			LSN_IWI_PER_GAME( PLAYER_4_BUTTON ),	// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_THIRD_SECTION_L,					// iLeft
			LSN_GENERAL_GROUP_H + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 4,	// iTop
			LSN_THIRD_SECTION_W,					// dwWidth
			LSN_DEF_BUTTON_HEIGHT,					// dwHeight
			LSN_BUTTONSTYLE,						// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR /*| WS_EX_NOPARENTNOTIFY*/,																								// dwStyleEx
			LSN_LSTR( LSN_INPUT_CONFIGURE ),		// pwcText
			0,										// sTextLen
			LSN_IWI_PER_GAME_SETTINGS_PANEL,		// dwParentId
		},
	};

#undef LSN_INPUT_H
#undef LSN_INPUT_W
#undef LSN_INPUT_GROUP_W
#undef LSN_SETTINGS_GROUP_H
#undef LSN_GENERAL_GROUP_H

#undef LSN_THIRD_SECTION_L
#undef LSN_THIRD_SECTION_W
#undef LSN_COMBO_L
#undef LSN_COMBO_W
#undef LSN_FIRST_SECTION_L
#undef LSN_FIRST_SECTION_W


	// == Functions.
	/**
	 * Creates the input-configuration window.
	 *
	 * \param _pwParent The parent of the dialog.
	 * \param _oOptions A reference to the options object.
	 * \return Returns TRUE if the dialog was created successfully.
	 */
	BOOL CInputWindowLayout::CreateInputDialog( CWidget * _pwParent, LSN_OPTIONS &_oOptions ) {
		lsn::CLayoutManager * plmLayout = static_cast<lsn::CLayoutManager *>(lsw::CBase::LayoutManager());
		INT_PTR ipProc = plmLayout->DialogBoxX( m_wlInputWindow, LSN_ELEMENTS( m_wlInputWindow ), _pwParent, reinterpret_cast<uint64_t>(&_oOptions) );
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
	 * \param _oOptions A reference to the options object.
	 * \return Returns the created widget.
	 */
	CWidget * CInputWindowLayout::CreateGlobalPage( CWidget * _pwParent, LSN_OPTIONS &_oOptions ) {
		return CreatePage( _pwParent, m_wlGlobalPanel, LSN_ELEMENTS( m_wlGlobalPanel ), _oOptions );
	}

	/**
	 * Creates the per-game-settings page.
	 *
	 * \param _pwParent the parent of the page.
	 * \param _oOptions A reference to the options object.
	 * \return Returns the created widget.
	 */
	CWidget * CInputWindowLayout::CreatePerGamePage( CWidget * _pwParent, LSN_OPTIONS &_oOptions ) {
		return CreatePage( _pwParent, m_wlPerGamePanel, LSN_ELEMENTS( m_wlPerGamePanel ), _oOptions );
	}

	/**
	 * Creates the tab pages.
	 *
	 * \param _pwParent The parent widget.
	 * \param _pwlLayout The page layout.
	 * \param _sTotal The number of items to which _pwlLayout points.
	 * \param _oOptions A reference to the options object.
	 * \return Returns the created page.
	 */
	CWidget * CInputWindowLayout::CreatePage( CWidget * _pwParent, const LSW_WIDGET_LAYOUT * _pwlLayout, size_t _sTotal, LSN_OPTIONS &_oOptions ) {
		lsn::CLayoutManager * plmLayout = static_cast<lsn::CLayoutManager *>(lsw::CBase::LayoutManager());
		CWidget * pwWidget = plmLayout->CreateDialogX( _pwlLayout, _sTotal, _pwParent, reinterpret_cast<uint64_t>(&_oOptions) );
		if ( pwWidget ) {
			// Success.  Do stuff.
		}
		return pwWidget;
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

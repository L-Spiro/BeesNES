
#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A layout template for "pages" (self-contained borderless dialogs meant for embedding into other dialogs).
 */

#include "LSNAudioOptionsWindowLayout.h"
#include "../../Localization/LSNLocalization.h"
#include "../Layout/LSNLayoutMacros.h"
#include "../Layout/LSNLayoutManager.h"



namespace lsn {

#define LSN_GENERAL_LABEL0_W								40
#define LSN_GENERAL_COMBO0_L								((LSN_LEFT_JUST + LSN_GROUP_LEFT) + LSN_GENERAL_LABEL0_W)
#define LSN_GENERAL_COMBO0_W								150
#define LSN_GENERAL_CHECK0_L								(LSN_GENERAL_COMBO0_L + LSN_GENERAL_COMBO0_W + LSN_LEFT_JUST * 2)
#define LSN_GENERAL_CHECK0_W								50
#define LSN_GENERAL_TRACK_L									LSN_GENERAL_COMBO0_L
#define LSN_GENERAL_TRACK_W									LSN_GENERAL_COMBO0_W

#define LSN_AUDIO_GLOBAL_PAGE_W								(LSN_LEFT_JUST + LSN_LEFT_JUST + (LSN_GENERAL_CHECK0_L + LSN_GENERAL_CHECK0_W + LSN_LEFT_JUST) + LSN_LEFT_JUST + LSN_LEFT_JUST)
#define LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP					LSN_TOP_JUST
#define LSN_AUDIO_OPTIONS_GENERAL_GROUP_W					(LSN_AUDIO_GLOBAL_PAGE_W - (LSN_LEFT_JUST * 2))
#define LSN_AUDIO_OPTIONS_GENERAL_GROUP_H					(LSN_GROUP_TOP + LSN_GROUP_BOTTOM + ((LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 2) + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST) * 2)

#define LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP					(LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_AUDIO_OPTIONS_GENERAL_GROUP_H)
#define LSN_AUDIO_OPTIONS_CHAR_GROUP_W						LSN_AUDIO_OPTIONS_GENERAL_GROUP_W
#define LSN_AUDIO_OPTIONS_CHAR_GROUP_H						(LSN_GROUP_TOP + LSN_GROUP_BOTTOM + (LSN_DEF_COMBO_HEIGHT) + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 3 + (LSN_TOP_JUST * 2) + LSN_DEF_CHECK_HEIGHT + LSN_TOP_JUST)

#define LSN_INV_CHECK_( P )									LSN_EVEN_DIVIDE( LSN_AUDIO_OPTIONS_CHAR_GROUP_W - (LSN_LEFT_JUST + LSN_GROUP_LEFT) * 2, LSN_LEFT_JUST + LSN_GROUP_LEFT, 2, P )

#define LSN_AUDIO_GLOBAL_PAGE_H								(LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_AUDIO_OPTIONS_CHAR_GROUP_H + (LSN_TOP_JUST * 1))
	/** The layout for the global panel. */
	LSW_WIDGET_LAYOUT CAudioOptionsWindowLayout::m_wlGlobalPage[] {
		{
			LSN_LT_AUDIO_OPTIONS_GLOBAL_PAGE,				// ltType
			LSN_AOWI_PAGE_GLOBAL,							// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			0,												// iLeft
			0,												// iTop
			LSN_AUDIO_GLOBAL_PAGE_W,						// dwWidth
			LSN_AUDIO_GLOBAL_PAGE_H,						// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | DS_3DLOOK | DS_FIXEDSYS | DS_SETFONT | DS_CONTROL /*| WS_CLIPSIBLINGS*/,	// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_CONTROLPARENT,							// dwStyleEx
			LSN_LSTR( LSN_PATCH_PATCH_ROM ),				// pwcText
			0,												// sTextLen
			LSN_AOWI_NONE,									// dwParentId
		},

#if 1
		// == General
		{
			LSW_LT_GROUPBOX,								// ltType
			LSN_AOWI_PAGE_GENERAL_GROUP,					// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST,									// iLeft
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP,			// iTop
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_W,				// dwWidth
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_H,				// dwHeight
			LSN_GROUPSTYLE,																											// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,											// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_GENERAL ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId

			//LSN_LOCK_LEFT,									// pcLeftSizeExp
			//LSN_LOCK_RIGHT,									// pcRightSizeExp
			//LSN_LOCK_TOP,									// pcTopSizeExp
			//LSN_LOCK_BOTTOM,								// pcBottomSizeExp
			//nullptr, 0,										// pcWidthSizeExp
			//nullptr, 0,										// pcHeightSizeExp
		},

		// Device.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_GENERAL_DEVICE_LABEL,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),			// iTop
			LSN_GENERAL_LABEL0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_DEVICE ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},

		{
			LSW_LT_COMBOBOX,								// ltType
			LSN_AOWI_PAGE_GENERAL_DEVICE_COMBO,				// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_COMBO0_L,							// iLeft
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP,																	// iTop
			LSN_GENERAL_COMBO0_W,							// dwWidth
			LSN_DEF_COMBO_HEIGHT,							// dwHeight
			LSN_COMBOSTYLE_LIST,							// dwStyle
			LSN_COMBOSTYLEEX_LIST,							// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},
		{
			LSW_LT_CHECK,									// ltType
			LSN_AOWI_PAGE_GENERAL_ENABLE_CHECK,				// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1),				// iTop
			LSN_GENERAL_CHECK0_W,							// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_ENABLED ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},

		// Format.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_GENERAL_FORMAT_LABEL,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 1,				// iTop
			LSN_GENERAL_LABEL0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_FORMAT ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			LSN_AOWI_PAGE_GENERAL_FORMAT_COMBO,				// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_COMBO0_L,							// iLeft
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 1,											// iTop
			LSN_GENERAL_COMBO0_W,							// dwWidth
			LSN_DEF_COMBO_HEIGHT,							// dwHeight
			LSN_COMBOSTYLE_LIST,							// dwStyle
			LSN_COMBOSTYLEEX_LIST,							// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},
		{
			LSW_LT_CHECK,									// ltType
			LSN_AOWI_PAGE_GENERAL_DITHER_CHECK,				// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 1,				// iTop
			LSN_GENERAL_CHECK0_W,							// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_DITHER ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},

		// Volume.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_GENERAL_VOLUME_LABEL,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 2,				// iTop
			LSN_GENERAL_LABEL0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_VOLUME ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},
		{
			LSW_LT_TRACKBAR,								// ltType
			LSN_AOWI_PAGE_GENERAL_VOLUME_TRACKBAR,			// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_TRACK_L,							// iLeft
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 2,																		// iTop
			LSN_GENERAL_TRACK_W,							// dwWidth
			LSN_DEF_TRACKBAR_HEIGHT,						// dwHeight
			LSN_TRACKBAR_STYLE,								// dwStyle
			0,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			LSN_AOWI_PAGE_GENERAL_VOLUME_EDIT,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			TRUE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_EDIT_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 2,				// iTop
			LSN_GENERAL_CHECK0_W,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},

		// Background Volume.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_GENERAL_BG_VOL_LABEL,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 2 + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST),				// iTop
			LSN_GENERAL_LABEL0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_BG_VOL ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},
		{
			LSW_LT_TRACKBAR,								// ltType
			LSN_AOWI_PAGE_GENERAL_BG_VOL_TRACKBAR,			// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_TRACK_L,							// iLeft
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 2 + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST),																			// iTop
			LSN_GENERAL_TRACK_W,							// dwWidth
			LSN_DEF_TRACKBAR_HEIGHT,						// dwHeight
			LSN_TRACKBAR_STYLE,								// dwStyle
			0,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			LSN_AOWI_PAGE_GENERAL_BG_VOL_EDIT,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			TRUE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_EDIT_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 2 + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST),				// iTop
			LSN_GENERAL_CHECK0_W,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},


		// == Characteristics
		{
			LSW_LT_GROUPBOX,								// ltType
			LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_GROUP,	// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST,									// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP,				// iTop
			LSN_AUDIO_OPTIONS_CHAR_GROUP_W,					// dwWidth
			LSN_AUDIO_OPTIONS_CHAR_GROUP_H,					// dwHeight
			LSN_GROUPSTYLE,																											// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,											// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_AUDIO_CHARACTERISTICS ),																	// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId

			//LSN_LOCK_LEFT,									// pcLeftSizeExp
			//LSN_LOCK_RIGHT,									// pcRightSizeExp
			//LSN_LOCK_TOP,									// pcTopSizeExp
			//LSN_LOCK_BOTTOM,								// pcBottomSizeExp
			//nullptr, 0,										// pcWidthSizeExp
			//nullptr, 0,										// pcHeightSizeExp
		},
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_PRESETS_LABEL,																	// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + LSN_TOP_JUST + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),// iTop
			LSN_GENERAL_LABEL0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_PRESET ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_PRESETS_COMBO,																	// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_COMBO0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + LSN_TOP_JUST,														// iTop
			LSN_AUDIO_OPTIONS_CHAR_GROUP_W - LSN_GROUP_LEFT - LSN_LEFT_JUST - LSN_GENERAL_COMBO0_L,									// dwWidth
			LSN_DEF_COMBO_HEIGHT,							// dwHeight
			LSN_COMBOSTYLE_LIST,							// dwStyle
			LSN_COMBOSTYLEEX_LIST,							// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},

		// LPF.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_LPF_LABEL,// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + LSN_TOP_JUST + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2),													// iTop
			LSN_GENERAL_LABEL0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_LPF_HZ ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_LPF_EDIT,	// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			TRUE,											// bActive
			LSN_GENERAL_COMBO0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2),																														// iTop
			LSN_GENERAL_COMBO0_W,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},
		{
			LSW_LT_CHECK,									// ltType
			LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_LPF_CHECK,// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + LSN_TOP_JUST + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2),													// iTop
			LSN_GENERAL_CHECK0_W,							// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_ENABLED ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},

		// HPF 0.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_HPF0_LABEL,																																									// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + LSN_TOP_JUST + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 1,		// iTop
			LSN_GENERAL_LABEL0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_HPF_1_HZ ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_HPF0_EDIT,// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			TRUE,											// bActive
			LSN_GENERAL_COMBO0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 1,																		// iTop
			LSN_GENERAL_COMBO0_W,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},
		{
			LSW_LT_CHECK,									// ltType
			LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_HPF0_CHECK,																																									// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + LSN_TOP_JUST + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 1,	// iTop
			LSN_GENERAL_CHECK0_W,							// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_ENABLED ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},

		// HPF 1.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_HPF1_LABEL,																																									// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + LSN_TOP_JUST + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2,	// iTop
			LSN_GENERAL_LABEL0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_HPF_2_HZ ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_HPF1_EDIT,// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			TRUE,											// bActive
			LSN_GENERAL_COMBO0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2,																		// iTop
			LSN_GENERAL_COMBO0_W,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},
		{
			LSW_LT_CHECK,									// ltType
			LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_HPF1_CHECK,																																									// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + LSN_TOP_JUST + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2,	// iTop
			LSN_GENERAL_CHECK0_W,							// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_ENABLED ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},

		// HPF 2.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_HPF2_LABEL,																																									// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + LSN_TOP_JUST + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 3,	// iTop
			LSN_GENERAL_LABEL0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_HPF_3_HZ ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_HPF2_EDIT,// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			TRUE,											// bActive
			LSN_GENERAL_COMBO0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 3,																		// iTop
			LSN_GENERAL_COMBO0_W,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},
		{
			LSW_LT_CHECK,									// ltType
			LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_HPF2_CHECK,																																									// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + LSN_TOP_JUST + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 3,	// iTop
			LSN_GENERAL_CHECK0_W,							// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_ENABLED ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},

		// Noise/Invert
		{
			LSW_LT_CHECK,									// ltType
			LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_INVERT_CHECK,																																									// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INV_CHECK_( 0 ),							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + LSN_TOP_JUST + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4,															// iTop
			LSN_INV_CHECK_( 1 ) - LSN_INV_CHECK_( 0 ),		// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_INVERT ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},
		{
			LSW_LT_CHECK,									// ltType
			LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_NOISE_CHECK,																																									// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INV_CHECK_( 1 ),							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + LSN_TOP_JUST + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4,															// iTop
			LSN_INV_CHECK_( 2 ) - LSN_INV_CHECK_( 1 ),		// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_NOISE_MAINS_HUM ),	// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},
#endif	// #if 0
	};


	/** The layout for the per-game-setup panel. */
	LSW_WIDGET_LAYOUT CAudioOptionsWindowLayout::m_wlPerGamePage[] {
		{
			LSN_LT_AUDIO_OPTIONS_PERGAME_PAGE,				// ltType
			LSN_AOWI_PAGE_PERGAME,							// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			0,												// iLeft
			0,												// iTop
			LSN_AUDIO_GLOBAL_PAGE_W,						// dwWidth
			LSN_AUDIO_GLOBAL_PAGE_H,						// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | DS_3DLOOK | DS_FIXEDSYS | DS_SETFONT | DS_CONTROL /*| WS_CLIPSIBLINGS*/,	// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_CONTROLPARENT,							// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_NONE,									// dwParentId
		},
#if 1
		// == General
		{
			LSW_LT_GROUPBOX,								// ltType
			LSN_AOWI_PAGE_GENERAL_GROUP,					// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST,									// iLeft
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP,			// iTop
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_W,				// dwWidth
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_H,				// dwHeight
			LSN_GROUPSTYLE,																											// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,											// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_GENERAL ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId

			//LSN_LOCK_LEFT,									// pcLeftSizeExp
			//LSN_LOCK_RIGHT,									// pcRightSizeExp
			//LSN_LOCK_TOP,									// pcTopSizeExp
			//LSN_LOCK_BOTTOM,								// pcBottomSizeExp
			//nullptr, 0,										// pcWidthSizeExp
			//nullptr, 0,										// pcHeightSizeExp
		},
		
		// Device.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_GENERAL_DEVICE_LABEL,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),			// iTop
			LSN_GENERAL_LABEL0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_DEVICE ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			LSN_AOWI_PAGE_GENERAL_DEVICE_COMBO,				// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_COMBO0_L,							// iLeft
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP,																	// iTop
			LSN_GENERAL_COMBO0_W,							// dwWidth
			LSN_DEF_COMBO_HEIGHT,							// dwHeight
			LSN_COMBOSTYLE_LIST,							// dwStyle
			LSN_COMBOSTYLEEX_LIST,							// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},
		{
			LSW_LT_CHECK,									// ltType
			LSN_AOWI_PAGE_GENERAL_ENABLE_CHECK,				// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1),				// iTop
			50,												// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_ENABLED ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},

		// Format.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_GENERAL_FORMAT_LABEL,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 1,				// iTop
			LSN_GENERAL_LABEL0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_FORMAT ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			LSN_AOWI_PAGE_GENERAL_FORMAT_COMBO,				// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_COMBO0_L,							// iLeft
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 1,											// iTop
			LSN_GENERAL_COMBO0_W,							// dwWidth
			LSN_DEF_COMBO_HEIGHT,							// dwHeight
			LSN_COMBOSTYLE_LIST,							// dwStyle
			LSN_COMBOSTYLEEX_LIST,							// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},
		{
			LSW_LT_CHECK,									// ltType
			LSN_AOWI_PAGE_GENERAL_DITHER_CHECK,				// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 1,				// iTop
			LSN_GENERAL_CHECK0_W,							// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_DITHER ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},

		// Volume.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_GENERAL_VOLUME_LABEL,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 2,					// iTop
			LSN_GENERAL_LABEL0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_VOLUME ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},
		{
			LSW_LT_TRACKBAR,								// ltType
			LSN_AOWI_PAGE_GENERAL_VOLUME_TRACKBAR,			// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_TRACK_L,							// iLeft
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 2,																													// iTop
			LSN_GENERAL_TRACK_W,							// dwWidth
			LSN_DEF_TRACKBAR_HEIGHT,						// dwHeight
			LSN_TRACKBAR_STYLE,								// dwStyle
			0,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			LSN_AOWI_PAGE_GENERAL_VOLUME_EDIT,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			TRUE,											// bActive
			(LSN_GENERAL_COMBO0_L + ((LSN_GENERAL_CHECK0_L + LSN_GENERAL_CHECK0_W - 50) - LSN_GENERAL_COMBO0_L)),																	// iLeft
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_EDIT_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 2,				// iTop
			50,												// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},

		// Background Volume.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_GENERAL_BG_VOL_LABEL,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 2 + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST),				// iTop
			LSN_GENERAL_LABEL0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_BG_VOL ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},
		{
			LSW_LT_TRACKBAR,								// ltType
			LSN_AOWI_PAGE_GENERAL_BG_VOL_TRACKBAR,			// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_TRACK_L,							// iLeft
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 2 + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST),																			// iTop
			LSN_GENERAL_TRACK_W,							// dwWidth
			LSN_DEF_TRACKBAR_HEIGHT,						// dwHeight
			LSN_TRACKBAR_STYLE,								// dwStyle
			0,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			LSN_AOWI_PAGE_GENERAL_BG_VOL_EDIT,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			TRUE,											// bActive
			(LSN_GENERAL_COMBO0_L + ((LSN_GENERAL_CHECK0_L + LSN_GENERAL_CHECK0_W - 50) - LSN_GENERAL_COMBO0_L)),																												// iLeft
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_EDIT_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 2 + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST),				// iTop
			50,												// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},


		// == Characteristics
		{
			LSW_LT_GROUPBOX,								// ltType
			LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_GROUP,	// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST,									// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP,				// iTop
			LSN_AUDIO_OPTIONS_CHAR_GROUP_W,					// dwWidth
			LSN_AUDIO_OPTIONS_CHAR_GROUP_H,					// dwHeight
			LSN_GROUPSTYLE,																											// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,											// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_AUDIO_CHARACTERISTICS ),																	// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId

			//LSN_LOCK_LEFT,									// pcLeftSizeExp
			//LSN_LOCK_RIGHT,									// pcRightSizeExp
			//LSN_LOCK_TOP,									// pcTopSizeExp
			//LSN_LOCK_BOTTOM,								// pcBottomSizeExp
			//nullptr, 0,										// pcWidthSizeExp
			//nullptr, 0,										// pcHeightSizeExp
		},
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_PRESETS_LABEL,																	// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + LSN_TOP_JUST + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),// iTop
			LSN_GENERAL_LABEL0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_PRESET ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_PRESETS_COMBO,																	// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_COMBO0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + LSN_TOP_JUST,														// iTop
			LSN_AUDIO_OPTIONS_CHAR_GROUP_W - LSN_GROUP_LEFT - LSN_LEFT_JUST - LSN_GENERAL_COMBO0_L,									// dwWidth
			LSN_DEF_COMBO_HEIGHT,							// dwHeight
			LSN_COMBOSTYLE_LIST,							// dwStyle
			LSN_COMBOSTYLEEX_LIST,							// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},

		// LPF.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_LPF_LABEL,// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + LSN_TOP_JUST + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2),													// iTop
			LSN_GENERAL_LABEL0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_LPF_HZ ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_LPF_EDIT,	// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			TRUE,											// bActive
			LSN_GENERAL_COMBO0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2),																														// iTop
			LSN_GENERAL_COMBO0_W,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},
		{
			LSW_LT_CHECK,									// ltType
			LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_LPF_CHECK,// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + LSN_TOP_JUST + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2),													// iTop
			LSN_GENERAL_CHECK0_W,							// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_ENABLED ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},

		// HPF 0.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_HPF0_LABEL,																																									// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + LSN_TOP_JUST + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 1,		// iTop
			LSN_GENERAL_LABEL0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_HPF_1_HZ ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_HPF0_EDIT,// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			TRUE,											// bActive
			LSN_GENERAL_COMBO0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 1,																		// iTop
			LSN_GENERAL_COMBO0_W,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},
		{
			LSW_LT_CHECK,									// ltType
			LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_HPF0_CHECK,																																									// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + LSN_TOP_JUST + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 1,	// iTop
			LSN_GENERAL_CHECK0_W,							// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_ENABLED ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},

		// HPF 1.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_HPF1_LABEL,																																									// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + LSN_TOP_JUST + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2,	// iTop
			LSN_GENERAL_LABEL0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_HPF_2_HZ ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_HPF1_EDIT,// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			TRUE,											// bActive
			LSN_GENERAL_COMBO0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2,																		// iTop
			LSN_GENERAL_COMBO0_W,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},
		{
			LSW_LT_CHECK,									// ltType
			LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_HPF1_CHECK,																																									// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + LSN_TOP_JUST + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2,	// iTop
			LSN_GENERAL_CHECK0_W,							// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_ENABLED ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},

		// HPF 2.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_HPF2_LABEL,																																									// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + LSN_TOP_JUST + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 3,	// iTop
			LSN_GENERAL_LABEL0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_HPF_3_HZ ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_HPF2_EDIT,// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			TRUE,											// bActive
			LSN_GENERAL_COMBO0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 3,																		// iTop
			LSN_GENERAL_COMBO0_W,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},
		{
			LSW_LT_CHECK,									// ltType
			LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_HPF2_CHECK,																																									// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + LSN_TOP_JUST + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 3,	// iTop
			LSN_GENERAL_CHECK0_W,							// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_ENABLED ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},

		// Noise/Invert
		{
			LSW_LT_CHECK,									// ltType
			LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_INVERT_CHECK,																																									// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INV_CHECK_( 0 ),							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + LSN_TOP_JUST + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4,															// iTop
			LSN_INV_CHECK_( 1 ) - LSN_INV_CHECK_( 0 ),		// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_INVERT ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},
		{
			LSW_LT_CHECK,									// ltType
			LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_NOISE_CHECK,																																									// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INV_CHECK_( 1 ),							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + LSN_TOP_JUST + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4,															// iTop
			LSN_INV_CHECK_( 2 ) - LSN_INV_CHECK_( 1 ),		// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_NOISE_MAINS_HUM ),	// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},
#endif	// #if 0
	};



#define LSN_AUDIO_OPTIONS_W									LSN_AUDIO_GLOBAL_PAGE_W
#define LSN_AUDIO_OPTIONS_H									(LSN_AUDIO_GLOBAL_PAGE_H + LSN_DEF_BUTTON_HEIGHT + (LSN_TOP_JUST * 2))

	// == Members.
	/** The layout for the template window. */
	LSW_WIDGET_LAYOUT CAudioOptionsWindowLayout::m_wlWindow[] = {
		{
			LSN_LT_AUDIO_OPTIONS_DIALOG,					// ltType
			LSN_AOWI_MAINWINDOW,							// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			0,												// iLeft
			0,												// iTop
			LSN_AUDIO_OPTIONS_W,							// dwWidth
			LSN_AUDIO_OPTIONS_H,							// dwHeight
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | DS_3DLOOK | DS_FIXEDSYS | DS_MODALFRAME | DS_CENTER,					// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_ ),					// pwcText
			0,												// sTextLen
			LSN_AOWI_NONE,									// dwParentId

			//LSN_PARENT_VCLEFT,								// pcLeftSizeExp
			//nullptr, 0,										// pcRightSizeExp
			//LSN_PARENT_VCTOP,								// pcTopSizeExp
			//nullptr, 0,										// pcBottomSizeExp
			//LSN_FIXED_WIDTH,								// pcWidthSizeExp
			//LSN_FIXED_HEIGHT,								// pcHeightSizeExp
		},

		{
			LSW_LT_TAB,										// ltType
			LSN_AOWI_TAB,									// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			0,												// iLeft
			0,												// iTop
			LSN_AUDIO_OPTIONS_W,							// dwWidth
			LSN_AUDIO_OPTIONS_H - LSN_DEF_BUTTON_HEIGHT - (LSN_TOP_JUST * 2),		// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | TCS_HOTTRACK,				// dwStyle
			WS_EX_ACCEPTFILES,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_MAINWINDOW,							// dwParentId

			//LSN_LOCK_LEFT,									// pcLeftSizeExp
			//LSN_LOCK_RIGHT,									// pcRightSizeExp
			//LSN_LOCK_TOP,									// pcTopSizeExp
			//LSN_LOCK_BOTTOM,								// pcBottomSizeExp
			//nullptr, 0,										// pcWidthSizeExp
			//nullptr, 0,										// pcHeightSizeExp
		},


		{
			LSW_LT_BUTTON,									// ltType
			LSN_AOWI_OK,									// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			TRUE,											// bActive
			LSN_LEFT_JUST,									// iLeft
			LSN_AUDIO_OPTIONS_H - LSN_DEF_BUTTON_HEIGHT - LSN_TOP_JUST,		// iTop
			LSN_DEF_BUTTON_WIDTH,							// dwWidth
			LSN_DEF_BUTTON_HEIGHT,							// dwHeight
			LSN_DEFBUTTONSTYLE,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																								// dwStyleEx
			LSN_LSTR( LSN_OK ),								// pwcText
			0,												// sTextLen
			LSN_AOWI_MAINWINDOW,							// dwParentId

			LSN_LOCK_LEFT,									// pcLeftSizeExp
			nullptr, 0,										// pcRightSizeExp
			nullptr, 0,										// pcTopSizeExp
			LSN_LOCK_BOTTOM,								// pcBottomSizeExp
			LSN_FIXED_WIDTH,								// pcWidthSizeExp
			LSN_FIXED_HEIGHT,								// pcHeightSizeExp
		},
		{
			LSW_LT_BUTTON,									// ltType
			LSN_AOWI_CANCEL,								// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_AUDIO_OPTIONS_W - LSN_DEF_BUTTON_WIDTH * 1 - LSN_LEFT_JUST,	// iLeft
			LSN_AUDIO_OPTIONS_H - LSN_DEF_BUTTON_HEIGHT	- LSN_TOP_JUST,		// iTop
			LSN_DEF_BUTTON_WIDTH,							// dwWidth
			LSN_DEF_BUTTON_HEIGHT,							// dwHeight
			LSN_BUTTONSTYLE,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																								// dwStyleEx
			LSN_LSTR( LSN_CANCEL ),							// pwcText
			0,												// sTextLen
			LSN_AOWI_MAINWINDOW,							// dwParentId

			nullptr, 0,										// pcLeftSizeExp
			LSN_LOCK_RIGHT,									// pcRightSizeExp
			nullptr, 0,										// pcTopSizeExp
			LSN_LOCK_BOTTOM,								// pcBottomSizeExp
			LSN_FIXED_WIDTH,								// pcWidthSizeExp
			LSN_FIXED_HEIGHT,								// pcHeightSizeExp
		},
	};

#undef LSN_AUDIO_OPTIONS_PAGE_W
#undef LSN_AUDIO_OPTIONS_PAGE_H
#undef LSN_AUDIO_OPTIONS_GENERAL_GROUP_H
#undef LSN_AUDIO_GLOBAL_PAGE_H
#undef LSN_AUDIO_GLOBAL_PAGE_W

#undef LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP
#undef LSN_AUDIO_OPTIONS_GENERAL_GROUP_W

#undef LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP
#undef LSN_GENERAL_CHECK0_W
#undef LSN_GENERAL_CHECK0_L
#undef LSN_GENERAL_COMBO0_W
#undef LSN_GENERAL_COMBO0_L
#undef LSN_GENERAL_LABEL0_W

	// == Functions.
	/**
	 * Creates the page.
	 *
	 * \param _pwParent the parent of the page.
	 * \param _oOptions A reference to the options object.
	 * \return Returns TRUE if the dialog was created successfully.
	 */
	BOOL CAudioOptionsWindowLayout::CreateAudioOptionsDialog( CWidget * _pwParent, LSN_OPTIONS &_oOptions ) {
		lsn::CLayoutManager * plmLayout = static_cast<lsn::CLayoutManager *>(lsw::CBase::LayoutManager());
		/*CInputWindow::LSN_CONTROLLER_SETUP_DATA csdData = {
			.poOptions = &_oOptions,
			.pmwMainWindow = _pmwMainWindow,
		};*/
		INT_PTR ipProc = plmLayout->DialogBoxX( m_wlWindow, LSN_ELEMENTS( m_wlWindow ), _pwParent, reinterpret_cast<uint64_t>(&_oOptions) );
		if ( ipProc != 0 ) {
			// Success.  Do stuff.
			return TRUE;
		}
		return FALSE;
	}

	/**
	 * Creates the global page.
	 *
	 * \param _pwParent The parent widget.
	 * \return Returns the created page.
	 */
	CWidget * CAudioOptionsWindowLayout::CreateGlobalPage( CWidget * _pwParent, LSN_OPTIONS &_oOptions ) {
		return CreatePage( _pwParent, m_wlGlobalPage, LSN_ELEMENTS( m_wlGlobalPage ), _oOptions );
	}

	/**
	 * Creates the per-game page.
	 *
	 * \param _pwParent The parent widget.
	 * \return Returns the created page.
	 */
	CWidget * CAudioOptionsWindowLayout::CreatePerGamePage( CWidget * _pwParent, LSN_OPTIONS &_oOptions ) {
		return CreatePage( _pwParent, m_wlPerGamePage, LSN_ELEMENTS( m_wlPerGamePage ), _oOptions );
	}

	/**
	 * Creates the tab pages.
	 *
	 * \param _pwParent The parent widget.
	 * \param _pwlLayout The page layout.
	 * \param _sTotal The number of items to which _pwlLayout points.
	 * \return Returns the created page.
	 */
	CWidget * CAudioOptionsWindowLayout::CreatePage( CWidget * _pwParent, const LSW_WIDGET_LAYOUT * _pwlLayout, size_t _sTotal, LSN_OPTIONS &_oOptions ) {
		lsn::CLayoutManager * plmLayout = static_cast<lsn::CLayoutManager *>(lsw::CBase::LayoutManager());
		CWidget * pwWidget = plmLayout->CreateDialogX( _pwlLayout, _sTotal, _pwParent, reinterpret_cast<uint64_t>(&_oOptions) );
		if ( pwWidget ) {
			// Success.  Do stuff.
		}
		return pwWidget;
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

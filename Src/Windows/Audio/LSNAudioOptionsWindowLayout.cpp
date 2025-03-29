
#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2025
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
#define LSN_GENERAL_COMBO0_W								200
#define LSN_GENERAL_CHECK0_L								(LSN_GENERAL_COMBO0_L + LSN_GENERAL_COMBO0_W + LSN_LEFT_JUST * 2)
#define LSN_GENERAL_CHECK0_W								50
#define LSN_GENERAL_TRACK_L									LSN_GENERAL_COMBO0_L
#define LSN_GENERAL_TRACK_W									LSN_GENERAL_COMBO0_W

#define LSN_META_LABEL0_W									(40 - LSN_GROUP_LEFT)
#define LSN_META_COMBO0_L									((LSN_LEFT_JUST + LSN_GROUP_LEFT) + LSN_GENERAL_LABEL0_W)
#define LSN_META_COMBO0_W									150
#define LSN_META_CHECK0_L									(LSN_GENERAL_COMBO0_L + LSN_GENERAL_COMBO0_W + LSN_LEFT_JUST * 2)
#define LSN_META_CHECK0_W									(50 - LSN_GROUP_LEFT)




#define LSN_AUDIO_GLOBAL_PAGE_W								(LSN_LEFT_JUST + LSN_LEFT_JUST + (LSN_GENERAL_CHECK0_L + LSN_GENERAL_CHECK0_W + LSN_LEFT_JUST))
#define LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP					LSN_TOP_JUST
#define LSN_AUDIO_OPTIONS_GENERAL_GROUP_W					(LSN_AUDIO_GLOBAL_PAGE_W - (LSN_LEFT_JUST * 2))
#define LSN_AUDIO_OPTIONS_GENERAL_GROUP_H					(LSN_GROUP_TOP + LSN_GROUP_BOTTOM + ((LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 2) + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST) * 2)

#define LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP					(LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_AUDIO_OPTIONS_GENERAL_GROUP_H)
#define LSN_AUDIO_OPTIONS_CHAR_GROUP_W						LSN_AUDIO_OPTIONS_GENERAL_GROUP_W
#define LSN_AUDIO_OPTIONS_CHAR_GROUP_H						(LSN_GROUP_TOP + LSN_GROUP_BOTTOM + (LSN_DEF_COMBO_HEIGHT) + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 3 + (LSN_TOP_JUST * 2) + LSN_DEF_CHECK_HEIGHT + LSN_TOP_JUST + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST) * 6)

#define LSN_AUDIO_OPTIONS_RAW_GROUP_TOP						LSN_TOP_JUST
#define LSN_META_RAW_GROUP_T								(LSN_AUDIO_OPTIONS_RAW_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2 + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 4 + LSN_TOP_JUST)
#define LSN_META_RAW_GROUP_H								(LSN_GROUP_TOP + LSN_GROUP_BOTTOM + LSN_DEF_COMBO_HEIGHT + ((LSN_DEF_CHECK_HEIGHT + LSN_TOP_JUST) * 3))
#define LSN_AUDIO_OPTIONS_RAW_GROUP_W						(LSN_AUDIO_GLOBAL_PAGE_W - (LSN_LEFT_JUST * 2))
#define LSN_AUDIO_OPTIONS_RAW_GROUP_H						(LSN_GROUP_TOP + LSN_GROUP_BOTTOM + (LSN_TOP_JUST + LSN_DEF_EDIT_HEIGHT) * 2 + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 4 + LSN_TOP_JUST + LSN_META_RAW_GROUP_H)

#define LSN_AUDIO_OPTIONS_OUT_GROUP_TOP						(LSN_AUDIO_OPTIONS_RAW_GROUP_TOP + LSN_AUDIO_OPTIONS_RAW_GROUP_H)
#define LSN_META_OUT_GROUP_T								(LSN_AUDIO_OPTIONS_OUT_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2 + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 4 +					\
																(/*LSN_DEF_STATIC_HEIGHT * 2 +*/ LSN_TOP_JUST))
#define LSN_META_OUT_GROUP_H								(LSN_GROUP_TOP + LSN_GROUP_BOTTOM + LSN_DEF_COMBO_HEIGHT + ((LSN_DEF_CHECK_HEIGHT + LSN_TOP_JUST) * 3))
#define LSN_AUDIO_OPTIONS_OUT_GROUP_H						(LSN_GROUP_TOP + LSN_GROUP_BOTTOM + (LSN_TOP_JUST + LSN_DEF_EDIT_HEIGHT) * 2 + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 4 /*+ (LSN_DEF_STATIC_HEIGHT * 2) + LSN_TOP_JUST + LSN_META_OUT_GROUP_H*/)


#define LSN_META_GROUP_W									(LSN_AUDIO_OPTIONS_GENERAL_GROUP_W - LSN_GROUP_LEFT * 2)

#define LSN_INV_CHECK_( P )									LSN_EVEN_DIVIDE( LSN_AUDIO_OPTIONS_CHAR_GROUP_W - (LSN_GROUP_LEFT * 2), LSN_LEFT_JUST + LSN_GROUP_LEFT, 3, P )
#define LSN_CONDITION_L( P )								LSN_EVEN_DIVIDE_EX( LSN_AUDIO_OPTIONS_RAW_GROUP_W - (LSN_GROUP_LEFT * 2), LSN_LEFT_JUST + LSN_GROUP_LEFT, 4, P, LSN_LEFT_JUST )
#define LSN_CONDITION_W( P )								LSN_EVEN_DIVIDE_WIDTH_EX( LSN_AUDIO_OPTIONS_RAW_GROUP_W - (LSN_GROUP_LEFT * 2), LSN_LEFT_JUST + LSN_GROUP_LEFT, 4, P, LSN_LEFT_JUST )
#define LSN_CONDITION2_W( P )								LSN_EVEN_DIVIDE_WIDTH_EX( LSN_AUDIO_OPTIONS_RAW_GROUP_W - (LSN_GROUP_LEFT * 2), LSN_LEFT_JUST + LSN_GROUP_LEFT, 2, P, LSN_LEFT_JUST )

#define LSN_META_CHECK_L( P )								LSN_EVEN_DIVIDE_EX( LSN_META_GROUP_W - (LSN_GROUP_LEFT * 2), LSN_LEFT_JUST + LSN_GROUP_LEFT * 2, 3, P, LSN_LEFT_JUST )
#define LSN_META_CHECK_W( P )								LSN_EVEN_DIVIDE_WIDTH_EX( LSN_META_GROUP_W - (LSN_GROUP_LEFT * 2), LSN_LEFT_JUST + LSN_GROUP_LEFT * 2, 3, P, LSN_LEFT_JUST )

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
			WS_CHILDWINDOW | WS_VISIBLE | DS_3DLOOK | DS_FIXEDSYS | DS_SETFONT | DS_CONTROL,																																	// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_CONTROLPARENT,																																			// dwStyleEx
			LSN_LSTR( LSN_PATCH_PATCH_ROM ),				// pwcText
			0,												// sTextLen
			LSN_AOWI_NONE,									// dwParentId
		},

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
			LSN_GROUPSTYLE,									// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																																		// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_GENERAL ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},
		
		// Device.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_GENERAL_DEVICE_LABEL,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),																										// iTop
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
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP,																																								// iTop
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
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1),																											// iTop
			50,												// dwWidth
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
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 1,															// iTop
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
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 1,																													// iTop
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
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 1,																// iTop
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
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 2,															// iTop
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
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 2,																													// iTop
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
			FALSE,											// bActive
			(LSN_GENERAL_COMBO0_L + ((LSN_GENERAL_CHECK0_L + LSN_GENERAL_CHECK0_W - 50) - LSN_GENERAL_COMBO0_L)),																												// iLeft
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_EDIT_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 2,															// iTop
			50,												// dwWidth
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
			FALSE,											// bActive
			(LSN_GENERAL_COMBO0_L + ((LSN_GENERAL_CHECK0_L + LSN_GENERAL_CHECK0_W - 50) - LSN_GENERAL_COMBO0_L)),																												// iLeft
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_EDIT_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 2 + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST),				// iTop
			50,												// dwWidth
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
			LSN_AOWI_PAGE_CHARACTERISTICS_GROUP,			// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST,									// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP,				// iTop
			LSN_AUDIO_OPTIONS_CHAR_GROUP_W,					// dwWidth
			LSN_AUDIO_OPTIONS_CHAR_GROUP_H,					// dwHeight
			LSN_GROUPSTYLE,									// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,											// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_AUDIO_CHARACTERISTICS ),																	// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_CHARACTERISTICS_PRESETS_LABEL,	// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),				// iTop
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
			LSN_AOWI_PAGE_CHARACTERISTICS_PRESETS_COMBO,	// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_COMBO0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP,																		// iTop
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
			LSN_AOWI_PAGE_CHARACTERISTICS_LPF_LABEL,		// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2),																// iTop
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
			LSN_AOWI_PAGE_CHARACTERISTICS_LPF_EDIT,			// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
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
			LSN_AOWI_PAGE_CHARACTERISTICS_LPF_CHECK,		// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2),																	// iTop
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
			LSN_AOWI_PAGE_CHARACTERISTICS_HPF0_LABEL,		// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 1,						// iTop
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
			LSN_AOWI_PAGE_CHARACTERISTICS_HPF0_EDIT,		// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
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
			LSN_AOWI_PAGE_CHARACTERISTICS_HPF0_CHECK,		// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 1,					// iTop
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
			LSN_AOWI_PAGE_CHARACTERISTICS_HPF1_LABEL,		// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2,					// iTop
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
			LSN_AOWI_PAGE_CHARACTERISTICS_HPF1_EDIT,		// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
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
			LSN_AOWI_PAGE_CHARACTERISTICS_HPF1_CHECK,		// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2,					// iTop
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
			LSN_AOWI_PAGE_CHARACTERISTICS_HPF2_LABEL,		// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 3,					// iTop
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
			LSN_AOWI_PAGE_CHARACTERISTICS_HPF2_EDIT,		// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
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
			LSN_AOWI_PAGE_CHARACTERISTICS_HPF2_CHECK,		// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 3,					// iTop
			LSN_GENERAL_CHECK0_W,							// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_ENABLED ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},

		// Volume.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_CHARACTERISTICS_VOLUME_LABEL,		// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),				// iTop
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
			LSN_AOWI_PAGE_CHARACTERISTICS_VOLUME_TRACKBAR,	// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_TRACK_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4,																		// iTop
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
			LSN_AOWI_PAGE_CHARACTERISTICS_VOLUME_EDIT,		// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_EDIT_HEIGHT) >> 1),				// iTop
			LSN_GENERAL_CHECK0_W,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},

		// Pulse-1 Volume.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_CHARACTERISTICS_PULSE1_VOLUME_LABEL,																																									// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST) * 1 + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),				// iTop
			LSN_GENERAL_LABEL0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_P1_VOLUME ),		// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},
		{
			LSW_LT_TRACKBAR,								// ltType
			LSN_AOWI_PAGE_CHARACTERISTICS_PULSE1_VOLUME_TRACKBAR,																																								// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_TRACK_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST) * 1,							// iTop
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
			LSN_AOWI_PAGE_CHARACTERISTICS_PULSE1_VOLUME_EDIT,																																									// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST) * 1 + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_EDIT_HEIGHT) >> 1),				// iTop
			LSN_GENERAL_CHECK0_W,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},

		// Pulse-2 Volume.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_CHARACTERISTICS_PULSE2_VOLUME_LABEL,																																									// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST) * 2 + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),				// iTop
			LSN_GENERAL_LABEL0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_P2_VOLUME ),		// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},
		{
			LSW_LT_TRACKBAR,								// ltType
			LSN_AOWI_PAGE_CHARACTERISTICS_PULSE2_VOLUME_TRACKBAR,																																								// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_TRACK_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST) * 2,							// iTop
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
			LSN_AOWI_PAGE_CHARACTERISTICS_PULSE2_VOLUME_EDIT,																																									// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST) * 2 + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_EDIT_HEIGHT) >> 1),				// iTop
			LSN_GENERAL_CHECK0_W,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},

		// Triangle Volume.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_CHARACTERISTICS_TRIANGLE_VOLUME_LABEL,																																								// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST) * 3 + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),				// iTop
			LSN_GENERAL_LABEL0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_TRI_VOLUME ),		// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},
		{
			LSW_LT_TRACKBAR,								// ltType
			LSN_AOWI_PAGE_CHARACTERISTICS_TRIANGLE_VOLUME_TRACKBAR,																																								// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_TRACK_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST) * 3,							// iTop
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
			LSN_AOWI_PAGE_CHARACTERISTICS_TRIANGLE_VOLUME_EDIT,																																									// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST) * 3 + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_EDIT_HEIGHT) >> 1),				// iTop
			LSN_GENERAL_CHECK0_W,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},

		// Noise Volume.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_CHARACTERISTICS_NOISE_VOLUME_LABEL,																																									// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST) * 4 + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),				// iTop
			LSN_GENERAL_LABEL0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_NOISE_VOLUME ),		// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},
		{
			LSW_LT_TRACKBAR,								// ltType
			LSN_AOWI_PAGE_CHARACTERISTICS_NOISE_VOLUME_TRACKBAR,																																								// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_TRACK_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST) * 4,							// iTop
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
			LSN_AOWI_PAGE_CHARACTERISTICS_NOISE_VOLUME_EDIT,// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST) * 4 + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_EDIT_HEIGHT) >> 1),				// iTop
			LSN_GENERAL_CHECK0_W,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},

		// DMC Volume.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_CHARACTERISTICS_DMC_VOLUME_LABEL,	// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST) * 5 + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),				// iTop
			LSN_GENERAL_LABEL0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_DMC_VOLUME ),		// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},
		{
			LSW_LT_TRACKBAR,								// ltType
			LSN_AOWI_PAGE_CHARACTERISTICS_DMC_VOLUME_TRACKBAR,																																									// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_TRACK_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST) * 5,							// iTop
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
			LSN_AOWI_PAGE_CHARACTERISTICS_DMC_VOLUME_EDIT,	// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST) * 5 + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_EDIT_HEIGHT) >> 1),				// iTop
			LSN_GENERAL_CHECK0_W,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},

		// Noise/Invert
		{
			LSW_LT_CHECK,									// ltType
			LSN_AOWI_PAGE_CHARACTERISTICS_INVERT_CHECK,		// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INV_CHECK_( 0 ),							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 8) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + LSN_DEF_TRACKBAR_HEIGHT * 6,											// iTop
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
			LSN_AOWI_PAGE_CHARACTERISTICS_NOISE_CHECK,		// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INV_CHECK_( 1 ),							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 8) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + LSN_DEF_TRACKBAR_HEIGHT * 6,											// iTop
			LSN_INV_CHECK_( 2 ) - LSN_INV_CHECK_( 1 ),		// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_NOISE_MAINS_HUM ),	// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},
		{
			LSW_LT_CHECK,									// ltType
			LSN_AOWI_PAGE_CHARACTERISTICS_OLD_NES_CHECK,	// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INV_CHECK_( 2 ),							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 8) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + LSN_DEF_TRACKBAR_HEIGHT * 6,											// iTop
			LSN_INV_CHECK_( 3 ) - LSN_INV_CHECK_( 2 ),		// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_RP2A03_4B2_40 ),	// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_GLOBAL,							// dwParentId
		},
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
			WS_CHILDWINDOW | WS_VISIBLE | DS_3DLOOK | DS_FIXEDSYS | DS_SETFONT | DS_CONTROL,																																	// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_CONTROLPARENT,																																			// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_NONE,									// dwParentId
		},

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
			LSN_GROUPSTYLE,									// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																																		// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_GENERAL ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},
		
		// Device.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_GENERAL_DEVICE_LABEL,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),																										// iTop
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
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP,																																								// iTop
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
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1),																											// iTop
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
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 1,															// iTop
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
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 1,																													// iTop
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
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 1,																// iTop
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
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 2,															// iTop
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
			FALSE,											// bActive
			(LSN_GENERAL_COMBO0_L + ((LSN_GENERAL_CHECK0_L + LSN_GENERAL_CHECK0_W - 50) - LSN_GENERAL_COMBO0_L)),																												// iLeft
			LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_EDIT_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 2,															// iTop
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
			FALSE,											// bActive
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
			LSN_AOWI_PAGE_CHARACTERISTICS_GROUP,			// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST,									// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP,				// iTop
			LSN_AUDIO_OPTIONS_CHAR_GROUP_W,					// dwWidth
			LSN_AUDIO_OPTIONS_CHAR_GROUP_H,					// dwHeight
			LSN_GROUPSTYLE,									// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,											// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_AUDIO_CHARACTERISTICS ),																	// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_CHARACTERISTICS_PRESETS_LABEL,	// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),				// iTop
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
			LSN_AOWI_PAGE_CHARACTERISTICS_PRESETS_COMBO,	// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_COMBO0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP,																		// iTop
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
			LSN_AOWI_PAGE_CHARACTERISTICS_LPF_LABEL,		// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2),																// iTop
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
			LSN_AOWI_PAGE_CHARACTERISTICS_LPF_EDIT,			// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
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
			LSN_AOWI_PAGE_CHARACTERISTICS_LPF_CHECK,		// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2),																	// iTop
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
			LSN_AOWI_PAGE_CHARACTERISTICS_HPF0_LABEL,		// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 1,						// iTop
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
			LSN_AOWI_PAGE_CHARACTERISTICS_HPF0_EDIT,		// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
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
			LSN_AOWI_PAGE_CHARACTERISTICS_HPF0_CHECK,		// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 1,					// iTop
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
			LSN_AOWI_PAGE_CHARACTERISTICS_HPF1_LABEL,		// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2,					// iTop
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
			LSN_AOWI_PAGE_CHARACTERISTICS_HPF1_EDIT,		// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
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
			LSN_AOWI_PAGE_CHARACTERISTICS_HPF1_CHECK,		// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2,					// iTop
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
			LSN_AOWI_PAGE_CHARACTERISTICS_HPF2_LABEL,		// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 3,					// iTop
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
			LSN_AOWI_PAGE_CHARACTERISTICS_HPF2_EDIT,		// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
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
			LSN_AOWI_PAGE_CHARACTERISTICS_HPF2_CHECK,		// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 3,					// iTop
			LSN_GENERAL_CHECK0_W,							// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_ENABLED ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},

		// Volume.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_CHARACTERISTICS_VOLUME_LABEL,		// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),				// iTop
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
			LSN_AOWI_PAGE_CHARACTERISTICS_VOLUME_TRACKBAR,	// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_TRACK_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4,																		// iTop
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
			LSN_AOWI_PAGE_CHARACTERISTICS_VOLUME_EDIT,		// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_EDIT_HEIGHT) >> 1),				// iTop
			LSN_GENERAL_CHECK0_W,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},

		// Pulse-1 Volume.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_CHARACTERISTICS_PULSE1_VOLUME_LABEL,																																									// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST) * 1 + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),				// iTop
			LSN_GENERAL_LABEL0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_P1_VOLUME ),		// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},
		{
			LSW_LT_TRACKBAR,								// ltType
			LSN_AOWI_PAGE_CHARACTERISTICS_PULSE1_VOLUME_TRACKBAR,																																								// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_TRACK_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST) * 1,							// iTop
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
			LSN_AOWI_PAGE_CHARACTERISTICS_PULSE1_VOLUME_EDIT,																																									// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST) * 1 + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_EDIT_HEIGHT) >> 1),				// iTop
			LSN_GENERAL_CHECK0_W,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},

		// Pulse-2 Volume.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_CHARACTERISTICS_PULSE2_VOLUME_LABEL,																																									// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST) * 2 + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),				// iTop
			LSN_GENERAL_LABEL0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_P2_VOLUME ),		// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},
		{
			LSW_LT_TRACKBAR,								// ltType
			LSN_AOWI_PAGE_CHARACTERISTICS_PULSE2_VOLUME_TRACKBAR,																																								// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_TRACK_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST) * 2,							// iTop
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
			LSN_AOWI_PAGE_CHARACTERISTICS_PULSE2_VOLUME_EDIT,																																									// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST) * 2 + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_EDIT_HEIGHT) >> 1),				// iTop
			LSN_GENERAL_CHECK0_W,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},

		// Triangle Volume.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_CHARACTERISTICS_TRIANGLE_VOLUME_LABEL,																																								// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST) * 3 + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),				// iTop
			LSN_GENERAL_LABEL0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_TRI_VOLUME ),		// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},
		{
			LSW_LT_TRACKBAR,								// ltType
			LSN_AOWI_PAGE_CHARACTERISTICS_TRIANGLE_VOLUME_TRACKBAR,																																								// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_TRACK_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST) * 3,							// iTop
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
			LSN_AOWI_PAGE_CHARACTERISTICS_TRIANGLE_VOLUME_EDIT,																																									// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST) * 3 + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_EDIT_HEIGHT) >> 1),				// iTop
			LSN_GENERAL_CHECK0_W,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},

		// Noise Volume.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_CHARACTERISTICS_NOISE_VOLUME_LABEL,																																									// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST) * 4 + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),				// iTop
			LSN_GENERAL_LABEL0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_NOISE_VOLUME ),		// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},
		{
			LSW_LT_TRACKBAR,								// ltType
			LSN_AOWI_PAGE_CHARACTERISTICS_NOISE_VOLUME_TRACKBAR,																																								// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_TRACK_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST) * 4,							// iTop
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
			LSN_AOWI_PAGE_CHARACTERISTICS_NOISE_VOLUME_EDIT,																																									// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST) * 4 + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_EDIT_HEIGHT) >> 1),				// iTop
			LSN_GENERAL_CHECK0_W,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},

		// DMC Volume.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_CHARACTERISTICS_DMC_VOLUME_LABEL,	// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST) * 5 + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),				// iTop
			LSN_GENERAL_LABEL0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_DMC_VOLUME ),		// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},
		{
			LSW_LT_TRACKBAR,								// ltType
			LSN_AOWI_PAGE_CHARACTERISTICS_DMC_VOLUME_TRACKBAR,																																									// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_TRACK_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST) * 5,							// iTop
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
			LSN_AOWI_PAGE_CHARACTERISTICS_DMC_VOLUME_EDIT,	// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 2) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + (LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST) * 5 + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_EDIT_HEIGHT) >> 1),				// iTop
			LSN_GENERAL_CHECK0_W,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},

		// Noise/Invert
		{
			LSW_LT_CHECK,									// ltType
			LSN_AOWI_PAGE_CHARACTERISTICS_INVERT_CHECK,		// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INV_CHECK_( 0 ),							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 8) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + LSN_DEF_TRACKBAR_HEIGHT * 6,											// iTop
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
			LSN_AOWI_PAGE_CHARACTERISTICS_NOISE_CHECK,		// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INV_CHECK_( 1 ),							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 8) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + LSN_DEF_TRACKBAR_HEIGHT * 6,											// iTop
			LSN_INV_CHECK_( 2 ) - LSN_INV_CHECK_( 1 ),		// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_NOISE_MAINS_HUM ),	// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},
		{
			LSW_LT_CHECK,									// ltType
			LSN_AOWI_PAGE_CHARACTERISTICS_OLD_NES_CHECK,	// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INV_CHECK_( 2 ),							// iLeft
			LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST * 8) * 1 + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 4 + LSN_DEF_TRACKBAR_HEIGHT * 6,											// iTop
			LSN_INV_CHECK_( 3 ) - LSN_INV_CHECK_( 2 ),		// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_RP2A03_4B2_40 ),	// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId
		},
	};

	/** The layout for recording window. */
	LSW_WIDGET_LAYOUT CAudioOptionsWindowLayout::m_wlRecordingPage[] = {
		{
			LSN_LT_AUDIO_OPTIONS_RECORDING_PAGE,			// ltType
			LSN_AOWI_PAGE_RECORDING,						// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			0,												// iLeft
			0,												// iTop
			LSN_AUDIO_GLOBAL_PAGE_W,						// dwWidth
			LSN_AUDIO_GLOBAL_PAGE_H,						// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | DS_3DLOOK | DS_FIXEDSYS | DS_SETFONT | DS_CONTROL,																																	// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_CONTROLPARENT,																																			// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_NONE,									// dwParentId
		},

		// == Raw
		{
			LSW_LT_GROUPBOX,								// ltType
			LSN_AOWI_PAGE_RAW_GROUP,						// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST,									// iLeft
			LSN_AUDIO_OPTIONS_RAW_GROUP_TOP,				// iTop
			LSN_AUDIO_OPTIONS_RAW_GROUP_W,					// dwWidth
			LSN_AUDIO_OPTIONS_RAW_GROUP_H,					// dwHeight
			LSN_GROUPSTYLE,									// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																																		// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_RAW ),				// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},

		// Path.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_RAW_PATH_LABEL,					// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_RAW_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),																												// iTop
			LSN_GENERAL_LABEL0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_PATH_ ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			LSN_AOWI_PAGE_RAW_PATH_EDIT,					// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_COMBO0_L,							// iLeft
			LSN_AUDIO_OPTIONS_RAW_GROUP_TOP + LSN_GROUP_TOP,// iTop
			LSN_GENERAL_COMBO0_W,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},
		{
			LSW_LT_BUTTON,									// ltType
			LSN_AOWI_PAGE_RAW_PATH_BUTTON,					// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_RAW_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_BUTTON_HEIGHT) >> 1),																												// iTop
			LSN_GENERAL_CHECK0_W,							// dwWidth
			LSN_DEF_BUTTON_HEIGHT,							// dwHeight
			LSN_DEFBUTTONSTYLE,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_BROWSE ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},

		// Hz.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_RAW_HZ_LABEL,						// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_RAW_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),																												// iTop
			LSN_GENERAL_LABEL0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_HZ_ ),				// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			LSN_AOWI_PAGE_RAW_HZ_EDIT,						// wId
			nullptr,										// lpwcClass
			FALSE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_COMBO0_L,							// iLeft
			LSN_AUDIO_OPTIONS_RAW_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST),																																// iTop
			LSN_GENERAL_COMBO0_W,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},
		{
			LSW_LT_CHECK,									// ltType
			LSN_AOWI_PAGE_RAW_ENABLE_CHECK,					// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_RAW_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 1,																	// iTop
			LSN_GENERAL_CHECK0_W,							// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_ENABLED ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},

		// Format.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_RAW_FORMAT_LABEL,					// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_RAW_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2 + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),																	// iTop
			LSN_GENERAL_LABEL0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_FORMAT ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			LSN_AOWI_PAGE_RAW_FORMAT_COMBO,					// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_COMBO0_L,							// iLeft
			LSN_AUDIO_OPTIONS_RAW_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2,																															// iTop
			LSN_GENERAL_COMBO0_W,							// dwWidth
			LSN_DEF_COMBO_HEIGHT,							// dwHeight
			LSN_COMBOSTYLE_LIST,							// dwStyle
			LSN_COMBOSTYLEEX_LIST,							// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},

		// Bits.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_RAW_BITS_LABEL,					// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_RAW_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2 + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),							// iTop
			LSN_GENERAL_LABEL0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_BITS_ ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			LSN_AOWI_PAGE_RAW_BITS_COMBO,					// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_COMBO0_L,							// iLeft
			LSN_AUDIO_OPTIONS_RAW_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2 + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST),																					// iTop
			LSN_GENERAL_COMBO0_W,							// dwWidth
			LSN_DEF_COMBO_HEIGHT,							// dwHeight
			LSN_COMBOSTYLE_LIST,							// dwStyle
			LSN_COMBOSTYLEEX_LIST,							// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},
		{
			LSW_LT_CHECK,									// ltType
			LSN_AOWI_PAGE_RAW_DITHER_CHECK,					// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_RAW_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2 + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1),							// iTop
			LSN_GENERAL_CHECK0_W,							// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_DITHER ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},

		// Start Condition.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_RAW_START_CONDITION_LABEL,		// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_CONDITION_L( 0 ),							// iLeft
			LSN_AUDIO_OPTIONS_RAW_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2 + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 2 + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),						// iTop
			LSN_CONDITION_W( 0 ),							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_START_CONDITION_ ),	// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			LSN_AOWI_PAGE_RAW_START_CONDITION_COMBO,		// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_CONDITION_L( 1 ),							// iLeft
			LSN_AUDIO_OPTIONS_RAW_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2 + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 2,																				// iTop
			LSN_CONDITION_W( 1 ),							// dwWidth
			LSN_DEF_COMBO_HEIGHT,							// dwHeight
			LSN_COMBOSTYLE_LIST,							// dwStyle
			LSN_COMBOSTYLEEX_LIST,							// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},

		// End Condition.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_RAW_STOP_CONDITION_LABEL,			// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_CONDITION_L( 2 ),							// iLeft
			LSN_AUDIO_OPTIONS_RAW_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2 + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 2 + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),						// iTop
			LSN_CONDITION_W( 2 ),							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_END_CONDITION_ ),	// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			LSN_AOWI_PAGE_RAW_STOP_CONDITION_COMBO,			// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_CONDITION_L( 3 ),							// iLeft
			LSN_AUDIO_OPTIONS_RAW_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2 + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 2,																				// iTop
			LSN_CONDITION_W( 3 ),							// dwWidth
			LSN_DEF_COMBO_HEIGHT,							// dwHeight
			LSN_COMBOSTYLE_LIST,							// dwStyle
			LSN_COMBOSTYLEEX_LIST,							// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			LSN_AOWI_PAGE_RAW_START_COMBO,					// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_CONDITION_L( 0 ),							// iLeft
			LSN_AUDIO_OPTIONS_RAW_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2 + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 3,																				// iTop
			LSN_CONDITION2_W( 0 ),							// dwWidth
			LSN_DEF_COMBO_HEIGHT,							// dwHeight
			LSN_COMBOSTYLE,									// dwStyle
			0,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			LSN_AOWI_PAGE_RAW_STOP_COMBO,					// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_CONDITION_L( 2 ),							// iLeft
			LSN_AUDIO_OPTIONS_RAW_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2 + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 3,																				// iTop
			LSN_CONDITION2_W( 1 ),							// dwWidth
			LSN_DEF_COMBO_HEIGHT,							// dwHeight
			LSN_COMBOSTYLE,									// dwStyle
			0,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},

		// Metadata.
		{
			LSW_LT_GROUPBOX,								// ltType
			LSN_AOWI_PAGE_RAW_GROUP,						// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_META_RAW_GROUP_T,							// iTop
			LSN_META_GROUP_W,								// dwWidth
			LSN_META_RAW_GROUP_H,							// dwHeight
			LSN_GROUPSTYLE,									// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																																		// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_METADATA ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			LSN_AOWI_PAGE_RAW_META_COMBO,					// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT * 2,				// iLeft
			LSN_META_RAW_GROUP_T + LSN_GROUP_TOP,			// iTop
			LSN_META_CHECK0_L - (LSN_LEFT_JUST + LSN_GROUP_LEFT * 2) - LSN_LEFT_JUST * 2,																																		// dwWidth
			LSN_DEF_COMBO_HEIGHT,							// dwHeight
			LSN_COMBOSTYLE_LIST,							// dwStyle
			LSN_COMBOSTYLEEX_LIST,							// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},
		{
			LSW_LT_CHECK,									// ltType
			LSN_AOWI_PAGE_RAW_META_CHECK,					// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_META_CHECK0_L,								// iLeft
			LSN_META_RAW_GROUP_T + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1),																														// iTop
			LSN_META_CHECK0_W,								// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_ENABLED ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},
		
		{
			LSW_LT_CHECK,									// ltType
			LSN_AOWI_PAGE_RAW_META_ALL_CHECK,				// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_META_CHECK_L( 0 ),							// iLeft
			LSN_META_RAW_GROUP_T + LSN_GROUP_TOP + LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST,																																			// iTop
			LSN_META_CHECK_W( 0 ),							// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_METADATA_ALL ),		// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},
		{
			LSW_LT_CHECK,									// ltType
			LSN_AOWI_PAGE_RAW_META_NONE_CHECK,				// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_META_CHECK_L( 1 ),							// iLeft
			LSN_META_RAW_GROUP_T + LSN_GROUP_TOP + LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST,																																			// iTop
			LSN_META_CHECK_W( 1 ),							// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_METADATA_NONE ),	// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},
		{
			LSW_LT_CHECK,									// ltType
			LSN_AOWI_PAGE_RAW_META_PULSE1_CHECK,			// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_META_CHECK_L( 2 ),							// iLeft
			LSN_META_RAW_GROUP_T + LSN_GROUP_TOP + LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST,																																			// iTop
			LSN_META_CHECK_W( 2 ),							// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_METADATA_PULSE1 ),	// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},

		{
			LSW_LT_CHECK,									// ltType
			LSN_AOWI_PAGE_RAW_META_PULSE2_CHECK,			// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_META_CHECK_L( 0 ),							// iLeft
			LSN_META_RAW_GROUP_T + LSN_GROUP_TOP + LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST + (LSN_DEF_CHECK_HEIGHT + LSN_TOP_JUST) * 1,																								// iTop
			LSN_META_CHECK_W( 0 ),							// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_METADATA_PULSE2 ),	// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},
		{
			LSW_LT_CHECK,									// ltType
			LSN_AOWI_PAGE_RAW_META_TRI_CHECK,				// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_META_CHECK_L( 1 ),							// iLeft
			LSN_META_RAW_GROUP_T + LSN_GROUP_TOP + LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST + (LSN_DEF_CHECK_HEIGHT + LSN_TOP_JUST) * 1,																								// iTop
			LSN_META_CHECK_W( 1 ),							// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_METADATA_TRIANGLE ),// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},
		{
			LSW_LT_CHECK,									// ltType
			LSN_AOWI_PAGE_RAW_META_NOISE_CHECK,				// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_META_CHECK_L( 2 ),							// iLeft
			LSN_META_RAW_GROUP_T + LSN_GROUP_TOP + LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST + (LSN_DEF_CHECK_HEIGHT + LSN_TOP_JUST) * 1,																								// iTop
			LSN_META_CHECK_W( 2 ),							// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_METADATA_NOISE ),	// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},

		{
			LSW_LT_CHECK,									// ltType
			LSN_AOWI_PAGE_RAW_META_DMC_CHECK,				// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_META_CHECK_L( 0 ),							// iLeft
			LSN_META_RAW_GROUP_T + LSN_GROUP_TOP + LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST + (LSN_DEF_CHECK_HEIGHT + LSN_TOP_JUST) * 2,																								// iTop
			LSN_META_CHECK_W( 0 ),							// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_METADATA_DMC ),		// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},
		{
			LSW_LT_CHECK,									// ltType
			LSN_AOWI_PAGE_RAW_META_STATUS_CHECK,			// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_META_CHECK_L( 1 ),							// iLeft
			LSN_META_RAW_GROUP_T + LSN_GROUP_TOP + LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST + (LSN_DEF_CHECK_HEIGHT + LSN_TOP_JUST) * 2,																								// iTop
			LSN_META_CHECK_W( 1 ),							// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_METADATA_STATUS ),	// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},
		{
			LSW_LT_CHECK,									// ltType
			LSN_AOWI_PAGE_RAW_META_FRAME_CHECK,				// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_META_CHECK_L( 2 ),							// iLeft
			LSN_META_RAW_GROUP_T + LSN_GROUP_TOP + LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST + (LSN_DEF_CHECK_HEIGHT + LSN_TOP_JUST) * 2,																								// iTop
			LSN_META_CHECK_W( 2 ),							// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_METADATA_FRAME_COUNTER ),																																								// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},



		// == Output Capture
		{
			LSW_LT_GROUPBOX,								// ltType
			LSN_AOWI_PAGE_OUT_GROUP,						// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST,									// iLeft
			LSN_AUDIO_OPTIONS_OUT_GROUP_TOP,				// iTop
			LSN_AUDIO_OPTIONS_RAW_GROUP_W,					// dwWidth
			LSN_AUDIO_OPTIONS_OUT_GROUP_H,					// dwHeight
			LSN_GROUPSTYLE,									// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																																		// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_CAPTURE_OUTPUT ),				// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},

		// Path.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_OUT_PATH_LABEL,					// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_OUT_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),																												// iTop
			LSN_GENERAL_LABEL0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_PATH_ ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			LSN_AOWI_PAGE_OUT_PATH_EDIT,					// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_COMBO0_L,							// iLeft
			LSN_AUDIO_OPTIONS_OUT_GROUP_TOP + LSN_GROUP_TOP,// iTop
			LSN_GENERAL_COMBO0_W,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},
		{
			LSW_LT_BUTTON,									// ltType
			LSN_AOWI_PAGE_OUT_PATH_BUTTON,					// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_OUT_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_BUTTON_HEIGHT) >> 1),																												// iTop
			LSN_GENERAL_CHECK0_W,							// dwWidth
			LSN_DEF_BUTTON_HEIGHT,							// dwHeight
			LSN_DEFBUTTONSTYLE,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_BROWSE ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},

		// Hz.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_OUT_HZ_LABEL,						// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_OUT_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),																												// iTop
			LSN_GENERAL_LABEL0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_HZ_ ),				// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			LSN_AOWI_PAGE_OUT_HZ_EDIT,						// wId
			nullptr,										// lpwcClass
			FALSE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_COMBO0_L,							// iLeft
			LSN_AUDIO_OPTIONS_OUT_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST),																																// iTop
			LSN_GENERAL_COMBO0_W,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},
		{
			LSW_LT_CHECK,									// ltType
			LSN_AOWI_PAGE_OUT_ENABLE_CHECK,					// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_OUT_GROUP_TOP + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1) + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 1,																	// iTop
			LSN_GENERAL_CHECK0_W,							// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_ENABLED ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},

		// Format.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_OUT_FORMAT_LABEL,					// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_OUT_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2 + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),																	// iTop
			LSN_GENERAL_LABEL0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_FORMAT ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			LSN_AOWI_PAGE_OUT_FORMAT_COMBO,					// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_COMBO0_L,							// iLeft
			LSN_AUDIO_OPTIONS_OUT_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2,																															// iTop
			LSN_GENERAL_COMBO0_W,							// dwWidth
			LSN_DEF_COMBO_HEIGHT,							// dwHeight
			LSN_COMBOSTYLE_LIST,							// dwStyle
			LSN_COMBOSTYLEEX_LIST,							// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},

		// Bits.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_OUT_BITS_LABEL,					// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
			LSN_AUDIO_OPTIONS_OUT_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2 + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),							// iTop
			LSN_GENERAL_LABEL0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_BITS_ ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			LSN_AOWI_PAGE_OUT_BITS_COMBO,					// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_COMBO0_L,							// iLeft
			LSN_AUDIO_OPTIONS_OUT_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2 + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST),																					// iTop
			LSN_GENERAL_COMBO0_W,							// dwWidth
			LSN_DEF_COMBO_HEIGHT,							// dwHeight
			LSN_COMBOSTYLE_LIST,							// dwStyle
			LSN_COMBOSTYLEEX_LIST,							// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},
		{
			LSW_LT_CHECK,									// ltType
			LSN_AOWI_PAGE_OUT_DITHER_CHECK,					// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_GENERAL_CHECK0_L,							// iLeft
			LSN_AUDIO_OPTIONS_OUT_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2 + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1),							// iTop
			LSN_GENERAL_CHECK0_W,							// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_DITHER ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},

		// Start Condition.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_OUT_START_CONDITION_LABEL,		// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_CONDITION_L( 0 ),							// iLeft
			LSN_AUDIO_OPTIONS_OUT_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2 + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 2 + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),						// iTop
			LSN_CONDITION_W( 0 ),							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_START_CONDITION_ ),	// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			LSN_AOWI_PAGE_OUT_START_CONDITION_COMBO,		// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_CONDITION_L( 1 ),							// iLeft
			LSN_AUDIO_OPTIONS_OUT_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2 + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 2,																				// iTop
			LSN_CONDITION_W( 1 ),							// dwWidth
			LSN_DEF_COMBO_HEIGHT,							// dwHeight
			LSN_COMBOSTYLE_LIST,							// dwStyle
			LSN_COMBOSTYLEEX_LIST,							// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},

		// End Condition.
		{
			LSW_LT_LABEL,									// ltType
			LSN_AOWI_PAGE_OUT_STOP_CONDITION_LABEL,			// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_CONDITION_L( 2 ),							// iLeft
			LSN_AUDIO_OPTIONS_OUT_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2 + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 2 + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),						// iTop
			LSN_CONDITION_W( 2 ),							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_END_CONDITION_ ),	// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			LSN_AOWI_PAGE_OUT_STOP_CONDITION_COMBO,			// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_CONDITION_L( 3 ),							// iLeft
			LSN_AUDIO_OPTIONS_OUT_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2 + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 2,																				// iTop
			LSN_CONDITION_W( 3 ),							// dwWidth
			LSN_DEF_COMBO_HEIGHT,							// dwHeight
			LSN_COMBOSTYLE_LIST,							// dwStyle
			LSN_COMBOSTYLEEX_LIST,							// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			LSN_AOWI_PAGE_OUT_START_COMBO,					// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_CONDITION_L( 0 ),							// iLeft
			LSN_AUDIO_OPTIONS_OUT_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2 + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 3,																				// iTop
			LSN_CONDITION2_W( 0 ),							// dwWidth
			LSN_DEF_COMBO_HEIGHT,							// dwHeight
			LSN_COMBOSTYLE,									// dwStyle
			0,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			LSN_AOWI_PAGE_OUT_STOP_COMBO,					// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_CONDITION_L( 2 ),							// iLeft
			LSN_AUDIO_OPTIONS_OUT_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2 + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 3,																				// iTop
			LSN_CONDITION2_W( 1 ),							// dwWidth
			LSN_DEF_COMBO_HEIGHT,							// dwHeight
			LSN_COMBOSTYLE,									// dwStyle
			0,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_RECORDING,						// dwParentId
		},

		// Description.
		//{
		//	LSW_LT_LABEL,									// ltType
		//	LSN_AOWI_PAGE_OUT_DESC_LABEL,					// wId
		//	nullptr,										// lpwcClass
		//	TRUE,											// bEnabled
		//	FALSE,											// bActive
		//	LSN_CONDITION_L( 0 ),							// iLeft
		//	LSN_AUDIO_OPTIONS_OUT_GROUP_TOP + LSN_GROUP_TOP + (LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST) * 2 + (LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST) * 4,																				// iTop
		//	LSN_CONDITION_L( 4 ) - LSN_CONDITION_L( 0 ),	// dwWidth
		//	LSN_DEF_STATIC_HEIGHT * 2,						// dwHeight
		//	LSN_STATICSTYLE,								// dwStyle
		//	0,												// dwStyleEx
		//	LSN_LSTR( LSN_AUDIO_OPTIONS_OUT_DESC ),			// pwcText
		//	0,												// sTextLen
		//	LSN_AOWI_PAGE_RECORDING,						// dwParentId
		//},

		// Metadata.
		//{
		//	LSW_LT_GROUPBOX,								// ltType
		//	LSN_AOWI_PAGE_OUT_GROUP,						// wId
		//	WC_BUTTONW,										// lpwcClass
		//	TRUE,											// bEnabled
		//	FALSE,											// bActive
		//	LSN_LEFT_JUST + LSN_GROUP_LEFT,					// iLeft
		//	LSN_META_OUT_GROUP_T,							// iTop
		//	LSN_META_GROUP_W,								// dwWidth
		//	LSN_META_OUT_GROUP_H,							// dwHeight
		//	LSN_GROUPSTYLE,									// dwStyle
		//	WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																																		// dwStyleEx
		//	LSN_LSTR( LSN_AUDIO_OPTIONS_METADATA ),			// pwcText
		//	0,												// sTextLen
		//	LSN_AOWI_PAGE_RECORDING,						// dwParentId
		//},
		//{
		//	LSW_LT_COMBOBOX,								// ltType
		//	LSN_AOWI_PAGE_OUT_META_COMBO,					// wId
		//	WC_COMBOBOXW,									// lpwcClass
		//	TRUE,											// bEnabled
		//	FALSE,											// bActive
		//	LSN_LEFT_JUST + LSN_GROUP_LEFT * 2,				// iLeft
		//	LSN_META_OUT_GROUP_T + LSN_GROUP_TOP,			// iTop
		//	LSN_META_CHECK0_L - (LSN_LEFT_JUST + LSN_GROUP_LEFT * 2) - LSN_LEFT_JUST * 2,																																		// dwWidth
		//	LSN_DEF_COMBO_HEIGHT,							// dwHeight
		//	LSN_COMBOSTYLE_LIST,							// dwStyle
		//	LSN_COMBOSTYLEEX_LIST,							// dwStyleEx
		//	nullptr,										// pwcText
		//	0,												// sTextLen
		//	LSN_AOWI_PAGE_RECORDING,						// dwParentId
		//},
		//{
		//	LSW_LT_CHECK,									// ltType
		//	LSN_AOWI_PAGE_OUT_META_CHECK,					// wId
		//	WC_BUTTONW,										// lpwcClass
		//	TRUE,											// bEnabled
		//	FALSE,											// bActive
		//	LSN_META_CHECK0_L,								// iLeft
		//	LSN_META_OUT_GROUP_T + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1),																														// iTop
		//	LSN_META_CHECK0_W,								// dwWidth
		//	LSN_DEF_CHECK_HEIGHT,							// dwHeight
		//	LSN_CHECKSTYLE,									// dwStyle
		//	0,												// dwStyleEx
		//	LSN_LSTR( LSN_AUDIO_OPTIONS_ENABLED ),			// pwcText
		//	0,												// sTextLen
		//	LSN_AOWI_PAGE_RECORDING,						// dwParentId
		//},
		//
		//{
		//	LSW_LT_CHECK,									// ltType
		//	LSN_AOWI_PAGE_OUT_META_ALL_CHECK,				// wId
		//	WC_BUTTONW,										// lpwcClass
		//	TRUE,											// bEnabled
		//	FALSE,											// bActive
		//	LSN_META_CHECK_L( 0 ),							// iLeft
		//	LSN_META_OUT_GROUP_T + LSN_GROUP_TOP + LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST,																																			// iTop
		//	LSN_META_CHECK_W( 0 ),							// dwWidth
		//	LSN_DEF_CHECK_HEIGHT,							// dwHeight
		//	LSN_CHECKSTYLE,									// dwStyle
		//	0,												// dwStyleEx
		//	LSN_LSTR( LSN_AUDIO_OPTIONS_METADATA_ALL ),		// pwcText
		//	0,												// sTextLen
		//	LSN_AOWI_PAGE_RECORDING,						// dwParentId
		//},
		//{
		//	LSW_LT_CHECK,									// ltType
		//	LSN_AOWI_PAGE_OUT_META_NONE_CHECK,				// wId
		//	WC_BUTTONW,										// lpwcClass
		//	TRUE,											// bEnabled
		//	FALSE,											// bActive
		//	LSN_META_CHECK_L( 1 ),							// iLeft
		//	LSN_META_OUT_GROUP_T + LSN_GROUP_TOP + LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST,																																			// iTop
		//	LSN_META_CHECK_W( 1 ),							// dwWidth
		//	LSN_DEF_CHECK_HEIGHT,							// dwHeight
		//	LSN_CHECKSTYLE,									// dwStyle
		//	0,												// dwStyleEx
		//	LSN_LSTR( LSN_AUDIO_OPTIONS_METADATA_NONE ),	// pwcText
		//	0,												// sTextLen
		//	LSN_AOWI_PAGE_RECORDING,						// dwParentId
		//},
		//{
		//	LSW_LT_CHECK,									// ltType
		//	LSN_AOWI_PAGE_OUT_META_PULSE1_CHECK,			// wId
		//	WC_BUTTONW,										// lpwcClass
		//	TRUE,											// bEnabled
		//	FALSE,											// bActive
		//	LSN_META_CHECK_L( 2 ),							// iLeft
		//	LSN_META_OUT_GROUP_T + LSN_GROUP_TOP + LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST,																																			// iTop
		//	LSN_META_CHECK_W( 2 ),							// dwWidth
		//	LSN_DEF_CHECK_HEIGHT,							// dwHeight
		//	LSN_CHECKSTYLE,									// dwStyle
		//	0,												// dwStyleEx
		//	LSN_LSTR( LSN_AUDIO_OPTIONS_METADATA_PULSE1 ),	// pwcText
		//	0,												// sTextLen
		//	LSN_AOWI_PAGE_RECORDING,						// dwParentId
		//},

		//{
		//	LSW_LT_CHECK,									// ltType
		//	LSN_AOWI_PAGE_OUT_META_PULSE2_CHECK,			// wId
		//	WC_BUTTONW,										// lpwcClass
		//	TRUE,											// bEnabled
		//	FALSE,											// bActive
		//	LSN_META_CHECK_L( 0 ),							// iLeft
		//	LSN_META_OUT_GROUP_T + LSN_GROUP_TOP + LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST + (LSN_DEF_CHECK_HEIGHT + LSN_TOP_JUST) * 1,																								// iTop
		//	LSN_META_CHECK_W( 0 ),							// dwWidth
		//	LSN_DEF_CHECK_HEIGHT,							// dwHeight
		//	LSN_CHECKSTYLE,									// dwStyle
		//	0,												// dwStyleEx
		//	LSN_LSTR( LSN_AUDIO_OPTIONS_METADATA_PULSE2 ),	// pwcText
		//	0,												// sTextLen
		//	LSN_AOWI_PAGE_RECORDING,						// dwParentId
		//},
		//{
		//	LSW_LT_CHECK,									// ltType
		//	LSN_AOWI_PAGE_OUT_META_TRI_CHECK,				// wId
		//	WC_BUTTONW,										// lpwcClass
		//	TRUE,											// bEnabled
		//	FALSE,											// bActive
		//	LSN_META_CHECK_L( 1 ),							// iLeft
		//	LSN_META_OUT_GROUP_T + LSN_GROUP_TOP + LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST + (LSN_DEF_CHECK_HEIGHT + LSN_TOP_JUST) * 1,																								// iTop
		//	LSN_META_CHECK_W( 1 ),							// dwWidth
		//	LSN_DEF_CHECK_HEIGHT,							// dwHeight
		//	LSN_CHECKSTYLE,									// dwStyle
		//	0,												// dwStyleEx
		//	LSN_LSTR( LSN_AUDIO_OPTIONS_METADATA_TRIANGLE ),// pwcText
		//	0,												// sTextLen
		//	LSN_AOWI_PAGE_RECORDING,						// dwParentId
		//},
		//{
		//	LSW_LT_CHECK,									// ltType
		//	LSN_AOWI_PAGE_OUT_META_NOISE_CHECK,				// wId
		//	WC_BUTTONW,										// lpwcClass
		//	TRUE,											// bEnabled
		//	FALSE,											// bActive
		//	LSN_META_CHECK_L( 2 ),							// iLeft
		//	LSN_META_OUT_GROUP_T + LSN_GROUP_TOP + LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST + (LSN_DEF_CHECK_HEIGHT + LSN_TOP_JUST) * 1,																								// iTop
		//	LSN_META_CHECK_W( 2 ),							// dwWidth
		//	LSN_DEF_CHECK_HEIGHT,							// dwHeight
		//	LSN_CHECKSTYLE,									// dwStyle
		//	0,												// dwStyleEx
		//	LSN_LSTR( LSN_AUDIO_OPTIONS_METADATA_NOISE ),	// pwcText
		//	0,												// sTextLen
		//	LSN_AOWI_PAGE_RECORDING,						// dwParentId
		//},

		//{
		//	LSW_LT_CHECK,									// ltType
		//	LSN_AOWI_PAGE_OUT_META_DMC_CHECK,				// wId
		//	WC_BUTTONW,										// lpwcClass
		//	TRUE,											// bEnabled
		//	FALSE,											// bActive
		//	LSN_META_CHECK_L( 0 ),							// iLeft
		//	LSN_META_OUT_GROUP_T + LSN_GROUP_TOP + LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST + (LSN_DEF_CHECK_HEIGHT + LSN_TOP_JUST) * 2,																								// iTop
		//	LSN_META_CHECK_W( 0 ),							// dwWidth
		//	LSN_DEF_CHECK_HEIGHT,							// dwHeight
		//	LSN_CHECKSTYLE,									// dwStyle
		//	0,												// dwStyleEx
		//	LSN_LSTR( LSN_AUDIO_OPTIONS_METADATA_DMC ),		// pwcText
		//	0,												// sTextLen
		//	LSN_AOWI_PAGE_RECORDING,						// dwParentId
		//},
		//{
		//	LSW_LT_CHECK,									// ltType
		//	LSN_AOWI_PAGE_OUT_META_STATUS_CHECK,			// wId
		//	WC_BUTTONW,										// lpwcClass
		//	TRUE,											// bEnabled
		//	FALSE,											// bActive
		//	LSN_META_CHECK_L( 1 ),							// iLeft
		//	LSN_META_OUT_GROUP_T + LSN_GROUP_TOP + LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST + (LSN_DEF_CHECK_HEIGHT + LSN_TOP_JUST) * 2,																								// iTop
		//	LSN_META_CHECK_W( 1 ),							// dwWidth
		//	LSN_DEF_CHECK_HEIGHT,							// dwHeight
		//	LSN_CHECKSTYLE,									// dwStyle
		//	0,												// dwStyleEx
		//	LSN_LSTR( LSN_AUDIO_OPTIONS_METADATA_STATUS ),	// pwcText
		//	0,												// sTextLen
		//	LSN_AOWI_PAGE_RECORDING,						// dwParentId
		//},
		//{
		//	LSW_LT_CHECK,									// ltType
		//	LSN_AOWI_PAGE_OUT_META_FRAME_CHECK,				// wId
		//	WC_BUTTONW,										// lpwcClass
		//	TRUE,											// bEnabled
		//	FALSE,											// bActive
		//	LSN_META_CHECK_L( 2 ),							// iLeft
		//	LSN_META_OUT_GROUP_T + LSN_GROUP_TOP + LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST + (LSN_DEF_CHECK_HEIGHT + LSN_TOP_JUST) * 2,																								// iTop
		//	LSN_META_CHECK_W( 2 ),							// dwWidth
		//	LSN_DEF_CHECK_HEIGHT,							// dwHeight
		//	LSN_CHECKSTYLE,									// dwStyle
		//	0,												// dwStyleEx
		//	LSN_LSTR( LSN_AUDIO_OPTIONS_METADATA_FRAME_COUNTER ),																																								// pwcText
		//	0,												// sTextLen
		//	LSN_AOWI_PAGE_RECORDING,						// dwParentId
		//},

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
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | DS_3DLOOK | DS_FIXEDSYS | DS_MODALFRAME | DS_CENTER,																							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,																														// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_ ),					// pwcText
			0,												// sTextLen
			LSN_AOWI_NONE,									// dwParentId
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
			LSN_AUDIO_OPTIONS_H - LSN_DEF_BUTTON_HEIGHT - (LSN_TOP_JUST * 2),																																					// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | TCS_HOTTRACK,																																							// dwStyle
			WS_EX_ACCEPTFILES,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_MAINWINDOW,							// dwParentId
		},


		{
			LSW_LT_BUTTON,									// ltType
			LSN_AOWI_OK,									// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			TRUE,											// bActive
			LSN_LEFT_JUST,									// iLeft
			LSN_AUDIO_OPTIONS_H - LSN_DEF_BUTTON_HEIGHT - LSN_TOP_JUST,																																							// iTop
			LSN_DEF_BUTTON_WIDTH,							// dwWidth
			LSN_DEF_BUTTON_HEIGHT,							// dwHeight
			LSN_DEFBUTTONSTYLE,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																																		// dwStyleEx
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
			LSN_AUDIO_OPTIONS_W - LSN_DEF_BUTTON_WIDTH * 1 - LSN_LEFT_JUST,																																						// iLeft
			LSN_AUDIO_OPTIONS_H - LSN_DEF_BUTTON_HEIGHT	- LSN_TOP_JUST,																																							// iTop
			LSN_DEF_BUTTON_WIDTH,							// dwWidth
			LSN_DEF_BUTTON_HEIGHT,							// dwHeight
			LSN_BUTTONSTYLE,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																																		// dwStyleEx
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

#undef LSN_AUDIO_OPTIONS_H
#undef LSN_AUDIO_OPTIONS_W

#undef LSN_META_CHECK_L
#undef LSN_META_CHECK_W

#undef LSN_META_GROUP_W
#undef LSN_META_RAW_GROUP_T
#undef LSN_META_OUT_GROUP_T
#undef LSN_META_RAW_GROUP_H
#undef LSN_META_OUT_GROUP_H

#undef LSN_AUDIO_OPTIONS_OUT_GROUP_H
#undef LSN_AUDIO_OPTIONS_OUT_GROUP_TOP
#undef LSN_AUDIO_OPTIONS_RAW_GROUP_H
#undef LSN_AUDIO_OPTIONS_RAW_GROUP_W
#undef LSN_AUDIO_OPTIONS_RAW_GROUP_TOP

#undef LSN_CONDITION_L
#undef LSN_INV_CHECK_

#undef LSN_AUDIO_OPTIONS_PAGE_W
#undef LSN_AUDIO_OPTIONS_PAGE_H
#undef LSN_AUDIO_OPTIONS_GENERAL_GROUP_H
#undef LSN_AUDIO_GLOBAL_PAGE_H
#undef LSN_AUDIO_GLOBAL_PAGE_W

#undef LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP
#undef LSN_AUDIO_OPTIONS_GENERAL_GROUP_W

#undef LSN_AUDIO_OPTIONS_CHAR_GROUP_H
#undef LSN_AUDIO_OPTIONS_CHAR_GROUP_W
#undef LSN_AUDIO_OPTIONS_CHAR_GROUP_TOP

#undef LSN_META_LABEL0_W
#undef LSN_META_COMBO0_L
#undef LSN_META_COMBO0_W
#undef LSN_META_CHECK0_L
#undef LSN_META_CHECK0_W

#undef LSN_GENERAL_TRACK_L
#undef LSN_GENERAL_TRACK_W
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
	 * \param _oOptions A reference to the options object.
	 * \return Returns the created page.
	 */
	CWidget * CAudioOptionsWindowLayout::CreateGlobalPage( CWidget * _pwParent, LSN_OPTIONS &_oOptions ) {
		return CreatePage( _pwParent, m_wlGlobalPage, LSN_ELEMENTS( m_wlGlobalPage ), _oOptions );
	}

	/**
	 * Creates the per-game page.
	 *
	 * \param _pwParent The parent widget.
	 * \param _oOptions A reference to the options object.
	 * \return Returns the created page.
	 */
	CWidget * CAudioOptionsWindowLayout::CreatePerGamePage( CWidget * _pwParent, LSN_OPTIONS &_oOptions ) {
		return CreatePage( _pwParent, m_wlPerGamePage, LSN_ELEMENTS( m_wlPerGamePage ), _oOptions );
	}

	/**
	 * Creates the recording page.
	 *
	 * \param _pwParent The parent widget.
	 * \param _oOptions A reference to the options object.
	 * \return Returns the created page.
	 */
	CWidget * CAudioOptionsWindowLayout::CreateRecordingPage( CWidget * _pwParent, LSN_OPTIONS &_oOptions ) {
		return CreatePage( _pwParent, m_wlRecordingPage, LSN_ELEMENTS( m_wlRecordingPage ), _oOptions );
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

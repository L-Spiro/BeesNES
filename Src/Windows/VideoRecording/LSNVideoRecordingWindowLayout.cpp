#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The layout for the Video Recording Settings dialog.
 */

#include "LSNVideoRecordingWindowLayout.h"
#include "../../Localization/LSNLocalization.h"
#include "../Layout/LSNLayoutMacros.h"
#include "../Layout/LSNLayoutManager.h"



namespace lsn {

#define LSN_VR_LABEL_L									(LSN_LEFT_JUST + LSN_GROUP_LEFT)
#define LSN_VR_LABEL_W									48
#define LSN_VR_CTRL_L									(LSN_VR_LABEL_L + LSN_VR_LABEL_W)
#define LSN_VR_CTRL_W									200
#define LSN_VR_RIGHT_L									(LSN_VR_CTRL_L + LSN_VR_CTRL_W + LSN_LEFT_JUST * 2)
#define LSN_VR_RIGHT_W									60
#define LSN_VR_W										(LSN_VR_RIGHT_L + LSN_VR_RIGHT_W + (LSN_LEFT_JUST * 2) + LSN_LEFT_JUST)
#define LSN_VR_GROUP_W									(LSN_VR_W - LSN_LEFT_JUST * 2)
#define LSN_VR_CONTENT_W								(LSN_VR_RIGHT_L + LSN_VR_RIGHT_W - LSN_VR_LABEL_L)
#define LSN_VR_ROW_H									(LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST)
#define LSN_VR_TRACK_ROW_H								(LSN_DEF_TRACKBAR_HEIGHT + LSN_TOP_JUST)
#define LSN_VR_DEFAULTS_W								(LSN_DEF_BUTTON_WIDTH + 20)
#define LSN_VR_TRACKBAR_STYLE							WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | TBS_AUTOTICKS | TBS_BOTH

#define LSN_VR_OUTPUT_T									LSN_TOP_JUST
#define LSN_VR_OUTPUT_H									(LSN_GROUP_TOP + LSN_VR_ROW_H * 2 + LSN_GROUP_BOTTOM)
#define LSN_VR_FOLDER_T									(LSN_VR_OUTPUT_T + LSN_GROUP_TOP)
#define LSN_VR_FFMPEG_T									(LSN_VR_FOLDER_T + LSN_VR_ROW_H)

#define LSN_VR_VIDEO_T									(LSN_VR_OUTPUT_T + LSN_VR_OUTPUT_H)
#define LSN_VR_VIDEO_H									(LSN_GROUP_TOP + LSN_VR_ROW_H * 6 + LSN_VR_TRACK_ROW_H * 2 + LSN_DEF_CHECK_HEIGHT + LSN_TOP_JUST + LSN_GROUP_BOTTOM)
#define LSN_VR_FORMAT_T									(LSN_VR_VIDEO_T + LSN_GROUP_TOP)
#define LSN_VR_PRESET_T									(LSN_VR_FORMAT_T + LSN_VR_ROW_H)
#define LSN_VR_QUALITY_T								(LSN_VR_PRESET_T + LSN_VR_ROW_H)
#define LSN_VR_QUALITY_TRACK_T							(LSN_VR_QUALITY_T + LSN_VR_ROW_H)
#define LSN_VR_ALT_QUALITY_T							(LSN_VR_QUALITY_TRACK_T + LSN_VR_TRACK_ROW_H)
#define LSN_VR_ALT_QUALITY_TRACK_T						(LSN_VR_ALT_QUALITY_T + LSN_VR_ROW_H)
#define LSN_VR_PIXEL_FORMAT_T							(LSN_VR_ALT_QUALITY_TRACK_T + LSN_VR_TRACK_ROW_H)
#define LSN_VR_FPS_T									(LSN_VR_PIXEL_FORMAT_T + LSN_VR_ROW_H)
#define LSN_VR_BLEND_T									(LSN_VR_FPS_T + LSN_VR_ROW_H)

#define LSN_VR_AUDIO_T									(LSN_VR_VIDEO_T + LSN_VR_VIDEO_H)
#define LSN_VR_AUDIO_H									(LSN_GROUP_TOP + LSN_VR_ROW_H * 2 + LSN_VR_TRACK_ROW_H + LSN_GROUP_BOTTOM)
#define LSN_VR_CODEC_T									(LSN_VR_AUDIO_T + LSN_GROUP_TOP)
#define LSN_VR_BITRATE_T								(LSN_VR_CODEC_T + LSN_VR_ROW_H)
#define LSN_VR_BITRATE_TRACK_T							(LSN_VR_BITRATE_T + LSN_VR_ROW_H)

#define LSN_VR_COND_T									(LSN_VR_AUDIO_T + LSN_VR_AUDIO_H)
#define LSN_VR_COND_H									(LSN_GROUP_TOP + LSN_VR_ROW_H * 2 + LSN_GROUP_BOTTOM)
#define LSN_VR_COND_ROW_T								(LSN_VR_COND_T + LSN_GROUP_TOP)
#define LSN_VR_HISTORY_ROW_T							(LSN_VR_COND_ROW_T + LSN_VR_ROW_H)
#define LSN_VR_COND_L( P )								LSN_EVEN_DIVIDE_EX( LSN_VR_CONTENT_W, LSN_VR_LABEL_L, 4, P, LSN_LEFT_JUST )
#define LSN_VR_COND_W( P )								LSN_EVEN_DIVIDE_WIDTH_EX( LSN_VR_CONTENT_W, LSN_VR_LABEL_L, 4, P, LSN_LEFT_JUST )
#define LSN_VR_COND2_W( P )								LSN_EVEN_DIVIDE_WIDTH_EX( LSN_VR_CONTENT_W, LSN_VR_LABEL_L, 2, P, LSN_LEFT_JUST )

#define LSN_VR_BUTTONS_T								(LSN_VR_COND_T + LSN_VR_COND_H + LSN_TOP_JUST)
/** The height of the dialog. */
#define LSN_VR_H										(LSN_VR_BUTTONS_T + LSN_DEF_BUTTON_HEIGHT + LSN_TOP_JUST)

	// == Members.
	/** The layout for the Video Recording Settings window. */
	LSW_WIDGET_LAYOUT CVideoRecordingWindowLayout::m_wlWindow[] = {
		{
			LSN_LT_VIDEO_RECORDING_DIALOG,				// ltType
			CWinUtilities::LSN_VRWI_DIALOG,				// wId
			nullptr,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			0,											// iLeft
			0,											// iTop
			LSN_VR_W,									// dwWidth
			LSN_VR_H,									// dwHeight
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | DS_3DLOOK | DS_FIXEDSYS | DS_MODALFRAME | DS_CENTER,	// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,	// dwStyleEx
			LSN_LSTR( LSN_VR_TITLE ),					// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_NONE,				// dwParentId
		},

		// == Output.
		{
			LSW_LT_GROUPBOX,							// ltType
			CWinUtilities::LSN_VRWI_OUTPUT_GROUP,		// wId
			WC_BUTTONW,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_LEFT_JUST,								// iLeft
			LSN_VR_OUTPUT_T,							// iTop
			LSN_VR_GROUP_W,								// dwWidth
			LSN_VR_OUTPUT_H,							// dwHeight
			LSN_GROUPSTYLE,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			LSN_LSTR( LSN_VR_OUTPUT ),					// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_LABEL,								// ltType
			CWinUtilities::LSN_VRWI_FOLDER_LABEL,		// wId
			nullptr,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_LABEL_L,								// iLeft
			LSN_VR_FOLDER_T + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),	// iTop
			LSN_VR_LABEL_W,								// dwWidth
			LSN_DEF_STATIC_HEIGHT,						// dwHeight
			LSN_STATICSTYLE,							// dwStyle
			0,											// dwStyleEx
			LSN_LSTR( LSN_VR_FOLDER_ ),					// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_EDIT,								// ltType
			CWinUtilities::LSN_VRWI_FOLDER_EDIT,		// wId
			nullptr,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_CTRL_L,								// iLeft
			LSN_VR_FOLDER_T,							// iTop
			LSN_VR_CTRL_W,								// dwWidth
			LSN_DEF_EDIT_HEIGHT,						// dwHeight
			LSN_EDITSTYLE,								// dwStyle
			WS_EX_CLIENTEDGE,							// dwStyleEx
			nullptr,									// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_BUTTON,								// ltType
			CWinUtilities::LSN_VRWI_FOLDER_BUTTON,		// wId
			WC_BUTTONW,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_RIGHT_L,								// iLeft
			LSN_VR_FOLDER_T,							// iTop
			LSN_VR_RIGHT_W,								// dwWidth
			LSN_DEF_BUTTON_HEIGHT,						// dwHeight
			LSN_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			LSN_LSTR( LSN_VR_BROWSE ),					// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_LABEL,								// ltType
			CWinUtilities::LSN_VRWI_FFMPEG_LABEL,		// wId
			nullptr,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_LABEL_L,								// iLeft
			LSN_VR_FFMPEG_T + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),	// iTop
			LSN_VR_LABEL_W,								// dwWidth
			LSN_DEF_STATIC_HEIGHT,						// dwHeight
			LSN_STATICSTYLE,							// dwStyle
			0,											// dwStyleEx
			LSN_LSTR( LSN_VR_FFMPEG_ ),					// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_EDIT,								// ltType
			CWinUtilities::LSN_VRWI_FFMPEG_EDIT,		// wId
			nullptr,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_CTRL_L,								// iLeft
			LSN_VR_FFMPEG_T,							// iTop
			LSN_VR_CTRL_W,								// dwWidth
			LSN_DEF_EDIT_HEIGHT,						// dwHeight
			LSN_EDITSTYLE,								// dwStyle
			WS_EX_CLIENTEDGE,							// dwStyleEx
			nullptr,									// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_BUTTON,								// ltType
			CWinUtilities::LSN_VRWI_FFMPEG_BUTTON,		// wId
			WC_BUTTONW,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_RIGHT_L,								// iLeft
			LSN_VR_FFMPEG_T,							// iTop
			LSN_VR_RIGHT_W,								// dwWidth
			LSN_DEF_BUTTON_HEIGHT,						// dwHeight
			LSN_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			LSN_LSTR( LSN_VR_BROWSE ),					// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},

		// == Video.
		{
			LSW_LT_GROUPBOX,							// ltType
			CWinUtilities::LSN_VRWI_VIDEO_GROUP,		// wId
			WC_BUTTONW,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_LEFT_JUST,								// iLeft
			LSN_VR_VIDEO_T,								// iTop
			LSN_VR_GROUP_W,								// dwWidth
			LSN_VR_VIDEO_H,								// dwHeight
			LSN_GROUPSTYLE,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			LSN_LSTR( LSN_VR_VIDEO ),					// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_LABEL,								// ltType
			CWinUtilities::LSN_VRWI_FORMAT_LABEL,		// wId
			nullptr,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_LABEL_L,								// iLeft
			LSN_VR_FORMAT_T + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),	// iTop
			LSN_VR_LABEL_W,								// dwWidth
			LSN_DEF_STATIC_HEIGHT,						// dwHeight
			LSN_STATICSTYLE,							// dwStyle
			0,											// dwStyleEx
			LSN_LSTR( LSN_VR_FORMAT_ ),					// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_COMBOBOX,							// ltType
			CWinUtilities::LSN_VRWI_FORMAT_COMBO,		// wId
			WC_COMBOBOXW,								// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_CTRL_L,								// iLeft
			LSN_VR_FORMAT_T,							// iTop
			LSN_VR_CTRL_W,								// dwWidth
			LSN_DEF_COMBO_HEIGHT,						// dwHeight
			LSN_COMBOSTYLE_LIST,						// dwStyle
			LSN_COMBOSTYLEEX_LIST,						// dwStyleEx
			nullptr,									// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_LABEL,								// ltType
			CWinUtilities::LSN_VRWI_PRESET_LABEL,		// wId
			nullptr,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_LABEL_L,								// iLeft
			LSN_VR_PRESET_T + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),	// iTop
			LSN_VR_LABEL_W,								// dwWidth
			LSN_DEF_STATIC_HEIGHT,						// dwHeight
			LSN_STATICSTYLE,							// dwStyle
			0,											// dwStyleEx
			LSN_LSTR( LSN_VR_PRESET_ ),					// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_COMBOBOX,							// ltType
			CWinUtilities::LSN_VRWI_PRESET_COMBO,		// wId
			WC_COMBOBOXW,								// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_CTRL_L,								// iLeft
			LSN_VR_PRESET_T,							// iTop
			LSN_VR_CTRL_W,								// dwWidth
			LSN_DEF_COMBO_HEIGHT,						// dwHeight
			LSN_COMBOSTYLE_LIST,						// dwStyle
			LSN_COMBOSTYLEEX_LIST,						// dwStyleEx
			nullptr,									// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_LABEL,								// ltType
			CWinUtilities::LSN_VRWI_QUALITY_LABEL,		// wId
			nullptr,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_LABEL_L,								// iLeft
			LSN_VR_QUALITY_T + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),	// iTop
			LSN_VR_LABEL_W,								// dwWidth
			LSN_DEF_STATIC_HEIGHT,						// dwHeight
			LSN_STATICSTYLE,							// dwStyle
			0,											// dwStyleEx
			LSN_LSTR( LSN_VR_QUALITY_ ),				// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_LABEL,								// ltType
			CWinUtilities::LSN_VRWI_QUALITY_HINT,		// wId
			nullptr,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_CTRL_L,								// iLeft
			LSN_VR_QUALITY_T + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),	// iTop
			LSN_VR_CTRL_W,								// dwWidth
			LSN_DEF_STATIC_HEIGHT,						// dwHeight
			LSN_STATICSTYLE,							// dwStyle
			0,											// dwStyleEx
			nullptr,									// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_CHECK,								// ltType
			CWinUtilities::LSN_VRWI_LOSSLESS_CHECK,		// wId
			WC_BUTTONW,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_RIGHT_L,								// iLeft
			LSN_VR_QUALITY_T + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1),	// iTop
			LSN_VR_RIGHT_W,								// dwWidth
			LSN_DEF_CHECK_HEIGHT,						// dwHeight
			LSN_CHECKSTYLE,								// dwStyle
			0,											// dwStyleEx
			LSN_LSTR( LSN_VR_LOSSLESS ),				// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_TRACKBAR,							// ltType
			CWinUtilities::LSN_VRWI_QUALITY_TRACKBAR,	// wId
			nullptr,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_CTRL_L,								// iLeft
			LSN_VR_QUALITY_TRACK_T,						// iTop
			LSN_VR_CTRL_W,								// dwWidth
			LSN_DEF_TRACKBAR_HEIGHT,					// dwHeight
			LSN_VR_TRACKBAR_STYLE,						// dwStyle
			0,											// dwStyleEx
			nullptr,									// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_EDIT,								// ltType
			CWinUtilities::LSN_VRWI_QUALITY_EDIT,		// wId
			nullptr,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_RIGHT_L,								// iLeft
			LSN_VR_QUALITY_TRACK_T + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_EDIT_HEIGHT) >> 1),	// iTop
			LSN_VR_RIGHT_W,								// dwWidth
			LSN_DEF_EDIT_HEIGHT,						// dwHeight
			LSN_EDITSTYLE,								// dwStyle
			WS_EX_CLIENTEDGE,							// dwStyleEx
			nullptr,									// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_LABEL,								// ltType
			CWinUtilities::LSN_VRWI_ALT_QUALITY_LABEL,	// wId
			nullptr,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_LABEL_L,								// iLeft
			LSN_VR_ALT_QUALITY_T + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),	// iTop
			LSN_VR_LABEL_W,								// dwWidth
			LSN_DEF_STATIC_HEIGHT,						// dwHeight
			LSN_STATICSTYLE,							// dwStyle
			0,											// dwStyleEx
			LSN_LSTR( LSN_VR_ALT_QUALITY_ ),			// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_LABEL,								// ltType
			CWinUtilities::LSN_VRWI_ALT_QUALITY_HINT,	// wId
			nullptr,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_CTRL_L,								// iLeft
			LSN_VR_ALT_QUALITY_T + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),	// iTop
			LSN_VR_CTRL_W,								// dwWidth
			LSN_DEF_STATIC_HEIGHT,						// dwHeight
			LSN_STATICSTYLE,							// dwStyle
			0,											// dwStyleEx
			nullptr,									// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_CHECK,								// ltType
			CWinUtilities::LSN_VRWI_ALT_QUALITY_CHECK,	// wId
			WC_BUTTONW,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_RIGHT_L,								// iLeft
			LSN_VR_ALT_QUALITY_T + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1),	// iTop
			LSN_VR_RIGHT_W,								// dwWidth
			LSN_DEF_CHECK_HEIGHT,						// dwHeight
			LSN_CHECKSTYLE,								// dwStyle
			0,											// dwStyleEx
			LSN_LSTR( LSN_VR_USE ),						// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_TRACKBAR,							// ltType
			CWinUtilities::LSN_VRWI_ALT_QUALITY_TRACKBAR,	// wId
			nullptr,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_CTRL_L,								// iLeft
			LSN_VR_ALT_QUALITY_TRACK_T,					// iTop
			LSN_VR_CTRL_W,								// dwWidth
			LSN_DEF_TRACKBAR_HEIGHT,					// dwHeight
			LSN_VR_TRACKBAR_STYLE,						// dwStyle
			0,											// dwStyleEx
			nullptr,									// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_EDIT,								// ltType
			CWinUtilities::LSN_VRWI_ALT_QUALITY_EDIT,	// wId
			nullptr,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_RIGHT_L,								// iLeft
			LSN_VR_ALT_QUALITY_TRACK_T + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_EDIT_HEIGHT) >> 1),	// iTop
			LSN_VR_RIGHT_W,								// dwWidth
			LSN_DEF_EDIT_HEIGHT,						// dwHeight
			LSN_EDITSTYLE,								// dwStyle
			WS_EX_CLIENTEDGE,							// dwStyleEx
			nullptr,									// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_LABEL,								// ltType
			CWinUtilities::LSN_VRWI_PIXEL_FORMAT_LABEL,	// wId
			nullptr,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_LABEL_L,								// iLeft
			LSN_VR_PIXEL_FORMAT_T + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),	// iTop
			LSN_VR_LABEL_W,								// dwWidth
			LSN_DEF_STATIC_HEIGHT,						// dwHeight
			LSN_STATICSTYLE,							// dwStyle
			0,											// dwStyleEx
			LSN_LSTR( LSN_VR_PIXEL_FORMAT_ ),			// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_COMBOBOX,							// ltType
			CWinUtilities::LSN_VRWI_PIXEL_FORMAT_COMBO,	// wId
			WC_COMBOBOXW,								// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_CTRL_L,								// iLeft
			LSN_VR_PIXEL_FORMAT_T,						// iTop
			LSN_VR_CTRL_W,								// dwWidth
			LSN_DEF_COMBO_HEIGHT,						// dwHeight
			LSN_COMBOSTYLE_LIST,						// dwStyle
			LSN_COMBOSTYLEEX_LIST,						// dwStyleEx
			nullptr,									// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_LABEL,								// ltType
			CWinUtilities::LSN_VRWI_FPS_LABEL,			// wId
			nullptr,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_LABEL_L,								// iLeft
			LSN_VR_FPS_T + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),	// iTop
			LSN_VR_LABEL_W,								// dwWidth
			LSN_DEF_STATIC_HEIGHT,						// dwHeight
			LSN_STATICSTYLE,							// dwStyle
			0,											// dwStyleEx
			LSN_LSTR( LSN_VR_FRAME_RATE_ ),				// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_EDIT,								// ltType
			CWinUtilities::LSN_VRWI_FPS_EDIT,			// wId
			nullptr,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_CTRL_L,								// iLeft
			LSN_VR_FPS_T,								// iTop
			LSN_VR_CTRL_W,								// dwWidth
			LSN_DEF_EDIT_HEIGHT,						// dwHeight
			LSN_EDITSTYLE,								// dwStyle
			WS_EX_CLIENTEDGE,							// dwStyleEx
			nullptr,									// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_CHECK,								// ltType
			CWinUtilities::LSN_VRWI_MATCH_CONSOLE_CHECK,	// wId
			WC_BUTTONW,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_RIGHT_L,								// iLeft
			LSN_VR_FPS_T + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1),	// iTop
			LSN_VR_RIGHT_W,								// dwWidth
			LSN_DEF_CHECK_HEIGHT,						// dwHeight
			LSN_CHECKSTYLE,								// dwStyle
			0,											// dwStyleEx
			LSN_LSTR( LSN_VR_MATCH_CONSOLE ),			// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_CHECK,								// ltType
			CWinUtilities::LSN_VRWI_BLEND_CHECK,		// wId
			WC_BUTTONW,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_CTRL_L,								// iLeft
			LSN_VR_BLEND_T,	// iTop
			LSN_VR_CTRL_W,								// dwWidth
			LSN_DEF_CHECK_HEIGHT,						// dwHeight
			LSN_CHECKSTYLE,								// dwStyle
			0,											// dwStyleEx
			LSN_LSTR( LSN_VR_BLEND_FRAMES ),			// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},

		// == Audio.
		{
			LSW_LT_GROUPBOX,							// ltType
			CWinUtilities::LSN_VRWI_AUDIO_GROUP,		// wId
			WC_BUTTONW,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_LEFT_JUST,								// iLeft
			LSN_VR_AUDIO_T,								// iTop
			LSN_VR_GROUP_W,								// dwWidth
			LSN_VR_AUDIO_H,								// dwHeight
			LSN_GROUPSTYLE,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			LSN_LSTR( LSN_VR_AUDIO ),					// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_LABEL,								// ltType
			CWinUtilities::LSN_VRWI_CODEC_LABEL,		// wId
			nullptr,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_LABEL_L,								// iLeft
			LSN_VR_CODEC_T + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),	// iTop
			LSN_VR_LABEL_W,								// dwWidth
			LSN_DEF_STATIC_HEIGHT,						// dwHeight
			LSN_STATICSTYLE,							// dwStyle
			0,											// dwStyleEx
			LSN_LSTR( LSN_VR_CODEC_ ),					// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_COMBOBOX,							// ltType
			CWinUtilities::LSN_VRWI_CODEC_COMBO,		// wId
			WC_COMBOBOXW,								// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_CTRL_L,								// iLeft
			LSN_VR_CODEC_T,								// iTop
			LSN_VR_CTRL_W,								// dwWidth
			LSN_DEF_COMBO_HEIGHT,						// dwHeight
			LSN_COMBOSTYLE_LIST,						// dwStyle
			LSN_COMBOSTYLEEX_LIST,						// dwStyleEx
			nullptr,									// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_LABEL,								// ltType
			CWinUtilities::LSN_VRWI_BITRATE_LABEL,		// wId
			nullptr,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_LABEL_L,								// iLeft
			LSN_VR_BITRATE_T + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),	// iTop
			LSN_VR_LABEL_W,								// dwWidth
			LSN_DEF_STATIC_HEIGHT,						// dwHeight
			LSN_STATICSTYLE,							// dwStyle
			0,											// dwStyleEx
			LSN_LSTR( LSN_VR_BITRATE_ ),				// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_LABEL,								// ltType
			CWinUtilities::LSN_VRWI_BITRATE_HINT,		// wId
			nullptr,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_CTRL_L,								// iLeft
			LSN_VR_BITRATE_T + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),	// iTop
			LSN_VR_CTRL_W,								// dwWidth
			LSN_DEF_STATIC_HEIGHT,						// dwHeight
			LSN_STATICSTYLE,							// dwStyle
			0,											// dwStyleEx
			nullptr,									// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_TRACKBAR,							// ltType
			CWinUtilities::LSN_VRWI_BITRATE_TRACKBAR,	// wId
			nullptr,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_CTRL_L,								// iLeft
			LSN_VR_BITRATE_TRACK_T,						// iTop
			LSN_VR_CTRL_W,								// dwWidth
			LSN_DEF_TRACKBAR_HEIGHT,					// dwHeight
			LSN_VR_TRACKBAR_STYLE,						// dwStyle
			0,											// dwStyleEx
			nullptr,									// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_EDIT,								// ltType
			CWinUtilities::LSN_VRWI_BITRATE_EDIT,		// wId
			nullptr,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_RIGHT_L,								// iLeft
			LSN_VR_BITRATE_TRACK_T + ((LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_EDIT_HEIGHT) >> 1),	// iTop
			LSN_VR_RIGHT_W,								// dwWidth
			LSN_DEF_EDIT_HEIGHT,						// dwHeight
			LSN_EDITSTYLE,								// dwStyle
			WS_EX_CLIENTEDGE,							// dwStyleEx
			nullptr,									// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},

		// == Conditions.
		{
			LSW_LT_GROUPBOX,							// ltType
			CWinUtilities::LSN_VRWI_CONDITIONS_GROUP,	// wId
			WC_BUTTONW,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_LEFT_JUST,								// iLeft
			LSN_VR_COND_T,								// iTop
			LSN_VR_GROUP_W,								// dwWidth
			LSN_VR_COND_H,								// dwHeight
			LSN_GROUPSTYLE,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			LSN_LSTR( LSN_VR_CONDITIONS ),				// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_LABEL,								// ltType
			CWinUtilities::LSN_VRWI_START_COND_LABEL,	// wId
			nullptr,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_COND_L( 0 ),							// iLeft
			LSN_VR_COND_ROW_T + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),	// iTop
			LSN_VR_COND_W( 0 ),							// dwWidth
			LSN_DEF_STATIC_HEIGHT,						// dwHeight
			LSN_STATICSTYLE,							// dwStyle
			0,											// dwStyleEx
			LSN_LSTR( LSN_VR_START_CONDITION_ ),		// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_COMBOBOX,							// ltType
			CWinUtilities::LSN_VRWI_START_COND_COMBO,	// wId
			WC_COMBOBOXW,								// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_COND_L( 1 ),							// iLeft
			LSN_VR_COND_ROW_T,							// iTop
			LSN_VR_COND_W( 1 ),							// dwWidth
			LSN_DEF_COMBO_HEIGHT,						// dwHeight
			LSN_COMBOSTYLE_LIST,						// dwStyle
			LSN_COMBOSTYLEEX_LIST,						// dwStyleEx
			nullptr,									// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_LABEL,								// ltType
			CWinUtilities::LSN_VRWI_END_COND_LABEL,		// wId
			nullptr,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_COND_L( 2 ),							// iLeft
			LSN_VR_COND_ROW_T + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),	// iTop
			LSN_VR_COND_W( 2 ),							// dwWidth
			LSN_DEF_STATIC_HEIGHT,						// dwHeight
			LSN_STATICSTYLE,							// dwStyle
			0,											// dwStyleEx
			LSN_LSTR( LSN_VR_END_CONDITION_ ),			// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_COMBOBOX,							// ltType
			CWinUtilities::LSN_VRWI_END_COND_COMBO,		// wId
			WC_COMBOBOXW,								// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_COND_L( 3 ),							// iLeft
			LSN_VR_COND_ROW_T,							// iTop
			LSN_VR_COND_W( 3 ),							// dwWidth
			LSN_DEF_COMBO_HEIGHT,						// dwHeight
			LSN_COMBOSTYLE_LIST,						// dwStyle
			LSN_COMBOSTYLEEX_LIST,						// dwStyleEx
			nullptr,									// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_COMBOBOX,							// ltType
			CWinUtilities::LSN_VRWI_START_COMBO,		// wId
			WC_COMBOBOXW,								// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_COND_L( 0 ),							// iLeft
			LSN_VR_HISTORY_ROW_T,						// iTop
			LSN_VR_COND2_W( 0 ),						// dwWidth
			LSN_DEF_COMBO_HEIGHT,						// dwHeight
			LSN_COMBOSTYLE,								// dwStyle
			0,											// dwStyleEx
			nullptr,									// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},
		{
			LSW_LT_COMBOBOX,							// ltType
			CWinUtilities::LSN_VRWI_END_COMBO,			// wId
			WC_COMBOBOXW,								// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_COND_L( 2 ),							// iLeft
			LSN_VR_HISTORY_ROW_T,						// iTop
			LSN_VR_COND2_W( 1 ),						// dwWidth
			LSN_DEF_COMBO_HEIGHT,						// dwHeight
			LSN_COMBOSTYLE,								// dwStyle
			0,											// dwStyleEx
			nullptr,									// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId
		},

		// == Buttons.
		{
			LSW_LT_BUTTON,								// ltType
			CWinUtilities::LSN_VRWI_OK,					// wId
			WC_BUTTONW,									// lpwcClass
			TRUE,										// bEnabled
			TRUE,										// bActive
			LSN_LEFT_JUST,								// iLeft
			LSN_VR_BUTTONS_T,							// iTop
			LSN_DEF_BUTTON_WIDTH,						// dwWidth
			LSN_DEF_BUTTON_HEIGHT,						// dwHeight
			LSN_DEFBUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			LSN_LSTR( LSN_OK ),							// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId

			LSN_LOCK_LEFT,								// pcLeftSizeExp
			nullptr, 0,									// pcRightSizeExp
			nullptr, 0,									// pcTopSizeExp
			LSN_LOCK_BOTTOM,							// pcBottomSizeExp
			LSN_FIXED_WIDTH,							// pcWidthSizeExp
			LSN_FIXED_HEIGHT,							// pcHeightSizeExp
		},
		{
			LSW_LT_BUTTON,								// ltType
			CWinUtilities::LSN_VRWI_DEFAULTS,			// wId
			WC_BUTTONW,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			((LSN_VR_W - LSN_VR_DEFAULTS_W) >> 1),		// iLeft
			LSN_VR_BUTTONS_T,							// iTop
			LSN_VR_DEFAULTS_W,							// dwWidth
			LSN_DEF_BUTTON_HEIGHT,						// dwHeight
			LSN_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			LSN_LSTR( LSN_VR_DEFAULTS ),				// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId

			nullptr, 0,									// pcLeftSizeExp
			nullptr, 0,									// pcRightSizeExp
			nullptr, 0,									// pcTopSizeExp
			LSN_LOCK_BOTTOM,							// pcBottomSizeExp
			LSN_FIXED_WIDTH,							// pcWidthSizeExp
			LSN_FIXED_HEIGHT,							// pcHeightSizeExp
		},
		{
			LSW_LT_BUTTON,								// ltType
			CWinUtilities::LSN_VRWI_CANCEL,				// wId
			WC_BUTTONW,									// lpwcClass
			TRUE,										// bEnabled
			FALSE,										// bActive
			LSN_VR_W - LSN_DEF_BUTTON_WIDTH - LSN_LEFT_JUST,	// iLeft
			LSN_VR_BUTTONS_T,							// iTop
			LSN_DEF_BUTTON_WIDTH,						// dwWidth
			LSN_DEF_BUTTON_HEIGHT,						// dwHeight
			LSN_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			LSN_LSTR( LSN_CANCEL ),						// pwcText
			0,											// sTextLen
			CWinUtilities::LSN_VRWI_DIALOG,				// dwParentId

			nullptr, 0,									// pcLeftSizeExp
			LSN_LOCK_RIGHT,								// pcRightSizeExp
			nullptr, 0,									// pcTopSizeExp
			LSN_LOCK_BOTTOM,							// pcBottomSizeExp
			LSN_FIXED_WIDTH,							// pcWidthSizeExp
			LSN_FIXED_HEIGHT,							// pcHeightSizeExp
		},
	};

#undef LSN_VR_H
#undef LSN_VR_BUTTONS_T
#undef LSN_VR_COND2_W
#undef LSN_VR_COND_W
#undef LSN_VR_COND_L
#undef LSN_VR_HISTORY_ROW_T
#undef LSN_VR_COND_ROW_T
#undef LSN_VR_COND_H
#undef LSN_VR_COND_T
#undef LSN_VR_BITRATE_TRACK_T
#undef LSN_VR_BITRATE_T
#undef LSN_VR_CODEC_T
#undef LSN_VR_AUDIO_H
#undef LSN_VR_AUDIO_T
#undef LSN_VR_BLEND_T
#undef LSN_VR_FPS_T
#undef LSN_VR_PIXEL_FORMAT_T
#undef LSN_VR_ALT_QUALITY_TRACK_T
#undef LSN_VR_ALT_QUALITY_T
#undef LSN_VR_QUALITY_TRACK_T
#undef LSN_VR_QUALITY_T
#undef LSN_VR_PRESET_T
#undef LSN_VR_FORMAT_T
#undef LSN_VR_VIDEO_H
#undef LSN_VR_VIDEO_T
#undef LSN_VR_FFMPEG_T
#undef LSN_VR_FOLDER_T
#undef LSN_VR_OUTPUT_H
#undef LSN_VR_OUTPUT_T
#undef LSN_VR_TRACKBAR_STYLE
#undef LSN_VR_DEFAULTS_W
#undef LSN_VR_TRACK_ROW_H
#undef LSN_VR_ROW_H
#undef LSN_VR_CONTENT_W
#undef LSN_VR_GROUP_W
#undef LSN_VR_W
#undef LSN_VR_RIGHT_W
#undef LSN_VR_RIGHT_L
#undef LSN_VR_CTRL_W
#undef LSN_VR_CTRL_L
#undef LSN_VR_LABEL_W
#undef LSN_VR_LABEL_L


	// == Functions.
	/**
	 * Creates the Video Recording Settings dialog.
	 *
	 * \param _pwParent The parent of the dialog.
	 * \param _veoOptions The options to edit.  They are changed only if the dialog is closed with OK.
	 * \param _rConsoleRate The frame rate of the current console.
	 * \param _wsDefaultFolder The folder into which movies are saved when no folder is set.
	 * \return Returns TRUE if the dialog was closed with OK.
	 */
	BOOL CVideoRecordingWindowLayout::CreateVideoRecordingDialog( CWidget * _pwParent, LSN_VIDEO_EXPORT_OPTIONS &_veoOptions,
		CVideoExporter::LSN_RATE _rConsoleRate, const std::wstring &_wsDefaultFolder ) {
		LSN_VIDEO_RECORDING_DATA vrdData;
		vrdData.pveoOptions = &_veoOptions;
		vrdData.rConsoleRate = _rConsoleRate;
		try {
			vrdData.wsDefaultFolder = _wsDefaultFolder;
		}
		catch ( ... ) { return FALSE; }

		lsn::CLayoutManager * plmLayout = static_cast<lsn::CLayoutManager *>(lsw::CBase::LayoutManager());
		INT_PTR ipProc = plmLayout->DialogBoxX( m_wlWindow, std::size( m_wlWindow ), _pwParent, reinterpret_cast<uint64_t>(&vrdData) );
		return ipProc == 1 ? TRUE : FALSE;
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

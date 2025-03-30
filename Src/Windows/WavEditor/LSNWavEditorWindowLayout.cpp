

#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The layout for the WAV-Editor dialog.
 */
 
#include "LSNWavEditorWindowLayout.h"
#include "../../Localization/LSNLocalization.h"
#include "../Layout/LSNLayoutMacros.h"
#include "../Layout/LSNLayoutManager.h"
#include "LSNWavEditorWindow.h"

#include <ListBox/LSWListBox.h>

#include "../../../resource.h"


#define LSN_SEQUE_LABEL_0_W									60
#define LSN_SEQUE_TIME_EDIT_0								200
#define LSN_SEQUE_TIME_EDIT_1								125
#define LSN_SEQUE_LOOP_SECONDS_W							35
#define LSN_FALLOFF_W										(LSN_SEQUE_TIME_EDIT_0 - LSN_SEQUE_TIME_EDIT_1 - LSN_LEFT_JUST - LSN_SEQUE_LOOP_SECONDS_W)
#define LSN_SEQUE_TIME_GROUP_W								((LSN_GROUP_LEFT * 2) + LSN_SEQUE_LABEL_0_W + LSN_LEFT_JUST + LSN_SEQUE_TIME_EDIT_0 + LSN_LEFT_JUST + LSN_SEQUE_TIME_EDIT_1 + (LSN_GROUP_RIGHT * 2))

#define LSN_SEQUE_RANGE_GROUP_T								(LSN_GROUP_TOP)
#define LSN_SEQUE_RANGE_GROUP_H								(LSN_GROUP_TOP + (LSN_DEF_COMBO_HEIGHT) + (LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT) + LSN_GROUP_BOTTOM)
#define LSN_SEQUE_LOOP_GROUP_T								(LSN_SEQUE_RANGE_GROUP_T + LSN_SEQUE_RANGE_GROUP_H)
#define LSN_SEQUE_LOOP_GROUP_H								(LSN_GROUP_TOP + (LSN_DEF_BUTTON_HEIGHT) + ((LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT) * 1) + ((LSN_TOP_JUST + LSN_DEF_EDIT_HEIGHT) * 1) + (LSN_TOP_JUST + (LSN_DEF_STATIC_HEIGHT * 2)) + LSN_GROUP_BOTTOM)

#define LSN_SEQUE_SIL_GROUP_T								(LSN_SEQUE_LOOP_GROUP_T + LSN_SEQUE_LOOP_GROUP_H)
#define LSN_SEQUE_SIL_GROUP_H								(LSN_GROUP_TOP + LSN_DEF_EDIT_HEIGHT + LSN_GROUP_BOTTOM)

#define LSN_OPER_GROUP_T									(LSN_SEQUE_SIL_GROUP_T + LSN_SEQUE_SIL_GROUP_H)
#define LSN_OPER_GROUP_H									(LSN_GROUP_TOP + LSN_OPER_TREE_H + LSN_GROUP_BOTTOM)
#define LSN_OPER_TREE_W										(LSN_INNER_GROUP_W - LSN_GROUP_LEFT - LSN_GROUP_RIGHT) - LSN_LEFT_JUST - (LSN_DEF_BUTTON_WIDTH * 2)
#define LSN_OPER_TREE_H										(LSN_DEF_BUTTON_HEIGHT + LSN_TOP_JUST + LSN_DEF_BUTTON_HEIGHT + (LSN_TOP_JUST * 5) + LSN_DEF_BUTTON_HEIGHT + LSN_TOP_JUST + LSN_DEF_BUTTON_HEIGHT)

#define LSN_SEQUE_TIME_GROUP_H								(LSN_OPER_GROUP_T + LSN_OPER_GROUP_H + LSN_GROUP_BOTTOM)
#define LSN_SEQUE_TIME_W									(LSN_GROUP_LEFT + LSN_SEQUE_TIME_GROUP_W + LSN_GROUP_LEFT)
#define LSN_SEQUE_TIME_H									(LSN_TOP_JUST + LSN_SEQUE_TIME_GROUP_H + LSN_TOP_JUST)
#define LSN_SEQUE_TIME_L									0/*LSN_LEFT_JUST*/
#define LSN_INNER_GROUP_L									(LSN_SEQUE_TIME_L + LSN_GROUP_LEFT)
#define LSN_INNER_GROUP_W									(LSN_SEQUE_TIME_GROUP_W - LSN_GROUP_LEFT - LSN_GROUP_RIGHT)
#define LSN_SEQUE_LOOP_EDIT_W								(LSN_EVEN_DIVIDE_WIDTH_EX( LSN_INNER_GROUP_W - LSN_GROUP_LEFT - LSN_GROUP_RIGHT, LSN_INNER_GROUP_L + LSN_GROUP_LEFT, 2, 1, LSN_LEFT_JUST ) - (LSN_SEQUE_LABEL_0_W + LSN_LEFT_JUST) - (LSN_SEQUE_LOOP_SECONDS_W + LSN_LEFT_JUST) - LSN_LEFT_JUST)

#define LSN_SEQUE_LOOP_1_4_L								LSN_EVEN_DIVIDE_EX( LSN_INNER_GROUP_W - LSN_GROUP_LEFT - LSN_GROUP_RIGHT, LSN_INNER_GROUP_L + LSN_GROUP_LEFT, 4, 0, LSN_LEFT_JUST )
#define LSN_SEQUE_LOOP_1_4_W								LSN_EVEN_DIVIDE_WIDTH_EX( LSN_INNER_GROUP_W - LSN_GROUP_LEFT - LSN_GROUP_RIGHT, LSN_INNER_GROUP_L + LSN_GROUP_LEFT, 4, 0, LSN_LEFT_JUST )
#define LSN_SEQUE_LOOP_2_4_L								LSN_EVEN_DIVIDE_EX( LSN_INNER_GROUP_W - LSN_GROUP_LEFT - LSN_GROUP_RIGHT, LSN_INNER_GROUP_L + LSN_GROUP_LEFT, 4, 1, LSN_LEFT_JUST )
#define LSN_SEQUE_LOOP_2_4_W								LSN_EVEN_DIVIDE_WIDTH_EX( LSN_INNER_GROUP_W - LSN_GROUP_LEFT - LSN_GROUP_RIGHT, LSN_INNER_GROUP_L + LSN_GROUP_LEFT, 4, 1, LSN_LEFT_JUST )
#define LSN_SEQUE_LOOP_2_2_L								LSN_EVEN_DIVIDE_EX( LSN_INNER_GROUP_W - LSN_GROUP_LEFT - LSN_GROUP_RIGHT, LSN_INNER_GROUP_L + LSN_GROUP_LEFT, 2, 1, LSN_LEFT_JUST )
#define LSN_SEQUE_LOOP_2_2_W								LSN_EVEN_DIVIDE_WIDTH_EX( LSN_INNER_GROUP_W - LSN_GROUP_LEFT - LSN_GROUP_RIGHT, LSN_INNER_GROUP_L + LSN_GROUP_LEFT, 2, 1, LSN_LEFT_JUST )



#define LSN_FSETS_FDATA_GROUP_T								(LSN_GROUP_TOP)
#define LSN_FSETS_FDATA_GROUP_H								(LSN_GROUP_TOP + LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT + LSN_GROUP_BOTTOM)

#define LSN_FSETS_CHAR_GROUP_T								(LSN_FSETS_FDATA_GROUP_T + LSN_FSETS_FDATA_GROUP_H)
#define LSN_FSETS_CHAR_GROUP_H								(LSN_GROUP_TOP + LSN_DEF_COMBO_HEIGHT + ((LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT) * 4) + LSN_GROUP_BOTTOM)
#define LSN_FSETS_CHAR_BUTTON_L								((LSN_INNER_GROUP_L + LSN_INNER_GROUP_W) - LSN_FSETS_CHAR_BUTTON_W - (LSN_FSETS_CHAR_BUTTON_W + LSN_LEFT_JUST) - LSN_GROUP_RIGHT)
#define LSN_FSETS_CHAR_BUTTON_W								((LSN_SEQUE_TIME_EDIT_1 - LSN_LEFT_JUST) / 2)

#define LSN_FSETS_MDATA_GROUP_T								(LSN_FSETS_CHAR_GROUP_T + LSN_FSETS_CHAR_GROUP_H)
#define LSN_FSETS_MDATA_GROUP_H								(LSN_GROUP_TOP + LSN_DEF_EDIT_HEIGHT + ((LSN_TOP_JUST + LSN_DEF_EDIT_HEIGHT) * 3) + LSN_GROUP_BOTTOM)

#define LSN_FSETS_GROUP_W									LSN_SEQUE_TIME_GROUP_W
#define LSN_FSETS_GROUP_H									(LSN_FSETS_MDATA_GROUP_T + LSN_FSETS_MDATA_GROUP_H + (LSN_TOP_JUST + LSN_DEF_EDIT_HEIGHT) + LSN_GROUP_BOTTOM)


#define LSN_FILES_GROUP_L									LSN_LEFT_JUST_PXL
#define LSN_FILES_TREE_L									(LSN_FILES_GROUP_L + LSN_GROUP_LEFT_PXL)

#define LSN_FILES_BUTTONS_W									(LSN_DEF_BUTTON_WIDTH_PXL * 2)
#define LSN_FILES_BUTTONS_H									LSN_DEF_BUTTON_HEIGHT_PXL

#define LSN_FILES_TREE_W									((559) - (LSN_FILES_BUTTONS_W - LSN_LEFT_JUST_PXL))

#define LSN_FILES_BUTTONS_L									(LSN_FILES_TREE_L + LSN_FILES_TREE_W + LSN_LEFT_JUST_PXL)

#define LSN_FILES_TREE_H									((LSN_FILES_BUTTONS_H * 5) + (LSN_TOP_JUST_PXL * 4) + (LSN_TOP_JUST_PXL * 10))
#define LSN_FILES_GROUP_T									LSN_TOP_JUST_PXL

#define LSN_FILES_GROUP_W									(LSN_GROUP_LEFT_PXL + LSN_FILES_TREE_W + LSN_LEFT_JUST_PXL + LSN_FILES_BUTTONS_W + LSN_GROUP_RIGHT_PXL)
#define LSN_FILES_GROUP_H									(LSN_GROUP_TOP_PXL + LSN_FILES_TREE_H + LSN_GROUP_BOTTOM_PXL)

#define LSN_SEQUE_TIME_T									(LSN_FILES_GROUP_T + LSN_FILES_GROUP_H)


#define LSN_W												(LSN_FILES_GROUP_W + (LSN_LEFT_JUST_PXL * 2))
#define LSN_H												(LSN_FILES_GROUP_T + (LSN_FILES_GROUP_H + 80) + LSN_TOP_JUST_PXL)

namespace lsn {

	/** The layout for the "File Settings" dialog. */
	LSW_WIDGET_LAYOUT CWavEditorWindowLayout::m_wlFileSettingsDialog[] = {
		{
			LSN_LT_WAV_EDITOR_FILE_SETTINGS,				// ltType
			LSN_WEWI_FSETS,									// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			0,												// iLeft
			0,												// iTop
			LSN_FSETS_GROUP_W,								// dwWidth
			LSN_FSETS_GROUP_H,								// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | DS_3DLOOK | DS_FIXEDSYS | DS_SETFONT | DS_CONTROL | WS_CLIPSIBLINGS,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_CONTROLPARENT,														// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_WEWI_NONE,									// dwParentId
		},

		// File Settings.
		{
			LSW_LT_GROUPBOX,								// ltType
			LSN_WEWI_SEQ_GROUP,								// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			0,												// iLeft
			0,												// iTop
			LSN_FSETS_GROUP_W,								// dwWidth
			LSN_FSETS_GROUP_H,								// dwHeight
			LSN_GROUPSTYLE,									// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,													// dwStyleEx
			LSN_LSTR( LSN_WE_FILE_SETTINGS ),				// pwcText
			0,												// sTextLen
			LSN_WEWI_FSETS,									// dwParentId
		},

		// File Data.
		{
			LSW_LT_GROUPBOX,								// ltType
			LSN_WEWI_FSETS_FDATA_GROUP,						// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L,								// iLeft
			LSN_FSETS_FDATA_GROUP_T,						// iTop
			LSN_INNER_GROUP_W,								// dwWidth
			LSN_FSETS_FDATA_GROUP_H,						// dwHeight
			LSN_GROUPSTYLE,									// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																							// dwStyleEx
			LSN_LSTR( LSN_WE_FILE_DATA ),					// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		// Track Title.
		{
			LSW_LT_LABEL,									// ltType
			LSN_WEWI_FSETS_FDATA_NAME_LABEL,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT,				// iLeft
			LSN_FSETS_FDATA_GROUP_T + LSN_GROUP_TOP + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),																			// iTop
			LSN_SEQUE_LABEL_0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_WE_TRACK_TITLE ),					// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			LSN_WEWI_FSETS_FDATA_NAME_EDIT,					// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT + LSN_LEFT_JUST + LSN_SEQUE_LABEL_0_W,																								// iLeft
			LSN_FSETS_FDATA_GROUP_T + LSN_GROUP_TOP,		// iTop
			LSN_SEQUE_TIME_EDIT_0 + LSN_LEFT_JUST + LSN_SEQUE_TIME_EDIT_1,																											// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},

		// Actual Hz.
		{
			LSW_LT_LABEL,									// ltType
			LSN_WEWI_FSETS_FDATA_HZ_LABEL,					// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT,				// iLeft
			LSN_FSETS_FDATA_GROUP_T + LSN_GROUP_TOP + (LSN_TOP_JUST + LSN_DEF_EDIT_HEIGHT) + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),									// iTop
			LSN_SEQUE_LABEL_0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_WE_ACTUAL_HZ ),					// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			LSN_WEWI_FSETS_FDATA_HZ_COMBO,					// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT + LSN_SEQUE_LABEL_0_W + LSN_LEFT_JUST,																								// iLeft
			LSN_FSETS_FDATA_GROUP_T + LSN_GROUP_TOP + (LSN_TOP_JUST + LSN_DEF_EDIT_HEIGHT),																							// iTop
			LSN_SEQUE_TIME_EDIT_0,							// dwWidth
			LSN_DEF_COMBO_HEIGHT,							// dwHeight
			LSN_COMBOSTYLE_LIST,							// dwStyle
			LSN_COMBOSTYLEEX_LIST,							// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			LSN_WEWI_FSETS_FDATA_HZ_EDIT,					// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT + LSN_SEQUE_LABEL_0_W + LSN_LEFT_JUST + LSN_SEQUE_TIME_EDIT_0 + LSN_LEFT_JUST,														// iLeft
			LSN_FSETS_FDATA_GROUP_T + LSN_GROUP_TOP + (LSN_TOP_JUST + LSN_DEF_EDIT_HEIGHT) + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_EDIT_HEIGHT) >> 1),									// iTop
			LSN_SEQUE_TIME_EDIT_1,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},

		// Characteristics.
		{
			LSW_LT_GROUPBOX,								// ltType
			LSN_WEWI_FSETS_CHAR_GROUP,						// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L,								// iLeft
			LSN_FSETS_CHAR_GROUP_T,							// iTop
			LSN_INNER_GROUP_W,								// dwWidth
			LSN_FSETS_CHAR_GROUP_H,							// dwHeight
			LSN_GROUPSTYLE,									// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																																		// dwStyleEx
			LSN_LSTR( LSN_WE_CHARACTERISTICS ),				// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},

		// Presets.
		{
			LSW_LT_LABEL,									// ltType
			LSN_WEWI_FSETS_CHAR_PRESET_LABEL,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT,				// iLeft
			LSN_FSETS_CHAR_GROUP_T + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),																														// iTop
			LSN_SEQUE_LABEL_0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_WE_PRESETS ),						// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			LSN_WEWI_FSETS_CHAR_PRESET_COMBO,				// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT + LSN_SEQUE_LABEL_0_W + LSN_LEFT_JUST,																																			// iLeft
			LSN_FSETS_CHAR_GROUP_T + LSN_GROUP_TOP,			// iTop
			LSN_SEQUE_TIME_EDIT_0,							// dwWidth
			LSN_DEF_COMBO_HEIGHT,							// dwHeight
			LSN_COMBOSTYLE_LIST,							// dwStyle
			LSN_COMBOSTYLEEX_LIST,							// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_BUTTON,									// ltType
			LSN_WEWI_FSETS_CHAR_SAVE_BUTTON,				// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_FSETS_CHAR_BUTTON_L,																																															// iLeft
			LSN_FSETS_CHAR_GROUP_T + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_BUTTON_HEIGHT) >> 1),																														// iTop
			LSN_FSETS_CHAR_BUTTON_W,						// dwWidth
			LSN_DEF_BUTTON_HEIGHT,							// dwHeight
			LSN_BUTTONSTYLE,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																																		// dwStyleEx
			LSN_LSTR( LSN_WE_SAVE ),						// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_BUTTON,									// ltType
			LSN_WEWI_FSETS_CHAR_LOAD_BUTTON,				// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_FSETS_CHAR_BUTTON_L + LSN_FSETS_CHAR_BUTTON_W + LSN_LEFT_JUST,																																					// iLeft
			LSN_FSETS_CHAR_GROUP_T + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_BUTTON_HEIGHT) >> 1),																														// iTop
			LSN_FSETS_CHAR_BUTTON_W,						// dwWidth
			LSN_DEF_BUTTON_HEIGHT,							// dwHeight
			LSN_BUTTONSTYLE,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																																		// dwStyleEx
			LSN_LSTR( LSN_WE_LOAD ),						// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},

		// LPF.
		{
			LSW_LT_CHECK,									// ltType
			LSN_WEWI_FSETS_CHAR_LPF_CHECK,					// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT,				// iLeft
			LSN_FSETS_CHAR_GROUP_T + LSN_GROUP_TOP + (LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT) + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1),																				// iTop
			LSN_SEQUE_LABEL_0_W,							// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_WE_LPF ),							// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			LSN_WEWI_FSETS_CHAR_LPF_EDIT,					// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT + LSN_SEQUE_LABEL_0_W + LSN_LEFT_JUST,																																			// iLeft
			LSN_FSETS_CHAR_GROUP_T + LSN_GROUP_TOP + (LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT) + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_EDIT_HEIGHT) >> 1),																				// iTop
			LSN_SEQUE_TIME_EDIT_1,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			LSN_WEWI_FSETS_CHAR_LPF_TYPE_COMBO,				// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT + LSN_SEQUE_LABEL_0_W + LSN_LEFT_JUST + LSN_SEQUE_TIME_EDIT_1 + LSN_LEFT_JUST,																									// iLeft
			LSN_FSETS_CHAR_GROUP_T + LSN_GROUP_TOP + (LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT),																																		// iTop
			LSN_SEQUE_LOOP_SECONDS_W,						// dwWidth
			LSN_DEF_COMBO_HEIGHT,							// dwHeight
			LSN_COMBOSTYLE_LIST,							// dwStyle
			LSN_COMBOSTYLEEX_LIST,							// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			LSN_WEWI_FSETS_CHAR_LPF_FALLOFF_LABEL,			// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT + LSN_SEQUE_LABEL_0_W + LSN_LEFT_JUST + LSN_SEQUE_TIME_EDIT_1 + LSN_LEFT_JUST + LSN_SEQUE_LOOP_SECONDS_W + LSN_LEFT_JUST,														// iLeft
			LSN_FSETS_CHAR_GROUP_T + LSN_GROUP_TOP + (LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT) + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),																				// iTop
			LSN_FALLOFF_W,									// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_WE_FALLOFF ),						// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			LSN_WEWI_FSETS_CHAR_LPF_FALLOFF_EDIT,			// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_FSETS_CHAR_BUTTON_L,						// iLeft
			LSN_FSETS_CHAR_GROUP_T + LSN_GROUP_TOP + (LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT) + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_EDIT_HEIGHT) >> 1),																				// iTop
			LSN_FSETS_CHAR_BUTTON_W,						// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			LSN_WEWI_FSETS_CHAR_LPF_DB_LABEL,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_FSETS_CHAR_BUTTON_L + LSN_FSETS_CHAR_BUTTON_W + LSN_LEFT_JUST,																																					// iLeft
			LSN_FSETS_CHAR_GROUP_T + LSN_GROUP_TOP + (LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT) + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),																				// iTop
			LSN_FSETS_CHAR_BUTTON_W,						// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_WE_DB_PER_OCTAVE ),				// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},


		// HPF0.
		{
			LSW_LT_CHECK,									// ltType
			LSN_WEWI_FSETS_CHAR_HPF0_CHECK,					// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT,				// iLeft
			LSN_FSETS_CHAR_GROUP_T + LSN_GROUP_TOP + ((LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT) * 2) + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1),																		// iTop
			LSN_SEQUE_LABEL_0_W,							// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_WE_HPF0 ),						// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			LSN_WEWI_FSETS_CHAR_HPF0_EDIT,					// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT + LSN_SEQUE_LABEL_0_W + LSN_LEFT_JUST,																																			// iLeft
			LSN_FSETS_CHAR_GROUP_T + LSN_GROUP_TOP + ((LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT) * 2) + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_EDIT_HEIGHT) >> 1),																			// iTop
			LSN_SEQUE_TIME_EDIT_1,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			LSN_WEWI_FSETS_CHAR_HPF0_TYPE_COMBO,			// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT + LSN_SEQUE_LABEL_0_W + LSN_LEFT_JUST + LSN_SEQUE_TIME_EDIT_1 + LSN_LEFT_JUST,																									// iLeft
			LSN_FSETS_CHAR_GROUP_T + LSN_GROUP_TOP + ((LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT) * 2),																																// iTop
			LSN_SEQUE_LOOP_SECONDS_W,						// dwWidth
			LSN_DEF_COMBO_HEIGHT,							// dwHeight
			LSN_COMBOSTYLE_LIST,							// dwStyle
			LSN_COMBOSTYLEEX_LIST,							// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			LSN_WEWI_FSETS_CHAR_HPF0_FALLOFF_LABEL,			// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT + LSN_SEQUE_LABEL_0_W + LSN_LEFT_JUST + LSN_SEQUE_TIME_EDIT_1 + LSN_LEFT_JUST + LSN_SEQUE_LOOP_SECONDS_W + LSN_LEFT_JUST,														// iLeft
			LSN_FSETS_CHAR_GROUP_T + LSN_GROUP_TOP + ((LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT) * 2) + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),																		// iTop
			LSN_FALLOFF_W,									// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_WE_FALLOFF ),						// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			LSN_WEWI_FSETS_CHAR_HPF0_FALLOFF_EDIT,			// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_FSETS_CHAR_BUTTON_L,						// iLeft
			LSN_FSETS_CHAR_GROUP_T + LSN_GROUP_TOP + ((LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT) * 2) + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_EDIT_HEIGHT) >> 1),																			// iTop
			LSN_FSETS_CHAR_BUTTON_W,						// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			LSN_WEWI_FSETS_CHAR_HPF0_DB_LABEL,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_FSETS_CHAR_BUTTON_L + LSN_FSETS_CHAR_BUTTON_W + LSN_LEFT_JUST,																																					// iLeft
			LSN_FSETS_CHAR_GROUP_T + LSN_GROUP_TOP + ((LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT) * 2) + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),																		// iTop
			LSN_FSETS_CHAR_BUTTON_W,						// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_WE_DB_PER_OCTAVE ),				// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},

		// HPF1.
		{
			LSW_LT_CHECK,									// ltType
			LSN_WEWI_FSETS_CHAR_HPF1_CHECK,					// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT,				// iLeft
			LSN_FSETS_CHAR_GROUP_T + LSN_GROUP_TOP + ((LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT) * 3) + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1),																		// iTop
			LSN_SEQUE_LABEL_0_W,							// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_WE_HPF1 ),						// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			LSN_WEWI_FSETS_CHAR_HPF1_EDIT,					// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT + LSN_SEQUE_LABEL_0_W + LSN_LEFT_JUST,																																			// iLeft
			LSN_FSETS_CHAR_GROUP_T + LSN_GROUP_TOP + ((LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT) * 3) + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_EDIT_HEIGHT) >> 1),																			// iTop
			LSN_SEQUE_TIME_EDIT_1,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			LSN_WEWI_FSETS_CHAR_HPF1_TYPE_COMBO,			// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT + LSN_SEQUE_LABEL_0_W + LSN_LEFT_JUST + LSN_SEQUE_TIME_EDIT_1 + LSN_LEFT_JUST,																									// iLeft
			LSN_FSETS_CHAR_GROUP_T + LSN_GROUP_TOP + ((LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT) * 3),																																// iTop
			LSN_SEQUE_LOOP_SECONDS_W,						// dwWidth
			LSN_DEF_COMBO_HEIGHT,							// dwHeight
			LSN_COMBOSTYLE_LIST,							// dwStyle
			LSN_COMBOSTYLEEX_LIST,							// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			LSN_WEWI_FSETS_CHAR_HPF1_FALLOFF_LABEL,			// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT + LSN_SEQUE_LABEL_0_W + LSN_LEFT_JUST + LSN_SEQUE_TIME_EDIT_1 + LSN_LEFT_JUST + LSN_SEQUE_LOOP_SECONDS_W + LSN_LEFT_JUST,														// iLeft
			LSN_FSETS_CHAR_GROUP_T + LSN_GROUP_TOP + ((LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT) * 3) + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),																		// iTop
			LSN_FALLOFF_W,									// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_WE_FALLOFF ),						// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			LSN_WEWI_FSETS_CHAR_HPF1_FALLOFF_EDIT,			// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_FSETS_CHAR_BUTTON_L,						// iLeft
			LSN_FSETS_CHAR_GROUP_T + LSN_GROUP_TOP + ((LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT) * 3) + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_EDIT_HEIGHT) >> 1),																			// iTop
			LSN_FSETS_CHAR_BUTTON_W,						// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			LSN_WEWI_FSETS_CHAR_HPF1_DB_LABEL,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_FSETS_CHAR_BUTTON_L + LSN_FSETS_CHAR_BUTTON_W + LSN_LEFT_JUST,																																					// iLeft
			LSN_FSETS_CHAR_GROUP_T + LSN_GROUP_TOP + ((LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT) * 3) + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),																		// iTop
			LSN_FSETS_CHAR_BUTTON_W,						// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_WE_DB_PER_OCTAVE ),				// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},

		// HPF2.
		{
			LSW_LT_CHECK,									// ltType
			LSN_WEWI_FSETS_CHAR_HPF2_CHECK,					// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT,				// iLeft
			LSN_FSETS_CHAR_GROUP_T + LSN_GROUP_TOP + ((LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT) * 4) + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_CHECK_HEIGHT) >> 1),																		// iTop
			LSN_SEQUE_LABEL_0_W,							// dwWidth
			LSN_DEF_CHECK_HEIGHT,							// dwHeight
			LSN_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_WE_HPF2 ),						// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			LSN_WEWI_FSETS_CHAR_HPF2_EDIT,					// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT + LSN_SEQUE_LABEL_0_W + LSN_LEFT_JUST,																																			// iLeft
			LSN_FSETS_CHAR_GROUP_T + LSN_GROUP_TOP + ((LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT) * 4) + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_EDIT_HEIGHT) >> 1),																			// iTop
			LSN_SEQUE_TIME_EDIT_1,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			LSN_WEWI_FSETS_CHAR_HPF2_TYPE_COMBO,			// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT + LSN_SEQUE_LABEL_0_W + LSN_LEFT_JUST + LSN_SEQUE_TIME_EDIT_1 + LSN_LEFT_JUST,																									// iLeft
			LSN_FSETS_CHAR_GROUP_T + LSN_GROUP_TOP + ((LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT) * 4),																																// iTop
			LSN_SEQUE_LOOP_SECONDS_W,						// dwWidth
			LSN_DEF_COMBO_HEIGHT,							// dwHeight
			LSN_COMBOSTYLE_LIST,							// dwStyle
			LSN_COMBOSTYLEEX_LIST,							// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			LSN_WEWI_FSETS_CHAR_HPF2_FALLOFF_LABEL,			// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT + LSN_SEQUE_LABEL_0_W + LSN_LEFT_JUST + LSN_SEQUE_TIME_EDIT_1 + LSN_LEFT_JUST + LSN_SEQUE_LOOP_SECONDS_W + LSN_LEFT_JUST,														// iLeft
			LSN_FSETS_CHAR_GROUP_T + LSN_GROUP_TOP + ((LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT) * 4) + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),																		// iTop
			LSN_FALLOFF_W,									// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_WE_FALLOFF ),						// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			LSN_WEWI_FSETS_CHAR_HPF2_FALLOFF_EDIT,			// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_FSETS_CHAR_BUTTON_L,						// iLeft
			LSN_FSETS_CHAR_GROUP_T + LSN_GROUP_TOP + ((LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT) * 4) + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_EDIT_HEIGHT) >> 1),																			// iTop
			LSN_FSETS_CHAR_BUTTON_W,						// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			LSN_WEWI_FSETS_CHAR_HPF2_DB_LABEL,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_FSETS_CHAR_BUTTON_L + LSN_FSETS_CHAR_BUTTON_W + LSN_LEFT_JUST,																																					// iLeft
			LSN_FSETS_CHAR_GROUP_T + LSN_GROUP_TOP + ((LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT) * 4) + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),																		// iTop
			LSN_FSETS_CHAR_BUTTON_W,						// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_WE_DB_PER_OCTAVE ),				// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},

		// Meta Data.
		{
			LSW_LT_GROUPBOX,								// ltType
			LSN_WEWI_FSETS_MDATA_GROUP,						// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L,								// iLeft
			LSN_FSETS_MDATA_GROUP_T,						// iTop
			LSN_INNER_GROUP_W,								// dwWidth
			LSN_FSETS_MDATA_GROUP_H,						// dwHeight
			LSN_GROUPSTYLE,									// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																							// dwStyleEx
			LSN_LSTR( LSN_WE_METADATA ),					// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},

		// Artist.
		{
			LSW_LT_LABEL,									// ltType
			LSN_WEWI_FSETS_MDATA_ARTIST_LABEL,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT,				// iLeft
			LSN_FSETS_MDATA_GROUP_T + LSN_GROUP_TOP + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),																			// iTop
			LSN_SEQUE_LABEL_0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_WE_ARTIST ),						// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			LSN_WEWI_FSETS_MDATA_ARTIST_EDIT,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT + LSN_LEFT_JUST + LSN_SEQUE_LABEL_0_W,																								// iLeft
			LSN_FSETS_MDATA_GROUP_T + LSN_GROUP_TOP,		// iTop
			LSN_SEQUE_TIME_EDIT_0 + LSN_LEFT_JUST + LSN_SEQUE_TIME_EDIT_1,																											// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},

		// Album.
		{
			LSW_LT_LABEL,									// ltType
			LSN_WEWI_FSETS_MDATA_ALBUM_LABEL,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT,				// iLeft
			LSN_FSETS_MDATA_GROUP_T + LSN_GROUP_TOP + ((LSN_TOP_JUST + LSN_DEF_EDIT_HEIGHT) * 1) + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),							// iTop
			LSN_SEQUE_LABEL_0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_WE_ALBUM ),						// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			LSN_WEWI_FSETS_MDATA_ALBUM_EDIT,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT + LSN_LEFT_JUST + LSN_SEQUE_LABEL_0_W,																								// iLeft
			LSN_FSETS_MDATA_GROUP_T + LSN_GROUP_TOP + ((LSN_TOP_JUST + LSN_DEF_EDIT_HEIGHT) * 1),																					// iTop
			LSN_SEQUE_TIME_EDIT_0 + LSN_LEFT_JUST + LSN_SEQUE_TIME_EDIT_1,																											// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},

		// Year.
		{
			LSW_LT_LABEL,									// ltType
			LSN_WEWI_FSETS_MDATA_YEAR_LABEL,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT,				// iLeft
			LSN_FSETS_MDATA_GROUP_T + LSN_GROUP_TOP + ((LSN_TOP_JUST + LSN_DEF_EDIT_HEIGHT) * 2) + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),							// iTop
			LSN_SEQUE_LABEL_0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_WE_YEAR ),						// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			LSN_WEWI_FSETS_MDATA_YEAR_EDIT,					// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT + LSN_LEFT_JUST + LSN_SEQUE_LABEL_0_W,																								// iLeft
			LSN_FSETS_MDATA_GROUP_T + LSN_GROUP_TOP + ((LSN_TOP_JUST + LSN_DEF_EDIT_HEIGHT) * 2),																					// iTop
			LSN_SEQUE_TIME_EDIT_0 + LSN_LEFT_JUST + LSN_SEQUE_TIME_EDIT_1,																											// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},

		// Comments.
		{
			LSW_LT_LABEL,									// ltType
			LSN_WEWI_FSETS_MDATA_COMMENTS_LABEL,			// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT,				// iLeft
			LSN_FSETS_MDATA_GROUP_T + LSN_GROUP_TOP + ((LSN_TOP_JUST + LSN_DEF_EDIT_HEIGHT) * 3) + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),							// iTop
			LSN_SEQUE_LABEL_0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_WE_COMMENTS ),					// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			LSN_WEWI_FSETS_MDATA_COMMENTS_EDIT,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT + LSN_LEFT_JUST + LSN_SEQUE_LABEL_0_W,																								// iLeft
			LSN_FSETS_MDATA_GROUP_T + LSN_GROUP_TOP + ((LSN_TOP_JUST + LSN_DEF_EDIT_HEIGHT) * 3),																					// iTop
			LSN_SEQUE_TIME_EDIT_0 + LSN_LEFT_JUST + LSN_SEQUE_TIME_EDIT_1,																											// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},

		// Apply to All.
		{
			LSW_LT_BUTTON,									// ltType
			LSN_WEWI_FSETS_APPLY_TO_ALL_BUTTON,				// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L,								// iLeft
			LSN_FSETS_MDATA_GROUP_T + LSN_GROUP_TOP + ((LSN_TOP_JUST + LSN_DEF_EDIT_HEIGHT) * 4) + LSN_GROUP_BOTTOM,																// iTop
			LSN_INNER_GROUP_W,								// dwWidth
			LSN_DEF_BUTTON_HEIGHT,							// dwHeight
			LSN_BUTTONSTYLE,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																							// dwStyleEx
			LSN_LSTR( LSN_WE_APPLY_TO_ALL ),				// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
	};

	/** The layout for the "Sequencing" dialog. */
	LSW_WIDGET_LAYOUT CWavEditorWindowLayout::m_wlSequencingDialog[] = {
		{
			LSN_LT_WAV_EDITOR_SEQUENCING,					// ltType
			LSN_WEWI_SEQ,									// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			0,												// iLeft
			0,												// iTop
			LSN_SEQUE_TIME_GROUP_W + LSN_SEQUE_TIME_L,		// dwWidth
			LSN_SEQUE_TIME_H,								// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | DS_3DLOOK | DS_FIXEDSYS | DS_SETFONT | DS_CONTROL | WS_CLIPSIBLINGS,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_CONTROLPARENT,														// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_WEWI_NONE,									// dwParentId
		},

		// Sequencing.
		{
			LSW_LT_GROUPBOX,								// ltType
			LSN_WEWI_SEQ_GROUP,								// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_SEQUE_TIME_L,								// iLeft
			0,												// iTop
			LSN_SEQUE_TIME_GROUP_W,							// dwWidth
			LSN_SEQUE_TIME_GROUP_H,							// dwHeight
			LSN_GROUPSTYLE,									// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,													// dwStyleEx
			LSN_LSTR( LSN_WE_SEQUENCING ),					// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},

		// Range.
		{
			LSW_LT_GROUPBOX,								// ltType
			LSN_WEWI_SEQ_RANGE_GROUP,						// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L,								// iLeft
			LSN_SEQUE_RANGE_GROUP_T,						// iTop
			LSN_INNER_GROUP_W,						// dwWidth
			LSN_SEQUE_RANGE_GROUP_H,						// dwHeight
			LSN_GROUPSTYLE,									// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,													// dwStyleEx
			LSN_LSTR( LSN_WE_RANGE ),						// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},

		// Start Time.
		{
			LSW_LT_LABEL,									// ltType
			LSN_WEWI_SEQ_START_LABEL,						// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT,				// iLeft
			LSN_SEQUE_RANGE_GROUP_T + LSN_GROUP_TOP + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),								// iTop
			LSN_SEQUE_LABEL_0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_WE_START_TIME ),					// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			LSN_WEWI_SEQ_START_COMBO,						// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT + LSN_SEQUE_LABEL_0_W + LSN_LEFT_JUST,														// iLeft
			LSN_SEQUE_RANGE_GROUP_T + LSN_GROUP_TOP,		// iTop
			LSN_SEQUE_TIME_EDIT_0,							// dwWidth
			LSN_DEF_COMBO_HEIGHT,							// dwHeight
			LSN_COMBOSTYLE_LIST,							// dwStyle
			LSN_COMBOSTYLEEX_LIST,							// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			LSN_WEWI_SEQ_START_EDIT,						// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT + LSN_SEQUE_LABEL_0_W + LSN_LEFT_JUST + LSN_SEQUE_TIME_EDIT_0 + LSN_LEFT_JUST,				// iLeft
			LSN_SEQUE_RANGE_GROUP_T + LSN_GROUP_TOP,		// iTop
			LSN_SEQUE_TIME_EDIT_1,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},

		// End Time.
		{
			LSW_LT_LABEL,									// ltType
			LSN_WEWI_SEQ_END_LABEL,							// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT,				// iLeft
			LSN_SEQUE_RANGE_GROUP_T + LSN_GROUP_TOP + (LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT) + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),								// iTop
			LSN_SEQUE_LABEL_0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_WE_END_TIME ),					// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			LSN_WEWI_SEQ_END_COMBO,							// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT + LSN_SEQUE_LABEL_0_W + LSN_LEFT_JUST,																							// iLeft
			LSN_SEQUE_RANGE_GROUP_T + LSN_GROUP_TOP + (LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT),																						// iTop
			LSN_SEQUE_TIME_EDIT_0,							// dwWidth
			LSN_DEF_COMBO_HEIGHT,							// dwHeight
			LSN_COMBOSTYLE_LIST,							// dwStyle
			LSN_COMBOSTYLEEX_LIST,							// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			LSN_WEWI_SEQ_END_EDIT,							// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT + LSN_SEQUE_LABEL_0_W + LSN_LEFT_JUST + LSN_SEQUE_TIME_EDIT_0 + LSN_LEFT_JUST,				// iLeft
			LSN_SEQUE_RANGE_GROUP_T + LSN_GROUP_TOP + (LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT),																						// iTop
			LSN_SEQUE_TIME_EDIT_1,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},

		// Looping.
		{
			LSW_LT_GROUPBOX,								// ltType
			LSN_WEWI_SEQ_LOOPS_GROUP,						// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L,								// iLeft
			LSN_SEQUE_LOOP_GROUP_T,							// iTop
			LSN_INNER_GROUP_W,						// dwWidth
			LSN_SEQUE_LOOP_GROUP_H,							// dwHeight
			LSN_GROUPSTYLE,									// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,													// dwStyleEx
			LSN_LSTR( LSN_WE_LOOPS ),						// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},

		// Loop/One-Shot.
		{
			LSW_LT_RADIO,									// ltType
			LSN_WEWI_SEQ_LOOP_RADIO,						// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			TRUE,											// bActive
			LSN_SEQUE_LOOP_1_4_L,							// iLeft
			LSN_SEQUE_LOOP_GROUP_T + LSN_GROUP_TOP + ((LSN_DEF_BUTTON_HEIGHT - LSN_DEF_RADIO_HEIGHT) >> 1),																			// iTop
			LSN_SEQUE_LOOP_1_4_W,							// dwWidth
			LSN_DEF_RADIO_HEIGHT,							// dwHeight
			LSN_RADIOSTYLE,									// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																							// dwStyleEx
			LSN_LSTR( LSN_WE_LOOP ),						// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_RADIO,									// ltType
			LSN_WEWI_SEQ_ONE_SHOT_RADIO,					// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_SEQUE_LOOP_2_4_L,							// iLeft
			LSN_SEQUE_LOOP_GROUP_T + LSN_GROUP_TOP + ((LSN_DEF_BUTTON_HEIGHT - LSN_DEF_RADIO_HEIGHT) >> 1),																			// iTop
			LSN_SEQUE_LOOP_2_4_W,							// dwWidth
			LSN_DEF_RADIO_HEIGHT,							// dwHeight
			LSN_RADIOSTYLE,									// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																							// dwStyleEx
			LSN_LSTR( LSN_WE_ONE_SHOT ),					// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_BUTTON,									// ltType
			LSN_WEWI_SEQ_FIND_LOOPS_BUTTON,					// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_SEQUE_LOOP_2_2_L,							// iLeft
			LSN_SEQUE_LOOP_GROUP_T + LSN_GROUP_TOP,			// iTop
			LSN_SEQUE_LOOP_2_2_W,							// dwWidth
			LSN_DEF_BUTTON_HEIGHT,							// dwHeight
			LSN_BUTTONSTYLE,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																							// dwStyleEx
			LSN_LSTR( LSN_WE_FIND_LOOPS ),					// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},

		// End Time.
		{
			LSW_LT_LABEL,									// ltType
			LSN_WEWI_SEQ_LOOPS_END_LABEL,					// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT,				// iLeft
			LSN_SEQUE_LOOP_GROUP_T + LSN_GROUP_TOP + (LSN_TOP_JUST + LSN_DEF_BUTTON_HEIGHT) + ((LSN_DEF_COMBO_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),								// iTop
			LSN_SEQUE_LABEL_0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_WE_END_TIME ),					// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			LSN_WEWI_SEQ_LOOPS_END_COMBO,					// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT + LSN_SEQUE_LABEL_0_W + LSN_LEFT_JUST,																							// iLeft
			LSN_SEQUE_LOOP_GROUP_T + LSN_GROUP_TOP + (LSN_TOP_JUST + LSN_DEF_BUTTON_HEIGHT),																						// iTop
			LSN_SEQUE_TIME_EDIT_0,							// dwWidth
			LSN_DEF_COMBO_HEIGHT,							// dwHeight
			LSN_COMBOSTYLE_LIST,							// dwStyle
			LSN_COMBOSTYLEEX_LIST,							// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			LSN_WEWI_SEQ_LOOPS_END_EDIT,					// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT + LSN_SEQUE_LABEL_0_W + LSN_LEFT_JUST + LSN_SEQUE_TIME_EDIT_0 + LSN_LEFT_JUST,													// iLeft
			LSN_SEQUE_LOOP_GROUP_T + LSN_GROUP_TOP + (LSN_TOP_JUST + LSN_DEF_BUTTON_HEIGHT),																						// iTop
			LSN_SEQUE_TIME_EDIT_1,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},

		// Post-Loop Delay.
		{
			LSW_LT_LABEL,									// ltType
			LSN_WEWI_SEQ_LOOPS_DELAY_LABEL,					// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT,				// iLeft
			LSN_SEQUE_LOOP_GROUP_T + LSN_GROUP_TOP + (LSN_TOP_JUST + LSN_DEF_BUTTON_HEIGHT) + ((LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT) * 1) + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),								// iTop
			LSN_SEQUE_LABEL_0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_WE_DELAY ),						// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			LSN_WEWI_SEQ_LOOPS_DELAY_EDIT,					// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT + LSN_SEQUE_LABEL_0_W + LSN_LEFT_JUST,																																		// iLeft
			LSN_SEQUE_LOOP_GROUP_T + LSN_GROUP_TOP + (LSN_TOP_JUST + LSN_DEF_BUTTON_HEIGHT) + ((LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT) * 1),																						// iTop
			LSN_SEQUE_LOOP_EDIT_W,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			LSN_WEWI_SEQ_LOOPS_DELAY_SECONDS_LABEL,			// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT + LSN_SEQUE_LABEL_0_W + LSN_LEFT_JUST + LSN_SEQUE_LOOP_EDIT_W + LSN_LEFT_JUST,																								// iLeft
			LSN_SEQUE_LOOP_GROUP_T + LSN_GROUP_TOP + (LSN_TOP_JUST + LSN_DEF_BUTTON_HEIGHT) + ((LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT) * 1) + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),								// iTop
			LSN_SEQUE_LOOP_SECONDS_W,						// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_WE_SECONDS ),						// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},

		// Fade-Out Time.
		{
			LSW_LT_LABEL,									// ltType
			LSN_WEWI_SEQ_LOOPS_FADE_LABEL,					// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_SEQUE_LOOP_2_2_L,							// iLeft
			LSN_SEQUE_LOOP_GROUP_T + LSN_GROUP_TOP + (LSN_TOP_JUST + LSN_DEF_BUTTON_HEIGHT) + ((LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT) * 1) + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),								// iTop
			LSN_SEQUE_LABEL_0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_WE_FADE_TIME ),					// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			LSN_WEWI_SEQ_LOOPS_FADE_EDIT,					// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_SEQUE_LOOP_2_2_L + LSN_SEQUE_LABEL_0_W + LSN_LEFT_JUST,																																							// iLeft
			LSN_SEQUE_LOOP_GROUP_T + LSN_GROUP_TOP + (LSN_TOP_JUST + LSN_DEF_BUTTON_HEIGHT) + ((LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT) * 1),																						// iTop
			LSN_SEQUE_LOOP_EDIT_W,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			LSN_WEWI_SEQ_LOOPS_FADE_SECONDS_LABEL,			// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_SEQUE_LOOP_2_2_L + LSN_SEQUE_LABEL_0_W + LSN_LEFT_JUST + LSN_SEQUE_LOOP_EDIT_W + LSN_LEFT_JUST,		// iLeft
			LSN_SEQUE_LOOP_GROUP_T + LSN_GROUP_TOP + (LSN_TOP_JUST + LSN_DEF_BUTTON_HEIGHT) + ((LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT) * 1) + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),								// iTop
			LSN_SEQUE_LOOP_SECONDS_W,						// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_WE_SECONDS ),						// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},

		// Description.
		{
			LSW_LT_LABEL,									// ltType
			LSN_WEWI_SEQ_LOOPS_DESC_LABEL,					// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT,				// iLeft
			LSN_SEQUE_LOOP_GROUP_T + LSN_GROUP_TOP + (LSN_TOP_JUST + LSN_DEF_BUTTON_HEIGHT) + (LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT) + (LSN_TOP_JUST + LSN_DEF_EDIT_HEIGHT),														// iTop
			(LSN_INNER_GROUP_W - LSN_GROUP_LEFT - LSN_GROUP_RIGHT) - LSN_LEFT_JUST - (LSN_DEF_STATIC_HEIGHT * 2),																											// dwWidth
			LSN_DEF_STATIC_HEIGHT * 2,						// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			LSN_WEWI_SEQ_LOOPS_WARNING_LABEL,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			(LSN_INNER_GROUP_W - LSN_GROUP_LEFT - LSN_GROUP_RIGHT) - LSN_LEFT_JUST - (LSN_DEF_STATIC_HEIGHT * 2) + LSN_INNER_GROUP_L + LSN_GROUP_LEFT,																// iLeft
			LSN_SEQUE_LOOP_GROUP_T + LSN_GROUP_TOP + (LSN_TOP_JUST + LSN_DEF_BUTTON_HEIGHT) + (LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT) + (LSN_TOP_JUST + LSN_DEF_EDIT_HEIGHT),														// iTop
			LSN_DEF_STATIC_HEIGHT * 2,						// dwWidth
			LSN_DEF_STATIC_HEIGHT * 2,						// dwHeight
			LSN_STATICSTYLE | SS_ICON,						// dwStyle
			0,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},

		// Silences.
		{
			LSW_LT_GROUPBOX,								// ltType
			LSN_WEWI_SEQ_SILENCE_GROUP,						// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L,								// iLeft
			LSN_SEQUE_SIL_GROUP_T,							// iTop
			LSN_INNER_GROUP_W,						// dwWidth
			LSN_SEQUE_SIL_GROUP_H,							// dwHeight
			LSN_GROUPSTYLE,									// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,													// dwStyleEx
			LSN_LSTR( LSN_WE_SILENCES ),						// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},

		// Opening Silence.
		{
			LSW_LT_LABEL,									// ltType
			LSN_WEWI_SEQ_SILENCE_OPEN_SIL_LABEL,			// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT,				// iLeft
			LSN_SEQUE_SIL_GROUP_T + LSN_GROUP_TOP + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),				// iTop
			LSN_SEQUE_LABEL_0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_WE_OPENING_SIL ),					// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			LSN_WEWI_SEQ_SILENCE_OPEN_SIL_EDIT,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT + LSN_SEQUE_LABEL_0_W + LSN_LEFT_JUST,																																		// iLeft
			LSN_SEQUE_SIL_GROUP_T + LSN_GROUP_TOP,																		// iTop
			LSN_SEQUE_LOOP_EDIT_W,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			LSN_WEWI_SEQ_SILENCE_OPEN_SECONDS_LABEL,		// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT + LSN_SEQUE_LABEL_0_W + LSN_LEFT_JUST + LSN_SEQUE_LOOP_EDIT_W + LSN_LEFT_JUST,																								// iLeft
			LSN_SEQUE_SIL_GROUP_T + LSN_GROUP_TOP + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),				// iTop
			LSN_SEQUE_LOOP_SECONDS_W,						// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_WE_SECONDS ),						// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},

		// Trailing Silence.
		{
			LSW_LT_LABEL,									// ltType
			LSN_WEWI_SEQ_SILENCE_TRAIL_LABEL,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_SEQUE_LOOP_2_2_L,							// iLeft
			LSN_SEQUE_SIL_GROUP_T + LSN_GROUP_TOP + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),				// iTop
			LSN_SEQUE_LABEL_0_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_WE_TRAIL ),						// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			LSN_WEWI_SEQ_SILENCE_TRAIL_EDIT,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_SEQUE_LOOP_2_2_L + LSN_SEQUE_LABEL_0_W + LSN_LEFT_JUST,																																							// iLeft
			LSN_SEQUE_SIL_GROUP_T + LSN_GROUP_TOP,			// iTop
			LSN_SEQUE_LOOP_EDIT_W,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			LSN_WEWI_SEQ_SILENCE_SECONDS_LABEL,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_SEQUE_LOOP_2_2_L + LSN_SEQUE_LABEL_0_W + LSN_LEFT_JUST + LSN_SEQUE_LOOP_EDIT_W + LSN_LEFT_JUST,								// iLeft
			LSN_SEQUE_SIL_GROUP_T + LSN_GROUP_TOP + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),									// iTop
			LSN_SEQUE_LOOP_SECONDS_W,						// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_WE_SECONDS ),						// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},

		// Operations.
		{
			LSW_LT_GROUPBOX,								// ltType
			LSN_WEWI_SEQ_OPS_GROUP,							// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L,								// iLeft
			LSN_OPER_GROUP_T,								// iTop
			LSN_INNER_GROUP_W,						// dwWidth
			LSN_OPER_GROUP_H,								// dwHeight
			LSN_GROUPSTYLE,									// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																																		// dwStyleEx
			LSN_LSTR( LSN_WE_OPERATIONS ),					// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_TREELISTVIEW,							// ltType
			LSN_WEWI_FILES_TREELISTVIEW,					// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT,				// iLeft
			LSN_OPER_GROUP_T + LSN_GROUP_TOP,				// iTop
			LSN_OPER_TREE_W,								// dwWidth
			LSN_OPER_TREE_H,								// dwHeight
			LVS_REPORT | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_OWNERDATA | WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP,																											// dwStyle
			LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER,		// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_BUTTON,									// ltType
			LSN_WEWI_SEQ_OPS_ADD_BUTTON,					// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT + LSN_LEFT_JUST + LSN_OPER_TREE_W,																																				// iLeft
			LSN_OPER_GROUP_T + LSN_GROUP_TOP,				// iTop
			LSN_DEF_BUTTON_WIDTH * 2,						// dwWidth
			LSN_DEF_BUTTON_HEIGHT,							// dwHeight
			LSN_BUTTONSTYLE,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																																		// dwStyleEx
			LSN_LSTR( LSN_WE_ADD_OPERATION ),				// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_BUTTON,									// ltType
			LSN_WEWI_SEQ_OPS_REMOVE_BUTTON,					// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT + LSN_LEFT_JUST + LSN_OPER_TREE_W,																																				// iLeft
			LSN_OPER_GROUP_T + LSN_GROUP_TOP + LSN_TOP_JUST + LSN_DEF_BUTTON_HEIGHT,																																			// iTop
			LSN_DEF_BUTTON_WIDTH * 2,						// dwWidth
			LSN_DEF_BUTTON_HEIGHT,							// dwHeight
			LSN_BUTTONSTYLE,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																																		// dwStyleEx
			LSN_LSTR( LSN_WE_REMOVE ),						// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_BUTTON,									// ltType
			LSN_WEWI_SEQ_OPS_UP_BUTTON,						// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT + LSN_LEFT_JUST + LSN_OPER_TREE_W,																																				// iLeft
			LSN_OPER_GROUP_T + LSN_GROUP_TOP + LSN_TOP_JUST + LSN_DEF_BUTTON_HEIGHT + (LSN_TOP_JUST *5) + LSN_DEF_BUTTON_HEIGHT,																								// iTop
			LSN_DEF_BUTTON_WIDTH * 2,						// dwWidth
			LSN_DEF_BUTTON_HEIGHT,							// dwHeight
			LSN_BUTTONSTYLE,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																																		// dwStyleEx
			LSN_LSTR( LSN_WE_MOVE_UP ),						// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
		{
			LSW_LT_BUTTON,									// ltType
			LSN_WEWI_SEQ_OPS_DOWN_BUTTON,					// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_INNER_GROUP_L + LSN_GROUP_LEFT + LSN_LEFT_JUST + LSN_OPER_TREE_W,																																				// iLeft
			LSN_OPER_GROUP_T + LSN_GROUP_TOP + LSN_TOP_JUST + LSN_DEF_BUTTON_HEIGHT + (LSN_TOP_JUST *5) + LSN_DEF_BUTTON_HEIGHT + LSN_TOP_JUST + LSN_DEF_BUTTON_HEIGHT,															// iTop
			LSN_DEF_BUTTON_WIDTH * 2,						// dwWidth
			LSN_DEF_BUTTON_HEIGHT,							// dwHeight
			LSN_BUTTONSTYLE,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																																		// dwStyleEx
			LSN_LSTR( LSN_WE_MOVE_DOWN ),					// pwcText
			0,												// sTextLen
			LSN_WEWI_SEQ,									// dwParentId
		},
	};

	/** The layout for main window. */
	LSW_WIDGET_LAYOUT CWavEditorWindowLayout::m_wlWindow[] {
		{
			LSN_LT_WAV_EDITOR_WINDOW,						// ltType
			LSN_WEWI_MAINWINDOW,							// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			TRUE,											// bActive
			35,												// iLeft
			50,												// iTop
			LSN_W,											// dwWidth
			LSN_H,											// dwHeight
			WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_VISIBLE | WS_CLIPSIBLINGS | DS_3DLOOK | DS_CENTER | DS_SETFONT,													// dwStyle
			WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,			// dwStyleEx
			LSN_LSTR( LSN_WE_WAV_EDITOR ),					// pwcText
			0,												// sTextLen
			LSN_WEWI_NONE,									// dwParentId
			/*
			LSN_PARENT_VCLEFT,								// pcLeftSizeExp
			LSN_PARENT_VCRIGHT,								// pcRightSizeExp
			LSN_PARENT_VCTOP,								// pcTopSizeExp
			LSN_PARENT_VCBOTTOM,							// pcBottomSizeExp
			nullptr, 0,										// pcWidthSizeExp
			nullptr, 0,										// pcHeightSizeExp
			*/
		},

		// Files.
		{
			LSW_LT_GROUPBOX,								// ltType
			LSN_WEWI_FILES_GROUP,							// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_FILES_GROUP_L,								// iLeft
			LSN_FILES_GROUP_T,								// iTop
			LSN_FILES_GROUP_W,								// dwWidth
			LSN_FILES_GROUP_H,								// dwHeight
			LSN_GROUPSTYLE,									// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,													// dwStyleEx
			LSN_LSTR( LSN_WE_FILES ),						// pwcText
			0,												// sTextLen
			LSN_WEWI_MAINWINDOW,							// dwParentId

			//LSN_LOCK_LEFT,									// pcLeftSizeExp
			//LSN_LOCK_RIGHT,									// pcRightSizeExp
			//LSN_LOCK_TOP,									// pcTopSizeExp
			//nullptr, 0,										// pcBottomSizeExp
			//nullptr, 0,										// pcWidthSizeExp
			//LSN_FIXED_HEIGHT,								// pcHeightSizeExp
		},
		{
			LSW_LT_TREELISTVIEW,							// ltType
			LSN_WEWI_FILES_TREELISTVIEW,					// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_FILES_TREE_L,								// iLeft
			LSN_FILES_GROUP_T + LSN_GROUP_TOP_PXL,			// iTop
			LSN_FILES_TREE_W,								// dwWidth
			LSN_FILES_TREE_H,								// dwHeight
			LVS_REPORT | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_OWNERDATA | WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP,						// dwStyle
			LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER,																						// dwStyleEx
			//LSN_EDITSTYLE,								// dwStyle
			//WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_WEWI_FILES_GROUP,							// dwParentId

			LSN_LOCK_LEFT,									// pcLeftSizeExp
			LSN_LOCK_RIGHT,									// pcRightSizeExp
			LSN_LOCK_TOP,									// pcTopSizeExp
			nullptr, 0,										// pcBottomSizeExp
			nullptr, 0,										// pcWidthSizeExp
			LSN_FIXED_HEIGHT,								// pcHeightSizeExp
		},
		{
			LSW_LT_BUTTON,									// ltType
			LSN_WEWI_FILES_ADD_BUTTON,						// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_FILES_BUTTONS_L,							// iLeft
			LSN_FILES_GROUP_T + LSN_GROUP_TOP_PXL,			// iTop
			LSN_FILES_BUTTONS_W,							// dwWidth
			LSN_FILES_BUTTONS_H,							// dwHeight
			LSN_BUTTONSTYLE,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,													// dwStyleEx
			LSN_LSTR( LSN_WE_ADD_WAV ),						// pwcText
			0,												// sTextLen
			LSN_WEWI_FILES_GROUP,							// dwParentId

			nullptr, 0,										// pcLeftSizeExp
			LSN_LOCK_RIGHT,									// pcRightSizeExp
			LSN_LOCK_TOP,									// pcTopSizeExp
			nullptr, 0,										// pcBottomSizeExp
			LSN_FIXED_WIDTH,								// pcWidthSizeExp
			LSN_FIXED_HEIGHT,								// pcHeightSizeExp
		},
		{
			LSW_LT_BUTTON,									// ltType
			LSN_WEWI_FILES_ADD_META_BUTTON,					// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_FILES_BUTTONS_L,							// iLeft
			LSN_FILES_GROUP_T + LSN_GROUP_TOP_PXL + ((LSN_TOP_JUST_PXL + LSN_FILES_BUTTONS_H) * 1),											// iTop
			LSN_FILES_BUTTONS_W,							// dwWidth
			LSN_FILES_BUTTONS_H,							// dwHeight
			LSN_BUTTONSTYLE,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,													// dwStyleEx
			LSN_LSTR( LSN_WE_ADD_METADATA ),				// pwcText
			0,												// sTextLen
			LSN_WEWI_FILES_GROUP,							// dwParentId

			nullptr, 0,										// pcLeftSizeExp
			LSN_LOCK_RIGHT,									// pcRightSizeExp
			LSN_LOCK_TOP,									// pcTopSizeExp
			nullptr, 0,										// pcBottomSizeExp
			LSN_FIXED_WIDTH,								// pcWidthSizeExp
			LSN_FIXED_HEIGHT,								// pcHeightSizeExp
		},
		{
			LSW_LT_BUTTON,									// ltType
			LSN_WEWI_FILES_REMOVE_BUTTON,					// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_FILES_BUTTONS_L,							// iLeft
			LSN_FILES_GROUP_T + LSN_GROUP_TOP_PXL + ((LSN_TOP_JUST_PXL + LSN_FILES_BUTTONS_H) * 2) + (LSN_TOP_JUST_PXL * 2),				// iTop
			LSN_FILES_BUTTONS_W,							// dwWidth
			LSN_FILES_BUTTONS_H,							// dwHeight
			LSN_BUTTONSTYLE,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,													// dwStyleEx
			LSN_LSTR( LSN_WE_REMOVE ),						// pwcText
			0,												// sTextLen
			LSN_WEWI_FILES_GROUP,							// dwParentId

			nullptr, 0,										// pcLeftSizeExp
			LSN_LOCK_RIGHT,									// pcRightSizeExp
			LSN_LOCK_TOP,									// pcTopSizeExp
			nullptr, 0,										// pcBottomSizeExp
			LSN_FIXED_WIDTH,								// pcWidthSizeExp
			LSN_FIXED_HEIGHT,								// pcHeightSizeExp
		},
		{
			LSW_LT_BUTTON,									// ltType
			LSN_WEWI_FILES_UP_BUTTON,						// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_FILES_BUTTONS_L,							// iLeft
			LSN_FILES_GROUP_T + LSN_GROUP_TOP_PXL + ((LSN_TOP_JUST_PXL + LSN_FILES_BUTTONS_H) * 3) + (LSN_TOP_JUST_PXL * 10),				// iTop
			LSN_FILES_BUTTONS_W,							// dwWidth
			LSN_FILES_BUTTONS_H,							// dwHeight
			LSN_BUTTONSTYLE,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,													// dwStyleEx
			LSN_LSTR( LSN_WE_MOVE_UP ),						// pwcText
			0,												// sTextLen
			LSN_WEWI_FILES_GROUP,							// dwParentId

			nullptr, 0,										// pcLeftSizeExp
			LSN_LOCK_RIGHT,									// pcRightSizeExp
			LSN_LOCK_TOP,									// pcTopSizeExp
			nullptr, 0,										// pcBottomSizeExp
			LSN_FIXED_WIDTH,								// pcWidthSizeExp
			LSN_FIXED_HEIGHT,								// pcHeightSizeExp
		},
		{
			LSW_LT_BUTTON,									// ltType
			LSN_WEWI_FILES_DOWN_BUTTON,						// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_FILES_BUTTONS_L,							// iLeft
			LSN_FILES_GROUP_T + LSN_GROUP_TOP_PXL + ((LSN_TOP_JUST_PXL + LSN_FILES_BUTTONS_H) * 4) + (LSN_TOP_JUST_PXL * 10),				// iTop
			LSN_FILES_BUTTONS_W,							// dwWidth
			LSN_FILES_BUTTONS_H,							// dwHeight
			LSN_BUTTONSTYLE,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,													// dwStyleEx
			LSN_LSTR( LSN_WE_MOVE_DOWN ),					// pwcText
			0,												// sTextLen
			LSN_WEWI_FILES_GROUP,							// dwParentId

			nullptr, 0,										// pcLeftSizeExp
			LSN_LOCK_RIGHT,									// pcRightSizeExp
			LSN_LOCK_TOP,									// pcTopSizeExp
			nullptr, 0,										// pcBottomSizeExp
			LSN_FIXED_WIDTH,								// pcWidthSizeExp
			LSN_FIXED_HEIGHT,								// pcHeightSizeExp
		},
	};

#undef LSN_H
#undef LSN_W

#undef LSN_SEQUE_TIME_T

#undef LSN_FILES_GROUP_H
#undef LSN_FILES_GROUP_W

#undef LSN_FILES_GROUP_T
#undef LSN_FILES_TREE_H

#undef LSN_FILES_BUTTONS_L

#undef LSN_FILES_TREE_W

#undef LSN_FILES_BUTTONS_H
#undef LSN_FILES_BUTTONS_W

#undef LSN_FILES_TREE_L
#undef LSN_FILES_GROUP_L

#undef LSN_FSETS_GROUP_H
#undef LSN_FSETS_GROUP_W

#undef LSN_FSETS_MDATA_GROUP_H
#undef LSN_FSETS_MDATA_GROUP_T

#undef LSN_FSETS_CHAR_BUTTON_W
#undef LSN_FSETS_CHAR_BUTTON_L
#undef LSN_FSETS_CHAR_GROUP_H
#undef LSN_FSETS_CHAR_GROUP_T
#undef LSN_FSETS_FDATA_GROUP_H
#undef LSN_FSETS_FDATA_GROUP_T

#undef LSN_SEQUE_LOOP_2_2_W
#undef LSN_SEQUE_LOOP_2_2_L
#undef LSN_SEQUE_LOOP_2_4_W
#undef LSN_SEQUE_LOOP_2_4_L
#undef LSN_SEQUE_LOOP_1_4_W
#undef LSN_SEQUE_LOOP_1_4_L

#undef LSN_SEQUE_LOOP_EDIT_W
#undef LSN_INNER_GROUP_W
#undef LSN_INNER_GROUP_L
#undef LSN_SEQUE_TIME_L
#undef LSN_SEQUE_TIME_H
#undef LSN_SEQUE_TIME_W
#undef LSN_SEQUE_TIME_GROUP_H

#undef LSN_OPER_TREE_H
#undef LSN_OPER_GROUP_H
#undef LSN_OPER_GROUP_T

#undef LSN_SEQUE_SIL_GROUP_H
#undef LSN_SEQUE_SIL_GROUP_T

#undef LSN_SEQUE_LOOP_GROUP_H
#undef LSN_SEQUE_LOOP_GROUP_T
#undef LSN_SEQUE_RANGE_GROUP_H
#undef LSN_SEQUE_RANGE_GROUP_T

#undef LSN_SEQUE_TIME_GROUP_W
#undef LSN_FALLOFF_W
#undef LSN_SEQUE_LOOP_SECONDS_W
#undef LSN_SEQUE_TIME_EDIT_1
#undef LSN_SEQUE_TIME_EDIT_0
#undef LSN_SEQUE_LABEL_0_W


	// == Functions.
	/**
	 * Creates the WAV-Eeitor window.
	 *
	 * \param _pwParent The parent of the window.
	 * \return Returns the created window.
	 */
	CWidget * CWavEditorWindowLayout::CreateWavEditorWindow( CWidget * _pwParent ) {
		CWavEditorWindow::PrepareWavEditor();

		lsn::CLayoutManager * plmLayout = static_cast<lsn::CLayoutManager *>(lsw::CBase::LayoutManager());
		CWidget * pwThis = plmLayout->CreateWindowX( m_wlWindow, LSN_ELEMENTS( m_wlWindow ),
			nullptr, 0,
			_pwParent );
		return pwThis;
	}

	/**
	 * Creates the WAV-Editor sequencer dialog.
	 * 
	 * \param _pwParent The parent of the window.
	 * \param _ui64Options Options to pass to the created dialog.
	 * \return Returns the created widget.
	 **/
	CWidget * CWavEditorWindowLayout::CreateSequencer( CWidget * _pwParent, uint64_t _ui64Options ) {
		return CreatePage( _pwParent, m_wlSequencingDialog, LSN_ELEMENTS( m_wlSequencingDialog ), _ui64Options );
	}

	/**
	 * Creates the WAV-Editor file settings dialog.
	 * 
	 * \param _pwParent The parent of the window.
	 * \param _ui64Options Options to pass to the created dialog.
	 * \return Returns the created widget.
	 **/
	CWidget * CWavEditorWindowLayout::CreateFileSettings( CWidget * _pwParent, uint64_t _ui64Options ) {
		return CreatePage( _pwParent, m_wlFileSettingsDialog, LSN_ELEMENTS( m_wlFileSettingsDialog ), _ui64Options );
	}

	/**
	* Creates the pages.
	*
	* \param _pwParent The parent widget.
	* \param _pwlLayout The page layout.
	* \param _sTotal The number of items to which _pwlLayout points.
	* \param _ui64Options The parameter to pass to the created control.
	* \return Returns the created page.
	*/
	CWidget * CWavEditorWindowLayout::CreatePage( CWidget * _pwParent, const LSW_WIDGET_LAYOUT * _pwlLayout, size_t _sTotal, uint64_t _ui64Options ) {
		lsn::CLayoutManager * plmLayout = static_cast<lsn::CLayoutManager *>(lsw::CBase::LayoutManager());
		CWidget * pwWidget = plmLayout->CreateDialogX( _pwlLayout, _sTotal, _pwParent, _ui64Options );
		if ( pwWidget ) {
			// Success.  Do stuff.
		}
		return pwWidget;
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

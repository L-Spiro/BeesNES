#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The layout for the Find Loop Points dialog.
 */

#include "LSNWavEditorFindLoopsWindowLayout.h"
#include "../../Localization/LSNLocalization.h"
#include "../Layout/LSNLayoutMacros.h"
#include "../Layout/LSNLayoutManager.h"

namespace lsn {

#define LSN_FL_W												360
#define LSN_FL_GROUP_W											(LSN_FL_W - LSN_LEFT_JUST * 2)
#define LSN_FL_L												(LSN_LEFT_JUST + LSN_GROUP_LEFT)
#define LSN_FL_INNER_W											(LSN_FL_GROUP_W - LSN_GROUP_LEFT * 2)
#define LSN_FL_INNER_R											(LSN_FL_L + LSN_FL_INNER_W)
#define LSN_FL_GAP												4
#define LSN_FL_LABEL_OFF										((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) / 2)
#define LSN_FL_ROW_H											(LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST)
#define LSN_FL_ROW_T( IDX )										(LSN_TOP_JUST + LSN_GROUP_TOP + LSN_FL_ROW_H * (IDX))
#define LSN_FL_TIME_LABEL_W										100
#define LSN_FL_TIME_EDIT_L										(LSN_FL_L + LSN_FL_TIME_LABEL_W + LSN_FL_GAP)
#define LSN_FL_TIME_EDIT_W										150
#define LSN_FL_TIME_RESULT_L									(LSN_FL_TIME_EDIT_L + LSN_FL_TIME_EDIT_W + LSN_FL_GAP)
#define LSN_FL_TIME_RESULT_W									(LSN_FL_INNER_R - LSN_FL_TIME_RESULT_L)
#define LSN_FL_SMALL_EDIT_W										30
#define LSN_FL_LOOPS_LABEL_W									24
#define LSN_FL_LOOPS_EDIT_L										(LSN_FL_L + LSN_FL_LOOPS_LABEL_W + LSN_FL_GAP)
#define LSN_FL_RANGE_LABEL_L									(LSN_FL_LOOPS_EDIT_L + LSN_FL_SMALL_EDIT_W + LSN_FL_GAP * 3)
#define LSN_FL_RANGE_LABEL_W									82
#define LSN_FL_RANGE_EDIT_L										(LSN_FL_RANGE_LABEL_L + LSN_FL_RANGE_LABEL_W + LSN_FL_GAP)
#define LSN_FL_MISMATCH_LABEL_L									(LSN_FL_RANGE_EDIT_L + LSN_FL_SMALL_EDIT_W + LSN_FL_GAP * 3)
#define LSN_FL_MISMATCH_LABEL_W									54
#define LSN_FL_MISMATCH_EDIT_L									(LSN_FL_MISMATCH_LABEL_L + LSN_FL_MISMATCH_LABEL_W + LSN_FL_GAP)
#define LSN_FL_SEARCH_H											(LSN_GROUP_TOP + LSN_FL_ROW_H * 3 - LSN_TOP_JUST + LSN_GROUP_BOTTOM)
#define LSN_FL_RESULTS_T										(LSN_TOP_JUST + LSN_FL_SEARCH_H + LSN_TOP_JUST)
#define LSN_FL_LIST_H											104
#define LSN_FL_DETAILS_H										42
#define LSN_FL_RESULTS_H										(LSN_GROUP_TOP + LSN_FL_LIST_H + LSN_TOP_JUST + LSN_FL_DETAILS_H + LSN_GROUP_BOTTOM)
#define LSN_FL_H												(LSN_FL_RESULTS_T + LSN_FL_RESULTS_H + LSN_TOP_JUST + LSN_DEF_BUTTON_HEIGHT + LSN_TOP_JUST)

	// == Members.
	/** The layout for the Find Loop Points dialog. */
	LSW_WIDGET_LAYOUT CWavEditorFindLoopsWindowLayout::m_wlDialog[] = {
		{
			LSN_LT_WAV_EDITOR_FIND_LOOPS_DIALOG,			// ltType
			CWinUtilities::LSN_WFLI_DIALOG,					// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			0,												// iLeft
			0,												// iTop
			LSN_FL_W,										// dwWidth
			LSN_FL_H,										// dwHeight
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | DS_3DLOOK | DS_FIXEDSYS | DS_MODALFRAME | DS_CENTER,	// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,	// dwStyleEx
			LSN_LSTR( LSN_WE_FIND_LOOPS ),					// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_WFLI_NONE,					// dwParentId
		},
		{
			LSW_LT_GROUPBOX,								// ltType
			CWinUtilities::LSN_WFLI_SEARCH_GROUP,			// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST,									// iLeft
			LSN_TOP_JUST,									// iTop
			LSN_FL_GROUP_W,									// dwWidth
			LSN_FL_SEARCH_H,								// dwHeight
			LSN_GROUPSTYLE,									// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			LSN_LSTR( LSN_WE_FL_SEARCH ),					// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_WFLI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			CWinUtilities::LSN_WFLI_START_LABEL,			// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_FL_L,										// iLeft
			LSN_FL_ROW_T( 0 ) + LSN_FL_LABEL_OFF,			// iTop
			LSN_FL_TIME_LABEL_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_WE_FL_START ),					// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_WFLI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			CWinUtilities::LSN_WFLI_START_EDIT,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			TRUE,											// bActive
			LSN_FL_TIME_EDIT_L,								// iLeft
			LSN_FL_ROW_T( 0 ),								// iTop
			LSN_FL_TIME_EDIT_W,								// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_WFLI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			CWinUtilities::LSN_WFLI_START_RESULT_LABEL,		// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_FL_TIME_RESULT_L,							// iLeft
			LSN_FL_ROW_T( 0 ) + LSN_FL_LABEL_OFF,			// iTop
			LSN_FL_TIME_RESULT_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_WFLI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			CWinUtilities::LSN_WFLI_LOOP_LABEL,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_FL_L,										// iLeft
			LSN_FL_ROW_T( 1 ) + LSN_FL_LABEL_OFF,			// iTop
			LSN_FL_TIME_LABEL_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_WE_FL_LOOP ),						// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_WFLI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			CWinUtilities::LSN_WFLI_LOOP_EDIT,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_FL_TIME_EDIT_L,								// iLeft
			LSN_FL_ROW_T( 1 ),								// iTop
			LSN_FL_TIME_EDIT_W,								// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_WFLI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			CWinUtilities::LSN_WFLI_LOOP_RESULT_LABEL,		// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_FL_TIME_RESULT_L,							// iLeft
			LSN_FL_ROW_T( 1 ) + LSN_FL_LABEL_OFF,			// iTop
			LSN_FL_TIME_RESULT_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_WFLI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			CWinUtilities::LSN_WFLI_LOOPS_LABEL,			// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_FL_L,										// iLeft
			LSN_FL_ROW_T( 2 ) + LSN_FL_LABEL_OFF,			// iTop
			LSN_FL_LOOPS_LABEL_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_WE_FL_LOOPS ),					// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_WFLI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			CWinUtilities::LSN_WFLI_LOOPS_EDIT,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_FL_LOOPS_EDIT_L,							// iLeft
			LSN_FL_ROW_T( 2 ),								// iTop
			LSN_FL_SMALL_EDIT_W,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_WFLI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			CWinUtilities::LSN_WFLI_RANGE_LABEL,			// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_FL_RANGE_LABEL_L,							// iLeft
			LSN_FL_ROW_T( 2 ) + LSN_FL_LABEL_OFF,			// iTop
			LSN_FL_RANGE_LABEL_W,							// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_WE_FL_RANGE ),					// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_WFLI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			CWinUtilities::LSN_WFLI_RANGE_EDIT,				// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_FL_RANGE_EDIT_L,							// iLeft
			LSN_FL_ROW_T( 2 ),								// iTop
			LSN_FL_SMALL_EDIT_W,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_WFLI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			CWinUtilities::LSN_WFLI_MISMATCH_LABEL,			// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_FL_MISMATCH_LABEL_L,						// iLeft
			LSN_FL_ROW_T( 2 ) + LSN_FL_LABEL_OFF,			// iTop
			LSN_FL_MISMATCH_LABEL_W,						// dwWidth
			LSN_DEF_STATIC_HEIGHT,							// dwHeight
			LSN_STATICSTYLE,								// dwStyle
			0,												// dwStyleEx
			LSN_LSTR( LSN_WE_FL_MISMATCH ),					// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_WFLI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			CWinUtilities::LSN_WFLI_MISMATCH_EDIT,			// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_FL_MISMATCH_EDIT_L,							// iLeft
			LSN_FL_ROW_T( 2 ),								// iTop
			LSN_FL_SMALL_EDIT_W,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_WFLI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_BUTTON,									// ltType
			CWinUtilities::LSN_WFLI_FIND,					// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_FL_INNER_R - LSN_DEF_BUTTON_WIDTH,			// iLeft
			LSN_FL_ROW_T( 2 ),								// iTop
			LSN_DEF_BUTTON_WIDTH,							// dwWidth
			LSN_DEF_BUTTON_HEIGHT,							// dwHeight
			LSN_DEFBUTTONSTYLE,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			LSN_LSTR( LSN_WE_FL_FIND ),						// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_WFLI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_GROUPBOX,								// ltType
			CWinUtilities::LSN_WFLI_RESULTS_GROUP,			// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_LEFT_JUST,									// iLeft
			LSN_FL_RESULTS_T,								// iTop
			LSN_FL_GROUP_W,									// dwWidth
			LSN_FL_RESULTS_H,								// dwHeight
			LSN_GROUPSTYLE,									// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			LSN_LSTR( LSN_WE_FL_RESULTS ),					// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_WFLI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_LISTBOX,									// ltType
			CWinUtilities::LSN_WFLI_LISTBOX,				// wId
			WC_LISTBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_FL_L,										// iLeft
			LSN_FL_RESULTS_T + LSN_GROUP_TOP,				// iTop
			LSN_FL_INNER_W,									// dwWidth
			LSN_FL_LIST_H,									// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | WS_HSCROLL | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT | LBS_HASSTRINGS,	// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY | WS_EX_CLIENTEDGE,	// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_WFLI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			CWinUtilities::LSN_WFLI_DETAILS_EDIT,			// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_FL_L,										// iLeft
			LSN_FL_RESULTS_T + LSN_GROUP_TOP + LSN_FL_LIST_H + LSN_TOP_JUST,	// iTop
			LSN_FL_INNER_W,									// dwWidth
			LSN_FL_DETAILS_H,								// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | WS_HSCROLL | ES_LEFT | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL | ES_AUTOHSCROLL,	// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_WFLI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_BUTTON,									// ltType
			CWinUtilities::LSN_WFLI_APPLY,					// wId
			WC_BUTTONW,										// lpwcClass
			FALSE,											// bEnabled
			FALSE,											// bActive
			LSN_FL_W - (LSN_DEF_BUTTON_WIDTH + LSN_LEFT_JUST) * 2,	// iLeft
			LSN_FL_H - LSN_DEF_BUTTON_HEIGHT - LSN_TOP_JUST,	// iTop
			LSN_DEF_BUTTON_WIDTH,							// dwWidth
			LSN_DEF_BUTTON_HEIGHT,							// dwHeight
			LSN_BUTTONSTYLE,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			LSN_LSTR( LSN_WE_FL_APPLY ),					// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_WFLI_DIALOG,					// dwParentId
		},
		{
			LSW_LT_BUTTON,									// ltType
			CWinUtilities::LSN_WFLI_CANCEL,					// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_FL_W - LSN_DEF_BUTTON_WIDTH - LSN_LEFT_JUST,	// iLeft
			LSN_FL_H - LSN_DEF_BUTTON_HEIGHT - LSN_TOP_JUST,	// iTop
			LSN_DEF_BUTTON_WIDTH,							// dwWidth
			LSN_DEF_BUTTON_HEIGHT,							// dwHeight
			LSN_BUTTONSTYLE,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			LSN_LSTR( LSN_CANCEL ),							// pwcText
			0,												// sTextLen
			CWinUtilities::LSN_WFLI_DIALOG,					// dwParentId
		},
	};

#undef LSN_FL_H
#undef LSN_FL_RESULTS_H
#undef LSN_FL_DETAILS_H
#undef LSN_FL_LIST_H
#undef LSN_FL_RESULTS_T
#undef LSN_FL_SEARCH_H
#undef LSN_FL_MISMATCH_EDIT_L
#undef LSN_FL_MISMATCH_LABEL_W
#undef LSN_FL_MISMATCH_LABEL_L
#undef LSN_FL_RANGE_EDIT_L
#undef LSN_FL_RANGE_LABEL_W
#undef LSN_FL_RANGE_LABEL_L
#undef LSN_FL_LOOPS_EDIT_L
#undef LSN_FL_LOOPS_LABEL_W
#undef LSN_FL_SMALL_EDIT_W
#undef LSN_FL_TIME_RESULT_W
#undef LSN_FL_TIME_RESULT_L
#undef LSN_FL_TIME_EDIT_W
#undef LSN_FL_TIME_EDIT_L
#undef LSN_FL_TIME_LABEL_W
#undef LSN_FL_ROW_T
#undef LSN_FL_ROW_H
#undef LSN_FL_LABEL_OFF
#undef LSN_FL_GAP
#undef LSN_FL_INNER_R
#undef LSN_FL_INNER_W
#undef LSN_FL_L
#undef LSN_FL_GROUP_W
#undef LSN_FL_W


	// == Functions.
	/**
	 * Creates the Find Loop Points dialog.
	 *
	 * \param _pwParent The parent of the dialog.
	 * \param _fldData The metadata to search and the returned ending point.
	 * \return Returns TRUE if a loop range was applied.
	 */
	BOOL CWavEditorFindLoopsWindowLayout::CreateFindLoopsDialog( CWidget * _pwParent, LSN_FIND_LOOPS_DATA &_fldData ) {
		lsn::CLayoutManager * plmLayout = static_cast<lsn::CLayoutManager *>(lsw::CBase::LayoutManager());
		INT_PTR ipProc = plmLayout->DialogBoxX( m_wlDialog, std::size( m_wlDialog ), _pwParent, reinterpret_cast<uint64_t>(&_fldData) );
		return ipProc == 1 ? TRUE : FALSE;
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

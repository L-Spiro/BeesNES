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


#define LSN_SEQUE_LABEL_0_W									70
#define LSN_SEQUE_TIME_EDIT_0								150
#define LSN_SEQUE_TIME_EDIT_1								100
#define LSN_SEQUE_TIME_GROUP_W								(LSN_GROUP_LEFT + LSN_SEQUE_LABEL_0_W + LSN_LEFT_JUST + LSN_SEQUE_TIME_EDIT_0 + LSN_LEFT_JUST + LSN_SEQUE_TIME_EDIT_1 + LSN_GROUP_LEFT)
#define LSN_SEQUE_TIME_GROUP_H								(LSN_GROUP_TOP + LSN_TOP_JUST + LSN_DEF_COMBO_HEIGHT + LSN_TOP_JUST)
#define LSN_SEQUE_TIME_W									(LSN_GROUP_LEFT + LSN_SEQUE_TIME_GROUP_W + LSN_GROUP_LEFT)
#define LSN_SEQUE_TIME_H									(LSN_TOP_JUST + LSN_SEQUE_TIME_GROUP_H + LSN_TOP_JUST)
#define LSN_SEQUE_TIME_L									LSN_LEFT_JUST

#define LSN_FILES_GROUP_L									LSN_LEFT_JUST_PXL
#define LSN_FILES_TREE_L									(LSN_FILES_GROUP_L + LSN_GROUP_LEFT_PXL)

#define LSN_FILES_BUTTONS_W									(LSN_DEF_BUTTON_WIDTH_PXL * 2)
#define LSN_FILES_BUTTONS_H									LSN_DEF_BUTTON_HEIGHT_PXL

#define LSN_FILES_TREE_W									((559) - (LSN_FILES_BUTTONS_W - LSN_LEFT_JUST_PXL))

#define LSN_FILES_BUTTONS_L									(LSN_FILES_TREE_L + LSN_FILES_TREE_W + LSN_LEFT_JUST_PXL)

#define LSN_FILES_TREE_H									((LSN_FILES_BUTTONS_H * 5) + (LSN_TOP_JUST_PXL * 4) + (LSN_TOP_JUST_PXL * 10))
#define LSN_FILES_GROUP_T									LSN_TOP_JUST_PXL

#define LSN_FILES_GROUP_W									(LSN_GROUP_LEFT_PXL + LSN_FILES_TREE_W + LSN_LEFT_JUST_PXL + LSN_FILES_BUTTONS_W + LSN_GROUP_LEFT_PXL)
#define LSN_FILES_GROUP_H									(LSN_GROUP_TOP_PXL + LSN_TOP_JUST_PXL + LSN_FILES_TREE_H + LSN_GROUP_BOTTOM_PXL)

#define LSN_SEQUE_TIME_T									(LSN_FILES_GROUP_T + LSN_FILES_GROUP_H)


#define LSN_W												(LSN_FILES_GROUP_W + (LSN_LEFT_JUST_PXL * 2))
#define LSN_H												(LSN_SEQUE_TIME_T + (LSN_SEQUE_TIME_H + 80) + LSN_TOP_JUST_PXL)

namespace lsn {

	/** The layout for the "File Settings" dialog. */
	LSW_WIDGET_LAYOUT CWavEditorWindowLayout::m_wlFileSettingsDialog[] = {
		{},
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
			LSN_SEQUE_TIME_W,								// dwWidth
			LSN_SEQUE_TIME_H,								// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | DS_3DLOOK | DS_FIXEDSYS | DS_SETFONT | DS_CONTROL | WS_CLIPSIBLINGS,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_CONTROLPARENT,														// dwStyleEx
			LSN_LSTR( LSN_PATCH_PATCH_ROM ),				// pwcText
			0,												// sTextLen
			LSN_WEWI_NONE,									// dwParentId

			//LSN_LOCK_LEFT,								// pcLeftSizeExp
			//LSN_LOCK_RIGHT,								// pcRightSizeExp
			//LSN_LOCK_TOP,									// pcTopSizeExp
			///*"20+"*/ LSN_LOCK_BOTTOM,					// pcBottomSizeExp
			//nullptr, 0,									// pcWidthSizeExp
			//nullptr, 0,									// pcHeightSizeExp
		},

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

			//LSN_LOCK_LEFT,								// pcLeftSizeExp
			//LSN_LOCK_RIGHT,								// pcRightSizeExp
			//LSN_LOCK_TOP,									// pcTopSizeExp
			//nullptr, 0,									// pcBottomSizeExp
			//nullptr, 0,									// pcWidthSizeExp
			//LSN_FIXED_HEIGHT,								// pcHeightSizeExp
		},
		{
			LSW_LT_LABEL,									// ltType
			LSN_WEWI_SEQ_START_LABEL,						// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			LSN_SEQUE_TIME_L + LSN_GROUP_LEFT,				// iLeft
			LSN_GROUP_TOP + ((LSN_DEF_EDIT_HEIGHT - LSN_DEF_STATIC_HEIGHT) >> 1),															// iTop
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
			LSN_SEQUE_TIME_L + LSN_GROUP_LEFT + LSN_SEQUE_LABEL_0_W + LSN_LEFT_JUST,														// iLeft
			LSN_GROUP_TOP,									// iTop
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
			LSN_SEQUE_TIME_L + LSN_GROUP_LEFT + LSN_SEQUE_LABEL_0_W + LSN_LEFT_JUST + LSN_SEQUE_TIME_EDIT_0 + LSN_LEFT_JUST,				// iLeft
			LSN_GROUP_TOP,									// iTop
			LSN_SEQUE_TIME_EDIT_1,							// dwWidth
			LSN_DEF_EDIT_HEIGHT,							// dwHeight
			LSN_EDITSTYLE,									// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
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
			LSN_WEWI_MAINWINDOW,							// dwParentId

			//LSN_LOCK_LEFT,									// pcLeftSizeExp
			//LSN_LOCK_RIGHT,									// pcRightSizeExp
			//LSN_LOCK_TOP,									// pcTopSizeExp
			//nullptr, 0,										// pcBottomSizeExp
			//nullptr, 0,										// pcWidthSizeExp
			//LSN_FIXED_HEIGHT,								// pcHeightSizeExp
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
			LSN_WEWI_MAINWINDOW,							// dwParentId

			//nullptr, 0,										// pcLeftSizeExp
			//LSN_LOCK_RIGHT,									// pcRightSizeExp
			//LSN_LOCK_TOP,									// pcTopSizeExp
			//nullptr, 0,										// pcBottomSizeExp
			//LSN_FIXED_WIDTH,								// pcWidthSizeExp
			//LSN_FIXED_HEIGHT,								// pcHeightSizeExp
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
			LSN_WEWI_MAINWINDOW,							// dwParentId

			//nullptr, 0,										// pcLeftSizeExp
			//LSN_LOCK_RIGHT,									// pcRightSizeExp
			//LSN_LOCK_TOP,									// pcTopSizeExp
			//nullptr, 0,										// pcBottomSizeExp
			//LSN_FIXED_WIDTH,								// pcWidthSizeExp
			//LSN_FIXED_HEIGHT,								// pcHeightSizeExp
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
			LSN_WEWI_MAINWINDOW,							// dwParentId

			//nullptr, 0,										// pcLeftSizeExp
			//LSN_LOCK_RIGHT,									// pcRightSizeExp
			//LSN_LOCK_TOP,									// pcTopSizeExp
			//nullptr, 0,										// pcBottomSizeExp
			//LSN_FIXED_WIDTH,								// pcWidthSizeExp
			//LSN_FIXED_HEIGHT,								// pcHeightSizeExp
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
			LSN_WEWI_MAINWINDOW,							// dwParentId

			//nullptr, 0,										// pcLeftSizeExp
			//LSN_LOCK_RIGHT,									// pcRightSizeExp
			//LSN_LOCK_TOP,									// pcTopSizeExp
			//nullptr, 0,										// pcBottomSizeExp
			//LSN_FIXED_WIDTH,								// pcWidthSizeExp
			//LSN_FIXED_HEIGHT,								// pcHeightSizeExp
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
			LSN_WEWI_MAINWINDOW,							// dwParentId

			//nullptr, 0,										// pcLeftSizeExp
			//LSN_LOCK_RIGHT,									// pcRightSizeExp
			//LSN_LOCK_TOP,									// pcTopSizeExp
			//nullptr, 0,										// pcBottomSizeExp
			//LSN_FIXED_WIDTH,								// pcWidthSizeExp
			//LSN_FIXED_HEIGHT,								// pcHeightSizeExp
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

#undef LSN_SEQUE_TIME_L
#undef LSN_SEQUE_TIME_H
#undef LSN_SEQUE_TIME_W
#undef LSN_SEQUE_TIME_GROUP_H
#undef LSN_SEQUE_TIME_GROUP_W
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

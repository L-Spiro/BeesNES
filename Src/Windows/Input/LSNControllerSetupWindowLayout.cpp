#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The input-configuration window layout.
 */

#include "LSNControllerSetupWindowLayout.h"
#include "../../Localization/LSNLocalization.h"
#include "../Input/LSNControllerSetupWindow.h"
#include "../Layout/LSNLayoutMacros.h"
#include "../Layout/LSNLayoutManager.h"
#include "LSNStdControllerPageLayout.h"



namespace lsn {

#define LSN_INPUT_GROUP_W							(LSN_STD_CONT_W)
#define LSN_INPUT_W									(LSN_INPUT_GROUP_W)
#define LSN_INPUT_H									(LSN_STD_CONT_H + LSN_DEF_BUTTON_HEIGHT + LSN_TOP_JUST * 2)

	// == Members.
	/** The layout for the main window. */
	LSW_WIDGET_LAYOUT CControllerSetupWindowLayout::m_wlInputWindow[] = {
		{
			LSN_LT_CONTROLLER_SETUP_DIALOG,			// ltType
			LSN_CSW_MAINWINDOW,						// wId
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
			LSN_CSW_NONE,							// dwParentId
		},

		{
			LSW_LT_TAB,								// ltType
			LSN_CSW_TAB,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			LSN_INPUT_GROUP_W,						// dwWidth
			LSN_STD_CONT_H,							// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | TCS_HOTTRACK,																					// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			LSN_CSW_MAINWINDOW,						// dwParentId
		},


		{
			LSW_LT_BUTTON,							// ltType
			LSN_CSW_OK,								// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			LSN_LEFT_JUST,							// iLeft
			LSN_INPUT_H - LSN_DEF_BUTTON_HEIGHT - LSN_TOP_JUST,		// iTop
			LSN_DEF_BUTTON_WIDTH,					// dwWidth
			LSN_DEF_BUTTON_HEIGHT,					// dwHeight
			LSN_DEFBUTTONSTYLE,						// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																								// dwStyleEx
			LSN_LSTR( LSN_OK ),						// pwcText
			0,										// sTextLen
			LSN_CSW_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			LSN_CSW_CANCEL,							// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			LSN_INPUT_W - LSN_DEF_BUTTON_WIDTH * 1 - LSN_LEFT_JUST,	// iLeft
			LSN_INPUT_H - LSN_DEF_BUTTON_HEIGHT - LSN_TOP_JUST,		// iTop
			LSN_DEF_BUTTON_WIDTH,					// dwWidth
			LSN_DEF_BUTTON_HEIGHT,					// dwHeight
			LSN_BUTTONSTYLE,						// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																								// dwStyleEx
			LSN_LSTR( LSN_CANCEL ),					// pwcText
			0,										// sTextLen
			LSN_CSW_MAINWINDOW,						// dwParentId
		},
	};


#undef LSN_INPUT_H
#undef LSN_INPUT_W
#undef LSN_INPUT_GROUP_W


	// == Functions.
	/**
	 * Creates the input-configuration window.
	 *
	 * \param _pwParent The parent of the dialog.
	 * \param _ioOptions A reference to the options object.
	 * \param _pmwMainWindow A pointer to the main window.
	 * \param _stPlayerIdx The player index.
	 * \return Returns TRUE if the dialog was created successfully.
	 */
	BOOL CControllerSetupWindowLayout::CreateInputDialog( CWidget * _pwParent, LSN_INPUT_OPTIONS &_ioOptions, lsn::CMainWindow * _pmwMainWindow, size_t _stPlayerIdx ) {
		lsn::CLayoutManager * plmLayout = static_cast<lsn::CLayoutManager *>(lsw::CBase::LayoutManager());
		CControllerSetupWindow::LSN_CONTROLLER_SETUP_DATA csdData = {
			.pioOptions = &_ioOptions,
			.pmwMainWindow = _pmwMainWindow,
			.stIdx = _stPlayerIdx
		};
		INT_PTR ipProc = plmLayout->DialogBoxX( m_wlInputWindow, std::size( m_wlInputWindow ), _pwParent, reinterpret_cast<uint64_t>(&csdData) );
		if ( ipProc != 0 ) {
			// Success.  Do stuff.
			return TRUE;
		}
		return FALSE;
	}

	/**
	 * Creates the tab pages.
	 *
	 * \param _pwParent The parent widget.
	 * \param _pwlLayout The page layout.
	 * \param _sTotal The number of items to which _pwlLayout points.
	 * \param _ioOptions A reference to the options object.
	 * \return Returns the created page.
	 */
	/*CWidget * CControllerSetupWindowLayout::CreatePage( CWidget * _pwParent, const LSW_WIDGET_LAYOUT * _pwlLayout, size_t _sTotal, LSN_INPUT_OPTIONS &_ioOptions ) {
		lsn::CLayoutManager * plmLayout = static_cast<lsn::CLayoutManager *>(lsw::CBase::LayoutManager());
		CWidget * pwWidget = plmLayout->CreateDialogX( _pwlLayout, _sTotal, _pwParent, reinterpret_cast<uint64_t>(&_ioOptions) );
		if ( pwWidget ) {
			// Success.  Do stuff.
		}
		return pwWidget;
	}*/

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

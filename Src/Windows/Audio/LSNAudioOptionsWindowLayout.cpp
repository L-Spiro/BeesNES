
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

#define LSN_AUDIO_GLOBAL_PAGE_W								(LSN_LEFT_JUST + LSN_LEFT_JUST + 500 + LSN_LEFT_JUST + LSN_LEFT_JUST)
#define LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP					LSN_TOP_JUST
#define LSN_AUDIO_OPTIONS_GENERAL_GROUP_W					(LSN_AUDIO_GLOBAL_PAGE_W - (LSN_LEFT_JUST * 2))
#define LSN_AUDIO_OPTIONS_GENERAL_GROUP_H					(LSN_GROUP_TOP + LSN_GROUP_BOTTOM + LSN_DEF_EDIT_HEIGHT + LSN_TOP_JUST + 500)
#define LSN_AUDIO_GLOBAL_PAGE_H								(LSN_AUDIO_OPTIONS_GENERAL_GROUP_H)
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
			WS_CHILDWINDOW | WS_VISIBLE | DS_3DLOOK | DS_FIXEDSYS | DS_SETFONT | DS_CONTROL | WS_CLIPSIBLINGS,	// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_CONTROLPARENT,							// dwStyleEx
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
			LSN_GROUPSTYLE,																											// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,											// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_GENERAL ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId

			LSN_LOCK_LEFT,									// pcLeftSizeExp
			LSN_LOCK_RIGHT,									// pcRightSizeExp
			LSN_LOCK_TOP,									// pcTopSizeExp
			LSN_LOCK_BOTTOM,								// pcBottomSizeExp
			nullptr, 0,										// pcWidthSizeExp
			nullptr, 0,										// pcHeightSizeExp
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
			WS_CHILDWINDOW | WS_VISIBLE | DS_3DLOOK | DS_FIXEDSYS | DS_SETFONT | DS_CONTROL | WS_CLIPSIBLINGS,	// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_CONTROLPARENT,							// dwStyleEx
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
			LSN_GROUPSTYLE,																											// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,											// dwStyleEx
			LSN_LSTR( LSN_AUDIO_OPTIONS_GENERAL ),			// pwcText
			0,												// sTextLen
			LSN_AOWI_PAGE_PERGAME,							// dwParentId

			LSN_LOCK_LEFT,									// pcLeftSizeExp
			LSN_LOCK_RIGHT,									// pcRightSizeExp
			LSN_LOCK_TOP,									// pcTopSizeExp
			LSN_LOCK_BOTTOM,								// pcBottomSizeExp
			nullptr, 0,										// pcWidthSizeExp
			nullptr, 0,										// pcHeightSizeExp
		},
	};

#undef LSN_AUDIO_OPTIONS_GENERAL_GROUP_TOP
#undef LSN_AUDIO_OPTIONS_GENERAL_GROUP_W

#define LSN_AUDIO_OPTIONS_W									LSN_AUDIO_GLOBAL_PAGE_W
#define LSN_AUDIO_OPTIONS_H									LSN_AUDIO_GLOBAL_PAGE_H

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

			LSN_PARENT_VCLEFT,								// pcLeftSizeExp
			nullptr, 0,										// pcRightSizeExp
			LSN_PARENT_VCTOP,								// pcTopSizeExp
			nullptr, 0,										// pcBottomSizeExp
			LSN_FIXED_WIDTH,								// pcWidthSizeExp
			LSN_FIXED_HEIGHT,								// pcHeightSizeExp
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
			LSN_AUDIO_OPTIONS_H,							// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | TCS_HOTTRACK,				// dwStyle
			WS_EX_ACCEPTFILES,														// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			LSN_AOWI_MAINWINDOW,							// dwParentId

			LSN_LOCK_LEFT,									// pcLeftSizeExp
			LSN_LOCK_RIGHT,									// pcRightSizeExp
			LSN_LOCK_TOP,									// pcTopSizeExp
			LSN_LOCK_BOTTOM,								// pcBottomSizeExp
			nullptr, 0,										// pcWidthSizeExp
			nullptr, 0,										// pcHeightSizeExp
		},
	};

#undef LSN_AUDIO_OPTIONS_PAGE_W
#undef LSN_AUDIO_OPTIONS_PAGE_H
#undef LSN_AUDIO_OPTIONS_GENERAL_GROUP_H
#undef LSN_AUDIO_GLOBAL_PAGE_H
#undef LSN_AUDIO_GLOBAL_PAGE_W



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


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

#define LSN_AUDIO_OPTIONS_W									(LSN_LEFT_JUST + LSN_LEFT_JUST + 200 + LSN_LEFT_JUST + LSN_LEFT_JUST)
#define LSN_AUDIO_OPTIONS_H									(50 + 150 + 20)

	// == Members.
	/** The layout for the template window. */
	LSW_WIDGET_LAYOUT CAudioOptionsWindowLayout::m_wlPage[] = {
		{
			LSN_LT_INPUT_DIALOG,					// ltType
			LSN_AOWI_MAINWINDOW,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			LSN_AUDIO_OPTIONS_W,							// dwWidth
			LSN_AUDIO_OPTIONS_H,							// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | DS_3DLOOK | DS_FIXEDSYS | DS_SETFONT | DS_CONTROL,										// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_CONTROLPARENT,												// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			LSN_AOWI_NONE,							// dwParentId

			LSN_PARENT_VCLEFT,						// pcLeftSizeExp
			nullptr, 0,								// pcRightSizeExp
			LSN_PARENT_VCTOP,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			LSN_FIXED_WIDTH,						// pcWidthSizeExp
			LSN_FIXED_HEIGHT,						// pcHeightSizeExp
		},
	};

#undef LSN_AUDIO_OPTIONS_H
#undef LSN_AUDIO_OPTIONS_W


	// == Functions.
	/**
	 * Creates the page.
	 *
	 * \param _pwParent the parent of the page.
	 * \return Returns the created widget.
	 */
	CWidget * CAudioOptionsWindowLayout::CreatePage( CWidget * _pwParent ) {
		lsn::CLayoutManager * plmLayout = static_cast<lsn::CLayoutManager *>(lsw::CBase::LayoutManager());
		CWidget * pwWidget = plmLayout->CreateDialogX( m_wlPage, LSN_ELEMENTS( m_wlPage ), _pwParent, 0 );
		if ( pwWidget ) {
			// Success.  Do stuff.
		}
		return pwWidget;
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

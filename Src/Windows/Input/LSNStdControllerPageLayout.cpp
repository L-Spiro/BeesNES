#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A dialog shaped like a standard controller.
 */

#include "LSNStdControllerPageLayout.h"
#include "../../Localization/LSNLocalization.h"
#include "../Layout/LSNLayoutMacros.h"
#include "../Layout/LSNLayoutManager.h"



namespace lsn {

#define LSN_TEMPLATE_W									(LSN_LEFT_JUST + LSN_LEFT_JUST + 200 + LSN_LEFT_JUST + LSN_LEFT_JUST)
#define LSN_TEMPLATE_H									(50 + 150 + 20)

	// == Members.
	/** The layout for the template window. */
	LSW_WIDGET_LAYOUT CStdControllerPageLayout::m_wlPage[] = {
		{
			LSN_LT_INPUT_DIALOG,					// ltType
			LSN_TWI_MAINWINDOW,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			LSN_TEMPLATE_W,							// dwWidth
			LSN_TEMPLATE_H,							// dwHeight
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | DS_3DLOOK | DS_FIXEDSYS | DS_MODALFRAME | DS_CENTER,					// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,												// dwStyleEx
			LSN_LSTR( LSN_INPUT_DIALOG ),			// pwcText
			0,										// sTextLen
			LSN_TWI_NONE,							// dwParentId
		},
	};

#undef LSN_TEMPLATE_H
#undef LSN_TEMPLATE_W


	// == Functions.
	/**
	 * Creates the page.
	 *
	 * \param _pwParent the parent of the page.
	 * \return Returns the created widget.
	 */
	CWidget * CStdControllerPageLayout::CreatePage( CWidget * _pwParent ) {
		return CreatePage( _pwParent, m_wlPage, LSN_ELEMENTS( m_wlPage ) );
	}

	/**
	 * Creates the tab pages.
	 *
	 * \param _pwParent The parent widget.
	 * \param _pwlLayout The page layout.
	 * \param _sTotal The number of items to which _pwlLayout points.
	 * \return Returns the created page.
	 */
	CWidget * CStdControllerPageLayout::CreatePage( CWidget * _pwParent, const LSW_WIDGET_LAYOUT * _pwlLayout, size_t _sTotal ) {
		lsn::CLayoutManager * plmLayout = static_cast<lsn::CLayoutManager *>(lsw::CBase::LayoutManager());
		CWidget * pwWidget = plmLayout->CreateDialogX( _pwlLayout, _sTotal, _pwParent, 0 );
		if ( pwWidget ) {
			// Success.  Do stuff.
		}
		return pwWidget;
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

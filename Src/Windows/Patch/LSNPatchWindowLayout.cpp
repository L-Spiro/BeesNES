#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The patch window layout.
 */
 
#include "LSNPatchWindowLayout.h"
#include "../../Localization/LSNLocalization.h"
#include "../Layout/LSNLayoutMacros.h"
#include "../Layout/LSNLayoutManager.h"
 

 namespace lsn {

	// == Members.
	/** The layout for the main window. */
	LSW_WIDGET_LAYOUT CPatchWindowLayout::m_wlPatchWindow[] = {
		{
			LSN_LT_INPUT_DIALOG,					// ltType
			LSN_PWI_MAINWINDOW,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			500,							// dwWidth
			500,							// dwHeight
			WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | DS_3DLOOK | DS_SETFONT,				// dwStyle
			WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,														// dwStyleEx
			LSN_LSTR( LSN_INPUT_DIALOG ),			// pwcText
			0,										// sTextLen
			LSN_PWI_NONE,							// dwParentId
		},
	};


	// == Functions.
	// Creates the window.
	CWidget * CPatchWindowLayout::CreatePatchWindow( CWidget * _pwParent ) {
		lsn::CLayoutManager * plmLayout = static_cast<lsn::CLayoutManager *>(lsw::CBase::LayoutManager());
		CWidget * pwThis = plmLayout->CreateWindowX( m_wlPatchWindow, LSN_ELEMENTS( m_wlPatchWindow ),
			nullptr, 0,
			_pwParent, 0 );

		return pwThis;
	}

 }	// namespace lsn

 #endif	// #ifdef LSN_USE_WINDOWS

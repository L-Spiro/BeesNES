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

#define LSN_W												550
#define LSN_H												350

namespace lsn {

	/** The layout for main window. */
	LSW_WIDGET_LAYOUT CWavEditorWindowLayout::m_wlWindow[] {
		{
			LSN_LT_WAV_EDITOR_WINDOW,						// ltType
			LSN_AOWI_MAINWINDOW,							// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			TRUE,											// bActive
			35,												// iLeft
			50,												// iTop
			LSN_W,											// dwWidth
			LSN_H,											// dwHeight
			WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | DS_3DLOOK | DS_SETFONT,				// dwStyle
			WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,														// dwStyleEx
			LSN_LSTR( LSN_WE_WAV_EDITOR ),					// pwcText
			0,												// sTextLen
			LSN_AOWI_NONE,									// dwParentId
			/*
			MX_PARENT_VCLEFT,								// pcLeftSizeExp
			MX_PARENT_VCRIGHT,								// pcRightSizeExp
			MX_PARENT_VCTOP,								// pcTopSizeExp
			MX_PARENT_VCBOTTOM,								// pcBottomSizeExp
			nullptr, 0,										// pcWidthSizeExp
			nullptr, 0,										// pcHeightSizeExp
			*/
		},
	};

#undef LSN_H
#undef LSN_W

	// == Functions.
	/**
	 * Creates the WAV-Eeitor window.
	 *
	 * \param _pwParent The parent of the window.
	 * \return Returns the created window.
	 */
	CWidget * CWavEditorWindowLayout::CreateWavEditorWindow( CWidget * _pwParent ) {
		CWavEditorWindow::PreparePeWorks();

		lsn::CLayoutManager * plmLayout = static_cast<lsn::CLayoutManager *>(lsw::CBase::LayoutManager());
		CWidget * pwThis = plmLayout->CreateWindowX( m_wlWindow, LSN_ELEMENTS( m_wlWindow ),
			nullptr, 0,
			_pwParent );
		return pwThis;
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

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
#include "../Input/LSNStdControllerPage.h"
#include "../Layout/LSNLayoutMacros.h"
#include "../Layout/LSNLayoutManager.h"



namespace lsn {

	// == Members.
	/** The layout for the template window. */
	LSW_WIDGET_LAYOUT CStdControllerPageLayout::m_wlPage[] = {
		{
			LSN_LT_STD_CONTROLLER_PAGE,				// ltType
			LSN_SCPI_MAINWINDOW,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			LSN_STD_CONT_W,							// dwWidth
			LSN_STD_CONT_H,							// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | DS_3DLOOK | DS_FIXEDSYS | DS_SETFONT | DS_CONTROL,										// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_CONTROLPARENT,												// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			LSN_SCPI_NONE,							// dwParentId

			/*LSN_PARENT_VCLEFT,						// pcLeftSizeExp
			nullptr, 0,								// pcRightSizeExp
			LSN_PARENT_VCTOP,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			LSN_FIXED_WIDTH,						// pcWidthSizeExp
			LSN_FIXED_HEIGHT,*/						// pcHeightSizeExp
		},
#define LSN_QUICK_CONTR( TYPE_, ID_, TEXT_, X_, Y_, W_, H_, STYLE_, STYLEEX_ )																		\
	{																																				\
		TYPE_,																																		\
		ID_,																																		\
		nullptr,																																	\
		TRUE,																																		\
		FALSE,																																		\
		X_,																																			\
		Y_,																																			\
		W_,																																			\
		H_,																																			\
		STYLE_,																																		\
		STYLEEX_,																																	\
		TEXT_,																																		\
		0,																																			\
		LSN_SCPI_MAINWINDOW,																														\
	},


#define LSN_QUICK_BUTTON( BUTTON_, TEXT_, X_, Y_ )																																				\
	LSN_QUICK_CONTR( LSW_LT_GROUPBOX, BUTTON_ + LSN_SCPI_GROUP, TEXT_, (X_), (Y_),																												\
		LSN_STD_CONT_BUTTON_GROUP_W, LSN_STD_CONT_BUTTON_GROUP_H, LSN_GROUPSTYLE, WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY )									\
	LSN_QUICK_CONTR( LSW_LT_LABEL, BUTTON_ + LSN_SCPI_LABEL, LSN_LSTR( LSN_BUTTON_ ), (X_) + LSN_GROUP_LEFT, (Y_) + LSN_GROUP_TOP + (LSN_DEF_BUTTON_HEIGHT - LSN_DEF_STATIC_HEIGHT) / 2,		\
		LSN_STD_CONT_BUTTON_LABEL_W, LSN_DEF_STATIC_HEIGHT, LSN_STATICSTYLE, 0 )																												\
	LSN_QUICK_CONTR( LSW_LT_BUTTON, BUTTON_ + LSN_SCPI_BUTTON, nullptr,																															\
		(X_) + LSN_GROUP_LEFT + LSN_STD_CONT_BUTTON_LABEL_W, (Y_) + LSN_GROUP_TOP,																												\
		LSN_STD_CONT_BUTTON_BUTTON_W, LSN_DEF_BUTTON_HEIGHT, LSN_BUTTONSTYLE, WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY )										\
	LSN_QUICK_CONTR( LSW_LT_LABEL, BUTTON_ + LSN_SCPI_TURBO_LABEL, LSN_LSTR( LSN_TURBO_ ),																										\
		(X_) + LSN_GROUP_LEFT, (Y_) + LSN_GROUP_TOP + (LSN_DEF_BUTTON_HEIGHT - LSN_DEF_STATIC_HEIGHT) / 2 + LSN_DEF_BUTTON_HEIGHT + 1,															\
		LSN_STD_CONT_BUTTON_LABEL_W, LSN_DEF_STATIC_HEIGHT, LSN_STATICSTYLE, 0 )																												\
	LSN_QUICK_CONTR( LSW_LT_BUTTON, BUTTON_ + LSN_SCPI_TURBO_BUTTON, nullptr,																													\
		(X_) + LSN_GROUP_LEFT + LSN_STD_CONT_BUTTON_LABEL_W, (Y_) + LSN_GROUP_TOP + LSN_DEF_BUTTON_HEIGHT + 1,																					\
		LSN_STD_CONT_BUTTON_BUTTON_W, LSN_DEF_BUTTON_HEIGHT, LSN_BUTTONSTYLE, WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY )										\
	LSN_QUICK_CONTR( LSW_LT_COMBOBOX, BUTTON_ + LSN_SCPI_TURBO_COMBO, nullptr,																													\
		(X_) + LSN_GROUP_LEFT, (Y_) + LSN_GROUP_TOP + LSN_DEF_BUTTON_HEIGHT * 2 + 2,																											\
		LSN_STD_CONT_BUTTON_FULL_W, LSN_DEF_COMBO_HEIGHT, LSN_COMBOSTYLE_LIST, LSN_COMBOSTYLEEX_LIST )																							\
	LSN_QUICK_CONTR( LSW_LT_LABEL, BUTTON_ + LSN_SCPI_TURBO_DEADZONE_LABEL, LSN_LSTR( LSN_DEAD_ ),																								\
		(X_) + LSN_GROUP_LEFT, (Y_) + LSN_GROUP_TOP + (LSN_DEF_TRACKBAR_HEIGHT - LSN_DEF_STATIC_HEIGHT) / 2 + LSN_DEF_BUTTON_HEIGHT * 2 + LSN_DEF_COMBO_HEIGHT + 3,								\
		LSN_STD_CONT_BUTTON_LABEL_W, LSN_DEF_STATIC_HEIGHT, LSN_STATICSTYLE, 0 )																												\
	LSN_QUICK_CONTR( LSW_LT_TRACKBAR, BUTTON_ + LSN_SCPI_TURBO_DEADZONE_TRACKBAR, nullptr,																										\
		(X_) + LSN_GROUP_LEFT + LSN_STD_CONT_BUTTON_LABEL_W, (Y_) + LSN_GROUP_TOP + LSN_DEF_BUTTON_HEIGHT * 2 + LSN_DEF_COMBO_HEIGHT + 3,														\
		LSN_STD_CONT_BUTTON_BUTTON_W, LSN_DEF_TRACKBAR_HEIGHT, LSN_TRACKBAR_STYLE, 0 )

		// Buttons.
		LSN_QUICK_CONTR( LSW_LT_GROUPBOX, LSN_SCPI_DPAD_GROUP, LSN_LSTR( LSN_DIRECTIONAL_PAD ), LSN_STD_CONT_DPAD_LEFT, LSN_STD_CONT_DPAD_TOP,
			LSN_STD_CONT_DPAD_GROUP_W, LSN_STD_CONT_DPAD_GROUP_H, LSN_GROUPSTYLE, WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY )
		LSN_QUICK_BUTTON( LSN_SCPI_BUTTON_UP_START, LSN_LSTR( LSN_BUTTON_UP ), LSN_STD_CONT_DPAD_LEFT + LSN_STD_CONT_DPAD_H_MIDDLE - LSN_STD_CONT_BUTTON_GROUP_W / 2 + LSN_GROUP_LEFT,
			LSN_STD_CONT_DPAD_TOP + LSN_GROUP_TOP )
		LSN_QUICK_BUTTON( LSN_SCPI_BUTTON_LEFT_START, LSN_LSTR( LSN_BUTTON_LEFT ), LSN_STD_CONT_DPAD_LEFT + LSN_GROUP_LEFT,
			LSN_STD_CONT_DPAD_TOP + LSN_STD_CONT_DPAD_V_MIDDLE - LSN_STD_CONT_BUTTON_GROUP_H / 2 + LSN_GROUP_TOP )
		LSN_QUICK_BUTTON( LSN_SCPI_BUTTON_RIGHT_START, LSN_LSTR( LSN_BUTTON_RIGHT ), LSN_STD_CONT_DPAD_LEFT + LSN_STD_CONT_DPAD_W - LSN_STD_CONT_BUTTON_GROUP_W + LSN_GROUP_LEFT,
			LSN_STD_CONT_DPAD_TOP + LSN_STD_CONT_DPAD_V_MIDDLE - LSN_STD_CONT_BUTTON_GROUP_H / 2 + LSN_GROUP_TOP )
		LSN_QUICK_BUTTON( LSN_SCPI_BUTTON_DOWN_START, LSN_LSTR( LSN_BUTTON_DOWN ), LSN_STD_CONT_DPAD_LEFT + LSN_STD_CONT_DPAD_H_MIDDLE - LSN_STD_CONT_BUTTON_GROUP_W / 2 + LSN_GROUP_LEFT,
			LSN_STD_CONT_DPAD_TOP + LSN_STD_CONT_DPAD_H - LSN_STD_CONT_BUTTON_GROUP_H + LSN_GROUP_TOP )

		LSN_QUICK_CONTR( LSW_LT_GROUPBOX, LSN_SCPI_SS_GROUP, LSN_LSTR( LSN_SELECT_START ), LSN_STD_CONT_SS_LEFT, LSN_STD_CONT_SS_TOP,
			LSN_STD_CONT_SS_GROUP_W, LSN_STD_CONT_SS_GROUP_H, LSN_GROUPSTYLE, WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY )
		LSN_QUICK_BUTTON( LSN_SCPI_BUTTON_SELECT_START, LSN_LSTR( LSN_BUTTON_SELECT ), LSN_STD_CONT_SS_LEFT + LSN_GROUP_LEFT,
			LSN_STD_CONT_SS_TOP + LSN_GROUP_TOP )
		LSN_QUICK_BUTTON( LSN_SCPI_BUTTON_START_START, LSN_LSTR( LSN_BUTTON_START ), LSN_STD_CONT_SS_LEFT + LSN_GROUP_LEFT + LSN_STD_CONT_SS_START_LEFT,
			LSN_STD_CONT_SS_TOP + LSN_GROUP_TOP )

		LSN_QUICK_CONTR( LSW_LT_GROUPBOX, LSN_SCPI_BUTTON_GROUP, LSN_LSTR( LSN_BUTTONS ), LSN_STD_CONT_BUT_LEFT, LSN_STD_CONT_BUT_TOP,
			LSN_STD_CONT_BUT_GROUP_W, LSN_STD_CONT_BUT_GROUP_H, LSN_GROUPSTYLE, WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY )
		LSN_QUICK_BUTTON( LSN_SCPI_BUTTON_B_START, LSN_LSTR( LSN_BUTTON_B ), LSN_STD_CONT_BUT_LEFT + LSN_GROUP_LEFT,
			LSN_STD_CONT_BUT_TOP + LSN_GROUP_TOP )
		LSN_QUICK_BUTTON( LSN_SCPI_BUTTON_A_START, LSN_LSTR( LSN_BUTTON_A ), LSN_STD_CONT_BUT_LEFT + LSN_GROUP_LEFT + LSN_STD_CONT_BUT_A_LEFT,
			LSN_STD_CONT_BUT_TOP + LSN_GROUP_TOP )


#undef LSN_QUICK_BUTTON
#undef LSN_QUICK_CONTR
	};

#undef LSN_STD_CONT_H
#undef LSN_STD_CONT_W


	// == Functions.
	/**
	 * Creates the page.
	 *
	 * \param _pwParent The parent of the page.
	 * \param _pmwMainWindow A pointer to the main window for USB controller access.
	 * \return Returns the created widget.
	 */
	CWidget * CStdControllerPageLayout::CreatePage( CWidget * _pwParent, LSN_OPTIONS &_oOptions, lsn::CMainWindow * _pmwMainWindow ) {
		return CreatePage( _pwParent, _oOptions, _pmwMainWindow, m_wlPage, LSN_ELEMENTS( m_wlPage ) );
	}

	/**
	 * Creates the tab pages.
	 *
	 * \param _pwParent The parent widget.
	 * \param _pwlLayout The page layout.
	 * \param _sTotal The number of items to which _pwlLayout points.
	 * \return Returns the created page.
	 */
	CWidget * CStdControllerPageLayout::CreatePage( CWidget * _pwParent, LSN_OPTIONS &_oOptions, lsn::CMainWindow * _pmwMainWindow, const LSW_WIDGET_LAYOUT * _pwlLayout, size_t _sTotal ) {
		lsn::CLayoutManager * plmLayout = static_cast<lsn::CLayoutManager *>(lsw::CBase::LayoutManager());
		CStdControllerPage::LSN_CONTROLLER_SETUP_DATA scdData = {
			.poOptions = &_oOptions,
			.pmwMainWindow = _pmwMainWindow
		};
		CWidget * pwWidget = plmLayout->CreateDialogX( _pwlLayout, _sTotal, _pwParent, reinterpret_cast<uint64_t>(&scdData) );
		if ( pwWidget ) {
			// Success.  Do stuff.
		}
		return pwWidget;
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

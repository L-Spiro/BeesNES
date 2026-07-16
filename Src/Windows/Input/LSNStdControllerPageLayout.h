#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A dialog shaped like a standard controller.
 */

#pragma once

#include "../../LSNLSpiroNes.h"
#include "../../Options/LSNOptions.h"
#include "../WinUtilities/LSNWinUtilities.h"

#include <Layout/LSWMenuLayout.h>
#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace lsn {

#define LSN_STD_CONT_BUTTON_FULL_H						((LSN_DEF_BUTTON_HEIGHT * 2) + LSN_DEF_COMBO_HEIGHT + LSN_DEF_TRACKBAR_HEIGHT + 3)
#define LSN_STD_CONT_BUTTON_LABEL_W						(25)
#define LSN_STD_CONT_BUTTON_BUTTON_W					(100 - (LSN_STD_CONT_BUTTON_LABEL_W))
#define LSN_STD_CONT_BUTTON_FULL_W						(LSN_STD_CONT_BUTTON_LABEL_W + LSN_STD_CONT_BUTTON_BUTTON_W)
#define LSN_STD_CONT_BUTTON_GROUP_W						(LSN_STD_CONT_BUTTON_FULL_W + LSN_GROUP_LEFT + LSN_GROUP_RIGHT)
#define LSN_STD_CONT_BUTTON_GROUP_H						(LSN_STD_CONT_BUTTON_FULL_H + LSN_GROUP_TOP + LSN_GROUP_BOTTOM)
#define LSN_STD_CONT_DPAD_LEFT							LSN_LEFT_JUST
#define LSN_STD_CONT_DPAD_TOP							LSN_TOP_JUST
#define LSN_STD_CONT_DPAD_W								(LSN_STD_CONT_BUTTON_GROUP_W * 2 + LSN_LEFT_JUST * 20)
#define LSN_STD_CONT_DPAD_H								(LSN_STD_CONT_BUTTON_GROUP_H * 3 + LSN_TOP_JUST * 4)
#define LSN_STD_CONT_DPAD_H_MIDDLE						(LSN_STD_CONT_DPAD_W / 2)
#define LSN_STD_CONT_DPAD_V_MIDDLE						(LSN_STD_CONT_DPAD_H / 2)
#define LSN_STD_CONT_DPAD_GROUP_W						(LSN_STD_CONT_DPAD_W + LSN_GROUP_LEFT + LSN_GROUP_RIGHT)
#define LSN_STD_CONT_DPAD_GROUP_H						(LSN_STD_CONT_DPAD_H + LSN_GROUP_TOP + LSN_GROUP_BOTTOM)
#define LSN_STD_CONT_SS_LEFT							(LSN_STD_CONT_DPAD_LEFT + LSN_STD_CONT_DPAD_GROUP_W + LSN_LEFT_JUST * 2)
#define LSN_STD_CONT_SS_TOP								(LSN_STD_CONT_DPAD_TOP + LSN_STD_CONT_DPAD_GROUP_H - LSN_GROUP_TOP - LSN_GROUP_BOTTOM - LSN_STD_CONT_BUTTON_GROUP_H)
#define LSN_STD_CONT_SS_START_LEFT						(LSN_STD_CONT_BUTTON_GROUP_W + LSN_LEFT_JUST * 4)
#define LSN_STD_CONT_SS_GROUP_W							(LSN_STD_CONT_SS_START_LEFT + LSN_STD_CONT_BUTTON_GROUP_W + LSN_GROUP_LEFT + LSN_GROUP_RIGHT)
#define LSN_STD_CONT_SS_GROUP_H							(LSN_STD_CONT_BUTTON_GROUP_H + LSN_GROUP_TOP + LSN_GROUP_BOTTOM)
#define LSN_STD_CONT_BUT_GROUP_W						LSN_STD_CONT_SS_GROUP_W
#define LSN_STD_CONT_BUT_GROUP_H						LSN_STD_CONT_SS_GROUP_H
#define LSN_STD_CONT_BUT_LEFT							(LSN_STD_CONT_SS_LEFT + LSN_STD_CONT_SS_GROUP_W + LSN_LEFT_JUST * 2)
#define LSN_STD_CONT_BUT_TOP							(LSN_STD_CONT_DPAD_TOP + (LSN_STD_CONT_DPAD_GROUP_H / 2) - (LSN_STD_CONT_BUT_GROUP_H / 2))
#define LSN_STD_CONT_BUT_A_LEFT							LSN_STD_CONT_SS_START_LEFT
#define LSN_STD_CONT_DEVICES_LIST_H						(LSN_STD_CONT_SS_TOP - LSN_STD_CONT_DPAD_TOP - (LSN_GROUP_TOP + LSN_GROUP_BOTTOM) - LSN_TOP_JUST)//(LSN_DEF_BUTTON_HEIGHT * 8 + LSN_TOP_JUST * 7)
#define LSN_STD_CONT_DEVICES_GROUP_H					((LSN_GROUP_TOP + LSN_GROUP_BOTTOM) + LSN_STD_CONT_DEVICES_LIST_H)
#define LSN_STD_CONT_QUICK_GROUP_LEFT					LSN_STD_CONT_BUT_LEFT
#define LSN_STD_CONT_QUICK_GROUP_H						(LSN_STD_CONT_BUT_TOP - LSN_STD_CONT_DPAD_TOP - LSN_TOP_JUST)
#define LSN_STD_CONT_QUICK_SET_TURBO_TOP				(LSN_STD_CONT_DPAD_TOP + LSN_GROUP_TOP + LSN_DEF_BUTTON_HEIGHT + LSN_TOP_JUST)
#define LSN_STD_CONT_QUICK_SET_TURBO_BOTTOM				(LSN_STD_CONT_QUICK_SET_TURBO_TOP + LSN_DEF_BUTTON_HEIGHT)
#define LSN_STD_CONT_QUICK_CLEAR_TOP					(LSN_STD_CONT_DPAD_TOP + LSN_STD_CONT_QUICK_GROUP_H - LSN_DEF_BUTTON_HEIGHT - LSN_GROUP_BOTTOM)
#define LSN_STD_CONT_QUICK_LABEL_TOP					(LSN_STD_CONT_QUICK_SET_TURBO_BOTTOM + LSN_TOP_JUST)
#define LSN_STD_CONT_QUICK_LABEL_HEIGHT					(LSN_STD_CONT_QUICK_CLEAR_TOP - LSN_STD_CONT_QUICK_LABEL_TOP - LSN_TOP_JUST)
#define LSN_STD_CONT_PRESET_GROUP_TOP					(LSN_STD_CONT_BUT_TOP + LSN_STD_CONT_BUT_GROUP_H + LSN_TOP_JUST)
#define LSN_STD_CONT_PRESET_GROUP_H						((LSN_GROUP_TOP + LSN_GROUP_BOTTOM) + LSN_DEF_COMBO_HEIGHT)

#define LSN_STD_CONT_W									(LSN_LEFT_JUST + LSN_LEFT_JUST + LSN_STD_CONT_BUT_LEFT + LSN_STD_CONT_BUT_GROUP_W + LSN_LEFT_JUST + LSN_LEFT_JUST)
#define LSN_STD_CONT_H									(LSN_STD_CONT_DPAD_GROUP_H + 20)

	class												CMainWindow;
	
	/**
	 * Class CStdControllerPageLayout
	 * \brief A dialog shaped like a standard controller.
	 *
	 * Description: A dialog shaped like a standard controller.
	 */
	class CStdControllerPageLayout {
	public :
		// == Functions.
		/**
		 * Creates the page.
		 *
		 * \param _pwParent The parent of the page.
		 * \param _ioOptions The input options to potentially modify.
		 * \param _pmwMainWindow A pointer to the main window for USB controller access.
		 * \param _stIdx The page's configuration index.
		 * \return Returns the created widget.
		 */
		static CWidget *								CreatePage( CWidget * _pwParent, LSN_INPUT_OPTIONS &_ioOptions, lsn::CMainWindow * _pmwMainWindow, size_t _stIdx );


	protected :
		// == Members.
		/** The layout for the per-game-setup panel. */
		static LSW_WIDGET_LAYOUT						m_wlPage[];


		// == Functions.
		/**
		 * Creates the pages.
		 *
		 * \param _pwParent The parent widget.
		 * \param _ioOptions The input options to potentially modify.
		 * \param _pmwMainWindow A pointer to the main window for USB controller access.
		 * \param _stIdx The page's configuration index.
		 * \param _pwlLayout The page layout.
		 * \param _sTotal The number of items to which _pwlLayout points.
		 * \return Returns the created page.
		 */
		static CWidget *								CreatePage( CWidget * _pwParent, LSN_INPUT_OPTIONS &_ioOptions, lsn::CMainWindow * _pmwMainWindow, size_t _stIdx, const LSW_WIDGET_LAYOUT * _pwlLayout, size_t _sTotal );
 		
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The input window layout.
 */

#pragma once

#include "../../LSNLSpiroNes.h"
#include <Layout/LSWMenuLayout.h>
#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace lsn {
	
	/**
	 * Class CInputWindowLayout
	 * \brief The input window layout.
	 *
	 * Description: The input window layout.
	 */
	class CInputWindowLayout {
	public :
		// == Enumerations.
		// Control ID's.
		enum LSN_INPUT_WINDOW_IDS : WORD {
			LSN_IWI_NONE,
			LSN_IWI_MAINWINDOW,
			LSN_IWI_TAB,
			LSN_IWI_OK,
			LSN_IWI_CANCEL,

			LSN_IWI_GENERAL_GROUP,
			LSN_IWI_GENERAL_CONSOLE_TYPE_LABEL,
			LSN_IWI_GENERAL_CONSOLE_TYPE_COMBO,
			LSN_IWI_GENERAL_USE_GLOBAL_CHECK,

#define LSN_IWI_PER_GAME( NAME )				LSN_IWI_PER_GAME_SETTINGS_ ## NAME
#define LSN_IWI_GLOBAL( NAME )					LSN_IWI_GLOBAL_SETTINGS_ ## NAME
#define LSN_BOTH( NAME )						LSN_IWI_PER_GAME( NAME ), LSN_IWI_GLOBAL( NAME )
			LSN_BOTH( PANEL ),
			LSN_BOTH( GROUP ),
			LSN_BOTH( PLAYER_1_LABEL ),
			LSN_BOTH( PLAYER_1_COMBO ),
			LSN_BOTH( PLAYER_1_BUTTON ),
			LSN_BOTH( PLAYER_2_LABEL ),
			LSN_BOTH( PLAYER_2_COMBO ),
			LSN_BOTH( PLAYER_2_BUTTON ),

			LSN_BOTH( USE_FOUR_SCORE_CHECK ),
			LSN_BOTH( EXPAND_LABEL ),
			LSN_BOTH( EXPAND_COMBO ),
			LSN_BOTH( EXPAND_BUTTON ),

			LSN_BOTH( PLAYER_3_LABEL ),
			LSN_BOTH( PLAYER_3_COMBO ),
			LSN_BOTH( PLAYER_3_BUTTON ),

			LSN_BOTH( PLAYER_4_LABEL ),
			LSN_BOTH( PLAYER_4_COMBO ),
			LSN_BOTH( PLAYER_4_BUTTON ),
#undef LSN_BOTH
		};


		// == Functions.
		/**
		 * Creates the input-configuration window.
		 *
		 * \param _pwParent The parent of the dialog.
		 * \return Returns TRUE if the dialog was created successfully.
		 */
		static BOOL								CreateInputDialog( CWidget * _pwParent );

		/**
		 * Creates the global-settings page.
		 *
		 * \param _pwParent the parent of the page.
		 * \return Returns the created widget.
		 */
		static CWidget *						CreateGlobalPage( CWidget * _pwParent );


	protected :
		// == Members.
		/** The layout for the main window. */
		static LSW_WIDGET_LAYOUT				m_wlInputWindow[];
		/** The layout for the global-setup panel. */
		static LSW_WIDGET_LAYOUT				m_wlGlobalPanel[];
		/** The class for the main window. */
		static ATOM								m_aInputClass;


		// == Functions.
		/**
		 * Creates the tab pages.
		 *
		 * \param _pwParent The parent widget.
		 * \param _pwlLayout The page layout.
		 * \param _sTotal The number of items to which _pwlLayout points.
		 * \return Returns the created page.
		 */
		static CWidget *						CreatePage( CWidget * _pwParent, const LSW_WIDGET_LAYOUT * _pwlLayout, size_t _sTotal );
		
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

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
#include "../../Options/LSNOptions.h"
#include <Layout/LSWMenuLayout.h>
#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace lsn {

	class										CMainWindow;
	
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


#define LSN_IWI_PER_GAME( NAME )				LSN_IWI_SETTINGS_ ## NAME
#define LSN_IWI_GLOBAL( NAME )					LSN_IWI_SETTINGS_ ## NAME
#define LSN_BOTH( NAME )						LSN_IWI_GLOBAL( NAME )
/*#define LSN_IWI_PER_GAME( NAME )				LSN_IWI_PER_GAME_SETTINGS_ ## NAME
#define LSN_IWI_GLOBAL( NAME )					LSN_IWI_GLOBAL_SETTINGS_ ## NAME
#define LSN_BOTH( NAME )						LSN_IWI_PER_GAME( NAME ), LSN_IWI_GLOBAL( NAME )*/
			LSN_IWI_PER_GAME_SETTINGS_PANEL,
			LSN_IWI_GLOBAL_SETTINGS_PANEL,
			//LSN_BOTH( PANEL ),
			LSN_BOTH( GENERAL_GROUP ),
			LSN_BOTH( CONSOLE_TYPE_LABEL ),
			LSN_BOTH( CONSOLE_TYPE_COMBO ),
			LSN_BOTH( USE_GLOBAL_CHECK ),
			LSN_BOTH( SETTINGS_GROUP ),
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
		 * \param _oOptions A reference to the options object.
		 * \param _pmwMainWindow A pointer to the main window to allow access to USB controllers.
		 * \return Returns TRUE if the dialog was created successfully.
		 */
		static BOOL								CreateInputDialog( CWidget * _pwParent, LSN_OPTIONS &_oOptions, lsn::CMainWindow * _pmwMainWindow );

		/**
		 * Creates the global-settings page.
		 *
		 * \param _pwParent the parent of the page.
		 * \param _oOptions A reference to the options object.
		 * \param _pmwMainWindow A pointer to the main window to allow access to USB controllers.
		 * \return Returns the created widget.
		 */
		static CWidget *						CreateGlobalPage( CWidget * _pwParent, LSN_OPTIONS &_oOptions, lsn::CMainWindow * _pmwMainWindow );

		/**
		 * Creates the per-game-settings page.
		 *
		 * \param _pwParent the parent of the page.
		 * \param _oOptions A reference to the options object.
		 * \param _pmwMainWindow A pointer to the main window to allow access to USB controllers.
		 * \return Returns the created widget.
		 */
		static CWidget *						CreatePerGamePage( CWidget * _pwParent, LSN_OPTIONS &_oOptions, lsn::CMainWindow * _pmwMainWindow );


	protected :
		// == Members.
		/** The layout for the main window. */
		static LSW_WIDGET_LAYOUT				m_wlInputWindow[];
		/** The layout for the global-setup panel. */
		static LSW_WIDGET_LAYOUT				m_wlGlobalPanel[];
		/** The layout for the per-game-setup panel. */
		static LSW_WIDGET_LAYOUT				m_wlPerGamePanel[];


		// == Functions.
		/**
		 * Creates the tab pages.
		 *
		 * \param _pwParent The parent widget.
		 * \param _pwlLayout The page layout.
		 * \param _sTotal The number of items to which _pwlLayout points.
		 * \param _oOptions A reference to the options object.
		 * \param _pmwMainWindow A pointer to the main window to allow access to USB controllers.
		 * \return Returns the created page.
		 */
		static CWidget *						CreatePage( CWidget * _pwParent, const LSW_WIDGET_LAYOUT * _pwlLayout, size_t _sTotal, LSN_OPTIONS &_oOptions, lsn::CMainWindow * _pmwMainWindow );
 		
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

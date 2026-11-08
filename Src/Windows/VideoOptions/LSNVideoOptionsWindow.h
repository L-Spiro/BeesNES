#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The Video Options dialog.
 */

#pragma once

#include "LSNVideoOptionsWindowLayout.h"

#include <MainWindow/LSWMainWindow.h>

#include <memory>
#include <vector>

using namespace lsw;

namespace lsn {

	class													CVideoOptionsPage;
	class													CVideoOptionsSettingsPage;

	/**
	 * Class CVideoOptionsWindow
	 * \brief The Video Options dialog.
	 *
	 * Description: The Video Options dialog.  It has 3 tabs:  Host Machine, Global Settings, and Per-Game Settings (whose check box turns
	 *	the per-game settings on or off).  Changes are applied immediately; Cancel restores the settings the dialog started with.
	 */
	class CVideoOptionsWindow : public lsw::CMainWindow {
	public :
		CVideoOptionsWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Functions.
		/**
		 * The WM_INITDIALOG handler.
		 *
		 * \return Returns an LSW_HANDLED code.
		 */
		LSW_HANDLED											InitDialog();

		/**
		 * The WM_CLOSE handler.
		 *
		 * \return Returns an LSW_HANDLED code.
		 */
		LSW_HANDLED											Close();

		/**
		 * Handles the WM_COMMAND message.
		 *
		 * \param _wCtrlCode 0 = from menu, 1 = from accelerator, otherwise it is a Control-defined notification code.
		 * \param _wId The ID of the control if _wCtrlCode is not 0 or 1.
		 * \param _pwSrc The source control if _wCtrlCode is not 0 or 1.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED									Command( WORD _wCtrlCode, WORD _wId, CWidget * _pwSrc );

		/**
		 * Handles WM_COMMAND from a menu.
		 * \brief Invoked for menu command selections.
		 *
		 * \param _wId The menu command identifier.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED									MenuCommand( WORD _wId );

		/**
		 * Informs the control that a child tab control has just had a check toggled.
		 *
		 * \param _pwTab A pointer to the tab control.
		 * \param _iTab The index of the tab that was just toggled.
		 **/
		virtual void										TabToggled( CWidget * _pwTab, int _iTab );


	protected :
		// == Enumerations.
		/** The tabs. */
		enum LSN_TABS : int {
			LSN_T_HOST,																							/**< The Host Machine tab. */
			LSN_T_GLOBAL,																						/**< The Global Settings tab. */
			LSN_T_PER_GAME,																						/**< The Per-Game Settings tab. */
		};


		// == Members.
		CVideoOptionsWindowLayout::LSN_VIDEO_OPTIONS_DATA *	m_pvodData;									/**< The data of the dialog. */
		CVideoOptionsWindowLayout::LSN_PAGE_DATA			m_pdHost;									/**< The data of the Host Machine page. */
		CVideoOptionsWindowLayout::LSN_PAGE_DATA			m_pdGlobal;									/**< The data of the Global Settings page. */
		CVideoOptionsWindowLayout::LSN_PAGE_DATA			m_pdPerGame;								/**< The data of the Per-Game Settings page. */
		std::vector<CWidget *>								m_vPages;									/**< The pages. */
		LSN_VIDEO_FILTER_OPTIONS							m_vfoGlobalBackup;							/**< The global filter settings when the dialog opened. */
		LSN_VIDEO_FILTER_OPTIONS							m_vfoPerGameBackup;							/**< The per-game filter settings when the dialog opened. */
		LSN_HOST_DISPLAY_OPTIONS							m_hdoHostBackup;							/**< The host-display settings when the dialog opened. */
		LSN_PALETTE_OPTIONS									m_poGlobalPaletteBackup[LSN_PM_CONSOLE_TOTAL];	/**< The global palette settings when the dialog opened. */
		LSN_PALETTE_OPTIONS									m_poPerGamePaletteBackup[LSN_PM_CONSOLE_TOTAL];	/**< The per-game palette settings when the dialog opened. */


		// == Functions.
		/**
		 * Restores the settings the dialog started with, applies them, and closes the dialog.
		 *
		 * \return Returns an LSW_HANDLED code.
		 */
		LSW_HANDLED											CancelAndClose();

		/**
		 * Determines whether the per-game settings are in use.
		 *
		 * \return Returns true if any region uses its per-game settings.
		 */
		bool												PerGameInUse() const;

		/**
		 * Applies the options to the emulator.
		 */
		void												ApplyChanges();


	private :
		typedef lsw::CMainWindow							Parent;
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

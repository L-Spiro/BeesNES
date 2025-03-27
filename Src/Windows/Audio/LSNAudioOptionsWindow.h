#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The audio options window.
 */

#pragma once

#include "LSNAudioOptionsWindowLayout.h"

#include <MainWindow/LSWMainWindow.h>

using namespace lsw;

namespace lsn {

	class													CMainWindow;

	/**
	 * Class CAudioOptionsWindow
	 * \brief The audio options window.
	 *
	 * Description: The audio options window.
	 */
	class CAudioOptionsWindow : public lsw::CMainWindow {
	public :
		CAudioOptionsWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


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
		 * Saves the current input configuration and closes the dialog.
		 */
		void												SaveAndClose();

		/**
		 * Informs the control that a child tab control has just had a check toggled.
		 * 
		 * \param _pwTab A pointer to the tab control.
		 * \param _iTab The index of the tab that was just toggled.
		 **/
		virtual void										TabToggled( CWidget * _pwTab, int _iTab );


	protected :
		// == Members.
		/** The options object. */
		LSN_OPTIONS *										m_poOptions;
		std::vector<CWidget *>								m_vPages;													/**< The pages. */

	private :
		typedef CAudioOptionsWindowLayout					Layout;
		typedef lsw::CMainWindow							Parent;
	};


}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

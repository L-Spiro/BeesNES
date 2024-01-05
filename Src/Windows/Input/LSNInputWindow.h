#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The input window.
 */

#pragma once

#include "LSNInputWindowLayout.h"
#include "LSNInputPage.h"
#include <MainWindow/LSWMainWindow.h>

using namespace lsw;

namespace lsn {

	class											CMainWindow;

	/**
	 * Class CInputWindow
	 * \brief The input window.
	 *
	 * Description: The input window.
	 */
	class CInputWindow : public lsw::CMainWindow {
	public :
		CInputWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Types.
		/** The structure to pass to _ui64Data when creating this window. */
		struct LSN_CONTROLLER_SETUP_DATA {
			/** The options object. */
			LSN_OPTIONS *							poOptions;
			/** The main window, which allows us access to the USB controllers. */
			lsn::CMainWindow *						pmwMainWindow;
		};


		// == Functions.
		/**
		 * The WM_INITDIALOG handler.
		 *
		 * \return Returns an LSW_HANDLED code.
		 */
		LSW_HANDLED								InitDialog();

		/**
		 * The WM_CLOSE handler.
		 *
		 * \return Returns an LSW_HANDLED code.
		 */
		LSW_HANDLED								Close();

		/**
		 * Handles the WM_COMMAND message.
		 *
		 * \param _wCtrlCode 0 = from menu, 1 = from accelerator, otherwise it is a Control-defined notification code.
		 * \param _wId The ID of the control if _wCtrlCode is not 0 or 1.
		 * \param _pwSrc The source control if _wCtrlCode is not 0 or 1.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED						Command( WORD _wCtrlCode, WORD _wId, CWidget * _pwSrc );

		/**
		 * Virtual client rectangle.  Can be used for things that need to be adjusted based on whether or not status bars, toolbars, etc. are present.
		 *	pwChild can be nullptr.  If not nullptr, it is assumed to be a child of the widget, and this widget might create a specific rectangle for
		 *	the pwChild, such as for splitter controls.
		 *
		 * \param pwChild The child window for whom the virtual client rectangle should be returned.
		 * \return Returns the virtual client rectangle for this window for the given widget.
		 */
		virtual const LSW_RECT					VirtualClientRect( const CWidget * pwChild ) const;

		/**
		 * Saves the current input configuration and closes the dialog.
		 */
		void									SaveAndClose();




	protected :
		// == Members.
		/** The input-settings pages. */
		std::vector<CInputPage *>				m_vPages;
		/** The options object. */
		LSN_OPTIONS *							m_poOptions;
		/** The main window. */
		lsn::CMainWindow *						m_pmwMainWindow;

	private :
		typedef CInputWindowLayout				Layout;
		typedef lsw::CMainWindow				Parent;
	};


}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

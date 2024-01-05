#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The controller-configuration dialog.
 */

#pragma once

#include "../../LSNLSpiroNes.h"
#include "../../Localization/LSNLocalization.h"
#include "LSNControllerSetupWindowLayout.h"
#include "LSNInputWindowLayout.h"
#include "LSNStdControllerPage.h"
#include <MainWindow/LSWMainWindow.h>
#include <Tab/LSWTab.h>

using namespace lsw;

namespace lsn {

	class											CMainWindow;

	/**
	 * Class CControllerSetupWindow
	 * \brief The controller-configuration dialog.
	 *
	 * Description: The controller-configuration dialog.
	 */
	class CControllerSetupWindow : public lsw::CMainWindow {
	public :
		CControllerSetupWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		~CControllerSetupWindow();


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
		LSW_HANDLED									InitDialog();

		/**
		 * The WM_CLOSE handler.
		 *
		 * \return Returns an LSW_HANDLED code.
		 */
		LSW_HANDLED									Close();

		/**
		 * Handles the WM_COMMAND message.
		 *
		 * \param _wCtrlCode 0 = from menu, 1 = from accelerator, otherwise it is a Control-defined notification code.
		 * \param _wId The ID of the control if _wCtrlCode is not 0 or 1.
		 * \param _pwSrc The source control if _wCtrlCode is not 0 or 1.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED							Command( WORD _wCtrlCode, WORD _wId, CWidget * _pwSrc );

		/**
		 * Gets the tab control.
		 * 
		 * \return Returns the main tab control.
		 **/
		CTab *										GetTab() const {
			return const_cast<CTab *>(static_cast<const CTab *>(FindChild( CControllerSetupWindowLayout::LSN_CSW_TAB )));
		}

		/**
		 * Window rectangle.
		 * 
		 * \param _pwChild If not nullptr, this is the child control for which to determine the window rectangle.
		 * \return Returns the window rectangle for this control or of a given child control.
		 **/
		virtual LSW_RECT							WindowRect( const CWidget * _pwChild = nullptr ) const;

		/**
		 * Client rectangle.
		 * 
		 * \param _pwChild If not nullptr, this is the child control for which to determine the client rectangle.
		 * \return Returns the client rectangle for this control or of a given child control.
		 **/
		virtual LSW_RECT							ClientRect( const CWidget * /*_pwChild*/ = nullptr ) const;


	protected :
		// == Members.
		/**  Controller configuration pages. */
		std::vector<CStdControllerPage *>			m_vPages;
		/** The options object. */
		LSN_OPTIONS *								m_poOptions;
		/** The main window, which allows us access to the USB controllers. */
		lsn::CMainWindow *							m_pmwMainWindow;


	private :
		typedef lsw::CMainWindow					Parent;

	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

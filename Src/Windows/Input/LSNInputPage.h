#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: An input-settings page (a tab in the main input-configuration dialog).
 */

#pragma once

#include "LSNInputWindowLayout.h"
#include "../../Options/LSNOptions.h"
#include <MainWindow/LSWMainWindow.h>

using namespace lsw;

namespace lsn {

	class											CMainWindow;

	/**
	 * Class CInputPage
	 * \brief An input-settings page (a tab in the main input-configuration dialog).
	 *
	 * Description: An input-settings page (a tab in the main input-configuration dialog).
	 */
	class CInputPage : public lsw::CWidget {
	public :
		CInputPage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 ) :
			lsw::CWidget( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
			m_poOptions( reinterpret_cast<LSN_CONTROLLER_SETUP_DATA *>(_ui64Data)->poOptions ),
			m_pmwMainWindow( reinterpret_cast<LSN_CONTROLLER_SETUP_DATA *>(_ui64Data)->pmwMainWindow ),
			m_pioInputOptions( nullptr ) {
		}


		// == Types.
		/** The structure to pass to _ui64Data when creating this window. */
		struct LSN_CONTROLLER_SETUP_DATA {
			/** The options object. */
			LSN_OPTIONS *						poOptions;
			/** The main window, which allows us access to the USB controllers. */
			lsn::CMainWindow *					pmwMainWindow;
		};


		// == Functions.
		/**
		 * Get the name of the page.
		 *
		 * \return Returns the name of the page.
		 */
		virtual std::wstring					GetName() const { return std::wstring(); }

		/**
		 * Saves the current input configuration.
		 */
		virtual void							Save() {}


	protected :
		// == Members.
		/** Pointer to the main window cor controller access. */
		lsn::CMainWindow *						m_pmwMainWindow;
		/** The options object. */
		LSN_OPTIONS *							m_poOptions;
		/** The input options that apply to this page. */
		LSN_INPUT_OPTIONS *						m_pioInputOptions;
		/** The temporary copy of the options. */
		LSN_INPUT_OPTIONS						m_ioTmpOptions;
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

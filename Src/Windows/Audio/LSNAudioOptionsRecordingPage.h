#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The audio recording window.
 */

#pragma once

#include "LSNAudioOptionsWindowLayout.h"
#include <MainWindow/LSWMainWindow.h>

using namespace lsw;

namespace lsn {

	class													CMainWindow;

	/**
	 * Class CAudioOptionsRecordingPage
	 * \brief The audio recording window.
	 *
	 * Description: The audio recording window.
	 */
	class CAudioOptionsRecordingPage : public lsw::CWidget {
	public :
		CAudioOptionsRecordingPage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Functions.
		/**
		 * The WM_INITDIALOG handler.
		 *
		 * \return Returns an LSW_HANDLED code.
		 */
		LSW_HANDLED											InitDialog();

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
		 * Verifies the inputs.
		 * 
		 * \param _wsMsg The error message to display.
		 * \return Returns the control that failed or nullptr.
		 **/
		CWidget *											Verify( std::wstring &_wsMsg );

		/**
		 * Saves the current input configuration and closes the dialog.
		 */
		void												Save();

		/**
		 * Updates the dialog.
		 **/
		void												Update();

		/**
		 * Sets the Hz.
		 * 
		 * \param _ui32Hz The Hz to set.
		 **/
		void												SetHz( uint32_t _ui32Hz );


	protected :
		// == Members.
		/** The options object. */
		LSN_OPTIONS *										m_poOptions;
		/** The actual Hz set in the settings. */
		uint32_t											m_ui32SettingsHz = 0;

	private :
		typedef CAudioOptionsWindowLayout					Layout;
		typedef lsw::CWidget								Parent;
	};


}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

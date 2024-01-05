#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A page for configuring the buttons on a standard controller.
 */

#pragma once

#include "../../LSNLSpiroNes.h"
#include "../../Localization/LSNLocalization.h"
#include "../../Input/LSNControllerListener.h"
#include "../../Options/LSNOptions.h"
#include <Button/LSWButton.h>
#include <ComboBox/LSWComboBox.h>
#include <Helpers/LSWInputListenerBase.h>
#include <TrackBar/LSWTrackBar.h>
#include <MainWindow/LSWMainWindow.h>

using namespace lsw;

namespace lsn {

	class											CMainWindow;

	/**
	 * Class CStdControllerPage
	 * \brief A template for dialog pages embedded into other dialogs/windows.
	 *
	 * Description: A template for dialog pages embedded into other dialogs/windows.
	 */
	class CStdControllerPage : public lsw::CWidget, public lsw::CInputListenerBase, public lsn::CControllerListener {
	public :
		CStdControllerPage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 ) :
			lsw::CWidget( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
			m_poOptions( reinterpret_cast<LSN_CONTROLLER_SETUP_DATA *>(_ui64Data)->poOptions ),
			m_pmwMainWindow( reinterpret_cast<LSN_CONTROLLER_SETUP_DATA *>(_ui64Data)->pmwMainWindow ) {
		}
		~CStdControllerPage() {
		}


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
		 * Gets the text for the "Listening" state.
		 *
		 * \return Returns the text to be used during the listening state.
		 */
		virtual const wchar_t *						ListeningText() const { return LSN_LSTR( LSN_LISTENING_ ); }

		/**
		 * Gets the default text for the listenable controls.  This text is used after listening fails.
		 *
		 * \return Returns the text string to be used after failed listening.
		 */
		virtual const wchar_t *						DefaultText() const { return L""; }

		/**
		 * Detrmines if key cominations are allowed, such as Shift+Key, Alt+Key, or Control+Key.  If false, Shift, Control, and Alt
		 *	can be used as keys by themselves.
		 *
		 * \return Return false if Sift, Control, and Alt should be possible keys by themselves, or true if Shift, Control, and Alt should be
		 *	used as modifiers for other keys.
		 */
		virtual bool								AllowKeyCombos() { return false; }

		/**
		 * Called after focus on the control is lost or a key has been registered.
		 *
		 * \param _pwControl The control used for listening.
		 * \param _bSuccess Set to true if the call is due to a key being registered.
		 * \return Returns true if the listening state is LSW_LS_LISTENING and the old control procedure was restored.
		 */
		virtual bool								StopListening_Keyboard( CWidget * _pwControl, bool _bSuccess );

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
		 * Fully updates the dialog based on current buttons and settings.
		 */
		void										UpdateDialog();

		/**
		 * Gets an array of the main buttons.  There will be 8 values in the array.
		 * 
		 * \return Returns an array containing the 8 primary buttons.
		 **/
		std::vector<lsw::CButton *>					MainButtons();

		/**
		 * Gets an array of the turbo buttons.  There will be 8 values in the array.
		 * 
		 * \return Returns an array containing the 8 turbo buttons.
		 **/
		std::vector<lsw::CButton *>					TurboButtons();

		/**
		 * Gets an array of the turbo combos.  There will be 8 values in the array, which will correspond to the 8 turbo buttons returned by TurboButtons().
		 * 
		 * \return Returns an array containing the 8 turbo combos.
		 **/
		std::vector<lsw::CComboBox *>				TurboCombos();

		/**
		 * Gets an array of the dead-zone trackbars.  There will be 8 values in the array.
		 * 
		 * \return Returns an array containing the 8 dead-zone trackbars.
		 **/
		std::vector<lsw::CTrackBar *>				DeadZoneTrackBars();


	protected :
		// == Members.
		/** The options object. */
		LSN_OPTIONS *								m_poOptions;
		/** The main window. */
		lsn::CMainWindow *							m_pmwMainWindow;

		/** The primary buttons. */
		CInputListenerBase::LSW_KEYBOARD_RESULT		m_krMainButtons[8];
		/** The turbo buttons. */
		CInputListenerBase::LSW_KEYBOARD_RESULT		m_krTurboButtons[8];
		/** The button to which we are currently listening. */
		size_t										m_stListeningIdx;
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

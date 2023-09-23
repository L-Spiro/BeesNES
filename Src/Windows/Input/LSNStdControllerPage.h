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
#include <Helpers/LSWInputListenerBase.h>
#include <MainWindow/LSWMainWindow.h>

using namespace lsw;

namespace lsn {

	/**
	 * Class CStdControllerPage
	 * \brief A template for dialog pages embedded into other dialogs/windows.
	 *
	 * Description: A template for dialog pages embedded into other dialogs/windows.
	 */
	class CStdControllerPage : public lsw::CWidget, public lsw::CInputListenerBase {
	public :
		CStdControllerPage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 ) :
			lsw::CWidget( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
		}


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


	protected :
		// == Members.
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

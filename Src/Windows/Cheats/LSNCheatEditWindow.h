#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The Add/Edit/Duplicate Cheat dialog.  Shows a cheat's title, description, and codes (side-by-side as Game Genie codes and as
 *	AAAA:VV:CC codes, each updating the other), whether it is enabled, and its hotkeys.
 */

#pragma once

#include "../../LSNLSpiroNes.h"
#include "../../Cheats/LSNCheatManager.h"
#include "../WinUtilities/LSNWinUtilities.h"
#include "LSNCheatEditWindowLayout.h"

#include <Helpers/LSWInputListenerBase.h>
#include <MainWindow/LSWMainWindow.h>

#include <string>
#include <vector>

using namespace lsw;

namespace lsn {

	/**
	 * Class CCheatEditWindow
	 * \brief The Add/Edit/Duplicate Cheat dialog.
	 *
	 * Description: The Add/Edit/Duplicate Cheat dialog.  Shows a cheat's title, description, and codes (side-by-side as Game Genie codes and
	 *	as AAAA:VV:CC codes, each updating the other), whether it is enabled, and its hotkeys.
	 */
	class CCheatEditWindow : public lsw::CMainWindow, public lsw::CInputListenerBase {
	public :
		CCheatEditWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Types.
		/** The parent type. */
		typedef lsw::CMainWindow										Parent;


		// == Functions.
		/**
		 * The WM_INITDIALOG handler.
		 *
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED												InitDialog();

		/**
		 * The WM_CLOSE handler.
		 *
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED												Close();

		/**
		 * Handles the WM_COMMAND message.
		 *
		 * \param _wCtrlCode 0 = from menu, 1 = from accelerator, otherwise it is a Control-defined notification code.
		 * \param _wId The ID of the control if _wCtrlCode is not 0 or 1.
		 * \param _pwSrc The source control if _wCtrlCode is not 0 or 1.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED												Command( WORD _wCtrlCode, WORD _wId, CWidget * _pwSrc );

		/**
		 * Handles WM_COMMAND without a source control.  The dialog manager sends IDOK (Enter) and IDCANCEL (Escape) this way.
		 *
		 * \param _wId The command identifier.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED												MenuCommand( WORD _wId );

		/**
		 * Gets the text for the "Listening" state.
		 *
		 * \return Returns the text to be used during the listening state.
		 */
		virtual const wchar_t *											ListeningText() const;

		/**
		 * Called after focus on the control is lost or a key has been registered.
		 *
		 * \param _pwControl The control used for listening.
		 * \param _bSuccess Set to true if the call is due to a key being registered.
		 * \return Returns true if the listening state is LSW_LS_LISTENING and the old control procedure was restored.
		 */
		virtual bool													StopListening_Keyboard( CWidget * _pwControl, bool _bSuccess );


	protected :
		// == Members.
		/** The cheat being edited and the returned cheat. */
		CCheatEditWindowLayout::LSN_CHEAT_EDIT_DATA *					m_pcedData;
		/** The Toggle (or Enable) hotkey being edited. */
		lsw::LSW_KEY													m_kKey1 = {};
		/** The Disable hotkey being edited. */
		lsw::LSW_KEY													m_kKey2 = {};
		/** True when the hotkeys are in Toggle mode, false for Enable/Disable mode. */
		bool															m_bToggleMode = true;
		/** The hotkey button that is listening for a key (1 or 2), or 0. */
		int																m_iListening = 0;
		/** Set while one code edit is being updated from the other, so that the update does not echo back. */
		bool															m_bSyncingCodes = false;


		// == Functions.
		/**
		 * Updates the other code edit after one of them changes.  Each line of one edit corresponds to the same line of the other.  A code that
		 *	cannot be a Game Genie code (an address below $8000) has a blank line on the Game Genie side, and editing the Game Genie side keeps
		 *	such codes.
		 *
		 * \param _bFromGameGenie If true, the Game Genie edit changed and the AAAA:VV:CC edit is updated, otherwise the reverse.
		 */
		void															SyncCodes( bool _bFromGameGenie );

		/**
		 * Updates the hotkey labels and buttons for the current mode and keys.
		 */
		void															UpdateHotkeyControls();

		/**
		 * Validates the dialog and stores the cheat into m_pcedData.  Shows an error on failure.
		 *
		 * \return Returns true if the dialog was valid and the cheat was stored.
		 */
		bool															Apply();

		/**
		 * Gets the text of a hotkey for its button.
		 *
		 * \param _kKey The hotkey.
		 * \return Returns the text of the hotkey, or the "(None)" text if no key is assigned.
		 */
		static std::wstring												KeyText( const lsw::LSW_KEY &_kKey );

		/**
		 * Determines if two hotkeys are the same key and modifier.
		 *
		 * \param _kLeft The left operand.
		 * \param _kRight The right operand.
		 * \return Returns true if both keys have a key code and the key codes and modifiers match.
		 */
		static bool														SameKey( const lsw::LSW_KEY &_kLeft, const lsw::LSW_KEY &_kRight );

		/**
		 * Splits the text of a multi-line edit into lines.
		 *
		 * \param _wsText The text to split.
		 * \return Returns the lines (without line breaks).
		 */
		static std::vector<std::wstring>								SplitLines( const std::wstring &_wsText );

	private :
		typedef CWinUtilities											Layout;
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

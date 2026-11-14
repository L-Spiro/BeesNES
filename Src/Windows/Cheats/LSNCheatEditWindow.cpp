#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The Add/Edit/Duplicate Cheat dialog.  Shows a cheat's title, description, and codes (side-by-side as Game Genie codes and as
 *	AAAA:VV:CC codes, each updating the other), whether it is enabled, and its hotkeys.
 */

#include "LSNCheatEditWindow.h"
#include "../../Cheats/LSNCheatConverter.h"
#include "../../Localization/LSNLocalization.h"

#include <Helpers/LSWHelpers.h>

#include <format>


namespace lsn {

	/**
	 * Converts an ASCII wide string (codes) to a narrow string.  Other characters become '?' so that they fail to parse.
	 *
	 * \param _wsText The text to convert.
	 * \return Returns the narrow string.
	 */
	static std::string									CheatEditNarrow( const std::wstring &_wsText ) {
		std::string sRet;
		sRet.reserve( _wsText.size() );
		for ( auto wcThis : _wsText ) { sRet.push_back( (wcThis < 0x80) ? char( wcThis ) : '?' ); }
		return sRet;
	}

	/**
	 * Converts an ASCII narrow string (codes) to a wide string.
	 *
	 * \param _sText The text to convert.
	 * \return Returns the wide string.
	 */
	static std::wstring									CheatEditWiden( const std::string &_sText ) {
		std::wstring wsRet;
		wsRet.reserve( _sText.size() );
		for ( auto cThis : _sText ) { wsRet.push_back( wchar_t( uint8_t( cThis ) ) ); }
		return wsRet;
	}

	/**
	 * Determines whether a string is empty or only whitespace.
	 *
	 * \param _wsText The text to check.
	 * \return Returns true if the text has no non-whitespace characters.
	 */
	static bool											CheatEditIsBlank( const std::wstring &_wsText ) {
		return _wsText.find_first_not_of( L" \t\r\n" ) == std::wstring::npos;
	}

	CCheatEditWindow::CCheatEditWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pcedData( reinterpret_cast<CCheatEditWindowLayout::LSN_CHEAT_EDIT_DATA *>(_ui64Data) ) {
	}

	// == Functions.
	/**
	 * The WM_INITDIALOG handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CCheatEditWindow::InitDialog() {
		Parent::InitDialog();
		if ( !m_pcedData ) { return LSW_H_CONTINUE; }
		const auto & ceEntry = m_pcedData->ceEntry;
		const auto & acActive = m_pcedData->acActive;

		if ( m_pcedData->pwcTitle ) { SetTextW( m_pcedData->pwcTitle ); }

		try {
			auto pwThis = FindChild( Layout::LSN_CEWI_TITLE_EDIT );
			if ( pwThis ) { pwThis->SetTextW( ceEntry.wsDescription.c_str() ); }

			// The description is shown like the notes in the Cheats dialog.
			pwThis = FindChild( Layout::LSN_CEWI_DESC_EDIT );
			if ( pwThis ) {
				std::wstring wsNotes = CUtilities::Replace( CUtilities::Replace( ceEntry.wsNotes, std::wstring( L"\r\n" ), std::wstring( L"\n" ) ), std::wstring( L"\n" ), std::wstring( L"\r\n" ) );
				pwThis->SetTextW( wsNotes.c_str() );
			}

			// The codes, side by side.  The edits get the same fixed-width font as the codes in the Cheats dialog.
			std::wstring wsGg, wsRaw;
			for ( size_t I = 0; I < ceEntry.vAddresses.size(); ++I ) {
				if ( I ) {
					wsGg += L"\r\n";
					wsRaw += L"\r\n";
				}
				const auto & aThis = ceEntry.vAddresses[I];
				wsRaw += CheatEditWiden( CCheatConverter::FormatRawCode( aThis ) );
				if ( ceEntry.ctType == LSN_CHEAT_ENTRY::LSN_CT_GAME_GENIE && aThis.sCode.size() ) {
					wsGg += CheatEditWiden( aThis.sCode );
				}
				else {
					wsGg += CheatEditWiden( CCheatConverter::FormatGameGenie( aThis ) );
				}
			}
			m_bSyncingCodes = true;
			pwThis = FindChild( Layout::LSN_CEWI_GG_EDIT );
			if ( pwThis ) {
				pwThis->SetSystemFixedFont();
				pwThis->SetTextW( wsGg.c_str() );
			}
			pwThis = FindChild( Layout::LSN_CEWI_RAW_EDIT );
			if ( pwThis ) {
				pwThis->SetSystemFixedFont();
				pwThis->SetTextW( wsRaw.c_str() );
			}
			m_bSyncingCodes = false;
		}
		catch ( ... ) { m_bSyncingCodes = false; }

		auto pwEnabled = FindChild( Layout::LSN_CEWI_ENABLED_CHECK );
		if ( pwEnabled ) { pwEnabled->SetCheck( acActive.bEnabled ); }

		m_bToggleMode = acActive.IsToggle();
		m_kKey1 = acActive.kToggleOrEnable;
		m_kKey2 = m_bToggleMode ? lsw::LSW_KEY() : acActive.kDisable;
		UpdateHotkeyControls();

		auto pwTitle = FindChild( Layout::LSN_CEWI_TITLE_EDIT );
		if ( pwTitle ) {
			::SetFocus( pwTitle->Wnd() );
			::SendMessageW( pwTitle->Wnd(), EM_SETSEL, 0, -1 );
		}
		return LSW_H_HANDLED;
	}

	/**
	 * The WM_CLOSE handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CCheatEditWindow::Close() {
		::EndDialog( Wnd(), 0 );
		return LSW_H_HANDLED;
	}

	/**
	 * Handles the WM_COMMAND message.
	 *
	 * \param _wCtrlCode 0 = from menu, 1 = from accelerator, otherwise it is a Control-defined notification code.
	 * \param _wId The ID of the control if _wCtrlCode is not 0 or 1.
	 * \param _pwSrc The source control if _wCtrlCode is not 0 or 1.
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CCheatEditWindow::Command( WORD _wCtrlCode, WORD _wId, CWidget * /*_pwSrc*/ ) {
		switch ( _wId ) {
			case Layout::LSN_CEWI_GG_EDIT : {
				if ( _wCtrlCode == EN_CHANGE ) { SyncCodes( true ); }
				break;
			}
			case Layout::LSN_CEWI_RAW_EDIT : {
				if ( _wCtrlCode == EN_CHANGE ) { SyncCodes( false ); }
				break;
			}
			case Layout::LSN_CEWI_TOGGLE_RADIO : {
				if ( _wCtrlCode == BN_CLICKED && !m_bToggleMode ) {
					m_bToggleMode = true;
					// One key now; the Disable key would otherwise be kept invisibly.
					m_kKey2 = lsw::LSW_KEY();
					UpdateHotkeyControls();
				}
				break;
			}
			case Layout::LSN_CEWI_ENABLE_DISABLE_RADIO : {
				if ( _wCtrlCode == BN_CLICKED && m_bToggleMode ) {
					m_bToggleMode = false;
					UpdateHotkeyControls();
				}
				break;
			}
			case Layout::LSN_CEWI_KEY1_BUTTON : {}							LSN_FALLTHROUGH
			case Layout::LSN_CEWI_KEY2_BUTTON : {
				if ( _wCtrlCode == BN_CLICKED && m_iListening == 0 ) {
					auto pwButton = FindChild( _wId );
					if ( pwButton ) {
						m_iListening = (_wId == Layout::LSN_CEWI_KEY1_BUTTON) ? 1 : 2;
						BeginListening_Keyboard( pwButton );
					}
				}
				break;
			}
			case Layout::LSN_CEWI_CLEAR_BUTTON : {
				if ( _wCtrlCode == BN_CLICKED ) {
					m_kKey1 = m_kKey2 = lsw::LSW_KEY();
					UpdateHotkeyControls();
				}
				break;
			}
			case IDOK : {}													LSN_FALLTHROUGH
			case Layout::LSN_CEWI_OK : {
				if ( Apply() ) {
					::EndDialog( Wnd(), 1 );
				}
				return LSW_H_HANDLED;
			}
			case IDCANCEL : {}												LSN_FALLTHROUGH
			case Layout::LSN_CEWI_CANCEL : {
				return Close();
			}
			default : {}
		}
		return LSW_H_CONTINUE;
	}

	/**
	 * Handles WM_COMMAND without a source control.  The dialog manager sends IDOK (Enter) and IDCANCEL (Escape) this way.
	 *
	 * \param _wId The command identifier.
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CCheatEditWindow::MenuCommand( WORD _wId ) {
		switch ( _wId ) {
			case IDOK : {}													LSN_FALLTHROUGH
			case IDCANCEL : {
				return Command( BN_CLICKED, _wId, nullptr );
			}
			default : {}
		}
		return Parent::MenuCommand( _wId );
	}

	/**
	 * Gets the text for the "Listening" state.
	 *
	 * \return Returns the text to be used during the listening state.
	 */
	const wchar_t * CCheatEditWindow::ListeningText() const {
		return LSN_LSTR( LSN_STR_PRESS_A_KEY );
	}

	/**
	 * Called after focus on the control is lost or a key has been registered.
	 *
	 * \param _pwControl The control used for listening.
	 * \param _bSuccess Set to true if the call is due to a key being registered.
	 * \return Returns true if the listening state is LSW_LS_LISTENING and the old control procedure was restored.
	 */
	bool CCheatEditWindow::StopListening_Keyboard( CWidget * _pwControl, bool _bSuccess ) {
		if ( !lsw::CInputListenerBase::StopListening_Keyboard( _pwControl, _bSuccess ) ) { return false; }
		int iListening = m_iListening;
		m_iListening = 0;
		if ( _bSuccess && m_kResult.bKeyCode ) {
			lsw::LSW_KEY & kTarget = (iListening == 2) ? m_kKey2 : m_kKey1;
			const lsw::LSW_KEY & kOther = (iListening == 2) ? m_kKey1 : m_kKey2;
			// The same key for both would really be a Toggle key.
			if ( !m_bToggleMode && SameKey( m_kResult, kOther ) ) {
				UpdateHotkeyControls();
				lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_STR_CHEAT_ERR_SAME_KEY ), LSN_LSTR( LSN_ERROR ) );
				return true;
			}
			kTarget = m_kResult;
		}
		// Losing focus without a key keeps the previous key.
		UpdateHotkeyControls();
		return true;
	}

	/**
	 * Updates the other code edit after one of them changes.  Each line of one edit corresponds to the same line of the other.  A code that
	 *	cannot be a Game Genie code (an address below $8000) has a blank line on the Game Genie side, and editing the Game Genie side keeps
	 *	such codes.
	 *
	 * \param _bFromGameGenie If true, the Game Genie edit changed and the AAAA:VV:CC edit is updated, otherwise the reverse.
	 */
	void CCheatEditWindow::SyncCodes( bool _bFromGameGenie ) {
		if ( m_bSyncingCodes ) { return; }
		auto pwSrc = FindChild( _bFromGameGenie ? Layout::LSN_CEWI_GG_EDIT : Layout::LSN_CEWI_RAW_EDIT );
		auto pwDst = FindChild( _bFromGameGenie ? Layout::LSN_CEWI_RAW_EDIT : Layout::LSN_CEWI_GG_EDIT );
		if ( !pwSrc || !pwDst ) { return; }
		try {
			std::vector<std::wstring> vSrc = SplitLines( pwSrc->GetTextW() );
			std::vector<std::wstring> vDst = SplitLines( pwDst->GetTextW() );
			// Lines past the end of the Game Genie side can still hold codes that have no Game Genie form.
			size_t stLines = _bFromGameGenie ? std::max( vSrc.size(), vDst.size() ) : vSrc.size();
			std::vector<std::wstring> vOut;
			vOut.resize( stLines );
			for ( size_t I = 0; I < stLines; ++I ) {
				LSN_CHEAT_ENTRY::LSN_ADDRESS aAddr;
				bool bGameGenie;
				if ( I < vSrc.size() && CCheatConverter::ParseAnyCode( CheatEditNarrow( vSrc[I] ), aAddr, bGameGenie ) ) {
					vOut[I] = CheatEditWiden( _bFromGameGenie ? CCheatConverter::FormatRawCode( aAddr ) : CCheatConverter::FormatGameGenie( aAddr ) );
				}
				else if ( _bFromGameGenie && (I >= vSrc.size() || CheatEditIsBlank( vSrc[I] )) && I < vDst.size() ) {
					// A blank Game Genie line can stand for a code that has no Game Genie form.  Keep it.
					LSN_CHEAT_ENTRY::LSN_ADDRESS aOld;
					bool bOldGameGenie;
					if ( CCheatConverter::ParseAnyCode( CheatEditNarrow( vDst[I] ), aOld, bOldGameGenie ) && CCheatConverter::FormatGameGenie( aOld ).empty() ) {
						vOut[I] = vDst[I];
					}
				}
			}
			// Drop trailing blank lines that only exist because of the line matching.
			while ( vOut.size() > vSrc.size() && vOut.size() && CheatEditIsBlank( vOut.back() ) ) { vOut.pop_back(); }

			std::wstring wsOut;
			for ( size_t I = 0; I < vOut.size(); ++I ) {
				if ( I ) { wsOut += L"\r\n"; }
				wsOut += vOut[I];
			}
			if ( wsOut != pwDst->GetTextW() ) {
				m_bSyncingCodes = true;
				pwDst->SetTextW( wsOut.c_str() );
				m_bSyncingCodes = false;
			}
		}
		catch ( ... ) { m_bSyncingCodes = false; }
	}

	/**
	 * Updates the hotkey labels and buttons for the current mode and keys.
	 */
	void CCheatEditWindow::UpdateHotkeyControls() {
		auto pwThis = FindChild( Layout::LSN_CEWI_TOGGLE_RADIO );
		if ( pwThis ) { pwThis->SetCheck( m_bToggleMode ); }
		pwThis = FindChild( Layout::LSN_CEWI_ENABLE_DISABLE_RADIO );
		if ( pwThis ) { pwThis->SetCheck( !m_bToggleMode ); }

		pwThis = FindChild( Layout::LSN_CEWI_KEY1_LABEL );
		if ( pwThis ) { pwThis->SetTextW( m_bToggleMode ? LSN_LSTR( LSN_STR_TOGGLE_KEY_ ) : LSN_LSTR( LSN_STR_ENABLE_KEY_ ) ); }
		try {
			pwThis = FindChild( Layout::LSN_CEWI_KEY1_BUTTON );
			if ( pwThis && m_iListening != 1 ) { pwThis->SetTextW( KeyText( m_kKey1 ).c_str() ); }
			pwThis = FindChild( Layout::LSN_CEWI_KEY2_BUTTON );
			if ( pwThis ) {
				if ( m_iListening != 2 ) { pwThis->SetTextW( KeyText( m_kKey2 ).c_str() ); }
				pwThis->SetVisible( !m_bToggleMode );
			}
		}
		catch ( ... ) {}
		pwThis = FindChild( Layout::LSN_CEWI_KEY2_LABEL );
		if ( pwThis ) { pwThis->SetVisible( !m_bToggleMode ); }
		pwThis = FindChild( Layout::LSN_CEWI_CLEAR_BUTTON );
		if ( pwThis ) { pwThis->SetEnabled( m_kKey1.bKeyCode != 0 || m_kKey2.bKeyCode != 0 ); }
	}

	/**
	 * Validates the dialog and stores the cheat into m_pcedData.  Shows an error on failure.
	 *
	 * \return Returns true if the dialog was valid and the cheat was stored.
	 */
	bool CCheatEditWindow::Apply() {
		if ( !m_pcedData ) { return false; }
		auto pwTitle = FindChild( Layout::LSN_CEWI_TITLE_EDIT );
		auto pwDesc = FindChild( Layout::LSN_CEWI_DESC_EDIT );
		auto pwGg = FindChild( Layout::LSN_CEWI_GG_EDIT );
		auto pwRaw = FindChild( Layout::LSN_CEWI_RAW_EDIT );
		auto pwEnabled = FindChild( Layout::LSN_CEWI_ENABLED_CHECK );
		if ( !pwTitle || !pwDesc || !pwGg || !pwRaw || !pwEnabled ) { return false; }

		auto aError = [&]( const std::wstring &_wsError, CWidget * _pwFocus ) {
			lsw::CBase::MessageBoxError( Wnd(), _wsError.c_str(), LSN_LSTR( LSN_ERROR ) );
			::SetFocus( _pwFocus->Wnd() );
			return false;
		};

		try {
			std::wstring wsTitle = pwTitle->GetTextW();
			{
				size_t stStart = wsTitle.find_first_not_of( L" \t\r\n" );
				if ( stStart == std::wstring::npos ) { return aError( LSN_LSTR( LSN_STR_CHEAT_ERR_NO_TITLE ), pwTitle ); }
				size_t stEnd = wsTitle.find_last_not_of( L" \t\r\n" );
				wsTitle = wsTitle.substr( stStart, stEnd - stStart + 1 );
			}

			// The AAAA:VV:CC side is the authority; the Game Genie side only decides whether the codes are kept as Game Genie codes.
			std::vector<std::wstring> vRaw = SplitLines( pwRaw->GetTextW() );
			std::vector<std::wstring> vGg = SplitLines( pwGg->GetTextW() );
			std::vector<LSN_CHEAT_ENTRY::LSN_ADDRESS> vCodes;
			bool bAllGameGenie = true;
			for ( size_t I = 0; I < vRaw.size(); ++I ) {
				if ( CheatEditIsBlank( vRaw[I] ) ) { continue; }
				LSN_CHEAT_ENTRY::LSN_ADDRESS aAddr;
				bool bGameGenie;
				if ( !CCheatConverter::ParseAnyCode( CheatEditNarrow( vRaw[I] ), aAddr, bGameGenie ) ) {
					size_t stLine = I + 1;
					return aError( std::vformat( std::wstring( LSN_LSTR( LSN_STR_CHEAT_ERR_BAD_CODE ) ), std::make_wformat_args( stLine, vRaw[I] ) ), pwRaw );
				}
				LSN_CHEAT_ENTRY::LSN_ADDRESS aGg;
				bool bGgIsGameGenie = false;
				if ( I < vGg.size() && CCheatConverter::ParseAnyCode( CheatEditNarrow( vGg[I] ), aGg, bGgIsGameGenie ) && bGgIsGameGenie &&
					aGg.ui16Address == aAddr.ui16Address && aGg.ui8Value == aAddr.ui8Value && aGg.bUseCompare == aAddr.bUseCompare &&
					(!aAddr.bUseCompare || aGg.ui8CompareValue == aAddr.ui8CompareValue) ) {
					aAddr.sCode = aGg.sCode;
				}
				else {
					bAllGameGenie = false;
				}
				vCodes.push_back( aAddr );
			}
			if ( vCodes.empty() ) { return aError( LSN_LSTR( LSN_STR_CHEAT_ERR_NO_CODES ), pwRaw ); }
			if ( !bAllGameGenie ) {
				for ( auto & aThis : vCodes ) { aThis.sCode = CCheatConverter::FormatRawCode( aThis ); }
			}
			if ( !m_bToggleMode && SameKey( m_kKey1, m_kKey2 ) ) {
				return aError( LSN_LSTR( LSN_STR_CHEAT_ERR_SAME_KEY ), FindChild( Layout::LSN_CEWI_KEY2_BUTTON ) );
			}

			auto & ceEntry = m_pcedData->ceEntry;
			ceEntry.wsDescription = wsTitle;
			ceEntry.wsNotes = pwDesc->GetTextW();
			ceEntry.vAddresses = std::move( vCodes );
			ceEntry.ctType = bAllGameGenie ? LSN_CHEAT_ENTRY::LSN_CT_GAME_GENIE : LSN_CHEAT_ENTRY::LSN_CT_CUSTOM;
			ceEntry.bEnabled = pwEnabled->IsChecked() != FALSE;

			auto & acActive = m_pcedData->acActive;
			acActive.bEnabled = ceEntry.bEnabled;
			acActive.bToggle = m_bToggleMode;
			acActive.kToggleOrEnable = m_kKey1;
			acActive.kDisable = m_bToggleMode ? lsw::LSW_KEY() : m_kKey2;
			return true;
		}
		catch ( ... ) {
			lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_OUT_OF_MEMORY ), LSN_LSTR( LSN_ERROR ) );
			return false;
		}
	}

	/**
	 * Gets the text of a hotkey for its button.
	 *
	 * \param _kKey The hotkey.
	 * \return Returns the text of the hotkey, or the "(None)" text if no key is assigned.
	 */
	std::wstring CCheatEditWindow::KeyText( const lsw::LSW_KEY &_kKey ) {
		if ( !_kKey.bKeyCode ) { return LSN_LSTR( LSN_STR_NO_HOTKEY ); }
		return lsw::CHelpers::ToString( _kKey, false );
	}

	/**
	 * Determines if two hotkeys are the same key and modifier.
	 *
	 * \param _kLeft The left operand.
	 * \param _kRight The right operand.
	 * \return Returns true if both keys have a key code and the key codes and modifiers match.
	 */
	bool CCheatEditWindow::SameKey( const lsw::LSW_KEY &_kLeft, const lsw::LSW_KEY &_kRight ) {
		return _kLeft.bKeyCode != 0 && _kLeft.bKeyCode == _kRight.bKeyCode && _kLeft.bKeyModifier == _kRight.bKeyModifier;
	}

	/**
	 * Splits the text of a multi-line edit into lines.
	 *
	 * \param _wsText The text to split.
	 * \return Returns the lines (without line breaks).
	 */
	std::vector<std::wstring> CCheatEditWindow::SplitLines( const std::wstring &_wsText ) {
		std::vector<std::wstring> vRet;
		std::wstring wsLine;
		for ( size_t I = 0; I < _wsText.size(); ++I ) {
			if ( _wsText[I] == L'\r' ) { continue; }
			if ( _wsText[I] == L'\n' ) {
				vRet.push_back( wsLine );
				wsLine.clear();
				continue;
			}
			wsLine.push_back( _wsText[I] );
		}
		if ( wsLine.size() || (_wsText.size() && _wsText.back() == L'\n') ) { vRet.push_back( wsLine ); }
		return vRet;
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

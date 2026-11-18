#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The Turbo Button Configuration dialog.  Creates, edits, and deletes turbo-pattern presets:  up to 64 on/off states (typed as
 *	1's and 0's or toggled with check boxes) and how they are played (repeating or once, frames per state, restarting on each press,
 *	holding the button after a one-shot pattern ends, and whether the global speed multiplier applies).
 */

#include "LSNTurboConfigWindow.h"
#include "../../Localization/LSNLocalization.h"
#include "../../Utilities/LSNUtilities.h"

#include <Base/LSWBase.h>
#include <ListBox/LSWListBox.h>

#include <algorithm>
#include <format>


namespace lsn {

	/**
	 * Converts a wide string to a UTF-16 string.
	 *
	 * \param _wsText The text to convert.
	 * \return Returns the converted text.
	 */
	static std::u16string										TurboToU16( const std::wstring &_wsText ) {
		return CUtilities::XStringToU16String( _wsText.c_str(), _wsText.size() );
	}

	/**
	 * Converts a UTF-16 string to a wide string.
	 *
	 * \param _u16Text The text to convert.
	 * \return Returns the converted text.
	 */
	static std::wstring											TurboToW( const std::u16string &_u16Text ) {
		return CUtilities::XStringToWString( _u16Text.c_str(), _u16Text.size() );
	}

	CTurboConfigWindow::CTurboConfigWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_ptcdData( reinterpret_cast<CTurboConfigWindowLayout::LSN_TURBO_CONFIG_DATA *>(_ui64Data) ) {
	}

	// == Functions.
	/**
	 * The WM_INITDIALOG handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CTurboConfigWindow::InitDialog() {
		Parent::InitDialog();
		if ( !m_ptcdData || !m_ptcdData->ptpPatterns ) { return LSW_H_CONTINUE; }

		auto pwThis = FindChild( Layout::LSN_TCWI_STATES_EDIT );
		if ( pwThis ) {
			pwThis->SetSystemFixedFont();
			// 64 states plus room for spaces between groups.
			::SendMessageW( pwThis->Wnd(), EM_LIMITTEXT, 128, 0 );
		}
		pwThis = FindChild( Layout::LSN_TCWI_NAME_EDIT );
		if ( pwThis ) { ::SendMessageW( pwThis->Wnd(), EM_LIMITTEXT, LSN_TURBO_NAME_MAX, 0 ); }
		pwThis = FindChild( Layout::LSN_TCWI_LENGTH_EDIT );
		if ( pwThis ) { ::SendMessageW( pwThis->Wnd(), EM_LIMITTEXT, 2, 0 ); }
		pwThis = FindChild( Layout::LSN_TCWI_FPS_EDIT );
		if ( pwThis ) { ::SendMessageW( pwThis->Wnd(), EM_LIMITTEXT, 3, 0 ); }
		AlignStateChecks();

		const LSN_TURBO_PATTERN * ptpFirst = m_ptcdData->ptpPatterns->Find( m_ptcdData->u16Select );
		if ( !ptpFirst && m_ptcdData->ptpPatterns->Patterns().size() ) { ptpFirst = &m_ptcdData->ptpPatterns->Patterns()[0]; }
		if ( ptpFirst ) {
			const std::u16string u16Name = ptpFirst->u16Name;
			FillList( u16Name );
			ShowPattern( u16Name, TurboToW( u16Name ), ptpFirst->tpPlayback );
		}
		else {
			FillList( std::u16string() );
			NewPattern( false );
		}
		return LSW_H_CONTINUE;
	}

	/**
	 * Places the 64 state check boxes and their index labels on whole pixels.  Dialog units do not map to a whole number of pixels, so
	 *	the layout's positions and widths round differently from column to column and some check boxes overlapped their neighbors.  Each
	 *	column is given the same whole-pixel pitch, each check box is made as wide as the check mark and centered in its column, and each
	 *	index label spans its column.
	 */
	void CTurboConfigWindow::AlignStateChecks() {
		// Gets a child's rectangle in the client coordinates of this dialog.
		auto aClientRect = [this]( WORD _wId, RECT &_rRet ) {
			CWidget * pwCtrl = FindChild( _wId );
			if ( !pwCtrl || !::GetWindowRect( pwCtrl->Wnd(), &_rRet ) ) { return false; }
			::MapWindowPoints( NULL, Wnd(), reinterpret_cast<POINT *>(&_rRet), 2 );
			return true;
		};
		RECT rFirst, rLast;
		if ( !aClientRect( Layout::LSN_TCWI_CHECK_FIRST, rFirst ) || !aClientRect( Layout::LSN_TCWI_CHECK_FIRST + 31, rLast ) ) { return; }

		// The span the layout gave the 32 columns, divided into equal whole-pixel columns and centered in that span.
		const LONG lSpan = rLast.right - rFirst.left;
		const LONG lPitch = lSpan / 32;
		if ( lPitch <= 0 ) { return; }
		const LONG lStart = rFirst.left + (lSpan - lPitch * 32) / 2;
		// The check box is as wide as its check mark (never wider than its column less a 1-pixel gap).
		const LONG lCheckW = std::clamp<LONG>( ::GetSystemMetrics( SM_CXMENUCHECK ), 1, std::max<LONG>( lPitch - 1, 1 ) );
		const LONG lCheckOff = (lPitch - lCheckW) / 2;

		for ( uint32_t I = 0; I < 64; ++I ) {
			const LONG lLeft = lStart + LONG( I % 32 ) * lPitch;
			RECT rThis;
			if ( aClientRect( WORD( Layout::LSN_TCWI_CHECK_FIRST + I ), rThis ) ) {
				::MoveWindow( FindChild( WORD( Layout::LSN_TCWI_CHECK_FIRST + I ) )->Wnd(), lLeft + lCheckOff, rThis.top, lCheckW, rThis.bottom - rThis.top, FALSE );
			}
			if ( aClientRect( WORD( Layout::LSN_TCWI_INDEX_FIRST + I ), rThis ) ) {
				::MoveWindow( FindChild( WORD( Layout::LSN_TCWI_INDEX_FIRST + I ) )->Wnd(), lLeft, rThis.top, lPitch, rThis.bottom - rThis.top, FALSE );
			}
		}
		::InvalidateRect( Wnd(), NULL, TRUE );
	}

	/**
	 * The WM_CLOSE handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CTurboConfigWindow::Close() {
		if ( !ResolveUnsaved() ) { return LSW_H_HANDLED; }
		::EndDialog( Wnd(), 1 );
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
	CWidget::LSW_HANDLED CTurboConfigWindow::Command( WORD _wCtrlCode, WORD _wId, CWidget * /*_pwSrc*/ ) {
		if ( _wId >= Layout::LSN_TCWI_CHECK_FIRST && _wId <= Layout::LSN_TCWI_CHECK_LAST ) {
			if ( _wCtrlCode == BN_CLICKED && !m_bUpdating ) {
				const uint32_t ui32Idx = uint32_t( _wId - Layout::LSN_TCWI_CHECK_FIRST );
				auto pwCheck = FindChild( _wId );
				if ( pwCheck ) { m_tpCurrent.SetState( ui32Idx, pwCheck->IsChecked() ); }
				UpdateStates( true, false );
				UpdateControls();
			}
			return LSW_H_HANDLED;
		}
		switch ( _wId ) {
			case Layout::LSN_TCWI_PRESETS_LIST : {
				if ( _wCtrlCode == LBN_SELCHANGE && !m_bUpdating ) { ListSelectionChanged(); }
				break;
			}
			case Layout::LSN_TCWI_NEW_BUTTON : {
				if ( _wCtrlCode == BN_CLICKED && ResolveUnsaved() ) { NewPattern( true ); }
				break;
			}
			case Layout::LSN_TCWI_DUPLICATE_BUTTON : {
				if ( _wCtrlCode == BN_CLICKED && ResolveUnsaved() ) { DuplicatePattern(); }
				break;
			}
			case Layout::LSN_TCWI_DELETE_BUTTON : {
				if ( _wCtrlCode == BN_CLICKED ) { DeletePattern(); }
				break;
			}
			case Layout::LSN_TCWI_STATES_EDIT : {
				if ( _wCtrlCode == EN_CHANGE && !m_bUpdating ) {
					// Typing sets the states and the length (when the text is valid).
					auto pwEdit = FindChild( _wId );
					LSN_TURBO_PLAYBACK tpTmp = m_tpCurrent;
					if ( pwEdit && CTurboPatterns::FromString( TurboToU16( pwEdit->GetTextW() ), tpTmp ) ) {
						m_tpCurrent.ui64Bits = tpTmp.ui64Bits;
						m_tpCurrent.ui8Length = tpTmp.ui8Length;
						UpdateStates( false, true );
					}
					UpdateControls();
				}
				break;
			}
			case Layout::LSN_TCWI_LENGTH_EDIT : {
				if ( _wCtrlCode == EN_CHANGE && !m_bUpdating ) {
					uint32_t ui32Len;
					if ( ReadNumber( _wId, 1, 64, ui32Len ) ) {
						m_tpCurrent.ui8Length = uint8_t( ui32Len );
						UpdateStates( true, false );
					}
					UpdateControls();
				}
				break;
			}
			case Layout::LSN_TCWI_FPS_EDIT : {}						LSN_FALLTHROUGH
			case Layout::LSN_TCWI_NAME_EDIT : {
				if ( _wCtrlCode == EN_CHANGE && !m_bUpdating ) { UpdateControls(); }
				break;
			}
			case Layout::LSN_TCWI_LOOP_RADIO : {}					LSN_FALLTHROUGH
			case Layout::LSN_TCWI_ONCE_RADIO : {}					LSN_FALLTHROUGH
			case Layout::LSN_TCWI_RESTART_CHECK : {}				LSN_FALLTHROUGH
			case Layout::LSN_TCWI_HOLD_CHECK : {}					LSN_FALLTHROUGH
			case Layout::LSN_TCWI_GLOBAL_CHECK : {
				if ( _wCtrlCode == BN_CLICKED && !m_bUpdating ) { UpdateControls(); }
				break;
			}
			case IDOK : {}											LSN_FALLTHROUGH
			case Layout::LSN_TCWI_SAVE_BUTTON : {
				SavePattern();
				return LSW_H_HANDLED;
			}
			case IDCANCEL : {}										LSN_FALLTHROUGH
			case Layout::LSN_TCWI_CLOSE_BUTTON : {
				return Close();
			}
		}
		return LSW_H_CONTINUE;
	}

	/**
	 * Handles WM_COMMAND without a source control.  The dialog manager sends IDOK (Enter) and IDCANCEL (Escape) this way.
	 *
	 * \param _wId The command identifier.
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CTurboConfigWindow::MenuCommand( WORD _wId ) {
		switch ( _wId ) {
			case IDOK : {}											LSN_FALLTHROUGH
			case IDCANCEL : {
				return Command( BN_CLICKED, _wId, nullptr );
			}
			default : {}
		}
		return Parent::MenuCommand( _wId );
	}

	/**
	 * Fills the list of presets.
	 *
	 * \param _u16Select The preset to select, or an empty string to select nothing.
	 */
	void CTurboConfigWindow::FillList( const std::u16string &_u16Select ) {
		auto plbList = static_cast<lsw::CListBox *>(FindChild( Layout::LSN_TCWI_PRESETS_LIST ));
		if ( !plbList || !m_ptcdData || !m_ptcdData->ptpPatterns ) { return; }
		m_bUpdating = true;
		try {
			plbList->ResetContent();
			const auto & vPatterns = m_ptcdData->ptpPatterns->Patterns();
			const LSN_TURBO_PATTERN * ptpSelect = m_ptcdData->ptpPatterns->Find( _u16Select );
			for ( size_t I = 0; I < vPatterns.size(); ++I ) {
				INT iIdx = plbList->AddString( TurboToW( vPatterns[I].u16Name ).c_str() );
				if ( iIdx >= 0 ) {
					plbList->SetItemData( iIdx, LPARAM( I ) );
					if ( ptpSelect == &vPatterns[I] ) { plbList->SetCurSel( iIdx ); }
				}
			}
			if ( !ptpSelect ) { plbList->SetCurSel( -1 ); }
		}
		catch ( ... ) {}
		m_bUpdating = false;
	}

	/**
	 * Shows a pattern in the controls and makes it the baseline for detecting unsaved changes.
	 *
	 * \param _u16Editing The name of the preset being edited, or an empty string for a new pattern.
	 * \param _wsName The name to show.
	 * \param _tpPlayback The pattern to show.
	 */
	void CTurboConfigWindow::ShowPattern( const std::u16string &_u16Editing, const std::wstring &_wsName, const LSN_TURBO_PLAYBACK &_tpPlayback ) {
		m_u16Editing = _u16Editing;
		m_tpCurrent = _tpPlayback;
		m_bUpdating = true;
		auto pwThis = FindChild( Layout::LSN_TCWI_NAME_EDIT );
		if ( pwThis ) { pwThis->SetTextW( _wsName.c_str() ); }
		pwThis = FindChild( Layout::LSN_TCWI_FPS_EDIT );
		if ( pwThis ) { pwThis->SetTextW( std::to_wstring( std::max<uint32_t>( _tpPlayback.ui8FramesPerState, 1 ) ).c_str() ); }
		pwThis = FindChild( Layout::LSN_TCWI_LOOP_RADIO );
		if ( pwThis ) { pwThis->SetCheck( _tpPlayback.bLoop ); }
		pwThis = FindChild( Layout::LSN_TCWI_ONCE_RADIO );
		if ( pwThis ) { pwThis->SetCheck( !_tpPlayback.bLoop ); }
		pwThis = FindChild( Layout::LSN_TCWI_RESTART_CHECK );
		if ( pwThis ) { pwThis->SetCheck( _tpPlayback.bRestartOnPress ); }
		pwThis = FindChild( Layout::LSN_TCWI_HOLD_CHECK );
		if ( pwThis ) { pwThis->SetCheck( _tpPlayback.bHoldAfterEnd ); }
		pwThis = FindChild( Layout::LSN_TCWI_GLOBAL_CHECK );
		if ( pwThis ) { pwThis->SetCheck( _tpPlayback.bUseGlobalSpeed ); }
		m_bUpdating = false;
		UpdateStates( true, true );

		// What is shown now is what was loaded.
		LSN_TURBO_PATTERN tpRead;
		m_wsBaselineName = _wsName;
		m_tpBaseline = ReadPattern( tpRead, false ) ? tpRead.tpPlayback : _tpPlayback;
		UpdateControls();
	}

	/**
	 * Reads the pattern from the controls.
	 *
	 * \param _tpPattern Holds the pattern (its path is not set).
	 * \param _bShowErrors If true, an invalid value is reported and its control is focused.
	 * \return Returns true if every value is valid.
	 */
	bool CTurboConfigWindow::ReadPattern( LSN_TURBO_PATTERN &_tpPattern, bool _bShowErrors ) {
		try {
			auto pwName = FindChild( Layout::LSN_TCWI_NAME_EDIT );
			_tpPattern.u16Name = TurboToU16( pwName ? pwName->GetTextW() : std::wstring() );
			// Surrounding spaces are not part of the name.
			while ( _tpPattern.u16Name.size() && _tpPattern.u16Name.back() == u' ' ) { _tpPattern.u16Name.pop_back(); }
			while ( _tpPattern.u16Name.size() && _tpPattern.u16Name.front() == u' ' ) { _tpPattern.u16Name.erase( _tpPattern.u16Name.begin() ); }
			if ( !CTurboPatterns::ValidName( _tpPattern.u16Name ) ) {
				if ( _bShowErrors ) { ShowError( Layout::LSN_TCWI_NAME_EDIT, LSN_LSTR( LSN_STR_TC_ERR_NAME ) ); }
				return false;
			}

			// The states edit must agree with the length (it is what the user sees).
			auto pwStates = FindChild( Layout::LSN_TCWI_STATES_EDIT );
			LSN_TURBO_PLAYBACK tpStates = m_tpCurrent;
			if ( !pwStates || !CTurboPatterns::FromString( TurboToU16( pwStates->GetTextW() ), tpStates ) ) {
				if ( _bShowErrors ) { ShowError( Layout::LSN_TCWI_STATES_EDIT, LSN_LSTR( LSN_STR_TC_ERR_STATES ) ); }
				return false;
			}
			uint32_t ui32Len;
			if ( !ReadNumber( Layout::LSN_TCWI_LENGTH_EDIT, 1, 64, ui32Len ) ) {
				if ( _bShowErrors ) { ShowError( Layout::LSN_TCWI_LENGTH_EDIT, LSN_LSTR( LSN_STR_TC_ERR_LENGTH ) ); }
				return false;
			}
			uint32_t ui32Fps;
			if ( !ReadNumber( Layout::LSN_TCWI_FPS_EDIT, 1, 255, ui32Fps ) ) {
				if ( _bShowErrors ) { ShowError( Layout::LSN_TCWI_FPS_EDIT, LSN_LSTR( LSN_STR_TC_ERR_FPS ) ); }
				return false;
			}

			LSN_TURBO_PLAYBACK & tpOut = _tpPattern.tpPlayback;
			tpOut = LSN_TURBO_PLAYBACK();
			tpOut.ui64Bits = m_tpCurrent.ui64Bits;
			tpOut.ui8Length = uint8_t( ui32Len );
			// Only the used states matter (unused states are cleared so that two equal patterns compare equal).
			if ( ui32Len < 64 ) { tpOut.ui64Bits &= (1ULL << ui32Len) - 1ULL; }
			tpOut.ui8FramesPerState = uint8_t( ui32Fps );
			auto aChecked = [&]( WORD _wId ) {
				auto pwThis = FindChild( _wId );
				return pwThis ? pwThis->IsChecked() : false;
			};
			tpOut.bLoop = aChecked( Layout::LSN_TCWI_LOOP_RADIO );
			tpOut.bRestartOnPress = aChecked( Layout::LSN_TCWI_RESTART_CHECK );
			tpOut.bHoldAfterEnd = aChecked( Layout::LSN_TCWI_HOLD_CHECK );
			tpOut.bUseGlobalSpeed = aChecked( Layout::LSN_TCWI_GLOBAL_CHECK );
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Determines whether the controls differ from the pattern as it was loaded or last saved.
	 *
	 * \return Returns true if there are unsaved changes.
	 */
	bool CTurboConfigWindow::IsDirty() {
		auto pwName = FindChild( Layout::LSN_TCWI_NAME_EDIT );
		if ( (pwName ? pwName->GetTextW() : std::wstring()) != m_wsBaselineName ) { return true; }
		LSN_TURBO_PATTERN tpRead;
		if ( !ReadPattern( tpRead, false ) ) { return true; }
		LSN_TURBO_PLAYBACK tpBase = m_tpBaseline;
		if ( tpBase.Length() < 64 ) { tpBase.ui64Bits &= (1ULL << tpBase.Length()) - 1ULL; }
		return !(tpRead.tpPlayback == tpBase);
	}

	/**
	 * If there are unsaved changes, asks whether to save them.
	 *
	 * \return Returns true if it is fine to move on (saved or discarded), false if the user canceled or saving failed.
	 */
	bool CTurboConfigWindow::ResolveUnsaved() {
		if ( !IsDirty() ) { return true; }
		switch ( ::MessageBoxW( Wnd(), LSN_LSTR( LSN_STR_TC_UNSAVED ), LSN_LSTR( LSN_STR_TC_TITLE ), MB_YESNOCANCEL | MB_ICONQUESTION ) ) {
			case IDYES : { return SavePattern(); }
			case IDNO : { return true; }
			default : { return false; }
		}
	}

	/**
	 * Saves the pattern in the controls as a preset.
	 *
	 * \return Returns true if it was saved.
	 */
	bool CTurboConfigWindow::SavePattern() {
		if ( !m_ptcdData || !m_ptcdData->ptpPatterns ) { return false; }
		LSN_TURBO_PATTERN tpPattern;
		if ( !ReadPattern( tpPattern, true ) ) { return false; }
		// Unique names (not counting case), other than the preset being edited.
		const LSN_TURBO_PATTERN * ptpSame = m_ptcdData->ptpPatterns->Find( tpPattern.u16Name );
		if ( ptpSame && ptpSame != m_ptcdData->ptpPatterns->Find( m_u16Editing ) ) {
			ShowError( Layout::LSN_TCWI_NAME_EDIT, LSN_LSTR( LSN_STR_TC_ERR_NAME_TAKEN ) );
			return false;
		}
		if ( !m_ptcdData->ptpPatterns->Save( tpPattern, m_u16Editing ) ) {
			lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_STR_TC_ERR_SAVE ), LSN_LSTR( LSN_ERROR ) );
			return false;
		}
		FillList( tpPattern.u16Name );
		ShowPattern( tpPattern.u16Name, TurboToW( tpPattern.u16Name ), tpPattern.tpPlayback );
		return true;
	}

	/**
	 * Starts a new pattern with default values and a unique name.
	 *
	 * \param _bUnsaved If true, the new pattern counts as an unsaved change (closing or switching asks to save it).
	 */
	void CTurboConfigWindow::NewPattern( bool _bUnsaved ) {
		FillList( std::u16string() );
		ShowPattern( std::u16string(), UniqueName( std::wstring(), false ), LSN_TURBO_PLAYBACK() );
		// An empty baseline name makes the new pattern count as changed until it is saved.
		if ( _bUnsaved ) { m_wsBaselineName.clear(); }
		auto pwName = FindChild( Layout::LSN_TCWI_NAME_EDIT );
		if ( pwName ) {
			::SetFocus( pwName->Wnd() );
			::SendMessageW( pwName->Wnd(), EM_SETSEL, 0, -1 );
		}
	}

	/**
	 * Starts a new pattern that is a copy of the one in the controls.
	 */
	void CTurboConfigWindow::DuplicatePattern() {
		LSN_TURBO_PATTERN tpPattern;
		if ( !ReadPattern( tpPattern, true ) ) { return; }
		FillList( std::u16string() );
		ShowPattern( std::u16string(), UniqueName( TurboToW( tpPattern.u16Name ), true ), tpPattern.tpPlayback );
		m_wsBaselineName.clear();
		auto pwName = FindChild( Layout::LSN_TCWI_NAME_EDIT );
		if ( pwName ) {
			::SetFocus( pwName->Wnd() );
			::SendMessageW( pwName->Wnd(), EM_SETSEL, 0, -1 );
		}
	}

	/**
	 * Deletes the preset being edited.
	 */
	void CTurboConfigWindow::DeletePattern() {
		if ( !m_ptcdData || !m_ptcdData->ptpPatterns ) { return; }
		if ( !m_ptcdData->ptpPatterns->Find( m_u16Editing ) ) {
			// A new pattern that was never saved:  just start over.
			NewPattern( false );
			return;
		}
		try {
			std::wstring wsName = TurboToW( m_u16Editing );
			std::wstring wsMsg = std::vformat( std::wstring_view( LSN_LSTR( LSN_STR_TC_CONFIRM_DELETE ) ), std::make_wformat_args( wsName ) );
			if ( ::MessageBoxW( Wnd(), wsMsg.c_str(), LSN_LSTR( LSN_STR_TC_TITLE ), MB_YESNO | MB_ICONQUESTION ) != IDYES ) { return; }
		}
		catch ( ... ) { return; }
		if ( !m_ptcdData->ptpPatterns->Delete( m_u16Editing ) ) {
			lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_STR_TC_ERR_DELETE ), LSN_LSTR( LSN_ERROR ) );
			return;
		}
		const auto & vPatterns = m_ptcdData->ptpPatterns->Patterns();
		if ( vPatterns.size() ) {
			const std::u16string u16Name = vPatterns[0].u16Name;
			FillList( u16Name );
			ShowPattern( u16Name, TurboToW( u16Name ), vPatterns[0].tpPlayback );
		}
		else {
			NewPattern( false );
		}
	}

	/**
	 * Shows the preset selected in the list.
	 */
	void CTurboConfigWindow::ListSelectionChanged() {
		auto plbList = static_cast<lsw::CListBox *>(FindChild( Layout::LSN_TCWI_PRESETS_LIST ));
		if ( !plbList || !m_ptcdData || !m_ptcdData->ptpPatterns ) { return; }
		INT iSel = plbList->GetCurSel();
		const auto & vPatterns = m_ptcdData->ptpPatterns->Patterns();
		if ( iSel < 0 ) { return; }
		size_t sIdx = size_t( plbList->GetItemData( iSel ) );
		if ( sIdx >= vPatterns.size() ) { return; }
		std::u16string u16Name = vPatterns[sIdx].u16Name;
		if ( u16Name == m_u16Editing ) { return; }

		if ( !ResolveUnsaved() ) {
			// Stay on the pattern being edited.
			FillList( m_u16Editing );
			return;
		}
		// Saving may have changed the list.
		const LSN_TURBO_PATTERN * ptpPattern = m_ptcdData->ptpPatterns->Find( u16Name );
		if ( !ptpPattern ) { return; }
		FillList( u16Name );
		ShowPattern( u16Name, TurboToW( u16Name ), ptpPattern->tpPlayback );
	}

	/**
	 * Updates the states edit and the check boxes from m_tpCurrent.
	 *
	 * \param _bStatesEdit If true, the states edit is updated.
	 * \param _bLengthEdit If true, the length edit is updated.
	 */
	void CTurboConfigWindow::UpdateStates( bool _bStatesEdit, bool _bLengthEdit ) {
		m_bUpdating = true;
		if ( _bStatesEdit ) {
			auto pwThis = FindChild( Layout::LSN_TCWI_STATES_EDIT );
			if ( pwThis ) {
				try {
					pwThis->SetTextW( TurboToW( CTurboPatterns::ToString( m_tpCurrent ) ).c_str() );
				}
				catch ( ... ) {}
			}
		}
		if ( _bLengthEdit ) {
			auto pwThis = FindChild( Layout::LSN_TCWI_LENGTH_EDIT );
			if ( pwThis ) { pwThis->SetTextW( std::to_wstring( m_tpCurrent.Length() ).c_str() ); }
		}
		const uint32_t ui32Len = m_tpCurrent.Length();
		for ( uint32_t I = 0; I < 64; ++I ) {
			auto pwCheck = FindChild( WORD( Layout::LSN_TCWI_CHECK_FIRST + I ) );
			if ( pwCheck ) {
				pwCheck->SetCheck( m_tpCurrent.State( I ) );
				// States past the length are kept but not played.
				pwCheck->SetEnabled( I < ui32Len );
			}
			auto pwIndex = FindChild( WORD( Layout::LSN_TCWI_INDEX_FIRST + I ) );
			if ( pwIndex ) { pwIndex->SetEnabled( I < ui32Len ); }
		}
		m_bUpdating = false;
	}

	/**
	 * Enables and disables controls and updates the information label.
	 */
	void CTurboConfigWindow::UpdateControls() {
		auto aChecked = [&]( WORD _wId ) {
			auto pwThis = FindChild( _wId );
			return pwThis ? pwThis->IsChecked() : false;
		};
		const bool bLoop = aChecked( Layout::LSN_TCWI_LOOP_RADIO );
		auto pwThis = FindChild( Layout::LSN_TCWI_RESTART_CHECK );
		if ( pwThis ) { pwThis->SetEnabled( bLoop ); }
		pwThis = FindChild( Layout::LSN_TCWI_HOLD_CHECK );
		if ( pwThis ) { pwThis->SetEnabled( !bLoop ); }
		pwThis = FindChild( Layout::LSN_TCWI_DELETE_BUTTON );
		if ( pwThis ) { pwThis->SetEnabled( m_ptcdData && m_ptcdData->ptpPatterns && m_ptcdData->ptpPatterns->Find( m_u16Editing ) != nullptr ); }

		pwThis = FindChild( Layout::LSN_TCWI_INFO_LABEL );
		if ( pwThis ) {
			try {
				uint32_t ui32Fps;
				if ( ReadNumber( Layout::LSN_TCWI_FPS_EDIT, 1, 255, ui32Fps ) ) {
					uint32_t ui32Frames = m_tpCurrent.Length() * ui32Fps;
					double dSeconds = ui32Frames / 60.0;
					const wchar_t * pwcFormat = aChecked( Layout::LSN_TCWI_GLOBAL_CHECK ) ? LSN_LSTR( LSN_STR_TC_INFO_GLOBAL ) : LSN_LSTR( LSN_STR_TC_INFO );
					pwThis->SetTextW( std::vformat( std::wstring_view( pwcFormat ), std::make_wformat_args( ui32Frames, dSeconds ) ).c_str() );
				}
				else {
					pwThis->SetTextW( L"" );
				}
			}
			catch ( ... ) {}
		}
	}

	/**
	 * Makes a name that no preset has yet.
	 *
	 * \param _wsBase The name to start from.
	 * \param _bCopy If true, names are made as copies of _wsBase ("X Copy", "X Copy 2"), otherwise as numbered patterns ("Pattern 1").
	 * \return Returns the unique name.
	 */
	std::wstring CTurboConfigWindow::UniqueName( const std::wstring &_wsBase, bool _bCopy ) const {
		try {
			for ( uint32_t I = 1; I < 100000; ++I ) {
				std::wstring wsName;
				if ( _bCopy ) {
					// Leave room for the suffix.
					std::wstring wsBase = _wsBase.substr( 0, LSN_TURBO_NAME_MAX - 12 );
					if ( I == 1 ) { wsName = std::vformat( std::wstring_view( LSN_LSTR( LSN_STR_TC_COPY_NAME ) ), std::make_wformat_args( wsBase ) ); }
					else { wsName = std::vformat( std::wstring_view( LSN_LSTR( LSN_STR_TC_COPY_NAME_N ) ), std::make_wformat_args( wsBase, I ) ); }
				}
				else {
					wsName = std::vformat( std::wstring_view( LSN_LSTR( LSN_STR_TC_NEW_NAME ) ), std::make_wformat_args( I ) );
				}
				if ( !m_ptcdData || !m_ptcdData->ptpPatterns || !m_ptcdData->ptpPatterns->Find( TurboToU16( wsName ) ) ) { return wsName; }
			}
		}
		catch ( ... ) {}
		return std::wstring();
	}

	/**
	 * Reads a whole number from an edit control.
	 *
	 * \param _wId The ID of the edit control.
	 * \param _ui32Min The smallest valid value.
	 * \param _ui32Max The largest valid value.
	 * \param _ui32Ret Holds the returned value.
	 * \return Returns true if the edit holds a whole number in range.
	 */
	bool CTurboConfigWindow::ReadNumber( WORD _wId, uint32_t _ui32Min, uint32_t _ui32Max, uint32_t &_ui32Ret ) {
		auto pwThis = FindChild( _wId );
		if ( !pwThis ) { return false; }
		std::wstring wsText = pwThis->GetTextW();
		uint64_t ui64Val = 0;
		size_t sDigits = 0;
		for ( auto wcThis : wsText ) {
			if ( wcThis == L' ' ) { continue; }
			if ( wcThis < L'0' || wcThis > L'9' ) { return false; }
			ui64Val = ui64Val * 10 + uint64_t( wcThis - L'0' );
			if ( ++sDigits > 9 ) { return false; }
		}
		if ( !sDigits || ui64Val < _ui32Min || ui64Val > _ui32Max ) { return false; }
		_ui32Ret = uint32_t( ui64Val );
		return true;
	}

	/**
	 * Shows an error and focuses a control.
	 *
	 * \param _wId The ID of the control.
	 * \param _pwcError The error to show.
	 */
	void CTurboConfigWindow::ShowError( WORD _wId, const wchar_t * _pwcError ) {
		lsw::CBase::MessageBoxError( Wnd(), _pwcError, LSN_LSTR( LSN_ERROR ) );
		auto pwThis = FindChild( _wId );
		if ( pwThis ) {
			::SetFocus( pwThis->Wnd() );
			::SendMessageW( pwThis->Wnd(), EM_SETSEL, 0, -1 );
		}
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

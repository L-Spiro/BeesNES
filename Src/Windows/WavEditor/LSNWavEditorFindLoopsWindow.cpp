#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The Find Loop Points dialog.  Searches the metadata of a track (the logged APU register writes) for ranges of events that
 *	repeat a given number of times and lets the user pick one of them as the ending point of the track.
 */

#include "LSNWavEditorFindLoopsWindow.h"
#include "../../Localization/LSNLocalization.h"
#include "LSNWavEditorSequencingPage.h"

#include <ListBox/LSWListBox.h>

#include <cmath>
#include <format>


namespace lsn {

	// == Members.
	/** The last approximate loop start (remembered between uses of the dialog). */
	std::wstring CWavEditorFindLoopsWindow::m_wsStart;
	/** The last approximate first loop point. */
	std::wstring CWavEditorFindLoopsWindow::m_wsLoop;
	/** The last number of loops. */
	std::wstring CWavEditorFindLoopsWindow::m_wsLoops = L"2";
	/** The last search range. */
	std::wstring CWavEditorFindLoopsWindow::m_wsRange = L"1";
	/** The last allowed percentage of mismatched events. */
	std::wstring CWavEditorFindLoopsWindow::m_wsMismatch = L"1";

	CWavEditorFindLoopsWindow::CWavEditorFindLoopsWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pfldData( reinterpret_cast<CWavEditorFindLoopsWindowLayout::LSN_FIND_LOOPS_DATA *>(_ui64Data) ) {
	}

	// == Functions.
	/**
	 * The WM_INITDIALOG handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CWavEditorFindLoopsWindow::InitDialog() {
		Parent::InitDialog();

		struct LSN_TEXTS {
			Layout::LSN_CONTROL_IDS										wId;
			const std::wstring *										pwsText;
		} tTexts[] = {
			{ Layout::LSN_WFLI_START_EDIT,								&m_wsStart },
			{ Layout::LSN_WFLI_LOOP_EDIT,								&m_wsLoop },
			{ Layout::LSN_WFLI_LOOPS_EDIT,								&m_wsLoops },
			{ Layout::LSN_WFLI_RANGE_EDIT,								&m_wsRange },
			{ Layout::LSN_WFLI_MISMATCH_EDIT,							&m_wsMismatch },
		};
		for ( auto I = std::size( tTexts ); I--; ) {
			auto pwThis = FindChild( tTexts[I].wId );
			if ( pwThis ) { pwThis->SetTextW( tTexts[I].pwsText->c_str() ); }
		}

		UpdateResultLabel( Layout::LSN_WFLI_START_EDIT, Layout::LSN_WFLI_START_RESULT_LABEL );
		UpdateResultLabel( Layout::LSN_WFLI_LOOP_EDIT, Layout::LSN_WFLI_LOOP_RESULT_LABEL );

		auto pwStart = FindChild( Layout::LSN_WFLI_START_EDIT );
		if ( pwStart ) { ::SetFocus( pwStart->Wnd() ); }
		return LSW_H_HANDLED;
	}

	/**
	 * The WM_CLOSE handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CWavEditorFindLoopsWindow::Close() {
		SaveTexts();
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
	CWidget::LSW_HANDLED CWavEditorFindLoopsWindow::Command( WORD _wCtrlCode, WORD _wId, CWidget * /*_pwSrc*/ ) {
		switch ( _wId ) {
			case IDOK : {}								LSN_FALLTHROUGH		// Enter in an edit control.
			case Layout::LSN_WFLI_FIND : {
				Find();
				return LSW_H_HANDLED;
			}
			case Layout::LSN_WFLI_START_EDIT : {
				if ( _wCtrlCode == EN_CHANGE ) { UpdateResultLabel( _wId, Layout::LSN_WFLI_START_RESULT_LABEL ); }
				break;
			}
			case Layout::LSN_WFLI_LOOP_EDIT : {
				if ( _wCtrlCode == EN_CHANGE ) { UpdateResultLabel( _wId, Layout::LSN_WFLI_LOOP_RESULT_LABEL ); }
				break;
			}
			case Layout::LSN_WFLI_LISTBOX : {
				switch ( _wCtrlCode ) {
					case LBN_SELCHANGE : {
						SelectionChanged();
						return LSW_H_HANDLED;
					}
					case LBN_DBLCLK : {
						Apply();
						return LSW_H_HANDLED;
					}
					default : {}
				}
				break;
			}
			case Layout::LSN_WFLI_APPLY : {
				Apply();
				return LSW_H_HANDLED;
			}
			case Layout::LSN_WFLI_CANCEL : {}				LSN_FALLTHROUGH
			case IDCANCEL : {
				SaveTexts();
				::EndDialog( Wnd(), 0 );
				return LSW_H_HANDLED;
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
	CWidget::LSW_HANDLED CWavEditorFindLoopsWindow::MenuCommand( WORD _wId ) {
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
	 * Reads the search parameters, searches for loops, and fills the list box with the results.
	 */
	void CWavEditorFindLoopsWindow::Find() {
		auto plbList = reinterpret_cast<CListBox *>(FindChild( Layout::LSN_WFLI_LISTBOX ));
		auto pwDetails = FindChild( Layout::LSN_WFLI_DETAILS_EDIT );
		auto pwApply = FindChild( Layout::LSN_WFLI_APPLY );
		if ( !plbList || !pwDetails || !pwApply ) { return; }
		if ( !m_pfldData || !m_pfldData->pvMetadata || !m_pfldData->pvMetadata->size() ) {
			lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_WE_FL_NO_METADATA ), LSN_LSTR( LSN_ERROR ) );
			return;
		}

		double dStart, dLoop, dLoops, dRange, dMismatch;
		if ( !ReadEdit( Layout::LSN_WFLI_START_EDIT, LSN_LSTR( LSN_WE_FL_ERR_START ), dStart ) ) { return; }
		if ( !ReadEdit( Layout::LSN_WFLI_LOOP_EDIT, LSN_LSTR( LSN_WE_FL_ERR_LOOP ), dLoop ) ) { return; }
		if ( !ReadEdit( Layout::LSN_WFLI_LOOPS_EDIT, LSN_LSTR( LSN_WE_FL_ERR_LOOPS ), dLoops ) ) { return; }
		if ( !(dLoops >= 1.0 && dLoops < 1000000.0) ) {
			EditError( Layout::LSN_WFLI_LOOPS_EDIT, LSN_LSTR( LSN_WE_FL_ERR_LOOPS ) );
			return;
		}
		if ( !ReadEdit( Layout::LSN_WFLI_RANGE_EDIT, LSN_LSTR( LSN_WE_FL_ERR_RANGE ), dRange ) ) { return; }
		if ( !(dRange >= 0.0) ) {
			EditError( Layout::LSN_WFLI_RANGE_EDIT, LSN_LSTR( LSN_WE_FL_ERR_RANGE ) );
			return;
		}
		if ( !ReadEdit( Layout::LSN_WFLI_MISMATCH_EDIT, LSN_LSTR( LSN_WE_FL_ERR_MISMATCH ), dMismatch ) ) { return; }
		if ( !(dMismatch >= 0.0 && dMismatch <= 100.0) ) {
			EditError( Layout::LSN_WFLI_MISMATCH_EDIT, LSN_LSTR( LSN_WE_FL_ERR_MISMATCH ) );
			return;
		}
		SaveTexts();

		HCURSOR hOld = ::SetCursor( ::LoadCursorW( NULL, IDC_WAIT ) );
		bool bFound = CWavEditor::FindLoops( (*m_pfldData->pvMetadata), dStart, dLoop, dRange, uint32_t( std::floor( dLoops ) ), dMismatch, m_vResults );

		plbList->ResetContent();
		pwApply->SetEnabled( FALSE );
		LONG lWidest = 0;
		HWND hList = plbList->Wnd();
		HDC hDc = ::GetDC( hList );
		HGDIOBJ hOldFont = NULL;
		if ( hDc ) {
			HFONT hFont = reinterpret_cast<HFONT>(::SendMessageW( hList, WM_GETFONT, 0, 0 ));
			if ( hFont ) { hOldFont = ::SelectObject( hDc, hFont ); }
		}
		try {
			for ( size_t I = 0; I < m_vResults.size(); ++I ) {
				std::wstring wsText = std::wstring( LSN_LSTR( LSN_WE_FL_ITEM_START ) ) + L" " + EventText( m_vResults[I].sStart ) +
					L"    " + LSN_LSTR( LSN_WE_FL_ITEM_END ) + L" " + EventText( m_vResults[I].sEnd );
				INT iIdx = plbList->AddString( wsText.c_str() );
				if ( iIdx < 0 ) { break; }
				plbList->SetItemData( iIdx, LPARAM( I ) );
				SIZE sExtent;
				if ( hDc && ::GetTextExtentPoint32W( hDc, wsText.c_str(), int( wsText.size() ), &sExtent ) ) {
					lWidest = std::max( lWidest, sExtent.cx );
				}
			}
		}
		catch ( ... ) { bFound = false; }
		if ( hDc ) {
			if ( hOldFont ) { ::SelectObject( hDc, hOldFont ); }
			::ReleaseDC( hList, hDc );
		}
		::SendMessageW( hList, LB_SETHORIZONTALEXTENT, WPARAM( lWidest + 8 ), 0 );
		::SetCursor( hOld );

		if ( !bFound ) {
			lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_OUT_OF_MEMORY ), LSN_LSTR( LSN_ERROR ) );
			pwDetails->SetTextW( L"" );
			return;
		}
		if ( !m_vResults.size() ) {
			pwDetails->SetTextW( LSN_LSTR( LSN_WE_FL_NONE_FOUND ) );
			return;
		}
		pwDetails->SetTextW( std::format( LSN_LSTR( LSN_WE_FL_FOUND ), m_vResults.size() ).c_str() );
	}

	/**
	 * Shows the details of the selected loop range and enables or disables the Apply button.
	 */
	void CWavEditorFindLoopsWindow::SelectionChanged() {
		auto plbList = reinterpret_cast<CListBox *>(FindChild( Layout::LSN_WFLI_LISTBOX ));
		auto pwDetails = FindChild( Layout::LSN_WFLI_DETAILS_EDIT );
		auto pwApply = FindChild( Layout::LSN_WFLI_APPLY );
		if ( !plbList || !pwDetails || !pwApply ) { return; }

		INT iSel = plbList->GetCurSel();
		size_t sIdx = iSel >= 0 ? size_t( plbList->GetItemData( iSel ) ) : SIZE_MAX;
		if ( sIdx >= m_vResults.size() || !m_pfldData || !m_pfldData->pvMetadata ) {
			pwApply->SetEnabled( FALSE );
			return;
		}
		const auto & lrRange = m_vResults[sIdx];
		const auto & vMeta = (*m_pfldData->pvMetadata);
		try {
			std::wstring wsText = std::wstring( LSN_LSTR( LSN_WE_FL_ITEM_START ) ) + L" " + EventText( lrRange.sStart ) + L"\r\n" +
				LSN_LSTR( LSN_WE_FL_FIRST_LOOP ) + L" " + EventText( lrRange.sLoop ) + L"\r\n" +
				LSN_LSTR( LSN_WE_FL_ITEM_END ) + L" " + EventText( lrRange.sEnd ) + L"\r\n" +
				std::format( LSN_LSTR( LSN_WE_FL_DETAILS ),
					vMeta[lrRange.sLoop].dTime - vMeta[lrRange.sStart].dTime, lrRange.sLoop - lrRange.sStart,
					vMeta[lrRange.sEnd].dTime - vMeta[lrRange.sStart].dTime, lrRange.sMismatches );
			pwDetails->SetTextW( wsText.c_str() );
		}
		catch ( ... ) {}
		pwApply->SetEnabled( TRUE );
	}

	/**
	 * Returns the ending point of the selected loop range and closes the dialog.
	 */
	void CWavEditorFindLoopsWindow::Apply() {
		auto plbList = reinterpret_cast<CListBox *>(FindChild( Layout::LSN_WFLI_LISTBOX ));
		if ( !plbList || !m_pfldData ) { return; }
		INT iSel = plbList->GetCurSel();
		if ( iSel < 0 ) { return; }
		size_t sIdx = size_t( plbList->GetItemData( iSel ) );
		if ( sIdx >= m_vResults.size() ) { return; }

		m_pfldData->sEnd = m_vResults[sIdx].sEnd;
		SaveTexts();
		::EndDialog( Wnd(), 1 );
	}

	/**
	 * Stores the texts of the edit controls so that they can be restored the next time the dialog is opened.
	 */
	void CWavEditorFindLoopsWindow::SaveTexts() {
		struct LSN_TEXTS {
			Layout::LSN_CONTROL_IDS										wId;
			std::wstring *												pwsText;
		} tTexts[] = {
			{ Layout::LSN_WFLI_START_EDIT,								&m_wsStart },
			{ Layout::LSN_WFLI_LOOP_EDIT,								&m_wsLoop },
			{ Layout::LSN_WFLI_LOOPS_EDIT,								&m_wsLoops },
			{ Layout::LSN_WFLI_RANGE_EDIT,								&m_wsRange },
			{ Layout::LSN_WFLI_MISMATCH_EDIT,							&m_wsMismatch },
		};
		try {
			for ( auto I = std::size( tTexts ); I--; ) {
				auto pwThis = FindChild( tTexts[I].wId );
				if ( pwThis ) { (*tTexts[I].pwsText) = pwThis->GetTextW(); }
			}
		}
		catch ( ... ) {}
	}

	/**
	 * Reads an expression from an edit control.  Shows an error and focuses the control on failure.
	 *
	 * \param _wId The ID of the edit control.
	 * \param _pwcError The error to show on failure.
	 * \param _dRet Holds the returned value.
	 * \return Returns true if the edit control holds a valid expression.
	 */
	bool CWavEditorFindLoopsWindow::ReadEdit( WORD _wId, const wchar_t * _pwcError, double &_dRet ) {
		auto pwEdit = FindChild( _wId );
		if ( !pwEdit ) { return false; }
		ee::CExpEvalContainer::EE_RESULT rRes;
		if ( pwEdit->GetTextAsDoubleExpression( rRes ) ) {
			_dRet = rRes.u.dVal;
			if ( !std::isnan( _dRet ) ) { return true; }
		}
		EditError( _wId, _pwcError );
		return false;
	}

	/**
	 * Shows an error about the value of an edit control and focuses the control.
	 *
	 * \param _wId The ID of the edit control.
	 * \param _pwcError The error to show.
	 */
	void CWavEditorFindLoopsWindow::EditError( WORD _wId, const wchar_t * _pwcError ) {
		lsw::CBase::MessageBoxError( Wnd(), _pwcError, LSN_LSTR( LSN_ERROR ) );
		auto pwEdit = FindChild( _wId );
		if ( pwEdit ) {
			::SetFocus( pwEdit->Wnd() );
			::SendMessageW( pwEdit->Wnd(), EM_SETSEL, 0, -1 );
		}
	}

	/**
	 * Shows the evaluated value of a time edit control (which often holds an expression such as a sample number divided by a sample rate) in
	 *	seconds in the label to its right.
	 *
	 * \param _wEditId The ID of the edit control.
	 * \param _wLabelId The ID of the label.
	 */
	void CWavEditorFindLoopsWindow::UpdateResultLabel( WORD _wEditId, WORD _wLabelId ) {
		auto pwEdit = FindChild( _wEditId );
		auto pwLabel = FindChild( _wLabelId );
		if ( !pwEdit || !pwLabel ) { return; }
		try {
			std::wstring wsText = pwEdit->GetTextW();
			if ( wsText.find_first_not_of( L" \t\r\n" ) == std::wstring::npos ) {
				pwLabel->SetTextW( L"" );
				return;
			}
			ee::CExpEvalContainer::EE_RESULT rRes;
			if ( pwEdit->GetTextAsDoubleExpression( rRes ) && !std::isnan( rRes.u.dVal ) ) {
				pwLabel->SetTextW( std::format( LSN_LSTR( LSN_WE_FL_SECONDS ), rRes.u.dVal ).c_str() );
			}
			else {
				pwLabel->SetTextW( LSN_LSTR( LSN_WE_FL_INVALID ) );
			}
		}
		catch ( ... ) {}
	}

	/**
	 * Gets the text of a metadata entry as it is shown in the metadata combo boxes of the WAV Editor.
	 *
	 * \param _sIdx The index of the metadata entry.
	 * \return Returns the text of the metadata entry.
	 */
	std::wstring CWavEditorFindLoopsWindow::EventText( size_t _sIdx ) const {
		if ( !m_pfldData || !m_pfldData->pvMetadata || _sIdx >= m_pfldData->pvMetadata->size() ) { return std::wstring(); }
		std::string sText = CWavEditorSequencingPage::MetadataText( (*m_pfldData->pvMetadata), _sIdx );
		std::wstring wsRet;
		wsRet.reserve( sText.size() );
		for ( auto C : sText ) { wsRet.push_back( wchar_t( uint8_t( C ) ) ); }
		return wsRet;
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

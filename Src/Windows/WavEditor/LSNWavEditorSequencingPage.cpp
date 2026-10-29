#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The audio options window.
 */

#include "LSNWavEditorSequencingPage.h"
#include "../../Localization/LSNLocalization.h"
#include "../Layout/LSNLayoutMacros.h"
#include "LSNWavEditorWindow.h"

#include <CheckButton/LSWCheckButton.h>
#include <ComboBox/LSWComboBox.h>
#include <TreeListView/LSWTreeListView.h>
#include <Tab/LSWTab.h>

#include <commdlg.h>
#include <filesystem>
#include <format>

#include "../../../resource.h"


namespace lsn {

	// == Members.
	/** The edit controls whose texts are stored per track. */
	const WORD CWavEditorSequencingPage::m_wEditIds[] = {
		Layout::LSN_WEWI_SEQ_START_EDIT,
		Layout::LSN_WEWI_SEQ_LOOPS_STOP_EDIT,
		Layout::LSN_WEWI_SEQ_LOOPS_DELAY_EDIT,
		Layout::LSN_WEWI_SEQ_LOOPS_FADE_EDIT,
		Layout::LSN_WEWI_SEQ_SILENCE_OPEN_SIL_EDIT,
		Layout::LSN_WEWI_SEQ_SILENCE_TRAIL_EDIT,
	};

	/** The combo boxes whose selections are stored per track. */
	const WORD CWavEditorSequencingPage::m_wComboIds[] = {
		Layout::LSN_WEWI_SEQ_START_COMBO,
		Layout::LSN_WEWI_SEQ_LOOPS_STOP_COMBO,
	};

	/** The check boxes and radio buttons whose check states are stored per track. */
	const WORD CWavEditorSequencingPage::m_wCheckIds[] = {
		Layout::LSN_WEWI_SEQ_START_EXACT_RADIO,
		Layout::LSN_WEWI_SEQ_START_MINUS_ONE_RADIO,
		Layout::LSN_WEWI_SEQ_START_SNAP_RADIO,
		Layout::LSN_WEWI_SEQ_STOP_EXACT_RADIO,
		Layout::LSN_WEWI_SEQ_STOP_MINUS_ONE_RADIO,
		Layout::LSN_WEWI_SEQ_STOP_SNAP_RADIO,
		Layout::LSN_WEWI_SEQ_LOOP_RADIO,
		Layout::LSN_WEWI_SEQ_ONE_SHOT_RADIO,
	};

	CWavEditorSequencingPage::CWavEditorSequencingPage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pwewoOptions( reinterpret_cast<LSN_WAV_EDITOR_WINDOW_OPTIONS *>(_ui64Data) ) {
	}

	// == Functions.
	/**
	 * Gets the stored text of an edit control.
	 * 
	 * \param _wId The ID of the edit control.
	 * \return Returns a pointer to the stored text of the given edit control or nullptr if _wId is not an edit control on this page.
	 **/
	std::wstring * CWavEditorSequencingPage::LSN_TRACK::Text( WORD _wId ) {
		switch ( _wId ) {
			case Layout::LSN_WEWI_SEQ_START_EDIT : { return &wsStartTime; }
			case Layout::LSN_WEWI_SEQ_LOOPS_STOP_EDIT : { return &wsStopTime; }
			case Layout::LSN_WEWI_SEQ_LOOPS_DELAY_EDIT : { return &wsPreFadeDur; }
			case Layout::LSN_WEWI_SEQ_LOOPS_FADE_EDIT : { return &wsFadeDur; }
			case Layout::LSN_WEWI_SEQ_SILENCE_OPEN_SIL_EDIT : { return &wsOpeningSilence; }
			case Layout::LSN_WEWI_SEQ_SILENCE_TRAIL_EDIT : { return &wsTrailingSilence; }
		}
		return nullptr;
	}

	/**
	 * Gets the stored selection index of a combo box.
	 * 
	 * \param _wId The ID of the combo box.
	 * \return Returns a pointer to the stored selection index of the given combo box or nullptr if _wId is not a combo box on this page.
	 **/
	int32_t * CWavEditorSequencingPage::LSN_TRACK::Selection( WORD _wId ) {
		switch ( _wId ) {
			case Layout::LSN_WEWI_SEQ_START_COMBO : { return &i32StartSelection; }
			case Layout::LSN_WEWI_SEQ_LOOPS_STOP_COMBO : { return &i32StopSelection; }
		}
		return nullptr;
	}

	/**
	 * Gets the stored check state of a check box or radio button.
	 * 
	 * \param _wId The ID of the check box or radio button.
	 * \return Returns a pointer to the stored check state of the given control or nullptr if _wId is not a check box or radio button on this page.
	 **/
	bool * CWavEditorSequencingPage::LSN_TRACK::Check( WORD _wId ) {
		switch ( _wId ) {
			case Layout::LSN_WEWI_SEQ_START_EXACT_RADIO : { return &bStartExact; }
			case Layout::LSN_WEWI_SEQ_START_MINUS_ONE_RADIO : { return &bStartMinusOne; }
			case Layout::LSN_WEWI_SEQ_START_SNAP_RADIO : { return &bStartSnap; }
			case Layout::LSN_WEWI_SEQ_STOP_EXACT_RADIO : { return &bStopExact; }
			case Layout::LSN_WEWI_SEQ_STOP_MINUS_ONE_RADIO : { return &bStopMinusOne; }
			case Layout::LSN_WEWI_SEQ_STOP_SNAP_RADIO : { return &bStopSnap; }
			case Layout::LSN_WEWI_SEQ_LOOP_RADIO : { return &bLoop; }
			case Layout::LSN_WEWI_SEQ_ONE_SHOT_RADIO : { return &bOneShot; }
		}
		return nullptr;
	}

	/**
	 * The WM_INITDIALOG handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CWavEditorSequencingPage::InitDialog() {
		Parent::InitDialog();
		m_bInternalUpdate = true;
		auto ptlTree = reinterpret_cast<CTreeListView *>(FindChild( Layout::LSN_WEWI_SEQ_OPS_TREELISTVEW ));
		if ( ptlTree ) {
			ptlTree->SetColumnText( LSN_LSTR( LSN_WE_OPERATIONS ), 0 );
			ptlTree->SetColumnWidth( 0, 250 );
			ptlTree->InsertColumn( LSN_LSTR( LSN_WE_PARMS ), 180, -1 );
			ptlTree->SetColumnWidth( 1, 450 );
			ptlTree->InsertColumn( LSN_LSTR( LSN_WE_DESC ), 180, -1 );
			ptlTree->SetColumnWidth( 2, 550 );
		}

		// Last texts.
		auto aTmp = FindChild( Layout::LSN_WEWI_SEQ_LOOPS_STOP_EDIT );
		if ( m_pwewoOptions->vPerFileOptions.size() ) {
			//if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->vPerFileOptions[0].wsStopTime.c_str() ); }
			aTmp = FindChild( Layout::LSN_WEWI_SEQ_LOOPS_DELAY_EDIT );
			if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->vPerFileOptions[0].wsPreFadeDur.c_str() ); }
			aTmp = FindChild( Layout::LSN_WEWI_SEQ_LOOPS_FADE_EDIT );
			if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->vPerFileOptions[0].wsFadeDur.c_str() ); }

			aTmp = FindChild( Layout::LSN_WEWI_SEQ_SILENCE_OPEN_SIL_EDIT );
			if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->vPerFileOptions[0].wsOpeningSilence.c_str() ); }
			aTmp = FindChild( Layout::LSN_WEWI_SEQ_SILENCE_TRAIL_EDIT );
			if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->vPerFileOptions[0].wsTrailingSilence.c_str() ); }


			// Last checks.
			aTmp = FindChild( Layout::LSN_WEWI_SEQ_START_EXACT_RADIO );
			if ( aTmp ) { aTmp->SetCheck( m_pwewoOptions->vPerFileOptions[0].i32StartMod == LSN_SST_EXACT ); }
			aTmp = FindChild( Layout::LSN_WEWI_SEQ_START_MINUS_ONE_RADIO );
			if ( aTmp ) { aTmp->SetCheck( m_pwewoOptions->vPerFileOptions[0].i32StartMod == LSN_SST_MINUS_ONE ); }
			aTmp = FindChild( Layout::LSN_WEWI_SEQ_START_SNAP_RADIO );
			if ( aTmp ) { aTmp->SetCheck( m_pwewoOptions->vPerFileOptions[0].i32StartMod == LSN_SST_SNAP ); }

			aTmp = FindChild( Layout::LSN_WEWI_SEQ_STOP_EXACT_RADIO );
			if ( aTmp ) { aTmp->SetCheck( m_pwewoOptions->vPerFileOptions[0].i32StopMod == LSN_SST_EXACT ); }
			aTmp = FindChild( Layout::LSN_WEWI_SEQ_STOP_MINUS_ONE_RADIO );
			if ( aTmp ) { aTmp->SetCheck( m_pwewoOptions->vPerFileOptions[0].i32StopMod == LSN_SST_MINUS_ONE ); }
			aTmp = FindChild( Layout::LSN_WEWI_SEQ_STOP_SNAP_RADIO );
			if ( aTmp ) { aTmp->SetCheck( m_pwewoOptions->vPerFileOptions[0].i32StopMod == LSN_SST_SNAP ); }

			aTmp = FindChild( Layout::LSN_WEWI_SEQ_LOOP_RADIO );
			if ( aTmp ) { aTmp->SetCheck( m_pwewoOptions->vPerFileOptions[0].bLoop ); }
			aTmp = FindChild( Layout::LSN_WEWI_SEQ_ONE_SHOT_RADIO );
			if ( aTmp ) { aTmp->SetCheck( !m_pwewoOptions->vPerFileOptions[0].bLoop ); }
		}

		auto pwWarning = FindChild( Layout::LSN_WEWI_SEQ_LOOPS_WARNING_LABEL );
		if ( pwWarning ) {
			/*pwWarning->SetIcons( NULL,
				reinterpret_cast<HICON>(::LoadImageW( CBase::GetModuleHandleW( nullptr ), MAKEINTRESOURCEW( IDI_WARNING_ICON_32 ), IMAGE_ICON, 0, 0, LR_LOADTRANSPARENT | LR_DEFAULTSIZE | LR_SHARED )) );*/
			::SendMessageW( pwWarning->Wnd(), STM_SETIMAGE, IMAGE_ICON, reinterpret_cast<LPARAM>(reinterpret_cast<HICON>(::LoadImageW( CBase::GetModuleHandleW( nullptr ), MAKEINTRESOURCEW( IDI_WARNING_ICON_32 ), IMAGE_ICON, 0, 0, LR_LOADTRANSPARENT | LR_DEFAULTSIZE | LR_SHARED ))) );
		}
		m_bInternalUpdate = false;
		Update();
		return LSW_H_CONTINUE;
	}

	/**
	 * Handles the WM_COMMAND message.
	 *
	 * \param _wCtrlCode 0 = from menu, 1 = from accelerator, otherwise it is a Control-defined notification code.
	 * \param _wId The ID of the control if _wCtrlCode is not 0 or 1.
	 * \param _pwSrc The source control if _wCtrlCode is not 0 or 1.
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CWavEditorSequencingPage::Command( WORD _wCtrlCode, WORD _wId, CWidget * /*_pwSrc*/ ) {
		switch ( _wId ) {
			case Layout::LSN_WEWI_SEQ_START_COMBO : {
				switch ( _wCtrlCode ) {
					case CBN_SELCHANGE : {
						auto pcbCombo = reinterpret_cast<CComboBox *>(FindChild( _wId ));
						if ( pcbCombo ) {
							if ( !m_bInternalUpdate ) {
								std::vector<LPARAM> vUpdateMe;
								GetPagesToUpdate( vUpdateMe );
								static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSeqComboSels( _wId, pcbCombo->GetCurSel(), vUpdateMe );
							}
							auto aSelected = pcbCombo->GetCurSelItemData();
							if ( m_pweEditor ) {
								auto pwsSet = m_pweEditor->WavById( UniqueId() );
								if ( pwsSet && size_t( aSelected ) < pwsSet->vMetadata.size() ) {
									auto sText = std::format( "{:.35g}", pwsSet->vMetadata[aSelected].dTime );
									auto pwThis = FindChild( Layout::LSN_WEWI_SEQ_START_EDIT );
									if ( pwThis ) {
										pwThis->SetTextA( sText.c_str() );
									}
								}
							}
						}
						break;
					}
				}
				break;
			}
			/*case Layout::LSN_WEWI_SEQ_END_COMBO : {
				switch ( _wCtrlCode ) {
					case CBN_SELCHANGE : {
						auto pcbCombo = reinterpret_cast<CComboBox *>(FindChild( _wId ));
						if ( pcbCombo ) {
							auto aSelected = pcbCombo->GetCurSelItemData();
							if ( m_pweEditor ) {
								auto pwsSet = m_pweEditor->WavById( UniqueId() );
								if ( pwsSet && size_t( aSelected ) < pwsSet->vMetadata.size() ) {
									auto sText = std::format( "{:.35g}", pwsSet->vMetadata[aSelected].dTime );
									auto pwThis = FindChild( Layout::LSN_WEWI_SEQ_END_EDIT );
									if ( pwThis ) {
										pwThis->SetTextA( sText.c_str() );
									}
								}
							}
						}
						break;
					}
				}
				break;
			}*/
			case Layout::LSN_WEWI_SEQ_LOOPS_STOP_COMBO : {
				switch ( _wCtrlCode ) {
					case CBN_SELCHANGE : {
						auto pcbCombo = reinterpret_cast<CComboBox *>(FindChild( _wId ));
						if ( pcbCombo ) {
							if ( !m_bInternalUpdate ) {
								std::vector<LPARAM> vUpdateMe;
								GetPagesToUpdate( vUpdateMe );
								static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSeqComboSels( _wId, pcbCombo->GetCurSel(), vUpdateMe );
							}
							auto aSelected = pcbCombo->GetCurSelItemData();
							if ( m_pweEditor ) {
								auto pwsSet = m_pweEditor->WavById( UniqueId() );
								if ( pwsSet && size_t( aSelected ) < pwsSet->vMetadata.size() ) {
									auto sText = std::format( "{:.35g}", pwsSet->vMetadata[aSelected].dTime );
									auto pwThis = FindChild( Layout::LSN_WEWI_SEQ_LOOPS_STOP_EDIT );
									if ( pwThis ) {
										pwThis->SetTextA( sText.c_str() );
									}
								}
							}
						}
						break;
					}
				}
				break;
			}
			case Layout::LSN_WEWI_SEQ_START_EDIT : {}
			//case Layout::LSN_WEWI_SEQ_END_EDIT : {}
			case Layout::LSN_WEWI_SEQ_LOOPS_STOP_EDIT : {}			LSN_FALLTHROUGH
			case Layout::LSN_WEWI_SEQ_LOOPS_DELAY_EDIT : {}			LSN_FALLTHROUGH
			case Layout::LSN_WEWI_SEQ_LOOPS_FADE_EDIT : {}			LSN_FALLTHROUGH
			case Layout::LSN_WEWI_SEQ_SILENCE_OPEN_SIL_EDIT : {}	LSN_FALLTHROUGH
			case Layout::LSN_WEWI_SEQ_SILENCE_TRAIL_EDIT : {
				if ( _wCtrlCode == EN_CHANGE ) {
					if ( !m_bInternalUpdate ) {
						// Update all or update selected.
						std::vector<LPARAM> vUpdateMe;
						GetPagesToUpdate( vUpdateMe );
						auto pwThis = FindChild( _wId );
						auto wsText = pwThis ? pwThis->GetTextW() : L"";
						static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSeqEditTexts( _wId, wsText, vUpdateMe );
					}
				}
				break;
			}
			case Layout::LSN_WEWI_SEQ_START_EXACT_RADIO : {
				if ( _wCtrlCode == BN_CLICKED ) {
					if ( !m_bInternalUpdate ) {
						std::vector<LPARAM> vUpdateMe;
						GetPagesToUpdate( vUpdateMe );
						auto pwThis = FindChild( _wId );
						bool bChecked = pwThis ? pwThis->IsChecked() : false;
						static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSeqCheckStates( _wId, bChecked, vUpdateMe );
						static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSeqCheckStates( Layout::LSN_WEWI_SEQ_START_MINUS_ONE_RADIO, !bChecked, vUpdateMe );
						static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSeqCheckStates( Layout::LSN_WEWI_SEQ_START_SNAP_RADIO, !bChecked, vUpdateMe );
					}
				}
				break;
			}
			case Layout::LSN_WEWI_SEQ_START_MINUS_ONE_RADIO : {
				if ( _wCtrlCode == BN_CLICKED ) {
					if ( !m_bInternalUpdate ) {
						std::vector<LPARAM> vUpdateMe;
						GetPagesToUpdate( vUpdateMe );
						auto pwThis = FindChild( _wId );
						bool bChecked = pwThis ? pwThis->IsChecked() : false;
						static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSeqCheckStates( _wId, bChecked, vUpdateMe );
						static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSeqCheckStates( Layout::LSN_WEWI_SEQ_START_EXACT_RADIO, !bChecked, vUpdateMe );
						static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSeqCheckStates( Layout::LSN_WEWI_SEQ_START_SNAP_RADIO, !bChecked, vUpdateMe );
					}
				}
				break;
			}
			case Layout::LSN_WEWI_SEQ_START_SNAP_RADIO : {
				if ( _wCtrlCode == BN_CLICKED ) {
					if ( !m_bInternalUpdate ) {
						std::vector<LPARAM> vUpdateMe;
						GetPagesToUpdate( vUpdateMe );
						auto pwThis = FindChild( _wId );
						bool bChecked = pwThis ? pwThis->IsChecked() : false;
						static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSeqCheckStates( _wId, bChecked, vUpdateMe );
						static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSeqCheckStates( Layout::LSN_WEWI_SEQ_START_EXACT_RADIO, !bChecked, vUpdateMe );
						static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSeqCheckStates( Layout::LSN_WEWI_SEQ_START_MINUS_ONE_RADIO, !bChecked, vUpdateMe );
					}
				}
				break;
			}

			case Layout::LSN_WEWI_SEQ_STOP_EXACT_RADIO : {
				if ( _wCtrlCode == BN_CLICKED ) {
					if ( !m_bInternalUpdate ) {
						std::vector<LPARAM> vUpdateMe;
						GetPagesToUpdate( vUpdateMe );
						auto pwThis = FindChild( _wId );
						bool bChecked = pwThis ? pwThis->IsChecked() : false;
						static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSeqCheckStates( _wId, bChecked, vUpdateMe );
						static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSeqCheckStates( Layout::LSN_WEWI_SEQ_STOP_MINUS_ONE_RADIO, !bChecked, vUpdateMe );
						static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSeqCheckStates( Layout::LSN_WEWI_SEQ_STOP_SNAP_RADIO, !bChecked, vUpdateMe );
					}
				}
				break;
			}
			case Layout::LSN_WEWI_SEQ_STOP_MINUS_ONE_RADIO : {
				if ( _wCtrlCode == BN_CLICKED ) {
					if ( !m_bInternalUpdate ) {
						std::vector<LPARAM> vUpdateMe;
						GetPagesToUpdate( vUpdateMe );
						auto pwThis = FindChild( _wId );
						bool bChecked = pwThis ? pwThis->IsChecked() : false;
						static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSeqCheckStates( _wId, bChecked, vUpdateMe );
						static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSeqCheckStates( Layout::LSN_WEWI_SEQ_STOP_EXACT_RADIO, !bChecked, vUpdateMe );
						static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSeqCheckStates( Layout::LSN_WEWI_SEQ_STOP_SNAP_RADIO, !bChecked, vUpdateMe );
					}
				}
				break;
			}
			case Layout::LSN_WEWI_SEQ_STOP_SNAP_RADIO : {
				if ( _wCtrlCode == BN_CLICKED ) {
					if ( !m_bInternalUpdate ) {
						std::vector<LPARAM> vUpdateMe;
						GetPagesToUpdate( vUpdateMe );
						auto pwThis = FindChild( _wId );
						bool bChecked = pwThis ? pwThis->IsChecked() : false;
						static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSeqCheckStates( _wId, bChecked, vUpdateMe );
						static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSeqCheckStates( Layout::LSN_WEWI_SEQ_STOP_EXACT_RADIO, !bChecked, vUpdateMe );
						static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSeqCheckStates( Layout::LSN_WEWI_SEQ_STOP_MINUS_ONE_RADIO, !bChecked, vUpdateMe );
					}
				}
				break;
			}

			case Layout::LSN_WEWI_SEQ_LOOP_RADIO : {
				if ( _wCtrlCode == BN_CLICKED ) {
					if ( !m_bInternalUpdate ) {
						std::vector<LPARAM> vUpdateMe;
						GetPagesToUpdate( vUpdateMe );
						auto pwThis = FindChild( _wId );
						bool bChecked = pwThis ? pwThis->IsChecked() : false;
						static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSeqCheckStates( _wId, bChecked, vUpdateMe );
						static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSeqCheckStates( Layout::LSN_WEWI_SEQ_ONE_SHOT_RADIO, !bChecked, vUpdateMe );
					}
				}
				break;
			}
			case Layout::LSN_WEWI_SEQ_ONE_SHOT_RADIO : {
				if ( _wCtrlCode == BN_CLICKED ) {
					if ( !m_bInternalUpdate ) {
						std::vector<LPARAM> vUpdateMe;
						GetPagesToUpdate( vUpdateMe );
						auto pwThis = FindChild( _wId );
						bool bChecked = pwThis ? pwThis->IsChecked() : false;
						static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSeqCheckStates( _wId, bChecked, vUpdateMe );
						static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSeqCheckStates( Layout::LSN_WEWI_SEQ_LOOP_RADIO, !bChecked, vUpdateMe );
					}
				}
				break;
			}
			
		}
		Update();
		return LSW_H_CONTINUE;
	}

	/**
	 * Verifies the values of a track.
	 * 
	 * \param _ui32Id The unique ID of the track.
	 * \param _tTrack The values to verify.
	 * \param _wsMsg The error message to display.
	 * \return Returns the control that failed or nullptr.
	 **/
	CWidget * CWavEditorSequencingPage::Verify( uint32_t _ui32Id, const LSN_TRACK &_tTrack, std::wstring &_wsMsg ) {
#define LSN_CHECKED( ID, STORE )		{ STORE = (*_tTrack.Check( Layout::ID )); }

		CWidget * pwWidget = nullptr;
#define LSN_CHECK_EDIT( ID, MSG )		{										\
	pwWidget = FindChild( Layout::ID );											\
	if ( !CWinUtilities::TextAsDoubleExpression( (*_tTrack.Text( Layout::ID )), eTest ) ) {	\
		_wsMsg = MSG;															\
		return pwWidget ? pwWidget : this;										\
	} }

		bool bChecked;
		double dStartTime = 0.0, /*dFullEndTime = 0.0, */dStopTime = 0.0, dFinalDur = 0.0;
		ee::CExpEvalContainer::EE_RESULT eTest;
		LSN_CHECK_EDIT( LSN_WEWI_SEQ_START_EDIT, LSN_LSTR( LSN_WE_OUTPUT_ERR_SEQ_START_TIME ) );
		if ( eTest.u.dVal < 0.0 ) {
			_wsMsg = LSN_LSTR( LSN_WE_OUTPUT_ERR_SEQ_START_TIME_NEG );
			return pwWidget;
		}
		dStartTime = eTest.u.dVal;
		/*LSN_CHECK_EDIT( LSN_WEWI_SEQ_END_EDIT, LSN_LSTR( LSN_WE_OUTPUT_ERR_SEQ_FULL_END_TIME ) );
		if ( eTest.u.dVal <= dStartTime ) {
			_wsMsg = LSN_LSTR( LSN_WE_OUTPUT_ERR_SEQ_FULL_END_TIME_LESS_THAN );
			return pwWidget;
		}
		dFullEndTime = eTest.u.dVal;
		double dMaxDur = dFullEndTime - dStartTime;*/


		LSN_CHECK_EDIT( LSN_WEWI_SEQ_LOOPS_STOP_EDIT, LSN_LSTR( LSN_WE_OUTPUT_ERR_SEQ_END_TIME ) );
		if ( eTest.u.dVal < 0.0 ) {
			_wsMsg = LSN_LSTR( LSN_WE_OUTPUT_ERR_SEQ_END_TIME_NEG );
			return pwWidget;
		}

		dStopTime = eTest.u.dVal;

		dFinalDur = dStopTime - dStartTime;
		LSN_CHECKED( LSN_WEWI_SEQ_LOOP_RADIO, bChecked );
		if ( bChecked ) {
			LSN_CHECK_EDIT( LSN_WEWI_SEQ_LOOPS_DELAY_EDIT, LSN_LSTR( LSN_WE_OUTPUT_ERR_SEQ_DEL_TIME ) );
			if ( eTest.u.dVal < 0.0 ) {
				_wsMsg = LSN_LSTR( LSN_WE_OUTPUT_ERR_SEQ_DEL_TIME_NEG );
				return pwWidget;
			}
			dFinalDur += eTest.u.dVal;

			LSN_CHECK_EDIT( LSN_WEWI_SEQ_LOOPS_FADE_EDIT, LSN_LSTR( LSN_WE_OUTPUT_ERR_SEQ_FAD_TIME ) );
			if ( eTest.u.dVal < 0.0 ) {
				_wsMsg = LSN_LSTR( LSN_WE_OUTPUT_ERR_SEQ_FAD_TIME_NEG );
				return pwWidget;
			}
			dFinalDur += eTest.u.dVal;
		}


		if ( m_pweEditor ) {
			auto pwfsSet = m_pweEditor->WavById( _ui32Id );
			if ( pwfsSet ) {
				double dMaxDur = double( pwfsSet->ui64FullSampleCnt ) / pwfsSet->wfFile.fcFormat.uiSampleRate;
				if ( dFinalDur - dMaxDur > DBL_EPSILON ) {
					pwWidget = FindChild( Layout::LSN_WEWI_SEQ_LOOPS_STOP_EDIT );
					_wsMsg = LSN_LSTR( LSN_WE_OUTPUT_ERR_SEQ_TOO_LONG );
					return pwWidget;
				}
			}
			
		}


		LSN_CHECK_EDIT( LSN_WEWI_SEQ_SILENCE_OPEN_SIL_EDIT, LSN_LSTR( LSN_WE_OUTPUT_ERR_SEQ_OPEN_TIME ) );
		if ( eTest.u.dVal < 0.0 ) {
			_wsMsg = LSN_LSTR( LSN_WE_OUTPUT_ERR_SEQ_OPEN_NEG );
			return pwWidget;
		}
		LSN_CHECK_EDIT( LSN_WEWI_SEQ_SILENCE_TRAIL_EDIT, LSN_LSTR( LSN_WE_OUTPUT_ERR_SEQ_TRAIL_TIME ) );
		if ( eTest.u.dVal < 0.0 ) {
			_wsMsg = LSN_LSTR( LSN_WE_OUTPUT_ERR_SEQ_TRAIL_NEG );
			return pwWidget;
		}

#undef LSN_CHECK_EDIT
#undef LSN_CHECKED
		return nullptr;
	}

	/**
	 * Saves the values of a track.
	 * 
	 * \param _tTrack The values to save.
	 * \param _pfPerFileOptions The object to which to save the window state.
	 * \param _ppfOutput The output object to which to transfer all the window settings.
	 */
	void CWavEditorSequencingPage::Save( const LSN_TRACK &_tTrack, LSN_WAV_EDITOR_WINDOW_OPTIONS::LSN_PER_FILE &_pfPerFileOptions, CWavEditor::LSN_PER_FILE * _ppfOutput ) {
#define LSN_CHECKED( ID, STORE )			{ STORE = (*_tTrack.Check( Layout::ID )); }

#define LSN_EDIT_TEXT( ID, STORE )			{ STORE = (*_tTrack.Text( Layout::ID )); }

#define LSN_EDIT_VAL( ID, STORE )			if ( _ppfOutput ) {					\
	if ( CWinUtilities::TextAsDoubleExpression( (*_tTrack.Text( Layout::ID )), eTest ) ) { STORE = eTest.u.dVal; } }

		// The items in the metadata combo boxes are in index order, so the selection index is the same as the item data.
#define LSN_COMBO_VAL( ID, STORE, TYPE )	{ STORE = static_cast<TYPE>((*_tTrack.Selection( Layout::ID ))); }

		ee::CExpEvalContainer::EE_RESULT eTest;
		bool bChecked;
		LSN_CHECKED( LSN_WEWI_SEQ_START_EXACT_RADIO, bChecked );
		if ( bChecked ) { _pfPerFileOptions.i32StartMod = LSN_SST_EXACT; }
		LSN_CHECKED( LSN_WEWI_SEQ_START_MINUS_ONE_RADIO, bChecked );
		if ( bChecked ) { _pfPerFileOptions.i32StartMod = LSN_SST_MINUS_ONE; }
		LSN_CHECKED( LSN_WEWI_SEQ_START_SNAP_RADIO, bChecked );
		if ( bChecked ) { _pfPerFileOptions.i32StartMod = LSN_SST_SNAP; }

		LSN_CHECKED( LSN_WEWI_SEQ_STOP_EXACT_RADIO, bChecked );
		if ( bChecked ) { _pfPerFileOptions.i32StopMod = LSN_SST_EXACT; }
		LSN_CHECKED( LSN_WEWI_SEQ_STOP_MINUS_ONE_RADIO, bChecked );
		if ( bChecked ) { _pfPerFileOptions.i32StopMod = LSN_SST_MINUS_ONE; }
		LSN_CHECKED( LSN_WEWI_SEQ_STOP_SNAP_RADIO, bChecked );
		if ( bChecked ) { _pfPerFileOptions.i32StopMod = LSN_SST_SNAP; }

		LSN_CHECKED( LSN_WEWI_SEQ_LOOP_RADIO, _pfPerFileOptions.bLoop );
		LSN_EDIT_TEXT( LSN_WEWI_SEQ_START_EDIT, _pfPerFileOptions.wsStartTime );
		LSN_COMBO_VAL( LSN_WEWI_SEQ_START_COMBO, _pfPerFileOptions.i32StartSelection, int32_t );
		LSN_EDIT_TEXT( LSN_WEWI_SEQ_LOOPS_STOP_EDIT, _pfPerFileOptions.wsStopTime );
		LSN_COMBO_VAL( LSN_WEWI_SEQ_LOOPS_STOP_COMBO, _pfPerFileOptions.i32EndSelection, int32_t );
		LSN_EDIT_TEXT( LSN_WEWI_SEQ_LOOPS_DELAY_EDIT, _pfPerFileOptions.wsPreFadeDur );
		LSN_EDIT_TEXT( LSN_WEWI_SEQ_LOOPS_FADE_EDIT, _pfPerFileOptions.wsFadeDur );

		LSN_EDIT_TEXT( LSN_WEWI_SEQ_SILENCE_OPEN_SIL_EDIT, _pfPerFileOptions.wsOpeningSilence );
		LSN_EDIT_TEXT( LSN_WEWI_SEQ_SILENCE_TRAIL_EDIT, _pfPerFileOptions.wsTrailingSilence );

		if ( _ppfOutput ) {
			LSN_EDIT_VAL( LSN_WEWI_SEQ_START_EDIT, _ppfOutput->dStartTime );
			LSN_CHECKED( LSN_WEWI_SEQ_START_EXACT_RADIO, bChecked );
			if ( bChecked ) { _ppfOutput->sstStartMod = LSN_SST_EXACT; }
			LSN_CHECKED( LSN_WEWI_SEQ_START_MINUS_ONE_RADIO, bChecked );
			if ( bChecked ) { _ppfOutput->sstStartMod = LSN_SST_MINUS_ONE; }
			LSN_CHECKED( LSN_WEWI_SEQ_START_SNAP_RADIO, bChecked );
			if ( bChecked ) { _ppfOutput->sstStartMod = LSN_SST_SNAP; }

			LSN_CHECKED( LSN_WEWI_SEQ_STOP_EXACT_RADIO, bChecked );
			if ( bChecked ) { _ppfOutput->sstStopMod = LSN_SST_EXACT; }
			LSN_CHECKED( LSN_WEWI_SEQ_STOP_MINUS_ONE_RADIO, bChecked );
			if ( bChecked ) { _ppfOutput->sstStopMod = LSN_SST_MINUS_ONE; }
			LSN_CHECKED( LSN_WEWI_SEQ_STOP_SNAP_RADIO, bChecked );
			if ( bChecked ) { _ppfOutput->sstStopMod = LSN_SST_SNAP; }

			//LSN_EDIT_VAL( LSN_WEWI_SEQ_END_EDIT, _ppfOutput->dEndTime );
			LSN_CHECKED( LSN_WEWI_SEQ_LOOP_RADIO, _ppfOutput->bLoop );
			LSN_EDIT_VAL( LSN_WEWI_SEQ_LOOPS_STOP_EDIT, _ppfOutput->dStopTime );
			if ( _ppfOutput->bLoop ) {
				LSN_EDIT_VAL( LSN_WEWI_SEQ_LOOPS_DELAY_EDIT, _ppfOutput->dDelayTime );
				LSN_EDIT_VAL( LSN_WEWI_SEQ_LOOPS_FADE_EDIT, _ppfOutput->dFadeTime );
			}
			LSN_EDIT_VAL( LSN_WEWI_SEQ_SILENCE_OPEN_SIL_EDIT, _ppfOutput->dOpeningSilence );
			LSN_EDIT_VAL( LSN_WEWI_SEQ_SILENCE_TRAIL_EDIT, _ppfOutput->dTrailingSilence );
		}

#undef LSN_COMBO_VAL
#undef LSN_EDIT_VAL
#undef LSN_EDIT_TEXT
#undef LSN_CHECKED
	}

	/**
	 * Fills out the values of a track with saved values from a project.
	 * 
	 * \param _pfPerFileOptions The per-file settings to load.
	 * \param _tTrack The track values to fill out.
	 **/
	void CWavEditorSequencingPage::Load( const LSN_WAV_EDITOR_WINDOW_OPTIONS::LSN_PER_FILE &_pfPerFileOptions, LSN_TRACK &_tTrack ) {
		_tTrack.wsStartTime = _pfPerFileOptions.wsStartTime;
		_tTrack.wsStopTime = _pfPerFileOptions.wsStopTime;
		_tTrack.wsPreFadeDur = _pfPerFileOptions.wsPreFadeDur;
		_tTrack.wsFadeDur = _pfPerFileOptions.wsFadeDur;
		_tTrack.wsOpeningSilence = _pfPerFileOptions.wsOpeningSilence;
		_tTrack.wsTrailingSilence = _pfPerFileOptions.wsTrailingSilence;

		// In the metadata combo boxes each item's data is its index.
		_tTrack.i32StartSelection = _pfPerFileOptions.i32StartSelection;
		_tTrack.i32StopSelection = _pfPerFileOptions.i32EndSelection;

		_tTrack.bStartExact = _pfPerFileOptions.i32StartMod == LSN_SST_EXACT;
		_tTrack.bStartMinusOne = _pfPerFileOptions.i32StartMod == LSN_SST_MINUS_ONE;
		_tTrack.bStartSnap = _pfPerFileOptions.i32StartMod == LSN_SST_SNAP;

		_tTrack.bStopExact = _pfPerFileOptions.i32StopMod == LSN_SST_EXACT;
		_tTrack.bStopMinusOne = _pfPerFileOptions.i32StopMod == LSN_SST_MINUS_ONE;
		_tTrack.bStopSnap = _pfPerFileOptions.i32StopMod == LSN_SST_SNAP;

		_tTrack.bLoop = _pfPerFileOptions.bLoop;
		_tTrack.bOneShot = !_pfPerFileOptions.bLoop;
	}

	/**
	 * Sets the values of a newly added track that depend on its WAV file (its start time comes from its metadata).
	 * 
	 * \param _ui32Id The unique ID of the track.
	 * \param _tTrack The track values to fill out.
	 **/
	void CWavEditorSequencingPage::InitTrack( uint32_t _ui32Id, LSN_TRACK &_tTrack ) {
		_tTrack.wsStartTime = L"0";
		_tTrack.wsStopTime = L"";
		_tTrack.i32StartSelection = -1;
		_tTrack.i32StopSelection = -1;

		auto pwfsSet = m_pweEditor ? m_pweEditor->WavById( _ui32Id ) : nullptr;
		if ( pwfsSet && pwfsSet->wsMetaPath.size() && pwfsSet->vMetadata.size() ) {
			_tTrack.i32StartSelection = 0;
			_tTrack.wsStartTime = std::format( L"{:.35g}", pwfsSet->vMetadata[0].dTime );
		}
	}

	/**
	 * Shows the values of a track in the controls.
	 * 
	 * \param _ui32Id The unique ID of the track.
	 * \param _tTrack The values to show.
	 **/
	void CWavEditorSequencingPage::LoadTrack( uint32_t _ui32Id, const LSN_TRACK &_tTrack ) {
		m_ui32Id = _ui32Id;
		m_bInternalUpdate = true;
		FillCombos();

		for ( auto I = std::size( m_wEditIds ); I--; ) {
			auto pwThis = FindChild( m_wEditIds[I] );
			if ( pwThis ) { pwThis->SetTextW( _tTrack.Text( m_wEditIds[I] )->c_str() ); }
		}
		for ( auto I = std::size( m_wComboIds ); I--; ) {
			auto pcbCombo = reinterpret_cast<CComboBox *>(FindChild( m_wComboIds[I] ));
			if ( pcbCombo ) {
				int32_t i32Sel = (*_tTrack.Selection( m_wComboIds[I] ));
				pcbCombo->SetCurSel( (i32Sel >= 0 && i32Sel < pcbCombo->GetCount()) ? i32Sel : -1 );
			}
		}
		for ( auto I = std::size( m_wCheckIds ); I--; ) {
			auto pwThis = FindChild( m_wCheckIds[I] );
			if ( pwThis ) { pwThis->SetCheck( (*_tTrack.Check( m_wCheckIds[I] )) ); }
		}

		m_bInternalUpdate = false;
		Update();
	}

	/**
	 * Copies the values in the controls to a track structure.
	 * 
	 * \param _tTrack Holds the values from the controls.
	 **/
	void CWavEditorSequencingPage::SaveTrack( LSN_TRACK &_tTrack ) {
		for ( auto I = std::size( m_wEditIds ); I--; ) {
			auto pwThis = FindChild( m_wEditIds[I] );
			if ( pwThis ) { (*_tTrack.Text( m_wEditIds[I] )) = pwThis->GetTextW(); }
		}
		for ( auto I = std::size( m_wComboIds ); I--; ) {
			auto pwThis = FindChild( m_wComboIds[I] );
			if ( pwThis ) { (*_tTrack.Selection( m_wComboIds[I] )) = pwThis->GetCurSel(); }
		}
		for ( auto I = std::size( m_wCheckIds ); I--; ) {
			auto pwThis = FindChild( m_wCheckIds[I] );
			if ( pwThis ) { (*_tTrack.Check( m_wCheckIds[I] )) = pwThis->IsChecked(); }
		}
	}

	/**
	 * Updates the dialog.
	 **/
	void CWavEditorSequencingPage::Update() {
		if ( !m_pweEditor ) { return; }
		bool bHasMeta = true;
		auto pwfsSet = m_pweEditor->WavById( m_ui32Id );
		if ( !pwfsSet || pwfsSet->wsMetaPath.size() == 0 ) {
			bHasMeta = false;
		}
		bool bLooping = false;
		auto pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_WEWI_SEQ_LOOP_RADIO ));
		if ( pcbCheck ) { bLooping = pcbCheck->IsChecked(); }

		bool bHasItems = false;
		bool bSelected = false;
		auto ptlTree = reinterpret_cast<CTreeListView *>(FindChild( Layout::LSN_WEWI_SEQ_OPS_TREELISTVEW ));
		if ( ptlTree ) {
			bHasItems = ptlTree->HasItem();
			std::vector<size_t> vSelected;
			ptlTree->GatherSelected( vSelected );
			bSelected = vSelected.size() != 0;
		}
		{
			struct LSN_CONTROLS {
				Layout::LSN_CONTROL_IDS									wId;
				bool													bCloseCondition0;
			} cControls[] = {
				{ Layout::LSN_WEWI_SEQ_START_COMBO,						bHasMeta },
				//{ Layout::LSN_WEWI_SEQ_END_COMBO,						bHasMeta },
				{ Layout::LSN_WEWI_SEQ_LOOPS_STOP_COMBO,				bHasMeta },

				//{ Layout::LSN_WEWI_SEQ_OPS_TREELISTVEW,					bHasItems },
				{ Layout::LSN_WEWI_SEQ_OPS_REMOVE_BUTTON,				bSelected },
				{ Layout::LSN_WEWI_SEQ_OPS_UP_BUTTON,					bSelected },
				{ Layout::LSN_WEWI_SEQ_OPS_DOWN_BUTTON,					bSelected },
			};
			for ( auto I = std::size( cControls ); I--; ) {
				auto pwThis = FindChild( cControls[I].wId );
				if ( pwThis ) {
					pwThis->SetEnabled( cControls[I].bCloseCondition0 );
				}
			}
		}
		{
			struct LSN_CONTROLS {
				Layout::LSN_CONTROL_IDS									wId;
				bool													bCloseCondition0;
			} cControls[] = {
				{ Layout::LSN_WEWI_SEQ_STOP_EXACT_RADIO,				!bLooping },
				{ Layout::LSN_WEWI_SEQ_STOP_MINUS_ONE_RADIO,			!bLooping },
				{ Layout::LSN_WEWI_SEQ_STOP_SNAP_RADIO,					!bLooping },

				{ Layout::LSN_WEWI_SEQ_LOOPS_DELAY_LABEL,				bLooping },
				{ Layout::LSN_WEWI_SEQ_LOOPS_DELAY_EDIT,				bLooping },
				{ Layout::LSN_WEWI_SEQ_LOOPS_DELAY_SECONDS_LABEL,		bLooping },
				{ Layout::LSN_WEWI_SEQ_LOOPS_FADE_LABEL,				bLooping },
				{ Layout::LSN_WEWI_SEQ_LOOPS_FADE_EDIT,					bLooping },
				{ Layout::LSN_WEWI_SEQ_LOOPS_FADE_SECONDS_LABEL,		bLooping },
			};
			for ( auto I = std::size( cControls ); I--; ) {
				auto pwThis = FindChild( cControls[I].wId );
				if ( pwThis ) {
					pwThis->SetVisible( cControls[I].bCloseCondition0 );
				}
			}
		}


		{
			// Hidden unless the values below show that the track is too short.
			auto pwWarning = FindChild( Layout::LSN_WEWI_SEQ_LOOPS_WARNING_LABEL );
			if ( pwWarning ) { pwWarning->SetVisible( false ); }
		}
		do {
			auto pwThis = FindChild( Layout::LSN_WEWI_SEQ_LOOPS_DESC_LABEL );
			if ( pwThis ) {
				if ( bLooping ) {
					ee::CExpEvalContainer::EE_RESULT rRes;
					double dStart = 0.0;
					double dEnd = 0.0;
					double dDelay = 0.0;
					double dFade = 0.0;
					auto pwStart = FindChild( Layout::LSN_WEWI_SEQ_START_EDIT );
					if ( pwStart ) {
						if ( pwStart->GetTextAsDoubleExpression( rRes ) ) {
							dStart = rRes.u.dVal;

							auto pwEnd = FindChild( Layout::LSN_WEWI_SEQ_LOOPS_STOP_EDIT );
							if ( pwEnd ) {
								if ( pwEnd->GetTextAsDoubleExpression( rRes ) ) {
									dEnd = rRes.u.dVal;

									auto pwDelay = FindChild( Layout::LSN_WEWI_SEQ_LOOPS_DELAY_EDIT );
									if ( pwDelay ) {
										if ( pwDelay->GetTextAsDoubleExpression( rRes ) ) {
											dDelay = rRes.u.dVal;

											auto pwFade = FindChild( Layout::LSN_WEWI_SEQ_LOOPS_FADE_EDIT );
											if ( pwFade ) {
												if ( pwFade->GetTextAsDoubleExpression( rRes ) && pwfsSet ) {
													dFade = rRes.u.dVal;

													dStart = std::max( dStart, 0.0 );
													dEnd = std::max( dStart, dEnd );
													dDelay = std::max( dDelay, 0.0 );
													dFade = std::max( dFade, 0.0 );
													double dTotalLen = (dEnd - dStart) + dDelay + dFade;
													dTotalLen = std::round( dTotalLen * pwfsSet->wfFile.fcFormat.uiSampleRate ) / pwfsSet->wfFile.fcFormat.uiSampleRate;
													pwThis->SetTextW( std::format( LSN_LSTR( LSN_WE_LOOP_DESC_1_TRACK ), dStart, dEnd - dStart, dDelay, dFade, dTotalLen ).c_str() );

													auto pwWarning = FindChild( Layout::LSN_WEWI_SEQ_LOOPS_WARNING_LABEL );
													if ( pwWarning && pwfsSet ) {
														double dFullEnd = double( pwfsSet->ui64FullSampleCnt ) / pwfsSet->wfFile.fcFormat.uiSampleRate;

														pwWarning->SetVisible( dTotalLen > (dFullEnd - dStart) );
													}
													break;
												}
											}
										}
									}
								}
							}
						}
					}
					pwThis->SetTextW( L"" );
					break;
				}
				else {
					if ( m_pweEditor && pwfsSet ) {
						ee::CExpEvalContainer::EE_RESULT rRes;
						double dStart = 0.0;
						double dEnd = 0.0;
						auto pwStart = FindChild( Layout::LSN_WEWI_SEQ_START_EDIT );
						if ( pwStart ) {
							if ( pwStart->GetTextAsDoubleExpression( rRes ) ) {
								dStart = rRes.u.dVal;

								auto pwEnd = FindChild( Layout::LSN_WEWI_SEQ_LOOPS_STOP_EDIT );
								if ( pwEnd ) {
									if ( pwEnd->GetTextAsDoubleExpression( rRes ) ) {
										dEnd = std::round( rRes.u.dVal * pwfsSet->wfFile.fcFormat.uiSampleRate ) / pwfsSet->wfFile.fcFormat.uiSampleRate;

										dStart = std::max( dStart, 0.0 );
										dEnd = std::max( dStart, dEnd );
										pwThis->SetTextW( std::format( LSN_LSTR( LSN_WE_ONE_SHOT_DESC_1_TRACK ), dStart, dEnd - dStart ).c_str() );

										auto pwWarning = FindChild( Layout::LSN_WEWI_SEQ_LOOPS_WARNING_LABEL );
										if ( pwWarning ) {
											double dFullEnd = double( pwfsSet->ui64FullSampleCnt ) / pwfsSet->wfFile.fcFormat.uiSampleRate;
											pwWarning->SetVisible( dEnd > dFullEnd );
										}
									}
								}
								break;
							}
						}
					}
					pwThis->SetTextW( L"" );
				}
			}
			break;
		} while ( false );

	}

	/**
	 * Shows the combined values of all the tracks being edited together (the selected tracks, or all tracks if none are selected).
	 **/
	void CWavEditorSequencingPage::Activate() {
		if ( !m_pweEditor ) { return; }
		m_ui32Id = 0;
		FillCombos();

		std::vector<LPARAM> vAffected;
		GetPagesToUpdate( vAffected );
		// With no tracks the controls keep their values, which are saved as the defaults for new tracks.
		if ( vAffected.size() ) {
			m_bInternalUpdate = true;
			auto pwewWindow = static_cast<CWavEditorWindow *>(m_pwParent);
			for ( auto I = std::size( m_wEditIds ); I--; ) {
				auto pwThis = FindChild( m_wEditIds[I] );
				if ( pwThis ) {
					pwThis->SetTextW( pwewWindow->GetAllSeqEditTexts( m_wEditIds[I], vAffected ).c_str() );
				}
			}
			for ( auto I = std::size( m_wCheckIds ); I--; ) {
				auto pwThis = FindChild( m_wCheckIds[I] );
				if ( pwThis ) {
					pwThis->SetCheck( pwewWindow->GetAllSeqCheckStates( m_wCheckIds[I], vAffected ) );
				}
			}
			m_bInternalUpdate = false;
		}
		Update();
	}

	/**
	 * Fills combo boxes with metadata or removes metadata from them.
	 **/
	void CWavEditorSequencingPage::FillCombos() {
		auto pwfsSet = m_pweEditor ? m_pweEditor->WavById( m_ui32Id ) : nullptr;
		
		if ( !pwfsSet || pwfsSet->wsMetaPath.size() == 0 ) {
			// Update() disables the combo boxes.  They keep their items in case the same track is shown again.
			for ( auto I = std::size( m_wComboIds ); I--; ) {
				auto pcbCombo = reinterpret_cast<CComboBox *>(FindChild( m_wComboIds[I] ));
				if ( pcbCombo ) {
					pcbCombo->SetCurSel( -1 );
				}
			}
			return;
		}
		// Filling them is slow with large metadata files, so only do it when they hold a different track's metadata.
		if ( m_ui32MetaId == m_ui32Id ) { return; }

		m_ui32MetaId = m_ui32Id;
		for ( auto I = std::size( m_wComboIds ); I--; ) {
			auto pcbCombo = reinterpret_cast<CComboBox *>(FindChild( m_wComboIds[I] ));
			if ( pcbCombo ) {
				if ( !FillComboWithMetadata( pcbCombo, pwfsSet->vMetadata ) ) {
					pcbCombo->ResetContent();
					m_ui32MetaId = 0;
				}
			}
		}
	}

	/**
	 * Fills a combo box with metadata.
	 * 
	 * \param _pwCombo The combo box to fill.
	 * \param _vMetadata The metadata to use to fill the combo box.
	 * \return Returns true if _pwCombo is a valid pointer to a combo box and _vMetadata is not empty.
	 **/
	bool CWavEditorSequencingPage::FillComboWithMetadata( CWidget * _pwCombo, const std::vector<CWavEditor::LSN_METADATA> &_vMetadata ) {
		if ( !_pwCombo ) { return false; }
		if ( !_vMetadata.size() ) { return false; }
		try {
			auto pcbCombo = static_cast<CComboBox *>(_pwCombo);
			HWND hWnd = pcbCombo->Wnd();
			if ( !hWnd ) { return false; }
			pcbCombo->ResetContent();

			// The combo box is owner-drawn without strings (CBS_OWNERDRAWFIXED, no CBS_HASSTRINGS): each item stores only its index into
			//	_vMetadata as its item data, and DrawItem() builds the text of the few items that are actually drawn.
			pcbCombo->InitStorage( _vMetadata.size(), 0 );
			for ( size_t I = 0; I < _vMetadata.size(); ++I ) {
				if ( ::SendMessageW( hWnd, CB_ADDSTRING, 0, LPARAM( I ) ) < 0 ) { return false; }	// CB_ERR or CB_ERRSPACE.
			}

			// Size the rows to the font (the fixed owner-draw height is otherwise based on the font the control had when it was created) and
			//	widen the list to fit the entry with the longest text.
			HDC hDc = ::GetDC( hWnd );
			if ( hDc ) {
				HFONT hFont = reinterpret_cast<HFONT>(::SendMessageW( hWnd, WM_GETFONT, 0, 0 ));
				HGDIOBJ hOldFont = hFont ? ::SelectObject( hDc, hFont ) : NULL;
				TEXTMETRICW tmMetrics;
				if ( ::GetTextMetricsW( hDc, &tmMetrics ) ) {
					::SendMessageW( hWnd, CB_SETITEMHEIGHT, 0, LPARAM( tmMetrics.tmHeight ) );
				}

				size_t sLongest = 0;
				for ( size_t I = 1; I < _vMetadata.size(); ++I ) {
					if ( _vMetadata[I].sText.size() > _vMetadata[sLongest].sText.size() ) { sLongest = I; }
				}
				std::string sText = MetadataText( _vMetadata, sLongest );
				SIZE sExtent;
				if ( ::GetTextExtentPoint32A( hDc, sText.c_str(), int( sText.size() ), &sExtent ) ) {
					LONG lWidth = sExtent.cx + 4 + ::GetSystemMetrics( SM_CXEDGE ) * 2;
					if ( pcbCombo->GetCount() > pcbCombo->GetMinVisible() ) {
						lWidth += ::GetSystemMetrics( SM_CXVSCROLL );
					}
					::SendMessageW( hWnd, CB_SETDROPPEDWIDTH, WPARAM( lWidth ), 0 );
				}

				if ( hOldFont ) { ::SelectObject( hDc, hOldFont ); }
				::ReleaseDC( hWnd, hDc );
			}
		}
		catch ( ... ) { return false; }
		return true;
	}

	/**
	 * Handles WM_DRAWITEM.  Draws the items of the owner-drawn metadata combo boxes from the WAV set's metadata.
	 *
	 * \param _wControlId The control identifier (DRAWITEMSTRUCT::CtlID).
	 * \param _disItem The DRAWITEMSTRUCT describing what to draw.
	 * \return Returns LSW_H_HANDLED if the item was drawn.
	 */
	CWidget::LSW_HANDLED CWavEditorSequencingPage::DrawItem( WORD _wControlId, const DRAWITEMSTRUCT &_disItem ) {
		if ( _disItem.CtlType != ODT_COMBOBOX ||
			(_wControlId != Layout::LSN_WEWI_SEQ_START_COMBO && _wControlId != Layout::LSN_WEWI_SEQ_LOOPS_STOP_COMBO) ) {
			return Parent::DrawItem( _wControlId, _disItem );
		}

		HDC hDc = _disItem.hDC;
		RECT rItem = _disItem.rcItem;
		bool bSelected = (_disItem.itemState & ODS_SELECTED) != 0;
		bool bDisabled = (_disItem.itemState & ODS_DISABLED) != 0;
		::FillRect( hDc, &rItem, ::GetSysColorBrush( bSelected ? COLOR_HIGHLIGHT : (bDisabled ? COLOR_BTNFACE : COLOR_WINDOW) ) );

		if ( _disItem.itemID != UINT( -1 ) && m_pweEditor ) {
			auto pwfsSet = m_pweEditor->WavById( m_ui32Id );
			size_t sIdx = size_t( _disItem.itemData );
			if ( pwfsSet && sIdx < pwfsSet->vMetadata.size() ) {
				try {
					std::string sText = MetadataText( pwfsSet->vMetadata, sIdx );
					COLORREF crOldColor = ::SetTextColor( hDc, ::GetSysColor( bDisabled ? COLOR_GRAYTEXT : (bSelected ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT) ) );
					int iOldMode = ::SetBkMode( hDc, TRANSPARENT );
					RECT rText = rItem;
					rText.left += 2;
					::DrawTextA( hDc, sText.c_str(), int( sText.size() ), &rText, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS );
					::SetBkMode( hDc, iOldMode );
					::SetTextColor( hDc, crOldColor );
				}
				catch ( ... ) {}
			}
		}

		if ( (_disItem.itemState & ODS_FOCUS) && !(_disItem.itemState & ODS_NOFOCUSRECT) ) {
			::DrawFocusRect( hDc, &rItem );
		}
		return LSW_H_HANDLED;
	}

	/**
	 * Gets the text shown for a metadata entry in the metadata combo boxes.
	 * 
	 * \param _vMetadata The metadata.
	 * \param _sIdx The index of the entry in _vMetadata.
	 * \return Returns the text to show for the entry.
	 **/
	std::string CWavEditorSequencingPage::MetadataText( const std::vector<CWavEditor::LSN_METADATA> &_vMetadata, size_t _sIdx ) {
		size_t sDigits = size_t( std::ceil( std::log10( _vMetadata.size() + 1 ) ) );
		return std::format( "{:0{}}: {:.30g} {}", _vMetadata[_sIdx].ui32Idx, sDigits, _vMetadata[_sIdx].dTime, _vMetadata[_sIdx].sText );
	}

	/**
	 * Gets an array of pages to update on text-editing for the 0th page.
	 * 
	 * \param _vPages Holds the returned array of pages to update.
	 **/
	void CWavEditorSequencingPage::GetPagesToUpdate( std::vector<LPARAM> &_vPages ) {
		_vPages.clear();
		if ( UniqueId() != 0 || !m_pwefpFiles ) {
			_vPages.push_back( UniqueId() );
			return;
		}
		auto ptlTree = reinterpret_cast<CTreeListView *>(m_pwefpFiles->FindChild( Layout::LSN_WEWI_FILES_TREELISTVIEW ));
		if ( ptlTree ) {
			ptlTree->GatherSelectedLParam( _vPages, true );
			// Remove extra WAVS and metadata.
			for ( auto I = _vPages.size(); I--; ) {
				if ( _vPages[I] & 0x80000000 ) {
					_vPages.erase( _vPages.begin() + I );
				}
			}

			// If the array is empty, we are updating all pages.
			if ( !_vPages.size() ) {
				ptlTree->GatherAllLParam( _vPages, true );
				// Remove extra WAVS and metadata.
				for ( auto I = _vPages.size(); I--; ) {
					if ( _vPages[I] & 0x80000000 ) {
						_vPages.erase( _vPages.begin() + I );
					}
				}
			}

			// Whatever is left should be updated, even if there is nothing left.
		}
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

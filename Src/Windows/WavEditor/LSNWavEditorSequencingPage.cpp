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

	CWavEditorSequencingPage::CWavEditorSequencingPage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pwewoOptions( reinterpret_cast<LSN_WAV_EDITOR_WINDOW_OPTIONS *>(_ui64Data) ) {
	}

	// == Functions.
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
		auto aTmp = FindChild( Layout::LSN_WEWI_SEQ_LOOPS_DELAY_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsPreFadeDur.c_str() ); }
		aTmp = FindChild( Layout::LSN_WEWI_SEQ_LOOPS_FADE_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsFadeDur.c_str() ); }

		aTmp = FindChild( Layout::LSN_WEWI_SEQ_SILENCE_OPEN_SIL_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsOpeningSilence.c_str() ); }
		aTmp = FindChild( Layout::LSN_WEWI_SEQ_SILENCE_TRAIL_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsTrailingSilence.c_str() ); }


		// Last checks.
		aTmp = FindChild( Layout::LSN_WEWI_SEQ_LOOP_RADIO );
		if ( aTmp ) { aTmp->SetCheck( m_pwewoOptions->bLoop ); }
		aTmp = FindChild( Layout::LSN_WEWI_SEQ_ONE_SHOT_RADIO );
		if ( aTmp ) { aTmp->SetCheck( !m_pwewoOptions->bLoop ); }


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
			case Layout::LSN_WEWI_SEQ_END_COMBO : {
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
			}
			case Layout::LSN_WEWI_SEQ_LOOPS_END_COMBO : {
				switch ( _wCtrlCode ) {
					case CBN_SELCHANGE : {
						auto pcbCombo = reinterpret_cast<CComboBox *>(FindChild( _wId ));
						if ( pcbCombo ) {
							auto aSelected = pcbCombo->GetCurSelItemData();
							if ( m_pweEditor ) {
								auto pwsSet = m_pweEditor->WavById( UniqueId() );
								if ( pwsSet && size_t( aSelected ) < pwsSet->vMetadata.size() ) {
									auto sText = std::format( "{:.35g}", pwsSet->vMetadata[aSelected].dTime );
									auto pwThis = FindChild( Layout::LSN_WEWI_SEQ_LOOPS_END_EDIT );
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
			case Layout::LSN_WEWI_SEQ_END_EDIT : {}
			case Layout::LSN_WEWI_SEQ_LOOPS_END_EDIT : {}			LSN_FALLTHROUGH
			case Layout::LSN_WEWI_SEQ_LOOPS_DELAY_EDIT : {}			LSN_FALLTHROUGH
			case Layout::LSN_WEWI_SEQ_LOOPS_FADE_EDIT : {}			LSN_FALLTHROUGH
			case Layout::LSN_WEWI_SEQ_SILENCE_OPEN_SIL_EDIT : {}	LSN_FALLTHROUGH
			case Layout::LSN_WEWI_SEQ_SILENCE_TRAIL_EDIT : {
				if ( _wCtrlCode == EN_CHANGE ) {
					if ( UniqueId() == 0 && !m_bInternalUpdate ) {
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
			case Layout::LSN_WEWI_SEQ_LOOP_RADIO : {
				if ( _wCtrlCode == BN_CLICKED ) {
					if ( UniqueId() == 0 && !m_bInternalUpdate ) {
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
					if ( UniqueId() == 0 && !m_bInternalUpdate ) {
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
	 * Verifies the inputs.
	 * 
	 * \param _wsMsg The error message to display.
	 * \return Returns the control that failed or nullptr.
	 **/
	CWidget * CWavEditorSequencingPage::Verify( std::wstring &_wsMsg ) {
#define LSN_CHECKED( ID, STORE )		{ STORE = false;						\
	auto wCheckTmp = FindChild( Layout::ID );									\
	if ( wCheckTmp ) { STORE = wCheckTmp->IsChecked(); } }

		CWidget * pwWidget = nullptr;
#define LSN_CHECK_EDIT( ID, MSG )		{										\
	pwWidget = FindChild( Layout::ID );											\
	if ( !pwWidget || !pwWidget->GetTextAsDoubleExpression( eTest ) ) {			\
		_wsMsg = MSG;															\
		return pwWidget ? pwWidget : this;										\
	} }

		bool bChecked;
		double dStartTime = 0.0, dFullEndTime = 0.0, dStopTime = 0.0, dFinalDur = 0.0;
		ee::CExpEvalContainer::EE_RESULT eTest;
		LSN_CHECK_EDIT( LSN_WEWI_SEQ_START_EDIT, LSN_LSTR( LSN_WE_OUTPUT_ERR_SEQ_START_TIME ) );
		if ( eTest.u.dVal < 0.0 ) {
			_wsMsg = LSN_LSTR( LSN_WE_OUTPUT_ERR_SEQ_START_TIME_NEG );
			return pwWidget;
		}
		dStartTime = eTest.u.dVal;
		LSN_CHECK_EDIT( LSN_WEWI_SEQ_END_EDIT, LSN_LSTR( LSN_WE_OUTPUT_ERR_SEQ_FULL_END_TIME ) );
		if ( eTest.u.dVal <= dStartTime ) {
			_wsMsg = LSN_LSTR( LSN_WE_OUTPUT_ERR_SEQ_FULL_END_TIME_LESS_THAN );
			return pwWidget;
		}
		dFullEndTime = eTest.u.dVal;
		double dMaxDur = dFullEndTime - dStartTime;


		LSN_CHECK_EDIT( LSN_WEWI_SEQ_LOOPS_END_EDIT, LSN_LSTR( LSN_WE_OUTPUT_ERR_SEQ_END_TIME ) );
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

		if ( dFinalDur - dMaxDur > DBL_EPSILON ) {
			pwWidget = FindChild( Layout::LSN_WEWI_SEQ_LOOPS_END_EDIT );
			_wsMsg = LSN_LSTR( LSN_WE_OUTPUT_ERR_SEQ_TOO_LONG );
			return pwWidget;
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
	 * Saves the current input configuration and closes the dialog.
	 * 
	 * \param _wewoOptions The object to which to save the window state.
	 * \param _ppfOutput The output object to which to transfer all the window settings.
	 */
	void CWavEditorSequencingPage::Save( LSN_WAV_EDITOR_WINDOW_OPTIONS &_wewoOptions, CWavEditor::LSN_PER_FILE * _ppfOutput ) {
#define LSN_CHECKED( ID, STORE )			{ STORE = false;					\
	auto wCheckTmp = FindChild( Layout::ID );									\
	if ( wCheckTmp ) { STORE = wCheckTmp->IsChecked(); } }

#define LSN_EDIT_TEXT( ID, STORE )			{									\
	pwWidget = FindChild( Layout::ID );											\
	if ( pwWidget ) { STORE = pwWidget->GetTextW(); } }

#define LSN_EDIT_VAL( ID, STORE )			if ( _ppfOutput ) {					\
	pwWidget = FindChild( Layout::ID );											\
	if ( pwWidget && pwWidget->GetTextAsDoubleExpression( eTest ) ) { STORE = eTest.u.dVal; } }

		CWidget * pwWidget = nullptr;
		ee::CExpEvalContainer::EE_RESULT eTest;
		LSN_CHECKED( LSN_WEWI_SEQ_LOOP_RADIO, _wewoOptions.bLoop );
		LSN_EDIT_TEXT( LSN_WEWI_SEQ_LOOPS_DELAY_EDIT, _wewoOptions.wsPreFadeDur );
		LSN_EDIT_TEXT( LSN_WEWI_SEQ_LOOPS_FADE_EDIT, _wewoOptions.wsFadeDur );

		LSN_EDIT_TEXT( LSN_WEWI_SEQ_SILENCE_OPEN_SIL_EDIT, _wewoOptions.wsOpeningSilence );
		LSN_EDIT_TEXT( LSN_WEWI_SEQ_SILENCE_TRAIL_EDIT, _wewoOptions.wsTrailingSilence );

		if ( _ppfOutput ) {
			LSN_EDIT_VAL( LSN_WEWI_SEQ_START_EDIT, _ppfOutput->dStartTime );
			LSN_EDIT_VAL( LSN_WEWI_SEQ_END_EDIT, _ppfOutput->dEndTime );
			LSN_CHECKED( LSN_WEWI_SEQ_LOOP_RADIO, _ppfOutput->bLoop );
			LSN_EDIT_VAL( LSN_WEWI_SEQ_LOOPS_END_EDIT, _ppfOutput->dStopTime );
			if ( _ppfOutput->bLoop ) {
				LSN_EDIT_VAL( LSN_WEWI_SEQ_LOOPS_DELAY_EDIT, _ppfOutput->dDelayTime );
				LSN_EDIT_VAL( LSN_WEWI_SEQ_LOOPS_FADE_EDIT, _ppfOutput->dFadeTime );
			}
			LSN_EDIT_VAL( LSN_WEWI_SEQ_SILENCE_OPEN_SIL_EDIT, _ppfOutput->dOpeningSilence );
			LSN_EDIT_VAL( LSN_WEWI_SEQ_SILENCE_TRAIL_EDIT, _ppfOutput->dTrailingSilence );
		}

#undef LSN_EDIT_VAL
#undef LSN_EDIT_TEXT
#undef LSN_CHECKED
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
		
		struct LSN_CONTROLS {
			Layout::LSN_WAV_EDITOR_WINDOW_IDS						wId;
			bool													bCloseCondition0;
		} cControls[] = {
			{ Layout::LSN_WEWI_SEQ_START_COMBO,						bHasMeta },
			{ Layout::LSN_WEWI_SEQ_END_COMBO,						bHasMeta },
			{ Layout::LSN_WEWI_SEQ_LOOPS_END_COMBO,					bHasMeta },

			{ Layout::LSN_WEWI_SEQ_LOOPS_DELAY_LABEL,				bLooping },
			{ Layout::LSN_WEWI_SEQ_LOOPS_DELAY_EDIT,				bLooping },
			{ Layout::LSN_WEWI_SEQ_LOOPS_DELAY_SECONDS_LABEL,		bLooping },
			{ Layout::LSN_WEWI_SEQ_LOOPS_FADE_LABEL,				bLooping },
			{ Layout::LSN_WEWI_SEQ_LOOPS_FADE_EDIT,					bLooping },
			{ Layout::LSN_WEWI_SEQ_LOOPS_FADE_SECONDS_LABEL,		bLooping },

			//{ Layout::LSN_WEWI_SEQ_OPS_TREELISTVEW,					bHasItems },
			{ Layout::LSN_WEWI_SEQ_OPS_REMOVE_BUTTON,				bSelected },
			{ Layout::LSN_WEWI_SEQ_OPS_UP_BUTTON,					bSelected },
			{ Layout::LSN_WEWI_SEQ_OPS_DOWN_BUTTON,					bSelected },
		};
		for ( auto I = LSN_ELEMENTS( cControls ); I--; ) {
			auto pwThis = FindChild( cControls[I].wId );
			if ( pwThis ) {
				pwThis->SetEnabled( cControls[I].bCloseCondition0 );
			}
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

							auto pwEnd = FindChild( Layout::LSN_WEWI_SEQ_LOOPS_END_EDIT );
							if ( pwEnd ) {
								if ( pwEnd->GetTextAsDoubleExpression( rRes ) ) {
									dEnd = rRes.u.dVal;

									auto pwDelay = FindChild( Layout::LSN_WEWI_SEQ_LOOPS_DELAY_EDIT );
									if ( pwDelay ) {
										if ( pwDelay->GetTextAsDoubleExpression( rRes ) ) {
											dDelay = rRes.u.dVal;

											auto pwFade = FindChild( Layout::LSN_WEWI_SEQ_LOOPS_FADE_EDIT );
											if ( pwFade ) {
												if ( pwFade->GetTextAsDoubleExpression( rRes ) ) {
													dFade = rRes.u.dVal;

													dStart = std::max( dStart, 0.0 );
													dEnd = std::max( dStart, dEnd );
													dDelay = std::max( dDelay, 0.0 );
													dFade = std::max( dFade, 0.0 );
													double dTotalLen = (dEnd - dStart) + dDelay + dFade;
													pwThis->SetTextW( std::format( LSN_LSTR( LSN_WE_LOOP_DESC_1_TRACK ), dStart, dEnd - dStart, dDelay, dFade, dTotalLen ).c_str() );

													auto pwWarning = FindChild( Layout::LSN_WEWI_SEQ_LOOPS_WARNING_LABEL );
													if ( pwWarning ) {
														double dFullEnd = 0.0;
														pwEnd = FindChild( Layout::LSN_WEWI_SEQ_END_EDIT );
														if ( pwEnd ) {
															if ( pwEnd->GetTextAsDoubleExpression( rRes ) ) {
																dFullEnd = rRes.u.dVal;

																pwWarning->SetVisible( dTotalLen > (dFullEnd - dStart) );
															}
														}
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
					ee::CExpEvalContainer::EE_RESULT rRes;
					double dStart = 0.0;
					double dEnd = 0.0;
					auto pwStart = FindChild( Layout::LSN_WEWI_SEQ_START_EDIT );
					if ( pwStart ) {
						if ( pwStart->GetTextAsDoubleExpression( rRes ) ) {
							dStart = rRes.u.dVal;

							auto pwEnd = FindChild( Layout::LSN_WEWI_SEQ_LOOPS_END_EDIT );
							if ( pwEnd ) {
								if ( pwEnd->GetTextAsDoubleExpression( rRes ) ) {
									dEnd = rRes.u.dVal;

									dStart = std::max( dStart, 0.0 );
									dEnd = std::max( dStart, dEnd );
									pwThis->SetTextW( std::format( LSN_LSTR( LSN_WE_ONE_SHOT_DESC_1_TRACK ), dStart, dEnd - dStart ).c_str() );

									auto pwWarning = FindChild( Layout::LSN_WEWI_SEQ_LOOPS_WARNING_LABEL );
									if ( pwWarning ) {
										double dFullEnd = 0.0;
										pwEnd = FindChild( Layout::LSN_WEWI_SEQ_END_EDIT );
										if ( pwEnd ) {
											if ( pwEnd->GetTextAsDoubleExpression( rRes ) ) {
												dFullEnd = rRes.u.dVal;

												pwWarning->SetVisible( dEnd > dFullEnd );
											}
										}
									}
									break;
								}
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
	 * Sets this page as active.  Allows the 0th page to gather text and checks from all the pages it affects.
	 **/
	void CWavEditorSequencingPage::Activate() {
		if ( !m_pweEditor ) { return; }
		if ( UniqueId() != 0 ) { return; }
		m_bInternalUpdate = true;
		std::vector<LPARAM> vAffected;
		GetPagesToUpdate( vAffected );
		auto pwThis = FindChild( Layout::LSN_WEWI_SEQ_START_EDIT );
		if ( pwThis ) {
			pwThis->SetTextW( static_cast<CWavEditorWindow *>(m_pwParent)->GetAllSeqEditTexts( Layout::LSN_WEWI_SEQ_START_EDIT, vAffected ).c_str() );
		}
		pwThis = FindChild( Layout::LSN_WEWI_SEQ_END_EDIT );
		if ( pwThis ) {
			pwThis->SetTextW( static_cast<CWavEditorWindow *>(m_pwParent)->GetAllSeqEditTexts( Layout::LSN_WEWI_SEQ_END_EDIT, vAffected ).c_str() );
		}
		pwThis = FindChild( Layout::LSN_WEWI_SEQ_LOOPS_END_EDIT );
		if ( pwThis ) {
			pwThis->SetTextW( static_cast<CWavEditorWindow *>(m_pwParent)->GetAllSeqEditTexts( Layout::LSN_WEWI_SEQ_LOOPS_END_EDIT, vAffected ).c_str() );
		}
		pwThis = FindChild( Layout::LSN_WEWI_SEQ_LOOPS_DELAY_EDIT );
		if ( pwThis ) {
			pwThis->SetTextW( static_cast<CWavEditorWindow *>(m_pwParent)->GetAllSeqEditTexts( Layout::LSN_WEWI_SEQ_LOOPS_DELAY_EDIT, vAffected ).c_str() );
		}
		pwThis = FindChild( Layout::LSN_WEWI_SEQ_LOOPS_FADE_EDIT );
		if ( pwThis ) {
			pwThis->SetTextW( static_cast<CWavEditorWindow *>(m_pwParent)->GetAllSeqEditTexts( Layout::LSN_WEWI_SEQ_LOOPS_FADE_EDIT, vAffected ).c_str() );
		}


		pwThis = FindChild( Layout::LSN_WEWI_SEQ_LOOP_RADIO );
		if ( pwThis ) {
			pwThis->SetCheck( static_cast<CWavEditorWindow *>(m_pwParent)->GetAllSeqCheckStates( Layout::LSN_WEWI_SEQ_LOOP_RADIO, vAffected ) );
		}
		pwThis = FindChild( Layout::LSN_WEWI_SEQ_ONE_SHOT_RADIO );
		if ( pwThis ) {
			pwThis->SetCheck( static_cast<CWavEditorWindow *>(m_pwParent)->GetAllSeqCheckStates( Layout::LSN_WEWI_SEQ_ONE_SHOT_RADIO, vAffected ) );
		}

		m_bInternalUpdate = false;
	}

	/**
	 * Fills combo boxes with metadata or removes metadata from them.
	 **/
	void CWavEditorSequencingPage::FillCombos() {
		auto pwfsSet = m_pweEditor ? m_pweEditor->WavById( m_ui32Id ) : nullptr;
		
		if ( !pwfsSet || pwfsSet->wsMetaPath.size() == 0 ) {
			auto pcbCombo = reinterpret_cast<CComboBox *>(FindChild( Layout::LSN_WEWI_SEQ_START_COMBO ));
			if ( pcbCombo ) {
				pcbCombo->ResetContent();
			}
			pcbCombo = reinterpret_cast<CComboBox *>(FindChild( Layout::LSN_WEWI_SEQ_END_COMBO ));
			if ( pcbCombo ) {
				pcbCombo->ResetContent();
			}
			pcbCombo = reinterpret_cast<CComboBox *>(FindChild( Layout::LSN_WEWI_SEQ_LOOPS_END_COMBO ));
			if ( pcbCombo ) {
				pcbCombo->ResetContent();
			}
		}
		if ( pwfsSet ) {
			auto pwThis = FindChild( Layout::LSN_WEWI_SEQ_START_EDIT );
			if ( pwThis ) {
				pwThis->SetTextW( L"0" );
			}
			pwThis = FindChild( Layout::LSN_WEWI_SEQ_END_EDIT );
			if ( pwThis ) {
				pwThis->SetTextW( std::format( L"{:.27g}", double( pwfsSet->wfFile.ui64Samples ) / pwfsSet->wfFile.fcFormat.uiSampleRate ).c_str() );
			}



			auto pcbCombo = reinterpret_cast<CComboBox *>(FindChild( Layout::LSN_WEWI_SEQ_START_COMBO ));
			if ( pcbCombo ) {
				if ( !FillComboWithMetadata( pcbCombo, pwfsSet->vMetadata ) ) {
					pcbCombo->ResetContent();
				}
				else {
					pcbCombo->SetCurSelByItemData( 0 );
					auto aSelected = pcbCombo->GetCurSelItemData();
					if ( aSelected != CB_ERR && size_t( aSelected ) < pwfsSet->vMetadata.size() ) {
						auto sText = std::format( "{:.35g}", pwfsSet->vMetadata[aSelected].dTime );
						pwThis = FindChild( Layout::LSN_WEWI_SEQ_START_EDIT );
						if ( pwThis ) {
							pwThis->SetTextA( sText.c_str() );
						}
					}
				}
			}
			pcbCombo = reinterpret_cast<CComboBox *>(FindChild( Layout::LSN_WEWI_SEQ_END_COMBO ));
			if ( pcbCombo ) {
				if ( !FillComboWithMetadata( pcbCombo, pwfsSet->vMetadata ) ) {
					pcbCombo->ResetContent();
				}
				else {
					pcbCombo->SetCurSelByItemData( pwfsSet->vMetadata.size() - 1 );
					/*auto aSelected = pcbCombo->GetCurSelItemData();
					if ( aSelected != CB_ERR && size_t( aSelected ) < pwfsSet->vMetadata.size() ) {
						auto sText = std::format( "{:.35g}", pwfsSet->vMetadata[aSelected].dTime );
						pwThis = FindChild( Layout::LSN_WEWI_SEQ_END_EDIT );
						if ( pwThis ) {
							pwThis->SetTextA( sText.c_str() );
						}
					}*/
				}
			}
			pcbCombo = reinterpret_cast<CComboBox *>(FindChild( Layout::LSN_WEWI_SEQ_LOOPS_END_COMBO ));
			if ( pcbCombo ) {
				if ( !FillComboWithMetadata( pcbCombo, pwfsSet->vMetadata ) ) {
					pcbCombo->ResetContent();
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
			size_t sDigits = size_t( std::ceil( std::log10( _vMetadata.size() + 1 ) ) );
			for ( size_t I = 0; I < _vMetadata.size(); ++I ) {
				std::ostringstream ossTmp;
				ossTmp << std::setw( sDigits ) << std::setfill( '0' ) << _vMetadata[I].ui32Idx;

				auto sTmp = std::format( "{}: {:.30g} {}", ossTmp.str(), _vMetadata[I].dTime, _vMetadata[I].sText );
				INT iIdx = pcbCombo->AddString( sTmp.c_str() );
				if ( iIdx != CB_ERR ) {
					pcbCombo->SetItemData( iIdx, I );
				}
			}
			pcbCombo->AutoSetMinListWidth();
		}
		catch ( ... ) { return false; }
		return true;
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

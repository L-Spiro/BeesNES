#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The page for file settings.
 */

#include "LSNWavEditorFileSettingsPage.h"
#include "../../Localization/LSNLocalization.h"
#include "../Layout/LSNLayoutMacros.h"
#include "../WinUtilities/LSNWinUtilities.h"
#include "LSNWavEditorWindow.h"

#include <CheckButton/LSWCheckButton.h>
#include <ComboBox/LSWComboBox.h>
#include <TreeListView/LSWTreeListView.h>

#include <commdlg.h>
#include <filesystem>

#include "../../../resource.h"


namespace lsn {

	CWavEditorFileSettingsPage::CWavEditorFileSettingsPage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pwewoOptions( reinterpret_cast<LSN_WAV_EDITOR_WINDOW_OPTIONS *>(_ui64Data) ) {
	}

	// == Functions.
	/**
	 * The WM_INITDIALOG handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CWavEditorFileSettingsPage::InitDialog() {
		Parent::InitDialog();
		m_bInternalUpdate = true;
		auto aTmp = FindChild( Layout::LSN_WEWI_FSETS_FDATA_HZ_COMBO );
		if ( aTmp ) { CWinUtilities::FillComboWithWavActialHz( aTmp, m_pwewoOptions->ui32ActualHz ); }
		aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_LPF_TYPE_COMBO );
		if ( aTmp ) { CWinUtilities::FillComboWithWavFilterType( aTmp, m_pwewoOptions->ui8LpfType ); }
		aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF0_TYPE_COMBO );
		if ( aTmp ) { CWinUtilities::FillComboWithWavFilterType( aTmp, m_pwewoOptions->ui8Hpf0Type ); }
		aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF1_TYPE_COMBO );
		if ( aTmp ) { CWinUtilities::FillComboWithWavFilterType( aTmp, m_pwewoOptions->ui8Hpf1Type ); }
		aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF2_TYPE_COMBO );
		if ( aTmp ) { CWinUtilities::FillComboWithWavFilterType( aTmp, m_pwewoOptions->ui8Hpf2Type ); }


		// Last texts.
		aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_VOL_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsCharVolume.c_str() ); }
		aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_LPF_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsCharLpfHz.c_str() ); }
		aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF0_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsCharHpf0Hz.c_str() ); }
		aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF1_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsCharHpf1Hz.c_str() ); }
		aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF2_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsCharHpf2Hz.c_str() ); }

		aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_LPF_FALLOFF_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsCharLpfFall.c_str() ); }
		aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF0_FALLOFF_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsCharHpf0Fall.c_str() ); }
		aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF1_FALLOFF_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsCharHpf1Fall.c_str() ); }
		aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF2_FALLOFF_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsCharHpf2Fall.c_str() ); }

		aTmp = FindChild( Layout::LSN_WEWI_FSETS_MDATA_ARTIST_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsArtist.c_str() ); }
		aTmp = FindChild( Layout::LSN_WEWI_FSETS_MDATA_ALBUM_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsAlbum.c_str() ); }
		aTmp = FindChild( Layout::LSN_WEWI_FSETS_MDATA_YEAR_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsYear.c_str() ); }
		aTmp = FindChild( Layout::LSN_WEWI_FSETS_MDATA_COMMENTS_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsComment.c_str() ); }


		// Last checks.
		aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_INV_CHECK );
		if ( aTmp ) { aTmp->SetCheck( m_pwewoOptions->bInvert ); }
		aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_LPF_CHECK );
		if ( aTmp ) { aTmp->SetCheck( m_pwewoOptions->bLpf ); }
		aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF0_CHECK );
		if ( aTmp ) { aTmp->SetCheck( m_pwewoOptions->bHpf0 ); }
		aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF1_CHECK );
		if ( aTmp ) { aTmp->SetCheck( m_pwewoOptions->bHpf1 ); }
		aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF2_CHECK );
		if ( aTmp ) { aTmp->SetCheck( m_pwewoOptions->bHpf2 ); }


		// Last combo selections.
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
	CWidget::LSW_HANDLED CWavEditorFileSettingsPage::Command( WORD _wCtrlCode, WORD _wId, CWidget * /*_pwSrc*/ ) {
		switch ( _wId ) {
			case Layout::LSN_WEWI_FSETS_APPLY_TO_ALL_BUTTON : {
				std::vector<LPARAM> vUpdateUs;
				GetPagesToUpdate( vUpdateUs, true );

				// Combos.
				static const WORD wComboIds[] = {
					Layout::LSN_WEWI_FSETS_CHAR_PRESET_COMBO,
					Layout::LSN_WEWI_FSETS_CHAR_LPF_TYPE_COMBO,
					Layout::LSN_WEWI_FSETS_CHAR_HPF0_TYPE_COMBO,
					Layout::LSN_WEWI_FSETS_CHAR_HPF1_TYPE_COMBO,
					Layout::LSN_WEWI_FSETS_CHAR_HPF2_TYPE_COMBO,
				};
				for ( auto I = LSN_ELEMENTS( wComboIds ); I--; ) {
					auto pwThis = FindChild( wComboIds[I] );
					if ( pwThis ) {
						static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSettingsComboSels( wComboIds[I], pwThis->GetCurSelItemData(), vUpdateUs );
					}
				}

				// Edits.
				static const WORD wTextIds[] = {
					Layout::LSN_WEWI_FSETS_CHAR_VOL_EDIT,
					Layout::LSN_WEWI_FSETS_CHAR_LPF_EDIT,
					Layout::LSN_WEWI_FSETS_CHAR_LPF_FALLOFF_EDIT,
					Layout::LSN_WEWI_FSETS_CHAR_HPF0_EDIT,
					Layout::LSN_WEWI_FSETS_CHAR_HPF0_FALLOFF_EDIT,
					Layout::LSN_WEWI_FSETS_CHAR_HPF1_EDIT,
					Layout::LSN_WEWI_FSETS_CHAR_HPF1_FALLOFF_EDIT,
					Layout::LSN_WEWI_FSETS_CHAR_HPF2_EDIT,
					Layout::LSN_WEWI_FSETS_CHAR_HPF2_FALLOFF_EDIT,
					Layout::LSN_WEWI_FSETS_MDATA_ARTIST_EDIT,
					Layout::LSN_WEWI_FSETS_MDATA_ALBUM_EDIT,
					Layout::LSN_WEWI_FSETS_MDATA_YEAR_EDIT,
					Layout::LSN_WEWI_FSETS_MDATA_COMMENTS_EDIT,
				};
				for ( auto I = LSN_ELEMENTS( wTextIds ); I--; ) {
					auto pwThis = FindChild( wTextIds[I] );
					if ( pwThis ) {
						static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSettingsEditTexts( wTextIds[I], pwThis->GetTextW(), vUpdateUs );
					}
				}

				// Checks.
				static const WORD wCheckIds[] = {
					Layout::LSN_WEWI_FSETS_CHAR_INV_CHECK,
					Layout::LSN_WEWI_FSETS_CHAR_LPF_CHECK,
					Layout::LSN_WEWI_FSETS_CHAR_HPF0_CHECK,
					Layout::LSN_WEWI_FSETS_CHAR_HPF1_CHECK,
					Layout::LSN_WEWI_FSETS_CHAR_HPF2_CHECK,
				};
				for ( auto I = LSN_ELEMENTS( wCheckIds ); I--; ) {
					auto pwThis = FindChild( wCheckIds[I] );
					if ( pwThis ) {
						static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSettingsCheckStates( wCheckIds[I], pwThis->IsChecked(), vUpdateUs );
					}
				}
				
				break;
			}
			case Layout::LSN_WEWI_FSETS_FDATA_HZ_COMBO : {}				LSN_FALLTHROUGH
			case Layout::LSN_WEWI_FSETS_CHAR_PRESET_COMBO : {}			LSN_FALLTHROUGH
			case Layout::LSN_WEWI_FSETS_CHAR_LPF_TYPE_COMBO : {}		LSN_FALLTHROUGH
			case Layout::LSN_WEWI_FSETS_CHAR_HPF0_TYPE_COMBO : {}		LSN_FALLTHROUGH
			case Layout::LSN_WEWI_FSETS_CHAR_HPF1_TYPE_COMBO : {}		LSN_FALLTHROUGH
			case Layout::LSN_WEWI_FSETS_CHAR_HPF2_TYPE_COMBO : {
				if ( _wCtrlCode == CBN_SELCHANGE ) {
					if ( UniqueId() == 0 && !m_bInternalUpdate ) {
						// Update all or update selected.
						std::vector<LPARAM> vUpdateMe;
						GetPagesToUpdate( vUpdateMe );
						auto pwThis = FindChild( _wId );
						auto lpParm = pwThis ? pwThis->GetCurSelItemData() : -50;
						static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSettingsComboSels( _wId, lpParm, vUpdateMe );
					}
				}
				break;
			}
			case Layout::LSN_WEWI_FSETS_FDATA_NAME_EDIT : {}			LSN_FALLTHROUGH
			case Layout::LSN_WEWI_FSETS_FDATA_HZ_EDIT : {}				LSN_FALLTHROUGH
			case Layout::LSN_WEWI_FSETS_CHAR_VOL_EDIT : {}				LSN_FALLTHROUGH
			case Layout::LSN_WEWI_FSETS_CHAR_LPF_EDIT : {}				LSN_FALLTHROUGH
			case Layout::LSN_WEWI_FSETS_CHAR_LPF_FALLOFF_EDIT : {}		LSN_FALLTHROUGH
			case Layout::LSN_WEWI_FSETS_CHAR_HPF0_EDIT : {}				LSN_FALLTHROUGH
			case Layout::LSN_WEWI_FSETS_CHAR_HPF0_FALLOFF_EDIT : {}		LSN_FALLTHROUGH
			case Layout::LSN_WEWI_FSETS_CHAR_HPF1_EDIT : {}				LSN_FALLTHROUGH
			case Layout::LSN_WEWI_FSETS_CHAR_HPF1_FALLOFF_EDIT : {}		LSN_FALLTHROUGH
			case Layout::LSN_WEWI_FSETS_CHAR_HPF2_EDIT : {}				LSN_FALLTHROUGH
			case Layout::LSN_WEWI_FSETS_CHAR_HPF2_FALLOFF_EDIT : {}		LSN_FALLTHROUGH
			case Layout::LSN_WEWI_FSETS_MDATA_ARTIST_EDIT : {}			LSN_FALLTHROUGH
			case Layout::LSN_WEWI_FSETS_MDATA_ALBUM_EDIT : {}			LSN_FALLTHROUGH
			case Layout::LSN_WEWI_FSETS_MDATA_YEAR_EDIT : {}			LSN_FALLTHROUGH
			case Layout::LSN_WEWI_FSETS_MDATA_COMMENTS_EDIT : {
				if ( _wCtrlCode == EN_CHANGE ) {
					if ( UniqueId() == 0 && !m_bInternalUpdate ) {
						// Update all or update selected.
						std::vector<LPARAM> vUpdateMe;
						GetPagesToUpdate( vUpdateMe );
						auto pwThis = FindChild( _wId );
						auto wsText = pwThis ? pwThis->GetTextW() : L"";
						static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSettingsEditTexts( _wId, wsText, vUpdateMe );
					}
				}
				break;
			}
			case Layout::LSN_WEWI_FSETS_CHAR_INV_CHECK : {}				LSN_FALLTHROUGH
			case Layout::LSN_WEWI_FSETS_CHAR_LPF_CHECK : {}				LSN_FALLTHROUGH
			case Layout::LSN_WEWI_FSETS_CHAR_HPF0_CHECK : {}			LSN_FALLTHROUGH
			case Layout::LSN_WEWI_FSETS_CHAR_HPF1_CHECK : {}			LSN_FALLTHROUGH
			case Layout::LSN_WEWI_FSETS_CHAR_HPF2_CHECK : {
				if ( _wCtrlCode == BN_CLICKED ) {
					if ( !m_bInternalUpdate ) {
						auto pwThis = FindChild( _wId );
						if ( pwThis ) {
							if ( pwThis->CheckState() == BST_INDETERMINATE ) {
								pwThis->SetCheck( false );
							}
						}
					}
					if ( UniqueId() == 0 && !m_bInternalUpdate ) {
						std::vector<LPARAM> vUpdateMe;
						GetPagesToUpdate( vUpdateMe );
						auto pwThis = FindChild( _wId );
						bool bChecked = pwThis ? pwThis->IsChecked() : false;
						static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSettingsCheckStates( _wId, bChecked, vUpdateMe );
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
	CWidget * CWavEditorFileSettingsPage::Verify( std::wstring &_wsMsg ) {
#define LSN_CHECKED( ID, STORE )		{ STORE = false;						\
	auto wCheckTmp = FindChild( Layout::ID );									\
	if ( wCheckTmp ) { STORE = wCheckTmp->IsChecked(); } }

		LPARAM lpParam = 0;
		bool bChecked = false;
		CWidget * pwWidget = nullptr;
		ee::CExpEvalContainer::EE_RESULT eTest;

#define LSN_CHECK_EDIT( ID, MSG )		{										\
	pwWidget = FindChild( Layout::ID );											\
	if ( !pwWidget || !pwWidget->GetTextAsDoubleExpression( eTest ) ) {			\
		_wsMsg = MSG;															\
		return pwWidget ? pwWidget : this;										\
	} }
#define LSN_CHECK_EDIT_EMPTY( ID, MSG )		{									\
	pwWidget = FindChild( Layout::ID );											\
	if ( !pwWidget || !pwWidget->GetTextW().size() ) {							\
		_wsMsg = MSG;															\
		return pwWidget ? pwWidget : this;										\
	} }

		
#define LSN_GET_COMBO( ID, STORE, BAD_ID ) { STORE = BAD_ID;					\
	pwWidget = FindChild( Layout::ID );											\
	if ( pwWidget ) {															\
		STORE = pwWidget->GetCurSelItemData();									\
	} }

		LSN_CHECK_EDIT_EMPTY( LSN_WEWI_FSETS_FDATA_NAME_EDIT, LSN_LSTR( LSN_WE_OUTPUT_ERR_SET_NO_NAME ) );

		LSN_GET_COMBO( LSN_WEWI_FSETS_FDATA_HZ_COMBO, lpParam, -80 );
		if ( lpParam == LPARAM( CWavEditor::LSN_AH_CUSTOM ) ) {
			LSN_CHECK_EDIT( LSN_WEWI_FSETS_FDATA_HZ_EDIT, LSN_LSTR( LSN_WE_OUTPUT_ERR_SET_BAD_SR ) );
			if ( eTest.u.dVal < 0.0 ) {
				_wsMsg = LSN_LSTR( LSN_WE_OUTPUT_ERR_SET_SR_NEG );
				return pwWidget;
			}
		}
		
		LSN_CHECK_EDIT( LSN_WEWI_FSETS_CHAR_VOL_EDIT, LSN_LSTR( LSN_WE_OUTPUT_ERR_SET_BAD_VOL ) );

		LSN_CHECKED( LSN_WEWI_FSETS_CHAR_LPF_CHECK, bChecked );
		if ( bChecked ) {
			LSN_CHECK_EDIT( LSN_WEWI_FSETS_CHAR_LPF_EDIT, LSN_LSTR( LSN_WE_OUTPUT_ERR_SET_BAD_HZ ) );
			if ( eTest.u.dVal < 0.0 ) {
				_wsMsg = LSN_LSTR( LSN_WE_OUTPUT_ERR_SET_HZ_NEG );
				return pwWidget;
			}
			LSN_GET_COMBO( LSN_WEWI_FSETS_CHAR_LPF_TYPE_COMBO, lpParam, -80 );
			if ( lpParam == LPARAM( CWavEditor::LSN_F_SINC ) ) {
				LSN_CHECK_EDIT( LSN_WEWI_FSETS_CHAR_LPF_FALLOFF_EDIT, LSN_LSTR( LSN_WE_OUTPUT_ERR_SET_FALLOFF_BAD ) );
				if ( eTest.u.dVal >= 0.0 ) {
					_wsMsg = LSN_LSTR( LSN_WE_OUTPUT_ERR_SET_FALLOFF_POS );
					return pwWidget;
				}
			}
		}
		LSN_CHECKED( LSN_WEWI_FSETS_CHAR_HPF0_CHECK, bChecked );
		if ( bChecked ) {
			LSN_CHECK_EDIT( LSN_WEWI_FSETS_CHAR_HPF0_EDIT, LSN_LSTR( LSN_WE_OUTPUT_ERR_SET_BAD_HZ ) );
			if ( eTest.u.dVal < 0.0 ) {
				_wsMsg = LSN_LSTR( LSN_WE_OUTPUT_ERR_SET_HZ_NEG );
				return pwWidget;
			}
			LSN_GET_COMBO( LSN_WEWI_FSETS_CHAR_HPF0_TYPE_COMBO, lpParam, -80 );
			if ( lpParam == LPARAM( CWavEditor::LSN_F_SINC ) ) {
				LSN_CHECK_EDIT( LSN_WEWI_FSETS_CHAR_HPF0_FALLOFF_EDIT, LSN_LSTR( LSN_WE_OUTPUT_ERR_SET_FALLOFF_BAD ) );
				if ( eTest.u.dVal >= 0.0 ) {
					_wsMsg = LSN_LSTR( LSN_WE_OUTPUT_ERR_SET_FALLOFF_POS );
					return pwWidget;
				}
			}
		}
		LSN_CHECKED( LSN_WEWI_FSETS_CHAR_HPF1_CHECK, bChecked );
		if ( bChecked ) {
			LSN_CHECK_EDIT( LSN_WEWI_FSETS_CHAR_HPF1_EDIT, LSN_LSTR( LSN_WE_OUTPUT_ERR_SET_BAD_HZ ) );
			if ( eTest.u.dVal < 0.0 ) {
				_wsMsg = LSN_LSTR( LSN_WE_OUTPUT_ERR_SET_HZ_NEG );
				return pwWidget;
			}
			LSN_GET_COMBO( LSN_WEWI_FSETS_CHAR_HPF1_TYPE_COMBO, lpParam, -80 );
			if ( lpParam == LPARAM( CWavEditor::LSN_F_SINC ) ) {
				LSN_CHECK_EDIT( LSN_WEWI_FSETS_CHAR_HPF1_FALLOFF_EDIT, LSN_LSTR( LSN_WE_OUTPUT_ERR_SET_FALLOFF_BAD ) );
				if ( eTest.u.dVal >= 0.0 ) {
					_wsMsg = LSN_LSTR( LSN_WE_OUTPUT_ERR_SET_FALLOFF_POS );
					return pwWidget;
				}
			}
		}
		LSN_CHECKED( LSN_WEWI_FSETS_CHAR_HPF2_CHECK, bChecked );
		if ( bChecked ) {
			LSN_CHECK_EDIT( LSN_WEWI_FSETS_CHAR_HPF2_EDIT, LSN_LSTR( LSN_WE_OUTPUT_ERR_SET_BAD_HZ ) );
			if ( eTest.u.dVal < 0.0 ) {
				_wsMsg = LSN_LSTR( LSN_WE_OUTPUT_ERR_SET_HZ_NEG );
				return pwWidget;
			}
			LSN_GET_COMBO( LSN_WEWI_FSETS_CHAR_HPF2_TYPE_COMBO, lpParam, -80 );
			if ( lpParam == LPARAM( CWavEditor::LSN_F_SINC ) ) {
				LSN_CHECK_EDIT( LSN_WEWI_FSETS_CHAR_HPF2_FALLOFF_EDIT, LSN_LSTR( LSN_WE_OUTPUT_ERR_SET_FALLOFF_BAD ) );
				if ( eTest.u.dVal >= 0.0 ) {
					_wsMsg = LSN_LSTR( LSN_WE_OUTPUT_ERR_SET_FALLOFF_POS );
					return pwWidget;
				}
			}
		}

#undef LSN_GET_COMBO
#undef LSN_CHECK_EDIT_EMPTY
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
	void CWavEditorFileSettingsPage::Save( LSN_WAV_EDITOR_WINDOW_OPTIONS &_wewoOptions, CWavEditor::LSN_PER_FILE * _ppfOutput ) {
#define LSN_CHECKED( ID, STORE )			{ STORE = false;					\
	auto wCheckTmp = FindChild( Layout::ID );									\
	if ( wCheckTmp ) { STORE = wCheckTmp->IsChecked(); } }

#define LSN_EDIT_TEXT( ID, STORE )			{									\
	pwWidget = FindChild( Layout::ID );											\
	if ( pwWidget ) { STORE = pwWidget->GetTextW(); } }

#define LSN_EDIT_VAL( ID, STORE )			if ( _ppfOutput ) {					\
	pwWidget = FindChild( Layout::ID );											\
	if ( pwWidget && pwWidget->GetTextAsDoubleExpression( eTest ) ) { STORE = eTest.u.dVal; } }

#define LSN_COMBO_VAL( ID, STORE, TYPE )	{									\
	pwWidget = FindChild( Layout::ID );											\
	if ( pwWidget ) { STORE = static_cast<TYPE>(pwWidget->GetCurSelItemData()); } }
	
		
		CWidget * pwWidget = nullptr;
		
		ee::CExpEvalContainer::EE_RESULT eTest;

		LSN_EDIT_TEXT( LSN_WEWI_FSETS_CHAR_VOL_EDIT, _wewoOptions.wsCharVolume );
		
		LSN_CHECKED( LSN_WEWI_FSETS_CHAR_INV_CHECK, _wewoOptions.bInvert );

		LSN_CHECKED( LSN_WEWI_FSETS_CHAR_LPF_CHECK, _wewoOptions.bLpf );
		LSN_CHECKED( LSN_WEWI_FSETS_CHAR_HPF0_CHECK, _wewoOptions.bHpf0 );
		LSN_CHECKED( LSN_WEWI_FSETS_CHAR_HPF1_CHECK, _wewoOptions.bHpf1 );
		LSN_CHECKED( LSN_WEWI_FSETS_CHAR_HPF2_CHECK, _wewoOptions.bHpf2 );

		LSN_EDIT_TEXT( LSN_WEWI_FSETS_CHAR_LPF_EDIT, _wewoOptions.wsCharLpfHz );
		LSN_COMBO_VAL( LSN_WEWI_FSETS_CHAR_LPF_TYPE_COMBO, _wewoOptions.ui8LpfType, uint8_t );
		LSN_EDIT_TEXT( LSN_WEWI_FSETS_CHAR_LPF_FALLOFF_EDIT, _wewoOptions.wsCharLpfFall );

		LSN_EDIT_TEXT( LSN_WEWI_FSETS_CHAR_HPF0_EDIT, _wewoOptions.wsCharHpf0Hz );
		LSN_COMBO_VAL( LSN_WEWI_FSETS_CHAR_HPF0_TYPE_COMBO, _wewoOptions.ui8Hpf0Type, uint8_t );
		LSN_EDIT_TEXT( LSN_WEWI_FSETS_CHAR_HPF0_FALLOFF_EDIT, _wewoOptions.wsCharHpf0Fall );

		LSN_EDIT_TEXT( LSN_WEWI_FSETS_CHAR_HPF1_EDIT, _wewoOptions.wsCharHpf1Hz );
		LSN_COMBO_VAL( LSN_WEWI_FSETS_CHAR_HPF1_TYPE_COMBO, _wewoOptions.ui8Hpf1Type, uint8_t );
		LSN_EDIT_TEXT( LSN_WEWI_FSETS_CHAR_HPF1_FALLOFF_EDIT, _wewoOptions.wsCharHpf1Fall );

		LSN_EDIT_TEXT( LSN_WEWI_FSETS_CHAR_HPF2_EDIT, _wewoOptions.wsCharHpf2Hz );
		LSN_COMBO_VAL( LSN_WEWI_FSETS_CHAR_HPF2_TYPE_COMBO, _wewoOptions.ui8Hpf2Type, uint8_t );
		LSN_EDIT_TEXT( LSN_WEWI_FSETS_CHAR_HPF2_FALLOFF_EDIT, _wewoOptions.wsCharHpf2Fall );

		LSN_EDIT_TEXT( LSN_WEWI_FSETS_MDATA_ARTIST_EDIT, _wewoOptions.wsArtist );
		LSN_EDIT_TEXT( LSN_WEWI_FSETS_MDATA_ALBUM_EDIT, _wewoOptions.wsAlbum );
		LSN_EDIT_TEXT( LSN_WEWI_FSETS_MDATA_YEAR_EDIT, _wewoOptions.wsYear );
		LSN_EDIT_TEXT( LSN_WEWI_FSETS_MDATA_COMMENTS_EDIT, _wewoOptions.wsComment );

		if ( _ppfOutput ) {
			LSN_EDIT_TEXT( LSN_WEWI_FSETS_FDATA_NAME_EDIT, _ppfOutput->wsName );
			LPARAM lpHz = CWavEditor::LSN_AH_CUSTOM;
			LSN_COMBO_VAL( LSN_WEWI_FSETS_FDATA_HZ_COMBO, lpHz, LPARAM );
			switch ( lpHz ) {
				case LPARAM( CWavEditor::LSN_AH_NTSC ) : {
					_ppfOutput->dActualHz = 1789772.7272727272727272727272727;
					break;
				}
				case LPARAM( CWavEditor::LSN_AH_PAL ) : {
					_ppfOutput->dActualHz = 1662607.03125;
					break;
				}
				case LPARAM( CWavEditor::LSN_AH_DENDY ) : {
					_ppfOutput->dActualHz = 1773447.5;
					break;
				}
				case LPARAM( CWavEditor::LSN_AH_PALM ) : {
					_ppfOutput->dActualHz = 1787805.9440559440559440559440559;
					break;
				}
				case LPARAM( CWavEditor::LSN_AH_PALN ) : {
					_ppfOutput->dActualHz = 1791028.125;
					break;
				}
				case LPARAM( CWavEditor::LSN_AH_BY_FILE ) : {
					auto wfsSet = m_pweEditor ? m_pweEditor->WavById( UniqueId() ) : nullptr;
					if ( wfsSet ) { _ppfOutput->dActualHz = double( wfsSet->wfFile.fcFormat.uiSampleRate ); }
					break;
				}
				default : {
					LSN_EDIT_VAL( LSN_WEWI_FSETS_FDATA_HZ_EDIT, _ppfOutput->dActualHz );
					break;
				}
			}


			LSN_EDIT_VAL( LSN_WEWI_FSETS_CHAR_VOL_EDIT, _ppfOutput->dVolume );
			LSN_CHECKED( LSN_WEWI_FSETS_CHAR_INV_CHECK, _ppfOutput->bInvert );
			LSN_CHECKED( LSN_WEWI_FSETS_CHAR_LPF_CHECK, _ppfOutput->bLpf );
			LSN_CHECKED( LSN_WEWI_FSETS_CHAR_HPF0_CHECK, _ppfOutput->bHpf0 );
			LSN_CHECKED( LSN_WEWI_FSETS_CHAR_HPF1_CHECK, _ppfOutput->bHpf1 );
			LSN_CHECKED( LSN_WEWI_FSETS_CHAR_HPF2_CHECK, _ppfOutput->bHpf2 );
			if ( _wewoOptions.bLpf ) {
				LSN_EDIT_VAL( LSN_WEWI_FSETS_CHAR_LPF_EDIT, _ppfOutput->dLpf );
				LSN_COMBO_VAL( LSN_WEWI_FSETS_CHAR_LPF_TYPE_COMBO, _ppfOutput->fLpfType, CWavEditor::LSN_FILTER );
				if ( _ppfOutput->fLpfType == CWavEditor::LSN_F_SINC ) {
					LSN_EDIT_VAL( LSN_WEWI_FSETS_CHAR_LPF_FALLOFF_EDIT, _ppfOutput->dFalloffLpf );
				}
			}
			if ( _wewoOptions.bHpf0 ) {
				LSN_EDIT_VAL( LSN_WEWI_FSETS_CHAR_HPF0_EDIT, _ppfOutput->dHpf0 );
				LSN_COMBO_VAL( LSN_WEWI_FSETS_CHAR_HPF0_TYPE_COMBO, _ppfOutput->fHpf0Type, CWavEditor::LSN_FILTER );
				if ( _ppfOutput->fHpf0Type == CWavEditor::LSN_F_SINC ) {
					LSN_EDIT_VAL( LSN_WEWI_FSETS_CHAR_HPF0_FALLOFF_EDIT, _ppfOutput->dFalloffHpf0 );
				}
			}
			if ( _wewoOptions.bHpf1 ) {
				LSN_EDIT_VAL( LSN_WEWI_FSETS_CHAR_HPF1_EDIT, _ppfOutput->dHpf1 );
				LSN_COMBO_VAL( LSN_WEWI_FSETS_CHAR_HPF1_TYPE_COMBO, _ppfOutput->fHpf1Type, CWavEditor::LSN_FILTER );
				if ( _ppfOutput->fHpf0Type == CWavEditor::LSN_F_SINC ) {
					LSN_EDIT_VAL( LSN_WEWI_FSETS_CHAR_HPF1_FALLOFF_EDIT, _ppfOutput->dFalloffHpf1 );
				}
			}
			if ( _wewoOptions.bHpf2 ) {
				LSN_EDIT_VAL( LSN_WEWI_FSETS_CHAR_HPF2_EDIT, _ppfOutput->dHpf2 );
				LSN_COMBO_VAL( LSN_WEWI_FSETS_CHAR_HPF2_TYPE_COMBO, _ppfOutput->fHpf2Type, CWavEditor::LSN_FILTER );
				if ( _ppfOutput->fHpf0Type == CWavEditor::LSN_F_SINC ) {
					LSN_EDIT_VAL( LSN_WEWI_FSETS_CHAR_HPF2_FALLOFF_EDIT, _ppfOutput->dFalloffHpf2 );
				}
			}

			LSN_EDIT_TEXT( LSN_WEWI_FSETS_MDATA_ARTIST_EDIT, _ppfOutput->wsArtist );
			LSN_EDIT_TEXT( LSN_WEWI_FSETS_MDATA_ALBUM_EDIT, _ppfOutput->wsAlbum );
			LSN_EDIT_TEXT( LSN_WEWI_FSETS_MDATA_YEAR_EDIT, _ppfOutput->wsYear );
			LSN_EDIT_TEXT( LSN_WEWI_FSETS_MDATA_COMMENTS_EDIT, _ppfOutput->wsComment );
		}


#undef LSN_EDIT_VAL
#undef LSN_EDIT_TEXT
#undef LSN_CHECKED
	}

	/**
	 * Updates the dialog.
	 **/
	void CWavEditorFileSettingsPage::Update() {
		bool bHasMeta = true;
		auto pwfsSet = m_pweEditor ? m_pweEditor->WavById( m_ui32Id ) : nullptr;
		if ( !pwfsSet || pwfsSet->wsMetaPath.size() == 0 ) {
			bHasMeta = false;
		}
		LPARAM lpHzData = 0;
		auto pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_WEWI_FSETS_FDATA_HZ_COMBO ));
		if ( pcbCombo ) { lpHzData = pcbCombo->GetCurSelItemData(); }

		LPARAM lpLpfPole = 0;
		pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_WEWI_FSETS_CHAR_LPF_TYPE_COMBO ));
		if ( pcbCombo ) { lpLpfPole = pcbCombo->GetCurSelItemData(); }
		LPARAM lpHpfPole0 = 0;
		pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF0_TYPE_COMBO ));
		if ( pcbCombo ) { lpHpfPole0 = pcbCombo->GetCurSelItemData(); }
		LPARAM lpHpfPole1 = 0;
		pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF1_TYPE_COMBO ));
		if ( pcbCombo ) { lpHpfPole1 = pcbCombo->GetCurSelItemData(); }
		LPARAM lpHpfPole2 = 0;
		pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF2_TYPE_COMBO ));
		if ( pcbCombo ) { lpHpfPole2 = pcbCombo->GetCurSelItemData(); }



		struct LSN_CONTROLS {
			Layout::LSN_WAV_EDITOR_WINDOW_IDS						wId;
			bool													bEnable;
		} cControls[] = {
			{ Layout::LSN_WEWI_FSETS_FDATA_HZ_EDIT,					lpHzData == LPARAM( CWavEditor::LSN_AH_CUSTOM ) },
			{ Layout::LSN_WEWI_FSETS_CHAR_LPF_FALLOFF_EDIT,			lpLpfPole == LPARAM( CWavEditor::LSN_F_SINC ) },
			{ Layout::LSN_WEWI_FSETS_CHAR_HPF0_FALLOFF_EDIT,		lpHpfPole0 == LPARAM( CWavEditor::LSN_F_SINC ) },
			{ Layout::LSN_WEWI_FSETS_CHAR_HPF1_FALLOFF_EDIT,		lpHpfPole1 == LPARAM( CWavEditor::LSN_F_SINC ) },
			{ Layout::LSN_WEWI_FSETS_CHAR_HPF2_FALLOFF_EDIT,		lpHpfPole2 == LPARAM( CWavEditor::LSN_F_SINC ) },
		};
		for ( auto I = LSN_ELEMENTS( cControls ); I--; ) {
			auto pwThis = FindChild( cControls[I].wId );
			if ( pwThis ) {
				pwThis->SetEnabled( cControls[I].bEnable );
			}
		}
	}

	/**
	 * Sets this page as active.  Allows the 0th page to gather text and checks from all the pages it affects.
	 **/
	void CWavEditorFileSettingsPage::Activate() {
		if ( !m_pweEditor ) { return; }
		if ( UniqueId() != 0 ) { return; }
		m_bInternalUpdate = true;

		std::vector<LPARAM> vAffected;
		GetPagesToUpdate( vAffected );

		auto pwThis = FindChild( Layout::LSN_WEWI_FSETS_FDATA_HZ_COMBO );
		if ( pwThis ) {
			pwThis->SetCurSelByItemData( static_cast<CWavEditorWindow *>(m_pwParent)->GetAllSettingsComboSel( Layout::LSN_WEWI_FSETS_FDATA_HZ_COMBO, vAffected, -80 ) );
		}
		pwThis = FindChild( Layout::LSN_WEWI_FSETS_CHAR_PRESET_COMBO );
		if ( pwThis ) {
			pwThis->SetCurSelByItemData( static_cast<CWavEditorWindow *>(m_pwParent)->GetAllSettingsComboSel( Layout::LSN_WEWI_FSETS_CHAR_PRESET_COMBO, vAffected, -80 ) );
		}
		pwThis = FindChild( Layout::LSN_WEWI_FSETS_CHAR_LPF_TYPE_COMBO );
		if ( pwThis ) {
			pwThis->SetCurSelByItemData( static_cast<CWavEditorWindow *>(m_pwParent)->GetAllSettingsComboSel( Layout::LSN_WEWI_FSETS_CHAR_LPF_TYPE_COMBO, vAffected, -1 ) );
		}
		pwThis = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF0_TYPE_COMBO );
		if ( pwThis ) {
			pwThis->SetCurSelByItemData( static_cast<CWavEditorWindow *>(m_pwParent)->GetAllSettingsComboSel( Layout::LSN_WEWI_FSETS_CHAR_HPF0_TYPE_COMBO, vAffected, -1 ) );
		}
		pwThis = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF1_TYPE_COMBO );
		if ( pwThis ) {
			pwThis->SetCurSelByItemData( static_cast<CWavEditorWindow *>(m_pwParent)->GetAllSettingsComboSel( Layout::LSN_WEWI_FSETS_CHAR_HPF1_TYPE_COMBO, vAffected, -1 ) );
		}
		pwThis = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF2_TYPE_COMBO );
		if ( pwThis ) {
			pwThis->SetCurSelByItemData( static_cast<CWavEditorWindow *>(m_pwParent)->GetAllSettingsComboSel( Layout::LSN_WEWI_FSETS_CHAR_HPF2_TYPE_COMBO, vAffected, -1 ) );
		}

		pwThis = FindChild( Layout::LSN_WEWI_FSETS_FDATA_NAME_EDIT );
		if ( pwThis ) {
			pwThis->SetTextW( static_cast<CWavEditorWindow *>(m_pwParent)->GetAllSettingsEditTexts( Layout::LSN_WEWI_FSETS_FDATA_NAME_EDIT, vAffected ).c_str() );
		}
		pwThis = FindChild( Layout::LSN_WEWI_FSETS_FDATA_HZ_EDIT );
		if ( pwThis ) {
			pwThis->SetTextW( static_cast<CWavEditorWindow *>(m_pwParent)->GetAllSettingsEditTexts( Layout::LSN_WEWI_FSETS_FDATA_HZ_EDIT, vAffected ).c_str() );
		}
		pwThis = FindChild( Layout::LSN_WEWI_FSETS_CHAR_VOL_EDIT );
		if ( pwThis ) {
			pwThis->SetTextW( static_cast<CWavEditorWindow *>(m_pwParent)->GetAllSettingsEditTexts( Layout::LSN_WEWI_FSETS_CHAR_VOL_EDIT, vAffected ).c_str() );
		}
		pwThis = FindChild( Layout::LSN_WEWI_FSETS_CHAR_LPF_EDIT );
		if ( pwThis ) {
			pwThis->SetTextW( static_cast<CWavEditorWindow *>(m_pwParent)->GetAllSettingsEditTexts( Layout::LSN_WEWI_FSETS_CHAR_LPF_EDIT, vAffected ).c_str() );
		}
		pwThis = FindChild( Layout::LSN_WEWI_FSETS_CHAR_LPF_FALLOFF_EDIT );
		if ( pwThis ) {
			pwThis->SetTextW( static_cast<CWavEditorWindow *>(m_pwParent)->GetAllSettingsEditTexts( Layout::LSN_WEWI_FSETS_CHAR_LPF_FALLOFF_EDIT, vAffected ).c_str() );
		}
		pwThis = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF0_EDIT );
		if ( pwThis ) {
			pwThis->SetTextW( static_cast<CWavEditorWindow *>(m_pwParent)->GetAllSettingsEditTexts( Layout::LSN_WEWI_FSETS_CHAR_HPF0_EDIT, vAffected ).c_str() );
		}
		pwThis = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF0_FALLOFF_EDIT );
		if ( pwThis ) {
			pwThis->SetTextW( static_cast<CWavEditorWindow *>(m_pwParent)->GetAllSettingsEditTexts( Layout::LSN_WEWI_FSETS_CHAR_HPF0_FALLOFF_EDIT, vAffected ).c_str() );
		}
		pwThis = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF1_EDIT );
		if ( pwThis ) {
			pwThis->SetTextW( static_cast<CWavEditorWindow *>(m_pwParent)->GetAllSettingsEditTexts( Layout::LSN_WEWI_FSETS_CHAR_HPF1_EDIT, vAffected ).c_str() );
		}
		pwThis = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF1_FALLOFF_EDIT );
		if ( pwThis ) {
			pwThis->SetTextW( static_cast<CWavEditorWindow *>(m_pwParent)->GetAllSettingsEditTexts( Layout::LSN_WEWI_FSETS_CHAR_HPF1_FALLOFF_EDIT, vAffected ).c_str() );
		}
		pwThis = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF2_EDIT );
		if ( pwThis ) {
			pwThis->SetTextW( static_cast<CWavEditorWindow *>(m_pwParent)->GetAllSettingsEditTexts( Layout::LSN_WEWI_FSETS_CHAR_HPF2_EDIT, vAffected ).c_str() );
		}
		pwThis = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF2_FALLOFF_EDIT );
		if ( pwThis ) {
			pwThis->SetTextW( static_cast<CWavEditorWindow *>(m_pwParent)->GetAllSettingsEditTexts( Layout::LSN_WEWI_FSETS_CHAR_HPF2_FALLOFF_EDIT, vAffected ).c_str() );
		}
		pwThis = FindChild( Layout::LSN_WEWI_FSETS_MDATA_ARTIST_EDIT );
		if ( pwThis ) {
			pwThis->SetTextW( static_cast<CWavEditorWindow *>(m_pwParent)->GetAllSettingsEditTexts( Layout::LSN_WEWI_FSETS_MDATA_ARTIST_EDIT, vAffected ).c_str() );
		}
		pwThis = FindChild( Layout::LSN_WEWI_FSETS_MDATA_ALBUM_EDIT );
		if ( pwThis ) {
			pwThis->SetTextW( static_cast<CWavEditorWindow *>(m_pwParent)->GetAllSettingsEditTexts( Layout::LSN_WEWI_FSETS_MDATA_ALBUM_EDIT, vAffected ).c_str() );
		}
		pwThis = FindChild( Layout::LSN_WEWI_FSETS_MDATA_YEAR_EDIT );
		if ( pwThis ) {
			pwThis->SetTextW( static_cast<CWavEditorWindow *>(m_pwParent)->GetAllSettingsEditTexts( Layout::LSN_WEWI_FSETS_MDATA_YEAR_EDIT, vAffected ).c_str() );
		}
		pwThis = FindChild( Layout::LSN_WEWI_FSETS_MDATA_COMMENTS_EDIT );
		if ( pwThis ) {
			pwThis->SetTextW( static_cast<CWavEditorWindow *>(m_pwParent)->GetAllSettingsEditTexts( Layout::LSN_WEWI_FSETS_MDATA_COMMENTS_EDIT, vAffected ).c_str() );
		}



		pwThis = FindChild( Layout::LSN_WEWI_FSETS_CHAR_INV_CHECK );
		if ( pwThis ) {
			pwThis->CheckButton( static_cast<CWavEditorWindow *>(m_pwParent)->GetAllSettingsCheckStates( Layout::LSN_WEWI_FSETS_CHAR_INV_CHECK, vAffected ) );
		}
		pwThis = FindChild( Layout::LSN_WEWI_FSETS_CHAR_LPF_CHECK );
		if ( pwThis ) {
			pwThis->CheckButton( static_cast<CWavEditorWindow *>(m_pwParent)->GetAllSettingsCheckStates( Layout::LSN_WEWI_FSETS_CHAR_LPF_CHECK, vAffected ) );
		}
		pwThis = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF0_CHECK );
		if ( pwThis ) {
			pwThis->CheckButton( static_cast<CWavEditorWindow *>(m_pwParent)->GetAllSettingsCheckStates( Layout::LSN_WEWI_FSETS_CHAR_HPF0_CHECK, vAffected ) );
		}
		pwThis = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF1_CHECK );
		if ( pwThis ) {
			pwThis->CheckButton( static_cast<CWavEditorWindow *>(m_pwParent)->GetAllSettingsCheckStates( Layout::LSN_WEWI_FSETS_CHAR_HPF1_CHECK, vAffected ) );
		}
		pwThis = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF2_CHECK );
		if ( pwThis ) {
			pwThis->CheckButton( static_cast<CWavEditorWindow *>(m_pwParent)->GetAllSettingsCheckStates( Layout::LSN_WEWI_FSETS_CHAR_HPF2_CHECK, vAffected ) );
		}

		m_bInternalUpdate = false;
	}

	/**
	 * Gets an array of pages to update on text-editing for the 0th page.
	 * 
	 * \param _vPages Holds the returned array of pages to update.
	 * \param _bUpdateAll If true, all pages are updated.
	 **/
	void CWavEditorFileSettingsPage::GetPagesToUpdate( std::vector<LPARAM> &_vPages, bool _bUpdateAll ) {
		_vPages.clear();
		if ( !_bUpdateAll ) {
			if ( UniqueId() != 0 || !m_pwefpFiles ) {
				_vPages.push_back( UniqueId() );
				return;
			}
		}
		auto ptlTree = reinterpret_cast<CTreeListView *>(m_pwefpFiles->FindChild( Layout::LSN_WEWI_FILES_TREELISTVIEW ));
		if ( ptlTree ) {
			if ( !_bUpdateAll ) {
				ptlTree->GatherSelectedLParam( _vPages, true );
				// Remove extra WAVS and metadata.
				for ( auto I = _vPages.size(); I--; ) {
					if ( _vPages[I] & 0x80000000 ) {
						_vPages.erase( _vPages.begin() + I );
					}
				}
			}

			// If the array is empty, we are updating all pages.
			if ( !_vPages.size() ) {
				ptlTree->GatherAllLParam( _vPages, true );
				// Remove extra WAVS and metadata.
				for ( auto I = _vPages.size(); I--; ) {
					if ( (_vPages[I] & 0x80000000) || _vPages[I] == UniqueId() ) {
						_vPages.erase( _vPages.begin() + I );
					}
				}
			}

			// Whatever is left should be updated, even if there is nothing left.
		}
	}

	/**
	 * Selects an initial Actual Hz value based off the Hz detected in the WAV file assigned to us.
	 **/
	void CWavEditorFileSettingsPage::InitialUpdate() {
		if ( !m_pweEditor ) { return; }
		if ( UniqueId() == 0 ) { return; }

		auto pwfsSet = m_pweEditor ? m_pweEditor->WavById( m_ui32Id ) : nullptr;
		if ( !pwfsSet ) { return; }

		m_bInternalUpdate = true;

		auto pwThis = FindChild( Layout::LSN_WEWI_FSETS_FDATA_HZ_COMBO );
		if ( pwThis ) {
			switch ( pwfsSet->wfFile.fcFormat.uiSampleRate ) {
				case 1789773 : {
					pwThis->SetCurSelByItemData( LPARAM( CWavEditor::LSN_AH_NTSC ) );
					break;
				}
				case 1662608 : {
					pwThis->SetCurSelByItemData( LPARAM( CWavEditor::LSN_AH_PAL ) );
					break;
				}
				case 1773448 : {
					pwThis->SetCurSelByItemData( LPARAM( CWavEditor::LSN_AH_DENDY ) );
					break;
				}
				case 1787806 : {
					pwThis->SetCurSelByItemData( LPARAM( CWavEditor::LSN_AH_PALM ) );
					break;
				}
				case 1791029 : {
					pwThis->SetCurSelByItemData( LPARAM( CWavEditor::LSN_AH_PALN ) );
					break;
				}
				default : {
					pwThis->SetCurSelByItemData( LPARAM( CWavEditor::LSN_AH_BY_FILE ) );
				}
			}
			
		}

		m_bInternalUpdate = false;
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

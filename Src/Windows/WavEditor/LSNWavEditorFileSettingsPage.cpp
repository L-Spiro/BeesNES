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

#undef LSN_GET_COMBO
#undef LSN_CHECK_EDIT_EMPTY
#undef LSN_CHECK_EDIT
#undef LSN_CHECKED
		return nullptr;
	}

	/**
	 * Saves the current input configuration and closes the dialog.
	 */
	void CWavEditorFileSettingsPage::Save() {
		
	}

	/**
	 * Updates the dialog.
	 **/
	void CWavEditorFileSettingsPage::Update() {
		bool bHasMeta = true;
		auto pwfsSet = m_pweEditor->WavById( m_ui32Id );
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
	 **/
	void CWavEditorFileSettingsPage::GetPagesToUpdate( std::vector<LPARAM> &_vPages ) {
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

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

	// == Members.
	/** The edit controls whose texts are stored per track. */
	const WORD CWavEditorFileSettingsPage::m_wEditIds[] = {
		Layout::LSN_WEWI_FSETS_FDATA_NAME_EDIT,
		Layout::LSN_WEWI_FSETS_FDATA_PREFIX_EDIT,
		Layout::LSN_WEWI_FSETS_FDATA_POSTFIX_EDIT,
		Layout::LSN_WEWI_FSETS_FDATA_HZ_EDIT,
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

	/** The combo boxes whose selections are stored per track. */
	const WORD CWavEditorFileSettingsPage::m_wComboIds[] = {
		Layout::LSN_WEWI_FSETS_FDATA_HZ_COMBO,
		Layout::LSN_WEWI_FSETS_CHAR_PRESET_COMBO,
		Layout::LSN_WEWI_FSETS_CHAR_LPF_TYPE_COMBO,
		Layout::LSN_WEWI_FSETS_CHAR_HPF0_TYPE_COMBO,
		Layout::LSN_WEWI_FSETS_CHAR_HPF1_TYPE_COMBO,
		Layout::LSN_WEWI_FSETS_CHAR_HPF2_TYPE_COMBO,
	};

	/** The check boxes whose check states are stored per track. */
	const WORD CWavEditorFileSettingsPage::m_wCheckIds[] = {
		Layout::LSN_WEWI_FSETS_CHAR_LOCK_CHECK,
		Layout::LSN_WEWI_FSETS_CHAR_INV_CHECK,
		Layout::LSN_WEWI_FSETS_CHAR_LPF_CHECK,
		Layout::LSN_WEWI_FSETS_CHAR_HPF0_CHECK,
		Layout::LSN_WEWI_FSETS_CHAR_HPF1_CHECK,
		Layout::LSN_WEWI_FSETS_CHAR_HPF2_CHECK,
		Layout::LSN_WEWI_FSETS_CHAR_RF_1_CHECK,
		Layout::LSN_WEWI_FSETS_CHAR_RF_2_CHECK,
		Layout::LSN_WEWI_FSETS_CHAR_SUNSOFT_5B_CHECK,
	};

	CWavEditorFileSettingsPage::CWavEditorFileSettingsPage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
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
	std::wstring * CWavEditorFileSettingsPage::LSN_TRACK::Text( WORD _wId ) {
		switch ( _wId ) {
			case Layout::LSN_WEWI_FSETS_FDATA_NAME_EDIT : { return &wsTitle; }
			case Layout::LSN_WEWI_FSETS_FDATA_PREFIX_EDIT : { return &wsTitlePreFix; }
			case Layout::LSN_WEWI_FSETS_FDATA_POSTFIX_EDIT : { return &wsTitlePostFix; }
			case Layout::LSN_WEWI_FSETS_FDATA_HZ_EDIT : { return &wsActualHz; }
			case Layout::LSN_WEWI_FSETS_CHAR_VOL_EDIT : { return &wsCharVolume; }
			case Layout::LSN_WEWI_FSETS_CHAR_LPF_EDIT : { return &wsCharLpfHz; }
			case Layout::LSN_WEWI_FSETS_CHAR_LPF_FALLOFF_EDIT : { return &wsCharLpfFall; }
			case Layout::LSN_WEWI_FSETS_CHAR_HPF0_EDIT : { return &wsCharHpf0Hz; }
			case Layout::LSN_WEWI_FSETS_CHAR_HPF0_FALLOFF_EDIT : { return &wsCharHpf0Fall; }
			case Layout::LSN_WEWI_FSETS_CHAR_HPF1_EDIT : { return &wsCharHpf1Hz; }
			case Layout::LSN_WEWI_FSETS_CHAR_HPF1_FALLOFF_EDIT : { return &wsCharHpf1Fall; }
			case Layout::LSN_WEWI_FSETS_CHAR_HPF2_EDIT : { return &wsCharHpf2Hz; }
			case Layout::LSN_WEWI_FSETS_CHAR_HPF2_FALLOFF_EDIT : { return &wsCharHpf2Fall; }
			case Layout::LSN_WEWI_FSETS_MDATA_ARTIST_EDIT : { return &wsArtist; }
			case Layout::LSN_WEWI_FSETS_MDATA_ALBUM_EDIT : { return &wsAlbum; }
			case Layout::LSN_WEWI_FSETS_MDATA_YEAR_EDIT : { return &wsYear; }
			case Layout::LSN_WEWI_FSETS_MDATA_COMMENTS_EDIT : { return &wsComment; }
		}
		return nullptr;
	}

	/**
	 * Gets the stored selection index of a combo box.
	 * 
	 * \param _wId The ID of the combo box.
	 * \return Returns a pointer to the stored selection index of the given combo box or nullptr if _wId is not a combo box on this page.
	 **/
	int32_t * CWavEditorFileSettingsPage::LSN_TRACK::Selection( WORD _wId ) {
		switch ( _wId ) {
			case Layout::LSN_WEWI_FSETS_FDATA_HZ_COMBO : { return &i32ActualHzSelection; }
			case Layout::LSN_WEWI_FSETS_CHAR_PRESET_COMBO : { return &i32CharPresetSelection; }
			case Layout::LSN_WEWI_FSETS_CHAR_LPF_TYPE_COMBO : { return &i32LpfTypeSelection; }
			case Layout::LSN_WEWI_FSETS_CHAR_HPF0_TYPE_COMBO : { return &i32Hpf0TypeSelection; }
			case Layout::LSN_WEWI_FSETS_CHAR_HPF1_TYPE_COMBO : { return &i32Hpf1TypeSelection; }
			case Layout::LSN_WEWI_FSETS_CHAR_HPF2_TYPE_COMBO : { return &i32Hpf2TypeSelection; }
		}
		return nullptr;
	}

	/**
	 * Gets the stored check state of a check box.
	 * 
	 * \param _wId The ID of the check box.
	 * \return Returns a pointer to the stored check state of the given check box or nullptr if _wId is not a check box on this page.
	 **/
	bool * CWavEditorFileSettingsPage::LSN_TRACK::Check( WORD _wId ) {
		switch ( _wId ) {
			case Layout::LSN_WEWI_FSETS_CHAR_LOCK_CHECK : { return &bLockVol; }
			case Layout::LSN_WEWI_FSETS_CHAR_INV_CHECK : { return &bInvert; }
			case Layout::LSN_WEWI_FSETS_CHAR_LPF_CHECK : { return &bLpf; }
			case Layout::LSN_WEWI_FSETS_CHAR_HPF0_CHECK : { return &bHpf0; }
			case Layout::LSN_WEWI_FSETS_CHAR_HPF1_CHECK : { return &bHpf1; }
			case Layout::LSN_WEWI_FSETS_CHAR_HPF2_CHECK : { return &bHpf2; }
			case Layout::LSN_WEWI_FSETS_CHAR_RF_1_CHECK : { return &bRf1; }
			case Layout::LSN_WEWI_FSETS_CHAR_RF_2_CHECK : { return &bRf2; }
			case Layout::LSN_WEWI_FSETS_CHAR_SUNSOFT_5B_CHECK : { return &bSunsoft5b; }
		}
		return nullptr;
	}

	/**
	 * The WM_INITDIALOG handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CWavEditorFileSettingsPage::InitDialog() {
		Parent::InitDialog();
		m_bInternalUpdate = true;

		auto aTmp = FindChild( Layout::LSN_WEWI_FSETS_FDATA_HZ_COMBO );
		if ( aTmp && m_pwewoOptions->vPerFileOptions.size() ) { CWinUtilities::FillComboWithWavActialHz( aTmp, m_pwewoOptions->vPerFileOptions[0].ui32ActualHz ); }

		/*aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_PRESET_COMBO );
		if ( aTmp ) { CWinUtilities::FillComboWithWavNoiseColor( aTmp, m_pwewoOptions->vPerFileOptions[0].ui32ActualHz ); }*/
		{
			auto pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_WEWI_FSETS_CHAR_PRESET_COMBO ));
			if ( pcbCombo ) {
				std::vector<CWinUtilities::LSN_COMBO_ENTRY> vPresets;
				for ( size_t I = 0; I < LSN_AUDIO_OPTIONS::PresetTotal(); ++I ) {
					CWinUtilities::LSN_COMBO_ENTRY ceTmp = { .pwcName = LSN_AUDIO_OPTIONS::s_apProfiles[I].wsName.c_str(), .lpParm = LPARAM( I ) };
					vPresets.push_back( ceTmp );
				}
				CWinUtilities::LSN_COMBO_ENTRY ceTmp = { .pwcName = LSN_LSTR( LSN_AUDIO_OPTIONS_CUSTOM ), .lpParm = LPARAM( -1 ) };
				vPresets.push_back( ceTmp );
				lsn::CWinUtilities::FillComboBox( pcbCombo, vPresets.data(), vPresets.size(), 0, -1 );
			}
		}

		if ( m_pwewoOptions->vPerFileOptions.size() ) {
			aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_LPF_TYPE_COMBO );
			if ( aTmp ) { CWinUtilities::FillComboWithWavFilterType( aTmp, m_pwewoOptions->vPerFileOptions[0].ui8LpfType ); }
			aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF0_TYPE_COMBO );
			if ( aTmp ) { CWinUtilities::FillComboWithWavFilterType( aTmp, m_pwewoOptions->vPerFileOptions[0].ui8Hpf0Type ); }
			aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF1_TYPE_COMBO );
			if ( aTmp ) { CWinUtilities::FillComboWithWavFilterType( aTmp, m_pwewoOptions->vPerFileOptions[0].ui8Hpf1Type ); }
			aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF2_TYPE_COMBO );
			if ( aTmp ) { CWinUtilities::FillComboWithWavFilterType( aTmp, m_pwewoOptions->vPerFileOptions[0].ui8Hpf2Type ); }


			// Last texts.
			//aTmp = FindChild( Layout::LSN_WEWI_FSETS_FDATA_NAME_EDIT );
			//if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->vPerFileOptions[0].wsTitle.c_str() ); }

			aTmp = FindChild( Layout::LSN_WEWI_FSETS_FDATA_PREFIX_EDIT );
			if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->vPerFileOptions[0].wsTitlePreFix.c_str() ); }
			aTmp = FindChild( Layout::LSN_WEWI_FSETS_FDATA_POSTFIX_EDIT );
			if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->vPerFileOptions[0].wsTitlePostFix.c_str() ); }
			

			aTmp = FindChild( Layout::LSN_WEWI_FSETS_FDATA_HZ_EDIT );
			if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->vPerFileOptions[0].wsActualHz.c_str() ); }
			aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_VOL_EDIT );
			if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->vPerFileOptions[0].wsCharVolume.c_str() ); }
			aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_LPF_EDIT );
			if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->vPerFileOptions[0].wsCharLpfHz.c_str() ); }
			aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF0_EDIT );
			if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->vPerFileOptions[0].wsCharHpf0Hz.c_str() ); }
			aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF1_EDIT );
			if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->vPerFileOptions[0].wsCharHpf1Hz.c_str() ); }
			aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF2_EDIT );
			if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->vPerFileOptions[0].wsCharHpf2Hz.c_str() ); }

			aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_LPF_FALLOFF_EDIT );
			if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->vPerFileOptions[0].wsCharLpfFall.c_str() ); }
			aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF0_FALLOFF_EDIT );
			if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->vPerFileOptions[0].wsCharHpf0Fall.c_str() ); }
			aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF1_FALLOFF_EDIT );
			if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->vPerFileOptions[0].wsCharHpf1Fall.c_str() ); }
			aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF2_FALLOFF_EDIT );
			if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->vPerFileOptions[0].wsCharHpf2Fall.c_str() ); }

			aTmp = FindChild( Layout::LSN_WEWI_FSETS_MDATA_ARTIST_EDIT );
			if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->vPerFileOptions[0].wsArtist.c_str() ); }
			aTmp = FindChild( Layout::LSN_WEWI_FSETS_MDATA_ALBUM_EDIT );
			if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->vPerFileOptions[0].wsAlbum.c_str() ); }
			aTmp = FindChild( Layout::LSN_WEWI_FSETS_MDATA_YEAR_EDIT );
			if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->vPerFileOptions[0].wsYear.c_str() ); }
			aTmp = FindChild( Layout::LSN_WEWI_FSETS_MDATA_COMMENTS_EDIT );
			if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->vPerFileOptions[0].wsComment.c_str() ); }


			// Last checks.
			aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_LOCK_CHECK );
			if ( aTmp ) { aTmp->SetCheck( m_pwewoOptions->vPerFileOptions[0].bLockVol ); }
			aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_INV_CHECK );
			if ( aTmp ) { aTmp->SetCheck( m_pwewoOptions->vPerFileOptions[0].bInvert ); }
			aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_LPF_CHECK );
			if ( aTmp ) { aTmp->SetCheck( m_pwewoOptions->vPerFileOptions[0].bLpf ); }
			aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF0_CHECK );
			if ( aTmp ) { aTmp->SetCheck( m_pwewoOptions->vPerFileOptions[0].bHpf0 ); }
			aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF1_CHECK );
			if ( aTmp ) { aTmp->SetCheck( m_pwewoOptions->vPerFileOptions[0].bHpf1 ); }
			aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF2_CHECK );
			if ( aTmp ) { aTmp->SetCheck( m_pwewoOptions->vPerFileOptions[0].bHpf2 ); }

			aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_RF_1_CHECK );
			if ( aTmp ) { aTmp->SetCheck( m_pwewoOptions->vPerFileOptions[0].bRf1 ); }
			aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_RF_2_CHECK );
			if ( aTmp ) { aTmp->SetCheck( m_pwewoOptions->vPerFileOptions[0].bRf2 ); }

			aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_SUNSOFT_5B_CHECK );
			if ( aTmp ) { aTmp->SetCheck( m_pwewoOptions->vPerFileOptions[0].bSunsoft5b ); }
		}

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
				for ( auto I = std::size( wComboIds ); I--; ) {
					auto pwThis = FindChild( wComboIds[I] );
					if ( pwThis ) {
						static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSettingsComboSels( wComboIds[I], pwThis->GetCurSel(), vUpdateUs );
					}
				}

				// Edits.
				static const WORD wTextIds[] = {
					Layout::LSN_WEWI_FSETS_FDATA_PREFIX_EDIT,
					Layout::LSN_WEWI_FSETS_FDATA_POSTFIX_EDIT,
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
				for ( auto I = std::size( wTextIds ); I--; ) {
					auto pwThis = FindChild( wTextIds[I] );
					if ( pwThis ) {
						static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSettingsEditTexts( wTextIds[I], pwThis->GetTextW(), vUpdateUs );
					}
				}

				// Checks.
				static const WORD wCheckIds[] = {
					Layout::LSN_WEWI_FSETS_CHAR_LOCK_CHECK,
					Layout::LSN_WEWI_FSETS_CHAR_INV_CHECK,
					Layout::LSN_WEWI_FSETS_CHAR_LPF_CHECK,
					Layout::LSN_WEWI_FSETS_CHAR_HPF0_CHECK,
					Layout::LSN_WEWI_FSETS_CHAR_HPF1_CHECK,
					Layout::LSN_WEWI_FSETS_CHAR_HPF2_CHECK,
					Layout::LSN_WEWI_FSETS_CHAR_RF_1_CHECK,
					Layout::LSN_WEWI_FSETS_CHAR_RF_2_CHECK,
					Layout::LSN_WEWI_FSETS_CHAR_SUNSOFT_5B_CHECK,
				};
				for ( auto I = std::size( wCheckIds ); I--; ) {
					auto pwThis = FindChild( wCheckIds[I] );
					if ( pwThis ) {
						static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSettingsCheckStates( wCheckIds[I], pwThis->IsChecked(), vUpdateUs );
					}
				}
				
				break;
			}
			case Layout::LSN_WEWI_FSETS_FDATA_HZ_COMBO : {}				LSN_FALLTHROUGH
			//case Layout::LSN_WEWI_FSETS_CHAR_PRESET_COMBO : {}			LSN_FALLTHROUGH
			case Layout::LSN_WEWI_FSETS_CHAR_LPF_TYPE_COMBO : {}		LSN_FALLTHROUGH
			case Layout::LSN_WEWI_FSETS_CHAR_HPF0_TYPE_COMBO : {}		LSN_FALLTHROUGH
			case Layout::LSN_WEWI_FSETS_CHAR_HPF1_TYPE_COMBO : {}		LSN_FALLTHROUGH
			case Layout::LSN_WEWI_FSETS_CHAR_HPF2_TYPE_COMBO : {
				if ( _wCtrlCode == CBN_SELCHANGE ) {
					if ( !m_bInternalUpdate ) {
						// Update all or update selected.
						std::vector<LPARAM> vUpdateMe;
						GetPagesToUpdate( vUpdateMe );
						auto pwThis = FindChild( _wId );
						auto iSel = pwThis ? pwThis->GetCurSel() : -1;
						static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSettingsComboSels( _wId, iSel, vUpdateMe );
					}
				}
				break;
			}
			case Layout::LSN_WEWI_FSETS_FDATA_NAME_EDIT : {}			LSN_FALLTHROUGH
			case Layout::LSN_WEWI_FSETS_FDATA_PREFIX_EDIT : {}			LSN_FALLTHROUGH
			case Layout::LSN_WEWI_FSETS_FDATA_POSTFIX_EDIT : {}			LSN_FALLTHROUGH
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
					if ( !m_bInternalUpdate ) {
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
			case Layout::LSN_WEWI_FSETS_CHAR_LOCK_CHECK : {}			LSN_FALLTHROUGH
			case Layout::LSN_WEWI_FSETS_CHAR_INV_CHECK : {}				LSN_FALLTHROUGH
			case Layout::LSN_WEWI_FSETS_CHAR_LPF_CHECK : {}				LSN_FALLTHROUGH
			case Layout::LSN_WEWI_FSETS_CHAR_HPF0_CHECK : {}			LSN_FALLTHROUGH
			case Layout::LSN_WEWI_FSETS_CHAR_HPF1_CHECK : {}			LSN_FALLTHROUGH
			case Layout::LSN_WEWI_FSETS_CHAR_HPF2_CHECK : {}
			case Layout::LSN_WEWI_FSETS_CHAR_RF_1_CHECK : {}
			case Layout::LSN_WEWI_FSETS_CHAR_RF_2_CHECK : {}
			case Layout::LSN_WEWI_FSETS_CHAR_SUNSOFT_5B_CHECK : {
				if ( _wCtrlCode == BN_CLICKED ) {
					if ( !m_bInternalUpdate ) {
						auto pwThis = FindChild( _wId );
						if ( pwThis ) {
							if ( pwThis->CheckState() == BST_INDETERMINATE ) {
								pwThis->SetCheck( false );
							}
						}
					}
					if ( !m_bInternalUpdate ) {
						std::vector<LPARAM> vUpdateMe;
						GetPagesToUpdate( vUpdateMe );
						auto pwThis = FindChild( _wId );
						bool bChecked = pwThis ? pwThis->IsChecked() : false;
						static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSettingsCheckStates( _wId, bChecked, vUpdateMe );
					}
				}
				break;
			}


			case Layout::LSN_WEWI_FSETS_CHAR_PRESET_COMBO : {
				if ( _wCtrlCode == CBN_SELCHANGE ) {
					if ( !m_bInternalUpdate ) {
						// Update all or update selected.
						std::vector<LPARAM> vUpdateMe;
						GetPagesToUpdate( vUpdateMe );
						auto pwThis = FindChild( _wId );
						auto iSel = pwThis ? pwThis->GetCurSel() : -1;
						static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSettingsComboSels( _wId, iSel, vUpdateMe );
					}
				}
				else if ( _wCtrlCode == CBN_SELENDOK ) {
					auto pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_WEWI_FSETS_CHAR_PRESET_COMBO ));
					if ( pcbCombo ) {
						auto lpSel = pcbCombo->GetCurSelItemData();
						if ( lpSel != -1 && size_t( lpSel ) < LSN_AUDIO_OPTIONS::PresetTotal() ) {
							auto aEdit = FindChild( Layout::LSN_WEWI_FSETS_CHAR_LPF_EDIT );
							//if ( aEdit ) { aEdit->SetTextA( std::format( "{:.27}", LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].fLpf ).c_str() ); }
							if ( aEdit ) {
								if ( LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].fZero != 0.0f ) {
									aEdit->SetTextA( std::format( "{{{:.27},{:.27}}}", LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].fLpf, LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].fZero ).c_str() );
								}
								else {
									aEdit->SetTextA( std::format( "{:.27}", LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].fLpf ).c_str() );
								}
							}

							aEdit = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF0_EDIT );
							if ( aEdit ) { aEdit->SetTextA( std::format( "{:.27}", LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].fHpf0 ).c_str() ); }

							aEdit = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF1_EDIT );
							if ( aEdit ) { aEdit->SetTextA( std::format( "{:.27}", LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].fHpf1 ).c_str() ); }

							aEdit = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF2_EDIT );
							if ( aEdit ) { aEdit->SetTextA( std::format( "{:.27}", LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].fHpf2 ).c_str() ); }

							bool bLockVolume = false;
							auto pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_WEWI_FSETS_CHAR_LOCK_CHECK ));
							if ( pcbCheck ) { bLockVolume = pcbCheck->IsChecked(); }
							if ( !bLockVolume ) {
								aEdit = FindChild( Layout::LSN_WEWI_FSETS_CHAR_VOL_EDIT );
								if ( aEdit ) { aEdit->SetTextA( std::format( "{:.27}", LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].fVolume ).c_str() ); }
							}

							pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_WEWI_FSETS_CHAR_LPF_CHECK ));
							if ( pcbCheck ) {
								pcbCheck->SetCheck( LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].bLpfEnable );
							}
							pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF0_CHECK ));
							if ( pcbCheck ) {
								pcbCheck->SetCheck( LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].bHpf0Enable );
							}
							pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF1_CHECK ));
							if ( pcbCheck ) {
								pcbCheck->SetCheck( LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].bHpf1Enable );
							}
							pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF2_CHECK ));
							if ( pcbCheck ) {
								pcbCheck->SetCheck( LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].bHpf2Enable );
							}

							pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_WEWI_FSETS_CHAR_RF_1_CHECK ));
							if ( pcbCheck ) {
								pcbCheck->SetCheck( LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].fmFilterMode == LSN_FM_RF_1 );
							}
							pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_WEWI_FSETS_CHAR_RF_2_CHECK ));
							if ( pcbCheck ) {
								pcbCheck->SetCheck( LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].fmFilterMode == LSN_FM_RF_2 );
							}
							pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_WEWI_FSETS_CHAR_SUNSOFT_5B_CHECK ));
							if ( pcbCheck ) {
								pcbCheck->SetCheck( LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].bSunsoft5b );
							}

							pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_WEWI_FSETS_CHAR_INV_CHECK ));
							if ( pcbCheck ) {
								pcbCheck->SetCheck( LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].bInvert );
							}
							std::vector<LPARAM> vUpdateUs;
							GetPagesToUpdate( vUpdateUs );

							// Checks.
							const WORD wCheckIds[] = {
								Layout::LSN_WEWI_FSETS_CHAR_LOCK_CHECK,			WORD( bLockVolume ),
								Layout::LSN_WEWI_FSETS_CHAR_INV_CHECK,			WORD( LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].bInvert ),
								Layout::LSN_WEWI_FSETS_CHAR_LPF_CHECK,			WORD( LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].bLpfEnable ),
								Layout::LSN_WEWI_FSETS_CHAR_HPF0_CHECK,			WORD( LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].bHpf0Enable ),
								Layout::LSN_WEWI_FSETS_CHAR_HPF1_CHECK,			WORD( LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].bHpf1Enable ),
								Layout::LSN_WEWI_FSETS_CHAR_HPF2_CHECK,			WORD( LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].bHpf2Enable ),
								Layout::LSN_WEWI_FSETS_CHAR_RF_1_CHECK,			WORD( LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].fmFilterMode == LSN_FM_RF_1 ),
								Layout::LSN_WEWI_FSETS_CHAR_RF_2_CHECK,			WORD( LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].fmFilterMode == LSN_FM_RF_2 ),
								Layout::LSN_WEWI_FSETS_CHAR_SUNSOFT_5B_CHECK,	WORD( LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].bSunsoft5b ),
							};
							for ( size_t I = 0; I < std::size( wCheckIds ); I += 2 ) {
								static_cast<CWavEditorWindow *>(m_pwParent)->SetAllSettingsCheckStates( wCheckIds[I], bool( wCheckIds[I+1] ), vUpdateUs );
							}
						}
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
	 * \param _tTrack The values to verify.
	 * \param _wsMsg The error message to display.
	 * \return Returns the control that failed or nullptr.
	 **/
	CWidget * CWavEditorFileSettingsPage::Verify( const LSN_TRACK &_tTrack, std::wstring &_wsMsg ) {
#define LSN_CHECKED( ID, STORE )		{ STORE = (*_tTrack.Check( Layout::ID )); }

		LPARAM lpParam = 0;
		bool bChecked = false;
		CWidget * pwWidget = nullptr;
		ee::CExpEvalContainer::EE_RESULT eTest;

#define LSN_CHECK_EDIT( ID, MSG )		{										\
	pwWidget = FindChild( Layout::ID );											\
	if ( !CWinUtilities::TextAsDoubleExpression( (*_tTrack.Text( Layout::ID )), eTest ) ) {	\
		_wsMsg = MSG;															\
		return pwWidget ? pwWidget : this;										\
	} }
#define LSN_CHECK_EDIT_EMPTY( ID, MSG )		{									\
	pwWidget = FindChild( Layout::ID );											\
	if ( !_tTrack.Text( Layout::ID )->size() ) {								\
		_wsMsg = MSG;															\
		return pwWidget ? pwWidget : this;										\
	} }

		
#define LSN_GET_COMBO( ID, STORE, BAD_ID ) { STORE = ComboData( Layout::ID, (*_tTrack.Selection( Layout::ID )), BAD_ID ); }

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
			/*LSN_CHECK_EDIT( LSN_WEWI_FSETS_CHAR_LPF_EDIT, LSN_LSTR( LSN_WE_OUTPUT_ERR_SET_BAD_HZ ) );
			if ( eTest.u.dVal < 0.0 ) {
				_wsMsg = LSN_LSTR( LSN_WE_OUTPUT_ERR_SET_HZ_NEG );
				return pwWidget;
			}*/
			pwWidget = FindChild( Layout::LSN_WEWI_FSETS_CHAR_LPF_EDIT );
			float fPole, fZero;
			if ( !GetLpfAndZero( _tTrack.wsCharLpfHz, fPole, fZero ) ) {
				_wsMsg = LSN_LSTR( LSN_WE_OUTPUT_ERR_SET_BAD_HZ );
				return pwWidget ? pwWidget : this;
			}
			if ( fPole < 0.0f || fZero < 0.0f ) {
				_wsMsg = LSN_LSTR( LSN_WE_OUTPUT_ERR_SET_HZ_NEG );
				return pwWidget ? pwWidget : this;
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
	 * Saves the values of a track.
	 * 
	 * \param _ui32Id The unique ID of the track.
	 * \param _tTrack The values to save.
	 * \param _pfPerFileOptions The object to which to save the window state.
	 * \param _ppfOutput The output object to which to transfer all the window settings.
	 */
	void CWavEditorFileSettingsPage::Save( uint32_t _ui32Id, const LSN_TRACK &_tTrack, LSN_WAV_EDITOR_WINDOW_OPTIONS::LSN_PER_FILE &_pfPerFileOptions, CWavEditor::LSN_PER_FILE * _ppfOutput ) {
#define LSN_CHECKED( ID, STORE )			{ STORE = (*_tTrack.Check( Layout::ID )); }

#define LSN_EDIT_TEXT( ID, STORE )			{ STORE = (*_tTrack.Text( Layout::ID )); }

#define LSN_EDIT_VAL( ID, STORE )			if ( _ppfOutput ) {					\
	if ( CWinUtilities::TextAsDoubleExpression( (*_tTrack.Text( Layout::ID )), eTest ) ) { STORE = eTest.u.dVal; } }

#define LSN_COMBO_VAL( ID, STORE, TYPE )	{ STORE = static_cast<TYPE>(ComboData( Layout::ID, (*_tTrack.Selection( Layout::ID )), LPARAM( STORE ) )); }
	
		
		ee::CExpEvalContainer::EE_RESULT eTest;
		LSN_EDIT_TEXT( LSN_WEWI_FSETS_FDATA_NAME_EDIT, _pfPerFileOptions.wsTitle );
		LSN_EDIT_TEXT( LSN_WEWI_FSETS_FDATA_PREFIX_EDIT, _pfPerFileOptions.wsTitlePreFix );
		LSN_EDIT_TEXT( LSN_WEWI_FSETS_FDATA_POSTFIX_EDIT, _pfPerFileOptions.wsTitlePostFix );

		LSN_EDIT_TEXT( LSN_WEWI_FSETS_FDATA_HZ_EDIT, _pfPerFileOptions.wsActualHz );
		LSN_COMBO_VAL( LSN_WEWI_FSETS_FDATA_HZ_COMBO, _pfPerFileOptions.ui32ActualHz, uint32_t );
		LSN_EDIT_TEXT( LSN_WEWI_FSETS_CHAR_VOL_EDIT, _pfPerFileOptions.wsCharVolume );
		
		LSN_CHECKED( LSN_WEWI_FSETS_CHAR_LOCK_CHECK, _pfPerFileOptions.bLockVol );
		LSN_CHECKED( LSN_WEWI_FSETS_CHAR_INV_CHECK, _pfPerFileOptions.bInvert );

		LSN_CHECKED( LSN_WEWI_FSETS_CHAR_LPF_CHECK, _pfPerFileOptions.bLpf );
		LSN_CHECKED( LSN_WEWI_FSETS_CHAR_HPF0_CHECK, _pfPerFileOptions.bHpf0 );
		LSN_CHECKED( LSN_WEWI_FSETS_CHAR_HPF1_CHECK, _pfPerFileOptions.bHpf1 );
		LSN_CHECKED( LSN_WEWI_FSETS_CHAR_HPF2_CHECK, _pfPerFileOptions.bHpf2 );

		LSN_CHECKED( LSN_WEWI_FSETS_CHAR_RF_1_CHECK, _pfPerFileOptions.bRf1 );
		LSN_CHECKED( LSN_WEWI_FSETS_CHAR_RF_2_CHECK, _pfPerFileOptions.bRf2 );
		LSN_CHECKED( LSN_WEWI_FSETS_CHAR_SUNSOFT_5B_CHECK, _pfPerFileOptions.bSunsoft5b );

		LSN_EDIT_TEXT( LSN_WEWI_FSETS_CHAR_LPF_EDIT, _pfPerFileOptions.wsCharLpfHz );
		LSN_COMBO_VAL( LSN_WEWI_FSETS_CHAR_LPF_TYPE_COMBO, _pfPerFileOptions.ui8LpfType, uint8_t );
		LSN_EDIT_TEXT( LSN_WEWI_FSETS_CHAR_LPF_FALLOFF_EDIT, _pfPerFileOptions.wsCharLpfFall );

		LSN_EDIT_TEXT( LSN_WEWI_FSETS_CHAR_HPF0_EDIT, _pfPerFileOptions.wsCharHpf0Hz );
		LSN_COMBO_VAL( LSN_WEWI_FSETS_CHAR_HPF0_TYPE_COMBO, _pfPerFileOptions.ui8Hpf0Type, uint8_t );
		LSN_EDIT_TEXT( LSN_WEWI_FSETS_CHAR_HPF0_FALLOFF_EDIT, _pfPerFileOptions.wsCharHpf0Fall );

		LSN_EDIT_TEXT( LSN_WEWI_FSETS_CHAR_HPF1_EDIT, _pfPerFileOptions.wsCharHpf1Hz );
		LSN_COMBO_VAL( LSN_WEWI_FSETS_CHAR_HPF1_TYPE_COMBO, _pfPerFileOptions.ui8Hpf1Type, uint8_t );
		LSN_EDIT_TEXT( LSN_WEWI_FSETS_CHAR_HPF1_FALLOFF_EDIT, _pfPerFileOptions.wsCharHpf1Fall );

		LSN_EDIT_TEXT( LSN_WEWI_FSETS_CHAR_HPF2_EDIT, _pfPerFileOptions.wsCharHpf2Hz );
		LSN_COMBO_VAL( LSN_WEWI_FSETS_CHAR_HPF2_TYPE_COMBO, _pfPerFileOptions.ui8Hpf2Type, uint8_t );
		LSN_EDIT_TEXT( LSN_WEWI_FSETS_CHAR_HPF2_FALLOFF_EDIT, _pfPerFileOptions.wsCharHpf2Fall );

		LSN_EDIT_TEXT( LSN_WEWI_FSETS_MDATA_ARTIST_EDIT, _pfPerFileOptions.wsArtist );
		LSN_EDIT_TEXT( LSN_WEWI_FSETS_MDATA_ALBUM_EDIT, _pfPerFileOptions.wsAlbum );
		LSN_EDIT_TEXT( LSN_WEWI_FSETS_MDATA_YEAR_EDIT, _pfPerFileOptions.wsYear );
		LSN_EDIT_TEXT( LSN_WEWI_FSETS_MDATA_COMMENTS_EDIT, _pfPerFileOptions.wsComment );

		if ( _ppfOutput ) {
			LSN_EDIT_TEXT( LSN_WEWI_FSETS_FDATA_NAME_EDIT, _ppfOutput->wsName );
			LSN_EDIT_TEXT( LSN_WEWI_FSETS_FDATA_PREFIX_EDIT, _ppfOutput->wsFilePrefix );
			LSN_EDIT_TEXT( LSN_WEWI_FSETS_FDATA_POSTFIX_EDIT, _ppfOutput->wsFilePostFix );

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
					auto wfsSet = m_pweEditor ? m_pweEditor->WavById( _ui32Id ) : nullptr;
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
			if ( _pfPerFileOptions.bLpf ) {
				//LSN_EDIT_VAL( LSN_WEWI_FSETS_CHAR_LPF_EDIT, _ppfOutput->dLpf );
				float fPole, fZero;
				if ( GetLpfAndZero( _tTrack.wsCharLpfHz, fPole, fZero ) ) {
					_ppfOutput->dLpf = fPole;
					_ppfOutput->dZero = fZero;
				}

				LSN_COMBO_VAL( LSN_WEWI_FSETS_CHAR_LPF_TYPE_COMBO, _ppfOutput->fLpfType, CWavEditor::LSN_FILTER );
				if ( _ppfOutput->fLpfType == CWavEditor::LSN_F_SINC ) {
					LSN_EDIT_VAL( LSN_WEWI_FSETS_CHAR_LPF_FALLOFF_EDIT, _ppfOutput->dFalloffLpf );
				}
			}
			if ( _pfPerFileOptions.bHpf0 ) {
				LSN_EDIT_VAL( LSN_WEWI_FSETS_CHAR_HPF0_EDIT, _ppfOutput->dHpf0 );
				LSN_COMBO_VAL( LSN_WEWI_FSETS_CHAR_HPF0_TYPE_COMBO, _ppfOutput->fHpf0Type, CWavEditor::LSN_FILTER );
				if ( _ppfOutput->fHpf0Type == CWavEditor::LSN_F_SINC ) {
					LSN_EDIT_VAL( LSN_WEWI_FSETS_CHAR_HPF0_FALLOFF_EDIT, _ppfOutput->dFalloffHpf0 );
				}
			}
			if ( _pfPerFileOptions.bHpf1 ) {
				LSN_EDIT_VAL( LSN_WEWI_FSETS_CHAR_HPF1_EDIT, _ppfOutput->dHpf1 );
				LSN_COMBO_VAL( LSN_WEWI_FSETS_CHAR_HPF1_TYPE_COMBO, _ppfOutput->fHpf1Type, CWavEditor::LSN_FILTER );
				if ( _ppfOutput->fHpf0Type == CWavEditor::LSN_F_SINC ) {
					LSN_EDIT_VAL( LSN_WEWI_FSETS_CHAR_HPF1_FALLOFF_EDIT, _ppfOutput->dFalloffHpf1 );
				}
			}
			if ( _pfPerFileOptions.bHpf2 ) {
				LSN_EDIT_VAL( LSN_WEWI_FSETS_CHAR_HPF2_EDIT, _ppfOutput->dHpf2 );
				LSN_COMBO_VAL( LSN_WEWI_FSETS_CHAR_HPF2_TYPE_COMBO, _ppfOutput->fHpf2Type, CWavEditor::LSN_FILTER );
				if ( _ppfOutput->fHpf0Type == CWavEditor::LSN_F_SINC ) {
					LSN_EDIT_VAL( LSN_WEWI_FSETS_CHAR_HPF2_FALLOFF_EDIT, _ppfOutput->dFalloffHpf2 );
				}
			}

			LSN_CHECKED( LSN_WEWI_FSETS_CHAR_RF_1_CHECK, _ppfOutput->bRf1 );
			LSN_CHECKED( LSN_WEWI_FSETS_CHAR_RF_2_CHECK, _ppfOutput->bRf2 );
			LSN_CHECKED( LSN_WEWI_FSETS_CHAR_SUNSOFT_5B_CHECK, _ppfOutput->bSunsoft5b );

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
	 * Fills out the values of a track with saved values from a project.
	 * 
	 * \param _pfPerFileOptions The per-file settings to load.
	 * \param _tTrack The track values to fill out.
	 **/
	void CWavEditorFileSettingsPage::Load( const LSN_WAV_EDITOR_WINDOW_OPTIONS::LSN_PER_FILE &_pfPerFileOptions, LSN_TRACK &_tTrack ) {
		_tTrack.wsTitle = _pfPerFileOptions.wsTitle;
		_tTrack.wsTitlePreFix = _pfPerFileOptions.wsTitlePreFix;
		_tTrack.wsTitlePostFix = _pfPerFileOptions.wsTitlePostFix;

		_tTrack.wsActualHz = _pfPerFileOptions.wsActualHz;
		_tTrack.wsCharVolume = _pfPerFileOptions.wsCharVolume;
		_tTrack.wsCharLpfHz = _pfPerFileOptions.wsCharLpfHz;
		_tTrack.wsCharHpf0Hz = _pfPerFileOptions.wsCharHpf0Hz;
		_tTrack.wsCharHpf1Hz = _pfPerFileOptions.wsCharHpf1Hz;
		_tTrack.wsCharHpf2Hz = _pfPerFileOptions.wsCharHpf2Hz;

		_tTrack.wsCharLpfFall = _pfPerFileOptions.wsCharLpfFall;
		_tTrack.wsCharHpf0Fall = _pfPerFileOptions.wsCharHpf0Fall;
		_tTrack.wsCharHpf1Fall = _pfPerFileOptions.wsCharHpf1Fall;
		_tTrack.wsCharHpf2Fall = _pfPerFileOptions.wsCharHpf2Fall;

		_tTrack.wsArtist = _pfPerFileOptions.wsArtist;
		_tTrack.wsAlbum = _pfPerFileOptions.wsAlbum;
		_tTrack.wsYear = _pfPerFileOptions.wsYear;
		_tTrack.wsComment = _pfPerFileOptions.wsComment;


		// Last checks.
		_tTrack.bLockVol = _pfPerFileOptions.bLockVol;
		_tTrack.bInvert = _pfPerFileOptions.bInvert;
		_tTrack.bLpf = _pfPerFileOptions.bLpf;
		_tTrack.bHpf0 = _pfPerFileOptions.bHpf0;
		_tTrack.bHpf1 = _pfPerFileOptions.bHpf1;
		_tTrack.bHpf2 = _pfPerFileOptions.bHpf2;

		_tTrack.bRf1 = _pfPerFileOptions.bRf1;
		_tTrack.bRf2 = _pfPerFileOptions.bRf2 && !_pfPerFileOptions.bRf1;	// Mutually exclusive; RF 1 wins (see Update()).
		_tTrack.bSunsoft5b = _pfPerFileOptions.bSunsoft5b;

		// Last combo-box selections.
		_tTrack.i32ActualHzSelection = ComboIndex( Layout::LSN_WEWI_FSETS_FDATA_HZ_COMBO, LPARAM( int32_t( _pfPerFileOptions.ui32ActualHz ) ) );
		_tTrack.i32LpfTypeSelection = ComboIndex( Layout::LSN_WEWI_FSETS_CHAR_LPF_TYPE_COMBO, _pfPerFileOptions.ui8LpfType );
		_tTrack.i32Hpf0TypeSelection = ComboIndex( Layout::LSN_WEWI_FSETS_CHAR_HPF0_TYPE_COMBO, _pfPerFileOptions.ui8Hpf0Type );
		_tTrack.i32Hpf1TypeSelection = ComboIndex( Layout::LSN_WEWI_FSETS_CHAR_HPF1_TYPE_COMBO, _pfPerFileOptions.ui8Hpf1Type );
		_tTrack.i32Hpf2TypeSelection = ComboIndex( Layout::LSN_WEWI_FSETS_CHAR_HPF2_TYPE_COMBO, _pfPerFileOptions.ui8Hpf2Type );
	}

	/**
	 * Sets the values of a newly added track that depend on its WAV file (its name and an initial Actual Hz value based off the Hz detected in the
	 *	WAV file).
	 * 
	 * \param _ui32Id The unique ID of the track.
	 * \param _tTrack The track values to fill out.
	 **/
	void CWavEditorFileSettingsPage::InitTrack( uint32_t _ui32Id, LSN_TRACK &_tTrack ) {
		auto pwfsSet = m_pweEditor ? m_pweEditor->WavById( _ui32Id ) : nullptr;
		if ( !pwfsSet ) { return; }

		std::filesystem::path pPath = pwfsSet->wfFile.wsPath;
		pPath = pPath.filename().replace_extension( "" );
		_tTrack.wsTitle = pPath.generic_wstring();

		LPARAM lpHz;
		switch ( pwfsSet->wfFile.fcFormat.uiSampleRate ) {
			case 1789773 : {
				lpHz = LPARAM( CWavEditor::LSN_AH_NTSC );
				break;
			}
			case 1662608 : {
				lpHz = LPARAM( CWavEditor::LSN_AH_PAL );
				break;
			}
			case 1773448 : {
				lpHz = LPARAM( CWavEditor::LSN_AH_DENDY );
				break;
			}
			case 1787806 : {
				lpHz = LPARAM( CWavEditor::LSN_AH_PALM );
				break;
			}
			case 1791029 : {
				lpHz = LPARAM( CWavEditor::LSN_AH_PALN );
				break;
			}
			default : {
				lpHz = LPARAM( CWavEditor::LSN_AH_BY_FILE );
			}
		}
		_tTrack.i32ActualHzSelection = ComboIndex( Layout::LSN_WEWI_FSETS_FDATA_HZ_COMBO, lpHz );
	}

	/**
	 * Shows the values of a track in the controls.
	 * 
	 * \param _ui32Id The unique ID of the track.
	 * \param _tTrack The values to show.
	 **/
	void CWavEditorFileSettingsPage::LoadTrack( uint32_t _ui32Id, const LSN_TRACK &_tTrack ) {
		m_ui32Id = _ui32Id;
		m_bInternalUpdate = true;

		for ( auto I = std::size( m_wEditIds ); I--; ) {
			auto pwThis = FindChild( m_wEditIds[I] );
			if ( pwThis ) { pwThis->SetTextW( _tTrack.Text( m_wEditIds[I] )->c_str() ); }
		}
		for ( auto I = std::size( m_wComboIds ); I--; ) {
			auto pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( m_wComboIds[I] ));
			if ( pcbCombo ) { pcbCombo->SetCurSel( (*_tTrack.Selection( m_wComboIds[I] )) ); }
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
	void CWavEditorFileSettingsPage::SaveTrack( LSN_TRACK &_tTrack ) {
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
	void CWavEditorFileSettingsPage::Update() {
		{
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
				Layout::LSN_CONTROL_IDS									wId;
				bool													bEnable;
			} cControls[] = {
				{ Layout::LSN_WEWI_FSETS_FDATA_HZ_EDIT,					lpHzData == LPARAM( CWavEditor::LSN_AH_CUSTOM ) },
				{ Layout::LSN_WEWI_FSETS_CHAR_LPF_FALLOFF_EDIT,			lpLpfPole == LPARAM( CWavEditor::LSN_F_SINC ) },
				{ Layout::LSN_WEWI_FSETS_CHAR_HPF0_FALLOFF_EDIT,		lpHpfPole0 == LPARAM( CWavEditor::LSN_F_SINC ) },
				{ Layout::LSN_WEWI_FSETS_CHAR_HPF1_FALLOFF_EDIT,		lpHpfPole1 == LPARAM( CWavEditor::LSN_F_SINC ) },
				{ Layout::LSN_WEWI_FSETS_CHAR_HPF2_FALLOFF_EDIT,		lpHpfPole2 == LPARAM( CWavEditor::LSN_F_SINC ) },
			};
			for ( auto I = std::size( cControls ); I--; ) {
				auto pwThis = FindChild( cControls[I].wId );
				if ( pwThis ) {
					pwThis->SetEnabled( cControls[I].bEnable );
				}
			}
		}

		bool bLockVolume = false;

		auto pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_WEWI_FSETS_CHAR_LOCK_CHECK ));
		if ( pcbCheck ) {
			bLockVolume = pcbCheck->IsChecked();
		}

		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_WEWI_FSETS_CHAR_RF_1_CHECK ));
		if ( pcbCheck ) {
			if ( pcbCheck->IsChecked() ) {
				pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_WEWI_FSETS_CHAR_RF_2_CHECK ));
				if ( pcbCheck ) {
					pcbCheck->SetCheck( FALSE );
				}
			}
		}
		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_WEWI_FSETS_CHAR_RF_2_CHECK ));
		if ( pcbCheck ) {
			if ( pcbCheck->IsChecked() ) {
				pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_WEWI_FSETS_CHAR_RF_1_CHECK ));
				if ( pcbCheck ) {
					pcbCheck->SetCheck( FALSE );
				}
			}
		}

		auto pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_WEWI_FSETS_CHAR_PRESET_COMBO ));
		if ( pcbCombo ) {
			LSN_AUDIO_PROFILE apProfile;
			pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_WEWI_FSETS_CHAR_LPF_CHECK ));
			if ( pcbCheck ) {
				apProfile.bLpfEnable = pcbCheck->IsChecked();
				pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF0_CHECK ));
				if ( pcbCheck ) {
					apProfile.bHpf0Enable = pcbCheck->IsChecked();
					pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF1_CHECK ));
					if ( pcbCheck ) {
						apProfile.bHpf1Enable = pcbCheck->IsChecked();
						pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF2_CHECK ));
						if ( pcbCheck ) {
							apProfile.bHpf2Enable = pcbCheck->IsChecked();
							pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_WEWI_FSETS_CHAR_INV_CHECK ));
							if ( pcbCheck ) {
								apProfile.bInvert = pcbCheck->IsChecked();
								pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_WEWI_FSETS_CHAR_SUNSOFT_5B_CHECK ));
								if ( pcbCheck ) {
									apProfile.bSunsoft5b = pcbCheck->IsChecked();
									auto aEdit = FindChild( Layout::LSN_WEWI_FSETS_CHAR_VOL_EDIT );
									ee::CExpEvalContainer::EE_RESULT rRes;
									if ( aEdit && aEdit->GetTextAsDoubleExpression( rRes ) ) {
										apProfile.fVolume = float( rRes.u.dVal );

										if ( GetLpfAndZero( apProfile.fLpf, apProfile.fZero ) ) {
										/*aEdit = FindChild( Layout::LSN_WEWI_FSETS_CHAR_LPF_EDIT );
										if ( aEdit && aEdit->GetTextAsDoubleExpression( rRes ) ) {
											apProfile.fLpf = float( rRes.u.dVal );*/
											aEdit = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF0_EDIT );
											if ( aEdit && aEdit->GetTextAsDoubleExpression( rRes ) ) {
												apProfile.fHpf0 = float( rRes.u.dVal );
												aEdit = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF1_EDIT );
												if ( aEdit && aEdit->GetTextAsDoubleExpression( rRes ) ) {
													apProfile.fHpf1 = float( rRes.u.dVal );
													aEdit = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF2_EDIT );
													if ( aEdit && aEdit->GetTextAsDoubleExpression( rRes ) ) {
														apProfile.fHpf2 = float( rRes.u.dVal );
														LPARAM lpSel = -1;
														for ( auto I = LSN_AUDIO_OPTIONS::PresetTotal(); I--; ) {
															auto aThis = LSN_AUDIO_OPTIONS::s_apProfiles[I];
													
															if ( aThis.bLpfEnable == apProfile.bLpfEnable &&
																aThis.bHpf0Enable == apProfile.bHpf0Enable &&
																aThis.bHpf1Enable == apProfile.bHpf1Enable &&
																aThis.bHpf2Enable == apProfile.bHpf2Enable &&
																aThis.bInvert == apProfile.bInvert &&
																aThis.bSunsoft5b == apProfile.bSunsoft5b &&
																(!apProfile.bLpfEnable || aThis.fLpf == apProfile.fLpf) &&
																(!apProfile.bLpfEnable || aThis.fZero == apProfile.fZero) &&
																(!apProfile.bHpf0Enable || aThis.fHpf0 == apProfile.fHpf0) &&
																(!apProfile.bHpf1Enable || aThis.fHpf1 == apProfile.fHpf1) &&
																(!apProfile.bHpf2Enable || aThis.fHpf2 == apProfile.fHpf2) &&
																(bLockVolume || aThis.fVolume == apProfile.fVolume) ) {
																lpSel = I;
																break;
															}
														}
														// Search custom presets.
														pcbCombo->SetCurSelByItemData( lpSel );
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	/**
	 * Shows the combined values of all the tracks being edited together (the selected tracks, or all tracks if none are selected).
	 **/
	void CWavEditorFileSettingsPage::Activate() {
		if ( !m_pweEditor ) { return; }
		m_ui32Id = 0;

		std::vector<LPARAM> vAffected;
		GetPagesToUpdate( vAffected );
		// With no tracks the controls keep their values, which are saved as the defaults for new tracks.
		if ( vAffected.size() ) {
			m_bInternalUpdate = true;
			auto pwewWindow = static_cast<CWavEditorWindow *>(m_pwParent);
			for ( auto I = std::size( m_wComboIds ); I--; ) {
				auto pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( m_wComboIds[I] ));
				if ( pcbCombo ) {
					pcbCombo->SetCurSel( pwewWindow->GetAllSettingsComboSel( m_wComboIds[I], vAffected ) );
				}
			}
			for ( auto I = std::size( m_wEditIds ); I--; ) {
				auto pwThis = FindChild( m_wEditIds[I] );
				if ( pwThis ) {
					pwThis->SetTextW( pwewWindow->GetAllSettingsEditTexts( m_wEditIds[I], vAffected ).c_str() );
				}
			}
			for ( auto I = std::size( m_wCheckIds ); I--; ) {
				auto pwThis = FindChild( m_wCheckIds[I] );
				if ( pwThis ) {
					pwThis->CheckButton( pwewWindow->GetAllSettingsCheckStates( m_wCheckIds[I], vAffected ) );
				}
			}
			m_bInternalUpdate = false;
		}

		Update();
	}

	/**
	 * Gets the unique ID's of the tracks to which changes in the controls are applied.  This is the shown track or else the selected tracks (or
	 *	all tracks if none are selected).
	 * 
	 * \param _vPages Holds the returned array of track ID's to update.
	 * \param _bUpdateAll If true, all tracks except the shown track are returned.
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
					if ( (_vPages[I] & 0x80000000) || uint32_t( _vPages[I] ) == UniqueId() ) {
						_vPages.erase( _vPages.begin() + I );
					}
				}
			}

			// Whatever is left should be updated, even if there is nothing left.
		}
	}

	/**
	 * Gets the index of the item in a combo box with the given item data.
	 * 
	 * \param _wId The ID of the combo box.
	 * \param _lpData The item data to find.
	 * \return Returns the index of the item with the given data or -1.
	 **/
	int32_t CWavEditorFileSettingsPage::ComboIndex( WORD _wId, LPARAM _lpData ) {
		auto pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( _wId ));
		if ( pcbCombo ) {
			for ( INT I = pcbCombo->GetCount(); I--; ) {
				if ( pcbCombo->GetItemData( I ) == _lpData ) { return int32_t( I ); }
			}
		}
		return -1;
	}

	/**
	 * Gets the item data of an item in a combo box.
	 * 
	 * \param _wId The ID of the combo box.
	 * \param _i32Index The index of the item.
	 * \param _lpBad The value to return if the combo box does not exist.
	 * \return Returns the item data of the given item.
	 **/
	LPARAM CWavEditorFileSettingsPage::ComboData( WORD _wId, int32_t _i32Index, LPARAM _lpBad ) {
		auto pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( _wId ));
		if ( !pcbCombo ) { return _lpBad; }
		return pcbCombo->GetItemData( _i32Index );
	}

	/**
	 * Gets the LPF Pole and Zero from a string.
	 * 
	 * \param _wsText The text to evaluate.
	 * \param _fPole Holds the returned Pole.
	 * \param _fZero Holds the returned Zero.
	 * \return Returns true if the text is a valid expression giving 1 or 2 values.
	 **/
	bool CWavEditorFileSettingsPage::GetLpfAndZero( const std::wstring &_wsText, float &_fPole, float &_fZero ) {
		std::vector<float> vPrimArray;
		ee::CExpEvalContainer::EE_RESULT rRes;
		if ( CWinUtilities::TextAsFloatVectorExpression( _wsText, vPrimArray ) ) {
			if ( vPrimArray.size() == 2 ) {
				_fPole = vPrimArray[0];
				_fZero = vPrimArray[1];
				return true;
			}
			if ( vPrimArray.size() == 1 ) {
				_fPole = vPrimArray[0];
				_fZero = 0.0;
				return true;
			}
		}
		else if ( CWinUtilities::TextAsDoubleExpression( _wsText, rRes ) ) {
			_fPole = float( rRes.u.dVal );
			_fZero = 0.0f;
			return true;
		}
		return false;
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

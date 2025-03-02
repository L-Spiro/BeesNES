#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The audio options window.
 */

#include "LSNAudioOptionsRecordingPage.h"
#include "../../Localization/LSNLocalization.h"
#include "../Layout/LSNLayoutMacros.h"
#include "LSNAudioOptionsGeneralPage.h"

#include <ListBox/LSWListBox.h>
#include <Tab/LSWTab.h>

#include "../../../resource.h"


namespace lsn {

	CAudioOptionsRecordingPage::CAudioOptionsRecordingPage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_poOptions( reinterpret_cast<LSN_OPTIONS *>(_ui64Data) ) {
	}

	// == Functions.
	/**
	 * The WM_INITDIALOG handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CAudioOptionsRecordingPage::InitDialog() {
		Parent::InitDialog();

		auto aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_RAW_HZ_EDIT );
		if ( aEdit ) { aEdit->SetTextA( std::to_string( m_poOptions->dApuHz ).c_str() ); }
		aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_OUT_HZ_EDIT );
		if ( aEdit ) { aEdit->SetTextW( LSN_LSTR( LSN_AUDIO_OPTIONS_DECIDED_BY_SETTINGS ) ); }

		aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_RAW_FORMAT_COMBO );
		if ( aEdit ) { CWinUtilities::FillComboWithWavFormats( aEdit, m_poOptions->stfStreamOptionsRaw.fFormat ); }
		aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_OUT_FORMAT_COMBO );
		if ( aEdit ) { CWinUtilities::FillComboWithWavFormats( aEdit, m_poOptions->stfStreamOptionsOutCaptire.fFormat ); }

		aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_RAW_BITS_COMBO );
		if ( aEdit ) { CWinUtilities::FillComboWithWavPcmBits( aEdit, m_poOptions->stfStreamOptionsRaw.ui32Bits ); }
		aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_OUT_BITS_COMBO );
		if ( aEdit ) { CWinUtilities::FillComboWithWavPcmBits( aEdit, m_poOptions->stfStreamOptionsOutCaptire.ui32Bits ); }

		aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_RAW_START_CONDITION_COMBO );
		if ( aEdit ) { CWinUtilities::FillComboWithWavStartConditions( aEdit, m_poOptions->stfStreamOptionsRaw.scStartCondition ); }
		aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_OUT_START_CONDITION_COMBO );
		if ( aEdit ) { CWinUtilities::FillComboWithWavStartConditions( aEdit, m_poOptions->stfStreamOptionsOutCaptire.scStartCondition ); }

		aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_RAW_STOP_CONDITION_COMBO );
		if ( aEdit ) { CWinUtilities::FillComboWithWavEndConditions( aEdit, m_poOptions->stfStreamOptionsRaw.seEndCondition ); }
		aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_OUT_STOP_CONDITION_COMBO );
		if ( aEdit ) { CWinUtilities::FillComboWithWavEndConditions( aEdit, m_poOptions->stfStreamOptionsOutCaptire.seEndCondition ); }

		aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_RAW_START_COMBO );
		if ( aEdit ) { CWinUtilities::FillComboWithStrings( aEdit, m_poOptions->vRawStartHistory, 0 ); }
		aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_OUT_START_COMBO );
		if ( aEdit ) { CWinUtilities::FillComboWithStrings( aEdit, m_poOptions->vOutStartHistory, 0 ); }

		aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_RAW_STOP_COMBO );
		if ( aEdit ) { CWinUtilities::FillComboWithStrings( aEdit, m_poOptions->vRawEndHistory, 0 ); }
		aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_OUT_STOP_COMBO );
		if ( aEdit ) { CWinUtilities::FillComboWithStrings( aEdit, m_poOptions->vOutEndHistory, 0 ); }


		lsw::CCheckButton * pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_RAW_ENABLE_CHECK ));
		if ( pcbCheck ) { pcbCheck->SetCheck( m_poOptions->stfStreamOptionsRaw.bEnabled ); }
		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_OUT_ENABLE_CHECK ));
		if ( pcbCheck ) { pcbCheck->SetCheck( m_poOptions->stfStreamOptionsOutCaptire.bEnabled ); }

		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_RAW_DITHER_CHECK ));
		if ( pcbCheck ) { pcbCheck->SetCheck( m_poOptions->stfStreamOptionsRaw.bDither ); }
		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_OUT_DITHER_CHECK ));
		if ( pcbCheck ) { pcbCheck->SetCheck( m_poOptions->stfStreamOptionsOutCaptire.bDither ); }

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
	CWidget::LSW_HANDLED CAudioOptionsRecordingPage::Command( WORD /*_wCtrlCode*/, WORD _wId, CWidget * /*_pwSrc*/ ) {
		switch ( _wId ) {
			case Layout::LSN_AOWI_CANCEL : {
				//return Close();
				break;
			}
			case Layout::LSN_AOWI_OK : {
				//SaveAndClose();
				return LSW_H_HANDLED;
			}
		}
		Update();
		return LSW_H_CONTINUE;
	}

	/**
	 * Saves the current input configuration and closes the dialog.
	 */
	void CAudioOptionsRecordingPage::Save() {
		
	}

	/**
	 * Updates the dialog.
	 **/
	void CAudioOptionsRecordingPage::Update() {
		bool bEnabled = true;
		lsw::CCheckButton * pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_RAW_ENABLE_CHECK ));
		if ( pcbCheck ) { bEnabled = pcbCheck->IsChecked(); }
		LPARAM lFormatSel = CWavFile::LSN_F_PCM;
		lsw::CComboBox * pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_RAW_FORMAT_COMBO ));
		if ( pcbCombo ) { lFormatSel = pcbCombo->GetCurSelItemData(); }
		LPARAM lStartCond = CWavFile::LSN_SC_NONE;
		pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_RAW_START_CONDITION_COMBO ));
		if ( pcbCombo ) { lStartCond = pcbCombo->GetCurSelItemData(); }
		LPARAM lEndCond = CWavFile::LSN_EC_NONE;
		pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_RAW_STOP_CONDITION_COMBO ));
		if ( pcbCombo ) { lEndCond = pcbCombo->GetCurSelItemData(); }
		LPARAM lpBits = 24;
		pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_RAW_BITS_COMBO ));
		if ( pcbCombo ) { lpBits = pcbCombo->GetCurSelItemData(); }


		

		struct LSN_CONTROLS {
			CAudioOptionsWindowLayout::LSN_AUDIO_OPTIONS_WINDOW_IDS						wId;
			bool																		bCloseCondition0;
			bool																		bCloseCondition1;
		} cControls[] = {
			{ CAudioOptionsWindowLayout::LSN_AOWI_PAGE_RAW_PATH_LABEL,					bEnabled,				bEnabled },
			{ CAudioOptionsWindowLayout::LSN_AOWI_PAGE_RAW_PATH_EDIT,					bEnabled,				bEnabled },
			{ CAudioOptionsWindowLayout::LSN_AOWI_PAGE_RAW_PATH_BUTTON,					bEnabled,				bEnabled },
			{ CAudioOptionsWindowLayout::LSN_AOWI_PAGE_RAW_HZ_LABEL,					bEnabled,				bEnabled },
			{ CAudioOptionsWindowLayout::LSN_AOWI_PAGE_RAW_FORMAT_LABEL,				bEnabled,				bEnabled },
			{ CAudioOptionsWindowLayout::LSN_AOWI_PAGE_RAW_FORMAT_COMBO,				bEnabled,				bEnabled },
			{ CAudioOptionsWindowLayout::LSN_AOWI_PAGE_RAW_BITS_LABEL,					bEnabled,				lFormatSel == CWavFile::LSN_F_PCM },
			{ CAudioOptionsWindowLayout::LSN_AOWI_PAGE_RAW_BITS_COMBO,					bEnabled,				lFormatSel == CWavFile::LSN_F_PCM },
			{ CAudioOptionsWindowLayout::LSN_AOWI_PAGE_RAW_DITHER_CHECK,				bEnabled,				lFormatSel == CWavFile::LSN_F_PCM && lpBits == 16 },
			{ CAudioOptionsWindowLayout::LSN_AOWI_PAGE_RAW_START_CONDITION_LABEL,		bEnabled,				bEnabled },
			{ CAudioOptionsWindowLayout::LSN_AOWI_PAGE_RAW_START_CONDITION_COMBO,		bEnabled,				bEnabled },
			{ CAudioOptionsWindowLayout::LSN_AOWI_PAGE_RAW_STOP_CONDITION_LABEL,		bEnabled,				bEnabled },
			{ CAudioOptionsWindowLayout::LSN_AOWI_PAGE_RAW_STOP_CONDITION_COMBO,		bEnabled,				bEnabled },
			{ CAudioOptionsWindowLayout::LSN_AOWI_PAGE_RAW_START_COMBO,					bEnabled,				lStartCond != CWavFile::LSN_SC_NONE && lStartCond != CWavFile::LSN_SC_FIRST_NON_ZERO },
			{ CAudioOptionsWindowLayout::LSN_AOWI_PAGE_RAW_STOP_COMBO,					bEnabled,				lEndCond != CWavFile::LSN_SC_NONE },
		};
		for ( auto I = LSN_ELEMENTS( cControls ); I--; ) {
			auto pwThis = FindChild( cControls[I].wId );
			if ( pwThis ) {
				pwThis->SetEnabled( cControls[I].bCloseCondition0 && cControls[I].bCloseCondition1 );
			}
		}


		bEnabled = true;
		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_OUT_ENABLE_CHECK ));
		if ( pcbCheck ) { bEnabled = pcbCheck->IsChecked(); }
		lFormatSel = CWavFile::LSN_F_PCM;
		pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_OUT_FORMAT_COMBO ));
		if ( pcbCombo ) { lFormatSel = pcbCombo->GetCurSelItemData(); }
		lStartCond = CWavFile::LSN_SC_NONE;
		pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_OUT_START_CONDITION_COMBO ));
		if ( pcbCombo ) { lStartCond = pcbCombo->GetCurSelItemData(); }
		lEndCond = CWavFile::LSN_EC_NONE;
		pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_OUT_STOP_CONDITION_COMBO ));
		if ( pcbCombo ) { lEndCond = pcbCombo->GetCurSelItemData(); }
		lpBits = 24;
		pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_OUT_BITS_COMBO ));
		if ( pcbCombo ) { lpBits = pcbCombo->GetCurSelItemData(); }

		LSN_CONTROLS cOutControls[] = {
			{ CAudioOptionsWindowLayout::LSN_AOWI_PAGE_OUT_PATH_LABEL,					bEnabled,				bEnabled },
			{ CAudioOptionsWindowLayout::LSN_AOWI_PAGE_OUT_PATH_EDIT,					bEnabled,				bEnabled },
			{ CAudioOptionsWindowLayout::LSN_AOWI_PAGE_OUT_PATH_BUTTON,					bEnabled,				bEnabled },
			{ CAudioOptionsWindowLayout::LSN_AOWI_PAGE_OUT_HZ_LABEL,					bEnabled,				bEnabled },
			{ CAudioOptionsWindowLayout::LSN_AOWI_PAGE_OUT_FORMAT_LABEL,				bEnabled,				bEnabled },
			{ CAudioOptionsWindowLayout::LSN_AOWI_PAGE_OUT_FORMAT_COMBO,				bEnabled,				bEnabled },
			{ CAudioOptionsWindowLayout::LSN_AOWI_PAGE_OUT_BITS_LABEL,					bEnabled,				lFormatSel == CWavFile::LSN_F_PCM },
			{ CAudioOptionsWindowLayout::LSN_AOWI_PAGE_OUT_BITS_COMBO,					bEnabled,				lFormatSel == CWavFile::LSN_F_PCM },
			{ CAudioOptionsWindowLayout::LSN_AOWI_PAGE_OUT_DITHER_CHECK,				bEnabled,				lFormatSel == CWavFile::LSN_F_PCM && lpBits == 16 },
			{ CAudioOptionsWindowLayout::LSN_AOWI_PAGE_OUT_START_CONDITION_LABEL,		bEnabled,				bEnabled },
			{ CAudioOptionsWindowLayout::LSN_AOWI_PAGE_OUT_START_CONDITION_COMBO,		bEnabled,				bEnabled },
			{ CAudioOptionsWindowLayout::LSN_AOWI_PAGE_OUT_STOP_CONDITION_LABEL,		bEnabled,				bEnabled },
			{ CAudioOptionsWindowLayout::LSN_AOWI_PAGE_OUT_STOP_CONDITION_COMBO,		bEnabled,				bEnabled },
			{ CAudioOptionsWindowLayout::LSN_AOWI_PAGE_OUT_START_COMBO,					bEnabled,				lStartCond != CWavFile::LSN_SC_NONE && lStartCond != CWavFile::LSN_SC_FIRST_NON_ZERO },
			{ CAudioOptionsWindowLayout::LSN_AOWI_PAGE_OUT_STOP_COMBO,					bEnabled,				lEndCond != CWavFile::LSN_SC_NONE },
		};
		for ( auto I = LSN_ELEMENTS( cOutControls ); I--; ) {
			auto pwThis = FindChild( cOutControls[I].wId );
			if ( pwThis ) {
				pwThis->SetEnabled( cOutControls[I].bCloseCondition0 && cOutControls[I].bCloseCondition1 );
			}
		}
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

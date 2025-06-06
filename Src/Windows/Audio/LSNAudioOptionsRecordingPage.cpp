#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The audio options window.
 */

#include "LSNAudioOptionsRecordingPage.h"
#include "../../Apu/LSNApu2A0X.h"
#include "../../Localization/LSNLocalization.h"
#include "../Layout/LSNLayoutMacros.h"
#include "LSNAudioOptionsGeneralPage.h"

#include <ListBox/LSWListBox.h>
#include <Tab/LSWTab.h>

#include <commdlg.h>
#include <filesystem>

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


		auto aEdit = FindChild( Layout::LSN_AOWI_PAGE_RAW_PATH_EDIT );
		if ( aEdit ) { aEdit->SetTextW( m_poOptions->stfStreamOptionsRaw.wsPath.c_str() ); }
		aEdit = FindChild( Layout::LSN_AOWI_PAGE_OUT_PATH_EDIT );
		if ( aEdit ) { aEdit->SetTextW( m_poOptions->stfStreamOptionsOutCapture.wsPath.c_str() ); }

		aEdit = FindChild( Layout::LSN_AOWI_PAGE_RAW_HZ_EDIT );
		if ( aEdit ) { aEdit->SetTextA( std::to_string( m_poOptions->dApuHz ).c_str() ); }
		aEdit = FindChild( Layout::LSN_AOWI_PAGE_OUT_HZ_EDIT );
		if ( aEdit ) { aEdit->SetTextW( LSN_LSTR( LSN_AUDIO_OPTIONS_DECIDED_BY_SETTINGS ) ); }

		aEdit = FindChild( Layout::LSN_AOWI_PAGE_RAW_FORMAT_COMBO );
		if ( aEdit ) { CWinUtilities::FillComboWithWavFormats( aEdit, m_poOptions->stfStreamOptionsRaw.fFormat ); }
		aEdit = FindChild( Layout::LSN_AOWI_PAGE_OUT_FORMAT_COMBO );
		if ( aEdit ) { CWinUtilities::FillComboWithWavFormats( aEdit, m_poOptions->stfStreamOptionsOutCapture.fFormat ); }

		aEdit = FindChild( Layout::LSN_AOWI_PAGE_RAW_BITS_COMBO );
		if ( aEdit ) { CWinUtilities::FillComboWithWavPcmBits( aEdit, m_poOptions->stfStreamOptionsRaw.ui32Bits ); }
		aEdit = FindChild( Layout::LSN_AOWI_PAGE_OUT_BITS_COMBO );
		if ( aEdit ) { CWinUtilities::FillComboWithWavPcmBits( aEdit, m_poOptions->stfStreamOptionsOutCapture.ui32Bits ); }

		aEdit = FindChild( Layout::LSN_AOWI_PAGE_RAW_START_CONDITION_COMBO );
		if ( aEdit ) { CWinUtilities::FillComboWithWavStartConditions( aEdit, m_poOptions->stfStreamOptionsRaw.scStartCondition ); }
		aEdit = FindChild( Layout::LSN_AOWI_PAGE_OUT_START_CONDITION_COMBO );
		if ( aEdit ) { CWinUtilities::FillComboWithWavStartConditions( aEdit, m_poOptions->stfStreamOptionsOutCapture.scStartCondition ); }

		aEdit = FindChild( Layout::LSN_AOWI_PAGE_RAW_STOP_CONDITION_COMBO );
		if ( aEdit ) { CWinUtilities::FillComboWithWavEndConditions( aEdit, m_poOptions->stfStreamOptionsRaw.seEndCondition ); }
		aEdit = FindChild( Layout::LSN_AOWI_PAGE_OUT_STOP_CONDITION_COMBO );
		if ( aEdit ) { CWinUtilities::FillComboWithWavEndConditions( aEdit, m_poOptions->stfStreamOptionsOutCapture.seEndCondition ); }

		aEdit = FindChild( Layout::LSN_AOWI_PAGE_RAW_START_COMBO );
		if ( aEdit ) { CWinUtilities::FillComboWithStrings( aEdit, m_poOptions->vRawStartHistory, 0 ); }
		aEdit = FindChild( Layout::LSN_AOWI_PAGE_OUT_START_COMBO );
		if ( aEdit ) { CWinUtilities::FillComboWithStrings( aEdit, m_poOptions->vOutStartHistory, 0 ); }

		aEdit = FindChild( Layout::LSN_AOWI_PAGE_RAW_STOP_COMBO );
		if ( aEdit ) { CWinUtilities::FillComboWithStrings( aEdit, m_poOptions->vRawEndHistory, 0 ); }
		aEdit = FindChild( Layout::LSN_AOWI_PAGE_OUT_STOP_COMBO );
		if ( aEdit ) { CWinUtilities::FillComboWithStrings( aEdit, m_poOptions->vOutEndHistory, 0 ); }


		aEdit = FindChild( Layout::LSN_AOWI_PAGE_RAW_META_COMBO );
		if ( aEdit ) { CWinUtilities::FillComboWithWavMetaDataFormats( aEdit, CWavFile::LSN_MF_AUDACITY ); }
		aEdit = FindChild( Layout::LSN_AOWI_PAGE_OUT_META_COMBO );
		if ( aEdit ) { CWinUtilities::FillComboWithWavMetaDataFormats( aEdit, CWavFile::LSN_MF_AUDACITY ); }


		lsw::CCheckButton * pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_ENABLE_CHECK ));
		if ( pcbCheck ) { pcbCheck->SetCheck( m_poOptions->stfStreamOptionsRaw.bEnabled ); }
		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_OUT_ENABLE_CHECK ));
		if ( pcbCheck ) { pcbCheck->SetCheck( m_poOptions->stfStreamOptionsOutCapture.bEnabled ); }

		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_DITHER_CHECK ));
		if ( pcbCheck ) { pcbCheck->SetCheck( m_poOptions->stfStreamOptionsRaw.bDither ); }
		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_OUT_DITHER_CHECK ));
		if ( pcbCheck ) { pcbCheck->SetCheck( m_poOptions->stfStreamOptionsOutCapture.bDither ); }



		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_META_CHECK ));
		if ( pcbCheck ) { pcbCheck->SetCheck( m_poOptions->stfStreamOptionsRaw.bMetaEnabled ); }
		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_OUT_META_CHECK ));
		if ( pcbCheck ) { pcbCheck->SetCheck( m_poOptions->stfStreamOptionsOutCapture.bMetaEnabled ); }


		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_META_PULSE1_CHECK ));
		if ( pcbCheck ) { pcbCheck->SetCheck( m_poOptions->stfStreamOptionsRaw.ui64MetaParm & LSN_RF_PULSE1 ); }
		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_OUT_META_PULSE1_CHECK ));
		if ( pcbCheck ) { pcbCheck->SetCheck( m_poOptions->stfStreamOptionsOutCapture.ui64MetaParm & LSN_RF_PULSE1 ); }

		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_META_PULSE2_CHECK ));
		if ( pcbCheck ) { pcbCheck->SetCheck( m_poOptions->stfStreamOptionsRaw.ui64MetaParm & LSN_RF_PULSE2 ); }
		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_OUT_META_PULSE2_CHECK ));
		if ( pcbCheck ) { pcbCheck->SetCheck( m_poOptions->stfStreamOptionsOutCapture.ui64MetaParm & LSN_RF_PULSE2 ); }

		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_META_TRI_CHECK ));
		if ( pcbCheck ) { pcbCheck->SetCheck( m_poOptions->stfStreamOptionsRaw.ui64MetaParm & LSN_RF_TRIANGLE ); }
		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_OUT_META_TRI_CHECK ));
		if ( pcbCheck ) { pcbCheck->SetCheck( m_poOptions->stfStreamOptionsOutCapture.ui64MetaParm & LSN_RF_TRIANGLE ); }

		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_META_NOISE_CHECK ));
		if ( pcbCheck ) { pcbCheck->SetCheck( m_poOptions->stfStreamOptionsRaw.ui64MetaParm & LSN_RF_NOISE ); }
		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_OUT_META_NOISE_CHECK ));
		if ( pcbCheck ) { pcbCheck->SetCheck( m_poOptions->stfStreamOptionsOutCapture.ui64MetaParm & LSN_RF_NOISE ); }

		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_META_DMC_CHECK ));
		if ( pcbCheck ) { pcbCheck->SetCheck( m_poOptions->stfStreamOptionsRaw.ui64MetaParm & LSN_RF_DMC ); }
		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_OUT_META_DMC_CHECK ));
		if ( pcbCheck ) { pcbCheck->SetCheck( m_poOptions->stfStreamOptionsOutCapture.ui64MetaParm & LSN_RF_DMC ); }

		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_META_STATUS_CHECK ));
		if ( pcbCheck ) { pcbCheck->SetCheck( m_poOptions->stfStreamOptionsRaw.ui64MetaParm & LSN_RF_STATUS ); }
		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_OUT_META_STATUS_CHECK ));
		if ( pcbCheck ) { pcbCheck->SetCheck( m_poOptions->stfStreamOptionsOutCapture.ui64MetaParm & LSN_RF_STATUS ); }

		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_META_FRAME_CHECK ));
		if ( pcbCheck ) { pcbCheck->SetCheck( m_poOptions->stfStreamOptionsRaw.ui64MetaParm & LSN_RF_FRAME_COUNTER ); }
		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_OUT_META_FRAME_CHECK ));
		if ( pcbCheck ) { pcbCheck->SetCheck( m_poOptions->stfStreamOptionsOutCapture.ui64MetaParm & LSN_RF_FRAME_COUNTER ); }

		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_META_P1_ON_OFF_CHECK ));
		if ( pcbCheck ) { pcbCheck->SetCheck( m_poOptions->stfStreamOptionsRaw.ui64MetaParm & LSN_RF_PULSE1_ON_OFF ); }
		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_META_P2_ON_OFF_CHECK ));
		if ( pcbCheck ) { pcbCheck->SetCheck( m_poOptions->stfStreamOptionsRaw.ui64MetaParm & LSN_RF_PULSE2_ON_OFF ); }
		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_META_TRI_ON_OFF_CHECK ));
		if ( pcbCheck ) { pcbCheck->SetCheck( m_poOptions->stfStreamOptionsRaw.ui64MetaParm & LSN_RF_TRIANGKE_ON_OFF ); }
		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_META_NOISE_ON_OFF_CHECK ));
		if ( pcbCheck ) { pcbCheck->SetCheck( m_poOptions->stfStreamOptionsRaw.ui64MetaParm & LSN_RF_NOISE_ON_OFF ); }

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
	CWidget::LSW_HANDLED CAudioOptionsRecordingPage::Command( WORD _wCtrlCode, WORD _wId, CWidget * /*_pwSrc*/ ) {
		switch ( _wId ) {
			case Layout::LSN_AOWI_PAGE_RAW_PATH_BUTTON : {
				OPENFILENAMEW ofnOpenFile = { sizeof( ofnOpenFile ) };
				std::wstring szFileName;
				szFileName.resize( 0xFFFF + 2 );

				std::wstring wsFilter = std::wstring( LSN_LSTR( LSN_AUDIO_OPTIONS_WAV_TYPES ), LSN_ELEMENTS( LSN_LSTR( LSN_AUDIO_OPTIONS_WAV_TYPES ) ) - 1 );
				ofnOpenFile.hwndOwner = Wnd();
				ofnOpenFile.lpstrFilter = wsFilter.c_str();
				ofnOpenFile.lpstrFile = szFileName.data();
				ofnOpenFile.nMaxFile = DWORD( szFileName.size() );
				ofnOpenFile.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
				ofnOpenFile.lpstrInitialDir = m_poOptions->wRawAudioPath.c_str();

				if ( ::GetSaveFileNameW( &ofnOpenFile ) ) {
					m_poOptions->wRawAudioPath = std::filesystem::path( ofnOpenFile.lpstrFile ).remove_filename();
					auto pPath = std::filesystem::path( ofnOpenFile.lpstrFile );
					if ( !pPath.has_extension() ) {
						pPath += ".wav";
					}
					auto aEdit = FindChild( Layout::LSN_AOWI_PAGE_RAW_PATH_EDIT );
					if ( aEdit ) { aEdit->SetTextW( pPath.generic_wstring().c_str() ); }
				}
				break;
			}
			case Layout::LSN_AOWI_PAGE_OUT_PATH_BUTTON : {
				OPENFILENAMEW ofnOpenFile = { sizeof( ofnOpenFile ) };
				std::wstring szFileName;
				szFileName.resize( 0xFFFF + 2 );

				std::wstring wsFilter = std::wstring( LSN_LSTR( LSN_AUDIO_OPTIONS_WAV_TYPES ), LSN_ELEMENTS( LSN_LSTR( LSN_AUDIO_OPTIONS_WAV_TYPES ) ) - 1 );
				ofnOpenFile.hwndOwner = Wnd();
				ofnOpenFile.lpstrFilter = wsFilter.c_str();
				ofnOpenFile.lpstrFile = szFileName.data();
				ofnOpenFile.nMaxFile = DWORD( szFileName.size() );
				ofnOpenFile.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
				ofnOpenFile.lpstrInitialDir = m_poOptions->wOutAudioPath.c_str();

				if ( ::GetSaveFileNameW( &ofnOpenFile ) ) {
					m_poOptions->wOutAudioPath = std::filesystem::path( ofnOpenFile.lpstrFile ).remove_filename();
					auto pPath = std::filesystem::path( ofnOpenFile.lpstrFile );
					if ( !pPath.has_extension() ) {
						pPath += ".wav";
					}
					auto aEdit = FindChild( Layout::LSN_AOWI_PAGE_OUT_PATH_EDIT );
					if ( aEdit ) { aEdit->SetTextW( pPath.generic_wstring().c_str() ); }
				}
				break;
			}

			case Layout::LSN_AOWI_PAGE_RAW_META_ALL_CHECK : {
				switch ( _wCtrlCode ) {
					case STN_CLICKED : {
						WORD wCheckMe[] = {
							Layout::LSN_AOWI_PAGE_RAW_META_PULSE1_CHECK,
							Layout::LSN_AOWI_PAGE_RAW_META_PULSE2_CHECK,
							Layout::LSN_AOWI_PAGE_RAW_META_TRI_CHECK,
							Layout::LSN_AOWI_PAGE_RAW_META_NOISE_CHECK,
							Layout::LSN_AOWI_PAGE_RAW_META_DMC_CHECK,
							Layout::LSN_AOWI_PAGE_RAW_META_STATUS_CHECK,
							Layout::LSN_AOWI_PAGE_RAW_META_FRAME_CHECK,
							Layout::LSN_AOWI_PAGE_RAW_META_P1_ON_OFF_CHECK,
							Layout::LSN_AOWI_PAGE_RAW_META_P2_ON_OFF_CHECK,
							Layout::LSN_AOWI_PAGE_RAW_META_TRI_ON_OFF_CHECK,
							Layout::LSN_AOWI_PAGE_RAW_META_NOISE_ON_OFF_CHECK,
							Layout::LSN_AOWI_PAGE_RAW_META_DMC_SET_CHECK,
						};
						for ( auto I = LSN_ELEMENTS( wCheckMe ); I--; ) {
							auto pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( wCheckMe[I] ));
							if ( pcbCheck ) { pcbCheck->SetCheck( TRUE ); }
						}
						break;
					}
				}
				break;
			}
			/*case Layout::LSN_AOWI_PAGE_OUT_META_ALL_CHECK : {
				switch ( _wCtrlCode ) {
					case STN_CLICKED : {
						WORD wCheckMe[] = {
							Layout::LSN_AOWI_PAGE_OUT_META_PULSE1_CHECK,
							Layout::LSN_AOWI_PAGE_OUT_META_PULSE2_CHECK,
							Layout::LSN_AOWI_PAGE_OUT_META_TRI_CHECK,
							Layout::LSN_AOWI_PAGE_OUT_META_NOISE_CHECK,
							Layout::LSN_AOWI_PAGE_OUT_META_DMC_CHECK,
							Layout::LSN_AOWI_PAGE_OUT_META_STATUS_CHECK,
							Layout::LSN_AOWI_PAGE_OUT_META_FRAME_CHECK,
						};
						for ( auto I = LSN_ELEMENTS( wCheckMe ); I--; ) {
							auto pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( wCheckMe[I] ));
							if ( pcbCheck ) { pcbCheck->SetCheck( TRUE ); }
						}
						break;
					}
				}
				break;
			}*/

			case Layout::LSN_AOWI_PAGE_RAW_META_NONE_CHECK : {
				switch ( _wCtrlCode ) {
					case STN_CLICKED : {
						WORD wCheckMe[] = {
							Layout::LSN_AOWI_PAGE_RAW_META_PULSE1_CHECK,
							Layout::LSN_AOWI_PAGE_RAW_META_PULSE2_CHECK,
							Layout::LSN_AOWI_PAGE_RAW_META_TRI_CHECK,
							Layout::LSN_AOWI_PAGE_RAW_META_NOISE_CHECK,
							Layout::LSN_AOWI_PAGE_RAW_META_DMC_CHECK,
							Layout::LSN_AOWI_PAGE_RAW_META_STATUS_CHECK,
							Layout::LSN_AOWI_PAGE_RAW_META_FRAME_CHECK,
							Layout::LSN_AOWI_PAGE_RAW_META_P1_ON_OFF_CHECK,
							Layout::LSN_AOWI_PAGE_RAW_META_P2_ON_OFF_CHECK,
							Layout::LSN_AOWI_PAGE_RAW_META_TRI_ON_OFF_CHECK,
							Layout::LSN_AOWI_PAGE_RAW_META_NOISE_ON_OFF_CHECK,
							Layout::LSN_AOWI_PAGE_RAW_META_DMC_SET_CHECK,
						};
						for ( auto I = LSN_ELEMENTS( wCheckMe ); I--; ) {
							auto pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( wCheckMe[I] ));
							if ( pcbCheck ) { pcbCheck->SetCheck( FALSE ); }
						}
						break;
					}
				}
				break;
			}
			/*case Layout::LSN_AOWI_PAGE_OUT_META_NONE_CHECK : {
				switch ( _wCtrlCode ) {
					case STN_CLICKED : {
						WORD wCheckMe[] = {
							Layout::LSN_AOWI_PAGE_OUT_META_PULSE1_CHECK,
							Layout::LSN_AOWI_PAGE_OUT_META_PULSE2_CHECK,
							Layout::LSN_AOWI_PAGE_OUT_META_TRI_CHECK,
							Layout::LSN_AOWI_PAGE_OUT_META_NOISE_CHECK,
							Layout::LSN_AOWI_PAGE_OUT_META_DMC_CHECK,
							Layout::LSN_AOWI_PAGE_OUT_META_STATUS_CHECK,
							Layout::LSN_AOWI_PAGE_OUT_META_FRAME_CHECK,
						};
						for ( auto I = LSN_ELEMENTS( wCheckMe ); I--; ) {
							auto pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( wCheckMe[I] ));
							if ( pcbCheck ) { pcbCheck->SetCheck( FALSE ); }
						}
						break;
					}
				}
				break;
			}*/
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
	CWidget * CAudioOptionsRecordingPage::Verify( std::wstring &_wsMsg ) {
		std::filesystem::path pRawPath;

		bool bEnabled = true;
		lsw::CCheckButton * pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_ENABLE_CHECK ));
		if ( pcbCheck ) { bEnabled = pcbCheck->IsChecked(); }
		LPARAM lStartCond = CWavFile::LSN_SC_NONE;
		lsw::CComboBox * pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_START_CONDITION_COMBO ));
		if ( pcbCombo ) { lStartCond = pcbCombo->GetCurSelItemData(); }
		LPARAM lEndCond = CWavFile::LSN_EC_NONE;
		pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_STOP_CONDITION_COMBO ));
		if ( pcbCombo ) { lEndCond = pcbCombo->GetCurSelItemData(); }

		uint64_t ui64StartSample = 0;
		uint64_t ui64StopSample = 0;
		double dStartDur = 0.0;
		double dStopDur = 0.0;
		auto aEdit = FindChild( Layout::LSN_AOWI_PAGE_RAW_PATH_EDIT );
		if ( aEdit && bEnabled ) {
			if ( !aEdit->GetTextW().size() ) {
				_wsMsg = LSN_LSTR( LSN_AUDIO_OPTIONS_ERR_INVALID_PATH );
				return aEdit;
			}
			pRawPath = std::filesystem::absolute( aEdit->GetTextW() ).make_preferred();
		}

		ee::CExpEvalContainer::EE_RESULT eTest;
		aEdit = FindChild( Layout::LSN_AOWI_PAGE_RAW_START_COMBO );
		if ( aEdit && bEnabled && lStartCond != CWavFile::LSN_SC_NONE && lStartCond != CWavFile::LSN_SC_FIRST_NON_ZERO ) {
			if ( lStartCond == CWavFile::LSN_SC_START_AT_SAMPLE ) {
				if ( !aEdit->GetTextAsUInt64Expression( eTest ) ) {
					_wsMsg = LSN_LSTR( LSN_AUDIO_OPTIONS_ERR_INVALID_START_COND );
					return aEdit;
				}
				ui64StartSample = eTest.u.ui64Val;
			}
			else {
				if ( !aEdit->GetTextAsDoubleExpression( eTest ) ) {
					_wsMsg = LSN_LSTR( LSN_AUDIO_OPTIONS_ERR_INVALID_START_COND );
					return aEdit;
				}
				dStartDur = eTest.u.dVal;
				if ( dStartDur <= 0.0 ) {
					_wsMsg = LSN_LSTR( LSN_AUDIO_OPTIONS_ERR_INVALID_DURATION );
					return aEdit;
				}
			}
		}
		aEdit = FindChild( Layout::LSN_AOWI_PAGE_RAW_STOP_COMBO );
		if ( aEdit && bEnabled && lEndCond != CWavFile::LSN_EC_NONE ) {
			if ( lEndCond == CWavFile::LSN_EC_END_AT_SAMPLE ) {
				if ( !aEdit->GetTextAsUInt64Expression( eTest ) ) {
					_wsMsg = LSN_LSTR( LSN_AUDIO_OPTIONS_ERR_INVALID_START_COND );
					return aEdit;
				}
				ui64StopSample = eTest.u.ui64Val;
				if ( ui64StopSample <= ui64StartSample ) {
					_wsMsg = LSN_LSTR( LSN_AUDIO_OPTIONS_ERR_INVALID_STOP_SAMPLE );
					return aEdit;
				}
			}
			else {
				if ( !aEdit->GetTextAsDoubleExpression( eTest ) ) {
					_wsMsg = LSN_LSTR( LSN_AUDIO_OPTIONS_ERR_INVALID_END_COND );
					return aEdit;
				}
				dStopDur = eTest.u.dVal;
				if ( dStopDur <= 0.0 ) {
					_wsMsg = LSN_LSTR( LSN_AUDIO_OPTIONS_ERR_INVALID_DURATION );
					return aEdit;
				}
			}
		}


		bEnabled = true;
		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_OUT_ENABLE_CHECK ));
		if ( pcbCheck ) { bEnabled = pcbCheck->IsChecked(); }
		lStartCond = CWavFile::LSN_SC_NONE;
		pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_AOWI_PAGE_OUT_START_CONDITION_COMBO ));
		if ( pcbCombo ) { lStartCond = pcbCombo->GetCurSelItemData(); }
		lEndCond = CWavFile::LSN_EC_NONE;
		pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_AOWI_PAGE_OUT_STOP_CONDITION_COMBO ));
		if ( pcbCombo ) { lEndCond = pcbCombo->GetCurSelItemData(); }

		aEdit = FindChild( Layout::LSN_AOWI_PAGE_OUT_PATH_EDIT );
		if ( aEdit && bEnabled ) {
			if ( !aEdit->GetTextW().size() ) {
				_wsMsg = LSN_LSTR( LSN_AUDIO_OPTIONS_ERR_INVALID_PATH );
				return aEdit;
			}
			if ( !pRawPath.empty() ) {
				if ( pRawPath == std::filesystem::absolute( aEdit->GetTextW() ).make_preferred() ) {
					_wsMsg = LSN_LSTR( LSN_AUDIO_OPTIONS_ERR_SAME_PATHS );
					return aEdit;
				}
			}
		}

		aEdit = FindChild( Layout::LSN_AOWI_PAGE_OUT_START_COMBO );
		if ( aEdit && bEnabled && lStartCond != CWavFile::LSN_SC_NONE && lStartCond != CWavFile::LSN_SC_FIRST_NON_ZERO ) {
			if ( lStartCond == CWavFile::LSN_SC_START_AT_SAMPLE ) {
				if ( !aEdit->GetTextAsUInt64Expression( eTest ) ) {
					_wsMsg = LSN_LSTR( LSN_AUDIO_OPTIONS_ERR_INVALID_START_COND );
					return aEdit;
				}
				ui64StartSample = eTest.u.ui64Val;
			}
			else {
				if ( !aEdit->GetTextAsDoubleExpression( eTest ) ) {
					_wsMsg = LSN_LSTR( LSN_AUDIO_OPTIONS_ERR_INVALID_START_COND );
					return aEdit;
				}
				dStartDur = eTest.u.dVal;
				if ( dStartDur <= 0.0 ) {
					_wsMsg = LSN_LSTR( LSN_AUDIO_OPTIONS_ERR_INVALID_DURATION );
					return aEdit;
				}
			}
		}
		aEdit = FindChild( Layout::LSN_AOWI_PAGE_OUT_STOP_COMBO );
		if ( aEdit && bEnabled && lEndCond != CWavFile::LSN_EC_NONE ) {
			if ( lEndCond == CWavFile::LSN_EC_END_AT_SAMPLE ) {
				if ( !aEdit->GetTextAsUInt64Expression( eTest ) ) {
					_wsMsg = LSN_LSTR( LSN_AUDIO_OPTIONS_ERR_INVALID_START_COND );
					return aEdit;
				}
				ui64StopSample = eTest.u.ui64Val;
				if ( ui64StopSample <= ui64StartSample ) {
					_wsMsg = LSN_LSTR( LSN_AUDIO_OPTIONS_ERR_INVALID_STOP_SAMPLE );
					return aEdit;
				}
			}
			else {
				if ( !aEdit->GetTextAsDoubleExpression( eTest ) ) {
					_wsMsg = LSN_LSTR( LSN_AUDIO_OPTIONS_ERR_INVALID_END_COND );
					return aEdit;
				}
				dStopDur = eTest.u.dVal;
				if ( dStopDur <= 0.0 ) {
					_wsMsg = LSN_LSTR( LSN_AUDIO_OPTIONS_ERR_INVALID_DURATION );
					return aEdit;
				}
			}
		}

		return nullptr;
	}

	/**
	 * Saves the current input configuration and closes the dialog.
	 */
	void CAudioOptionsRecordingPage::Save() {
		lsw::CCheckButton * pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_ENABLE_CHECK ));
		if ( pcbCheck ) { m_poOptions->stfStreamOptionsRaw.bEnabled = pcbCheck->IsChecked(); }
		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_DITHER_CHECK ));
		if ( pcbCheck ) { m_poOptions->stfStreamOptionsRaw.bDither = pcbCheck->IsChecked(); }
		lsw::CComboBox * pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_FORMAT_COMBO ));
		if ( pcbCombo ) { m_poOptions->stfStreamOptionsRaw.fFormat = static_cast<CWavFile::LSN_FORMAT>(pcbCombo->GetCurSelItemData()); }
		pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_START_CONDITION_COMBO ));
		if ( pcbCombo ) { m_poOptions->stfStreamOptionsRaw.scStartCondition = static_cast<CWavFile::LSN_START_CONDITIONS>(pcbCombo->GetCurSelItemData()); }
		pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_STOP_CONDITION_COMBO ));
		if ( pcbCombo ) { m_poOptions->stfStreamOptionsRaw.seEndCondition = static_cast<CWavFile::LSN_END_CONDITIONS>(pcbCombo->GetCurSelItemData()); }
		pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_BITS_COMBO ));
		if ( pcbCombo ) { m_poOptions->stfStreamOptionsRaw.ui32Bits = static_cast<uint32_t>(pcbCombo->GetCurSelItemData()); }

		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_OUT_ENABLE_CHECK ));
		if ( pcbCheck ) { m_poOptions->stfStreamOptionsOutCapture.bEnabled = pcbCheck->IsChecked(); }
		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_OUT_DITHER_CHECK ));
		if ( pcbCheck ) { m_poOptions->stfStreamOptionsOutCapture.bDither = pcbCheck->IsChecked(); }
		pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_AOWI_PAGE_OUT_FORMAT_COMBO ));
		if ( pcbCombo ) { m_poOptions->stfStreamOptionsOutCapture.fFormat = static_cast<CWavFile::LSN_FORMAT>(pcbCombo->GetCurSelItemData()); }
		pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_AOWI_PAGE_OUT_START_CONDITION_COMBO ));
		if ( pcbCombo ) { m_poOptions->stfStreamOptionsOutCapture.scStartCondition = static_cast<CWavFile::LSN_START_CONDITIONS>(pcbCombo->GetCurSelItemData()); }
		pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_AOWI_PAGE_OUT_STOP_CONDITION_COMBO ));
		if ( pcbCombo ) { m_poOptions->stfStreamOptionsOutCapture.seEndCondition = static_cast<CWavFile::LSN_END_CONDITIONS>(pcbCombo->GetCurSelItemData()); }
		pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_AOWI_PAGE_OUT_BITS_COMBO ));
		if ( pcbCombo ) { m_poOptions->stfStreamOptionsOutCapture.ui32Bits = static_cast<uint32_t>(pcbCombo->GetCurSelItemData()); }


		auto aEdit = FindChild( Layout::LSN_AOWI_PAGE_RAW_PATH_EDIT );
		if ( aEdit ) { m_poOptions->stfStreamOptionsRaw.wsPath = aEdit->GetTextW(); }
		aEdit = FindChild( Layout::LSN_AOWI_PAGE_OUT_PATH_EDIT );
		if ( aEdit ) { m_poOptions->stfStreamOptionsOutCapture.wsPath = aEdit->GetTextW(); }


		aEdit = FindChild( Layout::LSN_AOWI_PAGE_RAW_START_COMBO );
		if ( aEdit ) {
			CUtilities::AddOrMove( m_poOptions->vRawStartHistory, aEdit->GetTextW() );
		}
		aEdit = FindChild( Layout::LSN_AOWI_PAGE_OUT_START_COMBO );
		if ( aEdit ) {
			CUtilities::AddOrMove( m_poOptions->vOutStartHistory, aEdit->GetTextW() );
		}

		aEdit = FindChild( Layout::LSN_AOWI_PAGE_RAW_STOP_COMBO );
		if ( aEdit ) {
			CUtilities::AddOrMove( m_poOptions->vRawEndHistory, aEdit->GetTextW() );
		}
		aEdit = FindChild( Layout::LSN_AOWI_PAGE_OUT_STOP_COMBO );
		if ( aEdit ) {
			CUtilities::AddOrMove( m_poOptions->vOutEndHistory, aEdit->GetTextW() );
		}


		if ( m_poOptions->stfStreamOptionsRaw.wsPath.size() ) {
			try {
				auto pPath = std::filesystem::path( m_poOptions->stfStreamOptionsRaw.wsPath );
				if ( !pPath.has_extension() ) {
					pPath += ".wav";
					m_poOptions->stfStreamOptionsRaw.wsPath = pPath.generic_wstring();
				}
			}
			catch ( ... ) {}
		}
		if ( m_poOptions->stfStreamOptionsOutCapture.wsPath.size() ) {
			try {
				auto pPath = std::filesystem::path( m_poOptions->stfStreamOptionsOutCapture.wsPath );
				if ( !pPath.has_extension() ) {
					pPath += ".wav";
					m_poOptions->stfStreamOptionsOutCapture.wsPath = pPath.generic_wstring();
				}
			}
			catch ( ... ) {}
		}
		

		aEdit = FindChild( Layout::LSN_AOWI_PAGE_RAW_START_COMBO );
		switch ( m_poOptions->stfStreamOptionsRaw.scStartCondition ) {
			case CWavFile::LSN_SC_NONE : { break; }
			case CWavFile::LSN_SC_START_AT_SAMPLE : {
				if ( aEdit ) {
					ee::CExpEvalContainer::EE_RESULT eRes;
					if ( aEdit->GetTextAsUInt64Expression( eRes ) ) {
						m_poOptions->stfStreamOptionsRaw.ui64StartParm = eRes.u.ui64Val;
					}
				}
				break;
			}
			case CWavFile::LSN_SC_FIRST_NON_ZERO : { break; }
			case CWavFile::LSN_SC_ZERO_FOR_DURATION : {
				if ( aEdit ) {
					ee::CExpEvalContainer::EE_RESULT eRes;
					if ( aEdit->GetTextAsDoubleExpression( eRes ) ) {
						m_poOptions->stfStreamOptionsRaw.dStartParm = eRes.u.dVal;
					}
				}
				break;
			}
		}
		
		aEdit = FindChild( Layout::LSN_AOWI_PAGE_RAW_STOP_COMBO );
		switch ( m_poOptions->stfStreamOptionsRaw.seEndCondition ) {
			case CWavFile::LSN_EC_NONE : { break; }
			case CWavFile::LSN_EC_END_AT_SAMPLE : {
				if ( aEdit ) {
					ee::CExpEvalContainer::EE_RESULT eRes;
					if ( aEdit->GetTextAsUInt64Expression( eRes ) ) {
						m_poOptions->stfStreamOptionsRaw.ui64EndParm = eRes.u.ui64Val;
					}
				}
				break;
			}
			case CWavFile::LSN_EC_ZERO_FOR_DURATION : {}	LSN_FALLTHROUGH
			case CWavFile::LSN_EC_DURATION : {
				if ( aEdit ) {
					ee::CExpEvalContainer::EE_RESULT eRes;
					if ( aEdit->GetTextAsDoubleExpression( eRes ) ) {
						m_poOptions->stfStreamOptionsRaw.dEndParm = eRes.u.dVal;
					}
				}
				break;
			}
		}



		aEdit = FindChild( Layout::LSN_AOWI_PAGE_OUT_START_COMBO );
		switch ( m_poOptions->stfStreamOptionsOutCapture.scStartCondition ) {
			case CWavFile::LSN_SC_NONE : { break; }
			case CWavFile::LSN_SC_START_AT_SAMPLE : {
				if ( aEdit ) {
					ee::CExpEvalContainer::EE_RESULT eRes;
					if ( aEdit->GetTextAsUInt64Expression( eRes ) ) {
						m_poOptions->stfStreamOptionsOutCapture.ui64StartParm = eRes.u.ui64Val;
					}
				}
				break;
			}
			case CWavFile::LSN_SC_FIRST_NON_ZERO : { break; }
			case CWavFile::LSN_SC_ZERO_FOR_DURATION : {
				if ( aEdit ) {
					ee::CExpEvalContainer::EE_RESULT eRes;
					if ( aEdit->GetTextAsDoubleExpression( eRes ) ) {
						m_poOptions->stfStreamOptionsOutCapture.dStartParm = eRes.u.dVal;
					}
				}
				break;
			}
		}
		
		aEdit = FindChild( Layout::LSN_AOWI_PAGE_OUT_STOP_COMBO );
		switch ( m_poOptions->stfStreamOptionsOutCapture.seEndCondition ) {
			case CWavFile::LSN_EC_NONE : { break; }
			case CWavFile::LSN_EC_END_AT_SAMPLE : {
				if ( aEdit ) {
					ee::CExpEvalContainer::EE_RESULT eRes;
					if ( aEdit->GetTextAsUInt64Expression( eRes ) ) {
						m_poOptions->stfStreamOptionsOutCapture.ui64EndParm = eRes.u.ui64Val;
					}
				}
				break;
			}
			case CWavFile::LSN_EC_ZERO_FOR_DURATION : {}	LSN_FALLTHROUGH
			case CWavFile::LSN_EC_DURATION : {
				if ( aEdit ) {
					ee::CExpEvalContainer::EE_RESULT eRes;
					if ( aEdit->GetTextAsDoubleExpression( eRes ) ) {
						m_poOptions->stfStreamOptionsOutCapture.dEndParm = eRes.u.dVal;
					}
				}
				break;
			}
		}



		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_META_CHECK ));
		if ( pcbCheck ) { m_poOptions->stfStreamOptionsRaw.bMetaEnabled = pcbCheck->IsChecked(); }
		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_OUT_META_CHECK ));
		if ( pcbCheck ) { m_poOptions->stfStreamOptionsOutCapture.bMetaEnabled = pcbCheck->IsChecked(); }


		m_poOptions->stfStreamOptionsRaw.ui64MetaParm = m_poOptions->stfStreamOptionsOutCapture.ui64MetaParm = 0;
		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_META_PULSE1_CHECK ));
		if ( pcbCheck && pcbCheck->IsChecked() ) { m_poOptions->stfStreamOptionsRaw.ui64MetaParm |= LSN_RF_PULSE1; }
		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_OUT_META_PULSE1_CHECK ));
		if ( pcbCheck && pcbCheck->IsChecked() ) { m_poOptions->stfStreamOptionsOutCapture.ui64MetaParm |= LSN_RF_PULSE1; }

		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_META_PULSE2_CHECK ));
		if ( pcbCheck && pcbCheck->IsChecked() ) { m_poOptions->stfStreamOptionsRaw.ui64MetaParm |= LSN_RF_PULSE2; }
		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_OUT_META_PULSE2_CHECK ));
		if ( pcbCheck && pcbCheck->IsChecked() ) { m_poOptions->stfStreamOptionsOutCapture.ui64MetaParm |= LSN_RF_PULSE2; }

		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_META_TRI_CHECK ));
		if ( pcbCheck && pcbCheck->IsChecked() ) { m_poOptions->stfStreamOptionsRaw.ui64MetaParm |= LSN_RF_TRIANGLE; }
		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_OUT_META_TRI_CHECK ));
		if ( pcbCheck && pcbCheck->IsChecked() ) { m_poOptions->stfStreamOptionsOutCapture.ui64MetaParm |= LSN_RF_TRIANGLE; }

		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_META_NOISE_CHECK ));
		if ( pcbCheck && pcbCheck->IsChecked() ) { m_poOptions->stfStreamOptionsRaw.ui64MetaParm |= LSN_RF_NOISE; }
		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_OUT_META_NOISE_CHECK ));
		if ( pcbCheck && pcbCheck->IsChecked() ) { m_poOptions->stfStreamOptionsOutCapture.ui64MetaParm |= LSN_RF_NOISE; }

		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_META_DMC_CHECK ));
		if ( pcbCheck && pcbCheck->IsChecked() ) { m_poOptions->stfStreamOptionsRaw.ui64MetaParm |= LSN_RF_DMC; }
		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_OUT_META_DMC_CHECK ));
		if ( pcbCheck && pcbCheck->IsChecked() ) { m_poOptions->stfStreamOptionsOutCapture.ui64MetaParm |= LSN_RF_DMC; }

		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_META_STATUS_CHECK ));
		if ( pcbCheck && pcbCheck->IsChecked() ) { m_poOptions->stfStreamOptionsRaw.ui64MetaParm |= LSN_RF_STATUS; }
		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_OUT_META_STATUS_CHECK ));
		if ( pcbCheck && pcbCheck->IsChecked() ) { m_poOptions->stfStreamOptionsOutCapture.ui64MetaParm |= LSN_RF_STATUS; }

		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_META_FRAME_CHECK ));
		if ( pcbCheck && pcbCheck->IsChecked() ) { m_poOptions->stfStreamOptionsRaw.ui64MetaParm |= LSN_RF_FRAME_COUNTER; }
		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_OUT_META_FRAME_CHECK ));
		if ( pcbCheck && pcbCheck->IsChecked() ) { m_poOptions->stfStreamOptionsOutCapture.ui64MetaParm |= LSN_RF_FRAME_COUNTER; }

		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_META_P1_ON_OFF_CHECK ));
		if ( pcbCheck && pcbCheck->IsChecked() ) { m_poOptions->stfStreamOptionsRaw.ui64MetaParm |= LSN_RF_PULSE1_ON_OFF; }
		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_META_P2_ON_OFF_CHECK ));
		if ( pcbCheck && pcbCheck->IsChecked() ) { m_poOptions->stfStreamOptionsRaw.ui64MetaParm |= LSN_RF_PULSE2_ON_OFF; }
		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_META_TRI_ON_OFF_CHECK ));
		if ( pcbCheck && pcbCheck->IsChecked() ) { m_poOptions->stfStreamOptionsRaw.ui64MetaParm |= LSN_RF_TRIANGKE_ON_OFF; }
		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_META_NOISE_ON_OFF_CHECK ));
		if ( pcbCheck && pcbCheck->IsChecked() ) { m_poOptions->stfStreamOptionsRaw.ui64MetaParm |= LSN_RF_NOISE_ON_OFF; }
	}

	/**
	 * Updates the dialog.
	 **/
	void CAudioOptionsRecordingPage::Update() {
		bool bEnabled = true;
		lsw::CCheckButton * pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_ENABLE_CHECK ));
		if ( pcbCheck ) { bEnabled = pcbCheck->IsChecked(); }
		LPARAM lFormatSel = CWavFile::LSN_F_PCM;
		lsw::CComboBox * pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_FORMAT_COMBO ));
		if ( pcbCombo ) { lFormatSel = pcbCombo->GetCurSelItemData(); }
		LPARAM lStartCond = CWavFile::LSN_SC_NONE;
		pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_START_CONDITION_COMBO ));
		if ( pcbCombo ) { lStartCond = pcbCombo->GetCurSelItemData(); }
		LPARAM lEndCond = CWavFile::LSN_EC_NONE;
		pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_STOP_CONDITION_COMBO ));
		if ( pcbCombo ) { lEndCond = pcbCombo->GetCurSelItemData(); }
		LPARAM lpBits = 24;
		pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_BITS_COMBO ));
		if ( pcbCombo ) { lpBits = pcbCombo->GetCurSelItemData(); }

		bool bMetaEnabled = false;
		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_META_CHECK ));
		if ( pcbCheck ) { bMetaEnabled = pcbCheck->IsChecked(); }

		

		struct LSN_CONTROLS {
			Layout::LSN_AUDIO_OPTIONS_WINDOW_IDS						wId;
			bool																		bCloseCondition0;
			bool																		bCloseCondition1;
		} cControls[] = {
			{ Layout::LSN_AOWI_PAGE_RAW_PATH_LABEL,					bEnabled,				bEnabled },
			{ Layout::LSN_AOWI_PAGE_RAW_PATH_EDIT,					bEnabled,				bEnabled },
			{ Layout::LSN_AOWI_PAGE_RAW_PATH_BUTTON,				bEnabled,				bEnabled },
			{ Layout::LSN_AOWI_PAGE_RAW_HZ_LABEL,					bEnabled,				bEnabled },
			{ Layout::LSN_AOWI_PAGE_RAW_FORMAT_LABEL,				bEnabled,				bEnabled },
			{ Layout::LSN_AOWI_PAGE_RAW_FORMAT_COMBO,				bEnabled,				bEnabled },
			{ Layout::LSN_AOWI_PAGE_RAW_BITS_LABEL,					bEnabled,				lFormatSel == CWavFile::LSN_F_PCM },
			{ Layout::LSN_AOWI_PAGE_RAW_BITS_COMBO,					bEnabled,				lFormatSel == CWavFile::LSN_F_PCM },
			{ Layout::LSN_AOWI_PAGE_RAW_DITHER_CHECK,				bEnabled,				lFormatSel == CWavFile::LSN_F_PCM && lpBits == 16 },
			{ Layout::LSN_AOWI_PAGE_RAW_START_CONDITION_LABEL,		bEnabled,				bEnabled },
			{ Layout::LSN_AOWI_PAGE_RAW_START_CONDITION_COMBO,		bEnabled,				bEnabled },
			{ Layout::LSN_AOWI_PAGE_RAW_STOP_CONDITION_LABEL,		bEnabled,				bEnabled },
			{ Layout::LSN_AOWI_PAGE_RAW_STOP_CONDITION_COMBO,		bEnabled,				bEnabled },
			{ Layout::LSN_AOWI_PAGE_RAW_START_COMBO,				bEnabled,				lStartCond != CWavFile::LSN_SC_NONE && lStartCond != CWavFile::LSN_SC_FIRST_NON_ZERO },
			{ Layout::LSN_AOWI_PAGE_RAW_STOP_COMBO,					bEnabled,				lEndCond != CWavFile::LSN_EC_NONE },
			{ Layout::LSN_AOWI_PAGE_RAW_META_CHECK,					bEnabled,				bEnabled },
			{ Layout::LSN_AOWI_PAGE_RAW_META_COMBO,					bEnabled,				bMetaEnabled },
			{ Layout::LSN_AOWI_PAGE_RAW_META_ALL_CHECK,				bEnabled,				bMetaEnabled },
			{ Layout::LSN_AOWI_PAGE_RAW_META_NONE_CHECK,			bEnabled,				bMetaEnabled },
			{ Layout::LSN_AOWI_PAGE_RAW_META_PULSE1_CHECK,			bEnabled,				bMetaEnabled },
			{ Layout::LSN_AOWI_PAGE_RAW_META_PULSE2_CHECK,			bEnabled,				bMetaEnabled },
			{ Layout::LSN_AOWI_PAGE_RAW_META_TRI_CHECK,				bEnabled,				bMetaEnabled },
			{ Layout::LSN_AOWI_PAGE_RAW_META_NOISE_CHECK,			bEnabled,				bMetaEnabled },
			{ Layout::LSN_AOWI_PAGE_RAW_META_DMC_CHECK,				bEnabled,				bMetaEnabled },
			{ Layout::LSN_AOWI_PAGE_RAW_META_STATUS_CHECK,			bEnabled,				bMetaEnabled },
			{ Layout::LSN_AOWI_PAGE_RAW_META_FRAME_CHECK,			bEnabled,				bMetaEnabled },
			{ Layout::LSN_AOWI_PAGE_RAW_META_P1_ON_OFF_CHECK,		bEnabled,				bMetaEnabled },
			{ Layout::LSN_AOWI_PAGE_RAW_META_P2_ON_OFF_CHECK,		bEnabled,				bMetaEnabled },
			{ Layout::LSN_AOWI_PAGE_RAW_META_TRI_ON_OFF_CHECK,		bEnabled,				bMetaEnabled },
			{ Layout::LSN_AOWI_PAGE_RAW_META_NOISE_ON_OFF_CHECK,	bEnabled,				bMetaEnabled },
			{ Layout::LSN_AOWI_PAGE_RAW_META_DMC_SET_CHECK,			bEnabled,				bMetaEnabled },
		};
		for ( auto I = LSN_ELEMENTS( cControls ); I--; ) {
			auto pwThis = FindChild( cControls[I].wId );
			if ( pwThis ) {
				pwThis->SetEnabled( cControls[I].bCloseCondition0 && cControls[I].bCloseCondition1 );
			}
		}


		bEnabled = true;
		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_OUT_ENABLE_CHECK ));
		if ( pcbCheck ) { bEnabled = pcbCheck->IsChecked(); }
		lFormatSel = CWavFile::LSN_F_PCM;
		pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_AOWI_PAGE_OUT_FORMAT_COMBO ));
		if ( pcbCombo ) { lFormatSel = pcbCombo->GetCurSelItemData(); }
		lStartCond = CWavFile::LSN_SC_NONE;
		pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_AOWI_PAGE_OUT_START_CONDITION_COMBO ));
		if ( pcbCombo ) { lStartCond = pcbCombo->GetCurSelItemData(); }
		lEndCond = CWavFile::LSN_EC_NONE;
		pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_AOWI_PAGE_OUT_STOP_CONDITION_COMBO ));
		if ( pcbCombo ) { lEndCond = pcbCombo->GetCurSelItemData(); }
		lpBits = 24;
		pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( Layout::LSN_AOWI_PAGE_OUT_BITS_COMBO ));
		if ( pcbCombo ) { lpBits = pcbCombo->GetCurSelItemData(); }

		bMetaEnabled = false;
		pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_OUT_META_CHECK ));
		if ( pcbCheck ) { bMetaEnabled = pcbCheck->IsChecked(); }

		LSN_CONTROLS cOutControls[] = {
			{ Layout::LSN_AOWI_PAGE_OUT_PATH_LABEL,					bEnabled,				bEnabled },
			{ Layout::LSN_AOWI_PAGE_OUT_PATH_EDIT,					bEnabled,				bEnabled },
			{ Layout::LSN_AOWI_PAGE_OUT_PATH_BUTTON,				bEnabled,				bEnabled },
			{ Layout::LSN_AOWI_PAGE_OUT_HZ_LABEL,					bEnabled,				bEnabled },
			{ Layout::LSN_AOWI_PAGE_OUT_FORMAT_LABEL,				bEnabled,				bEnabled },
			{ Layout::LSN_AOWI_PAGE_OUT_FORMAT_COMBO,				bEnabled,				bEnabled },
			{ Layout::LSN_AOWI_PAGE_OUT_BITS_LABEL,					bEnabled,				lFormatSel == CWavFile::LSN_F_PCM },
			{ Layout::LSN_AOWI_PAGE_OUT_BITS_COMBO,					bEnabled,				lFormatSel == CWavFile::LSN_F_PCM },
			{ Layout::LSN_AOWI_PAGE_OUT_DITHER_CHECK,				bEnabled,				lFormatSel == CWavFile::LSN_F_PCM && lpBits == 16 },
			{ Layout::LSN_AOWI_PAGE_OUT_START_CONDITION_LABEL,		bEnabled,				bEnabled },
			{ Layout::LSN_AOWI_PAGE_OUT_START_CONDITION_COMBO,		bEnabled,				bEnabled },
			{ Layout::LSN_AOWI_PAGE_OUT_STOP_CONDITION_LABEL,		bEnabled,				bEnabled },
			{ Layout::LSN_AOWI_PAGE_OUT_STOP_CONDITION_COMBO,		bEnabled,				bEnabled },
			{ Layout::LSN_AOWI_PAGE_OUT_START_COMBO,				bEnabled,				lStartCond != CWavFile::LSN_SC_NONE && lStartCond != CWavFile::LSN_SC_FIRST_NON_ZERO },
			{ Layout::LSN_AOWI_PAGE_OUT_STOP_COMBO,					bEnabled,				lEndCond != CWavFile::LSN_EC_NONE },
			/*{ Layout::LSN_AOWI_PAGE_OUT_META_CHECK,					bEnabled,				bEnabled },
			{ Layout::LSN_AOWI_PAGE_OUT_META_COMBO,					bEnabled,				bMetaEnabled },
			{ Layout::LSN_AOWI_PAGE_OUT_META_ALL_CHECK,				bEnabled,				bMetaEnabled },
			{ Layout::LSN_AOWI_PAGE_OUT_META_NONE_CHECK,			bEnabled,				bMetaEnabled },
			{ Layout::LSN_AOWI_PAGE_OUT_META_PULSE1_CHECK,			bEnabled,				bMetaEnabled },
			{ Layout::LSN_AOWI_PAGE_OUT_META_PULSE2_CHECK,			bEnabled,				bMetaEnabled },
			{ Layout::LSN_AOWI_PAGE_OUT_META_TRI_CHECK,				bEnabled,				bMetaEnabled },
			{ Layout::LSN_AOWI_PAGE_OUT_META_NOISE_CHECK,			bEnabled,				bMetaEnabled },
			{ Layout::LSN_AOWI_PAGE_OUT_META_DMC_CHECK,				bEnabled,				bMetaEnabled },
			{ Layout::LSN_AOWI_PAGE_OUT_META_STATUS_CHECK,			bEnabled,				bMetaEnabled },
			{ Layout::LSN_AOWI_PAGE_OUT_META_FRAME_CHECK,			bEnabled,				bMetaEnabled },*/
		};
		for ( auto I = LSN_ELEMENTS( cOutControls ); I--; ) {
			auto pwThis = FindChild( cOutControls[I].wId );
			if ( pwThis ) {
				pwThis->SetEnabled( cOutControls[I].bCloseCondition0 && cOutControls[I].bCloseCondition1 );
			}
		}



		{
			size_t sCnt = 0;
			WORD wCheckMe[] = {
				Layout::LSN_AOWI_PAGE_RAW_META_PULSE1_CHECK,
				Layout::LSN_AOWI_PAGE_RAW_META_PULSE2_CHECK,
				Layout::LSN_AOWI_PAGE_RAW_META_TRI_CHECK,
				Layout::LSN_AOWI_PAGE_RAW_META_NOISE_CHECK,
				Layout::LSN_AOWI_PAGE_RAW_META_DMC_CHECK,
				Layout::LSN_AOWI_PAGE_RAW_META_STATUS_CHECK,
				Layout::LSN_AOWI_PAGE_RAW_META_FRAME_CHECK,

				Layout::LSN_AOWI_PAGE_RAW_META_P1_ON_OFF_CHECK,
				Layout::LSN_AOWI_PAGE_RAW_META_P2_ON_OFF_CHECK,
				//Layout::LSN_AOWI_PAGE_RAW_META_TRI_ON_OFF_CHECK,
				Layout::LSN_AOWI_PAGE_RAW_META_NOISE_ON_OFF_CHECK,
				//Layout::LSN_AOWI_PAGE_RAW_META_DMC_SET_CHECK,
			};
			for ( auto I = LSN_ELEMENTS( wCheckMe ); I--; ) {
				pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( wCheckMe[I] ));
				if ( pcbCheck && pcbCheck->IsChecked() ) { ++sCnt; }
			}
			pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_META_ALL_CHECK ));
			if ( pcbCheck ) {
				pcbCheck->SetCheck( sCnt == LSN_ELEMENTS( wCheckMe ) );
			}
			pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_RAW_META_NONE_CHECK ));
			if ( pcbCheck ) {
				pcbCheck->SetCheck( sCnt == 0 );
			}
		}
		/*{
			size_t sCnt = 0;
			WORD wCheckMe[] = {
				Layout::LSN_AOWI_PAGE_OUT_META_PULSE1_CHECK,
				Layout::LSN_AOWI_PAGE_OUT_META_PULSE2_CHECK,
				Layout::LSN_AOWI_PAGE_OUT_META_TRI_CHECK,
				Layout::LSN_AOWI_PAGE_OUT_META_NOISE_CHECK,
				Layout::LSN_AOWI_PAGE_OUT_META_DMC_CHECK,
				Layout::LSN_AOWI_PAGE_OUT_META_STATUS_CHECK,
				Layout::LSN_AOWI_PAGE_OUT_META_FRAME_CHECK,
			};
			for ( auto I = LSN_ELEMENTS( wCheckMe ); I--; ) {
				pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( wCheckMe[I] ));
				if ( pcbCheck && pcbCheck->IsChecked() ) { ++sCnt; }
			}
			pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_OUT_META_ALL_CHECK ));
			if ( pcbCheck ) {
				pcbCheck->SetCheck( sCnt == LSN_ELEMENTS( wCheckMe ) );
			}
			pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( Layout::LSN_AOWI_PAGE_OUT_META_NONE_CHECK ));
			if ( pcbCheck ) {
				pcbCheck->SetCheck( sCnt == 0 );
			}
		}*/
	}

	/**
	 * Sets the Hz.
	 * 
	 * \param _ui32Hz The Hz to set.
	 **/
	void CAudioOptionsRecordingPage::SetHz( uint32_t _ui32Hz ) {
		m_ui32SettingsHz = _ui32Hz;

		auto aEdit = FindChild( Layout::LSN_AOWI_PAGE_OUT_HZ_EDIT );
		if ( aEdit ) { aEdit->SetTextW( std::to_wstring( m_ui32SettingsHz ).c_str() ); }
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

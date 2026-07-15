#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A template for dialog pages embedded into other dialogs/windows.
 */

#include "LSNStdControllerPage.h"
#include "../../Crc/LSNCrc.h"
#include "../MainWindow/LSNMainWindow.h"
#include "../WinUtilities/LSNWinUtilities.h"

#include "LSNStdControllerPageLayout.h"
#include <Helpers/LSWHelpers.h>

#include <dbt.h>


namespace lsn {

	// == Members.
	/** Normal presets. */
	LSN_INPUT_EVENT CStdControllerPage::m_ieNormalPresets[LSN_P_TOTAL][IPeripheralBase::LSN_B_TOTAL] = {};

	/** Turbo presets. */
	LSN_INPUT_EVENT CStdControllerPage::m_ieTurboPresets[LSN_P_TOTAL][IPeripheralBase::LSN_B_TOTAL] = {};


	// == Contructors.
	CStdControllerPage::~CStdControllerPage() {
		if ( m_pwListenControl ) {
			StopListening_Keyboard( m_pwListenControl, false );
		}
	}

	// == Functions.
	/**
	 * The WM_INITDIALOG handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CStdControllerPage::InitDialog() {
		CreatePresets();

		CListView * ptControllerList = static_cast<CListView *>(FindChild( Layout::LSN_SCPI_INPUT_DEVICES_LISTVIEW ));
		if ( ptControllerList ) {
			m_iNameCol = ptControllerList->AddColumn( LSN_LSTR( LSN_DEVICE_NAME ) );
			if ( m_iNameCol >= 0 ) {
				ptControllerList->SetColumnWidth( m_iNameCol, 220 );
			}
			m_iTypeCol = ptControllerList->AddColumn( LSN_LSTR( LSN_DEVICE_TYPE_ ) );
			if ( m_iTypeCol >= 0 ) {
				ptControllerList->SetColumnWidth( m_iTypeCol, 100 );
			}
			m_iStatusCol = ptControllerList->AddColumn( LSN_LSTR( LSN_STATUS ) );
			if ( m_iStatusCol >= 0 ) {
				ptControllerList->SetColumnWidth( m_iStatusCol, 50 );
			}
			UpdateInputList();
		}


		std::memset( m_ieMainButtons, 0, sizeof( m_ieMainButtons ) );
		std::memset( m_ieTurboButtons, 0, sizeof( m_ieTurboButtons ) );

		if ( m_pioOptions ) {
			for ( size_t I = 0; I < std::size( m_ieMainButtons ); ++I ) {
				m_ieMainButtons[I] = m_pioOptions->ieButtonMap[m_stPlayerIdx][m_stConfigIdx][I];
			}
			for ( size_t I = 0; I < std::size( m_ieTurboButtons ); ++I ) {
				m_ieTurboButtons[I] = m_pioOptions->ieTurboButtonMap[m_stPlayerIdx][m_stConfigIdx][I];
			}
		}


		std::vector<lsw::CComboBox *> vTurboCombos = TurboCombos();
		std::vector<lsw::CTrackBar *> vDeadzoneTracks = DeadZoneTrackBars();
		UpdateButtons( true );
		for ( size_t I = 0; I < vTurboCombos.size(); ++I ) {
			vTurboCombos[I]->SetUserData( I );

			static const CWinUtilities::LSN_COMBO_ENTRY ceEntries[] = {
				//pwcName																					lpParm
				{ L"Std: 1111000011110000111100001111000011110000111100001111000011110000",					-1,		},
			};
			CWinUtilities::FillComboBox( vTurboCombos[I], ceEntries, std::size( ceEntries ), -1 );
		}
		for ( size_t I = 0; I < vDeadzoneTracks.size(); ++I ) {
			vDeadzoneTracks[I]->SetTicFreq( 5 );
			vDeadzoneTracks[I]->SetPos( TRUE, 20 );
		}

		{
			lsw::CComboBox * pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( CStdControllerPageLayout::LSN_SCPI_QUICK_CONFIGURE_PRESET_COMBO ));
			if ( pcbCombo ) {
				static const CWinUtilities::LSN_COMBO_ENTRY ceEntries[] = {
					//pwcName																					lpParm
					{ LSN_LSTR( LSN_WASD_OP_L_ ),																LSN_P_WASD_OP_L_,					},
					{ LSN_LSTR( LSN_WASD_UI_JK ),																LSN_P_WASD_UI_JK,					},
					{ LSN_LSTR( LSN_ARROW_KEYS_QW_AS ),															LSN_P_ARROW_KEYS_QW_AS,				},
					{ LSN_LSTR( LSN_HORI_SWITCH_Y_B_LAYOUT ),													LSN_P_HORI_SWITCH_Y_B_LAYOUT,		},
					{ LSN_LSTR( LSN_HORI_SWITCH_B_A_LAYOUT ),													LSN_P_HORI_SWITCH_B_A_LAYOUT,		},
					{ LSN_LSTR( LSN_AUDIO_OPTIONS_CUSTOM ),														-1,									},
				};
				CWinUtilities::FillComboBox( pcbCombo, ceEntries, std::size( ceEntries ), -1 );
			}
		}


		UpdateDialog();
		return LSW_H_CONTINUE;
	}

	/**
	 * Called after focus on the control is lost or a key has been registered.
	 *
	 * \param _pwControl The control used for listening.
	 * \param _bSuccess Set to true if the call is due to a key being registered.
	 * \return Returns true if the listening state is LSW_LS_LISTENING and the old control procedure was restored.
	 */
	bool CStdControllerPage::StopListening_Keyboard( CWidget * _pwControl, bool _bSuccess ) {
		if ( m_pucbControllerEventSource ) { return true; }	// It is being handled by the USB Controller path.
		if ( !lsw::CInputListenerBase::StopListening_Keyboard( _pwControl, _bSuccess ) ) { return false; }

		if ( m_pmwMainWindow ) {
			m_pmwMainWindow->LockControllers();
			for ( auto I = m_vControllers.size(); I--; ) {
				m_vControllers[I]->StopThread();
			}
			m_pmwMainWindow->UnlockControllers();
		}

		if ( _bSuccess ) {
			if ( m_stListeningIdx < 8 ) {
				m_ieMainButtons[m_stListeningIdx].dtType = LSN_INPUT_EVENT::LSN_DT_KEYBOARD;
				m_ieMainButtons[m_stListeningIdx].u.kb.kKey = m_kResult;
			}
			else {
				m_ieTurboButtons[m_stListeningIdx%8].dtType = LSN_INPUT_EVENT::LSN_DT_KEYBOARD;
				m_ieTurboButtons[m_stListeningIdx%8].u.kb.kKey = m_kResult;
			}
		}
		else {
			if ( !m_bAutoSet ) {
				if ( m_stListeningIdx < 8 ) {
					m_ieMainButtons[m_stListeningIdx].dtType = LSN_INPUT_EVENT::LSN_DT_KEYBOARD;
					std::memset( &m_ieMainButtons[m_stListeningIdx].u.kb.kKey, 0, sizeof( m_ieMainButtons[m_stListeningIdx].u.kb.kKey ) );
				}
				else {
					m_ieTurboButtons[m_stListeningIdx%8].dtType = LSN_INPUT_EVENT::LSN_DT_KEYBOARD;
					std::memset( &m_ieTurboButtons[m_stListeningIdx%8].u.kb.kKey, 0, sizeof( m_ieTurboButtons[m_stListeningIdx%8].u.kb.kKey ) );
				}
			}
			else if ( m_pwListenControl ) {
				if ( m_stListeningIdx < 8 ) {
					if ( m_ieMainButtons[m_stListeningIdx].dtType == LSN_INPUT_EVENT::LSN_DT_KEYBOARD && m_ieMainButtons[m_stListeningIdx].u.kb.kKey.bKeyCode != 0 ) {
						m_pwListenControl->SetTextW( lsw::CHelpers::ToString( m_ieMainButtons[m_stListeningIdx].u.kb.kKey, false ).c_str() );
					}
					else if ( m_ieMainButtons[m_stListeningIdx].dtType == LSN_INPUT_EVENT::LSN_DT_USB_CONTROLLER ) {
						m_pwListenControl->SetTextW( CUtilities::InputEventToString( m_ieMainButtons[m_stListeningIdx].u.cont.ieEvent ).c_str() );
					}
				}
				else {
					if ( m_ieTurboButtons[m_stListeningIdx%8].dtType == LSN_INPUT_EVENT::LSN_DT_KEYBOARD && m_ieTurboButtons[m_stListeningIdx%8].u.kb.kKey.bKeyCode != 0 ) {
						m_pwListenControl->SetTextW( lsw::CHelpers::ToString( m_ieTurboButtons[m_stListeningIdx%8].u.kb.kKey, false ).c_str() );
					}
					else if ( m_ieTurboButtons[m_stListeningIdx%8].dtType == LSN_INPUT_EVENT::LSN_DT_USB_CONTROLLER ) {
						m_pwListenControl->SetTextW( CUtilities::InputEventToString( m_ieTurboButtons[m_stListeningIdx%8].u.cont.ieEvent ).c_str() );
					}
				}
			}
			m_bAutoSet = false;
		}
		m_pwListenControl = nullptr;
		UpdateDialog();
		NextAutoSet();
		return true;
	}

	/**
	 * Called when a controller input event is generated.
	 * 
	 * \param _pucbController A pointer to the controller that triggered the event.
	 * \param _ieEvent A constant reference to the input event data.
	 **/
	void CStdControllerPage::OnInput( CUsbControllerBase * _pucbController, const CUsbControllerBase::LSN_INPUT_EVENT &_ieEvent ) {
		m_pucbControllerEventSource = _pucbController;
		m_ieControllerEvent = _ieEvent;

		if ( m_pmwMainWindow ) {
			m_pmwMainWindow->LockControllers();
		}
		for ( auto I = m_vControllers.size(); I--; ) {
			m_vControllers[I]->SignalStop();
		}
		if ( m_pmwMainWindow ) {
			m_pmwMainWindow->UnlockControllers();
		}

		::SendNotifyMessageW( Wnd(), CWinUtilities::LSN_USB_CONTROLLER_INPUT, 0, 0 );
	}

	/**
	 * Handles controller input during polling.
	 **/
	void CStdControllerPage::StopListening_Controller() {
		if ( !m_pucbControllerEventSource ) { return; }

		lsw::CInputListenerBase::StopListening_Keyboard( m_pwListenControl, false );
		
		
		if ( m_pmwMainWindow ) {
			m_pmwMainWindow->LockControllers();
		}
		/*for ( auto I = m_vControllers.size(); I--; ) {
			m_vControllers[I]->SignalStop();
		}*/

		for ( auto I = m_vControllers.size(); I--; ) {
			m_vControllers[I]->StopThread();
		}
		auto pucbCopy = m_pucbControllerEventSource;
		m_pucbControllerEventSource = nullptr;

		if ( m_stListeningIdx < 8 ) {
			m_ieMainButtons[m_stListeningIdx].dtType = LSN_INPUT_EVENT::LSN_DT_USB_CONTROLLER;
			m_ieMainButtons[m_stListeningIdx].u.cont.guId = (*pucbCopy->UniqueId());
			m_ieMainButtons[m_stListeningIdx].u.cont.guProductId = (*pucbCopy->ProductId());
			m_ieMainButtons[m_stListeningIdx].u.cont.ieEvent = m_ieControllerEvent;
			m_ieMainButtons[m_stListeningIdx].u.cont.fDeadzone = 0.25f;
		}
		else {
			m_ieTurboButtons[m_stListeningIdx%8].dtType = LSN_INPUT_EVENT::LSN_DT_USB_CONTROLLER;
			m_ieTurboButtons[m_stListeningIdx%8].u.cont.guId = (*pucbCopy->UniqueId());
			m_ieTurboButtons[m_stListeningIdx%8].u.cont.guProductId = (*pucbCopy->ProductId());
			m_ieTurboButtons[m_stListeningIdx%8].u.cont.ieEvent = m_ieControllerEvent;
			m_ieTurboButtons[m_stListeningIdx%8].u.cont.fDeadzone = 0.25f;
		}
		
		if ( m_pmwMainWindow ) {
			m_pmwMainWindow->UnlockControllers();
		}

		auto wsText = CUtilities::InputEventToString( m_ieControllerEvent );
		m_pwListenControl->SetTextW( wsText.c_str() );
		m_pwListenControl = nullptr;
		UpdateDialog();

		NextAutoSet();
	}

	/**
	 * Handles the WM_COMMAND message.
	 *
	 * \param _wCtrlCode 0 = from menu, 1 = from accelerator, otherwise it is a Control-defined notification code.
	 * \param _wId The ID of the control if _wCtrlCode is not 0 or 1.
	 * \param _pwSrc The source control if _wCtrlCode is not 0 or 1.
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CStdControllerPage::Command( WORD _wCtrlCode, WORD _wId, CWidget * _pwSrc ) {
		switch ( _wId ) {
			case Layout::LSN_SCPI_BUTTON_A_BUTTON : {}
			case Layout::LSN_SCPI_BUTTON_B_BUTTON : {}
			case Layout::LSN_SCPI_BUTTON_START_BUTTON : {}
			case Layout::LSN_SCPI_BUTTON_SELECT_BUTTON : {}
			case Layout::LSN_SCPI_BUTTON_UP_BUTTON : {}
			case Layout::LSN_SCPI_BUTTON_LEFT_BUTTON : {}
			case Layout::LSN_SCPI_BUTTON_RIGHT_BUTTON : {}
			case Layout::LSN_SCPI_BUTTON_DOWN_BUTTON : {
				if ( m_pwListenControl ) {
					StopListening_Keyboard( m_pwListenControl, false );
				}
				BeginListening( _pwSrc->GetUserData() & 0xFF, _pwSrc );
				m_bManualListen = true;
				m_bAutoSet = false;
				return LSW_H_HANDLED;
			}
			case Layout::LSN_SCPI_BUTTON_A_TURBO_BUTTON : {}
			case Layout::LSN_SCPI_BUTTON_B_TURBO_BUTTON : {}
			case Layout::LSN_SCPI_BUTTON_START_TURBO_BUTTON : {}
			case Layout::LSN_SCPI_BUTTON_SELECT_TURBO_BUTTON : {}
			case Layout::LSN_SCPI_BUTTON_UP_TURBO_BUTTON : {}
			case Layout::LSN_SCPI_BUTTON_LEFT_TURBO_BUTTON : {}
			case Layout::LSN_SCPI_BUTTON_RIGHT_TURBO_BUTTON : {}
			case Layout::LSN_SCPI_BUTTON_DOWN_TURBO_BUTTON : {
				if ( m_pwListenControl ) {
					StopListening_Keyboard( m_pwListenControl, false );
				}
				BeginListening( (_pwSrc->GetUserData() & 0xFF) + 8, _pwSrc );
				m_bManualListen = true;
				m_bAutoSet = false;
				return LSW_H_HANDLED;
			}
			case Layout::LSN_SCPI_QUICK_CONFIGURE_CLEAR_ALL_BUTTON : {
				if ( m_pwListenControl ) {
					StopListening_Keyboard( m_pwListenControl, false );
				}
				std::memset( m_ieMainButtons, 0, sizeof( m_ieMainButtons ) );
				std::memset( m_ieTurboButtons, 0, sizeof( m_ieTurboButtons ) );
				m_bManualListen = false;
				m_bAutoSet = false;
				m_stListeningIdx = 0;
				{
					auto vButtons = MainButtons();
					for ( auto I = vButtons.size(); I--; ) {
						vButtons[I]->SetTextA( "" );
					}
				}
				{
					auto vButtons = TurboButtons();
					for ( auto I = vButtons.size(); I--; ) {
						vButtons[I]->SetTextA( "" );
					}
				}
				UpdateDialog();
				return LSW_H_HANDLED;
			}
			case Layout::LSN_SCPI_QUICK_CONFIGURE_SET_ALL_BUTTON : {
				auto vButtons = MainButtons();
				m_sAutoListenStart = m_stListeningIdx;
				m_sAutoListenEnd = m_stListeningIdx % vButtons.size();
				if ( m_bManualListen ) { ++m_sAutoListenStart; }
				m_bAutoSet = true;
				m_sAutoListenStart = m_sAutoListenStart % vButtons.size();
				BeginListening( vButtons[m_sAutoListenStart]->GetUserData() & 0xFF, vButtons[m_sAutoListenStart] );
				return LSW_H_HANDLED;
			}
			case Layout::LSN_SCPI_QUICK_CONFIGURE_SET_TURBO_BUTTON : {
				auto vButtons = TurboButtons();
				m_sAutoListenStart = m_stListeningIdx;
				m_sAutoListenEnd = m_stListeningIdx % vButtons.size();
				if ( m_bManualListen ) { ++m_sAutoListenStart; }
				m_bAutoSet = true;
				m_sAutoListenStart = m_sAutoListenStart % vButtons.size();
				BeginListening( (vButtons[m_sAutoListenStart]->GetUserData() & 0xFF) + 8, vButtons[m_sAutoListenStart] );
				return LSW_H_HANDLED;
			}

			// Combo Boxes.
			case CStdControllerPageLayout::LSN_SCPI_QUICK_CONFIGURE_PRESET_COMBO : {
				if ( _wCtrlCode == CBN_SELENDOK ) {
					lsw::CComboBox * pcbCombo = reinterpret_cast<lsw::CComboBox *>(_pwSrc);
					if ( pcbCombo ) {
						LPARAM lpData = pcbCombo->GetCurSelItemData();
						switch ( lpData ) {
							case LSN_P_WASD_OP_L_ : {}						LSN_FALLTHROUGH
							case LSN_P_WASD_UI_JK : {}						LSN_FALLTHROUGH
							case LSN_P_ARROW_KEYS_QW_AS : {}				LSN_FALLTHROUGH
							case LSN_P_HORI_SWITCH_Y_B_LAYOUT : {}			LSN_FALLTHROUGH
							case LSN_P_HORI_SWITCH_B_A_LAYOUT : {
								std::memcpy( m_ieMainButtons, m_ieNormalPresets[lpData], sizeof( m_ieMainButtons ) );
								std::memcpy( m_ieTurboButtons, m_ieTurboPresets[lpData], sizeof( m_ieTurboButtons ) );

								UpdateButtons( false );
								UpdateDialog();
								break;
							}
						}
					}
				}
				break;
			}
		}
		return LSW_H_CONTINUE;
	}

	/**
	 * Handles WM_USER/custom messages.
	 * 
	 * \param _uMsg The message to handle.
	 * \param _wParam Additional message-specific information.
	 * \param _lParam Additional message-specific information.
	 * \return Returns an LSW_HANDLED code.
	 **/
	CWidget::LSW_HANDLED CStdControllerPage::CustomPrivateMsg( UINT _uMsg, WPARAM /*_wParam*/, LPARAM /*_lParam*/ ) {
		switch ( _uMsg ) {
			case CWinUtilities::LSN_USB_CONTROLLER_INPUT : {
				StopListening_Controller();
				break;
			}
		}
		return LSW_H_CONTINUE;
	}

	/**
	 * Fully updates the dialog based on current buttons and settings.
	 */
	void CStdControllerPage::UpdateDialog() {
		/*std::vector<lsw::CButton *> vMainButtons = MainButtons();
		std::vector<lsw::CButton *> vTurboButtons = TurboButtons();*/
		std::vector<lsw::CComboBox *> vTurboCombos = TurboCombos();

		for ( size_t I = 0; I < std::size( m_ieTurboButtons ); ++I ) {
			bool bKeySet = (m_ieTurboButtons[I].dtType == LSN_INPUT_EVENT::LSN_DT_KEYBOARD && m_ieTurboButtons[I].u.kb.kKey.bKeyCode != 0) ||
				m_ieTurboButtons[I].dtType == LSN_INPUT_EVENT::LSN_DT_USB_CONTROLLER;
			vTurboCombos[I]->SetEnabled( bKeySet );
		}

		std::vector<lsw::CTrackBar *> vDeadzoneTracks = DeadZoneTrackBars();
		for ( size_t I = 0; I < vDeadzoneTracks.size(); ++I ) {
			vDeadzoneTracks[I]->SetEnabled( FALSE );
		}

		LPARAM lpPreset = -1;
		for ( size_t I = 0; I < LSN_P_TOTAL; ++I ) {
			bool bMatch = true;
			for ( size_t P = 0; P < IPeripheralBase::LSN_B_TOTAL; ++P ) {
				if ( m_ieMainButtons[P].dtType != m_ieNormalPresets[I][P].dtType ) { bMatch = false; break; }

				if ( m_ieMainButtons[P].dtType == LSN_INPUT_EVENT::LSN_DT_KEYBOARD ) {
					if ( m_ieMainButtons[P].u.kb.kKey.bKeyCode != m_ieNormalPresets[I][P].u.kb.kKey.bKeyCode ) { bMatch = false; break; }
					if ( m_ieMainButtons[P].u.kb.kKey.bKeyModifier != m_ieNormalPresets[I][P].u.kb.kKey.bKeyModifier ) { bMatch = false; break; }
					if ( m_ieMainButtons[P].u.kb.kKey.dwScanCode != m_ieNormalPresets[I][P].u.kb.kKey.dwScanCode ) { bMatch = false; break; }
				}
				else if ( m_ieMainButtons[P].dtType == LSN_INPUT_EVENT::LSN_DT_USB_CONTROLLER ) {
					if ( m_ieMainButtons[P].u.cont.ieEvent.icType != m_ieNormalPresets[I][P].u.cont.ieEvent.icType ) { bMatch = false; break; }
					if ( m_ieMainButtons[P].u.cont.ieEvent.stIdx != m_ieNormalPresets[I][P].u.cont.ieEvent.stIdx ) { bMatch = false; break; }
					if ( std::memcmp( &m_ieMainButtons[P].u.cont.guProductId, &m_ieNormalPresets[I][P].u.cont.guProductId, sizeof( GUID ) ) != 0 ) { bMatch = false; break; }
					switch ( m_ieMainButtons[P].u.cont.ieEvent.icType ) {
						case CUsbControllerBase::LSN_IC_AXIS : {
							if ( m_ieMainButtons[P].u.cont.ieEvent.u.lAxis > 0 && !(m_ieNormalPresets[I][P].u.cont.ieEvent.u.lAxis > 0) ) { bMatch = false; break; }
							break;
						}
						case CUsbControllerBase::LSN_IC_POV : {
							if ( m_ieMainButtons[P].u.cont.ieEvent.u.dwPov != m_ieNormalPresets[I][P].u.cont.ieEvent.u.dwPov ) { bMatch = false; break; }
							break;
						}
					}
				}

				// If it still matches, check the turbo buttons.
				if ( bMatch ) {
					if ( m_ieTurboButtons[P].dtType != m_ieTurboPresets[I][P].dtType ) { bMatch = false; break; }

					if ( m_ieTurboButtons[P].dtType == LSN_INPUT_EVENT::LSN_DT_KEYBOARD ) {
						if ( m_ieTurboButtons[P].u.kb.kKey.bKeyCode != m_ieTurboPresets[I][P].u.kb.kKey.bKeyCode ) { bMatch = false; break; }
						if ( m_ieTurboButtons[P].u.kb.kKey.bKeyModifier != m_ieTurboPresets[I][P].u.kb.kKey.bKeyModifier ) { bMatch = false; break; }
						if ( m_ieTurboButtons[P].u.kb.kKey.dwScanCode != m_ieTurboPresets[I][P].u.kb.kKey.dwScanCode ) { bMatch = false; break; }
					}
					else if ( m_ieTurboButtons[P].dtType == LSN_INPUT_EVENT::LSN_DT_USB_CONTROLLER ) {
						if ( m_ieTurboButtons[P].u.cont.ieEvent.icType != m_ieTurboPresets[I][P].u.cont.ieEvent.icType ) { bMatch = false; break; }
						if ( m_ieTurboButtons[P].u.cont.ieEvent.stIdx != m_ieTurboPresets[I][P].u.cont.ieEvent.stIdx ) { bMatch = false; break; }
						if ( std::memcmp( &m_ieTurboButtons[P].u.cont.guProductId, &m_ieTurboPresets[I][P].u.cont.guProductId, sizeof( GUID ) ) != 0 ) { bMatch = false; break; }
						switch ( m_ieTurboButtons[P].u.cont.ieEvent.icType ) {
							case CUsbControllerBase::LSN_IC_AXIS : {
								if ( m_ieTurboButtons[P].u.cont.ieEvent.u.lAxis > 0 && !(m_ieTurboPresets[I][P].u.cont.ieEvent.u.lAxis > 0) ) { bMatch = false; break; }
								break;
							}
							case CUsbControllerBase::LSN_IC_POV : {
								if ( m_ieTurboButtons[P].u.cont.ieEvent.u.dwPov != m_ieTurboPresets[I][P].u.cont.ieEvent.u.dwPov ) { bMatch = false; break; }
								break;
							}
						}
					}
				}
			}
			if ( bMatch ) {
				lpPreset = static_cast<LPARAM>(I);
				break;
			}
		}

		lsw::CComboBox * pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( CStdControllerPageLayout::LSN_SCPI_QUICK_CONFIGURE_PRESET_COMBO ));
		if ( pcbCombo ) {
			pcbCombo->SetCurSelByItemData( lpPreset );
		}

		/*::RedrawWindow( Wnd(), NULL, NULL,
			RDW_INVALIDATE |
			RDW_ERASE | RDW_UPDATENOW | RDW_ALLCHILDREN );*/
	}

	/**
	 * The WM_DEVICECHANGE handler.
	 * 
	 * \param _wDbtEvent The event that has occurred.  One of the DBT_* values from the Dbt.h header file.
	 * \param _lParam A pointer to a structure that contains event-specific data. Its format depends on the value of the wParam parameter. For more information, refer to the documentation for each event.
	 * \return Returns an LSW_HANDLED code.
	 **/
	CWidget::LSW_HANDLED CStdControllerPage::DeviceChange( WORD _wDbtEvent, LPARAM /*_lParam*/ ) {
		switch ( _wDbtEvent ) {
			case DBT_DEVNODES_CHANGED : {
				UpdateInputList();
				break;
			}
		}
		return LSW_H_CONTINUE;
	}

	/**
	 * Gets an array of the main buttons.  There will be 8 values in the array.
	 * 
	 * \return Returns an array containing the 8 primary buttons.
	 **/
	std::vector<lsw::CButton *> CStdControllerPage::MainButtons() {
		static const WORD wButtons[] = {
			Layout::LSN_SCPI_BUTTON_UP_BUTTON,
			Layout::LSN_SCPI_BUTTON_LEFT_BUTTON,
			Layout::LSN_SCPI_BUTTON_RIGHT_BUTTON,
			Layout::LSN_SCPI_BUTTON_DOWN_BUTTON,
			Layout::LSN_SCPI_BUTTON_SELECT_BUTTON,
			Layout::LSN_SCPI_BUTTON_START_BUTTON,
			Layout::LSN_SCPI_BUTTON_B_BUTTON,
			Layout::LSN_SCPI_BUTTON_A_BUTTON,
		};
		
		try {
			std::vector<lsw::CButton *> vRet;
			for ( size_t I = 0; I < std::size( wButtons ); ++I ) {
				vRet.push_back( static_cast<lsw::CButton *>(FindChild( wButtons[I] )) );
			}
			return vRet;
		}
		catch ( ... ) { return std::vector<lsw::CButton *>(); }
	}

	/**
	 * Gets an array of the turbo buttons.  There will be 8 values in the array.
	 * 
	 * \return Returns an array containing the 8 turbo buttons.
	 **/
	std::vector<lsw::CButton *> CStdControllerPage::TurboButtons() {
		static const WORD wButtons[] = {
			Layout::LSN_SCPI_BUTTON_UP_TURBO_BUTTON,
			Layout::LSN_SCPI_BUTTON_LEFT_TURBO_BUTTON,
			Layout::LSN_SCPI_BUTTON_RIGHT_TURBO_BUTTON,
			Layout::LSN_SCPI_BUTTON_DOWN_TURBO_BUTTON,
			Layout::LSN_SCPI_BUTTON_SELECT_TURBO_BUTTON,
			Layout::LSN_SCPI_BUTTON_START_TURBO_BUTTON,
			Layout::LSN_SCPI_BUTTON_B_TURBO_BUTTON,
			Layout::LSN_SCPI_BUTTON_A_TURBO_BUTTON,
		};
		try {
			std::vector<lsw::CButton *> vRet;
			for ( size_t I = 0; I < std::size( wButtons ); ++I ) {
				vRet.push_back( static_cast<lsw::CButton *>(FindChild( wButtons[I] )) );
			}
			return vRet;
		}
		catch ( ... ) { return std::vector<lsw::CButton *>(); }
	}

	/**
	 * Gets an array of the turbo combos.  There will be 8 values in the array, which will correspond to the 8 turbo buttons returned by TurboButtons().
	 * 
	 * \return Returns an array containing the 8 turbo combos.
	 **/
	std::vector<lsw::CComboBox *> CStdControllerPage::TurboCombos() {
		static const WORD wCombos[] = {
			Layout::LSN_SCPI_BUTTON_UP_TURBO_COMBO,
			Layout::LSN_SCPI_BUTTON_LEFT_TURBO_COMBO,
			Layout::LSN_SCPI_BUTTON_RIGHT_TURBO_COMBO,
			Layout::LSN_SCPI_BUTTON_DOWN_TURBO_COMBO,
			Layout::LSN_SCPI_BUTTON_SELECT_TURBO_COMBO,
			Layout::LSN_SCPI_BUTTON_START_TURBO_COMBO,
			Layout::LSN_SCPI_BUTTON_B_TURBO_COMBO,
			Layout::LSN_SCPI_BUTTON_A_TURBO_COMBO,
		};
		try {
			std::vector<lsw::CComboBox *> vRet;
			for ( size_t I = 0; I < std::size( wCombos ); ++I ) {
				vRet.push_back( static_cast<lsw::CComboBox *>(FindChild( wCombos[I] )) );
			}
			return vRet;
		}
		catch ( ... ) { return std::vector<lsw::CComboBox *>(); }
	}

	/**
	 * Gets an array of the dead-zone trackbars.  There will be 8 values in the array.
	 * 
	 * \return Returns an array containing the 8 dead-zone trackbars.
	 **/
	std::vector<lsw::CTrackBar *> CStdControllerPage::DeadZoneTrackBars() {
		static const WORD wTrackBars[] = {
			Layout::LSN_SCPI_BUTTON_UP_DEADZONE_TRACKBAR,
			Layout::LSN_SCPI_BUTTON_LEFT_DEADZONE_TRACKBAR,
			Layout::LSN_SCPI_BUTTON_RIGHT_DEADZONE_TRACKBAR,
			Layout::LSN_SCPI_BUTTON_DOWN_DEADZONE_TRACKBAR,
			Layout::LSN_SCPI_BUTTON_SELECT_DEADZONE_TRACKBAR,
			Layout::LSN_SCPI_BUTTON_START_DEADZONE_TRACKBAR,
			Layout::LSN_SCPI_BUTTON_B_DEADZONE_TRACKBAR,
			Layout::LSN_SCPI_BUTTON_A_DEADZONE_TRACKBAR,
		};
		try {
			std::vector<lsw::CTrackBar *> vRet;
			for ( size_t I = 0; I < std::size( wTrackBars ); ++I ) {
				vRet.push_back( static_cast<lsw::CTrackBar *>(FindChild( wTrackBars[I] )) );
			}
			return vRet;
		}
		catch ( ... ) { return std::vector<lsw::CTrackBar *>(); }
	}

	/**
	 * Saves the current input configuration.
	 */
	void CStdControllerPage::Save() {
		if ( m_pioOptions ) {
			for ( size_t I = 0; I < std::size( m_ieMainButtons ); ++I ) {
				m_pioOptions->ieButtonMap[m_stPlayerIdx][m_stConfigIdx][I] = m_ieMainButtons[I];
			}
			for ( size_t I = 0; I < std::size( m_ieTurboButtons ); ++I ) {
				m_pioOptions->ieTurboButtonMap[m_stPlayerIdx][m_stConfigIdx][I] = m_ieTurboButtons[I];
			}
		}
	}

	/**
	 * Begins lisening on the given control and control index.
	 * 
	 * \param _sCntrlIdx The control index to which to begin the listen.
	 * \param _pwListeningControl A pointer to the control on which the listen happens.
	 **/
	void CStdControllerPage::BeginListening( size_t _sCntrlIdx, CWidget * _pwListeningControl ) {
		m_stListeningIdx = _sCntrlIdx;
		m_pwListenControl = _pwListeningControl;
		BeginListening_Keyboard( _pwListeningControl );
		if ( m_pmwMainWindow ) {
			m_pmwMainWindow->LockControllers();
			for ( auto I = m_vControllers.size(); I--; ) {
				m_vControllers[I]->BeginThread( this );
			}
			m_pmwMainWindow->UnlockControllers();
		}
	}

	/**
	 * Moves to the next auto-set key or ends auto-set.
	 **/
	void CStdControllerPage::NextAutoSet() {
		if ( m_bAutoSet ) {
			if ( m_stListeningIdx < 8 ) {
				auto vButtons = MainButtons();
				m_sAutoListenStart = (m_sAutoListenStart + 1) % vButtons.size();
				if ( m_sAutoListenStart == m_sAutoListenEnd ) {
					m_bAutoSet = false;
					m_stListeningIdx = m_sAutoListenEnd;
				}
				else {
					vButtons[m_sAutoListenStart]->SetFocus();
					BeginListening( vButtons[m_sAutoListenStart]->GetUserData() & 0xFF, vButtons[m_sAutoListenStart] );
				}
			}
			else {
				auto vButtons = TurboButtons();
				m_sAutoListenStart = (m_sAutoListenStart + 1) % vButtons.size();
				if ( m_sAutoListenStart == m_sAutoListenEnd ) {
					m_bAutoSet = false;
					m_stListeningIdx = m_sAutoListenEnd;
				}
				else {
					vButtons[m_sAutoListenStart]->SetFocus();
					BeginListening( (vButtons[m_sAutoListenStart]->GetUserData() & 0xFF) + 8, vButtons[m_sAutoListenStart] );
				}
			}
		}
	}

	/**
	 * Determines if a controller with the given CRC exists in m_vControllers.
	 * 
	 * \param _ui32Crc The CRC to check for a controller in m_vControllers having.
	 * \return Returns true if a controller in m_vControllers has a GUID with the given CRC32.
	 **/
	bool CStdControllerPage::ControllerHasCrc( uint32_t _ui32Crc ) const {
		for ( auto I = m_vControllers.size(); I--; ) {
			if ( _ui32Crc == GuidToCrc32( (*m_vControllers[I]->UniqueId()) ) ) { return true; }
		}
		return false;
	}

	/**
	 * Updates the main listview with controller and keyboard inputs.
	 **/
	void CStdControllerPage::UpdateInputList() {
		if ( m_pmwMainWindow ) {
			lsw::CListView * ptControllerList = static_cast<lsw::CListView *>(FindChild( Layout::LSN_SCPI_INPUT_DEVICES_LISTVIEW ));
			if ( ptControllerList ) {
				m_pmwMainWindow->LockControllers();

				m_vControllers = m_pmwMainWindow->Controllers();
			
				for ( size_t I = 0; I < m_vControllers.size(); ++I ) {
					// If this item is not in the listview, add it.
					uint32_t ui32Crc = GuidToCrc32( (*m_vControllers[I]->UniqueId()) );
					int32_t i32Idx = FindItemInListView( ptControllerList, ui32Crc );
					if ( -1 == i32Idx ) {
						auto iIdx = ptControllerList->InsertItem( m_vControllers[I]->InstanceName(), ui32Crc );
						if ( iIdx >= 0 ) {
							ptControllerList->SetItemText( iIdx, m_iTypeCol, LSN_LSTR( LSN_USB_CONTROLLER ) );
						}
					}
				}
				auto vKeyboards = CUtilities::GetConnectedKeyboards();
				for ( size_t I = 0; I < vKeyboards.size(); ++I ) {
					auto wsTmp = ee::CExpEval::ToUtf16( vKeyboards[I] );
					int32_t i32Idx = FindItemInListView( ptControllerList, wsTmp );
					if ( -1 == i32Idx ) {
						auto iIdx = ptControllerList->InsertItem( wsTmp.c_str(), 0 );
						if ( iIdx >= 0 ) {
							ptControllerList->SetItemText( iIdx, m_iTypeCol, LSN_LSTR( LSN_KEYBOARD ) );
						}
					}
				}

				// The list contains all items in m_vControllers and possibly more.  Update the status of each item.
				for ( int I = 0; I < ptControllerList->GetItemCount(); ++I ) {
					LPARAM lpData = ptControllerList->GetItemData( I );
					if ( lpData ) {
						ptControllerList->SetItemText( I, m_iStatusCol, HaveControllerWithCrc32( uint32_t( lpData ) ) ? LSN_LSTR( LSN_BALLOT_CHECK ) : LSN_LSTR( LSN_BALLOT_EX ) );
					}
					else {
						// Keyboard.
						ptControllerList->SetItemText( I, m_iStatusCol, true ? LSN_LSTR( LSN_BALLOT_CHECK ) : LSN_LSTR( LSN_BALLOT_EX ) );
					}
				}
			}

			m_pmwMainWindow->UnlockControllers();
		}
	}

	/**
	 * Updates the key buttons with their assignments.
	 * 
	 * \param _bSetUserData If true, user data is also assigned to each button.
	 **/
	void CStdControllerPage::UpdateButtons( bool _bSetUserData ) {
		std::vector<lsw::CButton *> vMainButtons = MainButtons();
		std::vector<lsw::CButton *> vTurboButtons = TurboButtons();
		for ( size_t I = 0; I < vMainButtons.size(); ++I ) {
			if ( _bSetUserData ) { vMainButtons[I]->SetUserData( I ); }
			if ( m_ieMainButtons[I].dtType == LSN_INPUT_EVENT::LSN_DT_KEYBOARD && m_ieMainButtons[I].u.kb.kKey.bKeyCode != 0 ) {
				vMainButtons[I]->SetTextW( lsw::CHelpers::ToString( m_ieMainButtons[I].u.kb.kKey, false ).c_str() );
			}
			else if ( m_ieMainButtons[I].dtType == LSN_INPUT_EVENT::LSN_DT_USB_CONTROLLER ) {
				vMainButtons[I]->SetTextW( CUtilities::InputEventToString( m_ieMainButtons[I].u.cont.ieEvent ).c_str() );
			}
			else {
				vMainButtons[I]->SetTextW( L"" );
			}
		}
		for ( size_t I = 0; I < vTurboButtons.size(); ++I ) {
			if ( _bSetUserData ) { vTurboButtons[I]->SetUserData( I ); }
			if ( m_ieTurboButtons[I].dtType == LSN_INPUT_EVENT::LSN_DT_KEYBOARD && m_ieTurboButtons[I].u.kb.kKey.bKeyCode != 0 ) {
				vTurboButtons[I]->SetTextW( lsw::CHelpers::ToString( m_ieTurboButtons[I].u.kb.kKey, false ).c_str() );
			}
			else if ( m_ieTurboButtons[I].dtType == LSN_INPUT_EVENT::LSN_DT_USB_CONTROLLER ) {
				vTurboButtons[I]->SetTextW( CUtilities::InputEventToString( m_ieTurboButtons[I].u.cont.ieEvent ).c_str() );
			}
			else {
				vTurboButtons[I]->SetTextW( L"" );
			}
		}
	}

	/**
	 * Determines if any entry in m_vControllers has a GUID with a CRC32 matching the given CRC32.
	 * 
	 * \param _ui32Crc The CRC32 to find on any item in m_vControllers.
	 * \return Returns true if any item in m_vControllers has a GUID with the given CRC32.
	 **/
	bool CStdControllerPage::HaveControllerWithCrc32( uint32_t _ui32Crc ) const {
		for ( size_t I = 0; I < m_vControllers.size(); ++I ) {
			if ( _ui32Crc == GuidToCrc32( (*m_vControllers[I]->UniqueId()) ) ) { return true; }
		}
		return false;
	}

	/**
	 * Finds a CListView item with an LPARAM matching the given CRC32.
	 * 
	 * \param _plvListView The listview to scan.
	 * \param _ui32Crc The CRC32 to find among the items in the given listview.
	 * \return Returns the index of the first item with the given CRC32 or -1.
	 **/
	int32_t CStdControllerPage::FindItemInListView( const lsw::CListView * _plvListView, uint32_t _ui32Crc ) {
		if ( !_plvListView ) { return -1; }
		LPARAM lpParm = static_cast<LPARAM>(_ui32Crc);
		for ( int I = 0; I < _plvListView->GetItemCount(); ++I ) {
			if ( lpParm == _plvListView->GetItemData( I ) ) {
				return I;
			}
		}
		return -1;
	}

	/**
	 * Finds a CListView item with a column-0 string matching the given name.
	 * 
	 * \param _plvListView The listview to scan.
	 * \param _wsName The name to find among the items in the given listview.
	 * \return Returns the index of the first item with the given name or -1.
	 **/
	int32_t CStdControllerPage::FindItemInListView( const lsw::CListView * _plvListView, const std::wstring &_wsName ) {
		if ( !_plvListView ) { return -1; }
		std::wstring wsTmp;
		for ( int I = 0; I < _plvListView->GetItemCount(); ++I ) {
			_plvListView->GetItemText( I, 0, wsTmp );
			if ( _wsName == wsTmp ) {
				return I;
			}
		}
		return -1;
	}

	/**
	 * Takes a GUID and gets its CRC32.
	 * 
	 * \param _gId The GUID to convert.
	 * \return Returns the CRC32 of the string form of the given GUID.
	 **/
	uint32_t CStdControllerPage::GuidToCrc32( const GUID &_gId ) {
		auto sStr = CUtilities::FormatGuidToString( _gId );
		return CCrc::GetCrc( reinterpret_cast<const uint8_t *>(sStr.data()), sStr.size() );
	}

	/**
	 * Sets up the presets.
	 **/
	void CStdControllerPage::CreatePresets() {
		static bool bSetUp = false;
		if ( !bSetUp ) {
			bSetUp = true;
			std::memset( m_ieNormalPresets, 0, sizeof( m_ieNormalPresets ) );
			std::memset( m_ieTurboPresets, 0, sizeof( m_ieTurboPresets ) );

#define LSN_SET_KEY( PRESET, IDX, SCAN, KEY )																						\
	m_ieNormalPresets[PRESET][IDX].dtType = LSN_INPUT_EVENT::LSN_DT_KEYBOARD;														\
	m_ieNormalPresets[PRESET][IDX].u.kb.kKey.dwScanCode = SCAN;																		\
	m_ieNormalPresets[PRESET][IDX].u.kb.kKey.bKeyCode = KEY

#define LSN_SET_TKEY( PRESET, IDX, SCAN, KEY )																						\
	m_ieTurboPresets[PRESET][IDX].dtType = LSN_INPUT_EVENT::LSN_DT_KEYBOARD;														\
	m_ieTurboPresets[PRESET][IDX].u.kb.kKey.dwScanCode = SCAN;																		\
	m_ieTurboPresets[PRESET][IDX].u.kb.kKey.bKeyCode = KEY

			LSN_SET_KEY( LSN_P_WASD_OP_L_, IPeripheralBase::LSN_B_UP, 0x00110001, 'W' );
			LSN_SET_KEY( LSN_P_WASD_OP_L_, IPeripheralBase::LSN_B_LEFT, 0x001E0001, 'A' );
			LSN_SET_KEY( LSN_P_WASD_OP_L_, IPeripheralBase::LSN_B_RIGHT, 0x00200001, 'D' );
			LSN_SET_KEY( LSN_P_WASD_OP_L_, IPeripheralBase::LSN_B_DOWN, 0x001F0001, 'S' );
			LSN_SET_KEY( LSN_P_WASD_OP_L_, IPeripheralBase::LSN_B_SELECT, 0x00180001, 'O' );
			LSN_SET_KEY( LSN_P_WASD_OP_L_, IPeripheralBase::LSN_B_START, 0x00190001, 'P' );
			LSN_SET_KEY( LSN_P_WASD_OP_L_, IPeripheralBase::LSN_B_B, 0x00260001, 'L' );
			LSN_SET_KEY( LSN_P_WASD_OP_L_, IPeripheralBase::LSN_B_A, 0x00270001, VK_OEM_1 );

			LSN_SET_TKEY( LSN_P_WASD_OP_L_, IPeripheralBase::LSN_B_UP, 0x00030001, '2' );
			LSN_SET_TKEY( LSN_P_WASD_OP_L_, IPeripheralBase::LSN_B_LEFT, 0x00100001, 'Q' );
			LSN_SET_TKEY( LSN_P_WASD_OP_L_, IPeripheralBase::LSN_B_RIGHT, 0x00120001, 'E' );
			LSN_SET_TKEY( LSN_P_WASD_OP_L_, IPeripheralBase::LSN_B_DOWN, 0x002D0001, 'X' );
			LSN_SET_TKEY( LSN_P_WASD_OP_L_, IPeripheralBase::LSN_B_SELECT, 0x000A0001, '9' );
			LSN_SET_TKEY( LSN_P_WASD_OP_L_, IPeripheralBase::LSN_B_START, 0x000B0001, '0' );
			LSN_SET_TKEY( LSN_P_WASD_OP_L_, IPeripheralBase::LSN_B_B, 0x00340001, VK_OEM_PERIOD );
			LSN_SET_TKEY( LSN_P_WASD_OP_L_, IPeripheralBase::LSN_B_A, 0x00350001, VK_OEM_2 );

			

			LSN_SET_KEY( LSN_P_WASD_UI_JK, IPeripheralBase::LSN_B_UP, 0x00110001, 'W' );
			LSN_SET_KEY( LSN_P_WASD_UI_JK, IPeripheralBase::LSN_B_LEFT, 0x001E0001, 'A' );
			LSN_SET_KEY( LSN_P_WASD_UI_JK, IPeripheralBase::LSN_B_RIGHT, 0x00200001, 'D' );
			LSN_SET_KEY( LSN_P_WASD_UI_JK, IPeripheralBase::LSN_B_DOWN, 0x001F0001, 'S' );
			LSN_SET_KEY( LSN_P_WASD_UI_JK, IPeripheralBase::LSN_B_SELECT, 0x00160001, 'U' );
			LSN_SET_KEY( LSN_P_WASD_UI_JK, IPeripheralBase::LSN_B_START, 0x00170001, 'I' );
			LSN_SET_KEY( LSN_P_WASD_UI_JK, IPeripheralBase::LSN_B_B, 0x00240001, 'J' );
			LSN_SET_KEY( LSN_P_WASD_UI_JK, IPeripheralBase::LSN_B_A, 0x00250001, 'K' );

			LSN_SET_TKEY( LSN_P_WASD_UI_JK, IPeripheralBase::LSN_B_B, 0x00320001, 'M' );
			LSN_SET_TKEY( LSN_P_WASD_UI_JK, IPeripheralBase::LSN_B_A, 0x00270001, VK_OEM_1 );



			LSN_SET_KEY( LSN_P_ARROW_KEYS_QW_AS, IPeripheralBase::LSN_B_UP, 0x01480001, VK_UP );
			LSN_SET_KEY( LSN_P_ARROW_KEYS_QW_AS, IPeripheralBase::LSN_B_LEFT, 0x014B0001, VK_LEFT );
			LSN_SET_KEY( LSN_P_ARROW_KEYS_QW_AS, IPeripheralBase::LSN_B_RIGHT, 0x014D0001, VK_RIGHT );
			LSN_SET_KEY( LSN_P_ARROW_KEYS_QW_AS, IPeripheralBase::LSN_B_DOWN, 0x01500001, VK_DOWN );
			LSN_SET_KEY( LSN_P_ARROW_KEYS_QW_AS, IPeripheralBase::LSN_B_SELECT, 0x00100001, 'Q' );
			LSN_SET_KEY( LSN_P_ARROW_KEYS_QW_AS, IPeripheralBase::LSN_B_START, 0x00110001, 'W' );
			LSN_SET_KEY( LSN_P_ARROW_KEYS_QW_AS, IPeripheralBase::LSN_B_B, 0x001E0001, 'A' );
			LSN_SET_KEY( LSN_P_ARROW_KEYS_QW_AS, IPeripheralBase::LSN_B_A, 0x001F0001, 'S' );

			LSN_SET_TKEY( LSN_P_ARROW_KEYS_QW_AS, IPeripheralBase::LSN_B_B, 0x002C0001, 'Z' );
			LSN_SET_TKEY( LSN_P_ARROW_KEYS_QW_AS, IPeripheralBase::LSN_B_A, 0x002D0001, 'X' );


#define LSN_POV( PRESET, IDX, POV )																	\
	m_ieNormalPresets[PRESET][IDX].dtType = LSN_INPUT_EVENT::LSN_DT_USB_CONTROLLER;					\
	m_ieNormalPresets[PRESET][IDX].u.cont.ieEvent.icType = CUsbControllerBase::LSN_IC_POV;			\
	m_ieNormalPresets[PRESET][IDX].u.cont.ieEvent.stIdx = 0;										\
	m_ieNormalPresets[PRESET][IDX].u.cont.ieEvent.u.dwPov = POV;									\
	std::memcpy( &m_ieNormalPresets[PRESET][IDX].u.cont.guProductId, &guId, sizeof( GUID ) )

#define LSN_BUTTON( PRESET, IDX, BUTTON )															\
	m_ieNormalPresets[PRESET][IDX].dtType = LSN_INPUT_EVENT::LSN_DT_USB_CONTROLLER;					\
	m_ieNormalPresets[PRESET][IDX].u.cont.ieEvent.icType = CUsbControllerBase::LSN_IC_BUTTON;		\
	m_ieNormalPresets[PRESET][IDX].u.cont.ieEvent.stIdx = BUTTON;									\
	std::memcpy( &m_ieNormalPresets[PRESET][IDX].u.cont.guProductId, &guId, sizeof( GUID ) )



#define LSN_TPOV( PRESET, IDX, POV )																\
	m_ieTurboPresets[PRESET][IDX].dtType = LSN_INPUT_EVENT::LSN_DT_USB_CONTROLLER;					\
	m_ieTurboPresets[PRESET][IDX].u.cont.ieEvent.icType = CUsbControllerBase::LSN_IC_POV;			\
	m_ieTurboPresets[PRESET][IDX].u.cont.ieEvent.stIdx = 0;											\
	m_ieTurboPresets[PRESET][IDX].u.cont.ieEvent.u.dwPov = POV;										\
	std::memcpy( &m_ieTurboPresets[PRESET][IDX].u.cont.guProductId, &guId, sizeof( GUID ) )

#define LSN_TBUTTON( PRESET, IDX, BUTTON )															\
	m_ieTurboPresets[PRESET][IDX].dtType = LSN_INPUT_EVENT::LSN_DT_USB_CONTROLLER;					\
	m_ieTurboPresets[PRESET][IDX].u.cont.ieEvent.icType = CUsbControllerBase::LSN_IC_BUTTON;		\
	m_ieTurboPresets[PRESET][IDX].u.cont.ieEvent.stIdx = BUTTON;									\
	std::memcpy( &m_ieTurboPresets[PRESET][IDX].u.cont.guProductId, &guId, sizeof( GUID ) )
			{
				GUID guId = {
					0x00C10F0D,
					0x0000,
					0x0000,
					{
						0x00,
						0x00,
						0x50,
						0x49,
						0x44,
						0x56,
						0x49,
						0x44
					}
				};


				LSN_POV( LSN_P_HORI_SWITCH_Y_B_LAYOUT, IPeripheralBase::LSN_B_UP, 0 );
				LSN_POV( LSN_P_HORI_SWITCH_Y_B_LAYOUT, IPeripheralBase::LSN_B_LEFT, 27000 );
				LSN_POV( LSN_P_HORI_SWITCH_Y_B_LAYOUT, IPeripheralBase::LSN_B_RIGHT, 9000 );
				LSN_POV( LSN_P_HORI_SWITCH_Y_B_LAYOUT, IPeripheralBase::LSN_B_DOWN, 18000 );

				LSN_BUTTON( LSN_P_HORI_SWITCH_Y_B_LAYOUT, IPeripheralBase::LSN_B_SELECT, 13 );
				LSN_BUTTON( LSN_P_HORI_SWITCH_Y_B_LAYOUT, IPeripheralBase::LSN_B_START, 12 );
				LSN_BUTTON( LSN_P_HORI_SWITCH_Y_B_LAYOUT, IPeripheralBase::LSN_B_B, 0 );
				LSN_BUTTON( LSN_P_HORI_SWITCH_Y_B_LAYOUT, IPeripheralBase::LSN_B_A, 1 );

				LSN_TBUTTON( LSN_P_HORI_SWITCH_Y_B_LAYOUT, IPeripheralBase::LSN_B_B, 3 );
				LSN_TBUTTON( LSN_P_HORI_SWITCH_Y_B_LAYOUT, IPeripheralBase::LSN_B_A, 2 );



				LSN_POV( LSN_P_HORI_SWITCH_B_A_LAYOUT, IPeripheralBase::LSN_B_UP, 0 );
				LSN_POV( LSN_P_HORI_SWITCH_B_A_LAYOUT, IPeripheralBase::LSN_B_LEFT, 27000 );
				LSN_POV( LSN_P_HORI_SWITCH_B_A_LAYOUT, IPeripheralBase::LSN_B_RIGHT, 9000 );
				LSN_POV( LSN_P_HORI_SWITCH_B_A_LAYOUT, IPeripheralBase::LSN_B_DOWN, 18000 );

				LSN_BUTTON( LSN_P_HORI_SWITCH_B_A_LAYOUT, IPeripheralBase::LSN_B_SELECT, 13 );
				LSN_BUTTON( LSN_P_HORI_SWITCH_B_A_LAYOUT, IPeripheralBase::LSN_B_START, 12 );

				LSN_BUTTON( LSN_P_HORI_SWITCH_B_A_LAYOUT, IPeripheralBase::LSN_B_B, 1 );
				LSN_BUTTON( LSN_P_HORI_SWITCH_B_A_LAYOUT, IPeripheralBase::LSN_B_A, 2 );

				LSN_TBUTTON( LSN_P_HORI_SWITCH_B_A_LAYOUT, IPeripheralBase::LSN_B_B, 0 );
				LSN_TBUTTON( LSN_P_HORI_SWITCH_B_A_LAYOUT, IPeripheralBase::LSN_B_A, 3 );
			}
#undef LSN_TBUTTON
#undef LSN_TPOV
#undef LSN_SET_TKEY
#undef LSN_SET_KEY
		}
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

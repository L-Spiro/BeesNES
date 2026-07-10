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

	// == Functions.
	/**
	 * The WM_INITDIALOG handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CStdControllerPage::InitDialog() {
		CListView * ptControllerList = static_cast<CListView *>(FindChild( Layout::LSN_SCPI_INPUT_DEVICES_LISTVIEW ));
		if ( ptControllerList && m_pmwMainWindow ) {
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

		std::vector<lsw::CButton *> vMainButtons = MainButtons();
		std::vector<lsw::CButton *> vTurboButtons = TurboButtons();
		std::vector<lsw::CComboBox *> vTurboCombos = TurboCombos();
		std::vector<lsw::CTrackBar *> vDeadzoneTracks = DeadZoneTrackBars();
		for ( size_t I = 0; I < vMainButtons.size(); ++I ) {
			vMainButtons[I]->SetUserData( I );
			if ( m_ieMainButtons[I].dtType == LSN_INPUT_EVENT::LSN_DT_KEYBOARD && m_ieMainButtons[I].u.kb.kKey.bKeyCode != 0 ) {
				vMainButtons[I]->SetTextW( lsw::CHelpers::ToString( m_ieMainButtons[I].u.kb.kKey, false ).c_str() );
			}
			else if ( m_ieMainButtons[I].dtType == LSN_INPUT_EVENT::LSN_DT_USB_CONTROLLER ) {
				vMainButtons[I]->SetTextW( CUtilities::InputEventToString( m_ieMainButtons[I].u.cont.ieEvent ).c_str() );
			}
		}
		for ( size_t I = 0; I < vTurboButtons.size(); ++I ) {
			vTurboButtons[I]->SetUserData( I );
			if ( m_ieTurboButtons[I].dtType == LSN_INPUT_EVENT::LSN_DT_KEYBOARD && m_ieTurboButtons[I].u.kb.kKey.bKeyCode != 0 ) {
				vTurboButtons[I]->SetTextW( lsw::CHelpers::ToString( m_ieTurboButtons[I].u.kb.kKey, false ).c_str() );
			}
			else if ( m_ieTurboButtons[I].dtType == LSN_INPUT_EVENT::LSN_DT_USB_CONTROLLER ) {
				vTurboButtons[I]->SetTextW( CUtilities::InputEventToString( m_ieTurboButtons[I].u.cont.ieEvent ).c_str() );
			}
		}
		for ( size_t I = 0; I < vTurboCombos.size(); ++I ) {
			vTurboCombos[I]->SetUserData( I );

			static const CWinUtilities::LSN_COMBO_ENTRY ceEnries[] = {
				//pwcName																					lpParm
				{ L"Std: 1111000011110000111100001111000011110000111100001111000011110000",					-1,		},
			};
			CWinUtilities::FillComboBox( vTurboCombos[I], ceEnries, std::size( ceEnries ), -1 );
		}
		for ( size_t I = 0; I < vDeadzoneTracks.size(); ++I ) {
			vDeadzoneTracks[I]->SetTicFreq( 5 );
			vDeadzoneTracks[I]->SetPos( TRUE, 20 );
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
		if ( !lsw::CInputListenerBase::StopListening_Keyboard( _pwControl, _bSuccess ) ) { return false; }

		for ( auto I = m_vControllers.size(); I--; ) {
			m_vControllers[I]->StopThread();
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
		UpdateDialog();
		return true;
	}

	/**
	 * Called when a controller input event is generated.
	 * 
	 * \param _pucbController A pointer to the controller that triggered the event.
	 * \param _ieEvent A constant reference to the input event data.
	 **/
	void CStdControllerPage::OnInput( CUsbControllerBase * _pucbController, const CUsbControllerBase::LSN_INPUT_EVENT &_ieEvent ) {
		if ( m_stListeningIdx < 8 ) {
			m_ieMainButtons[m_stListeningIdx].dtType = LSN_INPUT_EVENT::LSN_DT_USB_CONTROLLER;
			m_ieMainButtons[m_stListeningIdx].u.cont.guId = (*_pucbController->UniqueId());
			m_ieMainButtons[m_stListeningIdx].u.cont.ieEvent = _ieEvent;
		}
		else {
			m_ieTurboButtons[m_stListeningIdx%8].dtType = LSN_INPUT_EVENT::LSN_DT_USB_CONTROLLER;
			m_ieTurboButtons[m_stListeningIdx%8].u.cont.guId = (*_pucbController->UniqueId());
			m_ieTurboButtons[m_stListeningIdx%8].u.cont.ieEvent = _ieEvent;
		}
		// Stops the polling threads and updates the dialog.
		StopListening_Keyboard( m_pwListenControl, false );

		auto wsText = CUtilities::InputEventToString( _ieEvent );
		m_pwListenControl->SetTextW( wsText.c_str() );
	}

	/**
	 * Handles the WM_COMMAND message.
	 *
	 * \param _wCtrlCode 0 = from menu, 1 = from accelerator, otherwise it is a Control-defined notification code.
	 * \param _wId The ID of the control if _wCtrlCode is not 0 or 1.
	 * \param _pwSrc The source control if _wCtrlCode is not 0 or 1.
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CStdControllerPage::Command( WORD /*_wCtrlCode*/, WORD _wId, CWidget * _pwSrc ) {
		switch ( _wId ) {
			case Layout::LSN_SCPI_BUTTON_A_BUTTON : {}
			case Layout::LSN_SCPI_BUTTON_B_BUTTON : {}
			case Layout::LSN_SCPI_BUTTON_START_BUTTON : {}
			case Layout::LSN_SCPI_BUTTON_SELECT_BUTTON : {}
			case Layout::LSN_SCPI_BUTTON_UP_BUTTON : {}
			case Layout::LSN_SCPI_BUTTON_LEFT_BUTTON : {}
			case Layout::LSN_SCPI_BUTTON_RIGHT_BUTTON : {}
			case Layout::LSN_SCPI_BUTTON_DOWN_BUTTON : {
				m_stListeningIdx = _pwSrc->GetUserData() & 0xFF;
				m_pwListenControl = _pwSrc;
				BeginListening_Keyboard( _pwSrc );
				for ( auto I = m_vControllers.size(); I--; ) {
					m_vControllers[I]->BeginThread( this );
				}
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
				m_stListeningIdx = (_pwSrc->GetUserData() & 0xFF) + 8;
				m_pwListenControl = _pwSrc;
				BeginListening_Keyboard( _pwSrc );
				for ( auto I = m_vControllers.size(); I--; ) {
					m_vControllers[I]->BeginThread( this );
				}
				return LSW_H_HANDLED;
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
			bool bKeySet = (m_ieTurboButtons[I].dtType == LSN_INPUT_EVENT::LSN_DT_KEYBOARD && m_ieTurboButtons[I].u.kb.kKey.bKeyCode != 0);
			vTurboCombos[I]->SetEnabled( bKeySet );
		}

		std::vector<lsw::CTrackBar *> vDeadzoneTracks = DeadZoneTrackBars();
		for ( size_t I = 0; I < vDeadzoneTracks.size(); ++I ) {
			vDeadzoneTracks[I]->SetEnabled( FALSE );
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

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

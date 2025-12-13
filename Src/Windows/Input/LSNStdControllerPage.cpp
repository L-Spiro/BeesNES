#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A template for dialog pages embedded into other dialogs/windows.
 */

#include "LSNStdControllerPage.h"
#include "../MainWindow/LSNMainWindow.h"
#include "../WinUtilities/LSNWinUtilities.h"
#include "LSNStdControllerPageLayout.h"
#include <Helpers/LSWHelpers.h>
#include <ListView/LSWListView.h>

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
			INT iCol = ptControllerList->AddColumn( LSN_LSTR( LSN_DEVICE_NAME ) );
			iCol = ptControllerList->AddColumn( LSN_LSTR( LSN_DEVICE_TYPE_ ) );
			iCol = ptControllerList->AddColumn( LSN_LSTR( LSN_STATUS ) );
			m_pmwMainWindow->LockControllers();

			auto aControllerList = m_pmwMainWindow->Controllers();
			for ( size_t I = 0; I < aControllerList.size(); ++I ) {
				//ptControllerList->InsertItem( aControllerList[I]->
			}

			m_pmwMainWindow->UnlockControllers();
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
		}
		for ( size_t I = 0; I < vTurboButtons.size(); ++I ) {
			vTurboButtons[I]->SetUserData( I );
			if ( m_ieTurboButtons[I].dtType == LSN_INPUT_EVENT::LSN_DT_KEYBOARD && m_ieTurboButtons[I].u.kb.kKey.bKeyCode != 0 ) {
				vTurboButtons[I]->SetTextW( lsw::CHelpers::ToString( m_ieTurboButtons[I].u.kb.kKey, false ).c_str() );
			}
		}
		for ( size_t I = 0; I < vTurboCombos.size(); ++I ) {
			vTurboCombos[I]->SetUserData( I );

			static const CWinUtilities::LSN_COMBO_ENTRY ceEnries[] = {
				//pwcName																					lpParm
				{ L"Std: 1010101010101010101010101010101010101010101010101010101010101010",					-1,		},
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

		if ( m_stListeningIdx < 8 ) {
			m_ieMainButtons[m_stListeningIdx].dtType = LSN_INPUT_EVENT::LSN_DT_KEYBOARD;
			m_ieMainButtons[m_stListeningIdx].u.kb.kKey = m_kResult;
		}
		else {
			m_ieTurboButtons[m_stListeningIdx%8].dtType = LSN_INPUT_EVENT::LSN_DT_KEYBOARD;
			m_ieTurboButtons[m_stListeningIdx%8].u.kb.kKey = m_kResult;
		}

		UpdateDialog();
		return true;
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
				BeginListening_Keyboard( _pwSrc );
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
				BeginListening_Keyboard( _pwSrc );
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

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

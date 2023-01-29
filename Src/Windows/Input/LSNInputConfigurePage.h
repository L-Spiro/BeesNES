#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The global input settings page.
 */

#pragma once

#include "LSNInputPage.h"
#include "../../Localization/LSNLocalization.h"
#include "../WinUtilities/LSNWinUtilities.h"
#include "LSNControllerSetupWindowLayout.h"
#include <CheckButton/LSWCheckButton.h>


namespace lsn {

	/**
	 * Class CInputConfigurePage
	 * \brief The global input settings page.
	 *
	 * Description: The global input settings page.
	 */
	template <bool _bGlobal>
	class CInputConfigurePage : public CInputPage {
	public :
		CInputConfigurePage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 ) :
			CInputPage( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
			if ( m_poOptions ) {
				if constexpr ( _bGlobal ) {
					m_pioInputOptions = &m_poOptions->ioGlobalInputOptions;
				}
				else {
					m_pioInputOptions = &m_poOptions->ioThisGameInputOptions;
				}
				m_ioTmpOptions = (*m_pioInputOptions);
			}
		}


		// == Functions.
		/**
		 * Get the name of the page.
		 *
		 * \return Returns the name of the page.
		 */
		virtual std::wstring					GetName() const {
			if constexpr ( _bGlobal ) { return LSN_LSTR( LSN_INPUT_GLOBAL_SETTINGS ); }
			else { return LSN_LSTR( LSN_INPUT_PER_GAME_SETTINGS ); }
		}

		/**
		 * The WM_INITDIALOG handler.
		 *
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED						InitDialog() {
			lsw::CComboBox * pcbBox = static_cast<lsw::CComboBox *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( CONSOLE_TYPE_COMBO ) ));
			CWinUtilities::FillComboWithConsoleTypes( pcbBox, m_ioTmpOptions.ui8ConsoleType );
			pcbBox = static_cast<lsw::CComboBox *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( EXPAND_COMBO ) ));
			CWinUtilities::FillComboWithExpansion_Famicom( pcbBox, m_ioTmpOptions.ui8Expansion );
			UpdateConsoleType();
			ApplyOptionsToDialog();
			UpdateDialog();

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
		virtual LSW_HANDLED						Command( WORD _wCtrlCode, WORD _wId, CWidget * /*_pwSrc*/ ) {
			switch ( _wId ) {
				case CInputWindowLayout::LSN_IWI_GLOBAL( CONSOLE_TYPE_COMBO ) : {
					switch ( _wCtrlCode ) {
						case CBN_SELCHANGE : {
							ApplyDialogComboToOption( _wId, m_ioTmpOptions.ui8ConsoleType );
							UpdateConsoleType();
							break;
						}
					}
					break;
				}
#define LSN_UPDATE_COMBO( COMBO_ID, BYTE )																	\
	case CInputWindowLayout::LSN_IWI_GLOBAL( COMBO_ID ) : {													\
		switch ( _wCtrlCode ) {																				\
			case CBN_SELCHANGE : {																			\
				ApplyDialogComboToOption( _wId, m_ioTmpOptions.BYTE );										\
				UpdateDialog();																				\
				break;																						\
			}																								\
		}																									\
		break;																								\
	}
				LSN_UPDATE_COMBO( EXPAND_COMBO, ui8Expansion )
				LSN_UPDATE_COMBO( PLAYER_1_COMBO, ui8Player[0] )
				LSN_UPDATE_COMBO( PLAYER_2_COMBO, ui8Player[1] )
				LSN_UPDATE_COMBO( PLAYER_3_COMBO, ui8Player[2] )
				LSN_UPDATE_COMBO( PLAYER_4_COMBO, ui8Player[3] )
#undef LSN_UPDATE_COMBO
				case CInputWindowLayout::LSN_IWI_GLOBAL( USE_GLOBAL_CHECK ) : {
					switch ( _wCtrlCode ) {
						case STN_CLICKED : {
							ApplyDialogCheckToOption( _wId, m_ioTmpOptions.bUseGlobal );
							UpdateDialog();
							break;
						}
					}
					break;
				}
				case CInputWindowLayout::LSN_IWI_GLOBAL( USE_FOUR_SCORE_CHECK ) : {
					switch ( _wCtrlCode ) {
						case STN_CLICKED : {
							ApplyDialogCheckToOption( _wId, m_ioTmpOptions.bUseFourScore );
							UpdateConsoleType();
							break;
						}
					}
					break;
				}
				case CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_1_BUTTON ) : {
					CControllerSetupWindowLayout::CreateInputDialog( this, (*m_poOptions) );
					break;
				}
			}
			return LSW_H_CONTINUE;
		}

		/**
		 * Saves the current input configuration.
		 */
		virtual void							Save() {
			if ( m_pioInputOptions ) {
				(*m_pioInputOptions) = m_ioTmpOptions;
			}
		}


	protected :
		// == Members.


		// == Functions.
		/**
		 * Updates based off the console type combo box selection
		 */
		void									UpdateConsoleType() {
			lsw::CComboBox * pcbBox = static_cast<lsw::CComboBox *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( CONSOLE_TYPE_COMBO ) ));
			if ( pcbBox ) {
				switch ( pcbBox->GetCurSelItemData() ) {
					case LSN_CT_FAMICOM : {
						SelectFamicom();
						break;
					}
					case LSN_CT_NES : {
						SelectNes();
						break;
					}
				}
			}
		}

		/**
		 * Update the panel with Famicom settings.
		 */
		void									SelectFamicom() {
			lsw::CComboBox * pcbBox = static_cast<lsw::CComboBox *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_1_COMBO ) ));
			if ( pcbBox ) {
				CWinUtilities::FillComboWithControllerType_Famicom( pcbBox );
			}
			pcbBox = static_cast<lsw::CComboBox *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_2_COMBO ) ));
			if ( pcbBox ) {
				CWinUtilities::FillComboWithControllerType_Famicom( pcbBox );
			}
			pcbBox = static_cast<lsw::CComboBox *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_3_COMBO ) ));
			if ( pcbBox ) {
				CWinUtilities::FillComboWithControllerType_FourScore( pcbBox, m_ioTmpOptions.ui8Player[2], LSN_CT_NONE );
			}
			pcbBox = static_cast<lsw::CComboBox *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_4_COMBO ) ));
			if ( pcbBox ) {
				CWinUtilities::FillComboWithControllerType_FourScore( pcbBox, m_ioTmpOptions.ui8Player[3], LSN_CT_NONE );
			}

			UpdateDialog();
		}

		/**
		 * Update the panel with NES settings.
		 */
		void									SelectNes() {
			BOOL bUseFourScore = FALSE;
			lsw::CCheckButton * pUse4Score = static_cast<lsw::CCheckButton *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( USE_FOUR_SCORE_CHECK ) ));
			if ( pUse4Score ) {
				bUseFourScore = pUse4Score->IsChecked();
			}

			lsw::CComboBox * pcbBox;

			if ( bUseFourScore ) {
				pcbBox = static_cast<lsw::CComboBox *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_1_COMBO ) ));
				CWinUtilities::FillComboWithControllerType_FourScore( pcbBox, m_ioTmpOptions.ui8Player[0], LSN_CT_NONE );

				pcbBox = static_cast<lsw::CComboBox *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_2_COMBO ) ));
				CWinUtilities::FillComboWithControllerType_FourScore( pcbBox, m_ioTmpOptions.ui8Player[1], LSN_CT_NONE );

				pcbBox = static_cast<lsw::CComboBox *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_3_COMBO ) ));
				CWinUtilities::FillComboWithControllerType_FourScore( pcbBox, m_ioTmpOptions.ui8Player[2], LSN_CT_NONE );

				pcbBox = static_cast<lsw::CComboBox *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_4_COMBO ) ));
				CWinUtilities::FillComboWithControllerType_FourScore( pcbBox, m_ioTmpOptions.ui8Player[3], LSN_CT_NONE );
			}
			else {
				pcbBox = static_cast<lsw::CComboBox *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_1_COMBO ) ));
				CWinUtilities::FillComboWithControllerType_NES( pcbBox, m_ioTmpOptions.ui8Player[0], LSN_CT_NONE );

				pcbBox = static_cast<lsw::CComboBox *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_2_COMBO ) ));
				CWinUtilities::FillComboWithControllerType_NES( pcbBox, m_ioTmpOptions.ui8Player[1], LSN_CT_NONE );

				pcbBox = static_cast<lsw::CComboBox *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_3_COMBO ) ));
				CWinUtilities::FillComboWithControllerType_NES( pcbBox, m_ioTmpOptions.ui8Player[2], LSN_CT_NONE );

				pcbBox = static_cast<lsw::CComboBox *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_4_COMBO ) ));
				CWinUtilities::FillComboWithControllerType_NES( pcbBox, m_ioTmpOptions.ui8Player[3], LSN_CT_NONE );
			}

			UpdateDialog();
		}

		/**
		 * Fully updates the dialog based on current selections.
		 */
		void									UpdateDialog() {
			bool bEnablePanel = true;
			lsw::CCheckButton * pUseGlobal = static_cast<lsw::CCheckButton *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( USE_GLOBAL_CHECK ) ));
			if ( pUseGlobal ) {
				bEnablePanel = pUseGlobal->IsChecked() == FALSE;
			}

			LPARAM lpConsoleType = LSN_CT_FAMICOM;
			lsw::CComboBox * pcbBox = static_cast<lsw::CComboBox *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( CONSOLE_TYPE_COMBO ) ));
			if ( pcbBox ) {
				lpConsoleType = pcbBox->GetCurSelItemData();
				pcbBox->SetEnabled( bEnablePanel );
			}
			LPARAM lpExpType = LSN_CT_NONE;
			lsw::CComboBox * pcbExpBox = static_cast<lsw::CComboBox *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( EXPAND_COMBO ) ));
			if ( pcbExpBox ) {
				lpExpType = pcbExpBox->GetCurSelItemData();
				pcbExpBox->SetEnabled( bEnablePanel );
			}
			BOOL bUseFourScore = FALSE;
			lsw::CCheckButton * pUse4Score = static_cast<lsw::CCheckButton *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( USE_FOUR_SCORE_CHECK ) ));
			if ( pUse4Score ) {
				bUseFourScore = pUse4Score->IsChecked();
				pUse4Score->SetEnabled( bEnablePanel );
			}
			



			// Visibility settings.
			lsw::CWidget * pwTmp = FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( EXPAND_LABEL ) );
			if ( pwTmp ) {
				pwTmp->SetVisible( lpConsoleType == LSN_CT_FAMICOM );
			}
			pwTmp = FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( EXPAND_COMBO ) );
			if ( pwTmp ) {
				pwTmp->SetVisible( lpConsoleType == LSN_CT_FAMICOM );
			}
			pwTmp = FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( EXPAND_BUTTON ) );
			if ( pwTmp ) {
				pwTmp->SetVisible( lpConsoleType == LSN_CT_FAMICOM );
			}
			pwTmp = FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( USE_FOUR_SCORE_CHECK ) );
			if ( pwTmp ) {
				pwTmp->SetVisible( lpConsoleType == LSN_CT_NES );
			}

			bool bShowRest = (lpConsoleType == LSN_CT_NES && bUseFourScore) || (lpConsoleType == LSN_CT_FAMICOM && lpExpType == LSN_CT_FOUR_PLAYER_ADAPTER);
			pwTmp = FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_3_LABEL ) );
			if ( pwTmp ) {
				pwTmp->SetVisible( bShowRest );
			}
			pwTmp = FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_3_COMBO ) );
			if ( pwTmp ) {
				pwTmp->SetVisible( bShowRest );
			}
			pwTmp = FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_3_BUTTON ) );
			if ( pwTmp ) {
				pwTmp->SetVisible( bShowRest );
			}

			pwTmp = FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_4_LABEL ) );
			if ( pwTmp ) {
				pwTmp->SetVisible( bShowRest );
			}
			pwTmp = FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_4_COMBO ) );
			if ( pwTmp ) {
				pwTmp->SetVisible( bShowRest );
			}
			pwTmp = FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_4_BUTTON ) );
			if ( pwTmp ) {
				pwTmp->SetVisible( bShowRest );
			}


			// Enabled settings.
			pcbBox = static_cast<lsw::CComboBox *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_1_COMBO ) ));
			if ( pcbBox ) {
				bool bEnable = pcbBox->GetCurSelItemData() != LSN_CT_NONE;
				pwTmp = FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_1_BUTTON ) );
				if ( pwTmp ) {
					pwTmp->SetEnabled( bEnable && bEnablePanel );
				}
				pcbBox->SetEnabled( bEnablePanel );
			}
			pcbBox = static_cast<lsw::CComboBox *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_2_COMBO ) ));
			if ( pcbBox ) {
				bool bEnable = pcbBox->GetCurSelItemData() != LSN_CT_NONE;
				pwTmp = FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_2_BUTTON ) );
				if ( pwTmp ) {
					pwTmp->SetEnabled( bEnable && bEnablePanel );
				}
				pcbBox->SetEnabled( bEnablePanel );
			}
			pcbBox = static_cast<lsw::CComboBox *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_3_COMBO ) ));
			if ( pcbBox ) {
				bool bEnable = pcbBox->GetCurSelItemData() != LSN_CT_NONE;
				pwTmp = FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_3_BUTTON ) );
				if ( pwTmp ) {
					pwTmp->SetEnabled( bEnable && bEnablePanel );
				}
				pcbBox->SetEnabled( bEnablePanel );
			}
			pcbBox = static_cast<lsw::CComboBox *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_4_COMBO ) ));
			if ( pcbBox ) {
				bool bEnable = pcbBox->GetCurSelItemData() != LSN_CT_NONE;
				pwTmp = FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_4_BUTTON ) );
				if ( pwTmp ) {
					pwTmp->SetEnabled( bEnable && bEnablePanel );
				}
				pcbBox->SetEnabled( bEnablePanel );
			}
			pcbBox = static_cast<lsw::CComboBox *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( EXPAND_COMBO ) ));
			if ( pcbBox ) {
				bool bEnable = pcbBox->GetCurSelItemData() != LSN_CT_NONE;
				pwTmp = FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( EXPAND_BUTTON ) );
				if ( pwTmp ) {
					pwTmp->SetEnabled( bEnable && bEnablePanel );
				}
			}

			pwTmp = FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_1_LABEL ) );
			if ( pwTmp ) {
				pwTmp->SetEnabled( bEnablePanel );
			}
			pwTmp = FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_2_LABEL ) );
			if ( pwTmp ) {
				pwTmp->SetEnabled( bEnablePanel );
			}
			pwTmp = FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_3_LABEL ) );
			if ( pwTmp ) {
				pwTmp->SetEnabled( bEnablePanel );
			}
			pwTmp = FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_4_LABEL ) );
			if ( pwTmp ) {
				pwTmp->SetEnabled( bEnablePanel );
			}
			pwTmp = FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( EXPAND_LABEL ) );
			if ( pwTmp ) {
				pwTmp->SetEnabled( bEnablePanel );
			}
			pwTmp = FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( SETTINGS_GROUP ) );
			if ( pwTmp ) {
				pwTmp->SetEnabled( bEnablePanel );
			}
			pwTmp = FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( CONSOLE_TYPE_LABEL ) );
			if ( pwTmp ) {
				pwTmp->SetEnabled( bEnablePanel );
			}
		}

		/**
		 * Applies the options to the dialog boxs/checks.
		 */
		void									ApplyOptionsToDialog() {
			if ( !m_pioInputOptions ) { return; }
			lsw::CCheckButton * pUseGlobal = static_cast<lsw::CCheckButton *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( USE_GLOBAL_CHECK ) ));
			if ( pUseGlobal ) {
				pUseGlobal->SetCheck( m_ioTmpOptions.bUseGlobal ? TRUE : FALSE );
			}
			pUseGlobal = static_cast<lsw::CCheckButton *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( USE_FOUR_SCORE_CHECK ) ));
			if ( pUseGlobal ) {
				pUseGlobal->SetCheck( m_ioTmpOptions.bUseFourScore ? TRUE : FALSE );
			}

			lsw::CComboBox * pcbBox = static_cast<lsw::CComboBox *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( CONSOLE_TYPE_COMBO ) ));
			if ( pcbBox ) {
				INT iSel = pcbBox->SetCurSelByItemData( m_ioTmpOptions.ui8ConsoleType );
				if ( CB_ERR == iSel ) {
					pcbBox->SetCurSel( 0 );
					m_ioTmpOptions.ui8ConsoleType = uint8_t( pcbBox->GetCurSelItemData() );
				}
			}

			pcbBox = static_cast<lsw::CComboBox *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( EXPAND_COMBO ) ));
			if ( pcbBox ) {
				INT iSel = pcbBox->SetCurSelByItemData( m_ioTmpOptions.ui8Expansion );
				if ( CB_ERR == iSel ) {
					pcbBox->SetCurSel( 0 );
					m_ioTmpOptions.ui8Expansion = uint8_t( pcbBox->GetCurSelItemData() );
				}
			}

			pcbBox = static_cast<lsw::CComboBox *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_1_COMBO ) ));
			if ( pcbBox ) {
				INT iSel = pcbBox->SetCurSelByItemData( m_ioTmpOptions.ui8Player[0] );
				if ( CB_ERR == iSel ) {
					pcbBox->SetCurSel( 0 );
					m_ioTmpOptions.ui8Player[0] = uint8_t( pcbBox->GetCurSelItemData() );
				}
			}
			pcbBox = static_cast<lsw::CComboBox *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_2_COMBO ) ));
			if ( pcbBox ) {
				INT iSel = pcbBox->SetCurSelByItemData( m_ioTmpOptions.ui8Player[1] );
				if ( CB_ERR == iSel ) {
					pcbBox->SetCurSel( 0 );
					m_ioTmpOptions.ui8Player[1] = uint8_t( pcbBox->GetCurSelItemData() );
				}
			}
			pcbBox = static_cast<lsw::CComboBox *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_3_COMBO ) ));
			if ( pcbBox ) {
				INT iSel = pcbBox->SetCurSelByItemData( m_ioTmpOptions.ui8Player[2] );
				if ( CB_ERR == iSel ) {
					pcbBox->SetCurSel( 0 );
					m_ioTmpOptions.ui8Player[2] = uint8_t( pcbBox->GetCurSelItemData() );
				}
			}
			pcbBox = static_cast<lsw::CComboBox *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_4_COMBO ) ));
			if ( pcbBox ) {
				INT iSel = pcbBox->SetCurSelByItemData( m_ioTmpOptions.ui8Player[3] );
				if ( CB_ERR == iSel ) {
					pcbBox->SetCurSel( 0 );
					m_ioTmpOptions.ui8Player[3] = uint8_t( pcbBox->GetCurSelItemData() );
				}
			}
		}

		/**
		 * Gets the current selection from the given combo box (by ID) and writes it to the given byte.
		 *
		 * \param _wId The combo box ID.
		 * \param _ui8Data The byte to which to write the combo box’s selected data.
		 */
		void									ApplyDialogComboToOption( WORD _wId, uint8_t &_ui8Data ) {
			lsw::CComboBox * pcbBox = static_cast<lsw::CComboBox *>(FindChild( _wId ));
			if ( pcbBox ) {
				_ui8Data = static_cast<uint8_t>(pcbBox->GetCurSelItemData());
			}
		}

		/**
		 * Gets the current check status from the given check box (by ID) and writes it to the given byte.
		 *
		 * \param _wId The check box ID.
		 * \param _bData The bool to which to write the check box’s selected data.
		 */
		void									ApplyDialogCheckToOption( WORD _wId, bool &_bData ) {
			lsw::CCheckButton * pcbCheck = static_cast<lsw::CCheckButton *>(FindChild( _wId ));
			if ( pcbCheck ) {
				_bData = pcbCheck->IsChecked() != FALSE;
			}
		}

#if 0
		/**
		 * The WM_CTLCOLORSTATIC handler.
		 *
		 * \param _hDc Handle to the device context for the static control window
		 * \param _pwControl Handle to the static control.
		 * \param _hBrush If an application processes this message, the return value is a handle to a brush that the system uses to paint the background of the static control.
		 * \return Returns a LSW_HANDLED enumeration.
		 */
		virtual LSW_HANDLED						CtlColorStatic( HDC /*_hDc*/, CWidget * /*_pwControl*/, HBRUSH &_hBrush ) {
			_hBrush = ::GetSysColorBrush( COLOR_3DFACE );
			return LSW_H_HANDLED;
		}
#endif	// #if 0

#if 0
		/**
		 * The WM_CTLCOLORDLG handler.
		 *
		 * \param _hDc Handle to the device context for the dialog box
		 * \param _pwControl Handle to the dialog box.
		 * \param _hBrush If an application processes this message, the return value is a handle to a brush that the system uses to paint the background of the dialog box.
		 * \return Returns a LSW_HANDLED enumeration.
		 */
		virtual LSW_HANDLED						CtlColorDlg( HDC /*_hDc*/, CWidget * /*_pwControl*/, HBRUSH &_hBrush ) {
			_hBrush = ::GetSysColorBrush( COLOR_3DFACE );
			return LSW_H_HANDLED;
		}
#endif	// #if 0
	};

}	// namespace lsn


#endif	// #ifdef LSN_USE_WINDOWS

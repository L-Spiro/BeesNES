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
			CWinUtilities::FillComboWithConsoleTypes( pcbBox, LSN_CT_FAMICOM );
			pcbBox = static_cast<lsw::CComboBox *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( EXPAND_COMBO ) ));
			CWinUtilities::FillComboWithExpansion_Famicom( pcbBox, LSN_CT_NONE );
			UpdateConsoleType();

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
							UpdateConsoleType();
							break;
						}
					}
					break;
				}
				case CInputWindowLayout::LSN_IWI_GLOBAL( EXPAND_COMBO ) : {}
				case CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_1_COMBO ) : {}
				case CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_2_COMBO ) : {}
				case CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_3_COMBO ) : {}
				case CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_4_COMBO ) : {
					switch ( _wCtrlCode ) {
						case CBN_SELCHANGE : {
							UpdateDialog();
							break;
						}
					}
					break;
				}
				case CInputWindowLayout::LSN_IWI_GLOBAL( USE_GLOBAL_CHECK ) : {
					switch ( _wCtrlCode ) {
						case STN_CLICKED : {
							UpdateDialog();
							break;
						}
					}
					break;
				}
				case CInputWindowLayout::LSN_IWI_GLOBAL( USE_FOUR_SCORE_CHECK ) : {
					switch ( _wCtrlCode ) {
						case STN_CLICKED : {
							UpdateConsoleType();
							break;
						}
					}
					break;
				}
			}
			return LSW_H_CONTINUE;
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
				CWinUtilities::FillComboWithControllerType_FourScore( pcbBox, LSN_CT_NONE, LSN_CT_NONE );

				pcbBox = static_cast<lsw::CComboBox *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_2_COMBO ) ));
				CWinUtilities::FillComboWithControllerType_FourScore( pcbBox, LSN_CT_NONE, LSN_CT_NONE );

				pcbBox = static_cast<lsw::CComboBox *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_3_COMBO ) ));
				CWinUtilities::FillComboWithControllerType_FourScore( pcbBox, LSN_CT_NONE, LSN_CT_NONE );

				pcbBox = static_cast<lsw::CComboBox *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_4_COMBO ) ));
				CWinUtilities::FillComboWithControllerType_FourScore( pcbBox, LSN_CT_NONE, LSN_CT_NONE );
			}
			else {
				pcbBox = static_cast<lsw::CComboBox *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_1_COMBO ) ));
				CWinUtilities::FillComboWithControllerType_NES( pcbBox, LSN_CT_NONE, LSN_CT_NONE );

				pcbBox = static_cast<lsw::CComboBox *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_2_COMBO ) ));
				CWinUtilities::FillComboWithControllerType_NES( pcbBox, LSN_CT_NONE, LSN_CT_NONE );

				/*pcbBox = static_cast<lsw::CComboBox *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_3_COMBO ) ));
				CWinUtilities::FillComboWithControllerType_NES( pcbBox, LSN_CT_NONE );

				pcbBox = static_cast<lsw::CComboBox *>(FindChild( CInputWindowLayout::LSN_IWI_GLOBAL( PLAYER_4_COMBO ) ));
				CWinUtilities::FillComboWithControllerType_NES( pcbBox, LSN_CT_NONE );*/
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
	};

}	// namespace lsn


#endif	// #ifdef LSN_USE_WINDOWS

#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Utility functions relating to the OS.
 */

#include "LSNWinUtilities.h"
#include "../../Localization/LSNLocalization.h"

namespace lsn {

	/**
	 * Fills a combo box with console types (Nintendo Entertainment System® and Famicom®).
	 *
	 * \param _pwComboBox The combo box to fill.
	 * \param _lpDefaultSelect The default selection.
	 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
	 */
	bool CWinUtilities::FillComboWithConsoleTypes( CWidget * _pwComboBox, LPARAM _lpDefaultSelect ) {
		LSN_COMBO_ENTRY ceEnries[] = {
			//pwcName													lpParm
			{ LSN_LSTR( LSN_NES ),										LSN_CT_NES,					},
			{ LSN_LSTR( LSN_FAMICOM ),									LSN_CT_FAMICOM,				},
		};
		return FillComboBox( _pwComboBox, ceEnries, LSN_ELEMENTS( ceEnries ), _lpDefaultSelect, LSN_CT_NES );
	}

	/**
	 * Fills a combo box with Famicom® controller types (Standard Controller only).
	 *
	 * \param _pwComboBox The combo box to fill.
	 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
	 */
	bool CWinUtilities::FillComboWithControllerType_Famicom( CWidget * _pwComboBox ) {
		LSN_COMBO_ENTRY ceEnries[] = {
			//pwcName													lpParm
			{ LSN_LSTR( LSN_INPUT_STANDARD_CONTROLLER ),				LSN_CT_STANDARD,				},
		};
		return FillComboBox( _pwComboBox, ceEnries, LSN_ELEMENTS( ceEnries ), LSN_CT_STANDARD );
	}

	/**
	 * Fills a combo box with Nintendo Entertainment System® controller types.
	 *
	 * \param _pwComboBox The combo box to fill.
	 * \param _lpDefaultSelect The default selection.
	 * \param _lpSelectBackup The backup selection in case the default selection isn't in the list.
	 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
	 */
	bool CWinUtilities::FillComboWithControllerType_NES( CWidget * _pwComboBox, LPARAM _lpDefaultSelect, LPARAM _lpSelectBackup ) {
		LSN_COMBO_ENTRY ceEnries[] = {
			//pwcName													lpParm
			{ LSN_LSTR( LSN_INPUT_NONE ),								LSN_CT_NONE,				},
			{ LSN_LSTR( LSN_INPUT_STANDARD_CONTROLLER ),				LSN_CT_STANDARD,			},
		};
		return FillComboBox( _pwComboBox, ceEnries, LSN_ELEMENTS( ceEnries ), _lpDefaultSelect, _lpSelectBackup );
	}

	/**
	 * Fills a combo box with Nintendo Entertainment System® Four Score expansion types.
	 *
	 * \param _pwComboBox The combo box to fill.
	 * \param _lpDefaultSelect The default selection.
	 * \param _lpSelectBackup The backup selection in case the default selection isn't in the list.
	 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
	 */
	bool CWinUtilities::FillComboWithControllerType_FourScore( CWidget * _pwComboBox, LPARAM _lpDefaultSelect, LPARAM _lpSelectBackup ) {
		LSN_COMBO_ENTRY ceEnries[] = {
			//pwcName													lpParm
			{ LSN_LSTR( LSN_INPUT_NONE ),								LSN_CT_NONE,				},
			{ LSN_LSTR( LSN_INPUT_STANDARD_CONTROLLER ),				LSN_CT_STANDARD,			},
			/*{ LSN_LSTR( LSN_INPUT_SNES_MOUSE ),							LSN_CT_SNES_MOUSE,			},
			{ LSN_LSTR( LSN_INPUT_SUBOR_MOUSE ),						LSN_CT_SUBOR_MOUSE,			},*/
		};
		return FillComboBox( _pwComboBox, ceEnries, LSN_ELEMENTS( ceEnries ), _lpDefaultSelect, _lpSelectBackup );
	}

	/**
	 * Fills a combo box with Famicom® expansion types.
	 *
	 * \param _pwComboBox The combo box to fill.
	 * \param _lpDefaultSelect The default selection.
	 * \param _lpSelectBackup The backup selection in case the default selection isn't in the list.
	 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
	 */
	bool CWinUtilities::FillComboWithExpansion_Famicom( CWidget * _pwComboBox, LPARAM _lpDefaultSelect ) {
		LSN_COMBO_ENTRY ceEnries[] = {
			//pwcName													lpParm
			{ LSN_LSTR( LSN_INPUT_NONE ),								LSN_CT_NONE,								},
			{ LSN_LSTR( LSN_INPUT_ARKANOID_CONTROLLER ),				LSN_CT_ARKANOID,							},
			{ LSN_LSTR( LSN_INPUT_BANDAI_HYPER_SHOT ),					LSN_CT_BANDAI_HYPERSHOT,					},
			{ LSN_LSTR( LSN_INPUT_BARCODE_BATTLER ),					LSN_CT_BARCODE_BATTLER,						},
			{ LSN_LSTR( LSN_INPUT_BATTLE_BOX ),							LSN_CT_BATTLE_BOX,							},
			{ LSN_LSTR( LSN_INPUT_EXCITING_BOXING_PUNCHING_BAG ),		LSN_CT_EXCITING_BOXING_PUNCHING_BAG,		},
			{ LSN_LSTR( LSN_INPUT_FAMILY_BASIC_KEYBOARD ),				LSN_CT_FAMILY_BASIC_KEYBOARD,				},
			{ LSN_LSTR( LSN_INPUT_FAMILY_TRAINER ),						LSN_CT_FAMILY_TRAINER,						},
			{ LSN_LSTR( LSN_INPUT_4_PLAYER_ADAPTOR ),					LSN_CT_FOUR_PLAYER_ADAPTER,					},
			{ LSN_LSTR( LSN_INPUT_HORI_TRACK ),							LSN_CT_HORI_TRACK,							},
			{ LSN_LSTR( LSN_INPUT_JISSEN_MAHJONG_CONTROLLER ),			LSN_CT_JISSEN_MAHJONG_CONTROLLER,			},
			{ LSN_LSTR( LSN_INPUT_KONAMI_HYPER_SHOT ),					LSN_CT_KONAMI_HYPER_SHOT,					},
			{ LSN_LSTR( LSN_INPUT_OEKA_KIDS_TABLET ),					LSN_CT_OEKA_KIDS_TABLET,					},
			{ LSN_LSTR( LSN_INPUT_PACHINKO_CONTROLLER ),				LSN_CT_PACHINKO_CONTROLLER,					},
			{ LSN_LSTR( LSN_INPUT_PARTYTAP ),							LSN_CT_PARTYTAP,							},
			{ LSN_LSTR( LSN_INPUT_SUBOR_KEYBOARD ),						LSN_CT_SUBOR_KEYBOARD,						},
			{ LSN_LSTR( LSN_INPUT_TURBO_FILE ),							LSN_CT_TURBO_FILE,							},
			{ LSN_LSTR( LSN_INPUT_ZAPPER ),								LSN_CT_ZAPPER,								},
		};
		return FillComboBox( _pwComboBox, ceEnries, LSN_ELEMENTS( ceEnries ), _lpDefaultSelect, LSN_CT_NONE );
	}

	/**
	 * Fills a combo box with the given array of LSN_COMBO_ENTRY structures.
	 *
	 * \param _pwComboBox The combo box to fill.
	 * \param _pceEntries The array of combo-box entries.
	 * \param _stTotal The total number of entries to which _pceEntries points.
	 * \param _lpDefaultSelect The default selection.
	 * \param _lpSelectBackup The backup selection in case the default selection isn't in the list.
	 * \return Returns true if _pwComboBox is not nullptr, it is of type CComboBox, and all entries were added.
	 */
	bool CWinUtilities::FillComboBox( CWidget * _pwComboBox, const LSN_COMBO_ENTRY * _pceEntries, size_t _stTotal, LPARAM _lpDefaultSelect, LPARAM _lpSelectBackup ) {
		if ( nullptr == _pwComboBox ) { return false; }
		if ( !_pwComboBox->IsComboBox() ) { return false; }

		CComboBox * pcbBox = static_cast<CComboBox *>(_pwComboBox);
		pcbBox->ResetContent();
		for ( size_t I = 0; I < _stTotal; ++I ) {
			INT iIdx = pcbBox->AddString( _pceEntries[I].pwcName );
			if ( CB_ERR == iIdx ) { return false; }
			if ( CB_ERR == pcbBox->SetItemData( iIdx, _pceEntries[I].lpParm ) ) { return false; }
		}
		if ( CB_ERR == pcbBox->SetCurSelByItemData( _lpDefaultSelect ) ) {
			pcbBox->SetCurSelByItemData( _lpSelectBackup );
		}
		pcbBox->AutoSetMinListWidth();

		return true;
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

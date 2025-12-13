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
#include "../../Wav/LSNWavEditor.h"
#include "../../Wav/LSNWavFile.h"

namespace lsn {

	/**
	 * Fills a combo box with console types (Nintendo Entertainment System(R) and Famicom(R)).
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
		return FillComboBox( _pwComboBox, ceEnries, std::size( ceEnries ), _lpDefaultSelect, LSN_CT_NES );
	}

	/**
	 * Fills a combo box with Famicom(R) controller types (Standard Controller only).
	 *
	 * \param _pwComboBox The combo box to fill.
	 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
	 */
	bool CWinUtilities::FillComboWithControllerType_Famicom( CWidget * _pwComboBox ) {
		LSN_COMBO_ENTRY ceEnries[] = {
			//pwcName													lpParm
			{ LSN_LSTR( LSN_INPUT_STANDARD_CONTROLLER ),				LSN_CT_STANDARD,			},
		};
		return FillComboBox( _pwComboBox, ceEnries, std::size( ceEnries ), LSN_CT_STANDARD );
	}

	/**
	 * Fills a combo box with Nintendo Entertainment System(R) controller types.
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
		return FillComboBox( _pwComboBox, ceEnries, std::size( ceEnries ), _lpDefaultSelect, _lpSelectBackup );
	}

	/**
	 * Fills a combo box with Nintendo Entertainment System(R) Four Score expansion types.
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
		return FillComboBox( _pwComboBox, ceEnries, std::size( ceEnries ), _lpDefaultSelect, _lpSelectBackup );
	}

	/**
	 * Fills a combo box with Famicom(R) expansion types.
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
		return FillComboBox( _pwComboBox, ceEnries, std::size( ceEnries ), _lpDefaultSelect, LSN_CT_NONE );
	}

	/**
	 * Fills a combo box with WAV-file formats.
	 *
	 * \param _pwComboBox The combo box to fill.
	 * \param _lpDefaultSelect The default selection.
	 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
	 */
	bool CWinUtilities::FillComboWithWavFormats( CWidget * _pwComboBox, LPARAM _lpDefaultSelect ) {
		LSN_COMBO_ENTRY ceEnries[] = {
			//pwcName													lpParm
			{ LSN_LSTR( LSN_WAV_PCM ),									CWavFile::LSN_F_PCM,								},
			//{ LSN_LSTR( LSN_WAV_ADPCM ),								CWavFile::LSN_F_ADPCM,								},
			{ LSN_LSTR( LSN_WAV_IEEE_FLOAT ),							CWavFile::LSN_F_IEEE_FLOAT,							},
			//{ LSN_LSTR( LSN_WAV_ALAW ),									CWavFile::LSN_F_ALAW,								},
			//{ LSN_LSTR( LSN_WAV_MULAW ),								CWavFile::LSN_F_MULAW,								},
			//{ LSN_LSTR( LSN_WAV_DVI_ADPCM ),							CWavFile::LSN_F_DVI_ADPCM,							},
			//{ LSN_LSTR( LSN_WAV_YAMAHA_ADPCM ),							CWavFile::LSN_F_YAMAHA_ADPCM,						},
			//{ LSN_LSTR( LSN_WAV_GSM_6_10 ),								CWavFile::LSN_F_GSM_6_10,							},
			//{ LSN_LSTR( LSN_WAV_ITU_G_721_ADPCM ),						CWavFile::LSN_F_ITU_G_721_ADPCM,					},
			//{ LSN_LSTR( LSN_WAV_MPEG ),									CWavFile::LSN_F_MPEG,								},
		};
		return FillComboBox( _pwComboBox, ceEnries, std::size( ceEnries ), _lpDefaultSelect, CWavFile::LSN_F_PCM );
	}

	/**
	 * Fills a combo box with WAV-file streaming start conditions.
	 *
	 * \param _pwComboBox The combo box to fill.
	 * \param _lpDefaultSelect The default selection.
	 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
	 */
	bool CWinUtilities::FillComboWithWavStartConditions( CWidget * _pwComboBox, LPARAM _lpDefaultSelect ) {
		LSN_COMBO_ENTRY ceEnries[] = {
			//pwcName													lpParm
			{ LSN_LSTR( LSN_AUDIO_OPTIONS_NONE ),						CWavFile::LSN_SC_NONE,								},
			{ LSN_LSTR( LSN_AUDIO_OPTIONS_START_SAMPLE ),				CWavFile::LSN_SC_START_AT_SAMPLE,					},
			{ LSN_LSTR( LSN_AUDIO_OPTIONS_NON_SILENT ),					CWavFile::LSN_SC_FIRST_NON_ZERO,					},
			{ LSN_LSTR( LSN_AUDIO_OPTIONS_SILENCE_FOR ),				CWavFile::LSN_SC_ZERO_FOR_DURATION,					},
		};
		return FillComboBox( _pwComboBox, ceEnries, std::size( ceEnries ), _lpDefaultSelect, CWavFile::LSN_SC_NONE );
	}

	/**
	 * Fills a combo box with WAV-file streaming end conditions.
	 *
	 * \param _pwComboBox The combo box to fill.
	 * \param _lpDefaultSelect The default selection.
	 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
	 */
	bool CWinUtilities::FillComboWithWavEndConditions( CWidget * _pwComboBox, LPARAM _lpDefaultSelect ) {
		LSN_COMBO_ENTRY ceEnries[] = {
			//pwcName													lpParm
			{ LSN_LSTR( LSN_AUDIO_OPTIONS_NONE ),						CWavFile::LSN_EC_NONE,								},
			{ LSN_LSTR( LSN_AUDIO_OPTIONS_END_SAMPLE ),					CWavFile::LSN_EC_END_AT_SAMPLE,						},
			{ LSN_LSTR( LSN_AUDIO_OPTIONS_SILENCE_FOR ),				CWavFile::LSN_EC_ZERO_FOR_DURATION,					},
			{ LSN_LSTR( LSN_AUDIO_OPTIONS_DURATION ),					CWavFile::LSN_EC_DURATION,							},
		};
		return FillComboBox( _pwComboBox, ceEnries, std::size( ceEnries ), _lpDefaultSelect, CWavFile::LSN_SC_NONE );
	}

	/**
	 * Fills a combo box with WAV-file PCM bit values.
	 *
	 * \param _pwComboBox The combo box to fill.
	 * \param _lpDefaultSelect The default selection.
	 * \param _bInclude32 If true, 32 bits is included.
	 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
	 */
	bool CWinUtilities::FillComboWithWavPcmBits( CWidget * _pwComboBox, LPARAM _lpDefaultSelect, bool _bInclude32 ) {
		if ( _bInclude32 ) {
			LSN_COMBO_ENTRY ceEnries[] = {
				//pwcName													lpParm
				{ L"8",														8,													},
				{ L"16",													16,													},
				{ L"24",													24,													},
				{ L"32",													32,													},
			};
			return FillComboBox( _pwComboBox, ceEnries, std::size( ceEnries ), _lpDefaultSelect, 16 );
		}
		else {
			LSN_COMBO_ENTRY ceEnries[] = {
				//pwcName													lpParm
				{ L"8",														8,													},
				{ L"16",													16,													},
				{ L"24",													24,													},
			};
			return FillComboBox( _pwComboBox, ceEnries, std::size( ceEnries ), _lpDefaultSelect, 16 );
		}
	}

	/**
	 * Fills a combo box with WAV metadata formats.
	 *
	 * \param _pwComboBox The combo box to fill.
	 * \param _lpDefaultSelect The default selection.
	 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
	 */
	bool CWinUtilities::FillComboWithWavMetaDataFormats( CWidget * _pwComboBox, LPARAM _lpDefaultSelect ) {
		LSN_COMBO_ENTRY ceEnries[] = {
			//pwcName													lpParm
			{ LSN_LSTR( LSN_AUDACITY_METADATA ),						CWavFile::LSN_MF_AUDACITY,							},
			//{ LSN_LSTR( LSN_AUDIO_OPTIONS_CUSTOM ),						-1,													},
		};
		return FillComboBox( _pwComboBox, ceEnries, std::size( ceEnries ), _lpDefaultSelect, CWavFile::LSN_MF_AUDACITY );
	}

	/**
	 * Fills a combo box with WAV Editor "Actual Hz".
	 *
	 * \param _pwComboBox The combo box to fill.
	 * \param _lpDefaultSelect The default selection.
	 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
	 */
	bool CWinUtilities::FillComboWithWavActialHz( CWidget * _pwComboBox, LPARAM _lpDefaultSelect ) {
		LSN_COMBO_ENTRY ceEnries[] = {
			//pwcName													lpParm
			{ LSN_LSTR( LSN_WE_NTSC_HZ ),								LPARAM( CWavEditor::LSN_AH_NTSC ),					},
			{ LSN_LSTR( LSN_WE_PAL_HZ ),								LPARAM( CWavEditor::LSN_AH_PAL ),					},
			{ LSN_LSTR( LSN_WE_DENDY_HZ ),								LPARAM( CWavEditor::LSN_AH_DENDY ),					},
			{ LSN_LSTR( LSN_WE_PAL_M_HZ ),								LPARAM( CWavEditor::LSN_AH_PALM ),					},
			{ LSN_LSTR( LSN_WE_PAL_N_HZ ),								LPARAM( CWavEditor::LSN_AH_PALN ),					},
			{ LSN_LSTR( LSN_WE_SET_BY_FILE ),							LPARAM( CWavEditor::LSN_AH_BY_FILE ),				},
			{ LSN_LSTR( LSN_AUDIO_OPTIONS_CUSTOM ),						LPARAM( CWavEditor::LSN_AH_CUSTOM ),				},
		};
		return FillComboBox( _pwComboBox, ceEnries, std::size( ceEnries ), _lpDefaultSelect, CWavEditor::LSN_AH_NTSC );
	}

	/**
	 * Fills a combo box with WAV Editor noise color.
	 *
	 * \param _pwComboBox The combo box to fill.
	 * \param _lpDefaultSelect The default selection.
	 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
	 */
	bool CWinUtilities::FillComboWithWavNoiseColor( CWidget * _pwComboBox, LPARAM _lpDefaultSelect ) {
		LSN_COMBO_ENTRY ceEnries[] = {
			//pwcName													lpParm
			{ LSN_LSTR( LSN_WE_XX_BLACK ),								LPARAM( CWavEditor::LSN_MH_XX_BLACK ),				},
			{ LSN_LSTR( LSN_WE_X_BLACK ),								LPARAM( CWavEditor::LSN_MH_X_BLACK ),				},
			{ LSN_LSTR( LSN_WE_BLACK ),									LPARAM( CWavEditor::LSN_MH_BLACK ),					},
			{ LSN_LSTR( LSN_WE_GREY_00 ),								LPARAM( CWavEditor::LSN_MH_GREY_00 ),				},
			{ LSN_LSTR( LSN_WE_GREY_10 ),								LPARAM( CWavEditor::LSN_MH_GREY_10 ),				},
			{ LSN_LSTR( LSN_WE_GREY_20EM ),								LPARAM( CWavEditor::LSN_MH_GREY_20EM ),				},
			{ LSN_LSTR( LSN_WE_WHITE ),									LPARAM( CWavEditor::LSN_MH_WHITE ),					},
			{ LSN_LSTR( LSN_WE_RED ),									LPARAM( CWavEditor::LSN_MH_RED ),					},
			{ LSN_LSTR( LSN_WE_GREEN ),									LPARAM( CWavEditor::LSN_MH_GREEN ),					},
			{ LSN_LSTR( LSN_WE_BLUE ),									LPARAM( CWavEditor::LSN_MH_BLUE ),					},
			
		};
		return FillComboBox( _pwComboBox, ceEnries, std::size( ceEnries ), _lpDefaultSelect, CWavEditor::LSN_MH_BLACK );
	}

	/**
	 * Fills a combo box with WAV Editor noise type.
	 *
	 * \param _pwComboBox The combo box to fill.
	 * \param _lpDefaultSelect The default selection.
	 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
	 */
	bool CWinUtilities::FillComboWithWavNoiseType( CWidget * _pwComboBox, LPARAM _lpDefaultSelect ) {
		LSN_COMBO_ENTRY ceEnries[] = {
			//pwcName													lpParm
			{ LSN_LSTR( LSN_WE_GAUSSIAN ),								LPARAM( CWavEditor::LSN_WN_GAUSSIAN ),				},
			{ LSN_LSTR( LSN_WE_UNIFORM ),								LPARAM( CWavEditor::LSN_WN_UNIFORM ),				},
			
		};
		return FillComboBox( _pwComboBox, ceEnries, std::size( ceEnries ), _lpDefaultSelect, CWavEditor::LSN_WN_GAUSSIAN );
	}

	/**
	 * Fills a combo box with WAV Editor filter type.
	 *
	 * \param _pwComboBox The combo box to fill.
	 * \param _lpDefaultSelect The default selection.
	 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
	 */
	bool CWinUtilities::FillComboWithWavFilterType( CWidget * _pwComboBox, LPARAM _lpDefaultSelect ) {
		LSN_COMBO_ENTRY ceEnries[] = {
			//pwcName													lpParm
			{ LSN_LSTR( LSN_WE_POLE ),									LPARAM( CWavEditor::LSN_F_POLE ),					},
			//{ LSN_LSTR( LSN_WE_SINC ),									LPARAM( CWavEditor::LSN_F_SINC ),					},
		};
		return FillComboBox( _pwComboBox, ceEnries, std::size( ceEnries ), _lpDefaultSelect, CWavEditor::LSN_F_POLE );
	}

	/**
	 * Fills a combo box with WAV Editor mono-stereo-surround output types.
	 *
	 * \param _pwComboBox The combo box to fill.
	 * \param _lpDefaultSelect The default selection.
	 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
	 */
	bool CWinUtilities::FillComboWithWavStereoSettings( CWidget * _pwComboBox, LPARAM _lpDefaultSelect ) {
		LSN_COMBO_ENTRY ceEnries[] = {
			//pwcName													lpParm
			{ LSN_LSTR( LSN_WE_MONO ),									LPARAM( CWavEditor::LSN_C_MONO ),					},
			{ LSN_LSTR( LSN_WE_STEREO ),								LPARAM( CWavEditor::LSN_C_STEREO ),					},
			{ LSN_LSTR( LSN_WE_SURROUND ),								LPARAM( CWavEditor::LSN_C_SURROUND ),				},
		};
		return FillComboBox( _pwComboBox, ceEnries, std::size( ceEnries ), _lpDefaultSelect, CWavEditor::LSN_C_MONO );
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

	/**
	 * Writes a UI key to a stream.
	 * 
	 * \param _kKey The key values to write.
	 * \param _sStream The stream to which to write the key values.
	 * \return Returns true if writing of the key information to the stream succeeded.
	 **/
	bool CWinUtilities::WriteUiKey( const lsw::LSW_KEY &_kKey, lsn::CStream &_sStream ) {
		if ( !_sStream.Write( _kKey.dwScanCode ) ) { return false; }
		if ( !_sStream.WriteUi8( _kKey.bKeyCode ) ) { return false; }
		if ( !_sStream.WriteUi8( _kKey.bKeyModifier ) ) { return false; }
		return true;
	}

	/**
	 * Reads a UI key from a stream.
	 * 
	 * \param _kKey The key values to be filled by reading from the givens tream.
	 * \param _sStream The stream from which to read the key values.
	 * \return Returns true if reading of the key information from the stream succeeded.
	 **/
	bool CWinUtilities::ReadUiKey( lsw::LSW_KEY &_kKey, lsn::CStream &_sStream ) {
		if ( !_sStream.Read( _kKey.dwScanCode ) ) { return false; }
		if ( !_sStream.ReadUi8( _kKey.bKeyCode ) ) { return false; }
		if ( !_sStream.ReadUi8( _kKey.bKeyModifier ) ) { return false; }
		return true;
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

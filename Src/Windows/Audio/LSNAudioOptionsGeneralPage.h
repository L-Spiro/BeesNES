#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The general audio options and the per-game options.
 */

#pragma once

#include "../../Audio/LSNAudio.h"
#include "../../Localization/LSNLocalization.h"
#include "../../Windows/WinUtilities/LSNWinUtilities.h"
#include "LSNAudioOptionsWindowLayout.h"

#include <CheckButton/LSWCheckButton.h>
#include <Tab/LSWTab.h>
#include <TrackBar/LSWTrackBar.h>
#include <Widget/LSWWidget.h>

#define LSN_SLIDER_PREC							1000
#define LSN_SLIDER_PRECF						static_cast<float>(LSN_SLIDER_PREC)

#define LSN_SLIDER_PRES_VOL						1000
#define LSN_SLIDER_PRES_VOLF					static_cast<float>(LSN_SLIDER_PRES_VOL)
#define LSN_VOL_SLIDER_MULT						5

using namespace lsw;

namespace lsn {
	/**
	 * Class CAudioOptionsGeneralPage
	 * \brief The general audio options and the per-game options.
	 *
	 * Description: The general audio options and the per-game options.
	 */
	template <bool _bIsGlobal>
	class CAudioOptionsGeneralPage : public lsw::CWidget {
	public :
		CAudioOptionsGeneralPage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 ) :
			Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
			m_poOptions( reinterpret_cast<LSN_OPTIONS *>(_ui64Data) ) {
		}
		virtual ~CAudioOptionsGeneralPage() {
		}


		// == Functions.
		/**
		 * The WM_INITDIALOG handler.
		 *
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED									InitDialog() {
			auto aDevices = CAudio::GetAudioDevices();

			LSN_AUDIO_OPTIONS & aoOptions = _bIsGlobal ? m_poOptions->aoGlobalAudioOptions : m_poOptions->aoThisGameAudioOptions;

			lsw::CComboBox * pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_DEVICE_COMBO ));
			{
				std::vector<CWinUtilities::LSN_COMBO_ENTRY> vDevices;
				for ( size_t I = 0; I < aDevices.size(); ++I ) {
					CWinUtilities::LSN_COMBO_ENTRY ceTmp = { .pwcName = reinterpret_cast<const wchar_t *>(aDevices[I].c_str()), .lpParm = LPARAM( I ) };
					vDevices.push_back( ceTmp );
				}
				lsn::CWinUtilities::FillComboBox( pcbCombo, &vDevices[0], vDevices.size(), aoOptions.ui32Device, 0 );
			}
			{
				pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_FORMAT_COMBO ));
				auto aFormats = CAudio::GetAudioFormatsAndHz();
				std::vector<CWinUtilities::LSN_COMBO_ENTRY> vFormats;
				std::vector<std::wstring> vStrings;
				for ( size_t I = 0; I < aFormats.size(); ++I ) {
					size_t sIdx = aFormats[I] >> 24;
					vStrings.push_back( std::format( L"{}, {} Hz", LSN_AUDIO_OPTIONS::s_afFormats[sIdx].pwcName, (aFormats[I] & 0xFFFFFF) * 25 ) );
					CWinUtilities::LSN_COMBO_ENTRY ceTmp = { .pwcName = vStrings[vStrings.size()-1].c_str(), .lpParm = LPARAM( aFormats[I] ) };
					vFormats.push_back( ceTmp );
				}
				uint32_t ui32Code = (aoOptions.afFormat.sfFormat << 24) | (aoOptions.ui32OutputHz / 25);
				lsn::CWinUtilities::FillComboBox( pcbCombo, &vFormats[0], vFormats.size(), ui32Code, (LSN_SF_MONO_16 << 24) | (44100 / 25) );
			}

			lsw::CTrackBar * ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_VOLUME_TRACKBAR ));
			if ( ptbTrackBar ) {
				ptbTrackBar->SetRange( TRUE, 0, 1000 );
				ptbTrackBar->SetTicFreq( 100 );
				ptbTrackBar->SetPos( TRUE, LPARAM( std::round( aoOptions.fVolume * 100.0f ) ) );

				auto aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_VOLUME_EDIT );
				if ( aEdit ) { aEdit->SetTextA( std::to_string( ptbTrackBar->GetPos() ).c_str() ); }
			}
			ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_BG_VOL_TRACKBAR ));
			if ( ptbTrackBar ) {
				ptbTrackBar->SetRange( TRUE, 0, 100 );
				ptbTrackBar->SetTicFreq( 20 );
				ptbTrackBar->SetPos( TRUE, LPARAM( std::round( aoOptions.fBgVol * 100.0f ) ) );

				auto aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_BG_VOL_EDIT );
				if ( aEdit ) { aEdit->SetTextA( std::to_string( ptbTrackBar->GetPos() ).c_str() ); }
			}
			ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_VOLUME_TRACKBAR ));
			if ( ptbTrackBar ) {
				ptbTrackBar->SetRange( TRUE, 0, LSN_SLIDER_PRES_VOL * LSN_VOL_SLIDER_MULT );
				ptbTrackBar->SetTicFreq( LSN_SLIDER_PRES_VOL * LSN_VOL_SLIDER_MULT / 20 );
				ptbTrackBar->SetPos( TRUE, LPARAM( std::round( aoOptions.apCharacteristics.fVolume * LSN_SLIDER_PRES_VOLF ) ) );

				auto aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_VOLUME_EDIT );
				if ( aEdit ) { aEdit->SetTextA( std::to_string( ptbTrackBar->GetPos() / LSN_SLIDER_PRES_VOLF ).c_str() ); }
			}
			ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_PULSE1_VOLUME_TRACKBAR ));
			if ( ptbTrackBar ) {
				ptbTrackBar->SetRange( TRUE, 0, LSN_SLIDER_PREC );
				ptbTrackBar->SetTicFreq( LSN_SLIDER_PREC / 20 );
				ptbTrackBar->SetPos( TRUE, LPARAM( std::round( aoOptions.apCharacteristics.fP1Volume * LSN_SLIDER_PRECF ) ) );

				auto aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_PULSE1_VOLUME_EDIT );
				if ( aEdit ) { aEdit->SetTextA( std::to_string( ptbTrackBar->GetPos() / LSN_SLIDER_PRECF ).c_str() ); }
			}
			ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_PULSE2_VOLUME_TRACKBAR ));
			if ( ptbTrackBar ) {
				ptbTrackBar->SetRange( TRUE, 0, LSN_SLIDER_PREC );
				ptbTrackBar->SetTicFreq( LSN_SLIDER_PREC / 20 );
				ptbTrackBar->SetPos( TRUE, LPARAM( std::round( aoOptions.apCharacteristics.fP2Volume * LSN_SLIDER_PRECF ) ) );

				auto aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_PULSE2_VOLUME_EDIT );
				if ( aEdit ) { aEdit->SetTextA( std::to_string( ptbTrackBar->GetPos() / LSN_SLIDER_PRECF ).c_str() ); }
			}
			ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_TRIANGLE_VOLUME_TRACKBAR ));
			if ( ptbTrackBar ) {
				ptbTrackBar->SetRange( TRUE, 0, LSN_SLIDER_PREC );
				ptbTrackBar->SetTicFreq( LSN_SLIDER_PREC / 20 );
				ptbTrackBar->SetPos( TRUE, LPARAM( std::round( aoOptions.apCharacteristics.fTVolume * LSN_SLIDER_PRECF ) ) );

				auto aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_TRIANGLE_VOLUME_EDIT );
				if ( aEdit ) { aEdit->SetTextA( std::to_string( ptbTrackBar->GetPos() / LSN_SLIDER_PRECF ).c_str() ); }
			}
			ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_NOISE_VOLUME_TRACKBAR ));
			if ( ptbTrackBar ) {
				ptbTrackBar->SetRange( TRUE, 0, LSN_SLIDER_PREC );
				ptbTrackBar->SetTicFreq( LSN_SLIDER_PREC / 20 );
				ptbTrackBar->SetPos( TRUE, LPARAM( std::round( aoOptions.apCharacteristics.fNVolume * LSN_SLIDER_PRECF ) ) );

				auto aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_NOISE_VOLUME_EDIT );
				if ( aEdit ) { aEdit->SetTextA( std::to_string( ptbTrackBar->GetPos() / LSN_SLIDER_PRECF ).c_str() ); }
			}
			ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_DMC_VOLUME_TRACKBAR ));
			if ( ptbTrackBar ) {
				ptbTrackBar->SetRange( TRUE, 0, LSN_SLIDER_PREC );
				ptbTrackBar->SetTicFreq( LSN_SLIDER_PREC / 20 );
				ptbTrackBar->SetPos( TRUE, LPARAM( std::round( aoOptions.apCharacteristics.fDmcVolume * LSN_SLIDER_PRECF ) ) );

				auto aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_DMC_VOLUME_EDIT );
				if ( aEdit ) { aEdit->SetTextA( std::to_string( ptbTrackBar->GetPos() / LSN_SLIDER_PRECF ).c_str() ); }
			}

			{
				pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_PRESETS_COMBO ));
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

			lsw::CCheckButton * pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_ENABLE_CHECK ));
			if ( pcbCheck ) {
				pcbCheck->SetCheck( aoOptions.bEnabled );
			}
			pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_DITHER_CHECK ));
			if ( pcbCheck ) {
				pcbCheck->SetCheck( aoOptions.bDither );
			}
			pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_LPF_CHECK ));
			if ( pcbCheck ) {
				pcbCheck->SetCheck( aoOptions.apCharacteristics.bLpfEnable );
			}
			pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_HPF0_CHECK ));
			if ( pcbCheck ) {
				pcbCheck->SetCheck( aoOptions.apCharacteristics.bHpf0Enable );
			}
			pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_HPF1_CHECK ));
			if ( pcbCheck ) {
				pcbCheck->SetCheck( aoOptions.apCharacteristics.bHpf1Enable );
			}
			pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_HPF2_CHECK ));
			if ( pcbCheck ) {
				pcbCheck->SetCheck( aoOptions.apCharacteristics.bHpf2Enable );
			}
			pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_INVERT_CHECK ));
			if ( pcbCheck ) {
				pcbCheck->SetCheck( aoOptions.apCharacteristics.bInvert );
			}
			pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_NOISE_CHECK ));
			if ( pcbCheck ) {
				pcbCheck->SetCheck( aoOptions.apCharacteristics.bNoise );
			}
			pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_OLD_NES_CHECK ));
			if ( pcbCheck ) {
				pcbCheck->SetCheck( aoOptions.apCharacteristics.bRp2A02 );
			}

			


			auto aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_LPF_EDIT );
			if ( aEdit ) { aEdit->SetTextA( std::format( "{:.27}", aoOptions.apCharacteristics.fLpf ).c_str() ); }

			aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_HPF0_EDIT );
			if ( aEdit ) { aEdit->SetTextA( std::format( "{:.27}", aoOptions.apCharacteristics.fHpf0 ).c_str() ); }

			aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_HPF1_EDIT );
			if ( aEdit ) { aEdit->SetTextA( std::format( "{:.27}", aoOptions.apCharacteristics.fHpf1 ).c_str() ); }

			aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_HPF2_EDIT );
			if ( aEdit ) { aEdit->SetTextA( std::format( "{:.27}", aoOptions.apCharacteristics.fHpf2 ).c_str() ); }

			Update();
			return Parent::InitDialog();
		}

		/**
		 * Handles the WM_COMMAND message.
		 *
		 * \param _wCtrlCode 0 = from menu, 1 = from accelerator, otherwise it is a Control-defined notification code.
		 * \param _wId The ID of the control if _wCtrlCode is not 0 or 1.
		 * \param _pwSrc The source control if _wCtrlCode is not 0 or 1.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED									Command( WORD _wCtrlCode, WORD _wId, CWidget * _pwSrc ) {
			switch ( _wId ) {
				case CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_FORMAT_COMBO : {
					// Notify the parent of the change.
					if ( m_pwParent ) {
						//m_pwParent->Command( CWinUtilities::LSN_UPDATE_HZ, 0, nullptr );
						::SendMessageW( m_pwParent->Wnd(), WM_COMMAND, CWinUtilities::LSN_UPDATE_HZ, CWinUtilities::LSN_UPDATE_HZ );
					}
					break;
				}
				case CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_VOLUME_EDIT : {
					if ( _wCtrlCode == EN_CHANGE ) {
						lsw::CTrackBar * ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_VOLUME_TRACKBAR ));
						if ( ptbTrackBar ) {
							ee::CExpEvalContainer::EE_RESULT rRes;
							if ( _pwSrc->GetTextAsUInt64Expression( rRes ) ) {
								rRes.u.ui64Val = std::clamp<uint64_t>( rRes.u.ui64Val, 0, 1000 );
								ptbTrackBar->SetPos( TRUE, LPARAM( rRes.u.ui64Val ) );
							}
						}
					}
					break;
				}
				case CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_BG_VOL_EDIT : {
					if ( _wCtrlCode == EN_CHANGE ) {
						lsw::CTrackBar * ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_BG_VOL_TRACKBAR ));
						if ( ptbTrackBar ) {
							ee::CExpEvalContainer::EE_RESULT rRes;
							if ( _pwSrc->GetTextAsUInt64Expression( rRes ) ) {
								rRes.u.ui64Val = std::clamp<uint64_t>( rRes.u.ui64Val, 0, 100 );
								ptbTrackBar->SetPos( TRUE, LPARAM( rRes.u.ui64Val ) );
							}
						}
					}
					break;
				}
				case CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_VOLUME_EDIT : {
					if ( _wCtrlCode == EN_CHANGE ) {
						lsw::CTrackBar * ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_VOLUME_TRACKBAR ));
						if ( ptbTrackBar ) {
							ee::CExpEvalContainer::EE_RESULT rRes;
							if ( _pwSrc->GetTextAsDoubleExpression( rRes ) ) {
								rRes.u.ui64Val = std::clamp<uint64_t>( uint64_t( std::round( rRes.u.dVal * LSN_SLIDER_PRES_VOLF ) ), 0, LSN_SLIDER_PRES_VOL * LSN_VOL_SLIDER_MULT );
								ptbTrackBar->SetPos( TRUE, LPARAM( rRes.u.ui64Val ) );
							}
						}
					}
					break;
				}
				case CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_PULSE1_VOLUME_EDIT : {
					if ( _wCtrlCode == EN_CHANGE ) {
						lsw::CTrackBar * ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_PULSE1_VOLUME_TRACKBAR ));
						if ( ptbTrackBar ) {
							ee::CExpEvalContainer::EE_RESULT rRes;
							if ( _pwSrc->GetTextAsDoubleExpression( rRes ) ) {
								rRes.u.ui64Val = std::clamp<uint64_t>( uint64_t( std::round( rRes.u.dVal * LSN_SLIDER_PRECF ) ), 0, LSN_SLIDER_PREC );
								ptbTrackBar->SetPos( TRUE, LPARAM( rRes.u.ui64Val ) );
							}
						}
					}
					break;
				}
				case CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_PULSE2_VOLUME_EDIT : {
					if ( _wCtrlCode == EN_CHANGE ) {
						lsw::CTrackBar * ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_PULSE2_VOLUME_TRACKBAR ));
						if ( ptbTrackBar ) {
							ee::CExpEvalContainer::EE_RESULT rRes;
							if ( _pwSrc->GetTextAsDoubleExpression( rRes ) ) {
								rRes.u.ui64Val = std::clamp<uint64_t>( uint64_t( std::round( rRes.u.dVal * LSN_SLIDER_PRECF ) ), 0, LSN_SLIDER_PREC );
								ptbTrackBar->SetPos( TRUE, LPARAM( rRes.u.ui64Val ) );
							}
						}
					}
					break;
				}
				case CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_TRIANGLE_VOLUME_EDIT : {
					if ( _wCtrlCode == EN_CHANGE ) {
						lsw::CTrackBar * ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_TRIANGLE_VOLUME_TRACKBAR ));
						if ( ptbTrackBar ) {
							ee::CExpEvalContainer::EE_RESULT rRes;
							if ( _pwSrc->GetTextAsDoubleExpression( rRes ) ) {
								rRes.u.ui64Val = std::clamp<uint64_t>( uint64_t( std::round( rRes.u.dVal * LSN_SLIDER_PRECF ) ), 0, LSN_SLIDER_PREC );
								ptbTrackBar->SetPos( TRUE, LPARAM( rRes.u.ui64Val ) );
							}
						}
					}
					break;
				}
				case CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_NOISE_VOLUME_EDIT : {
					if ( _wCtrlCode == EN_CHANGE ) {
						lsw::CTrackBar * ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_NOISE_VOLUME_TRACKBAR ));
						if ( ptbTrackBar ) {
							ee::CExpEvalContainer::EE_RESULT rRes;
							if ( _pwSrc->GetTextAsDoubleExpression( rRes ) ) {
								rRes.u.ui64Val = std::clamp<uint64_t>( uint64_t( std::round( rRes.u.dVal * LSN_SLIDER_PRECF ) ), 0, LSN_SLIDER_PREC );
								ptbTrackBar->SetPos( TRUE, LPARAM( rRes.u.ui64Val ) );
							}
						}
					}
					break;
				}
				case CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_DMC_VOLUME_EDIT : {
					if ( _wCtrlCode == EN_CHANGE ) {
						lsw::CTrackBar * ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_DMC_VOLUME_TRACKBAR ));
						if ( ptbTrackBar ) {
							ee::CExpEvalContainer::EE_RESULT rRes;
							if ( _pwSrc->GetTextAsDoubleExpression( rRes ) ) {
								rRes.u.ui64Val = std::clamp<uint64_t>( uint64_t( std::round( rRes.u.dVal * LSN_SLIDER_PRECF ) ), 0, LSN_SLIDER_PREC );
								ptbTrackBar->SetPos( TRUE, LPARAM( rRes.u.ui64Val ) );
							}
						}
					}
					break;
				}

				case CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_PRESETS_COMBO : {
					if ( _wCtrlCode == CBN_SELENDOK ) {
						auto pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_PRESETS_COMBO ));
						if ( pcbCombo ) {
							auto lpSel = pcbCombo->GetCurSelItemData();
							if ( lpSel != -1 && size_t( lpSel ) < LSN_AUDIO_OPTIONS::PresetTotal() ) {
								auto aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_LPF_EDIT );
								if ( aEdit ) { aEdit->SetTextA( std::format( "{:.27}", LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].fLpf ).c_str() ); }

								aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_HPF0_EDIT );
								if ( aEdit ) { aEdit->SetTextA( std::format( "{:.27}", LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].fHpf0 ).c_str() ); }

								aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_HPF1_EDIT );
								if ( aEdit ) { aEdit->SetTextA( std::format( "{:.27}", LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].fHpf1 ).c_str() ); }

								aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_HPF2_EDIT );
								if ( aEdit ) { aEdit->SetTextA( std::format( "{:.27}", LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].fHpf2 ).c_str() ); }


								lsw::CCheckButton * pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_LPF_CHECK ));
								if ( pcbCheck ) {
									pcbCheck->SetCheck( LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].bLpfEnable );
								}
								pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_HPF0_CHECK ));
								if ( pcbCheck ) {
									pcbCheck->SetCheck( LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].bHpf0Enable );
								}
								pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_HPF1_CHECK ));
								if ( pcbCheck ) {
									pcbCheck->SetCheck( LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].bHpf1Enable );
								}
								pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_HPF2_CHECK ));
								if ( pcbCheck ) {
									pcbCheck->SetCheck( LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].bHpf2Enable );
								}
								pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_INVERT_CHECK ));
								if ( pcbCheck ) {
									pcbCheck->SetCheck( LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].bInvert );
								}
								pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_NOISE_CHECK ));
								if ( pcbCheck ) {
									pcbCheck->SetCheck( LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].bNoise );
								}
								pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_OLD_NES_CHECK ));
								if ( pcbCheck ) {
									pcbCheck->SetCheck( LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].bRp2A02 );
								}

								auto ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_VOLUME_TRACKBAR ));
								if ( ptbTrackBar ) {
									ptbTrackBar->SetPos( TRUE, LPARAM( std::round( LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].fVolume * LSN_SLIDER_PRES_VOLF ) ) );

									aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_VOLUME_EDIT );
									if ( aEdit ) { aEdit->SetTextA( std::to_string( ptbTrackBar->GetPos() / LSN_SLIDER_PRECF ).c_str() ); }
								}
								ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_PULSE1_VOLUME_TRACKBAR ));
								if ( ptbTrackBar ) {
									ptbTrackBar->SetPos( TRUE, LPARAM( std::round( LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].fP1Volume * LSN_SLIDER_PRECF ) ) );

									aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_PULSE1_VOLUME_EDIT );
									if ( aEdit ) { aEdit->SetTextA( std::to_string( ptbTrackBar->GetPos() / LSN_SLIDER_PRECF ).c_str() ); }
								}
								ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_PULSE2_VOLUME_TRACKBAR ));
								if ( ptbTrackBar ) {
									ptbTrackBar->SetPos( TRUE, LPARAM( std::round( LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].fP2Volume * LSN_SLIDER_PRECF ) ) );

									aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_PULSE2_VOLUME_EDIT );
									if ( aEdit ) { aEdit->SetTextA( std::to_string( ptbTrackBar->GetPos() / LSN_SLIDER_PRECF ).c_str() ); }
								}
								ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_TRIANGLE_VOLUME_TRACKBAR ));
								if ( ptbTrackBar ) {
									ptbTrackBar->SetPos( TRUE, LPARAM( std::round( LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].fTVolume * LSN_SLIDER_PRECF ) ) );

									aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_TRIANGLE_VOLUME_EDIT );
									if ( aEdit ) { aEdit->SetTextA( std::to_string( ptbTrackBar->GetPos() / LSN_SLIDER_PRECF ).c_str() ); }
								}
								ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_NOISE_VOLUME_TRACKBAR ));
								if ( ptbTrackBar ) {
									ptbTrackBar->SetPos( TRUE, LPARAM( std::round( LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].fNVolume * LSN_SLIDER_PRECF ) ) );

									aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_NOISE_VOLUME_EDIT );
									if ( aEdit ) { aEdit->SetTextA( std::to_string( ptbTrackBar->GetPos() / LSN_SLIDER_PRECF ).c_str() ); }
								}
								ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_DMC_VOLUME_TRACKBAR ));
								if ( ptbTrackBar ) {
									ptbTrackBar->SetPos( TRUE, LPARAM( std::round( LSN_AUDIO_OPTIONS::s_apProfiles[lpSel].fDmcVolume * LSN_SLIDER_PRECF ) ) );

									aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_DMC_VOLUME_EDIT );
									if ( aEdit ) { aEdit->SetTextA( std::to_string( ptbTrackBar->GetPos() / LSN_SLIDER_PRECF ).c_str() ); }
								}
							}
						}
					}
					break;
				}
			}
			Update();
			return Parent::Command( _wCtrlCode, _wId, _pwSrc );
		}

		/**
		 * WM_HSCROLL.
		 * 
		 * \param _uScrollPos Specifies the current position of the scroll box if the LOWORD is SB_THUMBPOSITION or SB_THUMBTRACK; otherwise, this word is not used.
		 * \param PARM Specifies a scroll bar value that indicates the user's scrolling request. This word can be one of the following values: SB_ENDSCROLL, SB_LEFT, SB_RIGHT, SB_LINELEFT, SB_LINERIGHT, SB_PAGELEFT, SB_PAGERIGHT, SB_THUMBPOSITION, SB_THUMBTRACK.
		 * \return Returns an LSW_HANDLED code.
		 **/
		virtual LSW_HANDLED									HScroll( USHORT /*_uScrollPos*/, USHORT /*_uScrollType*/, CWidget * _pwWidget ) {
			if ( !_pwWidget ) { return LSW_H_CONTINUE; }
			if ( _pwWidget->Id() == CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_VOLUME_TRACKBAR ) {
				lsw::CTrackBar * ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(_pwWidget);
				auto aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_VOLUME_EDIT );
				if ( aEdit ) { aEdit->SetTextA( std::to_string( ptbTrackBar->GetPos() ).c_str() ); }
			}
			else if ( _pwWidget->Id() == CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_BG_VOL_TRACKBAR ) {
				lsw::CTrackBar * ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(_pwWidget);
				auto aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_BG_VOL_EDIT );
				if ( aEdit ) { aEdit->SetTextA( std::to_string( ptbTrackBar->GetPos() ).c_str() ); }
			}
			else if ( _pwWidget->Id() == CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_VOLUME_TRACKBAR ) {
				lsw::CTrackBar * ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(_pwWidget);
				auto aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_VOLUME_EDIT );
				if ( aEdit ) { aEdit->SetTextA( std::to_string( ptbTrackBar->GetPos() / LSN_SLIDER_PRES_VOLF ).c_str() ); }
				Update();
			}
			else if ( _pwWidget->Id() == CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_PULSE1_VOLUME_TRACKBAR ) {
				lsw::CTrackBar * ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(_pwWidget);
				auto aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_PULSE1_VOLUME_EDIT );
				if ( aEdit ) { aEdit->SetTextA( std::to_string( ptbTrackBar->GetPos() / LSN_SLIDER_PRECF ).c_str() ); }
				Update();
			}
			else if ( _pwWidget->Id() == CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_PULSE2_VOLUME_TRACKBAR ) {
				lsw::CTrackBar * ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(_pwWidget);
				auto aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_PULSE2_VOLUME_EDIT );
				if ( aEdit ) { aEdit->SetTextA( std::to_string( ptbTrackBar->GetPos() / LSN_SLIDER_PRECF ).c_str() ); }
				Update();
			}
			else if ( _pwWidget->Id() == CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_TRIANGLE_VOLUME_TRACKBAR ) {
				lsw::CTrackBar * ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(_pwWidget);
				auto aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_TRIANGLE_VOLUME_EDIT );
				if ( aEdit ) { aEdit->SetTextA( std::to_string( ptbTrackBar->GetPos() / LSN_SLIDER_PRECF ).c_str() ); }
				Update();
			}
			else if ( _pwWidget->Id() == CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_NOISE_VOLUME_TRACKBAR ) {
				lsw::CTrackBar * ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(_pwWidget);
				auto aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_NOISE_VOLUME_EDIT );
				if ( aEdit ) { aEdit->SetTextA( std::to_string( ptbTrackBar->GetPos() / LSN_SLIDER_PRECF ).c_str() ); }
				Update();
			}
			else if ( _pwWidget->Id() == CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_DMC_VOLUME_TRACKBAR ) {
				lsw::CTrackBar * ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(_pwWidget);
				auto aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_DMC_VOLUME_EDIT );
				if ( aEdit ) { aEdit->SetTextA( std::to_string( ptbTrackBar->GetPos() / LSN_SLIDER_PRECF ).c_str() ); }
				Update();
			}
			return LSW_H_CONTINUE;
		}

		/**
		 * Updates the dialog.
		 **/
		void												Update() {
			auto pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_FORMAT_COMBO ));
			bool bDitherEnabled = false;
			if ( pcbCombo ) {
				auto lpCode = pcbCombo->GetCurSelItemData();
				auto sfFormat = static_cast<LSN_SAMPLE_FORMAT>((lpCode >> 24) & 0x0F);
				
				if ( size_t( sfFormat ) < LSN_AUDIO_OPTIONS::FormatTotal() && LSN_AUDIO_OPTIONS::s_afFormats[sfFormat].ui16BitsPerChannel == 16 ) {
					bDitherEnabled = true;
				}
			}

			if constexpr ( !_bIsGlobal ) {
				lsw::CTab * ptTab = nullptr;
				if ( m_pwParent ) {
					if ( m_pwParent->IsTab() ) {
						ptTab = reinterpret_cast<lsw::CTab *>(m_pwParent);
					}
					else {
						ptTab = reinterpret_cast<lsw::CTab *>(m_pwParent->FindChild( CAudioOptionsWindowLayout::LSN_AOWI_TAB ));
					}
				}
				if ( ptTab ) {
					WORD wId[] = {
						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_GROUP,
						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_DEVICE_LABEL,
						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_FORMAT_LABEL,
						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_VOLUME_LABEL,
						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_BG_VOL_LABEL,
						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_GROUP,
						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_PRESETS_LABEL,
						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_LPF_LABEL,
						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_HPF0_LABEL,
						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_HPF1_LABEL,
						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_HPF2_LABEL,
						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_VOLUME_LABEL,

						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_DEVICE_COMBO,
						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_ENABLE_CHECK,
						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_FORMAT_COMBO,
						//CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_DITHER_CHECK,
						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_VOLUME_TRACKBAR,
						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_VOLUME_EDIT,
						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_BG_VOL_TRACKBAR,
						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_BG_VOL_EDIT,

						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_PRESETS_COMBO,
						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_LPF_EDIT,
						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_LPF_CHECK,
						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_HPF0_EDIT,
						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_HPF0_CHECK,

						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_HPF1_EDIT,
						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_HPF1_CHECK,

						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_HPF2_EDIT,
						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_HPF2_CHECK,

						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_VOLUME_TRACKBAR,
						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_VOLUME_EDIT,

						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_PULSE1_VOLUME_TRACKBAR,
						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_PULSE1_VOLUME_EDIT,

						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_PULSE2_VOLUME_TRACKBAR,
						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_PULSE2_VOLUME_EDIT,

						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_TRIANGLE_VOLUME_TRACKBAR,
						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_TRIANGLE_VOLUME_EDIT,

						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_NOISE_VOLUME_TRACKBAR,
						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_NOISE_VOLUME_EDIT,

						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_DMC_VOLUME_TRACKBAR,
						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_DMC_VOLUME_EDIT,

						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_INVERT_CHECK,
						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_NOISE_CHECK,
						CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_OLD_NES_CHECK,
					};
					
					bool bEnabled = ptTab->IsChecked( 1 );
					if ( bDitherEnabled ) {
						bDitherEnabled = bEnabled;
					}
					for ( auto I = LSN_ELEMENTS( wId ); I--; ) {
						auto aTmp = FindChild( wId[I] );
						if ( aTmp ) {
							aTmp->SetEnabled( bEnabled );
						}
					}
				}

				::InvalidateRect( Wnd(), NULL, FALSE );
				Paint();
			}

			auto pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_DITHER_CHECK ));
			if ( pcbCheck ) {
				pcbCheck->SetEnabled( bDitherEnabled );
			}

			pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_PRESETS_COMBO ));
			if ( pcbCombo ) {
				LSN_AUDIO_PROFILE apProfile;
				pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_LPF_CHECK ));
				if ( pcbCheck ) {
					apProfile.bLpfEnable = pcbCheck->IsChecked();
					pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_HPF0_CHECK ));
					if ( pcbCheck ) {
						apProfile.bHpf0Enable = pcbCheck->IsChecked();
						pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_HPF1_CHECK ));
						if ( pcbCheck ) {
							apProfile.bHpf1Enable = pcbCheck->IsChecked();
							pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_HPF2_CHECK ));
							if ( pcbCheck ) {
								apProfile.bHpf2Enable = pcbCheck->IsChecked();
								pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_INVERT_CHECK ));
								if ( pcbCheck ) {
									apProfile.bInvert = pcbCheck->IsChecked();
									pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_NOISE_CHECK ));
									if ( pcbCheck ) {
										apProfile.bNoise = pcbCheck->IsChecked();
										pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_OLD_NES_CHECK ));
										if ( pcbCheck ) {
											apProfile.bRp2A02 = pcbCheck->IsChecked();
											auto aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_LPF_EDIT );
											ee::CExpEvalContainer::EE_RESULT rRes;
											if ( aEdit && aEdit->GetTextAsDoubleExpression( rRes ) ) {
												apProfile.fLpf = float( rRes.u.dVal );
												aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_HPF0_EDIT );
												if ( aEdit && aEdit->GetTextAsDoubleExpression( rRes ) ) {
													apProfile.fHpf0 = float( rRes.u.dVal );
													aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_HPF1_EDIT );
													if ( aEdit && aEdit->GetTextAsDoubleExpression( rRes ) ) {
														apProfile.fHpf1 = float( rRes.u.dVal );
														aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_HPF2_EDIT );
														if ( aEdit && aEdit->GetTextAsDoubleExpression( rRes ) ) {
															apProfile.fHpf2 = float( rRes.u.dVal );
															lsw::CTrackBar * ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_VOLUME_TRACKBAR ));
															if ( ptbTrackBar ) {
																apProfile.fVolume = (ptbTrackBar->GetPos() / LSN_SLIDER_PRES_VOLF);
																ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_PULSE1_VOLUME_TRACKBAR ));
																if ( ptbTrackBar ) {
																	apProfile.fP1Volume = (ptbTrackBar->GetPos() / LSN_SLIDER_PRECF);
																	ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_PULSE2_VOLUME_TRACKBAR ));
																	if ( ptbTrackBar ) {
																		apProfile.fP2Volume = (ptbTrackBar->GetPos() / LSN_SLIDER_PRECF);
																		ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_TRIANGLE_VOLUME_TRACKBAR ));
																		if ( ptbTrackBar ) {
																			apProfile.fTVolume = (ptbTrackBar->GetPos() / LSN_SLIDER_PRECF);
																			ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_NOISE_VOLUME_TRACKBAR ));
																			if ( ptbTrackBar ) {
																				apProfile.fNVolume = (ptbTrackBar->GetPos() / LSN_SLIDER_PRECF);
																				ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_DMC_VOLUME_TRACKBAR ));
																				if ( ptbTrackBar ) {
																					apProfile.fDmcVolume = (ptbTrackBar->GetPos() / LSN_SLIDER_PRECF);
																					LPARAM lpSel = -1;
																					for ( auto I = LSN_AUDIO_OPTIONS::PresetTotal(); I--; ) {
																						if ( LSN_AUDIO_OPTIONS::s_apProfiles[I].bLpfEnable == apProfile.bLpfEnable &&
																							LSN_AUDIO_OPTIONS::s_apProfiles[I].bHpf0Enable == apProfile.bHpf0Enable &&
																							LSN_AUDIO_OPTIONS::s_apProfiles[I].bHpf1Enable == apProfile.bHpf1Enable &&
																							LSN_AUDIO_OPTIONS::s_apProfiles[I].bHpf2Enable == apProfile.bHpf2Enable &&
																							LSN_AUDIO_OPTIONS::s_apProfiles[I].bInvert == apProfile.bInvert &&
																							LSN_AUDIO_OPTIONS::s_apProfiles[I].bNoise == apProfile.bNoise &&
																							LSN_AUDIO_OPTIONS::s_apProfiles[I].bRp2A02 == apProfile.bRp2A02 &&
																							(!apProfile.bLpfEnable || LSN_AUDIO_OPTIONS::s_apProfiles[I].fLpf == apProfile.fLpf) &&
																							(!apProfile.bHpf0Enable || LSN_AUDIO_OPTIONS::s_apProfiles[I].fHpf0 == apProfile.fHpf0) &&
																							(!apProfile.bHpf1Enable || LSN_AUDIO_OPTIONS::s_apProfiles[I].fHpf1 == apProfile.fHpf1) &&
																							(!apProfile.bHpf2Enable || LSN_AUDIO_OPTIONS::s_apProfiles[I].fHpf2 == apProfile.fHpf2) &&
																							LSN_AUDIO_OPTIONS::s_apProfiles[I].fVolume == apProfile.fVolume &&
																							LSN_AUDIO_OPTIONS::s_apProfiles[I].fP1Volume == apProfile.fP1Volume &&
																							LSN_AUDIO_OPTIONS::s_apProfiles[I].fP2Volume == apProfile.fP2Volume &&
																							LSN_AUDIO_OPTIONS::s_apProfiles[I].fTVolume == apProfile.fTVolume &&
																							LSN_AUDIO_OPTIONS::s_apProfiles[I].fNVolume == apProfile.fNVolume &&
																							LSN_AUDIO_OPTIONS::s_apProfiles[I].fDmcVolume == apProfile.fDmcVolume ) {
																							lpSel = I;
																							break;
																						}
																					}
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
							}
						}
					}
				}
			}

			
		}

		/**
		 * Saves the audio options from the dialog to the data structures.
		 **/
		void												Save() {
			LSN_AUDIO_OPTIONS & aoOptions = _bIsGlobal ? m_poOptions->aoGlobalAudioOptions : m_poOptions->aoThisGameAudioOptions;

			lsw::CComboBox * pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_DEVICE_COMBO ));
			if ( pcbCombo ) {
				aoOptions.ui32Device = static_cast<uint32_t>(pcbCombo->GetCurSelItemData());
			}
			pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_FORMAT_COMBO ));
			if ( pcbCombo ) {
				auto lpCode = pcbCombo->GetCurSelItemData();
				aoOptions.afFormat.sfFormat = static_cast<LSN_SAMPLE_FORMAT>((lpCode >> 24) & 0x0F);
				aoOptions.ui32OutputHz = (static_cast<uint32_t>(lpCode) & 0xFFFFFF) * 25;
			}


			lsw::CTrackBar * ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_VOLUME_TRACKBAR ));
			if ( ptbTrackBar ) {
				aoOptions.fVolume = (ptbTrackBar->GetPos() / 100.0f);
			}
			ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_BG_VOL_TRACKBAR ));
			if ( ptbTrackBar ) {
				aoOptions.fBgVol = (ptbTrackBar->GetPos() / 100.0f);
			}
			ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_VOLUME_TRACKBAR ));
			if ( ptbTrackBar ) {
				aoOptions.apCharacteristics.fVolume = (ptbTrackBar->GetPos() / LSN_SLIDER_PRES_VOLF);
			}
			ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_PULSE1_VOLUME_TRACKBAR ));
			if ( ptbTrackBar ) {
				aoOptions.apCharacteristics.fP1Volume = (ptbTrackBar->GetPos() / LSN_SLIDER_PRECF);
			}
			ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_PULSE2_VOLUME_TRACKBAR ));
			if ( ptbTrackBar ) {
				aoOptions.apCharacteristics.fP2Volume = (ptbTrackBar->GetPos() / LSN_SLIDER_PRECF);
			}
			ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_TRIANGLE_VOLUME_TRACKBAR ));
			if ( ptbTrackBar ) {
				aoOptions.apCharacteristics.fTVolume = (ptbTrackBar->GetPos() / LSN_SLIDER_PRECF);
			}
			ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_NOISE_VOLUME_TRACKBAR ));
			if ( ptbTrackBar ) {
				aoOptions.apCharacteristics.fNVolume = (ptbTrackBar->GetPos() / LSN_SLIDER_PRECF);
			}
			ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_DMC_VOLUME_TRACKBAR ));
			if ( ptbTrackBar ) {
				aoOptions.apCharacteristics.fDmcVolume = (ptbTrackBar->GetPos() / LSN_SLIDER_PRECF);
			}


			lsw::CCheckButton * pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_ENABLE_CHECK ));
			if ( pcbCheck ) {
				aoOptions.bEnabled = pcbCheck->IsChecked();
			}
			pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_DITHER_CHECK ));
			if ( pcbCheck ) {
				aoOptions.bDither = pcbCheck->IsChecked();
			}
			pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_LPF_CHECK ));
			if ( pcbCheck ) {
				aoOptions.apCharacteristics.bLpfEnable = pcbCheck->IsChecked();
			}
			pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_HPF0_CHECK ));
			if ( pcbCheck ) {
				aoOptions.apCharacteristics.bHpf0Enable = pcbCheck->IsChecked();
			}
			pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_HPF1_CHECK ));
			if ( pcbCheck ) {
				aoOptions.apCharacteristics.bHpf1Enable = pcbCheck->IsChecked();
			}
			pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_HPF2_CHECK ));
			if ( pcbCheck ) {
				aoOptions.apCharacteristics.bHpf2Enable = pcbCheck->IsChecked();
			}
			pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_INVERT_CHECK ));
			if ( pcbCheck ) {
				aoOptions.apCharacteristics.bInvert = pcbCheck->IsChecked();
			}
			pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_NOISE_CHECK ));
			if ( pcbCheck ) {
				aoOptions.apCharacteristics.bNoise = pcbCheck->IsChecked();
			}
			pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_OLD_NES_CHECK ));
			if ( pcbCheck ) {
				aoOptions.apCharacteristics.bRp2A02 = pcbCheck->IsChecked();
			}

			
			auto aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_LPF_EDIT );
			if ( aEdit ) {
				ee::CExpEvalContainer::EE_RESULT rRes;
				if ( aEdit->GetTextAsDoubleExpression( rRes ) ) {
					aoOptions.apCharacteristics.fLpf = float( rRes.u.dVal );
				}
			}
			aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_HPF0_EDIT );
			if ( aEdit ) {
				ee::CExpEvalContainer::EE_RESULT rRes;
				if ( aEdit->GetTextAsDoubleExpression( rRes ) ) {
					aoOptions.apCharacteristics.fHpf0 = float( rRes.u.dVal );
				}
			}
			aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_HPF1_EDIT );
			if ( aEdit ) {
				ee::CExpEvalContainer::EE_RESULT rRes;
				if ( aEdit->GetTextAsDoubleExpression( rRes ) ) {
					aoOptions.apCharacteristics.fHpf1 = float( rRes.u.dVal );
				}
			}
			aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_CHARACTERISTICS_HPF2_EDIT );
			if ( aEdit ) {
				ee::CExpEvalContainer::EE_RESULT rRes;
				if ( aEdit->GetTextAsDoubleExpression( rRes ) ) {
					aoOptions.apCharacteristics.fHpf2 = float( rRes.u.dVal );
				}
			}

			if ( m_pwParent ) {
				if ( m_pwParent->IsTab() ) {
					aoOptions.bUseGlobal = !reinterpret_cast<lsw::CTab *>(m_pwParent)->IsChecked( 1 );
				}
				else {
					lsw::CTab * ptTab = reinterpret_cast<lsw::CTab *>(m_pwParent->FindChild( CAudioOptionsWindowLayout::LSN_AOWI_TAB ));
					if ( ptTab ) {
						aoOptions.bUseGlobal = !ptTab->IsChecked( 1 );
					}
				}
			}
		}

		/**
		 * Gets the currently selected Hz.
		 * 
		 * \return Returns the Hz setting.
		 **/
		uint32_t											GetHz() {
			lsw::CComboBox * pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_FORMAT_COMBO ));
			if ( pcbCombo ) {
				auto lpCode = pcbCombo->GetCurSelItemData();
				//aoOptions.afFormat.sfFormat = static_cast<LSN_SAMPLE_FORMAT>((lpCode >> 24) & 0x0F);
				return (static_cast<uint32_t>(lpCode) & 0xFFFFFF) * 25;
			}
			return 1;
		}


	protected :
		// == Members.
		LSN_OPTIONS *										m_poOptions;												/**< The options object. */


		// == Functions.
		

	private :
		typedef CAudioOptionsWindowLayout					Layout;
		typedef lsw::CWidget								Parent;

	};

}	// namespace lsn

#undef LSN_SLIDER_PRECF
#undef LSN_SLIDER_PREC

#endif	// #ifdef LSN_USE_WINDOWS

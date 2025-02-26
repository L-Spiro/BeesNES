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
#include <TrackBar/LSWTrackBar.h>
#include <Widget/LSWWidget.h>

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
				ptbTrackBar->SetRange( TRUE, 0, 500 );
				ptbTrackBar->SetTicFreq( 50 );
				ptbTrackBar->SetPos( TRUE, LPARAM( aoOptions.fVolume * 100.0f ) );

				auto aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_VOLUME_EDIT );
				if ( aEdit ) { aEdit->SetTextA( std::to_string( ptbTrackBar->GetPos() ).c_str() ); }
			}
			ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_BG_VOL_TRACKBAR ));
			if ( ptbTrackBar ) {
				ptbTrackBar->SetRange( TRUE, 0, 100 );
				ptbTrackBar->SetTicFreq( 20 );
				ptbTrackBar->SetPos( TRUE, LPARAM( aoOptions.fBgVol * 100.0f ) );

				auto aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_BG_VOL_EDIT );
				if ( aEdit ) { aEdit->SetTextA( std::to_string( ptbTrackBar->GetPos() ).c_str() ); }
			}

			{
				pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_PRESETS_COMBO ));
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
			pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_LPF_CHECK ));
			if ( pcbCheck ) {
				pcbCheck->SetCheck( aoOptions.apCharacteristics.bLpfEnable );
			}
			pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_HPF0_CHECK ));
			if ( pcbCheck ) {
				pcbCheck->SetCheck( aoOptions.apCharacteristics.bHpf0Enable );
			}
			pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_HPF1_CHECK ));
			if ( pcbCheck ) {
				pcbCheck->SetCheck( aoOptions.apCharacteristics.bHpf1Enable );
			}
			pcbCheck = reinterpret_cast<lsw::CCheckButton *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_HPF2_CHECK ));
			if ( pcbCheck ) {
				pcbCheck->SetCheck( aoOptions.apCharacteristics.bHpf2Enable );
			}
			auto aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_LPF_EDIT );
			if ( aEdit ) { aEdit->SetTextA( std::to_string( aoOptions.apCharacteristics.fLpf ).c_str() ); }

			aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_HPF0_EDIT );
			if ( aEdit ) { aEdit->SetTextA( std::to_string( aoOptions.apCharacteristics.fHpf0 ).c_str() ); }

			aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_HPF1_EDIT );
			if ( aEdit ) { aEdit->SetTextA( std::to_string( aoOptions.apCharacteristics.fHpf1 ).c_str() ); }

			aEdit = FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_CHARACTERISTICS_HPF2_EDIT );
			if ( aEdit ) { aEdit->SetTextA( std::to_string( aoOptions.apCharacteristics.fHpf2 ).c_str() ); }
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
				case CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_VOLUME_EDIT : {
					if ( _wCtrlCode == EN_CHANGE ) {
						lsw::CTrackBar * ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_VOLUME_TRACKBAR ));
						if ( ptbTrackBar ) {
							ee::CExpEvalContainer::EE_RESULT rRes;
							if ( _pwSrc->GetTextAsUInt64Expression( rRes ) ) {
								rRes.u.ui64Val = std::clamp<uint64_t>( rRes.u.ui64Val, 0, 500 );
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
			}

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
			return LSW_H_CONTINUE;
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

#endif	// #ifdef LSN_USE_WINDOWS

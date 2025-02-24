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
#include "../../Windows/WinUtilities/LSNWinUtilities.h"
#include "LSNAudioOptionsWindowLayout.h"

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
			lsw::CComboBox * pcbCombo = reinterpret_cast<lsw::CComboBox *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_DEVICE_COMBO ));
			{
				std::vector<CWinUtilities::LSN_COMBO_ENTRY> vDevices;
				for ( size_t I = 0; I < aDevices.size(); ++I ) {
					CWinUtilities::LSN_COMBO_ENTRY ceTmp = { .pwcName = reinterpret_cast<const wchar_t *>(aDevices[I].c_str()), .lpParm = LPARAM( I ) };
					vDevices.push_back( ceTmp );
				}
				lsn::CWinUtilities::FillComboBox( pcbCombo, &vDevices[0], vDevices.size(), 0, 0 );
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
				lsn::CWinUtilities::FillComboBox( pcbCombo, &vFormats[0], vFormats.size(), 0, (LSN_SF_MONO_16 << 24) | (44100 / 25) );
			}

			lsw::CTrackBar * ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_VOLUME_TRACKBAR ));
			if ( ptbTrackBar ) {
				ptbTrackBar->SetRange( TRUE, 0, 500 );
				ptbTrackBar->SetTicFreq( 50 );
				ptbTrackBar->SetPos( TRUE, 100 );
			}
			ptbTrackBar = reinterpret_cast<lsw::CTrackBar *>(FindChild( CAudioOptionsWindowLayout::LSN_AOWI_PAGE_GENERAL_BG_VOL_TRACKBAR ));
			if ( ptbTrackBar ) {
				//ptbTrackBar->SetStyle( TBS_AUTOTICKS
				ptbTrackBar->SetRange( TRUE, 0, 100 );
				ptbTrackBar->SetTicFreq( 20 );
				ptbTrackBar->SetPos( TRUE, 20 );
			}
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
			return Parent::Command( _wCtrlCode, _wId, _pwSrc );
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

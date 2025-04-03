#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The page for file settings.
 */

#include "LSNWavEditorFileSettingsPage.h"
#include "../../Localization/LSNLocalization.h"
#include "../Layout/LSNLayoutMacros.h"
#include "../WinUtilities/LSNWinUtilities.h"

#include <commdlg.h>
#include <filesystem>

#include "../../../resource.h"


namespace lsn {

	CWavEditorFileSettingsPage::CWavEditorFileSettingsPage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pwewoOptions( reinterpret_cast<LSN_WAV_EDITOR_WINDOW_OPTIONS *>(_ui64Data) ) {
	}

	// == Functions.
	/**
	 * The WM_INITDIALOG handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CWavEditorFileSettingsPage::InitDialog() {
		Parent::InitDialog();

		auto aTmp = FindChild( Layout::LSN_WEWI_FSETS_FDATA_HZ_COMBO );
		if ( aTmp ) { CWinUtilities::FillComboWithWavActialHz( aTmp, LPARAM( -LSN_PM_NTSC ) ); }

		aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_LPF_TYPE_COMBO );
		if ( aTmp ) { CWinUtilities::FillComboWithWavFilterType( aTmp, 1 ); }
		aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF0_TYPE_COMBO );
		if ( aTmp ) { CWinUtilities::FillComboWithWavFilterType( aTmp, 1 ); }
		aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF1_TYPE_COMBO );
		if ( aTmp ) { CWinUtilities::FillComboWithWavFilterType( aTmp, 1 ); }
		aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF2_TYPE_COMBO );
		if ( aTmp ) { CWinUtilities::FillComboWithWavFilterType( aTmp, 1 ); }


		// Last texts.
		aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_VOL_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsCharVolume.c_str() ); }
		aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_LPF_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsCharLpfHz.c_str() ); }
		aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF0_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsCharHpf0Hz.c_str() ); }
		aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF1_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsCharHpf1Hz.c_str() ); }
		aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF2_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsCharHpf2Hz.c_str() ); }

		aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_LPF_FALLOFF_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsCharLpfFall.c_str() ); }
		aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF0_FALLOFF_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsCharHpf0Fall.c_str() ); }
		aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF1_FALLOFF_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsCharHpf1Fall.c_str() ); }
		aTmp = FindChild( Layout::LSN_WEWI_FSETS_CHAR_HPF2_FALLOFF_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsCharHpf2Fall.c_str() ); }

		aTmp = FindChild( Layout::LSN_WEWI_FSETS_MDATA_ARTIST_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsArtist.c_str() ); }
		aTmp = FindChild( Layout::LSN_WEWI_FSETS_MDATA_ALBUM_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsAlbum.c_str() ); }
		aTmp = FindChild( Layout::LSN_WEWI_FSETS_MDATA_YEAR_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsYear.c_str() ); }
		aTmp = FindChild( Layout::LSN_WEWI_FSETS_MDATA_COMMENTS_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsComment.c_str() ); }

		Update();
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
	CWidget::LSW_HANDLED CWavEditorFileSettingsPage::Command( WORD /*_wCtrlCode*/, WORD _wId, CWidget * /*_pwSrc*/ ) {
		switch ( _wId ) {
			/*case Layout::LSN_AOWI_PAGE_RAW_PATH_BUTTON : {
				OPENFILENAMEW ofnOpenFile = { sizeof( ofnOpenFile ) };
				std::wstring szFileName;
				szFileName.resize( 0xFFFF + 2 );

				std::wstring wsFilter = std::wstring( LSN_LSTR( LSN_AUDIO_OPTIONS_WAV_TYPES ), LSN_ELEMENTS( LSN_LSTR( LSN_AUDIO_OPTIONS_WAV_TYPES ) ) - 1 );
				ofnOpenFile.hwndOwner = Wnd();
				ofnOpenFile.lpstrFilter = wsFilter.c_str();
				ofnOpenFile.lpstrFile = szFileName.data();
				ofnOpenFile.nMaxFile = DWORD( szFileName.size() );
				ofnOpenFile.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
				ofnOpenFile.lpstrInitialDir = m_poOptions->wRawAudioPath.c_str();

				if ( ::GetSaveFileNameW( &ofnOpenFile ) ) {
					m_poOptions->wRawAudioPath = std::filesystem::path( ofnOpenFile.lpstrFile ).remove_filename();
					auto pPath = std::filesystem::path( ofnOpenFile.lpstrFile );
					if ( !pPath.has_extension() ) {
						pPath += ".wav";
					}
					auto aEdit = FindChild( Layout::LSN_AOWI_PAGE_RAW_PATH_EDIT );
					if ( aEdit ) { aEdit->SetTextW( pPath.generic_wstring().c_str() ); }
				}
				break;
			}
			case Layout::LSN_AOWI_PAGE_OUT_PATH_BUTTON : {
				OPENFILENAMEW ofnOpenFile = { sizeof( ofnOpenFile ) };
				std::wstring szFileName;
				szFileName.resize( 0xFFFF + 2 );

				std::wstring wsFilter = std::wstring( LSN_LSTR( LSN_AUDIO_OPTIONS_WAV_TYPES ), LSN_ELEMENTS( LSN_LSTR( LSN_AUDIO_OPTIONS_WAV_TYPES ) ) - 1 );
				ofnOpenFile.hwndOwner = Wnd();
				ofnOpenFile.lpstrFilter = wsFilter.c_str();
				ofnOpenFile.lpstrFile = szFileName.data();
				ofnOpenFile.nMaxFile = DWORD( szFileName.size() );
				ofnOpenFile.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
				ofnOpenFile.lpstrInitialDir = m_poOptions->wOutAudioPath.c_str();

				if ( ::GetSaveFileNameW( &ofnOpenFile ) ) {
					m_poOptions->wOutAudioPath = std::filesystem::path( ofnOpenFile.lpstrFile ).remove_filename();
					auto pPath = std::filesystem::path( ofnOpenFile.lpstrFile );
					if ( !pPath.has_extension() ) {
						pPath += ".wav";
					}
					auto aEdit = FindChild( Layout::LSN_AOWI_PAGE_OUT_PATH_EDIT );
					if ( aEdit ) { aEdit->SetTextW( pPath.generic_wstring().c_str() ); }
				}
				break;
			}*/

			
		}
		Update();
		return LSW_H_CONTINUE;
	}

	/**
	 * Verifies the inputs.
	 * 
	 * \param _wsMsg The error message to display.
	 * \return Returns the control that failed or nullptr.
	 **/
	CWidget * CWavEditorFileSettingsPage::Verify( std::wstring &/*_wsMsg*/ ) {
		

		return nullptr;
	}

	/**
	 * Saves the current input configuration and closes the dialog.
	 */
	void CWavEditorFileSettingsPage::Save() {
		
	}

	/**
	 * Updates the dialog.
	 **/
	void CWavEditorFileSettingsPage::Update() {
		
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

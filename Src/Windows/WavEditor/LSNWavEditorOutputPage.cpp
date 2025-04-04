#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The page for file settings.
 */

#include "LSNWavEditorOutputPage.h"
#include "../../Localization/LSNLocalization.h"
#include "../Layout/LSNLayoutMacros.h"
#include "../../Wav/LSNWavFile.h"
#include "../WinUtilities/LSNWinUtilities.h"


namespace lsn {

	CWavEditorOutputPage::CWavEditorOutputPage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pwewoOptions( reinterpret_cast<LSN_WAV_EDITOR_WINDOW_OPTIONS *>(_ui64Data) ) {
	}

	// == Functions.
	/**
	 * The WM_INITDIALOG handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CWavEditorOutputPage::InitDialog() {
		Parent::InitDialog();

		// Combo boxes.
		auto aTmp = FindChild( Layout::LSN_WEWI_OUTPUT_MAINS_COMBO );
		if ( aTmp ) { CWinUtilities::FillComboWithWavNoiseColor( aTmp, 2 ); }

		aTmp = FindChild( Layout::LSN_WEWI_OUTPUT_NOISE_COMBO );
		if ( aTmp ) { CWinUtilities::FillComboWithWavNoiseType( aTmp, 0 ); }

		aTmp = FindChild( Layout::LSN_WEWI_OUTPUT_MASTER_FORMAT_BITS_COMBO );
		if ( aTmp ) { CWinUtilities::FillComboWithWavPcmBits( aTmp, 32, true ); }

		aTmp = FindChild( Layout::LSN_WEWI_OUTPUT_MASTER_FORMAT_FORMAT_COMBO );
		if ( aTmp ) { CWinUtilities::FillComboWithWavFormats( aTmp, CWavFile::LSN_F_PCM ); }

		aTmp = FindChild( Layout::LSN_WEWI_OUTPUT_MASTER_FORMAT_STEREO_COMBO );
		if ( aTmp ) { CWinUtilities::FillComboWithWavStereoSettings( aTmp, 1 ); }


		// Last texts.
		aTmp = FindChild( Layout::LSN_WEWI_OUTPUT_MAINS_VOL_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsMainsHumVolume.c_str() ); }
		aTmp = FindChild( Layout::LSN_WEWI_OUTPUT_NOISE_VOL_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsWhiteNoiseVolume.c_str() ); }

		aTmp = FindChild( Layout::LSN_WEWI_OUTPUT_MASTER_VOL_ABSOLUTE_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsAbsoluteVolume.c_str() ); }
		aTmp = FindChild( Layout::LSN_WEWI_OUTPUT_MASTER_VOL_NORMALIZE_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsNormalizeVolume.c_str() ); }
		aTmp = FindChild( Layout::LSN_WEWI_OUTPUT_MASTER_VOL_LOUDNESS_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsLoudnessVolume.c_str() ); }

		aTmp = FindChild( Layout::LSN_WEWI_OUTPUT_MASTER_FORMAT_HZ_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsOutputHz.c_str() ); }

		aTmp = FindChild( Layout::LSN_WEWI_OUTPUT_MASTER_PATH_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsOutputFolder.c_str() ); }


		// Last checks.
		aTmp = FindChild( Layout::LSN_WEWI_OUTPUT_MAINS_CHECK );
		if ( aTmp ) { aTmp->SetCheck( m_pwewoOptions->bMainsHum ); }
		aTmp = FindChild( Layout::LSN_WEWI_OUTPUT_NOISE_CHECK );
		if ( aTmp ) { aTmp->SetCheck( m_pwewoOptions->bWhiteNoise ); }
		aTmp = FindChild( Layout::LSN_WEWI_OUTPUT_MASTER_FORMAT_DITHER_CHECK );
		if ( aTmp ) { aTmp->SetCheck( m_pwewoOptions->bDither ); }
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
	CWidget::LSW_HANDLED CWavEditorOutputPage::Command( WORD /*_wCtrlCode*/, WORD _wId, CWidget * /*_pwSrc*/ ) {
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
	CWidget * CWavEditorOutputPage::Verify( std::wstring &/*_wsMsg*/ ) {
		

		return nullptr;
	}

	/**
	 * Saves the current input configuration and closes the dialog.
	 */
	void CWavEditorOutputPage::Save() {
		
	}

	/**
	 * Updates the dialog.
	 **/
	void CWavEditorOutputPage::Update() {
		
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

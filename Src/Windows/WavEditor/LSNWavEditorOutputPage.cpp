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
		if ( aTmp ) { CWinUtilities::FillComboWithWavNoiseColor( aTmp, m_pwewoOptions->ui32MainsHum ); }

		aTmp = FindChild( Layout::LSN_WEWI_OUTPUT_NOISE_COMBO );
		if ( aTmp ) { CWinUtilities::FillComboWithWavNoiseType( aTmp, m_pwewoOptions->ui32WhiteNoise ); }

		aTmp = FindChild( Layout::LSN_WEWI_OUTPUT_MASTER_FORMAT_BITS_COMBO );
		if ( aTmp ) { CWinUtilities::FillComboWithWavPcmBits( aTmp, m_pwewoOptions->ui32OutBits, true ); }

		aTmp = FindChild( Layout::LSN_WEWI_OUTPUT_MASTER_FORMAT_FORMAT_COMBO );
		if ( aTmp ) { CWinUtilities::FillComboWithWavFormats( aTmp, m_pwewoOptions->ui32OutFormat ); }

		aTmp = FindChild( Layout::LSN_WEWI_OUTPUT_MASTER_FORMAT_STEREO_COMBO );
		if ( aTmp ) { CWinUtilities::FillComboWithWavStereoSettings( aTmp, m_pwewoOptions->ui32Stereo ); }


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

		aTmp = FindChild( Layout::LSN_WEWI_OUTPUT_MASTER_NUMBERED_CHECK );
		if ( aTmp ) { aTmp->SetCheck( m_pwewoOptions->bNumbered ); }

		// Last combo selections.
		
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
					auto pwWidget = FindChild( Layout::LSN_AOWI_PAGE_RAW_PATH_EDIT );
					if ( pwWidget ) { pwWidget->SetTextW( pPath.generic_wstring().c_str() ); }
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
					auto pwWidget = FindChild( Layout::LSN_AOWI_PAGE_OUT_PATH_EDIT );
					if ( pwWidget ) { pwWidget->SetTextW( pPath.generic_wstring().c_str() ); }
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
	CWidget * CWavEditorOutputPage::Verify( std::wstring &_wsMsg ) {
		
#define LSN_CHECKED( ID, STORE )		{ STORE = false;						\
	auto wCheckTmp = FindChild( Layout::ID );									\
	if ( wCheckTmp ) { STORE = wCheckTmp->IsChecked(); } }

		CWidget * pwWidget = nullptr;
#define LSN_CHECK_EDIT( ID, MSG )		{										\
	pwWidget = FindChild( Layout::ID );											\
	if ( !pwWidget || !pwWidget->GetTextAsDoubleExpression( eTest ) ) {			\
		_wsMsg = MSG;															\
		return pwWidget;														\
	} }

		bool bChecked;
		ee::CExpEvalContainer::EE_RESULT eTest;
		LSN_CHECKED( LSN_WEWI_OUTPUT_MAINS_CHECK, bChecked );
		if ( bChecked ) { LSN_CHECK_EDIT( LSN_WEWI_OUTPUT_MAINS_VOL_EDIT, LSN_LSTR( LSN_WE_OUTPUT_ERR_MAINSHUM_VOL ) ); }

		LSN_CHECKED( LSN_WEWI_OUTPUT_NOISE_CHECK, bChecked );
		if ( bChecked ) { LSN_CHECK_EDIT( LSN_WEWI_OUTPUT_NOISE_VOL_EDIT, LSN_LSTR( LSN_WE_OUTPUT_ERR_WHITE_NOISE_VOL ) ); }

		LSN_CHECKED( LSN_WEWI_OUTPUT_MASTER_VOL_ABSOLUTE_RADIO, bChecked );
		if ( bChecked ) { LSN_CHECK_EDIT( LSN_WEWI_OUTPUT_MASTER_VOL_ABSOLUTE_EDIT, LSN_LSTR( LSN_WE_OUTPUT_ERR_ABSOLUTE_VOL ) ); }
		LSN_CHECKED( LSN_WEWI_OUTPUT_MASTER_VOL_NORMALIZE_RADIO, bChecked );
		if ( bChecked ) { LSN_CHECK_EDIT( LSN_WEWI_OUTPUT_MASTER_VOL_NORMALIZE_EDIT, LSN_LSTR( LSN_WE_OUTPUT_ERR_NORMALIZE_LEVEL ) ); }
		LSN_CHECKED( LSN_WEWI_OUTPUT_MASTER_VOL_LOUDNESS_RADIO, bChecked );
		if ( bChecked ) { LSN_CHECK_EDIT( LSN_WEWI_OUTPUT_MASTER_VOL_LOUDNESS_EDIT, LSN_LSTR( LSN_WE_OUTPUT_ERR_LOUDNESS_LEVEL ) ); }

		LSN_CHECK_EDIT( LSN_WEWI_OUTPUT_MASTER_FORMAT_HZ_EDIT, LSN_LSTR( LSN_WEWI_OUTPUT_MASTER_FORMAT_HZ_EDIT ) );
		if ( pwWidget ) {
			if ( std::ceil( eTest.u.dVal ) <= 0.0 || std::ceil( eTest.u.dVal ) > double( UINT_MAX ) ) {
				_wsMsg = LSN_LSTR( LSN_WEWI_OUTPUT_MASTER_FORMAT_HZ_EDIT );
				return pwWidget;
			}
		}

		pwWidget = FindChild( Layout::LSN_WEWI_OUTPUT_MASTER_PATH_EDIT );
		if ( pwWidget ) {
			auto wsText = pwWidget->GetTextW();
			if ( !wsText.size() ) {
				_wsMsg = LSN_LSTR( LSN_WE_OUTPUT_ERR_OUT_PATH_EMPTY );
				return pwWidget;
			}
		}
		
		return nullptr;
#undef LSN_CHECK_EDIT
#undef LSN_CHECKED
	}

	/**
	 * Saves the current input configuration and closes the dialog.
	 * 
	 * \param _wewoOptions The object to which to save the window state.
	 * \param _poOutput The otuput object to which to transfer all the window settings.
	 */
	void CWavEditorOutputPage::Save( LSN_WAV_EDITOR_WINDOW_OPTIONS &_wewoOptions, CWavEditor::LSN_OUTPUT * _poOutput ) {
#define LSN_CHECKED( ID, STORE )		{ STORE = false;						\
	auto wCheckTmp = FindChild( Layout::ID );									\
	if ( wCheckTmp ) { STORE = wCheckTmp->IsChecked(); } }

#define LSN_EDIT_TEXT( ID, STORE )			{									\
	pwWidget = FindChild( Layout::ID );											\
	if ( pwWidget ) { STORE = pwWidget->GetTextW(); } }

#define LSN_EDIT_VAL( ID, STORE )			if ( _poOutput ) {					\
	pwWidget = FindChild( Layout::ID );											\
	if ( pwWidget && pwWidget->GetTextAsDoubleExpression( eTest ) ) { STORE = eTest.u.dVal; } }
	
		CWidget * pwWidget = nullptr;
		ee::CExpEvalContainer::EE_RESULT eTest;

		LSN_CHECKED( LSN_WEWI_OUTPUT_MAINS_CHECK, _wewoOptions.bMainsHum );
		LSN_EDIT_TEXT( LSN_WEWI_OUTPUT_MAINS_VOL_EDIT, _wewoOptions.wsMainsHumVolume );
		LSN_EDIT_VAL( LSN_WEWI_OUTPUT_MAINS_VOL_EDIT, _poOutput->dMainsHumVol );
		LSN_CHECKED( LSN_WEWI_OUTPUT_NOISE_CHECK, _wewoOptions.bWhiteNoise );
		LSN_EDIT_TEXT( LSN_WEWI_OUTPUT_NOISE_VOL_EDIT, _wewoOptions.wsWhiteNoiseVolume );
		LSN_EDIT_VAL( LSN_WEWI_OUTPUT_NOISE_VOL_EDIT, _poOutput->dWhiteNoiseVol );

		LSN_CHECKED( LSN_WEWI_OUTPUT_MASTER_VOL_ABSOLUTE_RADIO, _wewoOptions.bAbsolute );
		LSN_EDIT_TEXT( LSN_WEWI_OUTPUT_MASTER_VOL_ABSOLUTE_EDIT, _wewoOptions.wsAbsoluteVolume );
		LSN_EDIT_VAL( LSN_WEWI_OUTPUT_MASTER_VOL_ABSOLUTE_EDIT, _poOutput->dAbsoluteVol );
		LSN_CHECKED( LSN_WEWI_OUTPUT_MASTER_VOL_NORMALIZE_RADIO, _wewoOptions.bNormalize );
		LSN_EDIT_TEXT( LSN_WEWI_OUTPUT_MASTER_VOL_NORMALIZE_EDIT, _wewoOptions.wsNormalizeVolume );
		LSN_EDIT_VAL( LSN_WEWI_OUTPUT_MASTER_VOL_NORMALIZE_EDIT, _poOutput->dNormalizeTo );
		LSN_CHECKED( LSN_WEWI_OUTPUT_MASTER_VOL_LOUDNESS_RADIO, _wewoOptions.bLoudness );
		LSN_EDIT_TEXT( LSN_WEWI_OUTPUT_MASTER_VOL_LOUDNESS_EDIT, _wewoOptions.wsLoudnessVolume );
		LSN_EDIT_VAL( LSN_WEWI_OUTPUT_MASTER_VOL_LOUDNESS_EDIT, _poOutput->dLoudness );
		
		LSN_EDIT_TEXT( LSN_WEWI_OUTPUT_MASTER_FORMAT_HZ_EDIT, _wewoOptions.wsOutputHz );

		pwWidget = FindChild( Layout::LSN_WEWI_OUTPUT_MAINS_COMBO );
		if ( pwWidget ) {
			_wewoOptions.ui32MainsHum = static_cast<uint32_t>(pwWidget->GetCurSelItemData());
		}
		pwWidget = FindChild( Layout::LSN_WEWI_OUTPUT_NOISE_COMBO );
		if ( pwWidget ) {
			_wewoOptions.ui32WhiteNoise = static_cast<uint32_t>(pwWidget->GetCurSelItemData());
		}
		
		pwWidget = FindChild( Layout::LSN_WEWI_OUTPUT_MASTER_FORMAT_FORMAT_COMBO );
		if ( pwWidget ) {
			_wewoOptions.ui32OutFormat = static_cast<uint32_t>(pwWidget->GetCurSelItemData());
		}
		pwWidget = FindChild( Layout::LSN_WEWI_OUTPUT_MASTER_FORMAT_BITS_COMBO );
		if ( pwWidget ) {
			_wewoOptions.ui32OutBits = static_cast<uint32_t>(pwWidget->GetCurSelItemData());
		}
		pwWidget = FindChild( Layout::LSN_WEWI_OUTPUT_MASTER_FORMAT_STEREO_COMBO );
		if ( pwWidget ) {
			_wewoOptions.ui32Stereo = static_cast<uint32_t>(pwWidget->GetCurSelItemData());
		}
		

		LSN_CHECKED( LSN_WEWI_OUTPUT_MASTER_FORMAT_DITHER_CHECK, _wewoOptions.bDither );
		LSN_CHECKED( LSN_WEWI_OUTPUT_MASTER_NUMBERED_CHECK, _wewoOptions.bNumbered );

		LSN_EDIT_TEXT( LSN_WEWI_OUTPUT_MASTER_PATH_EDIT, _wewoOptions.wsOutputFolder );
		LSN_CHECKED( LSN_WEWI_OUTPUT_MASTER_NUMBERED_CHECK, _wewoOptions.bNumbered );

		if ( _poOutput ) {
			double dTmp;
			LSN_EDIT_VAL( LSN_WEWI_OUTPUT_MASTER_FORMAT_HZ_EDIT, dTmp );
			_poOutput->ui32Hz = uint32_t( std::ceil( dTmp ) );
			_poOutput->bMainsHum = _wewoOptions.bMainsHum;
			_poOutput->i32MainsHumType = int32_t( _wewoOptions.ui32MainsHum );
			_poOutput->bWhiteNoise = _wewoOptions.bWhiteNoise;
			_poOutput->i32WhiteNoiseType = int32_t( _wewoOptions.ui32WhiteNoise );

			if ( _wewoOptions.bAbsolute ) { _poOutput->i32VolType = CWavEditor::LSN_VT_ABS; }
			else if ( _wewoOptions.bNormalize ) { _poOutput->i32VolType = CWavEditor::LSN_VT_NORM; }
			else { _poOutput->i32VolType = CWavEditor::LSN_VT_LOUDNESS; }

			_poOutput->i32Format = int32_t( _wewoOptions.ui32OutFormat );
			_poOutput->ui16Bits = uint16_t( _wewoOptions.ui32OutBits );
			_poOutput->i32Channels = int32_t( _wewoOptions.ui32Stereo );

			_poOutput->bDither = _wewoOptions.bDither;
			_poOutput->bNumbered = _wewoOptions.bNumbered;

			_poOutput->wsFolder = _wewoOptions.wsOutputFolder;
			_poOutput->bNumbered = _wewoOptions.bNumbered;
		}

#undef LSN_EDIT_VAL
#undef LSN_EDIT_TEXT
#undef LSN_CHECKED
	}

	/**
	 * Updates the dialog.
	 **/
	void CWavEditorOutputPage::Update() {
		
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

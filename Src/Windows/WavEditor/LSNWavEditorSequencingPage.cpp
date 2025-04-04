#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The audio options window.
 */

#include "LSNWavEditorSequencingPage.h"
#include "../../Localization/LSNLocalization.h"
#include "../Layout/LSNLayoutMacros.h"

#include <TreeListView/LSWTreeListView.h>
#include <Tab/LSWTab.h>

#include <commdlg.h>
#include <filesystem>

#include "../../../resource.h"


namespace lsn {

	CWavEditorSequencingPage::CWavEditorSequencingPage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pwewoOptions( reinterpret_cast<LSN_WAV_EDITOR_WINDOW_OPTIONS *>(_ui64Data) ) {
	}

	// == Functions.
	/**
	 * The WM_INITDIALOG handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CWavEditorSequencingPage::InitDialog() {
		Parent::InitDialog();

		auto ptlTree = reinterpret_cast<CTreeListView *>(FindChild( Layout::LSN_WEWI_SEQ_OPS_TREELISTVEW ));
		if ( ptlTree ) {
			ptlTree->SetColumnText( LSN_LSTR( LSN_WE_OPERATIONS ), 0 );
			ptlTree->SetColumnWidth( 0, 250 );
			ptlTree->InsertColumn( LSN_LSTR( LSN_WE_PARMS ), 180, -1 );
			ptlTree->SetColumnWidth( 1, 450 );
			ptlTree->InsertColumn( LSN_LSTR( LSN_WE_DESC ), 180, -1 );
			ptlTree->SetColumnWidth( 2, 550 );
		}

		// Last texts.
		auto aTmp = FindChild( Layout::LSN_WEWI_SEQ_LOOPS_DELAY_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsPreFadeDur.c_str() ); }
		aTmp = FindChild( Layout::LSN_WEWI_SEQ_LOOPS_FADE_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsFadeDur.c_str() ); }

		aTmp = FindChild( Layout::LSN_WEWI_SEQ_SILENCE_OPEN_SIL_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsOpeningSilence.c_str() ); }
		aTmp = FindChild( Layout::LSN_WEWI_SEQ_SILENCE_TRAIL_EDIT );
		if ( aTmp ) { aTmp->SetTextW( m_pwewoOptions->wsTrailingSilence.c_str() ); }


		// Last checks.
		aTmp = FindChild( Layout::LSN_WEWI_SEQ_LOOP_RADIO );
		if ( aTmp ) { aTmp->SetCheck( m_pwewoOptions->bLoop ); }
		aTmp = FindChild( Layout::LSN_WEWI_SEQ_ONE_SHOT_RADIO );
		if ( aTmp ) { aTmp->SetCheck( !m_pwewoOptions->bLoop ); }

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
	CWidget::LSW_HANDLED CWavEditorSequencingPage::Command( WORD /*_wCtrlCode*/, WORD _wId, CWidget * /*_pwSrc*/ ) {
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
	CWidget * CWavEditorSequencingPage::Verify( std::wstring &/*_wsMsg*/ ) {
		

		return nullptr;
	}

	/**
	 * Saves the current input configuration and closes the dialog.
	 */
	void CWavEditorSequencingPage::Save() {
		
	}

	/**
	 * Updates the dialog.
	 **/
	void CWavEditorSequencingPage::Update() {
		
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

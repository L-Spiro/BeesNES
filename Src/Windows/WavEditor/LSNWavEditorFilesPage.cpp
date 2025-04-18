#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The page for loading files.
 */

#include "LSNWavEditorFilesPage.h"
#include "LSNWavEditorWindow.h"
#include "../../Localization/LSNLocalization.h"
#include "../Layout/LSNLayoutMacros.h"

#include <TreeListView/LSWTreeListView.h>

#include <commdlg.h>
#include <filesystem>

#include "../../../resource.h"


namespace lsn {

	CWavEditorFilesPage::CWavEditorFilesPage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pwewoOptions( reinterpret_cast<LSN_WAV_EDITOR_WINDOW_OPTIONS *>(_ui64Data) ) {
	}

	// == Functions.
	/**
	 * The WM_INITDIALOG handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CWavEditorFilesPage::InitDialog() {
		Parent::InitDialog();

		auto ptlTree = reinterpret_cast<CTreeListView *>(FindChild( Layout::LSN_WEWI_FILES_TREELISTVIEW ));
		if ( ptlTree ) {
			ptlTree->SetColumnText( LSN_LSTR( LSN_WE_FILES ), 0 );
			ptlTree->SetColumnWidth( 0, 350 );
			ptlTree->InsertColumn( LSN_LSTR( LSN_PATCH_DETAILS ), 180, -1 );
			ptlTree->SetColumnWidth( 1, 550 );
		}

		Update();
		return LSW_H_CONTINUE;
	}

	/**
	 * Called when a new WAV file has been loaded.
	 * 
	 * \param _ui32Id The ID of the WAV file to add.
	 * \return Returns true if the WAV file was added to the tree.
	 **/
	bool CWavEditorFilesPage::AddToTree( uint32_t _ui32Id ) {
		auto * pItem = m_pweEditor->WavById( _ui32Id );
		if ( !pItem ) { return false; }
		auto ptlTree = reinterpret_cast<CTreeListView *>(FindChild( Layout::LSN_WEWI_FILES_TREELISTVIEW ));
		if ( ptlTree ) {
			std::wstring wsTmp = std::filesystem::path( pItem->wfFile.wsPath ).filename().generic_wstring();
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( reinterpret_cast<const WCHAR *>(wsTmp.c_str()),
				_ui32Id, TVI_ROOT );
			HTREEITEM hItem = ptlTree->InsertItem( &isInsertMe );
			ptlTree->SetItemText( hItem, std::format( LSN_LSTR( LSN_WE_FILES_DESC ), pItem->wfFile.fcFormat.uiNumChannels, pItem->wfFile.ui64Samples / double( pItem->wfFile.fcFormat.uiSampleRate ), pItem->wfFile.fcFormat.uiSampleRate ).c_str(), 1 );
			return true;
		}
		return false;
	}

	/**
	 * Handles the WM_COMMAND message.
	 *
	 * \param _wCtrlCode 0 = from menu, 1 = from accelerator, otherwise it is a Control-defined notification code.
	 * \param _wId The ID of the control if _wCtrlCode is not 0 or 1.
	 * \param _pwSrc The source control if _wCtrlCode is not 0 or 1.
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CWavEditorFilesPage::Command( WORD /*_wCtrlCode*/, WORD _wId, CWidget * /*_pwSrc*/ ) {
		switch ( _wId ) {
			case Layout::LSN_WEWI_FILES_ADD_BUTTON : {
				if ( m_pwParent ) {
					OPENFILENAMEW ofnOpenFile = { sizeof( ofnOpenFile ) };
					std::wstring szFileName;
					szFileName.resize( 0x1FFFF + 2 );

					std::wstring wsFilter = std::wstring( LSN_LSTR( LSN_AUDIO_OPTIONS_WAV_TYPES ), LSN_ELEMENTS( LSN_LSTR( LSN_AUDIO_OPTIONS_WAV_TYPES ) ) - 1 );
					ofnOpenFile.hwndOwner = Wnd();
					ofnOpenFile.lpstrFilter = wsFilter.c_str();
					ofnOpenFile.nFilterIndex = 1;
					ofnOpenFile.lpstrFile = szFileName.data();
					ofnOpenFile.nMaxFile = DWORD( szFileName.size() );
					ofnOpenFile.Flags = OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
					ofnOpenFile.lpstrInitialDir = m_pwewoOptions->wsLastWavFolder.c_str();

					if ( ::GetOpenFileNameW( &ofnOpenFile ) ) {
						auto ptlTree = reinterpret_cast<CTreeListView *>(FindChild( Layout::LSN_WEWI_FILES_TREELISTVIEW ));
						ptlTree->BeginLargeUpdate();
						std::wstring wsDir = ofnOpenFile.lpstrFile;
						const wchar_t * pwcTmp = ofnOpenFile.lpstrFile + wsDir.size() + 1;
						if ( (*pwcTmp) == L'\0' ) {
							m_pwewoOptions->wsLastWavFolder = std::filesystem::path( ofnOpenFile.lpstrFile ).remove_filename();
							static_cast<CWavEditorWindow *>(m_pwParent)->AddWavFiles( wsDir );
						}
						else {
							m_pwewoOptions->wsLastWavFolder = std::filesystem::path( ofnOpenFile.lpstrFile );
							while ( (*pwcTmp) ) {
								std::wstring wsFile = pwcTmp;
								static_cast<CWavEditorWindow *>(m_pwParent)->AddWavFiles( wsDir + L"\\" + wsFile );
								pwcTmp += wsFile.size() + 1;
							}
						}
						ptlTree->FinishUpdate();
					}
				}
				break;
			}
			case Layout::LSN_WEWI_FILES_REMOVE_BUTTON : {
				if ( m_pwParent ) {
					auto ptlTree = reinterpret_cast<CTreeListView *>(FindChild( Layout::LSN_WEWI_FILES_TREELISTVIEW ));
					if ( ptlTree ) {
						std::vector<LPARAM> vSelected;
						if ( ptlTree->GatherSelectedLParam( vSelected, true ) ) {
							static_cast<CWavEditorWindow *>(m_pwParent)->Remove( vSelected );
							ptlTree->BeginLargeUpdate();
							for ( auto I = vSelected.size(); I--; ) {
								ptlTree->DeleteByLParam( vSelected[I] );
							}
							ptlTree->FinishUpdate();
						}
					}
				}
				break;
			}
			case Layout::LSN_WEWI_FILES_UP_BUTTON : {
				if ( m_pwParent ) {
					auto ptlTree = reinterpret_cast<CTreeListView *>(FindChild( Layout::LSN_WEWI_FILES_TREELISTVIEW ));
					if ( ptlTree ) {
						std::vector<LPARAM> vSelected;
						if ( ptlTree->GatherSelectedLParam( vSelected, true ) ) {
							static_cast<CWavEditorWindow *>(m_pwParent)->MoveUp( vSelected );
							ptlTree->MoveUp( vSelected );
						}
					}
				}
				break;
			}
			case Layout::LSN_WEWI_FILES_DOWN_BUTTON : {
				if ( m_pwParent ) {
					auto ptlTree = reinterpret_cast<CTreeListView *>(FindChild( Layout::LSN_WEWI_FILES_TREELISTVIEW ));
					if ( ptlTree ) {
						std::vector<LPARAM> vSelected;
						if ( ptlTree->GatherSelectedLParam( vSelected, true ) ) {
							//static_cast<CWavEditorWindow *>(m_pwParent)->MoveUp( vSelected );
							ptlTree->MoveDown( vSelected );
						}
					}
				}
				break;
			}
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
	CWidget * CWavEditorFilesPage::Verify( std::wstring &/*_wsMsg*/ ) {
		

		return nullptr;
	}

	/**
	 * Saves the current input configuration and closes the dialog.
	 */
	void CWavEditorFilesPage::Save() {
		
	}

	/**
	 * Updates the dialog.
	 **/
	void CWavEditorFilesPage::Update() {
		
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

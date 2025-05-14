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
#include "../Layout/LSNLayoutManager.h"

#include <ToolBar/LSWToolBar.h>
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
			ptlTree->SetColumnWidth( 0, 287 );
			ptlTree->InsertColumn( LSN_LSTR( LSN_PATCH_DETAILS ), 242, -1 );
			//ptlTree->SetColumnWidth( 1, 246 );
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
							ptlTree->BeginLargeUpdate();
							for ( auto I = vSelected.size(); I--; ) {
								ptlTree->DeleteByLParam( vSelected[I] );
							}
							ptlTree->FinishUpdate();
							static_cast<CWavEditorWindow *>(m_pwParent)->Remove( vSelected );
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
							ptlTree->MoveUp( vSelected );
							static_cast<CWavEditorWindow *>(m_pwParent)->MoveUp( vSelected );
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
							ptlTree->MoveDown( vSelected );
							static_cast<CWavEditorWindow *>(m_pwParent)->MoveDown( vSelected );
						}
					}
				}
				break;
			}
			case Layout::LSN_WEWI_SELECT_ALL : {
				auto ptlTree = reinterpret_cast<CTreeListView *>(FindChild( Layout::LSN_WEWI_FILES_TREELISTVIEW ));
				if ( ptlTree ) {
					ptlTree->SelectRootItems();
					std::vector<LPARAM> vSelected;
					if ( ptlTree->GatherSelectedLParam( vSelected, true ) ) {
						static_cast<CWavEditorWindow *>(m_pwParent)->SelectionChanged( vSelected );
					}
				}
				break;
			}
		}
		Update();
		return LSW_H_CONTINUE;
	}

	/**
	 * Handles the WM_CONTEXTMENU message.
	 * 
	 * \param _pwControl The control that was clicked.
	 * \param _iX The horizontal position of the cursor, in screen coordinates, at the time of the mouse click.
	 * \param _iY The vertical position of the cursor, in screen coordinates, at the time of the mouse click.
	 * \return Returns an LSW_HANDLED code.
	 **/
	CWidget::LSW_HANDLED CWavEditorFilesPage::ContextMenu( CWidget * _pwControl, INT _iX, INT _iY ) {
		if ( _pwControl->Id() == Layout::LSN_WEWI_FILES_TREELISTVIEW ) {
			auto ptlvTree = static_cast<CTreeListView *>(_pwControl);
			bool bExpSel = ptlvTree->AnySelectedHasUnexpandedChildren();
			bool bExpAll = ptlvTree->AnyHasUnexpandedChildren();
			bool bColSel = ptlvTree->AnySelectedHasExpandedChildren();
			bool bColAll = ptlvTree->AnyHasExpandedChildren();
			bool bHasItems = ptlvTree->HasItem();
			std::vector<size_t> vSelected;
			ptlvTree->GatherSelected( vSelected );
			bool bSelected = vSelected.size() != 0;
			LSW_MENU_ITEM miMenuBar[] = {
				{ FALSE,		Layout::LSN_WEWI_FILES_ADD_BUTTON,				FALSE,		FALSE,		TRUE,					LSN_LSTR( LSN_WE_ADD_WAV ),					FALSE },
				{ FALSE,		Layout::LSN_WEWI_FILES_REMOVE_BUTTON,			FALSE,		FALSE,		bSelected && bHasItems,	LSN_LSTR( LSN_WE_REMOVE ),					FALSE },
				{ FALSE,		Layout::LSN_WEWI_FILES_UP_BUTTON,				FALSE,		FALSE,		bSelected && bHasItems,	LSN_LSTR( LSN_WE_MOVE_UP ),					FALSE },
				{ FALSE,		Layout::LSN_WEWI_FILES_DOWN_BUTTON,				FALSE,		FALSE,		bSelected && bHasItems,	LSN_LSTR( LSN_WE_MOVE_DOWN ),				FALSE },
				{ TRUE,			0,												FALSE,		FALSE,		bHasItems,				nullptr,									FALSE },
				{ FALSE,		Layout::LSN_WEWI_SELECT_ALL,					FALSE,		FALSE,		bHasItems,				LSN_LSTR( LSN_WE_SELECT_ALL ),				FALSE },
				{ TRUE,			0,												FALSE,		FALSE,		bHasItems,				nullptr,									FALSE },
				{ FALSE,		Layout::LSN_WEWI_EXPAND_SELECTED,				FALSE,		FALSE,		bHasItems && bExpSel,	LSN_LSTR( LSN_PATCH_EXPAND_SELECTED ),		FALSE },
				{ FALSE,		Layout::LSN_WEWI_EXPAND_ALL,					FALSE,		FALSE,		bHasItems && bExpAll,	LSN_LSTR( LSN_PATCH_EXPAND_ALL ),			FALSE },
				{ FALSE,		Layout::LSN_WEWI_COLLAPSE_SELECTED,				FALSE,		FALSE,		bHasItems && bColSel,	LSN_LSTR( LSN_PATCH_COLLAPSE_SELECTED ),	FALSE },
				{ FALSE,		Layout::LSN_WEWI_COLLAPSE_ALL,					FALSE,		FALSE,		bHasItems && bColAll,	LSN_LSTR( LSN_PATCH_COLLAPSE_ALL ),			FALSE },
				
			};

			const LSW_MENU_LAYOUT miMenus[] = {
				{
					LSN_M_CONTEXT_MENU,
					0,
					0,
					LSN_ELEMENTS( miMenuBar ),
					miMenuBar
				},
			};
			lsn::CLayoutManager * plmLayout = static_cast<lsn::CLayoutManager *>(lsw::CBase::LayoutManager());
			Command( 0, static_cast<WORD>(plmLayout->CreatePopupMenuEx( this, miMenus, LSN_ELEMENTS( miMenus ), _iX, _iY, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD )), ptlvTree );
			return LSW_H_HANDLED;
		}
		return Parent::ContextMenu( _pwControl, _iX, _iY );
	}

	/**
	 * The WM_NOTIFY -> LVN_ITEMCHANGED handler.
	 *
	 * \param _lplvParm The notifacation structure.
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CWavEditorFilesPage::Notify_ItemChanged( LPNMLISTVIEW /*_lplvParm*/ ) {
		auto ptlTree = reinterpret_cast<CTreeListView *>(FindChild( Layout::LSN_WEWI_FILES_TREELISTVIEW ));
		if ( ptlTree ) {
			std::vector<LPARAM> vSelected;
			ptlTree->GatherSelectedLParam( vSelected, true );
			static_cast<CWavEditorWindow *>(m_pwParent)->SelectionChanged( vSelected );
			Update();
		}
		return LSW_H_CONTINUE;
	}

	/**
	 * The WM_NOTIFY -> LVN_ODSTATECHANGED handler.
	 *
	 * \param _lposcParm The notifacation structure.
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CWavEditorFilesPage::Notify_OdStateChange( LPNMLVODSTATECHANGE /*_lposcParm*/ ) {
		auto ptlTree = reinterpret_cast<CTreeListView *>(FindChild( Layout::LSN_WEWI_FILES_TREELISTVIEW ));
		if ( ptlTree ) {
			std::vector<LPARAM> vSelected;
			ptlTree->GatherSelectedLParam( vSelected, true );
			static_cast<CWavEditorWindow *>(m_pwParent)->SelectionChanged( vSelected );
			Update();
		}
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
		bool bHasItems = false;
		bool bSelected = false;
		auto ptlTree = reinterpret_cast<CTreeListView *>(FindChild( Layout::LSN_WEWI_FILES_TREELISTVIEW ));
		if ( ptlTree ) {
			bHasItems = ptlTree->HasItem();
			std::vector<size_t> vSelected;
			ptlTree->GatherSelected( vSelected );
			bSelected = vSelected.size() != 0;
		}
		{
			struct LSN_CONTROLS {
				Layout::LSN_WAV_EDITOR_WINDOW_IDS						wId;
				bool													bCloseCondition0;
			} cControls[] = {
				{ Layout::LSN_WEWI_FILES_ADD_META_BUTTON,				bSelected },
				{ Layout::LSN_WEWI_FILES_REMOVE_BUTTON,					bSelected },
				{ Layout::LSN_WEWI_FILES_UP_BUTTON,						bSelected },
				{ Layout::LSN_WEWI_FILES_DOWN_BUTTON,					bSelected },
			};
			for ( auto I = LSN_ELEMENTS( cControls ); I--; ) {
				auto pwThis = FindChild( cControls[I].wId );
				if ( pwThis ) {
					pwThis->SetEnabled( cControls[I].bCloseCondition0 );
				}
			}
		}
		if ( m_pwParent ) {
			CToolBar * plvToolBar = static_cast<CToolBar *>(m_pwParent->FindChild( Layout::LSN_WEWI_TOOLBAR0 ));
			if ( plvToolBar ) {
				struct LSN_CONTROLS {
					Layout::LSN_WAV_EDITOR_WINDOW_IDS					wId;
					bool												bCloseCondition0;
				} cControls[] = {
					//{ Layout::LSN_WEWI_FILES_ADD_BUTTON,				bSelected },
					{ Layout::LSN_WEWI_FILES_REMOVE_BUTTON,				bSelected },
					{ Layout::LSN_WEWI_FILES_UP_BUTTON,					bSelected },
					{ Layout::LSN_WEWI_FILES_DOWN_BUTTON,				bSelected },
				};
				for ( auto I = LSN_ELEMENTS( cControls ); I--; ) {
					plvToolBar->EnableButton( WORD( cControls[I].wId ), cControls[I].bCloseCondition0 );
				}
			}
		}
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

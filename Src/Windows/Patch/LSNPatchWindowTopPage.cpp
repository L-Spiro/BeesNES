#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The top part of the patch window.
 */

#include "LSNPatchWindowTopPage.h"
#include "../../Crc/LSNCrc.h"
#include "../../File/LSNStdFile.h"
#include "../../File/LSNZipFile.h"
#include "../../Localization/LSNLocalization.h"
#include "../../Patching/LSNBpsFile.h"
#include "../../Patching/LSNIpsFile.h"
#include "../../System/LSNSystemBase.h"
#include "../../Windows/Layout/LSNLayoutManager.h"
#include "../WinUtilities/LSNWinUtilities.h"

#include <commdlg.h>
#include <filesystem>
#include <set>
#include <shlobj.h>


namespace lsn {

	CPatchWindowTopPage::CPatchWindowTopPage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_poOptions( reinterpret_cast<LSN_OPTIONS *>(_ui64Data) ),
		m_bOutIsAutoFilled( false ) {
	}
	CPatchWindowTopPage::~CPatchWindowTopPage() {
	}

	// == Functions.
	/**
	 * The WM_INITDIALOG handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CPatchWindowTopPage::InitDialog() {
		Parent::InitDialog();
		lsw::CTreeListView * ptlTree = static_cast<lsw::CTreeListView *>(FindChild( CPatchWindowLayout::LSN_PWI_FILE_PATCH_TREELISTVIEW ));
		if ( ptlTree ) {
			ptlTree->SetColumnText( LSN_LSTR( LSN_PATCH_PATCH ), 0 );
			ptlTree->SetColumnWidth( 0, 450 );
			ptlTree->InsertColumn( LSN_LSTR( LSN_PATCH_DETAILS ), 180, -1 );
			ptlTree->SetColumnWidth( 1, 550 );
		}

		UpdateText();
		return LSW_H_CONTINUE;
	}

	/**
	 * The WM_CLOSE handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CPatchWindowTopPage::Close() {
		//::DestroyWindow( Wnd() );
		/*if ( m_pwParent ) {
			::PostMessageW( m_pwParent->Wnd(), CWinUtilities::LSN_CLOSE_PATCHER, 0, 0 );
		}
		::EndDialog( Wnd(), 0 );*/
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
	CWidget::LSW_HANDLED CPatchWindowTopPage::Command( WORD _wCtrlCode, WORD _wId, CWidget * _pwSrc ) {
		switch ( _wId ) {
			case CPatchWindowLayout::LSN_PWI_FILE_IN_BUTTON : {
				OPENFILENAMEW ofnOpenFile = { sizeof( ofnOpenFile ) };
				std::u16string szFileName;
				szFileName.resize( 0xFFFF + 2 );

				ofnOpenFile.hwndOwner = Wnd();
				ofnOpenFile.lpstrFilter = LSN_LSTR( LSN_ALL_SUPPORTED_FILES___NES____ZIP____NES___ZIP_ );
				ofnOpenFile.lpstrFile = reinterpret_cast<LPWSTR>(&szFileName[0]);
				ofnOpenFile.nMaxFile = DWORD( szFileName.size() );
				ofnOpenFile.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
				ofnOpenFile.lpstrInitialDir = m_poOptions->wsInRomInitPath.c_str();
	
				std::u16string u16Path, u16FileName;
				if ( ::GetOpenFileNameW( &ofnOpenFile ) ) {
					m_poOptions->wsInRomInitPath = std::filesystem::path( ofnOpenFile.lpstrFile ).remove_filename();

					lsn::CZipFile zfFile;
					if ( zfFile.Open( reinterpret_cast<const char16_t *>(ofnOpenFile.lpstrFile) ) && zfFile.IsArchive() ) {
						std::vector<std::u16string> vFiles;
						zfFile.GatherArchiveFiles( vFiles, u".nes" );

						if ( vFiles.size() == 1 ) {
							u16Path = reinterpret_cast<const char16_t *>(ofnOpenFile.lpstrFile);
							u16Path += u'{';
							u16Path += vFiles[0];
							u16Path += u'}';
							u16FileName = std::filesystem::path( vFiles[0] ).filename().u16string();
						}
					}
					else if ( !zfFile.IsArchive() ) {
						u16Path = reinterpret_cast<const char16_t *>(ofnOpenFile.lpstrFile);
						u16FileName = std::filesystem::path( ofnOpenFile.lpstrFile ).filename().u16string();
					}
					auto pwPathEdit = FindChild( CPatchWindowLayout::LSN_PWI_FILE_IN_EDIT );
					if ( pwPathEdit ) {
						pwPathEdit->SetTextW( reinterpret_cast<const wchar_t *>(u16Path.c_str()) );
					}
				}
				break;
			}
			case CPatchWindowLayout::LSN_PWI_FILE_PATCH_BUTTON : {
				OPENFILENAMEW ofnOpenFile = { sizeof( ofnOpenFile ) };
				std::u16string szFileName;
				szFileName.resize( 0xFFFF + 2 );

				ofnOpenFile.hwndOwner = Wnd();
				ofnOpenFile.lpstrFilter = LSN_LSTR( LSN_PATCH_FILE_TYPES );
				ofnOpenFile.lpstrFile = reinterpret_cast<LPWSTR>(&szFileName[0]);
				ofnOpenFile.nMaxFile = DWORD( szFileName.size() );
				ofnOpenFile.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
				ofnOpenFile.lpstrInitialDir = m_poOptions->wsPatchInitPath.c_str();
				
				std::u16string u16Path;
				if ( ::GetOpenFileNameW( &ofnOpenFile ) ) {
					m_poOptions->wsPatchInitPath = std::filesystem::path( ofnOpenFile.lpstrFile ).remove_filename();
					size_t sBpsStart = 1, sTxtStart = 1;
					lsn::CZipFile zfFile;
					if ( zfFile.Open( reinterpret_cast<const char16_t *>(ofnOpenFile.lpstrFile) ) && zfFile.IsArchive() ) {
						std::vector<std::u16string> vFiles;
						std::vector<std::vector<uint8_t>> vPatchImage;
						try {
							zfFile.ExtractToMemory( vFiles, vPatchImage, u".ips" );
							sBpsStart = vFiles.size();
							zfFile.ExtractToMemory( vFiles, vPatchImage, u".bps" );
							sTxtStart = vFiles.size();
							zfFile.ExtractToMemory( vFiles, vPatchImage, u".txt" );
							zfFile.ExtractToMemory( vFiles, vPatchImage, u".md" );
						}
						catch ( ... ) {
							// Stack overflow is the only possible exception.
						}

						try {
							m_vPatchInfo.clear();
							m_vPatchInfo.resize( vFiles.size() );
							for ( size_t I = 0; I < vFiles.size(); ++I ) {
								m_vPatchInfo[I].u16FullPath = reinterpret_cast<const char16_t *>(ofnOpenFile.lpstrFile);
								m_vPatchInfo[I].u16FullPath += u'{' + vFiles[I] + u'}';
								m_vPatchInfo[I].vLoadedPatchFile = std::move( vPatchImage[I] );
								u16Path += m_vPatchInfo[I].u16FullPath;
								u16Path += u';';
							}
						}
						catch ( ... ) {
						}
						
					}
					else if ( !zfFile.IsArchive() ) {
						lsn::CStdFile sfFile;
						if ( sfFile.Open( reinterpret_cast<const char16_t *>(ofnOpenFile.lpstrFile) ) ) {
							try {
								m_vPatchInfo.clear();
								m_vPatchInfo.resize( 1 );
								m_vPatchInfo[0].u16FullPath = reinterpret_cast<const char16_t *>(ofnOpenFile.lpstrFile);
								auto wsExt = CUtilities::ToLower( std::filesystem::path( m_vPatchInfo[0].u16FullPath ).extension().wstring() );
								if ( wsExt == L".bps" ) {
									sBpsStart = 0;
								}
								else if ( wsExt == L".txt" || wsExt == L".md" ) {
									sTxtStart = 0;
								}
							}
							catch ( ... ) {
							}
							if ( !sfFile.LoadToMemory( m_vPatchInfo[0].vLoadedPatchFile ) ) {
							}
							u16Path = reinterpret_cast<const char16_t *>(ofnOpenFile.lpstrFile);
						}
					}

					try {
						for ( size_t I = 0; I < m_vPatchInfo.size(); ++I ) {
							m_vPatchInfo[I].ui32PatchCrc = CCrc::GetCrc( m_vPatchInfo[I].vLoadedPatchFile.data(), m_vPatchInfo[I].vLoadedPatchFile.size() );
							if ( I < sTxtStart ) {
								if ( m_vPatchInfo[I].vLoadedPatchFile.size() >= 4 ) {
									m_vPatchInfo[I].ui32PatchCrcMinus4 = CCrc::GetCrc( m_vPatchInfo[I].vLoadedPatchFile.data(), m_vPatchInfo[I].vLoadedPatchFile.size() - 4 );
								}
							}
							if ( I >= sBpsStart && I < sTxtStart ) {
								if ( m_vPatchInfo[I].vLoadedPatchFile.size() >= 12 ) {
									m_vPatchInfo[I].ui32Crc = (*reinterpret_cast<uint32_t *>(&m_vPatchInfo[I].vLoadedPatchFile[m_vPatchInfo[I].vLoadedPatchFile.size()-12]));
								}
							}
							if ( I >= sTxtStart ) {
								m_vPatchInfo[I].bIsText = true;
							}

							//::OutputDebugStringW( reinterpret_cast<const wchar_t *>((m_vPatchInfo[I].u16FullPath + u"\r\n").c_str()) );
						}


						std::set<std::u16string> sPaths;
						for ( size_t I = 0; I < m_vPatchInfo.size(); ++I ) {
							CUtilities::LSN_FILE_PATHS fpPath;
							auto u16Tmp = m_vPatchInfo[I].u16FullPath;
							u16Tmp = CUtilities::Replace( u16Tmp, std::u16string( u"{" ), std::u16string( u"\\" ) );
							u16Tmp = CUtilities::Replace( u16Tmp, std::u16string( u"}" ), std::u16string( u"" ) );
							u16Tmp = CUtilities::Replace( u16Tmp, u'/', u'\\' );
							CUtilities::DeconstructFilePath( u16Tmp.c_str(), fpPath );
							auto aTmp = CUtilities::Utf16ToUtf8( u16Tmp.c_str() );
							std::u8string u8Str = CUtilities::XStringToU8String( aTmp.c_str(), aTmp.size() );
							m_vPatchInfo[I].vTokenizedFolders = ee::CExpEval::TokenizeUtf( u8Str, '\\', false );
						}
						auto aTmp = CreateBasicTree( m_vPatchInfo );
						SimplifyTree( aTmp );
						auto ptvTree = static_cast<lsw::CTreeListView *>(FindChild( CPatchWindowLayout::LSN_PWI_FILE_PATCH_TREELISTVIEW ));
						ptvTree->DeleteAll();
						ptvTree->BeginLargeUpdate();
						AddToTree( aTmp, TVI_ROOT, ptvTree );
						UpdateColors();
						ptvTree->FinishUpdate();
						UpdateText();
					}
					catch ( ... ) {
					}
					auto pwPathEdit = FindChild( CPatchWindowLayout::LSN_PWI_FILE_PATCH_TREELISTVIEW );
					if ( pwPathEdit ) {
						pwPathEdit->SetTextW( reinterpret_cast<const wchar_t *>(u16Path.c_str()) );
						//::OutputDebugStringW( reinterpret_cast<const wchar_t *>((u16Path + u"\r\n").c_str()) );
					}
				}
				break;
			}
			/*case CPatchWindowLayout::LSN_PWI_FILE_OUT_BUTTON : {
				OPENFILENAMEW ofnOpenFile = { sizeof( ofnOpenFile ) };
				std::u16string szFileName;
				szFileName.resize( 0xFFFF + 2 );

				ofnOpenFile.hwndOwner = Wnd();
				ofnOpenFile.lpstrFilter = LSN_LSTR( LSN_NES_FILES____NES____NES_ );
				ofnOpenFile.lpstrFile = reinterpret_cast<LPWSTR>(&szFileName[0]);
				ofnOpenFile.nMaxFile = DWORD( szFileName.size() );
				ofnOpenFile.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;
				ofnOpenFile.lpstrInitialDir = m_poOptions->wsOutRomInitPath.c_str();
	
				std::u16string u16Path, u16FileName;
				if ( ::GetSaveFileNameW( &ofnOpenFile ) ) {
					m_poOptions->wsOutRomInitPath = std::filesystem::path( ofnOpenFile.lpstrFile ).remove_filename();

					u16Path = reinterpret_cast<const char16_t *>(ofnOpenFile.lpstrFile);
					u16FileName = std::filesystem::path( ofnOpenFile.lpstrFile ).filename().u16string();
					auto pwPathEdit = FindChild( CPatchWindowLayout::LSN_PWI_FILE_OUT_EDIT );
					if ( pwPathEdit ) {
						pwPathEdit->SetTextW( reinterpret_cast<const wchar_t *>(u16Path.c_str()) );
						m_bOutIsAutoFilled = false;
					}
				}
				break;
			}*/
			case CPatchWindowLayout::LSN_PWI_FILE_IN_EDIT : {
				if ( _wCtrlCode == EN_CHANGE ) {
					m_vPatchRomFile.clear();
					auto wsText = _pwSrc->GetTextW();
					CUtilities::LSN_FILE_PATHS fpPath;
					CUtilities::DeconstructFilePath( reinterpret_cast<const char16_t *>(wsText.c_str()), fpPath );
					if ( CUtilities::LastChar( fpPath.u16sFullPath ) == u'}' ) {
						lsn::CZipFile zfFile;
						auto aZipPath = fpPath.u16sPath;
						aZipPath.pop_back();
						if ( zfFile.Open( aZipPath.c_str() ) && zfFile.IsArchive() ) {
							if ( zfFile.ExtractToMemory( fpPath.u16sFile, m_vPatchRomFile ) ) {
							}
						}
					}
					else {
						lsn::CStdFile sfFile;
						if ( sfFile.Open( fpPath.u16sFullPath.c_str() ) ) {
							if ( sfFile.LoadToMemory( m_vPatchRomFile ) ) {
							}
						}
					}
					m_u16RomPath = fpPath.u16sPath + fpPath.u16sFile;

					/*auto pwPathEdit = FindChild( CPatchWindowLayout::LSN_PWI_FILE_OUT_EDIT );
					if ( pwPathEdit ) {
						if ( m_bOutIsAutoFilled || !pwPathEdit->GetTextW().size() ) {
							if ( CUtilities::LastChar( fpPath.u16sFullPath ) == u'}' ) { fpPath.u16sPath.pop_back(); }
							std::filesystem::path pPath( fpPath.u16sPath );
							auto aTmp = pPath.remove_filename();
							aTmp /= fpPath.u16sFile;
							aTmp = aTmp.replace_filename( CUtilities::NoExtension( std::filesystem::path( fpPath.u16sFile ).filename().u16string() ) + u" Patched" );

							auto aFinal = aTmp;
							aFinal += ".nes";
							size_t sIdx = 0;
							while ( std::filesystem::exists( aFinal ) ) {
								aFinal = aTmp;
								aFinal += " " + std::to_string( sIdx++ );
								aFinal += ".nes";
							}
							
							pwPathEdit->SetTextW( aFinal.native().c_str() );
							m_bOutIsAutoFilled = true;
							m_poOptions->wsOutRomInitPath = aFinal.remove_filename();
						}
					}*/
					UpdateInfo();
					auto ptvTree = static_cast<lsw::CTreeListView *>(FindChild( CPatchWindowLayout::LSN_PWI_FILE_PATCH_TREELISTVIEW ));
					if ( ptvTree ) {
						ptvTree->BeginLargeUpdate();
						UpdateColors();
						ptvTree->FinishUpdate();
						UpdateText();
					}
					return LSW_H_CONTINUE;
				}
				break;
			}
			case CPatchWindowLayout::LSN_PWI_FILE_QUICK_PATCH_BUTTON : {
				auto ptvTree = static_cast<lsw::CTreeListView *>(FindChild( CPatchWindowLayout::LSN_PWI_FILE_PATCH_TREELISTVIEW ));
				if ( ptvTree && m_vPatchRomFile.size() ) {
					try {
						std::vector<HTREEITEM> vSelected;
						if ( ptvTree->GatherSelected( vSelected, true ) ) {
							// Trim the selection down to only patch files.
							bool bWarnOfDoom = false;
							TrimVector( ptvTree, vSelected, bWarnOfDoom );

							// Act on whatever remains.
							if ( vSelected.size() == 1 ) {
								if ( !bWarnOfDoom || lsw::CBase::PromptYesNo( Wnd(), LSN_LSTR( LSN_PATCH_WARNING_INCOMPATIBLE_FILE_1 ), LSN_LSTR( LSN_PATCH_WARNING_INCOMPATIBLE_FILE_TITLE ) ) ) {
									size_t sIdx = size_t( ptvTree->GetItemLParam( vSelected[0] ) );

									// Prompt for the save path.
									OPENFILENAMEW ofnOpenFile = { sizeof( ofnOpenFile ) };
									std::wstring szFileName;
									szFileName.resize( 0xFFFF + 2 );

									std::wstring wsFilter = std::wstring( LSN_LSTR( LSN_NES_FILES____NES____NES_ ), std::size( LSN_LSTR( LSN_NES_FILES____NES____NES_ ) ) - 1 );
									ofnOpenFile.hwndOwner = Wnd();
									ofnOpenFile.lpstrFilter = wsFilter.c_str();
									ofnOpenFile.lpstrFile = szFileName.data();
									ofnOpenFile.nMaxFile = DWORD( szFileName.size() );
									ofnOpenFile.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
									ofnOpenFile.lpstrInitialDir = m_poOptions->wsPatchOutFolder.c_str();
									if ( !m_poOptions->wsPatchOutFolder.size() ) {
										ofnOpenFile.lpstrInitialDir = m_poOptions->wsInRomInitPath.c_str();
									}

									CUtilities::LSN_FILE_PATHS fpPath;
									CUtilities::DeconstructFilePath( m_vPatchInfo[sIdx].u16FullPath, fpPath );
									{
										try {
											std::wstring wsTmpFileName = DefaultPatchName( m_rRomInfo.riInfo.s16RomName, fpPath.u16sFile ).generic_wstring();
											::lstrcpynW( ofnOpenFile.lpstrFile, wsTmpFileName.c_str(), ofnOpenFile.nMaxFile - 1 );
										}
										catch ( ... ) {}
									}

									if ( ::GetSaveFileNameW( &ofnOpenFile ) ) {
										auto pPath = std::filesystem::path( ofnOpenFile.lpstrFile );
										if ( !pPath.has_extension() ) {
											pPath += ".nes";
										}
										m_poOptions->wsPatchOutFolder = std::filesystem::path( pPath ).remove_filename().generic_wstring();

										try {
											CStream sPatch( m_vPatchInfo[sIdx].vLoadedPatchFile );
											LSN_PATCHED_FILE pfPatched;
											PatchFile( &sPatch, m_vPatchRomFile, pPath, pfPatched );
										}
										catch ( ... ) {
											lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_OUT_OF_MEMORY ) );
										}
									}
								}
							}
							else if ( vSelected.size() ) {
								if ( !bWarnOfDoom || lsw::CBase::PromptYesNo( Wnd(), LSN_LSTR( LSN_PATCH_WARNING_INCOMPATIBLE_FILE_X ), LSN_LSTR( LSN_PATCH_WARNING_INCOMPATIBLE_FILE_TITLE ) ) ) {
									// Prompt for the save folder.
									std::wstring wsDisplayName;
									wsDisplayName.resize( 0xFFFF + 2 );

									auto wsDefaultDir = m_poOptions->wsPatchOutFolder;
									if ( !m_poOptions->wsPatchOutFolder.size() ) {
										wsDefaultDir = m_poOptions->wsInRomInitPath;
									}

									BROWSEINFOW biBrowseInfo = {};
									biBrowseInfo.hwndOwner = Wnd();
									biBrowseInfo.pidlRoot = nullptr;													// Root folder (nullptr = "My Computer")
									biBrowseInfo.pszDisplayName = wsDisplayName.data();									// Out buffer for display name
									biBrowseInfo.lpszTitle = LSN_LSTR( LSN_WE_BROWSE_OUTPUT_FOLDER );					// Title text in the dialog
									biBrowseInfo.ulFlags = BIF_RETURNONLYFSDIRS;
									biBrowseInfo.iImage = 0;
									biBrowseInfo.lpfn = CWinUtilities::BrowseCallbackProc;
									biBrowseInfo.lParam = reinterpret_cast<LPARAM>(wsDefaultDir.c_str());
									{
										CWinUtilities::LSN_OLEINITIALIZE oOle;
										if ( oOle.Success() ) {
											biBrowseInfo.ulFlags |= BIF_NEWDIALOGSTYLE;
										}

										PIDLIST_ABSOLUTE pidlSelected = ::SHBrowseForFolderW( &biBrowseInfo );
										if ( pidlSelected ) {
											std::wstring szFileName;
											szFileName.resize( 0xFFFF + 2 );

											if ( ::SHGetPathFromIDListW( pidlSelected, szFileName.data() ) ) {
												m_poOptions->wsPatchOutFolder = szFileName.c_str();
												std::filesystem::path pOutFolder( m_poOptions->wsPatchOutFolder );
												bool bMemoryError = false;
												for ( size_t I = 0; I < vSelected.size(); ++I ) {
													size_t sIdx = size_t( ptvTree->GetItemLParam( vSelected[I] ) );
													CUtilities::LSN_FILE_PATHS fpPath;
													CUtilities::DeconstructFilePath( m_vPatchInfo[sIdx].u16FullPath, fpPath );
													{
														try {
															auto pPath = CUtilities::GenerateNewFilePath( pOutFolder, DefaultPatchName( m_rRomInfo.riInfo.s16RomName, fpPath.u16sFile ) );

															CStream sPatch( m_vPatchInfo[sIdx].vLoadedPatchFile );
															LSN_PATCHED_FILE pfPatched;
															PatchFile( &sPatch, m_vPatchRomFile, pPath, pfPatched );
														}
														catch ( ... ) {
															bMemoryError = false;
														}
													}
												}
												
												if ( bMemoryError ) {
													lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_PATCH_OUT_OF_MEMORY_CREATING_PATCH ) );
												}
											}
											::CoTaskMemFree( pidlSelected );
										}
									}
								}
							}
						}
					}
					catch ( ... ) {
						lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_OUT_OF_MEMORY ) );
					}
				}
				break;
			}
			/*case CPatchWindowLayout::LSN_PWI_FILE_OUT_EDIT : {
				if ( _wCtrlCode == EN_CHANGE ) {
					m_bOutIsAutoFilled = false;
					return LSW_H_CONTINUE;
				}
				break;
			}*/

			case CPatchWindowLayout::LSN_PWI_REVEAL_COMPATIBLE : {
				RevealCompatible();
				break;
			}
			case CPatchWindowLayout::LSN_PWI_SELECT_COMPATIBLE : {
				RevealCompatible( true );
				break;
			}
			case CPatchWindowLayout::LSN_PWI_REVEAL_CRC : {
				RevealCrcPatches();
				break;
			}
			case CPatchWindowLayout::LSN_PWI_COPY_EXPAND_SELECTED : {
				lsw::CTreeListView * ptlTree = static_cast<lsw::CTreeListView *>(FindChild( CPatchWindowLayout::LSN_PWI_FILE_PATCH_TREELISTVIEW ));
				if ( ptlTree ) {
					ptlTree->ExpandSelected();
				}
				break;
			}
			case CPatchWindowLayout::LSN_PWI_COPY_EXPAND_ALL : {
				lsw::CTreeListView * ptlTree = static_cast<lsw::CTreeListView *>(FindChild( CPatchWindowLayout::LSN_PWI_FILE_PATCH_TREELISTVIEW ));
				if ( ptlTree ) {
					ptlTree->ExpandAll();
				}
				break;
			}
			case CPatchWindowLayout::LSN_PWI_COPY_COLLAPSE_SELECTED : {
				lsw::CTreeListView * ptlTree = static_cast<lsw::CTreeListView *>(FindChild( CPatchWindowLayout::LSN_PWI_FILE_PATCH_TREELISTVIEW ));
				if ( ptlTree ) {
					ptlTree->CollapseSelected();
				}
				break;
			}
			case CPatchWindowLayout::LSN_PWI_COPY_COLLAPSE_ALL : {
				lsw::CTreeListView * ptlTree = static_cast<lsw::CTreeListView *>(FindChild( CPatchWindowLayout::LSN_PWI_FILE_PATCH_TREELISTVIEW ));
				if ( ptlTree ) {
					ptlTree->CollapseAll();
				}
				break;
			}
		}
		return LSW_H_CONTINUE;
	}

	/**
	 * Handles WM_TIMER.
	 * \brief Notified when a timer elapses.
	 *
	 * \param _uiptrId Timer identifier.
	 * \param _tpProc Optional callback associated with the timer.
	 * \return Returns a LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CPatchWindowTopPage::Timer( UINT_PTR _uiptrId, TIMERPROC /*_tpProc*/ ) {
		switch ( _uiptrId ) {
			case CWinUtilities::LSN_UPDATE_PATCHER_DESC : {
				UpdateText();
				m_tUpdateBottomTimer.Stop();
				break;
			}
		}
		return LSW_H_HANDLED;
	}

	/**
	 * Handles the WM_GETMINMAXINFO message.
	 * 
	 * \param _pmmiInfo The min/max info structure to fill out.
	 * \return Returns an LSW_HANDLED code.
	 **/
	CWidget::LSW_HANDLED CPatchWindowTopPage::GetMinMaxInfo( MINMAXINFO * _pmmiInfo ) {
		//return LSW_H_CONTINUE;
		LONG lLeft = 100;
		if ( m_pwParent ) {
			auto aTmp = m_pwParent->FindChild( CPatchWindowLayout::LSN_PWI_FILE_IN_EDIT );
			if ( aTmp ) {
				lLeft = aTmp->WindowRect().ScreenToClient( Wnd() ).left;
			}

			aTmp = m_pwParent->FindChild( CPatchWindowLayout::LSN_PWI_FILE_IN_BUTTON );
			if ( aTmp ) {
				lLeft += aTmp->WindowRect().Width();
			}

			lLeft += 130;
		}
		_pmmiInfo->ptMinTrackSize.x = lLeft;
		//_pmmiInfo->ptMinTrackSize.y = _pmmiInfo->ptMaxTrackSize.y = m_rStartingRect.Height();
		_pmmiInfo->ptMinTrackSize.y = m_rStartingRect.Height();
		return LSW_H_HANDLED;
	}

	/**
	 * Handles the WM_CONTEXTMENU message.
	 * 
	 * \param _pwControl The control that was clicked.
	 * \param _iX The horizontal position of the cursor, in screen coordinates, at the time of the mouse click.
	 * \param _iY The vertical position of the cursor, in screen coordinates, at the time of the mouse click.
	 * \return Returns an LSW_HANDLED code.
	 **/
	CWidget::LSW_HANDLED CPatchWindowTopPage::ContextMenu( CWidget * _pwControl, INT _iX, INT _iY ) {
		if ( _pwControl->Id() == CPatchWindowLayout::LSN_PWI_FILE_PATCH_TREELISTVIEW ) {
			auto ptlvTree = static_cast<CTreeListView *>(_pwControl);
			bool bHasItems = ptlvTree->GetItemCount() ? true : false;
			bool bExpSel = ptlvTree->AnySelectedHasUnexpandedChildren();
			bool bExpAll = ptlvTree->AnyHasUnexpandedChildren();
			bool bColSel = ptlvTree->AnySelectedHasExpandedChildren();
			bool bColAll = ptlvTree->AnyHasExpandedChildren();
			LSW_MENU_ITEM miMenuBar[] = {
				//bIsSeperator	dwId														bCheckable	bChecked	bEnabled		lpwcText, stTextLen							bSkip
				{ FALSE,		CPatchWindowLayout::LSN_PWI_REVEAL_COMPATIBLE,				FALSE,		FALSE,		bHasItems,		LSN_LSTR( LSN_PATCH_REVEAL_COMPATIBLE ),	FALSE },
				{ FALSE,		CPatchWindowLayout::LSN_PWI_SELECT_COMPATIBLE,				FALSE,		FALSE,		bHasItems,		LSN_LSTR( LSN_PATCH_SELECT_COMPATIBLE ),	FALSE },
				{ FALSE,		CPatchWindowLayout::LSN_PWI_REVEAL_CRC,						FALSE,		FALSE,		bHasItems,		LSN_LSTR( LSN_PATCH_REVEAL_BPS ),			FALSE },
				{ TRUE,			0,															FALSE,		FALSE,		TRUE,			nullptr,									FALSE },
				{ FALSE,		CPatchWindowLayout::LSN_PWI_COPY_EXPAND_SELECTED,			FALSE,		FALSE,		bExpSel,		LSN_LSTR( LSN_PATCH_EXPAND_SELECTED ),		FALSE },
				{ FALSE,		CPatchWindowLayout::LSN_PWI_COPY_EXPAND_ALL,				FALSE,		FALSE,		bExpAll,		LSN_LSTR( LSN_PATCH_EXPAND_ALL ),			FALSE },
				{ FALSE,		CPatchWindowLayout::LSN_PWI_COPY_COLLAPSE_SELECTED,			FALSE,		FALSE,		bColSel,		LSN_LSTR( LSN_PATCH_COLLAPSE_SELECTED ),	FALSE },
				{ FALSE,		CPatchWindowLayout::LSN_PWI_COPY_COLLAPSE_ALL,				FALSE,		FALSE,		bColAll,		LSN_LSTR( LSN_PATCH_COLLAPSE_ALL ),			FALSE },
			};

			const LSW_MENU_LAYOUT miMenus[] = {
				{
					LSN_M_CONTEXT_MENU,
					0,
					0,
					std::size( miMenuBar ),
					miMenuBar
				},
			};
			lsn::CLayoutManager * plmLayout = static_cast<lsn::CLayoutManager *>(lsw::CBase::LayoutManager());
			Command( 0, static_cast<WORD>(plmLayout->CreatePopupMenuEx( this, miMenus, std::size( miMenus ), _iX, _iY, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD )), ptlvTree );
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
	CWidget::LSW_HANDLED CPatchWindowTopPage::Notify_ItemChanged( LPNMLISTVIEW /*_lplvParm*/ ) {
		m_tUpdateBottomTimer.Start( Wnd(), CWinUtilities::LSN_UPDATE_PATCHER_DESC, 16 );
		return LSW_H_CONTINUE;
	}

	/**
	 * The WM_NOTIFY -> LVN_ODSTATECHANGED handler.
	 *
	 * \param _lposcParm The notifacation structure.
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CPatchWindowTopPage::Notify_OdStateChange( LPNMLVODSTATECHANGE /*_lposcParm*/ ) {
		m_tUpdateBottomTimer.Start( Wnd(), CWinUtilities::LSN_UPDATE_PATCHER_DESC, 16 );
		return LSW_H_CONTINUE;
	}

	/**
	 * Updates the source ROM information labels.
	 **/
	void CPatchWindowTopPage::UpdateInfo() {
		LSN_ROM rRom;
		if ( m_vPatchRomFile.size() && CSystemBase::LoadRom( m_vPatchRomFile, rRom, m_u16RomPath ) ) {
			m_rRomInfo = rRom;

			auto pwPathEdit = FindChild( CPatchWindowLayout::LSN_PWI_INFO_CRC_LABEL );
			if ( pwPathEdit ) {
				m_ui32FullCrc = CCrc::GetCrc( m_vPatchRomFile.data(), m_vPatchRomFile.size() );
				try {
					pwPathEdit->SetTextW( std::format( L"Full CRC32:\t\t{:08X}", m_ui32FullCrc ).c_str() );
				} catch ( ... ) {}
			}
			pwPathEdit = FindChild( CPatchWindowLayout::LSN_PWI_INFO_ROM_CRC_LABEL );
			if ( pwPathEdit ) {
				try {
					pwPathEdit->SetTextW( std::format( L"ROM CRC32:\t\t{:08X}", rRom.riInfo.ui32Crc ).c_str() );
				} catch ( ... ) {}
			}
			pwPathEdit = FindChild( CPatchWindowLayout::LSN_PWI_INFO_HEADERLESS_CRC_LABEL );
			if ( pwPathEdit ) {
				try {
					pwPathEdit->SetTextW( std::format( L"Headerless CRC32:\t{:08X}", rRom.riInfo.ui32HeaderlessCrc ).c_str() );
				} catch ( ... ) {}
			}

			pwPathEdit = FindChild( CPatchWindowLayout::LSN_PWI_INFO_MD5_LABEL );
			if ( pwPathEdit ) {
				try {
					pwPathEdit->SetTextW( std::format( L"MD5:\t\t\t{}", CUtilities::ToUpper( CMd5::ToString<std::wstring>( m_rRomInfo.riInfo.mhMd5 ) ) ).c_str() );
				} catch ( ... ) {}
			}
			pwPathEdit = FindChild( CPatchWindowLayout::LSN_PWI_INFO_HEADERLESS_MD5_LABEL );
			if ( pwPathEdit ) {
				try {
					pwPathEdit->SetTextW( std::format( L"Headerless MD5:\t\t{}", CUtilities::ToUpper( CMd5::ToString<std::wstring>( m_rRomInfo.riInfo.mhHeaderlessMd5 ) ) ).c_str() );
				} catch ( ... ) {}
			}

			pwPathEdit = FindChild( CPatchWindowLayout::LSN_PWI_INFO_ROM_PGM_SIZE_LABEL );
			if ( pwPathEdit ) {
				try {
					pwPathEdit->SetTextW( std::format( L"ROM PGM Size:\t\t{0:08X} ({0} bytes, {1:g} kibibytes)", rRom.vPrgRom.size(), rRom.vPrgRom.size() / 1024.0 ).c_str() );
				} catch ( ... ) {}
			}
			pwPathEdit = FindChild( CPatchWindowLayout::LSN_PWI_INFO_ROM_CHR_SIZE_LABEL );
			if ( pwPathEdit ) {
				try {
					pwPathEdit->SetTextW( std::format( L"ROM CHR Size:\t\t{0:08X} ({0} bytes, {1:g} kibibytes)", rRom.vChrRom.size(), rRom.vChrRom.size() / 1024.0 ).c_str() );
				} catch ( ... ) {}
			}
			pwPathEdit = FindChild( CPatchWindowLayout::LSN_PWI_INFO_ROM_MIRROR_LABEL );
			if ( pwPathEdit ) {
				switch ( rRom.riInfo.mmMirroring ) {
					case LSN_MM_VERTICAL : {
						pwPathEdit->SetTextW( std::format( L"Mirroring:\t\tVertical", rRom.vPrgRom.size(), rRom.vPrgRom.size() / 1024.0 ).c_str() );
						break;
					}
					case LSN_MM_HORIZONTAL : {
						pwPathEdit->SetTextW( std::format( L"Mirroring:\t\tHorizontal", rRom.vPrgRom.size(), rRom.vPrgRom.size() / 1024.0 ).c_str() );
						break;
					}
					case LSN_MM_4_SCREENS : {
						pwPathEdit->SetTextW( std::format( L"Mirroring:\t4 Screens", rRom.vPrgRom.size(), rRom.vPrgRom.size() / 1024.0 ).c_str() );
						break;
					}
					case LSN_MM_1_SCREEN_A : {
						pwPathEdit->SetTextW( std::format( L"Mirroring:\tScreen A", rRom.vPrgRom.size(), rRom.vPrgRom.size() / 1024.0 ).c_str() );
						break;
					}
					case LSN_MM_1_SCREEN_B : {
						pwPathEdit->SetTextW( std::format( L"Mirroring:\tScreen B", rRom.vPrgRom.size(), rRom.vPrgRom.size() / 1024.0 ).c_str() );
						break;
					}
				}
			}
		}
	}

	/**
	 * Updates the colors based on the currently loaded ROM file.
	 **/
	void CPatchWindowTopPage::UpdateColors() {
		auto ptvTree = static_cast<lsw::CTreeListView *>(FindChild( CPatchWindowLayout::LSN_PWI_FILE_PATCH_TREELISTVIEW ));
		if ( !ptvTree ) { return; }
		auto hThis = ptvTree->GetNext( TVI_ROOT );
		while ( hThis ) {
			LPARAM lpParm = ptvTree->GetItemLParam( hThis );
			RGBQUAD rgbqColor = { .rgbBlue = 0x00, .rgbGreen = 0x00, .rgbRed = 0x00, .rgbReserved = 0x00 };

			if ( size_t( lpParm ) < m_vPatchInfo.size() ) {
				if ( !m_vPatchInfo[lpParm].bIsText ) {
					if ( m_vPatchInfo[lpParm].ui32Crc ) {																						// BPS file.
						if ( m_vPatchRomFile.size() ) {
							if ( CrcMatch( m_vPatchInfo[lpParm].ui32Crc ) ) {
								rgbqColor = { .rgbBlue = 64, .rgbGreen = 255, .rgbRed = 159, .rgbReserved = 0xCF };							// Found a match.
							}
							else {
								rgbqColor = { .rgbBlue = 128, .rgbGreen = 128, .rgbRed = 255, .rgbReserved = 0xCF };							// Not a match.
							}
						}
						else {
							rgbqColor = { .rgbBlue = 170, .rgbGreen = 198, .rgbRed = 255, .rgbReserved = 0xCF };								// No ROM file loaded.
						}
					}
					else {																														// IPS file.
						if ( SiblingHasTextReferenceToCrc( ptvTree, hThis ) ) {
							rgbqColor = { .rgbBlue = 170, .rgbGreen = 255, .rgbRed = 213, .rgbReserved = 0xCF };								// A sibling text file contains a reference to the desired CRC.
						}
						else {
							rgbqColor = { .rgbBlue = 170, .rgbGreen = 198, .rgbRed = 255, .rgbReserved = 0xCF };								// No found relationship with the current ROM.
						}
					}
				}
				else {												// TXT/MD.
					rgbqColor = { .rgbBlue = 0xE0, .rgbGreen = 0xE0, .rgbRed = 0xE0, .rgbReserved = 0x3F };
				}
			}
			ptvTree->SetItemColor( hThis, rgbqColor );
			hThis = ptvTree->GetNext( hThis );
		}
	}

	/**
	 * Updates the text description on the bottom based on the currently selected items.
	 **/
	void CPatchWindowTopPage::UpdateText() {
		if ( m_pwParent ) {
			auto pwText = m_pwParent->FindChild( CPatchWindowLayout::LSN_PWI_BOTTOM_TAB_DESC_EDIT );
			if ( pwText ) {
				auto ptvTree = static_cast<lsw::CTreeListView *>(FindChild( CPatchWindowLayout::LSN_PWI_FILE_PATCH_TREELISTVIEW ));
				if ( !ptvTree ) {
					pwText->SetTextW( L"" );

					auto pwQuick = FindChild( CPatchWindowLayout::LSN_PWI_FILE_QUICK_PATCH_BUTTON );
					if ( pwQuick ) {
						pwQuick->SetEnabled( false );
					}
				}
				else {
					std::vector<HTREEITEM> vSelected;
					ptvTree->GatherSelected( vSelected, true );
					if ( vSelected.size() == 0 ) {
						pwText->SetTextW( L"" );

						auto pwQuick = FindChild( CPatchWindowLayout::LSN_PWI_FILE_QUICK_PATCH_BUTTON );
						if ( pwQuick ) {
							pwQuick->SetEnabled( false );
						}
					}
					else {
						std::wstring wsText;
						for ( size_t I = 0; I < vSelected.size(); ++I ) {
							size_t sIdx = size_t( ptvTree->GetItemLParam( vSelected[I] ) );
							if ( sIdx < m_vPatchInfo.size() ) {
								try {
									if ( wsText.size() ) {
										wsText += L"\r\n\r\n";
									}
									auto u16Tmp = m_vPatchInfo[sIdx].u16FullPath;
									u16Tmp = CUtilities::Replace( u16Tmp, std::u16string( u"{" ), std::u16string( u"\\" ) );
									u16Tmp = CUtilities::Replace( u16Tmp, std::u16string( u"}" ), std::u16string( u"" ) );
									u16Tmp = CUtilities::Replace( u16Tmp, u'/', u'\\' );
									CUtilities::LSN_FILE_PATHS fpPath;
									CUtilities::DeconstructFilePath( u16Tmp.c_str(), fpPath );

									wsText += CUtilities::XStringToWString( fpPath.u16sFile.c_str(), fpPath.u16sFile.size() );
									if ( m_vPatchInfo[sIdx].bIsText ) {
										wsText += L"\r\n";
										auto wsTmp = CUtilities::Replace( ptvTree->GetItemText( vSelected[I], 1 ), std::wstring( L"\\r" ), std::wstring() );
										wsTmp = CUtilities::Replace( wsTmp, std::wstring( L"\\n" ), std::wstring( L"\r\n" ) );
										wsText += wsTmp;
									}
									else {
										if ( !m_vPatchRomFile.size() ) {
										}
										else {
											wsText += L": ";
											if ( m_vPatchInfo[sIdx].ui32Crc ) {
												if ( CrcMatch( m_vPatchInfo[sIdx].ui32Crc ) ) {
													wsText += LSN_LSTR( LSN_PATCH_PATCH_IS_COMPATIBLE );
												}
												else {
													wsText += LSN_LSTR( LSN_PATCH_NOT_COMPATIBLE );
												}
											}
											else {
												if ( SiblingHasTextReferenceToCrc( ptvTree, vSelected[I] ) ) {
													wsText += LSN_LSTR( LSN_PATCH_LIKELY_COMPATIBLE );
												}
												else {
													wsText += LSN_LSTR( LSN_PATCH_COMPAT_UNKNOWN );
												}
											}
										}
									}
								}
								catch ( ... ) {}
							}
						}
						pwText->SetTextW( wsText.c_str() );



						bool bWarnOfDoom = false;
						TrimVector( ptvTree, vSelected, bWarnOfDoom );
						auto pwQuick = FindChild( CPatchWindowLayout::LSN_PWI_FILE_QUICK_PATCH_BUTTON );
						if ( pwQuick ) {
							pwQuick->SetEnabled( vSelected.size() != 0 );
						}
					}
				}
			}
		}
	}

	/**
	 * Reveals compatiable ROM hacks.  Finds BPS files with CRC’s matching the loaded ROM file and expands those while collapsing all others.
	 * 
	 * \param _bSelect If true, the items are also selected.
	 **/
	void CPatchWindowTopPage::RevealCompatible( bool _bSelect ) {
		lsw::CTreeListView * ptvTree = static_cast<lsw::CTreeListView *>(FindChild( CPatchWindowLayout::LSN_PWI_FILE_PATCH_TREELISTVIEW ));
		if ( ptvTree ) {
			ptvTree->BeginLargeUpdate();
			auto hThis = ptvTree->GetNext( TVI_ROOT );
			while ( hThis ) {
				bool bCompat = IsCompatible( ptvTree, hThis );
				ptvTree->SetItemExpand( hThis, bCompat );
				if ( _bSelect ) {
					ptvTree->SetItemSelection( hThis, bCompat && ptvTree->CountChildren( hThis ) == 0 );
				}
				hThis = ptvTree->GetNext( hThis );
			}
			ptvTree->FinishUpdate();
		}
	}

	/**
	 * Checks the given tree item for having a child that is a BPS file compatible with the current ROM.
	 * 
	 * \param _ptlvTree A pointer to the CTreeListView.
	 * \param _htiItem The item to recursively check for being compatible.
	 * \return Returns true if the item or any of its children are compatible with the current ROM file.
	 **/
	bool CPatchWindowTopPage::IsCompatible( lsw::CTreeListView * _ptlvTree, HTREEITEM _htiItem ) const {
		if ( !m_vPatchRomFile.size() ) { return false; }				// Nothing is loaded.
		size_t sIdx = size_t( _ptlvTree->GetItemLParam( _htiItem ) );
		if ( sIdx < m_vPatchInfo.size() ) {
			if ( !m_vPatchInfo[sIdx].bIsText ) {
				if ( m_vPatchInfo[sIdx].ui32Crc ) {
					if ( CrcMatch( m_vPatchInfo[sIdx].ui32Crc ) ) {
						return true;
					}
				}
				else {
					if ( SiblingHasTextReferenceToCrc( _ptlvTree, _htiItem ) ) { return true; }
				}
			}
		}
		std::vector<HTREEITEM> vChildren;
		auto sSize = _ptlvTree->GatherChildren( vChildren, _htiItem );
		if ( sSize != vChildren.size() ) { return false; }	// Memory failure.
		for ( auto & htiChild : vChildren ) {
			if ( IsCompatible( _ptlvTree, htiChild ) ) { return true; }
		}
		return false;
	}

	/**
	 * Given a tree item, its siblings are checked for being a text file, and then the text is searched for the current ROM file’s CRC’s.
	 * 
	 * \param _ptlvTree A pointer to the CTreeListView.
	 * \param _htiItem The item whose following siblings are to be checked for being text, and then the text searched for the current ROM’s CRC pair.
	 * \return Returns true if a text file is a sibling of the given item and that text file contains either of the CRC’s for the current ROM.
	 **/
	bool CPatchWindowTopPage::SiblingHasTextReferenceToCrc( lsw::CTreeListView * _ptlvTree, HTREEITEM _htiItem ) const {
		if ( !m_vPatchRomFile.size() ) { return false; }				// Nothing is loaded.

		HTREEITEM htiNext = _ptlvTree->GetNext( _htiItem ), htiParent = _ptlvTree->GetItemParent( _htiItem );
		std::wstring wsFindMeLowerFull = std::format( L"{:08x}", m_ui32FullCrc );
		std::wstring wsFindMeLower = std::format( L"{:08x}", m_rRomInfo.riInfo.ui32Crc );
		std::wstring wsFindMeHeaderlessLower = std::format( L"{:08x}", m_rRomInfo.riInfo.ui32HeaderlessCrc );
		std::wstring wsMd5 = CMd5::ToString<std::wstring>( m_rRomInfo.riInfo.mhMd5 );
		std::wstring wsMd5Headerless = CMd5::ToString<std::wstring>( m_rRomInfo.riInfo.mhHeaderlessMd5 );
		while ( htiNext && _ptlvTree->GetItemParent( htiNext ) == htiParent ) {
			size_t sIdx = size_t( _ptlvTree->GetItemLParam( htiNext ) );
			if ( sIdx < m_vPatchInfo.size() ) {
				if ( m_vPatchInfo[sIdx].bIsText ) {
					try {
						std::wstring wsLower = CUtilities::ToLower( _ptlvTree->GetItemText( htiNext, 1 ) );
						if ( std::wstring::npos != wsLower.find( wsFindMeLowerFull ) || std::wstring::npos != wsLower.find( wsFindMeLower ) || std::wstring::npos != wsLower.find( wsFindMeHeaderlessLower ) ||
							std::wstring::npos != wsLower.find( wsMd5 ) || std::wstring::npos != wsLower.find( wsMd5Headerless ) ) { return true; }
					}
					catch ( ... ) {}
				}
			}

			htiNext = _ptlvTree->GetNext( htiNext );
		}
		return false;
	}

	/**
	 * Checks for a child node that is a checksum patch.
	 * 
	 * \param _ptlvTree A pointer to the TreeListView.
	 * \param _htiItem The item to recursively check for being a checksum patch.
	 * \return Returns true if the item or any of its children are is a BPS or other checksum-based patch file.
	 **/
	bool CPatchWindowTopPage::HasCheckSumPatch( lsw::CTreeListView * _ptlvTree, HTREEITEM _htiItem ) const {
		if ( !m_vPatchRomFile.size() ) { return false; }				// Nothing is loaded.
		size_t sIdx = size_t( _ptlvTree->GetItemLParam( _htiItem ) );
		if ( sIdx < m_vPatchInfo.size() ) {
			if ( !m_vPatchInfo[sIdx].bIsText && m_vPatchInfo[sIdx].ui32Crc ) {
				return true;
			}
		}
		std::vector<HTREEITEM> vChildren;
		auto sSize = _ptlvTree->GatherChildren( vChildren, _htiItem );
		if ( sSize != vChildren.size() ) { return false; }						// Memory failure.
		for ( auto & htiChild : vChildren ) {
			if ( HasCheckSumPatch( _ptlvTree, htiChild ) ) { return true; }
		}
		return false;
	}

	/**
	 * Reveals all patch files with CRC checks.
	 **/
	void CPatchWindowTopPage::RevealCrcPatches() {
		lsw::CTreeListView * ptvTree = static_cast<lsw::CTreeListView *>(FindChild( CPatchWindowLayout::LSN_PWI_FILE_PATCH_TREELISTVIEW ));
		if ( ptvTree ) {
			ptvTree->BeginLargeUpdate();
			auto hThis = ptvTree->GetNext( TVI_ROOT );
			while ( hThis ) {
				ptvTree->SetItemExpand( hThis, HasCheckSumPatch( ptvTree, hThis ) );
				hThis = ptvTree->GetNext( hThis );
			}
			ptvTree->FinishUpdate();
		}
	}

	/**
	 * Adds all the nodes in _vNodes as children of _hParent.
	 * 
	 * \param _vNodes The nodes to add under _hParent.
	 * \param _hParent The parent under which to add _vNodes.
	 * \param _ptlTree The CTreeListView control.
	 **/
	void CPatchWindowTopPage::AddToTree( const std::vector<LSN_PATCH_INFO_TREE_ITEM> &_vNodes, HTREEITEM _hParent, lsw::CTreeListView * _ptlTree ) {
		for ( size_t I = 0; I < _vNodes.size(); ++I ) {
			auto u16Tmp = CUtilities::Utf8ToUtf16( _vNodes[I].u8Name.c_str() );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( reinterpret_cast<const WCHAR *>(u16Tmp.c_str()),
				_vNodes[I].sIdx, _hParent );
			HTREEITEM hItem = _ptlTree->InsertItem( &isInsertMe );
			if ( _vNodes[I].sIdx != ~size_t( 0 ) ) {
				if ( m_vPatchInfo[_vNodes[I].sIdx].ui32Crc ) {
					_ptlTree->SetItemText( hItem, std::format( L"Source ROM CRC32: {:08X}", m_vPatchInfo[_vNodes[I].sIdx].ui32Crc ).c_str(), 1 );
				}
				else if ( m_vPatchInfo[_vNodes[I].sIdx].bIsText && m_vPatchInfo[_vNodes[I].sIdx].vLoadedPatchFile.size() != 0 ) {
					auto aReplaced = CUtilities::Utf8ToUtf16( reinterpret_cast<const char8_t *>(m_vPatchInfo[_vNodes[I].sIdx].vLoadedPatchFile.data()) );
					aReplaced = CUtilities::Replace( aReplaced, std::u16string( u"\r" ), std::u16string( u"\\r" ) );
					aReplaced = CUtilities::Replace( aReplaced, std::u16string( u"\n" ), std::u16string( u"\\n" ) );
					_ptlTree->SetItemText( hItem,
						reinterpret_cast<const WCHAR *>(aReplaced.c_str()), 1 );
				}
			}
			AddToTree( _vNodes[I].vChildren, hItem, _ptlTree );
		}
	}

	/**
	 * Applies a patch to a given stream.
	 * 
	 * \param _psbPatch The patch stream.
	 * \param _vRom The ROM data.
	 * \param _pOutPath The output path.
	 * \param _pfPatchedInfo Information for the patched file.
	 * \throw Throws upon error.
	 **/
	void CPatchWindowTopPage::PatchFile( const CStreamBase * _psbPatch, std::vector<uint8_t> &_vRom, const std::filesystem::path &_pOutPath, LSN_PATCHED_FILE &_pfPatchedInfo ) {
		std::vector<uint8_t> vRomCopy;
		do {
			{
				// Check for being an IPS file.
				_psbPatch->MovePointerTo( 0 );
				CIpsFile ifIps( _psbPatch );
				if ( ifIps.VerifyHeader() ) {
					try {
						vRomCopy = _vRom;
						CStream sRomStream( vRomCopy );
						if ( !ifIps.ApplyPatch( &sRomStream ) ) {
							lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_INVALID_FILE_FORMAT ) );
							return;
						}

						CStdFile sfFile;
						if ( !sfFile.Create( _pOutPath ) ) {
							auto wsError = std::format( LSN_LSTR( LSN_PATCH_UNABLE_TO_CREATE_FILE ), _pOutPath.generic_wstring() );
							lsw::CBase::MessageBoxError( Wnd(), wsError.c_str() );
							return;
						}
						if ( !sfFile.WriteToFile( vRomCopy ) ) {
							auto wsError = std::format( LSN_LSTR( LSN_PATCH_UNABLE_TO_WRITE_FILE ), _pOutPath.generic_wstring() );
							lsw::CBase::MessageBoxError( Wnd(), wsError.c_str() );
							return;
						}
					
					}
					catch ( const std::exception &_e ) {
						std::string sError = _e.what();
						std::wstring wsError( sError.begin(), sError.end() );
					
						lsw::CBase::MessageBoxError( Wnd(), wsError.c_str() );
						return;
					}
					break;
				}
			}
			{
				// Check for being a BPS file.
				_psbPatch->MovePointerTo( 0 );
				CBpsFile bfBps( _psbPatch );
				if ( bfBps.VerifyHeader() ) {
					try {
						CStream sSrcStream( _vRom );
						CStream sRomStream( vRomCopy );
						if ( !bfBps.ApplyPatch( &sSrcStream, &sRomStream ) ) {
							lsw::CBase::MessageBoxError( Wnd(), LSN_LSTR( LSN_INVALID_FILE_FORMAT ) );
							return;
						}

						CStdFile sfFile;
						if ( !sfFile.Create( _pOutPath ) ) {
							auto wsError = std::format( LSN_LSTR( LSN_PATCH_UNABLE_TO_CREATE_FILE ), _pOutPath.generic_wstring() );
							lsw::CBase::MessageBoxError( Wnd(), wsError.c_str() );
							return;
						}
						if ( !sfFile.WriteToFile( vRomCopy ) ) {
							auto wsError = std::format( LSN_LSTR( LSN_PATCH_UNABLE_TO_WRITE_FILE ), _pOutPath.generic_wstring() );
							lsw::CBase::MessageBoxError( Wnd(), wsError.c_str() );
							return;
						}
					
					}
					catch ( const std::exception &_e ) {
						std::string sError = _e.what();
						std::wstring wsError( sError.begin(), sError.end() );
					
						lsw::CBase::MessageBoxError( Wnd(), wsError.c_str() );
						return;
					}
					break;
				}
			}
		} while ( false );

		_pfPatchedInfo.ui32Crc = CCrc::GetCrc( vRomCopy.data(), vRomCopy.size() );
		_pfPatchedInfo.u16Path = _pOutPath.generic_u16string();
	}

	/**
	 * Trims non-patch entries from the given vector.
	 * 
	 * \param _ptlTree The CTreeListView control.
	 * \param _vTrimMe The vector whose elements are to be removed should they not refer to valid patch files.
	 * \param _bWarnOfDoom If true upon return, at least one of the remaining items was not able to be verified as compatible with the current ROM.
	 * \return Returns a reference to _vTrimMe.
	 **/
	std::vector<HTREEITEM> & CPatchWindowTopPage::TrimVector( lsw::CTreeListView * _ptlTree, std::vector<HTREEITEM> &_vTrimMe, bool &_bWarnOfDoom ) {
		_bWarnOfDoom = false;
		for ( auto I = _vTrimMe.size(); I--; ) {
			if ( _ptlTree->CountChildren( _vTrimMe[I] ) ) {
				// Patch files are "leaf" nodes; no children.
				_vTrimMe.erase( _vTrimMe.begin() + I );
			}
			else {
				size_t sIdx = size_t( _ptlTree->GetItemLParam( _vTrimMe[I] ) );
				if ( sIdx >= m_vPatchInfo.size() ) {
					_vTrimMe.erase( _vTrimMe.begin() + I );
				}
				else {
					if ( m_vPatchInfo[sIdx].bIsText ) {
						_vTrimMe.erase( _vTrimMe.begin() + I );
					}
					else {
						// Is a patch file.  Check for being a CRC patch and consider warning the user.
						if ( m_vPatchInfo[sIdx].ui32Crc ) {										// BPS.
							if ( !CrcMatch( m_vPatchInfo[sIdx].ui32Crc ) ) {
								_bWarnOfDoom = true;
							}
						}
						else if ( !SiblingHasTextReferenceToCrc( _ptlTree, _vTrimMe[I] ) ) {	// IPS.
							_bWarnOfDoom = true;
						}
					}
				}
			}
		}
		return _vTrimMe;
	}

	/**
	 * Creates a non-optimized basic tree given patch information.
	 * 
	 * \param _vInfo The information from which to generate a basic tree structure.
	 * \return Returns the root nodes of the tree.
	 **/
	std::vector<CPatchWindowTopPage::LSN_PATCH_INFO_TREE_ITEM> CPatchWindowTopPage::CreateBasicTree( const std::vector<LSN_PATCH_INFO> &_vInfo ) {
		std::vector<LSN_PATCH_INFO_TREE_ITEM> vReturn;
		for ( size_t I = 0; I < _vInfo.size(); ++I ) {
			std::vector<LSN_PATCH_INFO_TREE_ITEM> * pvNodes = &vReturn;
			// For each parent.
			LSN_PATCH_INFO_TREE_ITEM * pnitiItem = nullptr;
			for ( size_t P = 0; P < _vInfo[I].vTokenizedFolders.size(); ++P ) {
				pnitiItem = FindNode( (*pvNodes), _vInfo[I].vTokenizedFolders[P] );
				if ( !pnitiItem ) {
					(*pvNodes).push_back( LSN_PATCH_INFO_TREE_ITEM() );
					pnitiItem = &(*pvNodes)[(*pvNodes).size()-1];
					pnitiItem->u8Name = _vInfo[I].vTokenizedFolders[P];
				}
				pvNodes = &pnitiItem->vChildren;
			}
			if ( pnitiItem ) {
				//pnitiItem->ppiInfo = &_vInfo[I];
				pnitiItem->sIdx = I;
			}
		}
		return vReturn;
	}

	/**
	 * Finds a node with the given name.  Returns a pointer to the node or nullptr.
	 * 
	 * \param _vNodes the nodes to search.
	 * \param _u8Name The name of the node to find.
	 * \return Returns the node if found or nullptr otherwise.
	 **/
	CPatchWindowTopPage::LSN_PATCH_INFO_TREE_ITEM * CPatchWindowTopPage::FindNode( std::vector<LSN_PATCH_INFO_TREE_ITEM> &_vNodes, const std::u8string &_u8Name ) {
		for ( size_t I = 0; I < _vNodes.size(); ++I ) {
			if ( _vNodes[I].u8Name == _u8Name ) { return &_vNodes[I]; }
		}
		return nullptr;
	}

	/**
	 * Simplifies the tree by joining a parent with its child if it has only 1 child.
	 * 
	 * \param _vTree The tree to simplify.
	 **/
	void CPatchWindowTopPage::SimplifyTree( std::vector<LSN_PATCH_INFO_TREE_ITEM> &_vTree ) {
		for ( size_t I = 0; I < _vTree.size(); ) {
			// If this node has only 1 child, merge it into this node.
			if ( _vTree[I].vChildren.size() == 1 ) {
				_vTree[I].u8Name += u8'\\';
				_vTree[I].u8Name += _vTree[I].vChildren[0].u8Name;
				//_vTree[I].ppiInfo = _vTree[I].vChildren[0].ppiInfo;
				_vTree[I].sIdx = _vTree[I].vChildren[0].sIdx;
				auto aTmp = std::move( _vTree[I].vChildren[0].vChildren );
				_vTree[I].vChildren = std::move( aTmp );
				I = 0;
				continue;
			}

			++I;
		}
		// Same thing for each child.
		for ( size_t I = 0; I < _vTree.size(); ++I ) {
			SimplifyTree( _vTree[I].vChildren );
		}
	}

	/**
	 * Constructs a file name using the current ROM name and the name of a patch.
	 * 
	 * \param _u16RomFileName The name of the ROM.
	 * \param _u16PatchFileName The name of the patch.
	 * \return Returns a new file name constructed from the name of the ROM and the name of the patch.
	 **/
	std::filesystem::path CPatchWindowTopPage::DefaultPatchName( const std::u16string &_u16RomFileName, const std::u16string &_u16PatchFileName ) {
		std::filesystem::path pTmp = std::filesystem::path( _u16RomFileName ).filename().replace_extension( u"" );
		pTmp += u"_";
		pTmp += std::filesystem::path( _u16PatchFileName ).filename().replace_extension( std::filesystem::path( _u16RomFileName ).extension() );
		return pTmp;
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

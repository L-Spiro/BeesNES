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
#include "../../System/LSNSystemBase.h"
#include "../../Windows/Layout/LSNLayoutManager.h"
#include "../WinUtilities/LSNWinUtilities.h"

#include <commdlg.h>
#include <filesystem>
#include <set>


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
		}

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
				ofnOpenFile.lpstrInitialDir = m_poOptions->wInRomInitPath.c_str();
	
				std::u16string u16Path, u16FileName;
				if ( ::GetOpenFileNameW( &ofnOpenFile ) ) {
					m_poOptions->wInRomInitPath = std::filesystem::path( ofnOpenFile.lpstrFile ).remove_filename();

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
				ofnOpenFile.lpstrInitialDir = m_poOptions->wPatchInitPath.c_str();
				
				std::u16string u16Path;
				if ( ::GetOpenFileNameW( &ofnOpenFile ) ) {
					m_poOptions->wPatchInitPath = std::filesystem::path( ofnOpenFile.lpstrFile ).remove_filename();
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

							::OutputDebugStringW( reinterpret_cast<const wchar_t *>((m_vPatchInfo[I].u16FullPath + u"\r\n").c_str()) );
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
						ptvTree->FinishUpdate();
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
				ofnOpenFile.lpstrInitialDir = m_poOptions->wOutRomInitPath.c_str();
	
				std::u16string u16Path, u16FileName;
				if ( ::GetSaveFileNameW( &ofnOpenFile ) ) {
					m_poOptions->wOutRomInitPath = std::filesystem::path( ofnOpenFile.lpstrFile ).remove_filename();

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
					auto wsText = _pwSrc->GetTextW();
					CUtilities::LSN_FILE_PATHS fpPath;
					CUtilities::DeconstructFilePath( reinterpret_cast<const char16_t *>(wsText.c_str()), fpPath );
					if ( CUtilities::LastChar( fpPath.u16sFullPath ) == u'}' ) {
						lsn::CZipFile zfFile;
						auto aZipPath = fpPath.u16sPath;
						aZipPath.pop_back();
						if ( zfFile.Open( aZipPath.c_str() ) && zfFile.IsArchive() ) {
							m_vPatchRomFile.clear();
							if ( zfFile.ExtractToMemory( fpPath.u16sFile, m_vPatchRomFile ) ) {
							}
						}
					}
					else {
						lsn::CStdFile sfFile;
						if ( sfFile.Open( fpPath.u16sFullPath.c_str() ) ) {
							m_vPatchRomFile.clear();
							if ( sfFile.LoadToMemory( m_vPatchRomFile ) ) {
							}
						}
					}
					m_u16RomPath = fpPath.u16sPath + fpPath.u16sFile;

					auto pwPathEdit = FindChild( CPatchWindowLayout::LSN_PWI_FILE_OUT_EDIT );
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
							m_poOptions->wOutRomInitPath = aFinal.remove_filename();
						}
					}
					UpdateInfo();
					return LSW_H_CONTINUE;
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
			bool bExpSel = ptlvTree->AnySelectedHasUnexpandedChildren();
			bool bExpAll = ptlvTree->AnyHasUnexpandedChildren();
			bool bColSel = ptlvTree->AnySelectedHasExpandedChildren();
			bool bColAll = ptlvTree->AnyHasExpandedChildren();
			LSW_MENU_ITEM miMenuBar[] = {
				//bIsSeperator	dwId													bCheckable	bChecked	bEnabled	lpwcText, stTextLen												bSkip
				/*{ FALSE,		CPatchWindowLayout::LSN_PWI_COPY_FIELD,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_05239A67_Copy__Field, _LEN_05239A67 ),			FALSE },
				{ FALSE,		CPatchWindowLayout::LSN_PWI_COPY_VALUE,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_43A1870B_Copy__Value, _LEN_43A1870B ),			FALSE },
				{ FALSE,		CPatchWindowLayout::LSN_PWI_COPY_OFFSET,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_EF323132_Copy__Offset, _LEN_EF323132 ),			FALSE },
				{ FALSE,		CPatchWindowLayout::LSN_PWI_COPY_BYTES,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_C928AB0C_Copy__Raw_Bytes, _LEN_C928AB0C ),		FALSE },
				{ FALSE,		CPatchWindowLayout::LSN_PWI_COPY_DESC,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_03939D2C_Copy__Desc, _LEN_03939D2C ),			FALSE },
				{ FALSE,		CPatchWindowLayout::LSN_PWI_COPY_ALL,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_9B7D368F_Copy_A_ll, _LEN_9B7D368F ),			FALSE },
				{ FALSE,		CPatchWindowLayout::LSN_PWI_COPY_FIELD_VALUE,				FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_E9B5F487_Copy_F_ield__Value, _LEN_E9B5F487 ),	FALSE },
				{ TRUE,			0,															FALSE,		FALSE,		TRUE,		nullptr,									FALSE },*/
				{ FALSE,		CPatchWindowLayout::LSN_PWI_COPY_EXPAND_SELECTED,			FALSE,		FALSE,		bExpSel,	LSN_LSTR( LSN_PATCH_EXPAND_SELECTED ),		FALSE },
				{ FALSE,		CPatchWindowLayout::LSN_PWI_COPY_EXPAND_ALL,				FALSE,		FALSE,		bExpAll,	LSN_LSTR( LSN_PATCH_EXPAND_ALL ),			FALSE },
				{ FALSE,		CPatchWindowLayout::LSN_PWI_COPY_COLLAPSE_SELECTED,			FALSE,		FALSE,		bColSel,	LSN_LSTR( LSN_PATCH_COLLAPSE_SELECTED ),	FALSE },
				{ FALSE,		CPatchWindowLayout::LSN_PWI_COPY_COLLAPSE_ALL,				FALSE,		FALSE,		bColAll,	LSN_LSTR( LSN_PATCH_COLLAPSE_ALL ),			FALSE },
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
	CWidget::LSW_HANDLED CPatchWindowTopPage::Notify_ItemChanged( LPNMLISTVIEW _lplvParm ) {
		::OutputDebugStringW( std::format( L"CPatchWindowTopPage::Notify_ItemChanged:\t{} {} {}\r\n", _lplvParm->iItem, _lplvParm->uOldState, _lplvParm->uNewState ).c_str() );
		return LSW_H_CONTINUE;
	}

	/**
	 * The WM_NOTIFY -> LVN_ODSTATECHANGED handler.
	 *
	 * \param _lposcParm The notifacation structure.
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CPatchWindowTopPage::Notify_OdStateChange( LPNMLVODSTATECHANGE _lposcParm ) {
		::OutputDebugStringW( std::format( L"CPatchWindowTopPage::Notify_OdStateChange:\t{} {}\r\n", _lposcParm->iFrom, _lposcParm->iTo ).c_str() );
		return LSW_H_CONTINUE;
	}

	/**
	 * Gets the window rectangle for the given widget or gets this control's window rectangle if _pwChild is nullptr.
	 * 
	 * \param _pwChild The widget whose window rectangle is to be gotten.
	 * \return Returns the child's window rectangle or this client rectangle.
	 **/
	//LSW_RECT CPatchWindowTopPage::WindowRect( const CWidget * _pwChild ) const {
	//	LSW_RECT rWindow;
	//	::GetWindowRect( Wnd(), &rWindow );
	//	//LSW_RECT rRet = m_pwParent ? m_pwParent->ClientRect( _pwChild ? _pwChild : this ) : ClientRect( _pwChild );
	//	LSW_RECT rRet;
	//	if ( (!_pwChild || _pwChild == this) && m_pwParent ) { rRet = m_pwParent->ClientRect( this ); }
	//	else { rRet = ClientRect( _pwChild ); }
	//	rRet.MoveBy( rWindow.left, rWindow.top );
	//	return rRet;
	//}

	/**
	 * Updates the source ROM information labels.
	 **/
	void CPatchWindowTopPage::UpdateInfo() {
		LSN_ROM rRom;
		if ( m_vPatchRomFile.size() && CSystemBase::LoadRom( m_vPatchRomFile, rRom, m_u16RomPath ) ) {
			
			auto pwPathEdit = FindChild( CPatchWindowLayout::LSN_PWI_INFO_CRC_LABEL );
			if ( pwPathEdit ) {
				auto aFullCrc = CCrc::GetCrc( m_vPatchRomFile.data(), m_vPatchRomFile.size() );
				pwPathEdit->SetTextW( std::format( L"Full CRC32:\t{:08X}", aFullCrc ).c_str() );
			}
			pwPathEdit = FindChild( CPatchWindowLayout::LSN_PWI_INFO_ROM_CRC_LABEL );
			if ( pwPathEdit ) {
				pwPathEdit->SetTextW( std::format( L"ROM CRC32:\t{:08X}", rRom.riInfo.ui32Crc ).c_str() );
			}

			pwPathEdit = FindChild( CPatchWindowLayout::LSN_PWI_INFO_ROM_PGM_SIZE_LABEL );
			if ( pwPathEdit ) {
				pwPathEdit->SetTextW( std::format( L"ROM PGM Size:\t{0:08X} ({0} bytes, {1:g} kibibytes)", rRom.vPrgRom.size(), rRom.vPrgRom.size() / 1024.0 ).c_str() );
			}
			pwPathEdit = FindChild( CPatchWindowLayout::LSN_PWI_INFO_ROM_CHR_SIZE_LABEL );
			if ( pwPathEdit ) {
				pwPathEdit->SetTextW( std::format( L"ROM CHR Size:\t{0:08X} ({0} bytes, {1:g} kibibytes)", rRom.vChrRom.size(), rRom.vChrRom.size() / 1024.0 ).c_str() );
			}
			pwPathEdit = FindChild( CPatchWindowLayout::LSN_PWI_INFO_ROM_MIRROR_LABEL );
			if ( pwPathEdit ) {
				switch ( rRom.riInfo.mmMirroring ) {
					case LSN_MM_VERTICAL : {
						pwPathEdit->SetTextW( std::format( L"Mirroring:\tVertical", rRom.vPrgRom.size(), rRom.vPrgRom.size() / 1024.0 ).c_str() );
						break;
					}
					case LSN_MM_HORIZONTAL : {
						pwPathEdit->SetTextW( std::format( L"Mirroring:\tHorizontal", rRom.vPrgRom.size(), rRom.vPrgRom.size() / 1024.0 ).c_str() );
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
	 * Adds all the nodes in _vNodes as children of _hParent.
	 * 
	 * \param _vNodes The nodes to add under _hParent.
	 * \param _hParent The parent under which to add _vNodes.
	 * \return DESC
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

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The patch window.
 */

#include "LSNPatchWindow.h"
#include "../../File/LSNStdFile.h"
#include "../../File/LSNZipFile.h"
#include "../../Localization/LSNLocalization.h"
#include "../WinUtilities/LSNWinUtilities.h"

#include <commdlg.h>
#include <filesystem>


namespace lsn {

	CPatchWindow::CPatchWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
	}
	CPatchWindow::~CPatchWindow() {
	}

	// == Functions.
	/**
	 * The WM_INITDIALOG handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CPatchWindow::InitDialog() {

		return CMainWindow::InitDialog();
	}

	/**
	 * The WM_CLOSE handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CPatchWindow::Close() {
		//::DestroyWindow( Wnd() );
		if ( m_pwParent ) {
			::PostMessageW( m_pwParent->Wnd(), CWinUtilities::LSN_CLOSE_PATCHER, 0, 0 );
		}
		::EndDialog( Wnd(), 0 );
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
	CWidget::LSW_HANDLED CPatchWindow::Command( WORD _wCtrlCode, WORD _wId, CWidget * _pwSrc ) {
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
				ofnOpenFile.lpstrInitialDir = m_wInRomInitPath.c_str();
	
				std::u16string u16Path;
				if ( ::GetOpenFileNameW( &ofnOpenFile ) ) {
					m_wInRomInitPath = std::filesystem::path( ofnOpenFile.lpstrFile ).remove_filename();

					lsn::CZipFile zfFile;
					if ( zfFile.Open( reinterpret_cast<const char16_t *>(ofnOpenFile.lpstrFile) ) && zfFile.IsArchive() ) {
						std::vector<std::u16string> vFiles;
						zfFile.GatherArchiveFiles( vFiles, u".nes" );

						if ( vFiles.size() == 1 ) {
							m_vPatchRomFile.clear();
							if ( !zfFile.ExtractToMemory( vFiles[0], m_vPatchRomFile ) ) {
							}
							u16Path = reinterpret_cast<const char16_t *>(ofnOpenFile.lpstrFile);
							u16Path += u'{';
							u16Path += vFiles[0];
							u16Path += u'}';
							/*CUtilities::LSN_FILE_PATHS fpPath;
							CUtilities::DeconstructFilePath( reinterpret_cast<const char16_t *>(ofnOpenFile.lpstrFile), fpPath );*/
							//fpPath.u16sFile
						}
					}
					else if ( !zfFile.IsArchive() ) {
						lsn::CStdFile sfFile;
						if ( sfFile.Open( reinterpret_cast<const char16_t *>(ofnOpenFile.lpstrFile) ) ) {
							m_vPatchRomFile.clear();
							if ( !sfFile.LoadToMemory( m_vPatchRomFile ) ) {
							}
							u16Path = reinterpret_cast<const char16_t *>(ofnOpenFile.lpstrFile);
						}
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
				ofnOpenFile.lpstrInitialDir = m_wPatchInitPath.c_str();
				
				std::u16string u16Path;
				if ( ::GetOpenFileNameW( &ofnOpenFile ) ) {
					m_wPatchInitPath = std::filesystem::path( ofnOpenFile.lpstrFile ).remove_filename();

					lsn::CZipFile zfFile;
					if ( zfFile.Open( reinterpret_cast<const char16_t *>(ofnOpenFile.lpstrFile) ) && zfFile.IsArchive() ) {
						std::vector<std::u16string> vFiles;
						zfFile.GatherArchiveFiles( vFiles, u".ips" );
						zfFile.GatherArchiveFiles( vFiles, u".bps" );

						if ( vFiles.size() == 1 ) {
							m_vPatchRomFile.clear();
							if ( !zfFile.ExtractToMemory( vFiles[0], m_vPatchRomFile ) ) {
							}
							u16Path = reinterpret_cast<const char16_t *>(ofnOpenFile.lpstrFile);
							u16Path += u'{';
							u16Path += vFiles[0];
							u16Path += u'}';
							/*CUtilities::LSN_FILE_PATHS fpPath;
							CUtilities::DeconstructFilePath( reinterpret_cast<const char16_t *>(ofnOpenFile.lpstrFile), fpPath );*/
							//fpPath.u16sFile
						}
					}
					else if ( !zfFile.IsArchive() ) {
						lsn::CStdFile sfFile;
						if ( sfFile.Open( reinterpret_cast<const char16_t *>(ofnOpenFile.lpstrFile) ) ) {
							m_vPatchRomFile.clear();
							if ( !sfFile.LoadToMemory( m_vPatchRomFile ) ) {
							}
							u16Path = reinterpret_cast<const char16_t *>(ofnOpenFile.lpstrFile);
						}
					}
					auto pwPathEdit = FindChild( CPatchWindowLayout::LSN_PWI_FILE_PATCH_EDIT );
					if ( pwPathEdit ) {
						pwPathEdit->SetTextW( reinterpret_cast<const wchar_t *>(u16Path.c_str()) );
					}
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
	CWidget::LSW_HANDLED CPatchWindow::GetMinMaxInfo( MINMAXINFO * _pmmiInfo ) {
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
		_pmmiInfo->ptMinTrackSize.y = _pmmiInfo->ptMaxTrackSize.y = m_rStartingRect.Height();
		return LSW_H_HANDLED;
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

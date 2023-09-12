#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The main window of the emulator.
 */

/**
 * First because of:
 *	1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\um\objidlbase.h(7882,79): error C2872: 'byte': ambiguous symbol
 *	1>C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\shared\rpcndr.h(191,23): message : could be 'unsigned char byte'
 *	1>T:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.31.31103\include\cstddef(28,12): message : or       'std::byte'
 **/
#include <LSWWin.h>
#include <shlwapi.h>

#include "LSNMainWindow.h"
#include "../../File/LSNStdFile.h"
#include "../../File/LSNZipFile.h"
#include "../../Input/LSNDirectInput8.h"
#include "../../Utilities/LSNUtilities.h"
#include "../../Localization/LSNLocalization.h"
#include "../Input/LSNInputWindowLayout.h"
#include "../Layout/LSNLayoutManager.h"
#include "../SelectRom/LSNSelectRomDialogLayout.h"
#include "LSNMainWindowLayout.h"
#include <Rebar/LSWRebar.h>
#include <StatusBar/LSWStatusBar.h>
#include <ToolBar/LSWToolBar.h>
#include <commdlg.h>
#include <filesystem>
#include <hidpi.h>

#include "../../../resource.h"


#define LSN_SCALE_RESOLUTION					30.0


namespace lsn {

	CMainWindow::CMainWindow( const lsw::LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_bnEmulator( this, this ),
		m_aiThreadState( LSN_TS_INACTIVE ),
		m_pabIsAlive( reinterpret_cast<std::atomic_bool *>(_ui64Data) ),
		m_bMaximized( false ) {

		
		static const struct {
			LPCWSTR				lpwsImageName;
			DWORD				dwConst;
		} sImages[] = {
			{ L"1", LSN_I_OPENROM },

			{ L"73", LSN_I_OPTIONS },
		};
		m_iImages.Create( 24, 24, ILC_COLOR32, LSN_I_TOTAL, LSN_I_TOTAL );
		//WCHAR szBuffer[MAX_PATH];
		std::wstring wsBuffer;
		const DWORD dwSize = 0xFFFF;
		wsBuffer.resize( dwSize + 1 ); 
		::GetModuleFileNameW( NULL, wsBuffer.data(), dwSize );
		PWSTR pwsEnd = std::wcsrchr( wsBuffer.data(), L'\\' ) + 1;
		std::wstring wsRoot = wsBuffer.substr( 0, pwsEnd - wsBuffer.data() );

		m_bnEmulator.SetFolder( wsRoot.c_str() );
		m_bnEmulator.LoadSettings();

		for ( size_t I = 0; I < LSN_I_TOTAL; ++I ) {
			std::wstring wsTemp = wsRoot + L"Resources\\";
			wsTemp += sImages[I].lpwsImageName;
			wsTemp += L".bmp";

			m_bBitmaps[sImages[I].dwConst].LoadFromFile( wsTemp.c_str(), 0, 0, LR_CREATEDIBSECTION );
			m_iImageMap[sImages[I].dwConst] = m_iImages.Add( m_bBitmaps[sImages[I].dwConst].Handle() );
		}

		//HICON hIcon = reinterpret_cast<HICON>(::LoadImageW( CBase::GetModuleHandleW( nullptr ), (wsRoot + L"Resources\\icons8-bee-48.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADTRANSPARENT ) );
		//HICON hIcon = reinterpret_cast<HICON>(::LoadImageW( CBase::GetModuleHandleW( nullptr ), (wsRoot + L"Resources\\icons8-bee-64.png").c_str(), IMAGE_BITMAP, 0, 0, LR_LOADTRANSPARENT ) );
		HICON hIcon = reinterpret_cast<HICON>(::LoadImageW( CBase::GetModuleHandleW( nullptr ), MAKEINTRESOURCEW( IDI_ICON2 ), IMAGE_ICON, 0, 0, LR_LOADTRANSPARENT ) );
		::SendMessageW( Wnd(), WM_SETICON, (WPARAM)ICON_SMALL, (LPARAM)hIcon );

		hIcon = reinterpret_cast<HICON>(::LoadImageW( CBase::GetModuleHandleW( nullptr ), MAKEINTRESOURCEW( IDI_ICON1 ), IMAGE_ICON, 0, 0, LR_LOADTRANSPARENT ) );
		::SendMessageW( Wnd(), WM_SETICON, (WPARAM)ICON_BIG, (LPARAM)hIcon );
		//::SendMessageW( Wnd(), WM_SETICON, (WPARAM)ICON_SMALL, (LPARAM)m_bBitmaps[0].Handle() );
		// Create the basic render target.
		m_biBlitInfo.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
		m_biBlitInfo.bmiHeader.biWidth = 0;
		m_biBlitInfo.bmiHeader.biHeight = 0;
		m_biBlitInfo.bmiHeader.biPlanes = 1;
		m_biBlitInfo.bmiHeader.biBitCount = 32;
		m_biBlitInfo.bmiHeader.biCompression = BI_RGB;
		m_biBlitInfo.bmiHeader.biSizeImage = DWORD( CFilterBase::RowStride( m_biBlitInfo.bmiHeader.biWidth, m_biBlitInfo.bmiHeader.biBitCount ) * m_biBlitInfo.bmiHeader.biHeight );
		m_biBlitInfo.bmiHeader.biXPelsPerMeter = 0;
		m_biBlitInfo.bmiHeader.biYPelsPerMeter = 0;
		m_biBlitInfo.bmiHeader.biClrUsed = 0;
		m_biBlitInfo.bmiHeader.biClrImportant = 0;
		
		m_biBarInfo.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
		m_biBarInfo.bmiHeader.biWidth = 16;
		m_biBarInfo.bmiHeader.biHeight = 16;
		m_biBarInfo.bmiHeader.biPlanes = 1;
		m_biBarInfo.bmiHeader.biBitCount = 24;
		m_biBarInfo.bmiHeader.biCompression = BI_RGB;
		m_biBarInfo.bmiHeader.biSizeImage = DWORD( CFilterBase::RowStride( m_biBarInfo.bmiHeader.biWidth, m_biBarInfo.bmiHeader.biBitCount ) * m_biBarInfo.bmiHeader.biHeight );
		m_biBarInfo.bmiHeader.biXPelsPerMeter = 0;
		m_biBarInfo.bmiHeader.biYPelsPerMeter = 0;
		m_biBarInfo.bmiHeader.biClrUsed = 0;
		m_biBarInfo.bmiHeader.biClrImportant = 0;
		m_vBars.resize( m_biBarInfo.bmiHeader.biSizeImage );

		UpdatedConsolePointer( false );
		
		{
			// Center it in the screen.
			LSW_RECT rFinal = FinalWindowRect();
			LSW_RECT rDesktop;
			if ( ::SystemParametersInfoW( SPI_GETWORKAREA, 0, &rDesktop, 0 ) ) {
			//if ( ::GetWindowRect( ::GetDesktopWindow(), &rDesktop ) ) {
				rDesktop.left = (rDesktop.Width() - rFinal.Width()) / 2;
				rDesktop.top = (rDesktop.Height() - rFinal.Height()) / 2;
				::MoveWindow( Wnd(), rDesktop.left, rDesktop.top, rFinal.Width(), rFinal.Height(), TRUE );
			}
		}

		//RegisterRawInput();
		ScanInputDevices();
		
		

		(*m_pabIsAlive) = true;
	}
	CMainWindow::~CMainWindow() {
		StopThread();
		DestroyControllers();
		m_bnEmulator.SaveSettings();
		(*m_pabIsAlive) = false;
	}

	// == Functions.
	/**
	 * The WM_INITDIALOG handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CMainWindow::InitDialog() {
		lsw::CToolBar * plvToolBar = static_cast<lsw::CToolBar *>(FindChild( CMainWindowLayout::LSN_MWI_TOOLBAR0 ));
		lsw::CRebar * plvRebar = static_cast<lsw::CRebar *>(FindChild( CMainWindowLayout::LSN_MWI_REBAR0 ));


		// ==== TOOL BAR ==== //
		if ( plvToolBar ) {
			plvToolBar->SetImageList( 0, m_iImages );
//#define LSN_TOOL_STR( TXT )					reinterpret_cast<INT_PTR>(TXT)
#define LSN_TOOL_STR( TXT )						0
			const TBBUTTON bButtons[] = {
				// iBitmap							idCommand									fsState				fsStyle			bReserved	dwData	iString
				{ m_iImageMap[LSN_I_OPENROM],		CMainWindowLayout::LSN_MWMI_OPENROM,		TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		LSN_TOOL_STR( LSN_LSTR( LSN_OPEN__ROM ) ) },
				{ -1,								0,											TBSTATE_ENABLED,	BTNS_SEP,		{ 0 },		0,		0 },
				{ m_iImageMap[LSN_I_OPTIONS],		CMainWindowLayout::LSN_MWMI_OPTIONS,		TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		LSN_TOOL_STR( LSN_LSTR( LSN_OPTIONS ) ) },
			};
#undef LSN_TOOL_STR

			plvToolBar->AddButtons( bButtons, LSN_ELEMENTS( bButtons ) );
		}

		LSW_RECT rRebarRect = ClientRect( this );
		LONG lRebarHeight = 0;
		if ( plvRebar ) {
			plvRebar->SetImageList( m_iImages );
			{
				LSW_REBARBANDINFO riRebarInfo;
				riRebarInfo.SetColors( ::GetSysColor( COLOR_BTNTEXT ), ::GetSysColor( COLOR_BTNFACE ) );
				riRebarInfo.SetStyle( RBBS_CHILDEDGE |
				  RBBS_FIXEDBMP );
				riRebarInfo.SetChild( plvToolBar->Wnd() );
				riRebarInfo.SetChildSize( plvToolBar->GetMinBoundingRect().Width(), plvToolBar->GetMinBoundingRect().Height() );
				riRebarInfo.SetId( CMainWindowLayout::LSN_MWI_TOOLBAR0 );
				plvRebar->InsertBand( -1, riRebarInfo );
			}

			lRebarHeight = plvRebar->WindowRect( this ).Height();
		   ::MoveWindow( plvRebar->Wnd(), 0, 0, rRebarRect.Width(), lRebarHeight, FALSE );

			plvRebar->UpdateRects();
		}

		

		// ==== STATUS BAR ==== //
		lsw::CStatusBar * psbStatus = StatusBar();
		LSW_RECT rStatusBar;
		rStatusBar.Zero();
		if ( psbStatus ) {
			const CStatusBar::LSW_STATUS_PART spParts[] = {
				{ rRebarRect.Width() - psbStatus->ClientRect( this ).Height() - 48, TRUE },
				{ rRebarRect.Width() - psbStatus->ClientRect( this ).Height(), TRUE },
			};
			psbStatus->SetParts( spParts, LSN_ELEMENTS( spParts ) );
			rStatusBar = psbStatus->WindowRect();
		}
		
		UpdateOpenRecent();
		ForceSizeUpdate();
		
		LSW_RECT rScreen = FinalWindowRect();
		::MoveWindow( Wnd(), rScreen.left, rScreen.top, rScreen.Width(), rScreen.Height(), TRUE );

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
	CWidget::LSW_HANDLED CMainWindow::Command( WORD /*_wCtrlCode*/, WORD _wId, CWidget * /*_pwSrc*/ ) {
		if ( _wId >= CMainWindowLayout::LSN_MWMI_SHOW_RECENT_BASE ) {
			_wId -= CMainWindowLayout::LSN_MWMI_SHOW_RECENT_BASE;
			if ( _wId < m_bnEmulator.RecentFiles().size() ) {
				if ( m_bnEmulator.RecentFiles()[_wId].size() ) {
					if ( m_bnEmulator.RecentFiles()[_wId][m_bnEmulator.RecentFiles()[_wId].size()-1] == u'}' ) {
						CUtilities::LSN_FILE_PATHS fpPath;
						CUtilities::DeconstructFilePath( m_bnEmulator.RecentFiles()[_wId], fpPath );
						fpPath.u16sPath.pop_back();	// Pops the trailing u'\\'.
						LoadZipRom( fpPath.u16sPath, fpPath.u16sFile );
					}
					else {
						lsn::CStdFile sfFile;
						std::u16string s16File = m_bnEmulator.RecentFiles()[_wId];
						if ( sfFile.Open( s16File.c_str() ) ) {
							std::vector<uint8_t> vExtracted;
							if ( sfFile.LoadToMemory( vExtracted ) ) {
								LoadRom( vExtracted, s16File );
							}
						}
					}
				}
			}
			return LSW_H_CONTINUE;
		}
		switch ( _wId ) {
			case CMainWindowLayout::LSN_MWMI_OPENROM : {
				OPENFILENAMEW ofnOpenFile = { sizeof( ofnOpenFile ) };
				std::wstring szFileName;
				szFileName.resize( 0xFFFF + 2 );

#define LSN_FILE_OPEN_FORMAT				LSN_LSTR( LSN_ALL_SUPPORTED_FILES___NES____ZIP____NES___ZIP_ ) LSN_LSTR( LSN_NES_FILES____NES____NES_ ) LSN_LSTR( LSN_ZIP_FILES____ZIP____ZIP_ ) LSN_LSTR( LSN_ALL_FILES___________ ) L"\0" //LSN_ALL_SUPPORTED LSN_NES_FILES LSN_ZIP_FILES LSN_ALL_FILES L"\0"
				std::wstring wsFilter = std::wstring( LSN_FILE_OPEN_FORMAT, LSN_ELEMENTS( LSN_FILE_OPEN_FORMAT ) - 1 );
				ofnOpenFile.hwndOwner = Wnd();
				ofnOpenFile.lpstrFilter = wsFilter.c_str();
				ofnOpenFile.lpstrFile = szFileName.data();
				ofnOpenFile.nMaxFile = DWORD( szFileName.size() );
				ofnOpenFile.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
				//ofnOpenFile.lpstrDefExt = L"";

				if ( ::GetOpenFileNameW( &ofnOpenFile ) ) {
					const LPWSTR lpstrExt = &ofnOpenFile.lpstrFile[ofnOpenFile.nFileExtension];
					if ( lpstrExt ) {
						if ( ::StrCmpIW( lpstrExt, L"zip" ) == 0 ) {
							lsn::CZipFile zfFile;
							if ( zfFile.Open( reinterpret_cast<const char16_t *>(ofnOpenFile.lpstrFile) ) ) {
								std::vector<std::u16string> vFiles;
								if ( zfFile.GatherArchiveFiles( vFiles ) ) {
									std::vector<std::u16string> vFinalFiles;
									for ( size_t I = 0; I < vFiles.size(); ++I ) {
										std::u16string s16Ext = lsn::CUtilities::GetFileExtension( vFiles[I] );
										if ( ::StrCmpIW( reinterpret_cast<const wchar_t *>(s16Ext.c_str()), L"nes" ) == 0 ) {
											vFinalFiles.push_back( vFiles[I] );
										}
									}
									std::vector<uint8_t> vExtracted;
									DWORD dwIdx;
									if ( vFinalFiles.size() == 1 ) {
										dwIdx = 0;
										zfFile.ExtractToMemory( vFinalFiles[dwIdx], vExtracted );
									}
									else {
										dwIdx = CSelectRomDialogLayout::CreateSelectRomDialog( this, &vFinalFiles );
										if ( dwIdx < DWORD( vFinalFiles.size() ) ) {
											zfFile.ExtractToMemory( vFinalFiles[dwIdx], vExtracted );
										}
										else { return LSW_H_CONTINUE; }
									}
									std::u16string u16sPath = reinterpret_cast<const char16_t *>(ofnOpenFile.lpstrFile);
									u16sPath += u"{";
									u16sPath.append( vFinalFiles[dwIdx].c_str() );
									u16sPath += u"}";
									LoadRom( vExtracted, u16sPath );
									return LSW_H_CONTINUE;
								}
							}
							return LSW_H_CONTINUE;
						}
						else {
							lsn::CStdFile sfFile;
							std::u16string s16File = reinterpret_cast<const char16_t *>(ofnOpenFile.lpstrFile);
							if ( sfFile.Open( s16File.c_str() ) ) {
								std::vector<uint8_t> vExtracted;
								if ( sfFile.LoadToMemory( vExtracted ) ) {
									LoadRom( vExtracted, s16File );
									return LSW_H_CONTINUE;
								}
							}
						}
					}
				}
#undef LSN_FILE_OPEN_FORMAT
				break;
			}
			case CMainWindowLayout::LSN_MWMI_VIDEO_SIZE_1X : {
				m_bnEmulator.SetScale( 1.0 );
				LSW_RECT rScreen = FinalWindowRect();
				::MoveWindow( Wnd(), rScreen.left, rScreen.top, rScreen.Width(), rScreen.Height(), TRUE );
				break;
			}
			case CMainWindowLayout::LSN_MWMI_VIDEO_SIZE_2X : {
				m_bnEmulator.SetScale( 2.0 );
				LSW_RECT rScreen = FinalWindowRect();
				::MoveWindow( Wnd(), rScreen.left, rScreen.top, rScreen.Width(), rScreen.Height(), TRUE );
				break;
			}
			case CMainWindowLayout::LSN_MWMI_VIDEO_SIZE_3X : {
				m_bnEmulator.SetScale( 3.0 );
				LSW_RECT rScreen = FinalWindowRect();
				::MoveWindow( Wnd(), rScreen.left, rScreen.top, rScreen.Width(), rScreen.Height(), TRUE );
				break;
			}
			case CMainWindowLayout::LSN_MWMI_VIDEO_SIZE_4X : {
				m_bnEmulator.SetScale( 4.0 );
				LSW_RECT rScreen = FinalWindowRect();
				::MoveWindow( Wnd(), rScreen.left, rScreen.top, rScreen.Width(), rScreen.Height(), TRUE );
				break;
			}
			case CMainWindowLayout::LSN_MWMI_VIDEO_SIZE_5X : {
				m_bnEmulator.SetScale( 5.0 );
				LSW_RECT rScreen = FinalWindowRect();
				::MoveWindow( Wnd(), rScreen.left, rScreen.top, rScreen.Width(), rScreen.Height(), TRUE );
				break;
			}
			case CMainWindowLayout::LSN_MWMI_VIDEO_SIZE_6X : {
				m_bnEmulator.SetScale( 6.0 );
				LSW_RECT rScreen = FinalWindowRect();
				::MoveWindow( Wnd(), rScreen.left, rScreen.top, rScreen.Width(), rScreen.Height(), TRUE );
				break;
			}
			case CMainWindowLayout::LSN_MWMI_VIDEO_FILTER_NONE : {
				m_bnEmulator.SetCurFilter( CFilterBase::LSN_F_RGB24 );
				break;
			}
			case CMainWindowLayout::LSN_MWMI_VIDEO_FILTER_NTSC_BLARGG : {
				m_bnEmulator.SetCurFilter( CFilterBase::LSN_F_NTSC_BLARGG );
				break;
			}
			case CMainWindowLayout::LSN_MWMI_VIDEO_FILTER_PAL_BLARGG : {
				m_bnEmulator.SetCurFilter( CFilterBase::LSN_F_PAL_BLARGG );
				break;
			}
			case CMainWindowLayout::LSN_MWMI_VIDEO_FILTER_AUTO_BLARGG : {
				m_bnEmulator.SetCurFilter( CFilterBase::LSN_F_AUTO_BLARGG );
				break;
			}
			case CMainWindowLayout::LSN_MWMI_VIDEO_FILTER_NTSC_BISQWIT : {
				m_bnEmulator.SetCurFilter( CFilterBase::LSN_F_NTSC_BISQWIT );
				break;
			}
			case CMainWindowLayout::LSN_MWMI_VIDEO_FILTER_NTSC_CRT : {
				m_bnEmulator.SetCurFilter( CFilterBase::LSN_F_NTSC_CRT );
				break;
			}
			case CMainWindowLayout::LSN_MWMI_VIDEO_FILTER_NTSC_CRT_FULL : {
				m_bnEmulator.SetCurFilter( CFilterBase::LSN_F_NTSC_CRT_FULL );
				break;
			}
			case CMainWindowLayout::LSN_MWMI_VIDEO_FILTER_PAL_CRT_FULL : {
				m_bnEmulator.SetCurFilter( CFilterBase::LSN_F_PAL_CRT_FULL );
				break;
			}
			case CMainWindowLayout::LSN_MWMI_VIDEO_FILTER_AUTO_CRT : {
				m_bnEmulator.SetCurFilter( CFilterBase::LSN_F_AUTO_CRT );
				break;
			}
			case CMainWindowLayout::LSN_MWMI_VIDEO_FILTER_AUTO_CRT_FULL : {
				m_bnEmulator.SetCurFilter( CFilterBase::LSN_F_AUTO_CRT_FULL );
				break;
			}

			case CMainWindowLayout::LSN_MWMI_INPUT : {
				CInputWindowLayout::CreateInputDialog( this, m_bnEmulator.Options() );
				break;
			}
		}
		return LSW_H_CONTINUE;
	}

	// WM_NCDESTROY.
	CWidget::LSW_HANDLED CMainWindow::NcDestroy() {
		if ( m_bnEmulator.GetSystem()->IsRomLoaded() ) {
			StopThread();
			uint64_t ui64Time = m_cClock.GetRealTick() - m_cClock.GetStartTick();
			double dTime = ui64Time / double( m_cClock.GetResolution() );
			char szBuffer[256];
			::sprintf_s( szBuffer, "Ticks: %llu. Time: %.8f (%.8f hours).\r\n"
				"Master Cycles: %llu (%.8f per second; expected %.8f).\r\n"
				"%.8f cycles per Tick().\r\n"
				"%.8f FPS.\r\n",
				m_bnEmulator.GetSystem()->GetTickCount(), dTime, dTime / 60.0 / 60.0,
				m_bnEmulator.GetSystem()->GetMasterCounter(), m_bnEmulator.GetSystem()->GetMasterCounter() / dTime, double( m_bnEmulator.GetSystem()->GetMasterHz() ) / m_bnEmulator.GetSystem()->GetMasterDiv(),
				m_bnEmulator.GetSystem()->GetMasterCounter() / double( m_bnEmulator.GetSystem()->GetTickCount() ),
				m_bnEmulator.GetSystem()->GetPpuFrameCount() / dTime
				);
			::OutputDebugStringA( szBuffer );
		}
		::PostQuitMessage( 0 );
		return LSW_H_CONTINUE;
	}

	// WM_GETMINMAXINFO.
	CWidget::LSW_HANDLED CMainWindow::GetMinMaxInfo( MINMAXINFO * _pmmiInfo ) {
		{
			// Minimum.
			LSW_RECT rRect = FinalWindowRect( 1.0 );
			_pmmiInfo->ptMinTrackSize.x = rRect.Width();
			_pmmiInfo->ptMinTrackSize.y = rRect.Height();
		}
		/*{
			// Maximum.
			LSW_RECT rDesktop = CHelpers::UsableDesktopRect();
			LSW_RECT rWindowArea = FinalWindowRect( 0.0 );
			double dScaleW = double( rDesktop.Width() - rWindowArea.Width() ) / FinalWidth( 1.0 );
			double dScaleH = double( rDesktop.Height() - rWindowArea.Height() ) / FinalHeight( 1.0 );
			LSW_RECT rRect = FinalWindowRect( std::min( dScaleW, dScaleH ) );
			_pmmiInfo->ptMaxSize.x = rRect.Width();
			_pmmiInfo->ptMaxSize.y = rRect.Height();

			_pmmiInfo->ptMaxTrackSize.x = rRect.Width() + 5;
			_pmmiInfo->ptMaxTrackSize.y = rRect.Height() + 5;
		}*/
		return LSW_H_HANDLED;
	}

	// WM_PAINT.
	CWidget::LSW_HANDLED CMainWindow::Paint() {
		if ( !m_pdcClient ) { return LSW_H_CONTINUE; }
		LSW_BEGINPAINT bpPaint( Wnd() );
		::SetStretchBltMode( bpPaint.hDc, COLORONCOLOR );

		DWORD dwFinalW = FinalWidth();
		DWORD dwFinalH = FinalHeight();
		bool bMirrored = false;
		const uint8_t * puiBuffer = nullptr;
		{
			lsw::CCriticalSection::CEnterCrit ecCrit( m_csRenderCrit );
			m_bnEmulator.Render( dwFinalW, dwFinalH );
			m_biBlitInfo.bmiHeader.biWidth = m_bnEmulator.RenderInfo().ui32Width;
			m_biBlitInfo.bmiHeader.biHeight = m_bnEmulator.RenderInfo().ui32Height;
			m_biBlitInfo.bmiHeader.biBitCount = m_bnEmulator.RenderInfo().ui16Bits;
			m_biBlitInfo.bmiHeader.biSizeImage = CFilterBase::RowStride( m_biBlitInfo.bmiHeader.biWidth, m_biBlitInfo.bmiHeader.biBitCount ) * m_biBlitInfo.bmiHeader.biHeight;
			bMirrored = m_bnEmulator.RenderInfo().bMirrored;
			puiBuffer = m_bnEmulator.RenderInfo().pui8LastFilteredResult;
		}

		LONG lWidth = m_rMaxRect.Width();
		LONG lHeight = m_rMaxRect.Height();
		//int iDestX = m_bMaximized ? ((lWidth - dwFinalW) >> 1) : 0;
		int iDestX = (lWidth - int( dwFinalW )) >> 1;
		int iDestY = (lHeight - int( dwFinalH )) >> 1;
		int iBarDestX = int( iDestX + int( dwFinalW ) );
		int iBarDestY = int( iDestY + int( dwFinalH ) );
		if ( iDestX ) {
			::StretchDIBits( bpPaint.hDc,
				0, 0,
				iDestX, int( dwFinalH ),
				0, 0, m_biBarInfo.bmiHeader.biWidth, m_biBarInfo.bmiHeader.biHeight,
				m_vBars.data(),
				&m_biBarInfo,
				DIB_RGB_COLORS,
				SRCCOPY );
		}
		if ( iDestY ) {
			::StretchDIBits( bpPaint.hDc,
				iDestX, 0,
				int( dwFinalW ), iDestY,
				0, 0, m_biBarInfo.bmiHeader.biWidth, m_biBarInfo.bmiHeader.biHeight,
				m_vBars.data(),
				&m_biBarInfo,
				DIB_RGB_COLORS,
				SRCCOPY );
		}

		
		int iBarWidth = int( lWidth ) - iBarDestX;
		if ( iBarWidth > 0 ) {
			::StretchDIBits( bpPaint.hDc,
				iBarDestX, 0,
				iBarWidth, int( dwFinalH ),
				0, 0, m_biBarInfo.bmiHeader.biWidth, m_biBarInfo.bmiHeader.biHeight,
				m_vBars.data(),
				&m_biBarInfo,
				DIB_RGB_COLORS,
				SRCCOPY );
		}
		int iBarHeight = int( lHeight ) - iBarDestY;
		if ( iBarHeight > 0 ) {
			::StretchDIBits( bpPaint.hDc,
				iDestX, iBarDestY,
				int( dwFinalW ), iBarHeight,
				0, 0, m_biBarInfo.bmiHeader.biWidth, m_biBarInfo.bmiHeader.biHeight,
				m_vBars.data(),
				&m_biBarInfo,
				DIB_RGB_COLORS,
				SRCCOPY );
		}

		if ( dwFinalW != DWORD( m_biBlitInfo.bmiHeader.biWidth ) || dwFinalH != DWORD( m_biBlitInfo.bmiHeader.biHeight ) || !bMirrored ) {
			//::SetStretchBltMode( bpPaint.hDc, HALFTONE );
			::StretchDIBits( bpPaint.hDc,
				iDestX, (bMirrored ? 0 : int( dwFinalH - 1 )) + iDestY,
				int( dwFinalW ), bMirrored ? int( dwFinalH ) : -int( dwFinalH ),
				0, 0, m_biBlitInfo.bmiHeader.biWidth, m_biBlitInfo.bmiHeader.biHeight,
				puiBuffer,
				&m_biBlitInfo,
				DIB_RGB_COLORS,
				SRCCOPY );
		}
		else {
			::SetDIBitsToDevice( bpPaint.hDc,
				iDestX, iDestY,
				m_biBlitInfo.bmiHeader.biWidth, m_biBlitInfo.bmiHeader.biHeight,
				0, 0,
				0, m_biBlitInfo.bmiHeader.biHeight,
				puiBuffer,
				&m_biBlitInfo,
				DIB_RGB_COLORS );
		}


		return LSW_H_HANDLED;
	}

	// WM_MOVE.
	CWidget::LSW_HANDLED CMainWindow::Move( LONG /*_lX*/, LONG /*_lY*/ ) {
		Tick();
		return LSW_H_CONTINUE;
	}

	/**
	 * The WM_INPUT handler.
	 *
	 * \param _iCode The input code. Use GET_RAWINPUT_CODE_WPARAM macro to get the value. Can be one of the following values: RIM_INPUT, RIM_INPUTSINK.
	 * \param _hRawInput A HRAWINPUT handle to the RAWINPUT structure that contains the raw input from the device. To get the raw data, use this handle in the call to GetRawInputData.
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CMainWindow::Input( INT /*_iCode*/, HRAWINPUT /*_hRawInput*/ ) {
		return LSW_H_CONTINUE;
#if 0
		if ( _iCode == RIM_INPUT ) {
			std::vector<LSW_RAW_INPUT_DEVICE_LIST> vList = CHelpers::GatherRawInputDevices( RIM_TYPEHID );
			// Remove non-game usage pages.
			for ( auto I = vList.size(); I--; ) {
				if ( vList[I].diInfo.hid.usUsage != HID_USAGE_GENERIC_GAMEPAD || vList[I].diInfo.hid.usUsagePage != HID_USAGE_PAGE_GENERIC ) {
					vList.erase( vList.begin() + I );
				}
			}

			std::vector<uint8_t> vInputs = CHelpers::GetRawInputData_Input( _hRawInput );
			if ( vInputs.size() ) {
				RAWINPUT * rihHeader = reinterpret_cast<RAWINPUT *>(vInputs.data());

				// Find the device among our list of devices.
				LSW_RAW_INPUT_DEVICE_LIST * prdilThis = nullptr;
				for ( auto I = vList.size(); I--; ) {
					if ( vList[I].hDevice == rihHeader->header.hDevice ) {
						// Get the stored device's associated preparsed data.
						prdilThis = &vList[I];
						break;
					}
				}
				if ( prdilThis ) {
					HIDP_CAPS cCaps;
					::HidP_GetCaps( reinterpret_cast<PHIDP_PREPARSED_DATA>(prdilThis->vPreparsedData.data()), &cCaps );

					ULONG ulMaLen = ::HidP_MaxUsageListLength( HidP_Input, HID_USAGE_PAGE_GENERIC, reinterpret_cast<PHIDP_PREPARSED_DATA>(prdilThis->vPreparsedData.data()) );
					std::vector<CHAR> vReport;
					vReport.resize( cCaps.OutputReportByteLength );

					ULONG ulLen = 0;
					NTSTATUS hStatus = ::HidP_GetUsages( HidP_Input, HID_USAGE_PAGE_GENERIC,
						0, nullptr, &ulLen,
						reinterpret_cast<PHIDP_PREPARSED_DATA>(prdilThis->vPreparsedData.data()),
						vReport.data(),
						ULONG( vReport.size() ) );
					switch ( hStatus ) {
						case HIDP_STATUS_SUCCESS : {
							ulLen++;
							break;
						}
						case HIDP_STATUS_INVALID_REPORT_LENGTH : {
							ulLen++;
							break;
						}
						case HIDP_STATUS_INVALID_REPORT_TYPE : {
							ulLen++;
							break;
						}
						case HIDP_STATUS_BUFFER_TOO_SMALL : {
							ulLen++;
							break;
						}
						case HIDP_STATUS_INCOMPATIBLE_REPORT_ID : {
							ulLen++;
							break;
						}
						case HIDP_STATUS_INVALID_PREPARSED_DATA : {
							ulLen++;
							break;
						}
						case HIDP_STATUS_USAGE_NOT_FOUND : {
							ulLen++;
							break;
						}
					}
				}


				/*BOOLEAN bPrep = ::HidD_GetPreparsedData( _hRawInput, &pdPreparsed );
				if ( bPrep ) {
					HIDP_CAPS cCaps;
					::HidP_GetCaps( pdPreparsed, &cCaps );
					std::vector<CHAR> vReport;
					vReport.resize( cCaps.OutputReportByteLength );

					ULONG ulLen = 0;
					NTSTATUS hStatus = HidP_GetUsages( HidP_Input, HID_USAGE_PAGE_GENERIC,
						0, nullptr, &ulLen,
						pdPreparsed,
						vReport.data(),
						ULONG( vReport.size() ) );

					::HidD_FreePreparsedData( pdPreparsed );
				}
				else {
					//CBase::PrintError( L"Failed to get preparsed data for HID device." );
				}*/
				return LSW_H_CONTINUE;
			}
		}
		return LSW_H_CONTINUE;
#endif
	}

	/**
	 * The WM_INPUT_DEVICE_CHANGE handler.
	 *
	 * \param _iNotifCode This parameter can be one of the following values: GIDC_ARRIVAL, GIDC_REMOVAL.
	 * \param _hDevice The HANDLE to the raw input device.
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CMainWindow::InputDeviceChanged( INT _iNotifCode, HANDLE /*_hDevice*/ ) {
		switch ( _iNotifCode ) {
			case GIDC_ARRIVAL : {
				break;
			}
			case GIDC_REMOVAL : {
				break;
			}
		}
		return LSW_H_CONTINUE;
	}

	/**
	 * The WM_SIZE handler.
	 *
	 * \param _wParam The type of resizing requested.
	 * \param _lWidth The new width of the client area.
	 * \param _lHeight The new height of the client area.
	 * \return Returns a LSW_HANDLED enumeration.
	 */
	CWidget::LSW_HANDLED CMainWindow::Size( WPARAM _wParam, LONG _lWidth, LONG _lHeight ) {
		Parent::Size( _wParam, _lWidth, _lHeight );
		m_bMaximized = _wParam == SIZE_MAXIMIZED;
		//if ( m_bMaximized ) {
			m_rMaxRect = ClientRect();
		//}
		if ( _wParam == SIZE_MAXIMIZED || _wParam == SIZE_RESTORED ) {
			LSW_RECT rWindowArea = FinalWindowRect( 0.0 );
			double dScaleW = double( _lWidth - rWindowArea.Width() ) / FinalWidth( 1.0 );
			double dScaleH = double( _lHeight - rWindowArea.Height() ) / FinalHeight( 1.0 );
			m_bnEmulator.SetScale( std::round( std::min( dScaleW, dScaleH ) * LSN_SCALE_RESOLUTION ) / LSN_SCALE_RESOLUTION );
			/*LSW_RECT rCur = WindowRect();
			LSW_RECT rNew = FinalWindowRect();
			::MoveWindow( Wnd(), rCur.left, rCur.top, rNew.Width(), rNew.Height(), TRUE );*/
		}

		return LSW_H_HANDLED;
	}

	/**
	 * The WM_SIZING handler.
	 *
	 * \param _iEdge The edge of the window that is being sized.
	 * \param _prRect A pointer to a RECT structure with the screen coordinates of the drag rectangle. To change the size or position of the drag rectangle, an application must change the members of this structure.
	 * \return Returns a LSW_HANDLED enumeration.
	 */
	CWidget::LSW_HANDLED CMainWindow::Sizing( INT _iEdge, LSW_RECT * _prRect ) {
		if ( m_pdcClient && !m_bMaximized ) {
			LSW_RECT rBorders = FinalWindowRect( 0.0 );
			// Dragging horizontal bars means "expand the width to match."
			if ( _iEdge == WMSZ_BOTTOM || _iEdge == WMSZ_BOTTOMRIGHT || _iEdge == WMSZ_BOTTOMLEFT ||
				_iEdge == WMSZ_TOP || _iEdge == WMSZ_TOPRIGHT || _iEdge == WMSZ_TOPLEFT ) {
				double dScale = double( _prRect->Height() - rBorders.Height() ) / FinalHeight( 1.0 );
				dScale = std::round( dScale * LSN_SCALE_RESOLUTION ) / LSN_SCALE_RESOLUTION;

				LSW_RECT rNew = FinalWindowRect( dScale );
				// If the drag was on the top, move the rectangle to maintain the BOTTOM coordinate.
				if ( _iEdge == WMSZ_TOP || _iEdge == WMSZ_TOPRIGHT || _iEdge == WMSZ_TOPLEFT ) {
					rNew.MoveBy( 0, (*_prRect).bottom - rNew.bottom );
				}
				// If the drag was on the left, move the rectangle to maintain the RIGHT coordinate.
				if ( _iEdge == WMSZ_BOTTOMLEFT || _iEdge == WMSZ_TOPLEFT ) {
					rNew.MoveBy( (*_prRect).right - rNew.right, 0 );
				}
				(*_prRect) = rNew;
				m_bnEmulator.SetScale( dScale );
			}
			// dragging verticals expands the height.
			else if ( _iEdge == WMSZ_LEFT || _iEdge == WMSZ_RIGHT ) {
				double dScale = double( _prRect->Width() - rBorders.Width() ) / FinalWidth( 1.0 );
				dScale = std::round( dScale * LSN_SCALE_RESOLUTION ) / LSN_SCALE_RESOLUTION;

				LSW_RECT rNew = FinalWindowRect( dScale );
				// If the drag was on the left, move the rectangle to maintain the RIGHT coordinate.
				if ( _iEdge == WMSZ_LEFT ) {
					rNew.MoveBy( (*_prRect).right - rNew.right, 0 );
				}
				(*_prRect) = rNew;
				m_bnEmulator.SetScale( dScale );
			}
			return LSW_H_HANDLED;
		}

		return LSW_H_CONTINUE;
	}

	/**
	 * The WM_INITMENUPOPUP handler.
	 *
	 * \param _hMenu A handle to the drop-down menu or submenu.
	 * \param _wPos The zero-based relative position of the menu item that opens the drop-down menu or submenu.
	 * \param _bIsWindowMenu Indicates whether the drop-down menu is the window menu. If the menu is the window menu, this parameter is TRUE; otherwise, it is FALSE.
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CMainWindow::InitMenuPopup( HMENU _hMenu, WORD /*_wPos*/, BOOL _bIsWindowMenu ) {
		if ( _bIsWindowMenu ) { return LSW_H_CONTINUE; }
		for ( int I = ::GetMenuItemCount( _hMenu ); --I >= 0; ) {
			
			UINT uiId = ::GetMenuItemID( _hMenu, I );
			switch ( uiId ) {
#define LSN_CHECK_SCALE( SCALE )																																						\
	case CMainWindowLayout::LSN_MWMI_VIDEO_SIZE_ ## SCALE ## X : {																														\
		MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( m_bnEmulator.GetScale() == double( SCALE ) ? MFS_CHECKED : MFS_UNCHECKED ) };	\
		::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );																															\
		break;																																											\
	}
				LSN_CHECK_SCALE( 1 );
				LSN_CHECK_SCALE( 2 );
				LSN_CHECK_SCALE( 3 );
				LSN_CHECK_SCALE( 4 );
				LSN_CHECK_SCALE( 5 );
				LSN_CHECK_SCALE( 6 );
#undef LSN_CHECK_SCALE

				case CMainWindowLayout::LSN_MWMI_VIDEO_FILTER_NONE : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( m_bnEmulator.GetCurFilter() == CFilterBase::LSN_F_RGB24 ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case CMainWindowLayout::LSN_MWMI_VIDEO_FILTER_NTSC_BLARGG : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( m_bnEmulator.GetCurFilter() == CFilterBase::LSN_F_NTSC_BLARGG ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case CMainWindowLayout::LSN_MWMI_VIDEO_FILTER_PAL_BLARGG : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( m_bnEmulator.GetCurFilter() == CFilterBase::LSN_F_PAL_BLARGG ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case CMainWindowLayout::LSN_MWMI_VIDEO_FILTER_NTSC_BISQWIT : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( m_bnEmulator.GetCurFilter() == CFilterBase::LSN_F_NTSC_BISQWIT ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case CMainWindowLayout::LSN_MWMI_VIDEO_FILTER_NTSC_CRT : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( m_bnEmulator.GetCurFilter() == CFilterBase::LSN_F_NTSC_CRT ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case CMainWindowLayout::LSN_MWMI_VIDEO_FILTER_NTSC_CRT_FULL : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( m_bnEmulator.GetCurFilter() == CFilterBase::LSN_F_NTSC_CRT_FULL ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case CMainWindowLayout::LSN_MWMI_VIDEO_FILTER_PAL_CRT_FULL : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( m_bnEmulator.GetCurFilter() == CFilterBase::LSN_F_PAL_CRT_FULL ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case CMainWindowLayout::LSN_MWMI_VIDEO_FILTER_AUTO_BLARGG : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( m_bnEmulator.GetCurFilter() == CFilterBase::LSN_F_AUTO_BLARGG ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case CMainWindowLayout::LSN_MWMI_VIDEO_FILTER_AUTO_CRT : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( m_bnEmulator.GetCurFilter() == CFilterBase::LSN_F_AUTO_CRT ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case CMainWindowLayout::LSN_MWMI_VIDEO_FILTER_AUTO_CRT_FULL : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( m_bnEmulator.GetCurFilter() == CFilterBase::LSN_F_AUTO_CRT_FULL ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
			}
		}
		return LSW_H_CONTINUE;
	}

	/**
	 * Advances the emulation state by the amount of time that has passed since the last advancement.
	 */
	void CMainWindow::Tick() {
		/*if ( m_pnsSystem.get() && m_pnsSystem->IsRomLoaded() && m_aiThreadState == LSN_TS_INACTIVE ) {
			m_pnsSystem->Tick();
		}*/
	}

	/**
	 * Informs the host that a frame has been rendered.  This typically causes a display update and a framebuffer swap.
	 */
	void CMainWindow::Swap() {
		{
			lsw::CCriticalSection::CEnterCrit ecCrit( m_csRenderCrit );
			m_bnEmulator.Swap();
		}
		::RedrawWindow( Wnd(), NULL, NULL,
			RDW_INVALIDATE |
			RDW_NOERASE | RDW_NOFRAME | RDW_VALIDATE |
			/*RDW_UPDATENOW |*/
			/*RDW_NOCHILDREN*/RDW_ALLCHILDREN );
	}

	/**
	 * Starts running the rom on a thread.  Tick() no longer becomes useful while the emulator is running in its own thread.
	 */
	void CMainWindow::StartThread() {
		StopThread();
		if ( m_bnEmulator.GetSystem() && m_bnEmulator.GetSystem()->IsRomLoaded() ) {
			m_ptThread = std::make_unique<std::thread>( EmuThread, this );
		}
	}

	/**
	 * Stops the game thread and waits for it to join before returning.
	 */
	void CMainWindow::StopThread() {
		if ( m_ptThread.get() ) {
			if ( m_aiThreadState == LSN_TS_ACTIVE ) {
				m_aiThreadState = LSN_TS_STOP;
				while ( true ) {
					std::this_thread::sleep_for( std::chrono::milliseconds( 30 ) );
					if ( m_aiThreadState != LSN_TS_STOP ) { break; }
				}
				m_ptThread->join();
			}
			m_ptThread.reset();
		}
	}

	/**
	 * Polls the given port and returns a byte containing the result of polling by combining the LSN_INPUT_BITS values.
	 *
	 * \param _ui8Port The port being polled (0 or 1).
	 * \return Returns the result of polling the given port.
	 */
	uint8_t CMainWindow::PollPort( uint8_t /*_ui8Port*/ ) {
		if ( GetFocus() ) {
			uint8_t ui8Ret = 0;
			if ( m_ptThread.get() ) {
				SHORT sState;
#define LSN_TICK_RAPID( RAPID_IDX, BUTTON )														\
		if ( m_bnEmulator.RapidFire()[RAPID_IDX] & 0b10000000 ) {								\
			ui8Ret |= BUTTON;																	\
		}																						\
		m_bnEmulator.RapidFire()[RAPID_IDX] = _rotl8( m_bnEmulator.RapidFire()[RAPID_IDX], 1 );
#define LSN_CKECK( MAIN_KEY, RAPID_KEY, RAPID_IDX, BUTTON )										\
	sState = ::GetAsyncKeyState( RAPID_KEY );													\
	if ( sState & 0x8000 ) {																	\
		LSN_TICK_RAPID( RAPID_IDX, BUTTON );													\
	}																							\
	else {																						\
		m_bnEmulator.RapidFire()[RAPID_IDX] = 0b11110000;										\
		sState = ::GetAsyncKeyState( MAIN_KEY );												\
		if ( sState & 0x8000 ) { ui8Ret |= BUTTON; }											\
	}
				if ( m_pdi8cControllers.size() >= 1 ) {
					m_pdi8cControllers[0]->Poll();
				}
				if ( m_pdi8cControllers.size() >= 1 ) {
					if ( m_pdi8cControllers[0]->JoyState().rgbButtons[3] ) { LSN_TICK_RAPID( 0, LSN_IB_B ); }
					else {
						m_bnEmulator.RapidFire()[0] = 0b11110000;
						if ( m_pdi8cControllers[0]->JoyState().rgbButtons[2] ) { ui8Ret |= LSN_IB_B; }
					}
				}
				else {
					LSN_CKECK( 'L', VK_OEM_PERIOD, 0, LSN_IB_B );
				}
				if ( m_pdi8cControllers.size() >= 1 ) {
					if ( m_pdi8cControllers[0]->JoyState().rgbButtons[1] ) { LSN_TICK_RAPID( 1, LSN_IB_A ); }
					else {
						m_bnEmulator.RapidFire()[1] = 0b11110000;
						if ( m_pdi8cControllers[0]->JoyState().rgbButtons[0] ) { ui8Ret |= LSN_IB_A; }
					}
				}
				else {
					LSN_CKECK( VK_OEM_1, VK_OEM_2, 1, LSN_IB_A );
				}

				if ( m_pdi8cControllers.size() >= 1 ) {
					if ( m_pdi8cControllers[0]->JoyState().rgbButtons[8] ) { LSN_TICK_RAPID( 2, LSN_IB_SELECT ); }
					else {
						m_bnEmulator.RapidFire()[2] = 0b11110000;
						if ( m_pdi8cControllers[0]->JoyState().rgbButtons[6] ) { ui8Ret |= LSN_IB_SELECT; }
					}
				}
				else {
					LSN_CKECK( 'O', '9', 2, LSN_IB_SELECT );
				}
				if ( m_pdi8cControllers.size() >= 1 ) {
					if ( m_pdi8cControllers[0]->JoyState().rgbButtons[9] ) { LSN_TICK_RAPID( 3, LSN_IB_START ); }
					else {
						m_bnEmulator.RapidFire()[3] = 0b11110000;
						if ( m_pdi8cControllers[0]->JoyState().rgbButtons[7] ) { ui8Ret |= LSN_IB_START; }
					}
				}
				else {
					LSN_CKECK( 'P', '0', 3, LSN_IB_START );
				}

				if ( m_pdi8cControllers.size() >= 1 ) {
					if ( static_cast<int16_t>(m_pdi8cControllers[0]->JoyState().lY) < -250 ||
						((static_cast<int16_t>(m_pdi8cControllers[0]->JoyState().rgdwPOV[0]) >= 0 && static_cast<int16_t>(m_pdi8cControllers[0]->JoyState().rgdwPOV[0]) <= 4500) ||
						(static_cast<int16_t>(m_pdi8cControllers[0]->JoyState().rgdwPOV[0]) >= 31500 && static_cast<int16_t>(m_pdi8cControllers[0]->JoyState().rgdwPOV[0]) <= 36000)) ) { ui8Ret |= LSN_IB_UP; }
				}
				else {
					LSN_CKECK( 'W', '2', 4, LSN_IB_UP );
				}
				if ( m_pdi8cControllers.size() >= 1 ) {
					if ( static_cast<int16_t>(m_pdi8cControllers[0]->JoyState().lY) > 250 ||
						(static_cast<int16_t>(m_pdi8cControllers[0]->JoyState().rgdwPOV[0]) >= 13500 && static_cast<int16_t>(m_pdi8cControllers[0]->JoyState().rgdwPOV[0]) <= 22500)
						/*static_cast<int16_t>(m_pdi8cControllers[0]->JoyState().rgdwPOV[0]) == 18000*/ ) { ui8Ret |= LSN_IB_DOWN; }
				}
				else {
					LSN_CKECK( 'S', 'X', 5, LSN_IB_DOWN );
				}
				if ( m_pdi8cControllers.size() >= 1 ) {
					if ( static_cast<int16_t>(m_pdi8cControllers[0]->JoyState().lX) < -250 ||
						(static_cast<int16_t>(m_pdi8cControllers[0]->JoyState().rgdwPOV[0]) >= 22500 && static_cast<int16_t>(m_pdi8cControllers[0]->JoyState().rgdwPOV[0]) <= 31500)
						/*static_cast<int16_t>(m_pdi8cControllers[0]->JoyState().rgdwPOV[0]) == 27000*/ ) { ui8Ret |= LSN_IB_LEFT; }
				}
				else {
					LSN_CKECK( 'A', 'Q', 6, LSN_IB_LEFT );
				}
				if ( m_pdi8cControllers.size() >= 1 ) {
					if ( static_cast<int16_t>(m_pdi8cControllers[0]->JoyState().lX) > 250 ||
						(static_cast<int16_t>(m_pdi8cControllers[0]->JoyState().rgdwPOV[0]) >= 4500 && static_cast<int16_t>(m_pdi8cControllers[0]->JoyState().rgdwPOV[0]) <= 13500)
						/*static_cast<int16_t>(m_pdi8cControllers[0]->JoyState().rgdwPOV[0]) == 9000*/ ) { ui8Ret |= LSN_IB_RIGHT; }
				}
				else {
					LSN_CKECK( 'D', 'E', 7, LSN_IB_RIGHT );
				}
#undef LSN_CKECK
#undef LSN_TICK_RAPID
			}
			else {
				BYTE bPoll[256];
				::GetKeyboardState( bPoll );
				
				if ( bPoll['L'] & 0x80 ) { ui8Ret |= LSN_IB_B; }
				if ( bPoll[VK_OEM_1] & 0x80 ) { ui8Ret |= LSN_IB_A; }

				if ( bPoll['O'] & 0x80 ) { ui8Ret |= LSN_IB_SELECT; }
				if ( bPoll['P'] & 0x80 ) { ui8Ret |= LSN_IB_START; }

				if ( bPoll['W'] & 0x80 ) { ui8Ret |= LSN_IB_UP; }
				if ( bPoll['S'] & 0x80 ) { ui8Ret |= LSN_IB_DOWN; }
				if ( bPoll['A'] & 0x80 ) { ui8Ret |= LSN_IB_LEFT; }
				if ( bPoll['D'] & 0x80 ) { ui8Ret |= LSN_IB_RIGHT; }
			}

			return ui8Ret;
		}
		return 0;
	}

	/**
	 * Virtual client rectangle.  Can be used for things that need to be adjusted based on whether or not status bars, toolbars, etc. are present.
	 *
	 * \param _pwChild Optional child control.
	 * \return Returns the virtual client rectangle of this object or of the optional child object.
	 */
	const lsw::LSW_RECT CMainWindow::VirtualClientRect( const CWidget * /*_pwChild*/ ) const {
		LSW_RECT rTemp = ClientRect( this );
		const CRebar * plvRebar = static_cast<const CRebar *>(FindChild( CMainWindowLayout::LSN_MWI_REBAR0 ));
		if ( plvRebar ) {
			LSW_RECT rRebar = plvRebar->ClientRect( this );
			rTemp.top += rRebar.Height();
		}

		const CStatusBar * psbStatus = StatusBar();
		if ( psbStatus ) {
			LSW_RECT rStatus = psbStatus->ClientRect( this );
			rTemp.bottom -= rStatus.Height();
		}
		return rTemp;
	}

	/**
	 * Gets the status bar.
	 *
	 * \return the status bar.
	 */
	lsw::CStatusBar * CMainWindow::StatusBar() {
		return static_cast<lsw::CStatusBar *>(FindChild( CMainWindowLayout::LSN_MWI_STATUSBAR ));
	}

	/**
	 * Gets the status bar.
	 *
	 * \return the status bar.
	 */
	const lsw::CStatusBar * CMainWindow::StatusBar() const {
		return static_cast<const lsw::CStatusBar *>(FindChild( CMainWindowLayout::LSN_MWI_STATUSBAR ));
	}

	/**
	 * Gets the window rectangle for correct output at a given scale and ratio.
	 *
	 * \param _dScale A scale override or -1.0 to use m_dScale.
	 * \return Returns the window rectangle for a given client area, derived from the desired output scale and ratio.
	 */
	LSW_RECT CMainWindow::FinalWindowRect( double _dScale ) const {
		
		LONG lHeight = 0L;
		const lsw::CRebar * plvRebar = static_cast<const lsw::CRebar *>(FindChild( CMainWindowLayout::LSN_MWI_REBAR0 ));
		if ( plvRebar ) {
			lHeight += plvRebar->WindowRect().Height();
		}
		const lsw::CStatusBar * psbStatus = StatusBar();
		if ( psbStatus ) {
			lHeight += psbStatus->WindowRect().Height();
		}
		LSW_RECT rClientRect;
		rClientRect.Zero();
		rClientRect.SetWidth( FinalWidth( _dScale ) );
		rClientRect.SetHeight( FinalHeight( _dScale ) );
		::AdjustWindowRectEx( &rClientRect, ::GetWindowLongW( Wnd(), GWL_STYLE ),
			TRUE, ::GetWindowLongW( Wnd(), GWL_EXSTYLE ) );
		rClientRect.bottom += lHeight;

		LSW_RECT rScreen = rClientRect.ClientToScreen( Wnd() );
		rScreen.SetWidth( rClientRect.Width() );
		rScreen.SetHeight( rClientRect.Height() );
		return rScreen;
	}

	/**
	 * Updates m_dRatio after a change to m_dRatioActual.
	 */
	void CMainWindow::UpdateRatio() {
		if ( m_pdcClient ) {
			m_bnEmulator.SetRatio( (m_bnEmulator.GetRatioActual() * m_pdcClient->DisplayHeight()) / m_pdcClient->DisplayWidth() );
		}
	}

	/**
	 * Sends a given palette to the console.
	 *
	 * \param _vPalette The loaded palette file.  Must be (0x40 * 3) bytes.
	 */
	void CMainWindow::SetPalette( const std::vector<uint8_t> &_vPalette ) {
		if ( _vPalette.size() != 0x40 * 3 && _vPalette.size() != (1 << 9) * 3 ) { return; }
		if ( !m_bnEmulator.GetSystem() ) { return; }
		lsn::LSN_PALETTE * ppPal = m_bnEmulator.GetSystem()->Palette();
		if ( !ppPal ) { return; }
		for ( size_t I = 0; I < _vPalette.size(); I += 3 ) {
			size_t stIdx = (I / 3);
			ppPal->uVals[stIdx].sRgb.ui8R = uint8_t( std::round( CHelpers::LinearTosRGB( _vPalette[I+2] / 255.0 ) * 255.0 ) );
			ppPal->uVals[stIdx].sRgb.ui8G = uint8_t( std::round( CHelpers::LinearTosRGB( _vPalette[I+1] / 255.0 ) * 255.0 ) );
			ppPal->uVals[stIdx].sRgb.ui8B = uint8_t( std::round( CHelpers::LinearTosRGB( _vPalette[I+0] / 255.0 ) * 255.0 ) );

			/*ppPal->uVals[stIdx].sRgb.ui8R = _vPalette[I+2];
			ppPal->uVals[stIdx].sRgb.ui8G = _vPalette[I+1];
			ppPal->uVals[stIdx].sRgb.ui8B = _vPalette[I+0];*/
		}
	}

	/**
	 * Loads a ROM given its in-memory image and its file name.
	 *
	 * \param _vRom The in-memory ROM file.
	 * \param _s16Path The full path to the ROM.
	 * \return Returns true if loading of the ROM succeeded.
	 */
	bool CMainWindow::LoadRom( const std::vector<uint8_t> &_vRom, const std::u16string &_s16Path ) {
		StopThread();
		if ( m_bnEmulator.LoadRom( _vRom, _s16Path ) ) {
			UpdatedConsolePointer();
			if ( m_bnEmulator.GetSystem()->GetRom() ) {
				std::u16string u16Name = u"BeesNES: " + CUtilities::NoExtension( CUtilities::GetFileName( m_bnEmulator.GetSystem()->GetRom()->riInfo.s16RomName ) );
				if ( m_bnEmulator.GetSystem()->GetRom()->riInfo.ui16Mapper == 4 ) {
					u16Name += u" (Partial Support)";
				}
				::SetWindowTextW( Wnd(), reinterpret_cast<LPCWSTR>(u16Name.c_str()) );
				UpdateOpenRecent();
			}
			m_bnEmulator.GetSystem()->ResetState( false );
			m_cClock.SetStartingTick();
			StartThread();
			return true;
		}
#if 0
		LSN_ROM rTmp;
		if ( CSystemBase::LoadRom( _vRom, rTmp, _s16Path ) ) {
			m_pnsSystem.reset();
			/*m_pnsSystem = std::make_unique<CRegionalSystem>();*/
			switch ( rTmp.riInfo.pmConsoleRegion ) {
				case LSN_PPU_METRICS::LSN_PM_NTSC : {
					m_pnsSystem = std::make_unique<CNtscSystem>();
					break;
				}
				case LSN_PPU_METRICS::LSN_PM_PAL : {
					m_pnsSystem = std::make_unique<CPalSystem>();
					break;
				}
				case LSN_PPU_METRICS::LSN_PM_DENDY : {
					m_pnsSystem = std::make_unique<CDendySystem>();
					break;
				}
				default : { m_pnsSystem = std::make_unique<CNtscSystem>(); }
			}
			UpdatedConsolePointer();
			if ( m_pnsSystem->LoadRom( rTmp ) ) {
				if ( m_pnsSystem->GetRom() ) {
					std::u16string u16Name = u"BeesNES: " + CUtilities::NoExtension( m_pnsSystem->GetRom()->riInfo.s16RomName );
					if ( m_pnsSystem->GetRom()->riInfo.ui16Mapper == 4 ) {
						u16Name += u" (Partial Support)";
					}
					::SetWindowTextW( Wnd(), reinterpret_cast<LPCWSTR>(u16Name.c_str()) );
				}
				m_pnsSystem->ResetState( false );
				m_cClock.SetStartingTick();
				StartThread();
				return true;
			}
		}
#endif
		::SetWindowTextW( Wnd(), L"BeesNES" );
		return false;
	}

	/**
	 * Loads a ROM file given a path to a ZIP and a file name inside the ZIP.
	 * 
	 * \param _s16ZipPath The path to the ZIP file.
	 * \param _s16File The file name inside the ZIP path to load.
	 * \return Returns true if the file was loaded.
	 **/
	bool CMainWindow::LoadZipRom( const std::u16string &_s16ZipPath, const std::u16string &_s16File ) {
		lsn::CZipFile zfFile;
		if ( zfFile.Open( _s16ZipPath.c_str() ) ) {
			std::vector<std::u16string> vFiles;
			if ( zfFile.GatherArchiveFiles( vFiles ) ) {
				for ( size_t I = 0; I < vFiles.size(); ++I ) {
					if ( vFiles[I] == _s16File ) {
						std::vector<uint8_t> vExtracted;
						zfFile.ExtractToMemory( vFiles[I], vExtracted );
						std::u16string u16sPath = _s16ZipPath;
						u16sPath += u"{";
						u16sPath.append( vFiles[I].c_str() );
						u16sPath += u"}";
						return LoadRom( vExtracted, u16sPath );
					}
				}
			}
		}
		return false;
	}

	/**
	 * Call when changing the m_pnsSystem pointer to hook everything (display client, input polling, etc.) back up to the new system.
	 * 
	 * \param _bMoveWindow If true, te window is resized.
	 */
	void CMainWindow::UpdatedConsolePointer( bool _bMoveWindow ) {
		if ( m_bnEmulator.GetSystem() ) {
			m_pdcClient = m_bnEmulator.GetDisplayClient();

			UpdateRatio();

			{
				std::wstring wsBuffer;
				const DWORD dwSize = 0xFFFF;
				wsBuffer.resize( dwSize + 1 ); 
				::GetModuleFileNameW( NULL, wsBuffer.data(), dwSize );
				PWSTR pwsEnd = std::wcsrchr( wsBuffer.data(), L'\\' ) + 1;
				std::wstring wsRoot = wsBuffer.substr( 0, pwsEnd - wsBuffer.data() );
				//std::wstring wsTemp = wsRoot + L"Palettes\\nespalette.pal";
				std::wstring wsTemp = wsRoot + L"Palettes\\ntscpalette.saturation1.2.pal";
				lsn::CStdFile sfFile;
				if ( sfFile.Open( reinterpret_cast<const char16_t *>(wsTemp.c_str()) ) ) {
					std::vector<uint8_t> vPal;
					if ( sfFile.LoadToMemory( vPal ) ) {
						SetPalette( vPal );
					}
				}
			}


			if ( _bMoveWindow && !m_bMaximized ) {
				// Update the window (size only).
				LSW_RECT rFinal = FinalWindowRect();
				::MoveWindow( Wnd(), rFinal.left, rFinal.top, rFinal.Width(), rFinal.Height(), TRUE );
			}
		}
	}

	/**
	 * Updates the "Open Recent" menu.
	 **/
	void CMainWindow::UpdateOpenRecent() {
		HMENU hMenu = ::GetSubMenu( ::GetMenu( Wnd() ), 0 );
		HMENU hListMenu = ::GetSubMenu( hMenu, 1 );
		if ( hListMenu != NULL ) {
			if ( ::DestroyMenu( hListMenu ) ) {
				hListMenu = NULL;
			}
		}

		// Store a local copy so that the main list is able to change while we work (should never happen in practice but let's just code safely).
		std::vector<std::u16string> vList = m_bnEmulator.RecentFiles();
		::DeleteMenu( hMenu, 1, MF_BYPOSITION );
		if ( vList.size() ) {
			std::vector<CUtilities::LSN_FILE_PATHS> vPaths = CUtilities::DeconstructFilePaths( vList );
			std::vector<size_t> vIndices;
			while ( CUtilities::DuplicateFiles( vPaths, vIndices ) ) {
				for ( auto I = vIndices.size(); I--; ) {
					CUtilities::CopyLastFolderToFileName( vPaths[vIndices[I]].u16sPath, vPaths[vIndices[I]].u16sFile );
				}
			}

			std::vector<LSW_MENU_ITEM> vMenu;
			LSW_MENU_ITEM miItem = { FALSE, 0, FALSE, FALSE, TRUE };
			for ( size_t I = 0; I < vList.size(); ++I ) {
				miItem.dwId = DWORD( CMainWindowLayout::LSN_MWMI_SHOW_RECENT_BASE + I );
				miItem.lpwcText = reinterpret_cast<LPCWSTR>(vPaths[I].u16sFile.c_str());
				vMenu.push_back( miItem );
			}
			LSW_MENU_LAYOUT miMenus = {
				CMainWindowLayout::LSN_MWMI_OPENRECENT_MENU,
				0,
				0,
				vMenu.size(),
				vMenu.data()
			};
			HMENU hThis = lsw::CBase::LayoutManager()->CreateMenu( &miMenus, 1 );
			//if ( !::ModifyMenuW( hMenu, CMainWindowLayout::LSN_MWMI_OPENRECENT, MF_BYCOMMAND | MF_POPUP | MF_STRING,
			//	reinterpret_cast<UINT_PTR>(hThis), LSN_LSTR( LSN_OPEN_REC_ENT ) ) ) {
			if ( ::InsertMenuW( hMenu, 1, MF_BYPOSITION | MF_POPUP | MF_STRING,
				reinterpret_cast<UINT_PTR>(hThis), LSN_LSTR( LSN_OPEN_REC_ENT ) ) ) {
				::DestroyMenu( hThis );
			}
			return;
		}
		::InsertMenuW( hMenu, 1, MF_BYPOSITION | MF_GRAYED | MF_STRING,
			static_cast<UINT_PTR>(CMainWindowLayout::LSN_MWMI_OPENRECENT), LSN_LSTR( LSN_OPEN_REC_ENT ) );
	}

	/**
	 * Registers for raw input.
	 * 
	 * \return Returns true if registration for raw input succeeded.
	 */
	bool CMainWindow::RegisterRawInput() {
		RAWINPUTDEVICE ridDevices[1];
        
		ridDevices[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
		ridDevices[0].usUsage = HID_USAGE_GENERIC_GAMEPAD;
		ridDevices[0].dwFlags = RIDEV_DEVNOTIFY;
		ridDevices[0].hwndTarget = Wnd();

		if ( CHelpers::RegisterRawInputDevices( ridDevices, 1 ) == FALSE ) {
			return false;
		}

		UINT uiTotal;
		::GetRegisteredRawInputDevices( NULL, &uiTotal, sizeof( RAWINPUTDEVICE ) );
		std::vector<RAWINPUTDEVICE> vDevs;
		vDevs.resize( uiTotal );
		::GetRegisteredRawInputDevices( vDevs.data(), &uiTotal, sizeof( RAWINPUTDEVICE ) );
		return true;
	}

	/**
	 * Scans for USB controllers.
	 */
	void CMainWindow::ScanInputDevices() {
		DestroyControllers();
		std::vector<DIDEVICEINSTANCE> vDevices = CDirectInput8::GatherDevices( DI8DEVCLASS_GAMECTRL );
		for ( size_t I = 0; I < vDevices.size(); ++I ) {
			CDirectInput8Controller * pdi8cController = new CDirectInput8Controller();
			if ( !pdi8cController->CreateController( vDevices[I].guidInstance, m_hWnd /*::GetModuleHandleW( NULL )*/ /*NULL*/ ) ) {
				delete pdi8cController;
			}
			else {
				m_pdi8cControllers.push_back( pdi8cController );
			}
		}

		/*std::vector<LSW_RAW_INPUT_DEVICE_LIST> vList = CHelpers::GatherRawInputDevices( RIM_TYPEHID );
		// Remove non-game usage pages.
		for ( auto I = vList.size(); I--; ) {
			if ( vList[I].diInfo.hid.usUsage != HID_USAGE_GENERIC_GAMEPAD || vList[I].diInfo.hid.usUsagePage != HID_USAGE_PAGE_GENERIC ) {
				vList.erase( vList.begin() + I );
			}
		}*/
		return;
	}

	/**
	 * Destroys all controller inputs.
	 **/
	void CMainWindow::DestroyControllers() {
		for ( auto I = m_pdi8cControllers.size(); I--; ) {
			delete m_pdi8cControllers[I];
		}
		m_pdi8cControllers.clear();
	}

	/**
	 * The emulator thread.
	 *
	 * \param _pmwWindow Pointer to this object.
	 */
	void CMainWindow::EmuThread( lsn::CMainWindow * _pmwWindow ) {
		if ( !_pmwWindow->m_bnEmulator.GetSystem() || !_pmwWindow->m_bnEmulator.GetSystem()->IsRomLoaded() ) {
			_pmwWindow->m_aiThreadState = LSN_TS_INACTIVE;
			return;
		}
		_pmwWindow->m_aiThreadState = LSN_TS_ACTIVE;
#ifdef LSN_WINDOWS
		::SetThreadAffinityMask( ::GetCurrentThread(), 1 );
#endif	// #ifdef LSN_WINDOWS

		while ( _pmwWindow->m_aiThreadState != LSN_TS_STOP ) {
			_pmwWindow->m_bnEmulator.GetSystem()->Tick();
			//::Sleep( 1 );
		}
		_pmwWindow->m_aiThreadState = LSN_TS_INACTIVE;
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS
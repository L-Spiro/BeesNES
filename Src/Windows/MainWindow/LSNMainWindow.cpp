#ifdef LSN_USE_WINDOWS

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
#include "../../Utilities/LSNUtilities.h"
#include "../../Localization/LSNLocalization.h"
#include "../SelectRom/LSNSelectRomDialogLayout.h"
#include "LSNMainWindowLayout.h"
#include <Rebar/LSWRebar.h>
#include <StatusBar/LSWStatusBar.h>
#include <ToolBar/LSWToolBar.h>
#include <commdlg.h>

namespace lsn {

	CMainWindow::CMainWindow( const lsw::LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_dScale( 3.0 ),
		m_dRatio( 292.608 / 256.0 ),
		m_stBufferIdx( 0 ),
		m_pabIsAlive( reinterpret_cast< std::atomic_bool *>(_ui64Data) ) {


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
		for ( size_t I = 0; I < LSN_I_TOTAL; ++I ) {
			std::wstring wsTemp = wsRoot + L"Resources\\";
			wsTemp += sImages[I].lpwsImageName;
			wsTemp += L".bmp";

			m_bBitmaps[sImages[I].dwConst].LoadFromFile( wsTemp.c_str(), 0, 0, LR_CREATEDIBSECTION );
			m_iImageMap[sImages[I].dwConst] = m_iImages.Add( m_bBitmaps[sImages[I].dwConst].Handle() );
		}

		m_pnsSystem = std::make_unique<CRegionalSystem>();
		m_pdcClient = m_pnsSystem->GetDisplayClient();
		if ( m_pdcClient ) {
			m_pdcClient->SetDisplayHost( this );
			// Create the basic render target.
			const size_t stBuffers = 2;
			const WORD wBitDepth = 24;
			m_vBasicRenderTarget.resize( stBuffers );
			for ( auto I = stBuffers; I--; ) {
				DWORD dwStride = RowStride( m_pdcClient->DisplayWidth(), wBitDepth );
				m_vBasicRenderTarget[I].resize( sizeof( BITMAPINFOHEADER ) + (dwStride * m_pdcClient->DisplayHeight()) );
				BITMAPINFO * pbiHeader = reinterpret_cast<BITMAPINFO *>(m_vBasicRenderTarget[I].data());
				pbiHeader->bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
				pbiHeader->bmiHeader.biWidth = m_pdcClient->DisplayWidth();
				pbiHeader->bmiHeader.biHeight = m_pdcClient->DisplayHeight();
				pbiHeader->bmiHeader.biPlanes = 1;
				pbiHeader->bmiHeader.biBitCount = wBitDepth;
				pbiHeader->bmiHeader.biCompression = BI_RGB;
				pbiHeader->bmiHeader.biSizeImage = dwStride * m_pdcClient->DisplayHeight();
				pbiHeader->bmiHeader.biXPelsPerMeter = 0;
				pbiHeader->bmiHeader.biYPelsPerMeter = 0;
				pbiHeader->bmiHeader.biClrUsed = 0;
				pbiHeader->bmiHeader.biClrImportant = 0;

				// For fun.
				uint8_t * pui8Pixels = m_vBasicRenderTarget[I].data() + pbiHeader->bmiHeader.biSize;
				for ( auto Y = m_pdcClient->DisplayHeight(); Y--; ) {
					for ( auto X = m_pdcClient->DisplayWidth(); X--; ) {
						uint8_t * pui8This = &pui8Pixels[Y*dwStride+X*3];
						/*pui8This[2] = uint8_t( CHelpers::LinearTosRGB( X / 255.0 ) * 255.0 );
						pui8This[1] = uint8_t( CHelpers::LinearTosRGB( Y / 255.0 ) * 255.0 );*/
						pui8This[2] = uint8_t( CHelpers::sRGBtoLinear( X / 255.0 ) * 255.0 );
						pui8This[0] = uint8_t( CHelpers::LinearTosRGB( Y / 255.0 ) * 255.0 );
					}
				}
			}
		}

		{
			// Center it in the screen.
			LSW_RECT rFinal = FinalWindowRect();
			LSW_RECT rDesktop;
			if ( ::GetWindowRect( ::GetDesktopWindow(), &rDesktop ) ) {
				rDesktop.left = (rDesktop.Width() - rFinal.Width()) / 2;
				rDesktop.top = (rDesktop.Height() - rFinal.Height()) / 2 - 32;
				/*rDesktop.SetWidth( rFinal.Width() );
				rDesktop.SetHeight( rFinal.Height() );*/
				::MoveWindow( Wnd(), rDesktop.left, rDesktop.top, rFinal.Width(), rFinal.Height(), TRUE );
			}
		}

		{
			std::wstring wsTemp = wsRoot + L"Palettes\\nespalette.pal";
			lsn::CStdFile sfFile;
			if ( sfFile.Open( reinterpret_cast<const char16_t *>(wsTemp.c_str()) ) ) {
				std::vector<uint8_t> vPal;
				if ( sfFile.LoadToMemory( vPal ) ) {
					SetPalette( vPal );
				}
			}
		}

		(*m_pabIsAlive) = true;
	}
	CMainWindow::~CMainWindow() {
		(*m_pabIsAlive) = false;
	}

	// == Functions.
	// WM_INITDIALOG.
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
				{ m_iImageMap[LSN_I_OPENROM],		CMainWindowLayout::LSN_MWMI_OPENROM,		TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		LSN_TOOL_STR( LSN_LSTR( LSN_OPEN_PROCESS ) ) },
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
		

		ForceSizeUpdate();
		
		LSW_RECT rScreen = FinalWindowRect();
		::MoveWindow( Wnd(), rScreen.left, rScreen.top, rScreen.Width(), rScreen.Height(), TRUE );

		return LSW_H_CONTINUE;
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED CMainWindow::Command( WORD /*_wCtrlCode*/, WORD _Id, CWidget * /*_pwSrc*/ ) {
		switch ( _Id ) {
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
									m_pnsSystem->LoadRom( vExtracted, vFinalFiles[dwIdx] );
									m_pnsSystem->ResetState( false );
									m_ui64TickCount = 0;
									m_cClock.SetStartingTick();
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
									m_pnsSystem->LoadRom( vExtracted, s16File );
									m_pnsSystem->ResetState( false );
									m_ui64TickCount = 0;
									m_cClock.SetStartingTick();
									return LSW_H_CONTINUE;
								}
							}
						}
					}
				}
#undef LSN_FILE_OPEN_FORMAT
				break;
			}
		}
		return LSW_H_CONTINUE;
	}

	// WM_NCDESTROY.
	CWidget::LSW_HANDLED CMainWindow::NcDestroy() {
		uint64_t ui64Time = m_cClock.GetRealTick() - m_cClock.GetStartTick();
		if ( m_pnsSystem->IsRomLoaded() ) {
			double dTime = ui64Time / double( m_cClock.GetResolution() );
			char szBuffer[256];
			::sprintf_s( szBuffer, "Ticks: %llu. Time: %.8f (%.8f hours).\r\n"
				"Master Cycles: %llu (%.8f per second; expected %.8f).\r\n"
				"%.8f cycles per Tick().\r\n"
				"%.8f FPS.\r\n",
				m_ui64TickCount, dTime, dTime / 60.0 / 60.0,
				m_pnsSystem->GetMasterCounter(), m_pnsSystem->GetMasterCounter() / dTime, double( m_pnsSystem->GetMasterHz() ) / m_pnsSystem->GetMasterDiv(),
				m_pnsSystem->GetMasterCounter() / double( m_ui64TickCount ),
				m_pnsSystem->GetPpuFrameCount() / dTime
				);
			::OutputDebugStringA( szBuffer );
		}
		::PostQuitMessage( 0 );
		return LSW_H_CONTINUE;
	}

	// WM_GETMINMAXINFO.
	CWidget::LSW_HANDLED CMainWindow::GetMinMaxInfo( MINMAXINFO * _pmmiInfo ) {
		LSW_RECT rRect = FinalWindowRect();
		_pmmiInfo->ptMinTrackSize.x = rRect.Width();
		_pmmiInfo->ptMinTrackSize.y = rRect.Height();
		_pmmiInfo->ptMaxTrackSize = _pmmiInfo->ptMinTrackSize;
		return LSW_H_HANDLED;
	}

	// WM_PAINT.
	CWidget::LSW_HANDLED CMainWindow::Paint() {
		if ( !m_pdcClient ) { return LSW_H_CONTINUE; }
		LSW_BEGINPAINT bpPaint( Wnd() );

		BITMAPINFO * pbiHeader = reinterpret_cast<BITMAPINFO *>(m_vBasicRenderTarget[m_stBufferIdx].data());
		DWORD dwFinalW = FinalWidth();
		DWORD dwFinalH = FinalHeight();
		if ( dwFinalW != m_pdcClient->DisplayWidth() || dwFinalH != m_pdcClient->DisplayHeight() ) {
			::SetStretchBltMode( bpPaint.hDc, COLORONCOLOR );
			::StretchDIBits( bpPaint.hDc,
				0, 0, int( dwFinalW ), int( dwFinalH ),
				0, 0, m_pdcClient->DisplayWidth(), m_pdcClient->DisplayHeight(),
				&pbiHeader->bmiColors,
				pbiHeader,
				DIB_RGB_COLORS,
				SRCCOPY );
		}
		else {
			::SetDIBitsToDevice( bpPaint.hDc,
				0, 0,
				m_pdcClient->DisplayWidth(), m_pdcClient->DisplayHeight(),
				0, 0,
				0, m_pdcClient->DisplayHeight(),
				&pbiHeader->bmiColors,
				pbiHeader,
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
	 * Advances the emulation state by the amount of time that has passed since the last advancement.
	 */
	void CMainWindow::Tick() {
		if ( m_pnsSystem->IsRomLoaded() ) {
			m_pnsSystem->Tick();
			++m_ui64TickCount;
		}
	}

	/**
	 * Informs the host that a frame has been rendered.  This typically causes a display update and a framebuffer swap.
	 */
	void CMainWindow::Swap() {
		if ( m_pdcClient ) {
			BITMAPINFO * pbiHeader = reinterpret_cast<BITMAPINFO *>(m_vBasicRenderTarget[m_stBufferIdx].data());
			m_stBufferIdx = (m_stBufferIdx + 1) % m_vBasicRenderTarget.size();
			//size_t stNextIdx = (m_stBufferIdx + 1) % m_vBasicRenderTarget.size();

			
			m_pdcClient->SetRenderTarget( reinterpret_cast<uint8_t *>(&pbiHeader->bmiColors), RowStride( m_pdcClient->DisplayWidth(), 24 ) );
			::RedrawWindow( Wnd(), NULL, NULL,
				RDW_INVALIDATE |
				RDW_NOERASE | RDW_NOFRAME | RDW_VALIDATE |
				RDW_UPDATENOW |
				RDW_NOCHILDREN );
		}

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
	 * \return Returns the window rectangle for a given client area, derived from the desired output scale and ratio.
	 */
	LSW_RECT CMainWindow::FinalWindowRect() const {
		
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
		rClientRect.SetWidth( FinalWidth() );
		rClientRect.SetHeight( FinalHeight() );
		::AdjustWindowRectEx( &rClientRect, ::GetWindowLongW( Wnd(), GWL_STYLE ),
			TRUE, ::GetWindowLongW( Wnd(), GWL_EXSTYLE ) );
		rClientRect.bottom += lHeight;

		LSW_RECT rScreen = rClientRect.ClientToScreen( Wnd() );
		rScreen.SetWidth( rClientRect.Width() );
		rScreen.SetHeight( rClientRect.Height() );
		return rScreen;
	}

	/**
	 * Sends a given palette to the console.
	 *
	 * \param _vPalette The loaded palette file.  Must be (0x40 * 3) bytes.
	 */
	void CMainWindow::SetPalette( const std::vector<uint8_t> &_vPalette ) {
		if ( _vPalette.size() != 0x40 * 3 ) { return; }
		if ( !m_pnsSystem.get() ) { return; }
		lsn::LSN_PALETTE * ppPal = m_pnsSystem->Palette();
		if ( !ppPal ) { return; }
		for ( size_t I = 0; I < _vPalette.size(); I += 3 ) {
			size_t stIdx = (I / 3);
			ppPal->uVals[stIdx].sRgb.ui8R = _vPalette[I+2];
			ppPal->uVals[stIdx].sRgb.ui8G = _vPalette[I+1];
			ppPal->uVals[stIdx].sRgb.ui8B = _vPalette[I+0];
		}
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS
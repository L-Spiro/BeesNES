#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The patch window.
 */

#include "LSNPatchWindow.h"
#include "../../Crc/LSNCrc.h"
#include "../../File/LSNStdFile.h"
#include "../../File/LSNZipFile.h"
#include "../../Localization/LSNLocalization.h"
#include "../../System/LSNSystemBase.h"
#include "../../Windows/Layout/LSNLayoutManager.h"
#include "../Layout/LSNLayoutMacros.h"
#include "../WinUtilities/LSNWinUtilities.h"

#include <commdlg.h>
#include <filesystem>
#include <set>

#include "../../../resource.h"


namespace lsn {

	CPatchWindow::CPatchWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_poOptions( reinterpret_cast<LSN_OPTIONS *>(_ui64Data) ),
		m_bOutIsAutoFilled( false ) {
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
		Parent::InitDialog();

		SetIcons( reinterpret_cast<HICON>(::LoadImageW( CBase::GetModuleHandleW( nullptr ), MAKEINTRESOURCEW( IDI_PATCH_ICON_16 ), IMAGE_ICON, 0, 0, LR_LOADTRANSPARENT )),
			reinterpret_cast<HICON>(::LoadImageW( CBase::GetModuleHandleW( nullptr ), MAKEINTRESOURCEW( IDI_PATCH_ICON_32 ), IMAGE_ICON, 0, 0, LR_LOADTRANSPARENT )) );

		LSW_RECT rClient = ClientRect( nullptr );

		LSW_WIDGET_LAYOUT wlLayout = static_cast<lsn::CLayoutManager *>(lsw::CBase::LayoutManager())->FixLayout( LSW_WIDGET_LAYOUT{
			LSW_LT_SPLITTER,											// ltType
			static_cast<WORD>(CPatchWindowLayout::LSN_PWI_SPLITTER),	// wId
			reinterpret_cast<LPCWSTR>(lsw::CBase::SplitterAtom()),		// lpwcClass
			TRUE,														// bEnabled
			FALSE,														// bActive
			0,															// iLeft
			0,															// iTop
			static_cast<DWORD>(rClient.Width()),						// dwWidth
			static_cast<DWORD>(rClient.Height()),						// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE,								// dwStyle
			0,															// dwStyleEx
			nullptr,													// pwcText
			0,															// sTextLen
			LSN_LT_PATCH_DIALOG,										// dwParentId

			LSN_LOCK_LEFT,												// pcLeftSizeExp
			LSN_LOCK_RIGHT,												// pcRightSizeExp
			LSN_LOCK_TOP,												// pcTopSizeExp
			LSN_LOCK_BOTTOM,											// pcBottomSizeExp
			nullptr, 0,													// pcWidthSizeExp
			nullptr, 0,													// pcHeightSizeExp
		} );
		
		
		CWidget * pwTopPage = CPatchWindowLayout::CPatchWindowLayout::CreateTopPage( this, (*m_poOptions) );
		CWidget * pwBottomPage = CPatchWindowLayout::CPatchWindowLayout::CreateBottomPage( this, (*m_poOptions) );
		auto rTopRect = pwTopPage->ClientRect();
		m_vPages.push_back( pwTopPage );
		m_vPages.push_back( pwBottomPage );

		CSplitter * aSplitter = static_cast<CSplitter *>(static_cast<lsw::CLayoutManager *>(lsw::CBase::LayoutManager())->CreateWidget( wlLayout, this, TRUE, NULL, 0 ));
		if ( !aSplitter ) {
			delete aSplitter;
			return LSW_H_CONTINUE;
		}
		m_psSplitter = aSplitter;
		
		aSplitter->SetSplitterType( CSplitter::LSW_SS_HORIZONTAL );
		aSplitter->Attach( pwTopPage, CSplitter::LSW_A_UPPER );
		aSplitter->Attach( pwBottomPage, CSplitter::LSW_A_BOTTOM );
		return LSW_H_CONTINUE;
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
	CWidget::LSW_HANDLED CPatchWindow::Command( WORD /*_wCtrlCode*/, WORD /*_wId*/, CWidget * /*_pwSrc*/ ) {
		return LSW_H_CONTINUE;
	}

	/**
	 * Handles the WM_GETMINMAXINFO message.
	 * 
	 * \param _pmmiInfo The min/max info structure to fill out.
	 * \return Returns an LSW_HANDLED code.
	 **/
	CWidget::LSW_HANDLED CPatchWindow::GetMinMaxInfo( MINMAXINFO * _pmmiInfo ) {
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
	 * Gets the client rectangle for the given widget or gets this control's client rectangle if _pwChild is nullptr.
	 * 
	 * \param _pwChild The widget whose client rectangle is to be gotten.
	 * \return Returns the child's client rectangle or this client rectangle.
	 **/
	LSW_RECT CPatchWindow::ClientRect( const CWidget * _pwChild ) const {
		for ( auto I = m_vPages.size(); I--; ) {
			if ( m_vPages[I] == _pwChild ) { return m_psSplitter->ClientRect( _pwChild ); }
		}
		return Parent::ClientRect( _pwChild );
	}

	/**
	 * Gets the window rectangle for the given widget or gets this control's window rectangle if _pwChild is nullptr.
	 * 
	 * \param _pwChild The widget whose window rectangle is to be gotten.
	 * \return Returns the child's window rectangle or this client rectangle.
	 **/
	LSW_RECT CPatchWindow::WindowRect( const CWidget * _pwChild ) const {
		for ( auto I = m_vPages.size(); I--; ) {
			if ( m_vPages[I] == _pwChild ) {
				LSW_RECT rTmp = m_psSplitter->WindowRect( _pwChild );
				::AdjustWindowRectEx( &rTmp, ::GetWindowLongW( Wnd(), GWL_STYLE ), FALSE, ::GetWindowLongW( Wnd(), GWL_EXSTYLE ) );
				return rTmp;
			}
		}
		return Parent::WindowRect( _pwChild );
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

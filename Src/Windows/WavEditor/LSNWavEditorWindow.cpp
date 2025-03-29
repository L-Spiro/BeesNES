#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The WAV-Editor window.
 */

#include "LSNWavEditorWindow.h"
#include "../../Utilities/LSNUtilities.h"
#include "../WinUtilities/LSNWinUtilities.h"

#include <Base/LSWWndClassEx.h>

#include "../../../resource.h"


namespace lsn {

	// == Members.
	/** The main window class. */
	ATOM CWavEditorWindow::m_aAtom = 0;

	CWavEditorWindow::CWavEditorWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget , HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout.ChangeClass( reinterpret_cast<LPCWSTR>(m_aAtom) ), _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
	}
	CWavEditorWindow::~CWavEditorWindow() {
	}


	// == Functions.
	/**
	 * The WM_INITDIALOG handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CWavEditorWindow::InitDialog() {
		SetIcons( reinterpret_cast<HICON>(::LoadImageW( CBase::GetModuleHandleW( nullptr ), MAKEINTRESOURCEW( IDI_WAV_EDIT_ICON_16 ), IMAGE_ICON, 0, 0, LR_LOADTRANSPARENT )),
			reinterpret_cast<HICON>(::LoadImageW( CBase::GetModuleHandleW( nullptr ), MAKEINTRESOURCEW( IDI_WAV_EDIT_ICON_32 ), IMAGE_ICON, 0, 0, LR_LOADTRANSPARENT )) );

		CWidget * pwSeqPage = Layout::CreateSequencer( this, 0 );
		if ( !pwSeqPage ) { return LSW_H_CONTINUE; }
		m_vSequencePages.push_back( static_cast<CWavEditorSequencingPage *>(pwSeqPage) );

		auto * ptGroup = FindChild( Layout::LSN_WEWI_FILES_GROUP );
		if ( ptGroup ) {
			auto aGroupRect = ptGroup->WindowRect().ScreenToClient( Wnd() );
			auto aThisRect = pwSeqPage->WindowRect().ScreenToClient( Wnd() );
			::MoveWindow( pwSeqPage->Wnd(), 0, aGroupRect.bottom, aThisRect.Width(), aThisRect.Height(), TRUE );
			auto aSeqGroup = pwSeqPage->FindChild( Layout::LSN_WEWI_SEQ_GROUP );
			if ( aSeqGroup ) {
				auto aTmp = aSeqGroup->WindowRect().ScreenToClient( Wnd() );
				::MoveWindow( ptGroup->Wnd(), aGroupRect.left, aGroupRect.top, aTmp.Width(), aGroupRect.Height(), TRUE );
				aGroupRect = ptGroup->WindowRect().ScreenToClient( Wnd() );
			}

			aThisRect = pwSeqPage->WindowRect();
			auto aWindowRect = WindowRect();
			LSW_RECT aAdjusted = aGroupRect;
			::AdjustWindowRectEx( &aAdjusted, GetStyle(), FALSE, GetStyleEx() );

			::MoveWindow( Wnd(), aWindowRect.left, aWindowRect.top, aAdjusted.Width() + aGroupRect.left * 2, aThisRect.bottom - aWindowRect.top + aGroupRect.left, TRUE );
		}

		UpdateRects();
		m_bInit = true;
		return LSW_H_CONTINUE;
	}

	/**
	 * The WM_CLOSE handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CWavEditorWindow::Close() {
		if ( m_pwParent ) {
			::PostMessageW( m_pwParent->Wnd(), CWinUtilities::LSN_CLOSE_WAV_EDITOR, 0, 0 );
		}
		::DestroyWindow( Wnd() );
		return LSW_H_HANDLED;
	}

	/**
	 * Handles the WM_COMMAND message.
	 *
	 * \param _wCtrlCode 0 = from menu, 1 = from accelerator, otherwise it is a Control-defined notification code.
	 * \param _wId The ID of the control if _wCtrlCode is not 0 or 1.
	 * \param _pwSrc The source control if _wCtrlCode is not 0 or 1.
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CWavEditorWindow::Command( WORD /*_wCtrlCode*/, WORD /*_wId*/, CWidget * /*_pwSrc*/ ) {
		return LSW_H_CONTINUE;
	}

	/**
	 * Handles the WM_GETMINMAXINFO message.
	 * 
	 * \param _pmmiInfo The min/max info structure to fill out.
	 * \return Returns an LSW_HANDLED code.
	 **/
	CWidget::LSW_HANDLED CWavEditorWindow::GetMinMaxInfo( MINMAXINFO * _pmmiInfo ) {
		if ( m_bInit ) {
			_pmmiInfo->ptMinTrackSize.x = m_rStartingRect.Width();
			_pmmiInfo->ptMinTrackSize.y = m_rStartingRect.Height();
			_pmmiInfo->ptMaxTrackSize.x = _pmmiInfo->ptMinTrackSize.x;
			_pmmiInfo->ptMaxTrackSize.y = _pmmiInfo->ptMinTrackSize.y;
			return LSW_H_HANDLED;
		}
		return LSW_H_CONTINUE;
	}

	/**
	 * Prepares to create the window.  Creates the atom if necessary.
	 **/
	void CWavEditorWindow::PrepareWavEditor() {
		if ( !m_aAtom ) {
			lsw::CWndClassEx wceEx;
			wceEx.SetInstance( lsw::CBase::GetThisHandle() );
			WCHAR szStr[23];
			lsn::CUtilities::RandomString( szStr, LSN_ELEMENTS( szStr ) );
			wceEx.SetClassName( szStr );
			wceEx.SetBackgroundBrush( reinterpret_cast<HBRUSH>(CTLCOLOR_DLG + 1) );
			wceEx.SetWindPro( CWidget::WindowProc );
			m_aAtom = lsw::CBase::RegisterClassExW( wceEx.Obj() );
		}
	}

 }	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

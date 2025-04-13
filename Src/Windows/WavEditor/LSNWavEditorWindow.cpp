#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The WAV-Editor window.
 */

#include "LSNWavEditorWindow.h"
#include "../../Localization/LSNLocalization.h"
#include "../../Utilities/LSNUtilities.h"
#include "../WinUtilities/LSNWinUtilities.h"

#include <Base/LSWWndClassEx.h>

#include "../../../resource.h"


namespace lsn {

	// == Members.
	/** The main window class. */
	ATOM CWavEditorWindow::m_aAtom = 0;

	CWavEditorWindow::CWavEditorWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget , HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout.ChangeClass( reinterpret_cast<LPCWSTR>(m_aAtom) ), _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_poOptions( reinterpret_cast<LSN_OPTIONS *>(_ui64Data) ) {
		m_wewoWindowOptions = m_poOptions->wewoWavEditorWindow;
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

		m_pwefFiles = static_cast<CWavEditorFilesPage *>(Layout::CreateFiles( this, m_wewoWindowOptions ));
		if ( !m_pwefFiles ) { return LSW_H_CONTINUE; }
		m_pwefFiles->SetWavEditorAndIndex( m_weEditor );

		CWidget * pwSeqPage = Layout::CreateSequencer( this, m_wewoWindowOptions );
		if ( !pwSeqPage ) { return LSW_H_CONTINUE; }
		m_vSequencePages.push_back( static_cast<CWavEditorSequencingPage *>(pwSeqPage) );

		CWidget * pwSettingsPage = Layout::CreateFileSettings( this, m_wewoWindowOptions );
		if ( !pwSettingsPage ) { return LSW_H_CONTINUE; }
		m_vSettingsPages.push_back( static_cast<CWavEditorFileSettingsPage *>(pwSettingsPage) );

		m_pweopOutput = static_cast<CWavEditorOutputPage *>(Layout::CreateOutput( this, m_wewoWindowOptions ));
		if ( !m_pweopOutput ) { return LSW_H_CONTINUE; }
		m_pweopOutput->SetWavEditorAndIndex( m_weEditor );

		auto * pwGroup = m_pwefFiles->FindChild( Layout::LSN_WEWI_FILES_GROUP );
		if ( pwGroup ) {
			LONG lMaxHeight = 0;
			auto aGroupRect = pwGroup->WindowRect().ScreenToClient( Wnd() );
			auto aFileSettingsRect = pwSettingsPage->WindowRect().ScreenToClient( Wnd() );
			auto aSeqRect = pwSeqPage->WindowRect().ScreenToClient( Wnd() );
			auto aOutputRect = m_pweopOutput->WindowRect().ScreenToClient( Wnd() );
			::MoveWindow( pwSeqPage->Wnd(), aGroupRect.left, aGroupRect.bottom, aSeqRect.Width(), aSeqRect.Height(), TRUE );
			::MoveWindow( pwSettingsPage->Wnd(), aSeqRect.Width() + aGroupRect.left * 3, aGroupRect.top, aFileSettingsRect.Width(), aFileSettingsRect.Height(), TRUE );
			::MoveWindow( m_pweopOutput->Wnd(), aSeqRect.Width() + aGroupRect.left * 3, aGroupRect.top + aFileSettingsRect.Height(), aOutputRect.Width(), aOutputRect.Height(), TRUE );

			

			auto pwSeqGroup = pwSeqPage->FindChild( Layout::LSN_WEWI_SEQ_RANGE_GROUP );
			if ( pwSeqGroup ) {
				auto rTmpGrpRect = pwSeqGroup->WindowRect().ScreenToClient( Wnd() );

				

				LONG lTreeRight = 100;
				static const WORD dwIds[] = {
					Layout::LSN_WEWI_FILES_ADD_BUTTON,
					Layout::LSN_WEWI_FILES_ADD_META_BUTTON,
					Layout::LSN_WEWI_FILES_REMOVE_BUTTON,
					Layout::LSN_WEWI_FILES_UP_BUTTON,
					Layout::LSN_WEWI_FILES_DOWN_BUTTON,
				};
				for ( size_t I = 0; I < LSN_ELEMENTS( dwIds ); ++I ) {
					auto pwButton = m_pwefFiles->FindChild( dwIds[I] );
					if ( pwButton ) {
						auto rTmpThisRect = pwButton->WindowRect().ScreenToClient( Wnd() );
						lTreeRight = rTmpGrpRect.right - rTmpThisRect.Width() - aGroupRect.left;
						::MoveWindow( pwButton->Wnd(), rTmpGrpRect.right - rTmpThisRect.Width(), rTmpThisRect.top, rTmpThisRect.Width(), rTmpThisRect.Height(), TRUE );
					}
				}
				

				auto pwTree = m_pwefFiles->FindChild( Layout::LSN_WEWI_FILES_TREELISTVIEW );
				if ( pwTree ) {
					auto rTmpTreeRect = pwTree->WindowRect().ScreenToClient( Wnd() );
					
					::MoveWindow( pwTree->Wnd(), rTmpGrpRect.left, rTmpTreeRect.top, lTreeRight - rTmpGrpRect.left, rTmpTreeRect.Height(), TRUE );
				}
			}


			aSeqRect = pwSeqPage->WindowRect().ScreenToClient( Wnd() );
			aOutputRect = m_pweopOutput->WindowRect().ScreenToClient( Wnd() );
			lMaxHeight = std::max( aSeqRect.top + aSeqRect.Height(), aOutputRect.top + aOutputRect.Height() );


			auto pwOk = FindChild( Layout::LSN_WEWI_OK );
			if ( pwOk ) {
				auto rOkRect = pwOk->WindowRect().ScreenToClient( Wnd() );
				::MoveWindow( pwOk->Wnd(), rOkRect.left, aOutputRect.bottom - rOkRect.Height(), aGroupRect.right - rOkRect.left, rOkRect.Height(), TRUE );
			}
			auto pwCncel = FindChild( Layout::LSN_WEWI_CANCEL );
			if ( pwCncel ) {
				auto rCancelRect = pwCncel->WindowRect().ScreenToClient( Wnd() );
				::MoveWindow( pwCncel->Wnd(), rCancelRect.left, aOutputRect.bottom - rCancelRect.Height(), rCancelRect.Width(), rCancelRect.Height(), TRUE );
			}

			aSeqRect = pwSeqPage->WindowRect();
			aFileSettingsRect = pwSettingsPage->WindowRect();
			auto aWindowRect = WindowRect();
			LSW_RECT aAdjusted = aGroupRect;
			aAdjusted.right = aAdjusted.left + aFileSettingsRect.right - aSeqRect.left;
			aAdjusted.bottom = aAdjusted.top + lMaxHeight;
			::AdjustWindowRectEx( &aAdjusted, GetStyle(), FALSE, GetStyleEx() );

			::MoveWindow( Wnd(), aWindowRect.left, aWindowRect.top, aAdjusted.Width() + aGroupRect.left * 2, aAdjusted.Height() + aGroupRect.left, TRUE );
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
	CWidget::LSW_HANDLED CWavEditorWindow::Command( WORD /*_wCtrlCode*/, WORD _wId, CWidget * /*_pwSrc*/ ) {
		switch ( _wId ) {
			case Layout::LSN_WEWI_OK : {
				if ( Verify() ) {
					// Save window settings and export.
					Save( m_wewoWindowOptions, &m_weEditor );
					m_poOptions->wewoWavEditorWindow = m_wewoWindowOptions;
					Close();
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

	/**
	 * Verifies each of the dialog contents.
	 *
	 * \return Returns true if no dialog failed verification and there is at least one loaded WAV file.
	 **/
	bool CWavEditorWindow::Verify() {
		// TODO: Check for loaded file.
		CWidget * pwBaddy = nullptr;
		std::wstring wsError;
		if ( (pwBaddy = m_pwefFiles->Verify( wsError )) ) {
			lsw::CBase::MessageBoxError( Wnd(), wsError.c_str(), LSN_LSTR( LSN_ERROR ) );
			pwBaddy->SetFocus();
			return false;
		}
		if ( (pwBaddy = m_pweopOutput->Verify( wsError )) ) {
			lsw::CBase::MessageBoxError( Wnd(), wsError.c_str(), LSN_LSTR( LSN_ERROR ) );
			pwBaddy->SetFocus();
			return false;
		}
		for ( size_t I = 1; I < m_vSequencePages.size(); ++I ) {
			if ( (pwBaddy = m_vSequencePages[I]->Verify( wsError )) ) {
				// TODO: Select file I-1.
				lsw::CBase::MessageBoxError( Wnd(), wsError.c_str(), LSN_LSTR( LSN_ERROR ) );
				pwBaddy->SetFocus();
				return false;
			}
		}

		for ( size_t I = 1; I < m_vSettingsPages.size(); ++I ) {
			if ( (pwBaddy = m_vSettingsPages[I]->Verify( wsError )) ) {
				// TODO: Select file I-1.
				lsw::CBase::MessageBoxError( Wnd(), wsError.c_str(), LSN_LSTR( LSN_ERROR ) );
				pwBaddy->SetFocus();
				return false;
			}
		}


		return true;
	}

	/**
	 * Saves the window state and optionally filled out the structures to use for actually performing the operations.  Verify() SHOULD be called if only filling out the window state, but
	 *	MUST be called if filling out the execution state.
	 * 
	 * \param _wewoWindowState The window state to fill out.
	 * \param _pweEditor The optional execution state to fill out.
	 **/
	void CWavEditorWindow::Save( LSN_WAV_EDITOR_WINDOW_OPTIONS &_wewoWindowState, CWavEditor * _pweEditor ) {
		CWavEditor::LSN_OUTPUT oOutput;
		
		//m_pwefFiles->Save( _wewoWindowState, _pweEditor );
		m_pweopOutput->Save( _wewoWindowState, _pweEditor ? &oOutput : nullptr );
		for ( size_t I = 1; I < m_vSequencePages.size(); ++I ) {
			//m_vSequencePages[I]->Save( _wewoWindowState, _pweEditor );
		}

		for ( size_t I = 1; I < m_vSettingsPages.size(); ++I ) {
			//m_vSettingsPages[I]->Save( _wewoWindowState, _pweEditor );
		}

		if ( _pweEditor ) {
			_pweEditor->SetParms( oOutput );
		}
	}

 }	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

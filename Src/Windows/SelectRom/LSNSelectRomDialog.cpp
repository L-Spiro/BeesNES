#ifdef LSN_USE_WINDOWS

#include "LSNSelectRomDialog.h"
#include "LSNSelectRomDialogLayout.h"
#include <Edit/LSWEdit.h>
#include <ListBox/LSWListBox.h>


namespace lsn {

	CSelectRomDialog::CSelectRomDialog( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pvFiles( reinterpret_cast<const std::vector<std::u16string> *>(_ui64Data) ) {

		
	}

	// == Functions.
	// WM_INITDIALOG.
	CWidget::LSW_HANDLED CSelectRomDialog::InitDialog() {

		CListBox * plvList = static_cast<CListBox *>(FindChild( CSelectRomDialogLayout::LSN_SFI_LISTBOX ));
		if ( plvList && m_pvFiles ) {
			for ( size_t I = 0; I < m_pvFiles->size(); I++ ) {
				INT iIdx = plvList->AddString( reinterpret_cast<LPCWSTR>((*m_pvFiles)[I].c_str()) );
				plvList->SetItemData( iIdx, I );
			}
		}

		return CMainWindow::InitDialog();
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED CSelectRomDialog::Command( WORD _wCtrlCode, WORD _wId, CWidget * _pwSrc ) {
		switch ( _wCtrlCode ) {
			case LBN_DBLCLK : {
				CListBox * plvList = static_cast<CListBox *>(FindChild( CSelectRomDialogLayout::LSN_SFI_LISTBOX ));
				if ( plvList ) {
					if ( plvList->GetCurSel() != LB_ERR ) {
						::EndDialog( Wnd(), plvList->GetCurSelItemData() );
					}
					return LSW_H_HANDLED;
				}
				return LSW_H_HANDLED;;
			}
			case EN_CHANGE : {
				CListBox * plvList = static_cast<CListBox *>(FindChild( CSelectRomDialogLayout::LSN_SFI_LISTBOX ));
				CEdit * peEdit = static_cast<CEdit *>(FindChild( CSelectRomDialogLayout::LSN_SFI_EDIT_SEARCH ));
				if ( peEdit && plvList ) {
					std::wstring wsText = peEdit->GetTextW();
					plvList->ResetContent();
					if ( !wsText.size() ) {
						for ( size_t I = 0; I < m_pvFiles->size(); I++ ) {
							INT iIdx = plvList->AddString( reinterpret_cast<LPCWSTR>((*m_pvFiles)[I].c_str()) );
							plvList->SetItemData( iIdx, I );
						}
					}
					else {
						for ( size_t I = 0; I < m_pvFiles->size(); I++ ) {
							if ( (*m_pvFiles)[I].find( reinterpret_cast<const char16_t *>(wsText.c_str()), 0 ) != std::string::npos ) {
								INT iIdx = plvList->AddString( reinterpret_cast<LPCWSTR>((*m_pvFiles)[I].c_str()) );
								plvList->SetItemData( iIdx, I );
							}
						}
					}
				}
				return LSW_H_HANDLED;;
			}
		}

		switch ( _wId ) {
			case CSelectRomDialogLayout::LSN_SFI_BUTTON_OK : {
				CListBox * plvList = static_cast<CListBox *>(FindChild( CSelectRomDialogLayout::LSN_SFI_LISTBOX ));
				if ( plvList ) {
					if ( plvList->GetCurSel() != LB_ERR ) {
						::EndDialog( Wnd(), plvList->GetCurSelItemData() );
					}
					return LSW_H_HANDLED;
				}
				break;
			}
			case CSelectRomDialogLayout::LSN_SFI_BUTTON_CANCEL : {
				return Close();
			}
		}

		return CMainWindow::Command( _wCtrlCode, _wId, _pwSrc );
	}

	// WM_CLOSE.
	CWidget::LSW_HANDLED CSelectRomDialog::Close() {
		::EndDialog( Wnd(), -1 );
		return LSW_H_HANDLED;
	}

	// WM_GETMINMAXINFO.
	CWidget::LSW_HANDLED CSelectRomDialog::GetMinMaxInfo( MINMAXINFO * _pmmiInfo ) {
		_pmmiInfo->ptMinTrackSize.x = 250;
		_pmmiInfo->ptMinTrackSize.y = 200;
		return LSW_H_HANDLED;
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

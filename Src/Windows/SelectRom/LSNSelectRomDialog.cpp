#ifdef LSN_USE_WINDOWS

#include "LSNSelectRomDialog.h"
#include "LSNSelectRomDialogLayout.h"
#include <ListBox/LSWListBox.h>


namespace lsn {

	CSelectRomDialog::CSelectRomDialog( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pvFiles( reinterpret_cast<const std::vector<std::u16string> *>(_ui64Data) ) {

		
	}

	// == Functions.
	// WM_INITDIALOG.
	CWidget::LSW_HANDLED CSelectRomDialog::InitDialog() {

		CListBox * plvLIst = static_cast<CListBox *>(FindChild( CSelectRomDialogLayout::LSN_SFI_LISTBOX ));
		if ( plvLIst && m_pvFiles ) {
			for ( size_t I = 0; I < m_pvFiles->size(); I++ ) {
				plvLIst->AddString( reinterpret_cast<LPCWSTR>((*m_pvFiles)[I].c_str()) );
			}
		}

		return CMainWindow::InitDialog();
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED CSelectRomDialog::Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc ) {
		switch ( _wCtrlCode ) {
			case LBN_DBLCLK : {
				CListBox * plvLIst = static_cast<CListBox *>(FindChild( CSelectRomDialogLayout::LSN_SFI_LISTBOX ));
				if ( plvLIst ) {
					::EndDialog( Wnd(), plvLIst->GetCurSel() );
					return LSW_H_HANDLED;
				}
				break;
			}
		}

		return CMainWindow::Command( _wCtrlCode, _Id, _pwSrc );
	}

	// WM_CLOSE.
	CWidget::LSW_HANDLED CSelectRomDialog::Close() {
		::EndDialog( Wnd(), -1 );
		return LSW_H_HANDLED;
	}

	// WM_GETMINMAXINFO.
	CWidget::LSW_HANDLED CSelectRomDialog::GetMinMaxInfo( MINMAXINFO * _pmmiInfo ) {
		_pmmiInfo->ptMinTrackSize.x = 250;
		_pmmiInfo->ptMinTrackSize.y = 250;
		return LSW_H_HANDLED;
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

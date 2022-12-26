#ifdef LSN_USE_WINDOWS

#include "LSNLayoutManager.h"
#include "../MainWindow/LSNMainWindow.h"
#include "../SelectRom/LSNSelectRomDialog.h"


namespace lsn {

	// == Functions.
	// Creates a class based on its type.
	CWidget * CLayoutManager::CreateWidget( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) {
		switch ( _wlLayout.ltType ) {
			case LSN_LT_MAIN_WINDOW : {
				return new CMainWindow( _wlLayout, _pwParent,  _bCreateWidget, _hMenu, _ui64Data );
			}
			case LSN_LT_SELECT_ROM_DIALOG : {
				return new CSelectRomDialog( _wlLayout, _pwParent,  _bCreateWidget, _hMenu, _ui64Data );
			}
		}
		return lsw::CLayoutManager::CreateWidget( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data );
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

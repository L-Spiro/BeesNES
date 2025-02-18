#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The layout manager provides the classes for the windows defined in this project.
 */

#include "LSNLayoutManager.h"
#include "../Audio/LSNAudioOptionsWindow.h"
#include "../Input/LSNControllerSetupWindow.h"
#include "../Input/LSNInputConfigurePage.h"
#include "../Input/LSNInputWindow.h"
#include "../Input/LSNStdControllerPage.h"
#include "../MainWindow/LSNMainWindow.h"
#include "../Patch/LSNPatchWindowBottomPage.h"
#include "../Patch/LSNPatchWindow.h"
#include "../Patch/LSNPatchWindowTopPage.h"
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
			case LSN_LT_INPUT_DIALOG : {
				return new CInputWindow( _wlLayout, _pwParent,  _bCreateWidget, _hMenu, _ui64Data );
			}
			case LSN_LT_GLOBAL_INPUT_PAGE : {
				return new CInputConfigurePage<true>( _wlLayout, _pwParent,  _bCreateWidget, _hMenu, _ui64Data );
			}
			case LSN_LT_PER_GAME_INPUT_PAGE : {
				return new CInputConfigurePage<false>( _wlLayout, _pwParent,  _bCreateWidget, _hMenu, _ui64Data );
			}
			case LSN_LT_CONTROLLER_SETUP_DIALOG : {
				return new CControllerSetupWindow( _wlLayout, _pwParent,  _bCreateWidget, _hMenu, _ui64Data );
			}
			case LSN_LT_STD_CONTROLLER_PAGE : {
				return new CStdControllerPage( _wlLayout, _pwParent,  _bCreateWidget, _hMenu, _ui64Data );
			}
			case LSN_LT_AUDIO_OPTIONS_DIALOG : {
				return new CAudioOptionsWindow( _wlLayout, _pwParent,  _bCreateWidget, _hMenu, _ui64Data );
			}
			case LSN_LT_PATCH_DIALOG : {
				return new CPatchWindow( _wlLayout, _pwParent,  _bCreateWidget, _hMenu, _ui64Data );
			}
			case LSN_LT_PATCH_PAGE_TOP : {
				return new CPatchWindowTopPage( _wlLayout, _pwParent,  _bCreateWidget, _hMenu, _ui64Data );
			}
			case LSN_LT_PATCH_PAGE_BOTTOM : {
				return new CPatchWindowBottomPage( _wlLayout, _pwParent,  _bCreateWidget, _hMenu, _ui64Data );
			}
		}
		return lsw::CLayoutManager::CreateWidget( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data );
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

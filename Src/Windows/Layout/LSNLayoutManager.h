#ifdef LSN_USE_WINDOWS

#pragma once

#include <Layout/LSWLayoutManager.h>

using namespace lsw;

namespace lsn {

	enum LSN_LAYOUT_TYPES : DWORD {
		LSN_LT_MAIN_WINDOW					= LSW_LT_CUSTOM,
		LSN_LT_SELECT_ROM_DIALOG,
	};

	class CLayoutManager : public lsw::CLayoutManager {
	public :
		// == Functions.
		// Creates a class based on its type.
		virtual CWidget *				CreateWidget( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data );
	};

}	// namespace ns4

#endif	// #ifdef LSN_USE_WINDOWS

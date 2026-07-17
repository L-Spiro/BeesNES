#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The ROM-selecting dialog.
 */

#pragma once

#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace lsn {
	
	class CSelectRomDialogLayout {
	public :
		// == Functions.
		// Creates the Select ROM dialog.
		static DWORD							CreateSelectRomDialog( CWidget * _pwParent, const std::vector<std::u16string> * _pvFiles );


	protected :
		// == Members.
		// The layout for the Open Process dialog.
		static LSW_WIDGET_LAYOUT				m_wlSelectRomDialog[];

	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

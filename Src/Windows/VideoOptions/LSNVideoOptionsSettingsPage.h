#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A settings page of the Video Options dialog (Global Settings or Per-Game Settings), which holds a tab for each filter.
 */

#pragma once

#include "LSNVideoOptionsWindowLayout.h"

#include <Widget/LSWWidget.h>

#include <memory>
#include <vector>

using namespace lsw;

namespace lsn {

	class													CVideoOptionsPage;

	/**
	 * Class CVideoOptionsSettingsPage
	 * \brief A settings page of the Video Options dialog.
	 *
	 * Description: A settings page of the Video Options dialog (Global Settings or Per-Game Settings), which holds a tab for each filter.
	 */
	class CVideoOptionsSettingsPage : public lsw::CWidget {
	public :
		CVideoOptionsSettingsPage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Functions.
		/**
		 * The WM_INITDIALOG handler.
		 *
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED									InitDialog();

		/**
		 * Reloads every filter page from the options.
		 */
		void												Refresh();


	protected :
		// == Members.
		CVideoOptionsWindowLayout::LSN_PAGE_DATA *			m_ppdData;									/**< The data of the page. */
		std::vector<std::unique_ptr<CVideoOptionsWindowLayout::LSN_PAGE_DATA>>
															m_vPageData;								/**< The data of the filter pages. */
		std::vector<CVideoOptionsPage *>					m_vPages;									/**< The filter pages. */


	private :
		typedef lsw::CWidget								Parent;
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

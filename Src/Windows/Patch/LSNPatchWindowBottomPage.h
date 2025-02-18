#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The bottom part of the patch window.
 */

#pragma once

#include "LSNPatchWindowLayout.h"

#include <Widget/LSWWidget.h>

using namespace lsw;

namespace lsn {
	/**
	 * Class CPatchWindowBottomPage
	 * \brief The bottom part of the patch window.
	 *
	 * Description: The bottom part of the patch window.
	 */
	class CPatchWindowBottomPage : public lsw::CWidget {
	public :
		CPatchWindowBottomPage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		virtual ~CPatchWindowBottomPage();


		// == Functions.
		/**
		 * The WM_INITDIALOG handler.
		 *
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED									InitDialog();

		/**
		 * Handles the WM_COMMAND message.
		 *
		 * \param _wCtrlCode 0 = from menu, 1 = from accelerator, otherwise it is a Control-defined notification code.
		 * \param _wId The ID of the control if _wCtrlCode is not 0 or 1.
		 * \param _pwSrc The source control if _wCtrlCode is not 0 or 1.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED									Command( WORD _wCtrlCode, WORD _wId, CWidget * _pwSrc );


	private :
		typedef CPatchWindowLayout							Layout;
		typedef lsw::CWidget								Parent;
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

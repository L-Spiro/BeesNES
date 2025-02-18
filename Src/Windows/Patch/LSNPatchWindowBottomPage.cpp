#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The top part of the patch window.
 */

#include "LSNPatchWindowBottomPage.h"
#include "../../Localization/LSNLocalization.h"
#include "../../System/LSNSystemBase.h"
#include "../../Windows/Layout/LSNLayoutManager.h"
#include "../WinUtilities/LSNWinUtilities.h"

namespace lsn {

	CPatchWindowBottomPage::CPatchWindowBottomPage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
	}
	CPatchWindowBottomPage::~CPatchWindowBottomPage() {
	}

	// == Functions.
	/**
	 * The WM_INITDIALOG handler.
	 *
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CPatchWindowBottomPage::InitDialog() {
		Parent::InitDialog();
		return LSW_H_CONTINUE;
	}

	/**
	 * Handles the WM_COMMAND message.
	 *
	 * \param _wCtrlCode 0 = from menu, 1 = from accelerator, otherwise it is a Control-defined notification code.
	 * \param _wId The ID of the control if _wCtrlCode is not 0 or 1.
	 * \param _pwSrc The source control if _wCtrlCode is not 0 or 1.
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CPatchWindowBottomPage::Command( WORD /*_wCtrlCode*/, WORD /*_wId*/, CWidget * /*_pwSrc*/ ) {
		
		return LSW_H_CONTINUE;
	}

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

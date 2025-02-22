#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The general audio options and the per-game options.
 */

#pragma once

#include "LSNAudioOptionsWindowLayout.h"

#include <Widget/LSWWidget.h>

using namespace lsw;

namespace lsn {
	/**
	 * Class CAudioOptionsGeneralPage
	 * \brief The general audio options and the per-game options.
	 *
	 * Description: The general audio options and the per-game options.
	 */
	template <bool _bIsGlobal>
	class CAudioOptionsGeneralPage : public lsw::CWidget {
	public :
		CAudioOptionsGeneralPage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 ) :
			Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
			m_poOptions( reinterpret_cast<LSN_OPTIONS *>(_ui64Data) ) {
		}
		virtual ~CAudioOptionsGeneralPage() {
		}


		// == Functions.
		/**
		 * The WM_INITDIALOG handler.
		 *
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED									InitDialog() {
			return Parent::InitDialog();
		}

		/**
		 * Handles the WM_COMMAND message.
		 *
		 * \param _wCtrlCode 0 = from menu, 1 = from accelerator, otherwise it is a Control-defined notification code.
		 * \param _wId The ID of the control if _wCtrlCode is not 0 or 1.
		 * \param _pwSrc The source control if _wCtrlCode is not 0 or 1.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED									Command( WORD _wCtrlCode, WORD _wId, CWidget * _pwSrc ) {
			return Parent::Command( _wCtrlCode, _wId, _pwSrc );
		}


	protected :
		// == Members.
		LSN_OPTIONS *										m_poOptions;												/**< The options object. */


		// == Functions.

	private :
		typedef CAudioOptionsWindowLayout					Layout;
		typedef lsw::CWidget								Parent;

	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

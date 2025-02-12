#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The layout for the audio options dialog.
 */

#pragma once

#include "../../LSNLSpiroNes.h"
#include "../../Options/LSNOptions.h"
#include <Layout/LSWMenuLayout.h>
#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace lsn {
	
	/**
	 * Class CAudioOptionsWindowLayout
	 * \brief The layout for the audio options dialog.
	 *
	 * Description: The layout for the audio options dialog.
	 */
	class CAudioOptionsWindowLayout {
	public :
		// == Enumerations.
		// Control ID's.
		enum LSN_AUDIO_OPTIONS_WINDOW_IDS : WORD {
			LSN_AOWI_NONE,
			LSN_AOWI_MAINWINDOW					= 947,
			LSN_AOWI_TAB,
			LSN_AOWI_OK,
			LSN_AOWI_CANCEL,
		};


		// == Functions.
		/**
		 * Creates the page.
		 *
		 * \param _pwParent the parent of the page.
		 * \param _oOptions A reference to the options object.
		 * \return Returns TRUE if the dialog was created successfully.
		 */
		static BOOL								CreateAudioOptionsDialog( CWidget * _pwParent, LSN_OPTIONS &_oOptions );


	protected :
		// == Members.
		/** The layout for the per-game-setup panel. */
		static LSW_WIDGET_LAYOUT				m_wlPage[];
 		
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

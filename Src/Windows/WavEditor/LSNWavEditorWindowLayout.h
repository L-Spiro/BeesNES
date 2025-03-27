#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The layout for the WAV-Editor dialog.
 */

#pragma once

#include "../../LSNLSpiroNes.h"
#include <Layout/LSWMenuLayout.h>
#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace lsn {
	
	/**
	 * Class CWavEditorWindowLayout
	 * \brief The layout for the WAV-Editor dialog.
	 *
	 * Description: The layout for the WAV-Editor dialog.
	 */
	class CWavEditorWindowLayout {
	public :
		// == Enumerations.
		// Control ID's.
		enum LSN_AUDIO_OPTIONS_WINDOW_IDS : WORD {
			LSN_AOWI_NONE,
			LSN_AOWI_MAINWINDOW					= 10890,
			LSN_AOWI_OK,
			LSN_AOWI_CANCEL,

		};


		// == Functions.
		/**
		 * Creates the WAV-Eeitor window.
		 *
		 * \param _pwParent The parent of the window.
		 * \return Returns the created window.
		 */
		static CWidget *						CreateWavEditorWindow( CWidget * _pwParent );

	protected :
		// == Members.
		/** The layout for main window. */
		static LSW_WIDGET_LAYOUT				m_wlWindow[];


		// == Functions.
 		
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

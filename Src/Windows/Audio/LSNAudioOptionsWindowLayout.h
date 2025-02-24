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


			LSN_AOWI_PAGE_GLOBAL,
			LSN_AOWI_PAGE_PERGAME,

			LSN_AOWI_PAGE_GENERAL_GROUP,
			LSN_AOWI_PAGE_GENERAL_DEVICE_LABEL,
			LSN_AOWI_PAGE_GENERAL_DEVICE_COMBO,
			LSN_AOWI_PAGE_GENERAL_ENABLE_CHECK,

			LSN_AOWI_PAGE_GENERAL_FORMAT_LABEL,
			LSN_AOWI_PAGE_GENERAL_FORMAT_COMBO,
			LSN_AOWI_PAGE_GENERAL_DITHER_CHECK,

			LSN_AOWI_PAGE_GENERAL_VOLUME_LABEL,
			LSN_AOWI_PAGE_GENERAL_VOLUME_TRACKBAR,

			LSN_AOWI_PAGE_GENERAL_BG_VOL_LABEL,
			LSN_AOWI_PAGE_GENERAL_BG_VOL_TRACKBAR,
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

		/**
		 * Creates the global page.
		 *
		 * \param _pwParent The parent widget.
		 * \return Returns the created page.
		 */
		static CWidget *						CreateGlobalPage( CWidget * _pwParent, LSN_OPTIONS &_oOptions );

		/**
		 * Creates the per-game page.
		 *
		 * \param _pwParent The parent widget.
		 * \return Returns the created page.
		 */
		static CWidget *						CreatePerGamePage( CWidget * _pwParent, LSN_OPTIONS &_oOptions );


	protected :
		// == Members.
		/** The layout for the global panel. */
		static LSW_WIDGET_LAYOUT				m_wlGlobalPage[];
		/** The layout for the per-game-setup panel. */
		static LSW_WIDGET_LAYOUT				m_wlPerGamePage[];
		/** The layout for main window. */
		static LSW_WIDGET_LAYOUT				m_wlWindow[];


		// == Functions.
		/**
		 * Creates the pages.
		 *
		 * \param _pwParent The parent widget.
		 * \param _pwlLayout The page layout.
		 * \param _sTotal The number of items to which _pwlLayout points.
		 * \return Returns the created page.
		 */
		static CWidget *						CreatePage( CWidget * _pwParent, const LSW_WIDGET_LAYOUT * _pwlLayout, size_t _sTotal, LSN_OPTIONS &_oOptions );
 		
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

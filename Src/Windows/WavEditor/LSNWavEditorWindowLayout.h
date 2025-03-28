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
		enum LSN_WAV_EDITOR_WINDOW_IDS : WORD {
			LSN_WEWI_NONE,
			LSN_WEWI_MAINWINDOW					= 10890,
			LSN_WEWI_OK,
			LSN_WEWI_CANCEL,

			LSN_WEWI_FILES_GROUP,
			LSN_WEWI_FILES_TREELISTVIEW,
			LSN_WEWI_FILES_ADD_BUTTON,
			LSN_WEWI_FILES_ADD_META_BUTTON,
			LSN_WEWI_FILES_REMOVE_BUTTON,
			LSN_WEWI_FILES_UP_BUTTON,
			LSN_WEWI_FILES_DOWN_BUTTON,

			LSN_WEWI_SEQ,
			LSN_WEWI_SEQ_GROUP,
			LSN_WEWI_SEQ_START_LABEL,
			LSN_WEWI_SEQ_START_COMBO,
			LSN_WEWI_SEQ_START_EDIT,
			LSN_WEWI_SEQ_OPEN_SIL_LABEL,
			LSN_WEWI_SEQ_OPEN_SIL_EDIT,
			LSN_WEWI_SEQ_LOOP_CHECK,
			LSN_WEWI_SEQ_LOOPS_GROUP,
			LSN_WEWI_SEQ_LOOPS_START_LABEL,
			LSN_WEWI_SEQ_LOOPS_START_COMBO,
			LSN_WEWI_SEQ_LOOPS_START_EDIT,
			LSN_WEWI_SEQ_LOOPS_END_LABEL,
			LSN_WEWI_SEQ_LOOPS_END_COMBO,
			LSN_WEWI_SEQ_LOOPS_END_EDIT,
			LSN_WEWI_SEQ_LOOPS_DELAY_LABEL,
			LSN_WEWI_SEQ_LOOPS_DELAY_EDIT,
			LSN_WEWI_SEQ_LOOPS_FADE_LABEL,
			LSN_WEWI_SEQ_LOOPS_FADE_EDIT,
			LSN_WEWI_SEQ_ONE_SHOT_GROUP,
			LSN_WEWI_SEQ_ONE_SHOT_END_LABEL,
			LSN_WEWI_SEQ_ONE_SHOT_END_COMBO,
			LSN_WEWI_SEQ_ONE_SHOT_END_EDIT,
			LSN_WEWI_SEQ_ONE_SHOT_TRAIL_LABEL,
			LSN_WEWI_SEQ_ONE_SHOT_TRAIL_EDIT,
			LSN_WEWI_SEQ_OPS_GROUP,
			LSN_WEWI_SEQ_OPS_TREELISTVEW,
			LSN_WEWI_SEQ_OPS_ADD_BUTTON,
			LSN_WEWI_SEQ_OPS_REMOVE_BUTTON,
			LSN_WEWI_SEQ_OPS_UP_BUTTON,
			LSN_WEWI_SEQ_OPS_DOWN_BUTTON,
		};


		// == Functions.
		/**
		 * Creates the WAV-Eeitor window.
		 *
		 * \param _pwParent The parent of the window.
		 * \return Returns the created window.
		 */
		static CWidget *						CreateWavEditorWindow( CWidget * _pwParent );

		/**
		 * Creates the WAV-Editor sequencer dialog.
		 * 
		 * \param _pwParent The parent of the window.
		 * \param _ui64Options Options to pass to the created dialog.
		 * \return Returns the created widget.
		 **/
		static CWidget *						CreateSequencer( CWidget * _pwParent, uint64_t _ui64Options = 0 );

	protected :
		// == Members.
		/** The layout for the "File Settings" dialog. */
		static LSW_WIDGET_LAYOUT				m_wlFileSettingsDialog[];
		/** The layout for the "Sequencing" dialog. */
		static LSW_WIDGET_LAYOUT				m_wlSequencingDialog[];
		/** The layout for main window. */
		static LSW_WIDGET_LAYOUT				m_wlWindow[];


		// == Functions.
		/**
		 * Creates the pages.
		 *
		 * \param _pwParent The parent widget.
		 * \param _pwlLayout The page layout.
		 * \param _sTotal The number of items to which _pwlLayout points.
		 * \param _ui64Options The parameter to pass to the created control.
		 * \return Returns the created page.
		 */
		static CWidget *						CreatePage( CWidget * _pwParent, const LSW_WIDGET_LAYOUT * _pwlLayout, size_t _sTotal, uint64_t _ui64Options = 0 );
 		
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

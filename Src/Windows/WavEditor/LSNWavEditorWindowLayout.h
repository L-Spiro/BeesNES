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
#include "../../Options/LSNOptions.h"
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
			LSN_WEWI_REBAR0,
			LSN_WEWI_TOOLBAR0,

			LSN_WEWI_SAVE,
			LSN_WEWI_LOAD,

			LSN_WEWI_FILES,
			LSN_WEWI_FILES_GROUP,
			LSN_WEWI_FILES_TREELISTVIEW,
			LSN_WEWI_FILES_ADD_BUTTON,
			LSN_WEWI_FILES_ADD_META_BUTTON,
			LSN_WEWI_FILES_REMOVE_BUTTON,
			LSN_WEWI_FILES_UP_BUTTON,
			LSN_WEWI_FILES_DOWN_BUTTON,

			LSN_WEWI_SEQ,
			LSN_WEWI_SEQ_GROUP,
			LSN_WEWI_SEQ_RANGE_GROUP,
			LSN_WEWI_SEQ_START_LABEL,
			LSN_WEWI_SEQ_START_COMBO,
			LSN_WEWI_SEQ_START_EDIT,

			LSN_WEWI_SEQ_END_LABEL,
			LSN_WEWI_SEQ_END_COMBO,
			LSN_WEWI_SEQ_END_EDIT,

			LSN_WEWI_SEQ_LOOP_RADIO,
			LSN_WEWI_SEQ_ONE_SHOT_RADIO,
			LSN_WEWI_SEQ_FIND_LOOPS_BUTTON,

			LSN_WEWI_SEQ_LOOPS_GROUP,
			LSN_WEWI_SEQ_LOOPS_STOP_LABEL,
			LSN_WEWI_SEQ_LOOPS_STOP_COMBO,
			LSN_WEWI_SEQ_LOOPS_STOP_EDIT,
			LSN_WEWI_SEQ_LOOPS_DELAY_LABEL,
			LSN_WEWI_SEQ_LOOPS_DELAY_EDIT,
			LSN_WEWI_SEQ_LOOPS_DELAY_SECONDS_LABEL,
			LSN_WEWI_SEQ_LOOPS_FADE_LABEL,
			LSN_WEWI_SEQ_LOOPS_FADE_EDIT,
			LSN_WEWI_SEQ_LOOPS_FADE_SECONDS_LABEL,
			LSN_WEWI_SEQ_LOOPS_DESC_LABEL,
			LSN_WEWI_SEQ_LOOPS_WARNING_LABEL,

			LSN_WEWI_SEQ_SILENCE_GROUP,
			LSN_WEWI_SEQ_SILENCE_OPEN_SIL_LABEL,
			LSN_WEWI_SEQ_SILENCE_OPEN_SIL_EDIT,
			LSN_WEWI_SEQ_SILENCE_OPEN_SECONDS_LABEL,
			LSN_WEWI_SEQ_SILENCE_TRAIL_LABEL,
			LSN_WEWI_SEQ_SILENCE_TRAIL_EDIT,
			LSN_WEWI_SEQ_SILENCE_SECONDS_LABEL,

			LSN_WEWI_SEQ_OPS_GROUP,
			LSN_WEWI_SEQ_OPS_TREELISTVEW,
			LSN_WEWI_SEQ_OPS_ADD_BUTTON,
			LSN_WEWI_SEQ_OPS_REMOVE_BUTTON,
			LSN_WEWI_SEQ_OPS_UP_BUTTON,
			LSN_WEWI_SEQ_OPS_DOWN_BUTTON,


			LSN_WEWI_FSETS,
			LSN_WEWI_FSETS_FDATA_GROUP,
			LSN_WEWI_FSETS_FDATA_NAME_LABEL,
			LSN_WEWI_FSETS_FDATA_NAME_EDIT,
			LSN_WEWI_FSETS_FDATA_HZ_LABEL,
			LSN_WEWI_FSETS_FDATA_HZ_COMBO,
			LSN_WEWI_FSETS_FDATA_HZ_EDIT,

			LSN_WEWI_FSETS_CHAR_GROUP,
			LSN_WEWI_FSETS_CHAR_PRESET_LABEL,
			LSN_WEWI_FSETS_CHAR_PRESET_COMBO,
			LSN_WEWI_FSETS_CHAR_SAVE_BUTTON,
			LSN_WEWI_FSETS_CHAR_LOAD_BUTTON,

			LSN_WEWI_FSETS_CHAR_VOL_LABEL,
			LSN_WEWI_FSETS_CHAR_VOL_EDIT,
			LSN_WEWI_FSETS_CHAR_LOCK_CHECK,
			LSN_WEWI_FSETS_CHAR_INV_CHECK,

			LSN_WEWI_FSETS_CHAR_LPF_CHECK,
			LSN_WEWI_FSETS_CHAR_LPF_EDIT,
			LSN_WEWI_FSETS_CHAR_LPF_TYPE_COMBO,
			LSN_WEWI_FSETS_CHAR_LPF_FALLOFF_LABEL,
			LSN_WEWI_FSETS_CHAR_LPF_FALLOFF_EDIT,
			LSN_WEWI_FSETS_CHAR_LPF_DB_LABEL,

			LSN_WEWI_FSETS_CHAR_HPF0_CHECK,
			LSN_WEWI_FSETS_CHAR_HPF0_EDIT,
			LSN_WEWI_FSETS_CHAR_HPF0_TYPE_COMBO,
			LSN_WEWI_FSETS_CHAR_HPF0_FALLOFF_LABEL,
			LSN_WEWI_FSETS_CHAR_HPF0_FALLOFF_EDIT,
			LSN_WEWI_FSETS_CHAR_HPF0_DB_LABEL,

			LSN_WEWI_FSETS_CHAR_HPF1_CHECK,
			LSN_WEWI_FSETS_CHAR_HPF1_EDIT,
			LSN_WEWI_FSETS_CHAR_HPF1_TYPE_COMBO,
			LSN_WEWI_FSETS_CHAR_HPF1_FALLOFF_LABEL,
			LSN_WEWI_FSETS_CHAR_HPF1_FALLOFF_EDIT,
			LSN_WEWI_FSETS_CHAR_HPF1_DB_LABEL,

			LSN_WEWI_FSETS_CHAR_HPF2_CHECK,
			LSN_WEWI_FSETS_CHAR_HPF2_EDIT,
			LSN_WEWI_FSETS_CHAR_HPF2_TYPE_COMBO,
			LSN_WEWI_FSETS_CHAR_HPF2_FALLOFF_LABEL,
			LSN_WEWI_FSETS_CHAR_HPF2_FALLOFF_EDIT,
			LSN_WEWI_FSETS_CHAR_HPF2_DB_LABEL,

			LSN_WEWI_FSETS_MDATA_GROUP,
			LSN_WEWI_FSETS_MDATA_ARTIST_LABEL,
			LSN_WEWI_FSETS_MDATA_ARTIST_EDIT,
			LSN_WEWI_FSETS_MDATA_ALBUM_LABEL,
			LSN_WEWI_FSETS_MDATA_ALBUM_EDIT,
			LSN_WEWI_FSETS_MDATA_YEAR_LABEL,
			LSN_WEWI_FSETS_MDATA_YEAR_EDIT,
			LSN_WEWI_FSETS_MDATA_COMMENTS_LABEL,
			LSN_WEWI_FSETS_MDATA_COMMENTS_EDIT,

			LSN_WEWI_FSETS_APPLY_TO_ALL_BUTTON,

			LSN_WEWI_OUTPUT,
			LSN_WEWI_OUTPUT_GROUP,
			LSN_WEWI_OUTPUT_NOISE_VOLUME_GROUP,
			LSN_WEWI_OUTPUT_MAINS_CHECK,
			LSN_WEWI_OUTPUT_MAINS_COMBO,
			LSN_WEWI_OUTPUT_MAINS_VOL_LABEL,
			LSN_WEWI_OUTPUT_MAINS_VOL_EDIT,
			LSN_WEWI_OUTPUT_NOISE_CHECK,
			LSN_WEWI_OUTPUT_NOISE_COMBO,
			LSN_WEWI_OUTPUT_NOISE_VOL_LABEL,
			LSN_WEWI_OUTPUT_NOISE_VOL_EDIT,

			LSN_WEWI_OUTPUT_MASTER_VOL_GROUP,
			LSN_WEWI_OUTPUT_MASTER_VOL_ABSOLUTE_RADIO,
			LSN_WEWI_OUTPUT_MASTER_VOL_ABSOLUTE_EDIT,
			LSN_WEWI_OUTPUT_MASTER_VOL_NORMALIZE_RADIO,
			LSN_WEWI_OUTPUT_MASTER_VOL_NORMALIZE_EDIT,
			LSN_WEWI_OUTPUT_MASTER_VOL_LOUDNESS_RADIO,
			LSN_WEWI_OUTPUT_MASTER_VOL_LOUDNESS_EDIT,

			LSN_WEWI_OUTPUT_MASTER_FORMAT_GROUP,
			LSN_WEWI_OUTPUT_MASTER_FORMAT_HZ_LABEL,
			LSN_WEWI_OUTPUT_MASTER_FORMAT_HZ_EDIT,
			LSN_WEWI_OUTPUT_MASTER_FORMAT_FORMAT_LABEL,
			LSN_WEWI_OUTPUT_MASTER_FORMAT_FORMAT_COMBO,
			LSN_WEWI_OUTPUT_MASTER_FORMAT_BITS_LABEL,
			LSN_WEWI_OUTPUT_MASTER_FORMAT_BITS_COMBO,
			LSN_WEWI_OUTPUT_MASTER_FORMAT_DITHER_CHECK,
			LSN_WEWI_OUTPUT_MASTER_FORMAT_STEREO_COMBO,

			LSN_WEWI_OUTPUT_MASTER_PATH_GROUP,
			LSN_WEWI_OUTPUT_MASTER_PATH_EDIT,
			LSN_WEWI_OUTPUT_MASTER_PATH_BUTTON,
			LSN_WEWI_OUTPUT_MASTER_NUMBERED_CHECK,


			// == Context Menu == //
			LSN_WEWI_EXPAND_SELECTED,
			LSN_WEWI_EXPAND_ALL,
			LSN_WEWI_COLLAPSE_SELECTED,
			LSN_WEWI_COLLAPSE_ALL,

			LSN_WEWI_SELECT_ALL,
		};


		// == Functions.
		/**
		 * Creates the WAV-Eeitor window.
		 *
		 * \param _pwParent The parent of the window.
		 * \param _oOptions A reference to the system options.
		 * \return Returns the created window.
		 */
		static CWidget *						CreateWavEditorWindow( CWidget * _pwParent, LSN_OPTIONS &_oOptions );

		/**
		 * Creates the WAV-Editor files dialog.
		 * 
		 * \param _pwParent The parent of the window.
		 * \param _wewoOptions Options to pass to the created dialog.
		 * \return Returns the created widget.
		 **/
		static CWidget *						CreateFiles( CWidget * _pwParent, LSN_WAV_EDITOR_WINDOW_OPTIONS & _wewoOptions );

		/**
		 * Creates the WAV-Editor sequencer dialog.
		 * 
		 * \param _pwParent The parent of the window.
		 * \param _wewoOptions Options to pass to the created dialog.
		 * \return Returns the created widget.
		 **/
		static CWidget *						CreateSequencer( CWidget * _pwParent, LSN_WAV_EDITOR_WINDOW_OPTIONS & _wewoOptions );

		/**
		 * Creates the WAV-Editor file settings dialog.
		 * 
		 * \param _pwParent The parent of the window.
		 * \param _wewoOptions Options to pass to the created dialog.
		 * \return Returns the created widget.
		 **/
		static CWidget *						CreateFileSettings( CWidget * _pwParent, LSN_WAV_EDITOR_WINDOW_OPTIONS & _wewoOptions );

		/**
		 * Creates the WAV-Editor output dialog.
		 * 
		 * \param _pwParent The parent of the window.
		 * \param _wewoOptions Options to pass to the created dialog.
		 * \return Returns the created widget.
		 **/
		static CWidget *						CreateOutput( CWidget * _pwParent, LSN_WAV_EDITOR_WINDOW_OPTIONS & _wewoOptions );

	protected :
		// == Members.
		/** The layout for the "Files" dialog. */
		static LSW_WIDGET_LAYOUT				m_wlFilesDialog[];
		/** The layout for the "File Settings" dialog. */
		static LSW_WIDGET_LAYOUT				m_wlFileSettingsDialog[];
		/** The layout for the "Sequencing" dialog. */
		static LSW_WIDGET_LAYOUT				m_wlSequencingDialog[];
		/** The layout for the "Output" dialog. */
		static LSW_WIDGET_LAYOUT				m_wlOutputDialog[];
		/** The layout for main window. */
		static LSW_WIDGET_LAYOUT				m_wlWindow[];


		// == Functions.
		/**
		 * Creates the pages.
		 *
		 * \param _pwParent The parent widget.
		 * \param _pwlLayout The page layout.
		 * \param _sTotal The number of items to which _pwlLayout points.
		 * \param _wewoOptions The parameter to pass to the created control.
		 * \return Returns the created page.
		 */
		static CWidget *						CreatePage( CWidget * _pwParent, const LSW_WIDGET_LAYOUT * _pwlLayout, size_t _sTotal, LSN_WAV_EDITOR_WINDOW_OPTIONS & _wewoOptions );
 		
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

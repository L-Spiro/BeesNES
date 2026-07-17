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

#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The page for sequencing settings.
 */

#pragma once

#include "LSNWavEditorWindowLayout.h"
#include "../../Options/LSNWavEditorWindowOptions.h"
#include "../../Wav/LSNWavEditor.h"

#include <MainWindow/LSWMainWindow.h>

using namespace lsw;

namespace lsn {

	class CWavEditorFilesPage;

	/**
	 * Class CWavEditorSequencingPage
	 * \brief The page for sequencing settings.
	 *
	 * Description: The page for sequencing settings.
	 */
	class CWavEditorSequencingPage : public lsw::CWidget {
	public :
		CWavEditorSequencingPage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Functions.
		/**
		 * The WM_INITDIALOG handler.
		 *
		 * \return Returns an LSW_HANDLED code.
		 */
		LSW_HANDLED											InitDialog();

		/**
		 * Sets the WAV Editor object and its unique ID.
		 * 
		 * \param _weEditor A reference to the WAV Editor object to which to associate this page.
		 * \param _ui32Id The unique ID to associate with this page.
		 * \param _pwefpFiles The files control, for accessing selected items.
		 **/
		void												SetWavEditorAndId( CWavEditor &_weEditor, uint32_t _ui32Id, CWavEditorFilesPage * _pwefpFiles ) {
			m_pweEditor = &_weEditor;
			m_ui32Id = _ui32Id;
			m_pwefpFiles = _pwefpFiles;
			FillCombos();
			Update();
		}

		/**
		 * Gets the unique ID assigned to this window.
		 * 
		 * \return Returns the unique ID assigned to this window.
		 **/
		uint32_t											UniqueId() const { return m_ui32Id; }

		/**
		 * Handles the WM_COMMAND message.
		 *
		 * \param _wCtrlCode 0 = from menu, 1 = from accelerator, otherwise it is a Control-defined notification code.
		 * \param _wId The ID of the control if _wCtrlCode is not 0 or 1.
		 * \param _pwSrc The source control if _wCtrlCode is not 0 or 1.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED									Command( WORD _wCtrlCode, WORD _wId, CWidget * _pwSrc );

		/**
		 * Verifies the inputs.
		 * 
		 * \param _wsMsg The error message to display.
		 * \return Returns the control that failed or nullptr.
		 **/
		CWidget *											Verify( std::wstring &_wsMsg );

		/**
		 * Saves the current input configuration and closes the dialog.
		 */
		void												Save();

		/**
		 * Updates the dialog.
		 **/
		void												Update();

		/**
		 * Fills combo boxes with metadata or removes metadata from them.
		 **/
		void												FillCombos();


	protected :
		// == Members.
		/** The WAV Editor object. */
		CWavEditor *										m_pweEditor = nullptr;
		/** The ID of the WAV set associated with this page. */
		uint32_t											m_ui32Id = 0;
		/** The options object. */
		LSN_WAV_EDITOR_WINDOW_OPTIONS *						m_pwewoOptions;
		/** The files page. */
		CWavEditorFilesPage *								m_pwefpFiles = nullptr;


		// == Functions.
		/**
		 * Fills a combo box with metadata.
		 * 
		 * \param _pwCombo The combo box to fill.
		 * \param _vMetadata The metadata to use to fill the combo box.
		 * \return Returns true if _pwCombo is a valid pointer to a combo box and _vMetadata is not empty.
		 **/
		bool												FillComboWithMetadata( CWidget * _pwCombo, const std::vector<CWavEditor::LSN_METADATA> &_vMetadata );

		/**
		 * Gets an array of pages to update on text-editing for the 0th page.
		 * 
		 * \param _vPages Holds the returned array of pages to update.
		 **/
		void												GetPagesToUpdate( std::vector<LPARAM> &_vPages );

	private :
		typedef CWavEditorWindowLayout						Layout;
		typedef lsw::CWidget								Parent;
	};


}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

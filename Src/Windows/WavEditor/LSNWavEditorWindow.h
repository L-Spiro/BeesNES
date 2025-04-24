#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The WAV-Editor window.
 */

#pragma once

#include "../../Options/LSNOptions.h"
#include "../../Wav/LSNWavEditor.h"
#include "LSNWavEditorFileSettingsPage.h"
#include "LSNWavEditorFilesPage.h"
#include "LSNWavEditorSequencingPage.h"
#include "LSNWavEditorOutputPage.h"
#include "LSNWavEditorWindowLayout.h"

#include <MainWindow/LSWMainWindow.h>

using namespace lsw;

namespace lsn {

	class CWavEditorWindow : public lsw::CMainWindow {
	public :
		CWavEditorWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		~CWavEditorWindow();


		// == Functions.
		/**
		 * The WM_INITDIALOG handler.
		 *
		 * \return Returns an LSW_HANDLED code.
		 */
		LSW_HANDLED											InitDialog();

		/**
		 * The WM_CLOSE handler.
		 *
		 * \return Returns an LSW_HANDLED code.
		 */
		LSW_HANDLED											Close();

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
		 * Handles the WM_GETMINMAXINFO message.
		 * 
		 * \param _pmmiInfo The min/max info structure to fill out.
		 * \return Returns an LSW_HANDLED code.
		 **/
		virtual LSW_HANDLED									GetMinMaxInfo( MINMAXINFO * _pmmiInfo );

		/**
		 * Load a .WAV file via the Add .WAV Files button.
		 * 
		 * \param _wsPath The path to the WAV file to load.  Spliced WAV files and associated metadata are also automatically loaded.
		 * \return Returns true if all given files are valid (exist and of the appropriate type).
		 **/
		virtual bool										AddWavFiles( const std::wstring &_wsPath );

		/**
		 * Removes files by unique ID.
		 * 
		 * \param _vIds The array of unique ID's to remove.
		 **/
		void												Remove( const std::vector<LPARAM> &_vIds );

		/**
		 * Moves files up 1 by unique ID.
		 * 
		 * \param _vIds The array of unique ID's to move.
		 **/
		void												MoveUp( const std::vector<LPARAM> &_vIds );

		/**
		 * Moves files down 1 by unique ID.
		 * 
		 * \param _vIds The array of unique ID's to move.
		 **/
		void												MoveDown( const std::vector<LPARAM> &_vIds );

		/**
		 * Indicates that the file selection has changed.
		 * 
		 * \param _vIds The now-selected ID's.
		 **/
		void												SelectionChanged( const std::vector<LPARAM> &_vIds );

		/**
		 * Sets the texts of all sequencer edits of the given ID.
		 * 
		 * \param _wId The ID of the edits to update.
		 * \param _wsText The text to apply.
		 * \param _vUpdateMe The unique ID's of the pages to modify.
		 **/
		void												SetAllSeqEditTexts( WORD _wId, const std::wstring &_wsText, const std::vector<LPARAM> &_vUpdateMe );

		/**
		 * Sets the check state of all sequencer checks/radio of the given ID.
		 * 
		 * \param _wId The ID of the edits to update.
		 * \param _bChecked to check or not.
		 * \param _vUpdateMe The unique ID's of the pages to modify.
		 **/
		void												SetAllSeqCheckStates( WORD _wId, bool _bChecked, const std::vector<LPARAM> &_vUpdateMe );

		/**
		 * Sets the texts of all settings edits of the given ID.
		 * 
		 * \param _wId The ID of the edits to update.
		 * \param _wsText The text to apply.
		 * \param _vUpdateMe The unique ID's of the pages to modify.
		 **/
		void												SetAllSettingsEditTexts( WORD _wId, const std::wstring &_wsText, const std::vector<LPARAM> &_vUpdateMe );

		/**
		 * Sets the check state of all settings checks/radio of the given ID.
		 * 
		 * \param _wId The ID of the edits to update.
		 * \param _bChecked to check or not.
		 * \param _vUpdateMe The unique ID's of the pages to modify.
		 **/
		void												SetAllSettingsCheckStates( WORD _wId, bool _bChecked, const std::vector<LPARAM> &_vUpdateMe );

		/**
		 * Sets the combo selections of all settings combos of the given ID.
		 * 
		 * \param _wId The ID of the combos to update.
		 * \param _lpSelection The data to select within each combo box.
		 * \param _vUpdateMe The unique ID's of the pages to modify.
		 * \param _lpBadSel In the case of conflicting data selections, this selection is returned.
		 **/
		void												SetAllSettingsComboSels( WORD _wId, LPARAM _lpSelection, const std::vector<LPARAM> &_vUpdateMe );

		/**
		 * Gets the text from all edits with the given ID on each of the pages specified by _vPages.  If the text values do not match exactly, the return string is empty.
		 * 
		 * \param _wId The ID of the edit controls to inspect on each page.
		 * \param _vPages The pages to inspect for a string.
		 * \return Returns the string to set based on the contents of each edit in each page.
		 **/
		std::wstring										GetAllSeqEditTexts( WORD _wId, const std::vector<LPARAM> &_vPages );

		/**
		 * Gets the check state derived from the check states of each check/radio across all pages.  If any check/radio is checked, true is returned.
		 * 
		 * \param _wId The ID of the edit controls to inspect on each page.
		 * \param _vPages The pages to inspect for a string.
		 * \return Returns the check state to use based on the check states of all involved checks/radios across each page.
		 **/
		bool												GetAllSeqCheckStates( WORD _wId, const std::vector<LPARAM> &_vPages );

		/**
		 * Gets the text from all edits with the given ID on each of the pages specified by _vPages.  If the text values do not match exactly, the return string is empty.
		 * 
		 * \param _wId The ID of the edit controls to inspect on each page.
		 * \param _vPages The pages to inspect for a string.
		 * \return Returns the string to set based on the contents of each edit in each page.
		 **/
		std::wstring										GetAllSettingsEditTexts( WORD _wId, const std::vector<LPARAM> &_vPages );

		/**
		 * Gets the check state derived from the check states of each check/radio across all pages.
		 * 
		 * \param _wId The ID of the edit controls to inspect on each page.
		 * \param _vPages The pages to inspect for a string.
		 * \return Returns the check state to use based on the check states of all involved checks/radios across each page.
		 **/
		int													GetAllSettingsCheckStates( WORD _wId, const std::vector<LPARAM> &_vPages );

		/**
		 * Gets the selected item of each combobox across all pages.  If they are the same, the value is returned, otherwise _lpBad is returned.
		 * 
		 * \param _wId The ID of the combobox controls to inspect on each page.
		 * \param _vPages The pages to inspect for a string.
		 * \param _lpBad The value to return upon inconsistent combobox selections.
		 * \return Returns the combobox data selection if all combos match, otherwise _lpBad.
		 **/
		LPARAM												GetAllSettingsComboSel( WORD _wId, const std::vector<LPARAM> &_vPages, LPARAM _lpBad );

		/**
		 * Updates the window.
		 * 
		 * \param _bSelchanged If true, thew newly focused sequencer page will receive a notification to tell it that it has just become active, which it will use
		 *	to gather the text from all the pages it affects to decide how to fill in its own edit texts.
		 **/
		void												Update( bool _bSelchanged = false );

		/**
		 * Prepares to create the window.  Creates the atom if necessary.
		 **/
		static void											PrepareWavEditor();


	public :
		// == Members.
		/** The WAV Editor object. */
		CWavEditor											m_weEditor;
		/** The sequencing dialogs. */
		std::vector<CWavEditorSequencingPage *>				m_vSequencePages;
		/** The settings dialogs. */
		std::vector<CWavEditorFileSettingsPage *>			m_vSettingsPages;
		/** The files page. */
		CWavEditorFilesPage *								m_pwefFiles = nullptr;
		/** The output page. */
		CWavEditorOutputPage *								m_pweopOutput = nullptr;
		/** A copy of the window options. */
		LSN_WAV_EDITOR_WINDOW_OPTIONS						m_wewoWindowOptions;
		/** A reference to the system options. */
		LSN_OPTIONS *										m_poOptions;
		/** Toggled when done initializing. */
		bool												m_bInit = false;
		/** The Sequencing rectangle. */
		LSW_RECT											m_rSeqRect;
		/** The settings rectangle. */
		LSW_RECT											m_rSetRect;
		/** The main window class. */
		static ATOM											m_aAtom;


		// == Functions.
		/**
		 * Verifies each of the dialog contents.
		 *
		 * \return Returns true if no dialog failed verification and there is at least one loaded WAV file.
		 **/
		bool												Verify();

		/**
		 * Saves the window state and optionally filled out the structures to use for actually performing the operations.  Verify() SHOULD be called if only filling out the window state, but
		 *	MUST be called if filling out the execution state.
		 * 
		 * \param _wewoWindowState The window state to fill out.
		 * \param _pweEditor The optional execution state to fill out.
		 * \return Returns true if the files were all created successfully.
		 **/
		bool												Save( LSN_WAV_EDITOR_WINDOW_OPTIONS &_wewoWindowState, CWavEditor * _pweEditor );


	private :
		typedef CWavEditorWindowLayout						Layout;

	};

 }	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS
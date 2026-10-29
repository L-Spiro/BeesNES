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

#include <ImageList/LSWImageList.h>
#include <Images/LSWBitmap.h>
#include <MainWindow/LSWMainWindow.h>

#include <map>

using namespace lsw;

namespace lsn {

	class CWavEditorWindow : public lsw::CMainWindow {
	public :
		CWavEditorWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		~CWavEditorWindow();


		// == Enumerations.
		// Images.
		enum LSN_IMAGES {
			LSN_I_SAVE,
			LSN_I_LOAD,
			LSN_I_EXIT,

			LSN_I_ADD_WAV,
			LSN_I_REM_WAV,
			LSN_I_UP_WAV,
			LSN_I_DOWN_WAV,

			LSN_I_TOTAL
		};


		// == Types.
		/** The values of the sequencing and file-settings controls for a single track. */
		struct LSN_TRACK {
			/** The values of the sequencing controls. */
			CWavEditorSequencingPage::LSN_TRACK				tSequencing;
			/** The values of the file-settings controls. */
			CWavEditorFileSettingsPage::LSN_TRACK			tSettings;
		};


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
		 * \param _ui32Id The ID of the already-loaded file data.  If 0, the file will be added to the tree.
		 * \return Returns true if all given files are valid (exist and of the appropriate type).
		 **/
		virtual bool										AddWavFiles( const std::wstring &_wsPath, uint32_t _ui32Id = 0 );

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
		 * Sets the stored sequencer edit text of the given ID in each given track.
		 * 
		 * \param _wId The ID of the edits to update.
		 * \param _wsText The text to apply.
		 * \param _vUpdateMe The unique ID's of the tracks to modify.
		 **/
		void												SetAllSeqEditTexts( WORD _wId, const std::wstring &_wsText, const std::vector<LPARAM> &_vUpdateMe );

		/**
		 * Sets the stored sequencer check/radio state of the given ID in each given track.
		 * 
		 * \param _wId The ID of the checks/radios to update.
		 * \param _bChecked to check or not.
		 * \param _vUpdateMe The unique ID's of the tracks to modify.
		 **/
		void												SetAllSeqCheckStates( WORD _wId, bool _bChecked, const std::vector<LPARAM> &_vUpdateMe );

		/**
		 * Sets the stored sequencer combo-box selection of the given ID in each given track.
		 * 
		 * \param _wId The ID of the combos to update.
		 * \param _i32Selection The index of the item to select within each combo box.
		 * \param _vUpdateMe The unique ID's of the tracks to modify.
		 **/
		void												SetAllSeqComboSels( WORD _wId, int32_t _i32Selection, const std::vector<LPARAM> &_vUpdateMe );

		/**
		 * Sets the stored settings edit text of the given ID in each given track.
		 * 
		 * \param _wId The ID of the edits to update.
		 * \param _wsText The text to apply.
		 * \param _vUpdateMe The unique ID's of the tracks to modify.
		 **/
		void												SetAllSettingsEditTexts( WORD _wId, const std::wstring &_wsText, const std::vector<LPARAM> &_vUpdateMe );

		/**
		 * Sets the stored settings check state of the given ID in each given track.
		 * 
		 * \param _wId The ID of the checks to update.
		 * \param _bChecked to check or not.
		 * \param _vUpdateMe The unique ID's of the tracks to modify.
		 **/
		void												SetAllSettingsCheckStates( WORD _wId, bool _bChecked, const std::vector<LPARAM> &_vUpdateMe );

		/**
		 * Sets the stored settings combo-box selection of the given ID in each given track.
		 * 
		 * \param _wId The ID of the combos to update.
		 * \param _i32Selection The index of the item to select within each combo box.
		 * \param _vUpdateMe The unique ID's of the tracks to modify.
		 **/
		void												SetAllSettingsComboSels( WORD _wId, int32_t _i32Selection, const std::vector<LPARAM> &_vUpdateMe );

		/**
		 * Gets the stored sequencer edit text of the given ID from each of the tracks specified by _vPages.  If the text values do not match exactly, the return string is empty.
		 * 
		 * \param _wId The ID of the edit control.
		 * \param _vPages The tracks to inspect for a string.
		 * \return Returns the string to set based on the stored text of each track.
		 **/
		std::wstring										GetAllSeqEditTexts( WORD _wId, const std::vector<LPARAM> &_vPages );

		/**
		 * Gets the check state derived from the stored sequencer check/radio states of the given ID in each of the tracks specified by _vPages.  If any check/radio is checked, true is returned.
		 * 
		 * \param _wId The ID of the check/radio.
		 * \param _vPages The tracks to inspect.
		 * \return Returns the check state to use based on the stored check states of all involved tracks.
		 **/
		bool												GetAllSeqCheckStates( WORD _wId, const std::vector<LPARAM> &_vPages );

		/**
		 * Gets the stored settings edit text of the given ID from each of the tracks specified by _vPages.  If the text values do not match exactly, the return string is empty.
		 * 
		 * \param _wId The ID of the edit control.
		 * \param _vPages The tracks to inspect for a string.
		 * \return Returns the string to set based on the stored text of each track.
		 **/
		std::wstring										GetAllSettingsEditTexts( WORD _wId, const std::vector<LPARAM> &_vPages );

		/**
		 * Gets the check state derived from the stored settings check states of the given ID in each of the tracks specified by _vPages.
		 * 
		 * \param _wId The ID of the check.
		 * \param _vPages The tracks to inspect.
		 * \return Returns BST_CHECKED or BST_UNCHECKED if all tracks match, otherwise BST_INDETERMINATE.
		 **/
		int													GetAllSettingsCheckStates( WORD _wId, const std::vector<LPARAM> &_vPages );

		/**
		 * Gets the stored settings combo-box selection of the given ID in each of the tracks specified by _vPages.  If they are the same, the selection is returned, otherwise -1 is returned.
		 * 
		 * \param _wId The ID of the combo box.
		 * \param _vPages The tracks to inspect.
		 * \return Returns the index of the selection if all tracks match, otherwise -1.
		 **/
		int32_t												GetAllSettingsComboSel( WORD _wId, const std::vector<LPARAM> &_vPages );

		/**
		 * Updates the window.
		 * 
		 * \param _bSelchanged If true, the controls are refilled with the values of the selected track, or with the combined values of the tracks
		 *	being edited together if there is not exactly 1 selected track.
		 **/
		void												Update( bool _bSelchanged = false );

		/**
		 * Virtual client rectangle.  Can be used for things that need to be adjusted based on whether or not status bars, toolbars, etc. are present.
		 * 
		 * \param _pwChild The child whose virtual client is to be obtained or nullptr to obtain this window's.
		 * \return Returns the selected virtual client.
		 **/
		const LSW_RECT										VirtualClientRect( const CWidget * _pwChild ) const;

		/**
		 * Saves the project.
		 **/
		void												SaveProject();

		/**
		 * Loads a project.
		 **/
		void												LoadProject();

		/**
		 * Prepares to create the window.  Creates the atom if necessary.
		 **/
		static void											PrepareWavEditor();


	public :
		// == Members.
		/** The WAV Editor object. */
		CWavEditor											m_weEditor;
		/** The sequencing dialog. */
		CWavEditorSequencingPage *							m_pwespSequencing = nullptr;
		/** The settings dialog. */
		CWavEditorFileSettingsPage *						m_pwefspSettings = nullptr;
		/** The values of the sequencing and settings controls for each track, by unique ID. */
		std::map<uint32_t, LSN_TRACK>						m_mTracks;
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
		/** the image list for the toolbar. */
		CImageList											m_iImages;
		/** Images for the toolbar. */
		CBitmap												m_bBitmaps[LSN_I_TOTAL];
		/** Toolbar image mapping. */
		INT													m_iImageMap[LSN_I_TOTAL];
		/** The main window class. */
		static ATOM											m_aAtom;


		// == Functions.
		/**
		 * Verifies each of the dialog contents.
		 *
		 * \param _bSilent If true, errors are not reported.
		 * \return Returns true if no dialog failed verification and there is at least one loaded WAV file.
		 **/
		bool												Verify( bool _bSilent );

		/**
		 * Saves the window state and optionally filled out the structures to use for actually performing the operations.  Verify() SHOULD be called if only filling out the window state, but
		 *	MUST be called if filling out the execution state.
		 * 
		 * \param _wewoWindowState The window state to fill out.
		 * \param _pweEditor The optional execution state to fill out.
		 * \return Returns true if the files were all created successfully.
		 **/
		bool												Save( LSN_WAV_EDITOR_WINDOW_OPTIONS &_wewoWindowState, CWavEditor * _pweEditor );

		/**
		 * Saves the project.
		 * 
		 * \param _wewoWindowState The window state to fill out.
		 * \return Returns true if there are no weird memory errors.
		 **/
		bool												SaveProject( LSN_WAV_EDITOR_WINDOW_OPTIONS &_wewoWindowState );

		/**
		 * Finds the stored control values of a track by unique ID.
		 * 
		 * \param _ui32Id The ID of the track to find.
		 * \return Returns a pointer to the desired track or nullptr;
		 **/
		LSN_TRACK *											TrackById( uint32_t _ui32Id ) {
			auto aFound = m_mTracks.find( _ui32Id );
			return aFound == m_mTracks.end() ? nullptr : &aFound->second;
		}


	private :
		typedef CWinUtilities								Layout;

	};

 }	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS
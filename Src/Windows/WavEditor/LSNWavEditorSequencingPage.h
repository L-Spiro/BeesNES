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
#include "../WinUtilities/LSNWinUtilities.h"

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


		// == Types.
		/** The values of this page's controls for a single track. */
		struct LSN_TRACK {
			/** The Start Time text. */
			std::wstring									wsStartTime = L"0";
			/** The Stop Time text. */
			std::wstring									wsStopTime;
			/** The Pre-Fade Duration text. */
			std::wstring									wsPreFadeDur;
			/** The Fade Duration text. */
			std::wstring									wsFadeDur;
			/** The Opening Silence text. */
			std::wstring									wsOpeningSilence;
			/** The Trailing Silence text. */
			std::wstring									wsTrailingSilence;
			/** The index of the selected item in the Start Time combo box. */
			int32_t											i32StartSelection = -1;
			/** The index of the selected item in the Stop Time combo box. */
			int32_t											i32StopSelection = -1;
			/** The Start Time Exact radio. */
			bool											bStartExact = true;
			/** The Start Time Minus One radio. */
			bool											bStartMinusOne = false;
			/** The Start Time Snap radio. */
			bool											bStartSnap = false;
			/** The Stop Time Exact radio. */
			bool											bStopExact = true;
			/** The Stop Time Minus One radio. */
			bool											bStopMinusOne = false;
			/** The Stop Time Snap radio. */
			bool											bStopSnap = false;
			/** The Loop radio. */
			bool											bLoop = false;
			/** The One-Shot radio. */
			bool											bOneShot = true;


			// == Functions.
			/**
			 * Gets the stored text of an edit control.
			 * 
			 * \param _wId The ID of the edit control.
			 * \return Returns a pointer to the stored text of the given edit control or nullptr if _wId is not an edit control on this page.
			 **/
			std::wstring *									Text( WORD _wId );

			/**
			 * Gets the stored text of an edit control.
			 * 
			 * \param _wId The ID of the edit control.
			 * \return Returns a pointer to the stored text of the given edit control or nullptr if _wId is not an edit control on this page.
			 **/
			const std::wstring *							Text( WORD _wId ) const { return const_cast<LSN_TRACK *>(this)->Text( _wId ); }

			/**
			 * Gets the stored selection index of a combo box.
			 * 
			 * \param _wId The ID of the combo box.
			 * \return Returns a pointer to the stored selection index of the given combo box or nullptr if _wId is not a combo box on this page.
			 **/
			int32_t *										Selection( WORD _wId );

			/**
			 * Gets the stored selection index of a combo box.
			 * 
			 * \param _wId The ID of the combo box.
			 * \return Returns a pointer to the stored selection index of the given combo box or nullptr if _wId is not a combo box on this page.
			 **/
			const int32_t *									Selection( WORD _wId ) const { return const_cast<LSN_TRACK *>(this)->Selection( _wId ); }

			/**
			 * Gets the stored check state of a check box or radio button.
			 * 
			 * \param _wId The ID of the check box or radio button.
			 * \return Returns a pointer to the stored check state of the given control or nullptr if _wId is not a check box or radio button on this page.
			 **/
			bool *											Check( WORD _wId );

			/**
			 * Gets the stored check state of a check box or radio button.
			 * 
			 * \param _wId The ID of the check box or radio button.
			 * \return Returns a pointer to the stored check state of the given control or nullptr if _wId is not a check box or radio button on this page.
			 **/
			const bool *									Check( WORD _wId ) const { return const_cast<LSN_TRACK *>(this)->Check( _wId ); }
		};


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
		 * Informs the page that the metadata of a WAV set was replaced.  The metadata combo boxes are refilled if they show that WAV set.
		 * 
		 * \param _ui32Id The unique ID of the WAV set whose metadata changed.
		 **/
		void												MetadataChanged( uint32_t _ui32Id ) {
			if ( m_ui32MetaId == _ui32Id ) { m_ui32MetaId = 0; }
			if ( m_ui32Id == _ui32Id ) {
				FillCombos();
				Update();
			}
		}

		/**
		 * Gets the text shown for a metadata entry in the metadata combo boxes.
		 * 
		 * \param _vMetadata The metadata.
		 * \param _sIdx The index of the entry in _vMetadata.
		 * \return Returns the text to show for the entry.
		 **/
		static std::string									MetadataText( const std::vector<CWavEditor::LSN_METADATA> &_vMetadata, size_t _sIdx );


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
		 * Handles WM_DRAWITEM.  Draws the items of the owner-drawn metadata combo boxes from the WAV set's metadata.
		 *
		 * \param _wControlId The control identifier (DRAWITEMSTRUCT::CtlID).
		 * \param _disItem The DRAWITEMSTRUCT describing what to draw.
		 * \return Returns LSW_H_HANDLED if the item was drawn.
		 */
		virtual LSW_HANDLED									DrawItem( WORD _wControlId, const DRAWITEMSTRUCT &_disItem );

		/**
		 * Verifies the values of a track.
		 * 
		 * \param _ui32Id The unique ID of the track.
		 * \param _tTrack The values to verify.
		 * \param _wsMsg The error message to display.
		 * \return Returns the control that failed or nullptr.
		 **/
		CWidget *											Verify( uint32_t _ui32Id, const LSN_TRACK &_tTrack, std::wstring &_wsMsg );

		/**
		 * Saves the values of a track.
		 * 
		 * \param _tTrack The values to save.
		 * \param _pfPerFileOptions The object to which to save the window state.
		 * \param _ppfOutput The output object to which to transfer all the window settings.
		 */
		void												Save( const LSN_TRACK &_tTrack, LSN_WAV_EDITOR_WINDOW_OPTIONS::LSN_PER_FILE &_pfPerFileOptions, CWavEditor::LSN_PER_FILE * _ppfOutput );

		/**
		 * Fills out the values of a track with saved values from a project.
		 * 
		 * \param _pfPerFileOptions The per-file settings to load.
		 * \param _tTrack The track values to fill out.
		 **/
		void												Load( const LSN_WAV_EDITOR_WINDOW_OPTIONS::LSN_PER_FILE &_pfPerFileOptions, LSN_TRACK &_tTrack );

		/**
		 * Sets the values of a newly added track that depend on its WAV file (its start time comes from its metadata).
		 * 
		 * \param _ui32Id The unique ID of the track.
		 * \param _tTrack The track values to fill out.
		 **/
		void												InitTrack( uint32_t _ui32Id, LSN_TRACK &_tTrack );

		/**
		 * Shows the values of a track in the controls.
		 * 
		 * \param _ui32Id The unique ID of the track.
		 * \param _tTrack The values to show.
		 **/
		void												LoadTrack( uint32_t _ui32Id, const LSN_TRACK &_tTrack );

		/**
		 * Copies the values in the controls to a track structure.
		 * 
		 * \param _tTrack Holds the values from the controls.
		 **/
		void												SaveTrack( LSN_TRACK &_tTrack );

		/**
		 * Updates the dialog.
		 **/
		void												Update();

		/**
		 * Shows the combined values of all the tracks being edited together (the selected tracks, or all tracks if none are selected).
		 **/
		void												Activate();

		/**
		 * Fills combo boxes with metadata or removes metadata from them.
		 **/
		void												FillCombos();

		/**
		 * Opens the Find Loop Points dialog for the shown track and applies the chosen ending point to the stopping-point controls.
		 **/
		void												FindLoops();


	protected :
		// == Members.
		/** The WAV Editor object. */
		CWavEditor *										m_pweEditor = nullptr;
		/** The ID of the WAV set whose values are shown, or 0 if showing the combined values of multiple tracks. */
		uint32_t											m_ui32Id = 0;
		/** The ID of the WAV set whose metadata is in the metadata combo boxes. */
		uint32_t											m_ui32MetaId = 0;
		/** The options object. */
		LSN_WAV_EDITOR_WINDOW_OPTIONS *						m_pwewoOptions;
		/** The files page. */
		CWavEditorFilesPage *								m_pwefpFiles = nullptr;
		/** Settings edits and checks internally. */
		bool												m_bInternalUpdate = false;
		/** The edit controls whose texts are stored per track. */
		static const WORD									m_wEditIds[];
		/** The combo boxes whose selections are stored per track. */
		static const WORD									m_wComboIds[];
		/** The check boxes and radio buttons whose check states are stored per track. */
		static const WORD									m_wCheckIds[];
		/** The characters typed so far into a metadata combo box (type-ahead selection). */
		std::string											m_sTypeAhead;
		/** The time (from ::GetTickCount64()) of the last character typed into a metadata combo box. */
		uint64_t											m_ui64TypeAheadTime = 0;
		/** The ID of the metadata combo box into which m_sTypeAhead was typed. */
		WORD												m_wTypeAheadId = 0;


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
		 * Gets the unique ID's of the tracks to which changes in the controls are applied.  This is the shown track or else the selected tracks (or
		 *	all tracks if none are selected).
		 * 
		 * \param _vPages Holds the returned array of track ID's to update.
		 **/
		void												GetPagesToUpdate( std::vector<LPARAM> &_vPages );

		/**
		 * Selects the first item in a metadata combo box whose text begins with the characters typed so far.  The combo boxes are owner-drawn
		 *	without strings, so Windows cannot do this itself.  Typing the same single character again moves to the next item that begins with it.
		 * 
		 * \param _wId The ID of the combo box.
		 * \param _wcChar The typed character.
		 **/
		void												TypeAhead( WORD _wId, wchar_t _wcChar );

		/**
		 * Determines whether the text of a metadata combo-box item begins with the given characters.  The index part is compared both with and
		 *	without its leading zeros, and the rest of the text is compared without regard to case.
		 * 
		 * \param _vMetadata The metadata.
		 * \param _sIdx The index of the entry in _vMetadata.
		 * \param _sDigits The number of digits to which indices are padded.
		 * \param _sPrefix The characters typed so far.
		 * \return Returns true if the text of the item begins with _sPrefix.
		 **/
		static bool											TypeAheadMatches( const std::vector<CWavEditor::LSN_METADATA> &_vMetadata, size_t _sIdx, size_t _sDigits,
			const std::string &_sPrefix );

		/**
		 * The subclass procedure for the metadata combo boxes and their drop-down lists.  Sends typed characters to TypeAhead().
		 * 
		 * \param _hWnd The combo box or its drop-down list.
		 * \param _uMsg The message.
		 * \param _wParam The WPARAM of the message.
		 * \param _lParam The LPARAM of the message.
		 * \param _uIdSubclass The ID of the combo box.
		 * \param _dwRefData Pointer to the CWavEditorSequencingPage object.
		 * \return Returns the result of the message.
		 **/
		static LRESULT CALLBACK								ComboSubclassProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam, UINT_PTR _uIdSubclass, DWORD_PTR _dwRefData );

	private :
		typedef CWinUtilities								Layout;
		typedef lsw::CWidget								Parent;
	};


}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

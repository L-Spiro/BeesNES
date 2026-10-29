#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The page for file settings.
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
	 * Class CWavEditorFileSettingsPage
	 * \brief The page for file settings.
	 *
	 * Description: The page for file settings.
	 */
	class CWavEditorFileSettingsPage : public lsw::CWidget {
	public :
		CWavEditorFileSettingsPage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Types.
		/** The values of this page's controls for a single track. */
		struct LSN_TRACK {
			/** The Name text. */
			std::wstring									wsTitle;
			/** The File Name Prefix text. */
			std::wstring									wsTitlePreFix;
			/** The File Name Postfix text. */
			std::wstring									wsTitlePostFix;
			/** The Actual Hz text. */
			std::wstring									wsActualHz;
			/** The Characteristics/Volume text. */
			std::wstring									wsCharVolume;
			/** The Characteristics/LPF Hz text. */
			std::wstring									wsCharLpfHz;
			/** The Characteristics/HPF 1 Hz text. */
			std::wstring									wsCharHpf0Hz;
			/** The Characteristics/HPF 2 Hz text. */
			std::wstring									wsCharHpf1Hz;
			/** The Characteristics/HPF 3 Hz text. */
			std::wstring									wsCharHpf2Hz;
			/** The Characteristics/LPF Falloff text. */
			std::wstring									wsCharLpfFall;
			/** The Characteristics/HPF 1 Falloff text. */
			std::wstring									wsCharHpf0Fall;
			/** The Characteristics/HPF 2 Falloff text. */
			std::wstring									wsCharHpf1Fall;
			/** The Characteristics/HPF 3 Falloff text. */
			std::wstring									wsCharHpf2Fall;
			/** The Artist text. */
			std::wstring									wsArtist;
			/** The Album text. */
			std::wstring									wsAlbum;
			/** The Year text. */
			std::wstring									wsYear;
			/** The Comment text. */
			std::wstring									wsComment;
			/** The index of the selected item in the Actual Hz combo box. */
			int32_t											i32ActualHzSelection = -1;
			/** The index of the selected item in the Preset combo box. */
			int32_t											i32CharPresetSelection = -1;
			/** The index of the selected item in the LPF Type combo box. */
			int32_t											i32LpfTypeSelection = 0;
			/** The index of the selected item in the HPF 1 Type combo box. */
			int32_t											i32Hpf0TypeSelection = 0;
			/** The index of the selected item in the HPF 2 Type combo box. */
			int32_t											i32Hpf1TypeSelection = 0;
			/** The index of the selected item in the HPF 3 Type combo box. */
			int32_t											i32Hpf2TypeSelection = 0;
			/** The Lock Volume check. */
			bool											bLockVol = false;
			/** The Invert check. */
			bool											bInvert = true;
			/** The LPF check. */
			bool											bLpf = true;
			/** The HPF 1 check. */
			bool											bHpf0 = true;
			/** The HPF 2 check. */
			bool											bHpf1 = true;
			/** The HPF 3 check. */
			bool											bHpf2 = true;
			/** The RF 1 check. */
			bool											bRf1 = false;
			/** The RF 2 check. */
			bool											bRf2 = false;
			/** The Sunsoft 5B check. */
			bool											bSunsoft5b = false;


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
			 * Gets the stored check state of a check box.
			 * 
			 * \param _wId The ID of the check box.
			 * \return Returns a pointer to the stored check state of the given check box or nullptr if _wId is not a check box on this page.
			 **/
			bool *											Check( WORD _wId );

			/**
			 * Gets the stored check state of a check box.
			 * 
			 * \param _wId The ID of the check box.
			 * \return Returns a pointer to the stored check state of the given check box or nullptr if _wId is not a check box on this page.
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
		 * Verifies the values of a track.
		 * 
		 * \param _tTrack The values to verify.
		 * \param _wsMsg The error message to display.
		 * \return Returns the control that failed or nullptr.
		 **/
		CWidget *											Verify( const LSN_TRACK &_tTrack, std::wstring &_wsMsg );

		/**
		 * Saves the values of a track.
		 * 
		 * \param _ui32Id The unique ID of the track.
		 * \param _tTrack The values to save.
		 * \param _pfPerFileOptions The object to which to save the window state.
		 * \param _ppfOutput The output object to which to transfer all the window settings.
		 */
		void												Save( uint32_t _ui32Id, const LSN_TRACK &_tTrack, LSN_WAV_EDITOR_WINDOW_OPTIONS::LSN_PER_FILE &_pfPerFileOptions, CWavEditor::LSN_PER_FILE * _ppfOutput );

		/**
		 * Fills out the values of a track with saved values from a project.
		 * 
		 * \param _pfPerFileOptions The per-file settings to load.
		 * \param _tTrack The track values to fill out.
		 **/
		void												Load( const LSN_WAV_EDITOR_WINDOW_OPTIONS::LSN_PER_FILE &_pfPerFileOptions, LSN_TRACK &_tTrack );

		/**
		 * Sets the values of a newly added track that depend on its WAV file (its name and an initial Actual Hz value based off the Hz detected in the
		 *	WAV file).
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


	protected :
		// == Members.
		/** The WAV Editor object. */
		CWavEditor *										m_pweEditor = nullptr;
		/** The ID of the WAV set whose values are shown, or 0 if showing the combined values of multiple tracks. */
		uint32_t											m_ui32Id = 0;
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
		/** The check boxes whose check states are stored per track. */
		static const WORD									m_wCheckIds[];


		// == Functions.
		/**
		 * Gets the unique ID's of the tracks to which changes in the controls are applied.  This is the shown track or else the selected tracks (or
		 *	all tracks if none are selected).
		 * 
		 * \param _vPages Holds the returned array of track ID's to update.
		 * \param _bUpdateAll If true, all tracks except the shown track are returned.
		 **/
		void												GetPagesToUpdate( std::vector<LPARAM> &_vPages, bool _bUpdateAll = false );

		/**
		 * Gets the index of the item in a combo box with the given item data.
		 * 
		 * \param _wId The ID of the combo box.
		 * \param _lpData The item data to find.
		 * \return Returns the index of the item with the given data or -1.
		 **/
		int32_t												ComboIndex( WORD _wId, LPARAM _lpData );

		/**
		 * Gets the item data of an item in a combo box.
		 * 
		 * \param _wId The ID of the combo box.
		 * \param _i32Index The index of the item.
		 * \param _lpBad The value to return if the combo box does not exist.
		 * \return Returns the item data of the given item.
		 **/
		LPARAM												ComboData( WORD _wId, int32_t _i32Index, LPARAM _lpBad );

		/**
		 * Gets the LPF Pole and Zero from a string.
		 * 
		 * \param _wsText The text to evaluate.
		 * \param _fPole Holds the returned Pole.
		 * \param _fZero Holds the returned Zero.
		 * \return Returns true if the text is a valid expression giving 1 or 2 values.
		 **/
		static bool											GetLpfAndZero( const std::wstring &_wsText, float &_fPole, float &_fZero );

		/**
		 * Gets the LPF Pole and Zero.
		 * 
		 * \param _fPole Holds the returned Pole.
		 * \param _fZero Holds the returned Zero.
		 * \return Returns true if the edit control is valid and the values inside it are valid.
		 **/
		bool												GetLpfAndZero( float &_fPole, float &_fZero ) {
			auto aEdit = FindChild( Layout::LSN_WEWI_FSETS_CHAR_LPF_EDIT );
			if ( aEdit ) {
				return GetLpfAndZero( aEdit->GetTextW(), _fPole, _fZero );
			}
			return false;
		}

	private :
		typedef CWinUtilities								Layout;
		typedef lsw::CWidget								Parent;
	};


}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

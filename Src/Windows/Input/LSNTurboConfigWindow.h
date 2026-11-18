#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The Turbo Button Configuration dialog.  Creates, edits, and deletes turbo-pattern presets:  up to 64 on/off states (typed as
 *	1's and 0's or toggled with check boxes) and how they are played (repeating or once, frames per state, restarting on each press,
 *	holding the button after a one-shot pattern ends, and whether the global speed multiplier applies).
 */

#pragma once

#include "../../LSNLSpiroNes.h"
#include "../../Input/LSNTurboPattern.h"
#include "../WinUtilities/LSNWinUtilities.h"
#include "LSNTurboConfigWindowLayout.h"

#include <MainWindow/LSWMainWindow.h>

#include <string>

using namespace lsw;

namespace lsn {

	/**
	 * Class CTurboConfigWindow
	 * \brief The Turbo Button Configuration dialog.
	 *
	 * Description: The Turbo Button Configuration dialog.  Creates, edits, and deletes turbo-pattern presets.
	 */
	class CTurboConfigWindow : public lsw::CMainWindow {
	public :
		CTurboConfigWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Types.
		/** The parent type. */
		typedef lsw::CMainWindow										Parent;


		// == Functions.
		/**
		 * The WM_INITDIALOG handler.
		 *
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED												InitDialog();

		/**
		 * The WM_CLOSE handler.
		 *
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED												Close();

		/**
		 * Handles the WM_COMMAND message.
		 *
		 * \param _wCtrlCode 0 = from menu, 1 = from accelerator, otherwise it is a Control-defined notification code.
		 * \param _wId The ID of the control if _wCtrlCode is not 0 or 1.
		 * \param _pwSrc The source control if _wCtrlCode is not 0 or 1.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED												Command( WORD _wCtrlCode, WORD _wId, CWidget * _pwSrc );

		/**
		 * Handles WM_COMMAND without a source control.  The dialog manager sends IDOK (Enter) and IDCANCEL (Escape) this way.
		 *
		 * \param _wId The command identifier.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED												MenuCommand( WORD _wId );


	protected :
		// == Members.
		/** The presets and the preset to select first. */
		CTurboConfigWindowLayout::LSN_TURBO_CONFIG_DATA *				m_ptcdData;
		/** The name of the preset being edited, or an empty string for a new (unsaved) pattern. */
		std::u16string													m_u16Editing;
		/** The pattern shown in the controls (its states and length are kept here as the controls change). */
		LSN_TURBO_PLAYBACK												m_tpCurrent;
		/** The pattern as it was loaded or last saved (to detect unsaved changes). */
		LSN_TURBO_PLAYBACK												m_tpBaseline;
		/** The name as it was loaded or last saved. */
		std::wstring													m_wsBaselineName;
		/** Set while the controls are being updated, so that their notifications are ignored. */
		bool															m_bUpdating = false;


		// == Functions.
		/**
		 * Fills the list of presets.
		 *
		 * \param _u16Select The preset to select, or an empty string to select nothing.
		 */
		void															FillList( const std::u16string &_u16Select );

		/**
		 * Shows a pattern in the controls and makes it the baseline for detecting unsaved changes.
		 *
		 * \param _u16Editing The name of the preset being edited, or an empty string for a new pattern.
		 * \param _wsName The name to show.
		 * \param _tpPlayback The pattern to show.
		 */
		void															ShowPattern( const std::u16string &_u16Editing, const std::wstring &_wsName, const LSN_TURBO_PLAYBACK &_tpPlayback );

		/**
		 * Reads the pattern from the controls.
		 *
		 * \param _tpPattern Holds the pattern (its path is not set).
		 * \param _bShowErrors If true, an invalid value is reported and its control is focused.
		 * \return Returns true if every value is valid.
		 */
		bool															ReadPattern( LSN_TURBO_PATTERN &_tpPattern, bool _bShowErrors );

		/**
		 * Determines whether the controls differ from the pattern as it was loaded or last saved.
		 *
		 * \return Returns true if there are unsaved changes.
		 */
		bool															IsDirty();

		/**
		 * If there are unsaved changes, asks whether to save them.
		 *
		 * \return Returns true if it is fine to move on (saved or discarded), false if the user canceled or saving failed.
		 */
		bool															ResolveUnsaved();

		/**
		 * Saves the pattern in the controls as a preset.
		 *
		 * \return Returns true if it was saved.
		 */
		bool															SavePattern();

		/**
		 * Starts a new pattern with default values and a unique name.
		 *
		 * \param _bUnsaved If true, the new pattern counts as an unsaved change (closing or switching asks to save it).
		 */
		void															NewPattern( bool _bUnsaved );

		/**
		 * Starts a new pattern that is a copy of the one in the controls.
		 */
		void															DuplicatePattern();

		/**
		 * Deletes the preset being edited.
		 */
		void															DeletePattern();

		/**
		 * Shows the preset selected in the list.
		 */
		void															ListSelectionChanged();

		/**
		 * Updates the states edit and the check boxes from m_tpCurrent.
		 *
		 * \param _bStatesEdit If true, the states edit is updated.
		 * \param _bLengthEdit If true, the length edit is updated.
		 */
		void															UpdateStates( bool _bStatesEdit, bool _bLengthEdit );

		/**
		 * Enables and disables controls and updates the information label.
		 */
		void															UpdateControls();

		/**
		 * Makes a name that no preset has yet.
		 *
		 * \param _wsBase The name to start from.
		 * \param _bCopy If true, names are made as copies of _wsBase ("X Copy", "X Copy 2"), otherwise as numbered patterns ("Pattern 1").
		 * \return Returns the unique name.
		 */
		std::wstring													UniqueName( const std::wstring &_wsBase, bool _bCopy ) const;

		/**
		 * Reads a whole number from an edit control.
		 *
		 * \param _wId The ID of the edit control.
		 * \param _ui32Min The smallest valid value.
		 * \param _ui32Max The largest valid value.
		 * \param _ui32Ret Holds the returned value.
		 * \return Returns true if the edit holds a whole number in range.
		 */
		bool															ReadNumber( WORD _wId, uint32_t _ui32Min, uint32_t _ui32Max, uint32_t &_ui32Ret );

		/**
		 * Shows an error and focuses a control.
		 *
		 * \param _wId The ID of the control.
		 * \param _pwcError The error to show.
		 */
		void															ShowError( WORD _wId, const wchar_t * _pwcError );

	private :
		typedef CWinUtilities											Layout;
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

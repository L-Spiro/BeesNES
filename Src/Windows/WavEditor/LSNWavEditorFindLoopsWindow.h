#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The Find Loop Points dialog.  Searches the metadata of a track (the logged APU register writes) for ranges of events that
 *	repeat a given number of times and lets the user pick one of them as the ending point of the track.
 */

#pragma once

#include "../../LSNLSpiroNes.h"
#include "../../Wav/LSNWavEditor.h"
#include "../WinUtilities/LSNWinUtilities.h"
#include "LSNWavEditorFindLoopsWindowLayout.h"

#include <MainWindow/LSWMainWindow.h>

using namespace lsw;

namespace lsn {

	/**
	 * Class CWavEditorFindLoopsWindow
	 * \brief The Find Loop Points dialog.
	 *
	 * Description: The Find Loop Points dialog.  Searches the metadata of a track (the logged APU register writes) for ranges of events that
	 *	repeat a given number of times and lets the user pick one of them as the ending point of the track.
	 */
	class CWavEditorFindLoopsWindow : public lsw::CMainWindow {
	public :
		CWavEditorFindLoopsWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


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


	protected :
		// == Members.
		/** The metadata to search and the returned ending point. */
		CWavEditorFindLoopsWindowLayout::LSN_FIND_LOOPS_DATA *			m_pfldData;
		/** The loop ranges found by the last search. */
		std::vector<CWavEditor::LSN_LOOP_RANGE>							m_vResults;
		/** The last approximate loop start (remembered between uses of the dialog). */
		static std::wstring												m_wsStart;
		/** The last approximate first loop point. */
		static std::wstring												m_wsLoop;
		/** The last number of loops. */
		static std::wstring												m_wsLoops;
		/** The last search range. */
		static std::wstring												m_wsRange;
		/** The last allowed percentage of mismatched events. */
		static std::wstring												m_wsMismatch;


		// == Functions.
		/**
		 * Reads the search parameters, searches for loops, and fills the list box with the results.
		 */
		void															Find();

		/**
		 * Shows the details of the selected loop range and enables or disables the Apply button.
		 */
		void															SelectionChanged();

		/**
		 * Returns the ending point of the selected loop range and closes the dialog.
		 */
		void															Apply();

		/**
		 * Stores the texts of the edit controls so that they can be restored the next time the dialog is opened.
		 */
		void															SaveTexts();

		/**
		 * Reads an expression from an edit control.  Shows an error and focuses the control on failure.
		 *
		 * \param _wId The ID of the edit control.
		 * \param _pwcError The error to show on failure.
		 * \param _dRet Holds the returned value.
		 * \return Returns true if the edit control holds a valid expression.
		 */
		bool															ReadEdit( WORD _wId, const wchar_t * _pwcError, double &_dRet );

		/**
		 * Shows an error about the value of an edit control and focuses the control.
		 *
		 * \param _wId The ID of the edit control.
		 * \param _pwcError The error to show.
		 */
		void															EditError( WORD _wId, const wchar_t * _pwcError );

		/**
		 * Gets the text of a metadata entry as it is shown in the metadata combo boxes of the WAV Editor.
		 *
		 * \param _sIdx The index of the metadata entry.
		 * \return Returns the text of the metadata entry.
		 */
		std::wstring													EventText( size_t _sIdx ) const;

	private :
		typedef CWinUtilities											Layout;
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

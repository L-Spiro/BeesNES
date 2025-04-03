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
		 * Prepares to create the window.  Creates the atom if necessary.
		 **/
		static void											PrepareWavEditor();


	public :
		// == Members.
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
		/** The main window class. */
		static ATOM											m_aAtom;


	private :
		typedef CWavEditorWindowLayout						Layout;

	};

 }	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS
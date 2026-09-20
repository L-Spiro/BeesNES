#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The Cheats dialog.
 */

#pragma once

#include "../../Cheats/LSNCheatManager.h"
#include "../WinUtilities/LSNWinUtilities.h"

#include <ImageList/LSWImageList.h>
#include <Images/LSWBitmap.h>
#include <MainWindow/LSWMainWindow.h>

using namespace lsw;

namespace lsn {

	/**
	 * Class CCheatsWindow
	 * \brief The Cheats dialog.
	 *
	 * Description: The Cheats dialog.
	 */
	class CCheatsWindow : public lsw::CMainWindow {
	public :
		CCheatsWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Enumerations.
		// Images.
		enum LSN_IMAGES {
			LSN_I_ADD,
			LSN_I_DELETE,
			LSN_I_EDIT,
			LSN_I_DUPLICATE,
			LSN_I_FILTER,
			LSN_I_SEARCH,

			LSN_I_TOTAL
		};

		// Menus.
		enum LSN_MENUS {
			LSN_M_CONTEXT_MENU								= 300,
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
		 * Handles WM_COMMAND from a menu.
		 * \brief Invoked for menu command selections.
		 *
		 * \param _wId The menu command identifier.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED									MenuCommand( WORD _wId );

		/**
		 * The WM_NOTIFY -> LVN_ITEMCHANGED handler.
		 *
		 * \param _lplvParm The notifacation structure.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED									Notify_ItemChanged( LPNMLISTVIEW _lplvParm );

		/**
		 * The WM_NOTIFY -> LVN_ODSTATECHANGED handler.
		 *
		 * \param _lposcParm The notifacation structure.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED									Notify_OdStateChange( LPNMLVODSTATECHANGE _lposcParm );

		/**
		 * Handles the WM_CONTEXTMENU message.
		 * 
		 * \param _pwControl The control that was clicked.
		 * \param _iX The horizontal position of the cursor, in screen coordinates, at the time of the mouse click.
		 * \param _iY The vertical position of the cursor, in screen coordinates, at the time of the mouse click.
		 * \return Returns an LSW_HANDLED code.
		 **/
		virtual LSW_HANDLED									ContextMenu( CWidget * _pwControl, INT _iX, INT _iY );


	protected :
		// == Members.
		CCheatManager *										m_pcmCheatManager = nullptr;				/**< The pointer to the cheat manager. */
		CImageList											m_iImages;									/** the image list for the toolbar. */
		CBitmap												m_bBitmaps[LSN_I_TOTAL];					/** Images for the toolbar. */
		INT													m_iImageMap[LSN_I_TOTAL];					/** Toolbar image mapping. */


		// == Functions.
		/**
		 * Fills the TreeListView with filtered cheats.
		 * 
		 * \return Returns the number of cheats added to the TreeListView.
		 **/
		size_t												AddCheats();

		/**
		 * Updates the addresses and notes edits based on the TreeListView selection.
		 **/
		void												UpdateSelection();


	private :
		//typedef CAudioOptionsWindowLayout					Layout;
		typedef CWinUtilities								Layout;
		typedef lsw::CMainWindow							Parent;
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

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

#include <Edit/LSWEdit.h>
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
		CImageList											m_iImages;									/**< the image list for the toolbar. */
		CBitmap												m_bBitmaps24[LSN_I_TOTAL];					/**< Images for the toolbar. */
		INT													m_iImageMap[LSN_I_TOTAL];					/**< Toolbar image mapping. */
		CBitmap												m_bBitmaps16[LSN_I_TOTAL];					/**< Images for the toolbar. */
		CEdit *												m_peFilterEdit = nullptr;					/**< The Filter toolbar edit. */
		CEdit *												m_peSearchEdit = nullptr;					/**< The Search toolbar edit. */


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

		/**
		 * Creates the string representation for a given cheat entry intended for use in the TreeListView.
		 * 
		 * \param _ceEntry The cheat entry whose string representation is to be generated.
		 * \return Returns the string representation of the given cheat entry.
		 **/
		static std::wstring									CheatEntryToString( const LSN_CHEAT_ENTRY &_ceEntry );

		/**
		 * Reveals enabled cheats.
		 * 
		 * \param _bSelect If true, the items are also selected.
		 **/
		void												RevealEnabled( bool _bSelect = false );
		
		/**
		 * Gathers selected cheats, omitting the game namess from the return.
		 * 
		 * \return Returns an array of cheat ID's with the game names omitted.
		 **/
		std::vector<LPARAM>									GatherSelectedGames() const;


	private :
		typedef CWinUtilities								Layout;
		typedef lsw::CMainWindow							Parent;
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

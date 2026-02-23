#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The top part of the patch window.
 */

#pragma once

#include "LSNPatchWindowLayout.h"
#include "../../Roms/LSNRom.h"

#include <Helpers/LSWHelpers.h>
#include <TreeListView/LSWTreeListView.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace lsn {
	/**
	 * Class CPatchWindowTopPage
	 * \brief The top part of the patch window.
	 *
	 * Description: The top part of the patch window.
	 */
	class CPatchWindowTopPage : public lsw::CWidget {
	public :
		CPatchWindowTopPage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		virtual ~CPatchWindowTopPage();


		// == Enumerations.
		// Menus.
		enum LSN_MENUS {
			LSN_M_CONTEXT_MENU								= 156,
		};


		// == Functions.
		/**
		 * The WM_INITDIALOG handler.
		 *
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED									InitDialog();

		/**
		 * The WM_CLOSE handler.
		 *
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED									Close();

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
		 * Handles WM_TIMER.
		 * \brief Notified when a timer elapses.
		 *
		 * \param _uiptrId Timer identifier.
		 * \param _tpProc Optional callback associated with the timer.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED									Timer( UINT_PTR _uiptrId, TIMERPROC /*_tpProc*/ );

		/**
		 * Handles the WM_GETMINMAXINFO message.
		 * 
		 * \param _pmmiInfo The min/max info structure to fill out.
		 * \return Returns an LSW_HANDLED code.
		 **/
		virtual LSW_HANDLED									GetMinMaxInfo( MINMAXINFO * _pmmiInfo );

		/**
		 * Handles the WM_CONTEXTMENU message.
		 * 
		 * \param _pwControl The control that was clicked.
		 * \param _iX The horizontal position of the cursor, in screen coordinates, at the time of the mouse click.
		 * \param _iY The vertical position of the cursor, in screen coordinates, at the time of the mouse click.
		 * \return Returns an LSW_HANDLED code.
		 **/
		virtual LSW_HANDLED									ContextMenu( CWidget * _pwControl, INT _iX, INT _iY );

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
		 * Gets the window rectangle for the given widget or gets this control's window rectangle if _pwChild is nullptr.
		 * 
		 * \param _pwChild The widget whose window rectangle is to be gotten.
		 * \return Returns the child's window rectangle or this client rectangle.
		 **/
		//virtual LSW_RECT									WindowRect( const CWidget * _pwChild ) const;


	protected :
		// == Types.
		/** Any available patch information. */
		struct LSN_PATCH_INFO {
			std::u16string									u16FullPath;												/**< The full path to the patch file. */
			std::vector<uint8_t>							vLoadedPatchFile;											/**< The in-memory patch file. */
			std::vector<std::u8string>						vTokenizedFolders;											/**< The tokenized folders. */
			uint32_t										ui32Crc = 0;												/**< The desired CRC for the source ROM. */
			uint32_t										ui32PatchCrc = 0;											/**< the CRC for the patch file. */
			uint32_t										ui32PatchCrcMinus4 = 0;										/**< the CRC for the patch file excluding the final 4 bytes. */
			bool											bIsText = false;											/**< Is a text file or .MD file. */
		};

		/** A class for a temporary tree. */
		struct LSN_PATCH_INFO_TREE_ITEM {
			std::vector<LSN_PATCH_INFO_TREE_ITEM>			vChildren;													/**< The children for this node. */
			std::u8string									u8Name;														/**< Name of the node. */
			size_t											sIdx = ~size_t( 0 );										/**< Index of the information item. */
		};


		// == Members.
		LSN_OPTIONS *										m_poOptions;												/**< The options object. */
		std::vector<uint8_t>								m_vPatchRomFile;											/**< The ROM to patch. */
		std::u16string										m_u16RomPath;												/**< Path to the ROM. */
		std::vector<LSN_PATCH_INFO>							m_vPatchInfo;												/**< The patch information. */
		LSN_ROM												m_rRomInfo;													/**< ROM information. */
		uint32_t											m_ui32FullCrc = 0;											/**< The full CRC size. */
		bool												m_bOutIsAutoFilled;											/**< THe output path was auto-generated. */
		lsw::LSW_TIMER										m_tUpdateBottomTimer;										/**< Timer for updating the bottom text. */


		// == Functions.
		/**
		 * Updates the source ROM information labels.
		 **/
		void												UpdateInfo();

		/**
		 * Updates the colors based on the currently loaded ROM file.
		 **/
		void												UpdateColors();

		/**
		 * Updates the text description on the bottom based on the currently selected items.
		 **/
		void												UpdateText();

		/**
		 * Reveals compatiable ROM hacks.  Finds BPS files with CRC’s matching the loaded ROM file and expands those while collapsing all others.
		 * 
		 * \param _bSelect If true, the items are also selected.
		 **/
		void												RevealCompatible( bool _bSelect = false );

		/**
		 * Checks the given tree item for having a child that is a BPS file compatible with the current ROM.
		 * 
		 * \param _ptlvTree A pointer to the TreeListView.
		 * \param _htiItem The item to recursively check for being compatible.
		 * \return Returns true if the item or any of its children are compatible with the current ROM file.
		 **/
		bool												IsCompatible( lsw::CTreeListView * _ptlvTree, HTREEITEM _htiItem ) const;

		/**
		 * Checks for a child node that is a checksum patch.
		 * 
		 * \param _ptlvTree A pointer to the TreeListView.
		 * \param _htiItem The item to recursively check for being a checksum patch.
		 * \return Returns true if the item or any of its children are is a BPS or other checksum-based patch file.
		 **/
		bool												HasCheckSumPatch( lsw::CTreeListView * _ptlvTree, HTREEITEM _htiItem ) const;

		/**
		 * Reveals all patch files with CRC checks.
		 **/
		void												RevealCrcPatches();

		/**
		 * Adds all the nodes in _vNodes as children of _hParent.
		 * 
		 * \param _vNodes The nodes to add under _hParent.
		 * \param _hParent The parent under which to add _vNodes.
		 **/
		void												AddToTree( const std::vector<LSN_PATCH_INFO_TREE_ITEM> &_vNodes, HTREEITEM _hParent, lsw::CTreeListView * _ptlTree );

		/**
		 * Creates a non-optimized basic tree given patch information.
		 * 
		 * \param _vInfo The information from which to generate a basic tree structure.
		 * \return Returns the root nodes of the tree.
		 **/
		static std::vector<LSN_PATCH_INFO_TREE_ITEM>		CreateBasicTree( const std::vector<LSN_PATCH_INFO> &_vInfo );

		/**
		 * Finds a node with the given name.  Returns a pointer to the node or nullptr.
		 * 
		 * \param _vNodes the nodes to search.
		 * \param _u8Name The name of the node to find.
		 * \return Returns the node if found or nullptr otherwise.
		 **/
		static LSN_PATCH_INFO_TREE_ITEM *					FindNode( std::vector<LSN_PATCH_INFO_TREE_ITEM> &_vNodes, const std::u8string &_u8Name );

		/**
		 * Simplifies the tree by joining a parent with its child if it has only 1 child.
		 * 
		 * \param _vTree The tree to simplify.
		 **/
		static void											SimplifyTree( std::vector<LSN_PATCH_INFO_TREE_ITEM> &_vTree );

	private :
		typedef CPatchWindowLayout							Layout;
		typedef lsw::CWidget								Parent;

	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

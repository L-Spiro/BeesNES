#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The patch window.
 */

#pragma once

#include "LSNPatchWindowLayout.h"
#include <MainWindow/LSWMainWindow.h>

using namespace lsw;

namespace lsn {

	class											CMainWindow;

	/**
	 * Class CPatchWindow
	 * \brief The patch window.
	 *
	 * Description: The patch window.
	 */
	class CPatchWindow : public lsw::CMainWindow {
	public :
		CPatchWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		virtual ~CPatchWindow();


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
		 * Handles the WM_GETMINMAXINFO message.
		 * 
		 * \param _pmmiInfo The min/max info structure to fill out.
		 * \return Returns an LSW_HANDLED code.
		 **/
		virtual LSW_HANDLED									GetMinMaxInfo( MINMAXINFO * _pmmiInfo );


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


		// == Members.
		LSN_OPTIONS *										m_poOptions;												/**< The options object. */
		std::vector<uint8_t>								m_vPatchRomFile;											/**< The ROM to patch. */
		std::u16string										m_u16RomPath;												/**< Path to the ROM. */
		std::vector<LSN_PATCH_INFO>							m_vPatchInfo;												/**< The patch information. */
		bool												m_bOutIsAutoFilled;											/**< THe output path was auto-generated. */


		// == Functions.
		/**
		 * Updates the source ROM information labels.
		 **/
		void												UpdateInfo();


	private :
		typedef CPatchWindowLayout							Layout;
		typedef lsw::CMainWindow							Parent;

	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

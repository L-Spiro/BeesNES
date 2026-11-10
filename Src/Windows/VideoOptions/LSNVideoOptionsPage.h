#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A page of fields in the Video Options dialog (the Host Machine page or the page of a filter).
 */

#pragma once

#include "LSNVideoOptionsWindowLayout.h"

#include <Widget/LSWWidget.h>

#include <vector>

using namespace lsw;

namespace lsn {

	/**
	 * Class CVideoOptionsPage
	 * \brief A page of fields in the Video Options dialog.
	 *
	 * Description: A page of fields in the Video Options dialog (the Host Machine page or the page of a filter).  Every change is written
	 *	to the options immediately and applied to the emulator, so that it shows on the screen.
	 */
	class CVideoOptionsPage : public lsw::CWidget {
	public :
		CVideoOptionsPage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Functions.
		/**
		 * The WM_INITDIALOG handler.
		 *
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED									InitDialog();

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
		 * Reloads every control from the options.
		 */
		void												Refresh();

		/**
		 * Reselects the entries of the combo boxes (without refilling them) and updates the labels that depend on the settings.  Used after
		 *	a field is typed into, since combo boxes such as the Deep Internals preset reflect the typed values.
		 */
		void												RefreshDependents();


	protected :
		// == Members.
		CVideoOptionsWindowLayout::LSN_PAGE_DATA *			m_ppdData;									/**< The data of the page. */
		LSN_PPU_METRICS										m_pmRegion = LSN_PM_NTSC_M;					/**< The region being edited. */
		std::vector<CVideoOptionsFields::LSN_VIDEO_FIELD>	m_vFields;									/**< The fields on the page. */
		bool												m_bUpdating = false;						/**< True while the controls are being filled (their notifications are ignored). */


		// == Functions.
		/**
		 * Gets the settings the page edits.
		 *
		 * \return Returns the settings the page edits.
		 */
		CVideoOptionsFields::LSN_FIELD_TARGET				Target() const;

		/**
		 * Determines whether the page can be edited (a per-game page can be edited only while per-game settings are in use).
		 *
		 * \return Returns true if the page can be edited.
		 */
		bool												CanEdit() const;

		/**
		 * Enables or disables each control according to the current settings.
		 */
		void												UpdateEnabled();

		/**
		 * Applies the options to the emulator.
		 */
		void												ApplyChanges();

		/**
		 * Asks the user for a palette file and loads it.
		 */
		void												LoadPaletteFromFile();

		/**
		 * Gets the starting region of the page:  the preset nearest the region of the running game.
		 *
		 * \return Returns the starting region of the page.
		 */
		LSN_PPU_METRICS										StartingRegion() const;

		/**
		 * Sizes the palette display to its fixed client area (LSN_PALETTE_DISPLAY_W by LSN_PALETTE_DISPLAY_H pixels plus its scroll bar)
		 *	and makes the Load combo box end where the display ends.
		 */
		void												SizePaletteDisplay();

		/**
		 * Fills in the descriptions of the Host Machine page and lays its rows out around their measured heights.
		 */
		void												LayOutDescriptions();

		/**
		 * Gets the rectangle of a child control in the client coordinates of this page.
		 *
		 * \param _pwChild The child control.
		 * \return Returns the rectangle of the child control in the client coordinates of this page.
		 */
		LSW_RECT											ChildRect( CWidget * _pwChild ) const;

		/**
		 * Converts dialog units to pixels using the font of this page.
		 *
		 * \param _iX The horizontal dialog units.
		 * \param _iY The vertical dialog units.
		 * \return Returns the size in pixels.
		 */
		POINT												DialogUnitsToPixels( INT _iX, INT _iY ) const;


	private :
		typedef lsw::CWidget								Parent;
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

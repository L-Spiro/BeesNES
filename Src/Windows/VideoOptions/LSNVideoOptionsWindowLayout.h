#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The layout for the Video Options dialog.
 */

#pragma once

#include "../../LSNLSpiroNes.h"
#include "../../Options/LSNOptions.h"
#include "../WinUtilities/LSNWinUtilities.h"
#include "LSNVideoOptionsFields.h"

#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

#include <vector>

using namespace lsw;

namespace lsn {

	class													CMainWindow;

	/**
	 * Class CVideoOptionsWindowLayout
	 * \brief The layout for the Video Options dialog.
	 *
	 * Description: The layout for the Video Options dialog.  The pages of the dialog are laid out at run time from the fields they show
	 *	(see CVideoOptionsFields).
	 */
	class CVideoOptionsWindowLayout {
	public :
		// == Types.
		/**
		 * Struct LSN_VIDEO_OPTIONS_DATA
		 * \brief The data passed to the Video Options dialog.
		 *
		 * Description: The data passed to the Video Options dialog.  The dialog edits the options in place (so that changes show on the
		 *	screen immediately) and restores them if it is cancelled.
		 */
		struct LSN_VIDEO_OPTIONS_DATA {
			LSN_OPTIONS *									poOptions = nullptr;						/**< The options to edit. */
			lsn::CMainWindow *								pmwMainWindow = nullptr;					/**< The main window, which applies changed options to the emulator. */
			LSN_PPU_METRICS									pmActiveRegion = LSN_PM_NTSC_M;				/**< The region of the running game (each page starts on the preset nearest to it). */
		};

		/**
		 * Struct LSN_PAGE_DATA
		 * \brief The data passed to a page of the Video Options dialog.
		 *
		 * Description: The data passed to a page of the Video Options dialog.
		 */
		struct LSN_PAGE_DATA {
			LSN_VIDEO_OPTIONS_DATA *						pvodData = nullptr;							/**< The data of the dialog. */
			CVideoOptionsFields::LSN_VIDEO_PAGE_KIND		vpkKind = CVideoOptionsFields::LSN_VPK_HOST;	/**< The kind of page. */
			bool											bGlobal = true;								/**< If true, the page edits the global settings, otherwise the per-game settings. */
		};


		// == Functions.
		/**
		 * Creates the Video Options dialog.
		 *
		 * \param _pwParent The parent of the dialog.
		 * \param _vodData The data of the dialog.
		 * \return Returns TRUE if the dialog was closed with OK.
		 */
		static BOOL											CreateVideoOptionsDialog( CWidget * _pwParent, LSN_VIDEO_OPTIONS_DATA &_vodData );

		/**
		 * Creates a page of fields (the Host Machine page or a filter page).
		 *
		 * \param _pwParent The parent widget.
		 * \param _ppdData The data of the page.  Must remain valid for the lifetime of the page.
		 * \param _pwMetrics A dialog with the font of the pages, used to size the right column (see RightColumnWidth()), or nullptr to use _pwParent.
		 * \return Returns the created page.
		 */
		static CWidget *									CreateFieldPage( CWidget * _pwParent, LSN_PAGE_DATA * _ppdData, CWidget * _pwMetrics = nullptr );

		/**
		 * Creates a settings page (the Global Settings or Per-Game Settings page, which holds a tab for each filter).
		 *
		 * \param _pwParent The parent widget.
		 * \param _ppdData The data of the page.  Must remain valid for the lifetime of the page.
		 * \return Returns the created page.
		 */
		static CWidget *									CreateSettingsPage( CWidget * _pwParent, LSN_PAGE_DATA * _ppdData );

		/**
		 * Gets the control ID of the label of a field.
		 *
		 * \param _vfField The field.
		 * \return Returns the control ID of the label of the field.
		 */
		static inline WORD									LabelId( CVideoOptionsFields::LSN_VIDEO_FIELD _vfField ) { return WORD( CWinUtilities::LSN_VOWI_FIELD_FIRST + _vfField * 2 ); }

		/**
		 * Gets the control ID of the control of a field.
		 *
		 * \param _vfField The field.
		 * \return Returns the control ID of the control of the field.
		 */
		static inline WORD									ControlId( CVideoOptionsFields::LSN_VIDEO_FIELD _vfField ) { return WORD( CWinUtilities::LSN_VOWI_FIELD_FIRST + _vfField * 2 + 1 ); }

		/**
		 * Gets the control ID of the description of a field (Host Machine page only).
		 *
		 * \param _vfField The field.
		 * \return Returns the control ID of the description of the field.
		 */
		static inline WORD									DescriptionId( CVideoOptionsFields::LSN_VIDEO_FIELD _vfField ) { return WORD( CWinUtilities::LSN_VOWI_FIELD_FIRST + CVideoOptionsFields::LSN_VF_TOTAL * 2 + _vfField ); }

		/**
		 * Gets the control ID of the separator below a field (Host Machine page only).
		 *
		 * \param _vfField The field.
		 * \return Returns the control ID of the separator below the field.
		 */
		static inline WORD									SeparatorId( CVideoOptionsFields::LSN_VIDEO_FIELD _vfField ) { return WORD( CWinUtilities::LSN_VOWI_FIELD_FIRST + CVideoOptionsFields::LSN_VF_TOTAL * 3 + _vfField ); }

		/**
		 * Gets the field of a control ID.
		 *
		 * \param _wId The control ID.
		 * \param _vfField Holds the returned field.
		 * \return Returns true if the control ID is the control (not the label) of a field.
		 */
		static inline bool									FieldOfControl( WORD _wId, CVideoOptionsFields::LSN_VIDEO_FIELD &_vfField ) {
			if ( _wId < CWinUtilities::LSN_VOWI_FIELD_FIRST || _wId > CWinUtilities::LSN_VOWI_FIELD_LAST ) { return false; }
			const uint32_t ui32Offset = _wId - CWinUtilities::LSN_VOWI_FIELD_FIRST;
			if ( !(ui32Offset & 1) ) { return false; }
			if ( (ui32Offset >> 1) >= CVideoOptionsFields::LSN_VF_TOTAL ) { return false; }
			_vfField = CVideoOptionsFields::LSN_VIDEO_FIELD( ui32Offset >> 1 );
			return true;
		}


		// == Enumerations.
		/** Metrics. */
		enum LSN_METRICS : LONG {
			LSN_PALETTE_DISPLAY_W							= 384,										/**< The width of the client area of the palette display, in pixels. */
			LSN_PALETTE_DISPLAY_H							= 96,										/**< The height of the client area of the palette display, in pixels. */
		};


	protected :
		// == Members.
		/** The layout for the window. */
		static LSW_WIDGET_LAYOUT							m_wlWindow[];
		/** The layout for a settings page. */
		static LSW_WIDGET_LAYOUT							m_wlSettingsPage[];


		// == Functions.
		/**
		 * Builds the layout of a page of fields.
		 *
		 * \param _vpkKind The kind of page.
		 * \param _iRightW The width of the right column, in dialog units.
		 * \param _vLayout Holds the returned layout.
		 */
		static void											BuildFieldPage( CVideoOptionsFields::LSN_VIDEO_PAGE_KIND _vpkKind, INT _iRightW, std::vector<LSW_WIDGET_LAYOUT> &_vLayout );

		/**
		 * Builds the layout of the Host Machine page:  each setting on the left with its description on the right.  The page lays the
		 *	rows out again once it knows the heights of the descriptions (see CVideoOptionsPage::LayOutDescriptions()).
		 *
		 * \param _iRightW The width of the right column, in dialog units.
		 * \param _vLayout Holds the returned layout.
		 */
		static void											BuildHostPage( INT _iRightW, std::vector<LSW_WIDGET_LAYOUT> &_vLayout );

		/**
		 * Gets the width of the right column of the pages, in dialog units:  just wide enough for the palette display (384 pixels plus its
		 *	border and scroll bar) inside its group.
		 *
		 * \param _pwParent The parent of the pages (a dialog with the same font as the pages).
		 * \return Returns the width of the right column, in dialog units.
		 */
		static INT											RightColumnWidth( CWidget * _pwParent );

		/**
		 * Creates the layout of a Restore Defaults button.
		 *
		 * \param _wId The control ID of the button.
		 * \param _iLeft The left of the button.
		 * \param _iTop The top of the button.
		 * \param _pcToolTip The tool tip of the button (UTF-8).
		 * \return Returns the layout of the button.
		 */
		static LSW_WIDGET_LAYOUT							DefaultsButton( WORD _wId, INT _iLeft, INT _iTop, const char * _pcToolTip );

		/**
		 * Adds a column of groups to the layout of a page.
		 *
		 * \param _pfgGroups The groups.
		 * \param _sTotal The number of groups.
		 * \param _iLeft The left of the column.
		 * \param _iTop The top of the column.
		 * \param _iWidth The width of the column.
		 * \param _wFirstGroupId The control ID of the first group box.
		 * \param _vLayout The layout to which to add the column.
		 * \return Returns the bottom of the column.
		 */
		static INT											AddColumn( const CVideoOptionsFields::LSN_FIELD_GROUP * _pfgGroups, size_t _sTotal, INT _iLeft, INT _iTop, INT _iWidth,
			WORD _wFirstGroupId, std::vector<LSW_WIDGET_LAYOUT> &_vLayout );
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The layout for the Find Loop Points dialog.
 */

#pragma once

#include "../../LSNLSpiroNes.h"
#include "../../Wav/LSNWavEditor.h"
#include "../WinUtilities/LSNWinUtilities.h"

#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace lsn {

	/**
	 * Class CWavEditorFindLoopsWindowLayout
	 * \brief The layout for the Find Loop Points dialog.
	 *
	 * Description: The layout for the Find Loop Points dialog.
	 */
	class CWavEditorFindLoopsWindowLayout {
	public :
		// == Types.
		/** The data passed to and returned from the Find Loop Points dialog. */
		struct LSN_FIND_LOOPS_DATA {
			const std::vector<CWavEditor::LSN_METADATA> *			pvMetadata = nullptr;						/**< The metadata to search. */
			size_t													sEnd = SIZE_MAX;							/**< Returns the index of the metadata entry that ends the selected loop range. */
		};


		// == Functions.
		/**
		 * Creates the Find Loop Points dialog.
		 *
		 * \param _pwParent The parent of the dialog.
		 * \param _fldData The metadata to search and the returned ending point.
		 * \return Returns TRUE if a loop range was applied.
		 */
		static BOOL													CreateFindLoopsDialog( CWidget * _pwParent, LSN_FIND_LOOPS_DATA &_fldData );


	protected :
		// == Members.
		/** The layout for the Find Loop Points dialog. */
		static LSW_WIDGET_LAYOUT									m_wlDialog[];
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

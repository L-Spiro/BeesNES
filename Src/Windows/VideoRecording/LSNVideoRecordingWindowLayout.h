#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The layout for the Video Recording Settings dialog.
 */

#pragma once

#include "../../LSNLSpiroNes.h"
#include "../../Options/LSNVideoExportOptions.h"
#include "../../Video/LSNVideoExporter.h"
#include "../WinUtilities/LSNWinUtilities.h"

#include <Layout/LSWMenuLayout.h>
#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

#include <string>

using namespace lsw;

namespace lsn {

	/**
	 * Class CVideoRecordingWindowLayout
	 * \brief The layout for the Video Recording Settings dialog.
	 *
	 * Description: The layout for the Video Recording Settings dialog.
	 */
	class CVideoRecordingWindowLayout {
	public :
		// == Types.
		/**
		 * Struct LSN_VIDEO_RECORDING_DATA
		 * \brief The data passed to the Video Recording Settings dialog.
		 *
		 * Description: The data passed to the Video Recording Settings dialog.
		 */
		struct LSN_VIDEO_RECORDING_DATA {
			LSN_VIDEO_EXPORT_OPTIONS *						pveoOptions = nullptr;						/**< The options to edit.  They are changed only if the dialog is closed with OK. */
			CVideoExporter::LSN_RATE						rConsoleRate;								/**< The frame rate of the current console (shown when matching the console). */
			std::wstring									wsDefaultFolder;							/**< The folder into which movies are saved when no folder is set. */
		};


		// == Functions.
		/**
		 * Creates the Video Recording Settings dialog.
		 *
		 * \param _pwParent The parent of the dialog.
		 * \param _veoOptions The options to edit.  They are changed only if the dialog is closed with OK.
		 * \param _rConsoleRate The frame rate of the current console.
		 * \param _wsDefaultFolder The folder into which movies are saved when no folder is set.
		 * \return Returns TRUE if the dialog was closed with OK.
		 */
		static BOOL											CreateVideoRecordingDialog( CWidget * _pwParent, LSN_VIDEO_EXPORT_OPTIONS &_veoOptions,
			CVideoExporter::LSN_RATE _rConsoleRate, const std::wstring &_wsDefaultFolder );


	protected :
		// == Members.
		/** The layout for the Video Recording Settings window. */
		static LSW_WIDGET_LAYOUT							m_wlWindow[];
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

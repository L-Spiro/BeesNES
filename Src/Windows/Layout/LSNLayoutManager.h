#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The layout manager provides the classes for the windows defined in this project.
 */

#pragma once

#include <Layout/LSWLayoutManager.h>

using namespace lsw;

namespace lsn {

	enum LSN_LAYOUT_TYPES : DWORD {
		LSN_LT_MAIN_WINDOW						= LSW_LT_CUSTOM,
		LSN_LT_SELECT_ROM_DIALOG,
		LSN_LT_INPUT_DIALOG,
		LSN_LT_GLOBAL_INPUT_PAGE,
		LSN_LT_PER_GAME_INPUT_PAGE,
		LSN_LT_CONTROLLER_SETUP_DIALOG,
		LSN_LT_STD_CONTROLLER_PAGE,

		LSN_LT_AUDIO_OPTIONS_DIALOG,
		LSN_LT_AUDIO_OPTIONS_GLOBAL_PAGE,
		LSN_LT_AUDIO_OPTIONS_PERGAME_PAGE,
		LSN_LT_AUDIO_OPTIONS_RECORDING_PAGE,

		LSN_LT_PATCH_DIALOG,
		LSN_LT_PATCH_PAGE_TOP,
		LSN_LT_PATCH_PAGE_BOTTOM,

		LSN_LT_WAV_EDITOR_WINDOW,
		LSN_LT_WAV_EDITOR_FILES,
		LSN_LT_WAV_EDITOR_AUDIO_CHARACTERISTICS,
		LSN_LT_WAV_EDITOR_SEQUENCING,
		LSN_LT_WAV_EDITOR_FILE_SETTINGS,
		LSN_LT_WAV_EDITOR_OUTPUT,
	};

	class CLayoutManager : public lsw::CLayoutManager {
	public :
		// == Functions.
		// Creates a class based on its type.
		virtual CWidget *						CreateWidget( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data );
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

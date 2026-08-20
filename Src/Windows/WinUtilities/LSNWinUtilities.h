#ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Utility functions relating to the OS.
 */

#pragma once

#include "../../LSNLSpiroNes.h"
#include "../../Utilities/LSNStream.h"
#include <ComboBox/LSWComboBox.h>
#include <Helpers/LSWInputListenerBase.h>
#include <Widget/LSWWidget.h>

#include <map>
#include <shlobj.h>
#include <vector>

using namespace lsw;

namespace lsn {

	/**
	 * Class CWinUtilities
	 * \brief Utility functions relating to the OS.
	 *
	 * Description: Utility functions relating to the OS.
	 */
	class CWinUtilities {
	public :
		// == Enumerations.
		/** Custom window messages. */
		enum LSN_MSG : uint16_t {
#if defined( LSN_WINDOWS )
			LSN_CLOSE_PATCHER									= WM_USER + 1,
			LSN_CLOSE_WAV_EDITOR,
			LSN_UPDATE_HZ,
			LSN_UPDATE_PATCHER_DESC,
			LSN_USB_CONTROLLER_INPUT,
			LSN_GPU_DRAW,
#endif	// #if defined( LSN_WINDOWS )
		};

#if defined( LSN_WINDOWS )
		/** Centralized control ID's. */
		enum LSN_CONTROL_IDS : WORD {
			// Specifically numbered items.
			LSN_CSW_NONE										= 0,	// CControllerSetupWindowLayout::LSN_CONTROLLER_SETUP_WINDOW_IDS
			LSN_IWI_NONE										= 0,	// CInputWindowLayout::LSN_INPUT_WINDOW_IDS
			LSN_AOWI_NONE										= 0,	// CAudioOptionsWindowLayout::LSN_AUDIO_OPTIONS_WINDOW_IDS
			LSN_MWI_NONE										= 0,	// CMainWindowLayout::LSN_MAIN_WINDOW_IDS
			LSN_GOWI_NONE										= 0,	// CGraphicsOptionsWindowLayout::LSN_GRAPHICS_OPTIONS_WINDOW_IDS.
			LSN_PWI_NONE										= 0,	// CPatchWindowLayout::LSN_PATCH_WINDOW_IDS
			LSN_SFI_NONE										= 0,	// CSelectRomDialogLayout::LSN_SELECT_FILE_IDS
			LSN_WEWI_NONE										= 0,	// CWavEditorWindowLayout::LSN_WAV_EDITOR_WINDOW_IDS
			// CStdControllerPageLayout::LSN_STD_CONTROLLER_PAGE_IDS ====== //
			LSN_SCPI_NONE										= 0,

			LSN_SCPI_GROUP										= 0,
			LSN_SCPI_LABEL										= 1,
			LSN_SCPI_BUTTON										= 2,
			LSN_SCPI_TURBO_LABEL								= 3,
			LSN_SCPI_TURBO_BUTTON								= 4,
			LSN_SCPI_TURBO_COMBO								= 5,
			LSN_SCPI_TURBO_DEADZONE_LABEL						= 6,
			LSN_SCPI_TURBO_DEADZONE_TRACKBAR					= 7,
			// ============================================================ //


			// CControllerSetupWindowLayout::LSN_CONTROLLER_SETUP_WINDOW_IDS
			//LSN_CSW_NONE										= 0,
			LSN_CSW_MAINWINDOW,
			LSN_CSW_TAB,
			LSN_CSW_OK,
			LSN_CSW_CANCEL,

			LSN_CSW_MAIN_PANEL,
			LSN_CSW_RAPID_PANEL,


			// CInputWindowLayout::LSN_INPUT_WINDOW_IDS
			//LSN_IWI_NONE										= 0,
			LSN_IWI_MAINWINDOW,
			LSN_IWI_TAB,
			LSN_IWI_OK,
			LSN_IWI_CANCEL,

			LSN_IWI_GENERAL_GROUP,
			LSN_IWI_GENERAL_CONSOLE_TYPE_LABEL,
			LSN_IWI_GENERAL_CONSOLE_TYPE_COMBO,
			LSN_IWI_GENERAL_USE_GLOBAL_CHECK,

			LSN_IWI_PER_GAME_SETTINGS_PANEL,
			LSN_IWI_GLOBAL_SETTINGS_PANEL,
			LSN_IWI_SETTINGS_GENERAL_GROUP,
			LSN_IWI_SETTINGS_CONSOLE_TYPE_LABEL,
			LSN_IWI_SETTINGS_CONSOLE_TYPE_COMBO,
			LSN_IWI_SETTINGS_USE_GLOBAL_CHECK,
			LSN_IWI_SETTINGS_SETTINGS_GROUP,
			LSN_IWI_SETTINGS_PLAYER_1_LABEL,
			LSN_IWI_SETTINGS_PLAYER_1_COMBO,
			LSN_IWI_SETTINGS_PLAYER_1_BUTTON,
			LSN_IWI_SETTINGS_PLAYER_2_LABEL,
			LSN_IWI_SETTINGS_PLAYER_2_COMBO,
			LSN_IWI_SETTINGS_PLAYER_2_BUTTON,

			LSN_IWI_SETTINGS_USE_FOUR_SCORE_CHECK,
			LSN_IWI_SETTINGS_EXPAND_LABEL,
			LSN_IWI_SETTINGS_EXPAND_COMBO,
			LSN_IWI_SETTINGS_EXPAND_BUTTON,

			LSN_IWI_SETTINGS_PLAYER_3_LABEL,
			LSN_IWI_SETTINGS_PLAYER_3_COMBO,
			LSN_IWI_SETTINGS_PLAYER_3_BUTTON,

			LSN_IWI_SETTINGS_PLAYER_4_LABEL,
			LSN_IWI_SETTINGS_PLAYER_4_COMBO,
			LSN_IWI_SETTINGS_PLAYER_4_BUTTON,


			// CStdControllerPageLayout::LSN_STD_CONTROLLER_PAGE_IDS
			/*LSN_SCPI_NONE								= 0,

			LSN_SCPI_GROUP								= 0,
			LSN_SCPI_LABEL								= 1,
			LSN_SCPI_BUTTON								= 2,
			LSN_SCPI_TURBO_LABEL						= 3,
			LSN_SCPI_TURBO_BUTTON						= 4,
			LSN_SCPI_TURBO_COMBO						= 5,
			LSN_SCPI_TURBO_DEADZONE_LABEL				= 6,
			LSN_SCPI_TURBO_DEADZONE_TRACKBAR			= 7,*/
			LSN_SCPI_MAINWINDOW,

			LSN_SCPI_DPAD_GROUP,

			LSN_SCPI_BUTTON_UP_START,
			LSN_SCPI_BUTTON_UP_GROUP							= LSN_SCPI_BUTTON_UP_START + LSN_SCPI_GROUP,
			LSN_SCPI_BUTTON_UP_LABEL							= LSN_SCPI_BUTTON_UP_START + LSN_SCPI_LABEL,
			LSN_SCPI_BUTTON_UP_BUTTON							= LSN_SCPI_BUTTON_UP_START + LSN_SCPI_BUTTON,
			LSN_SCPI_BUTTON_UP_TURBO_LABEL						= LSN_SCPI_BUTTON_UP_START + LSN_SCPI_TURBO_LABEL,
			LSN_SCPI_BUTTON_UP_TURBO_BUTTON						= LSN_SCPI_BUTTON_UP_START + LSN_SCPI_TURBO_BUTTON,
			LSN_SCPI_BUTTON_UP_TURBO_COMBO						= LSN_SCPI_BUTTON_UP_START + LSN_SCPI_TURBO_COMBO,
			LSN_SCPI_BUTTON_UP_DEADZONE_LABEL					= LSN_SCPI_BUTTON_UP_START + LSN_SCPI_TURBO_DEADZONE_LABEL,
			LSN_SCPI_BUTTON_UP_DEADZONE_TRACKBAR				= LSN_SCPI_BUTTON_UP_START + LSN_SCPI_TURBO_DEADZONE_TRACKBAR,

			LSN_SCPI_BUTTON_LEFT_START,
			LSN_SCPI_BUTTON_LEFT_GROUP							= LSN_SCPI_BUTTON_LEFT_START + LSN_SCPI_GROUP,
			LSN_SCPI_BUTTON_LEFT_LABEL							= LSN_SCPI_BUTTON_LEFT_START + LSN_SCPI_LABEL,
			LSN_SCPI_BUTTON_LEFT_BUTTON							= LSN_SCPI_BUTTON_LEFT_START + LSN_SCPI_BUTTON,
			LSN_SCPI_BUTTON_LEFT_TURBO_LABEL					= LSN_SCPI_BUTTON_LEFT_START + LSN_SCPI_TURBO_LABEL,
			LSN_SCPI_BUTTON_LEFT_TURBO_BUTTON					= LSN_SCPI_BUTTON_LEFT_START + LSN_SCPI_TURBO_BUTTON,
			LSN_SCPI_BUTTON_LEFT_TURBO_COMBO					= LSN_SCPI_BUTTON_LEFT_START + LSN_SCPI_TURBO_COMBO,
			LSN_SCPI_BUTTON_LEFT_DEADZONE_LABEL					= LSN_SCPI_BUTTON_LEFT_START + LSN_SCPI_TURBO_DEADZONE_LABEL,
			LSN_SCPI_BUTTON_LEFT_DEADZONE_TRACKBAR				= LSN_SCPI_BUTTON_LEFT_START + LSN_SCPI_TURBO_DEADZONE_TRACKBAR,

			LSN_SCPI_BUTTON_RIGHT_START,
			LSN_SCPI_BUTTON_RIGHT_GROUP							= LSN_SCPI_BUTTON_RIGHT_START + LSN_SCPI_GROUP,
			LSN_SCPI_BUTTON_RIGHT_LABEL							= LSN_SCPI_BUTTON_RIGHT_START + LSN_SCPI_LABEL,
			LSN_SCPI_BUTTON_RIGHT_BUTTON						= LSN_SCPI_BUTTON_RIGHT_START + LSN_SCPI_BUTTON,
			LSN_SCPI_BUTTON_RIGHT_TURBO_LABEL					= LSN_SCPI_BUTTON_RIGHT_START + LSN_SCPI_TURBO_LABEL,
			LSN_SCPI_BUTTON_RIGHT_TURBO_BUTTON					= LSN_SCPI_BUTTON_RIGHT_START + LSN_SCPI_TURBO_BUTTON,
			LSN_SCPI_BUTTON_RIGHT_TURBO_COMBO					= LSN_SCPI_BUTTON_RIGHT_START + LSN_SCPI_TURBO_COMBO,
			LSN_SCPI_BUTTON_RIGHT_DEADZONE_LABEL				= LSN_SCPI_BUTTON_RIGHT_START + LSN_SCPI_TURBO_DEADZONE_LABEL,
			LSN_SCPI_BUTTON_RIGHT_DEADZONE_TRACKBAR				= LSN_SCPI_BUTTON_RIGHT_START + LSN_SCPI_TURBO_DEADZONE_TRACKBAR,

			LSN_SCPI_BUTTON_DOWN_START,
			LSN_SCPI_BUTTON_DOWN_GROUP							= LSN_SCPI_BUTTON_DOWN_START + LSN_SCPI_GROUP,
			LSN_SCPI_BUTTON_DOWN_LABEL							= LSN_SCPI_BUTTON_DOWN_START + LSN_SCPI_LABEL,
			LSN_SCPI_BUTTON_DOWN_BUTTON							= LSN_SCPI_BUTTON_DOWN_START + LSN_SCPI_BUTTON,
			LSN_SCPI_BUTTON_DOWN_TURBO_LABEL					= LSN_SCPI_BUTTON_DOWN_START + LSN_SCPI_TURBO_LABEL,
			LSN_SCPI_BUTTON_DOWN_TURBO_BUTTON					= LSN_SCPI_BUTTON_DOWN_START + LSN_SCPI_TURBO_BUTTON,
			LSN_SCPI_BUTTON_DOWN_TURBO_COMBO					= LSN_SCPI_BUTTON_DOWN_START + LSN_SCPI_TURBO_COMBO,
			LSN_SCPI_BUTTON_DOWN_DEADZONE_LABEL					= LSN_SCPI_BUTTON_DOWN_START + LSN_SCPI_TURBO_DEADZONE_LABEL,
			LSN_SCPI_BUTTON_DOWN_DEADZONE_TRACKBAR				= LSN_SCPI_BUTTON_DOWN_START + LSN_SCPI_TURBO_DEADZONE_TRACKBAR,

			LSN_SCPI_SS_GROUP,

			LSN_SCPI_BUTTON_SELECT_START,
			LSN_SCPI_BUTTON_SELECT_GROUP						= LSN_SCPI_BUTTON_SELECT_START + LSN_SCPI_GROUP,
			LSN_SCPI_BUTTON_SELECT_LABEL						= LSN_SCPI_BUTTON_SELECT_START + LSN_SCPI_LABEL,
			LSN_SCPI_BUTTON_SELECT_BUTTON						= LSN_SCPI_BUTTON_SELECT_START + LSN_SCPI_BUTTON,
			LSN_SCPI_BUTTON_SELECT_TURBO_LABEL					= LSN_SCPI_BUTTON_SELECT_START + LSN_SCPI_TURBO_LABEL,
			LSN_SCPI_BUTTON_SELECT_TURBO_BUTTON					= LSN_SCPI_BUTTON_SELECT_START + LSN_SCPI_TURBO_BUTTON,
			LSN_SCPI_BUTTON_SELECT_TURBO_COMBO					= LSN_SCPI_BUTTON_SELECT_START + LSN_SCPI_TURBO_COMBO,
			LSN_SCPI_BUTTON_SELECT_DEADZONE_LABEL				= LSN_SCPI_BUTTON_SELECT_START + LSN_SCPI_TURBO_DEADZONE_LABEL,
			LSN_SCPI_BUTTON_SELECT_DEADZONE_TRACKBAR			= LSN_SCPI_BUTTON_SELECT_START + LSN_SCPI_TURBO_DEADZONE_TRACKBAR,

			LSN_SCPI_BUTTON_START_START,
			LSN_SCPI_BUTTON_START_GROUP							= LSN_SCPI_BUTTON_START_START + LSN_SCPI_GROUP,
			LSN_SCPI_BUTTON_START_LABEL							= LSN_SCPI_BUTTON_START_START + LSN_SCPI_LABEL,
			LSN_SCPI_BUTTON_START_BUTTON						= LSN_SCPI_BUTTON_START_START + LSN_SCPI_BUTTON,
			LSN_SCPI_BUTTON_START_TURBO_LABEL					= LSN_SCPI_BUTTON_START_START + LSN_SCPI_TURBO_LABEL,
			LSN_SCPI_BUTTON_START_TURBO_BUTTON					= LSN_SCPI_BUTTON_START_START + LSN_SCPI_TURBO_BUTTON,
			LSN_SCPI_BUTTON_START_TURBO_COMBO					= LSN_SCPI_BUTTON_START_START + LSN_SCPI_TURBO_COMBO,
			LSN_SCPI_BUTTON_START_DEADZONE_LABEL				= LSN_SCPI_BUTTON_START_START + LSN_SCPI_TURBO_DEADZONE_LABEL,
			LSN_SCPI_BUTTON_START_DEADZONE_TRACKBAR				= LSN_SCPI_BUTTON_START_START + LSN_SCPI_TURBO_DEADZONE_TRACKBAR,

			LSN_SCPI_BUTTON_GROUP,

			LSN_SCPI_BUTTON_A_START,
			LSN_SCPI_BUTTON_A_GROUP								= LSN_SCPI_BUTTON_A_START + LSN_SCPI_GROUP,
			LSN_SCPI_BUTTON_A_LABEL								= LSN_SCPI_BUTTON_A_START + LSN_SCPI_LABEL,
			LSN_SCPI_BUTTON_A_BUTTON							= LSN_SCPI_BUTTON_A_START + LSN_SCPI_BUTTON,
			LSN_SCPI_BUTTON_A_TURBO_LABEL						= LSN_SCPI_BUTTON_A_START + LSN_SCPI_TURBO_LABEL,
			LSN_SCPI_BUTTON_A_TURBO_BUTTON						= LSN_SCPI_BUTTON_A_START + LSN_SCPI_TURBO_BUTTON,
			LSN_SCPI_BUTTON_A_TURBO_COMBO						= LSN_SCPI_BUTTON_A_START + LSN_SCPI_TURBO_COMBO,
			LSN_SCPI_BUTTON_A_DEADZONE_LABEL					= LSN_SCPI_BUTTON_A_START + LSN_SCPI_TURBO_DEADZONE_LABEL,
			LSN_SCPI_BUTTON_A_DEADZONE_TRACKBAR					= LSN_SCPI_BUTTON_A_START + LSN_SCPI_TURBO_DEADZONE_TRACKBAR,

			LSN_SCPI_BUTTON_B_START,
			LSN_SCPI_BUTTON_B_GROUP								= LSN_SCPI_BUTTON_B_START + LSN_SCPI_GROUP,
			LSN_SCPI_BUTTON_B_LABEL								= LSN_SCPI_BUTTON_B_START + LSN_SCPI_LABEL,
			LSN_SCPI_BUTTON_B_BUTTON							= LSN_SCPI_BUTTON_B_START + LSN_SCPI_BUTTON,
			LSN_SCPI_BUTTON_B_TURBO_LABEL						= LSN_SCPI_BUTTON_B_START + LSN_SCPI_TURBO_LABEL,
			LSN_SCPI_BUTTON_B_TURBO_BUTTON						= LSN_SCPI_BUTTON_B_START + LSN_SCPI_TURBO_BUTTON,
			LSN_SCPI_BUTTON_B_TURBO_COMBO						= LSN_SCPI_BUTTON_B_START + LSN_SCPI_TURBO_COMBO,
			LSN_SCPI_BUTTON_B_DEADZONE_LABEL					= LSN_SCPI_BUTTON_B_START + LSN_SCPI_TURBO_DEADZONE_LABEL,
			LSN_SCPI_BUTTON_B_DEADZONE_TRACKBAR					= LSN_SCPI_BUTTON_B_START + LSN_SCPI_TURBO_DEADZONE_TRACKBAR,



			LSN_SCPI_INPUT_DEVICES_GROUP,
			LSN_SCPI_INPUT_DEVICES_LISTVIEW,

			LSN_SCPI_INPUT_DPAD_DOT,

			LSN_SCPI_QUICK_CONFIGURE_GROUP,
			LSN_SCPI_QUICK_CONFIGURE_CLEAR_ALL_BUTTON,
			LSN_SCPI_QUICK_CONFIGURE_SET_ALL_BUTTON,
			LSN_SCPI_QUICK_CONFIGURE_SET_TURBO_BUTTON,
			LSN_SCPI_QUICK_CONFIGURE_DASH_LABEL,
			
			LSN_SCPI_QUICK_CONFIGURE_PRESET_GROUP,
			LSN_SCPI_QUICK_CONFIGURE_PRESET_COMBO,

			LSN_SCPI_OK,
			LSN_SCPI_CANCEL,


			// CAudioOptionsWindowLayout::LSN_AUDIO_OPTIONS_WINDOW_IDS
			//LSN_AOWI_NONE										= 0,
			LSN_AOWI_MAINWINDOW,
			LSN_AOWI_TAB,
			LSN_AOWI_OK,
			LSN_AOWI_CANCEL,


			LSN_AOWI_PAGE_GLOBAL,
			LSN_AOWI_PAGE_PERGAME,
			LSN_AOWI_PAGE_RECORDING,

			LSN_AOWI_PAGE_GENERAL_GROUP,
			LSN_AOWI_PAGE_GENERAL_DEVICE_LABEL,
			LSN_AOWI_PAGE_GENERAL_DEVICE_COMBO,
			LSN_AOWI_PAGE_GENERAL_ENABLE_CHECK,

			LSN_AOWI_PAGE_GENERAL_FORMAT_LABEL,
			LSN_AOWI_PAGE_GENERAL_FORMAT_COMBO,
			LSN_AOWI_PAGE_GENERAL_DITHER_CHECK,

			LSN_AOWI_PAGE_GENERAL_VOLUME_LABEL,
			LSN_AOWI_PAGE_GENERAL_VOLUME_TRACKBAR,
			LSN_AOWI_PAGE_GENERAL_VOLUME_EDIT,

			LSN_AOWI_PAGE_GENERAL_BG_VOL_LABEL,
			LSN_AOWI_PAGE_GENERAL_BG_VOL_TRACKBAR,
			LSN_AOWI_PAGE_GENERAL_BG_VOL_EDIT,


			LSN_AOWI_PAGE_CHARACTERISTICS_GROUP,
			LSN_AOWI_PAGE_CHARACTERISTICS_PRESETS_LABEL,
			LSN_AOWI_PAGE_CHARACTERISTICS_PRESETS_COMBO,

			LSN_AOWI_PAGE_CHARACTERISTICS_LPF_LABEL,
			LSN_AOWI_PAGE_CHARACTERISTICS_LPF_EDIT,
			LSN_AOWI_PAGE_CHARACTERISTICS_LPF_CHECK,

			LSN_AOWI_PAGE_CHARACTERISTICS_HPF0_LABEL,
			LSN_AOWI_PAGE_CHARACTERISTICS_HPF0_EDIT,
			LSN_AOWI_PAGE_CHARACTERISTICS_HPF0_CHECK,

			LSN_AOWI_PAGE_CHARACTERISTICS_HPF1_LABEL,
			LSN_AOWI_PAGE_CHARACTERISTICS_HPF1_EDIT,
			LSN_AOWI_PAGE_CHARACTERISTICS_HPF1_CHECK,

			LSN_AOWI_PAGE_CHARACTERISTICS_HPF2_LABEL,
			LSN_AOWI_PAGE_CHARACTERISTICS_HPF2_EDIT,
			LSN_AOWI_PAGE_CHARACTERISTICS_HPF2_CHECK,

			LSN_AOWI_PAGE_CHARACTERISTICS_VOLUME_LABEL,
			LSN_AOWI_PAGE_CHARACTERISTICS_VOLUME_TRACKBAR,
			LSN_AOWI_PAGE_CHARACTERISTICS_VOLUME_EDIT,

			LSN_AOWI_PAGE_CHARACTERISTICS_PULSE1_VOLUME_LABEL,
			LSN_AOWI_PAGE_CHARACTERISTICS_PULSE1_VOLUME_TRACKBAR,
			LSN_AOWI_PAGE_CHARACTERISTICS_PULSE1_VOLUME_EDIT,

			LSN_AOWI_PAGE_CHARACTERISTICS_PULSE2_VOLUME_LABEL,
			LSN_AOWI_PAGE_CHARACTERISTICS_PULSE2_VOLUME_TRACKBAR,
			LSN_AOWI_PAGE_CHARACTERISTICS_PULSE2_VOLUME_EDIT,

			LSN_AOWI_PAGE_CHARACTERISTICS_TRIANGLE_VOLUME_LABEL,
			LSN_AOWI_PAGE_CHARACTERISTICS_TRIANGLE_VOLUME_TRACKBAR,
			LSN_AOWI_PAGE_CHARACTERISTICS_TRIANGLE_VOLUME_EDIT,

			LSN_AOWI_PAGE_CHARACTERISTICS_NOISE_VOLUME_LABEL,
			LSN_AOWI_PAGE_CHARACTERISTICS_NOISE_VOLUME_TRACKBAR,
			LSN_AOWI_PAGE_CHARACTERISTICS_NOISE_VOLUME_EDIT,

			LSN_AOWI_PAGE_CHARACTERISTICS_DMC_VOLUME_LABEL,
			LSN_AOWI_PAGE_CHARACTERISTICS_DMC_VOLUME_TRACKBAR,
			LSN_AOWI_PAGE_CHARACTERISTICS_DMC_VOLUME_EDIT,

			LSN_AOWI_PAGE_CHARACTERISTICS_INVERT_CHECK,
			LSN_AOWI_PAGE_CHARACTERISTICS_NOISE_CHECK,
			LSN_AOWI_PAGE_CHARACTERISTICS_OLD_NES_CHECK,


			LSN_AOWI_PAGE_RAW_GROUP,
			LSN_AOWI_PAGE_RAW_PATH_LABEL,
			LSN_AOWI_PAGE_RAW_PATH_EDIT,
			LSN_AOWI_PAGE_RAW_PATH_BUTTON,

			LSN_AOWI_PAGE_RAW_HZ_LABEL,
			LSN_AOWI_PAGE_RAW_HZ_EDIT,
			LSN_AOWI_PAGE_RAW_ENABLE_CHECK,

			LSN_AOWI_PAGE_RAW_FORMAT_LABEL,
			LSN_AOWI_PAGE_RAW_FORMAT_COMBO,

			LSN_AOWI_PAGE_RAW_BITS_LABEL,
			LSN_AOWI_PAGE_RAW_BITS_COMBO,
			LSN_AOWI_PAGE_RAW_DITHER_CHECK,

			LSN_AOWI_PAGE_RAW_START_CONDITION_LABEL,
			LSN_AOWI_PAGE_RAW_START_CONDITION_COMBO,

			LSN_AOWI_PAGE_RAW_STOP_CONDITION_LABEL,
			LSN_AOWI_PAGE_RAW_STOP_CONDITION_COMBO,

			LSN_AOWI_PAGE_RAW_START_COMBO,
			LSN_AOWI_PAGE_RAW_STOP_COMBO,

			LSN_AOWI_PAGE_RAW_META_GROUP,
			LSN_AOWI_PAGE_RAW_META_CHECK,
			LSN_AOWI_PAGE_RAW_META_COMBO,

			LSN_AOWI_PAGE_RAW_META_ALL_CHECK,
			LSN_AOWI_PAGE_RAW_META_NONE_CHECK,
			LSN_AOWI_PAGE_RAW_META_PULSE1_CHECK,
			LSN_AOWI_PAGE_RAW_META_PULSE2_CHECK,
			LSN_AOWI_PAGE_RAW_META_TRI_CHECK,
			LSN_AOWI_PAGE_RAW_META_NOISE_CHECK,
			LSN_AOWI_PAGE_RAW_META_DMC_CHECK,
			LSN_AOWI_PAGE_RAW_META_STATUS_CHECK,
			LSN_AOWI_PAGE_RAW_META_FRAME_CHECK,

			LSN_AOWI_PAGE_RAW_META_P1_ON_OFF_CHECK,
			LSN_AOWI_PAGE_RAW_META_P2_ON_OFF_CHECK,
			LSN_AOWI_PAGE_RAW_META_TRI_ON_OFF_CHECK,
			LSN_AOWI_PAGE_RAW_META_NOISE_ON_OFF_CHECK,
			LSN_AOWI_PAGE_RAW_META_DMC_SET_CHECK,


			LSN_AOWI_PAGE_RAW_META_BINARY_RADIO,
			LSN_AOWI_PAGE_RAW_META_DEC_RADIO,
			LSN_AOWI_PAGE_RAW_META_BOTH_RADIO,


			LSN_AOWI_PAGE_OUT_GROUP,
			LSN_AOWI_PAGE_OUT_PATH_LABEL,
			LSN_AOWI_PAGE_OUT_PATH_EDIT,
			LSN_AOWI_PAGE_OUT_PATH_BUTTON,

			LSN_AOWI_PAGE_OUT_HZ_LABEL,
			LSN_AOWI_PAGE_OUT_HZ_EDIT,
			LSN_AOWI_PAGE_OUT_ENABLE_CHECK,

			LSN_AOWI_PAGE_OUT_FORMAT_LABEL,
			LSN_AOWI_PAGE_OUT_FORMAT_COMBO,

			LSN_AOWI_PAGE_OUT_BITS_LABEL,
			LSN_AOWI_PAGE_OUT_BITS_COMBO,
			LSN_AOWI_PAGE_OUT_DITHER_CHECK,

			LSN_AOWI_PAGE_OUT_START_CONDITION_LABEL,
			LSN_AOWI_PAGE_OUT_START_CONDITION_COMBO,

			LSN_AOWI_PAGE_OUT_STOP_CONDITION_LABEL,
			LSN_AOWI_PAGE_OUT_STOP_CONDITION_COMBO,

			LSN_AOWI_PAGE_OUT_START_COMBO,
			LSN_AOWI_PAGE_OUT_STOP_COMBO,

			LSN_AOWI_PAGE_OUT_META_GROUP,
			LSN_AOWI_PAGE_OUT_META_CHECK,
			LSN_AOWI_PAGE_OUT_META_COMBO,

			LSN_AOWI_PAGE_OUT_META_ALL_CHECK,
			LSN_AOWI_PAGE_OUT_META_NONE_CHECK,
			LSN_AOWI_PAGE_OUT_META_PULSE1_CHECK,
			LSN_AOWI_PAGE_OUT_META_PULSE2_CHECK,
			LSN_AOWI_PAGE_OUT_META_TRI_CHECK,
			LSN_AOWI_PAGE_OUT_META_NOISE_CHECK,
			LSN_AOWI_PAGE_OUT_META_DMC_CHECK,
			LSN_AOWI_PAGE_OUT_META_STATUS_CHECK,
			LSN_AOWI_PAGE_OUT_META_FRAME_CHECK,

			LSN_AOWI_PAGE_OUT_META_BINARY_RADIO,
			LSN_AOWI_PAGE_OUT_META_DEC_RADIO,
			LSN_AOWI_PAGE_OUT_META_BOTH_RADIO,

			LSN_AOWI_PAGE_OUT_DESC_LABEL,


			// CMainWindowLayout::LSN_MAIN_WINDOW_IDS
			//LSN_MWI_NONE										= 0,
			LSN_MWI_MAINWINDOW,
			LSN_MWI_REBAR0,
			LSN_MWI_TOOLBAR0,
			LSN_MWI_STATUSBAR,


			// CGraphicsOptionsWindowLayout::LSN_GRAPHICS_OPTIONS_WINDOW_IDS.
			//LSN_GOWI_NONE										= 0,
			LSN_GOWI_MAINWINDOW,
			LSN_GOWI_TAB,
			LSN_GOWI_OK,
			LSN_GOWI_CANCEL,


			// CPatchWindowLayout::LSN_PATCH_WINDOW_IDS
			//LSN_PWI_NONE										 = 0,
			LSN_PWI_MAINWINDOW,
			LSN_PWI_TOP_PAGE,
			LSN_PWI_BOTTOM_PAGE,
			LSN_PWI_SPLITTER,

			LSN_PWI_FILE_GROUP,
			LSN_PWI_FILE_IN_LABEL,
			LSN_PWI_FILE_IN_EDIT,
			LSN_PWI_FILE_IN_BUTTON,

			LSN_PWI_FILE_PATCH_LABEL,
			LSN_PWI_FILE_PATCH_TREELISTVIEW,
			LSN_PWI_FILE_PATCH_BUTTON,

			/*LSN_PWI_FILE_OUT_LABEL,
			LSN_PWI_FILE_OUT_EDIT,
			LSN_PWI_FILE_OUT_BUTTON,*/
			LSN_PWI_FILE_QUICK_PATCH_BUTTON,


			LSN_PWI_INFO_GROUP,
			LSN_PWI_INFO_CRC_LABEL,
			LSN_PWI_INFO_HEADERLESS_CRC_LABEL,
			LSN_PWI_INFO_ROM_CRC_LABEL,

			LSN_PWI_INFO_MD5_LABEL,
			LSN_PWI_INFO_HEADERLESS_MD5_LABEL,

			LSN_PWI_INFO_ROM_PGM_SIZE_LABEL,
			LSN_PWI_INFO_ROM_CHR_SIZE_LABEL,
			LSN_PWI_INFO_ROM_MIRROR_LABEL,


			LSN_PWI_BOTTOM_TABS,
			LSN_PWI_BOTTOM_TAB_DESC_EDIT,


			// CSelectRomDialogLayout::LSN_SELECT_FILE_IDS
			//LSN_SFI_NONE										= 0,
			LSN_SFI_DIALOG,
			LSN_SFI_GROUP,
			LSN_SFI_GROUPSHOW,
			LSN_SFI_LISTBOX,
			LSN_SFI_LABEL_SEARCH,
			LSN_SFI_EDIT_SEARCH,
			LSN_SFI_BUTTON_OK,
			LSN_SFI_BUTTON_CANCEL,
			LSN_SFI_LABEL_FOUND,


			// CWavEditorWindowLayout::LSN_WAV_EDITOR_WINDOW_IDS
			//LSN_WEWI_NONE										= 0,
			LSN_WEWI_MAINWINDOW,
			LSN_WEWI_OK,
			LSN_WEWI_CANCEL,
			LSN_WEWI_REBAR0,
			LSN_WEWI_TOOLBAR0,

			LSN_WEWI_SAVE,
			LSN_WEWI_LOAD,

			LSN_WEWI_FILES,
			LSN_WEWI_FILES_GROUP,
			LSN_WEWI_FILES_TREELISTVIEW,
			LSN_WEWI_FILES_ADD_BUTTON,
			LSN_WEWI_FILES_ADD_META_BUTTON,
			LSN_WEWI_FILES_REMOVE_BUTTON,
			LSN_WEWI_FILES_UP_BUTTON,
			LSN_WEWI_FILES_DOWN_BUTTON,

			LSN_WEWI_SEQ,
			LSN_WEWI_SEQ_GROUP,
			LSN_WEWI_SEQ_RANGE_GROUP,
			LSN_WEWI_SEQ_START_LABEL,
			LSN_WEWI_SEQ_START_COMBO,
			LSN_WEWI_SEQ_START_EDIT,
			LSN_WEWI_SEQ_START_EXACT_RADIO,
			LSN_WEWI_SEQ_START_MINUS_ONE_RADIO,
			LSN_WEWI_SEQ_START_SNAP_RADIO,

			/*LSN_WEWI_SEQ_END_LABEL,
			LSN_WEWI_SEQ_END_COMBO,
			LSN_WEWI_SEQ_END_EDIT,*/

			LSN_WEWI_SEQ_LOOP_RADIO,
			LSN_WEWI_SEQ_ONE_SHOT_RADIO,
			LSN_WEWI_SEQ_FIND_LOOPS_BUTTON,

			LSN_WEWI_SEQ_LOOPS_GROUP,
			LSN_WEWI_SEQ_LOOPS_STOP_LABEL,
			LSN_WEWI_SEQ_LOOPS_STOP_COMBO,
			LSN_WEWI_SEQ_LOOPS_STOP_EDIT,

			LSN_WEWI_SEQ_STOP_EXACT_RADIO,
			LSN_WEWI_SEQ_STOP_MINUS_ONE_RADIO,
			LSN_WEWI_SEQ_STOP_SNAP_RADIO,

			LSN_WEWI_SEQ_LOOPS_DELAY_LABEL,
			LSN_WEWI_SEQ_LOOPS_DELAY_EDIT,
			LSN_WEWI_SEQ_LOOPS_DELAY_SECONDS_LABEL,
			LSN_WEWI_SEQ_LOOPS_FADE_LABEL,
			LSN_WEWI_SEQ_LOOPS_FADE_EDIT,
			LSN_WEWI_SEQ_LOOPS_FADE_SECONDS_LABEL,
			LSN_WEWI_SEQ_LOOPS_DESC_LABEL,
			LSN_WEWI_SEQ_LOOPS_WARNING_LABEL,

			LSN_WEWI_SEQ_SILENCE_GROUP,
			LSN_WEWI_SEQ_SILENCE_OPEN_SIL_LABEL,
			LSN_WEWI_SEQ_SILENCE_OPEN_SIL_EDIT,
			LSN_WEWI_SEQ_SILENCE_OPEN_SECONDS_LABEL,
			LSN_WEWI_SEQ_SILENCE_TRAIL_LABEL,
			LSN_WEWI_SEQ_SILENCE_TRAIL_EDIT,
			LSN_WEWI_SEQ_SILENCE_SECONDS_LABEL,

			LSN_WEWI_SEQ_OPS_GROUP,
			LSN_WEWI_SEQ_OPS_TREELISTVEW,
			LSN_WEWI_SEQ_OPS_ADD_BUTTON,
			LSN_WEWI_SEQ_OPS_REMOVE_BUTTON,
			LSN_WEWI_SEQ_OPS_UP_BUTTON,
			LSN_WEWI_SEQ_OPS_DOWN_BUTTON,


			LSN_WEWI_FSETS,
			LSN_WEWI_FSETS_FDATA_GROUP,
			LSN_WEWI_FSETS_FDATA_NAME_LABEL,
			LSN_WEWI_FSETS_FDATA_NAME_EDIT,

			LSN_WEWI_FSETS_FDATA_PREFIX_LABEL,
			LSN_WEWI_FSETS_FDATA_PREFIX_EDIT,
			LSN_WEWI_FSETS_FDATA_POSTFIX_LABEL,
			LSN_WEWI_FSETS_FDATA_POSTFIX_EDIT,

			LSN_WEWI_FSETS_FDATA_HZ_LABEL,
			LSN_WEWI_FSETS_FDATA_HZ_COMBO,
			LSN_WEWI_FSETS_FDATA_HZ_EDIT,

			LSN_WEWI_FSETS_CHAR_GROUP,
			LSN_WEWI_FSETS_CHAR_PRESET_LABEL,
			LSN_WEWI_FSETS_CHAR_PRESET_COMBO,
			LSN_WEWI_FSETS_CHAR_SAVE_BUTTON,
			LSN_WEWI_FSETS_CHAR_LOAD_BUTTON,

			LSN_WEWI_FSETS_CHAR_VOL_LABEL,
			LSN_WEWI_FSETS_CHAR_VOL_EDIT,
			LSN_WEWI_FSETS_CHAR_LOCK_CHECK,
			LSN_WEWI_FSETS_CHAR_INV_CHECK,

			LSN_WEWI_FSETS_CHAR_LPF_CHECK,
			LSN_WEWI_FSETS_CHAR_LPF_EDIT,
			LSN_WEWI_FSETS_CHAR_LPF_TYPE_COMBO,
			LSN_WEWI_FSETS_CHAR_LPF_FALLOFF_LABEL,
			LSN_WEWI_FSETS_CHAR_LPF_FALLOFF_EDIT,
			LSN_WEWI_FSETS_CHAR_LPF_DB_LABEL,

			LSN_WEWI_FSETS_CHAR_HPF0_CHECK,
			LSN_WEWI_FSETS_CHAR_HPF0_EDIT,
			LSN_WEWI_FSETS_CHAR_HPF0_TYPE_COMBO,
			LSN_WEWI_FSETS_CHAR_HPF0_FALLOFF_LABEL,
			LSN_WEWI_FSETS_CHAR_HPF0_FALLOFF_EDIT,
			LSN_WEWI_FSETS_CHAR_HPF0_DB_LABEL,

			LSN_WEWI_FSETS_CHAR_HPF1_CHECK,
			LSN_WEWI_FSETS_CHAR_HPF1_EDIT,
			LSN_WEWI_FSETS_CHAR_HPF1_TYPE_COMBO,
			LSN_WEWI_FSETS_CHAR_HPF1_FALLOFF_LABEL,
			LSN_WEWI_FSETS_CHAR_HPF1_FALLOFF_EDIT,
			LSN_WEWI_FSETS_CHAR_HPF1_DB_LABEL,

			LSN_WEWI_FSETS_CHAR_HPF2_CHECK,
			LSN_WEWI_FSETS_CHAR_HPF2_EDIT,
			LSN_WEWI_FSETS_CHAR_HPF2_TYPE_COMBO,
			LSN_WEWI_FSETS_CHAR_HPF2_FALLOFF_LABEL,
			LSN_WEWI_FSETS_CHAR_HPF2_FALLOFF_EDIT,
			LSN_WEWI_FSETS_CHAR_HPF2_DB_LABEL,

			LSN_WEWI_FSETS_CHAR_RF_1_CHECK,
			LSN_WEWI_FSETS_CHAR_RF_2_CHECK,
			LSN_WEWI_FSETS_CHAR_SUNSOFT_5B_CHECK,

			LSN_WEWI_FSETS_MDATA_GROUP,
			LSN_WEWI_FSETS_MDATA_ARTIST_LABEL,
			LSN_WEWI_FSETS_MDATA_ARTIST_EDIT,
			LSN_WEWI_FSETS_MDATA_ALBUM_LABEL,
			LSN_WEWI_FSETS_MDATA_ALBUM_EDIT,
			LSN_WEWI_FSETS_MDATA_YEAR_LABEL,
			LSN_WEWI_FSETS_MDATA_YEAR_EDIT,
			LSN_WEWI_FSETS_MDATA_COMMENTS_LABEL,
			LSN_WEWI_FSETS_MDATA_COMMENTS_EDIT,

			LSN_WEWI_FSETS_APPLY_TO_ALL_BUTTON,

			LSN_WEWI_OUTPUT,
			LSN_WEWI_OUTPUT_GROUP,
			LSN_WEWI_OUTPUT_NOISE_VOLUME_GROUP,
			LSN_WEWI_OUTPUT_MAINS_CHECK,
			LSN_WEWI_OUTPUT_MAINS_COMBO,
			LSN_WEWI_OUTPUT_MAINS_VOL_LABEL,
			LSN_WEWI_OUTPUT_MAINS_VOL_EDIT,
			LSN_WEWI_OUTPUT_NOISE_CHECK,
			LSN_WEWI_OUTPUT_NOISE_COMBO,
			LSN_WEWI_OUTPUT_NOISE_VOL_LABEL,
			LSN_WEWI_OUTPUT_NOISE_VOL_EDIT,

			LSN_WEWI_OUTPUT_MASTER_VOL_GROUP,
			LSN_WEWI_OUTPUT_MASTER_VOL_ABSOLUTE_RADIO,
			LSN_WEWI_OUTPUT_MASTER_VOL_ABSOLUTE_EDIT,
			LSN_WEWI_OUTPUT_MASTER_VOL_NORMALIZE_RADIO,
			LSN_WEWI_OUTPUT_MASTER_VOL_NORMALIZE_EDIT,
			LSN_WEWI_OUTPUT_MASTER_VOL_LOUDNESS_RADIO,
			LSN_WEWI_OUTPUT_MASTER_VOL_LOUDNESS_EDIT,

			LSN_WEWI_OUTPUT_MASTER_FORMAT_GROUP,
			LSN_WEWI_OUTPUT_MASTER_FORMAT_HZ_LABEL,
			LSN_WEWI_OUTPUT_MASTER_FORMAT_HZ_EDIT,
			LSN_WEWI_OUTPUT_MASTER_FORMAT_FORMAT_LABEL,
			LSN_WEWI_OUTPUT_MASTER_FORMAT_FORMAT_COMBO,
			LSN_WEWI_OUTPUT_MASTER_FORMAT_BITS_LABEL,
			LSN_WEWI_OUTPUT_MASTER_FORMAT_BITS_COMBO,
			LSN_WEWI_OUTPUT_MASTER_FORMAT_DITHER_CHECK,
			LSN_WEWI_OUTPUT_MASTER_FORMAT_STEREO_COMBO,

			LSN_WEWI_OUTPUT_MASTER_PATH_GROUP,
			LSN_WEWI_OUTPUT_MASTER_PATH_EDIT,
			LSN_WEWI_OUTPUT_MASTER_PATH_BUTTON,
			LSN_WEWI_OUTPUT_MASTER_NUMBERED_CHECK,


			// == Context Menu == //
			LSN_WEWI_EXPAND_SELECTED,
			LSN_WEWI_EXPAND_ALL,
			LSN_WEWI_COLLAPSE_SELECTED,
			LSN_WEWI_COLLAPSE_ALL,

			LSN_WEWI_SELECT_ALL,
		};
#endif	// #if defined( LSN_WINDOWS )


		// == Types.
		/** An entry in a combo box. */
		struct LSN_COMBO_ENTRY {
			const wchar_t *										pwcName;						/**< The name of the entry. */
			LPARAM												lpParm;							/**< The parameter of the entry. */
		};

		/** An ::OleInitialize() wrapper. */
		struct LSN_OLEINITIALIZE {
			LSN_OLEINITIALIZE( LPVOID _pvReserved = nullptr ) :
				bOle( CWinUtilities::OleInitialize( _pvReserved ) ) {
			}
			~LSN_OLEINITIALIZE() {
				if ( bOle ) {
					::OleUninitialize();
					bOle = false;
				}
			}


			// == Functions.
			/**
			 * Determines whether the call to ::OleInitialize() was successful or not.
			 * 
			 * \return Returns true if the call to ::OleInitialize() was successful.
			 **/
			inline bool											Success() const { return bOle; }

		protected :
			// == Members.
			bool												bOle = false;
		};


		// == Functions.
		/**
		 * Fills a combo box with console types (Nintendo Entertainment System(R) and Famicom(R)).
		 *
		 * \param _pwComboBox The combo box to fill.
		 * \param _lpDefaultSelect The default selection.
		 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
		 */
		static bool												FillComboWithConsoleTypes( CWidget * _pwComboBox, LPARAM _lpDefaultSelect );

		/**
		 * Fills a combo box with Famicom(R) controller types (Standard Controller only).
		 *
		 * \param _pwComboBox The combo box to fill.
		 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
		 */
		static bool												FillComboWithControllerType_Famicom( CWidget * _pwComboBox );

		/**
		 * Fills a combo box with Nintendo Entertainment System(R) controller types.
		 *
		 * \param _pwComboBox The combo box to fill.
		 * \param _lpDefaultSelect The default selection.
		 * \param _lpSelectBackup The backup selection in case the default selection isn't in the list.
		 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
		 */
		static bool												FillComboWithControllerType_NES( CWidget * _pwComboBox, LPARAM _lpDefaultSelect, LPARAM _lpSelectBackup );

		/**
		 * Fills a combo box with Nintendo Entertainment System(R) Four Score expansion types.
		 *
		 * \param _pwComboBox The combo box to fill.
		 * \param _lpDefaultSelect The default selection.
		 * \param _lpSelectBackup The backup selection in case the default selection isn't in the list.
		 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
		 */
		static bool												FillComboWithControllerType_FourScore( CWidget * _pwComboBox, LPARAM _lpDefaultSelect, LPARAM _lpSelectBackup );

		/**
		 * Fills a combo box with Famicom(R) expansion types.
		 *
		 * \param _pwComboBox The combo box to fill.
		 * \param _lpDefaultSelect The default selection.
		 * \param _lpSelectBackup The backup selection in case the default selection isn't in the list.
		 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
		 */
		static bool												FillComboWithExpansion_Famicom( CWidget * _pwComboBox, LPARAM _lpDefaultSelect );

		/**
		 * Fills a combo box with WAV-file formats.
		 *
		 * \param _pwComboBox The combo box to fill.
		 * \param _lpDefaultSelect The default selection.
		 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
		 */
		static bool												FillComboWithWavFormats( CWidget * _pwComboBox, LPARAM _lpDefaultSelect );

		/**
		 * Fills a combo box with WAV-file streaming start conditions.
		 *
		 * \param _pwComboBox The combo box to fill.
		 * \param _lpDefaultSelect The default selection.
		 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
		 */
		static bool												FillComboWithWavStartConditions( CWidget * _pwComboBox, LPARAM _lpDefaultSelect );

		/**
		 * Fills a combo box with WAV-file streaming end conditions.
		 *
		 * \param _pwComboBox The combo box to fill.
		 * \param _lpDefaultSelect The default selection.
		 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
		 */
		static bool												FillComboWithWavEndConditions( CWidget * _pwComboBox, LPARAM _lpDefaultSelect );

		/**
		 * Fills a combo box with WAV-file PCM bit values.
		 *
		 * \param _pwComboBox The combo box to fill.
		 * \param _lpDefaultSelect The default selection.
		 * \param _bInclude32 If true, 32 bits is included.
		 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
		 */
		static bool												FillComboWithWavPcmBits( CWidget * _pwComboBox, LPARAM _lpDefaultSelect, bool _bInclude32 = false );

		/**
		 * Fills a combo box with WAV metadata formats.
		 *
		 * \param _pwComboBox The combo box to fill.
		 * \param _lpDefaultSelect The default selection.
		 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
		 */
		static bool												FillComboWithWavMetaDataFormats( CWidget * _pwComboBox, LPARAM _lpDefaultSelect );

		/**
		 * Fills a combo box with WAV Editor "Actual Hz".
		 *
		 * \param _pwComboBox The combo box to fill.
		 * \param _lpDefaultSelect The default selection.
		 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
		 */
		static bool												FillComboWithWavActialHz( CWidget * _pwComboBox, LPARAM _lpDefaultSelect );

		/**
		 * Fills a combo box with WAV Editor noise color.
		 *
		 * \param _pwComboBox The combo box to fill.
		 * \param _lpDefaultSelect The default selection.
		 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
		 */
		static bool												FillComboWithWavNoiseColor( CWidget * _pwComboBox, LPARAM _lpDefaultSelect );

		/**
		 * Fills a combo box with WAV Editor noise type.
		 *
		 * \param _pwComboBox The combo box to fill.
		 * \param _lpDefaultSelect The default selection.
		 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
		 */
		static bool												FillComboWithWavNoiseType( CWidget * _pwComboBox, LPARAM _lpDefaultSelect );

		/**
		 * Fills a combo box with WAV Editor filter type.
		 *
		 * \param _pwComboBox The combo box to fill.
		 * \param _lpDefaultSelect The default selection.
		 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
		 */
		static bool												FillComboWithWavFilterType( CWidget * _pwComboBox, LPARAM _lpDefaultSelect );

		/**
		 * Fills a combo box with WAV Editor mono-stereo-surround output types.
		 *
		 * \param _pwComboBox The combo box to fill.
		 * \param _lpDefaultSelect The default selection.
		 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
		 */
		static bool												FillComboWithWavStereoSettings( CWidget * _pwComboBox, LPARAM _lpDefaultSelect );

		/**
		 * Fills a combo box with WAV metadata formats.
		 *
		 * \param _pwComboBox The combo box to fill.
		 * \param _lpDefaultSelect The default selection.
		 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
		 */
		template <typename _tKey, typename _tType>
		static bool												FillComboWithMap( CWidget * _pwComboBox, const std::map<_tKey, _tType> &_mMap, LPARAM _lpDefaultSelect ) {
			std::vector<LSN_COMBO_ENTRY> vEntries;
			try {
				if ( _mMap.empty() ) { return FillComboBox( _pwComboBox, nullptr, 0, _lpDefaultSelect ); }

				vEntries.resize( _mMap.size() );
				for ( const auto & I : _mMap ) {
					vEntries[I].pwcName = reinterpret_cast<const wchar_t *>(I.second);
					vEntries[I].lpParm = LPARAM( I );
				}
				if ( FillComboBox( _pwComboBox, &vEntries[0], vEntries.size(), _lpDefaultSelect ) ) {
					lsw::CComboBox * pcbCombo = reinterpret_cast<lsw::CComboBox *>(_pwComboBox);
					pcbCombo->SetSel( 0, 0 );
					return true;
				}
				return false;
			}
			catch ( ... ) { return false; }
		}

		/**
		 * Fills a combo box with an array of UTF-16 strings.
		 *
		 * \param _pwComboBox The combo box to fill.
		 * \param _lpDefaultSelect The default selection.
		 * \return Returns true if the combo box will filled.  _pwComboBox must not be nullptr, must be of type CComboBox, and the adding of each item must succeed.
		 */
		template <typename _tType>
		static bool												FillComboWithStrings( CWidget * _pwComboBox, const std::vector<_tType> &_vStrings, LPARAM _lpDefaultSelect ) {
			std::vector<LSN_COMBO_ENTRY> vEntries;
			try {
				if ( !_vStrings.size() ) { return FillComboBox( _pwComboBox, nullptr, 0, _lpDefaultSelect ); }
				vEntries.resize( _vStrings.size() );
				for ( size_t I = 0; I < _vStrings.size(); ++I ) {
					vEntries[I].pwcName = reinterpret_cast<const wchar_t *>(_vStrings[I].c_str());
					vEntries[I].lpParm = LPARAM( I );
				}
				if ( FillComboBox( _pwComboBox, &vEntries[0], vEntries.size(), _lpDefaultSelect ) ) {
					lsw::CComboBox * pcbCombo = reinterpret_cast<lsw::CComboBox *>(_pwComboBox);
					pcbCombo->SetSel( 0, 0 );
					return true;
				}
				return false;
			}
			catch ( ... ) { return false; }
			return true;
		}
		
		/**
		 * Fills a combo box with the given array of LSN_COMBO_ENTRY structures.
		 *
		 * \param _pwComboBox The combo box to fill.
		 * \param _pceEntries The array of combo-box entries.
		 * \param _stTotal The total number of entries to which _pceEntries points.
		 * \param _lpDefaultSelect The default selection.
		 * \param _lpSelectBackup The backup selection in case the default selection isn't in the list.
		 * \return Returns true if _pwComboBox is not nullptr, it is of type CComboBox, and all entries were added.
		 */
		static bool												FillComboBox( CWidget * _pwComboBox, const LSN_COMBO_ENTRY * _pceEntries, size_t _stTotal, LPARAM _lpDefaultSelect, LPARAM _lpSelectBackup = 0 );

		/**
		 * Writes a UI key to a stream.
		 * 
		 * \param _kKey The key values to write.
		 * \param _sStream The stream to which to write the key values.
		 * \return Returns true if writing of the key information to the stream succeeded.
		 **/
		static bool												WriteUiKey( const lsw::LSW_KEY &_kKey, lsn::CStream &_sStream );

		/**
		 * Reads a UI key from a stream.
		 * 
		 * \param _kKey The key values to be filled by reading from the givens tream.
		 * \param _sStream The stream from which to read the key values.
		 * \return Returns true if reading of the key information from the stream succeeded.
		 **/
		static bool												ReadUiKey( lsw::LSW_KEY &_kKey, lsn::CStream &_sStream );

		/**
		 * Callback procedure for the folder browser dialog.
		 *
		 * \param _hWnd   The dialog window handle.
		 * \param _uMsg   The message.
		 * \param _lParam The message parameter.
		 * \param _lpData The application-defined data (initial path pointer).
		 * \return Returns 0 to continue default processing.
		 */
		static int CALLBACK										BrowseCallbackProc( HWND _hWnd, UINT _uMsg, LPARAM /*_lParam*/, LPARAM lpData ) {
			if ( _uMsg == BFFM_INITIALIZED && lpData ) {
				// lpData is a pointer to the initial folder path
				::SendMessageW( _hWnd, BFFM_SETSELECTIONW, TRUE, lpData );
			}
			return 0;
		}

		/**
		 * Initializes OLE for the current thread.
		 * 
		 * \param _pvReserved Reserved; must be nullptr.
		 * \return Returns true if OLE was initialized successfully, false otherwise.
		 */
		static bool												OleInitialize( LPVOID _pvReserved = nullptr ) {
			HRESULT hrResult = ::OleInitialize( _pvReserved );
			return (hrResult == S_OK || hrResult == S_FALSE);
		}


	protected :
	};

}	// namespace lsn

#endif	// #ifdef LSN_USE_WINDOWS

/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Localization macros for en-us (English (United States)).
 */


#pragma once

#define LSN_NES																Nintendo Entertainment System®
#define LSN_FAMICOM															Famicom®

#define LSN_OPTIONS															Options
#define LSN_ALL_SUPPORTED_FILES___NES____ZIP____NES___ZIP_					All Supported Files (*.nes, *.zip)\0*.nes;*.zip\0
#define LSN_NES_FILES____NES____NES_										NES Files (*.nes)\0*.nes\0
#define LSN_ZIP_FILES____ZIP____ZIP_										ZIP Files (*.zip)\0*.zip\0
#define LSN_ALL_FILES___________											All Files (*.*)\0*.*\0
#define LSN__FILE															&File
#define LSN__OPTIONS														&Options
#define LSN__TOOLS															&Tools
#define LSN__WINDOW															&Window
#define LSN__HELP															&Help
// File menu.
#define LSN_OPEN__ROM														Open R&OM
#define LSN_OPEN_REC_ENT													Open Re&cent
// Options menu.
#define LSN_VIDEO_SIZE														Video Si&ze
#define LSN_VIDEO_FILTER													Video &Filter
#define LSN_INPUT															&Input
#define LSN_VIDEO															&Video
// Video sizes.
#define LSN_VIDEO_SIZE_1X													1×
#define LSN_VIDEO_SIZE_2X													2×
#define LSN_VIDEO_SIZE_3X													3×
#define LSN_VIDEO_SIZE_4X													4×
#define LSN_VIDEO_SIZE_5X													5×
#define LSN_VIDEO_SIZE_6X													6×
// Video filters.
#define LSN_VIDEO_FILTER_NONE												&None
#define LSN_VIDEO_FILTER_NTSC_BLARGG										NTSC (&Blargg)
#define LSN_VIDEO_FILTER_PAL_BLARGG											&PAL (Blargg)
#define LSN_VIDEO_FILTER_AUTO_BLARGG										&NTSC/PAL Auto (Blargg)
#define LSN_VIDEO_FILTER_NTSC_CRT											NTSC-CRT (&LMP88959)
#define LSN_VIDEO_FILTER_AUTO_CRT											NTSC-CRT/PAL &Auto (LMP88959/Blargg)
// Tools menu.

// Main menu.
#define LSN_SELECT_ROM														Select ROM
#define LSN_FILE_LISTING													File Listing
#define LSN_SEARCH_															Search:
#define LSN_OK																OK
#define LSN_CANCEL															Cancel

// Input dialog.
#define LSN_INPUT_DIALOG													Configure Input
#define LSN_INPUT_GENERAL													General
#define LSN_INPUT_CONSOLE_TYPE_												Console Type:
#define LSN_INPUT_USE_GLOBAL												Use Global
#define LSN_INPUT_SETTINGS													Settings
#define LSN_INPUT_PLAYER_1_													Player 1:
#define LSN_INPUT_PLAYER_2_													Player 2:
#define LSN_INPUT_PLAYER_3_													Player 3:
#define LSN_INPUT_PLAYER_4_													Player 4:
#define LSN_INPUT_CONFIGURATION												Configuration

#define LSN_OPENAL_AL_NO_ERROR_DESC											There is not currently an error.
#define LSN_OPENAL_AL_INVALID_NAME_DESC										A bad name (ID) was passed to an OpenAL function.
#define LSN_OPENAL_AL_INVALID_ENUM_DESC										An invalid enum value was passed to an OpenAL function.
#define LSN_OPENAL_AL_INVALID_VALUE_DESC									An invalid value was passed to an OpenAL function.
#define LSN_OPENAL_AL_INVALID_OPERATION_DESC								The requested operation is not valid.
#define LSN_OPENAL_AL_OUT_OF_MEMORY_DESC									The requested operation resulted in OpenAL running out of memory.

#define LSN_OPENAL_ALC_NO_ERROR_DESC										There is not currently an error.
#define LSN_OPENAL_ALC_INVALID_DEVICE_DESC									A bad device was passed to an OpenAL function.
#define LSN_OPENAL_ALC_INVALID_CONTEXT_DESC									A bad context was passed to an OpenAL function.
#define LSN_OPENAL_ALC_INVALID_ENUM_DESC									An unknown enum value was passed to an OpenAL function.
#define LSN_OPENAL_ALC_INVALID_VALUE_DESC									An invalid value was passed to an OpenAL function.
#define LSN_OPENAL_ALC_OUT_OF_MEMORY_DESC									The requested operation resulted in OpenAL running out of memory.

#define LSN_OPENAL_ERROR_UNKNOWN											Unknown OpenAL error.
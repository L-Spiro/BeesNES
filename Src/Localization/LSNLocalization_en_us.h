/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Localization macros for en-us (English (United States)).
 */


#pragma once

#define LSN_NES																Nintendo Entertainment System\u00AE						// Nintendo Entertainment System(R)
#define LSN_FAMICOM															Famicom\u00AE											// Famicom(R)
#define LSN_SNES															Super Nintendo Entertainment System\u00AE				// Super Nintendo Entertainment System(R)
#define LSN_BEESNES															BeesNES

#define LSN_MICROSOFT_DIRECT3D_9											Microsoft® Direct3D® 9
#define LSN_MICROSOFT_DIRECT3D_12											Microsoft® Direct3D® 12
#define LSN_VULKAN__1														Vulkan® 1
#define LSN_METAL_															Metal®

#define LSN_PERFORMING_ONE_TIME_CONVERSION_OF_XML_DATABASE__PLEASE_WAIT_	Performing One-Time Conversion of XML Database. Please Wait.

#define LSN_OPTIONS_TXT														Options
//#define LSN_ALL_SUPPORTED_FILES___NES____ZIP____NES___ZIP_					All Supported Files (*.nes, *.zip)\0*.nes;*.zip\0
#define LSN_ALL_SUPPORTED_FILES___NES____ZIP____NES___ZIP_					All Supported Files (*.nes, *.nsf, *.zip)\0*.nes;0*.nsf;*.zip\0
#define LSN_NES_FILES____NES____NES_										NES Files (*.nes)\0*.nes\0
#define LSN_ZIP_FILES____ZIP____ZIP_										ZIP Files (*.zip)\0*.zip\0
#define LSN_NSF_FILES____NSF____NSF_										NSF Files (*.nsf)\0*.nsf\0
#define LSN_ALL_FILES___________											All Files (*.*)\0*.*\0
#define LSN_BWAV_FILES____BWAV____BWAV_										BWAV Projects (*.bwav)\0*.bwav\0
#define LSN__FILE															&File
#define LSN__GAME															&Game
#define LSN__OPTIONS														&Options
#define LSN__TOOLS															&Tools
#define LSN__WINDOW															&Window
#define LSN__HELP															&Help
// File menu.
#define LSN_OPEN__ROM														Open R&OM
#define LSN_OPEN_REC_ENT													Open Re&cent
// Game menu.
#define LSN_GAME_PAUSE														&Pause
#define LSN_GAME_UNPAUSE													&Unpause
#define LSN_GAME_RESET														&Reset
#define LSN_GAME_POWER_CYCLE												Power &Cycle
#define LSN_GAME_RELOAD_ROM													Re&load ROM
#define LSN_GAME_POWER_OFF													Power &Off
// Options menu.
#define LSN_VIDEO_SIZE														Video Si&ze
#define LSN_FULL_SCREEN														Full &Screen
#define LSN_VIDEO_FILTER													Video &Filter
#define LSN_REGION															&Region
#define LSN_INPUT															&Input
#define LSN_VIDEO															&Video
#define LSN_AUDIO															&Audio
// Video sizes.
#define LSN_VIDEO_SIZE_1X													1\u00D7
#define LSN_VIDEO_SIZE_1_75X												1.75\u00D7
#define LSN_VIDEO_SIZE_2X													2\u00D7
#define LSN_VIDEO_SIZE_3X													3\u00D7
#define LSN_VIDEO_SIZE_3_5X													3.5\u00D7
#define LSN_VIDEO_SIZE_4X													4\u00D7
#define LSN_VIDEO_SIZE_5X													5\u00D7
#define LSN_VIDEO_SIZE_5_25X												5.25\u00D7
#define LSN_VIDEO_SIZE_6X													6\u00D7
#define LSN_VIDEO_SIZE_7X													7\u00D7
#define LSN_VIDEO_SIZE_8X													8\u00D7
#define LSN_VIDEO_SIZE_8_75X												8.75\u00D7
#define LSN_VIDEO_SIZE_9X													9\u00D7
#define LSN_VIDEO_SIZE_10X													10\u00D7
#define LSN_VIDEO_SIZE_10_5X												10.5\u00D7
// Video filters.
#define LSN_VIDEO_FILTER_NONE												Pale&tte (Software)
#define LSN_VIDEO_FILTER_DX9_PALETTE										Pale&tte (LSN_MICROSOFT_DIRECT3D_9)
#define LSN_VIDEO_FILTER_DX12_PALETTE										Pale&tte (LSN_MICROSOFT_DIRECT3D_12)
#define LSN_VIDEO_FILTER_VULKAN_PALETTE										Pale&tte (LSN_VULKAN__1)
#define LSN_VIDEO_FILTER_METAL_PALETTE										Pale&tte (LSN_METAL_)
#define LSN_VIDEO_FILTER_NTSC_BLARGG										NTSC-M (LSN_NES) (Blar&gg)
#define LSN_VIDEO_FILTER_NTSC_L_SPIRO										NTSC-M (LSN_NES) (&L. Spiro)
#define LSN_VIDEO_FILTER_PAL_L_SPIRO										PAL-&I (PAL-A LSN_NES) (L. Spiro)
#define LSN_VIDEO_FILTER_DENDY_L_SPIRO										PAL-&D/K (Dendy Famiclone) (L. Spiro)//&SECAM-B/D/G/H/K/K1/L (L. Spiro)
#define LSN_VIDEO_FILTER_PALM_L_SPIRO										PAL-&M (Brazil Famiclone) (L. Spiro)
#define LSN_VIDEO_FILTER_PALN_L_SPIRO										PAL-&N (Argentina Famiclone) (L. Spiro)
#define LSN_VIDEO_FILTER_NTSC_CRT_FULL										NTSC-M (LSN_NES) (LMP&88959)
#define LSN_VIDEO_FILTER_PAL_CRT_FULL										PAL-I (PAL-A LSN_NES) (LMP88&959)
#define LSN_VIDEO_FILTER_DENDY_CRT_FULL										PAL-D/&K (Dendy Famiclone) (LMP88959)
#define LSN_VIDEO_FILTER_AUTO_CRT_FULL										NTSC/PAL Auto (LMP889&59)
#define LSN_VIDEO_FILTER_AUTO_LSPIRO										NTSC/PAL &Auto (L. Spiro)

#define LSN_VIDEO_FILTER_NTSC_BLARGG_DX9_UPSCALE							NTSC-M (LSN_NES) (Blar&gg) (LSN_MICROSOFT_DIRECT3D_9)
#define LSN_VIDEO_FILTER_NTSC_J_BLARGG_DX9_UPSCALE							NTSC-J (LSN_FAMICOM) (Bla&rgg) (LSN_MICROSOFT_DIRECT3D_9)
#define LSN_VIDEO_FILTER_NTSC_CRT_FULL_DX9_UPSCALE							NTSC-M (LSN_NES) (LMP&88959) (LSN_MICROSOFT_DIRECT3D_9)
#define LSN_VIDEO_FILTER_NTSC_J_CRT_FULL_DX9_UPSCALE						NTSC-J (LSN_FAMICOM) (LM&P88959) (LSN_MICROSOFT_DIRECT3D_9)
#define LSN_VIDEO_FILTER_PAL_CRT_FULL_DX9_UPSCALE							PAL-I (PAL-A LSN_NES) (LMP88&959) (LSN_MICROSOFT_DIRECT3D_9)
#define LSN_VIDEO_FILTER_DENDY_CRT_FULL_DX9_UPSCALE							PAL-D/&K (Dendy Famiclone) (LMP88959) (LSN_MICROSOFT_DIRECT3D_9)
#define LSN_VIDEO_FILTER_LSPIRO_NTSC_DX9_UPSCALE							NTSC-M (LSN_NES) (&L. Spiro) (LSN_MICROSOFT_DIRECT3D_9)
#define LSN_VIDEO_FILTER_LSPIRO_NTSC_J_DX9_UPSCALE							NTSC-&J (LSN_FAMICOM) (L. Spiro) (LSN_MICROSOFT_DIRECT3D_9)
#define LSN_VIDEO_FILTER_LSPIRO_PAL_DX9_UPSCALE								PAL-&I (PAL-A LSN_NES) (L. Spiro) (LSN_MICROSOFT_DIRECT3D_9)
#define LSN_VIDEO_FILTER_LSPIRO_DENDY_DX9_UPSCALE							PAL-&D/K (Dendy Famiclone) (L. Spiro) (LSN_MICROSOFT_DIRECT3D_9)
#define LSN_VIDEO_FILTER_LSPIRO_PALM_DX9_UPSCALE							PAL-&M (Brazil Famiclone) (L. Spiro) (LSN_MICROSOFT_DIRECT3D_9)
#define LSN_VIDEO_FILTER_LSPIRO_PALN_DX9_UPSCALE							PAL-&N (Argentina Famiclone) (L. Spiro) (LSN_MICROSOFT_DIRECT3D_9)
#define LSN_VIDEO_FILTER_AUTO_CRT_FULL_DX9_UPSCALE							NTSC/PAL Auto (LMP889&59) (LSN_MICROSOFT_DIRECT3D_9)
#define LSN_VIDEO_FILTER_AUTO_LSPIRO_DX9_UPSCALE							NTSC/PAL &Auto (L. Spiro) (LSN_MICROSOFT_DIRECT3D_9)

#define LSN_VIDEO_FILTER_NTSC_BLARGG_DX12_UPSCALE							NTSC-M (LSN_NES) (Blar&gg) (LSN_MICROSOFT_DIRECT3D_12)
#define LSN_VIDEO_FILTER_NTSC_J_BLARGG_DX12_UPSCALE							NTSC-J (LSN_FAMICOM) (Bla&rgg) (LSN_MICROSOFT_DIRECT3D_12)
#define LSN_VIDEO_FILTER_NTSC_CRT_FULL_DX12_UPSCALE							NTSC-M (LSN_NES) (LMP&8812512) (LSN_MICROSOFT_DIRECT3D_12)
#define LSN_VIDEO_FILTER_NTSC_J_CRT_FULL_DX12_UPSCALE						NTSC-J (LSN_FAMICOM) (LM&P88959) (LSN_MICROSOFT_DIRECT3D_12)
#define LSN_VIDEO_FILTER_PAL_CRT_FULL_DX12_UPSCALE							PAL-I (PAL-A LSN_NES) (LMP88&12512) (LSN_MICROSOFT_DIRECT3D_12)
#define LSN_VIDEO_FILTER_DENDY_CRT_FULL_DX12_UPSCALE							PAL-D/&K (Dendy Famiclone) (LMP88959) (LSN_MICROSOFT_DIRECT3D_12)
#define LSN_VIDEO_FILTER_LSPIRO_NTSC_DX12_UPSCALE							NTSC-M (LSN_NES) (&L. Spiro) (LSN_MICROSOFT_DIRECT3D_12)
#define LSN_VIDEO_FILTER_LSPIRO_NTSC_J_DX12_UPSCALE							NTSC-&J (LSN_FAMICOM) (L. Spiro) (LSN_MICROSOFT_DIRECT3D_12)
#define LSN_VIDEO_FILTER_LSPIRO_PAL_DX12_UPSCALE							PAL-&I (PAL-A LSN_NES) (L. Spiro) (LSN_MICROSOFT_DIRECT3D_12)
#define LSN_VIDEO_FILTER_LSPIRO_DENDY_DX12_UPSCALE							PAL-&D/K (Dendy Famiclone) (L. Spiro) (LSN_MICROSOFT_DIRECT3D_12)
#define LSN_VIDEO_FILTER_LSPIRO_PALM_DX12_UPSCALE							PAL-&M (Brazil Famiclone) (L. Spiro) (LSN_MICROSOFT_DIRECT3D_12)
#define LSN_VIDEO_FILTER_LSPIRO_PALN_DX12_UPSCALE							PAL-&N (Argentina Famiclone) (L. Spiro) (LSN_MICROSOFT_DIRECT3D_12)
#define LSN_VIDEO_FILTER_AUTO_CRT_FULL_DX12_UPSCALE							NTSC/PAL Auto (LMP889&59) (LSN_MICROSOFT_DIRECT3D_12)
#define LSN_VIDEO_FILTER_AUTO_LSPIRO_DX12_UPSCALE							NTSC/PAL &Auto (L. Spiro) (LSN_MICROSOFT_DIRECT3D_12)

#define LSN_VIDEO_FILTER_NTSC_BLARGG_VULKAN_UPSCALE							NTSC-M (LSN_NES) (Blar&gg) (LSN_VULKAN__1)
#define LSN_VIDEO_FILTER_NTSC_J_BLARGG_VULKAN_UPSCALE						NTSC-J (LSN_FAMICOM) (Bla&rgg) (LSN_VULKAN__1)
#define LSN_VIDEO_FILTER_NTSC_CRT_FULL_VULKAN_UPSCALE						NTSC-M (LSN_NES) (LMP&88959) (LSN_VULKAN__1)
#define LSN_VIDEO_FILTER_NTSC_J_CRT_FULL_VULKAN_UPSCALE						NTSC-J (LSN_FAMICOM) (LM&P88959) (LSN_VULKAN__1)
#define LSN_VIDEO_FILTER_PAL_CRT_FULL_VULKAN_UPSCALE						PAL-I (PAL-A LSN_NES) (LMP88&959) (LSN_VULKAN__1)
#define LSN_VIDEO_FILTER_DENDY_CRT_FULL_VULKAN_UPSCALE						PAL-D/&K (Dendy Famiclone) (LMP88959) (LSN_VULKAN__1)
#define LSN_VIDEO_FILTER_LSPIRO_NTSC_VULKAN_UPSCALE							NTSC-M (LSN_NES) (&L. Spiro) (LSN_VULKAN__1)
#define LSN_VIDEO_FILTER_LSPIRO_NTSC_J_VULKAN_UPSCALE						NTSC-&J (LSN_FAMICOM) (L. Spiro) (LSN_VULKAN__1)
#define LSN_VIDEO_FILTER_LSPIRO_PAL_VULKAN_UPSCALE							PAL-&I (PAL-A LSN_NES) (L. Spiro) (LSN_VULKAN__1)
#define LSN_VIDEO_FILTER_LSPIRO_DENDY_VULKAN_UPSCALE						PAL-&D/K (Dendy Famiclone) (L. Spiro) (LSN_VULKAN__1)
#define LSN_VIDEO_FILTER_LSPIRO_PALM_VULKAN_UPSCALE							PAL-&M (Brazil Famiclone) (L. Spiro) (LSN_VULKAN__1)
#define LSN_VIDEO_FILTER_LSPIRO_PALN_VULKAN_UPSCALE							PAL-&N (Argentina Famiclone) (L. Spiro) (LSN_VULKAN__1)
#define LSN_VIDEO_FILTER_AUTO_CRT_FULL_VULKAN_UPSCALE						NTSC/PAL Auto (LMP889&59) (LSN_VULKAN__1)
#define LSN_VIDEO_FILTER_AUTO_LSPIRO_VULKAN_UPSCALE							NTSC/PAL &Auto (L. Spiro) (LSN_VULKAN__1)

#define LSN_VIDEO_FILTER_NTSC_BLARGG_METAL_UPSCALE							NTSC-M (LSN_NES) (Blar&gg) (LSN_METAL_)
#define LSN_VIDEO_FILTER_NTSC_CRT_FULL_METAL_UPSCALE						NTSC-M (LSN_NES) (LMP&88959) (LSN_METAL_)
#define LSN_VIDEO_FILTER_PAL_CRT_FULL_METAL_UPSCALE							PAL-I (PAL-A LSN_NES) (LMP88&959) (LSN_METAL_)
#define LSN_VIDEO_FILTER_DENDY_CRT_FULL_METAL_UPSCALE							PAL-D/&K (Dendy Famiclone) (LMP88959) (LSN_METAL_)
#define LSN_VIDEO_FILTER_LSPIRO_NTSC_METAL_UPSCALE							NTSC-M (LSN_NES) (&L. Spiro) (LSN_METAL_)
#define LSN_VIDEO_FILTER_LSPIRO_PAL_METAL_UPSCALE							PAL-&I (PAL-A LSN_NES) (L. Spiro) (LSN_METAL_)
#define LSN_VIDEO_FILTER_LSPIRO_DENDY_METAL_UPSCALE							PAL-&D/K (Dendy Famiclone) (L. Spiro) (LSN_METAL_)
#define LSN_VIDEO_FILTER_LSPIRO_PALM_METAL_UPSCALE							PAL-&M (Brazil Famiclone) (L. Spiro) (LSN_METAL_)
#define LSN_VIDEO_FILTER_LSPIRO_PALN_METAL_UPSCALE							PAL-&N (Argentina Famiclone) (L. Spiro) (LSN_METAL_)
#define LSN_VIDEO_FILTER_AUTO_CRT_FULL_METAL_UPSCALE						NTSC/PAL Auto (LMP889&59) (LSN_METAL_)
#define LSN_VIDEO_FILTER_AUTO_LSPIRO_METAL_UPSCALE							NTSC/PAL &Auto (L. Spiro) (LSN_METAL_)

#define LSN_VIDEO_FILTER_DIRECT3D_9											Mi&crosoft® Direct3D® 9
#define LSN_VIDEO_FILTER_DIRECT3D_12										Microso&ft® Direct3D® 12
#define LSN_VIDEO_FILTER_VULKAN_1											&Vulkan® 1
#define LSN_VIDEO_FILTER_METAL												M&etal®
// Regions.
#define LSN_REGION_AUTO														&Automatic
#define LSN_REGION_NTSC_M													NTSC-&M (LSN_NES)
#define LSN_REGION_NTSC_J													NTSC-&J (LSN_FAMICOM)
#define LSN_REGION_PAL														&PAL-I (PAL-A LSN_NES)
#define LSN_REGION_DENDY													PAL-D/K (&Dendy Famiclone)
#define LSN_REGION_PALM														PAL-M (&Brazil Famiclone)
#define LSN_REGION_PALN														PAL-N (Ar&gentina Famiclone)

// Tools menu.
#define LSN_TOOLS_PATCH														i&Patch
#define LSN_TOOLS_WAV_EDIT													&WAV Editor
#define LSN_TOOLS_RECORD_VIDEO												&Record Video
#define LSN_TOOLS_RECORD													&Record
#define LSN_TOOLS_STOP_RECORDING											&Stop Recording
#define LSN_TOOLS_RECORDING_PENDING											Recording &Pending\u2026								// Recording Pending...
#define LSN_TOOLS_RECORDING_SETTINGS										Recording S&ettings\u2026								// Recording Settings...

// Main menu.
#define LSN_SELECT_ROM														Select ROM
#define LSN_FILE_LISTING													File Listing
#define LSN_SEARCH_															Search:
#define LSN_OK																OK
#define LSN_CANCEL															Cancel
#define LSN_CLOSE															Close

// Input dialog.
#define LSN_INPUT_GLOBAL_SETTINGS											Global Settings
#define LSN_INPUT_PER_GAME_SETTINGS											Per-Game Settings
#define LSN_INPUT_DIALOG													Input Configuration
#define LSN_INPUT_GENERAL													General
#define LSN_INPUT_CONSOLE_TYPE_												Console Type:
#define LSN_INPUT_USE_GLOBAL												Use Global
#define LSN_INPUT_SETTINGS													Settings
#define LSN_INPUT_PLAYER_1_													Player 1:
#define LSN_INPUT_PLAYER_2_													Player 2:
#define LSN_INPUT_PLAYER_3_													Player 3:
#define LSN_INPUT_PLAYER_4_													Player 4:
#define LSN_INPUT_CONFIGURATION												Configuration
#define LSN_INPUT_CONFIGURE													Configure
#define LSN_INPUT_EXPANSION_PORT											Expansion Port:
#define LSN_INPUT_USE_4_SCORE												Use Four Score\u2122									// Use Four Score(TM)
#define LSN_INPUT_NONE														None
#define LSN_INPUT_STANDARD_CONTROLLER										Standard Controller
#define LSN_INPUT_SNES_MOUSE												LSN_SNES Mouse
#define LSN_INPUT_SUBOR_MOUSE												Subor\u00AE Mouse										// Subor(R) Mouse
#define LSN_INPUT_ARKANOID_CONTROLLER										Arkanoid Controller
#define LSN_INPUT_BANDAI_HYPER_SHOT											Bandai\u00AE Hyper Shot									// Bandai(R) Hyper Shot
#define LSN_INPUT_BARCODE_BATTLER											Barcode Battler
#define LSN_INPUT_BATTLE_BOX												Battle Box
#define LSN_INPUT_EXCITING_BOXING_PUNCHING_BAG								Exciting Boxing Punching Bag
#define LSN_INPUT_FAMILY_BASIC_KEYBOARD										Family Basic Keyboard
#define LSN_INPUT_FAMILY_TRAINER											Family Trainer
#define LSN_INPUT_4_PLAYER_ADAPTOR											4-Player Adaptor
#define LSN_INPUT_HORI_TRACK												Hori Track
#define LSN_INPUT_JISSEN_MAHJONG_CONTROLLER									Jissen Mahjong Controller
#define LSN_INPUT_KONAMI_HYPER_SHOT											Konami\u00AE Hyper Shot									// Konami(R) Hyper Shot
#define LSN_INPUT_OEKA_KIDS_TABLET											Oeka Kids Tablet
#define LSN_INPUT_PACHINKO_CONTROLLER										Pachinko Controller
#define LSN_INPUT_PARTYTAP													Partytap
#define LSN_INPUT_SUBOR_KEYBOARD											Subor\u00AE Keyboard									// Subor(R) Keyboard
#define LSN_INPUT_TURBO_FILE												Turbo File
#define LSN_INPUT_ZAPPER													Zapper

// Standard input configuration.
#define LSN_STD_INPUT_BUTTONS												Buttons
#define LSN_STD_INPUT_ALTTERNATIVE_BUTTONS_1								Alternative Buttons 1
#define LSN_STD_INPUT_ALTTERNATIVE_BUTTONS_2								Alternative Buttons 2
#define LSN_STD_INPUT_ALTTERNATIVE_BUTTONS_3								Alternative Buttons 3
#define LSN_STD_INPUT_ALTTERNATIVE_BUTTONS_4								Alternative Buttons 4
#define LSN_STD_INPUT_ALTTERNATIVE_BUTTONS_5								Alternative Buttons 5
#define LSN_STD_INPUT_ALTTERNATIVE_BUTTONS_6								Alternative Buttons 6
#define LSN_STD_INPUT_ALTTERNATIVE_BUTTONS_7								Alternative Buttons 7
#define LSN_STD_INPUT_RAPID													Rapid
#define LSN_DIRECTIONAL_PAD													Directional Pad
#define LSN_SELECT_START													Select/Start
#define LSN_BUTTONS															Buttons
#define LSN_BUTTON_															Button:
#define LSN_TURBO_															Turbo:
#define LSN_DEAD_															Dead:

#define LSN_INPUT_DEVICES													Input Devices
#define LSN_DEVICE_NAME														Device Name
#define LSN_DEVICE_TYPE_													Device Type
#define LSN_STATUS															Status
#define LSN_USB_CONTROLLER													USB Controller
#define LSN_KEYBOARD														Keyboard
#define LSN_CIRCLE															\u25EF
#define LSN_BALLOT_EX														\U0001F5F7
#define LSN_BALLOT_CHECK													\U0001F5F9

#define LSN_QUICK_SET														Quick Set
#define LSN_CLEAR_ALL														Clear All
#define LSN_SET_NORMAL_BUTTONS												Set Normal Buttons
#define LSN_SET_TURBO_BUTTONS												Set Turbo Buttons
#define LSN_DASH_LINE														\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

#define LSN_PRESET															Presets
#define LSN_WASD_OP_L_														WASD - OP - L;
#define LSN_WASD_UI_JK														WASD - UI - JK
#define LSN_ARROW_KEYS_QW_AS												\u2191\u2190\u2192\u2193 - QW - AS
#define LSN_HORI_SWITCH_Y_B_LAYOUT											HORI Nintendo Switch\u2122 Y+B Layout
#define LSN_HORI_SWITCH_B_A_LAYOUT											HORI Nintendo Switch\u2122 B+A Layout

#define LSN_LISTENING_														Listening\u2026											// Listening...

// Audio Options.
#define LSN_AUDIO_OPTIONS_													Audio Options
#define LSN_AUDIO_OPTIONS_GLOBAL_SETTINGS									Global Settings
#define LSN_AUDIO_OPTIONS_PERGAME_SETTINGS									Per-Game Settings
#define LSN_AUDIO_OPTIONS_STREAM_TO_FILE									Stream to File
#define LSN_AUDIO_OPTIONS_GENERAL											Output Settings
#define LSN_AUDIO_OPTIONS_USE_GLOBAL										Use Global
#define LSN_AUDIO_OPTIONS_DEVICE											Device:
#define LSN_AUDIO_OPTIONS_ENABLED											Enabled
#define LSN_AUDIO_OPTIONS_FORMAT											Format:
#define LSN_AUDIO_OPTIONS_DITHER											Dither
#define LSN_AUDIO_OPTIONS_VOLUME											Volume:
#define LSN_AUDIO_OPTIONS_BG_VOL											BG Vol:

#define LSN_AUDIO_OPTIONS_AUDIO_CHARACTERISTICS								Audio Characteristics
#define LSN_AUDIO_OPTIONS_PRESET											Preset:
#define LSN_AUDIO_OPTIONS_NES_FL_N0260073									LSN_NES Front-Loading N0260073
#define LSN_AUDIO_OPTIONS_NES_FL_N1151667									LSN_NES Front-Loading N1151667

#define LSN_AUDIO_OPTIONS_NES_FL_N13721267									LSN_NES Front-Loading N13721267
#define LSN_AUDIO_OPTIONS_NES_FL_N20914398									LSN_NES Front-Loading N20914398, Sunsoft 5B
#define LSN_AUDIO_OPTIONS_NES_FL_N32670105									LSN_NES Front-Loading N32670105

#define LSN_AUDIO_OPTIONS_NES_FL_N34169630									LSN_NES Front-Loading N34169630
#define LSN_AUDIO_OPTIONS_NES_FL_N8869725									LSN_NES Front-Loading N8869725, NESRGB
#define LSN_AUDIO_OPTIONS_NES_FL_N8869725_RCA								LSN_NES Front-Loading N8869725, NESRGB, Stock RCA
#define LSN_AUDIO_OPTIONS_NES_TL_NN101255729								LSN_NES Top-Loading NN101255729, NESRGB
#define LSN_AUDIO_OPTIONS_FAMI_H11111212									Japanese LSN_FAMICOM H11111212, RF Cables

#define LSN_AUDIO_OPTIONS_TWIN_FAMI_441850									Japanese Twin LSN_FAMICOM 441850, Stock RCA

#define LSN_AUDIO_OPTIONS_TWIN_FAMI_475711									Japanese Twin LSN_FAMICOM 475711, NESRGB, Stock RCA
#define LSN_AUDIO_OPTIONS_TWIN_FAMI_475711_SCART							Japanese Twin LSN_FAMICOM 475711, NESRGB, SCART Out
#define LSN_AUDIO_OPTIONS_NES_PAL_PM313006									PAL LSN_NES Front-Loading NESE-001 (AUS) PM313006
#define LSN_AUDIO_OPTIONS_NES_PAL_V7										PAL LSN_NES Front-Loading NESE-001 (GBR) PM608755

#define LSN_AUDIO_OPTIONS_CUSTOM											Custom
#define LSN_AUDIO_OPTIONS_LPF_HZ											LPF Hz:
#define LSN_AUDIO_OPTIONS_HPF_1_HZ											HPF 1 Hz:
#define LSN_AUDIO_OPTIONS_HPF_2_HZ											HPF 2 Hz:
#define LSN_AUDIO_OPTIONS_HPF_3_HZ											HPF 3 Hz:
#define LSN_AUDIO_OPTIONS_P1_VOLUME											Pulse 1 Vol:
#define LSN_AUDIO_OPTIONS_P2_VOLUME											Pulse 1 Vol:
#define LSN_AUDIO_OPTIONS_TRI_VOLUME										Triangle Vol:
#define LSN_AUDIO_OPTIONS_NOISE_VOLUME										Noise Vol:
#define LSN_AUDIO_OPTIONS_DMC_VOLUME										DMC Vol:
#define LSN_AUDIO_OPTIONS_LOCK_VOL											Lock Volume
#define LSN_AUDIO_OPTIONS_INVERT											Invert
#define LSN_AUDIO_OPTIONS_NOISE_MAINS_HUM									Noise/Mains Hum
#define LSN_AUDIO_OPTIONS_RP2A03_4B2_40										RP2A03 (Original Console)

#define LSN_AUDIO_OPTIONS_INTERNAL											Internal
#define LSN_AUDIO_OPTIONS_BUFFER_SIZE										Buffer Size:
#define LSN_AUDIO_OPTIONS_BUFFERS											Buffers:
#define LSN_AUDIO_OPTIONS_BUFFER_DELAY										Buffer Delay:
#define LSN_AUDIO_OPTIONS_ANTI_ALIASING_QUALITY								Anti-Aliasing Quality:

#define LSN_AUDIO_OPTIONS_RAW												Raw Audio Signal
#define LSN_AUDIO_OPTIONS_CAPTURE_OUTPUT									Capture Output
#define LSN_AUDIO_OPTIONS_PATH_												Path:
#define LSN_AUDIO_OPTIONS_BROWSE											Browse
#define LSN_AUDIO_OPTIONS_HZ_												Hz:
#define LSN_AUDIO_OPTIONS_BITS_												Bits:
#define LSN_AUDIO_OPTIONS_PCM												PCM
#define LSN_AUDIO_OPTIONS_F32												32-Bit Float
#define LSN_AUDIO_OPTIONS_ADPCM												ADPCM
#define LSN_AUDIO_OPTIONS_DECIDED_BY_SETTINGS								Decided via Settings
#define LSN_AUDIO_OPTIONS_RAW_DESC											The raw unfiltered {}-Hz audio signal will be captured to a WAV file set to {} Hz.
#define LSN_AUDIO_OPTIONS_OUT_DESC											Audio will be captuerd after filters, anti-aliasing, and down-sampling have been applied, but while still in 32-bit float format.
#define LSN_AUDIO_OPTIONS_START_CONDITION_									Start Condition:
#define LSN_AUDIO_OPTIONS_END_CONDITION_									End Condition:
#define LSN_AUDIO_OPTIONS_START_SAMPLE										Start at Sample\u2026									// Start at Sample...
#define LSN_AUDIO_OPTIONS_END_SAMPLE										End at Sample\u2026										// End at Sample...
#define LSN_AUDIO_OPTIONS_NONE												None
#define LSN_AUDIO_OPTIONS_NON_SILENT										First Non-Zero
#define LSN_AUDIO_OPTIONS_DURATION											Duration (Seconds)
#define LSN_AUDIO_OPTIONS_SILENCE_FOR										Silence for (Seconds)
#define LSN_AUDIO_OPTIONS_WAV_TYPES											WAV Files (*.wav)\0*.wav\0\0
#define LSN_AUDIO_OPTIONS_TXT_TYPES											TXT Files (*.txt)\0*.txt\0\0
#define LSN_AUDIO_OPTIONS_METADATA											Metadata
#define LSN_AUDIO_OPTIONS_METADATA_ALL										All
#define LSN_AUDIO_OPTIONS_METADATA_NONE										None
#define LSN_AUDIO_OPTIONS_METADATA_PULSE1									Pulse 1 ($4000-$4003)
#define LSN_AUDIO_OPTIONS_METADATA_PULSE2									Pulse 2 ($4004-$4007)
#define LSN_AUDIO_OPTIONS_METADATA_TRIANGLE									Triangle ($4008-$400B)
#define LSN_AUDIO_OPTIONS_METADATA_NOISE									Noise ($400C-$400F)
#define LSN_AUDIO_OPTIONS_METADATA_DMC										DMC ($4010-$4013)
#define LSN_AUDIO_OPTIONS_METADATA_STATUS									Status ($4015)
#define LSN_AUDIO_OPTIONS_METADATA_FRAME_COUNTER							Frame Counter ($4017)

#define LSN_AUDIO_OPTIONS_METADATA_PULSE1_ON_OFF							Pulse 1 On/Off
#define LSN_AUDIO_OPTIONS_METADATA_PULSE2_ON_OFF							Pulse 2 On/Off
#define LSN_AUDIO_OPTIONS_METADATA_TRIANGLE_ON_OFF							Triangle On/Off
#define LSN_AUDIO_OPTIONS_METADATA_NOISE_ON_OFF								Noise On/Off
#define LSN_AUDIO_OPTIONS_METADATA_DMC_SET									DMC Written

#define LSN_AUDIO_OPTIONS_CAPTUE_ALL										Capture Redundant

#define LSN_AUDIO_OPTIONS_ERR_INVALID_PATH									A valid path must be supplied.
#define LSN_AUDIO_OPTIONS_ERR_SAME_PATHS									Cannot output 2 streams to the same path at once.
#define LSN_AUDIO_OPTIONS_ERR_INVALID_START_COND							The selected starting condition requires a valid numeric input.  This can be in the form of raw numbers (\u201C2\u201D, \u201C56.2\u201D, \u201C360.0f\u201D, etc.) or as equations (\u201C2*60+13.5\u201D, \u201Cpow( 10, 6 / 20.0 )\u201D, etc.)
#define LSN_AUDIO_OPTIONS_ERR_INVALID_END_COND								The selected stopping condition requires a valid numeric input.  This can be in the form of raw numbers (\u201C10\u201D, \u201C5.5\u201D, \u201C0.5f\u201D, etc.) or as equations (\u201C3*60+10.5\u201D, \u201Clog10( 0.5 ) * 20\u201D, etc.)
#define LSN_AUDIO_OPTIONS_ERR_INVALID_DURATION								Durations must be positive values larger than 0.
#define LSN_AUDIO_OPTIONS_ERR_INVALID_STOP_SAMPLE							The stop sample must be greater than the start sample.

// Patcher.
#define LSN_PATCH_PATCH_ROM													iPatch: Patch a ROM
#define LSN_PATCH_FILE														File
#define LSN_PATCH_ROM_TO_PATCH												ROM to Patch:
#define LSN_PATCH_PATCH_FILE												Patch File(s):
#define LSN_PATCH_OUTPUT_FILE												Output File:
#define LSN_PATCH_BROWSE													Browse
#define LSN_PATCH_QUICK_PATCH												Quick Patch
#define LSN_PATCH_FILE_TYPES												Patch Files (*.ips, *.bps, *.zip)\0*.ips;*.bps;*.zip\0All Files (*.*)\0*.*\0\0
#define LSN_PATCH_SRC_INFO													Source ROM Information
#define LSN_PATCH_PATCH														Patch
#define LSN_PATCH_DETAILS													Details
#define LSN_PATCH_PATCH_IS_COMPATIBLE										Patch file is compatible with the current ROM file based on the embedded CRC.
#define LSN_PATCH_LIKELY_COMPATIBLE											* Patch file is likely compatible with the current ROM file based on the CRC being found in an accompanying text file. *
#define LSN_PATCH_COMPAT_UNKNOWN											** Patch file compatibility with the current ROM file is unknown. **
#define LSN_PATCH_NOT_COMPATIBLE											*** Patch file is not compatible with the current ROM file based on the embedded CRC. ***
#define LSN_PATCH_REVEAL_COMPATIBLE											&Reveal Compatible
#define LSN_PATCH_SELECT_COMPATIBLE											&Select Compatible
#define LSN_PATCH_REVEAL_BPS												Re&veal All BPS Files
#define LSN_PATCH_EXPAND_SELECTED											Expand Selec&ted
#define LSN_PATCH_EXPAND_ALL												E&xpand All
#define LSN_PATCH_COLLAPSE_SELECTED											C&ollapse Selected
#define LSN_PATCH_COLLAPSE_ALL												&Collapse All
#define LSN_PATCH_WARNING_INCOMPATIBLE_FILE_1								WARNING: The selected patch is incompatible with the current ROM file based on the patch’s embedded or inferred CRC.\r\n\r\nDo you really wish to continue?
#define LSN_PATCH_WARNING_INCOMPATIBLE_FILE_X								WARNING: At least one of the selected patches is incompatible with the current ROM file based on the patch’s embedded or inferred CRC.\r\n\r\nDo you really wish to continue?
#define LSN_PATCH_WARNING_INCOMPATIBLE_FILE_TITLE							Incompatible Patch
#define LSN_PATCH_UNABLE_TO_CREATE_FILE										Failed to create patch file: {}
#define LSN_PATCH_UNABLE_TO_WRITE_FILE										Failed to write patched ROM to file: {}
#define LSN_PATCH_OUT_OF_MEMORY_CREATING_PATCH								Out of memory creating 1 or more patch(es).

// WAV Editor
#define LSN_WE_WAV_EDITOR													WAV Editor
#define LSN_WE_FILES														Files
#define LSN_WE_FILES_DESC													Channels: {}; Dur: {:.7f}; Hz: {}
#define LSN_WE_MULTI_FILES_DESC												Channels: {}; Dur: {:.7f}; Hz: {}; Total Dur: {:.7f}
#define LSN_WE_FILE_SETTINGS												Per-File Settings
#define LSN_WE_FILE_SETTINGS_ALL											Per-File Settings (All Files)
#define LSN_WE_SEQUENCING													Per-File Sequencing
#define LSN_WE_SEQUENCING_ALL												Per-File Sequencing (All Files)
#define LSN_WE_OUTPUT														Output
#define LSN_WE_ADD_WAV														Add .WAV Files
#define LSN_WE_ADD_METADATA													Add Metadata
#define LSN_WE_ADD_OPERATION												Add Operation
#define LSN_WE_REMOVE														Remove
#define LSN_WE_MOVE_UP														Move Up
#define LSN_WE_MOVE_DOWN													Move Down
#define LSN_WE_SELECT_ALL													Select &All
#define LSN_WE_RANGE														Track Start
#define LSN_WE_RANGES														Track Starts
#define LSN_WE_LOOPS														Looping/One-Shot
#define LSN_WE_START_TIME													Start Time:
#define LSN_WE_END_TIME														End Time:
#define LSN_WE_STOP_TIME													Stop Time:
#define LSN_WE_OPENING_SIL													Opening Silence:
#define LSN_WE_TRAIL														Trailing Silence:
#define LSN_WE_SECONDS														Seconds
#define LSN_WE_EXACT_TIME													Exact Time
#define LSN_WE_MINUS_ONE													One Sample Back
#define LSN_WE_PLUS_ONE														One Sample After
#define LSN_WE_START_SNAP													Snap to One Sample Before First Different Sample
#define LSN_WE_STOP_SNAP													Snap to One Sample After First Different Sample
#define LSN_WE_LOOP															Loop
#define LSN_WE_ONE_SHOT														One-Shot
#define LSN_WE_FIND_LOOPS													Find Loop Points
#define LSN_WE_START														Start:
#define LSN_WE_END															End:
#define LSN_WE_DELAY														Pre-Fade Dur.:
#define LSN_WE_FADE_TIME													Fade Dur.:
#define LSN_WE_SILENCES														Silence
#define LSN_WE_OPERATIONS													Operations
#define LSN_WE_OPERATION													Operation
#define LSN_WE_PARMS														Parameters
#define LSN_WE_DESC															Description
#define LSN_WE_FILE_DATA													File Data
#define LSN_WE_TRACK_TITLE													Track Title:
#define LSN_WE_FILE_NAME_PREFIX												File Name Prefix:
#define LSN_WE_FILE_NAME_POSTFIX											File Name Postfix:
#define LSN_WE_ACTUAL_HZ													Actual Hz:
#define LSN_WE_CHARACTERISTICS												Characteristics
#define LSN_WE_PRESETS														Presets:
#define LSN_WE_SAVE															Save
#define LSN_WE_LOAD															Load
#define LSN_WE_EXIT															Exit
#define LSN_WE_FALLOFF														Falloff:
#define LSN_WE_DB_PER_OCTAVE												dB/Octave
#define LSN_WE_METADATA														Metadata
#define LSN_WE_ARTIST														Artist:
#define LSN_WE_ALBUM														Album:
#define LSN_WE_YEAR															Year:
#define LSN_WE_COMMENTS														Comments:
#define LSN_WE_APPLY_TO_ALL													Apply to All
#define LSN_WE_OUTPUT														Output
#define LSN_WE_NOISE														Noise (PPU Interference/White Noise)
#define LSN_WE_MASTER_VOLUME												Master Volume
#define LSN_WE_ABSOLUTE														Absolute:
#define LSN_WE_NORMALIZE													Normalize to:
#define LSN_WE_SET_LOUDNESS_TO												Set Loudness to:
#define LSN_WE_FORMAT														Format
#define LSN_WE_OUTPUT_FOLDER												Output Folder
#define LSN_WE_NUMBERED														Numbered
#define LSN_WE_EXPORT_ALL													Export All
#define LSN_WE_RF1															RF Cables 1
#define LSN_WE_RF2															RF Cables 2
#define LSN_WE_SUNSOFT_5B													Apply Sunsoft 5B volume curve.

#define LSN_WE_LOOP_DESC_1_TRACK											Track will start at {:.5g} seconds, play for {:.5g} seconds, continue for another {:.5g} seconds, and then fade out for {:.5g} seconds.  Total duration: {:.7g} seconds.
#define LSN_WE_ONE_SHOT_DESC_1_TRACK										Track will start at {:.5g} seconds and play for {:.7g} seconds.

#define LSN_WE_NTSC_HZ														NTSC (1,789,772.7272727272727272727272727 Hz)
#define LSN_WE_PAL_HZ														PAL (1,662,607.03125 Hz)
#define LSN_WE_DENDY_HZ														PAL \u201CDendy\u201D Famiclone (1,773,447.5 Hz)
#define LSN_WE_PAL_M_HZ														PAL-M Brazil Famiclone (1,787,805.9440559440559440559440559 Hz)
#define LSN_WE_PAL_N_HZ														PAL-N Argentina Famiclone (1,791,028.125 Hz)
#define LSN_WE_SET_BY_FILE													Set by File

#define LSN_WE_POLE															Pole
#define LSN_WE_SINC															Sinc

#define LSN_WE_MONO															Mono
#define LSN_WE_STEREO														Stereo
#define LSN_WE_SURROUND														Surround

#define LSN_WE_XX_BLACK														XX-Black (0D)
#define LSN_WE_X_BLACK														X-Black (1Dem)
#define LSN_WE_BLACK														Black (1D)
#define LSN_WE_GREY_00														Grey (00)
#define LSN_WE_GREY_10														Grey (10)
#define LSN_WE_GREY_20EM													Grey (20em)
#define LSN_WE_RED															Red (16)
#define LSN_WE_GREEN														Green (1A)
#define LSN_WE_BLUE															Blue (12)
#define LSN_WE_WHITE														White (30)

#define LSN_WE_GAUSSIAN														Gaussian
#define LSN_WE_UNIFORM														Uniform

#define LSN_WE_BROWSE_OUTPUT_FOLDER											Output Folder

#define LSN_WE_OUTPUT_ERR_MAINSHUM_VOL										Mains Hum volume must be a valid expression.  Typical results will be between 0 and 1.
#define LSN_WE_OUTPUT_ERR_WHITE_NOISE_VOL									White Noise volume must be a valid expression.  Typical results will be between 0 and 1.
#define LSN_WE_OUTPUT_ERR_ABSOLUTE_VOL										Absolute volume must be a valid expression.  Typical results will be between 0 and 10.
#define LSN_WE_OUTPUT_ERR_NORMALIZE_LEVEL									Normalize level must be a valid expression.  Typical results will be between 0 and 1.  Example expression: \u201Cpow( 10, -0.1 / 20.0 )\u201D (normalizes to -0.1 dB).
#define LSN_WE_OUTPUT_ERR_LOUDNESS_LEVEL									Loudness level must be a valid expression.  Typical results will be between 0 and 1.  Example expression: \u201Cpow( 10, -5.5 / 20.0 )\u201D (set average loudness to -5.5 dB).
#define LSN_WE_OUTPUT_ERR_OUT_HZ											The output Hz must be a valid expression.
#define LSN_WE_OUTPUT_ERR_OUT_HZ_NEGATIVE									The output Hz expression must result in a value between 1 and 4,294,967,295 inclusively.
#define LSN_WE_OUTPUT_ERR_OUT_PATH_EMPTY									No output folder provided.

#define LSN_WE_OUTPUT_ERR_SEQ_START_TIME									The track\u2019s starting point must be a valid expression.
#define LSN_WE_OUTPUT_ERR_SEQ_START_TIME_NEG								The track\u2019s starting point expression must result in a non-negative value.
#define LSN_WE_OUTPUT_ERR_SEQ_FULL_END_TIME									The track\u2019s maximum end point must be a valid expression.
#define LSN_WE_OUTPUT_ERR_SEQ_FULL_END_TIME_LESS_THAN						The track\u2019s maximum end point must be larger than the starting point.
#define LSN_WE_OUTPUT_ERR_SEQ_END_TIME										The track\u2019s stopping point must be a valid expression.
#define LSN_WE_OUTPUT_ERR_SEQ_END_TIME_NEG									The track\u2019s stopping point expression must result in a non-negative value.
#define LSN_WE_OUTPUT_ERR_SEQ_DEL_TIME										The track\u2019s delay time must be a valid expression.
#define LSN_WE_OUTPUT_ERR_SEQ_DEL_TIME_NEG									The track\u2019s delay time expression must result in a non-negative value.
#define LSN_WE_OUTPUT_ERR_SEQ_FAD_TIME										The track\u2019s fade time must be a valid expression.
#define LSN_WE_OUTPUT_ERR_SEQ_FAD_TIME_NEG									The track\u2019s fade time expression must result in a non-negative value.
#define LSN_WE_OUTPUT_ERR_SEQ_TOO_LONG										The track\u2019s duration is larger than the full source range specified by the Start Time and End Time.
#define LSN_WE_OUTPUT_ERR_SEQ_OPEN_TIME										The track\u2019s opening-sience must be a valid expression.
#define LSN_WE_OUTPUT_ERR_SEQ_OPEN_NEG										The track\u2019s opening-sience expression must result in a non-negative value.
#define LSN_WE_OUTPUT_ERR_SEQ_TRAIL_TIME									The track\u2019s trailing-sience must be a valid expression.
#define LSN_WE_OUTPUT_ERR_SEQ_TRAIL_NEG										The track\u2019s trailing-sience expression must result in a non-negative value.

#define LSN_WE_OUTPUT_ERR_SET_NO_NAME										The track must be given a name.
#define LSN_WE_OUTPUT_ERR_SET_BAD_SR										The track\u2019s sample rate must be a valid expression.
#define LSN_WE_OUTPUT_ERR_SET_SR_NEG										The track\u2019s sample-rate expression must result in a non-negative value.
#define LSN_WE_OUTPUT_ERR_SET_BAD_VOL										The volume must be a valid expression.
#define LSN_WE_OUTPUT_ERR_SET_BAD_HZ										The filter cut-off frequency must be a valid expression.
#define LSN_WE_OUTPUT_ERR_SET_HZ_NEG										The filter-cut\u2013off-frequency expression must result in a non-negative value.
#define LSN_WE_OUTPUT_ERR_SET_FALLOFF_BAD									The filter fall-off rate must be a valid expression.
#define LSN_WE_OUTPUT_ERR_SET_FALLOFF_POS									The filter-fall\u2013off-rate expression must result in a negative value.

#define LSN_WE_LOAD_VERIFY													All currently loaded files will be removed.\r\nContinue?

#define LSN_WE_TT_TREEVIEW													The list of .WAV files, along with any metadata, to process and export.
#define LSN_WE_TT_LOAD_WAV													Load a single .WAV file or a string of connected .WAV files, along with auto-detected metadata.
#define LSN_WE_TT_LOAD_META													Assigns a metadata file to the selected .WAV file sets.
#define LSN_WE_TT_REMOVE													Removes selected .WAV-file sets.
#define LSN_WE_TT_MOVE_UP													Moves selected .WAV-file sets up one index.
#define LSN_WE_TT_MOVE_DOWN													Moves selected .WAV-file sets down one index.

#define LSN_WE_TT_TITLE														The title will be used in the output file\u2019s metadata and file name ([prefix][title][postfix]).
#define LSN_WE_TT_PREFIX													The prefix will be used in the file name ([prefix][title][postfix]).
#define LSN_WE_TT_POSTFIX													The postfix will be used in the file name ([prefix][title][postfix]).
#define LSN_WE_TT_ACTUAL_HZ													Real consoles output audio at sample rates with fractions (IE 1,662,607.03125 Hz), but .WAV files can only have sample rates that are whole numbers.  Use this option to assign the correct Hz to the data in the .WAV file.
#define LSN_WE_TT_ACTUAL_HZ_EDIT											Use this when none of the presets applies.  This can be used to assign sample rates that are measured from real hardware and deviate from the ideal Hz due to temperature, age, and natural clock variance.//Use this when none of the presets applies.  This can be used to assign sample rates that are measured from real hardware and deviate from the idea Hz due to temperature, age, and natural clock variance.  Some measured hardware Hz are:\r\n1662607.03125 \x2F 19.9972 * 19.9971 \x2F\x2F PAL: 1662598.71705085574649274349212646484375\r\n1789772.727272727272 * (1000.0\x2F60.09848) \x2F (1000.0 \x2F 60.09812); \x2F\x2F N34169630: 1789762.00623316341079771518707275390625\r\n1789772.727272727272 * (1000.0\x2F60.09848) \x2F (1000.0 \x2F 60.09857); \x2F\x2F N1151667: 1789775.40753261814825236797332763671875\r\n1789772.727272727272 * (1000.0\x2F60.09848) \x2F (1000.0 \x2F 60.09965); \x2F\x2F N0260073: 1789807.57065131026320159435272216796875\r\n1789772.727272727272 * (1000.0\x2F60.09848) \x2F (1000.0 \x2F 60.09813); \x2F\x2F Twin Famicom RCA 475711: 1789762.30403981753624975681304931640625\r\n\r\nNote that all numerical edit boxes in LSN_BEESNES allow typing of actual equations, such as \u201C1662607.03125 \x2F 19.9972 * 19.9971\u201D.
#define LSN_WE_TT_PRESETS													Measured hardware presets.
#define LSN_WE_TT_PRESETS_SAVE												Save your own custom preset.
#define LSN_WE_TT_PRESETS_LOAD												Load your own custom preset(s).
#define LSN_WE_TT_CHAR_VOL													The output volume for a given console.  This can be locked to a specific value in order to create OST\u2019s with consistent volumes.
#define LSN_WE_TT_CHAR_LOCK													Locks the console volume.  Useful for making OST\u2019s with consistent volumes between each other.
#define LSN_WE_TT_CHAR_INVERT												Inverts the waveform.  Commonly set.
#define LSN_WE_TT_CHAR_LPF_CHECK											Enables or disables the LPF.
#define LSN_WE_TT_CHAR_HPF0_CHECK											Enables or disables the first HPF.
#define LSN_WE_TT_CHAR_HPF1_CHECK											Enables or disables the second HPF.
#define LSN_WE_TT_CHAR_HPF2_CHECK											Enables or disables the third HPF.
#define LSN_WE_TT_CHAR_LPF													The LPF cut-off frequency.
#define LSN_WE_TT_CHAR_HPF0													The first HPF cut-off frequency.
#define LSN_WE_TT_CHAR_HPF1													The second HPF cut-off frequency.
#define LSN_WE_TT_CHAR_HPF2													The third HPF cut-off frequency.
#define LSN_WE_TT_ARTIST													List the composer(s) to be added to the output file\u2019s metadata.
#define LSN_WE_TT_ALBUM														The name of the album (to be added to the output file\u2019s metadata).
#define LSN_WE_TT_YEAR														The year of the game release (to be added to the output file\u2019s metadata).
#define LSN_WE_TT_COMMENT													Additional comments (to be added to the output file\u2019s metadata).
#define LSN_WE_TT_APPLY_TO_ALL												Applies the current frequency response (LPF + HPF\u2019s), volume, lock, invert, artist, album, year, comment, prefix, and postfix to all .WAV-file sets.

#define LSN_WE_TT_START_TIME												Selects the time, in the file\u2019s native Hz, when the audio for the current track begins.
#define LSN_WE_TT_END_TIME													If one-shot, selects the time when the audio ends, otherwise it represents the time at which the pre-fade (and then fade) begins.  All times are in the file\u2019s native Hz.
#define LSN_WE_TT_TIME_COMBO												If metadata is included with the source .WAV file, its events are listed for easy time selection.  All times are in the file\u2019s native Hz.
#define LSN_WE_TT_EXACT_TIME												Start/stop at the exact specified time.
#define LSN_WE_TT_START_MINUS_ONE											Start one sample before the specified time.
#define LSN_WE_TT_STOP_PLUS_ONE												Stop one sample after the specified time.
#define LSN_WE_TT_START_SNAP												Start at the sample just before the first sample that is different from the one at the specified time.
#define LSN_WE_TT_STOP_SNAP													Stop at the sample just after the first sample that is different from the one at the specified time.
#define LSN_WE_TT_ONE_SHOT													If checked, the audio will play from start to end, and include the opening and trailing silence.
#define LSN_WE_TT_LOOPING													If checked, the audio will play from start to end, then continue playing for the pre-fade duration, and then continue playing during the fade-out, and include the opening and trailing silence.  The Stop Time will usually be set to the time the track loops (typically the exact time of the 2nd loop, in the file\u2019s native Hz).
#define LSN_WE_TT_PREFADE													In a looping track, this is tne number of seconds to continue playing before beginning the fade-out.
#define LSN_WE_TT_FADE														In a looping track, this is the duration, in seconds, of the studio fade-out that will be applied.
#define LSN_WE_TT_OPENING_SILENCE											The duration of opening silence to prepend to the output file.
#define LSN_WE_TT_TRAILING_SILENCE											The duration of trailing silence to append to the output file.

#define LSN_WE_TT_MAINS_HUM													If checked interference from the PPU is applied.
#define LSN_WE_TT_MAINS_COLOR												The most prominent color on the screen during audio output.  The PPU will output different volates based on the colors it is displaying, which create different-sounding buzzes and hums in the audio,
#define LSN_WE_TT_MAINS_VOLUME												The volume of the interference hum to apply.
#define LSN_WE_TT_MAINS_OFFSET												The offset of the interference to apply, in seconds.
#define LSN_WE_TT_WHITE_NOISE												If checked, physically based (Gaussian) or fake (Uniform) white noise is applied.
#define LSN_WE_TT_WHITE_NOISE_VOLUME										Volume of the white noise to apply.
#define LSN_WE_TT_WHITE_NOISE_COMBO											Gaussian produces physically realistic white noise.  Uniform white noise can be found in synthesis routines and in some DSP routines.
#define LSN_WE_TT_WHITE_NOISE_BANDWIDTH										The noise bandwidth.  Typically 100-200 kHz.
#define LSN_WE_TT_WHITE_NOISE_OHM											The system resistance in Ohms (\u03A9).  Fifty is a typical value for an RF system.
#define LSN_WE_TT_WHITE_NOISE_TEMP											The room temperature in Kelvin (290 for a standard room).

#define LSN_WE_TT_VOL_ABS													The audio will be multiplied by a constant value on output.
#define LSN_WE_TT_VOL_EDIT													The amount by which to multiple the final audio.
#define LSN_WE_TT_VOL_NORMAL												The audio will be normalized to a given level.  All numeric edit boxes in LSN_BEESNES allow typing equations, so it is possible to specify the normalization level in a dB -> linear equation or as a specific value.
#define LSN_WE_TT_VOL_NORMAL_EDIT											The level to which to normalize the final audio.  All numeric edit boxes in LSN_BEESNES allow typing equations, so it is possible to specify the normalization level in a dB -> linear equation or as a specific value.
#define LSN_WE_TT_VOL_LOUDNESS												Uses a gated RMS evaluation to set the audio to the given loudness level.
#define LSN_WE_TT_VOL_LOUDNESS_EDIT											The level to which to set the loudness.  All numeric edit boxes in LSN_BEESNES allow typing equations, so it is possible to specify the normalization level in a dB -> linear equation or as a specific value.

#define LSN_WE_TT_OUTPUT_HZ													The output .WAV file Hz.
#define LSN_WE_TT_OUTPUT_FMT												The output .WAV file format.
#define LSN_WE_TT_OUTPUT_BITS												The output .WAV file PCM bit depth.
#define LSN_WE_TT_OUTPUT_DITHER												If the PCM bit depth is 16, error-diffusion dithering can be applied.
#define LSN_WE_TT_OUTPUT_CHANS												Mono: All input channels will be mixed to mono.  Stereo: Mono inputs will remain mono, but noise will be stereo.  Stereo inputs will remain stereo.  Surround: Same rules as Stereo, but a quasi-surround effect will be applied.

#define LSN_WE_TT_FOLDER													The output folder to which each .WAV file will be saved.
#define LSN_WE_TT_BROWSE													Browse for an output folder.
#define LSN_WE_TT_NUMBERED													If checked,, file names will be numbered and the number will be added to each file\u2019s metadata.

// OpenAL.
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

// Buttons.
#define LSN_BUTTON_LEFT														Left
#define LSN_BUTTON_RIGHT													Right
#define LSN_BUTTON_UP														Up
#define LSN_BUTTON_DOWN														Down
#define LSN_BUTTON_SELECT													Select
#define LSN_BUTTON_START													Start
#define LSN_BUTTON_B														B
#define LSN_BUTTON_A														A

// Audio Formats.
#define LSN_AUDIO_FORMAT_MONO8												Mono, 8-bit PCM
#define LSN_AUDIO_FORMAT_MONO16												Mono, 16-bit PCM
#define LSN_AUDIO_FORMAT_MONO24												Mono, 24-bit PCM
#define LSN_AUDIO_FORMAT_MONO32F											Mono, 32-bit Float

// Core Audio.
#define LSN_AUDIO_HARDWARE_NO_ERROR											The function call completed successfully.
#define LSN_AUDIO_HARDWARE_NOT_RUNNING_ERROR								The function call requires that the hardware be running but it isn\u2019t.
#define LSN_AUDIO_HARDWARE_UNSPECIFIED_ERROR								The function call failed while doing something that doesn\u2019t provide any error messages.
#define LSN_AUDIO_HARDWARE_UNKNOWN_PROPERTY_ERROR							The AudioObject doesn\u2019t know about the property at the given address.
#define LSN_AUDIO_HARDWARE_BAD_PROPERTY_SIZE_ERROR							An improperly sized buffer was provided when accessing the data of a property.
#define LSN_AUDIO_HARDWARE_ILLEGAL_OPERATION_ERROR							The requested operation couldn\u2019t be completed.
#define LSN_AUDIO_HARDWARE_BAD_OBJECT_ERROR									The AudioObjectID passed to the function doesn\u2019t map to a valid AudioObject.
#define LSN_AUDIO_HARDWARE_BAD_DEVICE_ERROR									The AudioObjectID passed to the function doesn\u2019t map to a valid AudioDevice.
#define LSN_AUDIO_HARDWARE_BAD_STREAM_ERROR									The AudioObjectID passed to the function doesn\u2019t map to a valid AudioStream.
#define LSN_AUDIO_HARDWARE_UNSUPPORTED_OPERATION_ERROR						The AudioObject doesn\u2019t support the requested operation.
#define LSN_AUDIO_HARDWARE_NOT_READY_ERROR									The AudioObject isn\u2019t ready to do the requested operation.
#define LSN_AUDIO_DEVICE_UNSUPPORTED_FORMAT_ERROR							The AudioStream doesn\u2019t support the requested format.
#define LSN_AUDIO_DEVICE_PERMISSIONS_ERROR									The requested operation can\u2019t be completed because the process doesn\u2019t have permission.
#define LSN_CORE_AUDIO_ERROR_UNKNOWN										Unknown Core Audio error.

// WAV Files.
#define LSN_WAV_PCM															PCM
#define LSN_WAV_ADPCM														ADPCM
#define LSN_WAV_IEEE_FLOAT													IEEE Floating Point
#define LSN_WAV_ALAW														A Law
#define LSN_WAV_MULAW														Mu Law
#define LSN_WAV_DVI_ADPCM													DVI ADPCM
#define LSN_WAV_YAMAHA_ADPCM												Yamaha ADPCM
#define LSN_WAV_GSM_6_10													GSM Audio
#define LSN_WAV_ITU_G_721_ADPCM												ITU G ADPCM
#define LSN_WAV_MPEG														MPEG Audio
#define LSN_WAV_EXTENSIBLE													Extensible
#define LSN_AUDACITY_METADATA												Audacity Format: [start time]\\t[end time]\\t[label text]\\r\\n

// WAV Editor.
#define LSN_WE_FAILED_TO_CREATE_DIR											Failed to create directory \u201C{}\u201D.
#define LSN_WE_FAILED_TO_LOAD_SAMPLES										Failed to load samples from WAV file \u201C{}\u201D.
#define LSN_WE_CHANNEL_COUNT_ERROR											WAV file \u201C{}\u201D does not have the same number of channels as \u201C{}\u201D.
#define LSN_WE_SAMPLE_RATE_ERROR											WAV file \u201C{}\u201D does not have the same sample rate as \u201C{}\u201D.
#define LSN_WE_BAD_CHANNEL_MIX												Can only create stereo/surround WAV files using sources that are either 1 or 2 channels.
#define LSN_WE_FAILED_TO_SAVE_WAV											Failed to create WAV file: \u201C{}\u201D.
#define LSN_WE_FAILED_TO_LOAD_PROJECT										Failed to load BWAV project, probably due to a missing WAV or metadata file.
#define LSN_WE_FAILED_TO_CREATE_BAT											Failed to create .BAT file \u201C{}\u201D.
#define LSN_WE_FAILED_TO_WRITE_BAT											Failed to write to .BAT file \u201C{}\u201D.
#define LSN_WE_FAILED_TO_CREATE_METADATA									Failed to create metadata file \u201C{}\u201D.
#define LSN_WE_FAILED_TO_WRITE_METADATA										Failed to write to metadata file \u201C{}\u201D.

// Cheats.
#define LSN_STR__CHEATS														&Cheats
#define LSN_STR_CHEATS														Cheats
#define LSN_STR_AVAILABLE_CHEATS											Manage Cheats
#define LSN_STR_ACTIVATE													Activate
#define LSN_STR_ACTIVATE_WITH_HOTKEY										Activate With Hotkey
#define LSN_STR_DEACTIVATE													Deactivate
#define LSN_STR_TOGGLE_HOTKEY												Toggle Hotkey
#define LSN_STR_ACCEPTED_CHEAT_FORMATS										Accepts Game Genie\u00AE, AAAA?CC:VV, and AAAA:VV:CC formats.\r\nOne code per line.
#define LSN_STR_ADD															Add
#define LSN_STR_DELETE														Delete
#define LSN_STR_EDIT														Edit
#define LSN_STR_DUPLICATE													Duplicate
#define LSN_STR_FILTER														Filter
#define LSN_STR_SEARCH														Search
#define LSN_STR_SEARCH_GAME													Search Game
#define LSN_STR_ADD_CHEAT													Add Cheat
#define LSN_STR_EDIT_CHEAT													Edit Cheat
#define LSN_STR_DELETE_CHEAT												Delete Cheat
#define LSN_STR_DUPLICATE_CHEAT												Duplicate Cheat
#define LSN_STR__ADD_CHEAT													&Add Cheat
#define LSN_STR__EDIT_CHEAT													&Edit Cheat
#define LSN_STR_DE_LETE_CHEAT												De&lete Cheat
#define LSN_STR__DUPLICATE_CHEAT											&Duplicate Cheat
#define LSN_STR__REVEAL_ENABLED_CHEATS										&Reveal Enabled Cheats
#define LSN_STR__SELECT_ENABLED_CHEATS										&Select Enabled Cheats
#define LSN_STR_DESCRIPTION_												Description:
#define LSN_STR_NOTES														Notes
#define LSN_STR_CODE														Code
#define LSN_STR_CODES														Codes
#define LSN_STR_CODE_S_														Code(s)
#define LSN_STR_TOGGLE_ON_OFF												Toggle On/Off
#define LSN_STR_ACTIVATE_DEACTIVATE											Activate/Deactivate
#define LSN_STR_TOGGLE														Toggle
#define LSN_STR_FILTER_														Filter\u2026
#define LSN_STR_SEARCH_														Search\u2026
#define LSN_STR_HOTKEY_S_													Hotkey(s)
#define LSN_STR_VALID_JSON_BUT_INVALID_STRUCTURE							Valid .JSON file but invalid cheat-file structure.
#define LSN_STR_INVALID_CODE												Invalid code.


// Video export.
#define LSN_STR_H_264_X264												H.264 / AVC (x264)
#define LSN_STR_H_264_NVENC												H.264 / AVC (NVIDIA NVENC)
#define LSN_STR_H_264_AMF												H.264 / AVC (AMD AMF)
#define LSN_STR_H_264_QSV												H.264 / AVC (Intel Quick Sync)
#define LSN_STR_HEVC_X265												H.265 / HEVC (x265)
#define LSN_STR_HEVC_NVENC												H.265 / HEVC (NVIDIA NVENC)
#define LSN_STR_HEVC_AMF												H.265 / HEVC (AMD AMF)
#define LSN_STR_HEVC_QSV												H.265 / HEVC (Intel Quick Sync)
#define LSN_STR_AV1_SVT													AV1 (SVT-AV1)
#define LSN_STR_AV1_NVENC												AV1 (NVIDIA NVENC)
#define LSN_STR_VP9_LIBVPX												VP9 (WebM)
#define LSN_STR_APPLE_PRORES											Apple ProRes (QuickTime)
#define LSN_STR_FFV1_LOSSLESS											FFV1 Lossless (Matroska)
#define LSN_STR_UT_VIDEO_LOSSLESS										Ut Video Lossless (AVI)
#define LSN_STR_MPEG_4_PART_2											MPEG-4 Part 2

#define LSN_STR_VPF_YUV420P												YUV 4:2:0 (8-bit)
#define LSN_STR_VPF_YUV422P												YUV 4:2:2 (8-bit)
#define LSN_STR_VPF_YUV444P												YUV 4:4:4 (8-bit)
#define LSN_STR_VPF_NV12												NV12 (YUV 4:2:0 8-bit)
#define LSN_STR_VPF_P010												P010 (YUV 4:2:0 10-bit)
#define LSN_STR_VPF_YUV420P10											YUV 4:2:0 (10-bit)
#define LSN_STR_VPF_YUV422P10											YUV 4:2:2 (10-bit)
#define LSN_STR_VPF_YUV444P10											YUV 4:4:4 (10-bit)
#define LSN_STR_VPF_YUV444P16											YUV 4:4:4 (16-bit)
#define LSN_STR_VPF_GBRP												RGB 4:4:4 (8-bit)
#define LSN_STR_VPF_GBRP10												RGB 4:4:4 (10-bit)

#define LSN_STR_VAC_AAC													AAC
#define LSN_STR_VAC_ALAC												ALAC (Lossless)
#define LSN_STR_VAC_FLAC												FLAC (Lossless)
#define LSN_STR_VAC_OPUS												Opus
#define LSN_STR_VAC_MP3													MP3
#define LSN_STR_VAC_PCM_S16												PCM (16-bit)
#define LSN_STR_VAC_PCM_S24												PCM (24-bit)
#define LSN_STR_VAC_PCM_F32												PCM (32-bit Float)

#define LSN_STR_VQM_NONE												Not Applicable
#define LSN_STR_VQM_CRF													Constant Rate Factor (CRF)
#define LSN_STR_VQM_QP													Constant Quantizer (QP)
#define LSN_STR_VQM_CQ													Constant Quality (CQ)
#define LSN_STR_VQM_ICQ													Intelligent Constant Quality (ICQ)
#define LSN_STR_VQM_QSCALE												Quantizer Scale
#define LSN_STR_VQM_PROFILE												Profile
#define LSN_STR_VQM_BITRATE												Bitrate (kbps)

#define LSN_STR_CSP_SRGB													sRGB
#define LSN_STR_CSP_BT709													BT.709
#define LSN_STR_CSP_SMPTE_C													SMPTE C (SMPTE 170M/BT.601 525)
#define LSN_STR_CSP_NTSC_1953												NTSC 1953
#define LSN_STR_CSP_BT470_NTSC												BT.470 System M (NTSC)
#define LSN_STR_CSP_BT470_PAL												BT.470 System B/G (PAL)
#define LSN_STR_CSP_BT601_625												BT.601 625
#define LSN_STR_CSP_BT2020													BT.2020
#define LSN_STR_CSP_NTSC_J													NTSC-J
#define LSN_STR_CSP_TOSHIBA_CF2005											Toshiba CF2005
#define LSN_STR_CSP_SONY_PVM_20M2U											Sony PVM-20M2U
#define LSN_STR_CSP_SONY_PVM_20L2MDU										Sony PVM-20L2MDU
#define LSN_STR_CSP_SONY_P22_TRINITRON										Sony Trinitron (P22)
#define LSN_STR_CSP_TOSHIBA_1992											Toshiba TV (1992 Patent)
#define LSN_STR_CSP_MATSUSHITA_1979											Matsushita TV (1979 Patent)
#define LSN_STR_CSP_RCA_COLORTRAK											RCA ColorTrak Remote E13169GM
#define LSN_STR_CSP_SONY_GDM_17SE1											Sony GDM-17SE1 Trinitron Monitor
#define LSN_STR_CSP_APPLE_MULTIPLE_SCAN_1705								Apple Multiple Scan 1705 Monitor
#define LSN_STR_CSP_NEC_MULTISYNC_C400										NEC MultiSync C400 Monitor
#define LSN_STR_CSP_KDS_VS19												KDS VS19 Monitor

#define LSN_STR_PRORES_PROXY											Proxy
#define LSN_STR_PRORES_LT												LT
#define LSN_STR_PRORES_STANDARD											Standard
#define LSN_STR_PRORES_HQ												HQ
#define LSN_STR_PRORES_4444												4444
#define LSN_STR_PRORES_4444_XQ											4444 XQ

// Video export errors.
#define LSN_VE_TITLE													Video Export
#define LSN_VE_FFMPEG_NOT_FOUND											FFmpeg could not be found.  Put the FFmpeg executable in the BeesNES folder or in a folder listed in the PATH environment variable.  FFmpeg can be downloaded from ffmpeg.org.
#define LSN_VE_FFMPEG_FAILED_TO_RUN										FFmpeg (\u201C{}\u201D) could not be run.
#define LSN_VE_MISSING_ENCODER											This build of FFmpeg (\u201C{}\u201D) does not include the \u201C{}\u201D encoder needed for {}.
#define LSN_VE_MISSING_MUXER											This build of FFmpeg (\u201C{}\u201D) cannot write \u201C{}\u201D files.
#define LSN_VE_MISSING_FILTER											This build of FFmpeg (\u201C{}\u201D) does not include the \u201C{}\u201D filter.
#define LSN_VE_TEST_FAILED												FFmpeg could not encode {} with the selected settings.  If the encoder runs on a GPU then the GPU or its driver may not support it or these settings.\r\n\r\n{}
#define LSN_VE_MOVIES_FOLDER_FAILED										The folder for movies could not be created.  Check the output folder in the video recording settings.
#define LSN_VE_PIPE_FAILED												The pipes to FFmpeg could not be created.
#define LSN_VE_LAUNCH_FAILED											FFmpeg (\u201C{}\u201D) could not be started.
#define LSN_VE_FFMPEG_STOPPED											FFmpeg stopped accepting data while making \u201C{}\u201D (exit code {}).  Its log was saved to \u201C{}\u201D.\r\n\r\n{}
#define LSN_VE_FFMPEG_ERROR												FFmpeg reported an error while finishing \u201C{}\u201D (exit code {}).  Its log was saved to \u201C{}\u201D.\r\n\r\n{}
#define LSN_VE_FFMPEG_TIMEOUT											FFmpeg did not finish \u201C{}\u201D in time and was stopped.  The file may be incomplete.
#define LSN_VE_FRAME_TOO_LARGE											The captured frame ({}\u00D7{}) is too large for {} (the limit is {}\u00D7{}).
#define LSN_VE_FRAME_TOO_SMALL											The captured frame ({}\u00D7{}) is too small to export.
#define LSN_VE_UNSUPPORTED_CAPTURE										Frames captured in this pixel format cannot be exported.
#define LSN_VE_INVALID_FORMAT											The selected video format is not valid.
#define LSN_VE_FRAMES_DROPPED											The movie was made but {} emulated frames are missing from it (the first was PPU frame {}).  Please report this.
#define LSN_VE_NO_CAPTURES												No frames are being captured from the current video filter so the movie cannot be made.  Try another video filter or graphics API.
#define LSN_VE_DX12_CAPTURE_FORMAT										Direct3D 12 cannot capture the swap chain (DXGI format {} with {} samples).
#define LSN_VE_DX12_CAPTURE_BUFFER										Direct3D 12 could not create the {}-byte buffer used to read frames back from the GPU.
#define LSN_VE_DX12_CAPTURE_MAP											Direct3D 12 could not map the buffer used to read frames back from the GPU (HRESULT 0x{:08X}).
#define LSN_VE_CAPTURE_AREA_EMPTY										The capture area ({} {} to {} {}) does not overlap the {}\u00D7{} swap chain.
#define LSN_VE_DX12_NOT_RENDERED										Direct3D 12 did not render the frame that was to be captured.
#define LSN_VE_DX12_CAPTURE_LIST										Direct3D 12 could not create the command list used to capture frames.
#define LSN_VE_DX12_CAPTURE_HRESULT										Direct3D 12 failed to capture a frame:  {} returned HRESULT 0x{:08X}.
#define LSN_VE_FFMPEG_PATH_INVALID										FFmpeg could not be found at \u201C{}\u201D.  Check the FFmpeg path in the video recording settings.

// Video recording settings.
#define LSN_VR_TITLE													Video-Recording Settings
#define LSN_VR_OUTPUT													Output
#define LSN_VR_FOLDER_													Folder:
#define LSN_VR_FFMPEG_													FFmpeg:
#define LSN_VR_BROWSE													Browse
#define LSN_VR_FFMPEG_CUE												Search local folder, .\\ffmpeg\\bin, then PATH
#define LSN_VR_BROWSE_FOLDER											Select the folder into which to save movies.
#define LSN_VR_FFMPEG_TYPES												FFmpeg (ffmpeg.exe)\0ffmpeg.exe\0Programs (*.exe)\0*.exe\0All Files (*.*)\0*.*\0\0
#define LSN_VR_VIDEO													Video
#define LSN_VR_FORMAT_													Format:
#define LSN_VR_PRESET_													Preset:
#define LSN_VR_QUALITY_													Quality:
#define LSN_VR_QUALITY													Quality
#define LSN_VR_ALT_QUALITY_												Alt. Quality:
#define LSN_VR_ALT_QUALITY												Alt. Quality
#define LSN_VR_LOSSLESS													Lossless
#define LSN_VR_USE														Use
#define LSN_VR_RANGE_LOWER_BETTER										{}:  {} to {} (Lower Is Better)
#define LSN_VR_RANGE_HIGHER_BETTER										{}:  {} to {} (Higher Is Better)
#define LSN_VR_NAMED_VALUE												{}:  {} = {}
#define LSN_VR_ALWAYS_LOSSLESS											Always Lossless
#define LSN_VR_NO_PRESETS												No Presets
#define LSN_VR_PIXEL_FORMAT_											Pixel Format:
#define LSN_VR_FRAME_RATE_												Frame Rate:
#define LSN_VR_MATCH_CONSOLE											Match Console
#define LSN_VR_BLEND_FRAMES												Blend Frames When Not Matching the Console
#define LSN_VR_FPS_CUE													{} or {}
#define LSN_VR_AUDIO													Audio
#define LSN_VR_CODEC_													Codec:
#define LSN_VR_BITRATE_													Bitrate:
#define LSN_VR_KBPS														Kilobits per Second
#define LSN_VR_AUDIO_BITRATE											The audio bitrate
#define LSN_VR_NO_BITRATE												Not Applicable (Lossless)
#define LSN_VR_CONDITIONS												Conditions
#define LSN_VR_START_CONDITION_											Start Condition:
#define LSN_VR_END_CONDITION_											End Condition:
#define LSN_VR_NONE														None
#define LSN_VR_START_AT_FRAME											Start at Frame\u2026									// Start at Frame...
#define LSN_VR_END_AT_FRAME												End at Frame\u2026										// End at Frame...
#define LSN_VR_DURATION_SECONDS											Duration (Seconds)
#define LSN_VR_START_FRAME_CUE											First PPU frame to record (such as {})
#define LSN_VR_END_FRAME_CUE											PPU frame at which to stop (such as {})
#define LSN_VR_SECONDS_CUE												Seconds to record (such as {})
#define LSN_VR_DEFAULTS													Restore Defaults
#define LSN_VR_ERR_RANGE												{} must be a whole number from {} to {}.
#define LSN_VR_ERR_FPS													The frame rate must be a whole number of frames per second (such as {}) or a pair of whole numbers that make a fraction (such as {} for {} frames per second).  It must be above 0 and no more than {} frames per second.
#define LSN_VR_ERR_START_FRAME											The start frame must be a whole number (such as {}).
#define LSN_VR_ERR_END_FRAME											The end frame must be a whole number (such as {}).
#define LSN_VR_ERR_END_BEFORE_START										The end frame must be larger than the start frame.
#define LSN_VR_ERR_DURATION												The duration must be a number of seconds larger than 0 (such as {}).
#define LSN_VR_ERR_FFMPEG												FFmpeg could not be found at \u201C{}\u201D.  Enter the path of ffmpeg.exe (or of the folder that contains it).  Leave it blank to search for FFmpeg.

// Video Options.
#define LSN_STR_VO_TITLE															Video Options
#define LSN_STR_VO_HOST_MACHINE														Host Machine
#define LSN_STR_VO_GLOBAL_SETTINGS													Global Settings
#define LSN_STR_VO_PERGAME_SETTINGS													Per-Game Settings
#define LSN_STR_VO_TAB_PALETTE														Palette
#define LSN_STR_VO_TAB_BLARGG														Blargg
#define LSN_STR_VO_TAB_LSPIRO_NTSC													L. Spiro NTSC
#define LSN_STR_VO_TAB_LSPIRO_PAL													L. Spiro PAL
#define LSN_STR_VO_TAB_LMP_NTSC														LMP88959 NTSC
#define LSN_STR_VO_TAB_LMP_PAL														LMP88959 PAL
#define LSN_STR_VO_REGIONAL_PRESET													Regional Preset:
#define LSN_STR_VO_RESTORE_DEFAULTS													Restore Defaults
#define LSN_STR_VO_REGION_NTSC_M													NTSC-M
#define LSN_STR_VO_REGION_NTSC_J													NTSC-J
#define LSN_STR_VO_REGION_PAL_I														PAL-I
#define LSN_STR_VO_REGION_PAL_DK													PAL-D/K
#define LSN_STR_VO_REGION_PAL_M														PAL-M
#define LSN_STR_VO_REGION_PAL_N														PAL-N

// Video Options:  groups.
#define LSN_STR_VO_GROUP_DISPLAY													Display
#define LSN_STR_VO_GROUP_COLOR_CORRECTION											Color Correction
#define LSN_STR_VO_GROUP_PHOSPHOR_DECAY												Phosphor Decay
#define LSN_STR_VO_GROUP_INTERNAL_SCALING											Internal Scaling
#define LSN_STR_VO_GROUP_CONVOLUTION												Convolution
#define LSN_STR_VO_GROUP_PALETTE													Palette
#define LSN_STR_VO_GROUP_BLARGG														Blargg NTSC
#define LSN_STR_VO_GROUP_STANDARD													Standard Settings
#define LSN_STR_VO_GROUP_DEEP														Deep Internals
#define LSN_STR_VO_GROUP_NOISE														Noise
#define LSN_STR_VO_GROUP_SW_PHOSPHOR												Software Phosphor Decay
#define LSN_STR_VO_GROUP_LMP														Settings

// Video Options:  labels.
#define LSN_STR_VO_MONITOR_COLOR_SPACE												Monitor Color Space:
#define LSN_STR_VO_MONITOR_GAMMA													Monitor Gamma:
#define LSN_STR_VO_WHITE_ADAPTATION													White-Point Adaptation:
#define LSN_STR_VO_GAMUT															Out-of-Gamut Handling:
#define LSN_STR_VO_OUTPUT_RANGE														Output Range:
#define LSN_STR_VO_TV_PRESET														TV Preset:
#define LSN_STR_VO_COLOR_SPACE														Color Space:
#define LSN_STR_VO_GAMMA															Gamma:
#define LSN_STR_VO_CRT_WHITE														CRT White:
#define LSN_STR_VO_CRT_BRIGHTNESS													CRT Brightness:
#define LSN_STR_VO_ENABLED															Enabled
#define LSN_STR_VO_BASE_LEVEL														Base Level:
#define LSN_STR_VO_TIME																Time:
#define LSN_STR_VO_RED_MULTIPLIER													Red Multiplier (%):
#define LSN_STR_VO_BLUE_MULTIPLIER													Blue Multiplier (%):
#define LSN_STR_VO_SCALE_X															X:
#define LSN_STR_VO_SCALE_Y															Y:
#define LSN_STR_VO_DYNAMIC															Dynamic
#define LSN_STR_VO_LOW_FILTER														Low Filter:
#define LSN_STR_VO_MEDIUM_FILTER													Medium Filter:
#define LSN_STR_VO_HIGH_FILTER														High Filter:
#define LSN_STR_VO_MAX_FILTER														Max Filter:
#define LSN_STR_VO_LOAD																Load:
#define LSN_STR_VO_SHOW_COLOR_INDICES												Show Color Indices
#define LSN_STR_VO_HUE																Hue (Degrees):
#define LSN_STR_VO_HUE_OFFSET														Hue Offset (Degrees):
#define LSN_STR_VO_BRIGHTNESS														Brightness:
#define LSN_STR_VO_CONTRAST															Contrast:
#define LSN_STR_VO_SATURATION														Saturation:
#define LSN_STR_VO_WHITE_LEVEL														White Level:
#define LSN_STR_VO_BLACK_LEVEL														Black Level:
#define LSN_STR_VO_BLARGG_GAMMA														Gamma (Software):
#define LSN_STR_VO_SHARPNESS														Sharpness:
#define LSN_STR_VO_FRINGING															Fringing:
#define LSN_STR_VO_BLEED															Bleed:
#define LSN_STR_VO_ARTIFACTS														Artifacts:
#define LSN_STR_VO_MERGE_FIELDS														Merge Fields
#define LSN_STR_VO_CHROMA_FILTER													Chrominance Filter:
#define LSN_STR_VO_CHROMA_PARM0														Chrominance Parameter 1:
#define LSN_STR_VO_CHROMA_PARM1														Chrominance Parameter 2:
#define LSN_STR_VO_CHROMA_KERNEL													Chrominance Kernel Size:
#define LSN_STR_VO_LUMA_FILTER														Luminance Filter:
#define LSN_STR_VO_LUMA_PARM0														Luminance Parameter 1:
#define LSN_STR_VO_LUMA_PARM1														Luminance Parameter 2:
#define LSN_STR_VO_LUMA_KERNEL														Luminance Kernel Size:
#define LSN_STR_VO_VERTICAL_COMB													Vertical Comb:
#define LSN_STR_VO_DEEP_PRESET														Preset:
#define LSN_STR_VO_NOISE_TYPE														Type:
#define LSN_STR_VO_NOISE_AMOUNT														Amount:
#define LSN_STR_VO_NOISE															Noise:
#define LSN_STR_VO_BLEND															Blend
#define LSN_STR_VO_VSYNC															V-Sync
#define LSN_STR_VO_HSYNC															H-Sync
#define LSN_STR_VO_CHROMA_CORRECTION												Chroma Correction

// Video Options:  combo-box entries.
#define LSN_STR_VO_CUSTOM															Custom
#define LSN_STR_VO_NONE																None
#define LSN_STR_VO_GAMMA_AUTO														Automatic
#define LSN_STR_VO_GAMMA_CRT1														CRT 1
#define LSN_STR_VO_GAMMA_CRT2														CRT 2
#define LSN_STR_VO_GAMMA_SRGB														sRGB
#define LSN_STR_VO_GAMMA_SMPTE170M													SMPTE 170M
#define LSN_STR_VO_GAMMA_DCIP3														DCI-P3
#define LSN_STR_VO_GAMMA_ADOBERGB													Adobe RGB
#define LSN_STR_VO_GAMMA_SMPTE240M													SMPTE 240M
#define LSN_STR_VO_GAMMA_POW_1_96													Power 1.96
#define LSN_STR_VO_GAMMA_POW_2_0													Power 2.0
#define LSN_STR_VO_GAMMA_POW_2_2													Power 2.2
#define LSN_STR_VO_GAMMA_POW_2_35													Power 2.35
#define LSN_STR_VO_GAMMA_POW_2_4													Power 2.4
#define LSN_STR_VO_GAMMA_POW_2_5													Power 2.5
#define LSN_STR_VO_GAMMA_POW_2_7													Power 2.7
#define LSN_STR_VO_GAMMA_POW_2_8													Power 2.8
#define LSN_STR_VO_WA_BRADFORD														Bradford
#define LSN_STR_VO_WA_VON_KRIES														Von Kries
#define LSN_STR_VO_WA_XYZ_SCALING													XYZ Scaling
#define LSN_STR_VO_WA_CAT02															CAT02
#define LSN_STR_VO_GM_CLIP															Clip
#define LSN_STR_VO_GM_PRESERVE_HUE													Preserve Hue
#define LSN_STR_VO_OR_FULL															Full (0-255)
#define LSN_STR_VO_OR_LIMITED														Limited (16-235)
#define LSN_STR_VO_NT_CUSTOM														Custom Amount
#define LSN_STR_VO_NT_PHYSICAL														Physics-Based
#define LSN_STR_VO_CF_DELAY_LINE													1H Chroma Delay Line
#define LSN_STR_VO_CF_3_LINE														2H (3-Line) Chroma Comb
#define LSN_STR_VO_NCF_2_LINE														2-Line (1H) Comb
#define LSN_STR_VO_NCF_3_LINE														3-Line (2H) Comb
#define LSN_STR_VO_NCF_ADAPTIVE														Adaptive 3-Line (2D) Comb
#define LSN_STR_VO_DP_LSPIRO_NTSC													L. Spiro NTSC Sharp
#define LSN_STR_VO_DP_LSPIRO_NTSC_2													L. Spiro NTSC Slightly Sharp
#define LSN_STR_VO_DP_LSPIRO_NTSC_SOFT												L. Spiro NTSC Soft
#define LSN_STR_VO_DP_LSPIRO_NTSC_SOFT_2											L. Spiro NTSC Soft 2
#define LSN_STR_VO_DP_FAST															Fast (Box)
#define LSN_STR_VO_DP_PAL_I															PAL-I
#define LSN_STR_VO_DP_PAL_DK														PAL-D/K (Dendy)
#define LSN_STR_VO_DP_PAL_M															PAL-M
#define LSN_STR_VO_DP_PAL_N															PAL-N
#define LSN_STR_VO_FF_BOX															Box
#define LSN_STR_VO_FF_LANCZOS														Lanczos
#define LSN_STR_VO_FF_KAISER														Kaiser
#define LSN_STR_VO_FF_BLACKMAN														Blackman
#define LSN_STR_VO_FF_GAUSSIAN														Gaussian
#define LSN_STR_VO_FF_CARDINAL_SPLINE_UNIFORM										Cardinal Spline Uniform
#define LSN_STR_VO_FF_BARTLETT														Bartlett
#define LSN_STR_VO_FF_CRT_HUMP														CRT Hump
#define LSN_STR_VO_RF_POINT															Point
#define LSN_STR_VO_RF_LINEAR														Linear
#define LSN_STR_VO_RF_QUADRATICSHARP												Quadratic Sharp
#define LSN_STR_VO_RF_QUADRATIC														Quadratic
#define LSN_STR_VO_RF_QUADRATICAPPROX												Quadratic Approximate
#define LSN_STR_VO_RF_QUADRATICMIX													Quadratic Mix
#define LSN_STR_VO_RF_KAISER														Kaiser
#define LSN_STR_VO_RF_LANCZOS2														Lanczos2
#define LSN_STR_VO_RF_LANCZOS3														Lanczos3
#define LSN_STR_VO_RF_LANCZOS4														Lanczos4
#define LSN_STR_VO_RF_LANCZOS6														Lanczos6
#define LSN_STR_VO_RF_LANCZOS8														Lanczos8
#define LSN_STR_VO_RF_LANCZOS12														Lanczos12
#define LSN_STR_VO_RF_LANCZOS64														Lanczos64
#define LSN_STR_VO_RF_MITCHELL														Mitchell
#define LSN_STR_VO_RF_ROBIDOUX														Robidoux
#define LSN_STR_VO_RF_ROBIDOUXSHARP													Robidoux Sharp
#define LSN_STR_VO_RF_ROBIDOUXSOFT													Robidoux Soft
#define LSN_STR_VO_RF_CATMULLROM													Catmull-Rom
#define LSN_STR_VO_RF_BSPLINE														B-Spline
#define LSN_STR_VO_RF_ADOBEBICUBIC													Adobe Bicubic
#define LSN_STR_VO_RF_CARDINALSPLINEUNIFORM											Cardinal Spline Uniform
#define LSN_STR_VO_RF_HERMITE														Hermite
#define LSN_STR_VO_RF_HAMMING														Hamming
#define LSN_STR_VO_RF_HANNING														Hanning
#define LSN_STR_VO_RF_BLACKMAN														Blackman
#define LSN_STR_VO_RF_JINC															Jinc
#define LSN_STR_VO_RF_GAUSSIANSHARP													Gaussian Sharp
#define LSN_STR_VO_RF_GAUSSIAN														Gaussian
#define LSN_STR_VO_RF_BELL															Bell
#define LSN_STR_VO_PAL_STANDARD_NTSC												Standard NTSC
#define LSN_STR_VO_PAL_STANDARD_PAL													Standard PAL
#define LSN_STR_VO_PAL_TOSHIBA														Toshiba CF2005 (1985)
#define LSN_STR_VO_PAL_PANASONIC													Panasonic CT36D30B (2000)
#define LSN_STR_VO_PAL_FROM_FILE													From File\u2026
#define LSN_STR_VO_PAL_TYPES														Palette Files (*.pal, *.fpal)\0*.pal;*.fpal\0All Files (*.*)\0*.*\0\0

// Video Options:  tool tips.
#define LSN_STR_VO_TT_MONITOR_COLOR_SPACE											Select your monitor\u2019s color space.  sRGB is the most common on Microsoft\u00AE Windows\u00AE.
#define LSN_STR_VO_TT_MONITOR_GAMMA													Select your monitor\u2019s gamma curve.  sRGB is the most common on Microsoft\u00AE Windows\u00AE.
#define LSN_STR_VO_TT_WHITE_ADAPTATION												How the white point of the emulated CRT is mapped to the white point of your monitor.  None keeps the CRT\u2019s own white (such as the bluish white of Japanese televisions), as it looked on the original screen.  Bradford (the most common), Von Kries, XYZ Scaling, and CAT02 turn the CRT\u2019s white into your monitor\u2019s white, the way your eyes would adapt to it.
#define LSN_STR_VO_TT_GAMUT															How colors that your monitor cannot show are brought into its range.  Preserve Hue desaturates such a color toward gray just enough to fit, keeping its hue.  Clip clamps each color channel separately, which keeps more saturation but can shift the hue.
#define LSN_STR_VO_TT_OUTPUT_RANGE													The range of the values sent to your monitor.  PC monitors use Full (0-255).  Use Limited (16-235) only for a television or capture device that expects video levels; the wrong range makes blacks gray or crushes the shadows.  Applies only to GPU filters.
#define LSN_STR_VO_TT_TV_PRESET														Select a physical television to model.  This fills in the color space, the hue offset, and the saturation with the values of that television.
#define LSN_STR_VO_TT_COLOR_SPACE													Select the CRT\u2019s color space.
#define LSN_STR_VO_TT_GAMMA															Select the CRT\u2019s gamma curve.
#define LSN_STR_VO_TT_CRT_WHITE														The white level (Lw) of the CRT 1 gamma curve.  1.0 is the standard white level.
#define LSN_STR_VO_TT_CRT_BRIGHTNESS												The brightness (black lift, B) of the CRT 1 gamma curve.  Higher values raise the darkest shades.
#define LSN_STR_VO_TT_PHOSPHOR_ENABLED												Emulates the afterglow of the CRT\u2019s phosphors, which leaves a fading trail behind moving objects. On the L. Spiro filters, this also turns the phosphor decay of the software filter on or off (its level and time are under Software Phosphor Decay).
#define LSN_STR_VO_TT_BASE_LEVEL													The maximum phosphor decay level.
#define LSN_STR_VO_TT_TIME															The amount of time it takes to decay away in seconds.
#define LSN_STR_VO_TT_RED_MULTIPLIER												The red multiplier, typically around 45%.
#define LSN_STR_VO_TT_BLUE_MULTIPLIER												The blue multiplier, typically around 25%.
#define LSN_STR_VO_TT_SCALE_X														Internal scaling helps create a blocky pixelated look. Set to 1 for a smooth interpolation between neighboring pixels.
#define LSN_STR_VO_TT_SCALE_Y														Setting the Y to a higher value, such as 4 or 5, while keeping X at 1, can create a mild scanline effect.
#define LSN_STR_VO_TT_DYNAMIC														Scaling will be set to one level higher than the X and Y resolutions, maxed to the values set above.
#define LSN_STR_VO_TT_CONVOLUTION													Hardware bilinear filtering produces a poor result at low resolutions. Vertical/horizontal lines won\u2019t necessarily appear the same widths/heights as patterns repeat across the screen. Using hardware bilinear filtering doesn\u2019t fully eliminate this aliasing, and produces a blurry result.  Convolution can eliminate nearly all aliasing at low resolutions and keep the image sharp.
#define LSN_STR_VO_TT_LOW_FILTER													The convolution filter to use when the screen resolution is below 2.5\u00D7 the console\u2019s native resolution. Ultra-sharp filters such as Cardinal Spline Uniform, Kaiser, and Lanczos2 are recommended for blocky pixelated filters or sharp CRT screen effects, or moderately sharp filters such as Robidoux Sharp or Robidoux for low-end/blurry-screened CRT effects.
#define LSN_STR_VO_TT_MEDIUM_FILTER													The convolution filter to use when the screen resolution is above or equal to 2.5\u00D7 and below 3.5\u00D7 the console\u2019s native resolution. Robidoux Sharp is a good choice for sharper results, while Robidoux and Robidoux Soft are good for low-end/blurry-screened CRT effects.
#define LSN_STR_VO_TT_HIGH_FILTER													The convolution filter to use when the screen resolution is above or equal to 3.5\u00D7 and below 4.5\u00D7 the console\u2019s native resolution. At these scales, sharpening filters are no longer necessary, and can be expensive, so Linear is a good universal option that is also more efficient to run than most other filters.
#define LSN_STR_VO_TT_MAX_FILTER													The convolution filter to use when the screen resolution is above or equal to 4.5\u00D7. At these resolutions, hardware bilinear filtering eliminates aliasing well enough that convolution is no longer necessary (and can cause major performance problems). It is recommended to keep this as None unless you are looking for the highest-quality screenshots or movie exports you can get.
#define LSN_STR_VO_TT_LOAD															Select one of the built-in palettes, or select From File\u2026 to load a .pal or .fpal palette file.  The built-in palettes also select the CRT gamma curve that goes with them.
#define LSN_STR_VO_TT_PALETTE_DISPLAY												The colors of the loaded palette.  Scroll down to see the rest of a 512-entry palette.
#define LSN_STR_VO_TT_SHOW_COLOR_INDICES											Shows the index of each color on top of it in the palette display.
#define LSN_STR_VO_TT_HUE															Rotates every color around the color wheel, in degrees, like the Tint control of a television.
#define LSN_STR_VO_TT_HUE_OFFSET													An extra hue rotation, in degrees, added to Hue.  TV presets fill this in with the hue rotation of the decoder of that television.
#define LSN_STR_VO_TT_BRIGHTNESS													Raises or lowers the brightness of the whole picture.
#define LSN_STR_VO_TT_CONTRAST														The gain of the picture around middle gray.  Higher values make the darks darker and the lights lighter.
#define LSN_STR_VO_TT_SATURATION													The strength of the colors.  Lower values move the picture toward black and white.
#define LSN_STR_VO_TT_WHITE_LEVEL													The signal level shown as full white.  Lower values brighten the picture but can clip the brightest shades.
#define LSN_STR_VO_TT_BLACK_LEVEL													The signal level shown as black.  Higher values darken the shadows but can crush them to black.
#define LSN_STR_VO_TT_BLARGG_GAMMA													The gamma adjustment of Blargg\u2019s software filter (the GPU versions use the Gamma setting under Color Correction instead).  0 is the standard.
#define LSN_STR_VO_TT_SHARPNESS														The sharpness of edges.  Negative values soften the picture and positive values sharpen it.
#define LSN_STR_VO_TT_FRINGING														The amount of color fringing:  colored edges caused by luma leaking into the chroma signal.
#define LSN_STR_VO_TT_BLARGG_BLEED													The amount of color bleeding:  chroma spreading sideways past the edges of objects.
#define LSN_STR_VO_TT_ARTIFACTS														The amount of dot crawl and rainbow artifacts:  chroma leaking into the luma signal.
#define LSN_STR_VO_TT_MERGE_FIELDS													Blends the 2 fields with alternating artifact patterns to remove the flicker they cause, at the cost of slightly blurrier artifacts.
#define LSN_STR_VO_TT_LSPIRO_BLEED													How far colors smear sideways into neighboring pixels.
#define LSN_STR_VO_TT_CHROMA_FILTER													The filter that extracts the color (chrominance) signal from the composite signal.  Sharper filters keep color edges crisp but can ring; softer filters blur color edges.
#define LSN_STR_VO_TT_FILTER_PARMS													Lanczos:  Parameter 1 is the size of the window in lobes (it can be fractional, such as 2.475).  Kaiser:  Parameter 1 is \u03B2 (0 is a rectangular window; 5 is close to a Hamming window; 8.6 is close to a Blackman window; larger values taper the window more, trading sharpness for less ringing).  Blackman:  Parameter 1 is the cut-off frequency in Hz and Parameter 2 is the rate of the signal in Hz (42954545 for NTSC; 5320342.5 times the signals per pixel for PAL).  CRT Hump:  Parameter 1 is the width of the flat middle in signals and Parameter 2 is the power of the fall-offs (1 is linear).  The other filters have no parameters.  Selecting a filter fills in its default parameters.
#define LSN_STR_VO_TT_CHROMA_KERNEL													The number of signal samples the chrominance filter spans.  Larger kernels are more accurate but slower to run.
#define LSN_STR_VO_TT_LUMA_FILTER													The filter that extracts the brightness (luminance) signal from the composite signal.  Sharper filters keep details crisp but can ring; softer filters blur details.
#define LSN_STR_VO_TT_LUMA_KERNEL													The number of signal samples the luminance filter spans.  Larger kernels are more accurate but slower to run.
#define LSN_STR_VO_TT_VERTICAL_COMB													How the chroma of neighboring lines is combined.  None decodes each line on its own (simple PAL) and shows Hanover bars.  1H Chroma Delay Line averages each line with the line above (standard PAL).  2H (3-Line) Chroma Comb averages each line with the lines above and below.
#define LSN_STR_VO_TT_NTSC_COMB														How the luminance and chrominance are separated using neighboring lines, as in NTSC televisions.  None uses only the luminance and chrominance filters (notch and band-pass sets), which leaves dot crawl and rainbow artifacts.  2-Line (1H) Comb subtracts the line above (early-1980s sets with glass or CCD delay lines).  3-Line (2H) Comb uses the lines above and below (late-1980s and 1990s digital sets).  Adaptive 3-Line (2D) Comb combs with whichever neighboring line matches better and does not comb where neither matches (1990s high-end sets).  Combing removes most dot crawl but can leave hanging dots and blurred color at vertical color changes.
#define LSN_STR_VO_TT_DEEP_PRESET													Fills in the Deep Internals (the filters, their parameters, their kernel sizes, and the vertical comb filter) with a tested combination.  Shows Custom when the settings match no preset.
#define LSN_STR_VO_TT_NOISE_TYPE													The signal noise.  None adds no noise.  Custom Amount adds Gaussian noise of the amount set below.  Physics-Based models the thermal noise of the circuitry and will read your host machine\u2019s actual CPU temperature\u2014and base the noise levels off that\u2014which requires that BeesNES be run with administrator privileges.
#define LSN_STR_VO_TT_NOISE_AMOUNT													The standard deviation of the Custom Amount noise, relative to the full signal range.  0.0225 is a typical amount.
#define LSN_STR_VO_TT_SW_BASE_LEVEL													The maximum phosphor decay level of the software filter (the GPU versions use the Phosphor Decay settings under Common instead).
#define LSN_STR_VO_TT_SW_TIME														The amount of time it takes the phosphors of the software filter to decay away in seconds.
#define LSN_STR_VO_TT_LMP_NOISE														The amount of signal noise.
#define LSN_STR_VO_TT_BLEND															Blends each new field onto the previous image, which smooths out flicker.
#define LSN_STR_VO_TT_VSYNC															Locks the picture onto the vertical sync pulse.  When off, the picture rolls, as on a television whose vertical hold is out of adjustment.
#define LSN_STR_VO_TT_HSYNC															Locks each line onto the horizontal sync pulse.  When off, the lines drift sideways, as on a television whose horizontal hold is out of adjustment.
#define LSN_STR_VO_TT_CHROMA_CORRECTION												Removes the Hanover bars of PAL by cancelling the phase error of the chroma between lines, as PAL televisions do.
#define LSN_STR_VO_TT_RESTORE_DEFAULTS												Restores the default settings of this filter for the selected regional preset.
#define LSN_STR_VO_TT_RESTORE_CONV_DEFAULTS											Restores the default convolution settings of this filter for the selected regional preset.
#define LSN_STR_VO_TT_RESTORE_HOST_DEFAULTS											Restores the default settings of your monitor.
#define LSN_STR_VO_TT_REGIONAL_PRESET												Select the region whose settings to edit.  Each region keeps its own settings for this filter.

// General.
#define LSN_ERROR															Error
#define LSN_INTERNAL_ERROR													Internal error.
#define LSN_OUT_OF_MEMORY													Out of memory.
#define LSN_FAILED_TO_CREATE_DIRECTORIES									Failed to create directories.
#define LSN_FAILED_TO_OPEN_FILE												Failed to open file.
#define LSN_FAILED_TO_CREATE_FILE											Failed to create file.
#define LSN_FAILED_TO_WRITE_TO_FILE											Failed to write to file.
#define LSN_FAILED_TO_READ_FROM_FILE										Failed to read from file.
#define LSN_FAILED_TO_EXTRACT_FILE											Failed to extract file.
#define LSN_INVALID_FILE_FORMAT												Invalid file format.
#define LSN_UNSUPPORTED_VERSION												Unsupported versiom.
#define LSN_CONTINUE														Continue?
#define LSN_INVALID_JSON_FILE												Invalid JSON file.




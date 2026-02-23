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

#define LSN_OPTIONS_TXT														Options
#define LSN_ALL_SUPPORTED_FILES___NES____ZIP____NES___ZIP_					All Supported Files (*.nes, *.zip)\0*.nes;*.zip\0
#define LSN_NES_FILES____NES____NES_										NES Files (*.nes)\0*.nes\0
#define LSN_ZIP_FILES____ZIP____ZIP_										ZIP Files (*.zip)\0*.zip\0
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
#define LSN_VIDEO_FILTER													Video &Filter
#define LSN_REGION															&Region
#define LSN_INPUT															&Input
#define LSN_VIDEO															&Video
#define LSN_AUDIO															&Audio
// Video sizes.
#define LSN_VIDEO_SIZE_1X													1\u00D7
#define LSN_VIDEO_SIZE_2X													2\u00D7
#define LSN_VIDEO_SIZE_3X													3\u00D7
#define LSN_VIDEO_SIZE_4X													4\u00D7
#define LSN_VIDEO_SIZE_5X													5\u00D7
#define LSN_VIDEO_SIZE_6X													6\u00D7
#define LSN_VIDEO_SIZE_7X													7\u00D7
#define LSN_VIDEO_SIZE_8X													8\u00D7
// Video filters.
#define LSN_VIDEO_FILTER_NONE												Pale&tte (Software)
#define LSN_VIDEO_FILTER_DX9_PALETTE										Pale&tte (Microsoft® Direct3D® 9)
#define LSN_VIDEO_FILTER_NTSC_BLARGG										NTSC-M (Blar&gg)
#define LSN_VIDEO_FILTER_NTSC_L_SPIRO										NTSC-M (&L. Spiro)
#define LSN_VIDEO_FILTER_PAL_L_SPIRO										PAL-&B/B1/G/H/I/K (L. Spiro)
#define LSN_VIDEO_FILTER_DENDY_L_SPIRO										PAL-&D/D1 (L. Spiro)//&SECAM-B/D/G/H/K/K1/L (L. Spiro)
#define LSN_VIDEO_FILTER_PALM_L_SPIRO										PAL-&M (Brazil Famiclone) (L. Spiro)
#define LSN_VIDEO_FILTER_PALN_L_SPIRO										PAL-&N (Argentina Famiclone) (L. Spiro)
#define LSN_VIDEO_FILTER_NTSC_CRT_FULL										NTSC-M (LMP&88959)
#define LSN_VIDEO_FILTER_PAL_CRT_FULL										PAL-B/B1/D/D1/G/H/I/K (LMP88&959)
#define LSN_VIDEO_FILTER_AUTO_CRT_FULL										NTSC/PAL Auto (LMP889&59)
#define LSN_VIDEO_FILTER_AUTO_LSPIRO										NTSC/PAL &Auto (L. Spiro)
#define LSN_VIDEO_FILTER_LSPIRO_NTSC_DX9_UPSCALE							NTSC-M (&L. Spiro) (Microsoft® Direct3D® 9)
#define LSN_VIDEO_FILTER_LSPIRO_PAL_DX9_UPSCALE								PAL-&B/B1/G/H/I/K (L. Spiro) (Microsoft® Direct3D® 9)
#define LSN_VIDEO_FILTER_LSPIRO_DENDY_DX9_UPSCALE							PAL-&D/D1 (L. Spiro) (Microsoft® Direct3D® 9)
#define LSN_VIDEO_FILTER_LSPIRO_PALM_DX9_UPSCALE							PAL-&M (Brazil Famiclone) (L. Spiro) (Microsoft® Direct3D® 9)
#define LSN_VIDEO_FILTER_LSPIRO_PALN_DX9_UPSCALE							PAL-&N (Argentina Famiclone) (L. Spiro) (Microsoft® Direct3D® 9)
#define LSN_VIDEO_FILTER_AUTO_LSPIRO_DX9_UPSCALE							NTSC/PAL &Auto (L. Spiro) (Microsoft® Direct3D® 9)
#define LSN_VIDEO_FILTER_DIRECT3D_9											Mi&crosoft® Direct3D® 9
#define LSN_VIDEO_FILTER_DIRECT3D_12										Microso&ft® Direct3D® 12
#define LSN_VIDEO_FILTER_VULKAN_1											&Vulkan 1
// Regions.
#define LSN_REGION_AUTO														&Automatic
#define LSN_REGION_NTSC														&NTSC
#define LSN_REGION_PAL														&PAL
#define LSN_REGION_DENDY													PAL \u201C&Dendy\u201D Famiclone
#define LSN_REGION_PALM														&Brazil Famiclone
#define LSN_REGION_PALN														Ar&gentina Famiclone

// Tools menu.
#define LSN_TOOLS_PATCH														&Patcher
#define LSN_TOOLS_WAV_EDIT													&WAV Editor

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
#define LSN_AUDIO_OPTIONS_NES_FL_N20914398									LSN_NES Front-Loading N20914398, Sunsoft 5B
#define LSN_AUDIO_OPTIONS_NES_FL_N34169630									LSN_NES Front-Loading N34169630
#define LSN_AUDIO_OPTIONS_NES_FL_N8869725									LSN_NES Front-Loading N8869725, NESRGB
#define LSN_AUDIO_OPTIONS_NES_FL_N8869725_RCA								LSN_NES Front-Loading N8869725, NESRGB, Stock RCA
#define LSN_AUDIO_OPTIONS_NES_TL_NN101255729								LSN_NES Top-Loading NN101255729, NESRGB
#define LSN_AUDIO_OPTIONS_FAMI_H11111212									Japanese LSN_FAMICOM H11111212, RF Cables
#define LSN_AUDIO_OPTIONS_TWIN_FAMI_475711									Japanese Twin LSN_FAMICOM 475711, NESRGB, Stock RCA
#define LSN_AUDIO_OPTIONS_TWIN_FAMI_475711_SCART							Japanese Twin LSN_FAMICOM 475711, NESRGB, SCART Out
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
#define LSN_PATCH_PATCH_ROM													Patch a ROM
#define LSN_PATCH_FILE														File
#define LSN_PATCH_ROM_TO_PATCH												ROM to Patch:
#define LSN_PATCH_PATCH_FILE												Patch File(s):
#define LSN_PATCH_OUTPUT_FILE												Output File:
#define LSN_PATCH_BROWSE													Browse
#define LSN_PATCH_FILE_TYPES												Patch Files (*.ips, *.bps, *.zip)\0*.ips;*.bps;*.zip\0All Files (*.*)\0*.*\0\0
#define LSN_PATCH_SRC_INFO													Source ROM Information
#define LSN_PATCH_PATCH														Patch
#define LSN_PATCH_DETAILS													Details
#define LSN_PATCH_REVEAL_COMPATIBLE											&Reveal Compatible
#define LSN_PATCH_SELECT_COMPATIBLE											&Select Compatible
#define LSN_PATCH_REVEAL_BPS												Re&veal BPS
#define LSN_PATCH_EXPAND_SELECTED											Expand Selec&ted
#define LSN_PATCH_EXPAND_ALL												E&xpand All
#define LSN_PATCH_COLLAPSE_SELECTED											C&ollapse Selected
#define LSN_PATCH_COLLAPSE_ALL												&Collapse All

// WAV Editor
#define LSN_WE_WAV_EDITOR													WAV Editor
#define LSN_WE_FILES														Files
#define LSN_WE_FILES_DESC													Channels: {}; Dur: {:.7f}; Hz: {}
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
#define LSN_WE_TT_ACTUAL_HZ_EDIT											Use this when none of the presets applies.  This can be used to assign sample rates that are measured from real hardware and deviate from the ideal Hz due to temperature, age, and natural clock variance.//Use this when none of the presets applies.  This can be used to assign sample rates that are measured from real hardware and deviate from the idea Hz due to temperature, age, and natural clock variance.  Some measured hardware Hz are:\r\n1662607.03125 \x2F 19.9972 * 19.9971 \x2F\x2F PAL: 1662598.71705085574649274349212646484375\r\n1789772.727272727272 * (1000.0\x2F60.09848) \x2F (1000.0 \x2F 60.09812); \x2F\x2F N34169630: 1789762.00623316341079771518707275390625\r\n1789772.727272727272 * (1000.0\x2F60.09848) \x2F (1000.0 \x2F 60.09857); \x2F\x2F N1151667: 1789775.40753261814825236797332763671875\r\n1789772.727272727272 * (1000.0\x2F60.09848) \x2F (1000.0 \x2F 60.09965); \x2F\x2F N0260073: 1789807.57065131026320159435272216796875\r\n1789772.727272727272 * (1000.0\x2F60.09848) \x2F (1000.0 \x2F 60.09813); \x2F\x2F Twin Famicom RCA 475711: 1789762.30403981753624975681304931640625\r\n\r\nNote that all numerical edit boxes in BeesNES allow typing of actual equations, such as \u201C1662607.03125 \x2F 19.9972 * 19.9971\u201D.
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
#define LSN_WE_TT_LOOPING													If checked, the audio will play from start to end, then continue playing for the pre-fade duration, and then continue playing during the fade-out, and include the opening and trailing silence.  The End Time will usually be set to the time the track loops (typically the exact time of the 2nd loop, in the file\u2019s native Hz).
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
#define LSN_WE_TT_VOL_NORMAL												The audio will be normalized to a given level.  All numeric edit boxes in BeesNES allow typing equations, so it is possible to specify the normalization level in a dB -> linear equation or as a specific value.
#define LSN_WE_TT_VOL_NORMAL_EDIT											The level to which to normalize the final audio.  All numeric edit boxes in BeesNES allow typing equations, so it is possible to specify the normalization level in a dB -> linear equation or as a specific value.
#define LSN_WE_TT_VOL_LOUDNESS												Uses a gated RMS evaluation to set the audio to the given loudness level.
#define LSN_WE_TT_VOL_LOUDNESS_EDIT											The level to which to set the loudness.  All numeric edit boxes in BeesNES allow typing equations, so it is possible to specify the normalization level in a dB -> linear equation or as a specific value.

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


// General.
#define LSN_ERROR															Error
#define LSN_INTERNAL_ERROR													Internal error.
#define LSN_OUT_OF_MEMORY													Out of memory.
#define LSN_FAILED_TO_CREATE_DIRECTORIES									Failed to create directories.
#define LSN_FAILED_TO_OPEN_FILE												Failed to open file.
#define LSN_FAILED_TO_CREATE_FILE											Failed to create file.
#define LSN_FAILED_TO_WRITE_TO_FILE											Failed to write to file.
#define LSN_FAILED_TO_READ_FROM_FILE										Failed to read from file.
#define LSN_INVALID_FILE_FORMAT												Invalid file format.
#define LSN_UNSUPPORTED_VERSION												Unsupported versiom.
#define LSN_CONTINUE														Continue?





/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Emulator options.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Audio/LSNAudioOptions.h"
#include "../Filters/LSNFilterBase.h"
#include "../Wav/LSNWavFile.h"
#include "LSNInputOptions.h"



namespace lsn {

	/**
	 * Class LSN_OPTIONS
	 * \brief Emulator options.
	 *
	 * Description: Emulator options.
	 */
	struct LSN_OPTIONS {
		// == Inputs to Dialogs.
		double												dApuHz;

		// == Settings.
		LSN_INPUT_OPTIONS									ioGlobalInputOptions;										/**< Global input options. */
		LSN_INPUT_OPTIONS									ioThisGameInputOptions;										/**< This game's input options. */

		LSN_AUDIO_OPTIONS									aoGlobalAudioOptions;										/**< Global audio options. */
		LSN_AUDIO_OPTIONS									aoThisGameAudioOptions;										/**< This game's audio options. */
		CWavFile::LSN_STREAM_TO_FILE_OPTIONS				stfStreamOptionsRaw;										/**< Raw capture. */
		CWavFile::LSN_STREAM_TO_FILE_OPTIONS				stfStreamOptionsOutCaptire;									/**< Output capture. */

		LSN_PPU_METRICS										pmRegion = LSN_PPU_METRICS::LSN_PM_UNKNOWN;					/**< The region to use. */
		CFilterBase::LSN_FILTERS							fFilter = CFilterBase::LSN_F_AUTO_CRT_FULL;					/**< The current filter ID. */

		std::wstring										wInRomInitPath;												/**< The initial directory for the input ROM. */
		std::wstring										wOutRomInitPath;											/**< The initial directory for the output ROM. */
		std::wstring										wPatchInitPath;												/**< The initial directory for the patch file. */
		std::wstring										wDefaultRomPath;											/**< The normal path to ROM files. */
		std::wstring										wRawAudioPath;												/**< The directory where the last raw audio file was made. */
		std::wstring										wOutAudioPath;												/**< The directory where the last output-capture audio file was made. */
		std::vector<std::wstring>							vRawStartHistory;											/**< The history of raw start-condition parameters. */
		std::vector<std::wstring>							vRawEndHistory;												/**< The history of raw end-condition parameters. */
		std::vector<std::wstring>							vOutStartHistory;											/**< The history of output-capture start-condition parameters. */
		std::vector<std::wstring>							vOutEndHistory;												/**< The history of output-capture end-condition parameters. */
	};

}	// namespace lsn

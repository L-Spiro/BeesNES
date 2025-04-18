/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Remembers the last texts, checks, etc., for the WAV Editor window.
 */

#pragma once

#include "../LSNLSpiroNes.h"

#include <string>

namespace lsn {

	/**
	 * Class LSN_WAV_EDITOR_WINDOW_OPTIONS
	 * \brief Remembers the last texts, checks, etc., for the WAV Editor window.
	 *
	 * Description: Remembers the last texts, checks, etc., for the WAV Editor window.
	 */
	struct LSN_WAV_EDITOR_WINDOW_OPTIONS {
		/** The last text in Pre-Fade Dur.. */
		std::wstring												wsPreFadeDur = L"4";
		/** The last text in Fade Dur. */
		std::wstring												wsFadeDur = L"10";
		/** The last text in Opening Silence. */
		std::wstring												wsOpeningSilence = L"0.2";
		/** The last text in Trailing Silence. */
		std::wstring												wsTrailingSilence = L"0.5";
		/** The last text in Mains Hum Volume. */
		std::wstring												wsMainsHumVolume = L"1.0";
		/** The last text in White Noise Volume. */
		std::wstring												wsWhiteNoiseVolume = L"0.1";
		
		/** The last text in Guassian Noise Bandpass. */
		std::wstring												wsGuassianNoiseBandwidth = L"1000000";
		/** The last text in Guassian Noise Temperature. */
		std::wstring												wsGuassianNoiseTemperature = L"290";
		/** The last text in Guassian Noise Resistance. */
		std::wstring												wsGuassianNoiseResistance = L"50";

		/** The last text in Actual Hz. */
		std::wstring												wsActualHz = L"";
		/** The last text in Characteristics/Volume. */
		std::wstring												wsCharVolume = L"0.402";
		/** The last text in Characteristics/LPF Hz. */
		std::wstring												wsCharLpfHz = L"7050";
		/** The last text in Characteristics/HPF 1 Hz. */
		std::wstring												wsCharHpf0Hz = L"200";
		/** The last text in Characteristics/HPF 2 Hz. */
		std::wstring												wsCharHpf1Hz = L"34.5";
		/** The last text in Characteristics/HPF 3 Hz. */
		std::wstring												wsCharHpf2Hz = L"34.5";

		/** The last text in Characteristics/LPF Falloff. */
		std::wstring												wsCharLpfFall = L"log10( 0.5 ) * 20 // -6.0205999 dB";
		/** The last text in Characteristics/HPF 1 Falloff. */
		std::wstring												wsCharHpf0Fall = L"log10( 0.5 ) * 20 // -6.0205999 dB";
		/** The last text in Characteristics/HPF 2 Falloff. */
		std::wstring												wsCharHpf1Fall = L"log10( 0.5 ) * 20 // -6.0205999 dB";
		/** The last text in Characteristics/HPF 3 Falloff. */
		std::wstring												wsCharHpf2Fall = L"log10( 0.5 ) * 20 // -6.0205999 dB";

		/** The last text in Artist. */
		std::wstring												wsArtist = L"";
		/** The last text in Album. */
		std::wstring												wsAlbum = L"";
		/** The last text in Year. */
		std::wstring												wsYear = L"";
		/** The last text in Comment. */
		std::wstring												wsComment = L"Created with BeesNES";

		/** The last text in Absolute Volume. */
		std::wstring												wsAbsoluteVolume = L"5.0";
		/** The last text in Normalize to. */
		std::wstring												wsNormalizeVolume = L"pow( 10, -5.5 / 20.0 ) // -5.5 dB";
		/** The last text in Set Loudness to. */
		std::wstring												wsLoudnessVolume = L"0.5";
		/** The last text in Output Hz. */
		std::wstring												wsOutputHz = L"48000";
		/** The last text in Output Folder. */
		std::wstring												wsOutputFolder = L"";

		/** The last Loop check. */
		bool														bLoop = false;

		/** The last Actual Hz combo selection. */
		uint32_t													ui32ActualHz = uint32_t( -LSN_PM_NTSC );
		/** The last Preset combo selection. */
		uint32_t													ui32CharPreset = uint32_t( -1 );
		/** The last Invert check. */
		bool														bInvert = true;
		/** The last LPF Hz check. */
		bool														bLpf = true;
		/** The last HPF 0 Hz check. */
		bool														bHpf0 = true;
		/** The last HPF 1 Hz check. */
		bool														bHpf1 = true;
		/** The last HPF 2 Hz check. */
		bool														bHpf2 = true;

		/** The last LPF Type combo selection. */
		uint8_t														ui8LpfType = uint8_t( 0 );
		/** The last HPF 0 Type combo selection. */
		uint8_t														ui8Hpf0Type = uint8_t( 0 );
		/** The last HPF 1 Type combo selection. */
		uint8_t														ui8Hpf1Type = uint8_t( 0 );
		/** The last HPF 2 Type combo selection. */
		uint8_t														ui8Hpf2Type = uint8_t( 0 );

		/** The last Mains Hum check. */
		bool														bMainsHum = false;
		/** The last White Noise check. */
		bool														bWhiteNoise = false;
		/** The last Dither check. */
		bool														bDither = true;

		/** The last Absolute check. */
		bool														bAbsolute = true;
		/** The last Normalize check. */
		bool														bNormalize = false;
		/** The last Loudness check. */
		bool														bLoudness = false;
		/** The last Numbered check. */
		bool														bNumbered = false;

		/** The last Mains Hum combo selection. */
		uint32_t													ui32MainsHum = 2;
		/** The last White Noise combo selection. */
		uint32_t													ui32WhiteNoise = 0;
		/** The last Format combo selection. */
		uint32_t													ui32OutFormat = CWavFile::LSN_F_PCM;
		/** The last Bits combo selection. */
		uint32_t													ui32OutBits = 32;
		/** The last Stereo combo selection. */
		uint32_t													ui32Stereo = 1;



		/** The last WAV-file folder. */
		std::wstring												wsLastWavFolder = L"";
		/** The last Meta-file folder. */
		std::wstring												wsLastMetaFolder = L"";

	};

}	// namespace lsn

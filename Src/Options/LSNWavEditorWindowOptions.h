/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Remembers the last texts, checks, etc., for the WAV Editor window.
 */

#pragma once

#include "../LSNLSpiroNes.h"
#include "../Utilities/LSNStreamBase.h"
#include "../Wav/LSNWavFile.h"

#include <string>
#include <vector>

namespace lsn {

	/** Start/stop time. */
	enum LSN_START_STOP_TIME {
		LSN_SST_EXACT,
		LSN_SST_MINUS_ONE,
		LSN_SST_SNAP,
	};

	/**
	 * Class LSN_WAV_EDITOR_WINDOW_OPTIONS
	 * \brief Remembers the last texts, checks, etc., for the WAV Editor window.
	 *
	 * Description: Remembers the last texts, checks, etc., for the WAV Editor window.
	 */
	struct LSN_WAV_EDITOR_WINDOW_OPTIONS {
		/** Per-file options. */
		struct LSN_PER_FILE {
			/** The file paths. */
			std::vector<std::wstring>								vWavPaths;
			/** The original input paths. */
			std::vector<std::wstring>								vWavInputPaths;
			/** The metadada path. */
			std::wstring											wsMetaPath;

			/** The last text in Start Time. */
			std::wstring											wsStartTime = L"0";
			/** The last text in End Time. */
			std::wstring											wsEndTime = L"";
			/** The last text in Stop Time. */
			std::wstring											wsStopTime = L"";
			/** The last text in Pre-Fade Dur. */
			std::wstring											wsPreFadeDur = L"4";
			/** The last text in Fade Dur. */
			std::wstring											wsFadeDur = L"10";
			/** The last text in Opening Silence. */
			std::wstring											wsOpeningSilence = L"0.2";
			/** The last text in Trailing Silence. */
			std::wstring											wsTrailingSilence = L"0.5";

			/** The last text in Title. */
			std::wstring											wsTitle = L"";
			/** The last text in File Name Prefix. */
			std::wstring											wsTitlePreFix = L"";
			/** The last text in Title Postfix. */
			std::wstring											wsTitlePostFix = L"";
			/** The last text in Actual Hz. */
			std::wstring											wsActualHz = L"";
			/** The last text in Characteristics/Volume. */
			std::wstring											wsCharVolume = L"0.395";
			/** The last text in Characteristics/LPF Hz. */
			std::wstring											wsCharLpfHz = L"17000";
			/** The last text in Characteristics/HPF 1 Hz. */
			std::wstring											wsCharHpf0Hz = L"285.170928955078125";
			/** The last text in Characteristics/HPF 2 Hz. */
			std::wstring											wsCharHpf1Hz = L"85.50933074951171875";
			/** The last text in Characteristics/HPF 3 Hz. */
			std::wstring											wsCharHpf2Hz = L"7.361726284027099609375";

			/** The last text in Characteristics/LPF Falloff. */
			std::wstring											wsCharLpfFall = L"log10( 0.5 ) * 20 // -6.0205999 dB";
			/** The last text in Characteristics/HPF 1 Falloff. */
			std::wstring											wsCharHpf0Fall = L"log10( 0.5 ) * 20 // -6.0205999 dB";
			/** The last text in Characteristics/HPF 2 Falloff. */
			std::wstring											wsCharHpf1Fall = L"log10( 0.5 ) * 20 // -6.0205999 dB";
			/** The last text in Characteristics/HPF 3 Falloff. */
			std::wstring											wsCharHpf2Fall = L"log10( 0.5 ) * 20 // -6.0205999 dB";

			/** The last text in Artist. */
			std::wstring											wsArtist = L"";
			/** The last text in Album. */
			std::wstring											wsAlbum = L"";
			/** The last text in Year. */
			std::wstring											wsYear = L"";
			/** The last text in Comment. */
			std::wstring											wsComment = L"Created with BeesNES";

			/** The last Start Time combo selection. */
			int32_t													i32StartSelection = 0;
			/** The last End Time combo selection. */
			int32_t													i32EndSelection = -1;
			/** The Start Time modifier. */
			int32_t													i32StartMod = int32_t( LSN_SST_EXACT );
			/** The End Time modifier. */
			int32_t													i32StopMod = int32_t( LSN_SST_EXACT );
			/** The last Actual Hz combo selection. */
			uint32_t												ui32ActualHz = uint32_t( -LSN_PM_NTSC );
			/** The last Preset combo selection. */
			uint32_t												ui32CharPreset = uint32_t( -1 );
			

			/** The last LPF Type combo selection. */
			uint8_t													ui8LpfType = uint8_t( 0 );
			/** The last HPF 0 Type combo selection. */
			uint8_t													ui8Hpf0Type = uint8_t( 0 );
			/** The last HPF 1 Type combo selection. */
			uint8_t													ui8Hpf1Type = uint8_t( 0 );
			/** The last HPF 2 Type combo selection. */
			uint8_t													ui8Hpf2Type = uint8_t( 0 );

			/** The last Lock Volume check. */
			bool													bLockVol = false;
			/** The last Invert check. */
			bool													bInvert = true;
			/** The last LPF Hz check. */
			bool													bLpf = true;
			/** The last HPF 0 Hz check. */
			bool													bHpf0 = true;
			/** The last HPF 1 Hz check. */
			bool													bHpf1 = true;
			/** The last HPF 2 Hz check. */
			bool													bHpf2 = true;

			/** RF 1. */
			bool													bRf1 = false;
			/** RF 2. */
			bool													bRf2 = false;
			/** The Sunsoft 5B volume-crunch. */
			bool													bSunsoft5b = false;

			/** The last Loop check. */
			bool													bLoop = false;



			// == Functions.
			/**
			 * Saves the structure to a stream.
			 * 
			 * \param _sbStream The stream to which to save the structure data.
			 * \return Returns true if all writes were successful.  A return of false could indicate a memory failure or a not enough space on disk.
			 **/
			bool													Save( CStreamBase &_sbStream ) const {
				if ( vWavPaths.size() > UINT_MAX ) { return false; }
				if ( vWavInputPaths.size() > UINT_MAX ) { return false; }

				if ( !_sbStream.WriteUi32( uint32_t( vWavPaths.size() ) ) ) { return false; }
				for ( size_t I = 0; I < vWavPaths.size(); ++I ) {
					if ( !_sbStream.WriteStringU16( vWavPaths[I] ) ) { return false; }
				}
				if ( !_sbStream.WriteUi32( uint32_t( vWavInputPaths.size() ) ) ) { return false; }
				for ( size_t I = 0; I < vWavInputPaths.size(); ++I ) {
					if ( !_sbStream.WriteStringU16( vWavInputPaths[I] ) ) { return false; }
				}

				if ( !_sbStream.WriteStringU16( wsMetaPath ) ) { return false; }

				if ( !_sbStream.WriteStringU16( wsStartTime ) ) { return false; }
				if ( !_sbStream.WriteStringU16( wsEndTime ) ) { return false; }
				if ( !_sbStream.WriteStringU16( wsStopTime ) ) { return false; }
				if ( !_sbStream.WriteStringU16( wsPreFadeDur ) ) { return false; }
				if ( !_sbStream.WriteStringU16( wsFadeDur ) ) { return false; }
				if ( !_sbStream.WriteStringU16( wsOpeningSilence ) ) { return false; }
				if ( !_sbStream.WriteStringU16( wsTrailingSilence ) ) { return false; }

				if ( !_sbStream.WriteStringU16( wsTitle ) ) { return false; }
				if ( !_sbStream.WriteStringU16( wsTitlePreFix ) ) { return false; }
				if ( !_sbStream.WriteStringU16( wsTitlePostFix ) ) { return false; }
				if ( !_sbStream.WriteStringU16( wsActualHz ) ) { return false; }
				if ( !_sbStream.WriteStringU16( wsCharVolume ) ) { return false; }
				if ( !_sbStream.WriteStringU16( wsCharLpfHz ) ) { return false; }
				if ( !_sbStream.WriteStringU16( wsCharHpf0Hz ) ) { return false; }
				if ( !_sbStream.WriteStringU16( wsCharHpf1Hz ) ) { return false; }
				if ( !_sbStream.WriteStringU16( wsCharHpf2Hz ) ) { return false; }

				if ( !_sbStream.WriteStringU16( wsCharLpfFall ) ) { return false; }
				if ( !_sbStream.WriteStringU16( wsCharHpf0Fall ) ) { return false; }
				if ( !_sbStream.WriteStringU16( wsCharHpf1Fall ) ) { return false; }
				if ( !_sbStream.WriteStringU16( wsCharHpf2Fall ) ) { return false; }

				if ( !_sbStream.WriteStringU16( wsArtist ) ) { return false; }
				if ( !_sbStream.WriteStringU16( wsAlbum ) ) { return false; }
				if ( !_sbStream.WriteStringU16( wsYear ) ) { return false; }
				if ( !_sbStream.WriteStringU16( wsComment ) ) { return false; }

				if ( !_sbStream.WriteI32( i32StartSelection ) ) { return false; }
				if ( !_sbStream.WriteI32( i32EndSelection ) ) { return false; }

				if ( !_sbStream.WriteI32( i32StartMod ) ) { return false; }
				if ( !_sbStream.WriteI32( i32StopMod ) ) { return false; }

				if ( !_sbStream.WriteUi32( ui32ActualHz ) ) { return false; }
				if ( !_sbStream.WriteUi32( ui32CharPreset ) ) { return false; }

				if ( !_sbStream.WriteUi8( ui8LpfType ) ) { return false; }
				if ( !_sbStream.WriteUi8( ui8Hpf0Type ) ) { return false; }
				if ( !_sbStream.WriteUi8( ui8Hpf1Type ) ) { return false; }
				if ( !_sbStream.WriteUi8( ui8Hpf2Type ) ) { return false; }

				if ( !_sbStream.WriteBool( bLockVol ) ) { return false; }
				if ( !_sbStream.WriteBool( bInvert ) ) { return false; }
				if ( !_sbStream.WriteBool( bLpf ) ) { return false; }
				if ( !_sbStream.WriteBool( bHpf0 ) ) { return false; }
				if ( !_sbStream.WriteBool( bHpf1 ) ) { return false; }
				if ( !_sbStream.WriteBool( bHpf2 ) ) { return false; }
				if ( !_sbStream.WriteBool( bLoop ) ) { return false; }

				if ( !_sbStream.WriteBool( bSunsoft5b ) ) { return false; }
				if ( !_sbStream.WriteBool( bRf1 ) ) { return false; }
				if ( !_sbStream.WriteBool( bRf2 ) ) { return false; }
				return true;
			}

			/**
			 * Loads from the given stream into the structure.
			 * 
			 * \param _sbStream The stream from which to load the data.
			 * \param _ui32Version The version of the file.
			 * \return Returns true if the file format is expected and memory is able to be allocated for each string in the structure.
			 **/
			bool													Load( const CStreamBase &_sbStream, uint32_t _ui32Version ) {
				try {
					(*this) = LSN_WAV_EDITOR_WINDOW_OPTIONS::LSN_PER_FILE();

					uint32_t ui32Total = 0;
					if ( !_sbStream.ReadUi32( ui32Total ) ) { return false; }
					vWavPaths.resize( ui32Total );
					for ( size_t I = 0; I < vWavPaths.size(); ++I ) {
						if ( !_sbStream.ReadStringU16( vWavPaths[I] ) ) { return false; }
					}
					ui32Total = 0;
					if ( !_sbStream.ReadUi32( ui32Total ) ) { return false; }
					vWavInputPaths.resize( ui32Total );
					for ( size_t I = 0; I < vWavInputPaths.size(); ++I ) {
						if ( !_sbStream.ReadStringU16( vWavInputPaths[I] ) ) { return false; }
					}

					if ( !_sbStream.ReadStringU16( wsMetaPath ) ) { return false; }

					if ( !_sbStream.ReadStringU16( wsStartTime ) ) { return false; }
					if ( !_sbStream.ReadStringU16( wsEndTime ) ) { return false; }
					if ( !_sbStream.ReadStringU16( wsStopTime ) ) { return false; }
					if ( !_sbStream.ReadStringU16( wsPreFadeDur ) ) { return false; }
					if ( !_sbStream.ReadStringU16( wsFadeDur ) ) { return false; }
					if ( !_sbStream.ReadStringU16( wsOpeningSilence ) ) { return false; }
					if ( !_sbStream.ReadStringU16( wsTrailingSilence ) ) { return false; }

					if ( !_sbStream.ReadStringU16( wsTitle ) ) { return false; }
					if ( _ui32Version >= 1 ) {
						if ( !_sbStream.ReadStringU16( wsTitlePreFix ) ) { return false; }
						if ( !_sbStream.ReadStringU16( wsTitlePostFix ) ) { return false; }
					}
					if ( !_sbStream.ReadStringU16( wsActualHz ) ) { return false; }
					if ( !_sbStream.ReadStringU16( wsCharVolume ) ) { return false; }
					if ( !_sbStream.ReadStringU16( wsCharLpfHz ) ) { return false; }
					if ( !_sbStream.ReadStringU16( wsCharHpf0Hz ) ) { return false; }
					if ( !_sbStream.ReadStringU16( wsCharHpf1Hz ) ) { return false; }
					if ( !_sbStream.ReadStringU16( wsCharHpf2Hz ) ) { return false; }

					if ( !_sbStream.ReadStringU16( wsCharLpfFall ) ) { return false; }
					if ( !_sbStream.ReadStringU16( wsCharHpf0Fall ) ) { return false; }
					if ( !_sbStream.ReadStringU16( wsCharHpf1Fall ) ) { return false; }
					if ( !_sbStream.ReadStringU16( wsCharHpf2Fall ) ) { return false; }

					if ( !_sbStream.ReadStringU16( wsArtist ) ) { return false; }
					if ( !_sbStream.ReadStringU16( wsAlbum ) ) { return false; }
					if ( !_sbStream.ReadStringU16( wsYear ) ) { return false; }
					if ( !_sbStream.ReadStringU16( wsComment ) ) { return false; }

					if ( _ui32Version >= 2 ) {
						if ( !_sbStream.ReadI32( i32StartSelection ) ) { return false; }
						if ( !_sbStream.ReadI32( i32EndSelection ) ) { return false; }

						if ( _ui32Version >= 3 ) {
							if ( !_sbStream.ReadI32( i32StartMod ) ) { return false; }
							if ( !_sbStream.ReadI32( i32StopMod ) ) { return false; }
						}
					}

					if ( !_sbStream.ReadUi32( ui32ActualHz ) ) { return false; }
					if ( !_sbStream.ReadUi32( ui32CharPreset ) ) { return false; }

					if ( !_sbStream.ReadUi8( ui8LpfType ) ) { return false; }
					if ( !_sbStream.ReadUi8( ui8Hpf0Type ) ) { return false; }
					if ( !_sbStream.ReadUi8( ui8Hpf1Type ) ) { return false; }
					if ( !_sbStream.ReadUi8( ui8Hpf2Type ) ) { return false; }

					if ( !_sbStream.ReadBool( bLockVol ) ) { return false; }
					if ( !_sbStream.ReadBool( bInvert ) ) { return false; }
					if ( !_sbStream.ReadBool( bLpf ) ) { return false; }
					if ( !_sbStream.ReadBool( bHpf0 ) ) { return false; }
					if ( !_sbStream.ReadBool( bHpf1 ) ) { return false; }
					if ( !_sbStream.ReadBool( bHpf2 ) ) { return false; }
					if ( !_sbStream.ReadBool( bLoop ) ) { return false; }

					if ( _ui32Version >= 4 ) {
						if ( !_sbStream.ReadBool( bSunsoft5b ) ) { return false; }
						if ( !_sbStream.ReadBool( bRf1 ) ) { return false; }
						if ( !_sbStream.ReadBool( bRf2 ) ) { return false; }
					}
				}
				catch ( ... ) { return false; }
				return true;
			}
		};


		/** Per-file options. */
		std::vector<LSN_PER_FILE>									vPerFileOptions;
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
		/** The last Projects folder. */
		std::wstring												wsLastProjectsFolder = L"";

		// == Functions.
		/**
		 * Saves the structure to a stream.
		 * 
		 * \param _sbStream The stream to which to save the structure data.
		 * \return Returns true if all writes were successful.  A return of false could indicate a memory failure or a not enough space on disk.
		 **/
		bool														Save( CStreamBase &_sbStream ) const {
			if ( !_sbStream.WriteStringU16( wsMainsHumVolume ) ) { return false; }
			if ( !_sbStream.WriteStringU16( wsWhiteNoiseVolume ) ) { return false; }
			if ( !_sbStream.WriteStringU16( wsGuassianNoiseBandwidth ) ) { return false; }
			if ( !_sbStream.WriteStringU16( wsGuassianNoiseTemperature ) ) { return false; }
			if ( !_sbStream.WriteStringU16( wsGuassianNoiseResistance ) ) { return false; }
			if ( !_sbStream.WriteStringU16( wsAbsoluteVolume ) ) { return false; }
			if ( !_sbStream.WriteStringU16( wsNormalizeVolume ) ) { return false; }
			if ( !_sbStream.WriteStringU16( wsLoudnessVolume ) ) { return false; }
			if ( !_sbStream.WriteStringU16( wsOutputHz ) ) { return false; }
			if ( !_sbStream.WriteStringU16( wsOutputFolder ) ) { return false; }
			
			if ( !_sbStream.WriteUi32( ui32MainsHum ) ) { return false; }
			if ( !_sbStream.WriteUi32( ui32WhiteNoise ) ) { return false; }
			if ( !_sbStream.WriteUi32( ui32OutFormat ) ) { return false; }
			if ( !_sbStream.WriteUi32( ui32OutBits ) ) { return false; }
			if ( !_sbStream.WriteUi32( ui32Stereo ) ) { return false; }

			if ( !_sbStream.WriteBool( bMainsHum ) ) { return false; }
			if ( !_sbStream.WriteBool( bWhiteNoise ) ) { return false; }
			if ( !_sbStream.WriteBool( bDither ) ) { return false; }
			if ( !_sbStream.WriteBool( bAbsolute ) ) { return false; }
			if ( !_sbStream.WriteBool( bNormalize ) ) { return false; }
			if ( !_sbStream.WriteBool( bLoudness ) ) { return false; }
			if ( !_sbStream.WriteBool( bNumbered ) ) { return false; }

			if ( vPerFileOptions.size() > UINT_MAX ) { return false; }
			if ( !_sbStream.WriteUi32( uint32_t( vPerFileOptions.size() ) ) ) { return false; }
			for ( size_t I = 0; I < vPerFileOptions.size(); ++I ) {
				vPerFileOptions[I].Save( _sbStream );
			}
			return true;
		}

		/**
		 * Loads from the given stream into the structure.
		 * 
		 * \param _sbStream The stream from which to load the data.
		 * \param _ui32Version The version of the file.
		 * \return Returns true if the file format is expected and memory is able to be allocated for each string in the structure.
		 **/
		bool														Load( const CStreamBase &_sbStream, uint32_t _ui32Version ) {
			try { 
				(*this) = LSN_WAV_EDITOR_WINDOW_OPTIONS();

				if ( !_sbStream.ReadStringU16( wsMainsHumVolume ) ) { return false; }
				if ( !_sbStream.ReadStringU16( wsWhiteNoiseVolume ) ) { return false; }
				if ( !_sbStream.ReadStringU16( wsGuassianNoiseBandwidth ) ) { return false; }
				if ( !_sbStream.ReadStringU16( wsGuassianNoiseTemperature ) ) { return false; }
				if ( !_sbStream.ReadStringU16( wsGuassianNoiseResistance ) ) { return false; }
				if ( !_sbStream.ReadStringU16( wsAbsoluteVolume ) ) { return false; }
				if ( !_sbStream.ReadStringU16( wsNormalizeVolume ) ) { return false; }
				if ( !_sbStream.ReadStringU16( wsLoudnessVolume ) ) { return false; }
				if ( !_sbStream.ReadStringU16( wsOutputHz ) ) { return false; }
				if ( !_sbStream.ReadStringU16( wsOutputFolder ) ) { return false; }
			
				if ( !_sbStream.ReadUi32( ui32MainsHum ) ) { return false; }
				if ( !_sbStream.ReadUi32( ui32WhiteNoise ) ) { return false; }
				if ( !_sbStream.ReadUi32( ui32OutFormat ) ) { return false; }
				if ( !_sbStream.ReadUi32( ui32OutBits ) ) { return false; }
				if ( !_sbStream.ReadUi32( ui32Stereo ) ) { return false; }

				if ( !_sbStream.ReadBool( bMainsHum ) ) { return false; }
				if ( !_sbStream.ReadBool( bWhiteNoise ) ) { return false; }
				if ( !_sbStream.ReadBool( bDither ) ) { return false; }
				if ( !_sbStream.ReadBool( bAbsolute ) ) { return false; }
				if ( !_sbStream.ReadBool( bNormalize ) ) { return false; }
				if ( !_sbStream.ReadBool( bLoudness ) ) { return false; }
				if ( !_sbStream.ReadBool( bNumbered ) ) { return false; }

				uint32_t ui32Total = 0;
				if ( !_sbStream.ReadUi32( ui32Total ) ) { return false; }
				vPerFileOptions.resize( ui32Total );
				for ( size_t I = 0; I < vPerFileOptions.size(); ++I ) {
					if ( !vPerFileOptions[I].Load( _sbStream, _ui32Version ) ) { return false; }
				}
			}
			catch ( ... ) { return false; }
			return true;
		}
	};

}	// namespace lsn

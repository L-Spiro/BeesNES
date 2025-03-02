/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Uses OpenAL or Core Audio to handle all for our audio needs.  As an emulator, we just need a single device, context, and source.
 *	This class owns and manages the objects and provides the interface we need to generate audio.
 */


#pragma once

#include <cstdint>
#include <string>

namespace lsn {

	/** Format types. */
	enum LSN_SAMPLE_FORMAT {
		LSN_SF_MONO_8,									/**< Mono 8-bit PCM. */
		LSN_SF_MONO_16,									/**< Mono 16-bit PCM. */
		LSN_SF_MONO_24,									/**< Mono 24-bit PCM. */
		LSN_SF_MONO_F32									/**< Mono 32-bit float. */
	};


	// == Types.
	/** Audio formats. */
	struct LSN_AUDIO_FORMATS {
		LSN_SAMPLE_FORMAT								sfFormat;								/**< The sample format. */
		uint32_t										ui16Channels;							/**< The number of channels associated with the format. */
		uint16_t										ui16BitsPerChannel;						/**< The number of bits in the format. */
		const wchar_t *									pwcName;								/**< The name of the format. */
	};

	/** The audio format. */
	struct LSN_AUDIO_FORMAT {
		/** The sample format. */
		LSN_SAMPLE_FORMAT								sfFormat = LSN_SF_MONO_16;
		/** The number of channels. */
		uint16_t										ui16Channels = 1;
		/** The number of bits in a sample. */
		uint16_t										ui16BitsPerChannel = 16;


		// == Operators.
		/**
		 * Inequality operator.
		 * 
		 * \param _fFormat The format against which to compare.
		 * \return Returns true if the given format does not equal this format.
		 **/
		bool											operator != ( const LSN_AUDIO_FORMAT &_fFormat ) const {
			return sfFormat != _fFormat.sfFormat ||
				ui16Channels != _fFormat.ui16Channels ||
				ui16BitsPerChannel != _fFormat.ui16BitsPerChannel;
		}
	};

	/** Preset audio profiles. */
	struct LSN_AUDIO_PROFILE {
		std::wstring									wsName;
		float											fLpf = 14000.0f;
		float											fHpf0 = 194.0f;
		float											fHpf1 = 37.0f;
		float											fHpf2 = 37.0f;
		float											fVolume = 0.625f;
		bool											bLpfEnable = false;
		bool											bHpf0Enable = true;
		bool											bHpf1Enable = true;
		bool											bHpf2Enable = true;
		bool											bInvert = true;
		bool											bNoise = true;
		bool											bNoise2 = false;
	};

	/** Audio-related options. */
	struct LSN_AUDIO_OPTIONS {
		uint32_t										ui32Device = 0;
		uint32_t										ui32OutputHz = 44100;
		LSN_AUDIO_FORMAT								afFormat;
		float											fVolume = 3.0f;
		float											fBgVol = 0.2f;
		bool											bUseGlobal = true;
		bool											bEnabled = true;
		bool											bDither = false;

		LSN_AUDIO_PROFILE								apCharacteristics;

		

		// == Members.
		static LSN_AUDIO_FORMATS						s_afFormats[];
		static LSN_AUDIO_PROFILE						s_apProfiles[];


		// == Functions.
		/**
		 * Gets audio-format details by enumeration.
		 * 
		 * \param _sfFormat The format whose details are to be found.
		 * \return Returns a pointer to the specified format or nullptr.
		 **/
		static const LSN_AUDIO_FORMATS *				FormatByEnum( LSN_SAMPLE_FORMAT _sfFormat );

		/**
		 * Gets total audio formats.
		 * 
		 * \return Returns the total values in s_afFormats.
		 **/
		static size_t									FormatTotal();

		/**
		 * Gets total presets.
		 * 
		 * \return Returns the total values in s_apProfiles.
		 **/
		static size_t									PresetTotal();
	};

}	// namespace lsn

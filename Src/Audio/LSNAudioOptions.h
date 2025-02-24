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

	/** Audio-related options. */
	struct LSN_AUDIO_OPTIONS {
		uint32_t										ui32OutputHz = 44100;
		LSN_AUDIO_FORMAT								afFormat;
		

		// == Members.
		static LSN_AUDIO_FORMATS						s_afFormats[];


		// == Functions.
		/**
		 * Gets audio-format details by enumeration.
		 * 
		 * \param _sfFormat The format whose details are to be found.
		 * \return Returns a pointer to the specified format or nullptr.
		 **/
		const LSN_AUDIO_FORMATS *						FormatByEnum( LSN_SAMPLE_FORMAT _sfFormat );

		/**
		 * Gets total audio formats.
		 * 
		 * \return Returns the total values in s_afFormats.
		 **/
		const size_t									FormatTotal();
	};

}	// namespace lsn
